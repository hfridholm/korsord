/*
 * k-grid-prep.c - prepare grid before generation
 *
 * Change the preperation.
 * Call grid_words_use from outside prep call
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 *
 */
int PREP_EMPTY_CHANCE = 70;

// __builtin_clz counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1 << (sizeof(n) * 8 - __builtin_clz(n)))

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
  for(int x = (grid->width + 3); x-- > 3;)
  {
    for(int y = (grid->height + 3); y-- > 3;)
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
        int index = xy_real_index_get(grid, x, y);

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
  for(int x = (grid->width + 3); x-- > 3;)
  {
    for(int y = 3; y < (grid->height + 3); y++)
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
        int index = xy_real_index_get(grid, x, y);

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
  for(int x = 3; x < (grid->width + 3); x++)
  {
    for(int y = (grid->height + 3); y-- > 3;)
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
        int index = xy_real_index_get(grid, x, y);

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

    square_t* square = real_square_get(grid, square_index);

    // Don't overwrite model letters
    if(square->type == SQUARE_LETTER) continue;

    square->type = SQUARE_BLOCK;
  }


  // 3. Randomly assign SQUARE_BLOCK to squares at edges
  for(int index = 0; index < count; index++)
  {
    int square_index = indexes[index];

    int start_x = real_index_x_get(grid, square_index);
    int start_y = real_index_y_get(grid, square_index);

    /*
     * X X X X X
     * X # . + .
     */

    /*
     * The square to the right of the top left block must be empty
     */
    if(start_x < grid->width)
    {
      xy_real_square_set_empty(grid, start_x + 1, start_y);
    }

    bool last_is_block = false;

    for(int x = (start_x + 2); x < (grid->width + 2); x++)
    {
      // This ensures that the egde is being followed
      if(!xy_real_square_is_border(grid, x, start_y - 1) ||
          xy_real_square_is_border(grid, x, start_y))
      {
        break;
      }

      square_t* square = xy_real_square_get(grid, x, start_y);

      // Don't overwrite model letters
      if(square->type == SQUARE_LETTER) continue;

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

      // If the last is a block, this has a chance of also being a block
      if((rand() % 100) > PREP_EMPTY_CHANCE)
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

      // If the last is a block, this has a chance of also being a block
      if((rand() % 100) > PREP_EMPTY_CHANCE)
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
 * Identify words in grid and _use them,
 * so they can't be used elsewhere in the grid
 */
static void grid_words_use(wbase_t* wbase, grid_t* grid)
{
  char** words = NULL;
  size_t count = 0;

  if(grid_words_get(&words, &count, grid) != 0)
  {
    return;
  }

  for(size_t index = 0; index < count; index++)
  {
    wbase_word_use(wbase, words[index]);
  }

  words_free(&words, count);
}

/*
 * Prepare the grid before generation
 */
void grid_prep(wbase_t* wbase, grid_t* grid)
{
  grid_prep_blocks(grid);

  grid_words_use(wbase, grid);
}
