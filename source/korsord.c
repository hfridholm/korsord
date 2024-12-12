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

#include "k-grid.h"
#include "k-wbase.h"

bool running = false;

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

  // 1. Load the word bases
  trie_t* trie = trie_create("words.txt");

  if(!trie)
  {
    perror("trie_create");

    return 1;
  }

  // 2. Generate crossword grid with word bases
  grid_t* grid = grid_gen(trie, 10, 5);

  printf("Generated grid\n");

  grid_print(grid);


  grid_free(&grid);

  trie_free(&trie);

  return 0;
}
