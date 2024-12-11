/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define WORDS_IMPLEMENT
#include "words.h"

#include <stdio.h>

/*
 *
 */
int main(int argc, char* argv[])
{
  printf("korsord.c\n");

  trie_t* trie = trie_create("words.txt");

  if(!trie)
  {
    perror("trie_create");

    return 1;
  }

  char* pattern = "h l ";

  char** words;
  size_t count;

  if(words_search(&words, &count, trie, pattern) == 0)
  {
    for(size_t index = 0; index < count; index++)
    {
      printf("#%02ld: %s\n", index + 1, words[index]);
    }

    words_free(&words, count);
  }

  trie_free(&trie);

  return 0;
}
