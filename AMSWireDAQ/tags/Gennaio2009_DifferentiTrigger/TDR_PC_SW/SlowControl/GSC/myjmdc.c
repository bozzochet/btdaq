// file myjmdc.c
//
// Send commands to JMDC via 1553 or HRDL server
//
// A.Lebedev, May-2008...
//

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
  int i;
  int k = 1;
  int n;
  int16 dat[100000];
  int16 status, old_status;

  setbuf(stdout, NULL);
  printf("Usage: \n");
  printf("  %s 1553 <Server> [P]\n", argv[0]);
  printf("  %s HRDL <Server> [P]\n", argv[0]);
  printf("  %s EAS  <Server> [P]\n", argv[0]);

  if (argc < 2) PANIC("Too few arguments.\n");

  if (!strcasecmp(argv[1], "1553")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if (!strcasecmp(argv[1], "HRDL")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    k = 3;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if (!strcasecmp(argv[1], "EAS")) {
    if (argc < 3) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_EAS       = TRUE;
    k = 3;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("P      = %d\n",   P);
    printf("New Peter's eassserver will be used\n");
  }

//~--

  NODE_ADR = 0x013;

//~--

  check_JMDC_Q_list_status(&old_status, &err);
  if (err) EXIT("read_JMDC_Q_list_status failed");
  printf("Q-list Status = %d\n", old_status);

  start_JMDC_Q_list(&err);
  if (err) EXIT("start_JMDC_Q_list failed");
  printf("Q-list Started OK\n");

  check_JMDC_Q_list_status(&status, &err);
  if (err) EXIT("read_JMDC_Q_list_status failed");
  printf("Q-list Status = %d\n", status);

  stop_JMDC_Q_list(&err);
  if (err) EXIT("stop_JMDC_Q_list failed");
  printf("Q-list Stopped OK\n");

  check_JMDC_Q_list_status(&status, &err);
  if (err) EXIT("read_JMDC_Q_list_status failed");
  printf("Q-list Status = %d\n", status);

  start_JMDC_Q_list(&err);
  if (err) EXIT("start_JMDC_Q_list failed");
  printf("Q-list Started OK\n");

  check_JMDC_Q_list_status(&status, &err);
  if (err) EXIT("read_JMDC_Q_list_status failed");
  printf("Q-list Status = %d\n", status);

  write_JMDC_Q_list_status(old_status, &err);
  if (err) EXIT("write_JMDC_Q_list_status failed");
  printf("Q-list Status written OK\n");

  check_JMDC_Q_list_status(&status, &err);
  if (err) EXIT("read_JMDC_Q_list_status failed");
  printf("Q-list Status = %d\n", status);

//~--

  read_JMDC_Q_list(&n, dat, &err);
  if (err) EXIT("read_JMDC_Q_list failed");

  printf("Q-list: n = %d\n", dat[0]);

  for (i=0; i<n; i+=4) {
    printf("%04X ", dat[i+0]);
    printf("%04X ", dat[i+1]);
    printf("%04X ", dat[i+2]);
    printf("%04X ", dat[i+3]);
    printf("\n");
    if (dat[i+4] == 0xFFFF) break;
  }

  return 0;
}

//~============================================================================
