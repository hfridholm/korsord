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

#include "k-grid.h"
#include "k-wbase.h"

bool running = false;

pthread_mutex_t lock;
grid_t* curr_grid = NULL;

/*
 *
 */
static void* print_routine(void* arg)
{
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

/*
 *
 */
int main(int argc, char* argv[])
{
  signal(SIGINT, stop_handler);

  srand(time(NULL));


  printf("korsord.c\n");

  running = true;


  pthread_mutex_init(&lock, NULL);

  pthread_t thread;

  if(pthread_create(&thread, NULL, print_routine, NULL) != 0)
  {
    perror("Failed to create thread");

    return 1;
  }


  // 1. Load the word bases
  trie_t* trie = trie_create("words.txt");

  if(!trie)
  {
    perror("Failed to create trie");

    return 2;
  }

  // 2. Generate crossword grid with word bases
  grid_t* grid = grid_gen(trie, 10, 5);

  printf("Generated grid\n");

  grid_print(grid);


  running = false;

  // Wait for the second thread to finish
  // pthread_cancel(thread);

  pthread_join(thread, NULL);

  pthread_mutex_destroy(&lock);


  grid_free(&grid);

  trie_free(&trie);

  return 0;
}