#
# gen.py - generate crossword completely with openai
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os

# Base directory where the programs are located
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG_DIR = os.path.join(os.path.expanduser('~'), ".korsord")

#
# Generate temp.words
#
def words_gen(args):
    words_script = os.path.join(BASE_DIR, "words-gen.py")

    if not os.path.isfile(words_script):
        print(f"korsord: {words_script}: File not found")
        sys.exit(1)

    min_word_amount = int(args.width / 4 * args.height + args.height / 4 * args.width)

    word_amount = min_word_amount * 10

    max_length = max(args.width, args.height)

    subprocess.run(["python", words_script, "--new", args.theme, "--amount", str(word_amount), "--length", str(max_length)])

#
# Generate temp.model
#
def model_gen(args):
    model_script = os.path.join(BASE_DIR, "model-gen.py")

    if not os.path.isfile(model_script):
        print(f"korsord: {model_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", args.image] if args.image else []

    subprocess.run(["python", model_script, "--force", str(args.width), str(args.height)] + image_arg)

#
# Generate temp.grid and temp.clues
#
def grid_gen(args):
    grid_program = os.path.join(BASE_DIR, "grid-gen")

    if not os.path.isfile(grid_program):
        print(f"korsord: {grid_program}: File not found")
        sys.exit(1)

    subprocess.run([grid_program, "temp", "temp", "svenska/870k", "-l", "6"])

#
# Generate temp.png
#
def image_gen(args):
    image_script = os.path.join(BASE_DIR, "image-gen.py")

    if not os.path.isfile(image_script):
        print(f"korsord: {image_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", image_script, args.image])

#
# Generate clues in temp.clues
#
def clues_gen(args):
    clues_script = os.path.join(BASE_DIR, "clues-gen.py")

    if not os.path.isfile(clues_script):
        print(f"korsord: {clues_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", clues_script, "--theme", args.theme])

#
# Render final images
#
def render_images(args):
    render_script = os.path.join(BASE_DIR, "render.py")

    if not os.path.isfile(render_script):
        print(f"korsord: {render_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", "temp.png"] if args.image else []

    subprocess.run(["python", render_script, "temp.grid", "temp.clues"] + image_arg)

#
# Main routine
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Generate crossword completely using openai"
    )

    parser.add_argument("theme",
        type=str,
        help="Theme of crossword"
    )

    parser.add_argument("width",
        type=int,
        help="Width of grid"
    )

    parser.add_argument("height",
        type=int,
        help="Height of grid"
    )

    parser.add_argument("--image",
        type=str, default=None,
        help="Prompt for image"
    )

    args = parser.parse_args()

    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)

    # 1. Generate words
    words_gen(args)

    # 1. Generate model
    model_gen(args)

    # 1. Generate grid
    # grid_gen(args)

    # 1. Generate clues
    # clues_gen(args)

    # 1. Generate image
    if args.image:
        image_gen(args)

    # 1. Render images
    # render_images(args)
