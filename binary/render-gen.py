#
# render-gen.py - render crossword to image
#
# Written by Hampus Fridholm
#

from PIL import Image, ImageDraw, ImageFont, ImageColor
import textwrap
import os
import string
import random
import argparse
from common import *

MAX_LENGTH = 30
WRAP_WIDTH = 10

LETTER_FONT_SIZE = 100
NUMBER_FONT_SIZE = 40
CLUE_FONT_SIZE   = 25

LINE_MARGIN = 10
SQUARE_SIZE = 200
LINE_WIDTH  = 2

#
# Square struct
#
class Square:
    def __init__(self, square_type, letter=None):
        self.type   = square_type
        self.letter = letter

#
# Grid struct
#
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
def grid_load(grid_name):
    file = None

    try:
        grid_file = grid_file_get(grid_name)

        file = open(grid_file, 'r')

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
            if symbol == 'X':
                grid.squares[x][y] = Square("BORDER")

            elif symbol == '.':
                grid.squares[x][y] = Square("EMPTY")

            elif symbol == '#':
                grid.squares[x][y] = Square("BLOCK")

            else:
                letter = symbol.lower()

                if letter.isalpha() and letter.isascii():
                    grid.squares[x][y] = Square("LETTER", letter)

                else: # Invalid symbols in grid
                    print(f"Invalid symbol ({x}, {y}): '{symbol}'")
                    return None

    return grid

#
# Load font
#
def font_load(font_name):
    global CLUE_FONT, LETTER_FONT, NUMBER_FONT

    font_file = font_file_get(font_name)

    try:
        CLUE_FONT   = ImageFont.truetype(font_file, CLUE_FONT_SIZE)

        LETTER_FONT = ImageFont.truetype(font_file, LETTER_FONT_SIZE)

        NUMBER_FONT = ImageFont.truetype(font_file, NUMBER_FONT_SIZE)

        return True

    except IOError:
        print(f"Failed to load font: {font_name}")
        return False

#
# Load clues from file
#
def clues_file_load(filepath):
    clues = {}

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                if len(split_line) < 2:
                    return None

                word = split_line[0].strip().lower()
                clue = split_line[1].strip()

                if len(clue) > MAX_LENGTH:
                    print(f"Clue is too long: ({clue})")
                    return None

                clues[word] = clue;

    except FileNotFoundError:
        print(f"Clues file not found")
        return None

    except Exception as exception:
        print(f"Failed to read clues file")
        return None

    return clues

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
# Load words from files
#
def words_files_load(words_names):
    words_files = {}

    for words_name in words_names:
        print(f"Loading words: {words_name}")

        words_file = words_file_get(words_name)

        curr_words = words_file_load(words_file)

        if not curr_words:
            print(f"Failed to load words: {words_name}")
            continue

        words_files[words_name] = curr_words

    return words_files

#
# Load clues from files
#
def clues_load(clues_names):
    clues = {}

    for clues_name in clues_names:
        print(f"Loading clues: {clues_name}")

        clues_file = clues_file_get(clues_name)

        curr_clues = clues_file_load(clues_file)

        if not curr_clues:
            print(f"Failed to load clues: {clues_name}")
            continue

        for word, clue in curr_clues.items():
            if word not in clues:
                clues[word] = clue

    return clues

#
# Save words and their clues
#
def clues_save(clues, clues_name):
    # 1. Get max width for alignment
    max_width = 0

    for word in clues.keys():
        max_width = max(max_width, len(word) + 1)

    try:
        clues_file = clues_file_get(clues_name)

        with open(clues_file, 'w') as file:
            for word, clue in clues.items():
                curr_width = max_width - len(word)

                file.write(f"{word}{' ' * curr_width}: {clue}\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Save missing clues by appending them to missing.clues
#
def missing_clues_save(missing_clues):
    clues = clues_load(["missing"])

    for word in missing_clues:
        if word not in clues.keys():
            clues[word] = ""

    clues_save(clues, "missing")

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

            if square.type == "EMPTY":
                word = ""

            elif square.type == "BLOCK":
                if len(word) > 1:
                    if (x, y) in block_words:
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif square.type == "LETTER":
                word = square.letter + word

            if (x == 0 or grid.squares[x - 1][y].type == "BORDER") and (y < grid.height - 1):
                # The clue square is one below
                if len(word) > 1:
                    if (x, y + 1) in block_words:
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

            if square.type == "EMPTY":
                word = ""

            elif square.type == "BLOCK":
                if len(word) > 1:
                    if (x, y) in block_words:
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif square.type == "LETTER":
                word = square.letter + word

            if (y == 0 or grid.squares[x][y - 1].type == "BORDER") and x > 0:
                # The clue square is one to the left
                if len(word) > 1:
                    if (x - 1, y) in block_words:
                        block_words[(x - 1, y)].append(word)

                    else:
                        block_words[(x - 1, y)] = [word]

                word = ""

    return block_words

#
# Draw the outline for a square
#
def square_draw(draw, x, y, square_type, half=False):
    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    draw.rectangle([x, y, x + w, y + h], fill="white", outline="black", width=LINE_WIDTH)

#
#
# Draw outline by shifting text slightly in multiple directions
#
def text_draw(draw, x, y, text, color, font, outline_thickness=0, outline_color="black"):
    for dx in range(-outline_thickness, outline_thickness + 1):
        for dy in range(-outline_thickness, outline_thickness + 1):
            if dx != 0 or dy != 0:
                draw.text((x + dx, y + dy), text, font=font, fill=outline_color)
    
    draw.text((x, y), text, fill=color, font=font)

#
# Draw the text for the clue
#
def clue_draw(draw, x, y, clue, color, half=False, outline_thickness=0, outline_color="black"):
    global CLUE_FONT

    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    # Calculate text size
    bbox = draw.textbbox((0, 0), clue, font=CLUE_FONT)

    text_h = bbox[3] - bbox[1]

    wrapped_text = textwrap.fill(clue, width=WRAP_WIDTH, break_long_words=True)

    line_amount = len(wrapped_text.split("\n"))

    # Height of text lines
    lines_h = text_h + (line_amount - 1) * (text_h + LINE_MARGIN)

    text_y = y + max(0, (h - lines_h) // 2)

    # Draw the wrapped text line by line
    for index, line in enumerate(wrapped_text.split("\n")):
        # Calculate text size
        bbox = draw.textbbox((0, 0), line, font=CLUE_FONT)

        # Calculate width and height of the text
        text_w = bbox[2] - bbox[0]
        text_h = bbox[3] - bbox[1]

        text_x = x + max(0, (w - text_w) // 2)

        # Draw the text on the image
        text_draw(draw, text_x, text_y, line, color, CLUE_FONT, outline_thickness, outline_color) 

        text_y += text_h + LINE_MARGIN

#
# Draw letter in grid
#
def letter_draw(draw, x, y, letter, color, outline_thickness=0, outline_color="black"):
    global LETTER_FONT

    w = SQUARE_SIZE
    h = SQUARE_SIZE

    text = letter.upper()

    # Calculate text size
    bbox = draw.textbbox((0, 0), text, font=LETTER_FONT)

    text_w = bbox[2] - bbox[0]
    text_h = bbox[3] - bbox[1]

    text_x = x + max(0, (w - text_w) // 2)
    text_y = y + max(0, (h - text_h) // 2)

    text_draw(draw, text_x, text_y, text, color, LETTER_FONT, outline_thickness, outline_color)

#
# Get color of word
#
# If there are fewer colors than files, the first color is used as default
#
def word_color_get(words_files, word):
    if not words_files:
        return "black"

    colors = ["red", "orange", "yellow"]

    files = list(words_files.keys())

    for file, words in words_files.items():
        if word in words:
            file_index = files.index(file)

            color_index = max(0, len(colors) - file_index - 1)

            color = colors[color_index]

            return color

    return "black"

#
# Draw crossword grid
#
def grid_draw(draw, grid, block_words):
    for x in range(grid.width):
        for y in range(grid.height):
            square = grid.squares[x][y]

            if square.type == "BORDER":
                continue

            img_x = (x * SQUARE_SIZE)
            img_y = (y * SQUARE_SIZE)

            # Draw grid square
            square_draw(draw, img_x, img_y, square.type)

            # Square is not block
            if (x, y) not in block_words:
                continue

            words = block_words[(x, y)]

            # Clue square is divided in two   
            if len(words) > 1:
                for index, word in enumerate(words):
                    img_x = (x * SQUARE_SIZE)
                    img_y = (y * SQUARE_SIZE) if index == 0 else ((y + 1/2) * SQUARE_SIZE)

                    square_draw(draw, img_x, img_y, "BLOCK", half=True)

#
# Draw clues in grid
#
def clues_draw(draw, grid, block_words, clues, words_files):
    is_complete = True

    for x in range(grid.width):
        for y in range(grid.height):
            square = grid.squares[x][y]

            if square.type == "BORDER":
                continue

            img_x = (x * SQUARE_SIZE)
            img_y = (y * SQUARE_SIZE)

            # Square is not block
            if (x, y) not in block_words:
                continue

            words = block_words[(x, y)]

            # Clue square is divided in two   
            if len(words) > 1:
                for index, word in enumerate(words):
                    img_x = (x * SQUARE_SIZE)
                    img_y = (y * SQUARE_SIZE) if index == 0 else ((y + 1/2) * SQUARE_SIZE)

                    if word not in clues:
                        is_complete = False
                        continue

                    clue = clues[word]

                    color = word_color_get(words_files, word)

                    if clue:
                        clue_draw(draw, img_x, img_y, clue, color, half=True)

            # Clue square has only one word
            else:
                word = words[0]

                if word not in clues:
                    is_complete = False
                    continue

                clue = clues[word]

                color = word_color_get(words_files, word)

                if clue:
                    clue_draw(draw, img_x, img_y, clue, color)

    return is_complete

#
# Get path to new result directory
#
# def new_result_dir_get():
#     count = 1
#     new_result_dir = f"korsord{count}"
# 
#     while os.path.exists(f"{args.results_dir}/{new_result_dir}"):
#         count += 1
#         new_result_dir = f"korsord{count}"
# 
#     return new_result_dir

#
# Draw helping letter number
#
def number_draw(draw, x, y, number):
    global NUMBER_FONT

    text = str(number)

    bbox = draw.textbbox((0, 0), text, font=NUMBER_FONT)

    text_w = bbox[2] - bbox[0]

    text_x = x + SQUARE_SIZE - text_w - 10
    text_y = y + 10

    text_draw(draw, text_x, text_y, text, "gray", NUMBER_FONT)

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

#
# Draw helping letter numbers
#
def numbers_draw(draw, grid):
    numbers = numbers_gen()

    for x in range(grid.width):
        for y in range(grid.height):
            square = grid.squares[x][y]

            if square.type != "LETTER":
                continue

            number = numbers[square.letter]

            img_x = (x * SQUARE_SIZE)
            img_y = (y * SQUARE_SIZE)

            # Draw letter number
            number_draw(draw, img_x, img_y, number)

#
# Draw direction arrow from clue square
#
def arrow_draw(img, x, y, direction):
    arrow_file = os.path.join(BASE_DIR, "../assets/images/arrow.png")

    try:
        arrow_image = Image.open(arrow_file)

        if direction == "down":
            img_x = (x * SQUARE_SIZE)
            img_y = (y * SQUARE_SIZE)

            arrow_image = arrow_image.rotate(0, expand=True)

            img.paste(arrow_image, (img_x, img_y), arrow_image)

        if direction == "right":
            img_x = (x * SQUARE_SIZE)
            img_y = ((y + 1) * SQUARE_SIZE) - 60

            arrow_image = arrow_image.rotate(90, expand=True)

            img.paste(arrow_image, (img_x, img_y), arrow_image)

    except:
        print(f"korsord: Arrow image not found")

#
# Draw direction arrows from clue squares
#
def arrows_draw(img, grid, block_words):
    for y in range(grid.height):
        for x in range(grid.width):
            square = grid.squares[x][y]

            if square.type != "LETTER":
                continue

            if y > 0:
                square_over = grid.squares[x][y - 1]

                if square_over.type == "BORDER":
                    arrow_draw(img, x, y, "down")

            else:
                arrow_draw(img, x, y, "down")

            if x > 0:
                square_left = grid.squares[x - 1][y]

                if square_left.type == "BORDER":
                    arrow_draw(img, x, y, "right")

            else:
                arrow_draw(img, x, y, "right")

#
# Draw in grid letters
#
def letters_draw(draw, grid):
    for x in range(grid.width):
        for y in range(grid.height):
            square = grid.squares[x][y]

            if square.type == "LETTER":
                img_x = (x * SQUARE_SIZE)
                img_y = (y * SQUARE_SIZE)

                letter_draw(draw, img_x, img_y, square.letter, "black")

#
# Get the words which clues are missing
#
def missing_clues_get(block_words, clues):
    missing_clues = []

    for block in block_words:
        for word in block_words[block]:
            if word not in clues.keys() or not clues[word]:
                missing_clues.append(word)

    return missing_clues

#
# Draw image behind grid
#
def image_draw(img, grid, image_name):
    min_x = grid.width  - 1
    max_x = 0

    min_y = grid.height - 1
    max_y = 0

    img_exists = False

    for x in range(0, grid.width, 1):
        for y in range(0, grid.height, 1):
            square = grid.squares[x][y]

            if square.type != "BORDER":
                continue

            img_exists = True

            min_x = min(min_x, x)
            max_x = max(max_x, x + 1)

            min_y = min(min_y, y)
            max_y = max(max_y, y + 1)
        
    if not img_exists:
        print(f"korsord: Image doesn't exist")
        return

    image_file = image_file_get(image_name)

    try:
        image = Image.open(image_file)

        box_width  = SQUARE_SIZE * (max_x - min_x)
        box_height = SQUARE_SIZE * (max_y - min_y)

        original_width, original_height = image.size

        scaling_factor = max(box_width / original_width, box_height / original_height)

        new_width  = int(original_width  * scaling_factor)
        new_height = int(original_height * scaling_factor)

        resized_image = image.resize((new_width, new_height))

        img_x = min_x * SQUARE_SIZE + (box_width  - new_width)  // 2
        img_y = min_y * SQUARE_SIZE + (box_height - new_height) // 2

        print(f"image_draw {img_x}, {img_y} {new_width}x{new_height}")

        img.paste(resized_image, (img_x, img_y))

    except:
        print(f"korsord: Image {image_name} not found")

#
# Initialize image
#
def image_init(width, height):
    img_w = width  * SQUARE_SIZE
    img_h = height * SQUARE_SIZE

    img = Image.new('RGBA', (img_w, img_h), color=(0, 0, 0, 0))

    return img

#
# Save image with name
#
def image_save(img, name):
    file = render_file_get(name)

    img.save(file, "PNG")

#
# Main function
#
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="render grid to image")

    parser.add_argument("--grid",
        type=str, default="temp",
        help="Name of grid"
    )

    parser.add_argument("--clues",
        type=str, default="temp",
        help="Name of clues"
    )

    parser.add_argument("--words",
        type=str, default=None,
        help="Name of words"
    )

    parser.add_argument("--partial",
        action='store_true',
        help="Allow clues to be missing"
    )

    parser.add_argument("--image",
        type=str, default=None,
        help="Name of image"
    )

    args = parser.parse_args()

    if args.words:
        args.words = args.words.split(' ')

    if args.clues:
        args.clues = args.clues.split(' ')

    # Load grid
    print(f"Loading grid")

    grid = grid_load(args.grid)

    if not grid:
        print(f"Failed to load grid")
        sys.exit(1)

    print(f"Loaded grid")



    print(f"Finding words in grid")

    # Get blocks and their words
    block_words = block_words_find(grid)

    if not block_words:
        print(f"Failed to find block words")
        sys.exit(2)

    print(f"Found words in grid")



    print(f"Loading clues")

    clues = clues_load(args.clues)

    
    print(f"Checking missing clues")

    missing_clues = missing_clues_get(block_words, clues)

    if not args.partial and missing_clues:
        print(f"korsord: Some clues are missing")
        print(missing_clues)
        missing_clues_save(missing_clues)
        sys.exit(3)


    if not os.path.exists(RENDER_DIR):
        os.makedirs(RENDER_DIR)


    print(f"Loading font")

    if not font_load("Ubuntu-B"):
        print(f"korsord: Failed to load font")
        sys.exit(4)

    print(f"Loaded font")


    img = image_init(grid.width, grid.height)

    draw = ImageDraw.Draw(img)

    
    if args.image:
        print(f"Drawing image")

        image_draw(img, grid, args.image)

        print(f"Drew image")
    

    print(f"Drawing grid")

    grid_draw(draw, grid, block_words)

    print(f"Drew grid")


    print(f"Drawing clues")

    is_complete = clues_draw(draw, grid, block_words, clues, None)

    if not args.partial and not is_complete:
        print(f"korsord: Some clues were missing")
        sys.exit(5);
    
    print(f"Drew clues")


    print(f"Drawing arrows")

    arrows_draw(img, grid, block_words)

    print(f"Drew arrows")


    image_save(img, "normal")

    print(f"Saved normal crossword image")


    print(f"Drawing letter numbers")

    numbers_draw(draw, grid)

    print(f"Drew letter numbers")

    image_save(img, "helping")

    print(f"Saved helping crossword image")


    print(f"Drawing clues")

    words_files = words_files_load(args.words)

    is_complete = clues_draw(draw, grid, block_words, clues, words_files)

    if not args.partial and not is_complete:
        print(f"korsord: Some clues were missing")
        sys.exit(5);
    
    print(f"Drew clues")


    print(f"Filling in words")

    letters_draw(draw, grid)

    print(f"Filled in words")

    image_save(img, "solved")

    print(f"Saved solved crossword image")
