/*
 * k-grid-gen.c - generate words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

extern bool running;

#include <pthread.h>

extern grid_t* curr_grid;
extern pthread_mutex_t lock;

#define GEN_DONE     0
#define GEN_NO_WORDS 1


#include <unistd.h>


int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y);

/*
 * Recursive function
 */
int grid_horizontal_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;


  grid_print(grid);

  usleep(1000000);


  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == SQUARE_BLOCK) return 1;


  pthread_mutex_lock(&lock);

  curr_grid = grid;

  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
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


  // If the length is 1, it should be marked as crossed
  if(max_length == 1)
  {
    square_index = (cross_y * grid->width) + cross_x;

    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return GEN_DONE;
  }


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

  pattern[max_length] = '\0';


  grid_t* new_grid = grid_dup(grid);


  char** words = NULL;
  size_t word_count = 0;

  grid_words_search(&words, &word_count, wbase, pattern, grid, start_x, cross_y, max_length, false);

  if(word_count == 0)
  {
    // Here: no words fit pattern
    pthread_mutex_lock(&lock);

    curr_grid = NULL;
    grid_free(&new_grid);

    pthread_mutex_unlock(&lock);
    
    return GEN_NO_WORDS;
  }

  for(size_t word_index = 0; word_index < word_count; word_index++)
  {
    char* word = words[word_index];

    // 1. Insert the word in the grid
    if(grid_horizontal_word_insert(new_grid, word, start_x, cross_y) == 1)
    {
      // If the word fits perfect, the crossword is solved?
      break;
    }

    // 1.1 Mark the word as used
    wbase_word_use(wbase, word);


    bool word_is_done = true;

    for(int index = 0; word[index] != '\0'; index++)
    {
      int next_x = start_x + index;

      // A square that is already crossed is done
      if(xy_square_is_crossed(new_grid, next_x, cross_y)) continue;

      // Recursivly call the vertical gen function
      if(grid_vertical_word_gen(wbase, best, new_grid, next_x, cross_y) == GEN_NO_WORDS)
      {
        // Here: The word couldn't solve the crossword
        word_is_done = false;

        break;
      }
    }

    if(word_is_done)
    {
      // Here: Every letter of word has been crossed
      break;
    }

    // 3. Remove the tested word from the grid
    grid_horizontal_word_reset(grid, new_grid, word, start_x, cross_y);

    // 3.1 Unmark the word as used, so it can be used somewhere else
    wbase_word_unuse(wbase, word);
  }

  words_free(&words, word_count);


  pthread_mutex_lock(&lock);

  curr_grid = NULL;
  grid_free(&new_grid);

  pthread_mutex_unlock(&lock);

  return GEN_DONE;
}

/*
 * Recursive function
 */
int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;


  grid_print(grid);

  usleep(1000000);


  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == SQUARE_BLOCK) return 1;


  pthread_mutex_lock(&lock);
  curr_grid = grid;
  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
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


  // If the length is 1, it should be marked as crossed
  if(max_length == 1)
  {
    square_index = (cross_y * grid->width) + cross_x;

    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return GEN_DONE;
  }


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

  pattern[max_length] = '\0';


  grid_t* new_grid = grid_dup(grid);


  char** words = NULL;
  size_t word_count = 0;

  grid_words_search(&words, &word_count, wbase, pattern, grid, cross_x, start_y, max_length, true);

  if(word_count == 0)
  {
    // Here: no words fit pattern
    pthread_mutex_lock(&lock);

    curr_grid = NULL;
    grid_free(&new_grid);

    pthread_mutex_unlock(&lock);
    
    return GEN_NO_WORDS;
  }

  for(size_t word_index = 0; word_index < word_count; word_index++)
  {
    char* word = words[word_index];

    // 1. Insert the word in the grid
    if(grid_vertical_word_insert(new_grid, word, cross_x, start_y) == 1)
    {
      // If the word fits perfect, the crossword is solved?
      break;
    }

    // 1.1 Mark the word as used
    wbase_word_use(wbase, word);


    bool word_is_done = true;

    for(int index = 0; word[index] != '\0'; index++)
    {
      int next_y = start_y + index;

      // A square that is already crossed is done
      if(xy_square_is_crossed(new_grid, cross_x, next_y)) continue;

      // Recursivly call the vertical gen function
      if(grid_horizontal_word_gen(wbase, best, new_grid, cross_x, next_y) == GEN_NO_WORDS)
      {
        // Here: The word couldn't solve the crossword
        word_is_done = false;

        break;
      }
    }

    if(word_is_done)
    {
      // Here: Every letter of word has been crossed
      break;
    }

    // 3. Remove the tested word from the grid
    grid_vertical_word_reset(grid, new_grid, word, cross_x, start_y);

    // 3.1 Unmark the word as used, so it can be used somewhere else
    wbase_word_unuse(wbase, word);
  }

  words_free(&words, word_count);


  pthread_mutex_lock(&lock);

  curr_grid = NULL;
  grid_free(&new_grid);

  pthread_mutex_unlock(&lock);

  return GEN_DONE;
}

/*
 *
 */
grid_t* grid_gen(wbase_t* wbase, int width, int height)
{
  grid_t* grid = grid_create(width, height);

  // Set neccessary block squares
  // Change this to a real prepare function
  grid->squares[0].type = SQUARE_BLOCK;

  grid->squares[width - 1].type = SQUARE_BLOCK;

  grid->squares[(height - 1) * width].type = SQUARE_BLOCK;



  grid_t* best = grid_dup(grid);

  grid_vertical_word_gen(wbase, best, grid, 1, 1);

  grid_free(&grid);

  return best;
}
