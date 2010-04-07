// file CRC-CCITT.c
//
// CRC-CCITT calculation (used in communications with CAB)
//
// A.Lebedev - Nov 2006...

#include "mylib.h"

//~----------------------------------------------------------------------------

int16 CRC_CCITT_1(int8 *p, int8 n) {

  int16 CRC = 0xFFFF;
    while (n--) {
    CRC  = (CRC >> 8) | (CRC << 8);
    CRC ^= *p++;
    CRC ^= (CRC & 0xFF) >> 4;
    CRC ^= (CRC << 8) << 4;
    CRC ^= ((CRC & 0xFF) << 4) << 1;
  }
  
  return CRC;
}

//~----------------------------------------------------------------------------

int16 CRC_CCITT_2(int8 *p, int8 n) {

  int16 CRC = 0xFFFF;
//int16 g   = 0x1021;
  int16 g   = 0x8408;
  
  while (n--) {
    int i;
    CRC ^= (int8)*p++;
    for (i=0; i<8; i++) CRC = (CRC >> 1) ^ ((CRC & 0x0001) ? g : 0);
  }

  return CRC;
}

//~----------------------------------------------------------------------------

int16 CRC_CCITT_3(int8 *p, int8 n) {

  int16 CRC = 0xFFFF;
  int16 g   = 0x1021;
//int16 g   = 0x8408;
  
  while (n--) {
    int i;
    CRC ^= (int16)((int8)*p++ << 8);
    for (i=0; i<8; i++) CRC = (CRC << 1) ^ ((CRC & 0x8000) ? g : 0);
  }

  return CRC;
}

//~----------------------------------------------------------------------------

int main(void) {

  int i;
  
//char string[] = "123456789";
//char string[] = "A";
//int8 string[] = {0x0A, 0x28, 0x04, 0x01,    0x00};
//int8 string[] = {0x0A, 0x03, 0x03,          0x00};
//int8 string[] = {0x0A, 0x03, 0x03};

//int8 string[] = {0x0A, 0x06, 0x08, 0xAD, 0x01, 0xF0, 0x00, 0x00};  // from Crisa CRC=0x92DD
  int8 string[] = {0x0A, 0x09, 0x07, 0xAD, 0x20, 0xF0, 0x00, 0x6B, 0x41};  // from Crisa CRC=0x6B41

  int n = sizeof(string);                     
//printf("string = \"%s\" : ", string);
  printf("string = ");
  for (i=0; i<n; i++) printf("%02X ", string[i]);
  printf("\n");
    printf("CRC_1 = %04X : ", CRC_CCITT_1(string, sizeof(string)));
  printf("CRC_2 = %04X : ", CRC_CCITT_2(string, sizeof(string)));
  printf("CRC_3 = %04X : ", CRC_CCITT_3(string, sizeof(string)));

  printf("\n");
  
  return 0;
}

//~============================================================================
