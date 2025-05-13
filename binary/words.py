#
# words.py - commands for managing words
#
# Written by Hampus Fridholm
#

import argparse
import subprocess
import sys
import os
import re
from common import *

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
                sys.exit(0)

        subprocess.run(["touch", new_file])

#
# Handling the 'del' command
#
def words_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete words")

    del_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of words"
    )

    del_args = del_parser.parse_args(extra_args)

    words_file = words_file_get(del_args.name)

    if not os.path.exists(words_file):
        print(f"korsord: {del_args.name}: Words not found")
        sys.exit(0)

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
            print(f"korsord: {edit_args.name}: Words not found")
            sys.exit(0)

        else:
            subprocess.run(["touch", words_file])

    subprocess.run(["vim", words_file])

#
# Handling the 'view' command
#
def words_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View words")

    view_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of words"
    )

    view_args = view_parser.parse_args(extra_args)

    words_file = words_file_get(view_args.name)

    if not os.path.exists(words_file):
        print(f"korsord: {view_args.name}: Words not found")
        sys.exit(0)

    subprocess.run(["less", words_file])

#
# Get all words files
#
def words_files_get():
    words_files = []

    for root, dirs, files in os.walk(WORDS_DIR):
        for file in files:
            if file.endswith('.words'):
                words_file = os.path.join(root, file)

                words_files.append(words_file)

    return words_files

#
# Handling the 'list' command
#
def words_list(extra_args):
    words_files = words_files_get()

    if len(words_files) == 0:
        print(f"No words exist")
        sys.exit(0)

    max_width = 0

    for file in words_files:
        name = words_name_get(file)

        max_width = max(max_width, len(name) + 1)

    for file in words_files:
        name = words_name_get(file)

        line_count = line_count_get(file)

        if not line_count:
            line_count = 0

        curr_width = max_width - len(name)

        print(f"{name}{' ' * curr_width}: {line_count}")

#
# Handling the 'copy' command
#
def words_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Copy words")

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
        sys.exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Words already exists")
        sys.exit(0)

    subprocess.run(["cp", words_file, copy_file])

#
# Load words
#
def words_file_load(filepath):
    words = []

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                if(len(split_line) < 1):
                    return None

                word = split_line[0].strip().lower()

                words.append(word);

        return words

    except FileNotFoundError:
        print(f"korsord: Words file not found")
        return None

    except Exception as exception:
        print(f"korsord: Failed to read words file")
        return None

#
# Load words from files of inputted names
#
# grid-gen.py has this same function, maybe add it to common?
#
def words_load(words_names):
    words = set()

    for words_name in words_names:
        print(f"Loading words: {words_name}")

        words_file = words_file_get(words_name)

        curr_words = words_file_load(words_file)

        if not curr_words:
            print(f"Failed to load words: {words_name}")
            continue

        words.update(curr_words)

    return list(words)

#
# Save words
#
def words_save(words, filepath):
    try:
        with open(filepath, 'w') as file:
            for word in words:
                file.write(f"{word}\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Handling the 'merge' command
#
def words_merge(extra_args):
    merge_parser = argparse.ArgumentParser(description="Merge words")

    merge_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of merged words"
    )

    merge_parser.add_argument('words',
        type=str,
        help="Words to merge"
    )

    merge_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing words"
    )

    merge_args = merge_parser.parse_args(extra_args)

    if merge_args.words:
        merge_args.words = merge_args.words.split(' ')

    words = words_load(merge_args.words)

    words_file = words_file_get(merge_args.name)

    if os.path.exists(words_file) and not merge_args.force:
        print(f"korsord: {merge_args.name}: Words already exists")
        sys.exit(0)

    words_save(words, words_file)

#
# Handling the 'filter' command
#
def words_filter(extra_args):
    filter_parser = argparse.ArgumentParser(description="Filter words")

    filter_parser.add_argument('words',
        type=str,
        help="Words to filter"
    )

    filter_parser.add_argument('--length',
        type=int, default=None,
        help="Max length of word"
    )

    filter_parser.add_argument('--pattern',
        type=str, default=None,
        help="Word must match pattern"
    )

    filter_parser.add_argument('--save',
        type=str, default=None,
        help="Name of filterd words"
    )

    filter_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing words"
    )

    filter_args = filter_parser.parse_args(extra_args)

    if not filter_args.words:
        filter_parser.print_help()
        sys.exit(0)

    filter_args.words = filter_args.words.split(' ')

    words = words_load(filter_args.words)

    pattern = f"^{filter_args.pattern}$" if filter_args.pattern else None

    filtered_words = []

    for word in words:
        if pattern and not re.match(pattern, word):
            continue

        if filter_args.length and len(word) > filter_args.length:
            continue

        filtered_words.append(word)

    print(f"Filtered words: {len(filtered_words)}")

    if filter_args.save:
        words_file = words_file_get(filter_args.save)

        if os.path.exists(words_file) and not filter_args.force:
            print(f"korsord: {filter_args.save}: Words already exists")
            sys.exit(0)

        words_save(filtered_words, words_file)

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Words handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, view, edit, new, del, copy, list, merge, filter"
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

    elif args.command == "view":
        words_view(extra_args)

    elif args.command == "new":
        words_new(extra_args)

    elif args.command == "del":
        words_del(extra_args)

    elif args.command == "list":
        words_list(extra_args)

    elif args.command == "copy":
        words_copy(extra_args)

    elif args.command == "merge":
        words_merge(extra_args)

    elif args.command == "filter":
        words_filter(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
