// file dallas-6.c
//
// Read Dallas Temperatures via network
//
// A.Lebedev, Nov-2008...
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

int main(int argc, char *argv[]) {

  int16 err;
  int k = 1;
  int8 bus = 9, n;
  bool ok[64];
  float tmp[64];
  int16 age[64];

  setbuf(stdout, NULL);
  setup_command_path();
  use_CAN = FALSE;

  printf("Usage: \n");
  printf("  %s <USCM_ID> [<bus> [<JMDC_ID> [P]]]\n", argv[0]);
  printf("  %s <Server> <USCM_ID> [<bus> [<JMDC_ID> [P]]]\n", argv[0]);
  printf("  %s 1553 <Server> <Node> [<bus> [P]]\n", argv[0]);
  printf("  %s HRDL <Server> <Node> [<bus> [P]]\n", argv[0]);
  printf("  %s EAS[:1553|:HRDL|:CAN|:ECHO] <Server> <Node> [<bus> [P]]\n", argv[0]);

  if (argc < 2) PANIC("Too few arguments.\n");

  if      (FALSE) {
  }
/*
  else if (!strcasecmp(argv[1], "CAN")) {
    CS_port       = CAN_SERVER_PORT;
    use_CAN       = FALSE;
    use_TCP       = TRUE;
    use_Peter_TCP = FALSE;
    printf("CAN Server = %s\n", CS_address);
    printf("Port       = %d\n", CS_port);
    printf("My format will be used\n");
  }
*/
  else if (!strcasecmp(argv[1], "1553")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
    if (argc > k) bus     = atoi(argv[k++]);
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
    printf("Bus    = %d\n",   bus);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
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
    if (argc > k) bus     = atoi(argv[k++]);
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
    printf("Bus    = %d\n",   bus);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if ((! strcasecmp(argv[1], "EAS")) ||
           (!strncasecmp(argv[1], "EAS:", 4))) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    if      (!strcasecmp(argv[1], "EAS:1553")) CS_port = 61002;
    else if (!strcasecmp(argv[1], "EAS:HRDL")) CS_port = 61001;
    else if (!strcasecmp(argv[1], "EAS:CAN"))  CS_port = 61005;
    else if (!strcasecmp(argv[1], "EAS:ECHO")) CS_port = 61009;
    else                                       CS_port = 61001;
    use_APID      = 0x03D6;
    use_EAS       = TRUE;
    k = 3;
    if (argc > k) USCM_ID = strtoul(argv[k++], NULL, 16);
    if (argc > k) bus     = atoi(argv[k++]);
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", USCM_ID);
    printf("Bus    = %d\n",   bus);
    printf("P      = %d\n",   P);
    printf("New Peter's eassserver will be used\n");
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
      if (argc > k) bus     = atoi(argv[k++]);
      if (argc > k) JMDC_ID = atoi(argv[k++]);
      if (argc > k) P       = atoi(argv[k++]);
      printf("Server  = %s\n",   CS_address);
      printf("Port    = %d\n",   CS_port);
      printf("USCM_ID = %02X\n", USCM_ID);
      printf("Bus    = %d\n",   bus);
      printf("JMDC_ID = %d\n",   JMDC_ID);
      printf("P       = %d\n",   P);
      printf("My format will be used\n");
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
      if (argc > k) bus     = atoi(argv[k++]);
      if (argc > k) JMDC_ID = atoi(argv[k++]);
      if (argc > k) P       = atoi(argv[k++]);
      printf("USCM_ID = %02X\n", USCM_ID);
      printf("Bus TX  = %c\n",   BUS_FOR_REQUEST ? 'B' : 'A');
      printf("Bus RX  = %c\n",   BUS_FOR_REPLY   ? 'B' : 'A');
      printf("Bus    = %d\n",   bus);
      printf("JMDC_ID = %d\n",   JMDC_ID);
      printf("P       = %d\n",   P);
      printf("Direct connection to CAN-bus will be used\n");
      if (!initialize_EPP()) exit(1);
    }
  }

//~--

  NODE_ADR = USCM_ID;
  
  while (1) {
    read_DS1820_temp_long(bus, &n, ok, tmp, age, &err);
    if (err) printf("%s error = %04X\n",   timestamp(1), err);
    else     printf("%s USCM_ID %X: t =%+6.1f\n", 
                    timestamp(1), USCM_ID, tmp[0]);
    sleep(60);
  }

  return 0;
}

//~============================================================================
