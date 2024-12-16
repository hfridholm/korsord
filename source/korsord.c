/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define _GNU_SOURCE
#include <sched.h>
#include <pthread.h>

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

bool running = false;


static char doc[] = "korsord - swedish crossword generator";

static char args_doc[] = "[MODEL]";

static struct argp_option options[] =
{
  { "primary", 'p', "FILE",   0, "Primary words file" },
  { "backup",  'b', "FILE",   0, "Backup words file" },
  { "visual",  'v', 0,        0, "Visualize generation" },
  { "debug",   'd', 0,        0, "Print debug messages" },
  { "single",  's', 0,        0, "Only try generate once" },
  { "fps",     'f', "NUMBER", 0, "Frames per second" },
  { 0 }
};

struct args
{
  char* model;
  char* primary;
  char* backup;
  bool  visual;
  bool  debug;
  bool  single;
  int   fps;
};

struct args args =
{
  .model   = NULL,
  .primary = NULL,
  .backup  = "../assets/backup.words",
  .visual  = false,
  .debug   = false,
  .single  = false,
  .fps     = 1
};

/*
 * This is the option parsing function used by argp
 */
static error_t opt_parse(int key, char* arg, struct argp_state* state)
{
  struct args* args = state->input;

  switch(key)
  {
    case 'f':
      if(!arg) argp_usage(state);

      int number = atoi(arg);

      if(number >= 1 && number <= 100)
      {
        args->fps = number;
      }
      else argp_usage(state);

      break;

    case 'v':
      args->visual = true;
      break;

    case 'd':
      args->debug = true;
      break;

    case 's':
      args->single = true;
      break;

    case 'p':
      args->primary = arg;
      break;

    case 'b':
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

  printf("Start printing grid\n");

  int delay = 1000000 / args.fps;

  while(running)
  {
    curr_grid_print();

    stats_print();

    usleep(delay);

    refresh();
  }

  printf("Stop printing grid\n");

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
  printf("Stop program\n");

  running = false;
}

/*
 * Init the ncurses library and screen
 */
static int curses_init(void)
{
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

  clear();
  refresh();

  return 0;
}

/*
 * Close the ncurses library and restore the terminal
 */
static void curses_free(void)
{
  clear();

  refresh();

  endwin();
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

  // Only enter ncurses mode if not in debug mode
  if(!args.debug && curses_init() != 0)
  {
    perror("curses_init");

    return 1;
  }


  curr_grid_init();
  stats_init();


  running = true;

  pthread_t thread;

  if(pthread_create(&thread, NULL, print_routine, NULL) != 0)
  {
    perror("Failed to create thread");

    curses_free();

    return 1;
  }


  // 1. Load the word bases
  wbase_t* wbase = wbase_create(args.primary, args.backup);

  if(!wbase)
  {
    perror("Failed to create wbase");

    curses_free();
    
    return 2;
  }

  // 2. Generate crossword grid with word bases
  grid_t* grid = NULL;

  while(running && !grid_is_done(grid))
  {
    printf("Generating grid...\n");

    curr_grid_set(NULL);

    grid_free(&grid);

    stats_clear();

    grid = grid_gen(wbase, args.model);

    // If the user only wants 1 run, break
    if(args.single) break;
  }

  if(grid)
  {
    printf("Generated grid\n");

    curr_grid_set(grid);


    clear();

    curr_grid_print();

    stats_print();

    refresh();

    getch();
  }

  running = false;

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);


  grid_free(&grid);

  wbase_free(&wbase);


  curr_grid_free();
  stats_free();


  curses_free();

  return 0;
}
