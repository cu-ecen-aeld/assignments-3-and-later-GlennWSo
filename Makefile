writer: finder-app/writer.c 
	${CROSS_COMPILE}gcc finder-app/writer.c -o writer
	echo $CROSS_COMPILE > log_target.txt

hello.o: hello.c
	gcc hello.c -o hello.o


clean:
	rm writer log_target.txt

