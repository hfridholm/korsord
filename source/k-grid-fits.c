/*
 * k-grid-fits.c - check if a word fits
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 * The larger this value is:
 * - the smarter the algorithm can work (prioritize)
 * - the longer the function takes
 *
 * But, even if this function is slower, the algorithm might be faster.
 *
 * This is what you have to take to account when choosing this value.
 */
int MAX_EXIST_AMOUNT = 1000;

/*
 * This function returns the amount of words that exist vertically
 *
 * RETURN (int amount)
 * - min 0                | No words exist
 * - max MAX_EXIST_AMOUNT | Too many words exist
 *
 * Rename to: vert_words_are_available
 */
int vert_words_exist(wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->height + 1];

  if(vert_full_pattern_get(full_pattern, grid, cross_x) != 0)
  {
    return 0; // No words exist
  }

  int start_ys[cross_y + 1];
  int start_count = 0;

  bool start_is_blocked = vert_start_ys_get(start_ys, &start_count, grid, cross_x, cross_y);

  int stop_ys[grid->height - cross_y];
  int stop_count = 0;

  bool stop_is_blocked = vert_stop_ys_get(stop_ys, &stop_count, grid, cross_x, cross_y);


  // Both start and stop is cross_y (1 letter)
  if(start_is_blocked && stop_is_blocked)
  {
    // MAX_EXIST_AMOUNT gives this word the worst priority
    return MAX_EXIST_AMOUNT;
  }


  char pattern[grid->height + 1];

  // The amount of words that exist
  int amount = 0;

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    int start_y = start_ys[start_index];

    for(int stop_index = 0; stop_index < stop_count; stop_index++)
    {
      int stop_y  = stop_ys[stop_index];

      // Don't bother the case where start and stop is cross_y
      if(start_y == stop_y) continue;

      int length = (1 + stop_y - start_y);

      // Create current pattern
      sprintf(pattern, "%.*s", length, full_pattern + start_y);


      int max_amount = (MAX_EXIST_AMOUNT - amount);

      amount += wbase_words_exist_for_pattern(wbase, pattern, max_amount);
      
      // This is opimization only done for performance
      if(amount >= MAX_EXIST_AMOUNT) break;
    }
  }

  return MIN(amount, MAX_EXIST_AMOUNT);
}

/*
 * This function returns the amount of words that exist horizontally
 *
 * RETURN (int amount)
 * - min 0                | No words exist
 * - max MAX_EXIST_AMOUNT | Too many words exist
 *
 * Rename to: horiz_words_are_available
 */
int horiz_words_exist(wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(horiz_full_pattern_get(full_pattern, grid, cross_y) != 0)
  {
    return 0; // No words exist
  }

  int start_xs[cross_x + 1];
  int start_count = 0;

  bool start_is_blocked = horiz_start_xs_get(start_xs, &start_count, grid, cross_x, cross_y);

  int stop_xs[grid->width - cross_x];
  int stop_count = 0;

  bool stop_is_blocked = horiz_stop_xs_get(stop_xs, &stop_count, grid, cross_x, cross_y);


  // Both start and stop is cross_x (1 letter)
  if(start_is_blocked && stop_is_blocked)
  {
    // MAX_EXIST_AMOUNT gives this word the worst priority
    return MAX_EXIST_AMOUNT;
  }


  char pattern[grid->width + 1];

  // The amount of words that exist
  int amount = 0;

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    int start_x = start_xs[start_index];

    for(int stop_index = 0; stop_index < stop_count; stop_index++)
    {
      int stop_x  = stop_xs[stop_index];

      // Don't bother the case where start and stop is cross_x
      if(start_x == stop_x) continue;

      int length = (1 + stop_x - start_x);

      // Create current pattern
      sprintf(pattern, "%.*s", length, full_pattern + start_x);


      int max_amount = (MAX_EXIST_AMOUNT - amount);

      amount += wbase_words_exist_for_pattern(wbase, pattern, max_amount);
      
      // This is opimization only done for performance
      if(amount >= MAX_EXIST_AMOUNT) break;
    }
  }

  return MIN(amount, MAX_EXIST_AMOUNT);
}

/*
 * This is just a helper struct to sort indexes
 */
typedef struct
{
  int index;
  int amount;
} iamount_t;

/*
 * If index1 should be placed before index2,
 * compare function should return negative value
 */
int indexes_compare(const void* index1, const void* index2)
{
  return (((iamount_t*) index1)->amount - ((iamount_t*) index2)->amount);
}

/*
 * Check if the word fits horizontally (on 1st level)
 *
 * The function also provides ranked indexes of
 * which letters have the least existing words
 *
 * indexes is an array of indexes to letters,
 * which are not crossed.
 *
 * EXPECTS:
 * - indexes are allocated
 *
 * RETURN (int count)
 * -  0 | Vertical word don't fit
 * - >0 | Number of letters
 */
int horiz_word_fits(int* indexes, wbase_t* wbase, grid_t* grid, const char* word, int start_x, int y)
{
  // 1. Get the word amount for each index (letter)
  iamount_t iamounts[strlen(word)];
  int count = 0;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int x = start_x + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(grid, x, y)) continue;

    int amount = vert_words_exist(wbase, grid, x, y);

    // If no words exist vertically, the horizontal word don't fit
    if(amount == 0) return 0;

    iamounts[count++] = (iamount_t) {
      .index = index,
      .amount = amount
    };
  }

  // Sort the indexes based on word amount
  qsort(iamounts, count, sizeof(iamount_t), indexes_compare);

  // Store the sorted indexes (letters) based on word amount
  for(int index = 0; index < count; index++)
  {
    indexes[index] = iamounts[index].index;
  }

  return count;
}

/*
 * Check if the word fits vertically (on 1st level)
 *
 * The function also provides ranked indexes of
 * which letters have the least existing words
 *
 * indexes is an array of indexes to letters,
 * which are not crossed.
 *
 * EXPECTS:
 * - indexes are allocated
 *
 * RETURN (int count)
 * -  0 | Vertical word don't fit
 * - >0 | Number of letters
 */
int vert_word_fits(int* indexes, wbase_t* wbase, grid_t* grid, const char* word, int x, int start_y)
{
  // 1. Get the word amount for each index (letter)
  iamount_t iamounts[strlen(word)];
  int count = 0;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int y = start_y + index;

    // A square that is already crossed is done
    if(xy_square_is_crossed(grid, x, y)) continue;

    int amount = horiz_words_exist(wbase, grid, x, y);

    // If no words exist horizontally, the vertical word don't fit
    if(amount == 0) return 0;

    iamounts[count++] = (iamount_t) {
      .index = index,
      .amount = amount
    };
  }

  // Sort the indexes based on word amount
  qsort(iamounts, count, sizeof(iamount_t), indexes_compare);

  // Store the sorted indexes (letters) based on word amount
  for(int index = 0; index < count; index++)
  {
    indexes[index] = iamounts[index].index;
  }

  return count;
}
