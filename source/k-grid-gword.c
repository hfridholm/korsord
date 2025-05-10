/*
 * k-grid-gword.c - grid word functions
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-grid-span.h"

#include "k-wbase.h"

/*
 * Shuffle grid words
 */
static void gwords_shuffle(gword_t* gwords, size_t count)
{
  for(size_t index = 0; index < count; index++)
  {
    size_t rand_index = (rand() % count);

    gword_t temp_gword = gwords[index];

    gwords[index] = gwords[rand_index];

    gwords[rand_index] = temp_gword;
  }
}

/*
 * Free grid words
 */
void gwords_free(gword_t** gwords, size_t count)
{
  if(!gwords || !(*gwords)) return;

  for(size_t index = 0; index < count; index++)
  {
    free((*gwords)[index].word);
  }

  free(*gwords);

  *gwords = NULL;
}

// __builtin_clzll counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1ULL << (64 - __builtin_clzll(n)))

/*
 * This function searches for words fitting the pattern,
 * and labels them as gword_t with start and stop
 *
 * The function allocates memory for gwords,
 * which the user will have to free
 *
 * RETURN (int status)
 * - 0 | Success
 * - 1 | Failed to allocate gwords
 */
static int gwords_search(gword_t** gwords, size_t* count, trie_t* trie, trie_t* used_trie, const char* pattern, int start, int stop)
{
  char** words = NULL;
  size_t word_count = 0;

  words_search(&words, &word_count, trie, used_trie, pattern);

  if (word_count == 0)
  {
    // No words to add
    return 0;
  }

  if (*count == 0 || ((*count) + word_count) >= CAPACITY(*count))
  {
    gword_t* new_gwords = realloc(*gwords, sizeof(gword_t) * CAPACITY((*count) + word_count));

    if (!new_gwords)
    {
      words_free(&words, word_count);

      return 1;
    }

    *gwords = new_gwords;
  }

  for (size_t index = 0; index < word_count; index++)
  {
    (*gwords)[(*count) + index] = (gword_t)
    { 
      .word  = words[index],
      .start = start,
      .stop  = stop
    };
  }

  *count += word_count;

  // Only free the pointer to the array of words,
  // because the string pointers are being reused by gword_t
  free(words);

  return 0;
}

/*
 * Get the full pattern of a horizontal line
 */
int horiz_full_pattern_get(char* pattern, grid_t* grid, int y)
{
  for(int x = 0; x < grid->width; x++)
  {
    square_t* square = xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[x] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[grid->width] = '\0';

  return 0;
}

/*
 * This struct is only used by these internal functions
 */
typedef struct gwords_t
{
  gword_t* gwords;
  size_t   count;
} gwords_t;

/*
 * Search grid words from an array of word bases
 */
static void gwords_array_search(size_t* total_count, gwords_t* gwords_array, wbase_t* wbase, trie_t* used_trie, const char* pattern, int start, int stop)
{
  for(size_t index = 0; index < wbase->count; index++)
  {
    gword_t** curr_gwords = &gwords_array[index].gwords;
    size_t*   curr_count  = &gwords_array[index].count;

    size_t old_count = *curr_count;

    trie_t* curr_trie = wbase->tries[index];

    gwords_search(curr_gwords, curr_count, curr_trie, used_trie, pattern, start, stop);

    // Increase total_count by how many gwords was added
    *total_count += (*curr_count - old_count);
  }
}

/*
 * Shuffle an array of grid words
 */
static void gwords_array_shuffle(gwords_t* gwords_array, size_t count)
{
  for(size_t index = 0; index < count; index++)
  {
    gword_t* curr_gwords = gwords_array[index].gwords;
    size_t   curr_count  = gwords_array[index].count;

    gwords_shuffle(curr_gwords, curr_count);
  }
}

/*
 * Merge array of grid words together to one array of grid words
 *
 * Only free the pointer to the merged grid words arrays,
 * as the actual words is still being used
 */
static void gwords_array_merge(gword_t* gwords, gwords_t* gwords_array, size_t count)
{
  gword_t* pointer = gwords;

  for(size_t index = 0; index < count; index++)
  {
    gword_t* curr_gwords = gwords_array[index].gwords;
    size_t   curr_count  = gwords_array[index].count;

    memcpy(pointer, curr_gwords, sizeof(gword_t) * curr_count);

    pointer += curr_count;

    free(curr_gwords);
  }
}

/*
 * Free the grid words array and all the words
 *
 * in case of the memory allocation to gwords failed
 */
static void gwords_array_free(gwords_t* gwords_array, size_t count)
{
  for(size_t index = 0; index < count; index++)
  {
    gwords_t gwords = gwords_array[index];

    gwords_free(&gwords.gwords, gwords.count);
  }
}

/*
 * Get the horizontal grid words through cross x, y
 */
int horiz_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(horiz_full_pattern_get(full_pattern, grid, cross_y) != 0)
  {
    return GWORDS_FAIL;
  }

  int start_xs[cross_x + 1];
  int start_count = 0;

  bool start_is_blocked = horiz_non_break_start_xs_get(start_xs, &start_count, wbase, grid, cross_x, cross_y);

  int stop_xs[grid->width - cross_x];
  int stop_count = 0;

  bool stop_is_blocked = horiz_non_break_stop_xs_get(stop_xs, &stop_count, wbase, grid, cross_x, cross_y);


  if(start_is_blocked && stop_is_blocked)
  {
    // Here: Both start and stop is cross_x (1 letter)
    return GWORDS_SINGLE;
  }


  gwords_t gwords_array[wbase->count];
  memset(gwords_array, 0, sizeof(gwords_t) * wbase->count);


  size_t total_count = 0;

  char pattern[grid->width + 1];

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    for(int stop_index = 0; stop_index < stop_count; stop_index++)
    {
      int start_x = start_xs[start_index];
      int stop_x  = stop_xs[stop_index];

      // Don't bother the case where start and stop is cross_x
      if(start_x == stop_x) continue;

      int length = (1 + stop_x - start_x);

      // Create current pattern
      sprintf(pattern, "%.*s", length, full_pattern + start_x);

      gwords_array_search(&total_count, gwords_array, wbase, grid->words, pattern, start_x, stop_x);
    }
  }

  if(total_count == 0)
  {
    return GWORDS_NO_WORDS;
  }

  // 2. Suffle word lists seperately
  gwords_array_shuffle(gwords_array, wbase->count);

  // 3. Concatonate shuffled lists
  gword_t* new_gwords = malloc(sizeof(gword_t) * total_count);

  if(!new_gwords)
  {
    gwords_array_free(gwords_array, wbase->count);

    return GWORDS_FAIL;
  }

  *gwords = new_gwords;
  *count  = total_count;

  gwords_array_merge(*gwords, gwords_array, wbase->count);

  return GWORDS_DONE;
}

/*
 * Get the full pattern of a vertical line
 */
int vert_full_pattern_get(char* pattern, grid_t* grid, int x)
{
  for(int y = 0; y < grid->height; y++)
  {
    square_t* square = xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[y] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[grid->height] = '\0';

  return 0;
}

/*
 * Get the vertical grid words through cross x, y
 */
int vert_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->height + 1];

  if(vert_full_pattern_get(full_pattern, grid, cross_x) != 0)
  {
    return GWORDS_FAIL;
  }

  int start_ys[cross_y + 1];
  int start_count = 0;

  bool start_is_blocked = vert_non_break_start_ys_get(start_ys, &start_count, wbase, grid, cross_x, cross_y);

  int stop_ys[grid->height - cross_y];
  int stop_count = 0;

  bool stop_is_blocked = vert_non_break_stop_ys_get(stop_ys, &stop_count, wbase, grid, cross_x, cross_y);


  if(start_is_blocked && stop_is_blocked)
  {
    // Here: Both start and stop is cross_y (1 letter)
    return GWORDS_SINGLE;
  }


  gwords_t gwords_array[wbase->count];
  memset(gwords_array, 0, sizeof(gwords_t) * wbase->count);


  size_t total_count = 0;

  char pattern[grid->height + 1];

  for(int start_index = 0; start_index < start_count; start_index++)
  {
    for(int stop_index = 0; stop_index < stop_count; stop_index++)
    {
      int start_y = start_ys[start_index];
      int stop_y  = stop_ys[stop_index];

      // Don't bother the case where start and stop is cross_y
      if(start_y == stop_y) continue;

      int length = (1 + stop_y - start_y);

      // Create current pattern
      sprintf(pattern, "%.*s", length, full_pattern + start_y);

      gwords_array_search(&total_count, gwords_array, wbase, grid->words, pattern, start_y, stop_y);
    }
  }

  if(total_count == 0)
  {
    return GWORDS_NO_WORDS;
  }

  // 2. Suffle word lists seperately
  gwords_array_shuffle(gwords_array, wbase->count);

  // 3. Concatonate shuffled lists
  gword_t* new_gwords = malloc(sizeof(gword_t) * total_count);

  if(!new_gwords)
  {
    gwords_array_free(gwords_array, wbase->count);

    return GWORDS_FAIL;
  }

  *gwords = new_gwords;
  *count  = total_count;

  gwords_array_merge(*gwords, gwords_array, wbase->count);

  return GWORDS_DONE;
}
