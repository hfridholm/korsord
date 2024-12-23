/*
 * k-wbase.c
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
 * RETURN (wbase_t* wbase)
 * - NULL | Failed to create wbase
 */
wbase_t* wbase_create(const char* primary_filepath, const char* backup_filepath, int max_length)
{
  // 1. Allocate memory for wbase
  wbase_t* wbase = malloc(sizeof(wbase_t));

  if(!wbase) return NULL;

  // 2. Create primary and backup trie
  wbase->primary = trie_create(primary_filepath, max_length);

  wbase->backup = trie_create(backup_filepath, max_length);

  return wbase;
}

/*
 * Free word base structure wbase
 */
void wbase_free(wbase_t** wbase)
{
  if(!wbase || !(*wbase)) return;

  trie_free(&(*wbase)->primary);

  trie_free(&(*wbase)->backup);

  free(*wbase);

  *wbase = NULL;
}

/*
 *
 */
wbase_t* wbase_dup(wbase_t* wbase)
{
  // 1. Allocate memory for wbase
  wbase_t* dup = malloc(sizeof(wbase_t));

  if(!wbase) return NULL;

  // 2. Create primary and backup trie
  dup->primary = trie_dup(wbase->primary);

  dup->backup = trie_dup(wbase->backup);

  return dup;
}

/*
 *
 */
wbase_t* wbase_copy(wbase_t* copy, wbase_t* wbase)
{
  if(wbase->primary)
  {
    trie_copy(copy->primary, wbase->primary);
  }

  if(wbase->backup)
  {
    trie_copy(copy->backup, wbase->backup);
  }

  return copy;
}

/*
 *
 */
void wbase_reset(wbase_t* wbase)
{
  if(wbase->primary)
  {
    trie_reset(wbase->primary);
  }

  if(wbase->backup)
  {
    trie_reset(wbase->backup);
  }
}

/*
 *
 */
void wbase_word_use(wbase_t* wbase, const char* word)
{
  if(wbase->primary)
  {
    trie_word_use(wbase->primary, word);
  }

  if(wbase->backup)
  {
    trie_word_use(wbase->backup, word);
  }
}

/*
 *
 */
void wbase_word_unuse(wbase_t* wbase, const char* word)
{
  if(wbase->primary)
  {
    trie_word_unuse(wbase->primary, word);
  }

  if(wbase->backup)
  {
    trie_word_unuse(wbase->backup, word);
  }
}
