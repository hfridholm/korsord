/*
 * k-grid-curr.c - manipulate current grid
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include <ncurses.h>
#include <pthread.h>

pthread_mutex_t curr_lock;
grid_t* curr_grid = NULL;

/*
 * Set the current grid equal to grid
 *
 * The function just copies the content, not the variable
 */
void curr_grid_set(grid_t* grid)
{
  pthread_mutex_lock(&curr_lock);

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

  pthread_mutex_unlock(&curr_lock);
}

/*
 * Initialize curr_grid object
 */
void curr_grid_init(void)
{
  curr_grid = NULL;

  pthread_mutex_init(&curr_lock, NULL);
}

/*
 * Free the curr_grid object
 */
void curr_grid_free(void)
{
  grid_free(&curr_grid);

  pthread_mutex_destroy(&curr_lock);
}

/*
 * Print the current grid to the terminal
 */
void curr_grid_print(void)
{
  pthread_mutex_lock(&curr_lock);

  grid_print(curr_grid);

  pthread_mutex_unlock(&curr_lock);
}

/*
 * Print the current grid to ncurses screen
 */
void curr_grid_ncurses_print(void)
{
  pthread_mutex_lock(&curr_lock);

  if(curr_grid)
  {
    int h = getmaxy(stdscr);
    int w = getmaxx(stdscr);

    int start_x = (w / 2) + MAX(0, ((w / 2) - (curr_grid->width  + 2) * 2) / 2);
    int start_y = MAX(0, (h - (curr_grid->height + 2)) / 2);

    grid_ncurses_print(curr_grid, start_x, start_y);
  }

  pthread_mutex_unlock(&curr_lock);
}
