# Notes
- add indexes for å ä ö

An future idea:

- add random "ligtning", by changing the word when it goes "segt"
  (let the algorithm try another word and see if it breakes through)

I don't know if this is true anymore:

- something is wrong: when I run the program multiple times, I get different best grids
  if the algorithm really checks every combination, I would get the same best every time.


Memory allocations

memory should be allocated in large chunks
The size of the allocated memory should be larger than count
It can be allcoated in powers of 2 (2, 4, 8, 16).
When to realloc: when count is at one of these powers of 2.


- Prevent words from stacking SQUARE_BLOCk after each other
  The following horizontal word would stack block squares:

  #_HELLO_ -> ##HELLO#

- add block_count to grid_t
  (when block_count is large based on square_count,
   that is an indicator that something must change)

new best grid: 868
new best grid: 872
new best grid: 874
new best grid: 878
new best grid: 880
new best grid: 883
new best grid: 887

Something real weird is happening...

- Fix problem with "new best grid" skyrocketing
- Add "output" or "result" argument
  (store the result grid to a file)
- remove outer border from model.txt
  (don't show the border to the user)
  (it is an internal thing that shold not be public)
- create input thread routine, for:
  - refreshing grid
  - aborting search
- write comments explaining things in more detail
- maybe: write if statements:

if something_is_true()
{

}

or

if (something_is_true())
{

}

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


- fix problem with skyrocketing block_count
I have a fealing that the problem is that singles (1 letter words) don't get restored
- maybe: remove best_grid
