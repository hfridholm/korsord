/*
 *
 */

#include "k-wbase.h"
#include "k-wbase-intern.h"

/*
 * Note: Add smart realloc chunks of powers of 2
 */
static int word_append(char*** words, size_t* count, char* word)
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
  for(size_t index = 0; index < count; index++)
  {
    size_t rand_index = (rand() % count);

    char* temp_word = words[index];

    words[index] = words[rand_index];

    words[rand_index] = temp_word;
  }
}

/*
 * EXPECTS:
 * - words are either allocated or NULL
 * - count is defined as an integer
 *
 * PARAMS
 * - char* word | The letters sience root
 */
static void _words_search(char*** words, size_t* count, node_t* node, const char* pattern, int index, char* word)
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

  _words_search(words, count, (node_t*) trie, pattern, 0, "");

  return 0;
}

