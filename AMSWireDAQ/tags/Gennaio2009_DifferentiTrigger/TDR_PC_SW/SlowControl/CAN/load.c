// file load.c
//
// to load new software into USCM FLASH memory
//
// A.Lebedev Oct-2003...

#define REAL 1             // 0 - debug, 1 - real work
#define DUMP 0             // to dump the flash memory load into file

#include "uscmlib.h"

int JMDC_ID;
int USCM_ID;
int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

int P;

//~============================================================================

int32 begin = 0;
int32 end;
int16 CKS;

FILE *file;
int eof;
int8 *mem;
bool is_HBB;

//~----------------------------------------------------------------------------

int parse_and_load_line(int8 *b, bool load) {

  int i, j, l;
  static int8 old_type = 0xFF;
  static int32 LBA = 0x00000000;
  int32 address;
#if REAL
  int16 err;
#endif
  static bool first = TRUE;

  int16 offset;
  int8  type;
  int16 length;
  int8  data[255];
  int8  chksum;
  int8  mychksum;

//printf("\nread  = %s", b);

  l = strlen((char*)b);
  if (l == 0 || b[0] == '*') return 0;;

  for (i=0, j=0; i<l; i++) {
    char c = b[i];
    if (c != '\n' && c != '\r' && c != ' ' && c != '\t') {
      b[j++] = c;
    }
  }

  b[j] = '\0';

//printf("clean = %s\n", b);
  l = strlen((char*)b);

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
    
  length = b[1];
  if ((1 + 2 + 4 + 2 + length * 2 + 2) != l) {
    printf("bad length 2\n");
    return -5;
  }
    
  offset = 256 * b[2] + b[3];
    
  type = b[4];
  if (type > 0x05) {
    printf("bad type\n");
    return -6;
  }
  if (type == 0x01) eof = 1;

  for (i=0, j=5; i<length; i++, j++) {
    data[i] = b[j];
  }

  chksum = b[j];
  for (mychksum=0, i=1; i<length+6; i++) mychksum += b[i];
  if (mychksum) {
    printf("bad chksum\n");
    return -7;
  }
    
#if 0
printf("length = %02X\n", length);
printf("offset = %04X\n", offset);
printf("type   = %02X\n", type);
printf("data   = ");
for(i=0;i<length;i++) printf("%02X", data[i]);
printf("\n");
printf("chksum = %02X,   ", chksum);
printf("my chksum = %02X (must be 00)\n", mychksum);
#endif

  if (type != old_type) {
//  printf("\r%02X                        \n", type);
    old_type = type;
  }
  
  switch (type) {
    case 0x00:
      address = LBA | (int32)offset;
      if (first) {
        begin = address;
//printf("set begin = %X\n", begin);
        first = 0;
      }
      if (load) {
#if REAL
        write_USCM_memory(length, address, data, &err);
//printf("write_USCM_memory(length=%d, address=%06X\n",
//                          length,    address);
        if (err) {
          printf("write_flash_memory failed. Error code = %04X\n", err);
          return -8;
        }
#endif
        memcpy(&mem[address-begin], data, length);
      }
      break;
    case 0x01:
      break;
    case 0x02:
      break;
    case 0x03:
      break;
    case 0x04:
      LBA = ((int32)data[0] << 8 | (int32)data[1]) << 16;
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

  FILE *file;
  int8 b[512];
  char *filename;
  int i, n, nmax;
  int status, error;
  int32 offset = 0x20000;
#if REAL
  int16 err;
  int8 done;
  int32 bgn;
#endif
  int from = -1, to = -1;

  P = 0;
  USCM_ID = 0x00;
  JMDC_ID = 0x03;
  
  setbuf(stdout, NULL);
  setup_command_path();

  if (!initialize_EPP()) exit(1);
  
  printf("Usage: %s <filename> <USCM_ID> [<JMDC_ID> [<Printout Level>]]\n", argv[0]);
  if (argc < 3) {
    printf("<filename> and <USCM_ID> are mandatory\n");
    exit(1);
  }
  filename = argv[1];
  USCM_ID = strtoul(argv[2], NULL, 16);
  if (argc > 3) JMDC_ID = strtoul(argv[3], NULL, 16);
  if (argc > 4) P = atoi(argv[4]);

  printf("File    = %s\n",   filename);
  printf("USCM ID = 0x%X\n", USCM_ID);
  printf("JMDC ID = %d\n",   JMDC_ID);
  printf("P       = %d\n",   P);
  
  mem = malloc(0x20000);
  for (i=0; i<0x20000; i++) mem[i] = 0xFF;
  
  file = fopen(filename, "rt");
  if (errno) {
    perror("");
    printf("File '%s' not opened\n", filename);
    exit(1);
  }

  printf("File '%s' opened\n", filename);

  error = FALSE;

  if (!error) {
    printf("File checking...\n");
    n = 0;
    eof = 0;
    while (fgets((char*)b, 512, file) != NULL) {
      status = parse_and_load_line(b, 0);
      if (status) {
        printf("parse_and_load_line error %d\n", status);
        error = TRUE;
        break;
      }
      n++;
    }
    nmax = n;
    fclose(file);
    file = fopen(filename, "rt");
    if (errno) {
      perror("");
      printf("File '%s' not opened\n", filename);
      error = TRUE;
    }
  }

  n = 0;

#if REAL
  if (!error) {
    if (!initialize_EPP()) {
      printf("Failed to initialize the EPPCAN box\n");
      error = TRUE;
    }
  } 
#endif

  if (!error) {
    if (begin == 0x20000) {
      from = 1;
      to   = 6;
    }
    else if (begin == 0x38000) {
      from = 7;
      to   = 7;
    }
    else if (begin == 0x3FFFE) {
      from = 8;
      to   = 8;
    }
    else {
      printf("Wrong begin address: %X\n", begin);
      error = TRUE;
    }
  }

#if REAL
  if (!error) {
    printf("Waiting for reboot...");
    boot_from_OTP(&err);
    for (i=0; i<3; i++) {
      printf(".");
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) {
        break;
      }
    }
    if (err) {
      printf("Failed (may be OTP software is out of date)\n");
//    error = TRUE;       //   allow for very old OTP software
    }
    else {
      printf("Done\n");
    }
  }

  if (!error) {
    int8 dat[1024];
    get_short_status(dat, &err);
    if (err) {
      printf("get short status failed\n");
      error = TRUE;
    }
    else {
      is_HBB = (dat[0] > 1) ? FALSE : TRUE;
      if (is_HBB) printf("HBB software detected\n");
      else        printf("SXu software detected\n");
    }
  }

  if (!error) {
    int j;
    for (j=from; j<=to; j++) {
      start_FLASH_erasure(j, &err);
      if (err) {
        printf("erase FLASH sector %d failed 1\n", j);
        error = TRUE;
        break;
      }
      printf("Waiting for erase sector %d ...", j);
      for (i=50; i>0; i--) {
        usleep(100000);
        check_FLASH_erasure(&done, &err);
        if (err) {
          printf("\nquery FLASH sector %d status failed\n", j);
          error = TRUE;
        }
        if (done) break;
        printf(".");
      }
      if (!done) {
        printf("\nerase FLASH failed 2");
        error = TRUE;
      }
      printf("\n");
      if (error) break;
    }
  }
#endif

  if (!error) {
    printf("Start loading...\n");
    eof = 0;
    while (fgets((char*)b, 512, file) != NULL) {
      status = parse_and_load_line(b, 1);
      if (status) {
        printf("parse_and_load_line error %d\n", status);
        error = TRUE;
        break;
      }
      n++;
      if (!P && !(n % 10)) printf("\rline %4d/%d", n, nmax);
    }
  }

  fclose(file);
  printf("\r%d line(s) read\n", n);
  printf(" File '%s' closed\n", filename);

  packbytes(&end, &mem[begin+9-offset], 3);
  printf("end   = %06X\n", end);
  
  if (!error) {
    for (CKS=0, i=begin; i<=end; i++) CKS += (int16)mem[i-offset];
    printf("CKS = %04X\n", CKS);
    mem[end+1-offset] = CKS >> 8;
    mem[end+2-offset] = CKS >> 0;
  }

#if DUMP
  file = fopen("memory2", "w");
  if (file) {
    fwrite(mem, 0x20000, 1, file);
    fclose(file);
    printf("File writing OK\n");
  }
#endif

#if REAL
  if (!error) {
//P = 3;
    write_USCM_memory(2, end+1, &mem[end+1-offset], &err);
    if (err) {
      printf("Checksum writing failed\n");
      error = TRUE;
    }
    else {
      printf("Checksum writing OK\n");
    }
  }

  if (!error) {
//  printf("begin address = %X\n", begin);
    if (!(begin & 0x00000FFF)) {
      int16 chk_1, chk_2;
      validate_FLASH(begin, &bgn, &end, &chk_1, &chk_2, &err);
      if (err) {
        printf("validate command failed, err = 0x%04X\n", err);
        error = TRUE;
      }
      else {
        if (is_HBB) printf(
          "validate: adr = %06X, bgn = %06X, end = %06X, chk_1 = %04X: O.K.\n", 
          begin, bgn, end, chk_1);
        else        printf(
          "validate: adr = %06X, bgn = %06X, end = %06X, chk_1 = %04X, chk_2 = %04X: %s\n", 
          begin, bgn, end, chk_1, chk_2, (chk_1 == chk_2) ? "O.K." : "FAILED");
      }
    }
  }
#endif

  if (error) printf("LOADING FAILED.\n");
  else       printf("LOADING SUCCEEDED.\n");

#if REAL
  if (!error) {
    printf("Waiting for final reboot...");
    boot_from_FLASH(&err);
    for (i=0; i<3; i++) {
      printf(".");
      ping_USCM(0, NULL, NULL, NULL, &err);
      if (!err) {
        break;
      }
    }
    if (err) {
      printf("Failed\n");
      error = TRUE;
    }
    else {
      printf("Done\n");
    }
  }
#endif

  return 0;
}

//~============================================================================
