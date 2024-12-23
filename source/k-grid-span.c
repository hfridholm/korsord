/*
 * k-grid-span.c - calculate word span (start and stop)
 */

#include "k-grid.h"
#include "k-grid-intern.h"

/*
 * Get start xs of horizontal words
 *
 * RETURN (bool is_blocked)
 * - horizontal word is blocked to the left (start)
 */
bool horiz_start_xs_get(int* start_xs, int* count, grid_t* grid, int cross_x, int cross_y)
{
  bool is_blocked = true;

  for(int start_x = (cross_x + 1); start_x-- > 0;)
  {
    if(xy_square_is_blocking(grid, start_x, cross_y)) break;

    if(start_x < cross_x) is_blocked = false;


    if(start_x > 0)
    {
      if(!block_is_allowed(grid, start_x - 1, cross_y)) continue;
    }

    start_xs[(*count)++] = start_x;
  }

  return is_blocked;
}

/*
 * Get stop xs of horizontal words
 *
 * RETURN (bool is_blocked)
 * - horizontal word is blocked to the right (stop)
 */
bool horiz_stop_xs_get(int* stop_xs, int* count, grid_t* grid, int cross_x, int cross_y)
{
  bool is_blocked = true;

  for(int stop_x = cross_x; stop_x < grid->width; stop_x++)
  {
    if(xy_square_is_blocking(grid, stop_x, cross_y)) break;

    if(stop_x > cross_x) is_blocked = false;


    if(stop_x < (grid->width - 1))
    {
      if(!block_is_allowed(grid, stop_x + 1, cross_y)) continue;
    }

    stop_xs[(*count)++] = stop_x;
  }

  return is_blocked;
}

/*
 * Get horizontal start xs which don't break vertical words
 *
 * These start xs are ONLY used for horiz_gwords_get,
 * otherwise a loop would occour, because
 * horiz_block_brakes_words uses horiz_start_xs_get
 *
 * RETURN (bool is_blocked)
 * - horizontal word is blocked to the left (start)
 */
bool horiz_non_break_start_xs_get(int* start_xs, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  int temp_start_xs[cross_x + 1];
  int temp_count = 0;

  // 1. Get start xs
  if(horiz_start_xs_get(temp_start_xs, &temp_count, grid, cross_x, cross_y))
  {
    memcpy(start_xs, temp_start_xs, sizeof(int) * temp_count);

    *count = temp_count;

    return true; // Is blocked
  }

  // 2. Only keep the start ys that don't break words in two
  for(int index = 0; index < temp_count; index++)
  {
    int start_x = temp_start_xs[index];

    if ((start_x == 0) ||
        !horiz_start_block_brakes_words(wbase, grid, start_x - 1, cross_y))
    {
      start_xs[(*count)++] = start_x;
    }
  }

  return false; // Is not blocked
}

/*
 * Get horizontal stop xs which don't break vertical words
 *
 * These stop xs are ONLY used for horiz_gwords_get,
 * otherwise a loop would occour, because
 * horiz_block_brakes_words uses horiz_stop_xs_get
 *
 * RETURN (bool is_blocked)
 * - horizontal word is blocked to the right (stop)
 */
bool horiz_non_break_stop_xs_get(int* stop_xs, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  int temp_stop_xs[grid->width - cross_x];
  int temp_count = 0;

  // 1. Get stop xs
  if(horiz_stop_xs_get(temp_stop_xs, &temp_count, grid, cross_x, cross_y))
  {
    memcpy(stop_xs, temp_stop_xs, sizeof(int) * temp_count);

    *count = temp_count;

    return true; // Is blocked
  }

  // 2. Only keep the stop xs that don't break words in two
  *count = 0;

  for(int index = 0; index < temp_count; index++)
  {
    int stop_x = temp_stop_xs[index];

    if ((stop_x == (grid->width - 1)) ||
        !horiz_stop_block_brakes_words(wbase, grid, stop_x + 1, cross_y))
    {
      stop_xs[(*count)++] = stop_x;
    }
  }

  return false; // Is not blocked
}

/*
 *
 */
bool vert_start_ys_get(int* start_ys, int* count, grid_t* grid, int cross_x, int cross_y)
{
  bool is_blocked = true;

  for(int start_y = (cross_y + 1); start_y-- > 0;)
  {
    if(xy_square_is_blocking(grid, cross_x, start_y)) break;

    if(start_y < cross_y) is_blocked = false;


    if(start_y > 0)
    {
      if(!block_is_allowed(grid, cross_x, start_y - 1)) continue;
    }

    start_ys[(*count)++] = start_y;
  }

  return is_blocked;
}

/*
 * RETURN (bool is_blocked)
 * - true | It is blocked downwards
 */
bool vert_stop_ys_get(int* stop_ys, int* count, grid_t* grid, int cross_x, int cross_y)
{
  bool is_blocked = true;

  for(int stop_y = cross_y; stop_y < grid->height; stop_y++)
  {
    if(xy_square_is_blocking(grid, cross_x, stop_y)) break;

    if(stop_y > cross_y) is_blocked = false;


    if(stop_y < (grid->height - 1))
    {
      if(!block_is_allowed(grid, cross_x, stop_y + 1)) continue;
    }

    stop_ys[(*count)++] = stop_y;
  }

  return is_blocked;
}

/*
 * Get vertical start ys which don't break horizontal words
 *
 * These start ys are ONLY used for vert_gwords_get,
 * otherwise a loop would occour, because
 * vert_block_brakes_words uses vert_start_ys_get
 */
bool vert_non_break_start_ys_get(int* start_ys, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  int temp_start_ys[cross_y + 1];
  int temp_count = 0;

  // 1. Get start ys
  if(vert_start_ys_get(temp_start_ys, &temp_count, grid, cross_x, cross_y))
  {
    memcpy(start_ys, temp_start_ys, sizeof(int) * temp_count);

    *count = temp_count;

    return true; // Is blocked
  }

  // 2. Only keep the start ys that don't break words in two
  for(int index = 0; index < temp_count; index++)
  {
    int start_y = temp_start_ys[index];

    if ((start_y == 0) ||
        !vert_start_block_brakes_words(wbase, grid, cross_x, start_y - 1))
    {
      start_ys[(*count)++] = start_y;
    }
  }

  return false; // Is not blocked
}

/*
 * Get vertical stop ys which don't break horizontal words
 *
 * These stop ys are ONLY used for vert_gwords_get,
 * otherwise a loop would occour, because
 * vert_block_brakes_words uses vert_stop_ys_get
 */
bool vert_non_break_stop_ys_get(int* stop_ys, int* count, wbase_t* wbase, grid_t* grid, int cross_x, int cross_y)
{
  int temp_stop_ys[grid->height - cross_y];
  int temp_count = 0;

  // 1. Get stop ys
  if(vert_stop_ys_get(temp_stop_ys, &temp_count, grid, cross_x, cross_y))
  {
    memcpy(stop_ys, temp_stop_ys, sizeof(int) * temp_count);

    *count = temp_count;

    return true; // Is blocked
  }

  // 2. Only keep the stop ys that don't break words in two
  *count = 0;

  for(int index = 0; index < temp_count; index++)
  {
    int stop_y = temp_stop_ys[index];

    if ((stop_y == (grid->height - 1)) ||
        !vert_stop_block_brakes_words(wbase, grid, cross_x, stop_y + 1))
    {
      stop_ys[(*count)++] = stop_y;
    }
  }

  return false; // Is not blocked
}
