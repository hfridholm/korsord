/*
 * k-grid-best.h - declarations of best grid functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_GRID_BEST_H
#define K_GRID_BEST_H

typedef struct grid_t grid_t;


extern void best_grid_init(void);

extern void best_grid_free(void);

extern void best_grid_set(grid_t* grid);

extern int  best_grid_cross_count_get(void);


extern void best_grid_print(void);

extern void best_grid_ncurses_print(void);

#endif // K_GRID_BEST_H
