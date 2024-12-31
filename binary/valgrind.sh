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
make debug

# Check if the program was compiled successfully
if [ ! -f ./korsord ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Step 2: Run the program with supplied arguments
valgrind_flags="--leak-check=full --track-origins=no --leak-resolution=med"

echo "valgrind $valgrind_flags ./korsord $@"
valgrind $valgrind_flags ./korsord "$@"

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 2
fi
