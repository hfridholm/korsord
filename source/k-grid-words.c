/*
 *
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 *
 */
static bool length_is_allowed(grid_t* grid, int start_x, int start_y, int length, bool vertical)
{
  int stop_x, stop_y;

  if(vertical)
  {
    stop_x = start_x;
    stop_y = (start_y + length - 1);
  }
  else
  {
    stop_x = (start_x + length - 1);
    stop_y = start_y;
  }

  if(vertical)
  {
    int block_y = (start_y + length);

    if(xy_square_is_letter(grid, start_x, block_y)) return false;
  }
  else
  {
    int block_x = (start_x + length);

    if(xy_square_is_letter(grid, block_x, start_y)) return false;
  }

  if(!pattern_is_allowed_trap(grid, stop_x, stop_y)) return false;

  if(!pattern_is_allowed_crowd(grid, stop_x, stop_y)) return false;

  return true;
}

/*
 * EXPECTS:
 * - words are either allocated or NULL
 * - count is defined as an integer
 */
void grid_words_search(char*** words, size_t* count, wbase_t* wbase, const char* pattern, grid_t* grid, int start_x, int start_y, int max_length, bool vertical)
{
  char** primary_words = NULL;
  size_t primary_count = 0;

  char** backup_words = NULL;
  size_t backup_count = 0;

  char* temp_pattern = strdup(pattern);

  for(int length = (max_length + 1); length-- > 2;)
  {
    if(!length_is_allowed(grid, start_x, start_y, length, vertical)) continue;

    temp_pattern[length] = '\0';

    words_search(&primary_words, &primary_count, wbase->primary, temp_pattern);

    words_search(&backup_words,  &backup_count,  wbase->backup, temp_pattern);
  }

  free(temp_pattern);

  if(primary_count == 0 && backup_count == 0)
  {
    // No words was found
    return;
  }

  // 2. Shuffle the two word lists seperately
  words_shuffle(primary_words, primary_count);

  words_shuffle(backup_words,  backup_count);

  // 3. Concatonate the two shuffled lists
  *count = (primary_count + backup_count);

  *words = malloc(sizeof(char*) * *count);

  /*
  memcpy(*words, primary_words, sizeof(char*) * primary_count);

  memcpy(*words + primary_count, backup_words, sizeof(char*) * backup_count);

  free(primary_words);

  free(backup_words);
  */

  for(int index = 0; index < primary_count; index++)
  {
    (*words)[index] = strdup(primary_words[index]);
  }

  for(int index = 0; index < backup_count; index++)
  {
    (*words)[primary_count + index] = strdup(backup_words[index]);
  }

  words_free(&primary_words, primary_count);

  words_free(&backup_words, backup_count);
}
