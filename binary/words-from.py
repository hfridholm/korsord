#
# words-from.py - extract words from file
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
from common import *

#
# Extract the first word of line
#
def word_extract(line):
    first_word = line.split()[0] if line.split() else ""

    # The word must have only ascii characters
    if not first_word.isalpha() or not first_word.isascii():
        return None

    # The word must have at least 2 letters
    if len(first_word) < 2:
        return None

    return first_word.lower()

#
# Extract the first word of each line from input file
#
def words_extract(input_file):
    words = []

    try:
        print(f"Extracting words from {input_file}")

        with open(input_file, 'r') as infile:
            for line in infile:
                word = word_extract(line)

                if word:
                    words.append(word)

        print(f"Extracted words from {input_file}")

    except FileNotFoundError:
        print(f"The file {input_file} was not found.")
        return None

    except Exception as exception:
        print(f"An error occurred: {exception}")
        return None
    
    return words

#
# Filter words by removing duplicates
#
def words_filter(words):
    print(f"Filtering words: {len(words)}")

    seen_words = set()
    result = []

    for word in words:
        if word not in seen_words:
            result.append(word)
            seen_words.add(word)

    print(f"Filtered words: {len(result)}")

    return result

#
# Save words
#
def words_save(words, filepath):
    try:
        with open(filepath, 'w') as file:
            for word in words:
                file.write(f"{word}\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Main routine
#
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Extract words from file")
    
    parser.add_argument('file',
        type=str,
        help="File to extract words from"
    )

    parser.add_argument('--name',
        type=str, default="temp",
        help="Name of words"
    )

    args = parser.parse_args()

    words = words_extract(args.file)

    if not words:
        print(f"No words to extract")
        sys.exit(0)

    words = words_filter(words)

    words_file = words_file_get(args.name)

    words_save(words, words_file)
