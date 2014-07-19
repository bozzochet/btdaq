// file slave.c
//
// study of TCPIP protocol - A.Lebedev Apr-2004...
// see also file master.c
//
// everything stolen from Weixin Qiao and reworked

#include "mylib.h"
#include <stdlib.h> 
#include <stdio.h> 
#include <errno.h> 
#include <string.h> 
#include <netdb.h> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <pthread.h>
#include <sys/poll.h>

int P = 5;

typedef struct {
  int16 siz;
  int16 cnt;
  int8 *dat;
} _msg;

//-----------------------------------------------------------------------------

int setup_slave_socket(int portnumber) {

  int status;
  int sock;
  struct sockaddr_in adr;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock == -1) {
    perror("socket failed");
    exit(1);
  }

  bzero(&adr, sizeof(adr));
  adr.sin_family      = AF_INET;
  adr.sin_addr.s_addr = htonl(INADDR_ANY);
  adr.sin_port        = htons(portnumber);

  status = bind(sock, (struct sockaddr*)(&adr), sizeof(adr));
//status = bind(sock, &adr, sizeof(adr));
  if (status == -1) {
    perror("bind failed");
    exit(1);
  }

//status = listen(sock, 5);
  status = listen(sock, 20);
  if (status == -1) {
    perror("listen failed");
    exit(1);
  } 

  return sock;
}

//-----------------------------------------------------------------------------

int accept_master_connection(int sock, int *fd) {

  struct sockaddr_in adr;
  int siz;
  struct pollfd ufd;
  int status;
  static int old_status = -1, old_ufd_revents = -1;
  int timeout = 2; // in msec

  ufd.fd     = sock;
  ufd.events = POLLIN;
  status = poll(&ufd, 1, timeout);
  if (status == -1) {
    perror("acc::poll: failed");
    exit(1);
  }

  if (status != old_status || ufd.revents != old_ufd_revents) {
    old_status = status;
    old_ufd_revents = ufd.revents;
    if (P > 0) printf("acc::poll: status = %d, revents = %04X\n", 
                       status, ufd.revents);
  }

  if (status == 0) return 0;

  siz = sizeof(adr);
  *fd = accept(sock, (struct sockaddr*)(&adr), (socklen_t*)&siz);
//*fd = accept(sock, 0, 0);
  if (*fd == -1) {
    perror("acc::accept: failed");
    exit(1);
  }
  if (P > 0) printf("acc:       master connected from %s\n",
                     inet_ntoa(adr.sin_addr));
  return 1;
}

//-----------------------------------------------------------------------------

int RCV(int sock, int *fd, char *dat, int *siz) {

  int status;
  static int connected = 0;
  struct pollfd ufd;
  static int old_status = -1, old_ufd_revents = -1;
  int timeout = 2; // in msec

  if (!connected) connected = accept_master_connection(sock, fd);

  if (connected ) {

    ufd.fd     = *fd;
    ufd.events = POLLIN;
    status = poll(&ufd, 1, timeout);
    if (status == -1) {
      perror("RCV::poll: failed");
      exit(1);
    }

    if (status != old_status || ufd.revents != old_ufd_revents) {
      old_status = status;
      old_ufd_revents = ufd.revents;
      if (P > 0) printf("RCV::poll: status = %d, revents = %04X\n", 
                         status, ufd.revents);
    }

    if (status) {
      if (ufd.revents == POLLIN) {
        *siz = recv(*fd, dat, *siz, 0);
        if (P > 0) printf("RCV::recv: siz = %d\n", *siz);
        if (*siz) return 1;
      }
      else {
        if (P > 0) printf("RCV:       closing connection...\n");
        close(*fd);
        connected = 0;
        return 0;
      }
    }
    else {
      return 0;
    }
  }
  else {
    return 0;
  }

  return 0;
}

//-----------------------------------------------------------------------------

void SND(int fd, char *reply, int cnt) {

  int status;
  struct pollfd ufd;
  int timeout = 2;  // in msec

  ufd.fd     = fd;
  ufd.events = POLLOUT;
  status = poll(&ufd, 1, timeout);
  if (P > 0) printf("SND::poll: status = %d, revents = %04X\n",
                     status, ufd.revents);
  if (status == -1) {
    perror("SND::poll: failed");
    exit(1);
  }

  if (status > 0) {
    if (ufd.revents == POLLOUT) {
      status = send(fd, reply, cnt, 0);
      if (P > 0) printf("SND::send: status = %d\n", status);
      if (status == -1) {
        perror("SND::send: failed");
        exit(1);
      }
    }
  }
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int portnumber = 2323; 

  char request[256];
  char reply[256];
  int sock, fd;
  int siz;
  int n = 0;

//  _msg SND, *pSND = &SND;
//  _msg RCV, *pRCV = &RCV;

  setbuf(stdout, NULL);
  sock = setup_slave_socket(portnumber);

  while (1) {
    siz = sizeof(request);
    if (RCV(sock, &fd, request, &siz)) {
      request[siz] = '\0';
      if (P > 0) printf("recieved:  siz = %d, %s\n", siz, request);
      sleep(argc > 1 ? atoi(argv[1]) : 0);
      sprintf(reply, "REPLY %3d: %s", n++, request);
      SND(fd, reply, strlen(reply));
    }
  }    

  return 0;
}

//-----------------------------------------------------------------------------
