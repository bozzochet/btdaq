// file ping-can.c
//
// Send PING command to CAN-bus
//
// Modification for network(s) connection
//
// Everything stolen from Jinghui Zhang and reworked
//
// A.Lebedev, Apr-2007...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x198;            // 1xx stands for Newborn ID
int BUS_FOR_REQUEST = 0;        // global variable
int BUS_FOR_REPLY   = 0;        // global variable
int P = 0;

char CS_address[100] = {"127.0.0.1"};  // global variable
int  CS_port;                          // global variable
int16 NODE_ADR;                        // global variable

int16 Request_Secondary_Header;        // global variable
int16 Reply_Secondary_Header[3];       // global variable

//~============================================================================

void ping__USCM(int n, int8 *dat, int *nn, int8 *datdat, int16 *err) {

// this is standard ping_USCM with unlimited n.

  int32 RQ_typ = 0x01;
  int32 RQ_cnt = n;
  int32 RP_cnt;

//if (RQ_cnt > 8192) {
//  *err = 0x0100;
//  return;
//}

//to_USCM(R_W,  RQ_typ, RQ_cnt, RQ_dat,  RP_siz, &RP_cnt, RP_dat, &RP_err);
  to_USCM(READ, RQ_typ, RQ_cnt,    dat, 1000000, &RP_cnt, datdat,     err);

  if (*err || use_MEM) return;
  
  if (nn) *nn = RP_cnt;
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;
  int k = 1;
  int n = 0, nn;
  int8 dat[1000000], datdat[sizeof(dat)];
  int i, c = 0, cc = 100;

  setbuf(stdout, NULL);
  setup_command_path();
  use_CAN = FALSE;

  printf("Usage: \n");
  printf("  %s <USCM_ID> [<n> [<JMDC_ID> [P]]]\n", argv[0]);
  printf("  %s <Server> <USCM_ID> [<n> [<JMDC_ID> [P]]]\n", argv[0]);
  printf("  %s 1553 <Server> <Node> [<n> [P]]\n", argv[0]);
  printf("  %s HRDL <Server> <Node> [<n> [P]]\n", argv[0]);
  printf("  %s EAS[:1553|:HRDL|:CAN|:ECHO] <Server> <Node> [<n> [P]]\n", argv[0]);

  if (argc < 2) PANIC("Too few arguments.\n");

  if (!strcasecmp(argv[1], "1553")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
    if (argc > k) n       = atoi(argv[k++]);
    if (n > sizeof(dat)) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
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
    if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
    if (argc > k) n       = atoi(argv[k++]);
    if (n > sizeof(dat)) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
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
    else if (!strcasecmp(argv[1], "EAS:CAN"))
      CS_port = 61005;
    else if (!strcasecmp(argv[1], "EAS:ECHO"))
      CS_port = 61009;
    else
      CS_port = 61001;
    use_APID      = 0x03D6;
    use_EAS       = TRUE;
    k = 3;
    if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
    if (argc > k) n       = atoi(argv[k++]);
    if (n > sizeof(dat)) PANIC("Parameter 'n' is too big.\n");
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
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
      if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
      if (argc > k) n       = atoi(argv[k++]);
      if (n > sizeof(dat)) PANIC("Parameter 'n' is too big.\n");
      if (argc > k) JMDC_ID = atoi(argv[k++]);
      if (argc > k) P       = atoi(argv[k++]);
      printf("Server  = %s\n",   CS_address);
      printf("Port    = %d\n",   CS_port);
      printf("USCM_ID = %02X\n", USCM_ID);
      printf("n       = %d\n",   n);
      printf("JMDC_ID = %d\n",   JMDC_ID);
      printf("P       = %d\n",   P);
      printf("My format will be used\n");
      if (n >    100) cc = 10;
      if (n > 100000) cc =  1;
    }
    else {
      use_CAN = TRUE;
      k = 1;
      if (argc > k) {
        USCM_ID = strtoul(argv[k++], NULL, 16);
        switch (USCM_ID & 0xFF) {
          case 0xAA: USCM_ID = USCM_ID / 256; BUS_FOR_REQUEST = 0; BUS_FOR_REPLY = 0; break;
          case 0xAB: USCM_ID = USCM_ID / 256; BUS_FOR_REQUEST = 0; BUS_FOR_REPLY = 1; break;
          case 0xBA: USCM_ID = USCM_ID / 256; BUS_FOR_REQUEST = 1; BUS_FOR_REPLY = 0; break;
          case 0xBB: USCM_ID = USCM_ID / 256; BUS_FOR_REQUEST = 1; BUS_FOR_REPLY = 1; break;
        }
      }
      if (argc > k) n       = atoi(argv[k++]);
      if (n > sizeof(dat)) PANIC("Parameter 'n' is too big.\n");
      if (argc > k) JMDC_ID = atoi(argv[k++]);
      if (argc > k) P       = atoi(argv[k++]);
      printf("USCM_ID = %02X\n", USCM_ID);
      printf("Bus TX  = %c\n",   BUS_FOR_REQUEST ? 'B' : 'A');
      printf("Bus RX  = %c\n",   BUS_FOR_REPLY   ? 'B' : 'A');
      printf("n       = %d\n",   n);
      printf("JMDC_ID = %d\n",   JMDC_ID);
      printf("P       = %d\n",   P);
      printf("Direct connection to CAN-bus will be used\n");
      if (n >    100) cc = 10;
      if (n > 100000) cc =  1;
      if (!initialize_EPP()) exit(1);
    }
  }

//~--

  NODE_ADR = USCM_ID;
  
//~--

  while (1) {
    struct timeval now;
    gettimeofday(&now, NULL);
//  for (i=0; i<n; i++) dat[i] = i + now.tv_usec;
    for (i=0; i<n; i++) dat[i] = i + 0x11;
    ping__USCM(n, dat, &nn, datdat, &err);
    if (!err) {
      if (n != nn) {
        printf("\r%d: sent %d bytes, received %d bytes.\n", c, n, nn);
      }
      else {
        for (i=0; i<n; i++) {
          if (dat[i] != datdat[i]) {
            printf("\r%d: %d-th byte: sent 0x%02hX, received 0x%02hX.\n",
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
