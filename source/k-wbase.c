/*
 * k-wbase.c - word base manipulation functions
 */

#include "k-wbase.h"
#include "k-wbase-intern.h"

/*
 * RETURN (char letter)
 * - '_' | Index out of range
 * - 'a' - 'z'
 */
char index_letter_get(int index)
{
  if(index >= 0 && index <= 26)
  {
    return 'a' + index;
  }

  return '_';
}

/*
 * RETURN (int index)
 * - -1  | Letter doesn't exist
 * - min | 0
 * - max | ALPHABET_SIZE - 1
 */
int letter_index_get(char letter)
{
  if(letter >= 'a' && letter <= 'z')
  {
    return letter - 'a';
  }

  return -1;
}

/*
 * Create word base structure wbase
 *
 * PARAMS
 * - char** wfiles  | Word files
 * - size_t count   | Number of word files
 *
 * RETURN (wbase_t* wbase)
 * - NULL | Failed to create wbase
 */
wbase_t* wbase_create(char** wfiles, size_t count)
{
  if (!wfiles || count == 0) return NULL;

  // 1. Allocate memory for wbase
  wbase_t* wbase = malloc(sizeof(wbase_t));

  if(!wbase) return NULL;

  trie_t** tries = malloc(sizeof(trie_t*) * count);

  if(!tries)
  {
    free(wbase);

    return NULL;
  }

  wbase->tries = tries;
  wbase->count = count;

  for(size_t index = 0; index < count; index++)
  {
    wbase->tries[index] = trie_load(wfiles[index]);
  }

  return wbase;
}

/*
 * Free word base struct
 */
void wbase_free(wbase_t** wbase)
{
  if(!wbase || !(*wbase)) return;

  for(size_t index = 0; index < (*wbase)->count; index++)
  {
    trie_free(&(*wbase)->tries[index]);
  }

  free((*wbase)->tries);

  free(*wbase);

  *wbase = NULL;
}
