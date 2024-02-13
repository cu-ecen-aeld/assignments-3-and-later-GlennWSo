#!/usr/bin/env bash


for arg in $@
do
  if [ $arg == "-c" ] || [ $arg == "--clean" ]; then
    make clean
  fi
done

# build
CC=gcc make aesdsocket


rm res.txt
./aesdsocket -d && cat test.txt | nc localhost 9000 > res.txt

res=$(cat res.txt)
expected="0123456789"
if [  "$res" == $expected ]; then
  echo ok
else
  echo  test failed ':('
  echo expected was $expected
  echo but result was: $res
  exit 1
fi
