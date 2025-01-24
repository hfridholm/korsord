# Model

To generate a crossword grid, you need a model to build it from. The model describes the size of the crossword, where the grid squares are located, where images are and where lettes should be placed. Models are represented by characters in a grid formation stored in `.model` files. The different characters represts different types of squares:

`X` Not a square - either not belonging to the crossword or part of an image

`.` Empty square - up to the generator to fill in with either letter or clue

`#` Block square - either one or more clues, or another non-letter square

`a` Letter square - letter, part of a word (must be lowercase)

```txt, my.model
X X # f
X X . i
# . . s
. . . k
```

The grid formation of the model places every row on a new line and every column with one space between each character. The height of the crossword is the amount of rows (lines) and the width of the crossword is the maximum amount of columns (characters). If one line has fewer characters than the widest line, the rest of the squares will be sat to **not a square**. Only characters seperated by a space in between are counted:

```txt, my.model
XX# . f       X # . f
X X . i       X X . i
# . . s  -->  # . . s
. .           . . X X
```

## Default Models

* `kvartetten.model` Four small 9x11 grids around a small image in the middle

* `large1.model` Large 18x23 grid with an image in the top left corner

* `large2.model` Large 18x23 grid with an image in the top right corner

* `small1.model` Small 18x11 grid with an image in the top left corner

* `small2.model` Small 18x11 grid with an image in the top center
