#include "sys/syslog.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>


int main(){
	// int domain, type, protocol;
	// domain = AF_LOCAL;
	// type = SOCK_STREAM;
	// int protocol = 0; // IP
   openlog(NULL, 0, LOG_USER);

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	// hints.ai_protocol = 0;
	
	struct addrinfo *servinfo;
  char port[5] = "9000";
  int res = getaddrinfo(NULL, port, &hints, &servinfo);
	if (res != 0 ) {
		// perror("getaddrinfo failed");
		fprintf(stderr, "getaddrinfo failed: %s", strerror(-res));
		exit(1);
	}
	printf("get addres ok\n");

	int sockfd = socket(
		servinfo->ai_family,
		// AF_LOCAL,
		servinfo->ai_socktype,
		servinfo->ai_protocol
	);
	if (sockfd == -1 ) {
		perror("socket failed");
		exit(1);
	}
   syslog(LOG_INFO, "sock ok\n");


	res = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	if (res != 0 ) {
		perror("bind failed");
		exit(1);
	}
   syslog(LOG_INFO, "bind ok\n");

	res = listen(sockfd, 10);
	if (res != 0 ) {
		perror("listen failed");
		exit(1);
	}

   syslog(LOG_INFO, "listen ok\n");

	// struct sockaddr_storage client_addr;
	struct sockaddr client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int clientfd = accept(sockfd, &client_addr, &addr_size);
	if (clientfd == -1 ) {
		perror("acc failed");
		exit(1);
	}
  syslog(LOG_INFO, "Accepted connection from %s\n", client_addr.sa_data);

	char read_buffer[1000] = "";
	read(clientfd, &read_buffer, 1000);
	char *writepath ="/var/tmp/aesdsocketdata";
	FILE *fd = fopen(writepath, "a");
	if ( !fd ) {
		syslog(LOG_PERROR, "could not open or create new file: %s\nerror: %s\n",writepath, strerror(errno));
		exit(1);
	};
 fprintf(fd, "%s\n", read_buffer);
 fclose(fd);

	freeaddrinfo(servinfo);
}
