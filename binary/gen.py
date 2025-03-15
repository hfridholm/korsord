#
# gen.py - generate crossword completely with openai
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
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
# Generate temp.grid and temp.clues
#
def grid_gen(args):
    grid_program = os.path.join(BASE_DIR, "grid-gen")

    if not os.path.isfile(grid_program):
        print(f"korsord: {grid_program}: File not found")
        sys.exit(1)

    result = subprocess.run([grid_program, "temp.model", "temp.words", "svenska/270k.words"])

    return result.returncode

#
# Generate temp.png
#
def image_gen(args):
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

    # result = subprocess.run(["python", clues_script] + theme_arg)
    result = subprocess.run(["python", clues_script])

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
