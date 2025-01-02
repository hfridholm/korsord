/*
 * k-grid-export.c - export grid and words to file
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

/*
 * Export used words
 *
 * RETURN (int status)
 * - 0 | Success
 * - 1 | Failed to get grid words
 * - 2 | Failed to open file
 */
int used_words_export(grid_t* grid)
{
  // 1. Get all words in grid
  char** words = NULL;
  size_t count = 0;

  if(grid_words_get(&words, &count, grid) != 0)
  {
    error_print("Failed to get grid words");

    return 1;
  }

  // 2. Open or create file to append to
  FILE* file = fopen("used.words", "a");

  if(!file)
  {
    words_free(&words, count);

    error_print("Failed to open file");

    return 2;
  }

  // 4. Print each word formatted on new line
  for(size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    fprintf(file, "%s\n", word);
  }

  fclose(file);

  words_free(&words, count);

  return 0;
}

/*
 * Export grid words
 *
 * RETURN (int status)
 * - 0 | Success
 * - 1 | Failed to get grid words
 * - 2 | Failed to open file
 */
int grid_words_export(grid_t* grid)
{
  // 1. Get all words in grid
  char** words = NULL;
  size_t count = 0;

  if(grid_words_get(&words, &count, grid) != 0)
  {
    error_print("Failed to get grid words");

    return 1;
  }

  // 2. Get max width for alignment
  int max_width = 0;

  for(size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    int curr_width = strlen(word) + 1;

    max_width = MAX(max_width, curr_width);
  }

  // 3. Open or create file to write to
  FILE* file = fopen("result.words", "w");

  if(!file)
  {
    words_free(&words, count);

    error_print("Failed to open file");

    return 2;
  }

  // 4. Print each word formatted on new line
  for(size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    int curr_width = max_width - strlen(word);

    fprintf(file, "%s%*c:\n", word, curr_width, ' ');
  }

  fclose(file);

  words_free(&words, count);

  return 0;
}

/*
 * Export grid to file
 */
int grid_export(grid_t* grid)
{
  if(!grid || !grid->squares) return 1;

  // 3. Open or create file to write to
  FILE* file = fopen("result.grid", "w");

  if(!file)
  {
    error_print("Failed to open file");

    return 2;
  }

  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      square_t* square = xy_square_get(grid, x, y);

      if(!square) continue;

      char symbol = '\0';

      switch(square->type)
      {
        case SQUARE_LETTER:
          symbol = square->letter;
          break;

        case SQUARE_BLOCK:
          symbol = '#';
          break;

        case SQUARE_EMPTY:
          symbol = '.';
          break;

        case SQUARE_BORDER:
          symbol = 'X';
          break;

        default:
          break;
      }

      if(x < (grid->width - 1))
      {
        fprintf(file, "%c ", symbol);
      }
      else fprintf(file, "%c", symbol);
    }

    fprintf(file, "\n");
  }

  fclose(file);

  return 0;
}
