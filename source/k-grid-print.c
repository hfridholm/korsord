/*
 * k-grid-print.c - print grid to screen
 *
 * Written by Hampus Fridholm
 */

#include "k-grid.h"
#include "k-grid-intern.h"

#include <ncurses.h>

/*
 * Print crossword grid in ncurses mode
 */
void grid_ncurses_print(grid_t* grid)
{
  if(!grid || !grid->squares) return;

  int h = getmaxy(stdscr);
  int w = getmaxx(stdscr);

  int start_x = MAX(0, (w - (grid->width  + 2) * 2) / 2);
  int start_y = MAX(0, (h - (grid->height + 2)) / 2);

  for(int y = 0; y < (grid->height + 2); y++)
  {
    for(int x = 0; x < (grid->width + 2); x++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

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
 * 
 */
void grid_print(grid_t* grid)
{
  if(!grid || !grid->squares) return;

  for(int y = 0; y < (grid->height + 2); y++)
  {
    for(int x = 0; x < (grid->width + 2); x++)
    {
      square_t* square = xy_real_square_get(grid, x, y);

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
