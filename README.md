# Korsord

Swedish crossword generator

## Algorithm

Generate a word vertically and then generate a word horizontally from one of the letters. Then other word is generated vertically from one of the horizontal letters. This process is done until the whole crossword is complete. If the words don't go together, the last inserted words are removed and new ones are tried. This is done by recursive backtracking.

## OpenAI

Install OpenAI python package

```bash
pip install openai
```

Paste OpenAI api key in this file:

```bash
binary/api.key
```

## Debug

See the debug messages of program

```bash
tail -f output.txt
```

Add this line of code to exit program

```bash
kill(0, SIGINT);
```

Allows core dumps

```bash
ulimit -c unlimited
```
