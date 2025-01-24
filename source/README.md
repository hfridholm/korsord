## Algorithm

1. Generate a word vertically 
2. Generate a word horizontally from one of the letters. 
3. Another word is generated vertically from one of the horizontal letters.

This process is done until the whole crossword is complete. If the words don't go together, the last inserted words are removed and new ones are tried. This is done by recursive backtracking.
