#
# render.py - render crossword to image
#
# Written by Hampus Fridholm
#

# Import image library
from PIL import Image, ImageDraw, ImageFont
import textwrap
import os
import string
import random
import argparse

MAX_LENGTH = 30
WRAP_WIDTH = 10

LETTER_FONT_SIZE = 100
NUMBER_FONT_SIZE = 40
CLUE_FONT_SIZE   = 25

LINE_MARGIN = 10
SQUARE_SIZE = 200
LINE_WIDTH  = 2

#
# Parse command line arguments
#
parser = argparse.ArgumentParser(description="render exported crossword to images")

parser.add_argument("--results-dir", type=str, help="Directory to store crosswords")
parser.add_argument("--result-dir",  type=str, help="Directory name of saved images")

parser.add_argument("--font-dir",    type=str, help="Directory to font")
parser.add_argument("--font-file",   type=str, help="File name of font")

args = parser.parse_args()

RESULTS_DIR = args.results_dir if args.results_dir else "results"
RESULT_DIR  = args.result_dir  if args.result_dir  else "korsord"

FONT_DIR  = args.font_dir  if args.font_dir  else "../assets/fonts"
FONT_FILE = args.font_file if args.font_file else "Ubuntu-B.ttf"

#
# Square struct
#
class Square:
    def __init__(self, square_type, letter=None):
        self.type   = square_type
        self.letter = letter

class Grid:
    def __init__(self, width, height):
        self.width  = width
        self.height = height

        self.squares = []

        for x in range(width):
            self.squares.append([])

            for y in range(height):
                self.squares[x].append(Square("EMPTY"))

#
# Load crossword grid
#
def grid_load(filepath):
    file = None

    try:
        file = open(filepath, 'r')

    except FileNotFoundError:
        print(f"Grid file not found")
        return None

    except Exception as exception:
        print(f"Failed to read grid file")
        return None

    # Get width and height of grid
    width  = 0
    height = 0

    for line in file.readlines():
        width = max(width, len(line) // 2)
        height += 1

    # Extract squares of grid
    grid = Grid(width, height)

    # Go back to the beginning of the file
    file.seek(0)

    for y, line in enumerate(file.readlines()):
        # Get every other symbol of line (grid symbols)
        for x, symbol in enumerate(line[::2]):
            if (symbol == 'X'):
                grid.squares[x][y] = Square("BORDER")

            elif (symbol == '.'):
                grid.squares[x][y] = Square("EMPTY")

            elif (symbol == '#'):
                grid.squares[x][y] = Square("BLOCK")

            else:
                letter = symbol.lower()

                if(letter.isalpha() and letter.isascii()):
                    grid.squares[x][y] = Square("LETTER", letter)

                else: # Invalid symbols in grid
                    print(f"Invalid symbol ({x}, {y}): '{symbol}'")
                    return None

    return grid

#
# Load words and their clues in a dictionary
#
def word_clues_load(filepath):
    word_clues = {}

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                if(len(split_line) < 2):
                    return None

                word = split_line[0].strip().lower()
                clue = split_line[1].strip()

                if(len(clue) > MAX_LENGTH):
                    print(f"Clue is too long: ({clue})")
                    return None

                word_clues[word] = clue;

    except FileNotFoundError:
        print(f"Words file not found")
        return None

    except Exception as exception:
        print(f"Failed to read words file")
        return None

    return word_clues

#
# Find blocks and their words
#
def block_words_find(grid):
    block_words = {}

    # Extract horizontal words
    #
    # By going from the top to the bottom (y getting bigger),
    # the double clue squares becomes correct
    #
    for y in range(grid.height):
        word = ""

        for x in range(grid.width - 1, -1, -1):
            square = grid.squares[x][y]

            if(square.type == "EMPTY"):
                word = ""

            elif(square.type == "BLOCK"):
                if(len(word) > 1):
                    if((x, y) in block_words):
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif (square.type == "LETTER"):
                word = square.letter + word

            if ((x == 0 or grid.squares[x - 1][y].type == "BORDER") and y < grid.height - 1):
                # The clue square is one below
                if(len(word) > 1):
                    if((x, y + 1) in block_words):
                        block_words[(x, y + 1)].append(word)

                    else:
                        block_words[(x, y + 1)] = [word]

                word = ""

    # Extract vertical words
    #
    # By going from right to left (x getting smaller),
    # the double clue squares becomes correct
    #
    for x in range(grid.width - 1, -1, -1):
        word = ""

        for y in range(grid.height - 1, -1, -1):
            square = grid.squares[x][y]

            if(square.type == "EMPTY"):
                word = ""

            elif (square.type == "BLOCK"):
                if(len(word) > 1):
                    if((x, y) in block_words):
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif(square.type == "LETTER"):
                word = square.letter + word

            if ((y == 0 or grid.squares[x][y - 1].type == "BORDER") and x > 0):
                # The clue square is one to the left
                if(len(word) > 1):
                    if((x - 1, y) in block_words):
                        block_words[(x - 1, y)].append(word)

                    else:
                        block_words[(x - 1, y)] = [word]

                word = ""

    return block_words

print(f"Loading grid {'result.grid'}")

#
# Load grid
#
grid = grid_load("result.grid")

if(grid == None):
    print(f"Failed to load grid")
    exit(1)

print(f"Loaded grid {'result.grid'}")

print(f"Finding words in grid")

# Get blocks and their words
block_words = block_words_find(grid)

if(block_words == None):
    print(f"Failed to find block words")
    exit(2)

print(f"Found words in grid")

print(f"Loading word clues {'result.words'}")

# Load words along with threir clues
word_clues = word_clues_load("result.words")

if(word_clues == None):
    print(f"Failed to load clues")
    exit(3)

print(f"Loaded word clues {'result.words'}")

# Initialize img, draw and font variables
img_w = grid.width  * SQUARE_SIZE
img_h = grid.height * SQUARE_SIZE

img = Image.new('RGBA', (img_w, img_h), color=(0, 0, 0, 0))
draw = ImageDraw.Draw(img)

FONT_PATH = f"{FONT_DIR}/{FONT_FILE}"

print(f"Loading font {FONT_PATH}")

try:
    clue_font = ImageFont.truetype(FONT_PATH, CLUE_FONT_SIZE)

    letter_font = ImageFont.truetype(FONT_PATH, LETTER_FONT_SIZE)

    number_font = ImageFont.truetype(FONT_PATH, NUMBER_FONT_SIZE)

except IOError:
    print(f"Failed to load font")
    exit(1)

print(f"Loaded font {FONT_PATH}")

#
# Draw the outline for a square
#
def square_draw(x, y, square_type, half=False):
    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    draw.rectangle([x, y, x + w, y + h], fill="white", outline="black", width=LINE_WIDTH)

#
# Draw the text for the clue
#
def clue_draw(x, y, clue, half=False):
    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    # Calculate text size
    bbox = draw.textbbox((0, 0), clue, font=clue_font)

    text_h = bbox[3] - bbox[1]

    wrapped_text = textwrap.fill(clue, width=WRAP_WIDTH, break_long_words=True)

    line_amount = len(wrapped_text.split("\n"))

    # Height of text lines
    lines_h = text_h + (line_amount - 1) * (text_h + LINE_MARGIN)

    text_y = y + max(0, (h - lines_h) // 2)

    # Draw the wrapped text line by line
    for index, line in enumerate(wrapped_text.split("\n")):
        # Calculate text size
        bbox = draw.textbbox((0, 0), line, font=clue_font)

        # Calculate width and height of the text
        text_w = bbox[2] - bbox[0]
        text_h = bbox[3] - bbox[1]

        text_x = x + max(0, (w - text_w) // 2)

        # Draw the text on the image
        draw.text((text_x, text_y), line, fill="black", font=clue_font)

        text_y += text_h + LINE_MARGIN

#
# Draw letter in grid
#
def letter_draw(x, y, letter):
    w = SQUARE_SIZE
    h = SQUARE_SIZE

    text = letter.upper()

    # Calculate text size
    bbox = draw.textbbox((0, 0), text, font=letter_font)

    text_w = bbox[2] - bbox[0]
    text_h = bbox[3] - bbox[1]

    text_x = x + max(0, (w - text_w) // 2)
    text_y = y + max(0, (h - text_h) // 2)

    draw.text((text_x, text_y), text, fill="red", font=letter_font)

print(f"Drawing word clues")

#
# Draw crossword grid
#
for x in range(grid.width):
    for y in range(grid.height):
        square = grid.squares[x][y]

        if(square.type == "BORDER"):
            continue

        img_x = (x * SQUARE_SIZE)
        img_y = (y * SQUARE_SIZE)

        # Draw grid square
        square_draw(img_x, img_y, square.type)

        # Square is not block
        if((x, y) not in block_words):
            continue

        words = block_words[(x, y)]

        # Clue square is divided in two   
        if(len(words) > 1):
            for index, word in enumerate(words):
                img_x = (x * SQUARE_SIZE)
                img_y = (y * SQUARE_SIZE) if index == 0 else ((y + 1/2) * SQUARE_SIZE)

                square_draw(img_x, img_y, "BLOCK", half=True)

                if(word not in word_clues):
                    continue

                clue = word_clues[word]

                if(clue):
                    clue_draw(img_x, img_y, clue, half=True)

        # Clue square has only one word
        else:
            word = words[0]

            if(word not in word_clues):
                continue

            clue = word_clues[word]

            if(clue):
                clue_draw(img_x, img_y, clue)

print(f"Drew word clues")

#
# Get path to new result directory
#
def new_result_dir_get():
    count = 1
    new_result_dir = f"korsord{count}"

    while os.path.exists(f"{RESULTS_DIR}/{new_result_dir}"):
        count += 1
        new_result_dir = f"korsord{count}"

    return new_result_dir

# Get the path to new result directory
if(RESULT_DIR == None):
    RESULT_DIR = new_result_dir_get()

if not os.path.exists(f"{RESULTS_DIR}/{RESULT_DIR}"):
    os.makedirs(f"{RESULTS_DIR}/{RESULT_DIR}")


img.save(f"{RESULTS_DIR}/{RESULT_DIR}/normal.png", "PNG")

print(f"Saved normal crossword image")

#
# Draw helping letter number
#
def number_draw(x, y, number):
    text = number

    text_x = x + 10
    text_y = y + 10

    draw.text((text_x, text_y), text, fill="gray", font=number_font)

#
# Create random numbers dictionary
#
def numbers_gen():
    ordered_numbers = {}

    for index, letter in enumerate(string.ascii_lowercase):
        ordered_numbers[letter] = str(index + 1)

    # Step 1: Extract the values and shuffle them
    values = list(ordered_numbers.values())

    random.shuffle(values)

    # Step 2: Reassign the shuffled values back to the dictionary
    return dict(zip(ordered_numbers.keys(), values))

numbers = numbers_gen()

print(f"Drawing letter numbers")

#
# Draw helping letter numbers
#
for x in range(grid.width):
    for y in range(grid.height):
        square = grid.squares[x][y]

        if(square.type != "LETTER"):
            continue

        number = numbers[square.letter]

        img_x = (x * SQUARE_SIZE)
        img_y = (y * SQUARE_SIZE)

        # Draw letter number
        number_draw(img_x, img_y, number)

print(f"Drew letter numbers")

img.save(f"{RESULTS_DIR}/{RESULT_DIR}/helping.png", "PNG")

print(f"Saved helping crossword image")

print(f"Filling in words")

# Create solved crossword image
for x in range(grid.width):
    for y in range(grid.height):
        square = grid.squares[x][y]

        img_x = (x * SQUARE_SIZE)
        img_y = (y * SQUARE_SIZE)

        if(square.type == "LETTER"):
            letter_draw(img_x, img_y, square.letter)

print(f"Filled in words")

# Save solved crossword image
img.save(f"{RESULTS_DIR}/{RESULT_DIR}/solved.png", "PNG")

print(f"Saved solved crossword image")
