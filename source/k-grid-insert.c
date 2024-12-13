/*
 * k-grid-insert.c - insert and reset words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * When inserting, the function:
 * - pastes the letters from the word
 * - pastes a block square at the end
 */
int grid_vertical_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, y;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    // 1. Get the old square
    square_t* old_square = grid_xy_square_get(grid, start_x, y);

    if(!old_square) break;

    // 2. Create the new square
    square_t new_square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index]
    };

    if(old_square->type == SQUARE_LETTER)
    {
      grid->cross_count++;

      new_square.is_crossed = true;
    }
    else is_perfect = false;

    // 3. Assign the new square
    *old_square = new_square;
  }
  
  // Insert block square at end of word
  y = start_y + index;

  square_t* square = grid_xy_square_get(grid, start_x, y);

  if(square) square->type = SQUARE_BLOCK;


  return is_perfect ? INSERT_PERFECT : INSERT_DONE;
}

/*
 *
 */
int grid_horizontal_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, x;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    // 1. Get the old square
    square_t* old_square = grid_xy_square_get(grid, x, start_y);
    
    if(!old_square) break;

    // 2. Create the new square
    square_t new_square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index]
    };

    if(old_square->type == SQUARE_LETTER)
    {
      grid->cross_count++;
    
      new_square.is_crossed = true;
    }
    else is_perfect = false;

    // 3. Assign the new square
    *old_square = new_square;
  }

  // Insert block square at end of word
  x = start_x + index;

  square_t* square = grid_xy_square_get(grid, x, start_y);

  if(square) square->type = SQUARE_BLOCK;


  return is_perfect ? INSERT_PERFECT : INSERT_DONE;
}

/*
 *
 */
void grid_horizontal_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, x;
  square_t *original_square, *square;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    original_square = grid_xy_square_get(original, x, start_y);

    square = grid_xy_square_get(grid, x, start_y);

    if(!original_square || !square) break;


    if(square->is_crossed) grid->cross_count--;

    *square = *original_square;
  }

  // Reset the block at the end of the word
  x = start_x + index;

  square = grid_xy_square_get(grid, x, start_y);

  original_square = grid_xy_square_get(original, x, start_y);


  if(square && original_square) *square = *original_square;
}

/*
 *
 */
void grid_vertical_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, y;
  square_t *original_square, *square;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    original_square = grid_xy_square_get(original, start_x, y);

    square = grid_xy_square_get(grid, start_x, y);

    if(!original_square || !square) break;


    if(square->is_crossed) grid->cross_count--;

    *square = *original_square;
  }

  // Reset the block at the end of the word
  y = start_y + index;

  square = grid_xy_square_get(grid, start_x, y);

  original_square = grid_xy_square_get(original, start_x, y);


  if(square && original_square) *square = *original_square;
}
