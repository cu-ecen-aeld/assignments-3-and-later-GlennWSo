#!/bin/sh

#start

PID_FILE=/tmp/aesdsocket_pid


for arg in $@
do
  echo $arg
  if [ "$arg" = "-S" ]; then
    echo starting
    if [ -e $PID_FILE ]; then
      echo $PID_FILE already exists, exiting...
      exit 1
    fi
    ./aesdsocket -d 
    echo new pid:
    cat $PID_FILE
  fi
  if [ "$arg" = "-K" ]; then
    echo stopping
    if [ -e $PID_FILE ]; then
      echo sending TERM to socket-server
      kill -s TERM $(cat $PID_FILE)
      exit 0
    fi
    echo cant find pid
    exit 1
  fi
done

