/*
 *
 */

#include "k-stats.h"
#include "k-stats-intern.h"

pthread_mutex_t stats_lock;

stats_t stats;

/*
 *
 */
void stats_init(void)
{
  stats = (stats_t) { 0 };

  pthread_mutex_init(&stats_lock, NULL);
}

/*
 *
 */
void stats_free(void)
{
  stats = (stats_t) { 0 };

  pthread_mutex_destroy(&stats_lock);
}

/*
 *
 */
void stats_clear(void)
{
  pthread_mutex_lock(&stats_lock);

  stats = (stats_t) { 0 };

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_letter_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.letter++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_trap_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.trap++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_crowd_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.crowd++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_edge_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.edge++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_corner_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.corner++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_block_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.block++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_none_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.patt.none++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_patt_test_incr(void)
{
  pthread_mutex_lock(&stats_lock);

  stats.test++;

  pthread_mutex_unlock(&stats_lock);
}

/*
 *
 */
void stats_print(void)
{
  pthread_mutex_lock(&stats_lock);

  printf("letter: %ld\n", stats.patt.letter);
  printf("trap  : %ld\n", stats.patt.trap);
  printf("crowd : %ld\n", stats.patt.crowd);
  printf("edge  : %ld\n", stats.patt.edge);
  printf("corner: %ld\n", stats.patt.corner);
  printf("block : %ld\n", stats.patt.block);
  printf("none  : %ld\n", stats.patt.none);
  printf("test  : %ld\n", stats.test);

  pthread_mutex_unlock(&stats_lock);
}
