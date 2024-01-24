// #include "examples/systemcalls/systemcalls.h"
// #include <cstdlib>
#include <stdio.h>
#include <unistd.h>


int main(){
	char* args[] = {"echo", "hello", "world", NULL};
	int res = execv("/usr/bin/env", args);
	printf("res: %i", res);
	perror("execv");
	return 1;
}
