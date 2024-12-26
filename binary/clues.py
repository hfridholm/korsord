#
# clues.py - generate crossword clues using AI
#
# Written by Hampus Fridholm
#
# https://github.com/openai/openai-python
#

from openai import OpenAI

MAX_CLUE_LENGTH = 30

api_key_file = "api.key"

# Function to read the API key from a local file
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
        Ditt svar ska inte sluta med en punkt.
        Ditt svar får inte vara mer än {MAX_CLUE_LENGTH} tecken.
        Själva ordet får inte stå med i ledtråden.
    """

    try:
        completion = client.chat.completions.create(
            model="gpt-3.5-turbo",
            messages=[
                {"role": "system", "content": "You are a helpful assistant."},
                {"role": "user", "content": prompt}
            ],
            max_tokens=MAX_CLUE_LENGTH,
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

                if(len(split_line) < 2):
                    return None

                word = split_line[0].strip().lower()
                clue = split_line[1].strip()

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
    # 1. Load word clues
    word_clues = word_clues_load("result.words")

    if(word_clues == None):
        print(f"Failed to load word clues")
        exit(1)

    # 2. Fill in missing clues using ChatGPT
    for word, clue in word_clues.items():
        if(len(clue) == 0):
            clue = word_clue_gen(word)

        word_clues[word] = clue

    # 3. Load word clues
    word_clues_save(word_clues, "result.words")
