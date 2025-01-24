/*
 * k-stats.h - debugging statistics
 */

#ifndef K_STATS_H
#define K_STATS_H

extern void stats_init(void);

extern void stats_free(void);

extern void stats_clear(void);


extern void stats_patt_letter_incr(void);

extern void stats_patt_trap_incr(void);

extern void stats_patt_crowd_incr(void);

extern void stats_patt_done_incr(void);

extern void stats_patt_block_incr(void);

extern void stats_patt_none_incr(void);


extern void stats_test_incr(void);


extern void stats_ncurses_print(void);

extern void stats_print(void);

#endif // K_STATS_H
