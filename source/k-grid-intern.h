/*
 * k-grid-intern.h - intern declarations in k-grid
 *
 * Written by Hampus Fridholm
 */

#ifndef K_GRID_INTERN_H
#define K_GRID_INTERN_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef enum square_type_t
{
  SQUARE_LETTER,
  SQUARE_BLOCK,
  SQUARE_BORDER,
  SQUARE_EMPTY
} square_type_t;

typedef struct square_t
{
  square_type_t type;
  char          letter;
  bool          is_crossed;
} square_t;

typedef struct grid_t
{
  square_t* squares;
  int       square_count;
  int       width;
  int       height;
  int       cross_count;
  int       word_count;
} grid_t;

extern int  grid_vertical_word_insert(grid_t* grid, const char* word, int start_x, int start_y);

extern int  grid_horizontal_word_insert(grid_t* grid, const char* word, int start_x, int start_y);


extern void grid_horizontal_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y);

extern void grid_vertical_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y);


extern grid_t* grid_create(int width, int height);

extern grid_t* grid_copy(grid_t* copy, grid_t* grid);

extern grid_t* grid_dup(grid_t* grid);

#endif // K_GRID_INTERN_H
