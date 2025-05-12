#
# grid.py - commands for managing grids
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
from common import *

#
# Handling the 'gen' command
#
def grid_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "grid-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'del' command
#
def grid_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete grid")

    del_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of grid"
    )

    del_args = del_parser.parse_args(extra_args)

    grid_file = grid_file_get(del_args.name)

    if not os.path.exists(grid_file):
        print(f"korsord: {del_args.name}: Grid not found")
        sys.exit(0)

    subprocess.run(["rm", grid_file])

#
# Handling the 'view' command
#
def grid_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View grid")

    view_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of grid"
    )

    view_args = view_parser.parse_args(extra_args)

    grid_file = grid_file_get(view_args.name)

    if not os.path.exists(grid_file):
        print(f"korsord: {view_args.name}: Grid not found")
        sys.exit(0)

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

        sys.exit(0)

    subprocess.run(["vim", grid_file])

#
# Get all grid files
#
def grid_files_get():
    grid_files = []

    for root, dirs, files in os.walk(GRIDS_DIR):
        for file in files:
            if file.endswith('.grid'):
                grid_file = os.path.join(root, file)

                grid_files.append(grid_file)

    return grid_files

#
# Handling the 'list' command
#
def grid_list(extra_args):
    grid_files = grid_files_get()

    if len(grid_files) == 0:
        print(f"No grid exist")
        sys.exit(0)

    max_width = 0

    for file in grid_files:
        name = grid_name_get(file)

        max_width = max(max_width, len(name) + 1)

    for file in grid_files:
        name = grid_name_get(file)

        grid_size = grid_size_get(file)

        if not grid_size:
            continue

        curr_width = max_width - len(name)

        print(f"{name}{' ' * curr_width}: {grid_size[0]}x{grid_size[1]}")

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
        sys.exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Grid already exists")
        sys.exit(0)

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
        help="gen, view, edit, del, copy, list"
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

    elif args.command == "view":
        grid_view(extra_args)

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
