import argparse

#
# Function to extract the first word of line
#
def extract_line_word(line):
    first_word = line.split()[0] if line.split() else ""

    # The word must have only ascii characters
    if(not first_word.isalpha() or not first_word.isascii()):
        return None

    # The word must have at least 2 letters
    if(len(first_word) < 2):
        return None

    return first_word.lower()

#
# Function to extract the first word of each line
#
def extract_first_words(input_file, output_file):
    try:
        with open(input_file, 'r') as infile:
            with open(output_file, 'w') as outfile:
                for line in infile:
                    first_word = extract_line_word(line)

                    if(first_word == None):
                        continue

                    outfile.write(first_word + '\n')

        print(f"First words have been written to {output_file}")

    except FileNotFoundError:
        print(f"The file {input_file} was not found.")

    except Exception as e:
        print(f"An error occurred: {e}")

# Main function to handle argument parsing
def main():
    # Set up argument parser
    parser = argparse.ArgumentParser(description="Extract the first word from each line of an input file and save them to an output file.")
    
    # Add arguments for input and output file names
    parser.add_argument('input_file', help="The input file containing lines of text")
    parser.add_argument('output_file', help="The output file to store the first words")

    # Parse the arguments
    args = parser.parse_args()

    # Call the function with the provided file names
    extract_first_words(args.input_file, args.output_file)

# Run the script
if __name__ == '__main__':
    main()
