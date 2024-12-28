/*
 * k-grid-best.c - manipulate best grid
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include <ncurses.h>
#include <pthread.h>

pthread_mutex_t best_lock;
grid_t* best_grid = NULL;

/*
 * Get the cross_count of best grid
 *
 * RETURN (int cross_count)
 */
int best_grid_cross_count_get(void)
{
  pthread_mutex_lock(&best_lock);

  int cross_count;

  if(best_grid)
  {
    cross_count = best_grid->cross_count;
  }
  else
  {
    cross_count = 0;
  }

  pthread_mutex_unlock(&best_lock);

  return cross_count;
}

/*
 * Set the best grid equal to grid
 *
 * The function just copies the content, not the variable
 */
void best_grid_set(grid_t* grid)
{
  pthread_mutex_lock(&best_lock);

  if(best_grid)
  {
    grid_copy(best_grid, grid);
  }
  else if(grid)
  {
    best_grid = grid_dup(grid);
  }
  else
  {
    best_grid = NULL;
  }

  pthread_mutex_unlock(&best_lock);
}

/*
 * Initialize best_grid object
 */
void best_grid_init(void)
{
  best_grid = NULL;

  pthread_mutex_init(&best_lock, NULL);
}

/*
 * Free the best_grid object
 */
void best_grid_free(void)
{
  grid_free(&best_grid);

  pthread_mutex_destroy(&best_lock);
}

/*
 * Print the bestent grid to the terminal
 */
void best_grid_print(void)
{
  pthread_mutex_lock(&best_lock);

  grid_print(best_grid);

  pthread_mutex_unlock(&best_lock);
}

/*
 * Print the bestent grid to ncurses screen
 */
void best_grid_ncurses_print(void)
{
  pthread_mutex_lock(&best_lock);

  if(best_grid)
  {
    int h = getmaxy(stdscr);
    int w = getmaxx(stdscr);

    int start_x = MAX(0, ((w / 2) - best_grid->width * 2) / 2);
    int start_y = MAX(0, (h       - best_grid->height   ) / 2);

    grid_ncurses_print(best_grid, start_x, start_y);
  }

  pthread_mutex_unlock(&best_lock);
}
