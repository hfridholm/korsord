#!/bin/bash

#
# interact.sh - start korsord in interact mode
#
# Written by Hampus Fridholm
#
# with help from ChatGPT
#

# Step 1: Compile the program
echo
echo "Compiling the program..."
make speed

# Check if the program was compiled successfully
if [ ! -f ./korsord ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Step 2: Run the program with supplied arguments
echo "./korsord $@ -i"
# -d is for debug mode
./korsord "$@" "-i"

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 2
fi
