/*
 * korsord - swedish crossword generator
 *
 * Written by Hampus Fridholm
 */

#define GETWRDS_IMPLEMENT
#include "getwrds.h"

#include <stdio.h>

/*
 *
 */
int main(int argc, char* argv[])
{
  printf("korsord.c\n");

  char* pattern = "H L ";

  char** words;
  size_t count;

  if(getwrds(&words, &count, pattern) == 0)
  {
    for(size_t index = 0; index < count; index++)
    {
      printf("#%02ld: %s\n", index + 1, words[index]);
    }
  }

  return 0;
}
