/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define WORDS_IMPLEMENT
#include "words.h"

#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>

bool running = false;

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
  int       cross_count;
  int       word_count;
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

  grid->cross_count = 0;
  grid->word_count = 0;

  for(int index = 0; index < grid->count; index++)
  {
    grid->squares[index] = (square_t)
    {
      .type = TYPE_NONE,
      .letter = '\0',
      .is_crossed = false
    };
  }

  // Set neccessary block squares
  grid->squares[0].type = TYPE_BLOCK;

  grid->squares[width - 1].type = TYPE_BLOCK;

  grid->squares[(height - 1) * width].type = TYPE_BLOCK;

  return grid;
}

/*
 *
 */
void grid_copy(grid_t* copy, grid_t* grid)
{
  if(copy->count != grid->count) return;

  for(int index = 0; index < copy->count; index++)
  {
    copy->squares[index] = grid->squares[index];
  }

  copy->cross_count = grid->cross_count;
  copy->word_count = grid->word_count;
}

/*
 *
 */
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

  dup_grid->cross_count = grid->cross_count;
  dup_grid->word_count = grid->word_count;

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


      if(square.is_crossed)
      {
        printf("\033[32m%c \033[0m", symbol);
      }
      else printf("%c ", symbol);
    }

    printf("\n");
  }

  printf("\n");
}

/*
 *
 */
static int grid_vertical_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, y, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    square_index = (y * grid->width) + start_x;

    if(square_index >= grid->count) break;


    square_t old_square = grid->squares[square_index];

    square_t square =
    {
      .type = TYPE_LETTER,
      .letter = word[index]
    };

    if(old_square.type == TYPE_LETTER)
    {
      grid->cross_count++;
      square.is_crossed = true;
    }
    else is_perfect = false;

    grid->squares[square_index] = square;
  }


  if(is_perfect) return 1;

  
  // Insert block square at end of word
  y = start_y + index;

  square_index = (y * grid->width) + start_x;

  if(y < grid->height && square_index < grid->count)
  {
    grid->squares[square_index].type = TYPE_BLOCK;
  }

  // Insert neibouring block squares at start of word
  if(start_y == 0)
  {
    if((start_x + 1) < grid->width)
    {
      square_index = (start_y * grid->width) + (start_x + 1);

      grid->squares[square_index].type = TYPE_BLOCK;
    }

    square_index = (start_y * grid->width) + (start_x - 1);

    grid->squares[square_index].type = TYPE_BLOCK;
  }

  return 0;
}

/*
 *
 */
static int grid_horizontal_word_insert(grid_t* grid, const char* word, int start_x, int start_y)
{
  bool is_perfect = true;

  int index, x, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    square_index = (start_y * grid->width) + x;

    if(square_index >= grid->count) break;


    square_t old_square = grid->squares[square_index];

    square_t square =
    {
      .type = TYPE_LETTER,
      .letter = word[index]
    };

    if(old_square.type == TYPE_LETTER)
    {
      grid->cross_count++;
      square.is_crossed = true;
    }
    else is_perfect = false;

    grid->squares[square_index] = square;
  }


  if(is_perfect) return 1;

  
  // Insert block square at end of word
  x = start_x + index;

  square_index = (start_y * grid->width) + x;

  if(x < grid->width && square_index < grid->count)
  {
    grid->squares[square_index].type = TYPE_BLOCK;
  }

  // Insert neibouring block squares at start of word
  if(start_x == 0)
  {
    if((start_y + 1) < grid->height)
    {
      square_index = ((start_y + 1) * grid->width) + start_x;

      grid->squares[square_index].type = TYPE_BLOCK;
    }

    square_index = ((start_y - 1) * grid->width) + start_x;

    grid->squares[square_index].type = TYPE_BLOCK;
  }

  return 0;
}

/*
 *
 */
static void grid_horizontal_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, x, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    x = start_x + index;

    square_index = (start_y * grid->width) + x;

    if(square_index >= grid->count) break;


    if(grid->squares[square_index].is_crossed)
    {
      grid->cross_count--;
    }

    grid->squares[square_index] = original->squares[square_index];
  }

  // Reset the block at the end of the word
  x = start_x + index;

  square_index = (start_y * grid->width) + x;

  if(x < grid->width && square_index < grid->count)
  {
    grid->squares[square_index] = original->squares[square_index];
  }
}

/*
 *
 */
static void grid_vertical_word_reset(grid_t* original, grid_t* grid, const char* word, int start_x, int start_y)
{
  // Reset word letters
  int index, y, square_index;

  for(index = 0; word[index] != '\0'; index++)
  {
    y = start_y + index;

    square_index = (y * grid->width) + start_x;

    if(square_index >= grid->count) break;


    if(grid->squares[square_index].is_crossed)
    {
      grid->cross_count--;
    }

    grid->squares[square_index] = original->squares[square_index];
  }

  // Reset the block at the end of the word
  y = start_y + index;

  square_index = (y * grid->width) + start_x;

  if(y < grid->height && square_index < grid->count)
  {
    grid->squares[square_index] = original->squares[square_index];
  }
}

static int grid_vertical_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y);

static int grid_horizontal_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y);

/*
 * Recursive function
 */
static int grid_horizontal_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;

  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == TYPE_BLOCK) return 1;


  if(grid->cross_count > best->cross_count)
  {
    // printf("new best grid: %d\n", grid->cross_count);
    grid_copy(best, grid);
  }


  // Instead of marking this as crossed, don't call _gen with this y
  if(cross_y == 0)
  {
    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return 0;
  }


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

    int block_x = (start_x + length);

    // Here: Reject bad x, y
    if(cross_y >= (grid->height - 2))
    {
      if(block_x == (grid->width - 2) || block_x == (grid->width - 1)) continue;
    }

    // Do not block corner
    if(cross_y == 1)
    {
      if(block_x == (grid->width - 2) || block_x == (grid->width - 1)) continue;
    }
    if(cross_y == 2)
    {
      if(block_x == (grid->width - 1)) continue;
    }

    // Do not block letter
    if(block_x < grid->width)
    {
      square_index = (cross_y * grid->width) + block_x;

      if(grid->squares[square_index].type == TYPE_LETTER) continue;
    }


    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      words_shuffle(words, count);

      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        word_use(trie, word);

        // If the word fits perfect, the crossword is solved?
        if(grid_horizontal_word_insert(new_grid, word, start_x, cross_y) == 1)
        {
          words_free(&words, count);

          grid_free(&new_grid);

          return 0; // Change out these returns
        }

        grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_x = start_x + i;

          square_index = (cross_y * new_grid->width) + next_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          // usleep(1000);

          if(grid_vertical_word_gen(trie, best, new_grid, next_x, cross_y) != 0)
          {
            words_free(&words, count);

            grid_free(&new_grid);

            return 2;
          }
        }

        grid_horizontal_word_reset(grid, new_grid, word, start_x, cross_y);

        word_unuse(trie, word);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 0;
}

/*
 * Recursive function
 */
static int grid_vertical_word_gen(trie_t* trie, grid_t* best, grid_t* grid, int cross_x, int cross_y)
{
  if(!running) return 1;

  int square_index = (cross_y * grid->width) + cross_x;

  square_t square = grid->squares[square_index];

  if(square.type == TYPE_BLOCK) return 1;



  if(grid->cross_count > best->cross_count)
  {
    grid_copy(best, grid);
  }


  // Instead of marking this as crossed, don't call _gen with this x
  if(cross_x == 0)
  {
    grid->squares[square_index].is_crossed = true;

    grid->cross_count++;

    return 0;
  }


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

    int block_y = (start_y + length);

    // Here: Reject bad x, y
    if(cross_x >= (grid->width - 2))
    {
      if(block_y == (grid->height - 2) || block_y == (grid->height - 1)) continue;
    }

    // Do not block corner
    if(cross_x == 1)
    {
      if(block_y == (grid->height - 2) || block_y == (grid->height - 1)) continue;
    }
    if(cross_x == 2)
    {
      if(block_y == (grid->height - 1)) continue;
    }

    // Do not block letter
    if(block_y < grid->height)
    {
      square_index = (block_y * grid->width) + cross_x;

      if(grid->squares[square_index].type == TYPE_LETTER) continue;
    }


    if(words_search(&words, &count, trie, pattern) == 0 && count > 0)
    {
      words_shuffle(words, count);

      for(int index = 0; index < count; index++)
      {
        char* word = words[index];

        word_use(trie, word);

        // If the word fits perfect, the crossword is solved?
        if(grid_vertical_word_insert(new_grid, word, cross_x, start_y) == 1)
        {
          words_free(&words, count);

          grid_free(&new_grid);

          return 0; // Change out these returns
        }

        // grid_print(new_grid);

        for(int i = 0; i < length; i++)
        {
          int next_y = start_y + i;

          square_index = (next_y * new_grid->width) + cross_x;

          if(new_grid->squares[square_index].is_crossed) continue;

          // usleep(1000);

          if(grid_horizontal_word_gen(trie, best, new_grid, cross_x, next_y) != 0)
          {
            words_free(&words, count);

            grid_free(&new_grid);

            return 2;
          }
        }

        grid_vertical_word_reset(grid, new_grid, word, cross_x, start_y);

        word_unuse(trie, word);
      }

      words_free(&words, count);
    }
  }

  grid_free(&new_grid);

  return 0;
}

/*
 *
 */
grid_t* grid_gen(trie_t* trie, int width, int height)
{
  grid_t* grid = grid_create(width, height);

  grid_t* best = grid_dup(grid);

  grid_vertical_word_gen(trie, best, grid, 1, 1);

  grid_free(&grid);

  return best;
}

/*
 *
 */
static void stop_handler(int signum)
{
  printf("Stop program\n");

  running = false;
}

/*
 *
 */
int main(int argc, char* argv[])
{
  signal(SIGINT, stop_handler);

  srand(time(NULL));

  printf("korsord.c\n");

  running = true;

  trie_t* trie = trie_create("words.txt");

  if(!trie)
  {
    perror("trie_create");

    return 1;
  }

  grid_t* grid = grid_gen(trie, 10, 5);

  printf("Generated grid\n");

  grid_print(grid);


  grid_free(&grid);

  trie_free(&trie);

  return 0;
}
