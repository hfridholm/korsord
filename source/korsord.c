/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define DEBUG_IMPLEMENT
#include "debug.h"

#define FILE_IMPLEMENT
#include "file.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <argp.h>
#include <ncurses.h>
#include <pthread.h>

#include "k-grid.h"
#include "k-wbase.h"
#include "k-stats.h"

#include "k-grid-curr.h"
#include "k-grid-best.h"

bool is_running = false;

#define INPUT_DELAY 100000

extern int MAX_CROWD_AMOUNT;
extern int MAX_WORD_LENGTH;
extern int MAX_EXIST_AMOUNT;
extern int HALF_WORD_AMOUNT;
extern int PREP_EMPTY_CHANCE;


static char doc[] = "korsord - swedish crossword generator";

static char args_doc[] = "[MODEL] [WORDS...]";

static struct argp_option options[] =
{
  { "visual",   'v', 0,        0, "Visualize generation" },
  { "interact", 'i', 0,        0, "Enter interactive mode" },
  { "output",   'o', "FILE",   0, "Output debug to file" },
  { "used",     'u', "FILE",   0, "Disregard used words" },
  { "fps",      'f', "AMOUNT", 0, "Frames per second" },
  { "length",   'l', "LENGTH", 0, "Max length of words" },
  { "crowd",    'c', "AMOUNT", 0, "Max amount of nerby blocks" },
  { "exist",    'e', "AMOUNT", 0, "Amount of precission" },
  { "half",     'h', "AMOUNT", 0, "Progress preserve amount" },
  { "prep",     'p', "AMOUNT", 0, "Prepare empty procentage" },
  { 0 }
};

struct args
{
  char*  model;
  char** wfiles;
  size_t wfile_count;
  char*  used_wfile;
  bool   visual;
  bool   ncurses;
  int    fps;
  char*  output;
};

// Default values of korsord arguments
struct args args =
{
  .model       = NULL,
  .wfiles      = NULL,
  .wfile_count = 0,
  .visual      = false,
  .ncurses     = false,
  .fps         = 1,
  .output      = NULL
};

// __builtin_clz counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1 << (sizeof(n) * 8 - __builtin_clz(n)))

/*
 * Append word file to array of word files
 */
static int wfile_append(char*** wfiles, size_t* count, char* wfile)
{
  if(*count == 0 || ((*count) + 1) >= CAPACITY(*count))
  {
    char** new_wfiles = realloc(*wfiles, sizeof(char*) * CAPACITY((*count) + 1));

    if(!new_wfiles) return 1;

    *wfiles = new_wfiles;
  }

  (*wfiles)[(*count)++] = wfile;

  return 0;
}

/*
 * This is the option parsing function used by argp
 */
static error_t opt_parse(int key, char* arg, struct argp_state* state)
{
  struct args* args = state->input;

  int number;

  switch(key)
  {
    case 'f':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1 && number <= 100)
      {
        args->fps = number;
      }
      else argp_usage(state);

      break;

    case 'p':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 0 && number <= 100)
      {
        PREP_EMPTY_CHANCE = number;
      }
      else argp_usage(state);

      break;

    case 'c':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1 && number <= 7)
      {
        MAX_CROWD_AMOUNT = number;
      }
      else argp_usage(state);

      break;

    case 'h':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1)
      {
        HALF_WORD_AMOUNT = number;
      }
      else argp_usage(state);

      break;

    case 'e':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1 && number <= 100000)
      {
        MAX_EXIST_AMOUNT = number;
      }
      else argp_usage(state);

      break;

    case 'l':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1)
      {
        MAX_WORD_LENGTH = number;
      }
      else argp_usage(state);

      break;

    case 'v':
      args->visual = true;
      break;

    case 'i':
      args->ncurses = true;

      args->visual = true;
      break;

    case 'o':
      if(!arg || *arg == '-') argp_usage(state);

      args->output = arg;
      break;

    case 'u':
      if(!arg || *arg == '-') argp_usage(state);

      args->used_wfile = arg;
      break;

    case ARGP_KEY_ARG:
      if(state->arg_num > 0)
      {
        wfile_append(&args->wfiles, &args->wfile_count, arg);
      }
      else
      {
        args->model = arg;
      }
      break;

    case ARGP_KEY_END:
      if(state->arg_num < 2) argp_usage(state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

/*
 * Routine for async printing of grid
 */
static void* print_routine(void* arg)
{
  if(!args.visual) return NULL;

  info_print("Start print routine");

  int delay = 1000000 / args.fps;

  while(is_running)
  {
    if(args.ncurses)
    {
      erase();

      best_grid_ncurses_print();
      curr_grid_ncurses_print();

      stats_ncurses_print();

      refresh();
    }
    else
    {
      best_grid_print();
      curr_grid_print();

      stats_print();
    }

    usleep(delay);
  }

  info_print("Stop print routine");

  return NULL;
}

/*
 * Stop signal handler
 *
 * In case of the user killing the program,
 * everything should be cleaned up afterwards
 */
static void stop_handler(int signum)
{
  info_print("Stop program");

  is_generating = false;

  is_running = false;
}

/*
 * Init the ncurses library and screen
 */
static int ncurses_init(void)
{
  info_print("Initializing ncurses");

  initscr();

  noecho();
  curs_set(0);

  if(start_color() == ERR || !has_colors())
  {
    endwin();

    perror("ncurses colors");

    return 1;
  }

  use_default_colors();

  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  init_pair(2, COLOR_WHITE, COLOR_BLACK);
  init_pair(3, COLOR_BLUE,  COLOR_BLACK);
  init_pair(4, COLOR_RED,   COLOR_BLACK);
  init_pair(5, COLOR_RED,   COLOR_BLACK);

  erase();
  refresh();

  info_print("Initialized ncurses");

  return 0;
}

/*
 * Close the ncurses library and restore the terminal
 */
static void ncurses_free(void)
{
  info_print("Freeing ncurses");

  clear();

  refresh();

  endwin();

  info_print("Freed ncurses");
}

/*
 * This routine generates a grid
 *
 * A thread routine can only pass one argument,
 * it just happens to be enough for me :D
 *
 * PARAMS:
 * - void* wbase | Thread complient pointer to wbase
 */
static void* gen_routine(void* wbase)
{
  // 1. Un-using every word in word base
  wbase_reset(wbase);

  curr_grid_set(NULL);
  best_grid_set(NULL);


  // 2. Generate grid
  info_print("Generating grid");

  grid_t* grid = grid_gen(wbase, args.model);

  if(grid)
  {
    curr_grid_set(grid);
    best_grid_set(grid);

    info_print("Generated grid");


    // 3. Export result to file
    info_print("Exporting results");

    grid_export(grid);

    grid_words_export(grid);

    used_words_export(grid);

    info_print("Exported results");


    // 4. Free grid
    grid_free(&grid);
  }
  else
  {
    error_print("Generation failed");
  }

  return NULL;
}

/*
 * Routine for interactivly generating crossword grids using ncurses
 */
static void interact_routine(wbase_t* wbase)
{
  info_print("Start interact routine");

  pthread_t thread = 0;

  int key;
  while(is_running && (key = getch()) != ERR)
  {
    switch(key)
    {
      case 'g': case 'r': 
        // Break the switch statement
        if ((key == 'g' &&  is_generating) ||
            (key == 'r' && !is_generating)) break;

        // This will stop the gen routine
        is_generating = false;

        pthread_join(thread, NULL);
        thread = 0;
          
        curr_grid_set(NULL);
        stats_clear();

        if(pthread_create(&thread, NULL, gen_routine, wbase) != 0)
        {
          error_print("Failed to create gen thread");
        }
        break;

      case 's':
        // Break the switch statement
        if(!is_generating) break;

        info_print("Stopping grid generation");

        is_generating = false;
        // Wait for the second thread to finish
        // pthread_cancel(thread);

        pthread_join(thread, NULL);
        thread = 0;

        info_print("Stopped grid generation");
        break;

      case 'q':
        stop_handler(SIGINT);

      default:
        break;
    }

    usleep(INPUT_DELAY);

    flushinp(); // Flush input buffer
  }
  
  is_generating = false;

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);
  thread = 0;

  info_print("Stop interact routine");
}

/*
 * This is the main routine for debug mode
 */
static void debug_routine(wbase_t* wbase)
{
  info_print("Start debug routine");

  curr_grid_set(NULL);
  stats_clear();

  pthread_t thread = 0;

  if(pthread_create(&thread, NULL, gen_routine, wbase) != 0)
  {
    error_print("Failed to create gen thread");
  }

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);

  best_grid_print();
  curr_grid_print();

  stats_print();

  curr_grid_set(NULL);
  best_grid_set(NULL);

  stats_clear();

  info_print("Stop debug routine");
}

static struct argp argp = { options, opt_parse, args_doc, doc };

/*
 * RETURN (int status)
 * - 0 | Success
 *
 * Note: Refactor this into step functions
 * (now the freeing at error is crazy)
 */
int main(int argc, char* argv[])
{
  argp_parse(&argp, argc, argv, 0, 0, &args);

  signal(SIGINT, stop_handler);

  // srand(time(NULL));
  
  if(args.ncurses)
  {
    args.output = "output.txt";
  }

  if(args.output)
  {
    if(debug_file_open(args.output) != 0)
    {
      perror("debug_file_open");

      return 1;
    }
  }

  info_print("Start main");

  if(args.ncurses)
  {
    if(ncurses_init() != 0)
    {
      perror("ncurses_init");

      return 2;
    }
  }


  info_print("Creating word base");

  // 1. Load the word bases
  wbase_t* wbase = wbase_create(args.wfiles, args.wfile_count);

  if(!wbase)
  {
    perror("Failed to create word base");

    ncurses_free();
    
    debug_file_close();

    free(args.wfiles);

    return 3;
  }

  info_print("Created word base");


  info_print("Loading used words");

  trie_t* used_words = trie_load(args.used_wfile);

  if(!used_words) used_words = trie_create();

  info_print("Loaded used words");


  curr_grid_init();
  best_grid_init();

  stats_init();


  is_running = true;

  pthread_t thread = 0;

  if(pthread_create(&thread, NULL, print_routine, NULL) != 0)
  {
    perror("Failed to create thread");

    ncurses_free();

    wbase_free(&wbase);

    curr_grid_free();
    best_grid_free();
    
    stats_free();

    return 1;
  }


  // Enter the main routine (main loop)
  if(args.ncurses)
  {
    interact_routine(wbase);
  }
  else
  {
    debug_routine(wbase);
  }


  is_running = false;

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);


  trie_free(&used_words);

  wbase_free(&wbase);


  curr_grid_free();
  best_grid_free();
  
  stats_free();


  if(args.ncurses)
  {
    ncurses_free();
  }

  info_print("Stop main");

  debug_file_close();

  free(args.wfiles);

  return 0;
}
