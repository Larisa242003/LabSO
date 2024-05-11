#!/bin/bash

# Specify the isolated directory where dangerous files will be moved
isolated_dir="director_izolat"

# Check if the file is ASCII
if [[ $(file -b --mime-encoding "$1") == "binary" ]]; then
    echo "Error: File $1 is binary and cannot be syntactically analyzed."
    exit 1
fi

# Get the number of lines, words, and characters in the file
num_lines=$(wc -l < "$1")
num_words=$(wc -w < "$1")
num_chars=$(wc -m < "$1")

# Perform keyword search for malicious terms
if grep -q -i -E "corrupted|dangerous|risk|attack|malware|malicious" "$1"; then
    echo "Malicious content detected in the file $1. Moving to isolated directory."
    mv "$1" "$isolated_dir"
    exit 0
fi

# Check for non-ASCII characters
if strings -a "$1" | grep -q "[^ -~]"; then
    echo "Non-ASCII characters found in the file $1. Moving to isolated directory."
    mv "$1" "$isolated_dir"
    exit 0
fi

echo "File $1 is safe. Analysis results:"
echo "Number of lines: $num_lines"
echo "Number of words: $num_words"
echo "Number of characters: $num_chars"
