/*
 * k-grid-model.c - import grid from model
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"
#include "k-file.h"

/*
 *
 */
grid_t* grid_model_load(const char* filepath)
{
  if(!filepath) return NULL;

  size_t file_size = file_size_get(filepath);

  char* buffer = malloc(sizeof(char) * (file_size + 1));

  if(file_read(buffer, file_size, filepath) == 0)
  {
    return NULL;
  }

  buffer[file_size] = '\0';


  char* buffer_copy = strdup(buffer);

  int width  = 0;
  int height = 0;

  char* token = strtok(buffer_copy, "\n");

  for(height = 0; token; height++)
  {
    width = MAX(width, (strlen(token) + 1) / 2);

    token = strtok(NULL, "\n");
  }

  if(width < 3 || height < 3)
  {
    free(buffer_copy);
    free(buffer);

    return NULL;
  }

  // Initialize empty grid
  grid_t* grid = grid_create(width - 2, height - 2);


  strcpy(buffer_copy, buffer);

  token = strtok(buffer_copy, "\n");

  for(int y = 0; (y < height) && token; y++)
  {
    // The actual physical width of token
    int curr_width = strlen(token) / 2;

    for(int x = 0; x < width; x++)
    {
      int square_index = (y * width) + x;

      square_t* square = grid->squares + square_index;

      if(x >= curr_width)
      {
        square->type = SQUARE_BORDER;
        continue;
      }

      char symbol = token[x * 2];

      // Choose square type depending on symbol
      switch(symbol)
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
          int letter_index = letter_index_get(symbol);

          if(letter_index != -1)
          {
            square->type = SQUARE_LETTER;
            square->letter = symbol;
            square->is_crossed = false;
          }
          break;
      }
    }

    token = strtok(NULL, "\n");
  }

  free(buffer_copy);
  free(buffer);

  return grid;
}
