/*
 * k-wbase.c - get words that matches a pattern
 *
 * Written by Hampus Fridholm
 *
 * FUNCTIONS:
 *
 * trie_t* trie_create(const char* filepath)
 *
 * void    trie_free(trie_t** trie)
 *
 *
 * int     words_search(char*** words, size_t* count, trie_t* trie, const char* pattern)
 *
 * void    words_free(char*** words, size_t count)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "k-wbase.h"

typedef struct node_t
{
  node_t* children[ALPHABET_SIZE];
  bool    is_end_of_word;
  bool    is_used;
} node_t;

/*
 *
 */
static inline node_t* node_create(void)
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
static inline void node_free(node_t** node)
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
static inline int letter_index_get(char letter)
{
  if(letter >= 'a' && letter <= 'z')
  {
    return letter - 'a';
  }

  return -1;
}

/*
 *
 */
static inline char index_letter_get(int index)
{
  if(index >= 0 && index <= 26)
  {
    return 'a' + index;
  }

  return '_';
}

/*
 *
 */
static inline void word_insert(trie_t* trie, const char* word)
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
static inline char* string_lower(char* string)
{
  if(!string) return NULL;

  for(char* letter = string; *letter; letter++)
  {
    *letter = tolower(*letter);
  }

  return string;
}

/*
 *
 */
static inline int word_append(char*** words, size_t* count, char* word)
{
  char** new_words = realloc(*words, sizeof(char*) * (*count + 1));

  if(!new_words) return 1;

  *words = new_words;

  (*words)[(*count)++] = strdup(word);

  return 0;
}

/*
 *
 */
void words_free(char*** words, size_t count)
{
  if(!words || !(*words)) return;

  for(size_t index = 0; index < count; index++)
  {
    free((*words)[index]);
  }

  free(*words);

  *words = NULL;
}

/*
 *
 */
void words_shuffle(char** words, size_t count)
{
  for(int index = 0; index < count; index++)
  {
    int rand_index = (rand() % count);

    char* temp_word = words[index];

    words[index] = words[rand_index];

    words[rand_index] = temp_word;
  }
}

/*
 * PARAMS
 * - char* word | The letters sience root
 */
static inline void _words_search(char*** words, size_t* count, node_t* node, const char* pattern, int index, char* word)
{
  // Base case - the end of the word
  if(pattern[index] == '\0')
  {
    if(node->is_end_of_word && !node->is_used)
    {
      word_append(words, count, word);
    }

    return;
  }

  // Search words with next letter
  int letter_index = letter_index_get(pattern[index]);

  for(int child_index = 0; child_index < ALPHABET_SIZE; child_index++)
  {
    node_t* child = node->children[child_index];

    // Only go through the allocated letters
    if(!child) continue;

    // Possibly, only search letter in pattern
    if(letter_index != -1 && letter_index != child_index) continue; 


    char new_word[index + 1];

    char letter = index_letter_get(child_index);

    snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

    _words_search(words, count, child, pattern, index + 1, new_word);
  }
}

/*
 *
 */
int words_search(char*** words, size_t* count, trie_t* trie, const char* pattern)
{
  if(!words || !count || !trie || !pattern) return 1;

  *words = NULL;
  *count = 0;

  _words_search(words, count, (node_t*) trie, pattern, 0, "");

  return 0;
}

/*
 * Get the size of the file at the supplied path
 *
 * The function returns the number of bytes in the file
 *
 * PARAMS
 * - const char* filepath | Path to file
 *
 * RETURN (size_t size)
 * - 0  | Failed to open file, or file is empty
 * - >0 | Number of bytes in file
 */
static inline size_t file_size_get(const char* filepath)
{
  FILE* stream = fopen(filepath, "rb");

  if(!stream) return 0;

  fseek(stream, 0, SEEK_END); 

  size_t size = ftell(stream);

  fseek(stream, 0, SEEK_SET); 

  fclose(stream);

  return size;
}

/*
 * Read a number of bytes from file to memory at pointer
 *
 * The function returns the number of read bytes
 *
 * PARAMS
 * - void*       pointer  | Pointer to memory buffer
 * - size_t      size     | Number of bytes to read
 * - const char* filepath | Path to file
 *
 * RETURN (same as fread, size_t read_size)
 * - 0  | Failed to read file, or bad input
 * - >0 | Number of read bytes
 */
static inline size_t file_read(void* pointer, size_t size, const char* filepath)
{
  if(!pointer) return 0;

  FILE* stream = fopen(filepath, "rb");

  if(!stream) return 0;

  size_t read_size = fread(pointer, 1, size, stream);

  fclose(stream);

  return read_size;
}

/*
 *
 */
trie_t* trie_create(const char* filepath)
{
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

    word_insert(trie, word);

    free(word);

    token = strtok(NULL, "\n");
  }

  free(buffer);

  return trie;
}

/*
 *
 */
void word_use(trie_t* trie, const char* word)
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
void word_unuse(trie_t* trie, const char* word)
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
