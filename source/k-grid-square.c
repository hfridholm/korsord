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
  // return (y * grid->width) + x;
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
  // return (y * grid->width) + x;
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
 *
 */
void grid_xy_square_set_crossed(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_square_get(grid, x, y);

  if(square) square->is_crossed = true;
}

/*
 *
 */
bool xy_real_square_is_block(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_real_square_get(grid, x, y);

  return (square && square->type == SQUARE_BLOCK);
}

/*
 *
 */
bool xy_square_is_letter(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_square_get(grid, x, y);

  return (square && square->type == SQUARE_LETTER);
}

/*
 *
 */
bool xy_square_is_crossed(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_square_get(grid, x, y);

  return (square && square->is_crossed);
}

/*
 *
 */
bool grid_xy_real_square_is_border(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_real_square_get(grid, x, y);

  return (square && square->type == SQUARE_BORDER);
}

/*
 *
 */
void grid_xy_real_square_set_block(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_real_square_get(grid, x, y);

  if(square) square->type = SQUARE_BLOCK;
}

/*
 * Both SQUARE_BORDER and SQUARE_BLOCK is blocking
 */
bool xy_square_is_blocking(grid_t* grid, int x, int y)
{
  square_t* square = grid_xy_square_get(grid, x, y);

  return (!square || square->type == SQUARE_BLOCK
                  || square->type == SQUARE_BORDER);
}
