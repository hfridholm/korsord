#!/bin/bash

#
# Written by Hampus Fridholm
#
# with help from ChatGPT
#

# Step 1: Compile the program
echo "Compiling the program..."
make korsord

# Check if the program was compiled successfully
if [ ! -f ./korsord ]; then
    echo "Error: Compilation failed."
    exit 1
fi

# Step 2: Run the program with supplied arguments
echo "Running the program with arguments: $@"
./korsord "$@"

# Check the exit code of the program
if [ $? -ne 0 ]; then
    echo "Error: Program execution failed with exit code $?."
    exit 2
fi

# Check if gmon.out exists (it will be created if profiling was enabled)
if [ -f gmon.out ]; then
  # 3. Run gprof with the gmon.out file
  gprof ./korsord gmon.out
else
  echo "Error: gmon.out not found. Make sure you compiled with profiling enabled."
  exit 3
fi
