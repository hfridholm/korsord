/*
 * k-grid-prep.c - prepare grid before generation
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 *
 */
int PREP_EMPTY_CHANCE = 70;

// __builtin_clzll counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1ULL << (64 - __builtin_clzll(n)))

/*
 * Append index to index array
 */
static int index_append(int** indexes, int* count, int index)
{
  if(*count == 0 || ((*count) + 1) >= CAPACITY(*count))
  {
    int* new_indexes = realloc(*indexes, sizeof(int) * CAPACITY((*count) + 1));

    if(!new_indexes) return 1;

    *indexes = new_indexes;
  }

  (*indexes)[(*count)++] = index;

  return 0;
}

/*
 * Get indexes of corners in grid
 */
static void grid_corner_indexes_get(int** indexes, int* count, grid_t* grid)
{
  /*   
   * Add indexes of top left corners
   */
  for(int x = (grid->width + 3); x-- > 3;)
  {
    for(int y = (grid->height + 3); y-- > 3;)
    {
      if (!xy_real_square_is_border(grid, x, y) &&
        /*
         *  . X . .
         *  X A . .
         *  . .\. .
         *  . . \ .
         */
         ((xy_real_square_is_border(grid, x,     y - 1) &&
           xy_real_square_is_border(grid, x - 1, y    )) ||
        /*
         *  X . . .
         *  . A . .
         *  . .\. .
         *  . . \ .
         */
          (xy_real_square_is_border(grid, x - 1, y - 1) &&
          !xy_real_square_is_border(grid, x,     y - 1) &&
          !xy_real_square_is_border(grid, x - 1, y    ))))
      {
        int index = xy_real_index_get(grid, x, y);

        index_append(indexes, count, index);
      }
    }
  }

  /*
   * Add indexes of bottom left corners
   */
  for(int x = (grid->width + 3); x-- > 3;)
  {
    for(int y = 3; y < (grid->height + 3); y++)
    {
      if (!xy_real_square_is_border(grid, x, y) &&
        /*
         *  . . / .
         *  . ./. .
         *  X V . .
         *  . X . .
         */
         ((xy_real_square_is_border(grid, x - 1, y    ) &&
           xy_real_square_is_border(grid, x,     y + 1)) ||
        /*
         *  . . / .
         *  - ./. .
         *  X V . .
         *  . . . .
         */
          (xy_real_square_is_border(grid, x - 1, y    ) &&
          !xy_real_square_is_border(grid, x - 1, y - 1))))
      {
        int index = xy_real_index_get(grid, x, y);

        index_append(indexes, count, index);
      }
    }
  }

  /*
   * Add indexes of top right corners
   */
  for(int x = 3; x < (grid->width + 3); x++)
  {
    for(int y = (grid->height + 3); y-- > 3;)
    {
      if (!xy_real_square_is_border(grid, x, y) &&
        /*
         *  . . X .
         *  . . A X
         *  . ./. .
         *  . / . .
         */
         ((xy_real_square_is_border(grid, x,     y - 1) &&
           xy_real_square_is_border(grid, x + 1, y    )) ||
        /*
         *  . - X .
         *  . . A .
         *  . ./. .
         *  . / . .
         */
          (xy_real_square_is_border(grid, x,     y - 1) &&
          !xy_real_square_is_border(grid, x - 1, y - 1))))
      {
        int index = xy_real_index_get(grid, x, y);

        index_append(indexes, count, index);
      }
    }
  }
}

/*
 * Randomly assign SQUARE_BLOCK to squares at top edge
 *
 * X X X X X
 * X # . + .
 */
static void grid_prep_top_blocks(grid_t* grid, int start_x, int start_y)
{
  // The square to the right of the top left block can't be block
  if(start_x < grid->width)
  {
    square_t* square = xy_real_square_get(grid, start_x + 1, start_y);

    if(square && square->type == SQUARE_BLOCK)
    {
      square->type = SQUARE_EMPTY;
    }
  }

  bool last_is_block = false;

  for(int x = (start_x + 2); x < (grid->width + 2); x++)
  {
    // This ensures that the egde is being followed
    if (!xy_real_square_is_border(grid, x, start_y - 1) ||
         xy_real_square_is_border(grid, x, start_y))
    {
      break;
    }

    square_t* square = xy_real_square_get(grid, x, start_y);

    // Don't overwrite model letters
    if(square->type == SQUARE_LETTER) continue;

    /*
     * This square gets to be a block if either:
     * - it already is a block, or
     * - the last square wasn't a block, or
     * - it randomly is decided to be one
     */
    if (square->type == SQUARE_BLOCK ||
        !last_is_block ||
        (rand() % 100) > PREP_EMPTY_CHANCE)
    {
      square->type    = SQUARE_BLOCK;
      square->is_prep = true;

      last_is_block = true;
    }
    else last_is_block = false;
  }
}

/*
 * Randomly assign SQUARE_BLOCK to squares at left edge
 *
 * X X
 * X #
 * X +
 * X .
 */
static void grid_prep_left_blocks(grid_t* grid, int start_x, int start_y)
{
  bool last_is_block = true;

  for(int y = (start_y + 1); y < (grid->height + 2); y++)
  {
    // This ensures that the egde is being followed
    if(!xy_real_square_is_border(grid, start_x - 1, y) ||
        xy_real_square_is_border(grid, start_x, y))
    {
      break;
    }

    square_t* square = xy_real_square_get(grid, start_x, y);

    // Don't overwrite model letters
    if(square->type == SQUARE_LETTER) continue;

    /*
     * This square gets to be a block if either:
     * - it already is a block, or
     * - the last square wasn't a block, or
     * - it randomly is decided to be one
     */
    if (square->type == SQUARE_BLOCK ||
        !last_is_block ||
        (rand() % 100) > PREP_EMPTY_CHANCE)
    {
      square->type    = SQUARE_BLOCK;
      square->is_prep = true;

      last_is_block = true;
    }
    else last_is_block = false;
  }
}

/*
 * Prepare the grid before generation
 *
 * Add blocks at left and top side of grid
 *
 * Extract words from grid
 */
void grid_prep(grid_t* grid)
{
  // 1. Get the squares in the corners
  int* indexes = NULL;
  int  count = 0;

  grid_corner_indexes_get(&indexes, &count, grid);

  // 2. Assign SQUARE_BLOCK to the squares in the corners
  for(int index = 0; index < count; index++)
  {
    int square_index = indexes[index];

    square_t* square = real_square_get(grid, square_index);

    // Don't overwrite model letters
    if(square->type == SQUARE_LETTER) continue;

    square->type    = SQUARE_BLOCK;
    square->is_prep = true;
  }

  // 3. Randomly assign SQUARE_BLOCK to squares at edges
  for(int index = 0; index < count; index++)
  {
    int square_index = indexes[index];

    int start_x = real_index_x_get(grid, square_index);
    int start_y = real_index_y_get(grid, square_index);

    grid_prep_top_blocks(grid, start_x, start_y);

    grid_prep_left_blocks(grid, start_x, start_y);
  }

  free(indexes);
}
