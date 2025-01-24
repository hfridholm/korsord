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
 */

/*
 * Check if nerby block is crowded, accounting for new block
 *
 * PARAMS:
 * - int real_x | Real x of nerby (this) block
 * - int real_y | Real y of nerby (this) block
 *
 * RETURN (bool is_crowded)
 */
static bool nerby_block_is_crowded(grid_t* grid, int real_x, int real_y)
{
  int block_amount = 1;
  bool nerby_prep = false;

  for(int x = (real_x - 1); x <= (real_x + 1); x++)
  {
    for(int y = (real_y - 1); y <= (real_y + 1); y++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

      if (!square) return true;

      if (square->type != SQUARE_BLOCK) continue;

      if (!square->is_prep)
      {
        block_amount++;

        if (block_amount > MAX_CROWD_AMOUNT) return true;
      }
      else if (!nerby_prep)
      {
        block_amount++;

        nerby_prep = true;

        if (block_amount > MAX_CROWD_AMOUNT) return true;
      }
    }
  }

  return false;
}

/*
 *
 * This function checks if the pattern is crowded with block squares
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

      if (!square) return false;

      if (square->type != SQUARE_BLOCK) continue;

      if (!square->is_prep)
      {
        block_amount++;

        if (block_amount > MAX_CROWD_AMOUNT) return false;

        if (nerby_block_is_crowded(grid, x, y)) return false;
      }
      else if (!nerby_prep)
      {
        block_amount++;

        nerby_prep = true;

        if (block_amount > MAX_CROWD_AMOUNT) return false;
      }
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
 */
static bool patt_block_is_allowed(grid_t* grid, int block_x, int block_y)
{
  int real_x = block_x + 3;
  int real_y = block_y + 3;

  /*
   *  . . . . .
   *  . . . . .
   *  . . + a a
   *  . . b . .
   *  . . b . .
   */
  if ((xy_real_square_is_blocking(grid, real_x + 1, real_y    )  || // a
       xy_real_square_is_blocking(grid, real_x + 2, real_y    )) &&

      (xy_real_square_is_blocking(grid, real_x,     real_y + 1)  || // b
       xy_real_square_is_blocking(grid, real_x,     real_y + 2)))
  {
    return false;
  }

  /*
   *  . . . . . .
   *  . . . . . .
   *  . . a + . .
   *  . . b . . .
   *  . . b . . .
   *
   * OBS: This case is legal:
   *  . . . . . .
   *  . X _ . . .
   *  . . a + . .
   *  . . b . . .
   *  . . b . . .
   */
  if (xy_real_square_is_block   (grid, real_x - 1, real_y    )  && // a

    !(xy_real_square_is_border  (grid, real_x - 2, real_y - 1)  && // X
     !xy_real_square_is_blocking(grid, real_x - 1, real_y - 1)) && // _

     (xy_real_square_is_blocking(grid, real_x - 1, real_y + 1)  || // b
      xy_real_square_is_blocking(grid, real_x - 1, real_y + 2)))
  {
    return false;
  }

  /*
   *  . . . . . .
   *  . . . . . .
   *  . a . + . .
   *  . b . . . .
   *  . b . . . .
   *
   * OBS: This case is legal:
   *  . . . . . .
   *  X _ . . . .
   *  . a . + . .
   *  . b . . . .
   *  . b . . . .
   */
  if (xy_real_square_is_block   (grid, real_x - 2, real_y    )  && // a

    !(xy_real_square_is_border  (grid, real_x - 3, real_y - 1)  && // X
     !xy_real_square_is_blocking(grid, real_x - 2, real_y - 1)) && // _

     (xy_real_square_is_blocking(grid, real_x - 2, real_y + 1)  || // b
      xy_real_square_is_blocking(grid, real_x - 2, real_y + 2)))
  {
    return false;
  }

  /*
   *  . . . . .
   *  . . a b c
   *  . . . . .
   *  . . + . .
   *  . . . . .
   *
   * OBS: This case is legal:
   *  . . . X .
   *  . . a _ c
   *  . . . . .
   *  . . + . .
   *  . . . . .
   */
  if (xy_real_square_is_block   (grid, real_x,     real_y - 2) && // a

     (xy_real_square_is_blocking(grid, real_x + 1, real_y - 2) || // b

     (xy_real_square_is_blocking(grid, real_x + 2, real_y - 2) && // c
     !xy_real_square_is_border  (grid, real_x + 1, real_y - 3)))) // X
  {
    return false;
  }

  /*
   *  . . . . .
   *  . . a b c
   *  . . + . .
   *  . . . . .
   *  . . . . .
   *
   * OBS: This case is legal:
   *  . . . X .
   *  . . a _ c
   *  . . + . .
   *  . . . . .
   *  . . . . .
   */
  if (xy_real_square_is_block   (grid, real_x,     real_y - 1) && // a

     (xy_real_square_is_blocking(grid, real_x + 1, real_y - 1) || // b

     (xy_real_square_is_blocking(grid, real_x + 2, real_y - 1) && // c
     !xy_real_square_is_border  (grid, real_x + 1, real_y - 2)))) // X
  {
    return false;
  }

  /*
   * This is only legal if it has been done in the prep stage
   * . X .
   * . + .
   * . . .
   */
  if(!xy_real_square_is_block (grid, real_x, real_y    ) && 
      xy_real_square_is_border(grid, real_x, real_y - 1))
  {
    return false;
  }

  /*
   * This is only legal if it has been done in the prep stage
   * . . .
   * X + .
   * . . .
   */
  if(!xy_real_square_is_block (grid, real_x    , real_y) && 
      xy_real_square_is_border(grid, real_x - 1, real_y))
  {
    return false;
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
  // An already blocking square is of course allowed
  if(xy_square_is_blocking(grid, block_x, block_y))
  {
    stats_patt_done_incr();

    return true;
  }

  // 1. Check if block would overwrite a letter
  if(xy_square_is_letter(grid, block_x, block_y))
  {
    stats_patt_letter_incr();

    return false;
  }

  // 2. Check if block would block other blocks
  if(!patt_block_is_allowed(grid, block_x, block_y))
  {
    stats_patt_block_incr();

    return false;
  }

  // 3. Check if block is trapping letters
  if(!patt_trap_is_allowed(grid, block_x, block_y))
  {
    stats_patt_trap_incr();

    return false;
  }

  // 4. Check if block makes the grid to crowded
  if(!patt_crowd_is_allowed(grid, block_x, block_y))
  {
    stats_patt_crowd_incr();

    return false;
  }

  // The block square is allowed
  stats_patt_none_incr();

  return true;
}
