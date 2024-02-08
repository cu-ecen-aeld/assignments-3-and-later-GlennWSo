SHELL := /usr/bin/env bash

all: aesdsocket
	echo all done

aesdsocket: aesdsocket.c 
	$(CC)  aesdsocket.c -o aesdsocket 

clean:
	if [ -f "aesdsocket" ]; then \
		echo removing aesdsocket; \
		rm aesdsocket; \
	fi
