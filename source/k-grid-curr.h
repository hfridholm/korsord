/*
 * k-grid-curr.h - declarations of current grid functions
 */

#ifndef K_GRID_CURR_H
#define K_GRID_CURR_H

typedef struct grid_t grid_t;


extern void curr_grid_init(void);

extern void curr_grid_free(void);

extern void curr_grid_set(grid_t* grid);


extern void curr_grid_print(void);

extern void curr_grid_ncurses_print(void);

#endif // K_GRID_CURR_H
