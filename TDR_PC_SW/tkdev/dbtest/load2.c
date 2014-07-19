// Authors: P.Zuccon, D.Caraffini - INFN di Perugia


{
  gInterpreter->AddIncludePath("../AMSbase/include/");
  gInterpreter->AddIncludePath("../TrackerLib/include/");
  gROOT->ProcessLine(".L ../lib/libAMSbase.so");
  gROOT->ProcessLine(".L ../lib/libTracker.so");
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();
  
  /*TFile* f=TFile::Open("CalDB.root")        ;
  TrCalDB* cald=(TrCalDB*) f->Get("TrCalDB");
  AMSTrRawCluster::UsingTrCalDB(cald)       ;
  AMSTrCluster::UsingTrCalDB(cald)   ;
  TrRecon::UsingTrCalDB(cald) ;
    */
  gROOT->SetStyle("Plain");

}

