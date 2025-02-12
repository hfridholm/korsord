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

CONFIG_DIR = "~/.korsord"

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Swedish crossword generator"
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

    print(args)

    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)

    words_script = os.path.join(BASE_DIR, "words.py")

    if not os.path.isfile(words_script):
        print(f"korsord: {words_script}: File not found")
        sys.exit(1)

    word_amount = int(args.width / 4 * args.height + args.height / 4 * args.width)

    print(f"word amount: {word_amount}")

    subprocess.run(["python", words_script, args.theme, "--amount", str(word_amount), "--name", "temp.words", "--force"])


    model_script = os.path.join(BASE_DIR, "model.py")

    if not os.path.isfile(model_script):
        print(f"korsord: {model_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", args.image] if args.image else []

    subprocess.run(["python", model_script, str(args.width), str(args.height)] + image_arg)


    grid_program = os.path.join(BASE_DIR, "grid-gen")

    if not os.path.isfile(grid_program):
        print(f"korsord: {grid_program}: File not found")
        sys.exit(1)

    subprocess.run([grid_program, "temp.model", "temp.words", "svenska/870k.words", "-l 6"])


    if args.image:
        image_script = os.path.join(BASE_DIR, "image.py")

        if not os.path.isfile(image_script):
            print(f"korsord: {image_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", image_script, args.image])

    
    clues_script = os.path.join(BASE_DIR, "clues.py")

    if not os.path.isfile(clues_script):
        print(f"korsord: {clues_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", clues_script, "temp.clues", "--theme", args.theme])
    
    
    render_script = os.path.join(BASE_DIR, "render.py")

    if not os.path.isfile(render_script):
        print(f"korsord: {render_script}: File not found")
        sys.exit(1)

    image_arg = ["--image", "temp.png"] if args.image else []

    subprocess.run(["python", render_script, "temp.grid", "temp.clues"] + image_arg)
