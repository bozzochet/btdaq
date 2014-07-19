// file vers.c
//
// rather ugly program to get version out of *.hex file (see also load.c)
// note: this program is for HBB software only
//
// A.Lebedev Mar-2005...
//

#include "mylib.h"

int32 address_for_validate = 0xFFFFFFFF;

typedef struct {
  int16 offset;
  int8  type;
  int16 length;
  int8  address[3];
  int8  data[255];
  int8  chksum;
} HEX_RECORD;

HEX_RECORD record;

int8 memory[0x40000];

FILE *file;
int eof;

//~----------------------------------------------------------------------------

int parse_and_load_line(char *b, int load) {

  int i, j, l;
  int8 chksum;
  static int8 old_type = 0xFF;
  static int32 LBA = 0x00000000;
  int32 address;
  static int first = 1;

//printf("\nread  = %s", b);

  l = strlen(b);
  if (l == 0 || b[0] == '*') return 0;;

  for (i=0, j=0; i<l; i++) {
    char c = b[i];
    if (c != '\n' && c != '\r' && c != ' ' && c != '\t') {
      b[j++] = c;
    }
  }

  b[j] = '\0';

//printf("clean = %s\n", b);
  l = strlen(b);

  if (l == 0) return 0;;

  if (eof) {
    printf("bad EOF\n");
    return -1;
  }
  
  if (l < 11 || l % 2 != 1) {
    printf("bad length 1\n");
    return -2;
  }

  if (b[0] != ':') {
    printf("bad mark\n");
    return -3;
  }

  for (i=1; i<l; i++) {
    if (!isxdigit(b[i])) {
      printf("non hex\n");
      return -4;
    }
  }
    
  for (i=1, j=1; i<l; j++) {
    int8 c, d1, d2;
    c = toupper(b[i]);
    d1 = c > '9' ? 10 + c - 'A' : c - '0';
    i = i + 1;
    c = toupper(b[i]);
    d2 = c > '9' ? 10 + c - 'A' : c - '0';
    i = i + 1;
    b[j] = 16 * d1 + d2;
  }
    
  record.length = b[1];
  if ((1 + 2 + 4 + 2 + record.length * 2 + 2) != l) {
    printf("bad length 2\n");
    return -5;
  }
    
  record.offset = 256 * b[2] + b[3];
    
  record.type = b[4];
  if (record.type > 0x05) {
    printf("bad type\n");
    return -6;
  }
  if (record.type == 0x01) eof = 1;

  for (i=0, j=5; i<record.length; i++, j++) {
    record.data[i] = b[j];
  }

  record.chksum = b[j];

  chksum = 0;
  for (i=1; i<record.length+6; i++) {
    chksum = chksum + b[i];
  }
  if (chksum) {
    printf("bad chksum\n");
    return -7;
  }
    
#if 0
printf("record.length = %02X\n",record.length);
printf("record.offset = %04X\n",record.offset);
printf("record.type   = %02X\n",record.type);
#endif

#if 0
printf("record: offset = %04X ",record.offset);
printf("data = ");
for(i=0;i<record.length;i++) printf("%02X",record.data[i]);
printf("   ");
for(i=0;i<record.length;i++) {
int8 c = record.data[i];
printf("%c", c < 0x20 || c >= 0x80 ? '.' : c);
}
printf("\n");
#endif

#if 0
  if (record.offset >= 0x0010 && record.offset < 0x0060) {
    for(i=0; i<record.length; i++) {
      int8 c = record.data[i];
      printf("%c", c < 0x20 || c >= 0x80 ? '.' : c);
    }
  }
  if (record.offset == 0x0060) printf("\n");
#endif

#if 0
printf("record.chksum = %02X\n",record.chksum);
printf("my chksum     = %02X (must be 00)\n",chksum);
#endif

  if (record.type != old_type) {
//  printf("\r%02X                        \n", record.type);
    old_type = record.type;
  }
  
  switch (record.type) {
    case 0x00:
      address = LBA | (int32)record.offset;
      if (first) {
        address_for_validate = address;
        first = 0;
      }
      record.address[0] = unpack32(address, 0x00FF0000);
      record.address[1] = unpack32(address, 0x0000FF00);
      record.address[2] = unpack32(address, 0x000000FF);
      if (load) {
//      write_flash_memory(&record, &err);
//      if (err) {
//        printf("write_flash_memory failed. Error code = %04X\n", err);
//        return -8;
//      }
      }
      
      memcpy(&memory[address], &record.data[0], record.length);

#if 0
printf("record: address = %06X ", address);
printf("data = ");
for(i=0;i<record.length;i++) printf("%02X",record.data[i]);
printf("   ");
for(i=0;i<record.length;i++) {
int8 c = record.data[i];
printf("%c", c < 0x20 || c >= 0x80 ? '.' : c);
}
printf("\n");
#endif

      break;
    case 0x01:
      break;
    case 0x02:
      break;
    case 0x03:
      break;
    case 0x04:
      LBA = ((int32)record.data[0] << 8 | (int32)record.data[1]) << 16;
//printf("LBA = %08X                        \n", LBA);
      break;
    case 0x05:
      break;
    default:
      break;
  }
  
  return 0;
}

//~============================================================================

int main(int argc, char *argv[]) {

  char b[512];
  char *filename;
  int n;
  int status, error;
 
  setbuf(stdout, NULL);

  printf("Usage: %s <filename> \n", argv[0]);
  if (argc < 2) {
    printf("<filename> is mandatory\n");
    exit(1);
  }
  
  memset(memory, 0x00, 0x40000);
  
  filename = argv[1];

  file = fopen(filename, "rt");
  if (errno) {
    perror("");
    printf("File '%s' not opened\n", filename);
    exit(1);
  }

  printf("File '%s' opened\n", filename);

  error = FALSE;

  if (!error) {
    n = 0;
    eof = 0;
    while (fgets(b, 512, file) != NULL) {
      status = parse_and_load_line(b, 0);
      if (status) {
        printf("parse_and_load_line error %d\n", status);
        error = TRUE;
        break;
      }
      n++;
    }
  }

//printf("Address for validate = %X\n", address_for_validate);

#if 0
{
  int32 i, j;
  for (j=0; j<0x40000-8; j++) {
    if (!memcmp(&memory[j], "USCM3.3", 7)) {
      printf("Found at %06X\n", j);
      printf("%s\n", &memory[j]);
      for (i=j; i<j+100; i++) {
        int8 c = memory[i];
        printf("%c", c < 0x20 || c >= 0x80 ? '.' : c);
      }
      printf("\n");
      for (i=j; i<j+100; i++) {
        int8 c = memory[i] >> 4;
        printf("%X", c);
      }
      printf("\n");
      for (i=j; i<j+100; i++) {
        int8 c = memory[i] & 0x0F;
        printf("%X", c);
      }
      printf("\n");
    }
  }
}
#else
{
  int32 j;
  for (j=0; j<0x40000-8; j++) if (!memcmp(&memory[j], "USCM3.3", 7)) printf("%s\n", &memory[j]);
  for (j=0; j<0x40000-8; j++) if (!memcmp(&memory[j], "PDSC1.0", 7)) printf("%s\n", &memory[j]);
}
#endif

  return 0;
}

//~============================================================================
