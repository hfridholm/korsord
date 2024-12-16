/*
 * k-stats-intern.h - internal declarations
 *
 * Written by Hampus Fridholm
 */

#ifndef K_STATS_INTERN_H
#define K_STATS_INTERN_H

#include <stddef.h>
#include <pthread.h>
#include <stdio.h>

typedef struct stats_patt_t
{
  size_t letter;
  size_t trap;
  size_t crowd;
  size_t edge;
  size_t corner;
  size_t block;
  size_t none;
} stats_patt_t;

typedef struct stats_t
{
  stats_patt_t patt;
  size_t test;
} stats_t;

#endif // K_STATS_INTERN_H
