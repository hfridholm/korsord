# Notes

If a letter fails, try removing the letters that is not _crossed and call _gen again for the same direction from _embed.
This keeps the progress that the other letters achieved. Something like this is what caused the bug erlier with blank stripes in grid. But if done correctly, that is not an issue.

- add helping numbers to render.py script (rendered before solved letters)
- refine python words prepare script
- add min_length to wbase_create
- change out the word lists completely
- instead of _used flag, store used words in used_trie.
  (in _search and _exist functions: pass along the current used_trie node along with search node)
  (duplicate used_trie along with grid in _gen functions)
- add prep block procent to argp argument (60% is it now, but change default to something else)
- the same way grid is reverted, wbase should also be reverted
- create EMBED_ status codes and add EMBED_HALF
- call _gen again when EMBED_HALF
- rename old_grid and new_grid better, like embed_grid, test_grid and grid
- get better words for crosswords
- add argp to render.py
- maybe add argp to clues.py
- add blocks near block ridge (2 squares in) in prep stage

- change if-statements to switch statements when checking _status in -gen.c
- remove old_grid from _reset, instead set is_crossed on SQUARE_BLOCK and from that _reset

[result.words]
hus        :
skateboard :
...

[user altered result.words]
hus        : i ett sådant kan man bo
skateboard : platta med fyra hjul
...

maybe (this is highly recommended)
- expand border to 2 in width (search box of 5x5 instead of 3x3)
- remove outer border from anything to the user (model and print)

maybe 6x6
. . . . . .
. . . . . .
. . . . . .
. . . + . .
. . . . . .
. . . . . .

By being able to check 3 squares to the left and top,
start blocks can be properly handled.
The blocks at the left and top edge don't need either 2 blocks to the right or at bottom

These cases are legal

```
. . . . X .
. . . # _ #
. . . . . .
. . . + . .
. . . . . .
. . . . . .

. . . . . .
. # . . . .
X _ . . . .
. # . + . .
. . . . . .
. . . . . .
```

- SQUARE_BLOCK med endast 1 bokstav under och till höger (utan start) ska inte få finnas
. . . . .
. . . . .
. . + . # Om båda blocks är här får en block i mitten inte finnas
. . . . .
. . # . .

. . . . .
. . . . . Om SQUARE_BLOCK eller SQUARE_BORDER är både i område a och b,
. . + a a får inte en block placeras i mitten
. . b . .
. . b . . Detta gör patt_corner_is_allowed obsolete och den kan tas bort


. . . . .
. . . . . Om SQUARE_BLOCK är i a och SQUARE_BLOCK eller SQUARE_BORDER är i område c,
a b + . . eller
c d . . . Om SQUARE_BLOCK är i b och SQUARE_BLOCK eller SQUARE_BORDER är i område d,
c d . . . får inte en block placeras i mitten

. . a c c
. . b d d
. . + . . --||--
. . . . .
. . . . .


maybe in future:
- extract the squares of 5x5 box into a new mini grid,
  that then can be checked mirrored by switching x and y:

  patt_block_is_allowed(5x5, x, y)
  patt_block_is_allowed(5x5, y, x)


idea:
- SQUARE_BLOCK from prep should not be counted for in _crowd function

- maybe add flags to makefile to silent output
- maybe: move lock inside stats struct and lock inside grid struct
- write comments explaining things in more detail
- try implementing the old algorithm and see if that was better

new idea:
- squares that are not _crossed should be able to be changed by new words,
  if the words are still valid and available.
  These kinds of small adjustments prevents unnecessary recursion backtracking.

- maybe: Change start and stop in gword_t to
union
{
  int start_x;
  int start_y;
};

- maybe: store gwords in lookup table with grid hash keys
this way, if the algorithm has to go back,
it don't have to calculate all the allowed words again


When I have implemented non_block xs/ys and refined _word_fits:
- _words_exist_for_pattern takes more and more time


An future idea:

- add random "ligtning", by changing the word when it goes "segt"
  (let the algorithm try another word and see if it breakes through)


new best grid: 868
new best grid: 872
new best grid: 874
new best grid: 878
new best grid: 880
new best grid: 883
new best grid: 887

- Fix problem with "new best grid" skyrocketing
I have a fealing that the problem is that singles (1 letter words) don't get restored

New concept idea:

start n (ex 2) threads of vertical_grid_word_gen / horizontal in grid_gen

Both threads will work on the same grid (shared somehow) and triverse
different parts of the grid.

The upside is that problems in the beginning of one thread can be detected by another thread.

If square_t has fields for which thread and which number of word it belongs to,
the thread that created the word can imidiatly go back and try another word.

in square_t
{
  int thread_id;
  int word_id;
}

If another thread detects a problem with a word, it can try to fix it,
perserving the is_crossed squares by including just them in the pattern.

igelkott:

_gELko_T

__EL___T

Then the thread that created the word don't have to be alerted and backtrace.



Another simpler idea:

Instead of recusivly calling next word_gen function on one letter at a time,
without checking the other letter of the current word:

First, check that at least one word is possible for every letter, before
going through the word's all letters

This is doing more computational work, but prevents obvious bad words from 
being traversed from the first "good" letter
(when the second letter is obviously "bad")
