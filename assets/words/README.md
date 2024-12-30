# Words

To be able to generate a good crossword, you need a lot of different words that the computer can choose from. These words are stored in `.words` files with each word on a seperate line. When creating your own words file, you will write your words like shown:

```txt, my.words
...
saffran
choklad
polkagris
...
```

It doesn't matter if you happens to write the same word twice, or if you write words containing characters that are not allowed. This is because there exist a script that filters your word list, only keeping the allowed words and removing the bad ones. To filter a words file, just run this command:

```bash
python ordlista.py [OPTION]
```

## Allowed words

For now, the words that are allowed, are words containing only ascii alphabet letters. This **includes** every lowercase letter `a - z` and every uppercase letter `A - Z`. Unfortunately, this **excludes** the swedish vowels `å` `ä` and `ö`. This is a real bummer, because many swedish words depend on these letters.
