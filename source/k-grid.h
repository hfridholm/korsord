/*
 * k-grid.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_GRID_H
#define K_GRID_H

typedef struct node_t trie_t;

typedef struct grid_t grid_t;


extern int grid_vertical_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y);

extern int grid_horizontal_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y);


extern grid_t* grid_gen(trie_t* trie, int width, int height);

extern void grid_free(grid_t** grid);

extern void grid_print(grid_t* grid);

#endif // K_GRID_H
