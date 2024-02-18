#include <fcntl.h>
#include <signal.h>
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
#include <stdbool.h>

int sockfd;
int acceptfd;
FILE *client_file;
FILE *dump_fd;
struct addrinfo *servinfo;
const char WRITEPATH[] ="/var/tmp/aesdsocketdata";

void cleanup(bool purge) {
	syslog(LOG_DEBUG, "cleaning up");
	fclose(client_file);
	fclose(dump_fd);
	close(acceptfd);
	close(sockfd);
	freeaddrinfo(servinfo);
	remove(WRITEPATH);
};

static bool terminate = false;
static bool purge = false;
static void catch_function(int signo) {
	syslog(LOG_INFO, "Caught signal, exiting: %d", signo);
	terminate = true;
	purge = (signo != SIGQUIT);
}

/// read single char, can be nonblocking via fcntl
/// 
/// returns: 1 on char read, 0 on EOF, -1 on err
ssize_t ngetc(int fd, char *c){
	return read(fd, c, 1);
}

int main(int argc, char *argv[]) {
	// int domain, type, protocol;
	// domain = AF_LOCAL;
	// type = SOCK_STREAM;
	// int protocol = 0; // IP
	openlog(NULL, 0, LOG_USER);
	if (signal(SIGINT | SIGTERM | SIGQUIT, catch_function) == SIG_ERR) {
		perror("signal");
		exit(1);
	}


	struct addrinfo hints;
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	// hints.ai_protocol = 0;
	
	
  char port[100] = "9000";
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

	sockfd = socket(
		servinfo->ai_family,
		servinfo->ai_socktype,
		servinfo->ai_protocol
	);
	if (sockfd == -1 ) {
		syslog(LOG_ERR, "socket failed: %s", strerror(errno));
		exit(1);
	}
	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0){
		syslog(LOG_ERR,"setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
	}
	syslog(LOG_INFO, "sock ok\n");


	res = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
	if (res != 0 ) {
		syslog(LOG_ERR, "bind failed: %s", strerror(errno));
		exit(1);
	}
   syslog(LOG_INFO, "bind ok\n");

	res = listen(sockfd, 10);
	if (res != 0 ) {
		syslog(LOG_ERR, "listen failed: %s", strerror(errno));
		exit(1);
	}

   syslog(LOG_INFO, "listen ok\n");

	// struct sockaddr_storage client_addr;
	struct sockaddr client_addr;
	socklen_t addr_size = sizeof(client_addr);

	int flags = fcntl(sockfd, F_GETFL, 0);
	res = fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
	if (res==-1) {
		syslog(LOG_ERR, "fcntl: %s", strerror(errno));
		exit(1);
	}
	while (!terminate) {
		acceptfd = accept(sockfd, &client_addr, &addr_size);
		if (acceptfd==-1){
			if (errno==EWOULDBLOCK) {
				usleep(20000);
				continue;
			}
			syslog(LOG_ERR, "accept: %s", strerror(errno));
			exit(1);
		}
		syslog(LOG_INFO, "accepted");
		break;
	};
	// fcntl(sockfd, F_SETFL, flags);
	// flags = fcntl(acceptfd, F_GETFL, 0);
	// fcntl(acceptfd, F_SETFL, flags & (!O_NONBLOCK));

	
	//ref https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
	// so instead of branching lets just work wth size for v6, the larger format
	struct sockaddr_in* pV6Addr = (struct sockaddr_in*)&client_addr;
	struct in_addr ipAddr = pV6Addr->sin_addr;
	char addr_str[INET6_ADDRSTRLEN]= "";
	inet_ntop(AF_INET, &ipAddr, addr_str, INET6_ADDRSTRLEN );
  syslog(LOG_INFO, "Accepted connection from %s \n", addr_str);


	dump_fd =fopen(WRITEPATH, "w");
	if ( !dump_fd ) {
		syslog(LOG_PERROR, "could not open or create new file: %s\nerror: %s\n",WRITEPATH, strerror(errno));
		exit(1);
	};

	// client_file = fdopen(acceptfd, "r");
	int gets_res;
	char c;
	while (!terminate) {
		gets_res = ngetc(acceptfd, &c);
		syslog(LOG_DEBUG, "last res: %i", gets_res);
		if (gets_res == 0) {
			syslog(LOG_DEBUG, "Read EOF from socket");
			break;
		}
		if (gets_res == -1) {
			if (errno == EAGAIN) {
				usleep(1000);
				continue;
			}
			syslog(LOG_ERR, "ngetc: %s", strerror(errno));
			exit(1);
		}
		syslog(LOG_DEBUG, "last char: %c", c);
		fputc(c, dump_fd);
		if (c == '\n'){
			break;
		}
	}
	fclose(dump_fd);
	syslog(LOG_DEBUG, "fin listing");

  // TODO f. Returns the full content of /var/tmp/aesdsocketdata to the client as soon as the received data packet completes.
	dump_fd = fopen(WRITEPATH, "r");
	if (dump_fd == NULL) {
		syslog(LOG_ERR, "fopen error:%s", strerror(errno));
	}

	char read_buffer[100] = "";
  unsigned long buffer_size = sizeof(read_buffer);
	unsigned long read_res;
	while (!terminate) {
		// read_res = fgets(&read_buffer[0], buffer_size, dump_fd);
		read_res = fread(read_buffer, 1, buffer_size - 1,  dump_fd);
		syslog(LOG_INFO, " file read_res: %lu", read_res);
		if (read_res == 0) {
			if (feof(dump_fd)) {
				syslog(LOG_DEBUG, "eof reached");
				break;
			} 
			syslog(LOG_ERR, "fread: %s", strerror(errno));
			exit(1);
		}
		syslog(LOG_DEBUG, "read: %s", read_buffer);
		// printf("read: %s", read_buffer);
		read_buffer[read_res] = 0;
		write(acceptfd, read_buffer, buffer_size-1);
	}

	cleanup(purge);

}


