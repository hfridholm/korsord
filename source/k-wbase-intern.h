/*
 * k-wbase-intern.h - intern declarations in k-wbase
 *
 * Written by Hampus Fridholm
 */

#ifndef K_WBASE_INTERN_H
#define K_WBASE_INTERN_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#include "debug.h"

#define ALPHABET_SIZE 29

typedef struct node_t node_t;

typedef struct node_t trie_t;

typedef struct node_t
{
  node_t* children[ALPHABET_SIZE];
  bool    is_end_of_word;
  bool    is_used;
} node_t;


extern trie_t* trie_create(const char* filepath);

extern void    trie_free(trie_t** trie);


extern void trie_word_use(trie_t* trie, const char* word);

extern void trie_word_unuse(trie_t* trie, const char* word);

#endif // K_WBASE_INTERN_H
