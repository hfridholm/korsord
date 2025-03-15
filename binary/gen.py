#
# gen.py - generate crossword completely with openai
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
import time
from common import *

WORDS_FACTOR = 10

#
# Generate temp.words
#
def words_gen(args):
    if not args.theme:
        print(f"korsord: No supplied theme")
        return 1

    words_script = os.path.join(BASE_DIR, "words-gen.py")

    if not os.path.isfile(words_script):
        print(f"korsord: {words_script}: File not found")
        sys.exit(1)

    min_word_amount = int(args.width / 4 * args.height + args.height / 4 * args.width)

    word_amount = min_word_amount * WORDS_FACTOR

    max_length = max(args.width, args.height)

    result = subprocess.run(["python", words_script, "--new", args.theme, "--amount", str(word_amount), "--length", str(max_length)])

    return result.returncode

#
# Generate temp.model
#
def model_gen(args):
    model_script = os.path.join(BASE_DIR, "model-gen.py")

    if not os.path.isfile(model_script):
        print(f"korsord: {model_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", args.image] if args.image else []

    result = subprocess.run(["python", model_script, "--force", str(args.width), str(args.height)] + image_arg)

    return result.returncode

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
# Get the number of words used in crossword
#
def used_word_amount_get(theme_words):
    used_count = 0

    clues_file = clues_file_get("temp")

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

GRID_GEN_TIME  = 10 # Seconds
GRID_GEN_COUNT = 10 # Tries

#
# Generate temp.grid and temp.clues
#
def grid_gen(args):
    grid_program = os.path.join(BASE_DIR, "grid-gen")

    if not os.path.isfile(grid_program):
        print(f"korsord: {grid_program}: File not found")
        sys.exit(1)

    model_arg = ["temp.model"]
    words_arg = ["temp.words", "svenska/270k.words"]


    words_file = words_file_get("temp")

    theme_words = words_file_load(words_file)

    grid_file = grid_file_get("temp")
    clues_file = clues_file_get("temp")

    best_grid = None
    best_clues = None
    best_count = 0

    for iteration in range(GRID_GEN_COUNT):
        try:
            result = subprocess.run([grid_program] + model_arg + words_arg, timeout=GRID_GEN_TIME)

            # To make next grid generation random, wait 1 more second
            time.sleep(1)

            if result.returncode != 0:
                continue
            
            # Get amount of words from 'temp.words'
            curr_count = used_word_amount_get(theme_words)

            print(f"Theme words: {curr_count}")

            if curr_count > best_count:
                best_grid = file_read(grid_file)

                best_clues = file_read(clues_file)

                best_count = curr_count
    
        except subprocess.TimeoutExpired:
            print(f"korsord: Grid generation timed out")
            continue

    if best_grid:
        file_write(grid_file, best_grid)

    if best_clues:
        file_write(clues_file, best_clues)

    # If a grid was not generated, return error code
    if not best_grid or not best_clues:
        return 1

    print(f"Best theme words: {best_count}")

    return 0

#
# Generate temp.png
#
def image_gen(args):
    if not args.theme:
        print(f"korsord: Theme not specified")
        sys.exit(1)

    image_script = os.path.join(BASE_DIR, "image-gen.py")

    if not os.path.isfile(image_script):
        print(f"korsord: {image_script}: File not found")
        sys.exit(1)

    result = subprocess.run(["python", image_script, args.theme])

    return result.returncode

#
# Generate clues in temp.clues
#
def clues_gen(args):
    clues_script = os.path.join(BASE_DIR, "clues-gen.py")

    if not os.path.isfile(clues_script):
        print(f"korsord: {clues_script}: File not found")
        sys.exit(1)

    theme_arg = ["--theme", args.theme] if args.theme else []

    result = subprocess.run(["python", clues_script, "--force"] + theme_arg)

    return result.returncode

#
# Render final images
#
def render_gen(args):
    render_script = os.path.join(BASE_DIR, "render-gen.py")

    if not os.path.isfile(render_script):
        print(f"korsord: {render_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", "temp"] if args.image else []

    result = subprocess.run(["python", render_script] + image_arg)

    return result.returncode

#
# Main routine
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Generate crossword completely using openai"
    )

    parser.add_argument("--theme",
        type=str, default=None,
        help="Theme of crossword"
    )

    parser.add_argument("--width",
        type=int, default=None,
        help="Width of grid"
    )

    parser.add_argument("--height",
        type=int, default=None,
        help="Height of grid"
    )

    parser.add_argument("--image",
        type=str, default=None,
        help="Prompt for image"
    )

    parser.add_argument("--step",
        type=str, default="words",
        help="Start at step of generation"
    )

    args = parser.parse_args()

    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)


    steps = ["words", "model", "grid", "clues", "image", "render"]

    try:
        step_index = steps.index(args.step)

    except:
        print(f"Steps: {' '.join(steps)}")
        print(f"korsord: {args.step}: Step not found")
        sys.exit(1)


    if step_index <= 0 and not args.theme:
        print(f"korsord: Theme must be supplied for words")
        sys.exit(1)

    if step_index <= 1 and not (args.width and args.height):
        print(f"korsord: Size must be supplied for model")
        sys.exit(1)


    # 1. Generate words
    if step_index <= 0 and words_gen(args) != 0:
        print(f"Failed to generate words")
        sys.exit(2)

    # 2. Generate model
    if step_index <= 1 and model_gen(args) != 0:
        print(f"Failed to generate model")
        sys.exit(3)

    # 3. Generate grid
    if step_index <= 2 and grid_gen(args) != 0:
        print(f"Failed to generate grid")
        sys.exit(4)

    # 4. Generate clues
    if step_index <= 3 and clues_gen(args) != 0:
        print(f"Failed to generate clues")
        sys.exit(5)

    # 5. Generate image
    if step_index <= 4 and args.image and image_gen(args) != 0:
        print(f"Failed to generate image")
        sys.exit(6)

    # 6. Render images
    if step_index <= 5 and render_gen(args) != 0:
        print(f"Failed to render images")
        sys.exit(7)
