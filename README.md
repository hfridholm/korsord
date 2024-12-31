# Korsord

This project is a **swedish crossword generator**. The word *korsord* means crossword in swedish. A swedish style crossword differs a lot from other kinds of crosswords. In the swedish style, all the words are more dependent of each other. The clues are written directly in the grid in front of the word. Words are both written from left to right or from top to bottom. In the crosswords that this program generates, no squares are left blank, every square is either a clue or a letter of a word.

![korsord.png](assets/example/korsord.png)

Crosswords made with this generator don't need arrows showing where the word should go. Vertical words starting from an edge go to the right of the clue square. Horizontal words starting from an edge go right over the clue square. No words are only one letter, they are at least two letters.

## Usage

Even though this project helps to generate crossword grids, the process of creating a crossword is still very much a lot of work. The process of creating your own crossword can be described as follows:

### 1. [Come up with words of a theme for your crossword](assets/words/README.md)

Every good crossword has some kind of theme. Coming up with many words for that theme is crucial for creating a great crossword. Write down as many words as you can associated with that theme in a list.

When you are done with your words, save them in a `.words` file in the `assets/words/` directory.


### 2. [Pick a suitable crossword model or create your own](assets/models/README.md)

Crosswords can be of many sizes, some are small and others are huge. Think about if you also want a picture inside your crossword and where that should be placed.

When you know what model you want, choose one of the `.model` files in the `assets/models` directory. If none of the existing models are what you are looking for, feel free to create your own.


### 3. [Generate the crossword grid with all the words](binary/README.md#korsord)

When you have choosen all the words you want to exist in your crossword and picked out a suitable model, it's time to generate the grid. This is done with the `korsord` program. When supplying the model and the word lists, you only have to give the path **from** the models directory respectively the words directory.

```bash
korsord [OPTION...] [MODEL] [WORDS...]
```

You can only generate a grid from one model at a time, but you can tell the program to pick words from multiple word lists. The used words will be prioritized based on the inputted order. In the following example, `my.words` will be prioritized over `other.words`.

```bash
korsord my.model my.words other.words
```

But keep in mind, if no words from *my.words* fit in the grid, a word from *other.words* will be used. This is the reason why it is crucial to write down as many words as possible in your own word list, so that the chance of one of your words being picked increases.


### 4. [Write the clues for each word in the grid](binary/README.md#cluespy)

After a grid is generated, the results are temporarly stored. All the words in the grid are stored in `result.words`, each word on seperate lines. Now the hard part, coming up with good clues for each word. 

The clues should be written after the clue-seperator `:` after each word on each line. If you want help with ideas to clues, or just don't bother, you can let ChatGPT write them for you.

```bash
python clues.py [OPTION]
```

This script will ask ChatGPT to generate clues for each word and write them correctly to the *result.words* file. After the AI has written it's answers, you can always edit them or let the AI try one more time, until you're satisfied with the clues.


### 5. [Render the crossword grid with your clues](binary/README.md#renderpy)

When you have generated the crossword grid and written down all the clues, it's time to render the crossword and store it as an image. This is done with the `render.py` script.

```bash
python render.py [OPTION]
```

The rendered images of the crossword will by default be stored in a new folder in the `results` directory:

`normal.png`  Squares are drawn and clues are written before each word

`helping.png` Additional random numbers are written in each letter square, grouping equal letters

`solved.png`  Finally, all the correct letters are filled, showing the solution

![images.png](assets/example/images.png)

### 6. Finish the crossword by pasting your image

![korsord.png](assets/example/korsord.png)
