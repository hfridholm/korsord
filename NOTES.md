# Notes

## Constant improvements
- write good comments in code explaining things in more detail
- write in README.md

## Quick Fixes
- fix grid-gen (korsord.c) file handling
- return proper exit code on fail in korsord.c
- fix render.py file handling and result storing
- create 'format' command for 'words', that aligns ':' based on all words
- fix file paths in grid-gen (korsord.c)
- fix image position and size in model-gen.py
- get gen.py working
- Add messages in makefile
- README.md in source/ explain the algorithm and refrence function names
- write about the default models
- write about makefile and scripts in binary
- change if-statements to switch statements when checking _status in -gen.c
- Fix problem with "new best grid" skyrocketing
  (I have a fealing that the problem is that singles (1 letter words) don't get restored)
- modify render.py to accept multiple ranked .clues files
  (multiple .clues files to extract and fill the requirement from .grid)

## Potential Quick Fixes
- maybe hard code BASE_DIR in scripts
- rename 'show' command to 'view'
- rename temp. files to maybe 'default' or something similar
- add flags to makefile to silent output
- move lock inside stats struct and lock inside grid struct
- remove old_grid from _reset, instead set is_crossed on SQUARE_BLOCK and from that _reset

## Word Files
- only read first word of each line in model_load in korsord
- make it possible to read word files with existing clues (word : clue)

### Maybe
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

## Commands

korsord gen "Julafton" 15 17 --image "stor bild på en vit ren i högra hörnet"

korsord render my.grid my.clues --image my.png
