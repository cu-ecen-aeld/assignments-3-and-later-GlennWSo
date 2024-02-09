#include <unistd.h>
#include "sys/syslog.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>


int main(int argc, char *argv[]) {
	// int domain, type, protocol;
	// domain = AF_LOCAL;
	// type = SOCK_STREAM;
	// int protocol = 0; // IP
	openlog(NULL, 0, LOG_USER);
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
	syslog(LOG_INFO, "arg count is: %i\n", argc);
	for (int i=1; i < argc; i++) {
		char *arg = argv[i];
		syslog(LOG_INFO, "arg%i: %s\n", i, arg);
		if ( 0==strcmp(arg, "-d") ) {
			printf("becoming daemon\n");
			if (daemon(0,0)){
				perror("daemon");
			};
		}

	}

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

	//ref https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
	// so instead of branching lets just work wth size for v6, the larger format
	struct sockaddr_in* pV6Addr = (struct sockaddr_in*)&client_addr;
	struct in_addr ipAddr = pV6Addr->sin_addr;
	char addr_str[INET6_ADDRSTRLEN]= "";
	inet_ntop( AF_INET, &ipAddr, addr_str, INET6_ADDRSTRLEN );
  syslog(LOG_INFO, "Accepted connection from %s \n", addr_str);

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
