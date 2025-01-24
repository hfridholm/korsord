/*
 * k-grid-span.h - calculate word span (start and stop)
 */

#ifndef K_GRID_SPAN_H
#define K_GRID_SPAN_H

#include <stdbool.h>

typedef struct grid_t grid_t;


extern bool vert_start_ys_get(int* start_ys, int* count, grid_t* grid, int cross_x, int cross_y);

extern bool vert_stop_ys_get(int* stop_ys, int* count, grid_t* grid, int cross_x, int cross_y);

extern bool horiz_start_xs_get(int* start_xs, int* count, grid_t* grid, int cross_x, int cross_y);

extern bool horiz_stop_xs_get(int* stop_xs, int* count, grid_t* grid, int cross_x, int cross_y);


extern bool vert_non_break_start_ys_get(int* start_ys, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern bool vert_non_break_stop_ys_get(int* stop_ys, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern bool horiz_non_break_start_xs_get(int* start_xs, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

extern bool horiz_non_break_stop_xs_get(int* stop_xs, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y);

#endif // K_GRID_SPAN_H
