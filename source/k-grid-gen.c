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
#define GEN_FAIL     2
#define GEN_STOP     3

/*
 * If either grid_vertical_word_gen or
 *           grid_horizontal_word_gen
 * return GEN_DONE:
 *
 * That means that the genration was successfull.
 *
 * The added words should then be preserved upstream.
 *
 * The new_grid should be saved to upsteam grid.
 */

#include <unistd.h>


int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y);

/*
 *
 */
static void start_and_stop_x_get(int* start_x, int* stop_x, grid_t* grid, int cross_x, int cross_y)
{
  for(*start_x = (cross_x + 1); (*start_x)-- > 0;)
  {
    square_t* square = grid_xy_square_get(grid, *start_x, cross_y);

    if(!square || square->type == SQUARE_BLOCK ||
                  square->type == SQUARE_BORDER) break;
  }

  (*start_x)++;

  for(*stop_x = cross_x; *stop_x < grid->width; (*stop_x)++)
  {
    square_t* square = grid_xy_square_get(grid, *stop_x, cross_y);

    if(!square || square->type == SQUARE_BLOCK ||
                  square->type == SQUARE_BORDER) break;
  }

  (*stop_x)--;
}

/*
 *
 */
int grid_horizontal_pattern_get(char* pattern, grid_t* grid, int max_length, int start_x, int y)
{
  for(int index = max_length; index-- > 0;)
  {
    int x = (start_x + index);

    square_t* square = grid_xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[index] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[max_length] = '\0';

  return 0;
}

/*
 * Recursive function
 */
int grid_horizontal_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  grid_print(grid); usleep(100000);


  square_t* square = grid_xy_square_get(grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  pthread_mutex_lock(&lock);

  curr_grid = grid;

  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  int start_x, stop_x;

  start_and_stop_x_get(&start_x, &stop_x, grid, cross_x, cross_y);


  int max_length = (1 + stop_x - start_x);


  // If the length is 1, it should be marked as crossed
  if(max_length == 1)
  {
    grid_xy_square_set_crossed(grid, cross_x, cross_y);

    grid->cross_count++;

    return GEN_DONE;
  }


  char pattern[max_length + 1];

  if(grid_horizontal_pattern_get(pattern, grid, max_length, start_x, cross_y) != 0)
  {
    return GEN_FAIL;
  }


  grid_t* new_grid = grid_dup(grid);

  bool has_failed = true;


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
    if(grid_horizontal_word_insert(new_grid, word, start_x, cross_y) == INSERT_PERFECT)
    {
      // If the word fits perfect, the crossword is solved?
      has_failed = false;
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
      int status = grid_vertical_word_gen(wbase, best, new_grid, next_x, cross_y);

      if(status == GEN_NO_WORDS || status == GEN_FAIL)
      {
        // Here: The word couldn't solve the crossword
        word_is_done = false;
        break;
      }

      if(status == GEN_STOP)
      {
        words_free(&words, word_count);


        pthread_mutex_lock(&lock);

        curr_grid = NULL;
        grid_free(&new_grid);

        pthread_mutex_unlock(&lock);

        return GEN_STOP;
      }
    }

    if(word_is_done)
    {
      // Here: Every letter of word has been crossed
      has_failed = false;
      break;
    }

    // 3. Remove the tested word from the grid
    grid_horizontal_word_reset(grid, new_grid, word, start_x, cross_y);

    // 3.1 Unmark the word as used, so it can be used somewhere else
    wbase_word_unuse(wbase, word);
  }

  words_free(&words, word_count);

  if(has_failed)
  {
    pthread_mutex_lock(&lock);

    curr_grid = NULL;

    grid_free(&new_grid);

    pthread_mutex_unlock(&lock);

    return GEN_FAIL;
  }


  pthread_mutex_lock(&lock);

  grid_copy(grid, new_grid);

  curr_grid = grid;

  grid_free(&new_grid);

  pthread_mutex_unlock(&lock);

  return GEN_DONE;
}

/*
 *
 */
static void start_and_stop_y_get(int* start_y, int* stop_y, grid_t* grid, int cross_x, int cross_y)
{
  for(*start_y = (cross_y + 1); (*start_y)-- > 0;)
  {
    square_t* square = grid_xy_square_get(grid, cross_x, *start_y);

    if(!square || square->type == SQUARE_BLOCK ||
                  square->type == SQUARE_BORDER) break;
  }

  (*start_y)++;

  for(*stop_y = cross_y; *stop_y < grid->height; (*stop_y)++)
  {
    square_t* square = grid_xy_square_get(grid, cross_x, *stop_y);

    if(!square || square->type == SQUARE_BLOCK ||
                  square->type == SQUARE_BORDER) break;
  }

  (*stop_y)--;
}

/*
 *
 */
int grid_vertical_pattern_get(char* pattern, grid_t* grid, int max_length, int x, int start_y)
{
  for(int index = max_length; index-- > 0;)
  {
    int y = (start_y + index);

    square_t* square = grid_xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[index] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[max_length] = '\0';

  return 0;
}

/*
 * Recursive function
 */
int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  grid_print(grid); usleep(100000);


  square_t* square = grid_xy_square_get(grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  pthread_mutex_lock(&lock);

  curr_grid = grid;

  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  int start_y, stop_y;

  start_and_stop_y_get(&start_y, &stop_y, grid, cross_x, cross_y);


  int max_length = (1 + stop_y - start_y);


  // If the length is 1, it should be marked as crossed
  if(max_length == 1)
  {
    grid_xy_square_set_crossed(grid, cross_x, cross_y);

    grid->cross_count++;

    return GEN_DONE;
  }


  char pattern[max_length + 1];

  if(grid_vertical_pattern_get(pattern, grid, max_length, cross_x, start_y) != 0)
  {
    return GEN_FAIL;
  }


  grid_t* new_grid = grid_dup(grid);

  // Maybe: Change this to a default false statement
  // so, it has to be sat to true to be triggered
  // (rename has_failed to something like: has_not_failed)
  bool has_failed = true;


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
    if(grid_vertical_word_insert(new_grid, word, cross_x, start_y) == INSERT_PERFECT)
    {
      // If the word fits perfect, the crossword is solved?
      has_failed = false;
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
      int status = grid_horizontal_word_gen(wbase, best, new_grid, cross_x, next_y);

      if(status == GEN_NO_WORDS || status == GEN_FAIL)
      {
        // Here: The word couldn't solve the crossword
        word_is_done = false;

        break;
      }

      if(status == GEN_STOP)
      {
        words_free(&words, word_count);


        pthread_mutex_lock(&lock);

        curr_grid = NULL;
        grid_free(&new_grid);

        pthread_mutex_unlock(&lock);

        return GEN_STOP;
      }
    }

    if(word_is_done)
    {
      // Here: Every letter of word has been crossed
      has_failed = false;
      break;
    }

    // 3. Remove the tested word from the grid
    grid_vertical_word_reset(grid, new_grid, word, cross_x, start_y);

    // 3.1 Unmark the word as used, so it can be used somewhere else
    wbase_word_unuse(wbase, word);
  }

  words_free(&words, word_count);

  if(has_failed)
  {
    pthread_mutex_lock(&lock);

    curr_grid = NULL;

    grid_free(&new_grid);

    pthread_mutex_unlock(&lock);

    return GEN_FAIL;
  }


  pthread_mutex_lock(&lock);

  grid_copy(grid, new_grid);

  curr_grid = grid;

  grid_free(&new_grid);

  pthread_mutex_unlock(&lock);

  return GEN_DONE;
}

/*
 *
 */
grid_t* grid_gen(wbase_t* wbase, const char* filepath)
{
  grid_t* grid = grid_model_load(filepath);

  if(!grid)
  {
    return NULL;
  }

  grid_prep(grid);

  grid_t* best = grid_dup(grid);

  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      square_t* square = grid_xy_square_get(grid, x, y);

      if(square->type == SQUARE_EMPTY)
      //(square->type == SQUARE_LETTER && !square->is_crossed))
      {
        grid_vertical_word_gen(wbase, best, grid, x, y);
      }
    }
  }

  grid_free(&grid);

  return best;
}
