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
#include "treedef.h"
#include "nodedef.h"

#define JMDC_0 0
#define JMDC_1 1
#define JMDC_2 2
#define JMDC_3 3

#define ON  1
#define OFF 0

int P = 0;

bool use_CAN = FALSE;                              // global variable
bool use_MEM = FALSE;                              // global variable
bool use_UDP = FALSE;                              // global variable
bool use_TCP = FALSE;                              // global variable
bool use_EAS = FALSE;                              // global variable
bool use_BUFF = FALSE;                             // global variable
bool use_Peter_TCP = FALSE;                        // global variable
bool use_PCIAMSW = FALSE;                          // global variable

char CS_address[100] = {"127.0.0.1"};  // global variable
int  CS_port;                          // global variable
int16 NODE_ADR;                        // global variable

int16 Request_Secondary_Header;        // global variable
int16 Reply_Secondary_Header[3];       // global variable

char *filename = "/home/ams/jmdcfiles/jap.img";

//~----------------------------------------------------------------------------

void work_on_my_tree(void) {

  int i, j, k;

  for (k=0; k<DIM(tree); k++) {
    _tree *n = &tree[k];
    for (j=0; j<24; j++) {
      struct _link *l = &n->lnk[j];
      if (!l->nam) continue;
      for (i=0; i<512; i++) {
//printf("i=%d NodeName[i]=%s l->nam=%s\n", i, NodeName[i], l->nam);
        if (strncmp(NodeName[i], l->nam, strlen(l->nam)-1)) {
          l->adr = i;
//printf("i=%d NodeName[i]=%s l->nam=%s\n", i, NodeName[i], l->nam);
          break;
        }
      }
//printf("i=%d\n", i);
      if (i > 0x1FF) {
        printf("CDDC link name not found\n");
        exit(1);
      }
    }
  }

  if (P > 10) {
    printf("CDDC Internal Table:\n");
    printf("--------------------\n");
    for (k=0; k<DIM(tree); k++) {
      _tree *n = &tree[k];
      printf("%s:\n", n->nam);
      for (j=0; j<24; j++) {
        struct _link *l = &n->lnk[j];
        if (!l->nam) continue;
        printf("   %-3s %-15s %03X\n", l->crt, l->nam, l->adr);
      }
    }
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err = 0;
  int i, j, k = 1;
  int16 *dat = NULL;
  int16 *data = NULL;
  int16 m = 10;
  int16 n;
  int16 JMDC = 0;
 
  int jinj_nodes[4];
  int jinf_nodes[4][24];
  int xdr_nodes[4][24][24];

  setbuf(stdout, NULL);
  printf("Usage: \n");
  printf("  %s 1553 <Server> <JMDC> [P]\n", argv[0]);
  printf("  %s HRDL <Server> <JMDC> [P]\n", argv[0]);
  printf("  %s EAS  <Server> <JMDC> [P]\n", argv[0]);

  if (argc < 3) PANIC("Too few arguments.\n");

  if (!strcasecmp(argv[1], "1553")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61002;
    use_APID      = 0x0018;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    JMDC = atoi(argv[3]);
    k = 4;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("JMDC   = %d\n",   JMDC);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if (!strcasecmp(argv[1], "HRDL")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_TCP       = TRUE;
    use_Peter_TCP = TRUE;
    JMDC = atoi(argv[3]);
    k = 4;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("JMDC   = %d\n",   JMDC);
    printf("P      = %d\n",   P);
    printf("Peter's format with TCP packet length %d bytes will be used\n", 
            PETER_TCP_PACKET_LENGTH);
  }
  else if (!strcasecmp(argv[1], "EAS")) {
    if (argc < 4) PANIC("Too few arguments.\n");
    strncpy(CS_address, argv[2], 80);
    CS_port       = 61001;
    use_APID      = 0x03D6;
    use_EAS       = TRUE;
    JMDC = atoi(argv[3]);
    k = 4;
    if (argc > k) P       = atoi(argv[k++]);
    printf("Server = %s\n",   CS_address);
    printf("Port   = %d\n",   CS_port);
    printf("JMDC   = %d\n",   JMDC);
    printf("P      = %d\n",   P);
    printf("New Peter's eassserver will be used\n");
  }

  data = malloc (n*sizeof(int16));
  for (i=0; i<n; i++) *(data+i)=i;

  work_on_my_tree();

  /*

  NODE_ADR = 0x017;
  set_owner(JMDC, JMDC, JMDC, JMDC, &err);
  if (err){
    printf ("cannot set ownership! err = %04X\n", err);
    //    exit(1);
  }

  NODE_ADR = 0x010;
  hrdl_light(ON, &err);
  if (err){
    printf ("cannot switch on hrdl light! err = %04X\n", err);
    //    exit(1);
  }

  NODE_ADR = 0x010;
  hrdl_data(ON, &err);
  if (err){
    printf ("cannot start hrdl data! err = %04X\n", err);
    //    exit(1);
  }
  /*
  NODE_ADR = 0x010;
  load_JMDC_FLASH_file("01234567.exe", &err);
  if (err){
    printf ("cannot load FLASH file! err = %04X\n", err);
    exit(1);
  }
  NODE_ADR = 0x00C + JMDC;

  for (i=0; i<4; i++) {
    jinj_nodes[i]=OFF;
    for(j=0; j<24; j++){
      jinf_nodes[i][j]=OFF;
      for(k=0; k<24; k++)xdr_nodes[i][j][k]=OFF;
    }
  }

  for (i=0; i<4; i++){
    JINJ_PATH = 0;
    JINF_PATH = 0;
    AMSW_PATH[0] = 0;
    open_envelop (dat);
    pack16(&JINJ_PATH, 0xFF00, i);
    pack16(&JINJ_PATH, 0xFF, 0x3F);
    ping_JINF(m, data, NULL, NULL, &err);
    close_envelop(&dat, &n);
    if (P>10) print_envelop(dat, n);
    send_envelop(dat, n, &err);
    if (err) continue; 
    jinj_nodes[i] = ON;
    for (j=0; j<24; j++){
      JINF_PATH = 0;
      AMSW_PATH[0] = 0;
      open_envelop (dat);
      pack16(&JINF_PATH, 0xFF00, j);
      pack16(&JINF_PATH, 0xFF, 0x3F);
      ping_JINF(m, data, NULL, NULL, &err);
      close_envelop(&dat, &n);
      if (P>10) print_envelop(dat, n);
      send_envelop(dat, n, &err);
      if (err) continue;
      jinf_nodes[i][j] = ON;
      for (k=0; k<24; k++){
	AMSW_PATH[0] = 0;
	open_envelop (dat);
	pack16(&AMSW_PATH[0], 0xFF00, k);
	pack16(&AMSW_PATH[0], 0xFF, 0x3F);
	ping_JINF(m, data, NULL, NULL, &err);
	close_envelop(&dat, &n);
	if (P>10) print_envelop(dat, n);
	send_envelop(dat, n, &err);
	if (err) continue;
	xdr_nodes[i][j][k] = ON;
      }
    }
  }

  */

  printf("Connection table:\n");
  printf("--------------------\n");
  char ESC=27;
  for (i=0; i<DIM(tree); i++) {
    _tree *n = &tree[i];
    if (strncmp(n->nam, "JINJ", 4)==0){
      if ((n->nam[strlen(n->nam)-1] - '0')<=9){
	if (jinj_nodes[i]) printf("%c[1m", ESC); // turn on bold
	printf(" %5s[X] \n", n->nam);
	printf("%c[0m", ESC); // turn off bold
	for (j=0; j<24; j++) {
	  struct _link *l = &n->lnk[j];
	  if (!l->nam) continue;
	  if (jinf_nodes[i][j]) printf("%c[1m", ESC); // turn on bold
	  printf(" %15s[X] ", l->nam);
	  printf("%c[0m", ESC); // turn off bold
	  int x;
	  bool found = FALSE;
	  for (x=0; x<DIM(tree); x++)
	    if (strncmp(l->nam, tree[x].nam, strlen(l->nam)-3)==0) {
	      found = TRUE;
	      break;
	    }
	  printf("{ ");
	  for (k=0; k<24; k++){
	    if (xdr_nodes[i][j][k]) printf("%c[1m", ESC); // turn on bold
	    if (found) printf("%2s[X] ", tree[x].lnk[k].crt);
	    else printf ("%2s[ ] ", "--");
	    printf("%c[0m", ESC); // turn off bold
	  }
	  printf("}\n");
	}
      }
    }
  }
  return 0;
}

//~============================================================================
