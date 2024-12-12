/*
 * k-grid-gen.c - generate words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

extern bool running;

/*
 * Recursive function
 */
int grid_horizontal_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;

  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == SQUARE_BLOCK) return 1;


  if(grid->cross_count > best->cross_count)
  {
    // printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  // Instead of marking this as crossed, don't call _gen with this y
  if(cross_y == 0)
  {
    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return 0;
  }


  int start_x;

  for(start_x = (cross_x + 1); start_x-- > 0;)
  {
    square_index = (cross_y * grid->width) + start_x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_BLOCK) break;
  }

  start_x++;

  int stop_x;

  for(stop_x = cross_x; stop_x < grid->width; stop_x++)
  {
    square_index = (cross_y * grid->width) + stop_x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_BLOCK) break;
  }

  stop_x--;

  int max_length = (1 + stop_x - start_x);


  char pattern[max_length + 1];

  for(int index = max_length; index-- > 0;)
  {
    int x = (start_x + index);

    square_index = (cross_y * grid->width) + x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_LETTER)
    {
      pattern[index] = square.letter;
    }
    else pattern[index] = '_';
  }


  grid_t* new_grid = grid_dup(grid);

  char** words;
  size_t count;

  for(int length = (max_length + 1); length-- > 2;)
  {
    pattern[length] = '\0';

    int block_x = (start_x + length);

    // Here: Reject bad x, y
    if(cross_y >= (grid->height - 2))
    {
      if(block_x == (grid->width - 2) || block_x == (grid->width - 1)) continue;
    }

    // Do not block corner
    if(cross_y == 1)
    {
      if(block_x == (grid->width - 2) || block_x == (grid->width - 1)) continue;
    }
    if(cross_y == 2)
    {
      if(block_x == (grid->width - 1)) continue;
    }

    // Do not block letter
    if(block_x < grid->width)
    {
      square_index = (cross_y * grid->width) + block_x;

      if(grid->squares[square_index].type == SQUARE_LETTER) continue;
    }


    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      words_shuffle(words, count);

      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        word_use(trie, word);

        // If the word fits perfect, the crossword is solved?
        if(grid_horizontal_word_insert(new_grid, word, start_x, cross_y) == 1)
        {
          words_free(&words, count);

          grid_free(&new_grid);

          return 0; // Change out these returns
        }

        grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_x = start_x + i;

          square_index = (cross_y * new_grid->width) + next_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          // usleep(1000);

          if(grid_vertical_word_gen(trie, best, new_grid, next_x, cross_y) != 0)
          {
            words_free(&words, count);

            grid_free(&new_grid);

            return 2;
          }
        }

        grid_horizontal_word_reset(grid, new_grid, word, start_x, cross_y);

        word_unuse(trie, word);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 0;
}

/*
 * Recursive function
 */
int grid_vertical_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;

  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == SQUARE_BLOCK) return 1;



  if(grid->cross_count > best->cross_count)
  {
    grid_copy(best, grid);
  }


  // Instead of marking this as crossed, don't call _gen with this x
  if(cross_x == 0)
  {
    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return 0;
  }


  int start_y;

  for(start_y = (cross_y + 1); start_y-- > 0;)
  {
    square_index = (start_y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_BLOCK) break;
  }

  start_y++;

  int stop_y;

  for(stop_y = cross_y; stop_y < grid->height; stop_y++)
  {
    square_index = (stop_y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_BLOCK) break;
  }

  stop_y--;

  int max_length = (1 + stop_y - start_y);


  char pattern[max_length + 1];

  for(int index = max_length; index-- > 0;)
  {
    int y = (start_y + index);

    square_index = (y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == SQUARE_LETTER)
    {
      pattern[index] = square.letter;
    }
    else pattern[index] = '_';
  }


  grid_t* new_grid = grid_dup(grid);

  char** words;
  size_t count;

  for(int length = (max_length + 1); length-- > 2;)
  {
    pattern[length] = '\0';

    int block_y = (start_y + length);

    // Here: Reject bad x, y
    if(cross_x >= (grid->width - 2))
    {
      if(block_y == (grid->height - 2) || block_y == (grid->height - 1)) continue;
    }

    // Do not block corner
    if(cross_x == 1)
    {
      if(block_y == (grid->height - 2) || block_y == (grid->height - 1)) continue;
    }
    if(cross_x == 2)
    {
      if(block_y == (grid->height - 1)) continue;
    }

    // Do not block letter
    if(block_y < grid->height)
    {
      square_index = (block_y * grid->width) + cross_x;

      if(grid->squares[square_index].type == SQUARE_LETTER) continue;
    }


    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      words_shuffle(words, count);

      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        word_use(trie, word);

        // If the word fits perfect, the crossword is solved?
        if(grid_vertical_word_insert(new_grid, word, cross_x, start_y) == 1)
        {
          words_free(&words, count);

          grid_free(&new_grid);

          return 0; // Change out these returns
        }

        // grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_y = start_y + i;

          square_index = (next_y * new_grid->width) + cross_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          // usleep(1000);

          if(grid_horizontal_word_gen(trie, best, new_grid, cross_x, next_y) != 0)
          {
            words_free(&words, count);

            grid_free(&new_grid);

            return 2;
          }
        }

        grid_vertical_word_reset(grid, new_grid, word, cross_x, start_y);

        word_unuse(trie, word);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 0;
}

/*
 *
 */
grid_t* grid_gen(trie_t* trie, int width, int height)
{
  grid_t* grid = grid_create(width, height);

  // Set neccessary block squares
  // Change this to a real prepare function
  grid->squares[0].type = SQUARE_BLOCK;

  grid->squares[width - 1].type = SQUARE_BLOCK;

  grid->squares[(height - 1) * width].type = SQUARE_BLOCK;



  grid_t* best = grid_dup(grid);

  grid_vertical_word_gen(trie, best, grid, 1, 1);

  grid_free(&grid);

  return best;
}
