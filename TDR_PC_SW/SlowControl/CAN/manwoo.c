// file manwoo.c
//
// Calculate some things for CERNOX sensors
//
// A.Lebedev Feb-2007...

#include "mylib.h"

//        1234567890123456
real A4 = 726613416.515625;
real A3 = -25073071.935692;
real A2 =    374105.066020;
real A1 =     15385.405567;
real A0 =        -4.908300;

real R, RR, T;

typedef struct {
  real T;
  real R;
} _T;

_T Table[39] = {
  { 41.978, 347.601},
  { 44.973, 327.373},
  { 47.977, 309.406},
  { 49.971, 298.468},
  { 54.972, 274.593},
  { 59.980, 254.252},
  { 64.984, 236.917},
  { 69.987, 221.887},
  { 74.991, 208.667},
  { 79.988, 197.025},
  { 84.990, 186.649},
  { 89.990, 177.343},
  { 94.993, 168.915},
  {100.000, 161.266},
  {110.095, 147.839},
  {120.003, 136.682},
  {130.011, 127.031},
  {140.017, 118.675},
  {150.025, 111.346},
  {160.037, 104.903},
  {170.058,  99.167},
  {180.054,  94.065},
  {190.057,  89.460},
  {200.073,  85.333},
  {210.090,  81.577},
  {220.098,  78.163},
  {230.099,  75.051},
  {240.102,  72.197},
  {250.087,  69.577},
  {260.085,  67.167},
  {270.099,  64.936},
  {280.114,  62.867},
  {290.139,  60.950},
  {300.164,  59.166},
  {310.186,  57.501},
  {315.189,  56.716},
  {320.169,  55.959},
  {326.190,  55.076},
  {330.164,  54.513}};

//~----------------------------------------------------------------------------

real lint(real x, int i) {

  real x1 = 1.0 / Table[i-1].R;
  real x2 = 1.0 / Table[i+1].R;
  real y1 =       Table[i-1].T;
  real y2 =       Table[i+1].T;
  
  return (y1 + (y2 - y1) / (x2 - x1) * (x - x1));
}

//~----------------------------------------------------------------------------

real qint1(real x, int i) {

  real x1, x2, x3, y1, y2, y3, A2, A1, A0, D;

  x1 = 1.0 / Table[i-1].R;
  x2 = 1.0 / Table[i+1].R;
  x3 = 1.0 / Table[i+2].R;
  y1 =       Table[i-1].T;
  y2 =       Table[i+1].T;
  y3 =       Table[i+2].T;
  
  D  =   (x2 - x1) * (x3 - x1) * (x2 - x3);  
  A2 =  ((y2 - y1) * (x3 - x1)             - (y3 - y1) * (x2 - x1))              / D;
  A1 = -((y2 - y1) * (x3 - x1) * (x3 + x1) - (y3 - y1) * (x2 - x1) * ( x2 + x1)) / D;
  A0 = y1 - (A2 * x1 + A1) * x1;
  
  return ((A2 * x + A1) * x + A0);
}

//~----------------------------------------------------------------------------

real qint2(real x, int i) {

  real x1, x2, x3, y1, y2, y3, A2, A1, A0, D;

  x1 = 1.0 / Table[i-2].R;
  x2 = 1.0 / Table[i-1].R;
  x3 = 1.0 / Table[i+1].R;
  y1 =       Table[i-2].T;
  y2 =       Table[i-1].T;
  y3 =       Table[i+1].T;
  
  D  =   (x2 - x1) * (x3 - x1) * (x2 - x3);  
  A2 =  ((y2 - y1) * (x3 - x1)             - (y3 - y1) * (x2 - x1))              / D;
  A1 = -((y2 - y1) * (x3 - x1) * (x3 + x1) - (y3 - y1) * (x2 - x1) * ( x2 + x1)) / D;
  A0 = y1 - (A2 * x1 + A1) * x1;
  
  return ((A2 * x + A1) * x + A0);
}

//~----------------------------------------------------------------------------

int main(int argc, char *argv[]) {

  int i, n;
  real lin, quad1, quad2;
  real err1, err2, err3, err4;
  real rms1, rms2, rms3, rms4;
  int    n1,   n2,   n3,   n4;

  printf("A4 = %16Lf\n", A4);
  printf("A3 = %16Lf\n", A3);
  printf("A2 = %16Lf\n", A2);
  printf("A1 = %16Lf\n", A1);
  printf("A0 = %16Lf\n", A0);

  R = 347.601;
  RR = 1.0 / R;
  
  printf("R  = %16Lf\n", R);
  printf("RR = %16Lf\n", RR);

  T = A4 * powl(RR,4) + A3 * powl(RR,3) + A2 * powl(RR,2) + A1 * powl(RR,1) + A0;
  
  printf("T  = %16Lf\n", T);
  
  n = sizeof(Table) / sizeof(Table[0]);
  printf("sizeof(Table) = %d, sizeof(Table[0]) = %d, n = %d\n",
          sizeof(Table),      sizeof(Table[0]),      n);

  rms1 = rms2 = rms3 = rms4 = 0.0;
  n1   = n2   = n3   = n4   = 0;

  printf("-- R -- --1/R -- -- T -- ------- Manwoo ------- ");
  printf("------ Linear -------- ---- Quadratic 1 ----- ---- Quadratic 2 ----- \n");

  for (i=0; i<n; i++) {
    err1 = err2 = err3 = err4 = 0.0;
    R  = Table[i].R;
    RR = 1.0 / R;
    T = A4 * powl(RR,4) + A3 * powl(RR,3) + A2 * powl(RR,2) + A1 * powl(RR,1) + A0;
    err1 = T - Table[i].T;
    rms1 = rms1 + err1 * err1;
    n1++;
    printf("%7.3Lf %8Lf %7.3Lf %10Lf (%+8Lf) ", R, RR, Table[i].T, T, err1);
    if ((i > 0) && (i < (n-1))) {
      lin = lint(1.0 / Table[i].R, i);
      err2 = lin - Table[i].T;
      rms2 = rms2 + err2 * err2;
      n2++;
      printf("%10Lf (%+8Lf) ", lin, err2);
    }
    else {
      printf("                       ");
    }
    if ((i > 0) && (i < (n-2))) {
      quad1 = qint1(1.0 / Table[i].R, i);
      err3  = quad1 - Table[i].T;
      rms3 = rms3 + err3 * err3;
      n3++;
      printf("%10Lf (%+8Lf) ", quad1, err3);
    }
    else {
      printf("                       ");
    }
    if ((i > 1) && (i < (n-1))) {
      quad2 = qint2(1.0 / Table[i].R, i);
      err4  = quad2 - Table[i].T;
      rms4 = rms4 + err4 * err4;
      n4++;
      printf("%10Lf (%+8Lf) ", quad2, err4);
    }
    else {
      printf("                       ");
    }
    printf("\n");
  }

  rms1 = sqrtl(rms1/(real)n1);
  rms2 = sqrtl(rms2/(real)n2);
  rms3 = sqrtl(rms3/(real)n3);
  rms4 = sqrtl(rms4/(real)n4);
  
  printf("                                ");
  printf("rms = %7Lf", rms1);
  printf("         rms = %7Lf", rms2);
  printf("         rms = %7Lf", rms3);
  printf("         rms = %7Lf", rms4);
  printf("\n");

  return 0;
}

//~============================================================================
