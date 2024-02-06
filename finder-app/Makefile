SHELL := /usr/bin/env bash

all: writer
	echo all done

writer: writer.c 
	$(CC) $(INCLUDES) $(OBJS) -o $(TARGET) $(LDFLAGS)

clean:
	rm writer
