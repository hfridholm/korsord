#!/bin/bash

#
# generate.sh - generate crossword from grid
#
# Written by Hampus Fridholm
#

# Step 1: Generate missing clues with ChatGPT
echo "python clues.py"
python clues.py

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 1
fi

# Step 2: Render the crossword using words and clues
echo "python render.py"
python render.py

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 2
fi
