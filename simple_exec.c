// #include "examples/systemcalls/systemcalls.h"
// #include <cstdlib>
#include <stdio.h>
#include <unistd.h>


int main(){
	char* args[] = {"hello", "world"};
	int res = execv("/nix/store/zx8aqgdy735qzk81glfwil6mbi6ddqb1-coreutils-9.4/bin/echo", args);
	printf("res: %s", res);
	return 1;
}
