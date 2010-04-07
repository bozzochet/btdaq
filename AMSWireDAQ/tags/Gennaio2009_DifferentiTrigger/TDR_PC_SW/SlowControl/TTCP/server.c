// file server.c
//
// study of TCPIP protocol - A.Lebedev Apr-2004...
//
// stolen from Linux Socket Programming book (ttcp-server.c)
// and reworked using ideas stolen from Weixin Qiao
//

//----------------------------------------------------------------------------
//
//   ttcp-server.c
//
//   Test support for transaction-TCP (T/TCP).
//
//----------------------------------------------------------------------------

#include "mylib.h"

#define DEFAULT_PORT	9999

int P = 5;

#define QUIT(msg) {perror(msg); exit(1);}

//----------------------------------------------------------------------------

int RCV(int port, int *client, char *buffer, int siz) {

  static int first = 1;
  static int sd;
  struct sockaddr_in adr;
  int one = 1;
  
  if (first) {
    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) QUIT("socket:");
    setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *)&one, sizeof(one));

    bzero(&adr, sizeof(adr));
    adr.sin_family = AF_INET;
    adr.sin_port = htons(port);
    adr.sin_addr.s_addr = INADDR_ANY;
    if (bind(sd, (struct sockaddr*)(&adr), sizeof(adr)) != 0) QUIT("bind");

    if (listen(sd, 20) < 0) QUIT("listen");

    first = 0;
  }

//*client = accept(sd, 0, 0);
  siz = sizeof(adr);
  *client = accept(sd, (struct sockaddr*)(&adr), (socklen_t*)&siz);
  if (*client < 0) QUIT("accept");
  if (P > 0) printf("master connected from %s\n", inet_ntoa(adr.sin_addr));
  
  return recv(*client, buffer, siz, 0);
}

//----------------------------------------------------------------------------

void SND(int client, char *buffer, int cnt) {

  send(client, buffer, cnt, 0);
  close(client);
  printf("disconnected\n");
}

//----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int port=DEFAULT_PORT;
  int l, n = 0;
  char request[1024], reply[1024];
  int client;

  setbuf(stdout, NULL);
  printf("Usage: %s [port [P]]\n", argv[0]);

  if (argc > 1) port = atoi(argv[1]);
  if (argc > 2) P = atoi(argv[2]);

  printf("port = %d\n", port);

  while (1) {
    l = RCV(port, &client, request, sizeof(request));
    request[l] = '\0';
    sprintf(reply, " %s %d", request, n++);
    SND(client, reply, strlen(reply));
  }
}

//----------------------------------------------------------------------------
