SHELL := /usr/bin/env bash

all: writer
	echo all done

writer: writer.c 
	$(CC) $(INCLUDES) $(OBJS) -o writer $(LDFLAGS) 

clean:
	rm writer
