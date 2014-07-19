// file 6.c
//
// Low-level CAN-bus tester
//
// A.Lebedev Mar-2005...
//

#include "canlib.h"

int P = 3;

int JMDC_ID = 0x00;
int USCM_ID = 0x44;

//~----------------------------------------------------------------------------

int main(int argc, char* argv[]) {

#define N 12

  _CAN_msg *request = malloc(sizeof(*request));
  _CAN_msg *reply   = malloc(sizeof(*reply));

  int16 error;

  int i;
  int found;
  char *key[N][5] = {{"NXT",    "0001FF00", "0",  "0",  "0"},
                     {"ERR",    "2001FF00", "0",  "0",  "0"},
                     {"ABO",    "4001FF00", "0",  "0",  "0"},
                     {"END",    "6001FF00", "0",  "0",  "0"},
                     {"PING",   "6001FF00", "2", "01",  "0"},
                     {"DALLASd","6001FF00", "2", "02", "03"},
                     {"DALLASi","6001FF00", "2", "02", "04"},
                     {"ADC",    "6001FF00", "2", "02", "05"},
                     {"DACr",   "6001FF00", "2", "02", "06"},
                     {"DACw",   "6801FF00", "2", "42", "06"},
                     {"BOOT",   "6801FF00", "2", "41", "0B"},
                     {"POWERFF","6801FF00", "2", "41", "07"}};

  setbuf(stdout, NULL);

  found = 0;
  for (i=0; i<N; i++) {
    if (argc > 1 && strcasecmp(argv[1],key[i][0]) == 0) {
      found = 1;
      argc = 5;
      argv[1] = key[i][1];
      argv[2] = key[i][2];
      argv[3] = key[i][3];
      argv[4] = key[i][4];
    }
  }

  if (argc > 1) request->ID  = strtoul(argv[ 1], NULL, 16);

  if (argc > 2) request->len = strtoul(argv[ 2], NULL, 16);

  for (i=0; i<(argc-3)&&(i<8); i++) {
    request->dat[i] = strtoul(argv[i+3], NULL, 16);
  }

  if (found) pack32(&request->ID, 0x00FF0000, (int32)JMDC_ID);
  if (found) pack32(&request->ID, 0x0000FF00, (int32)USCM_ID);

  CAN_SND_RCV(INITIALIZE, request, reply, &error);

  CAN_SND_RCV(START, request, reply, &error);

  while (1) {
    int status;
    status = CAN_SND_RCV(CONTINUE, request, reply, &error);
    if (status == DONE) exit(0);
  }
  return 0;
}

//~============================================================================
