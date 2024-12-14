/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * The x and y is not accounting for border
 *
 * real_x = fake_x + 1
 * real_y = fake_y + 1
 *
 * 0 1 2
 * 1 . .
 * 2 . .
 */

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
 * The x and y is not accounting for border
 *
 * real_x = fake_x + 1
 * real_y = fake_y + 1
 *
 * This function checks if a letter square is being trapped
 *
 */
bool pattern_is_allowed_trap(grid_t* grid, int block_x, int block_y)
{
  /*
   * # . .
   * . + .
   * # . .
   *
   * # . #
   * . + .
   * . . .
   */
  if(xy_real_square_is_block(grid, block_x, block_y))
  {
    if(xy_real_square_is_block(grid, block_x + 2, block_y))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x, block_y + 2))
    {
      return false;
    }
  }

  /*
   * . . #
   * . + .
   * . . #
   *
   * . . .
   * . + .
   * # . #
   */
  if(xy_real_square_is_block(grid, block_x + 2, block_y + 2))
  {
    if(xy_real_square_is_block(grid, block_x + 2, block_y))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x, block_y + 2))
    {
      return false;
    }
  }

  return true;
}

/*
 *
 */
bool pattern_is_allowed_edge(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * # + #
   * . X .
   */
  if(grid_xy_real_square_is_border(grid, block_x + 1, block_y + 2))
  {
    if(xy_real_square_is_block(grid, block_x, block_y + 1))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 2, block_y + 1))
    {
      return false;
    }
  }

  /*
   * . # .
   * . + X
   * . # .
   */
  if(grid_xy_real_square_is_border(grid, block_x + 2, block_y + 1))
  {
    if(xy_real_square_is_block(grid, block_x + 1, block_y))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 1, block_y + 2))
    {
      return false;
    }
  }

  /*
   * . X .
   * . + .
   * . . .
   */
  if(grid_xy_real_square_is_border(grid, block_x + 1, block_y))
  {
    return false;
  }

  /*
   * . . .
   * X + .
   * . . .
   */
  if(grid_xy_real_square_is_border(grid, block_x, block_y + 1))
  {
    return false;
  }

  return true;
}

/*
 *
 */
static bool pattern_is_allowed_corner(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * . + X
   * . X .
   */
  if(grid_xy_real_square_is_border(grid, block_x + 2, block_y + 1))
  {
    if(grid_xy_real_square_is_border(grid, block_x + 1, block_y + 2))
    {
      return false;
    }
  }

  return true;
}

/*
 * The x and y is not accounting for border
 *
 * PARAMS
 * - int block_x | Not real x
 * - int block_y | Not real y
 */
bool block_square_is_allowed(grid_t* grid, int block_x, int block_y)
{
  if(xy_square_is_letter(grid, block_x, block_y)) return false;

  if(!pattern_is_allowed_trap(grid, block_x, block_y)) return false;

  if(!pattern_is_allowed_crowd(grid, block_x, block_y)) return false;

  if(!pattern_is_allowed_edge(grid, block_x, block_y)) return false;

  if(!pattern_is_allowed_corner(grid, block_x, block_y)) return false;

  return true;
}
