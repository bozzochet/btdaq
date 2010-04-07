//  $Id: TrParDB.C,v 1.1 2008/06/23 14:07:53 oliva Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrParDB.C
///\brief Source file of TrParDB class
///
///\date  2008/06/19 AO  First version
///$Date: 2008/06/23 14:07:53 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrParDB.h"

#include "TFile.h"

ClassImp(TrParDB);

TrParDB* TrParDB::Head=0;


//--------------------------------------------------
void TrParDB::init(){
  if (Head==0) {
    Head = this;
    for(int oct=0; oct<8; oct++){
      int crate = TkDBc::Head->_octant_crate[oct];
      for (int tdr=0; tdr<24; tdr++) {
	int hwid = crate*100+tdr;
	trpar_hwidmap[hwid] = new TrLadPar(hwid);
      }
    }
  } 
  else{ 
    printf("Only one instance of TrParDB allowed.\n");
    printf("Use TrParDB:Head->Load() to load a new parameters set.\n");
  }
}

//--------------------------------------------------
TrParDB::TrParDB(char * filename){
  if (filename) Load(filename);
}

//--------------------------------------------------
TrParDB::~TrParDB() {
  for (trparIT pp=trpar_hwidmap.begin(); pp!=trpar_hwidmap.end(); ++pp)
    if(pp->second) delete pp->second;
}

//--------------------------------------------------
TrLadPar* TrParDB::FindPar_TkAssemblyId(int tkassemblyid) {
  TkLadder* ll = TkDBc::Head->FindTkAssemblyId(tkassemblyid);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else return 0;
}


//--------------------------------------------------
TrLadPar* TrParDB::FindPar_HwId(int hwid){ 
  trparIT aa = trpar_hwidmap.find(hwid);
  if (aa!=trpar_hwidmap.end()) return aa->second; 
  else                         return 0;
}

//--------------------------------------------------
TrLadPar* TrParDB::FindPar_TkId(int tkid){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else    return 0;
}

//--------------------------------------------------
TrLadPar* TrParDB::FindPar_LadName(string& name){
  TkLadder* ll=TkDBc::Head->FindLadName(name);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else    return 0;
}

//--------------------------------------------------
TrLadPar* TrParDB::FindPar_JMDC(int JMDCid){
  int crate = (JMDCid-282)/24;
  int tdr   = JMDCid-crate*24-282;
  int hwid  = crate*100+tdr;
  printf(" crate %d tdr %d hwid %d \n",crate,tdr,hwid);
  return FindPar_HwId(hwid);
}
 
//--------------------------------------------------
TrLadPar* TrParDB::GetEntry(int ii){
  int count=0;
  for(trparIT pp=trpar_hwidmap.begin();pp!=trpar_hwidmap.end();pp++){
    if(count==ii) return &(*pp->second);
    count++;
  }
  return 0;
}


//--------------------------------------------------
void TrParDB::Load(char * filename){
  TFile f(filename);
  Head = (TrParDB*) f.Get("TrParDB");
  f.Close();
  return;
}

//--------------------------------------------------
