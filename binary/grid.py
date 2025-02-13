#
# grid.py - commands for managing grids
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

GRIDS_DIR = os.path.join(CONFIG_DIR, "grids")

#
# Get the file path of a grid by name
#
def grid_file_get(name):
    return os.path.join(GRIDS_DIR, f"{name}.grid")

#
# Handling the 'gen' command
#
def grid_gen(extra_args):
    gen_program = os.path.join(BASE_DIR, "gen")

    if not os.path.isfile(gen_program):
        print(f"korsord: {gen_program}: File not found")
        sys.exit(1)

    subprocess.run([gen_program] + extra_args)

#
# Handling the 'del' command
#
def grid_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete grid")

    del_parser.add_argument('name',
        type=str,
        help="Name of grid"
    )

    del_args = del_parser.parse_args(extra_args)

    grid_file = grid_file_get(del_args.name)

    if not os.path.exists(grid_file):
        print(f"korsord: {del_args.name}: Grid not found")
        exit(0)

    subprocess.run(["rm", grid_file])

#
# Handling the 'show' command
#
def grid_show(extra_args):
    show_parser = argparse.ArgumentParser(description="Show grid")

    show_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of grid"
    )

    show_args = show_parser.parse_args(extra_args)

    grid_file = grid_file_get(show_args.name)

    if not os.path.exists(grid_file):
        print(f"korsord: {show_args.name}: Grid not found")
        exit(0)

    subprocess.run(["cat", grid_file])

#
# Handling the 'edit' command
#
def grid_edit(extra_args):
    edit_parser = argparse.ArgumentParser(description="Edit grid")

    edit_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of grid"
    )

    edit_args = edit_parser.parse_args(extra_args)

    grid_file = grid_file_get(edit_args.name)

    if not os.path.exists(grid_file):
        if edit_args.name == "temp":
            print(f"korsord: Grid not found")

        else:
            print(f"korsord: {edit_args.name}: Grid not found")

        exit(0)

    subprocess.run(["vim", grid_file])

#
# Handling the 'list' command
#
def grid_list(extra_args):
    files_exist = False

    for root, dirs, files in os.walk(GRIDS_DIR):
        for file in files:
            if file.endswith('.grid'):
                files_exist = True

                grid_file = os.path.join(root, file)

                print(f"{grid_name_get(grid_file)}")

    if not files_exist:
        print(f"No grids exist")

#
# Handling the 'copy' command
#
def grid_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of grid")

    copy_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of grid"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing grid"
    )

    copy_args = copy_parser.parse_args(extra_args)

    grid_file = grid_file_get(copy_args.name)
    copy_file = grid_file_get(copy_args.copy)

    if not os.path.exists(grid_file):
        print(f"korsord: {copy_args.name}: Grid not found")
        exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Grid already exists")
        exit(0)

    subprocess.run(["cp", grid_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Swedish crossword generator",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, show, edit, del, copy, list"
    )

    args, extra_args = parser.parse_known_args()

    # Print help menu if no command was supplied
    if not args.command:
        parser.print_help()
        sys.exit(0)


    # Ensure that BASE_DIR is set correctly and exists
    if not os.path.exists(BASE_DIR):
        print(f"korsord: {BASE_DIR}: Directory not found")
        sys.exit(1)

    # Ensure that GRIDS_DIR is set correctly and exists
    if not os.path.exists(GRIDS_DIR):
        print(f"korsord: {GRIDS_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        grid_gen(extra_args)

    elif args.command == "show":
        grid_show(extra_args)

    elif args.command == "edit":
        grid_edit(extra_args)

    elif args.command == "del":
        grid_del(extra_args)

    elif args.command == "list":
        grid_list(extra_args)

    elif args.command == "copy":
        grid_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
