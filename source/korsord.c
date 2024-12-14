/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <argp.h>

#include "k-grid.h"
#include "k-wbase.h"

bool running = false;

pthread_mutex_t lock;
grid_t* curr_grid = NULL;


static char doc[] = "korsord - swedish crossword generator";

static char args_doc[] = "[MODEL]";

static struct argp_option options[] =
{
  { "primary", 'p', "FILE", 0, "Primary words file" },
  { "backup",  'b', "FILE", 0, "Backup words file" },
  { "visual",  'v', 0,      0, "Visualize generation" },
  { 0 }
};

struct args
{
  char* model;
  char* primary;
  char* backup;
  bool  visual;
};

struct args args =
{
  .model   = NULL,
  .primary = NULL,
  .backup  = "../assets/backup.words",
  .visual  = false
};

/*
 * This is the option parsing function used by argp
 */
static error_t opt_parse(int key, char* arg, struct argp_state* state)
{
  struct args* args = state->input;

  switch(key)
  {
    case 'v':
      args->visual = true;
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
 *
 */
static void* print_routine(void* arg)
{
  if(!args.visual) return NULL;

  printf("Start printing grid\n");

  while(running)
  {
    pthread_mutex_lock(&lock);

    if(curr_grid) grid_print(curr_grid);

    pthread_mutex_unlock(&lock);

    usleep(100000);
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


static struct argp argp = { options, opt_parse, args_doc, doc };

/*
 * RETURN (int status)
 * - 0 | Success
 */
int main(int argc, char* argv[])
{
  argp_parse(&argp, argc, argv, 0, 0, &args);

  signal(SIGINT, stop_handler);

  srand(time(NULL));


  running = true;


  pthread_mutex_init(&lock, NULL);

  pthread_t thread;

  if(pthread_create(&thread, NULL, print_routine, NULL) != 0)
  {
    perror("Failed to create thread");

    return 1;
  }


  // 1. Load the word bases
  wbase_t* wbase = wbase_create(args.primary, args.backup);

  if(!wbase)
  {
    perror("Failed to create wbase");

    return 2;
  }

  // 2. Generate crossword grid with word bases
  grid_t* grid = grid_gen(wbase, args.model);

  if(grid)
  {
    printf("Generated grid\n");

    grid_print(grid);
  }

  running = false;

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);

  pthread_mutex_destroy(&lock);


  grid_free(&grid);

  wbase_free(&wbase);

  return 0;
}
