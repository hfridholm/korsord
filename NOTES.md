# Notes

## Constant improvements
- write good comments in code explaining things in more detail
- write in README.md

## Quick Fixes
- remove generate.sh
- add argp to clues.py
- change if-statements to switch statements when checking _status in -gen.c
- Fix problem with "new best grid" skyrocketing
  (I have a fealing that the problem is that singles (1 letter words) don't get restored)

## Potential Quick Fixes
- add flags to makefile to silent output
- move lock inside stats struct and lock inside grid struct
- remove old_grid from _reset, instead set is_crossed on SQUARE_BLOCK and from that _reset

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
