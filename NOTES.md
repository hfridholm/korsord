# Notes
- save best grid and "utgå ifrån" that in new search
- refactor the code in _gen functions
- add indexes for å ä ö
- add random "ligtning", by changing the word when it goes "segt"
  (let the algorithm try another word and see if it breakes through)

```
# j # . # v # . . # 
g u l # g a n s k a 
# n e g . p . . . .
```

- prevent empty squares from being boxed in like this
- read pattern file, which is the base for grid

Example:

#####........#
#####.........
#.............
SOCKERVADD#...
..............
#.............

This requires better checking of block squares
As the program is written right now, it only supports rectangular grids
