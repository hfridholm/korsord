/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include <pthread.h>

pthread_mutex_t grid_lock;
grid_t* curr_grid = NULL;

/*
 *
 */
void curr_grid_set(grid_t* grid)
{
  pthread_mutex_lock(&grid_lock);

  if(curr_grid)
  {
    grid_copy(curr_grid, grid);
  }
  else if(grid)
  {
    curr_grid = grid_dup(grid);
  }
  else
  {
    curr_grid = NULL;
  }

  pthread_mutex_unlock(&grid_lock);
}

/*
 *
 */
void curr_grid_init(void)
{
  curr_grid = NULL;

  pthread_mutex_init(&grid_lock, NULL);
}

/*
 *
 */
void curr_grid_free(void)
{
  grid_free(&curr_grid);

  pthread_mutex_destroy(&grid_lock);
}

/*
 *
 */
void curr_grid_print(void)
{
  pthread_mutex_lock(&grid_lock);

  grid_print(curr_grid);

  pthread_mutex_unlock(&grid_lock);
}

/*
 *
 */
void curr_grid_ncurses_print(void)
{
  pthread_mutex_lock(&grid_lock);

  grid_ncurses_print(curr_grid);

  pthread_mutex_unlock(&grid_lock);
}
