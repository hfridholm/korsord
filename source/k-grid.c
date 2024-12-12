/*
 * k-grid.c - basic grid functions
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * X X X X X
 * X . . . X
 * X . . . X
 * X . . . X
 * X X X X X
 *
 * The outer most squares are not ment to be accessed
 *
 * They are a buffer when close to an edge
 */

/*
 * Get the real index, including for the border squares
 */
int grid_xy_real_index_get(grid_t* grid, int x, int y)
{
  return (y * (grid->width + 2)) + x;
}

/*
 * Get the real square, including border square
 */
square_t* grid_xy_real_square_get(grid_t* grid, int x, int y)
{
  int index = grid_xy_real_index_get(grid, x, y);

  return grid->squares + index;
}


/*
 * EXPECTS:
 * - grid is allocated
 *
 * RETURN (int index)
 * - -1 | Error
 */
int grid_xy_index_get(grid_t* grid, int x, int y)
{
  // 1. Check so x, y is inside grid
  if(x < 0 || x >= grid->width)  return -1;

  if(y < 0 || y >= grid->height) return -1;

  return (y + 1) * (grid->width + 2) + (x + 1);
}

/*
 * EXPECTS:
 * - grid is allocated
 *
 * RETURN (square_t* square)
 */
square_t* grid_xy_square_get(grid_t* grid, int x, int y)
{
  int index = grid_xy_index_get(grid, x, y);

  if(index == -1) return NULL;

  return grid->squares + index;
}

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

  grid->square_count = (width * height);

  grid->squares = malloc(sizeof(square_t) * grid->square_count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  grid->cross_count = 0;
  grid->word_count = 0;

  for(int index = 0; index < grid->square_count; index++)
  {
    grid->squares[index] = (square_t)
    {
      .type = SQUARE_EMPTY,
      .letter = '\0',
      .is_crossed = false
    };
  }

  return grid;
}

/*
 * Copy crossword grid struct
 *
 * The copy and the grid have to be same size
 *
 * RETURN (grid_t* copy)
 * - NULL | Bad input
 */
grid_t* grid_copy(grid_t* copy, grid_t* grid)
{
  if(copy->square_count != grid->square_count)
  {
    return NULL;
  }

  for(int index = 0; index < copy->square_count; index++)
  {
    copy->squares[index] = grid->squares[index];
  }

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
  grid_t* dup = malloc(sizeof(grid_t));

  if(!dup) return NULL;

  dup->width  = grid->width;
  dup->height = grid->height;

  dup->square_count = grid->square_count;

  dup->squares = malloc(sizeof(square_t) * dup->square_count);

  if(!dup->squares)
  {
    free(dup);

    return NULL;
  }

  for(int index = 0; index < dup->square_count; index++)
  {
    dup->squares[index] = grid->squares[index];
  }

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
