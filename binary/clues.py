#
# clues.py - commands for managing clues
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

CLUES_DIR = os.path.join(CONFIG_DIR, "clues")

#
# Get the file path of a clues by name
#
def clues_file_get(name):
    return os.path.join(CLUES_DIR, f"{name}.clues")

#
# Get the name of a clues file
#
def clues_name_get(file):
    return os.path.splitext(file.replace(CLUES_DIR, ''))[0].strip('/')

#
# Handling the 'gen' command
#
def clues_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "clues-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'del' command
#
def clues_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete clues")

    del_parser.add_argument('name',
        type=str,
        help="Name of clues"
    )

    del_args = del_parser.parse_args(extra_args)

    clues_file = clues_file_get(del_args.name)

    if not os.path.exists(clues_file):
        print(f"korsord: {del_args.name}: Clues not found")
        exit(0)

    subprocess.run(["rm", clues_file])

#
# Handling the 'edit' command
#
def clues_edit(extra_args):
    edit_parser = argparse.ArgumentParser(description="Edit clues")

    edit_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of clues"
    )

    edit_args = edit_parser.parse_args(extra_args)

    clues_file = clues_file_get(edit_args.name)

    if not os.path.exists(clues_file):
        if edit_args.name == "temp":
            print(f"korsord: Clues not found")

        else:
            print(f"korsord: {edit_args.name}: Clues not found")

        exit(0)

    subprocess.run(["vim", clues_file])

#
# Handling the 'list' command
#
def clues_list(extra_args):
    files_exist = False

    for root, dirs, files in os.walk(CLUES_DIR):
        for file in files:
            if file.endswith('.clues'):
                files_exist = True

                clues_file = os.path.join(root, file)

                print(f"{clues_name_get(clues_file)}")

    if not files_exist:
        print(f"No clues exist")

#
# Handling the 'copy' command
#
def clues_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of clues")

    copy_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of clues"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing clues"
    )

    copy_args = copy_parser.parse_args(extra_args)

    clues_file = clues_file_get(copy_args.name)
    copy_file = clues_file_get(copy_args.copy)

    if not os.path.exists(clues_file):
        print(f"korsord: {copy_args.name}: Clues not found")
        exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Clues already exists")
        exit(0)

    subprocess.run(["cp", clues_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Crossword clues handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, edit, del, copy, list"
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

    # Ensure that CLUES_DIR is set correctly and exists
    if not os.path.exists(CLUES_DIR):
        print(f"korsord: {CLUES_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        clues_gen(extra_args)

    elif args.command == "edit":
        clues_edit(extra_args)

    elif args.command == "del":
        clues_del(extra_args)

    elif args.command == "list":
        clues_list(extra_args)

    elif args.command == "copy":
        clues_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
