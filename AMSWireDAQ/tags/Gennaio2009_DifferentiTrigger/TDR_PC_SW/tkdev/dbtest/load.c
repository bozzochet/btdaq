// Authors: P.Zuccon, D.Caraffini - INFN di Perugia


{
  
  /*TFile* f=TFile::Open("CalDB.root")        ;
  TrCalDB* cald=(TrCalDB*) f->Get("TrCalDB");
  AMSTrRawCluster::UsingTrCalDB(cald)       ;
  AMSTrCluster::UsingTrCalDB(cald)   ;
  TrRecon::UsingTrCalDB(cald) ;
    */
  gROOT->SetStyle("Plain");



  gROOT->ProcessLine(".L point.C+");
  gROOT->ProcessLine(".L TkObject.C+");
  gROOT->ProcessLine(".L TkLadder.C+");
  gROOT->ProcessLine(".L TkPlane.C+");
  gROOT->ProcessLine(".L TkDBc.C+");
  gROOT->ProcessLine(".L TrCalDB.C+");
  gROOT->ProcessLine(".L TrLadCal.C+");
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();

  gROOT->ProcessLine(".L astring.C+");
  //  gROOT->ProcessLine(".L upool.C+");
  gROOT->ProcessLine(".L id.C+");
  gROOT->ProcessLine(".L node.C+");
/*   gROOT->ProcessLine(".L amsdbc.C+"); */

/*   gROOT->ProcessLine(".L timeid.C+"); */

}
