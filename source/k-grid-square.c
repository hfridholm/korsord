/*
 * k-grid.c - basic grid square functions
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * X X X X X X X X
 * X X X X X X X X
 * X X X X X X X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X X X X X X
 * X X X X X X X X
 *
 * The outer most squares are not ment to be accessed
 *
 * They are a buffer when close to an edge
 */

/*
 * Get the real index, including for the border squares
 */
int xy_real_index_get(grid_t* grid, int x, int y)
{
  return (y * (grid->width + 5)) + x;
}

/*
 * Get the real square, including border square
 */
square_t* xy_real_square_get(grid_t* grid, int x, int y)
{
  int index = xy_real_index_get(grid, x, y);

  return grid->squares + index;
}

/*
 *
 */
int real_index_x_get(grid_t* grid, int index)
{
  return (index % (grid->width + 5));
}

/*
 *
 */
int real_index_y_get(grid_t* grid, int index)
{
  return (index / (grid->width + 5));
}

/*
 *
 */
square_t* real_square_get(grid_t* grid, int index)
{
  return grid->squares + index;
}

/*
 * EXPECTS:
 * - grid is allocated
 *
 * RETURN (int index)
 * - -1 | Error
 */
int xy_index_get(grid_t* grid, int x, int y)
{
  // 1. Check so x, y is inside grid
  if(x < 0 || x >= grid->width)  return -1;

  if(y < 0 || y >= grid->height) return -1;

  return (y + 3) * (grid->width + 5) + (x + 3);
}

/*
 * EXPECTS:
 * - grid is allocated
 *
 * RETURN (square_t* square)
 */
square_t* xy_square_get(grid_t* grid, int x, int y)
{
  int index = xy_index_get(grid, x, y);

  if(index == -1) return NULL;

  return grid->squares + index;
}

/*
 * Set is_crossed flag to square in grid
 */
void xy_square_set_crossed(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  if(square) square->is_crossed = true;

  grid->cross_count++;
}

/*
 * Set real square type SQUARE_EMPTY
 */
void xy_real_square_set_empty(grid_t* grid, int x, int y)
{
  square_t* square = xy_real_square_get(grid, x, y);

  if(square) square->type = SQUARE_EMPTY;
}

/*
 * Check if real square is SQUARE_BLOCK
 */
bool xy_real_square_is_block(grid_t* grid, int x, int y)
{
  square_t* square = xy_real_square_get(grid, x, y);

  return (square && square->type == SQUARE_BLOCK);
}

/*
 * Check if square is SQUARE_BLOCK
 */
bool xy_square_is_block(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return (square && square->type == SQUARE_BLOCK);
}

/*
 * Check if square is SQUARE_LETTER
 */
bool xy_square_is_letter(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return (square && square->type == SQUARE_LETTER);
}

/*
 * Check if square has flag is_crossed
 */
bool xy_square_is_crossed(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return (square && square->is_crossed);
}

/*
 * Check if real square is SQUARE_BORDER
 */
bool xy_real_square_is_border(grid_t* grid, int x, int y)
{
  square_t* square = xy_real_square_get(grid, x, y);

  return (square && square->type == SQUARE_BORDER);
}

/*
 * Check if square is SQUARE_BORDER
 */
bool xy_square_is_border(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return (square && square->type == SQUARE_BORDER);
}

/*
 * Set real square type SQUARE_BLOCK
 */
void xy_real_square_set_block(grid_t* grid, int x, int y)
{
  square_t* square = xy_real_square_get(grid, x, y);

  if(square) square->type = SQUARE_BLOCK;
}

/*
 * Both SQUARE_BORDER and SQUARE_BLOCK is blocking
 */
bool xy_real_square_is_blocking(grid_t* grid, int x, int y)
{
  square_t* square = xy_real_square_get(grid, x, y);

  return (!square || square->type == SQUARE_BLOCK
                  || square->type == SQUARE_BORDER);
}

/*
 * Both SQUARE_BORDER and SQUARE_BLOCK is blocking
 */
bool xy_square_is_blocking(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return (!square || square->type == SQUARE_BLOCK
                  || square->type == SQUARE_BORDER);
}

/*
 * Check if square is done (crossed letter)
 */
bool xy_square_is_done(grid_t* grid, int x, int y)
{
  square_t* square = xy_square_get(grid, x, y);

  return !(square->type == SQUARE_EMPTY ||
          (square->type == SQUARE_LETTER && !square->is_crossed));
}
