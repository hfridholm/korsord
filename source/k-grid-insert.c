/*
 * k-grid-insert.c - insert and reset words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 *
 */
int grid_vertical_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, y, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    square_index = (y * grid->width) + start_x;

    if(square_index >= grid->square_count) break;


    square_t old_square = grid->squares[square_index];

    square_t square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index]
    };

    if(old_square.type == SQUARE_LETTER)
    {
      grid->cross_count++;
      square.is_crossed = true;
    }
    else is_perfect = false;

    grid->squares[square_index] = square;
  }


  if(is_perfect) return 1;

  
  // Insert block square at end of word
  y = start_y + index;

  square_index = (y * grid->width) + start_x;

  if(y < grid->height && square_index < grid->square_count)
  {
    grid->squares[square_index].type = SQUARE_BLOCK;
  }

  // Insert neibouring block squares at start of word
  if(start_y == 0)
  {
    if((start_x + 1) < grid->width)
    {
      square_index = (start_y * grid->width) + (start_x + 1);

      grid->squares[square_index].type = SQUARE_BLOCK;
    }

    square_index = (start_y * grid->width) + (start_x - 1);

    grid->squares[square_index].type = SQUARE_BLOCK;
  }

  return 0;
}

/*
 *
 */
int grid_horizontal_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, x, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    square_index = (start_y * grid->width) + x;

    if(square_index >= grid->square_count) break;


    square_t old_square = grid->squares[square_index];

    square_t square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index]
    };

    if(old_square.type == SQUARE_LETTER)
    {
      grid->cross_count++;
      square.is_crossed = true;
    }
    else is_perfect = false;

    grid->squares[square_index] = square;
  }


  if(is_perfect) return 1;

  
  // Insert block square at end of word
  x = start_x + index;

  square_index = (start_y * grid->width) + x;

  if(x < grid->width && square_index < grid->square_count)
  {
    grid->squares[square_index].type = SQUARE_BLOCK;
  }

  // Insert neibouring block squares at start of word
  if(start_x == 0)
  {
    if((start_y + 1) < grid->height)
    {
      square_index = ((start_y + 1) * grid->width) + start_x;

      grid->squares[square_index].type = SQUARE_BLOCK;
    }

    square_index = ((start_y - 1) * grid->width) + start_x;

    grid->squares[square_index].type = SQUARE_BLOCK;
  }

  return 0;
}

/*
 *
 */
void grid_horizontal_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, x, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    square_index = (start_y * grid->width) + x;

    if(square_index >= grid->square_count) break;


    if(grid->squares[square_index].is_crossed)
    {
      grid->cross_count--;
    }

    grid->squares[square_index] = original->squares[square_index];
  }

  // Reset the block at the end of the word
  x = start_x + index;

  square_index = (start_y * grid->width) + x;

  if(x < grid->width && square_index < grid->square_count)
  {
    grid->squares[square_index] = original->squares[square_index];
  }
}

/*
 *
 */
void grid_vertical_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, y, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    square_index = (y * grid->width) + start_x;

    if(square_index >= grid->square_count) break;


    if(grid->squares[square_index].is_crossed)
    {
      grid->cross_count--;
    }

    grid->squares[square_index] = original->squares[square_index];
  }

  // Reset the block at the end of the word
  y = start_y + index;

  square_index = (y * grid->width) + start_x;

  if(y < grid->height && square_index < grid->square_count)
  {
    grid->squares[square_index] = original->squares[square_index];
  }
}
