/*
 * k-stats.h - debugging statistics
 *
 * Written by Hampus Fridholm
 */

#ifndef K_STATS_H
#define K_STATS_H

extern void stats_init(void);

extern void stats_free(void);

extern void stats_print(void);

extern void stats_clear(void);

extern void stats_patt_letter_incr(void);

extern void stats_patt_trap_incr(void);

extern void stats_patt_crowd_incr(void);

extern void stats_patt_edge_incr(void);

extern void stats_patt_corner_incr(void);

extern void stats_patt_block_incr(void);

extern void stats_patt_none_incr(void);

extern void stats_patt_test_incr(void);

#endif // K_STATS_H
