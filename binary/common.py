#
# common.py - common functions for scripts
#
# Written by Hampus Fridholm
#

import sys
import os

# Base directory where the programs are located
BASE_DIR = os.path.dirname(os.path.abspath(__file__))

CONFIG_DIR = os.path.join(os.path.expanduser('~'), ".korsord")


PROMPT_DIR = os.path.join(BASE_DIR, "../assets/prompts")

#
# Get the file path of a prompt by name
#
def prompt_file_get(name):
    return os.path.join(PROMPT_DIR, f"{name}.txt")


FONT_DIR = os.path.join(BASE_DIR, "../assets/fonts")

#
# Get font file by name
#
def font_file_get(name):
    return os.path.join(FONT_DIR, f"{name}.ttf")


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


GRIDS_DIR = os.path.join(CONFIG_DIR, "grids")

#
# Get the file path of a grid by name
#
def grid_file_get(name):
    return os.path.join(GRIDS_DIR, f"{name}.grid")

#
# Get the name of a grid file
#
def grid_name_get(file):
    return os.path.splitext(file.replace(GRIDS_DIR, ''))[0].strip('/')


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


RENDER_DIR = os.path.join(CONFIG_DIR, "render")

#
# Get the file path of a render image by name
#
def render_file_get(name):
    return os.path.join(RENDER_DIR, f"{name}.png")


MODELS_DIR = os.path.join(CONFIG_DIR, "models")

#
# Get the file path of a model by name
#
def model_file_get(name):
    return os.path.join(MODELS_DIR, f"{name}.model")

#
# Get the name of a model file
#
def model_name_get(file):
    return os.path.splitext(os.path.basename(file))[0]


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
# Get number of lines in file
#
def line_count_get(path):
    try:
        with open(path, 'r') as file:
            return len(file.readlines())

    except Exception as exception:
        return None

#
# Get size of grid / model
#
# RETURN (width, height) or None
#
def grid_size_get(path):
    try:
        with open(path, 'r') as file:
            lines = file.readlines()

            if len(lines) == 0:
                return None

            height = len(lines)
            width = len(lines[0]) // 2

            for line in lines:
                width = max(width, len(line) // 2)

            return (width, height)

    except Exception as exception:
        return None

#
# Load placeholder prompt from file and substitute values
#
def prompt_load(prompt_name, values):
    prompt_file = prompt_file_get(prompt_name)

    try:
        with open(prompt_file, 'r') as file:
            prompt = file.read()

        # Substitute each placeholder with the corresponding value
        for placeholder, value in values.items():
            prompt = prompt.replace(str(placeholder), str(value))

        return prompt

    except FileNotFoundError:
        print(f"Error: The file '{prompt_file}' was not found.")
        return None

#
# Function to read the API key from a local file
#
def api_key_get(filepath):
    try:
        with open(filepath, 'r') as file:
            api_key = file.read().strip()

        return api_key

    except Exception as exception:
        print(f"Error reading API key: {exception}")
        return None
