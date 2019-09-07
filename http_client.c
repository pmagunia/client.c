#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdint.h>

#define MAX_BUFFER_SIZE 2048

int main(int argc, char *argv[]) {
  struct addrinfo hints;
  struct addrinfo *res;
  int sockfd, bytes_recv, i, j, status;
  char request[1000];
  char buff[MAX_BUFFER_SIZE];
  char buffer[MAX_BUFFER_SIZE];
  char* path;

  // All paths will be from http://localhost:8888/
  path = strstr(argv[1], ":8888/");
  path += 5;
  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  getaddrinfo("127.0.0.1", "8888", &hints, &res);

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

  status = connect(sockfd, res->ai_addr, res->ai_addrlen);

  sprintf(request, "GET %s HTTP/1.0\r\nHost: localhost: 8888\r\n\r\n", path);
  send(sockfd, request, strlen(request), 0);

  FILE *file = fopen("output", "wb");

  bytes_recv = recv(sockfd, buff, MAX_BUFFER_SIZE, 0);
  
  // Remove the header, keep the body
  for (i = 0; i < 500; i++) {
    if (buff[i] == '\r' && buff[i+1] == '\r' && buff[i+2] == '\r' && buff[i+3] == '\n') {
      i = i + 4;
      break;
    }
  }
  for (j = i; j < bytes_recv; j++) {
    buffer[j-i] = buff[j];
  }
  fwrite(buffer, bytes_recv-i, 1, file);
  
  do {
    bytes_recv = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
    fwrite(buffer, bytes_recv, 1, file);
  } while(bytes_recv > 0);
  
  fclose(file);

  return 0;
}
