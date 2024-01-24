#include "examples/systemcalls/systemcalls.h"
// #include <cstdlib>
#include <stdio.h>
#include <unistd.h>


int main() {
	// char* args[] = {"/usr/bin/env", "echo", "hello", "world", NULL};
	bool res = do_exec_redirect("derp.txt", 3, "/usr/bin/env", "echo", "hi there");
	printf("do_exec returned: %i", res);
}
