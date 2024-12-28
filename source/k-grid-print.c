/*
 * k-grid-print.c - print grid to screen
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include "k-wbase.h"

#include <ncurses.h>

/*
 * Print crossword grid in ncurses mode
 */
void grid_ncurses_print(grid_t* grid, int start_x, int start_y)
{
  if(!grid || !grid->squares) return;

  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      square_t* square = xy_square_get(grid, x, y);

      if(!square) continue;

      int screen_x = start_x + (x * 2);
      int screen_y = start_y + y;

      switch(square->type)
      {
        case SQUARE_LETTER:
          if(square->is_crossed)
          {
            attron(COLOR_PAIR(1));
            mvprintw(screen_y, screen_x, "%c", square->letter);
            attroff(COLOR_PAIR(1));
          }
          else
          {
            attron(COLOR_PAIR(2));
            mvprintw(screen_y, screen_x, "%c", square->letter);
            attroff(COLOR_PAIR(2));
          }
          break;

        case SQUARE_EMPTY:
          attron(COLOR_PAIR(3));
          mvprintw(screen_y, screen_x, ".");
          attroff(COLOR_PAIR(3));
          break;

        case SQUARE_BLOCK:
          attron(COLOR_PAIR(4));
          mvprintw(screen_y, screen_x, "#");
          attroff(COLOR_PAIR(4));
          break;

        case SQUARE_BORDER:
          attron(COLOR_PAIR(5));
          mvprintw(screen_y, screen_x, "X");
          attroff(COLOR_PAIR(5));
          break;

        default:
          break;
      }
    }
  }
}

/*
 * Print crossword grid in terminal
 */
void grid_print(grid_t* grid)
{
  if(!grid || !grid->squares) return;

  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      square_t* square = xy_square_get(grid, x, y);

      if(!square) continue;

      switch(square->type)
      {
        case SQUARE_LETTER:
          if(square->is_crossed)
          {
            printf("\033[32m%c \033[0m", square->letter);
          }
          else printf("\033[37m%c \033[0m", square->letter);

          break;

        case SQUARE_BLOCK:
          printf("\033[31m# \033[0m");
          break;

        case SQUARE_EMPTY:
          printf("\033[37m. \033[0m");
          break;

        case SQUARE_BORDER:
          printf("\033[35mX \033[0m");
          break;

        default:
          break;
      }
    }
    printf("\n");
  }
  printf("\n");
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
