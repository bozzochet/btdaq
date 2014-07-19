#include "TLine.h"
#include "TMarker.h"
#include "RTDisplay.h"
#include "TROOT.h"

RTrackerDisplay* RTrackerDisplay::fSelfPtr = 0;

RTrackerDisplay::RTrackerDisplay() 
  : counter(0), fFile(0), fTree(0), fCanvas(0)
{
  fSelfPtr = this;
  
  layer_pad[0]=0; layer_pad[1]=1; layer_pad[2]=3; layer_pad[3]=5;
  layer_pad[4]=7; layer_pad[5]=9; layer_pad[6]=11; layer_pad[7]=13;
  layer_pad[8]=15;

  fEvent = 0;
  gROOT->SetStyle("Plain");
}

RTrackerDisplay::~RTrackerDisplay()
{
  if (fCanvas) delete fCanvas;
}

int RTrackerDisplay::OpenFile(const char* fname)
{
  if (!fname) return -1;
  fFile = TFile::Open(fname); 
  if (!fFile) return -1;

  fTree = (TTree*)fFile->Get("t4");
  if (!fTree) return -1;

  TBranch *bra = fTree->GetBranch("Tracker"); 
  if (!bra) return -1;
  bra->SetAddress(&fEvent);

  TrCalDB *cald = (TrCalDB*)fFile->Get("TrCalDB");
  if (!cald) {
    cout << "TrCalDB not found" << endl;
    return -1;
  }

  AMSTrRawCluster::UsingTrCalDB(cald);
  AMSTrCluster   ::UsingTrCalDB(cald);

  return 0;
}

int RTrackerDisplay::CloseFile()
{ 
  if (!fFile) return -1;
  fFile->Close();
  return 0;
}

int RTrackerDisplay::DrawHits()
{
  if (!fCanvas || !fEvent) return -1;
  
  for(int ii=0;ii<fEvent->GetnTrRecHits();ii++){
    AMSTrRecHit* hit=fEvent->GetTrRecHit(ii);

    hit->BuildCoordinates();
    int res=hit->GetResolvedMultiplicity();
    for (int mm=0;mm<hit->GetMultiplicity();mm++){
        fCanvas->cd(layer_pad[hit->GetLayer()]);
       double X1=(hit->GetCoord(mm))[0];
       double Y1=(hit->GetCoord(1))[1];
//       printf(" %f %f \n",X1,Y1);

       TMarker* pp=new TMarker(X1,Y1,20);
       if(res==mm) 
        pp->SetMarkerColor(kRed);
       else
       pp->SetMarkerColor(kBlue);
        pp->Draw();
        Hits.push_back(pp);


       fCanvas->cd(layer_pad[hit->GetLayer()]+1);
       TMarker* pp2=new TMarker(-Y1,X1,20);
        if(res==mm)
   pp2->SetMarkerColor(kRed);
 else
   pp2->SetMarkerColor(kBlue);
       pp2->Draw();
       Hits.push_back(pp2);
    }    
//    printf ("\n");

  }

  fCanvas->Update();
  return 0;
}

void RTrackerDisplay::ClearHits(){
  
  for (int ii=0;ii<(int)Hits.size();ii++)
    if(Hits[ii]) delete Hits[ii];
  Hits.clear();
  fCanvas->Update();

}

void RTrackerDisplay::PreparePlanes()
{
  if(!fCanvas) return;

  fCanvas->Clear();
  fCanvas->cd();
  fCanvas->Divide(2,8,0.001,0.001);

  for (int jj=1;jj<17;jj++){
    (fCanvas->cd(jj))->Range(-75,-75,75,75); 
  }
  DrawPlanes(0);
  DrawPlanes(1);

  fCanvas->Update();
}


void RTrackerDisplay::DrawPlanes(int off){

  for (int ii=0;ii<192;ii++){
    TkLadder * lad= TkDBc::Head->GetEntry(ii);
    fCanvas->cd(layer_pad[lad->GetLayer()]+off);
    int tkid=lad->GetTkId();
    int nsen=lad->_nsensors;
  
    float X1= (TkCoo::GetGlobalA(tkid,0,0))[0];
    float Y1= (TkCoo::GetGlobalA(tkid,0,0))[1];
    X1= X1 - (lad->rot.GetEl(0,0))*0.7836;
    Y1= Y1 + (lad->rot.GetEl(1,1))*0.07125;

    float Y2= Y1 + (lad->rot.GetEl(1,1)) * TkDBc::Head->_ssize_inactive[1];
    float X2= X1 + (lad->rot.GetEl(0,0))* (nsen)* (TkDBc::Head->_SensorPitchK);
//     printf(" LAD:  %s  %d  %d\n",lad->name,lad->GetTkId(),lad->GetHwId());
//     printf ( " 1 :(   %+8.3f      %+8.3f )       2:  (   %+8.3f      %+8.3f )\n",X1,Y1, X1,Y2);  
//     printf ( " 3 :(   %+8.3f      %+8.3f )       4:  (   %+8.3f      %+8.3f )\n\n",X2,Y1, X2,Y2);  

    TLine* L1;
    TLine* L2;
    TLine* L3;
    TLine* L4;

    if(off){
      L1= new TLine(Y1,X1,Y1,X2);
      L2= new TLine(Y1,X2,Y2,X2);
      L3= new TLine(Y2,X2,Y2,X1);
      L4= new TLine(Y2,X1,Y1,X1);
    }
    else{
      L1= new TLine(X1,Y1,X1,Y2);
      L2= new TLine(X1,Y2,X2,Y2);
      L3= new TLine(X2,Y2,X2,Y1);
      L4= new TLine(X2,Y1,X1,Y1);
    }
    L1->Draw();
    L2->Draw();
    L3->Draw();
    L4->Draw();
  }
    fCanvas->Update();
  return ;
}

int RTrackerDisplay::DrawNext(){
    
  if(counter>=0) 
    return DrawEvent(++counter);
  return -1;
}


int RTrackerDisplay::DrawPrevious(){
    
  if(--counter>=0) 
    return DrawEvent(counter);
  return -1;
}

#include "TrRecon.h"
#include "VCon_root.h"

int RTrackerDisplay::DrawEvent(int ii)
{
  if (!fTree || !fEvent) return -1;

  fEvent->Clear(1);
  GetEntry(ii);

  if (!TrRecon::gethead()) {
    TrRecon::Create();
    TrRecon::TRCon = new VCon_root;
  }

  TrRecon::gethead()->BuildClusterTkIdMap();
  TrRecon::gethead()->BuildLadderClusterMap();

  ClearHits();
  DrawHits();
  fCanvas->Update();

  return 0;
}
