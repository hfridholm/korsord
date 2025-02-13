#
# clues.py - generate crossword clues using openai
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI
import argparse
import os

api_key_file = "api.key"

CONFIG_DIR = os.path.join(os.path.expanduser('~'), ".korsord")

WORDS_DIR = os.path.join(CONFIG_DIR, "words")

#
# Get the file path of a words by name
#
def words_file_get(name):
    return os.path.join(WORDS_DIR, f"{name}.words")

CLUES_DIR = os.path.join(CONFIG_DIR, "clues")

#
# Get the file path of a clues by name
#
def clues_file_get(name):
    return os.path.join(CLUES_DIR, f"{name}.clues")

#
# Function to read the API key from a local file
#
def api_key_get(filepath):
    try:
        with open(filepath, 'r') as file:
            api_key = file.read().strip()

        return api_key

    except Exception as e:
        print(f"Error reading API key: {e}")
        return None

# Start an openai client using API key
client = OpenAI(api_key=api_key_get(api_key_file))

#
# Ask ChatGPT for a clue to a word
#
def word_clue_gen(word):
    prompt = f"""
Skriv en ledtråd till ordet '{word}'.
Ledtråden ska användas i ett korsord.
Ditt svar ska innehålla färre än {args.amount} ord.
Själva ordet får inte stå med i ledtråden.
Ordet 'ledtråd' får inte finnas med i ditt svar.
Inga långa ord får finnas med.
    """

    if args.theme:
        prompt += f"Temat är: \'{args.theme}\'"

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "Du har en stor språkförståelse"},
                {"role": "user", "content": prompt}
            ],
            max_tokens=args.length,
            temperature=0.7
        )

        message = completion.choices[0].message.content

        clue = message.strip().strip(".").upper()

        return clue

    except Exception as exception:
        print(f"ChatGPT error: {exception}")
        return None

#
# Load words and their clues in a dictionary
#
def word_clues_load(filepath):
    word_clues = {}

    try:
        with open(filepath, 'r') as file:
            for line in file.readlines():
                split_line = line.split(":", 1)

                word = split_line[0].strip().lower()
                clue = split_line[1].strip() if len(split_line) == 2 else ""

                word_clues[word] = clue;

    except FileNotFoundError:
        print(f"Words file not found")
        return None

    except Exception as exception:
        print(f"Failed to read words file")
        return None

    return word_clues

#
# Save words and their clues
#
def word_clues_save(word_clues, filepath):
    # 1. Get max width for alignment
    max_width = 0

    for word in word_clues:
        max_width = max(max_width, len(word) + 1)

    try:
        with open(filepath, 'w') as file:
            for word, clue in word_clues.items():
                curr_width = max_width - len(word)

                file.write(f"{word}{' ' * curr_width}: {clue}\n")

    except Exception as exception:
        print(f"Failed to write words file")

#
# Main function
#
if __name__ == "__main__":
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="generate crossword clues using openai")

    parser.add_argument("--theme",
        type=str, default=None,
        help="Theme of words"
    )

    parser.add_argument("--name",
        type=str, default="temp",
        help="Name of clues"
    )

    parser.add_argument("--words",
        type=str, default=None,
        help="Name of words"
    )

    parser.add_argument("--amount",
        type=int, default=4,
        help="Amount of words in clue"
    )

    parser.add_argument("--length",
        type=int, default=30,
        help="Max length of clue"
    )

    parser.add_argument("--force",
        action='store_true',
        help="Modify existing clues"
    )

    args, words = parser.parse_known_args()


    clues_file = clues_file_get(args.name)

    if args.words:
        words_file = words_file_get(args.words)

        if not os.path.exists(words_file):
            print(f"korsord: {args.words}: Words not found")
            exit(0)

        if os.path.exists(clues_file) and not args.force:
            print(f"korsord: {args.name}: Clues already exist")
            exit(0)

        word_clues = word_clues_load(words_file)

    elif not os.path.exists(clues_file):
        print(f"korsord: {args.name}: Clues not found")
        exit(0)

    else:
        word_clues = word_clues_load(clues_file)


    if not word_clues:
        print(f"korsord: {args.name}: Failed to load words")
        exit(1)


    # Get max width for alignment
    max_width = 0

    for word in word_clues:
        max_width = max(max_width, len(word) + 1)


    # Fill in missing clues using ChatGPT
    for word, clue in word_clues.items():

        # Generate missing clues, or overwrite clues if specified
        if (not words and len(clue) == 0) or (words and word in words):
            for count in range(10):
                clue = word_clue_gen(word)

                if len(clue) <= args.length:
                    break

            curr_width = max_width - len(word)

            print(f"{word}{' ' * curr_width}: {clue}")

        word_clues[word] = clue

    # Save result
    word_clues_save(word_clues, clues_file)
