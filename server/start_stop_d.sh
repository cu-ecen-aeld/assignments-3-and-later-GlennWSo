#!/bin/sh

#start

for arg in $@
do
  echo $arg
  if [ "$arg" = "-S" ]; then
    echo starting
    if [ -e /var/tmp/aesdsocket_pid ]; then
      echo /var/tmp/aesdsocket_pid already exists, exiting...
      exit 1
    fi
    ./aesdsocket -d 
    echo new pid:
    cat /var/tmp/aesdsocket_pid
  fi
  if [ "$arg" = "-K" ]; then
    echo stopping
    if [ -e /var/tmp/aesdsocket_pid ]; then
      echo sending TERM to socket-server
      kill -s TERM $(cat /var/tmp/aesdsocket_pid)
      exit 0
    fi
    echo cant find pid
    exit 1
  fi
done

