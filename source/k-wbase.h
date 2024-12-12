/*
 * k-wbase.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_WBASE_H
#define K_WBASE_H

#include <stddef.h>

#define ALPHABET_SIZE 29

typedef struct node_t node_t;

typedef struct node_t trie_t;


extern trie_t* trie_create(const char* filepath);

extern void    trie_free(trie_t** trie);


extern int  words_search(char*** words, size_t* count, trie_t* trie, const char* pattern);

extern void words_shuffle(char** words, size_t count);

extern void words_free(char*** words, size_t count);


extern void word_use(trie_t* trie, const char* word);

extern void word_unuse(trie_t* trie, const char* word);

#endif // K_WBASE_H
