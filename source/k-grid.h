/*
 * k-grid.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_GRID_H
#define K_GRID_H

typedef struct grid_t grid_t;

typedef struct wbase_t wbase_t;


extern grid_t* grid_gen(wbase_t* wbase, const char* filepath);

extern void grid_free(grid_t** grid);

extern void grid_print(grid_t* grid);

#endif // K_GRID_H
