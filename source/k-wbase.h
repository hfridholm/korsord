/*
 * k-wbase.h - declarations of public functions
 *
 * Written by Hampus Fridholm
 */

#ifndef K_WBASE_H
#define K_WBASE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct node_t trie_t;

typedef struct wbase_t
{
  trie_t* primary;
  trie_t* backup;
} wbase_t;


extern int  letter_index_get(char letter);

extern char index_letter_get(int index);


extern wbase_t* wbase_create(const char* primary_filepath, const char* backup_filepath, int max_length);

extern void     wbase_reset(wbase_t* wbase);

extern void     wbase_free(wbase_t** wbase);


extern int  words_search(char*** words, size_t* count, trie_t* trie, const char* pattern);

extern void words_shuffle(char** words, size_t count);

extern void words_free(char*** words, size_t count);


extern void wbase_word_use(wbase_t* wbase, const char* word);

extern void wbase_word_unuse(wbase_t* wbase, const char* word);


extern int  wbase_words_exist_for_pattern(wbase_t* wbase, const char* pattern, int max_amount);

extern bool wbase_word_exists_for_pattern(wbase_t* wbase, const char* pattern);

#endif // K_WBASE_H
