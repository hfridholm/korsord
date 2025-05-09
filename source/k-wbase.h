/*
 * k-wbase.h - declarations of public functions
 */

#ifndef K_WBASE_H
#define K_WBASE_H

#include <stddef.h>
#include <stdbool.h>

typedef struct node_t trie_t;

typedef struct wbase_t
{
  trie_t** tries;
  size_t   count;
} wbase_t;


extern int  letter_index_get(char letter);

extern char index_letter_get(int index);


extern trie_t* trie_create(void);

extern trie_t* trie_load(char* wfile);

extern void    trie_reset(trie_t* trie);

extern void    trie_free(trie_t** trie);

extern trie_t* trie_dup(trie_t* trie);

extern void    trie_copy(trie_t** copy, trie_t* trie);


extern void trie_word_insert(trie_t* trie, const char* word);

extern void trie_word_remove(trie_t* trie, const char* word);


extern wbase_t* wbase_create(char** wfiles, size_t count);

extern void     wbase_reset(wbase_t* wbase);

extern void     wbase_free(wbase_t** wbase);


extern int  words_search(char*** words, size_t* count, trie_t* trie, trie_t* used_trie, const char* pattern);

extern void words_shuffle(char** words, size_t count);

extern void words_free(char*** words, size_t count);


extern void wbase_word_use(wbase_t* wbase, const char* word);

extern void wbase_word_unuse(wbase_t* wbase, const char* word);


extern int  wbase_words_exist_for_pattern(wbase_t* wbase, trie_t* used_trie, const char* pattern, int max_amount);

extern bool wbase_word_exists_for_pattern(wbase_t* wbase, trie_t* used_trie, const char* pattern);


typedef struct grid_t grid_t;

extern int grid_words_get(char*** words, size_t* count, grid_t* grid);

#endif // K_WBASE_H
