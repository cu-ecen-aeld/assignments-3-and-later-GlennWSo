#include "sys/syslog.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int sockfd;
int acceptfd;
FILE *client_file;
FILE *dump_fd;
struct addrinfo *servinfo;
const char WRITEPATH[] = "/var/tmp/aesdsocketdata";

static bool terminate = false;
static bool purge = false;
static bool once = false;
char PID_PATH[] = "/tmp/aesdsocket_pid";
void cleanup() {
  syslog(LOG_DEBUG, "cleaning up");
  close(sockfd);
  freeaddrinfo(servinfo);
  remove(PID_PATH);
};
void drop_client(char *addr, bool purge) {
  syslog(LOG_INFO, "Closed connection from %s", addr);
  syslog(LOG_DEBUG, "terminate: %d", terminate);
  fclose(dump_fd);
  close(acceptfd);
  if (purge) {
    remove(WRITEPATH);
  }
}

static void catch_function(int signo) {
  syslog(LOG_INFO, "Caught signal, exiting");
  syslog(LOG_DEBUG, "signo: %d", signo);
  terminate = true;
  if (signo != SIGQUIT) {
    purge = true;
  }
}

/// read single char, can be nonblocking via fcntl
///
/// returns: 1 on char read, 0 on EOF, -1 on err
ssize_t ngetc(int fd, char *c) { return read(fd, c, 1); }

int main(int argc, char *argv[]) {
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

  char port[100] = "9000";
  int res = getaddrinfo(NULL, port, &hints, &servinfo);
  if (res != 0) {
    fprintf(stderr, "getaddrinfo failed: %s", strerror(-res));
    exit(1);
  }
  printf("get addres ok\n");
  syslog(LOG_INFO, "arg count is: %i\n", argc);
  for (int i = 1; i < argc; i++) {
    char *arg = argv[i];
    syslog(LOG_INFO, "arg%i: %s\n", i, arg);
    if (0 == strcmp(arg, "-d")) {
      printf("becoming daemon\n");
      if (daemon(0, 0)) {
        perror("daemon");
      };
    }
    if (0 == strcmp(arg, "--clear")) {
      printf("clearing dump file before accepting clients: %s\n", WRITEPATH);
      if (remove(WRITEPATH)) {
        perror("remove");
      };
    }
    if (0 == strcmp(arg, "--purge")) {
      printf("clear dump file after clients: %s\n", WRITEPATH);
      purge = true;
    }
    if (0 == strcmp(arg, "--once")) {
      printf("once mode: will exit after first client\n");
      once = true;
    }
  }
  FILE *pid_file = fopen(PID_PATH, "w");
  if (pid_file == NULL) {
    syslog(LOG_ERR, "fopen(%s, \"w\")\terror:\n\t%s", PID_PATH,
           strerror(errno));
    exit(1);
  }
  int mypid = getpid();
  char pid_str[8] = "";
  sprintf(pid_str, "%d", mypid);
  fwrite(pid_str, 1, 8, pid_file);
  if (ferror(pid_file)){
    syslog(LOG_ERR, "fwrite: %s", strerror(errno));
    exit(1);
  };
  fclose(pid_file);

  sockfd = socket(servinfo->ai_family, servinfo->ai_socktype | SOCK_NONBLOCK,
                  servinfo->ai_protocol);
  if (sockfd == -1) {
    syslog(LOG_ERR, "socket failed: %s", strerror(errno));
    exit(1);
  }
  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) <
      0) {
    syslog(LOG_ERR, "setsockopt(SO_REUSEADDR) failed: %s", strerror(errno));
  }
  syslog(LOG_INFO, "sock ok\n");

  res = bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen);
  if (res != 0) {
    syslog(LOG_ERR, "bind failed: %s", strerror(errno));
    exit(1);
  }
  syslog(LOG_INFO, "bind ok\n");

  while (!terminate) {
    syslog(LOG_DEBUG, "main loop");
    res = listen(sockfd, 10);
    if (res != 0) {
      syslog(LOG_ERR, "listen failed: %s", strerror(errno));
      exit(1);
    }

    syslog(LOG_INFO, "listen ok\n");

    struct sockaddr client_addr;
    socklen_t addr_size = sizeof(client_addr);

    while (!terminate) {
      acceptfd = accept(sockfd, &client_addr, &addr_size);
      if (acceptfd == -1) {
        if (errno == EWOULDBLOCK) {
          usleep(20000);
          continue;
        }
        syslog(LOG_ERR, "accept: %s", strerror(errno));
        exit(1);
      }
      syslog(LOG_INFO, "accepted");
      break;
    };

    // ref
    // https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c
    //  so instead of branching lets just work wth size for v6, the larger
    //  format
    struct sockaddr_in *pV6Addr = (struct sockaddr_in *)&client_addr;
    struct in_addr ipAddr = pV6Addr->sin_addr;
    char addr_str[INET6_ADDRSTRLEN] = "";
    inet_ntop(AF_INET, &ipAddr, addr_str, INET6_ADDRSTRLEN);
    syslog(LOG_INFO, "Accepted connection from %s \n", addr_str);

    dump_fd = fopen(WRITEPATH, "a");
    if (!dump_fd) {
      syslog(LOG_PERROR, "could not open or create new file: %s\nerror: %s\n",
             WRITEPATH, strerror(errno));
      exit(1);
    };

    int gets_res;
    char c;
    while (!terminate) {
      gets_res = ngetc(acceptfd, &c);
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
      fputc(c, dump_fd);
      if (c == '\n') {
        break;
      }
    }
    fclose(dump_fd);
    syslog(LOG_DEBUG, "fin listing");
    dump_fd = fopen(WRITEPATH, "r");
    if (dump_fd == NULL) {
      syslog(LOG_ERR, "fopen error:%s", strerror(errno));
    }

    char read_buffer[100] = "";
    unsigned long buffer_size = sizeof(read_buffer);
    unsigned long read_res;
    while (!terminate) {
      read_res = fread(read_buffer, 1, buffer_size - 1, dump_fd);
      if (read_res == 0) {
        if (feof(dump_fd)) {
          syslog(LOG_DEBUG, "eof reached");
          break;
        }
        syslog(LOG_ERR, "fread: %s", strerror(errno));
        exit(1);
      }
      read_buffer[read_res] = 0;
      int cum = 0;
      while (!terminate && (read_res > cum)) {
        res = write(acceptfd, &read_buffer[cum], read_res);
        if (res == -1) {
          if (errno == EAGAIN) {
            continue;
          }
          syslog(LOG_ERR, "write: %s", strerror(errno));
          exit(1);
        }
        cum += res;
      }
    }
    syslog(LOG_DEBUG, "writeback fin");

    usleep(20000);
    drop_client(addr_str, purge);
    if (once) {
      break;
    }
  }
  cleanup();
}
