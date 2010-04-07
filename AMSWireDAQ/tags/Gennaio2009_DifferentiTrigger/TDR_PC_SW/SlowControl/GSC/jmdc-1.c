// file jmdc.c
//
// Send commands to JMDC via 1553 or HRDL server
//
// A.Lebedev, May-2008...
//
// A.Basili,  Aug-2008...
//

#include "jmdclib.h"
#include "jinflib.h"

#define JMDC_0 0
#define JMDC_1 1
#define JMDC_2 2
#define JMDC_3 3

#define ON     1
#define OFF    0
#define FOUND  2

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

  int k = 1;
  int16 err = 0;
  
  //  char *filename = "./jap.img";

  setbuf(stdout, NULL);
  printf("Usage: \n");
  printf("  %s 1553 <Server> <Node> [[P]]\n", argv[0]);
  printf("  %s HRDL <Server> <Node> [[P]]\n", argv[0]);
  printf("  %s EAS[:1553|:HRDL] <Server> <Node> [[P]]\n", argv[0]);

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
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
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
    if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
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
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("Node   = %02X\n", NODE_ADR);
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
      if (argc > k) NODE_ADR = strtoul(argv[k++], NULL, 16);
      if (argc > k) P       = atoi(argv[k++]);
      printf("Server  = %s\n",   CS_address);
      printf("Port    = %d\n",   CS_port);
      printf("Node    = %02X\n", NODE_ADR);
      printf("P       = %d\n",   P);
      printf("My format will be used\n");
    }
    else {
      use_AMSW = TRUE;
      k = 1;
      if (argc > k) TX = RX = atoi(argv[k++]);
      if (argc > k) AMSW_PATH[0] = strtoul(argv[k++], NULL, 16);
      if (argc > k) AMSW_PATH[1] = strtoul(argv[k++], NULL, 16);
      if (argc > k) P       = atoi(argv[k++]);
      printf("TX    = %d\n", TX);
      printf("RX    = %d\n", RX);
      printf("Path  = 0x%04X 0x%04X\n", AMSW_PATH[0], AMSW_PATH[1]);
      printf("P     = %d\n", P);
      printf("Direct connection to AMS-Wire will be used\n");
    }
  }

  file_summary (&err);

  return 0;
}

//~============================================================================
