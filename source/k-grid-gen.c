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
#include <unistd.h>

extern grid_t* curr_grid;
extern pthread_mutex_t lock;

extern struct args args;

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

int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y);

/*
 * Recursive function
 */
int grid_horizontal_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  // grid_print(grid);
  // usleep(1000000);


  square_t* square = grid_xy_square_get(grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  pthread_mutex_lock(&lock);

  curr_grid = grid;

  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    // printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  gword_t* gwords = NULL;
  size_t word_count = 0;

  int status = horizontal_gwords_get(&gwords, &word_count, wbase, grid, cross_x, cross_y);

  // printf("word_count: %ld\n", word_count);

  // If the length is 1, it should be marked as crossed
  if(status == GWORDS_SINGLE)
  {
    grid_xy_square_set_crossed(grid, cross_x, cross_y);

    grid->cross_count++;

    return GEN_DONE;
  }

  if(word_count == 0)
  {
    // Here: no words fit pattern
    return GEN_NO_WORDS;
  }


  grid_t* new_grid = grid_dup(grid);

  bool has_failed = true;

  for(size_t word_index = 0; word_index < word_count; word_index++)
  {
    gword_t gword = gwords[word_index];

    char* word  = gword.word;
    int start_x = gword.start;

    // 1. Insert the word in the grid
    if(grid_horizontal_word_insert(new_grid, word, start_x, cross_y) == INSERT_PERFECT)
    {
      // If the word fits perfect, the crossword is solved?
      has_failed = false;
      break;
    }

    // 1.1 Mark the word as used
    wbase_word_use(wbase, word);

    
    // Before traversing all letters, check if they
    // över huvud taget is valid
    bool word_fits = true;

    for(int index = 0; word[index] != '\0'; index++)
    {
      int next_x = start_x + index;

      // A square that is already crossed is done
      if(xy_square_is_crossed(new_grid, next_x, cross_y)) continue;

      if(!vertical_word_exists(wbase, grid, next_x, cross_y))
      {
        word_fits = false;
        break;
      }
    }

    // If the word doesn't have a chance of fitting
    if(!word_fits)
    {
      // 3. Remove the tested word from the grid
      grid_horizontal_word_reset(grid, new_grid, word, start_x, cross_y);

      // 3.1 Unmark the word as used, so it can be used somewhere else
      wbase_word_unuse(wbase, word);

      continue;
    }


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
        gwords_free(&gwords, word_count);


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

  gwords_free(&gwords, word_count);

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
 * Recursive function
 */
int grid_vertical_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  // grid_print(grid);
  // usleep(1000000);


  square_t* square = grid_xy_square_get(grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  pthread_mutex_lock(&lock);

  curr_grid = grid;

  pthread_mutex_unlock(&lock);


  if(grid->cross_count > best->cross_count)
  {
    // printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  gword_t* gwords = NULL;
  size_t word_count = 0;

  int status = vertical_gwords_get(&gwords, &word_count, wbase, grid, cross_x, cross_y);

  // printf("word_count: %ld\n", word_count);

  // If the length is 1, it should be marked as crossed
  if(status == GWORDS_SINGLE)
  {
    grid_xy_square_set_crossed(grid, cross_x, cross_y);

    grid->cross_count++;

    return GEN_DONE;
  }

  if(word_count == 0)
  {
    // Here: no words fit pattern
    return GEN_NO_WORDS;
  }


  grid_t* new_grid = grid_dup(grid);

  // Maybe: Change this to a default false statement
  // so, it has to be sat to true to be triggered
  // (rename has_failed to something like: has_not_failed)
  bool has_failed = true;

  for(size_t word_index = 0; word_index < word_count; word_index++)
  {
    gword_t gword = gwords[word_index];

    char* word  = gword.word;
    int start_y = gword.start;

    // 1. Insert the word in the grid
    if(grid_vertical_word_insert(new_grid, word, cross_x, start_y) == INSERT_PERFECT)
    {
      // If the word fits perfect, the crossword is solved?
      has_failed = false;
      break;
    }

    // 1.1 Mark the word as used
    wbase_word_use(wbase, word);


    // Before traversing all letters, check if they
    // över huvud taget is valid
    bool word_fits = true;

    for(int index = 0; word[index] != '\0'; index++)
    {
      int next_y = start_y + index;

      // A square that is already crossed is done
      if(xy_square_is_crossed(new_grid, next_y, cross_y)) continue;

      if(!horizontal_word_exists(wbase, grid, next_y, cross_y))
      {
        word_fits = false;
        break;
      }
    }

    // If the word doesn't have a chance of fitting
    if(!word_fits)
    {
      // 3. Remove the tested word from the grid
      grid_vertical_word_reset(grid, new_grid, word, cross_x, start_y);

      // 3.1 Unmark the word as used, so it can be used somewhere else
      wbase_word_unuse(wbase, word);

      continue;
    }


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
        gwords_free(&gwords, word_count);


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

  gwords_free(&gwords, word_count);

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

  printf("grid:\n");
  grid_print(grid);

  printf("best:\n");
  grid_print(best);


  grid_free(&best);

  return grid;
}

/*
 * Check if grid is done and complete
 *
 * RETURN (bool is_done)
 * - false | Grid is not done or allocated
 */
bool grid_is_done(grid_t* grid)
{
  if(!grid) return false;

  for(int index = 0; index < grid->square_count; index++)
  {
    int x = (index % grid->width);
    int y = (index / grid->width);

    square_t* square = grid_xy_square_get(grid, x, y);

    if(square->type == SQUARE_EMPTY) return false;
  }

  return true;
}
