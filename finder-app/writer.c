#include <stdio.h>
int main(int argc, char *argv[]) {
   

   if (argc != 3) {
      printf("%s requires 2 args\n", argv[0]);
      printf("arg count is: %i\n", argc);
      for (int i=1; i < argc; i++) {
         printf("arg%i: %s\n", i, argv[i]);
       }
      return 1;
   }

   char *writepath = argv[1];
   char *writestr = argv[2];
   
   FILE *file;
   file = fopen(writepath, "w");
   fprintf(file, "%s", writestr);
   fclose(file);
   return 0;
}


