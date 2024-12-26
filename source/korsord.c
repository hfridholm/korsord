/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>

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

#include "k-grid.h"
#include "k-wbase.h"
#include "k-stats.h"

#include "k-grid-curr.h"
#include "k-grid-best.h"

bool is_running = false;

#define INPUT_DELAY 100000

extern int MAX_CROWD_AMOUNT;
extern int MAX_EXIST_AMOUNT;
extern int HALF_WORD_AMOUNT;


static char doc[] = "korsord - swedish crossword generator";

static char args_doc[] = "[MODEL]";

static struct argp_option options[] =
{
  { "primary",  'p', "FILE",   0, "Primary words file" },
  { "backup",   'b', "FILE",   0, "Backup words file" },
  { "visual",   'v', 0,        0, "Visualize generation" },
  { "interact", 'i', 0,        0, "Enter interactive mode" },
  { "debug",    'd', 0,        0, "Print debug messages" },
  { "output",   'o', "FILE",   0, "Output debug to file" },
  { "result",   'r', "FILE",   0, "Save result to file" },
  { "fps",      'f', "AMOUNT", 0, "Frames per second" },
  { "length",   'l', "LENGTH", 0, "Max length of words" },
  { "crowd",    'c', "AMOUNT", 0, "Max amount of nerby blocks" },
  { "exist",    'e', "AMOUNT", 0, "Amount of precission" },
  { "half",     'h', "AMOUNT", 0, "Progress preserve amount" },
  { 0 }
};

struct args
{
  char* model;
  char* primary;
  char* backup;
  bool  visual;
  bool  ncurses;
  int   fps;
  int   length;
  char* output;
  char* result;
};

// Default values of korsord arguments
struct args args =
{
  .model   = NULL,
  .primary = NULL,
  .backup  = "svenska.words",
  .visual  = false,
  .ncurses = false,
  .fps     = 1,
  .length  = 10,
  .output  = NULL,
  .result  = NULL
};

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
        args->length = number;
      }
      else argp_usage(state);

      break;

    case 'v':
      args->visual = true;
      break;

    case 'd':
      args->ncurses = false;
      break;

    case 'i':
      args->ncurses = true;

      args->visual = true;
      break;

    case 'p':
      if(!arg || *arg == '-') argp_usage(state);

      args->primary = arg;
      break;

    case 'o':
      if(!arg || *arg == '-') argp_usage(state);

      args->output = arg;
      break;

    case 'r':
      if(!arg || *arg == '-') argp_usage(state);

      args->result = arg;
      break;

    case 'b':
      if(!arg || *arg == '-') argp_usage(state);

      args->backup = arg;
      break;

    case ARGP_KEY_ARG:
      if(state->arg_num >= 1) argp_usage(state);

      args->model = arg;
      break;

    case ARGP_KEY_END:
      if(state->arg_num < 1) argp_usage(state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
  }

  return 0;
}

/*
 * core_id = 0, 1, ... n-1, where n is the system's number of cores
 *
 * https://stackoverflow.com/questions/1407786/how-to-set-cpu-affinity-of-a-particular-pthread
 *
 * Maybe use this function in the future
 */
static int stick_this_thread_to_core(int core_id) 
{
  int num_cores = sysconf(_SC_NPROCESSORS_ONLN);

  if (core_id < 0 || core_id >= num_cores)
  {
    return EINVAL;
  }

  cpu_set_t cpuset;
  CPU_ZERO(&cpuset);
  CPU_SET(core_id, &cpuset);

  return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
}

/*
 *
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
  init_pair(5, COLOR_RED,  COLOR_BLACK);

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
  info_print("Generating grid");

  // 1. Un-using every word in word base
  wbase_reset(wbase);

  grid_t* grid = grid_gen(wbase, args.model);

  curr_grid_set(grid);

  info_print("Generated grid");


  info_print("Saving result");

  grid_export(grid);

  grid_words_export(grid);

  info_print("Saved result");


  grid_free(&grid);

  return NULL;
}

/*
 *
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
 */
int main(int argc, char* argv[])
{
  argp_parse(&argp, argc, argv, 0, 0, &args);

  signal(SIGINT, stop_handler);

  // srand(time(NULL));

  if(args.ncurses) args.output = "output.txt";

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


  curr_grid_init();
  best_grid_init();

  stats_init();


  is_running = true;

  pthread_t thread = 0;

  if(pthread_create(&thread, NULL, print_routine, NULL) != 0)
  {
    perror("Failed to create thread");

    ncurses_free();

    return 1;
  }

  info_print("Creating word base");

  // 1. Load the word bases
  wbase_t* wbase = wbase_create(args.primary, args.backup, args.length);

  if(!wbase)
  {
    perror("Failed to create word base");

    ncurses_free();
    
    return 2;
  }

  info_print("Created word base");


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

  return 0;
}
