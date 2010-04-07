// file t.c
//
//  Command Line (a.k.a. "Text") Commander for AMS-2
//  (grandgrandchild of AMS-1 "t" program)
//  Version for CAN+USCM
//
//  A.Lebedev Dec 2004...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x190;    // 1xx stands for Newborn ID
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY   = 0;                           // global variable

int8 dat[8];

//~----------------------------------------------------------------------------

static void Usage(char *name) {

  printf("Usage:\n");
  printf("       %s R|W <Node> <DataType> [<Data>] - USCM commanding\n", name);
  exit(1);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int8 Data[10000];
  int32 DataCount;
  int16 Node;
  int32 DataType;
  int8 R_W = 0;
  int32 RQ_typ;
  int i;
  int OpCode;
  int16 err;

//~--

  setbuf(stdout, NULL);
  setup_command_path();

  if (argc < 4) Usage(argv[0]);

  OpCode = toupper((int)*argv[1]);

  if      (OpCode == 'R') R_W = READ;
  else if (OpCode == 'W') R_W = WRITE;
  else                    Usage(argv[0]);
  
  if (argv[1][1] == '+') P = 3;

  Node     = strtoul(argv[2], NULL, 16);
  USCM_ID  = Node;
  DataType = strtoul(argv[3], NULL, 16);

  DataCount = 0;

  i = 4;
  while (1) {
    int l;
    char b[80];
    char *p;
    static FILE *file = NULL;
    if (i >= argc) break;
    if (file) {
      if (fscanf(file, "%s", b) <= 0) break;
      p = b;
    }
    else {
      if (argv[i][0] != '@') {
        p = argv[i++];
      }
      else {
        if (argv[i][1] != '@') {
          if (!(file = fopen(&argv[i][1], "r"))) {
            printf("File %s not found.\n", &argv[i][1]);
            exit(1);
          }
          continue;
        }
        else {
          int file;
          struct stat b, *sta = &b;
          int32 DataSize;
          printf("Binary input from file %s requested\n", &argv[i][2]);
          if ((file = open(&argv[i][2], O_RDONLY)) < 0) {
            printf("File %s not found.\n", &argv[i][2]);
            exit(1);
          }
          else {
            printf("open: file = %d\n", file);
            if (fstat(file, sta) < 0) {
               printf("Failed to get size of file %s\n", &argv[i][2]);
               exit(1);
            }
            printf("sta->st_size = %d\n", (int)sta->st_size);
            DataSize = read(file, &Data[DataCount], sta->st_size);
            printf("file read, DataSize = %d\n", (unsigned int)DataSize);
            DataCount = DataCount + DataSize;
//          exit(1);
            break;
          }
        }
      }
    }
    l = strlen(p);
    if (l > 2) {
      if (DataCount % 2) DataCount++;
      Data[DataCount++] = strtoul(p, NULL, 16) >> 8;
      Data[DataCount++] = strtoul(p, NULL, 16) & 0x00FF;
    }
    else {
      Data[DataCount++] = strtoul(p, NULL, 16);
    }
  }

//~--

  printf("SND: ");
  for (i=0; i<DataCount; i++) {
    printf("%02X ", Data[i]);
  }
  printf("\n");
  
  RQ_typ = DataType;
//to_USCM(R_W, RQ_typ,    RQ_cnt, RQ_dat, RP_siz,    &RP_cnt, RP_dat, &RP_err);
  to_USCM(R_W, RQ_typ, DataCount,   Data,  10000, &DataCount,   Data,    &err);

  if (err) {
    printf("RCV: error 0x%04X\n", err);
  }
  else {
    printf("RCV: ");
    for (i=0; i<DataCount; i++) {
      printf("%02X ", Data[i]);
    }
    printf("\n");
  }

  return 0;
}

//~============================================================================
