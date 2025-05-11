#
# grid-gen.py - generate best grid using grid-gen
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
import time
from common import *

#
# Load words
#
def words_file_load(filepath):
    words = []

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                if(len(split_line) < 1):
                    return None

                word = split_line[0].strip().lower()

                words.append(word);

        return words

    except FileNotFoundError:
        print(f"korsord: Words file not found")
        return None

    except Exception as exception:
        print(f"korsord: Failed to read words file")
        return None

#
# Load words from files of inputted names
#
def words_load(words_names):
    words = set()

    for words_name in words_names:
        print(f"Loading words: {words_name}")

        words_file = words_file_get(words_name)

        curr_words = words_file_load(words_file)

        if not curr_words:
            print(f"Failed to load words: {words_name}")
            return None

        words.update(curr_words)

    return list(words)

#
# Get the number of words used in crossword
#
def used_word_amount_get(theme_words):
    used_count = 0

    clues_file = clues_file_get(args.name)

    grid_words = words_file_load(clues_file)

    for word in grid_words:
        if word in theme_words:
            used_count += 1

    return used_count

#
# Read data from file
#
def file_read(filepath):
    try:
        with open(filepath, "r") as file:
            return file.read()

    except FileNotFoundError:
        print(f"korsord: Words file not found")
        return None

    except Exception as exception:
        print(f"korsord: Failed to read words file")
        return None

#
# Write data to file
#
def file_write(filepath, data):
    try:
        with open(filepath, 'w') as file:
            file.write(data)

    except Exception as exception:
        print(f"korsord: Failed to write words file")

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Generate best grid using grid-gen"
    )

    parser.add_argument("model",
        type=str,
        help="Name of model"
    )

    parser.add_argument("words",
        type=str,
        help="Names of words"
    )

    parser.add_argument("--backup",
        type=str, default="svenska/270k",
        help="Name of backup words"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of grid and clues"
    )

    parser.add_argument("--length",
        type=int, default=8,
        help="Max length of words"
    )

    parser.add_argument("--amount",
        type=int, default=10,
        help="Number of generations"
    )

    parser.add_argument("--time",
        type=int, default=10,
        help="Time of single generation"
    )

    args = parser.parse_args()

    # Expand arguments
    if args.words:
        args.words = args.words.split(' ')


    # Load grid-gen
    grid_program = os.path.join(BASE_DIR, "grid-gen")

    if not os.path.isfile(grid_program):
        print(f"korsord: {grid_program}: File not found")
        sys.exit(1)


    # Define arguments for grid-gen program
    words_arg = args.words + [args.backup]


    # Load all theme words
    theme_words = words_load(args.words)

    if args.words and not theme_words:
        print(f"Failed to load words")
        sys.exit(2)


    # Generate best grid and clues
    grid_file  = grid_file_get(args.name)
    clues_file = clues_file_get(args.name)

    best_grid  = None
    best_clues = None
    best_count = 0

    for iteration in range(args.amount):
        try:
            result = subprocess.run([grid_program,
                                     "--name",   args.name,
                                     "--length", str(args.length),
                                     args.model,
                                    ] + words_arg,
                                    timeout=args.time)

            # To make next grid generation random, wait 1 more second
            time.sleep(1)

            if result.returncode != 0:
                continue

            # Get amount of words from 'temp.words'
            curr_count = used_word_amount_get(theme_words)

            if curr_count > best_count:
                best_grid  = file_read(grid_file)

                best_clues = file_read(clues_file)

                best_count = curr_count
    
        except subprocess.TimeoutExpired:
            print(f"korsord: Grid generation timed out")
            continue

    # Store best grid and clues
    if best_grid:
        file_write(grid_file, best_grid)

    if best_clues:
        file_write(clues_file, best_clues)

    # If a grid was not generated, return error code
    if not best_grid or not best_clues:
        print(f"korsord: Failed to generate grid")
        sys.exit(1)

    print(f"Best theme words: {best_count}")
