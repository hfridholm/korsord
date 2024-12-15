# Notes
- add indexes for å ä ö
- maybe: Change start and stop in gword_t to
union
{
  int start_x;
  int start_y;
};


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

New bug:

./korsord -d -s model.txt

X X X X X X X X X X X X X X X X X 
X X X X X X X # # # # b # # f # X 
X X X X X X X # i s o l a t o r X 
X X X X X X X # n y r a k a d # X 
X X X X X X X v s # # d # k e k X 
X X X X X X X # m p # r a # r o X 
X X X X X X X k u # b i s # # m X 
X X X X X X X # s n o k # u p p X 
X X X X X X X # s # f e s t # a X 
X # k # # m # # l # # d e l # # X 
X # l a # i n h a k # o # e k e X 
X b a # m l # v # p # m u t a # X 
X # n j a # p i # m n # v a l t X 
X s k o r e a # s # a h # n # a X 
X # . . . . . . . . . . # d o g X 
X e r a # # t b # a n # j e r g X 
X # e l g # i # a l e # u # t a X 
X X X X X X X X X X X X X X X X X

I don't know why the empty space is there?

This is a snapshot under other generation:

X X X X X X X X X X X X X X X X X 
X X X X X X X # # # # i # # . # X 
X X X X X X X # g r a n # . . . X 
X X X X X X X # a u # g . . . . X 
X X X X X X X m p # . e . . . . X 
X X X X X X X # a v p r o v a # X 
X X X X X X X i n i # # . . . . X 
X X X X X X X # d a t j a # . . X 
X X X X X X X # e t t a # . . . X 
X # . # # . # # # i # k h a t # X 
X # . . . . . . . k . t . . . . X 
X . . . . . . . . a . s l . . . X 
X # . . . . . . . n . t d . . . X 
X . . . . . . . . t . a u . . . X 
X # . . . . . . . # . d l . . . X 
X . . . . . . . . . . g m . . . X 
X # . . . . . . . . . a a . . . X 
X X X X X X X X X X X X X X X X X

It seams like one word has been inserted uncomplete:

h_ldulma

After looking in backup.words, no word even is close to that.

This is the result of that run:

X X X X X X X X X X X X X X X X X 
X X X X X X X # # # # i # # r # X 
X X X X X X X # g r a n # d e j X 
X X X X X X X # a u # g # s n # X 
X X X X X X X m p # g e t # # . X 
X X X X X X X # a v p r o v a # X 
X X X X X X X i n i # # k y s s X 
X X X X X X X # d a t j a # k p X 
X X X X X X X # e t t a # b e y X 
X # v # # p # # # i # o n # # # X 
X # e . . . . . # k u r a # . . X 
X s k o l l i v # a n d # e m u X 
X # # v a k # a g n # # a k # l X 
X k p i # a r # # t i # r o # # X 
X # o s # # y l # # # k a r s k X 
X s e c # k a f f e # o # # u r X 
X # t h a i # v e d y x a # p i X 
X X X X X X X X X X X X X X X X X
