// file sfet2conf.c
//
// A simple example to send SFET2 Command
//
// A.Lebedev - Jun 2007...
// A.Basili - Aug 2007

#include "sfet2conf.h"

#include "jinflib.h"
#include "sdr2def.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

int16 AMSW_PATH[5];         // global variable

int TX = 0;                 // global variable
int RX = 0;                 // global variable

int P = 0;                  // global variable

_cmd SFET2_table;
char *filename1 = "./setup.conf";
char *filename2 = "./control1.conf";
char *filename3 = "./control2.conf";
char *filename4 = "./control3.conf";
char *filename5 = "./control4.conf";

//~----------------------------------------------------------------------------

void read_command_file(char *filename, _cmd *SFET2_table, int size) {

  char fmt[80];
  int value;
  int i = 0;
  FILE *file;
  file = NULL;
  for (i=0; i<80; i++){
    fmt [i]= 0;
  }

  if (!file) file = fopen(filename, "r");
  if (!file) {
    fprintf(stderr, "File %s absent.\n", filename);
    exit(1);
  }
  while (1) {
    char b[8192];
    int k = -1;
    for (i=0; i<8192; i++){
      b [i]= 0;
    }
    if (!fgets(b, sizeof(b), file)) break;
    b[strlen(b)-1] = '\0';
    for (i=0; i<size; i++) {
      sprintf(fmt, "%s %s", SFET2_table[i].nam, SFET2_table[i].fmt);
      k = sscanf(b, fmt, &value);
      //      fprintf (stderr, "test: index = %04x, strlen = %04d, Nconv = %d, value = %04x\n", i, strlen(b), k, value);
      if (k == 1) {
	if (value < pow (2, SFET2_table[i].len)){
	  SFET2_table[i].val = value;
	  sprintf(fmt, "%%-50s 0x%%03x  %%04%s\n", &SFET2_table[i].fmt[1]);
 	  // exec_SDR2_SC_command (WRITE,  link, SFET2_table[i].addr, SFET2_table[i].val, NULL, NULL, &err);
	  // exec_SDR2_SC_command (WRITE,  link, SFET2_table[i].addr, value, NULL, NULL, &err);
	  // printf("writing 0x%04x to register 0x%03x\n", value, SFET2_table[i].addr);
	  // printf(fmt, table[i].nam, table[i].addr, table[i].val);
	}
	else printf("%-50s 0x%03x  ????\n", SFET2_table[i].nam, SFET2_table[i].addr);
	break;
      }
    }
  }
  fclose (file);
}

//~----------------------------------------------------------------------------

void jtag (bool r_w, int16 op, char *filename, int8 link, int16 *nerr){
  int i, n = 0;
  char fmt[80];
  _cmd *table;
  int16 b = 0;  
  int16 err;
  table = 0;
  if (r_w){

    // write operation on JTAG       //

    switch (op) {
    case TDC_SETUP:
      n = sizeof(SFET2_setup_table) / sizeof(SFET2_setup_table[0]);
      read_command_file(filename, SFET2_setup_table, n);
      table = SFET2_setup_table;
      break;
    case TDC_CONTROL:  
      n = sizeof(SFET2_control_table) / sizeof(SFET2_control_table[0]);
      read_command_file(filename, SFET2_control_table, n);
      table = SFET2_control_table;
      break;
    }

    for (i=0; i<n; i++) {
      if (P>0) fprintf(stderr, "%-50s 0x%03x  %04x\n", table[i].nam, table[i].addr, table[i].val);
      exec_SDR2_SC_command (WRITE,  link, table[i].addr, table[i].val, NULL, NULL, &err);
      if (err) {
	fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
	fprintf(stderr, "ERROR CODE: %04x\n", err);
	exit(1);
      }
    }
    
    //load the command to execute the setup on the jtag
    fprintf(stderr, "LOAD ACTEL REGISTERS                                      ");
    exec_SDR2_SC_command (WRITE,  link, 0x701, op, NULL, NULL, &err);
    if (err) {
      fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
      fprintf(stderr, "ERROR CODE: %04x\n", err);
      exit(1);
    }
    fprintf(stderr, "DONE\n");
    fprintf(stderr, "LOAD IR 0x%04x                                            ", op);
    //start the jtag
    //  while (1){
    exec_SDR2_SC_command (WRITE,  link, 0x701, op, NULL, NULL, &err);
    //    usleep (50000);
    if (err) {
      fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
      fprintf(stderr, "ERROR CODE: %04x\n", err);
      exit(1);
    }
    //  }
    fprintf(stderr, "DONE\n");
    fprintf(stderr, "JTAG WRITE RUNNING...                                     ");
    exec_SDR2_SC_command (WRITE,  link, 0x700, 1, NULL, NULL, &err);
    if (err) {
      fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
      fprintf(stderr, "ERROR CODE: %04x\n", err);
      exit(1);
    }
    b = 0;
    while (1){
      exec_SDR2_SC_command (READ,  link, 0x700, 0, &b, NULL, &err);
      if (err) {
	fprintf(stderr, "exec_SDR2_SC_command failed to READ\n");
	fprintf(stderr, "ERROR CODE: %04x\n", err);
	exit(1);
      }
      //polling on status bit
      if (!(0x4 & b)) {
	fprintf(stderr, "DONE\n");
	break;
      }
    }
  } else {

    // read operation on JTAG        //

    switch (op) {
    case TDC_SETUP  :
      n = sizeof(SFET2_setup_table) / sizeof(SFET2_setup_table[0]);
      table = SFET2_setup_table;
      fprintf(stderr, "CLEAN ALL SETUP REGISTERS                                 ");
      for (i=0; i<n; i++){
	exec_SDR2_SC_command (WRITE,  link, table[i].addr, 0, NULL, NULL, &err);
	if (err) {
	  fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
	  fprintf(stderr, "ERROR CODE: %04x\n", err);
	  exit(1);
	}
      }
      fprintf(stderr, "DONE\n");
      break;
    case TDC_CONTROL  :  
      n = sizeof(SFET2_control_table) / sizeof(SFET2_control_table[0]);
      table = SFET2_control_table;
      fprintf(stderr, "CLEAN ALL CONTROL REGISTERS                               ");
      for (i=0; i<n; i++){
	exec_SDR2_SC_command (WRITE,  link, table[i].addr, 0, NULL, NULL, &err);
	if (err) {
	  fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
	  fprintf(stderr, "ERROR CODE: %04x\n", err);
	  exit(1);
	}
      }
      fprintf(stderr, "DONE\n");
      break;
    case TDC_STATUS  :  
      n = sizeof(SFET2_status_table) / sizeof(SFET2_status_table[0]);
      table = SFET2_status_table;
      break;
    case TDC_IDCODE  :  
      n = sizeof(SFET2_idcode_table) / sizeof(SFET2_idcode_table[0]);
      table = SFET2_idcode_table;
      break;
    }
    
    //load the command to execute the setup on the jtag
    fprintf(stderr, "LOAD IR 0x%04x                                            ", op);
    exec_SDR2_SC_command (WRITE,  link, 0x701, op, NULL, NULL, &err);
    if (err) {
      fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
      fprintf(stderr, "ERROR CODE: %04x\n", err);
      exit(1);
    }
    fprintf(stderr, "DONE\n");
    
    fprintf(stderr, "JTAG READ RUNNING...                                      ");
    //  while (1){
    exec_SDR2_SC_command (WRITE,  link, 0x700, 2, NULL, NULL, &err);
    //usleep (50000);
    if (err) {
      fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
      fprintf(stderr, "ERROR CODE: %04x\n", err);
      exit(1);
    }
    //  }
    b = 0;
    while (1){
      exec_SDR2_SC_command (READ,  link, 0x700, 0, &b, NULL, &err);
      if (err) {
	fprintf(stderr, "exec_SDR2_SC_command failed to READ\n");
	fprintf(stderr, "ERROR CODE: %04x\n", err);
	exit(1);
      }
      //polling on status bit
      if (!(0x4 & b)) {
	fprintf(stderr, "DONE\n");
	break;
      }
    }
    for (i=0; i<n; i++) {
      sprintf(fmt, "%%-50s 0x%%03x  %%04%s\n", &table[i].fmt[1]);
      exec_SDR2_SC_command (READ,  link, table[i].addr, 0, &b, NULL, &err);
      if (err) {
	fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
	fprintf(stderr, "ERROR CODE: %04x\n", err);
	exit(1);
      }
      if (op != TDC_STATUS) {
	if (b != table[i].val) {
	  printf ("\nwrong value:\n");
	  *nerr = *nerr + 1;
	  printf(fmt, table[i].nam, table[i].addr, b);
	} 
      } else printf(fmt, table[i].nam, table[i].addr, b);
    }
  }
}


//~============================================================================

int main(int argc, char *argv[]) {

  int i;
  int8 link = 0;
  int16 err = 0;
  //  _cmd *table;  
  int16 nerr = 0;

  setbuf(stdout, NULL);
  setup_command_path();
  use_AMSW = FALSE;

  fprintf(stderr, "Usage: %s [port [{Path|0} [link (0-6) [setup control1 control2 control3 control4 [[[P]]]]]]]\n", argv[0]);
  if (argc < 2) {
    printf("Too few arguments.\n");
    exit(1);
  }
  for (i=0; i<10; i++) AMSW_PATH[i] = 0;
  
  i = 1;
  if (argc > i) TX           = RX = atoi(argv[i++]);
  if (argc > i) AMSW_PATH[0] = strtoul(argv[i++], NULL, 16);
  if (argc > i) link         = atof(argv[i++]);
  if (argc > i) filename1    = argv[i++];
  if (argc > i) filename2    = argv[i++];
  if (argc > i) filename3    = argv[i++];
  if (argc > i) filename4    = argv[i++];
  if (argc > i) filename5    = argv[i++];
  if (argc > i) P            = atoi(argv[i++]);
  
  fprintf(stderr, "port      = %d\n", TX);
  fprintf(stderr, "Path      = 0x%04X\n", AMSW_PATH[0]);
  fprintf(stderr, "link      = %d\n", link);
  fprintf(stderr, "setup     = %s\n", filename1);
  fprintf(stderr, "control1  = %s\n", filename2);
  fprintf(stderr, "control2  = %s\n", filename3);
  fprintf(stderr, "control3  = %s\n", filename4);
  fprintf(stderr, "control4  = %s\n", filename5);
  fprintf(stderr, "P         = %d\n", P);
  if (link == 3){
    printf("Not valid config for SPT2.\n");
    exit(1);
  }
  //enable internal jtag;
printf ("WRITE = %04X, link = %04X, err = %04X \n", WRITE, link, err);
  exec_SDR2_SC_command(WRITE, link, 0x702, 0, NULL, NULL, &err);
  if (err) {
    fprintf(stderr, "failed to enable internal jtag\n");
    fprintf(stderr, "ERROR CODE: %04x\n", err);
    exit(1);
  }

  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                    WRITE CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (WRITE, TDC_CONTROL, filename2, link, NULL);

  fprintf(stderr, "RESET JTAG                                                ");
  exec_SDR2_SC_command (WRITE,  link, 0x700, 3, NULL, NULL, &err);
  if (err) {
    fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
    exit(1);
  }
  fprintf(stderr, "DONE\n");

  fprintf(stderr, "DISABLE TDC CLOCK                                  ");
  exec_SDR2_SC_command (WRITE,  link, 0x808, 0, NULL, NULL, &err);
  if (err) {
    fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
    exit(1);
  }
  fprintf(stderr, "DONE\n");

  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     WRITE SETUP REGISTER                   *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (WRITE, TDC_SETUP, filename1, link, &nerr);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     READ SETUP REGISTER                    *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (READ, TDC_SETUP, filename1, link, &nerr);
  if (nerr != 0){
    fprintf(stderr, "TEST ON SETUP REGISTER FAILED! NUMBER OF ERRORS = %d\n", nerr);
  } else {
    fprintf(stderr, "TEST ON SETUP REGISTER OK!\n");
  }
  fprintf(stderr, "ENABLE TDC CLOCK                                   ");
  exec_SDR2_SC_command (WRITE,  link, 0x808, 1, NULL, NULL, &err);
  if (err) {
    fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
    exit(1);
  }
  fprintf(stderr, "DONE\n");
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     READ CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (READ, TDC_CONTROL, NULL, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                    WRITE CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (WRITE, TDC_CONTROL, filename3, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     READ CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (READ, TDC_CONTROL, NULL, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                    WRITE CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (WRITE, TDC_CONTROL, filename4, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     READ CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (READ, TDC_CONTROL, NULL, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                    WRITE CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (WRITE, TDC_CONTROL, filename5, link, NULL);
  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                     READ CONTROL REGISTER                  *\n");
  fprintf(stderr, "**************************************************************\n");
  jtag (READ, TDC_CONTROL, NULL, link, NULL);

  fprintf(stderr, "**************************************************************\n");
  fprintf(stderr, "*                       SET TRIGGER DELAY                    *\n");
  fprintf(stderr, "**************************************************************\n");

  fprintf(stderr, "TRIGGER DELAY = 0x0001                                    ");
  exec_SDR2_SC_command (WRITE,  link, 0x900, 1, NULL, NULL, &err);
  if (err) {
    fprintf(stderr, "exec_SDR2_SC_command failed to WRITE\n");
    exit(1);
  }
  fprintf(stderr, "DONE\n");

  return 0;
}

//~============================================================================
