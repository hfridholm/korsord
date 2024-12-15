/*
 * k-grid-prep.c - prepare grid before generation
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * Assign the out most squares as SQUARE_BORDER
 *
 * This outer border is a buffer that prevents index out of range
 */
static void grid_prep_border(grid_t* grid)
{
  square_t* square;

  for(int x = 0; x < (grid->width + 2); x++)
  {
    square = xy_real_square_get(grid, x, 0);
    
    if(square) square->type = SQUARE_BORDER;

    square = xy_real_square_get(grid, x, grid->height + 1);
    
    if(square) square->type = SQUARE_BORDER;
  }

  for(int y = 0; y < (grid->height + 2); y++)
  {
    square = xy_real_square_get(grid, 0, y);
    
    if(square) square->type = SQUARE_BORDER;

    square = xy_real_square_get(grid, grid->width + 1, y);
    
    if(square) square->type = SQUARE_BORDER;
  }
}

// __builtin_clz counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1 << (sizeof(n) * 8 - __builtin_clz(n)))

/*
 *
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
 *
 */
static void grid_corner_indexes_get(int** indexes, int* count, grid_t* grid)
{
  /*   
   * . X . . 
   * X A . .
   * . .\. .
   * . . \ .
   *
   * X . . . 
   * . A . .
   * . .\. .
   * . . \ .
   */
  for(int x = (grid->width + 1); x-- > 1;)
  {
    for(int y = (grid->height + 1); y-- > 1;)
    {
      if(xy_real_square_is_border(grid, x, y))
      {
        continue;
      }

      if((xy_real_square_is_border(grid, x, y - 1) &&
          xy_real_square_is_border(grid, x - 1, y)) ||

         (xy_real_square_is_border(grid, x - 1, y - 1) &&
         !xy_real_square_is_border(grid, x, y - 1) &&
         !xy_real_square_is_border(grid, x - 1, y)))
      {
        int index = (y * (grid->width + 2) + x);

        index_append(indexes, count, index);
      }
    }
  }

  /*
   * . . / .
   * . ./. .
   * X V . .
   * . X . . 
   *
   * The pattern below is at the next to left edge
   *
   * . . / .
   * - ./. .
   * X V . .
   * . . . . 
   */
  for(int x = (grid->width + 1); x-- > 1;)
  {
    for(int y = 1; y < (grid->height + 1); y++)
    {
      if(xy_real_square_is_border(grid, x, y))
      {
        continue;
      }

      if((xy_real_square_is_border(grid, x - 1, y) &&
          xy_real_square_is_border(grid, x, y + 1)) ||

         (xy_real_square_is_border(grid, x - 1, y) &&
         !xy_real_square_is_border(grid, x - 1, y - 1)))
      {
        int index = (y * (grid->width + 2) + x);

        index_append(indexes, count, index);
      }
    }
  }

  /*
   * . . X .
   * . . A X
   * . ./. .
   * . / . .
   *
   * The pattern below is at the under edge of right block
   *
   * . - X .
   * . . A .
   * . ./. .
   * . / . .
   */
  for(int x = 1; x < (grid->width + 1); x++)
  {
    for(int y = (grid->height + 1); y-- > 1;)
    {
      if(xy_real_square_is_border(grid, x, y))
      {
        continue;
      }

      if((xy_real_square_is_border(grid, x, y - 1) &&
          xy_real_square_is_border(grid, x + 1, y)) ||

         (xy_real_square_is_border(grid, x, y - 1) &&
         !xy_real_square_is_border(grid, x - 1, y - 1)))
      {
        int index = (y * (grid->width + 2) + x);

        index_append(indexes, count, index);
      }
    }
  }
}

/*
 * Add blocks at left and top side of grid
 */
static void grid_prep_blocks(grid_t* grid)
{
  // 1. Get the squares in the corners
  int* indexes = NULL;
  int  count = 0;

  grid_corner_indexes_get(&indexes, &count, grid);

  // 2. Assign SQUARE_BLOCK to the squares in the corners
  for(int index = 0; index < count; index++)
  {
    int square_index = indexes[index];

    int x = square_index % (grid->width + 2);
    int y = square_index / (grid->width + 2);

    xy_real_square_set_block(grid, x, y);
  }

  // 3. Randomly assign SQUARE_BLOCK to squares at edges
  for(int index = 0; index < count; index++)
  {
    int square_index = indexes[index];

    int start_x = square_index % (grid->width + 2);
    int start_y = square_index / (grid->width + 2);

    // Important: At least one open square next to top left block
    // Implement this as soon as possible

    /*
     * X X X X
     * X # + .
     */
    bool last_is_block = true;

    for(int x = (start_x + 1); x < (grid->width + 0); x++)
    {
      // This ensures that the egde is being followed
      if(!xy_real_square_is_border(grid, x, start_y - 1) ||
          xy_real_square_is_border(grid, x, start_y))
      {
        break;
      }

      square_t* square = xy_real_square_get(grid, x, start_y);

      if(square->type == SQUARE_BLOCK)
      {
        last_is_block = true;

        continue;
      }

      // If the last square is not a block, this must be a block
      if(!last_is_block)
      {
        square->type = SQUARE_BLOCK;

        last_is_block = true;

        continue;
      }

      // If the last is a block, this has a 60% chance of also being a block
      if((rand() % 100) > 60)
      {
        square->type = SQUARE_BLOCK;

        last_is_block = true;
      }
      else last_is_block = false;
    }

    /*
     * X X
     * X #
     * X +
     * X .
     */
    last_is_block = true;

    for(int y = start_y; y < (grid->height + 2); y++)
    {
      // This ensures that the egde is being followed
      if(!xy_real_square_is_border(grid, start_x - 1, y) ||
          xy_real_square_is_border(grid, start_x, y))
      {
        break;
      }

      square_t* square = xy_real_square_get(grid, start_x, y);

      if(square->type == SQUARE_BLOCK)
      {
        last_is_block = true;

        continue;
      }

      // If the last square is not a block, this must be a block
      if(!last_is_block)
      {
        square->type = SQUARE_BLOCK;

        last_is_block = true;

        continue;
      }

      // If the last is a block, this has a 60% chance of also being a block
      if((rand() % 100) > 60)
      {
        square->type = SQUARE_BLOCK;

        last_is_block = true;
      }
      else last_is_block = false;
    }
  }

  free(indexes);
}

/*
 * Prepare the grid before generation
 */
int grid_prep(grid_t* grid)
{
  grid_prep_border(grid);

  grid_prep_blocks(grid);

  return 0;
}
