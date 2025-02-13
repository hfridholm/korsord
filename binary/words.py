#
# words.py - commands for managing words
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

WORDS_DIR = os.path.join(CONFIG_DIR, "words")

#
# Get the file path of a words by name
#
def words_file_get(name):
    return os.path.join(WORDS_DIR, f"{name}.words")

#
# Get the name of a words file
#
def words_name_get(file):
    return os.path.splitext(file.replace(WORDS_DIR, ''))[0].strip('/')

#
# Handling the 'gen' command
#
def words_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "words-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'new' command
#
def words_new(extra_args):
    new_parser = argparse.ArgumentParser(description="New words")

    new_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of new words"
    )

    new_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing words"
    )

    new_parser.add_argument('--from',
        type=str, dest="from_path", default=None,
        help="Extract words from file"
    )

    new_args = new_parser.parse_args(extra_args)


    if new_args.from_path:
        from_script = os.path.join(BASE_DIR, "words-from.py")

        if not os.path.isfile(from_script):
            print(f"korsord: {from_script}: File not found")
            sys.exit(1)

        subprocess.run(["python", from_script, "--name", new_args.name, new_args.from_path])

    else:
        new_file = words_file_get(new_args.name)

        if os.path.exists(new_file):
            if new_args.force:
                subprocess.run(["rm", new_file])

            else:
                print(f"korsord: {new_args.name}: Words already exists")
                exit(0)

        subprocess.run(["touch", new_file])

#
# Handling the 'del' command
#
def words_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete words")

    del_parser.add_argument('name',
        type=str,
        help="Name of words"
    )

    del_args = del_parser.parse_args(extra_args)

    words_file = words_file_get(del_args.name)

    if not os.path.exists(words_file):
        print(f"korsord: {del_args.name}: Words not found")
        exit(0)

    subprocess.run(["rm", words_file])

#
# Handling the 'edit' command
#
def words_edit(extra_args):
    edit_parser = argparse.ArgumentParser(description="Edit words")

    edit_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of words"
    )

    edit_parser.add_argument('--new',
        action='store_true',
        help="Create new words and edit it"
    )

    edit_args = edit_parser.parse_args(extra_args)

    words_file = words_file_get(edit_args.name)

    if not os.path.exists(words_file):
        if not edit_args.new:
            if edit_args.name == "temp":
                print(f"korsord: Words not found")

            else:
                print(f"korsord: {edit_args.name}: Words not found")

            exit(0)

        else:
            subprocess.run(["touch", words_file])

    subprocess.run(["vim", words_file])

#
# Handling the 'list' command
#
def words_list(extra_args):
    files_exist = False

    for root, dirs, files in os.walk(WORDS_DIR):
        for file in files:
            if file.endswith('.words'):
                files_exist = True

                words_file = os.path.join(root, file)

                print(f"{words_name_get(words_file)}")

    if not files_exist:
        print(f"No words exist")

#
# Handling the 'copy' command
#
def words_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of words")

    copy_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of words"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing words"
    )

    copy_args = copy_parser.parse_args(extra_args)

    words_file = words_file_get(copy_args.name)
    copy_file = words_file_get(copy_args.copy)

    if not os.path.exists(words_file):
        print(f"korsord: {copy_args.name}: Words not found")
        exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Words already exists")
        exit(0)

    subprocess.run(["cp", words_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Crossword words handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, edit, new, del, copy, list"
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

    # Ensure that WORDS_DIR is set correctly and exists
    if not os.path.exists(WORDS_DIR):
        print(f"korsord: {WORDS_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        words_gen(extra_args)

    elif args.command == "edit":
        words_edit(extra_args)

    elif args.command == "new":
        words_new(extra_args)

    elif args.command == "del":
        words_del(extra_args)

    elif args.command == "list":
        words_list(extra_args)

    elif args.command == "copy":
        words_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
