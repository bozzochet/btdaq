#ifndef Calib_h
#define Calib_h
#include "TH1F.h"
#include "RHClass.hh"
#include <cstdio>

#define TDRNUM 24

typedef struct calib{
  float ped[1024];
  float rsig[1024];
  float sig[1024];
  int status[1024];
} calib;


class Calib {

public:
  int RunNum;
  char caldir[300];
  calib cals[TDRNUM];

  Calib(int run);
  int Calpos(int tdrnum, RHClass* rh);
  void RepererCalibs(RHClass* rh);
  int LireCalib(FILE* fich, calib* cal, int ntdr);
  float ret_piedestal(int ntdr, int canal) { 
    calib* cal = &(cals[ntdr]);
    return(cal->ped[canal]); }
  float ret_sigma(int ntdr, int canal) { 
    calib* cal = &(cals[ntdr]);
    return(cal->sig[canal]); }
  int ret_statut(int ntdr, int canal) { 
    calib* cal = &(cals[ntdr]);
    return(cal->status[canal]); }

};

#endif
