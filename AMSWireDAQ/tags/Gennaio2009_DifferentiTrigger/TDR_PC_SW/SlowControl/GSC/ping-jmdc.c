// file ping-jmdc.c
//
// Send PING commands to JMDC
//
// A.Lebedev May-2008...

#include "jmdclib.h"

int P = 0;

bool use_CAN = FALSE;                              // global variable
bool use_MEM = FALSE;                              // global variable
bool use_UDP = FALSE;                              // global variable
bool use_TCP = FALSE;                              // global variable
bool use_EAS = FALSE;                              // global variable
bool use_Peter_TCP = FALSE;                        // global variable
bool use_PCIAMSW = FALSE;                          // global variable

char CS_address[100] = {"127.0.0.1"};  // global variable
int  CS_port;                          // global variable
int16 NODE_ADR;                        // global variable

int16 Request_Secondary_Header;        // global variable
int16 Reply_Secondary_Header[3];       // global variable

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int k = 1;
  int n = 0, nn;
  int16 dat[0x10000], datdat[sizeof(dat)/2];
  int i, c = 0, cc = 100;

  setbuf(stdout, NULL);
  printf("Usage: \n");
  printf("  %s 1553 <Server> [<n> [P]]\n", argv[0]);
  printf("  %s HRDL <Server> [<n> [P]]\n", argv[0]);
  printf("  %s EAS <Server> [<n> [P]]\n", argv[0]);

  if (argc < 2) PANIC("Too few arguments.\n");

  if (!strcasecmp(argv[1], "1553")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) n        = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
    cc = 1;
  }
  else if (!strcasecmp(argv[1], "HRDL")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) n        = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
    cc = 1;
  }
  else if (!strcasecmp(argv[1], "EAS")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_EAS       = TRUE;
    k = 3;
    if (argc > k) n        = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("New Peter's eassserver will be used\n");
    cc = 1;
  }

//~--

  NODE_ADR = 0x013;

//~--

  while (1) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (i=0; i<n; i++) dat[i] = i + now.tv_usec;
    ping_JMDC(n, dat, &nn, datdat, &err);
    if (!err) {
      if (n != nn) {
        printf("\r%d: sent %d words, received %d words.\n", c, n, nn);
      }
      else {
        for (i=0; i<n; i++) {
          if (dat[i] != datdat[i]) {
            printf("\r%d: %d-th word: sent 0x%04X, received 0x%04X.\n",
                    c, i, dat[i], datdat[i]);
            break;
          }
        }
      }  
      if (!(++c % cc)) printf("\r             %d\r", c);
    }
    else {
      printf("\r%d: *** err = 0x%04X %c\n", c, err, 7);
    }
//break;
  }
  
  return 0;
}

//~============================================================================
