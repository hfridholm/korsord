#
# ordlista.py - word list scraping tool
#
# Written by Hampus Fridholm
#

import argparse

#
# Extract the first word of line
#
def word_extract(line):
    first_word = line.split()[0] if line.split() else ""

    # The word must have only ascii characters
    if(not first_word.isalpha() or not first_word.isascii()):
        return None

    # The word must have at least 2 letters
    if(len(first_word) < 2):
        return None

    return first_word.lower()

#
# Extract the first word of each line from input file
#
def words_extract(input_file):
    words = []

    try:
        print(f"Extracting words from {input_file}")

        with open(input_file, 'r') as infile:
            for line in infile:
                word = word_extract(line)

                if(word != None):
                    words.append(word)

        print(f"Extracted words from {input_file}")

    except FileNotFoundError:
        print(f"The file {input_file} was not found.")
        return None

    except Exception as exception:
        print(f"An error occurred: {exception}")
        return None
    
    return words

#
# Filter words by removing duplicates
#
def words_filter(words):
    print(f"Filtering words: {len(words)}")

    seen_words = set()
    result = []

    for word in words:
        if word not in seen_words:
            result.append(word)
            seen_words.add(word)

    print(f"Filtered words: {len(result)}")

    return result

#
# Write words to output file on seperate lines
#
def words_write(words, output_file):
    try:
        print(f"Writing words to {output_file}")

        with open(output_file, 'w') as outfile:
            for word in words:
                outfile.write(word + '\n')

        print(f"Wrote words to {output_file}")

    except Exception as exception:
        print(f"An error occurred: {exception}")

#
# Main routine
#
if __name__ == '__main__':
    parser = argparse.ArgumentParser(description="Extract the first word from each line of an input file and save them to an output file.")
    
    parser.add_argument('input_file', help="The input file containing lines of text")
    parser.add_argument('output_file', help="The output file to store the first words")

    args = parser.parse_args()

    words = words_extract(args.input_file)

    if (words != None):
        words = words_filter(words)

        words_write(words, args.output_file)
