/*
 * k-grid-gen.c - generate words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

extern bool running;

#include <pthread.h>
#include <unistd.h>

extern grid_t* curr_grid;
extern pthread_mutex_t lock;

extern struct args args;

#define GEN_DONE 0
#define GEN_FAIL 1
#define GEN_STOP 2

/*
 * If either vert_word_gen or horiz_word_gen
 * return GEN_DONE:
 *
 * That means that the genration was successfull.
 *
 * The added words should then be preserved upstream.
 *
 * The new_grid should be saved to upsteam grid.
 */

/*
 * Check if the word fits horizontally (on 1st level)
 */
static bool horiz_word_fits(wbase_t* wbase, grid_t* grid, const char* word, int start_x, int y)
{
  for(int index = 0; word[index] != '\0'; index++)
  {
    int x = start_x + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(grid, x, y)) continue;

    if(!vert_word_exists(wbase, grid, x, y))
    {
      return false;
    }
  }

  return true;
}

static int vert_word_gen(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, int cross_x, int cross_y);

/*
 * Recursive function
 */
static int horiz_word_gen(wbase_t* wbase, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  // grid_print(grid);
  // usleep(1000000);


  square_t* square = xy_square_get(grid, cross_x, cross_y);

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

  int status = horiz_gwords_get(&gwords, &word_count, wbase, grid, cross_x, cross_y);

  // printf("word_count: %ld\n", word_count);

  // If the length is 1, it should be marked as crossed
  if(status == GWORDS_SINGLE)
  {
    xy_square_set_crossed(grid, cross_x, cross_y);

    grid->cross_count++;

    return GEN_DONE;
  }

  if(word_count == 0)
  {
    // Here: no words fit pattern
    return GEN_FAIL;
  }


  grid_t* new_grid = grid_dup(grid);

  bool has_failed = true;

  for(size_t word_index = 0; word_index < word_count; word_index++)
  {
    gword_t gword = gwords[word_index];

    char* word  = gword.word;
    int start_x = gword.start;

    // 1. Insert the word in the grid
    if(horiz_word_insert(wbase, new_grid, word, start_x, cross_y) == INSERT_PERFECT)
    {
      // If the word fits perfect, the crossword is solved?
      has_failed = false;
      break;
    }


    // If the word doesn't have a chance of fitting
    if(!horiz_word_fits(wbase, new_grid, word, start_x, cross_y))
    {
      // 3. Remove the tested word from the grid
      horiz_word_reset(wbase, grid, new_grid, word, start_x, cross_y);

      continue;
    }


    bool word_is_done = true;

    for(int index = 0; word[index] != '\0'; index++)
    {
      int next_x = start_x + index;

      // A square that is already crossed is done
      if(xy_square_is_crossed(new_grid, next_x, cross_y)) continue;

      // Recursivly call the vertical gen function
      int status = vert_word_gen(wbase, best, new_grid, next_x, cross_y);

      if(status == GEN_FAIL)
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
    horiz_word_reset(wbase, grid, new_grid, word, start_x, cross_y);
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
static bool vert_word_fits(wbase_t* wbase, grid_t* grid, const char* word, int x, int start_y)
{
  for(int index = 0; word[index] != '\0'; index++)
  {
    int y = start_y + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(grid, x, y)) continue;

    if(!horiz_word_exists(wbase, grid, x, y))
    {
      return false;
    }
  }

  return true;
}

/*
 * When embeding a werd, new words perpendicular to it is generated
 */
static int vert_word_embed(wbase_t* wbase, grid_t* best_grid, grid_t* new_grid, const char* word, int cross_x, int start_y)
{
  for(int index = 0; word[index] != '\0'; index++)
  {
    int next_y = start_y + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(new_grid, cross_x, next_y)) continue;

    // Recursivly call the vertical gen function
    int status = horiz_word_gen(wbase, best_grid, new_grid, cross_x, next_y);

    if(status == GEN_FAIL)
    {
      return GEN_FAIL;
    }

    if(status == GEN_STOP)
    {
      return GEN_STOP;
    }
  }

  return GEN_DONE;
}

/*
 *
 */
static int vert_word_test(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, grid_t* new_grid, const char* word, int x, int y)
{
  // 1. Insert the word in the grid
  if(vert_word_insert(wbase, new_grid, word, x, y) == INSERT_PERFECT)
  {
    // If the word fits perfect, the crossword is solved?
    return GEN_DONE;
  }


  // 2. If the word doesn't have a chance of fitting
  if(!vert_word_fits(wbase, new_grid, word, x, y))
  {
    // 3. Remove the tested word from the grid
    vert_word_reset(wbase, old_grid, new_grid, word, x, y);

    return GEN_FAIL;
  }


  // 3. 
  int embed_status = vert_word_embed(wbase, best_grid, new_grid, word, x, y);

  if(embed_status == GEN_STOP)
  {
    return GEN_STOP;
  }

  if(embed_status == GEN_DONE)
  {
    return GEN_DONE;
  }


  // 4. Remove the tested word from the grid
  vert_word_reset(wbase, old_grid, new_grid, word, x, y);

  return GEN_FAIL;
}

/*
 *
 */
static int vert_words_test(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, grid_t* new_grid, gword_t* gwords, size_t word_count, int x)
{
  for(size_t index = 0; index < word_count; index++)
  {
    gword_t gword = gwords[index];

    char* word  = gword.word;
    int start_y = gword.start;

    int test_status = vert_word_test(wbase, best_grid, old_grid, new_grid, word, x, start_y);

    if(test_status == GEN_DONE)
    {
      return GEN_DONE;
    }

    if(test_status == GEN_STOP)
    {
      return GEN_STOP;
    }
  }

  return GEN_FAIL;
}

/*
 * Recursive function
 */
static int vert_word_gen(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  // grid_print(grid);
  // usleep(1000000);


  square_t* square = xy_square_get(old_grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  if(old_grid->cross_count > best_grid->cross_count)
  {
    // printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best_grid, old_grid);
  }

  // 1. Generate all possible words
  gword_t* gwords = NULL;
  size_t word_count = 0;

  int gwords_status = vert_gwords_get(&gwords, &word_count, wbase, old_grid, cross_x, cross_y);

  // If the length is 1, it should be marked as crossed
  if(gwords_status == GWORDS_SINGLE)
  {
    xy_square_set_crossed(old_grid, cross_x, cross_y);

    old_grid->cross_count++;

    return GEN_DONE;
  }

  if(gwords_status == GWORDS_FAIL || word_count == 0)
  {
    // Here: no words fit pattern
    return GEN_FAIL;
  }


  // 2. Test all words in the new grid
  grid_t* new_grid = grid_dup(old_grid);


  // Make current grid point to the new grid
  pthread_mutex_lock(&lock);

  curr_grid = new_grid;

  pthread_mutex_unlock(&lock);


  int test_status = vert_words_test(wbase, best_grid, old_grid, new_grid, gwords, word_count, cross_x);

  gwords_free(&gwords, word_count);


  // Make current grid point to the old grid again
  pthread_mutex_lock(&lock);

  curr_grid = old_grid;

  pthread_mutex_unlock(&lock);


  if(test_status == GEN_DONE)
  {
    grid_copy(old_grid, new_grid);
  }

  grid_free(&new_grid);

  return test_status;
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

  grid_t* best_grid = grid_dup(grid);

  for(int index = 0; index < grid->square_count; index++)
  {
    int x = (index % grid->width);
    int y = (index / grid->width);

    if(xy_square_is_done(grid, x, y)) continue;
      
    int gen_status = vert_word_gen(wbase, best_grid, grid, x, y);

    if(gen_status == GEN_STOP) break;

    if(gen_status == GEN_FAIL) break;
  }

  printf("grid:\n");
  grid_print(grid);

  printf("best:\n");
  grid_print(best_grid);


  grid_free(&best_grid);

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

    square_t* square = xy_square_get(grid, x, y);

    if(square->type == SQUARE_EMPTY) return false;
  }

  return true;
}
