#define __TrDHF_C__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrDHF.h
///\brief Source file of TrDHF class
///
///\date  2008/02/16 AO  First version
///
//////////////////////////////////////////////////////////////////////////

#include "TrDHF.h"

#include <iostream>

using namespace std;

ClassImp(TrDHF)


//---------------------------------------------------------------
TrDHF::TrDHF(TDirectory* Dir, char* name, char* title){ 

  fHashTable.clear();
  fDir = Dir;
  if (Dir==0) fDir = gDirectory;

  // Directory tree:
  // |
  // +-- layer 1 -+- ladder 101
  // |            +- ladder 102
  // |            +- ...
  // ...

  // layers
  char dirname[80];
  char dirtitle[80];
  for (int ll=0; ll<maxlay; ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    sprintf(dirtitle,"Layer %d",layer);
    fDirLayers[ll] = fDir->mkdir(dirname,dirtitle);
  }
  // slot
  char subdirname[80];
  char subdirtitle[80];
  char sidename[2] = {'M','P'};
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    int ll = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",ladder->GetTkId());
    sprintf(subdirtitle,"Slot %d, Side %c",ladder->GetSlot(),sidename[ladder->GetSide()]);
    fDirLayers[ll]->mkdir(subdirname,subdirtitle);
  }
  // Set the name and the title of the class  
  this->SetNameTitle(name,title);
}


//---------------------------------------------------------------
TrDHF::TrDHF(const TrDHF &orig): TNamed(orig) {
  fDir = orig.fDir;
  if (orig.fHashTable.empty()) {
    fHashTable.clear();
    fHashTable.insert(orig.fHashTable.begin(),orig.fHashTable.end());
  }
}


//---------------------------------------------------------------
TrDHF::~TrDHF(){
  if (!fHashTable.empty()) fHashTable.clear();
}


//---------------------------------------------------------------
void TrDHF::Sumw2(){
  //Sets all histograms to have  statistic error calculation
  map<int,TObject*>::iterator it;
  for (it=fHashTable.begin(); it!=fHashTable.end(); it++) {
    TObject* histo = it->second;
    if      (strcmp(histo->ClassName(),"TH1D")!=0) ((TH1D*) histo)->Sumw2();
    else if (strcmp(histo->ClassName(),"TH2D")!=0) ((TH2D*) histo)->Sumw2();
    else if (strcmp(histo->ClassName(),"TH3D")!=0) ((TH3D*) histo)->Sumw2();
  }
}


//---------------------------------------------------------------
void TrDHF::Add(TObject* obj){
  int ihash = obj->Hash();
  fHashTable.insert(pair<int,TObject*>(ihash,obj));
}


//---------------------------------------------------------------
TObject* TrDHF::Get(char* name){
  // Return a pointer the the histogram with the reqired name. 
  // The output need a cast for the correct type.

  // return fHlist->FindObject(name); // Requires a sequential scan
  TString istring(name);
  int ihash = istring.Hash();
  return fHashTable.find(ihash)->second;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineTracker(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn) {

  /* Appends 1/2 TH1F histos in the main directory */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH1D* histo;

  fDir->cd();
  if (!pn) {
    sprintf(histoname,"%s_all",name);
    histo = new TH1D(histoname,title, binx, lowx, upx);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH1D(histoname, title, binx, lowx, upx);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH1D(histoname, title, binx, lowx, upx);
    Add(histo);
  }
  pp->cd();
  return pp;
}



//---------------------------------------------------------------
TDirectory* TrDHF::DefineTracker(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				 Int_t biny, Double_t lowy, Double_t upy, Int_t pn) {
  
  /* Appends 1/2 TH1F histos in the main directory */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH2D* histo;
  
  fDir->cd();
  if (!pn) {
    sprintf(histoname,"%s_all",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
    Add(histo);
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineTracker(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				 Int_t biny, Double_t lowy, Double_t upy, 
				 Int_t binz, Double_t lowz, Double_t upz, Int_t pn) {
  
  /* Appends 1/2 TH1F histos in the main directory */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH3D* histo;
  
  fDir->cd();
  if (!pn) {
    sprintf(histoname,"%s_all",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_all_P",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
    sprintf(histoname,"%s_all_N",name);
    histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
    Add(histo);
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineLayers(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn) {

  /* Appends 8/16 TH1F histos in the layer directories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH1D* histo;

  char  dirname[80];
  for (int ll=0; ll<maxlay; ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if (!pn) {
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH1D(histoname,title, binx, lowx, upx);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}



//---------------------------------------------------------------
TDirectory* TrDHF::DefineLayers(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				Int_t biny, Double_t lowy, Double_t upy, Int_t pn){

  /* Appends 8/16 TH2F histos in the layer directories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH2D* histo;

  char  dirname[80];
  for (int ll=0; ll<maxlay; ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if(!pn){
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH2D (histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineLayers(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				Int_t biny, Double_t lowy, Double_t upy, 
				Int_t binz, Double_t lowz, Double_t upz, Int_t pn){

  /* Appends 8/16 TH2F histos in the layer directories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH3D* histo;

  char  dirname[80];
  for (int ll=0; ll<maxlay; ll++) {
    int layer = ll+1;
    sprintf(dirname,"layer%1d",layer);
    fDir->cd(dirname);
    if(!pn){
      sprintf(histoname,"%s_layer%1d",name,layer);
      histo = new TH3D (histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
      sprintf(histoname,"%s_layer%1d_N",name,layer);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineLadders(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn) {
  
  /* Appends 192/384 TH1F histos in the ladders subdirectories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH1D* histo;

  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!pn){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH1D(histoname, title, binx, lowx, upx);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}



//---------------------------------------------------------------
TDirectory* TrDHF::DefineLadders(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				 Int_t biny, Double_t lowy, Double_t upy, Int_t pn){

  /* Appends 192/384 TH1F histos in the ladders subdirectories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH2D* histo;

  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!pn){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH2D(histoname, title, binx, lowx, upx, biny, lowy, upy);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineLadders(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
				 Int_t biny, Double_t lowy, Double_t upy, 
				 Int_t binz, Double_t lowz, Double_t upz, Int_t pn){

  /* Appends 192/384 TH1F histos in the ladders subdirectories */
  TDirectory* pp = gDirectory;
  char  histoname[80];
  TH3D* histo;

  char  subdirname[80];
  for (int ii=0; ii<TkDBc::Head->GetEntries(); ii++) {
    TkLadder* ladder = TkDBc::Head->GetEntry(ii);
    int TkId = ladder->GetTkId(); 
    int ll   = ladder->GetLayer() - 1; 
    sprintf(subdirname,"%+03d",TkId);
    fDirLayers[ll]->cd(subdirname);
    if(!pn){
      sprintf(histoname,"%s_%+03d",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
    else {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
      sprintf(histoname,"%s_%+03d_N",name,TkId);
      histo = new TH3D(histoname, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz);
      Add(histo);
    }
  }
  pp->cd();
  return pp;
}


//---------------------------------------------------------------
TDirectory* TrDHF::DefineEntries(char* name, Int_t pn) { 
  /* defines an entry histogram */
  TDirectory* pp = gDirectory;
  fDir->cd();
  char  histoname[80];
  TH2D* histo; 
  if(!pn){
    sprintf(histoname,"%s_entries",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,8,0.5,8.5);
    Add(histo);
  }
  else {
    sprintf(histoname,"%s_entries_P",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,8,0.5,8.5);
    Add(histo);
    sprintf(histoname,"%s_entries_N",name);
    histo = new TH2D(histoname,"; slot; layer",31,-15.5,15.5,8,0.5,8.5);
    Add(histo);
  }
  pp->cd();
  return pp;
}



//---------------------------------------------------------------
TDirectory* TrDHF::Define(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn) {
  TDirectory* pp = gDirectory;
  DefineEntries(name, pn);
  DefineTracker(name, title, binx, lowx, upx, pn);
  DefineLayers (name, title, binx, lowx, upx, pn);
  DefineLadders(name, title, binx, lowx, upx, pn);
  pp->cd();
  return pp;
}



//---------------------------------------------------------------
TDirectory* TrDHF::Define(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
			  Int_t biny, Double_t lowy, Double_t upy, Int_t pn){
  TDirectory* pp = gDirectory;
  DefineEntries(name, pn);
  DefineTracker(name, title, binx, lowx, upx, biny, lowy, upy, pn);
  DefineLayers (name, title, binx, lowx, upx, biny, lowy, upy, pn);
  DefineLadders(name, title, binx, lowx, upx, biny, lowy, upy, pn);
  pp->cd();
  return pp;
}
 

//---------------------------------------------------------------
TDirectory* TrDHF::Define(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
			  Int_t biny, Double_t lowy, Double_t upy, 
			  Int_t binz, Double_t lowz, Double_t upz, Int_t pn) {
  TDirectory* pp = gDirectory;
  DefineEntries(name, pn);
  DefineTracker(name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, pn);
  DefineLayers (name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, pn);
  DefineLadders(name, title, binx, lowx, upx, biny, lowy, upy, binz, lowz, upz, pn);
  pp->cd();
  return pp;
}
 



//---------------------------------------------------------------
void TrDHF::FillTracker(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {

  char histoname[80];
  sprintf(histoname,"%s_all",name);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_all_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_all_N",name);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillTracker() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillTracker() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}




//---------------------------------------------------------------
void TrDHF::FillTracker(Int_t pn, TrClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {

  char histoname[80];
  sprintf(histoname,"%s_all",name);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_all_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_all_N",name);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillTracker() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillTracker() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillTracker(Int_t side, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_all_P",name);
  else         sprintf(histoname,"%s_all_N",name);
 
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillTracker() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillTracker() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillTracker(char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  char histoname[80];
  sprintf(histoname,"%s_all",name);
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillTracker() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillTracker() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLayer(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  int TkId         = cluster->GetTkId(); 
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer(); 
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_layer%1d_N",name,layer);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLayer() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLayer() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLayer(Int_t pn, TrClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  int TkId         = cluster->GetTkId(); 
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer(); 
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_layer%1d_P",name,layer);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_layer%1d_N",name,layer);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLayer() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLayer() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLayer(Int_t side, Int_t layer, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_layer%1d_P",name,layer);
  else         sprintf(histoname,"%s_layer%1d_N",name,layer);
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLayer() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLayer() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLayer(Int_t layer, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the layer histograms (p/n)
  char histoname[80];
  sprintf(histoname,"%s_layer%1d",name,layer);
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLayer() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLayer() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLadder(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  int  TkId = cluster->GetTkId(); 
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_%+03d_N",name,TkId);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLadder() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLadder() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}




//---------------------------------------------------------------
void TrDHF::FillLadder(Int_t pn, TrClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  int  TkId = cluster->GetTkId(); 
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_%+03d_P",name,TkId);
    }
    else  {  
      sprintf(histoname,"%s_%+03d_N",name,TkId);
    }
  }
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLadder() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLadder() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}




//---------------------------------------------------------------
void TrDHF::FillLadder(Int_t side, Int_t TkId, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  char histoname[80];
  if (side==1) sprintf(histoname,"%s_%+03d_P",name,TkId);
  else         sprintf(histoname,"%s_%+03d_N",name,TkId);
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLadder() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
     else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
   else {
      cout <<"TrDHF:FillLadder() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::FillLadder(Int_t TkId, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  // fill the ladder histograms (p/n)
  char histoname[80];
  sprintf(histoname,"%s_%+03d",name,TkId);
  TObject* tmp = Get(histoname);
  if (!tmp) {
    cout << "TrDHF::FillLadder() Error: " << histoname << " does not exist, ignoring for the moment ..." << endl;
  }
  else {
    if      (strcmp("TH1D",tmp->ClassName())==0) {     // is a TH1D
      TH1D* hh = (TH1D*) tmp;
      hh->Fill(X1,X2);
    }
    else if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
      TH2D* hh = (TH2D*) tmp;
      hh->Fill(X1,X2,X3);
    }
    else if (strcmp("TH3D",tmp->ClassName())==0) {     // is a TH3D
      TH3D* hh = (TH3D*) tmp;
      hh->Fill(X1,X2,X3,w);
    }
    else {
      cout <<"TrDHF:FillLadder() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
    }
  }
}



//---------------------------------------------------------------
void TrDHF::Fill(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (pn, cluster, name, X1, X2, X3, w); 
  FillTracker(pn, cluster, name, X1, X2, X3, w); 
  FillLayer  (pn, cluster, name, X1, X2, X3, w);
  FillLadder (pn, cluster, name, X1, X2, X3, w);
}



//---------------------------------------------------------------
void TrDHF::Fill(Int_t pn, TrClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w) {
  FillEntry  (pn, cluster, name, X1, X2, X3, w); 
  FillTracker(pn, cluster, name, X1, X2, X3, w); 
  FillLayer  (pn, cluster, name, X1, X2, X3, w);
  FillLadder (pn, cluster, name, X1, X2, X3, w);
}



//---------------------------------------------------------------
void TrDHF::FillEntry(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w){
  char histoname[80];
  int TkId         = cluster->GetTkId();
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  sprintf(histoname,"%s_entries",name);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_entries_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_entries_N",name);
    }
  }
  TObject* tmp = Get(histoname);
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrDHF:FillEntry() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
  }
}



//---------------------------------------------------------------
void TrDHF::FillEntry(Int_t pn, TrClusterR* cluster, char *name, Double_t X1, Double_t X2, Double_t X3, Double_t w){
  char histoname[80];
  int TkId         = cluster->GetTkId();
  TkLadder* ladder = TkDBc::Head->FindTkId(TkId);
  int layer        = ladder->GetLayer();
  int slot         = ladder->GetSlot();
  sprintf(histoname,"%s_entries",name);
  if (pn) {
    if (cluster->GetSide()==1) {
      sprintf(histoname,"%s_entries_P",name);
    }
    else if (cluster->GetSide()==0) {  
      sprintf(histoname,"%s_entries_N",name);
    }
  }
  TObject* tmp = Get(histoname);
  if (strcmp("TH2D",tmp->ClassName())==0) {     // is a TH2D
    TH2D* hh = (TH2D*) tmp;
    hh->Fill((float)slot,(float)layer,1.);
  }
  else {
    cout <<"TrDHF:FillEntry() Error: " << histoname << " type " << tmp->ClassName() << " supported" << endl;
  }
}










//---------------------------------------------------------------

void TrDHF::BookHistos(){
//    Define("Occupancy","Occupancy",1024,0,1024);
//    Define("Width","Width",100,0,100,1);
    Define("Signal","Signal; ADC counts; events",800,0,200,1);
//    Define("SN","SN",500,0,250,1);
//    Define("SeedSN","SeedSN",500,0,250,1);
//    Define("OccupancyCOG","OccupancyCOG",1024,0,1024);
//    Define("Eta","Eta",120,-0.1,1.1,1);
//    Define("STot_VS_SeedAdd","STot_VS_SeedAdd",1024,0,1024,500,0,500,0);
//    Define("EtaRaw","EtaRaw",120,-0.6,0.6,1);
//    Define("EtaRaw2","EtaRaw2",120,-0.6,0.6,1);
//    Define("OccupancyCOGBeam","OccupancyCOGBeam",1024,0,1024);
//    Define("SignalBeam","SignalBeam",500,0,500,1);
   return;
}

void TrDHF::FillAll(Event* ev){
  //printf("Filling\n");
//   for (int ii=0;ii<ev->GetnTrClusters();ii++){
//     TrClusterR* clu=ev->GetTrCluster(ii);

//      Fill(0,clu,"Occupancy",clu->GetSeedAddress()*1.);
//      Fill(1,clu,"Width",clu->GetLength()*1.);
//      Fill(0,clu,"OccupancyCOG",clu->GetCofG()*1.);
//     Fill(1,clu,"Signal",clu->GetTotSignal()*1.);
//      //Fill(1,clu,"SN",clu->GetTotSN()*1.);
//      Fill(1,clu,"SeedSN",clu->GetSeedSN()*1.);
//      Fill(1,clu,"Eta",clu->GetEta()*1.);
//      //   Fill(1,clu,"EtaRaw",clu->GetEtaRaw()*1.);
//      Fill(0,clu,"STot_VS_SeedAdd",clu->GetSeedAddress()*1.,clu->GetTotSignal());
//      if(clu->_tkid==900) {
//        if ((clu->GetCofG()>370&&clu->GetCofG()<430) || (clu->GetCofG()>725&&clu->GetCofG()<750)) {
//          Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	 Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
// 	 }
//      }
//      if(clu->_tkid==901) {
//        if ((clu->GetCofG()>390&&clu->GetCofG()<435) || (clu->GetCofG()>735&&clu->GetCofG()<755)) {
//          Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	 Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
//        }
//      }
//      if(clu->_tkid==904) {
//        if ((clu->GetCofG()>450&&clu->GetCofG()<500) || (clu->GetCofG()>715&&clu->GetCofG()<735)) {
//          Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	 Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
//        }
//      }
//      if(clu->_tkid==905) {
//        if ((clu->GetCofG()>430&&clu->GetCofG()<480) || (clu->GetCofG()>960&&clu->GetCofG()<9995)) {
//          Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	 Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
//        }
//      }
//      if(clu->_tkid==2110) {
//        if ((clu->GetCofG()>65&&clu->GetCofG()<150) || (clu->GetCofG()>740&&clu->GetCofG()<830)) {
//          Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	 Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
//        }
//      }
//      if(clu->_tkid==2111) {
//        if ((clu->GetCofG()>100&&clu->GetCofG()<200) || (clu->GetCofG()>740&&clu->GetCofG()<820)) {
//          if (!(clu->GetCofG()>128&&clu->GetCofG()<129 || clu->GetCofG()>133&&clu->GetCofG()<134 || clu->GetCofG()>136&&clu->GetCofG()<138 || clu->GetCofG()>130&&clu->GetCofG()<131 || clu->GetCofG()>194&&clu->GetCofG()<196 || clu->GetCofG()>197&&clu->GetCofG()<199 || clu->GetCofG()>234&&clu->GetCofG()<236)) {
//            Fill(0,clu,"OccupancyCOGBeam",clu->GetCofG()*1.);
// 	   Fill(1,clu,"SignalBeam",clu->GetTotSignal()*1.);
// 	 }  
//        }
//      }
//   }
   return;
}

