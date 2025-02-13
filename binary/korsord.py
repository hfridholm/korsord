#
# korsord.py - bundled program for korsord commands
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os

# Base directory where the programs are located
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Swedish crossword generator"
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, render, words, clues, model, grid"
    )

    parser.add_argument("args",
        nargs=argparse.REMAINDER, default=[]
    )

    args = parser.parse_args()


    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)


    # Handling the 'render' script
    elif args.command == "render":
        render_script = os.path.join(BASE_DIR, "render.py")

        if not os.path.isfile(render_script):
            print(f"korsord: {render_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", render_script] + args.args)


    # Handling the 'words' script
    elif args.command == "words":
        words_script = os.path.join(BASE_DIR, "words.py")

        if not os.path.isfile(words_script):
            print(f"korsord: {words_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", words_script] + args.args)


    # Handling the 'clues' script
    elif args.command == "clues":
        clues_script = os.path.join(BASE_DIR, "clues.py")

        if not os.path.isfile(clues_script):
            print(f"korsord: {clues_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", clues_script] + args.args)


    # Handling the 'model' script
    elif args.command == "model":
        model_script = os.path.join(BASE_DIR, "model.py")

        if not os.path.isfile(model_script):
            print(f"korsord: {model_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", model_script] + args.args)


    # Handling the 'grid' script
    elif args.command == "grid":
        grid_script = os.path.join(BASE_DIR, "grid.py")

        if not os.path.isfile(grid_script):
            print(f"korsord: {grid_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", grid_script] + args.args)


    # Handling the 'gen' script
    elif args.command == "gen":
        gen_script = os.path.join(BASE_DIR, "gen.py")

        if not os.path.isfile(gen_script):
            print(f"korsord: {gen_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", gen_script] + args.args)

    else:
        print(f"korsord: {args.command}: Command not found")
