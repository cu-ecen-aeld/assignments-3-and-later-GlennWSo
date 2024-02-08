#include "sys/syslog.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>


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
  char port[5] = "1234";
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

	struct sockaddr_storage client_addr;
	socklen_t addr_size = sizeof(client_addr);
	int accfc = accept(sockfd, (struct sockaddr *)&client_addr, &addr_size);
	if (res == -1 ) {
		perror("acc failed");
		exit(1);
	}
   syslog(LOG_INFO, "acc ok\n");


	freeaddrinfo(servinfo);
}
