/*
 * k-grid-model.c - import grid from model
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

extern size_t file_read(void* pointer, size_t size, const char* filepath);

extern size_t file_size_get(const char* filepath);

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
    width = MAX(width, strlen(token) / 2);

    token = strtok(NULL, "\n");
  }

  if(width < 3 || height < 3)
  {
    free(buffer_copy);
    free(buffer);

    return NULL;
  }

  grid_t* grid = grid_create(width - 2, height - 2);


  strcpy(buffer_copy, buffer);

  token = strtok(buffer_copy, "\n");

  for(int y = 0; (y < height) && token; y++)
  {
    // The actual physical width of token
    int curr_width = strlen(token) / 2;

    for(int x = 0; x < width; x++)
    {
      int index = (y * width) + x;

      square_t* square = grid->squares + index; 

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
          int index = letter_index_get(symbol);

          if(index != -1)
          {
            square->letter = symbol;
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
