/*
 * k-grid-patt.c - check if block is allowed
 *
 * These functions only use real squares and
 * are therefor dependent on border layout
 *
 * . . . 3 . .
 * . . . 2 . .
 * . . . 1 . .
 * 3 2 1 + 1 2
 * . . . 1 . .
 * . . . 2 . .
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-stats.h"

/*
 *
 */
int MAX_CROWD_AMOUNT = 2;

/*
 * This function checks if the pattern is crowded with block squares
 *
 * Grid prepare blocks (blocks at top and left edges) only count for 1 block together
 *
 *  X . . . .
 *  X # . . .
 *  X # + . . This would give nerby blocks: 1
 *  X # . . .
 *  X . . . .
 *
 *  X . . . .
 *  X . . # .
 *  X # + . . This would give nerby blocks: 2
 *  X # . . .
 *  X . . . .
 *
 * EXPECTS:
 * - (block_x, block_y) is not SQUARE_BLOCK
 *
 * PARAMS:
 * - int block_x | Not real x
 * - int block_y | Not real y
 *
 * RETURN (bool is_allowed)
 */
static bool patt_crowd_is_allowed(grid_t* grid, int block_x, int block_y)
{
  int real_x = block_x + 3;
  int real_y = block_y + 3;

  int block_amount = 0;
  bool nerby_prep = false;

  for(int x = (real_x - 1); x <= (real_x + 1); x++)
  {
    for(int y = (real_y - 1); y <= (real_y + 1); y++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

      if(!square) return false;

      if (square->type == SQUARE_BLOCK &&
         (!square->is_prep || (square->is_prep && !nerby_prep)))
      {
        block_amount++;

        if(block_amount >= MAX_CROWD_AMOUNT) return false;
      }

      if(square->is_prep) nerby_prep = true;
    }
  }

  return true;
}

/*
 * This function checks if a letter square is being trapped
 *
 * RETURN (bool is_allowed)
 *
 * Future: Increase the scope to 5x5
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
  if(xy_real_square_is_block(grid, block_x + 2, block_y + 2))
  {
    if(xy_real_square_is_block(grid, block_x + 4, block_y + 2))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 2, block_y + 4))
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
  if(xy_real_square_is_block(grid, block_x + 4, block_y + 4))
  {
    if(xy_real_square_is_block(grid, block_x + 4, block_y + 2))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 2, block_y + 4))
    {
      return false;
    }
  }

  return true;
}

/*
 * RETURN (bool is_allowed)
 *
 * This function will be obsolete in the future
 */
static bool patt_edge_is_allowed(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * # + #
   * . X .
   */
  if(xy_real_square_is_border(grid, block_x + 3, block_y + 4))
  {
    if(xy_real_square_is_block(grid, block_x + 2, block_y + 3))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 4, block_y + 3))
    {
      return false;
    }
  }

  /*
   * . # .
   * . + X
   * . # .
   */
  if(xy_real_square_is_border(grid, block_x + 4, block_y + 3))
  {
    if(xy_real_square_is_block(grid, block_x + 3, block_y + 2))
    {
      return false;
    }

    if(xy_real_square_is_block(grid, block_x + 3, block_y + 4))
    {
      return false;
    }
  }

  /*
   * This is only legal if it has been done in the prep stage
   * . X .
   * . + .
   * . . .
   */
  if(!xy_real_square_is_block(grid, block_x + 3, block_y + 3) && 
      xy_real_square_is_border(grid, block_x + 3, block_y + 2))
  {
    return false;
  }

  /*
   * This is only legal if it has been done in the prep stage
   * . . .
   * X + .
   * . . .
   */
  if(!xy_real_square_is_block(grid, block_x + 3, block_y + 3) && 
      xy_real_square_is_border(grid, block_x + 2, block_y + 3))
  {
    return false;
  }

  return true;
}

/*
 * RETURN (bool is_allowed)
 *
 * This function will be obsolete in the future
 */
static bool patt_corner_is_allowed(grid_t* grid, int block_x, int block_y)
{
  /*
   * . . .
   * . + X
   * . X .
   */
  if(xy_real_square_is_border(grid, block_x + 4, block_y + 3))
  {
    if(xy_real_square_is_border(grid, block_x + 3, block_y + 4))
    {
      return false;
    }
  }

  return true;
}

/*
 * Check if a blocking square is allowed
 *
 * The order of the checks influence performance
 * The checks that catch most should be first
 *
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
  // 1. Check if block would overwrite a letter
  if(xy_square_is_letter(grid, block_x, block_y))
  {
    stats_patt_letter_incr();

    return false;
  }

  // An already blocking square is of course allowed
  if(xy_square_is_blocking(grid, block_x, block_y))
  {
    stats_patt_block_incr();

    return true;
  }

  // 2. Check if block makes for a good edge
  if(!patt_edge_is_allowed(grid, block_x, block_y))
  {
    stats_patt_edge_incr();

    return false;
  }

  // 3. Check if block is trapping letters
  if(!patt_trap_is_allowed(grid, block_x, block_y))
  {
    stats_patt_trap_incr();

    return false;
  }

  // 4. Check if block is at the bottom right corner
  if(!patt_corner_is_allowed(grid, block_x, block_y))
  {
    stats_patt_corner_incr();

    return false;
  }

  // 5. Check if block makes the grid to crowded
  if(!patt_crowd_is_allowed(grid, block_x, block_y))
  {
    stats_patt_crowd_incr();

    return false;
  }

  // The block square is allowed
  stats_patt_none_incr();

  return true;
}
