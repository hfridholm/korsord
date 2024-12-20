# Notes
- add indexes for å ä ö
- create different targets in makefile (for speed and debug)
- remove grid_is_done
- maybe add flags to makefile to silent output
- optimize _words_search with letter_index == -1 check
- add MAX_CROWD_AMOUNT to argp args
- add MAX_EXIST_AMOUNT to argp args
- maybe: move lock inside stats struct and lock inside grid struct
- Add "output" or "result" argument
  (store the result grid to a file)
- write comments explaining things in more detail

- maybe: Change start and stop in gword_t to
union
{
  int start_x;
  int start_y;
};


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


Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 51.03      1.73     1.73    27400     0.00     0.00  _words_search
 25.96      2.61     0.88   789450     0.00     0.00  _word_exists_for_pattern
  4.57      2.77     0.15 36499347     0.00     0.00  xy_index_get
  2.06      2.83     0.07 30600044     0.00     0.00  xy_real_square_get
  1.92      2.90     0.07 35781044     0.00     0.00  xy_square_get
  1.77      2.96     0.06 21578090     0.00     0.00  xy_real_square_is_block
  1.47      3.01     0.05  1819387     0.00     0.00  patt_crowd_is_allowed
  1.33      3.06     0.04 30600412     0.00     0.00  xy_real_index_get
  0.88      3.08     0.03  4244408     0.00     0.00  block_is_allowed
  0.74      3.11     0.03        2     0.01     0.01  node_free
  0.59      3.13     0.02 25962471     0.00     0.00  index_letter_get
  0.59      3.15     0.02  9689914     0.00     0.00  xy_square_is_blocking
  0.59      3.17     0.02   586947     0.00     0.00  node_create
  0.59      3.19     0.02   542574     0.00     0.00  horiz_word_exists
  0.59      3.21     0.02   504747     0.00     0.00  trie_word_use
  0.59      3.23     0.02   148947     0.00     0.00  string_lower
  0.59      3.25     0.02                             _init
  0.44      3.27     0.01  9019521     0.00     0.00  xy_real_square_is_border
  0.29      3.27     0.01 29716159     0.00     0.00  letter_index_get
  0.29      3.29     0.01  3643006     0.00     0.00  xy_square_is_letter
  0.29      3.29     0.01  1578902     0.00     0.00  word_exists_for_pattern
  0.29      3.31     0.01   543049     0.00     0.00  horiz_start_xs_get
  0.29      3.31     0.01   504663     0.00     0.00  trie_word_unuse
  0.29      3.33     0.01   489033     0.00     0.00  vert_word_insert
  0.29      3.33     0.01   489027     0.00     0.00  vert_word_fits
  0.29      3.35     0.01   488987     0.00     0.00  vert_word_reset
  0.29      3.35     0.01   148947     0.00     0.00  trie_word_insert
  0.29      3.37     0.01    57501     0.00     0.00  vert_stop_ys_get
  0.29      3.38     0.01        9     0.00     0.00  grid_print
  0.15      3.38     0.01    12950     0.00     0.00  xy_square_set_crossed
  0.15      3.38     0.01        2     0.00     0.01  trie_free
  0.15      3.39     0.01                             xy_square_is_border
  0.00      3.39     0.00  3460085     0.00     0.00  xy_square_is_block
