/*
 * k-grid-print.c - print grid to screen
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * 
 */
void grid_print(grid_t* grid)
{
  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      int index = (y * grid->width) + x;

      square_t square = grid->squares[index];

      switch(square.type)
      {
        case SQUARE_LETTER:
          if(square.is_crossed)
          {
            printf("\033[32m%c \033[0m", square.letter);
          }
          else printf("%c ", square.letter);

          break;

        case SQUARE_BLOCK:
          printf("\033[31m# \033[0m");
          break;

        case SQUARE_EMPTY:
          printf("\033[37m. \033[0m");
          break;

        default:
          break;
      }
    }
    printf("\n");
  }
  printf("\n");
}
