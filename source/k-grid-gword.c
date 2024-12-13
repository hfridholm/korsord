/*
 * k-grid-gword.c - grid word
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

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
static int gwords_search(gword_t** gwords, size_t* count, trie_t* trie, const char* pattern, int start, int stop)
{
  char** words = NULL;
  size_t word_count = 0;

  words_search(&words, &word_count, trie, pattern);

  if(word_count == 0)
  {
    // No words to add
    return 0;
  }

  gword_t* temp_gwords = realloc(*gwords, sizeof(gword_t) * ((*count) + word_count));

  if(!temp_gwords)
  {
    words_free(&words, word_count);

    perror("realloc gwords");

    return 1;
  }

  *gwords = temp_gwords;

  for(size_t index = 0; index < word_count; index++)
  {
    (*gwords)[(*count) + index] = (gword_t)
    { 
      .word  = words[index],
      .start = start,
      .stop  = stop
    };
  }

  *count += word_count;

  return 0;
}

/*
 *
 */
static int horizontal_full_pattern_get(char* pattern, grid_t* grid, int y)
{
  for(int x = 0; x < grid->width; x++)
  {
    square_t* square = grid_xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[x] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[grid->width] = '\0';

  return 0;
}

/*
 *
 */
static void horizontal_start_xs_get(int* start_xs, int* count, grid_t* grid, int cross_x, int cross_y)
{
  for(int start_x = (cross_x + 1); start_x-- > 0;)
  {
    if(xy_square_is_blocking(grid, start_x, cross_y)) break;

    if(start_x > 0)
    {
      if(!block_square_is_allowed(grid, start_x - 1, cross_y)) continue;
    }

    start_xs[(*count)++] = start_x;
  }
}

/*
 *
 */
static void horizontal_stop_xs_get(int* stop_xs, int* count, grid_t* grid, int cross_x, int cross_y)
{
  for(int stop_x = cross_x; stop_x < grid->width; stop_x++)
  {
    if(xy_square_is_blocking(grid, stop_x, cross_y)) break;

    if(stop_x < (grid->width - 1))
    {
      if(!block_square_is_allowed(grid, stop_x + 1, cross_y)) continue;
    }

    stop_xs[(*count)++] = stop_x;
  }
}

/*
 *
 */
int horizontal_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(horizontal_full_pattern_get(full_pattern, grid, cross_y) != 0)
  {
    return GWORDS_FAIL;
  }

  int start_xs[cross_x + 1];
  int start_count = 0;

  horizontal_start_xs_get(start_xs, &start_count, grid, cross_x, cross_y);

  int stop_xs[grid->width - cross_x];
  int stop_count = 0;

  horizontal_stop_xs_get(stop_xs, &stop_count, grid, cross_x, cross_y);


  printf("horizontal:\n");
  printf("cross x:%d cross y:%d\n", cross_x, cross_y);
  printf("start_count: %d\n", start_count);
  printf("stop_count: %d\n", stop_count);

  if(start_count == 1 && stop_count == 1)
  {
    // Here: Both start and stop is cross_x (1 letter)
    return GWORDS_SINGLE;
  }


  gword_t* primary_gwords = NULL;
  size_t primary_count = 0;

  gword_t* backup_gwords = NULL;
  size_t backup_count = 0;

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

      printf("pattern: (%s)\n", pattern);

      gwords_search(&primary_gwords, &primary_count, wbase->primary, pattern, start_x, stop_x);

      gwords_search(&backup_gwords, &backup_count, wbase->backup, pattern, start_x, stop_x);
    }
  }

  printf("primary_count: %ld\n", primary_count);
  printf("backup_count: %ld\n", backup_count);

  if(primary_count == 0 && backup_count == 0)
  {
    // No words was found
    return GWORDS_NO_WORDS;
  }


  // 2. Shuffle the two word lists seperately
  gwords_shuffle(primary_gwords, primary_count);

  gwords_shuffle(backup_gwords,  backup_count);

  // 3. Concatonate the two shuffled lists
  *count = (primary_count + backup_count);

  *gwords = malloc(sizeof(gword_t) * *count);

  memcpy(*gwords, primary_gwords, sizeof(gword_t) * primary_count);

  memcpy(*gwords + primary_count, backup_gwords, sizeof(gword_t) * backup_count);

  free(primary_gwords);

  free(backup_gwords);

  return GWORDS_DONE;
}

/*
 *
 */
static int vertical_full_pattern_get(char* pattern, grid_t* grid, int x)
{
  for(int y = 0; y < grid->height; y++)
  {
    square_t* square = grid_xy_square_get(grid, x, y);

    if(!square) return 1;

    pattern[y] = (square->type == SQUARE_LETTER) ? square->letter : '_';
  }

  pattern[grid->height] = '\0';

  return 0;
}

/*
 *
 */
static void vertical_start_ys_get(int* start_ys, int* count, grid_t* grid, int cross_x, int cross_y)
{
  for(int start_y = (cross_y + 1); start_y-- > 0;)
  {
    if(xy_square_is_blocking(grid, cross_x, start_y)) break;

    if(start_y > 0)
    {
      if(!block_square_is_allowed(grid, cross_x, start_y - 1)) continue;
    }

    start_ys[(*count)++] = start_y;
  }
}

/*
 *
 */
static void vertical_stop_ys_get(int* stop_ys, int* count, grid_t* grid, int cross_x, int cross_y)
{
  for(int stop_y = cross_y; stop_y < grid->height; stop_y++)
  {
    if(xy_square_is_blocking(grid, cross_x, stop_y)) break;

    if(stop_y < (grid->height - 1))
    {
      if(!block_square_is_allowed(grid, cross_x, stop_y + 1)) continue;
    }

    stop_ys[(*count)++] = stop_y;
  }
}

/*
 *
 */
int vertical_gwords_get(gword_t** gwords, size_t* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  // 1. Create full pattern
  char full_pattern[grid->width + 1];

  if(vertical_full_pattern_get(full_pattern, grid, cross_x) != 0)
  {
    return GWORDS_FAIL;
  }

  int start_ys[cross_y + 1];
  int start_count = 0;

  vertical_start_ys_get(start_ys, &start_count, grid, cross_x, cross_y);

  int stop_ys[grid->width - cross_y];
  int stop_count = 0;

  vertical_stop_ys_get(stop_ys, &stop_count, grid, cross_x, cross_y);


  printf("vertical:\n");
  printf("cross x:%d cross y:%d\n", cross_x, cross_y);
  printf("start_count: %d\n", start_count);
  printf("stop_count: %d\n", stop_count);

  if(start_count == 1 && stop_count == 1)
  {
    // Here: Both start and stop is cross_y (1 letter)
    return GWORDS_SINGLE;
  }


  gword_t* primary_gwords = NULL;
  size_t primary_count = 0;

  gword_t* backup_gwords = NULL;
  size_t backup_count = 0;

  char pattern[grid->width + 1];

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


      gwords_search(&primary_gwords, &primary_count, wbase->primary, pattern, start_y, stop_y);

      gwords_search(&backup_gwords, &backup_count, wbase->backup, pattern, start_y, stop_y);
    }
  }

  printf("primary_count: %ld\n", primary_count);
  printf("backup_count: %ld\n", backup_count);

  if(primary_count == 0 && backup_count == 0)
  {
    // No words was found
    return GWORDS_NO_WORDS;
  }

  // The following could be extracted to a function:


  // 2. Shuffle the two word lists seperately
  gwords_shuffle(primary_gwords, primary_count);

  gwords_shuffle(backup_gwords,  backup_count);

  // 3. Concatonate the two shuffled lists
  *count = (primary_count + backup_count);

  *gwords = malloc(sizeof(gword_t) * *count);

  memcpy(*gwords, primary_gwords, sizeof(gword_t) * primary_count);

  memcpy(*gwords + primary_count, backup_gwords, sizeof(gword_t) * backup_count);

  free(primary_gwords);

  free(backup_gwords);

  return GWORDS_DONE;
}
