// file ttcp-client.c
//
// Test support for transaction-TCP (T/TCP)
// Stolen from somewhere
//
// A.Lebedev Mar-2007...
//

#include "mylib.h"

#define DEFAULT_PORT 9999

/*
#ifndef TCP_NOPUSH
#error TCP_NOPUSH not defined
#endif
*/
#ifndef MSG_FIN
#error MSG_FIN or MSG_EOF not defined
#endif

/*---------------------------------------------------------------------*/
/*--- main - try to connect to a t/tcp server.                      ---*/
/*---------------------------------------------------------------------*/

int main(int count, char *strings[]) {

  int sd;
  int port = DEFAULT_PORT;
  char *host = "127.0.0.1";
  struct sockaddr_in addr;
  int addr_len = sizeof(addr);
  char buffer[1024];

  if (count == 3) {
    port = atoi(strings[2]);
    host = strings[1];
  }
  else {
    printf("usage: %s <server's address> <server's port>\n...using default port (%d)\n", 
           strings[0], port);
  }

  sd = socket(PF_INET, SOCK_STREAM, 0);

//~-- Set destination address

  bzero(&addr, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  inet_aton(host, &addr.sin_addr);

//~-- Send message using directed protocol

  if (sendto(sd, "sean\n", 5, 0, (struct sockaddr*)&addr, sizeof(addr)) < 0) PANIC("sendto");

//~-- Get message using directed protocol

  bzero(buffer, sizeof(buffer));
  recvfrom(sd, buffer, sizeof(buffer), 0, (struct sockaddr*)&addr, (socklen_t*)&addr_len);
  printf("%s\n", buffer);
  close(sd);

  return 0;
}
