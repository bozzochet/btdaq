// file master.c
//
// study of TCPIP protocol - A.Lebedev Apr-2004...
// see also file slave.c
//
// everything stolen from Weixin Qiao and reworked

#include "mylib.h"

//char *host = "pcaal02";
//char *host = "helium";
char *host = "localhost";
int port   = 9999; 

int P = 5;

typedef struct {
  int16 siz;
  int16 cnt;
  int8 *dat;
} _msg;

#define QUIT(msg) {perror(msg); exit(1);}

//-----------------------------------------------------------------------------

void SNDRCV(char *hostname, int port, _msg *SND, _msg *RCV, int16 *err) {

  int sd;
  int status;
  static struct timeval now, then;
  static struct sockaddr_in dest;
  static struct hostent *host;
  struct pollfd ufd;
  int timeout = 2; // in msec
  static int old_status = -1, old_ufd_revents = -1;

  gettimeofday(&then, NULL);

  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) QUIT("socket");

  if (!(host = gethostbyname(hostname))) QUIT("host");
  
  bzero(&dest, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_addr   = *((struct in_addr *)host->h_addr);
  dest.sin_port   = htons(port);

  if (connect(sd, (struct sockaddr *)&dest, sizeof(dest))) {
    perror("connect");
    *err = 0x0001;
    close(sd);
    return;
  }

  status = send(sd, SND->dat, SND->cnt, 0);
  if (P > 2) printf("SNDRCV::send: %s, status = %d\n", SND->dat, status);

  while (1) {
    gettimeofday(&now, NULL);
    if (delta_t(&now, &then) > 3.0) {
      if (P > 1) printf("SNDRCV:       time-out:\n");
      *err = 0x0002;
      break;
    }
    ufd.fd     = sd;
    ufd.events = POLLIN;
    if ((status = poll(&ufd, 1, timeout)) < 0) QUIT("poll");
    if (status != old_status || ufd.revents != old_ufd_revents) {
      old_status = status;
      old_ufd_revents = ufd.revents;
      if (P > 2) printf("SNDRCV::poll: status = %d, revents = %04X\n", 
                         status, ufd.revents);
    }
    if (status) {
      if (ufd.revents & POLLIN) {
        status = recv(sd, RCV->dat, RCV->siz, 0);
        if (status <= 0) {
          if (P > 1) printf("SNDRCV::recv: status = %d\n", status);
          *err = 0x0003;
          break;
        }
        else {
          RCV->cnt = status;
          if (P > 2) printf("SNDRCV::recv: cnt = %d\n", RCV->cnt);
          *err = 0;
          break;
        }
      }
      else {
        *err = 0x0004;
        break;
      }
    }
  }

  close(sd);
}

//-----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 reply[256];
  int16 err;
  int autorepeat = 0;
  int8 *request = NULL;

  _msg *pSND;
  _msg *pRCV;

  setbuf(stdout, NULL);
  printf("Usage: %s [host [port [P]]]\n", argv[0]);

  if (argc > 1) host =      argv[1];
  if (argc > 2) port = atoi(argv[2]);
  if (argc > 3) P    = atoi(argv[3]);

  printf("host = %s, port = %d\n", host, port);

  pSND = malloc(sizeof(_msg));
  pRCV = malloc(sizeof(_msg));

  while (1) {
    char *BOOT = {"BOOT"};
    char *IDLE = {"IDLE"};
    char *PING = {"PING"};
    char *QUIT = {"QUIT"};
    char *RUN  = {"RUN"};
    if (!autorepeat) request = NULL;
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'A': autorepeat = 1 - autorepeat; break;
        case 'B': request = (int8*)BOOT; break;
        case 'I': request = (int8*)IDLE; break;
        case 'P': request = (int8*)PING; break;
        case 'Q': request = (int8*)QUIT; break;
        case 'R': request = (int8*)RUN; break;
        case '+': P = MIN(P+1, 3); break;
        case '-': P = MAX(P-1, 0); break;
      }
    }
    if (request) {
      pSND->cnt = strlen((char*)request);
      pSND->dat = request;
      pRCV->siz = sizeof(reply);
      pRCV->dat = reply;
      SNDRCV(host, port, pSND, pRCV, &err);
      if (err) {
        if (P > 0) printf("error:        err = %04X\n", err);
        exit(1);
      }
      else {
        reply[pRCV->cnt] = '\0';
        if (P > 0) printf("recieved:     cnt = %d, %s\n", pRCV->cnt, reply);
      }
    }
  }

  return 0;
}

//-----------------------------------------------------------------------------
