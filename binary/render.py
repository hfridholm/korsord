#
# render.py - commands for managing rendered images
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
def render_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "render-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'del' command
#
def render_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete render")

    del_parser.add_argument('name',
        type=str,
        help="Name of render"
    )

    del_args = del_parser.parse_args(extra_args)

    render_file = render_file_get(del_args.name)

    if not os.path.exists(render_file):
        print(f"korsord: {del_args.name}: Image not found")
        sys.exit(0)

    subprocess.run(["rm", render_file])

#
# Handling the 'view' command
#
def render_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View render")

    view_parser.add_argument('--name',
        type=str, default="normal",
        help="Name of render"
    )

    view_args = view_parser.parse_args(extra_args)

    render_file = render_file_get(view_args.name)

    if not os.path.exists(render_file):
        print(f"korsord: {view_args.name}: Image not found")
        sys.exit(0)

    subprocess.run(["xdg-open", render_file])

#
# Get all render files
#
def render_files_get():
    render_files = []

    for root, dirs, files in os.walk(RENDER_DIR):
        for file in files:
            if file.endswith('.png'):
                render_file = os.path.join(root, file)

                render_files.append(render_file)

    return render_files

#
# Handling the 'list' command
#
def render_list(extra_args):
    render_files = render_files_get()

    if len(render_files) == 0:
        print(f"No images exist")
        sys.exit(0)

    max_width = 0

    for file in render_files:
        name = render_name_get(file)

        max_width = max(max_width, len(name) + 1)

    for file in render_files:
        name = render_name_get(file)

        curr_width = max_width - len(name)

        print(f"{name}{' ' * curr_width}")

#
# Handling the 'copy' command
#
def render_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of render")

    copy_parser.add_argument('--name',
        type=str, default="normal",
        help="Name of render"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing render"
    )

    copy_args = copy_parser.parse_args(extra_args)

    render_file = render_file_get(copy_args.name)

    if string_is_file(copy_args.copy):
        copy_file = copy_args.copy

    else:
        copy_file = render_file_get(copy_args.copy)

    if not os.path.exists(render_file):
        print(f"korsord: {copy_args.name}: Image not found")
        sys.exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Image already exists")
        sys.exit(0)

    subprocess.run(["cp", render_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Crossword render handler",
        add_help=False
    )

    parser.add_argument("command",
        nargs="?",
        help="gen, view, del, copy, list"
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

    # Ensure that RENDER_DIR is set correctly and exists
    if not os.path.exists(RENDER_DIR):
        print(f"korsord: {RENDER_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        render_gen(extra_args)

    elif args.command == "view":
        render_view(extra_args)

    elif args.command == "del":
        render_del(extra_args)

    elif args.command == "list":
        render_list(extra_args)

    elif args.command == "copy":
        render_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
