/*
 * k-grid.c - basic grid functions
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * Extra SQUARE_BORDER squares are added around the grid
 *
 * X X X X X X X X
 * X X X X X X X X
 * X X X X X X X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X X X X X X
 * X X X X X X X X
 */

/*
 * Create crossword grid struct
 *
 * RETURN (grid_t* grid)
 * - NULL | Failed to create grid
 */
grid_t* grid_create(int width, int height)
{
  grid_t* grid = malloc(sizeof(grid_t));

  if(!grid) return NULL;

  grid->width  = width;
  grid->height = height;

  int real_count = (width + 5) * (height + 5);

  grid->squares = malloc(sizeof(square_t) * real_count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  grid->cross_count = 0;
  grid->word_count = 0;

  // Initialize empty squares and border squares
  for(int x = 0; x < (width + 5); x++)
  {
    for(int y = 0; y < (height + 5); y++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

      *square = (square_t)
      {
        .letter     = '\0',
        .is_crossed = false,
        .is_prep    = false
      };

      if ((x >= 3) && (x < (width  + 3)) &&
          (y >= 3) && (y < (height + 3)))
      {
        square->type = SQUARE_EMPTY;
      }
      else
      {
        square->type = SQUARE_BORDER;
      }
    }
  }

  return grid;
}

/*
 * Empty allocated grid struct
 *
 * RETURN (grid_t* grid)
 * - NULL | Bad input
 */
grid_t* grid_clear(grid_t* grid)
{
  if(!grid) return NULL;

  for(int x = 0; x < grid->width; x++)
  {
    for(int y = 0; y < grid->height; y++)
    {
      square_t* square = xy_square_get(grid, x, y);

      *square = (square_t)
      {
        .type = SQUARE_EMPTY,
        .letter = '\0',
        .is_crossed = false
      };
    }
  }

  grid->cross_count = 0;
  grid->word_count = 0;

  return grid;
}

/*
 * Copy crossword grid struct
 * 
 * If grid is NULL copy just gets cleared
 *
 * EXPECT:
 * - copy and grid have the same size
 *
 * RETURN (grid_t* copy)
 * - NULL | Bad input
 */
grid_t* grid_copy(grid_t* copy, grid_t* grid)
{
  if(!copy) return NULL;

  if(!grid) return grid_clear(copy);

  if ((copy->width  != grid->width) ||
      (copy->height != grid->height))
  {
    return NULL;
  }

  int real_count = (grid->width + 5) * (grid->height + 5);

  memcpy(copy->squares, grid->squares, sizeof(square_t) * real_count);

  copy->cross_count = grid->cross_count;
  copy->word_count = grid->word_count;

  return copy;
}

/*
 * Duplicate crossword grid struct
 *
 * RETURN (grid_t* dup)
 * - NULL | Failed to create duplicate
 */
grid_t* grid_dup(grid_t* grid)
{
  if(!grid) return NULL;

  grid_t* dup = malloc(sizeof(grid_t));

  if(!dup) return NULL;

  dup->width  = grid->width;
  dup->height = grid->height;

  int real_count = (grid->width + 5) * (grid->height + 5);

  dup->squares = malloc(sizeof(square_t) * real_count);

  if(!dup->squares)
  {
    free(dup);

    return NULL;
  }

  memcpy(dup->squares, grid->squares, sizeof(square_t) * real_count);

  dup->cross_count = grid->cross_count;
  dup->word_count = grid->word_count;

  return dup;
}

/*
 * Free crossword grid struct
 *
 * After the struct is freed, the pointer is set to NULL
 */
void grid_free(grid_t** grid)
{
  if(!grid || !(*grid)) return;

  free((*grid)->squares);

  free(*grid);

  *grid = NULL;
}
