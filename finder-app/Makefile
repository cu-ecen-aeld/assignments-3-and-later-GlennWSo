SHELL := /usr/bin/env bash

writer: writer.c 
	${CROSS_COMPILE}gcc writer.c -o writer
	echo ${CROSS_COMPILE} > ../log_target.txt
	# if [ ${CROSS_COMPILE} == "aarch64-unknown-linux-gnu-" ]; then \
	# 	file writer > ../assignments/assignment2/fileresult.txt; \
	# fi; 

clean:
	rm writer
