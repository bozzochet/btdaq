// file client.c
//
// stolen from Linux Socket Programming book (test-port-client.c)
// and reworked
//
// A.lebedev, May 2004...
//

//-------------------------------------------------------------------------
//
//   This is a port-read client.  It will accept any IP address and port
//   number on the commandline, connect to the server, send the message
//   (if any defined), read the reply, and close.
//
//-------------------------------------------------------------------------

#include "mylib.h"

#define MAXBUF 1024

#define QUIT(msg) {perror(msg); exit(1);}

//-------------------------------------------------------------------------

int main(int count, char *strings[]) {

  int sd, bytes_read;
  struct sockaddr_in dest;
  char buffer[MAXBUF];
  int n = 0;

  if (count < 3  ||  count > 4) {
    fprintf(stderr, "usage: %s <IP-addr> <port> [<send-msg>]\n", strings[0]);
    exit(0);
  }

  while(1) {

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) QUIT("socket:");

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
/*
    if (inet_addr(strings[1], &dest.sin_addr.s_addr) == 0) QUIT(strings[1]);
*/
    dest.sin_port = htons(atoi(strings[2]));

    if (connect(sd, (struct sockaddr *)&dest, sizeof(dest))) QUIT("connect");

    if (count == 4) {
      sprintf(buffer, "%s\n", strings[3]);
      send(sd, buffer, strlen(buffer), 0);
    }

    bzero(buffer, MAXBUF);
    bytes_read = recv(sd, buffer, MAXBUF, 0);
//    if (bytes_read > 0) printf("%d %s", n++, buffer);
    printf("%d %d %s", n++, bytes_read, buffer);

    close(sd);
  }
}

//-------------------------------------------------------------------------
