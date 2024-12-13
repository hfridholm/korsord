/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * This function checks if the pattern is crowded with block squares
 *
 * PARAMS:
 * - int block_x | Not real x
 * - int block_y | Not real y
 */
bool pattern_is_allowed_crowd(grid_t* grid, int block_x, int block_y)
{
  int block_amount = 0;

  for(int x = block_x; x >= (block_x + 2); x++)
  {
    for(int y = block_y; y >= (block_y + 2); y++)
    {
      if(xy_real_square_is_block(grid, x, y))
      {
        block_amount++;
      }

      if(block_amount > 1) return false;
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
bool pattern_is_allowed_trap(grid_t* grid, int block_x, int block_y)
{
  if(xy_real_square_is_block(grid, block_x - 1, block_y - 1))
  {
    if(xy_real_square_is_block(grid, block_x + 1, block_y - 1))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x - 1, block_y + 1))
    {
      return false;
    }
  }

  return true;
}
