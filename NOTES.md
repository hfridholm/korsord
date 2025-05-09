# Notes

## Quick Fixes
- change all args.REMAINDER to string.split(' ')
- add 'words' to render-gen.py
- add 'words' arg to gen.py, which consists of pre generated words (in priority order)
- add 'model', 'grid', 'clues', 'image' which gets overwritten at each step
- change 'theme' to 'prompt' in gen.py. Theme, image_pos and image_prompt is then extracted from prompt
- don't print "Använd inte följande ord:" om inga ord finns
- Add messages in makefile
- write about the default models
- write about makefile and scripts in binary

## korsord.c
- create korsord.man
- fix grid-gen (korsord.c) file handling
- return proper exit code on fail in korsord.c
- fix file paths in grid-gen (korsord.c)
- README.md in source/ explain the algorithm and refrence function names
- change if-statements to switch statements when checking _status in -gen.c
- Fix problem with "new best grid" skyrocketing
  (I have a fealing that the problem is that singles (1 letter words) don't get restored)
- Delete debug.sh and valgrind.sh

## Words
- only read first word of each line in trie_load in korsord.c
- make it possible to read word files with existing clues (word : clue)

## Grid Prep
- maybe: add a few words from the first word list to the grid in different places
  (this ensures that at least these words from the first word list are in the solution)
  (it maybe also make the generation easier, or harder, I don't know)

## Used Trie
- this is a new field (trie_t* words) in grid_t
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
