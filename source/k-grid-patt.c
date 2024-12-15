/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

extern stats_t stats;

#define MAX_CROWD_AMOUNT 3

/*
 * The x and y is not accounting for border
 *
 * real_x = fake_x + 1
 * real_y = fake_y + 1
 *
 * 0 1 2
 * 1 . .
 * 2 . .
 *
 * When converting fake x and y to real x and y,
 * 0 - 1 = -1, which is not a valid real x or y,
 * therefor, no subraction is allowed
 */

/*
 * This function checks if the pattern is crowded with block squares
 *
 * PARAMS:
 * - int block_x | Not real x
 * - int block_y | Not real y
 */
static bool patt_crowd_is_allowed(grid_t* grid, int block_x, int block_y)
{
  int block_amount = 0;

  for(int x = block_x; x <= (block_x + 2); x++)
  {
    for(int y = block_y; y <= (block_y + 2); y++)
    {
      if(xy_real_square_is_block(grid, x, y))
      {
        block_amount++;

        if(block_amount > MAX_CROWD_AMOUNT) return false;
      }
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
static bool patt_trap_is_allowed(grid_t* grid, int block_x, int block_y)
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
static bool patt_edge_is_allowed(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * # + #
   * . X .
   */
  if(xy_real_square_is_border(grid, block_x + 1, block_y + 2))
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
  if(xy_real_square_is_border(grid, block_x + 2, block_y + 1))
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
   * This is legal if it has been done in the prep stage
   * . X .
   * . + .
   * . . .
   */
  if(!xy_square_is_block(grid, block_x, block_y) && 
      xy_real_square_is_border(grid, block_x + 1, block_y))
  {
    return false;
  }

  /*
   * This is legal if it has been done in the prep stage
   * . . .
   * X + .
   * . . .
   */
  if(!xy_square_is_block(grid, block_x, block_y) && 
      xy_real_square_is_border(grid, block_x, block_y + 1))
  {
    return false;
  }

  return true;
}

/*
 *
 */
static bool patt_corner_is_allowed(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * . + X
   * . X .
   */
  if(xy_real_square_is_border(grid, block_x + 2, block_y + 1))
  {
    if(xy_real_square_is_border(grid, block_x + 1, block_y + 2))
    {
      return false;
    }
  }

  return true;
}

/*
 * The x and y is not accounting for border
 *
 * EXPECTS:
 * - block_x and block_y are inside grid
 *
 * PARAMS
 * - int block_x | Not real x
 * - int block_y | Not real y
 */
bool block_is_allowed(grid_t* grid, int block_x, int block_y)
{
  // An already blocking square is of course allowed
  if(xy_square_is_blocking(grid, block_x, block_y))
  {
    stats.patt.block_count++;

    return true;
  }

  if(xy_square_is_letter(grid, block_x, block_y))
  {
    stats.patt.letter_count++;

    return false;
  }

  if(!patt_trap_is_allowed(grid, block_x, block_y))
  {
    stats.patt.trap_count++;

    return false;
  }

  if(!patt_crowd_is_allowed(grid, block_x, block_y))
  {
    stats.patt.crowd_count++;

    return false;
  }

  if(!patt_edge_is_allowed(grid, block_x, block_y))
  {
    stats.patt.edge_count++;

    return false;
  }

  if(!patt_corner_is_allowed(grid, block_x, block_y))
  {
    stats.patt.corner_count++;

    return false;
  }

  stats.patt.none_count++;

  return true;
}
