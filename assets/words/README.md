# Words

To be able to generate a good crossword, you need a lot of different words that the computer can choose from. These words are stored in `.words` files with each word on a seperate line. When creating your own words file, you will write your words like shown:

```txt, my.words
...
saffran
choklad
polkagris
...
```

## Allowed words

For now, the words that are allowed, are words containing only ascii alphabet letters. This **includes** every lowercase letter `a - z` and every uppercase letter `A - Z`. Unfortunately, this **excludes** the swedish vowels `å` `ä` and `ö`. This is a real bummer, because many swedish words depend on these letters.
