/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define WORDS_IMPLEMENT
#include "words.h"

#include <stdio.h>
#include <time.h>
#include <unistd.h>

typedef enum
{
  TYPE_LETTER,
  TYPE_BLOCK,
  TYPE_NONE
} type_t;

typedef struct
{
  type_t type;
  char   letter;
  bool   is_crossed;
} square_t;

typedef struct
{
  int       width;
  int       height;
  square_t* squares;
  int       count;
} grid_t;

/*
 *
 */
grid_t* grid_create(int width, int height)
{
  grid_t* grid = malloc(sizeof(grid_t));

  if(!grid) return NULL;

  grid->width  = width;
  grid->height = height;

  grid->count = (width * height);

  grid->squares = malloc(sizeof(square_t) * grid->count);

  if(!grid->squares)
  {
    free(grid);

    return NULL;
  }

  for(int index = 0; index < grid->count; index++)
  {
    grid->squares[index] = (square_t)
    {
      .type = TYPE_NONE,
      .letter = '\0',
      .is_crossed = false
    };
  }

  grid->squares[0].type = TYPE_BLOCK;

  return grid;
}

grid_t* grid_dup(grid_t* grid)
{
  grid_t* dup_grid = malloc(sizeof(grid_t));

  if(!dup_grid) return NULL;

  dup_grid->width  = grid->width;
  dup_grid->height = grid->height;

  dup_grid->count = grid->count;

  dup_grid->squares = malloc(sizeof(square_t) * dup_grid->count);

  if(!dup_grid->squares)
  {
    free(dup_grid);

    return NULL;
  }

  for(int index = 0; index < dup_grid->count; index++)
  {
    dup_grid->squares[index] = grid->squares[index];
  }

  return dup_grid;
}

/*
 *
 */
void grid_free(grid_t** grid)
{
  if(!grid || !(*grid)) return;

  free((*grid)->squares);

  free(*grid);

  *grid = NULL;
}

void grid_print(grid_t* grid)
{
  for(int y = 0; y < grid->height; y++)
  {
    for(int x = 0; x < grid->width; x++)
    {
      int index = (y * grid->width) + x;

      square_t square = grid->squares[index];

      char symbol = (square.type == TYPE_NONE ? '.' : square.type == TYPE_BLOCK ? '#' : square.letter);

      printf("%c", symbol);
    }

    printf("\n");
  }
}

/*
 *
 */
static void grid_word_insert(grid_t* grid, const char* word, int start_x, int start_y, bool vertical)
{
  int x = start_x;
  int y = start_y;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int square_index = (y * grid->width) + x;

    if(square_index >= grid->count) break;


    square_t old_square = grid->squares[square_index];

    square_t square =
    {
      .type = TYPE_LETTER,
      .letter = word[index],
      .is_crossed = old_square.type == TYPE_LETTER
    };

    grid->squares[square_index] = square;

    x = vertical ? x : x + 1;
    y = vertical ? y + 1 : y;
  }
}

/*
 *
 */
static void grid_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y, bool vertical)
{
  int x = start_x;
  int y = start_y;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int square_index = (y * grid->width) + x;

    if(square_index >= grid->count) break;

    grid->squares[square_index] = original->squares[square_index];

    x = vertical ? x : x + 1;
    y = vertical ? y + 1 : y;
  }
}

static int grid_vertical_word_gen(trie_t* trie, grid_t* grid, int cross_x, int cross_y);

static int grid_horizontal_word_gen(trie_t* trie, grid_t* grid, int cross_x, int cross_y);

/*
 * Recursive function
 */
static int grid_horizontal_word_gen(trie_t* trie, grid_t* grid, int cross_x, int cross_y)
{
  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == TYPE_BLOCK) return 1;


  int start_x;

  for(start_x = (cross_x + 1); start_x-- > 0;)
  {
    square_index = (cross_y * grid->width) + start_x;

    square = grid->squares[square_index];

    if(square.type == TYPE_BLOCK) break;
  }

  start_x++;

  int stop_x;

  for(stop_x = cross_x; stop_x < grid->width; stop_x++)
  {
    square_index = (cross_y * grid->width) + stop_x;

    square = grid->squares[square_index];

    if(square.type == TYPE_BLOCK) break;
  }

  stop_x--;

  int max_length = (1 + stop_x - start_x);


  char pattern[max_length + 1];

  for(int index = max_length; index-- > 0;)
  {
    int x = (start_x + index);

    square_index = (cross_y * grid->width) + x;

    square = grid->squares[square_index];

    if(square.type == TYPE_LETTER)
    {
      pattern[index] = square.letter;
    }
    else pattern[index] = '_';
  }


  grid_t* new_grid = grid_dup(grid);

  char** words;
  size_t count;

  for(int length = (max_length + 1); length-- > 2;)
  {
    pattern[length] = '\0';

    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        grid_word_insert(new_grid, word, start_x, cross_y, false);

        grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_x = start_x + i;

          square_index = (cross_y * new_grid->width) + next_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          usleep(1000000);

          grid_vertical_word_gen(trie, new_grid, next_x, cross_y);
        }

        grid_word_reset(grid, new_grid, word, start_x, cross_y, false);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 1;
}

/*
 * Recursive function
 */
static int grid_vertical_word_gen(trie_t* trie, grid_t* grid, int cross_x, int cross_y)
{
  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == TYPE_BLOCK) return 1;


  int start_y;

  for(start_y = (cross_y + 1); start_y-- > 0;)
  {
    square_index = (start_y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == TYPE_BLOCK) break;
  }

  start_y++;

  int stop_y;

  for(stop_y = cross_y; stop_y < grid->height; stop_y++)
  {
    square_index = (stop_y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == TYPE_BLOCK) break;
  }

  stop_y--;

  int max_length = (1 + stop_y - start_y);


  char pattern[max_length + 1];

  for(int index = max_length; index-- > 0;)
  {
    int y = (start_y + index);

    square_index = (y * grid->width) + cross_x;

    square = grid->squares[square_index];

    if(square.type == TYPE_LETTER)
    {
      pattern[index] = square.letter;
    }
    else pattern[index] = '_';
  }


  grid_t* new_grid = grid_dup(grid);

  char** words;
  size_t count;

  for(int length = (max_length + 1); length-- > 2;)
  {
    pattern[length] = '\0';

    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        grid_word_insert(new_grid, word, cross_x, start_y, true);

        grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_y = start_y + i;

          square_index = (next_y * new_grid->width) + cross_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          usleep(1000000);

          grid_horizontal_word_gen(trie, new_grid, cross_x, next_y);
        }

        grid_word_reset(grid, new_grid, word, cross_x, start_y, true);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 1;
}

/*
 *
 */
int main(int argc, char* argv[])
{
  srand(time(NULL));

  printf("korsord.c\n");

  trie_t* trie = trie_create("words.txt");

  if(!trie)
  {
    perror("trie_create");

    return 1;
  }

  grid_t* grid = grid_create(5, 5);


  grid_vertical_word_gen(trie, grid, 1, 1);


  grid_free(&grid);

  trie_free(&trie);

  return 0;
}
