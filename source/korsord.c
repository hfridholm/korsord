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

#include "k-intern.h"

bool is_running = false;

extern int MAX_CROWD_AMOUNT;
extern int MAX_WORD_LENGTH;
extern int MAX_EXIST_AMOUNT;

static char doc[] = "korsord - swedish crossword generator";

static char args_doc[] = "[MODEL] [WORDS...]";

static struct argp_option options[] =
{
  { "interact", 'i', 0,        0, "Interactive generation" },
  { "length",   'l', "LENGTH", 0, "Max length of words" },
  { "crowd",    'c', "AMOUNT", 0, "Max amount of nerby blocks" },
  { "exist",    'e', "AMOUNT", 0, "Amount of precission" },
  { "name",     'n', "NAME",   0, "Name of grid and clues" },
  { 0 }
};

struct args
{
  char*  model;
  char** wfiles;
  size_t wfile_count;
  bool   interact;
  char*  name;
};

// Default values of korsord arguments
struct args args =
{
  .model       = NULL,
  .wfiles      = NULL,
  .wfile_count = 0,
  .interact    = false,
  .name        = "temp",
};

// __builtin_clzll counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1ULL << (64 - __builtin_clzll(n)))

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
    case 'c':
      if(!arg || *arg == '-') argp_usage(state);

      number = atoi(arg);

      if(number >= 1 && number <= 7)
      {
        MAX_CROWD_AMOUNT = number;
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

    case 'i':
      args->interact = true;
      break;

    case 'n':
      args->name = arg;
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
  info_print("Start print routine");

  int delay = 1000000 / 30;

  while(is_running)
  {
    erase();

    best_grid_ncurses_print();
    curr_grid_ncurses_print();

    stats_ncurses_print();

    refresh();

    usleep(delay);
  }

  info_print("Stop print routine");

  return NULL;
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

    error_print("ncurses colors");

    return 1;
  }

  use_default_colors();

  init_pair(1, COLOR_GREEN, -1);
  init_pair(2, COLOR_WHITE, -1);
  init_pair(3, COLOR_BLUE,  -1);
  init_pair(4, COLOR_RED,   -1);
  init_pair(5, COLOR_RED,   -1);

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
  curr_grid_set(NULL);
  best_grid_set(NULL);


  // 2. Generate grid
  info_print("Generating grid");

  grid_t* model = model_load(args.model);

  grid_t* grid = grid_gen(wbase, model);

  if(grid)
  {
    curr_grid_set(grid);
    best_grid_set(grid);

    info_print("Generated grid");


    // 3. Export result to file
    info_print("Exporting results");

    grid_export(grid, args.name);

    info_print("Exported results");


    grid_free(&grid);
  }
  else
  {
    error_print("Generation failed");
  }

  grid_free(&model);

  return NULL;
}

/*
 * Routine for interactivly generating crossword grids using ncurses
 */
static int interact_routine(wbase_t* wbase)
{
  info_print("Start interact routine");

  if(ncurses_init() != 0)
  {
    error_print("ncurses_init");

    return 2;
  }

  pthread_t print_thread = 0;

  if(pthread_create(&print_thread, NULL, print_routine, NULL) != 0)
  {
    error_print("Failed to create print thread");

    ncurses_free();

    return 1;
  }

  pthread_t gen_thread = 0;

  int key;
  while (is_running && (key = getch()) != ERR)
  {
    switch (key)
    {
      case 'g': case 'r': 
        // Break the switch statement
        if ((key == 'g' &&  is_generating) ||
            (key == 'r' && !is_generating)) break;

        // This will stop the gen routine
        is_generating = false;

        pthread_join(gen_thread, NULL);
        gen_thread = 0;
          
        curr_grid_set(NULL);
        stats_clear();

        if(pthread_create(&gen_thread, NULL, gen_routine, wbase) != 0)
        {
          error_print("Failed to create gen thread");
        }
        break;

      case 's':
        // Break the switch statement
        if(!is_generating) break;

        info_print("Stopping grid generation");

        is_generating = false;

        pthread_join(gen_thread, NULL);
        gen_thread = 0;

        info_print("Stopped grid generation");
        break;

      case 'q': case 3:
        is_generating = false;

        is_running = false;
        break;

      default:
        break;
    }

    usleep(100000);

    flushinp(); // Flush input buffer
  }
  
  is_generating = false;

  pthread_join(gen_thread, NULL);
  gen_thread = 0;

  pthread_join(print_thread, NULL);
  print_thread = 0;

  ncurses_free();

  info_print("Stop interact routine");

  return 0;
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
  srand(time(NULL));

  char debug_file[64];

  if (debug_file_get(debug_file) != 0)
  {
    fprintf(stderr, "korsord: Failed to get debug.log");

    return 1;
  }

  if (debug_file_open(debug_file) != 0)
  {
    fprintf(stderr, "korsord: Failed to open debug.log");

    return 2;
  }

  argp_parse(&argp, argc, argv, 0, 0, &args);

  info_print("Start main");

  info_print("Creating word base");

  wbase_t* wbase = wbase_create(args.wfiles, args.wfile_count);

  if(!wbase)
  {
    error_print("Failed to create word base");

    debug_file_close();

    free(args.wfiles);

    return 2;
  }

  info_print("Created word base");


  curr_grid_init();
  best_grid_init();

  stats_init();

  is_running = true;

  if(args.interact)
  {
    interact_routine(wbase);
  }
  else
  {
    gen_routine(wbase);
  }

  is_running = false;

  curr_grid_free();
  best_grid_free();
  
  stats_free();


  wbase_free(&wbase);


  info_print("Stop main");

  debug_file_close();

  free(args.wfiles);

  return 0;
}
