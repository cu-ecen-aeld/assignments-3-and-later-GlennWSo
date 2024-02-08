#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>


int main(){
	int domain, type, protocol;
	domain = AF_LOCAL;
	type = SOCK_STREAM;
	protocol = 0; // IP
	
	
	int sockfd = socket(domain, type, protocol);

	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	struct addrinfo *servinfo;
  char port[5] = "1234";
  int res = getaddrinfo(NULL, port, &hints, &servinfo);
	if (res != 0 ) {
		perror("getaddrinfo failed");
		exit(1);
	}

	res = bind(sockfd, servinfo->ai_addr , servinfo->ai_addrlen);
	if (res != 0 ) {
		perror("bind failed");
		exit(1);
	}


	freeaddrinfo(servinfo);
}
