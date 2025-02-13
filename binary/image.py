#
# image.py - commands for managing images
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

IMAGES_DIR = os.path.join(CONFIG_DIR, "images")

#
# Get the file path of a image by name
#
def image_file_get(name):
    return os.path.join(IMAGES_DIR, f"{name}.png")

#
# Get the name of a image file
#
def image_name_get(file):
    return os.path.splitext(file.replace(IMAGES_DIR, ''))[0].strip('/')

#
# Handling the 'gen' command
#
def image_gen(extra_args):
    gen_script = os.path.join(BASE_DIR, "image-gen.py")

    if not os.path.isfile(gen_script):
        print(f"korsord: {gen_script}: File not found")
        sys.exit(1)

    subprocess.run(["python", gen_script] + extra_args)

#
# Handling the 'view' command
#
def image_view(extra_args):
    view_parser = argparse.ArgumentParser(description="View image")

    view_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of image"
    )

    view_args = view_parser.parse_args(extra_args)

    image_file = image_file_get(view_args.name)

    if not os.path.exists(image_file):
        print(f"korsord: {view_args.name}: Image not found")
        exit(0)

    subprocess.run(["feh", image_file])

#
# Handling the 'del' command
#
def image_del(extra_args):
    del_parser = argparse.ArgumentParser(description="Delete image")

    del_parser.add_argument('name',
        type=str, default="temp",
        help="Name of image"
    )

    del_args = del_parser.parse_args(extra_args)

    image_file = image_file_get(del_args.name)

    if not os.path.exists(image_file):
        print(f"korsord: {del_args.name}: Image not found")
        exit(0)

    subprocess.run(["rm", image_file])

#
# Handling the 'list' command
#
def image_list(extra_args):
    files_exist = False

    for root, dirs, files in os.walk(IMAGES_DIR):
        for file in files:
            if file.endswith('.png'):
                files_exist = True

                image_file = os.path.join(root, file)

                print(f"{image_name_get(image_file)}")

    if not files_exist:
        print(f"No image exist")

#
# Handling the 'copy' command
#
def image_copy(extra_args):
    copy_parser = argparse.ArgumentParser(description="Save copy of image")

    copy_parser.add_argument('--name',
        type=str, default="temp",
        help="Name of image"
    )

    copy_parser.add_argument('copy',
        type=str,
        help="Name of copy"
    )

    copy_parser.add_argument('--force',
        action='store_true',
        help="Overwrite existing image"
    )

    copy_args = copy_parser.parse_args(extra_args)

    image_file = image_file_get(copy_args.name)
    copy_file = image_file_get(copy_args.copy)

    if not os.path.exists(image_file):
        print(f"korsord: {copy_args.name}: Image not found")
        exit(0)

    if os.path.exists(copy_file) and not copy_args.force:
        print(f"korsord: {copy_args.copy}: Image already exists")
        exit(0)

    subprocess.run(["cp", image_file, copy_file])

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(
        description="Crossword image handler",
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

    # Ensure that IMAGES_DIR is set correctly and exists
    if not os.path.exists(IMAGES_DIR):
        print(f"korsord: {IMAGES_DIR}: Directory not found")
        sys.exit(1)

    # Parse command
    if args.command == "gen":
        image_gen(extra_args)

    elif args.command == "view":
        image_view(extra_args)

    elif args.command == "del":
        image_del(extra_args)

    elif args.command == "list":
        image_list(extra_args)

    elif args.command == "copy":
        image_copy(extra_args)

    else:
        print(f"korsord: {args.command}: Command not found")
