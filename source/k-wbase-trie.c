/*
 * k-wbase-trie.c - get words that matches a pattern
 *
 * Written by Hampus Fridholm
 *
 * FUNCTIONS:
 *
 * trie_t* trie_create(const char* filepath, int max_length)
 *
 * void    trie_free(trie_t** trie)
 */

#include "k-wbase.h"
#include "k-wbase-intern.h"

#include "k-file.h"

/*
 *
 */
static node_t* node_create(void)
{
  node_t* node = malloc(sizeof(node_t));

  node->is_end_of_word = false;
  node->is_used = false;

  for(int index = 0; index < ALPHABET_SIZE; index++)
  {
    node->children[index] = NULL;
  }

  return node;
}

/*
 *
 */
static void node_free(node_t** node)
{
  if(!node || !(*node)) return;

  for(int index = 0; index < ALPHABET_SIZE; index++)
  {
    node_free((*node)->children + index);
  }

  free(*node);

  *node = NULL;
}

void trie_free(trie_t** trie) { node_free((node_t**) trie); }

/*
 *
 */
static void trie_word_insert(trie_t* trie, const char* word)
{
  node_t* node = (node_t*) trie;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int child_index = letter_index_get(word[index]);

    if(child_index == -1) return;


    if(!node->children[child_index])
    {
      node->children[child_index] = node_create();
    }

    node = node->children[child_index];
  }

  node->is_end_of_word = true;
}

/*
 *
 */
static char* string_lower(char* string)
{
  if(!string) return NULL;

  for(char* letter = string; *letter; letter++)
  {
    *letter = tolower(*letter);
  }

  return string;
}

/*
 * Create trie struct
 *
 * RETURN (trie_t* trie)
 * - NULL | Failed to read file
 */
trie_t* trie_create(const char* filepath, int max_length)
{
  if(!filepath) return NULL;

  size_t file_size = file_size_get(filepath);

  char* buffer = malloc(sizeof(char) * (file_size + 1));

  if(file_read(buffer, file_size, filepath) == 0)
  {
    return NULL;
  }

  buffer[file_size] = '\0';


  trie_t* trie = node_create();

  char* token = strtok(buffer, "\n");

  while(token)
  {
    char* word = string_lower(strdup(token));

    if(strlen(word) <= max_length)
    {
      trie_word_insert(trie, word);
    }

    free(word);

    token = strtok(NULL, "\n");
  }

  free(buffer);

  return trie;
}

/*
 *
 */
void trie_word_use(trie_t* trie, const char* word)
{
  node_t* node = (node_t*) trie;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int child_index = letter_index_get(word[index]);

    if(child_index == -1) return;


    if(!node->children[child_index])
    {
      node = NULL;
      break;
    }

    node = node->children[child_index];
  }

  if(node) node->is_used = true;
}

/*
 *
 */
void trie_word_unuse(trie_t* trie, const char* word)
{
  node_t* node = (node_t*) trie;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int child_index = letter_index_get(word[index]);

    if(child_index == -1) return;


    if(!node->children[child_index])
    {
      node = NULL;
      break;
    }

    node = node->children[child_index];
  }

  if(node) node->is_used = false;
}

/*
 *
 */
static void node_reset(node_t* node)
{
  if(!node) return;

  for(int index = 0; index < ALPHABET_SIZE; index++)
  {
    node_reset(node->children[index]);
  }

  node->is_used = false;
}

void trie_reset(trie_t* trie) { node_reset((node_t*) trie); }
