#!/bin/bash

#
# valgrind.sh - run korsord with valgrind
#
# Written by Hampus Fridholm
#
# with help from ChatGPT
#

# Step 1: Compile the program
echo
echo "Compiling the program..."
make korsord

# Check if the program was compiled successfully
if [ ! -f ./korsord ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Step 2: Run the program with supplied arguments
echo "valgrind --leak-check=no --track-origins=no --leak-resolution=med ./korsord $@"
valgrind --leak-check=no --track-origins=no --leak-resolution=med ./korsord "$@"

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 2
fi
