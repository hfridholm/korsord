/*
 * k-grid.h - declarations of public functions
 */

#ifndef K_GRID_H
#define K_GRID_H

#include <stdbool.h>
#include <stddef.h>

/*
 * This is a flag that is exposed to the user
 *
 * Maybe in future: add mutex locks for safty
 */
extern bool is_generating;


typedef struct grid_t grid_t;

typedef struct wbase_t wbase_t;


extern grid_t* grid_gen(wbase_t* wbase, char* filepath);

extern void    grid_free(grid_t** grid);


extern int grid_words_export(grid_t* grid);

extern int used_words_export(grid_t* grid);

extern int grid_export(grid_t* grid);

#endif // K_GRID_H
