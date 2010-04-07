// file ttce-1.c.c
//
// this is VK's program k reworked by AL
//
// A.Lebedev - Dec 2007...
//

#include "uscmlib.h"

int JMDC_ID = 0x03;
int USCM_ID = 0x6C;
int P = 0;

int BUS_FOR_REQUEST = 0;                           // global variable
int BUS_FOR_REPLY = 0;                             // global variable

bool Debug = FALSE;

FILE *file;

//~============================================================================

void p(int cnt, int32 d)  {
  int i;
  for (i=cnt-1; i>=0; i--) printf("%1d", 1 & (d >> i));
}

//~----------------------------------------------------------------------------

void find_Dallas_sensors(void) {

  int i, t;
  int16 err;
  int8 b;
  int ntry = 5;
  int8 n1[9];
  int64 ID[9][64];

  start_DS1820_scan(0xf, &err);
  if (err) PANIC("find_Dallas_sensors FAILED\n");

  do {
    check_TTCE_DS1820_scan(&b, NULL, &err);
    if (err) break;
  } while (b != 0);
  if (err) PANIC("find_Dallas_sensors FAILED\n");
  
  setup_TTCE_DS1820_readout(0xf, &err);
  if (err) PANIC("find_Dallas_sensors FAILED\n");

  for (i=0; i<4; i++) {
    for (t=0; t<ntry; t++) {
      read_TTCE_DS1820_table(i, &n1[i], &ID[i][0], &err);
      if (!err) break;
    }
    if (err) PANIC("find_Dallas_sensors FAILED\n");
  }
  
  printf("Bus #                  ");
  for (i=0; i<4; i++) printf(" %2d",    3-i);
  printf("\n");
  printf("Nb of sensors to read: ");
  for (i=0; i<4; i++) printf(" %2d", n1[3-i]);
  printf("\n");
}

//~----------------------------------------------------------------------------

void execute_USCM_command(int16 *err) {

  int i, ii, iii;
  int64 ID[64], ID1[64];
//int8 nds, nds1, ctrl28V[5], ctrlLOOP[64], Pressure[8];
  int8 nds, nds1, ctrl28V[5], ctrlLOOP[64];
  int16 Pressure[4];
  bool ok[64];
  float temp[72];
  int16 adc[72];
  int8 RC[22], VR[22];
  float temp1[22];
  static int hh=0;
  unsigned int cc;
  struct timeval now, then;

//~---

  printf("\n");

  for (ii=0; ii<4; ii++) {
    read_TTCE_DS1820_table(ii, &nds, ID, err);
    if (!*err) {
      printf("%2d",nds);
      for (i=0; i<nds; i++) {
        if (!(i % 8)) printf("\n");
        printf("%016llX ",ID[i]);
      }
      printf("\n");
    }
    nds1 = 0;
    iii = 2000000000;
    while (nds1 == 0 && iii-- != 0) {
	    read_TTCE_DS1820_temp(ii, &nds1, ok, temp, err);    
      if (!*err) {
        if (nds1 != 0) { 
          printf("%2d",nds1);
          for (i=0; i<nds1; i++) {
            if (!(i % 8)) printf("\n");
            if (ok[i]) printf("          %6.2f ",temp[i]); 
            else       printf("          ...... ");
          }
          printf("\n");
        }
      }
    }
    for (i=0; i<nds; i++) {
      ID1[i] = ID[nds-1-i];
    }
    write_TTCE_DS1820_table(ii, nds, ID1, err);
  }

//~---

  read_TTCE_Pt1000_ADC(adc, err);

  if (!*err) {
    for (i=0; i<72; i++) {
      if ((adc[i] & 0xf000) != 0) {
        break;
      }
    }

    printf("\n");
    for (i=0; i<24; i++) {
      for (ii=0; ii<3; ii++) {
        temp[3*i+ii] = (adc[3*i+ii]-adc[ii])/16.0;
      }
    }

    printf("REF0     %4d  REF1     %4d  REF2     %4d\n", adc[0],  adc[1],  adc[2]);
    printf("Pt010P %6.2f  Pt011P %6.2f  Pt012P %6.2f\n",temp[3], temp[4], temp[5]);
    printf("Pt020P %6.2f  Pt021P %6.2f  Pt022P %6.2f\n",temp[6], temp[7], temp[8]);
    printf("Pt030P %6.2f  Pt031P %6.2f  Pt032P %6.2f\n",temp[9], temp[10],temp[11]);
    printf("Pt040P %6.2f  Pt041P %6.2f  Pt042P %6.2f\n",temp[12],temp[13],temp[14]);
    printf("Pt050P %6.2f  Pt051P %6.2f  Pt052P %6.2f\n",temp[15],temp[16],temp[17]);
    printf("Pt06_P %6.2f  Pt07_P %6.2f  Pt08_P %6.2f\n",temp[18],temp[19],temp[20]);
    printf("Pt09_P %6.2f  Pt10_P %6.2f  Pt11_P %6.2f\n",temp[21],temp[22],temp[23]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[24],temp[25],temp[26]);
    printf("Pt010S %6.2f  Pt011S %6.2f  Pt012S %6.2f\n",temp[27],temp[28],temp[29]);
    printf("Pt020S %6.2f  Pt021S %6.2f  Pt022S %6.2f\n",temp[30],temp[31],temp[32]);
    printf("Pt030S %6.2f  Pt031S %6.2f  Pt032S %6.2f\n",temp[33],temp[34],temp[35]);
    printf("Pt040S %6.2f  Pt041S %6.2f  Pt042S %6.2f\n",temp[36],temp[37],temp[38]);
    printf("Pt050S %6.2f  Pt051S %6.2f  Pt052S %6.2f\n",temp[39],temp[40],temp[41]);
    printf("Pt06_S %6.2f  Pt07_S %6.2f  Pt08_S %6.2f\n",temp[42],temp[43],temp[44]);
    printf("Pt09_S %6.2f  Pt10_S %6.2f  Pt11_S %6.2f\n",temp[45],temp[46],temp[47]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[48],temp[49],temp[50]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[51],temp[52],temp[53]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[54],temp[55],temp[56]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[57],temp[58],temp[59]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[60],temp[61],temp[62]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[63],temp[64],temp[65]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[66],temp[67],temp[68]);
    printf("    NC %6.2f      NC %6.2f      NC %6.2f\n",temp[69],temp[70],temp[71]);
  }

//~---

  read_TTCE_Pt1000_temp(RC, VR, temp1, err);

  if (!*err) {
    for (i=0; i<11; i++) {
      printf(" T%02dP  ", i+1);
    }
    printf("\n");

    for (i=0; i<11; i++) {
      printf("%6.2f ", temp1[i]);
    }
    printf("\n");

    for (i=0; i<11; i++) {
      printf(" %1d %1d   ",VR[i], RC[i]);
    }
    printf("\n");
    printf("\n");      

    for (i=0; i<11; i++) {
      printf(" T%02dS  ", i+1);
    }
    printf("\n");

    for (i=11; i<22; i++) {
      printf("%6.2f ",temp1[i]);
    }
    printf("\n");

    for (i=11; i<22; i++) {
      printf(" %1d %1d   ",VR[i], RC[i]);
    }
    printf("\n");
  }

//~---

  check_TTCE_28VDC_power(ctrl28V, err);

  if (!*err) {
    for (i=0; i<5; i++) {
      p(8, ctrl28V[i]);
      printf("  ");
    }
    printf("\n");      
  }
    
//~---

  read_TTCE_pressure(Pressure, err);
  
  if (!*err) {
    for (i=0; i<4; i++) {
      printf("p%1d=%5.2f   ",
//           i, 2.5 * (16 * (int8)Pressure[2*i] + Pressure[2*i + 1]) / 4096);
             i, 2.5 * (float)Pressure[i] / 4096);
    }
    printf("\n");      
  }
  printf("\n");

//~---

  read_TTCE_loop_ctrl(ctrlLOOP, err);

  if (!*err) {
    printf("flight=%1d  ", 1 & (ctrlLOOP[0]>>2));
    printf("heat=%1d  ",   1 & (ctrlLOOP[0]>>3));
    printf("test=%1d  ",   1 & (ctrlLOOP[0]>>1));
    printf("enable=%1d\n", 1 & (ctrlLOOP[0]>>0));
    printf("setT=%4d\n", (signed char)ctrlLOOP[1]);
    printf("k1=%d\n",                 ctrlLOOP[2]);
    printf("k2=%d\n",                 ctrlLOOP[3]);
    printf("k3=%d\n",                 ctrlLOOP[4]);
    printf("iband=%4.2f\n",           ctrlLOOP[5]/16.0);
    printf("f_val=%d\n", 256 * (signed char)ctrlLOOP[6] + ctrlLOOP[7]);

    cc = ((unsigned int)ctrlLOOP[8] << 8) + ctrlLOOP[9];
    if (cc & 0x0800) cc = cc | 0xFFFFF000;
    printf("T_tst=%4.2f\n", (signed short)cc / 16.0);

    printf("p_val=%d\n", 2 * (256 * (signed char)ctrlLOOP[10] + ctrlLOOP[11]));

    printf("i_val=%d\n", 2 * (256 * (signed char)ctrlLOOP[12] + ctrlLOOP[13]));

    printf("vout=%3.1f V  (%03d)   ", 28 * ctrlLOOP[14]/255., ctrlLOOP[14]);
    if (1 & (ctrlLOOP[0]>>3)) 
      printf("P=%3.1f W", (28 * ctrlLOOP[14] / 255) * (28 * ctrlLOOP[14] / 255) / 35.1);
      
    printf("\n");
    printf("\n");
     
    printf("flight=%1d  ", 1 & (ctrlLOOP[32]>>2));
    printf("heat=%1d  ",   1 & (ctrlLOOP[32]>>3));
    printf("test=%1d  ",   1 & (ctrlLOOP[32]>>1));
    printf("enable=%1d\n", 1 & ctrlLOOP[32]);
    printf("setT=%4d\n", (signed char)ctrlLOOP[33]);
    printf("k1=%d\n",                 ctrlLOOP[34]);
    printf("k2=%d\n",                 ctrlLOOP[35]);
    printf("k3=%d\n",                 ctrlLOOP[36]);
    printf("iband=%4.2f\n",           ctrlLOOP[37] / 16.0);

    printf("f_val=%d\n", 256 * (signed char)ctrlLOOP[38] + ctrlLOOP[39]);

    cc = ((unsigned int)ctrlLOOP[40] << 8) + ctrlLOOP[41];
    if (cc & 0x0800) cc = cc | 0xFFFFF000;
    printf("T_tst=%4.2f\n", (signed short)cc / 16.0);

    printf("p_val=%d\n", 2 * (256 * (signed char)ctrlLOOP[42] + ctrlLOOP[43]));

    printf("i_val=%d\n", 2 * (256 * (signed char)ctrlLOOP[44] + ctrlLOOP[45]));

    printf("vout=%3.1f V  (%03d)   ", 28 * ctrlLOOP[46]/255., ctrlLOOP[46]);
    if (1 & (ctrlLOOP[32]>>3)) 
      printf("P=%3.1f W", (28 * ctrlLOOP[46] / 255) * (28 * ctrlLOOP[46] / 255) / 35.1);
      
    printf("\n");     
  }

//~---

  do gettimeofday(&now, NULL); while ((int)now.tv_sec < (int)then.tv_sec + 5);

  gettimeofday(&then, NULL); 

  hh++;
  if (hh > 3) {
    hh = 0;
    printf("%d ", (int)now.tv_sec);
    fprintf(file, "%d   ",(int)now.tv_sec);
    for (i=0; i<7; i++) {
      printf("       %6.2f ", temp1[i]);
      fprintf(file, "%6.2f ", temp1[i]);
    }
    printf(       "%6.2f ", temp1[8]);
    fprintf(file, "%6.2f ", temp1[8]);
    printf(       "%6.2f ",temp1[9]);
    fprintf(file, "%6.2f ",temp1[9]);
    printf(       "%1d ", 1 & (ctrlLOOP[0] >> 2));
    fprintf(file, "%1d ", 1 & (ctrlLOOP[0] >> 2));
    printf(       "%8d ", 2 * (256 * (signed char)ctrlLOOP[10] + ctrlLOOP[11]));
    fprintf(file, "%8d ", 2 * (256 * (signed char)ctrlLOOP[10] + ctrlLOOP[11]));
    printf(       "%8d ", 2 * (256 * (signed char)ctrlLOOP[12] + ctrlLOOP[13]));
    fprintf(file, "%8d ", 2 * (256 * (signed char)ctrlLOOP[12] + ctrlLOOP[13]));
    printf(       "%4d ", (unsigned char)ctrlLOOP[14]);
    fprintf(file, "%4d ", (unsigned char)ctrlLOOP[14]);
    printf(       "%02X %02X", ctrl28V[0], ctrl28V[4]); 
    fprintf(file, "%02X %02X", ctrl28V[0], ctrl28V[4]); 

//  printf(       "\n%s\n", ctime(&(now.tv_sec)));
//  fprintf(file, "%s\n",   ctime(&(now.tv_sec)));
    printf(       "\n");
    fprintf(file, "\n");
  }
}

//~============================================================================

int main(int argc, char *argv[]) {

  int16 err;

//---

  file = fopen("LOOP_01.dat","w");

  setbuf(stdout, NULL);
  setup_command_path();

  printf("Usage: %s [<USCM_ID> [<JMDC_ID> ]]\n",argv[0]);

  if (argc > 1) USCM_ID = strtoul(argv[1], NULL, 16);
  if (argc > 2) JMDC_ID = strtoul(argv[2], NULL, 16);

  printf("USCM_ID = 0x%03X\n", USCM_ID);
  printf("JMDC_ID = %d\n",   JMDC_ID);
  
  if (!initialize_EPP()) exit(1);

//---

  find_Dallas_sensors();

  while (1) {
    if (kbhit()) {
      int ch = toupper(fgetc(stdin));
      switch (ch) {
        case 'D': Debug = TRUE; printf("Fake error:\n"); break;
        case 'P': P = 3 - P; break;
        case 'Q': exit(0);
      }
    }
//  printf("\x01b[2J");
    execute_USCM_command(&err);
//  usleep(4000000);
  }
  
  return 0;
}

//~============================================================================
