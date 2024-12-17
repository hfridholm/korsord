/*
 * k-grid.c - basic grid functions
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

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

  int real_count = (width + 2) * (height + 2);

  grid->squares = malloc(sizeof(square_t) * real_count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  grid->cross_count = 0;
  grid->word_count = 0;

  for(int index = 0; index < real_count; index++)
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
 * Empty allocated grid struct
 *
 * RETURN (grid_t* grid)
 * - NULL | Bad input
 */
grid_t* grid_clear(grid_t* grid)
{
  if(!grid) return NULL;

  int real_count = (grid->width + 2) * (grid->height + 2);

  for(int index = 0; index < real_count; index++)
  {
    grid->squares[index] = (square_t)
    {
      .type = SQUARE_EMPTY,
      .letter = '\0',
      .is_crossed = false
    };
  }

  grid->cross_count = 0;
  grid->word_count = 0;

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
  if(!copy) return NULL;

  // If the grid is NULL, that means clearing copy
  if(!grid) return grid_clear(copy);

  if(copy->square_count != grid->square_count)
  {
    return NULL;
  }

  int real_count = (grid->width + 2) * (grid->height + 2);

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
  grid_t* dup = malloc(sizeof(grid_t));

  if(!dup) return NULL;

  dup->width  = grid->width;
  dup->height = grid->height;

  dup->square_count = grid->square_count;

  int real_count = (grid->width + 2) * (grid->height + 2);

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
