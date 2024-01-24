#include "examples/systemcalls/systemcalls.h"
// #include <cstdlib>
#include <stdio.h>
#include <unistd.h>


int main() {
	// char* args[] = {"/usr/bin/env", "echo", "hello", "world", NULL};
	bool res = do_exec(4, "/usr/bin/env", "echo", "hello", "world");
	printf("do_exec returned: %b", res);
}
