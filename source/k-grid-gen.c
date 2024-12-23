/*
 * k-grid-gen.c - generate words in grid
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-grid-curr.h"
#include "k-grid-best.h"

bool is_generating = false;

int HALF_WORD_AMOUNT = 10;

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

static int vert_word_gen(wbase_t* wbase, grid_t* old_grid, int cross_x, int cross_y);

/*
 * When embeding a word, new words perpendicular to it is generated
 *
 * The newly generated words are stored in a copy of the grid,
 * this way, if not all words succeed, the old grid is perserved
 *
 * If the words are generated in the old_grid,
 * the grid will probaly not be solved if one letter fails
 *
 * PARAMS
 * - int* indexes | Which indexes (letters) to embed
 * - int  count   | Number of letters to embed
 *
 * RETURN (int status)
 */
static int horiz_word_embed(wbase_t* wbase, grid_t* old_grid, const char* word, int start_x, int y, int* indexes, int count)
{
  // 1. Duplicating the old grid, to work on seperate temp grid
  grid_t* new_grid = grid_dup(old_grid);

  /*
   * Important to note that:
   * "index" is the index of the indexes array
   * indexes[index] is the index of the letter
   */
  for(int index = 0; index < count; index++)
  {
    int x = start_x + indexes[index];

    /*
     * A square that is already crossed is done
     *
     * This is very important to check, because
     * otherwise, vert_word_gen will fail, because
     * the word that has been filled in is 
     * not available anymore
     */
    if(xy_square_is_crossed(new_grid, x, y)) continue;

    // Recursivly call the vertical gen function
    int gen_status = vert_word_gen(wbase, new_grid, x, y);

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
static int horiz_word_test(wbase_t* wbase, grid_t* old_grid, grid_t* new_grid, const char* word, int x, int y)
{
  // 1. Insert the word in the grid
  if(horiz_word_insert(wbase, new_grid, word, x, y) == INSERT_PERFECT)
  {
    // If the word fits perfect, the crossword is solved?
    return GEN_DONE;
  }


  // Get an ordered list of indexes to letters to embed
  int indexes[strlen(word)];

  int count = horiz_word_fits(indexes, wbase, new_grid, word, x, y);

  // 2. If the word doesn't fit
  if(count == 0)
  {
    // 3. Remove the tested word from the grid
    horiz_word_reset(wbase, old_grid, new_grid, word, x, y);

    return GEN_FAIL;
  }


  // 3. Embed the word horizontally, by generating words for letters
  int embed_status = horiz_word_embed(wbase, new_grid, word, x, y, indexes, count);

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
static int horiz_words_test(wbase_t* wbase, grid_t* old_grid, grid_t* new_grid, gword_t* gwords, size_t word_count, int y)
{
  for(size_t index = 0; index < word_count; index++)
  {
    if(!is_generating) return GEN_STOP;

    gword_t gword = gwords[index];

    char* word  = gword.word;
    int start_x = gword.start;

    int test_status = horiz_word_test(wbase, old_grid, new_grid, word, start_x, y);

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
static int horiz_word_gen(wbase_t* wbase, grid_t* old_grid, int cross_x, int cross_y)
{
  if(!is_generating) return GEN_STOP;


  curr_grid_set(old_grid);

  // curr_grid_print();
  // usleep(1000000);


  square_t* square = xy_square_get(old_grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  if(old_grid->cross_count > best_grid_cross_count_get())
  {
    // info_print("new best grid: %d", grid->cross_count);
    best_grid_set(old_grid);
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


  int test_status = horiz_words_test(wbase, old_grid, new_grid, gwords, word_count, cross_y);

  gwords_free(&gwords, word_count);


  if(test_status == GEN_DONE)
  {
    grid_copy(old_grid, new_grid);
  }

  grid_free(&new_grid);

  return test_status;
}

/*
 * When embeding a word, new words perpendicular to it is generated
 *
 * The newly generated words are stored in a copy of the grid,
 * this way, if not all words succeed, the old grid is perserved
 *
 * If the words are generated in the old_grid,
 * the grid will probaly not be solved if one letter fails
 *
 * PARAMS
 * - int* indexes | Which indexes (letters) to embed
 * - int  count   | Number of letters to embed
 *
 * RETURN (int status)
 */
static int vert_word_embed(wbase_t* wbase, grid_t* old_grid, const char* word, int x, int start_y, int* indexes, int count)
{
  // 1. Duplicating the old grid, to work on seperate temp grid
  grid_t* new_grid = grid_dup(old_grid);

  /*
   * Important to note that:
   * "index" is the index of the indexes array
   * indexes[index] is the index of the letter
   */
  for(int index = 0; index < count; index++)
  {
    int y = start_y + indexes[index];

    // A square that is already crossed is done
    if(xy_square_is_crossed(new_grid, x, y)) continue;

    // Recursivly call the horizontal gen function
    int gen_status = horiz_word_gen(wbase, new_grid, x, y);

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
static int vert_word_test(wbase_t* wbase, grid_t* old_grid, grid_t* new_grid, const char* word, int x, int y)
{
  // 1. Insert the word in the grid
  if(vert_word_insert(wbase, new_grid, word, x, y) == INSERT_PERFECT)
  {
    // If the word fits perfect, the crossword is solved?
    return GEN_DONE;
  }


  // Get an ordered list of indexes to letters to embed
  int indexes[strlen(word)];

  int count = vert_word_fits(indexes, wbase, new_grid, word, x, y);

  // 2. If the word doesn't fit
  if(count == 0)
  {
    // 3. Remove the tested word from the grid
    vert_word_reset(wbase, old_grid, new_grid, word, x, y);

    return GEN_FAIL;
  }


  // 3. Embed the word vertically, by generating words for letters
  int embed_status = vert_word_embed(wbase, new_grid, word, x, y, indexes, count);

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
static int vert_words_test(wbase_t* wbase, grid_t* old_grid, grid_t* new_grid, gword_t* gwords, size_t word_count, int x)
{
  for(size_t index = 0; index < word_count; index++)
  {
    if(!is_generating) return GEN_STOP;

    gword_t gword = gwords[index];

    char* word  = gword.word;
    int start_y = gword.start;

    int test_status = vert_word_test(wbase, old_grid, new_grid, word, x, start_y);

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
static int vert_word_gen(wbase_t* wbase, grid_t* old_grid, int cross_x, int cross_y)
{
  if(!is_generating) return GEN_STOP;


  curr_grid_set(old_grid);

  // curr_grid_print();
  // usleep(1000000);


  square_t* square = xy_square_get(old_grid, cross_x, cross_y);

  if(!square || square->type == SQUARE_BLOCK) return GEN_FAIL;


  if(old_grid->cross_count > best_grid_cross_count_get())
  {
    // info_print("new best grid: %d", grid->cross_count);
    best_grid_set(old_grid);
  }

  // 1. Generate all possible words
  gword_t* gwords = NULL;
  size_t word_count = 0;

  int gwords_status = vert_gwords_get(&gwords, &word_count, wbase, old_grid, cross_x, cross_y);

  // info_print("vert_gwords_get count: %d", word_count);

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


  int test_status = vert_words_test(wbase, old_grid, new_grid, gwords, word_count, cross_x);

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
  // 1. Load grid from the model
  grid_t* grid = grid_model_load(filepath);

  if(!grid) return NULL;

  // 2. Prepare the grid for generation
  grid_prep(grid);

  best_grid_set(grid);

  is_generating = true;

  for(int index = 0; index < grid->square_count; index++)
  {
    int x = (index % grid->width);
    int y = (index / grid->width);

    if(xy_square_is_done(grid, x, y)) continue;
      
    int gen_status = vert_word_gen(wbase, grid, x, y);

    if(gen_status == GEN_STOP)
    {
      error_print("Generation stopped");
      break;
    }

    if(gen_status == GEN_FAIL)
    {
      error_print("Generation failed");
      break;
    }
  }

  is_generating = false;

  curr_grid_set(grid);

  return grid;
}
