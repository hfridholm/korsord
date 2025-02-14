#
# words-gen.py - generate crossword words using openai
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import argparse
import os
from common import *
import re

api_key_file = "api.key"

# Start an openai client using API key
client = OpenAI(api_key=api_key_get(api_key_file))

#
# Ask ChatGPT for a clue to a word
#
def words_gen(existing_words):
    values = {
        "{amount}": args.amount,
        "{theme}": args.theme,
        "{length}": args.length,
        "{words}": '\n'.join(existing_words[-10:])
    }

    prompt = prompt_load("words", values)

    if not prompt:
        print(f"Failed to load prompt")
        return None

    print(f"Prompt:\n{prompt}\n")

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Du har kunskap från svenska ordböcker"},
                {"role": "user", "content": prompt}
            ],
            temperature=0.7
        )

        message = completion.choices[0].message.content

        print(f"Response:\n{message}\n")

        gen_words = []

        for line in message.splitlines():
            line_words = re.findall(r'[A-Za-zåäöÅÄÖ]+', line)

            # If there are more than one word on line, or
            # if there are no word on line,
            # disregard line
            if len(line_words) != 1:
                continue

            word = line_words[0].strip().lower()

            if word not in existing_words:
                gen_words.append(word)

        return gen_words

    except Exception as exception:
        print(f"ChatGPT error: {exception}")
        return None

#
# Load words
#
def words_load(filepath):
    words = []

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                if(len(split_line) < 1):
                    return None

                word = split_line[0].strip().lower()

                words.append(word);

    except FileNotFoundError:
        print(f"korsord: Words file not found")
        return None

    except Exception as exception:
        print(f"korsord: Failed to read words file")
        return None

    return words

#
# Save words
#
def words_save(words, filepath):
    # 1. Get max width for alignment
    max_width = 0

    for word in words:
        max_width = max(max_width, len(word) + 1)

    try:
        with open(filepath, 'w') as file:
            for word in words:
                curr_width = max_width - len(word)

                file.write(f"{word}{' ' * curr_width}:\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Check if word is valid
#
def word_is_allowed(word):
    if any(char in word for char in ['å', 'ä', 'ö']):
        return False

    if len(word) > args.length or len(word) < 2:
        return False

    if not word.isalpha():
        return False

    return True

#
#
#
def words_filter(words):
    filtered_words = []

    for word in words:
        if len(filtered_words) >= args.amount:
            break

        if(word_is_allowed(word) and word not in filtered_words):
            filtered_words.append(word)

    return filtered_words

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="generate crossword words using ai")

    parser.add_argument("theme",
        type=str,
        help="Theme of words"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of words"
    )

    parser.add_argument("--amount",
        type=int, default=10,
        help="Amount of words"
    )

    parser.add_argument("--length",
        type=int, default=10,
        help="Max length of words"
    )

    parser.add_argument("--append",
        action='store_true',
        help="Append to existing words"
    )

    parser.add_argument("--new",
        action='store_true',
        help="Remove existing words"
    )

    args = parser.parse_args()


    words_file = words_file_get(args.name)

    existing_words = words_load(words_file);

    if existing_words:
        if args.new:
            existing_words = []

        elif not args.append:
            print(f"korsord: Words already exist")
            sys.exit(0)

    else: # if no words exist
        existing_words = []

    new_words = []

    for count in range(1, 10):
        if len(new_words) >= args.amount:
            break

        gen_words = words_gen(existing_words + new_words)

        if gen_words:
            new_words += gen_words

            new_words = words_filter(new_words)

            print(f"#{count} Generated words: {len(new_words)}")

    words_save(existing_words + new_words, words_file)

    print(f"Generated words")
