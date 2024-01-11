#!/usr/bin/env bash
# Run unit tests for the assignment

# Automate these steps from the readme:
# Create a build subdirectory, change into it, run
# cmake .. && make && run the assignment-autotest application
dir=$1
pattern=$2

if test ! -d $dir; then
  echo $dir is not a directory
  exit 1
fi
  
# echo dir $dir
# echo pattern $pattern
nfiles=$(grep $pattern $dir -r -l | wc -l)
nlines=$(grep $pattern $dir -r | wc -l)
echo The number of files are $nfiles and the number of matching lines are $nlines



