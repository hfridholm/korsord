/*
 * k-grid.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_GRID_H
#define K_GRID_H

#include <stdbool.h>
#include <stddef.h>

typedef struct stats_patt_t
{
  size_t letter_count;
  size_t trap_count;
  size_t crowd_count;
  size_t edge_count;
  size_t corner_count;
  size_t block_count;
  size_t none_count;
} stats_patt_t;

typedef struct stats_t
{
  stats_patt_t patt;
  size_t test_count;
} stats_t;

typedef struct grid_t grid_t;

typedef struct wbase_t wbase_t;


extern grid_t* grid_gen(wbase_t* wbase, const char* filepath);

extern void grid_free(grid_t** grid);

extern void grid_print(grid_t* grid);

extern bool grid_is_done(grid_t* grid);

#endif // K_GRID_H
