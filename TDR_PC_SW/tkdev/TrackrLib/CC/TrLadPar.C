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


//--------------------------------------------------
TrLadPar::TrLadPar(short int hwid) {
  _hwid = hwid;
  Clear();
}


//--------------------------------------------------
TrLadPar::TrLadPar(const TrLadPar& orig):TObject(orig){
  _hwid = orig._hwid;
  _gain = orig._gain;
  for (int ii=0; ii<16; ii++) _VAgain[ii] = orig._VAgain[ii];
  for (int ii=0; ii<4;  ii++) _langaupar_muons[ii] = orig._langaupar_muons[ii];
}


//--------------------------------------------------
void TrLadPar::Clear(){
  _filled=0;

  // defaults
  _gain = 1.;
  for (int ii=0; ii<16; ii++) _VAgain[ii] = 1.;
  for (int ii=0; ii<4;  ii++) _langaupar_muons[ii] = 1.;
}


//--------------------------------------------------
void TrLadPar::PrintInfo(int long_format){
  TkLadder* lad = TkDBc::Head->FindHwId(_hwid);
  if(!lad) {
    printf("TrLadPar::PrintInfo() - Error - Can't find the ladder with HwId %4d\n",_hwid);
    return;
  }
  printf("Ladder Name: %s  Filled: %d\n",lad->name,_filled);
  printf("Layer: %d Slot: %2d  Side: %d \n", 
	 lad->GetLayer(),lad->GetSlot(),lad->GetSide()); 
  printf("Octant: %d %s Crate: %d TDR:  %2d  PwGroup: %d PwgrupPos: %d\n",
	 lad->GetOctant(),TkDBc::Head->GetOctName(lad->GetOctant()),
	 lad->GetCrate(),lad->GetTdr(),
	 lad->GetPwGroup(),lad->GetPwgroupPos());
  if(long_format){
  }
}


