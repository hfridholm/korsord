#
# render.py - render crossword to image
#
# Written by Hampus Fridholm
#

# Import image library
from PIL import Image, ImageDraw, ImageFont
import textwrap

FONT_FILE = "/usr/share/fonts/truetype/ubuntu/Ubuntu-B.ttf"

MAX_CLUE_LENGTH = 30
FONT_SIZE = 30
WRAP_WIDTH = 10
LINE_MARGIN = 10
SQUARE_SIZE = 200
LINE_WIDTH = 1

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

                if(len(clue) > MAX_CLUE_LENGTH):
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
    for y in range(grid.height):
        word = ""

        for x in range(grid.width - 1, -1, -1):
            square = grid.squares[x][y]

            if(square.type == "EMPTY"):
                return None

            elif(square.type == "BLOCK"):
                if(len(word) > 1):
                    if((x, y) in block_words):
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif (square.type == "LETTER"):
                word = square.letter + word

            if ((x == 0 or square.type == "BORDER") and y < grid.height - 1):
                # The clue square is one below
                if(len(word) > 1):
                    if((x, y + 1) in block_words):
                        block_words[(x, y + 1)].append(word)

                    else:
                        block_words[(x, y + 1)] = [word]

                word = ""

    # Extract vertical words
    for x in range(grid.width):
        word = ""

        for y in range(grid.height - 1, -1, -1):
            square = grid.squares[x][y]

            if(square.type == "EMPTY"):
                return None

            elif (square.type == "BLOCK"):
                if(len(word) > 1):
                    if((x, y) in block_words):
                        block_words[(x, y)].append(word)

                    else:
                        block_words[(x, y)] = [word]

                word = ""

            elif(square.type == "LETTER"):
                word = square.letter + word

            if ((y == 0 or square.type == "BORDER") and x > 0):
                # The clue square is one to the left
                if(len(word) > 1):
                    if((x - 1, y) in block_words):
                        block_words[(x - 1, y)].append(word)

                    else:
                        block_words[(x - 1, y)] = [word]

                word = ""

    return block_words

#
# Load grid
#
grid = grid_load("result.grid")

if(grid == None):
    print(f"Failed to load grid")
    exit(1)

# Get blocks and their words
block_words = block_words_find(grid)

if(block_words == None):
    print(f"Failed to find block words")
    exit(2)

for block in block_words:
    print(f"{block}: {block_words[block]}")

# Load words along with threir clues
word_clues = word_clues_load("result.words")

if(word_clues == None):
    print(f"Failed to load clues")
    exit(3)

# Initialize img, draw and font variables
img_w = grid.width  * SQUARE_SIZE
img_h = grid.height * SQUARE_SIZE

img = Image.new('RGB', (img_w, img_h), color='white')
draw = ImageDraw.Draw(img)

try:
    font = ImageFont.truetype(FONT_FILE, FONT_SIZE)
    print(f"Loaded {FONT_FILE}")

except IOError:
    print(f"Failed to load font")
    exit(1)

#
# Draw the outline for a square
#
def square_draw(x, y, half=False):
    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    draw.rectangle([x, y, x + w, y + h], outline='black', width=LINE_WIDTH)

#
# Draw the text for the clue
#
def clue_draw(x, y, clue, half=False):
    w = SQUARE_SIZE
    h = (SQUARE_SIZE // 2) if half else SQUARE_SIZE

    # Calculate text size
    bbox = draw.textbbox((0, 0), clue, font=font)

    text_h = bbox[3] - bbox[1]

    wrapped_text = textwrap.fill(clue, width=WRAP_WIDTH, break_long_words=True)

    line_amount = len(wrapped_text.split("\n"))

    # Height of text lines
    lines_h = text_h + (line_amount - 1) * (text_h + LINE_MARGIN)

    text_y = y + max(0, (h - lines_h) // 2)

    print(f"SQUARE_SIZE: {SQUARE_SIZE} h: {h} text_h: {text_h} text_y: {text_y}")

    # Draw the wrapped text line by line
    for index, line in enumerate(wrapped_text.split("\n")):
        # Calculate text size
        bbox = draw.textbbox((0, 0), line, font=font)

        # Calculate width and height of the text
        text_w = bbox[2] - bbox[0]
        text_h = bbox[3] - bbox[1]

        text_x = x + max(0, (w - text_w) // 2)

        # Draw the text on the image
        draw.text((text_x, text_y), line, fill="black", font=font)

        text_y += text_h + LINE_MARGIN

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
        square_draw(img_x, img_y)

        # Square is not block
        if((x, y) not in block_words):
            continue

        words = block_words[(x, y)]

        # Clue square is divided in two   
        if(len(words) > 1):
            for index, word in enumerate(words):
                img_x = (x * SQUARE_SIZE)
                img_y = (y * SQUARE_SIZE) if index == 0 else ((y + 1/2) * SQUARE_SIZE)

                square_draw(img_x, img_y, half=True)

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

# Save image
img.save("korsord.png")
