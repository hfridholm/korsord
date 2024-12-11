/*
 * getwrds.h - get words that matches a pattern
 */

#ifndef GETWRDS_H
#define GETWRDS_H

#include <stddef.h>

extern int getwrds(char*** words, size_t* count, const char* pattern);

#endif // GETWRDS_H

/*
 *
 */

#ifdef GETWRDS_IMPLEMENT

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ALPHABET_SIZE 29

typedef struct node_t node_t;

typedef struct node_t trie_t;

typedef struct node_t
{
  node_t* children[ALPHABET_SIZE];
  bool    is_end_of_word;
} node_t;

/*
 *
 */
static inline node_t* node_create(void)
{
  node_t* node = malloc(sizeof(node_t));

  node->is_end_of_word = false;

  for(int index = 0; index < ALPHABET_SIZE; index++)
  {
    node->children[index] = NULL;
  }

  return node;
}

#define trie_create node_create

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
}

#define trie_free node_free

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

  return '-';
}

/*
 *
 */
static inline int word_insert(trie_t* trie, const char* word)
{
  node_t* node = (node_t*) trie;

  for(int index = 0; word[index] != '\0'; index++)
  {
    int child_index = letter_index_get(word[index]);

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
static inline void words_free(char*** words, size_t count)
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
 * PARAMS
 * - char* word | The letters sience root
 */
static inline void _words_search(char*** words, size_t* count, node_t* node, char* word, const char* pattern, int index)
{
  // Base case - the end of the word
  if(pattern[index] == '\0')
  {
    word_append(words, count, word);

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

    snprintf(new_word, index + 1, "%.*s%c", index, word, letter);

    _words_search(words, count, child, new_word, pattern, index + 1);
  }
}

/*
 *
 */
int words_search(char*** words, size_t* count, trie_t* trie, const char* pattern)
{
  _words_search(words, count, (node_t*) trie, "", pattern, 0);
}

/*
 * 
 */
int getwrds(char*** words, size_t* count, const char* pattern)
{
  if(!words || !count || !pattern) return 1;

  printf("getwrds\n");

  char* word = strdup("GrOdA");

  printf("Original: %s\n", word);

  string_lower(word);

  printf("Lower: %s\n", word);


  trie_t* trie = trie_create();

  word_insert(trie, word);

  trie_free(&trie);


  free(word);

  return 2;
}

#endif // GETWRDS_IMPLEMENT
