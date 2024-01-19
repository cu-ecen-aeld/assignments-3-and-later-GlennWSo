#include <stdio.h>
int main() {
   FILE *file;
   file = fopen("tmp/hello.txt", "w");
   fprintf(file, "Hello, World!");
   fclose(file);
   return 0;
}


