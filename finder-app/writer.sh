#!/usr/bin/env bash
# Run unit tests for the assignment

# Automate these steps from the readme:
# Create a build subdirectory, change into it, run
# cmake .. && make && run the assignment-autotest application
writepath=$1
writestr=$2
if [ $# -eq 0 ]; then
  echo no args
  exit 1
fi

if test ! -f $writepath; then
  pdir=$(dirname $writepath)
  if test ! -d $pdir; then
    echo $pdir does not exists, creating a directory
    mkdir -p $pdir
  fi
  echo $writeparth does not exists, creating a new file
  touch $writepath
fi

echo $writestr > $writepath
