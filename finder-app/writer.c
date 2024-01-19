#include <stdio.h>
int main(int argc, char *argv[]) {
   
   argc -= 1;

   if (argc != 2) {
      printf("%s requires 2 args\n", argv[0]);
      printf("arg count is: %i\n", argc);
      for (int i=1; i < argc + 1; i++) {
         printf("arg%i: %s\n", i, argv[i]);
       }
      return 1;
   }
   
   FILE *file;
   file = fopen("tmp/hello.txt", "w");
   fprintf(file, "Hello, World!");
   fclose(file);
   return 0;
}


