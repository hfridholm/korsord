/*
 * k-wbase.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_WBASE_H
#define K_WBASE_H

#include <stddef.h>

typedef struct node_t trie_t;

typedef struct wbase_t
{
  trie_t* primary;
  trie_t* backup;
} wbase_t;


extern wbase_t* wbase_create(const char* primary_filepath, const char* backup_filepath);

extern void wbase_free(wbase_t** wbase);


extern int  words_search(char*** words, size_t* count, trie_t* trie, const char* pattern);

extern void words_shuffle(char** words, size_t count);

extern void words_free(char*** words, size_t count);


#endif // K_WBASE_H
