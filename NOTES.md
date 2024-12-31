# Notes

## Constant improvements
- write good comments in code explaining things in more detail
- write in README.md

## Quick Fixes
- README.md in source/ explain the algorithm and refrence function names
- only extract and save to result.grid when successful
- remove set_core function in korsord.c
- try using different font, and strive to add multiple fonts to assets/fonts/
- write about the default models
- write about makefile and scripts in binary
- add argp to clues.py
- change if-statements to switch statements when checking _status in -gen.c
- Fix problem with "new best grid" skyrocketing
  (I have a fealing that the problem is that singles (1 letter words) don't get restored)
- don't check start/stop xs/ys longer than max_length

## Potential Quick Fixes
- add flags to makefile to silent output
- move lock inside stats struct and lock inside grid struct
- remove old_grid from _reset, instead set is_crossed on SQUARE_BLOCK and from that _reset

## Word Files
- only read first word of each line in model_load in korsord
- make it possible to read word files with existing clues (word : clue)
- make it possible to store multiple clues in one words file
* collect clues in large words file, where old clues can be reused
  (when rendering, it doesn't matter that other not used words and clues are in the file)

## Grid Prep
- maybe: add a few words from the first word list to the grid in different places
  (this ensures that at least these words from the first word list are in the solution)
  (it maybe also make the generation easier, or harder, I don't know)

## Used Trie
- create new variable (trie_t* used_words) that is being passed on in _gen functions
- replace the old _word_use functions with inserting the word in used_words trie
- pass along current used_words node in _search and _exist functions
- store a temporary copy of used_words along side the temporary copy of grid in _gen
- remove _used flag from trie node

## Gen Half
- return new status code GEN_HALF when _word_embed partially succeeds

When GEN_HALF is returned by _word_embed:
1. Remove all non _crossed letters from current word
2. Call _word_gen for the same direction

This tries to keep the progress that the other successful letters achieved.
