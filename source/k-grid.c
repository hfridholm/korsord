/*
 * k-grid.c - basic grid functions
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 *
 */
grid_t* grid_create(int width, int height)
{
  grid_t* grid = malloc(sizeof(grid_t));

  if(!grid) return NULL;

  grid->width  = width;
  grid->height = height;

  grid->count = (width * height);

  grid->squares = malloc(sizeof(square_t) * grid->count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  grid->cross_count = 0;
  grid->word_count = 0;

  for(int index = 0; index < grid->count; index++)
  {
    grid->squares[index] = (square_t)
    {
      .type = TYPE_NONE,
      .letter = '\0',
      .is_crossed = false
    };
  }

  // Set neccessary block squares
  grid->squares[0].type = TYPE_BLOCK;

  grid->squares[width - 1].type = TYPE_BLOCK;

  grid->squares[(height - 1) * width].type = TYPE_BLOCK;

  return grid;
}

/*
 *
 */
void grid_copy(grid_t* copy, grid_t* grid)
{
  if(copy->count != grid->count) return;

  for(int index = 0; index < copy->count; index++)
  {
    copy->squares[index] = grid->squares[index];
  }

  copy->cross_count = grid->cross_count;
  copy->word_count = grid->word_count;
}

/*
 *
 */
grid_t* grid_dup(grid_t* grid)
{
  grid_t* dup_grid = malloc(sizeof(grid_t));

  if(!dup_grid) return NULL;

  dup_grid->width  = grid->width;
  dup_grid->height = grid->height;

  dup_grid->count = grid->count;

  dup_grid->squares = malloc(sizeof(square_t) * dup_grid->count);

  if(!dup_grid->squares)
  {
    free(dup_grid);

    return NULL;
  }

  for(int index = 0; index < dup_grid->count; index++)
  {
    dup_grid->squares[index] = grid->squares[index];
  }

  dup_grid->cross_count = grid->cross_count;
  dup_grid->word_count = grid->word_count;

  return dup_grid;
}

/*
 *
 */
void grid_free(grid_t** grid)
{
  if(!grid || !(*grid)) return;

  free((*grid)->squares);

  free(*grid);

  *grid = NULL;
}

void grid_print(grid_t* grid)
{
  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      int index = (y * grid->width) + x;

      square_t square = grid->squares[index];

      char symbol = (square.type == TYPE_NONE ? '.' : square.type == TYPE_BLOCK ? '#' : square.letter);


      if(square.is_crossed)
      {
        printf("\033[32m%c \033[0m", symbol);
      }
      else printf("%c ", symbol);
    }

    printf("\n");
  }

  printf("\n");
}
