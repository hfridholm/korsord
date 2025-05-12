#
# clues.py - generate crossword clues using openai
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import argparse
import os
from common import *

# Start an openai client using API key
api_key = api_key_get()

if not api_key:
    print(f"korsord: API key not found")
    sys.exit(1)

client = OpenAI(api_key=api_key)

#
# Ask ChatGPT for a clue to a word
#
def word_clue_gen(word):
    values = {
        "{amount}": args.amount,
        "{word}": word,
    }

    prompt = prompt_load("clues", values)

    if not prompt:
        print(f"Failed to load prompt")
        return None

    if args.theme:
        prompt += f"Om ordet passar temat \'{args.theme}\' använd det temat, annars ge en generell ledtråd."

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Du har en stor språkförståelse"},
                {"role": "user", "content": prompt}
            ],
            max_tokens=args.length,
            temperature=0.7
        )

        message = completion.choices[0].message.content

        clue = message.strip().strip(".").upper()

        return clue

    except Exception as exception:
        print(f"ChatGPT error: {exception}")
        return None

#
# Load words and their clues in a dictionary
#
def word_clues_load(filepath):
    word_clues = {}

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                word = split_line[0].strip().lower()
                clue = split_line[1].strip() if len(split_line) == 2 else ""

                word_clues[word] = clue;

    except FileNotFoundError:
        print(f"Words file not found")
        return None

    except Exception as exception:
        print(f"Failed to read words file")
        return None

    return word_clues

#
# Save words and their clues
#
def word_clues_save(word_clues, filepath):
    # 1. Get max width for alignment
    max_width = 0

    for word in word_clues:
        max_width = max(max_width, len(word) + 1)

    try:
        with open(filepath, 'w') as file:
            for word, clue in word_clues.items():
                curr_width = max_width - len(word)

                file.write(f"{word}{' ' * curr_width}: {clue}\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="generate crossword clues using openai")

    parser.add_argument("--theme",
        type=str, default=None,
        help="Theme of words"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of clues"
    )

    parser.add_argument("--words",
        type=str, default=None,
        help="Name of words"
    )

    parser.add_argument("--amount",
        type=int, default=4,
        help="Amount of words in clue"
    )

    parser.add_argument("--length",
        type=int, default=30,
        help="Max length of clue"
    )

    parser.add_argument("--force",
        action='store_true',
        help="Modify existing clues"
    )

    args, words = parser.parse_known_args()


    clues_file = clues_file_get(args.name)

    if args.words:
        words_file = words_file_get(args.words)

        if not os.path.exists(words_file):
            print(f"korsord: {args.words}: Words not found")
            sys.exit(0)

        if os.path.exists(clues_file) and not args.force:
            print(f"korsord: {args.name}: Clues already exist")
            sys.exit(0)

        word_clues = word_clues_load(words_file)

    elif not os.path.exists(clues_file):
        print(f"korsord: {args.name}: Clues not found")
        sys.exit(0)

    else:
        word_clues = word_clues_load(clues_file)


    if not word_clues:
        print(f"korsord: {args.name}: Failed to load words")
        sys.exit(1)


    print(f"Generating clues")

    # Get max width for alignment
    max_width = 0

    for word in word_clues:
        max_width = max(max_width, len(word) + 1)


    # Fill in missing clues using ChatGPT
    for word, clue in word_clues.items():

        # Generate missing clues, or overwrite clues if specified, or is forcing
        if (not words and len(clue) == 0) or (words and word in words) or args.force:
            for count in range(10):
                clue = word_clue_gen(word)

                if len(clue) <= args.length:
                    break

            curr_width = max_width - len(word)

            print(f"{word}{' ' * curr_width}: {clue}")

        word_clues[word] = clue

    print(f"Generated clues")

    word_clues_save(word_clues, clues_file)

    print(f"Done")
