run: hello.o allways
	./hello.o

allways:

hello.o: hello.c
	gcc hello.c -o hello.o
