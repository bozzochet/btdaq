//////////////////////////////////////////////////////////////////////////
///
///\file  TrLadPar.C
///\brief Source file of TrLadCal class
///
///\date  2008/06/19 AO  First version
///
//////////////////////////////////////////////////////////////////////////
#include "TrLadPar.h"
#include "TkDBc.h"

#include <cstdio>
#include <string>
#include <cstring> 
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

ClassImp(TrLadPar);

TrLadPar::TrLadPar(short int hwid) {
  _hwid = hwid;
  Clear();
}

TrLadPar::TrLadPar(const TrLadPar& orig):TObject(orig){
  _hwid = orig._hwid;
  for (int ii=0; ii< 2; ii++) _gain[ii]   = orig._gain[ii];
  for (int ii=0; ii<16; ii++) _vagain[ii] = orig._vagain[ii];
}

void TrLadPar::Clear(){
  for (int ii=0; ii< 2; ii++) _gain[ii]   = 1.;
  for (int ii=0; ii<16; ii++) _vagain[ii] = 1.;
}

void TrLadPar::PrintInfo(){
  TkLadder* lad = TkDBc::Head->FindHwId(_hwid);
  if(!lad) {
    printf("TrLadPar::PrintInfo() - Error - Can't find the ladder with HwId %4d\n",_hwid);
    return;
  }
  printf("%03d %+3d ",_hwid,lad->GetTkId());
  for (int ii=0; ii< 2; ii++) printf("%5.3f ",_gain[ii]);
  for (int ii=0; ii<16; ii++) printf("%5.3f ",_vagain[ii]);
  printf("\n");
}


