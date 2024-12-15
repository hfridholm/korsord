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
#include <string.h>
#include <unistd.h>

// For debugging purpouses
#include <signal.h>

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define INSERT_DONE    1
#define INSERT_PERFECT 2

#define GWORDS_DONE     0
#define GWORDS_NO_WORDS 1
#define GWORDS_FAIL     2
#define GWORDS_SINGLE   3
 
/*
 * gword_t - grid word
 *
 * This struct represents a word 
 * that starts and stops somewhere
 */
typedef struct gword_t
{
  char* word;
  int   start; // Either x or y
  int   stop;  // Same   x or y
} gword_t;

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


extern bool horiz_word_exists(wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern bool vert_word_exists(wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);


extern int horiz_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern int vert_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern void gwords_free(gword_t** gwords, size_t count);



extern void xy_square_set_crossed(grid_t* grid, int x, int y);

extern void xy_real_square_set_block(grid_t* grid, int x, int y);


extern square_t* xy_real_square_get(grid_t* grid, int x, int y);

extern int       xy_real_index_get(grid_t* grid, int x, int y);


extern square_t* xy_square_get(grid_t* grid, int x, int y);

extern int       xy_index_get(grid_t* grid, int x, int y);


extern bool block_is_allowed(grid_t* grid, int block_x, int block_y);


extern bool xy_square_is_blocking(grid_t* grid, int x, int y);

extern bool xy_square_is_letter(grid_t* grid, int x, int y);

extern bool xy_square_is_done(grid_t* grid, int x, int y);

extern bool xy_square_is_block(grid_t* grid, int x, int y);

extern bool xy_real_square_is_block(grid_t* grid, int x, int y);

extern bool xy_square_is_crossed(grid_t* grid, int x, int y);

extern bool xy_real_square_is_border(grid_t* grid, int x, int y);

extern bool xy_square_is_border(grid_t* grid, int x, int y);


extern int  vert_word_insert(wbase_t* wbase, grid_t* grid, const char* word, int x, int start_y);

extern void vert_word_reset(wbase_t* wbase, grid_t* original, grid_t* grid, const char* word, int x, int start_y);


extern int  horiz_word_insert(wbase_t* wbase, grid_t* grid, const char* word, int start_x, int y);

extern void horiz_word_reset(wbase_t* wbase, grid_t* original, grid_t* grid, const char* word, int start_x, int y);


extern grid_t* grid_create(int width, int height);

extern grid_t* grid_copy(grid_t* copy, grid_t* grid);

extern grid_t* grid_dup(grid_t* grid);

extern grid_t* grid_model_load(const char* filepath);

extern int     grid_prep(grid_t* grid);

#endif // K_GRID_INTERN_H
