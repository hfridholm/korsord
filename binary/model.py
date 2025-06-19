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

    new_parser.add_argument('--name',
        type=str, default="temp",
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

    del_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of model"
    )

    del_args = del_parser.parse_args(extra_args)

    model_file = model_file_get(del_args.name)

    if not os.path.exists(model_file):
        print(f"korsord: {del_args.name}: Model not found")
        sys.exit(0)

    subprocess.run(["rm", model_file])

#
# Handling the 'view' command
#
def model_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View model")

    view_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of model"
    )

    view_args = view_parser.parse_args(extra_args)

    model_file = model_file_get(view_args.name)

    if not os.path.exists(model_file):
        print(f"korsord: {view_args.name}: Model not found")
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
            print(f"korsord: {edit_args.name}: Model not found")
            sys.exit(0)

        else:
            subprocess.run(["touch", model_file])

    subprocess.run(["vim", model_file])

#
# Get all model files
#
def model_files_get():
    model_files = []

    for root, dirs, files in os.walk(MODELS_DIR):
        for file in files:
            if file.endswith('.model'):
                model_file = os.path.join(root, file)

                model_files.append(model_file)

    return model_files

#
# Handling the 'list' command
#
def model_list(extra_args):
    model_files = model_files_get()

    if len(model_files) == 0:
        print(f"No model exist")
        sys.exit(0)

    max_width = 0

    for file in model_files:
        name = model_name_get(file)

        max_width = max(max_width, len(name) + 1)

    for file in model_files:
        name = model_name_get(file)

        model_size = grid_size_get(file)

        if not model_size:
            model_size = [0, 0]

        curr_width = max_width - len(name)

        print(f"{name}{' ' * curr_width}: {model_size[0]}x{model_size[1]}")

#
# Handling the 'copy' command
#
def model_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Copy model")

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
        description="Model handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, view, edit, new, del, copy, list"
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

    elif args.command == "view":
        model_view(extra_args)

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
