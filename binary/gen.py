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

    result = subprocess.run(["python",   words_script, args.theme,
                             "--new",
                             "--amount", str(word_amount),
                             "--length", str(args.length) if args.length else str(max_length),
                             "--name",   args.name
                            ])
    
    print(f"")

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

    result = subprocess.run(["python", model_script, str(args.width), str(args.height),
                             "--force",
                             "--name", args.name,
                            ] + image_arg)

    print(f"")

    return result.returncode

#
# Generate temp.grid and temp.clues
#
def grid_gen(args):
    grid_script = os.path.join(BASE_DIR, "grid-gen.py")

    if not os.path.isfile(grid_script):
        print(f"korsord: {grid_script}: File not found")
        sys.exit(1)

    length_arg = ["--length", str(args.length)] if args.length else []

    result = subprocess.run(["python", grid_script, args.model,
                             "--name",  args.name,
                             "--words", ' '.join(args.words),
                            ] + length_arg)

    print(f"")

    return result.returncode

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

    result = subprocess.run(["python", image_script, args.theme,
                             "--name", args.name
                             ])

    print(f"")

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

    result = subprocess.run(["python", clues_script,
                             "--force",
                             "--name", args.name,
                            ] + theme_arg)

    print(f"")

    return result.returncode

#
# Render final images
#
def render_gen(args):
    render_script = os.path.join(BASE_DIR, "render-gen.py")

    if not os.path.isfile(render_script):
        print(f"korsord: {render_script}: File not found")
        sys.exit(1)

    words_arg = ["--words", ' '.join(args.words)] if args.words else []

    result = subprocess.run(["python", render_script,
                             "--image", args.name,
                             "--grid",  args.name,
                             "--clues", args.clues,
                            ] + words_arg)

    print(f"")

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

    parser.add_argument("--length",
        type=int, default=None,
        help="Max length of words"
    )

    parser.add_argument("--image",
        type=str, default=None,
        help="Prompt for image"
    )

    parser.add_argument("--step",
        type=str, default="words",
        help="Start at step of generation"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of korsord"
    )

    # Define arguments for each step result
    parser.add_argument("--words",
        type=str, default=None,
        help="Names of words"
    )

    parser.add_argument("--clues",
        type=str, default=None,
        help="Name of clues"
    )

    parser.add_argument("--model",
        type=str, default=None,
        help="Name of model"
    )

    args = parser.parse_args()

    # Expand arguments
    if args.words:
        args.words = args.words.split(' ')


    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)

    # Parse step argument
    steps = ["words", "model", "grid", "clues", "image", "render"]

    try:
        step_index = steps.index(args.step)

    except:
        print(f"Steps: {' '.join(steps)}")
        print(f"korsord: {args.step}: Step not found")
        sys.exit(1)

    # Populate width and height if model is supplied
    if args.model:
        model_file = model_file_get(args.model)

        model_size = grid_size_get(model_file)

        if model_size:
            args.width, args.height = model_size


    if step_index <= 0 and not args.words and not args.theme:
        print(f"korsord: Theme must be supplied for words")
        sys.exit(1)

    if step_index <= 1 and not args.model and not (args.width and args.height):
        print(f"korsord: Size must be supplied for model")
        sys.exit(1)

    if step_index <= 4 and args.image and not args.theme:
        print(f"korsord: Theme must be supplied for image")
        sys.exit(1)


    # 1. Generate words
    if step_index <= 0 and not args.words:
        if words_gen(args) != 0:
            print(f"Failed to generate words")
            sys.exit(2)

    if not args.words:
        args.words = [args.name]

    # 2. Generate model
    if step_index <= 1 and not args.model:
        if model_gen(args) != 0:
            print(f"Failed to generate model")
            sys.exit(3)

    if not args.model:
        args.model = args.name

    # 3. Generate grid
    if step_index <= 2 and grid_gen(args) != 0:
        print(f"Failed to generate grid")
        sys.exit(4)

    # 4. Generate clues
    if step_index <= 3 and not args.clues:
        if clues_gen(args) != 0:
            print(f"Failed to generate clues")
            sys.exit(5)

    if not args.clues:
        args.clues = args.name

    # 5. Generate image
    if step_index <= 4 and args.theme and image_gen(args) != 0:
        print(f"Failed to generate image")
        sys.exit(6)

    # 6. Render images
    if step_index <= 5 and render_gen(args) != 0:
        print(f"Failed to generate render")
        sys.exit(7)

    print(f"Done")
