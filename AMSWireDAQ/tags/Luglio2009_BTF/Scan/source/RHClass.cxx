#include "RHClass.hh"
#include <stdlib.h>

ClassImp(RHClass);

RHClass::RHClass(){
  Run=0;
  ntdrRaw=0;
  ntdrCmp=0;
  nJinf=0;
  sprintf(date," ");
  memset(JinfMap,-1,NJINF*sizeof(JinfMap[0]));
  memset(tdrRawMap,-1,NTDRS*sizeof(tdrRawMap[0]));
  memset(tdrCmpMap,-1,NTDRS*sizeof(tdrCmpMap[0]));

  for (int ii=0;ii<NTDRS;ii++)
    for (int jj=0;jj<16;jj++){
      CNMean[ii][jj]=0.;
      CNSigma[ii][jj]=0.;
    }

}

int RHClass:: FindPos(int tdrnum) {

  for (int ii=0;ii<ntdrCmp;ii++)
    if(tdrCmpMap[ii]==tdrnum)  return ii;
  printf("RunHeader::FindPos:  Fatal Error can't find position for TDR %d\n",tdrnum);
  exit(4);

}

void RHClass::Print(){
  printf("---------------------------------------------\n");
  printf(" The header says: \n");
  printf("Run: %d Date: %s   \n",Run,date);
  printf("# Jinf = %d\n",nJinf);
  for (int ii=0;ii<nJinf;ii++)
    printf("Jinf Map pos: %d Jinf num: %d\n",ii,JinfMap[ii]);

  printf("# TDR RAW = %d\n",ntdrRaw);
  for (int ii=0;ii<ntdrRaw;ii++)
    printf("TDR RAW Map pos: %d tdrnum: %d\n",ii,tdrRawMap[ii]);

  printf("# TDR CMP = %d\n",ntdrCmp);
  for (int ii=0;ii<ntdrCmp;ii++)
    printf("TDR CMP Map pos: %d tdrnum: %d\n",ii,tdrCmpMap[ii]);
//   for (int ii=0;ii<NTDRS;ii++){
//     printf("TDR: %d\n",ii);
//       printf(" %6.2f ",CNMean[ii][jj]);
//     printf(" \n");
//     for (int jj=0;jj<16;jj++)
//       printf(" %6.2f ",CNSigma[ii][jj]);
//     printf(" \n");
//     printf(" \n");
//   }
  return;
}




