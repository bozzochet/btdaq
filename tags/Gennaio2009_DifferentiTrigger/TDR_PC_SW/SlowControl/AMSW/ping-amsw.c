// file ping-amsw.c
//
// Send PING command to AMS-Wire link
//
// A.Lebedev Oct-2006...

#include "daqlib.h"

//~----------------------------------------------------------------------------

int TX = 2;                 // global variable
int RX = 2;                 // global variable

int P = 0;                                         // global variable
 
int16 AMSW_PATH[5];         // global variable
int16 JINF_PATH = 0x0000;   // global variable

int16 NODE_ADR;                        // global variable
char CS_address[100] = {"127.0.0.1"};  // global variable
int  CS_port;                          // global variable

//~============================================================================

void ping__JINF(int n, int16 *dat, int *nn, int16 *datdat, int16 *err) {

// this is standard ping_JINF with increased RP_siz.

  int32 RQ_typ = 0x0D;
  int32 RQ_cnt = n;
  int32 RP_cnt;

//to_AMSW(R_W,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, *RP_err);
  to_AMSW(READ, RQ_typ, RQ_cnt,    dat, 1000000, &RP_cnt, datdat,     err);

  if (*err) return;
  
  if (nn) *nn = MAX(0, RP_cnt-2);
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int k = 1;
  int n = 0, nn;
  int16 dat[1000000], datdat[sizeof(dat)/2];
  int i, c = 0, cc = 100;

  for (i=0; i<5; i++) AMSW_PATH[i] = 0;
  
  setbuf(stdout, NULL);
  setup_command_path();
  use_AMSW = FALSE;
  
  printf("Usage: \n");
  printf("  %s <TX&RX> <Path> {<Path>|0} [<n> [P]]\n", argv[0]);
  printf("  %s <Server> <Node> [<n> [P]]\n", argv[0]);
  printf("  %s 1553 <Server> <Node> [<n> [P]]\n", argv[0]);
  printf("  %s HRDL <Server> <Node> [<n> [P]]\n", argv[0]);
  printf("  %s EAS[:1553|:HRDL] <Server> <Node> [<n> [P]]\n", argv[0]);

  if (argc < 2) PANIC("Too few arguments.\n");

  if (!strcasecmp(argv[1], "1553")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
    if (argc > k) n        = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
    cc = 1;
  }
  else if (!strcasecmp(argv[1], "HRDL")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
    if (argc > k) n        = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
    cc = 1;
  }
  else if ((! strcasecmp(argv[1], "EAS")) |
           (!strncasecmp(argv[1], "EAS:", 4))) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    if      (!strcasecmp(argv[1], "EAS:1553"))
      CS_port = 61002;
    else if (!strcasecmp(argv[1], "EAS:HRDL"))
      CS_port = 61001;
    else
      CS_port = 61001;
    use_APID      = 0x03D6;
    use_EAS       = TRUE;
    k = 3;
    if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
    if (argc > k) n       = atoi(argv[k++]);
    if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
    printf("n      = %d\n",   n);
    printf("P      = %d\n",   P);
    printf("New Peter's eassserver will be used\n");
    cc = 1;
  }
  else {
    if (strlen(argv[1]) != strspn(argv[1], "0123456789abcdefABCDEF")) {
      if (argc < 3) PANIC("Too few arguments.\n");
      strncpy(CS_address, argv[1], 80);
      CS_port = CAN_SERVER_PORT;
      use_TCP       = TRUE;
      use_Peter_TCP = FALSE;
      k = 2;
      if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
      if (argc > k) n        = atoi(argv[k++]);
      if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
      if (argc > k) P       = atoi(argv[k++]);
      printf("Server  = %s\n",   CS_address);
      printf("Port    = %d\n",   CS_port);
      printf("Node    = %02X\n", NODE_ADR);
      printf("n       = %d\n",   n);
      printf("P       = %d\n",   P);
      printf("My format will be used\n");
      if (n >    100) cc = 10;
      if (n > 100000) cc =  1;
    }
    else {
      use_AMSW = TRUE;
      k = 1;
      if (argc > k) TX = RX = atoi(argv[k++]);
      if (argc > k) AMSW_PATH[0] = strtoul(argv[k++], NULL, 16);
      if (argc > k) AMSW_PATH[1] = strtoul(argv[k++], NULL, 16);
      if (argc > k) n       = atoi(argv[k++]);
      if (n > sizeof(dat)/2) PANIC("Parameter 'n' is too big.\n");
      if (argc > k) P       = atoi(argv[k++]);
      printf("TX    = %d\n", TX);
      printf("RX    = %d\n", RX);
      printf("Path  = 0x%04X 0x%04X\n", AMSW_PATH[0], AMSW_PATH[1]);
      printf("n     = %d\n", n);
      printf("P     = %d\n", P);
      printf("Direct connection to AMS-Wire will be used\n");
      if (n >    100) cc = 10;
      if (n > 100000) cc =  1;
    }
  }

//~--

  set_AMSW_timeout(1.0);
  use_PCIAMSW = FALSE;
  
//~--

  while (1) {
    struct timeval now;
    gettimeofday(&now, NULL);
    for (i=0; i<n; i++) dat[i] = i + now.tv_usec;
    ping__JINF(n, dat, &nn, datdat, &err);
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
