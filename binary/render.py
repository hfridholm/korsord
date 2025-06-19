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
from shutil import which

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

    del_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of render"
    )

    del_args = del_parser.parse_args(extra_args)

    render_dir = render_dir_get(del_args.name)

    if not os.path.exists(render_dir):
        print(f"korsord: {del_args.name}: Render not found")
        sys.exit(0)

    subprocess.run(["rm", "-r", render_dir])

#
# Handling the 'view' command
#
def render_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View render")

    view_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of render"
    )

    view_parser.add_argument('--type',
        type=str, default="normal",
        help="Type of render"
    )

    view_args = view_parser.parse_args(extra_args)

    render_file = render_file_get(view_args.name, view_args.type)

    if not os.path.exists(render_file):
        print(f"korsord: {render_file}: Render not found")
        sys.exit(0)

    if which("xdg-open") is None:
        print(f"Error: Command 'xdg-open' not found")
        sys.exit(1)

    subprocess.run(["xdg-open", render_file])

#
# Handling the 'list' command
#
def render_list(extra_args):
    renders = []

    for render in os.listdir(RENDER_DIR):
        render_dir = render_dir_get(render)

        if os.path.isdir(render_dir):
            renders.append(render)

    if len(renders) == 0:
        print(f"No renders exist")
        sys.exit(0)

    max_width = 0

    for name in renders:
        max_width = max(max_width, len(name) + 1)

    for name in renders:
        curr_width = max_width - len(name)

        print(f"{name}{' ' * curr_width}")

#
# Handling the 'copy' command
#
def render_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Copy render")

    copy_parser.add_argument('--name',
        type=str, default="temp",
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

    render_dir = render_dir_get(copy_args.name)

    if os.path.isdir(os.path.dirname(copy_args.copy)):
        copy_dir = copy_args.copy

    else:
        copy_dir = render_dir_get(copy_args.copy)

    if not os.path.exists(render_dir):
        print(f"korsord: {copy_args.name}: Render not found")
        sys.exit(0)

    if os.path.exists(copy_dir) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Render already exists")
        sys.exit(0)

    subprocess.run(["cp", "-r", render_dir, copy_dir])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Render handler",
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
