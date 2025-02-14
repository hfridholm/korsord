#
# model.py - commands for managing models
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
def model_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "model-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'new' command
#
def model_new(extra_args):
    new_parser = argparse.ArgumentParser(description="New model")

    new_parser.add_argument('name',
        type=str,
        help="Name of new model"
    )

    new_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing model"
    )

    new_args = new_parser.parse_args(extra_args)

    new_file = model_file_get(new_args.name)

    if os.path.exists(new_file) and not new_args.force:
        print(f"korsord: {new_args.name}: Model already exists")
        sys.exit(0)

    subprocess.run(["touch", new_file])

#
# Handling the 'del' command
#
def model_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete model")

    del_parser.add_argument('name',
        type=str,
        help="Name of model"
    )

    del_args = del_parser.parse_args(extra_args)

    model_file = model_file_get(del_args.name)

    if not os.path.exists(model_file):
        print(f"korsord: {del_args.name}: Model not found")
        sys.exit(0)

    subprocess.run(["rm", model_file])

#
# Handling the 'show' command
#
def model_show(extra_args):
    show_parser = argparse.ArgumentParser(description="Show model")

    show_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of model"
    )

    show_args = show_parser.parse_args(extra_args)

    model_file = model_file_get(show_args.name)

    if not os.path.exists(model_file):
        print(f"korsord: {show_args.name}: Model not found")
        sys.exit(0)

    subprocess.run(["cat", model_file])

#
# Handling the 'edit' command
#
def model_edit(extra_args):
    edit_parser = argparse.ArgumentParser(description="Edit model")

    edit_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of model"
    )

    edit_parser.add_argument('--new',
        action='store_true',
        help="Create new model and edit it"
    )

    edit_args = edit_parser.parse_args(extra_args)

    model_file = model_file_get(edit_args.name)

    if not os.path.exists(model_file):
        if not edit_args.new:
            if edit_args.name == "temp":
                print(f"korsord: Model not found")

            else:
                print(f"korsord: {edit_args.name}: Model not found")

            sys.exit(0)

        else:
            subprocess.run(["touch", model_file])

    subprocess.run(["vim", model_file])

#
# Handling the 'list' command
#
def model_list(extra_args):
    files_exist = False

    for root, dirs, files in os.walk(MODELS_DIR):
        for file in files:
            if file.endswith('.model'):
                files_exist = True

                model_file = os.path.join(root, file)

                print(f"{model_name_get(model_file)}")

    if not files_exist:
        print(f"No models exist")

#
# Handling the 'copy' command
#
def model_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of model")

    copy_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of model"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing model"
    )

    copy_args = copy_parser.parse_args(extra_args)

    model_file = model_file_get(copy_args.name)
    copy_file = model_file_get(copy_args.copy)

    if not os.path.exists(model_file):
        print(f"korsord: {copy_args.name}: Model not found")
        sys.exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Model already exists")
        sys.exit(0)

    subprocess.run(["cp", model_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Crossword model handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, show, edit, new, del, copy, list"
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

    # Ensure that MODELS_DIR is set correctly and exists
    if not os.path.exists(MODELS_DIR):
        print(f"korsord: {MODELS_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        model_gen(extra_args)

    elif args.command == "show":
        model_show(extra_args)

    elif args.command == "edit":
        model_edit(extra_args)

    elif args.command == "new":
        model_new(extra_args)

    elif args.command == "del":
        model_del(extra_args)

    elif args.command == "list":
        model_list(extra_args)

    elif args.command == "copy":
        model_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
