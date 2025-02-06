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
        nargs="?", default=None,
        help="gen, clues, render"
    )

    parser.add_argument("args",
        nargs=argparse.REMAINDER, default=[]
    )

    args = parser.parse_args()

    if not args.command:
        parser.print_help()
        sys.exit(0)

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

    # Handling the 'clues' script
    elif args.command == "clues":
        clues_script = os.path.join(BASE_DIR, "clues.py")

        if not os.path.isfile(clues_script):
            print(f"korsord: {clues_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", clues_script] + args.args)

    # Handling the 'gen' executable
    elif args.command == "gen":
        gen_program = os.path.join(BASE_DIR, "gen")

        if not os.path.isfile(gen_program):
            print(f"korsord: {gen_program}: File not found")
            sys.exit(1)

        subprocess.run([gen_program] + args.args)

    else:
        print(f"korsord: {args.command}: Command not found")
