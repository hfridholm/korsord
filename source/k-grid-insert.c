/*
 * k-grid-insert.c - insert and reset words in grid
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 * Insert vertical word
 *
 * When inserting, the function:
 * - pastes the letters from the word
 * - pastes blocks before and after word
 */
int vert_word_insert(wbase_t* wbase, grid_t* grid, const char* word, int x, int start_y)
{
  bool is_perfect = true;

  int index, y;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    // 1. Get the old square
    square_t* old_square = xy_square_get(grid, x, y);

    if(!old_square) break;

    // 2. Create the new square
    square_t new_square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index],
      .is_crossed = false
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
  if(start_y + index < grid->height)
  {
    square_t* square = xy_square_get(grid, x, start_y + index);

    if(square && square->type != SQUARE_BORDER) 
    {
      square->type = SQUARE_BLOCK;
    }
  }

  // Insert block square at beginning of word
  if(start_y > 0)
  {
    square_t* square = xy_square_get(grid, x, start_y - 1);

    if(square && square->type != SQUARE_BORDER) 
    {
      square->type = SQUARE_BLOCK;
    }
  }

  trie_word_insert(grid->words, word);

  return is_perfect ? INSERT_PERFECT : INSERT_DONE;
}

/*
 * Insert horizontal word
 *
 * When inserting, the function:
 * - pastes the letters from the word
 * - pastes blocks before and after word
 */
int horiz_word_insert(wbase_t* wbase, grid_t* grid, const char* word, int start_x, int y)
{
  bool is_perfect = true;

  int index, x;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    // 1. Get the old square
    square_t* old_square = xy_square_get(grid, x, y);
    
    if(!old_square) break;

    // 2. Create the new square
    square_t new_square =
    {
      .type = SQUARE_LETTER,
      .letter = word[index],
      .is_crossed = false
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
  if(start_x + index < grid->width)
  {
    square_t* square = xy_square_get(grid, start_x + index, y);

    if(square && square->type != SQUARE_BORDER) 
    {
      square->type = SQUARE_BLOCK;
    }
  }

  // Insert block square at beginning of word
  if(start_x > 0)
  {
    square_t* square = xy_square_get(grid, start_x - 1, y);

    if(square && square->type != SQUARE_BORDER) 
    {
      square->type = SQUARE_BLOCK;
    }
  }

  trie_word_insert(grid->words, word);

  return is_perfect ? INSERT_PERFECT : INSERT_DONE;
}

/*
 * Remove non crossed letters of horizontal word
 */
void horiz_word_remove(grid_t* grid, const char* word, int start_x, int y)
{
  for (int index = 0; word[index] != '\0'; index++)
  {
    int x = start_x + index;

    square_t* square = xy_square_get(grid, x, y);

    if (square && !square->is_crossed)
    {
      *square = (square_t)
      {
        .type       = SQUARE_EMPTY,
        .letter     = 0,
        .is_crossed = false,
      };
    }
  }

  trie_word_remove(grid->words, word);
}

/*
 * Remove non crossed letters of vertical word
 */
void vert_word_remove(grid_t* grid, const char* word, int x, int start_y)
{
  for (int index = 0; word[index] != '\0'; index++)
  {
    int y = start_y + index;

    square_t* square = xy_square_get(grid, x, y);

    if (square && !square->is_crossed)
    {
      *square = (square_t)
      {
        .type       = SQUARE_EMPTY,
        .letter     = 0,
        .is_crossed = false,
      };
    }
  }

  trie_word_remove(grid->words, word);
}
