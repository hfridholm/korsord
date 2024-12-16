/*
 * k-grid-gen.c - generate words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include <unistd.h>

extern bool running;
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
 * When embeding a word, new words perpendicular to it is generated
 *
 * The newly generated words are stored in a copy of the grid,
 * this way, if not all words succeed, the old grid is perserved
 *
 * If the words are generated in the old_grid,
 * the grid will probaly not be solved if one letter fails
 */
static int horiz_word_embed(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, const char* word, int start_x, int y)
{
  // 1. Duplicating the old grid, to work on seperate temp grid
  grid_t* new_grid = grid_dup(old_grid);


  for(int index = 0; word[index] != '\0'; index++)
  {
    int x = start_x + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(new_grid, x, y)) continue;

    // Recursivly call the vertical gen function
    int gen_status = vert_word_gen(wbase, best_grid, new_grid, x, y);

    if(gen_status == GEN_FAIL)
    {
      grid_free(&new_grid);

      return GEN_FAIL;
    }

    if(gen_status == GEN_STOP)
    {
      grid_free(&new_grid);

      return GEN_STOP;
    }
  }

  grid_copy(old_grid, new_grid);

  grid_free(&new_grid);

  return GEN_DONE;
}

/*
 *
 */
static int horiz_word_test(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, grid_t* new_grid, const char* word, int x, int y)
{
  // 1. Insert the word in the grid
  if(horiz_word_insert(wbase, new_grid, word, x, y) == INSERT_PERFECT)
  {
    // If the word fits perfect, the crossword is solved?
    return GEN_DONE;
  }


  // 2. If the word doesn't have a chance of fitting
  if(!horiz_word_fits(wbase, new_grid, word, x, y))
  {
    // 3. Remove the tested word from the grid
    horiz_word_reset(wbase, old_grid, new_grid, word, x, y);

    return GEN_FAIL;
  }


  // 3. 
  int embed_status = horiz_word_embed(wbase, best_grid, new_grid, word, x, y);

  if(embed_status == GEN_STOP)
  {
    return GEN_STOP;
  }

  if(embed_status == GEN_DONE)
  {
    return GEN_DONE;
  }


  // 4. Remove the tested word from the grid
  horiz_word_reset(wbase, old_grid, new_grid, word, x, y);

  return GEN_FAIL;
}

/*
 *
 */
static int horiz_words_test(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, grid_t* new_grid, gword_t* gwords, size_t word_count, int y)
{
  for(size_t index = 0; index < word_count; index++)
  {
    if(!running) return GEN_STOP;

    gword_t gword = gwords[index];

    char* word  = gword.word;
    int start_x = gword.start;

    int test_status = horiz_word_test(wbase, best_grid, old_grid, new_grid, word, start_x, y);

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
static int horiz_word_gen(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, int cross_x, int cross_y)
{
  if(!running) return GEN_STOP;


  curr_grid_set(old_grid);
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

  int gwords_status = horiz_gwords_get(&gwords, &word_count, wbase, old_grid, cross_x, cross_y);

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


  int test_status = horiz_words_test(wbase, best_grid, old_grid, new_grid, gwords, word_count, cross_y);

  gwords_free(&gwords, word_count);


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
 * When embeding a word, new words perpendicular to it is generated
 */
static int vert_word_embed(wbase_t* wbase, grid_t* best_grid, grid_t* old_grid, const char* word, int x, int start_y)
{
  // 1. Duplicating the old grid, to work on seperate temp grid
  grid_t* new_grid = grid_dup(old_grid);


  for(int index = 0; word[index] != '\0'; index++)
  {
    int y = start_y + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(new_grid, x, y)) continue;

    // Recursivly call the horizontal gen function
    int gen_status = horiz_word_gen(wbase, best_grid, new_grid, x, y);

    if(gen_status == GEN_FAIL)
    {
      grid_free(&new_grid);

      return GEN_FAIL;
    }

    if(gen_status == GEN_STOP)
    {
      grid_free(&new_grid);

      return GEN_STOP;
    }
  }

  grid_copy(old_grid, new_grid);

  grid_free(&new_grid);

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
    if(!running) return GEN_STOP;

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


  curr_grid_set(old_grid);
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


  int test_status = vert_words_test(wbase, best_grid, old_grid, new_grid, gwords, word_count, cross_x);

  gwords_free(&gwords, word_count);


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

    if(gen_status == GEN_STOP)
    {
      printf("Generation stopped\n");
      break;
    }

    if(gen_status == GEN_FAIL)
    {
      printf("Generation failed\n");
      break;
    }
  }

  curr_grid_set(grid);

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
