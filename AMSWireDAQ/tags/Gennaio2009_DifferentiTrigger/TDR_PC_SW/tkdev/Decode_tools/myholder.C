#include "myholder.h"
#include "tkdcards.h"


myholder* myholder::Head=0;
AMSNodeMap myholder::EventMap;




void myholder::data(){
  siamsdata();

  reamsdata();
}

void myholder::siamsdata(){
  _sitkdata();
}


void myholder::reamsdata(){
  _retkdata();
}





void myholder::_sitkdata(){


  TKGEOMFFKEY.init();
  TRMCFFKEY.init();
  TRCALIB.init();
  TRALIG.init();
  
//   FFKEY("TKGE",(float*)&TKGEOMFFKEY,sizeof(TKGEOMFFKEY_DEF)/sizeof(integer),
// 	"MIXED");
  
//   FFKEY("TRMC",(float*)&TRMCFFKEY,sizeof(TRMCFFKEY_DEF)/sizeof(integer),"MIXED");

//   FFKEY("TRCALIB",(float*)&TRCALIB,sizeof(TRCALIB_DEF)/sizeof(integer),"MIXED");
 
//   FFKEY("TRALIG",(float*)&TRALIG,sizeof(TRALIG_DEF)/sizeof(integer),"MIXED");
}

void myholder::_retkdata(){

  TRCLFFKEY.init();

  //  FFKEY("TRCL",(float*)&TRCLFFKEY,sizeof(TRCLFFKEY_DEF)/sizeof(integer),"MIXED");

  TRFITFFKEY.init();

  //FFKEY("TRFIT",(float*)&TRFITFFKEY,sizeof(TRFITFFKEY_DEF)/sizeof(integer),"MIXED");
  //initialization of the fortran fit routines (still needed ?)
  // TKFINI();
}


