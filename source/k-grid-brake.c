/*
 * k-grid-brake.c - would block brake words?
 *
 * The functions in this file is only ran,
 * if the following functions is used:
 *
 * horiz_non_break_start_xs_get
 * horiz_non_break_stop_xs_get
 *
 * vert_non_break_start_xs_get
 * vert_non_break_stop_xs_get
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-grid-span.h"

#include "k-wbase.h"

/*
 * Check if a word exists vertically, starting at start_y
 *
 * RETURN (bool does_exist)
 */
static bool vert_start_word_exists(wbase_t* wbase, grid_t* grid, int x, int start_y)
{
  // 1. Create full pattern
  char full_pattern[grid->height + 1];

  if(vert_full_pattern_get(full_pattern, grid, x) != 0)
  {
    return false; // No words exist
  }

  int stop_ys[grid->height - start_y];
  int stop_count = 0;

  bool stop_is_blocked = vert_stop_ys_get(stop_ys, &stop_count, grid, x, start_y);

  // Both start and stop is start_y (1 letter)
  if(stop_is_blocked)
  {
    return true; // No words exist, but it is not ment to either
  }


  char pattern[grid->height + 1];

  for(int stop_index = 0; stop_index < stop_count; stop_index++)
  {
    int stop_y = stop_ys[stop_index];

    // Don't bother the case where start and stop is cross_y
    if(start_y == stop_y) continue;

    int length = (1 + stop_y - start_y);

    // Create current pattern
    sprintf(pattern, "%.*s", length, full_pattern + start_y);

    if(wbase_word_exists_for_pattern(wbase, grid->words, pattern))
    {
      return true;
    }
  }

  return false;
}

/*
 * Check if a word exists vertically, starting at start_y
 *
 * RETURN (bool does_exist)
 */
static bool vert_stop_word_exists(wbase_t* wbase, grid_t* grid, int x, int stop_y)
{
  // 1. Create full pattern
  char full_pattern[grid->height + 1];

  if(vert_full_pattern_get(full_pattern, grid, x) != 0)
  {
    return false; // No words exist
  }

  int start_ys[stop_y + 1];
  int start_count = 0;

  bool start_is_blocked = vert_start_ys_get(start_ys, &start_count, grid, x, stop_y);

  // Both start and stop is stop_y (1 letter)
  if(start_is_blocked)
  {
    return true; // No words exist, but it is not ment to either
  }


  char pattern[grid->height + 1];

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    int start_y = start_ys[start_index];

    // Don't bother the case where start and stop is cross_y
    if(start_y == stop_y) continue;

    int length = (1 + stop_y - start_y);

    // Create current pattern
    sprintf(pattern, "%.*s", length, full_pattern + start_y);

    if(wbase_word_exists_for_pattern(wbase, grid->words, pattern))
    {
      return true;
    }
  }

  return false;
}

/*
 * Check if a word exists horizontally, starting at start_x
 *
 * RETURN (bool does_exist)
 */
static int horiz_start_word_exists(wbase_t* wbase, grid_t* grid, int start_x, int y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(horiz_full_pattern_get(full_pattern, grid, y) != 0)
  {
    return false; // No words exist
  }

  int stop_xs[grid->width - start_x];
  int stop_count = 0;

  bool stop_is_blocked = horiz_stop_xs_get(stop_xs, &stop_count, grid, start_x, y);

  // Both start and stop is start_x (1 letter)
  if(stop_is_blocked)
  {
    return true; // No words exist, but it is not ment to either
  }


  char pattern[grid->width + 1];

  for(int stop_index = 0; stop_index < stop_count; stop_index++)
  {
    int stop_x = stop_xs[stop_index];

    if(start_x == stop_x) continue;

    int length = (1 + stop_x - start_x);

    // Create current pattern
    sprintf(pattern, "%.*s", length, full_pattern + start_x);

    if(wbase_word_exists_for_pattern(wbase, grid->words, pattern))
    {
      return true;
    }
  }

  return false;
}

/*
 * Check if a word exists horizontally, stopping at stop_x
 *
 * RETURN (bool does_exist)
 */
static int horiz_stop_word_exists(wbase_t* wbase, grid_t* grid, int stop_x, int y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(horiz_full_pattern_get(full_pattern, grid, y) != 0)
  {
    return false; // No words exist
  }

  int start_xs[stop_x + 1];
  int start_count = 0;

  bool start_is_blocked = horiz_start_xs_get(start_xs, &start_count, grid, stop_x, y);

  // Both start and stop is stop_x (1 letter)
  if(start_is_blocked)
  {
    return true; // No words exist, but it is not ment to either
  }


  char pattern[grid->width + 1];

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    int start_x = start_xs[start_index];

    if(start_x == stop_x) continue;

    int length = (1 + stop_x - start_x);

    // Create current pattern
    sprintf(pattern, "%.*s", length, full_pattern + start_x);

    if(wbase_word_exists_for_pattern(wbase, grid->words, pattern))
    {
      return true;
    }
  }

  return false;
}

/*
 * Check if block square would brake words in two,
 * meaning it would make it impossible to fill in words
 *
 *   ?
 * ? # ?
 *   _
 *
 * RETURN (bool do_brake_words)
 */
bool vert_start_block_brakes_words(wbase_t* wbase, grid_t* grid, int block_x, int block_y)
{
  // 1. Check so the word to the left is not broken
  if ((block_x > 0) &&
     !xy_square_is_blocking(grid, block_x - 1, block_y) &&
     !horiz_stop_word_exists(wbase, grid, block_x - 1, block_y))
  {
    return true;
  }

  // 2. Check so the word to the right is not broken
  if ((block_x < (grid->width - 1)) &&
     !xy_square_is_blocking(grid, block_x + 1, block_y) &&
     !horiz_start_word_exists(wbase, grid, block_x + 1, block_y))
  {
    return true;
  }

  // 3. Check so the word on the opposite side is not broken
  if ((block_y > 0) &&
     !xy_square_is_blocking(grid, block_x, block_y - 1) &&
     !vert_stop_word_exists(wbase, grid, block_x, block_y - 1))
  {
    return true;
  }

  return false;
}

/*
 * Check if block square would brake words in two,
 * meaning it would make it impossible to fill in words
 *
 *   ?
 * ? # _
 *   ?
 *
 * RETURN (bool do_brake_words)
 */
bool horiz_start_block_brakes_words(wbase_t* wbase, grid_t* grid, int block_x, int block_y)
{
  // 1. Check so the word on top is not broken
  if ((block_y > 0) &&
     !xy_square_is_blocking(grid, block_x, block_y - 1) &&
     !vert_stop_word_exists(wbase, grid, block_x, block_y - 1))
  {
    return true;
  }

  // 2. Check so the word on bottom is not broken
  if ((block_y < (grid->height - 1)) &&
     !xy_square_is_blocking(grid, block_x, block_y + 1) &&
     !vert_start_word_exists(wbase, grid, block_x, block_y + 1))
  {
    return true;
  }

  // 3. Check so the word on the opposite side is not broken
  if ((block_x > 0) &&
     !xy_square_is_blocking(grid, block_x - 1, block_y) &&
     !horiz_stop_word_exists(wbase, grid, block_x - 1, block_y))
  {
    return true;
  }


  return false;
}

/*
 * Check if block square would brake words in two,
 * meaning it would make it impossible to fill in words
 *
 *   _
 * ? # ?
 *   ?
 *
 * RETURN (bool do_brake_words)
 */
bool vert_stop_block_brakes_words(wbase_t* wbase, grid_t* grid, int block_x, int block_y)
{
  // 1. Check so the word to the left is not broken
  if ((block_x > 0) &&
     !xy_square_is_blocking(grid, block_x - 1, block_y) &&
     !horiz_stop_word_exists(wbase, grid, block_x - 1, block_y))
  {
    return true;
  }

  // 2. Check so the word to the right is not broken
  if ((block_x < (grid->width - 1)) &&
     !xy_square_is_blocking(grid, block_x + 1, block_y) &&
     !horiz_start_word_exists(wbase, grid, block_x + 1, block_y))
  {
    return true;
  }

  // 3. Check so the word on the opposite side is not broken
  if ((block_y < (grid->height - 1)) &&
     !xy_square_is_blocking(grid, block_x, block_y + 1) &&
     !vert_start_word_exists(wbase, grid, block_x, block_y + 1))
  {
    return true;
  }

  return false;
}

/*
 * Check if block square would brake words in two,
 * meaning it would make it impossible to fill in words
 *
 *   ?
 * _ # ?
 *   ?
 *
 * RETURN (bool do_brake_words)
 */
bool horiz_stop_block_brakes_words(wbase_t* wbase, grid_t* grid, int block_x, int block_y)
{
  // 1. Check so the word on top is not broken
  if ((block_y > 0) &&
     !xy_square_is_blocking(grid, block_x, block_y - 1) &&
     !vert_stop_word_exists(wbase, grid, block_x, block_y - 1))
  {
    return true;
  }

  // 2. Check so the word on bottom is not broken
  if ((block_y < (grid->height - 1)) &&
     !xy_square_is_blocking(grid, block_x, block_y + 1) &&
     !vert_start_word_exists(wbase, grid, block_x, block_y + 1))
  {
    return true;
  }

  // 3. Check so the word on the opposite side is not broken
  if ((block_x < (grid->width - 1)) &&
     !xy_square_is_blocking(grid, block_x + 1, block_y) &&
     !horiz_start_word_exists(wbase, grid, block_x + 1, block_y))
  {
    return true;
  }

  return false;
}
