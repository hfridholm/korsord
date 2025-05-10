/*
 * k-grid.c - basic grid functions
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

#include "k-intern.h"

#include "file.h"

/*
 * Extra SQUARE_BORDER squares are added around the grid
 *
 * X X X X X X X X
 * X X X X X X X X
 * X X X X X X X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X . . . X X
 * X X X X X X X X
 * X X X X X X X X
 */

/*
 * Create crossword grid struct
 *
 * RETURN (grid_t* grid)
 * - NULL | Failed to create grid
 */
grid_t* grid_create(int width, int height)
{
  grid_t* grid = malloc(sizeof(grid_t));

  if(!grid) return NULL;

  grid->width  = width;
  grid->height = height;

  int real_count = (width + 5) * (height + 5);

  grid->squares = malloc(sizeof(square_t) * real_count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  grid->cross_count = 0;

  grid->words = trie_create();

  // Initialize empty squares and border squares
  for(int x = 0; x < (width + 5); x++)
  {
    for(int y = 0; y < (height + 5); y++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

      *square = (square_t)
      {
        .letter     = '\0',
        .is_crossed = false,
        .is_prep    = false
      };

      if ((x >= 3) && (x < (width  + 3)) &&
          (y >= 3) && (y < (height + 3)))
      {
        square->type = SQUARE_EMPTY;
      }
      else
      {
        square->type = SQUARE_BORDER;
      }
    }
  }

  return grid;
}

/*
 * Empty allocated grid struct
 *
 * RETURN (grid_t* grid)
 * - NULL | Bad input
 */
grid_t* grid_clear(grid_t* grid)
{
  if(!grid) return NULL;

  for(int x = 0; x < grid->width; x++)
  {
    for(int y = 0; y < grid->height; y++)
    {
      square_t* square = xy_square_get(grid, x, y);

      *square = (square_t)
      {
        .type = SQUARE_EMPTY,
        .letter = '\0',
        .is_crossed = false
      };
    }
  }

  grid->cross_count = 0;

  return grid;
}

/*
 * Copy crossword grid struct
 * 
 * If grid is NULL copy just gets cleared
 *
 * EXPECT:
 * - copy and grid have the same size
 *
 * RETURN (grid_t* copy)
 * - NULL | Bad input
 */
grid_t* grid_copy(grid_t* copy, grid_t* grid)
{
  if(!copy) return NULL;

  if(!grid) return grid_clear(copy);

  if ((copy->width  != grid->width) ||
      (copy->height != grid->height))
  {
    return NULL;
  }

  int real_count = (grid->width + 5) * (grid->height + 5);

  memcpy(copy->squares, grid->squares, sizeof(square_t) * real_count);

  copy->cross_count = grid->cross_count;

  trie_copy(&copy->words, grid->words);

  return copy;
}

/*
 * Duplicate crossword grid struct
 *
 * RETURN (grid_t* dup)
 * - NULL | Failed to create duplicate
 */
grid_t* grid_dup(grid_t* grid)
{
  if(!grid) return NULL;

  grid_t* dup = malloc(sizeof(grid_t));

  if(!dup) return NULL;

  dup->width  = grid->width;
  dup->height = grid->height;

  int real_count = (grid->width + 5) * (grid->height + 5);

  dup->squares = malloc(sizeof(square_t) * real_count);

  if(!dup->squares)
  {
    free(dup);

    return NULL;
  }

  memcpy(dup->squares, grid->squares, sizeof(square_t) * real_count);

  dup->cross_count = grid->cross_count;

  dup->words = trie_dup(grid->words);

  return dup;
}

/*
 * Free crossword grid struct
 *
 * After the struct is freed, the pointer is set to NULL
 */
void grid_free(grid_t** grid)
{
  if(!grid || !(*grid)) return;

  free((*grid)->squares);

  trie_free(&(*grid)->words);

  free(*grid);

  *grid = NULL;
}

/*
 * Load grid from model
 *
 * RETURN (grid_t* grid)
 */
grid_t* model_load(char* name)
{
  if (!name) return NULL;

  char model_file[1024];

  if (model_file_get(model_file, name) != 0)
  {
    return NULL;
  }

  // 1. Read model file
  size_t file_size = file_size_get(model_file);

  if (file_size == 0)
  {
    return NULL;
  }

  char* buffer = malloc(sizeof(char) * (file_size + 1));

  if (file_read(buffer, file_size, model_file) == 0)
  {
    free(buffer);

    return NULL;
  }

  buffer[file_size] = '\0';


  // 2. Get width and height of model grid
  char* buffer_copy = strdup(buffer);

  int width  = 0;
  int height = 0;

  char* token = strtok(buffer_copy, "\n");

  for (height = 0; token; height++)
  {
    width = MAX(width, (strlen(token) + 1) / 2);

    token = strtok(NULL, "\n");
  }

  if (width < 3 || height < 3)
  {
    free(buffer_copy);
    free(buffer);

    return NULL;
  }

  // 3. Populate empty grid with model squares
  grid_t* grid = grid_create(width, height);

  strcpy(buffer_copy, buffer);

  token = strtok(buffer_copy, "\n");

  for (int y = 0; (y < height) && token; y++)
  {
    int curr_width = strlen(token) / 2;

    for (int x = 0; x < width; x++)
    {
      square_t* square = xy_square_get(grid, x, y);

      char symbol = token[x * 2];

      switch (symbol)
      {
        case 'X':
          square->type = SQUARE_BORDER;
          break;

        case '.':
          square->type = SQUARE_EMPTY;
          break;

        case '#':
          square->type = SQUARE_BLOCK;
          break;

        default:
          if (letter_index_get(symbol) != -1)
          {
            square->type = SQUARE_LETTER;

            square->letter = symbol;
          }
          break;
      }
    }
    token = strtok(NULL, "\n");
  }

  free(buffer_copy);
  free(buffer);

  // Extract words in grid and store them
  char** words = NULL;
  size_t count = 0;

  if (grid_words_get(&words, &count, grid) == 0)
  {
    for (size_t index = 0; index < count; index++)
    {
      trie_word_insert(grid->words, words[index]);
    }

    words_free(&words, count);
  }

  return grid;
}

/*
 * Export words to used words file
 */
static int used_words_export(char** words, size_t count)
{
  char words_file[1024];

  if (words_file_get(words_file, "used") != 0)
  {
    error_print("Failed to get used words file");

    return 1;
  }

  FILE* file = fopen(words_file, "a");

  if (!file)
  {
    error_print("Failed to open: %s", words_file);

    return 2;
  }

  // 4. Print each word formatted on new line
  for (size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    fprintf(file, "%s\n", word);
  }

  fclose(file);

  return 0;
}

/*
 * Export words to clues file
 */
static int clue_words_export(char** words, size_t count, char* name)
{
  // 2. Get max width for alignment
  int max_width = 0;

  for (size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    int curr_width = strlen(word) + 1;

    max_width = MAX(max_width, curr_width);
  }

  char clues_file[1024];

  if (clues_file_get(clues_file, name) != 0)
  {
    error_print("Failed to get clues file: %s", name);

    return 1;
  }

  // 3. Open or create file to write to
  FILE* file = fopen(clues_file, "w");

  if (!file)
  {
    error_print("Failed to open: %s", clues_file);

    return 2;
  }

  // 4. Print each word formatted on new line
  for (size_t index = 0; index < count; index++)
  {
    char* word = words[index];

    int curr_width = max_width - strlen(word);

    fprintf(file, "%s%*c:\n", word, curr_width, ' ');
  }

  fclose(file);

  return 0;
}

/*
 * Export words, both to clues files and used words file
 */
static int words_export(grid_t* grid, char* name)
{
  // 1. Get all words in grid
  char** words = NULL;
  size_t count = 0;

  if(grid_words_get(&words, &count, grid) != 0)
  {
    error_print("Failed to get grid words");

    return 1;
  }

  int status = 0;

  if (clue_words_export(words, count, name) != 0)
  {
    status |= 0b01;
  }

  if (used_words_export(words, count) != 0)
  {
    status |= 0b10;
  }

  words_free(&words, count);

  return status;
}

/*
 * Export grid to file
 */
int grid_export(grid_t* grid, char* name)
{
  if (!grid || !grid->squares)
  {
    error_print("Failed to export: Missing grid");

    return 1;
  }

  char grid_file[1024];

  if (grid_file_get(grid_file, name) != 0)
  {
    return 2;
  }

  FILE* file = fopen(grid_file, "w");

  if (!file)
  {
    error_print("Failed to open file");

    return 3;
  }

  for (int y = 0; y < grid->height; y++)
  {
    for (int x = 0; x < grid->width; x++)
    {
      square_t* square = xy_square_get(grid, x, y);

      if (!square) continue;

      char symbol = '\0';

      switch (square->type)
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

      if (x < (grid->width - 1))
      {
        fprintf(file, "%c ", symbol);
      }
      else fprintf(file, "%c", symbol);
    }

    fprintf(file, "\n");
  }

  fclose(file);

  if (words_export(grid, name) != 0)
  {
    return 4;
  }

  return 0;
}
