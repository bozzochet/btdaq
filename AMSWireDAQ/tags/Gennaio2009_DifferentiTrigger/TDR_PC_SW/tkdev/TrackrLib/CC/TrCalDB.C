//  $Id: TrCalDB.C,v 1.10 2008/10/14 10:06:00 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrCalDB.C
///\brief Source file of TrCalDB class
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/20 SH  File name changed, some utils are added
///\date  2008/01/23 SH  Some comments are added
///$Date: 2008/10/14 10:06:00 $
///
///$Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrCalDB.h"
#include "TFile.h"

ClassImp(TrCalDB);

TrCalDB* TrCalDB::Head=0;

float* TrCalDB::linear=0;


void TrCalDB::init(){

  if(Head==0){
    Head=this;
    run=0;
    
    for(int oct=0;oct<8;oct++){
      int crate=TkDBc::Head-> _octant_crate[oct];
      for(int tdr=0;tdr<24;tdr++){
	int hwid=crate*100+tdr;
	trcal_hwidmap[hwid]=new TrLadCal(hwid);
      }
    }
  } else{ 
    printf("Only One instance of CalDB Allowed\n");
    printf("Use CalDB:Head->Load() to load a new calibration\n");
  }
}

TrCalDB::TrCalDB(char * filename){
    if(filename) Load(filename);
}



TrCalDB::~TrCalDB(){
  for ( trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();++pp)
    if(pp->second) delete pp->second;
  if(linear) delete [] linear;
}


void TrCalDB::Clear(){
  for ( trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();++pp)
    if(pp->second)  pp->second->Clear();
  if(linear) delete [] linear;
}





TrLadCal* TrCalDB::FindCal_TkAssemblyId( int tkassemblyid){

  TkLadder* ll=TkDBc::Head->FindTkAssemblyId(tkassemblyid);
  if(ll) return FindCal_HwId(ll->GetHwId());
  else return 0;
}


TrLadCal* TrCalDB::FindCal_HwId( int hwid){ 
  trcalIT aa=trcal_hwidmap.find(hwid);
  if(aa != trcal_hwidmap.end()) 
    return aa->second; 
  else 
    return 0;
}

TrLadCal* TrCalDB::FindCal_TkId( int tkid){

  TkLadder* ll=TkDBc::Head->FindTkId(tkid);
  if(ll) 
    return FindCal_HwId(ll->GetHwId());
  else 
    return 0;
}


TrLadCal* TrCalDB::FindCal_LadName( string& name){

  TkLadder* ll=TkDBc::Head->FindLadName(name);
  if(ll) 
    return FindCal_HwId(ll->GetHwId());
  else 
    return 0;
}




TrLadCal* TrCalDB::FindCal_JMDC( int JMDCid){
  int crate=(JMDCid-282)/24;
  int tdr=JMDCid-crate*24-282;
  int hwid=crate*100+tdr;
  printf(" crate %d tdr %d hwid %d \n",crate, tdr, hwid);
  return FindCal_HwId( hwid);
}
 
TrLadCal* TrCalDB::GetEntry(int ii){
  int count=0;
  for(trcalIT pp=trcal_hwidmap.begin();pp!=trcal_hwidmap.end();pp++){
    if(count==ii) return &(*pp->second);
    count++;
  }
  return 0;
}



void TrCalDB::Load(char * filename){

  TFile* f=TFile::Open(filename);
  Head=(TrCalDB*) f->Get("TrCalDB");
  f->Close();
  return ;
}

void TrCalDB::CalDB2Lin(){

  if(! linear) {
    printf("TrCalDB::CalDB2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
  }
  linear[0]=(float) run;
  for (trcalIT aa=trcal_hwidmap.begin(); aa!=trcal_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=1+(crate*24+tdr)*TrLadCal::GetSize();
    aa->second->Cal2Lin(&(linear[offset]));
  }

}



void TrCalDB::Lin2CalDB(){

  if(! linear){
    printf("TrCalDB::Lin2CalDB()- Error! the linear space pointer is NULL!\n");
    printf(" Calibration is NOT updated!!!\n");
    return;
  }
  run=(int) linear[0];
  for (trcalIT aa=trcal_hwidmap.begin(); aa!=trcal_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    //printf("Filling Calibration %03d\n",hwid);
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=1+(crate*24+tdr)*TrLadCal::GetSize();
    aa->second->Lin2Cal(&(linear[offset]));
    aa->second->SetFilled();
  }
  return;
 
}


void SLin2CalDB(){
  if(TrCalDB::Head)
    TrCalDB::Head->Lin2CalDB();
  return;
}
