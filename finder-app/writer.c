#include "sys/syslog.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
   
   openlog(NULL, 0, LOG_USER);
   syslog(LOG_INFO, "arg count is: %i\n", argc);
   printf("arg count is: %i\n", argc);
   for (int i=1; i < argc; i++) {
      printf("arg%i: %s\n", i, argv[i]);
      syslog(LOG_INFO, "arg%i: %s\n", i, argv[i]);
    }

   if (argc != 3) {
      printf("%s requires 2 args\n", argv[0]);
      syslog(LOG_ERR, "%s requires 2 args\n", argv[0]);
      return 1;
   }

   char *writepath = argv[1];
   char *writestr = argv[2];
   
   FILE *fd;
   fd = fopen(writepath, "w");
   printf("hello");
   if ( !fd ) {
      perror("could not open or create new file");
      syslog(LOG_PERROR,  "could not open or create new file: %s", strerror(errno));
      fprintf(stderr, "path argument was: %s", writepath);

   };
   fprintf(fd, "%s\n", writestr);
   fclose(fd);
   return 0;
}


