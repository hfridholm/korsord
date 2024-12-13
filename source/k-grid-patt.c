/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * This function checks if the pattern is crowded with block squares
 */
bool pattern_is_allowed_crowd(grid_t* grid, int stop_x, int stop_y)
{
  int block_amount = 0;

  for(int x = stop_x; x >= (stop_x + 2); x++)
  {
    for(int y = stop_y; y >= (stop_y + 2); y++)
    {
      if(xy_real_square_is_block(grid, x, y))
      {
        block_amount++;
      }

      if(block_amount > 2) return false;
    }
  }

  return true;
}

/*
 * This function checks if a letter square is being trapped
 *
 * # . .
 * . + .
 * # . .
 *
 * # . #
 * . + .
 * . . .
 */
bool pattern_is_allowed_trap(grid_t* grid, int stop_x, int stop_y)
{
  if(xy_real_square_is_block(grid, stop_x - 1, stop_y - 1))
  {
    if(xy_real_square_is_block(grid, stop_x + 1, stop_y - 1))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, stop_x - 1, stop_y + 1))
    {
      return false;
    }
  }

  return true;
}
