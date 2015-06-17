#include "slowgui2.h"

#include <iostream>
using namespace std;

void SlowGui::CreatePlotFrame(TGCompositeFrame *frame) {
  // Creates the frame in which the plot controls will be displayed

  TGHorizontalFrame *hmain=new TGHorizontalFrame(frame,40,200);

  CreateTDRPlotPage(hmain);
  frame->AddFrame(hmain,fL7);
}


void SlowGui::CreateTDRPlotPage(TGCompositeFrame *frame) {
  // Creates the vartious control elements to be displayed

  TGGroupFrame *TDRframe=new TGGroupFrame(frame,"Ladder bias current",kVerticalFrame);

  TGHorizontalFrame *hmain=new TGHorizontalFrame(TDRframe,40,200);
  TGVerticalFrame *avframe;

  for (Int_t tdr=0,step=8; tdr<NTDR; tdr+=step) {
    avframe=new TGVerticalFrame(hmain,200,40);
    for (Int_t i=tdr; i<tdr+step; i++) {
      PlotTDR[i]=new TGCheckButton(avframe,Form("%2d",i));
      avframe->AddFrame(PlotTDR[i],fL7);
    }
    hmain->AddFrame(avframe,fL7);
  }

  TGTextButton *doplot=new TGTextButton(TDRframe,"New Plot");
  doplot->Connect("Clicked()","SlowGui",this,"CreatePlot()");

  TDRframe->AddFrame(hmain,fL7);
  TDRframe->AddFrame(doplot,fL7);
  
  frame->AddFrame(TDRframe,fL7);

}


void SlowGui::InitGraphs() {
  // initializes the graphs, attributing a (hopefully) unique 
  // color line/style combination for each tdr.

  Int_t color[5]={kRed,kGreen,kBlue,kMagenta,kCyan};

  for (Int_t i=0; i<NTDR; i++) {
    LadderCurrent[i]=new TGraph();
    LadderCurrent[i]->SetName(Form("GraphTDRCurrent%03d",i));
    LadderCurrent[i]->SetMarkerStyle(20+i%11);
    LadderCurrent[i]->SetMarkerColor(color[i%5]);
    LadderCurrent[i]->SetLineColor(color[i%5]);
    LadderCurrent[i]->SetLineStyle(1+i%9);
  }
}


void SlowGui::CreatePlot() {
  // creates a new plot according to the selected check boxes
  NewCanvas(400,200);

  PlotCanvas->cd();
  TH1F *frame=PlotCanvas->DrawFrame(0,0,10,10);

  frame->SetXTitle("Time");
  frame->SetYTitle("Leakage current (#muA)");
  frame->GetXaxis()->SetTimeDisplay(1);
  frame->GetXaxis()->SetTimeFormat("%H:%M");

  Int_t cnt=0;
  for (Int_t i=0; i<NTDR; i++) if (PlotTDR[i]->IsOn()) cnt++;

  TLegend *leg=new TLegend(0.75,0.9-cnt*0.05,1.0,0.9);
  //leg->SetFillColor(0);
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);

  for (Int_t i=0; i<NTDR; i++) {
    if (PlotTDR[i]->IsOn()) { 
      printf("TDR %d will be plotted\n",i );
      PlotTDR[i]->SetState(kButtonUp);
      LadderCurrent[i]->Draw("l");
      leg->AddEntry(LadderCurrent[i],Form("TDR %2d",i),"l");
    }
  }

  leg->Draw();
  UpdateCanvases();

}


void SlowGui::NewCanvas(Int_t x, Int_t y, const Char_t *name) {
  // creates a new canvas, choosing a unique name

  Char_t cannam[100];

  CanvasCount=0; 
 
  if (strlen(name)==0) { // automatic name assignment verifies if canvas already exists or not
    do  {
      //gROOT->GetListOfCanvases()->ls();      
      sprintf(cannam,"c%d",CanvasCount++);
      //cout << (gROOT->GetListOfCanvases()->FindObject(cannam)) << endl;
      
    }
    while (gROOT->GetListOfCanvases()->FindObject(cannam)) ;
  }
  else sprintf(cannam,"%s",name);
   
  PlotCanvas=new TCanvas(cannam,cannam,x,y);
  TCanvasImp* PlotCanvasImp=PlotCanvas->GetCanvasImp();
  PlotCanvasImp->ShowMenuBar(kFALSE); // I do not want to have the "Exit root" from the menu...
  PlotCanvasImp->ShowStatusBar(kTRUE);
  PlotCanvasImp->ShowToolBar(kTRUE);
}



void SlowGui::UpdateCanvases() {
  // Looks for the opened canvases and updates them

  TSeqCollection *list=gROOT->GetListOfCanvases();

  Int_t NCanvas=list->GetEntries();
  for (Int_t i=0; i<NCanvas; i++) {
    TCanvas* c=(TCanvas*)list->At(i);
    printf("updatecanvases: entry %d, name %s\n", i, c->GetName());
    RefreshCanvas(c);
  }
}


void SlowGui::RefreshCanvas(TCanvas *aCanvas) {
  // checks the limits of the graphs inside the canvas, and update
  // the frame dimentions

  Double_t minx=0, miny=0, maxx=0, maxy=0;
  Double_t x0,y0,x1,y1, width, height;
 
  TH1F *frame=0;
  frame=(TH1F*)aCanvas->FindObject("hframe");
  if (!frame) return;

  Int_t cnt=0;
  for (Int_t i=0; i<NTDR; i++) {
    TGraph *gr=0;
    gr=(TGraph*)aCanvas->FindObject(Form("GraphTDRCurrent%03d",i));
    if (!gr) continue; // non-present graph
    if (gr->GetN()<2) continue; // we need at least 2 points to use the next lines
    maxx=TMath::MaxElement(gr->GetN(),gr->GetX());
    minx=TMath::MinElement(gr->GetN(),gr->GetX());
    maxy=TMath::MaxElement(gr->GetN(),gr->GetY());
    miny=TMath::MinElement(gr->GetN(),gr->GetY());
    if (!cnt) { x0=minx; x1=maxx; y0=miny; y1=maxy; cnt=1;}
    else {
      x0=TMath::Min(x0,minx);
      x1=TMath::Max(x1,maxx);
      y0=TMath::Min(y0,miny);
      y1=TMath::Max(y1,maxy);
      cnt++;
    }
  }
  if (!cnt) return; // no filled graphs found
  width=0.1*(x1-x0); height=0.1*(y1-y0);
  x0-=width; x1+=2.5*width; y0-=height; y1+=height; // for x1, we give more space for the legend

  frame->SetBins(frame->GetNbinsX(),x0,x1);
  frame->SetMinimum(y0);
  frame->SetMaximum(y1);
  aCanvas->Modified();
  aCanvas->Update();

}


void SlowGui::FillGraphs() {
  // test only
  printf("updating plots...\n");

  time_t tim;
  time(&tim);

  if (DataFile) fprintf(DataFile,"%d  ",tim);

  for (int tdr=0; tdr<NTDR; tdr++) {
    Int_t npoints=LadderCurrent[tdr]->GetN();
    LadderCurrent[tdr]->Set(1+npoints);
    Double_t val=-5+10*gRandom->Rndm();
    LadderCurrent[tdr]->SetPoint(npoints,tim, val);
    if (DataFile) fprintf(DataFile,"%5.2f  ",val);
  }

  if (DataFile) fprintf(DataFile,"\n");
  fflush(DataFile);
  UpdateCanvases();

}


// void SlowGui::OpenDataFile() {
//   time_t tim;
//   time(&tim);
//   tm *ltim=localtime(&tim);
//   Char_t time[50];
//   strftime(time,50,"%d%m%Y%H%M%S",ltim);
//   DataFile=fopen(Form("data_%s.txt",time),"wt");
// }


void SlowGui::CloseDataFile() {
  fclose(DataFile);
}
