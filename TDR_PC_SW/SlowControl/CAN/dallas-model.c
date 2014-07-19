// file dallas-model.c
//
// Program to study DS1820 temperature calculation methods.
//
// A.Lebedev - Aug-2006...

#include "mylib.h"

//~----------------------------------------------------------------------------
//
// "Old" DS1820 temperature conversion (stolen from SXu and reworked)
// (for family code 0x10 only)
//
// was proven to be wrong after CERN climate chamber tests
//

void convert_1(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  short t1, t2;
  
  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb == 0x00) && (lsb == 0xAA)) return;  // power on
  if (perc == 0) return;
    
  if (msb) {
    if (lsb) {
      t2 = (128 * (lsb - 256)) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 < (short)0xFF80) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
  else {
    if (lsb == 0xFF) {
      *temp1 = 0x80;
      *temp2 = 0x7FFF;
    }
    else {
      t2 = (128 * lsb) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 > (short)0x007F) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
}

//~----------------------------------------------------------------------------
//
// "New" DS1820 temperature conversion (stolen from SXu and reworked)
// (for family code 0x10 only)
//
// was proven to be "correct" after CERN climate chamber tests
//

void convert_2(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  short t1, t2;
  
  *temp1 = 0x80;
  *temp2 = 0x8000;

  lsb = lsb & 0xFE;
  
  if ((msb == 0x00) && (lsb == 0xAA)) return;  // power on
  if (perc == 0) return;
    
  if (msb) {
    if (lsb) {
      t2 = (128 * (lsb - 256)) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 < (short)0xFF80) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
  else {
    if (lsb == 0xFF) {
      *temp1 = 0x80;
      *temp2 = 0x7FFF;
    }
    else {
      t2 = (128 * lsb) + (192 - 256 * remain / perc);
      t1 = ((t2 % 128) > 64) ? (t2 / 128 + 1) : (t2 / 128);
      *temp1 = (t1 > (short)0x007F) ? 0x80 : t1;
      *temp2 = t2;
    }
  }
}

//~----------------------------------------------------------------------------
//
// DS1820 temperature conversion from Dallas/Maxim documents (as is)
//

void convert_3(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb && lsb > 0x80) || (!msb && lsb <= 0x7F)) *temp1 = lsb;
  *temp2 = (int16)msb             << 15 | 
           (int16)(lsb & 0xFE)    <<  7 | 
           (int16)(perc - remain) <<  4;
}

//~----------------------------------------------------------------------------
//
// DS1820 temperature conversion from Dallas/Maxim documents (well, equivalent)
//

void convert_4(int8 lsb, int8 msb, int8 remain, int8 perc, 
               int8 *temp1, int16 *temp2) {

  *temp1 = 0x80;
  *temp2 = 0x8000;

  if ((msb && lsb > 0x80) || (!msb && lsb <= 0x7F)) *temp1 = lsb;
  *temp2 = (int16)msb                      << 15 | 
           (int16)lsb                      <<  7 | 
           (int16)((0x10 - remain) & 0x07) <<  4;
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i;
  int8 lsb, msb, remain, perc;
  int8  temp1_1, temp1_2, temp1_3, temp1_4;
  int16 temp2_1, temp2_2, temp2_3, temp2_4;
  float t1_1, t1_2, t1_3, t1_4;
  float t2_1, t2_2, t2_3, t2_4;
  float dt1, dt2, dt3, dt4;
  
  setbuf(stdout, NULL);
  if (argc != 4) {
    printf("Usage: %s MSB LSB REMAIN\n", argv[0]);
    exit(1);
  }

  msb    = strtoul(argv[1], NULL, 16);
  lsb    = strtoul(argv[2], NULL, 16);
  remain = strtoul(argv[3], NULL, 16);
  perc   = 0x10;
  
  for (i=0; i<0x1000; i++) {
//for (i=0; i<0x002F; i++) {
    msb    = (i & 0x0800) ? 0x00 : 0xFF;
    lsb    = (i & 0x07F8) >> 3;
    remain = (i & 0x000F);
    remain = 16 - remain;
    convert_1(lsb, msb, remain, perc, &temp1_1, &temp2_1);
    convert_2(lsb, msb, remain, perc, &temp1_2, &temp2_2);
    convert_3(lsb, msb, remain, perc, &temp1_3, &temp2_3);
    convert_4(lsb, msb, remain, perc, &temp1_4, &temp2_4);
    t1_1 = (float)(char)temp1_1 / 2.0;
    t1_2 = (float)(char)temp1_2 / 2.0;
    t1_3 = (float)(char)temp1_3 / 2.0;
    t1_4 = (float)(char)temp1_4 / 2.0;
    t2_1 = (float)(short)temp2_1 / 256.0;
    t2_2 = (float)(short)temp2_2 / 256.0;
    t2_3 = (float)(short)temp2_3 / 256.0 - 0.25;
    t2_4 = (float)(short)temp2_4 / 256.0 - 0.25;
    dt1 = t1_1 - t2_1;
    dt2 = t1_2 - t2_2;
    dt3 = t1_3 - t2_3;
    dt4 = t1_4 - t2_4;

    if (t2_3 != t2_4) printf("%c", '\7');
    
    printf("%02hhX %02hhX %02hhX ", msb, lsb, remain);
    if (temp1_1 != 0x80  ) printf("%+6.1f ", t1_1);
    else                   printf("       ");
    if (temp2_1 != 0x8000) printf("%+7.2f ", t2_1);
    else                   printf("        ");
    if (temp1_2 != 0x80  ) printf("%+6.1f ", t1_2);
    else                   printf("       ");
    if (temp2_2 != 0x8000) printf("%+7.2f ", t2_2);
    else                   printf("        ");
    if (temp1_2 != 0x80 && temp2_2 != 0x8000) printf("%+5.2f ", dt2);
    else                                      printf("      ");
    if (temp1_3 != 0x80  ) printf("%+6.1f ", t1_3);
    else                   printf("       ");
    if (temp2_3 != 0x8000) printf("%+7.2f ", t2_3);
    else                   printf("        ");
    if (temp1_3 != 0x80 && temp2_3 != 0x8000) printf("%+5.2f ", dt3);
    else                                      printf("      ");

#if 0
    printf("%04X    ", temp2_3);
    printf("%04hX    ", (short)temp2_3);
    printf("%4d    ", (short)temp2_3);
    printf("%f    ", (float)(short)temp2_3);
    printf("%f    ", (float)(short)temp2_3/256.0);
    printf("%.2f    ", (float)(short)temp2_3/256.0);
#endif


    printf("%s ", (t2_1 == t2_2) ? "  " : "12");
    printf("%s ", (t2_1 == t2_3) ? "  " : "13");
    printf("%s ", (t2_2 == t2_3) ? "  " : "23");
    printf("\n");
  }

  return 0;
}

//~============================================================================
