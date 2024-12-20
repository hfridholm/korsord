/*
 *
 */

#include "k-wbase.h"
#include "k-wbase-intern.h"

// __builtin_clz counts the leading zeros, so the bit length is:
#define CAPACITY(n) (1 << (sizeof(n) * 8 - __builtin_clz(n)))

/*
 *
 */
static int word_append(char*** words, size_t* count, char* word)
{
  if(*count == 0 || ((*count) + 1) >= CAPACITY(*count))
  {
    char** new_words = realloc(*words, sizeof(char*) * CAPACITY((*count) + 1));

    if(!new_words) return 1;

    *words = new_words;
  }

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

  if(letter_index != -1)
  {
    node_t* child = node->children[letter_index];

    // If no words have the letter, abort
    if(!child) return;

    char new_word[index + 2];

    char letter = pattern[index];

    snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

    _words_search(words, count, child, pattern, index + 1, new_word);
  }
  else
  {
    for(int child_index = 0; child_index < ALPHABET_SIZE; child_index++)
    {
      node_t* child = node->children[child_index];

      // Only go through the allocated letters
      if(!child) continue;


      char new_word[index + 2];

      char letter = index_letter_get(child_index);

      snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

      _words_search(words, count, child, pattern, index + 1, new_word);
    }
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

/*
 * RETURN (int amount)
 */
static int _words_exist_for_pattern(node_t* node, const char* pattern, int index, char* word, int max_amount)
{
  // Base case - the end of the word
  if(pattern[index] == '\0')
  {
    // This evaluates to 0 if false and 1 if true
    // which represents that 'a' word exist
    return (node->is_end_of_word && !node->is_used);
  }

  // Search words with next letter
  int letter_index = letter_index_get(pattern[index]);

  int amount = 0;

  if(letter_index != -1)
  {
    node_t* child = node->children[letter_index];

    // If no words have the letter, amount 0 is returned
    if(!child) return 0;

    char new_word[index + 2];

    char letter = pattern[index];

    snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

    amount = _words_exist_for_pattern(child, pattern, index + 1, new_word, max_amount);
  }
  else
  {
    for(int child_index = 0; child_index < ALPHABET_SIZE; child_index++)
    {
      node_t* child = node->children[child_index];

      // Only go through the allocated letters
      if(!child) continue;


      char new_word[index + 2];

      char letter = index_letter_get(child_index);

      snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

      // max_amount - amount means that the next node
      // only get to search the REST of max_amount
      amount += _words_exist_for_pattern(child, pattern, index + 1, new_word, max_amount - amount);

      // This is opimization only for performance
      if(amount >= max_amount) break;
    }
  }

  return MIN(amount, max_amount);
}

/*
 * RETURN (int amount)
 */
static int words_exist_for_pattern(trie_t* trie, const char* pattern, int max_amount)
{
  if(!trie || !pattern) return 0;

  return _words_exist_for_pattern((node_t*) trie, pattern, 0, "", max_amount);
}

/*
 * RETURN (int amount)
 */
int wbase_words_exist_for_pattern(wbase_t* wbase, const char* pattern, int max_amount)
{
  int amount = 0;

  amount += words_exist_for_pattern(wbase->primary, pattern, max_amount - amount);

  if(amount >= max_amount) return max_amount;

  amount += words_exist_for_pattern(wbase->backup,  pattern, max_amount - amount);

  return MIN(amount, max_amount);
}

/*
 * RETURN (bool does_exist)
 */
static bool _word_exists_for_pattern(node_t* node, const char* pattern, int index, char* word)
{
  // Base case - the end of the word
  if(pattern[index] == '\0')
  {
    // return (node->is_end_of_word && !node->is_used);
    return node->is_end_of_word;
  }

  // Search words with next letter
  int letter_index = letter_index_get(pattern[index]);

  if(letter_index != -1)
  {
    node_t* child = node->children[letter_index];

    // If no words have the letter, amount 0 is returned
    if(!child) return false;

    char new_word[index + 2];

    char letter = pattern[index];

    snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

    return _word_exists_for_pattern(child, pattern, index + 1, new_word);
  }
  else
  {
    for(int child_index = 0; child_index < ALPHABET_SIZE; child_index++)
    {
      node_t* child = node->children[child_index];

      // Only go through the allocated letters
      if(!child) continue;


      char new_word[index + 2];

      char letter = index_letter_get(child_index);

      snprintf(new_word, index + 2, "%.*s%c", index, word, letter);

      if(_word_exists_for_pattern(child, pattern, index + 1, new_word))
      {
        return true;
      }
    }
  }

  return false;
}

/*
 * RETURN (bool does_exist)
 */
static bool word_exists_for_pattern(trie_t* trie, const char* pattern)
{
  if(!trie || !pattern) return false;

  return _word_exists_for_pattern((node_t*) trie, pattern, 0, "");
}

/*
 * RETURN (bool does_exist)
 */
bool wbase_word_exists_for_pattern(wbase_t* wbase, const char* pattern)
{
  if(word_exists_for_pattern(wbase->primary, pattern))
  {
    return true;
  }

  if(word_exists_for_pattern(wbase->backup, pattern))
  {
    return true;
  }

  return false;
}
