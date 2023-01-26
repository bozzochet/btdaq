#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
//----------------ROOT includes---------------------------
#include "TCanvas.h"
#include "TPad.h"
#include "TNtuple.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TLine.h"
#include "TText.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TROOT.h"
#include "TString.h"
#include "TApplication.h"

#define NJINF 24 //maximum number of JINF



void LinesVas(int nva=16, int nch_for_va=64);
void InitStyle();

using namespace std;

class ConfPars{

public:
  char CALPATH[255];
  unsigned int refmask;
  ConfPars();
};

ConfPars::ConfPars() {
  sprintf(CALPATH,"./CalData");
  refmask=0;
}

ConfPars* CPars;

bool mute=false;

void ShowHelp(char *cmd);
void RefMask(ConfPars *CPars, int run_number, int jinfnum, const char *nameprefixin);
Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, const char *nameout, const char *outkind);
Int_t Summary(char *filename, const char *nameout, const char *outkind);
int SummaryComplete(char *dir, int run_number, int jinfnum, const char *outkind, const char *nameprefix);

//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
  
  int ret=0;

  TString _name = argv[0];
  if (_name.Contains("Mute")) mute=true;

  TApplication* myapp = NULL;
  if (!mute) myapp = new TApplication("myapp", &argc, argv);

  CPars= new ConfPars();
  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (argc<2) {
    ret=1;
    printf("Too few arguments: %s <run #> [type of file](everything for pdf, \"ps\" for PostScript)\n", argv[0]);
    printf("                              [name prefix](default is \"\")\n");
    printf("                              [dir](default is \"./CalData\")\n");
  }
  else if (argc==2) {
    for (int ii=0;ii<NJINF;ii++) {
      RefMask(CPars, atoi(argv[1]), ii, "");
      ret=SummaryComplete(CPars->CALPATH, atoi(argv[1]), ii, "pdf", "");
    }
  }
  if (argc>2) {
    char type[255];
    sprintf(type,"pdf");
    if (!strcmp(argv[2],"ps"))
      sprintf(type,"ps");
    if (argc==3) {
      for (int ii=0;ii<NJINF;ii++) {
	RefMask(CPars,atoi(argv[1]), ii, "");
	ret=SummaryComplete(CPars->CALPATH, atoi(argv[1]), ii, type, "");
      }
    }
    else if (argc==4) {
      for (int ii=0;ii<NJINF;ii++) {
	RefMask(CPars,atoi(argv[1]), ii, argv[3]);
	ret=SummaryComplete(CPars->CALPATH, atoi(argv[1]), ii, type, argv[3]);
      }
    }
    else if (argc>4) {
      sprintf(CPars->CALPATH,"%s/",argv[4]);
      for (int ii=0;ii<NJINF;ii++) {
	RefMask(CPars,atoi(argv[1]), ii, argv[3]);
	ret=SummaryComplete(CPars->CALPATH, atoi(argv[1]), ii, type, argv[3]);
      }
    }
  }

  printf("%s\n",(ret)?"ERROR":"READY");

  if (!mute) myapp->Run();
  
  delete CPars;

  return ret;
}

void RefMask(ConfPars *CPars, int run_number, int jinfnum, const char *nameprefixin) {
  CPars->refmask=0; 
  char calfileprefix[512];
  char prefix[255];
  sprintf(prefix,"%s",nameprefixin);
  if (prefix[0]!=0) sprintf(prefix,"%s_",prefix);
  //  sprintf(calfileprefix,"%s/%s%06d", CPars->CALPATH, prefix, run_number);
  sprintf(calfileprefix,"%s/%s%d", CPars->CALPATH, prefix, run_number);
  
  char calfilename[512];
  
  for (int ii=0;ii<24;ii++) {
    sprintf(calfilename,"%s_%02d%02d.cal", calfileprefix, jinfnum, ii);
    struct stat buf;
    if (stat(calfilename,&buf)==0) {
      CPars->refmask=CPars->refmask|1<<ii;
      printf("I,ve found %s for TDR number %d\n", calfilename,ii);
    }
  }
  return;
}

int SummaryComplete(char *dir, int run_number, int jinfnum, const char *outkind, const char *nameprefix) {
  int ret=0;
  int lasttdr=0;
  int tdrcount=0;

  char calfileprefix[255];
  char nameouttemp[255];
  char nameout[255];
  char nameprefixtemp[255];
  //  char systemcommand[255];

  if (nameprefix[0]!=0) sprintf(nameprefixtemp,"summary-%s_",nameprefix);
  else sprintf(nameprefixtemp,"summary-%s",nameprefix);
  //  sprintf(calfileprefix,"%s/%s%02d_%06d", dir, nameprefixtemp, jinfnum, run_number);
  sprintf(calfileprefix,"%s/%s%02d_%d", dir, nameprefixtemp, jinfnum, run_number);
  //	printf("%s\n", calfileprefix);
  sprintf(nameout,"%s.cal.%s", calfileprefix, outkind);

  //  sprintf(systemcommand,"evince %s &",nameout);

  for (int ii=0;ii<24;ii++){
    //    LPRINTF("%d : %d\n", ii, CPars->refmask&(1<<ii)); //only for debug
    if(CPars->refmask&(1<<ii)) {
      tdrcount++;
    }
  }
  if (tdrcount!=0) printf("We have %d file of calibrations for run #%d, Jinf #%d, in the directory \"%s\" with prefix name \"%s\"\n", tdrcount, run_number, jinfnum, dir, nameprefix);
	
  for (int ii=0;ii<24;ii++){
    //    LPRINTF("%d : %d\n", ii, CPars->refmask&(1<<ii)); //only for debug
    if(CPars->refmask&(1<<ii)) {
      lasttdr++;
      printf("Summarizing %dth calibration file...\n",lasttdr);
      if (lasttdr==tdrcount) {
	printf("last TDR (# %d)..\n",ii);
	sprintf(nameouttemp,"%s)",nameout);
      }
      else if (lasttdr==1) {
	sprintf(nameouttemp,"%s(",nameout);
	printf("first TDR (# %d)...\n",ii);
      }
      else {
	sprintf(nameouttemp,"%s",nameout);
	printf("n-th TDR (# %d)...\n",ii);
      }
      ret=Summary(dir, run_number, jinfnum, ii, nameprefix, nameouttemp, outkind);
    }
  }
  //  struct stat buff;
  // if (!mute) {
  //   if (stat(nameout,&buff)==0) system(systemcommand);
  // }
  return ret;
}

Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, const char *nameout, const char *outkind) {
  int ret=0;

  char calfileprefix[255];
  Char_t filename[255];
  char nameprefixintemp[255];

  if (nameprefixin[0]!=0) sprintf(nameprefixintemp,"%s_",nameprefixin);
  else sprintf(nameprefixintemp,"%s",nameprefixin);
  //  sprintf(calfileprefix,"%s/%s%06d", dir, nameprefixintemp, run_number);
  sprintf(calfileprefix,"%s/%s%d", dir, nameprefixintemp, run_number);
  sprintf(filename,"%s_%02d%02d.cal", calfileprefix,jinfnum, tdr_number);
  ret=Summary(filename, nameout, outkind);
	
  return ret;
}

Int_t Summary(char *filename, const char *nameout, const char *outkind) {

  static int count=0;

  InitStyle();

  gROOT->SetStyle("StyleWhite");
  //  gStyle->SetPaperSize(27,20);

  //MD: make all of them as vectors
  Int_t dumi;
  Float_t dumf;
  Double_t dumd;
  char dums[256];
  Double_t CN[160];
  Int_t va[10240];//red but not used
  Int_t chofva[10240];//red but not used
  Int_t ch[10240];//red but not used
  Double_t ped[10240];
  Float_t rsig[10240];
  Float_t sig[10240];
  Int_t status[10240];
  TCanvas* c = NULL;

  for (Int_t ii=0;ii<10240;ii++) {
    ped[ii] = 0;
    rsig[ii] = 0;
    sig[ii] = 0;
    status[ii] = 0;
  }
  
  TLatex *comment=new TLatex(1-0.01,0.01,Form("%s",filename));
  comment->SetNDC();  
  comment->SetTextAngle(90);
  comment->SetTextAlign(11);
  comment->SetTextSize(0.025);

  FILE *calfil=NULL;
  calfil=fopen(filename,"r");
  if(! calfil) {
    printf("file %s does not exist\n",filename);
    return 1;
  }

  int const dimline = 512;
  char line[dimline];

  int nva=0;
  int nch=0;
  
  while (1) {
    if (fgets(line, dimline, calfil) != NULL) {
      if (*line == '#') { /* ignore comment line, but there shouln't be */
	continue;
      }
      else {
	int firstn = sscanf(line, "%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s", dums, dums, dums,
			    dums, dums, dums, dums, dums);
	//	printf("%d)\n", firstn);
	if (firstn == 3 || firstn == 8) {
	  if (firstn==3) {
	    sscanf(line, "%d,\t%f,\t%lf", &dumi, &dumf, &dumd);
	    //	    printf("I did read: %d) %f %lf\n", dumi, dumf, dumd);
	    CN[dumi]=dumd;
	    nva++;
	  }
	  else {
	    sscanf(line,"%d %d %d %lf %f %f %f %d", &ch[nch], &va[nch], &chofva[nch],
		   &ped[nch], &rsig[nch], &sig[nch],
		   &dumf, &status[nch]);
	    // printf("I did read: %d) VA=%d, ch_VA=%d ped=%lf rawsig=%f sig=%f ?=%f status=%d\n", ch[nch], va[nch], chofva[nch],
	    //  	   ped[nch], rsig[nch], sig[nch],
	    //  	   dumf, status[nch]);
	    nch++;
	  }
	}
	else {
	  printf("The number of elements must be 3 or 8. On this line we had %d elements. Skipping it...\n", firstn);
	}
      }
    }
    else {
      //      printf("... closing cal file\n");
      fclose(calfil);
      break;
    }
  }			 

  printf("I did read: %d VAs and %d channels\n", nva, nch);
  
  c = new TCanvas(Form("c_%d", count), Form("%s_%d",filename,0),0*100,0*10,600,400);
  comment->Draw();
  c->Update();
  c->Divide(2,2);
  c->SetFillStyle(0);
  c->cd(1);
  gPad->SetFillStyle(0);

  c->cd(1);
  TH2F *fram=new TH2F(Form("fram_%d", count), Form("%s: pedestals",filename),nch,1,nch,2000,0,2000);
  fram->SetStats(0);
  fram->Draw();
  TH1F *isto=new TH1F(Form("isto_%d", count), Form("%s: pedestals",filename),nch,1,nch);
  for (Int_t ii=0;ii<nch;ii++) isto->SetBinContent(ii,ped[ii]);
  isto->Draw("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch/nva)));
  gPad->Update();	

  c->cd(2);
  gPad->SetFillStyle(0);
  TH2F *fram2=new TH2F(Form("fram2_%d", count), Form("%s: sigma",filename),nch,1,nch,11,0,10);
  fram2->SetStats(0);
  fram2->Draw();
  TH1F *isto2=new TH1F(Form("isto2_%d", count), Form("%s: sigma",filename),nch,1,nch);  
  for (int ii=0;ii<nch;ii++) isto2->SetBinContent(ii,sig[ii]);
  isto2->Draw("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch/nva)));
  gPad->Update();

  c->cd(3);
  gPad->SetFillStyle(0);
  TH2F *fram3=new TH2F(Form("fram3_%d", count), Form("%s: sigma raw",filename),nch,1,nch,21,0,20);
  fram3->SetStats(0);
  fram3->Draw();
  TH1F *isto3=new TH1F(Form("isto3_%d", count), Form("%s: sigma raw",filename),nch,1,nch);
  for (int ii=0;ii<nch;ii++) isto3->SetBinContent(ii,rsig[ii]);
  isto3->Draw("same");
  gPad->Update();
  LinesVas(nva, ((int)(nch/nva)));
  gPad->Update();

  c->cd(4);
  gPad->SetFillStyle(0);
  TH2F *fram4=new TH2F(Form("fram4_%d", count), Form("%s: strip status",filename),nch,1,nch,11,0,10);
  fram4->SetStats(0);
  fram4->Draw();
  TH1F *isto4=new TH1F(Form("isto4_%d", count), Form("%s: strip status",filename),nch,1,nch);	
  // int chip=0;
  // int channelofchip=0;
  for (int jj=0;jj<nch;jj++) {
    isto4->SetBinContent(jj,status[jj]);
    //printf("canale: %d, chip: %d, CN: %lf\n",jj,chip,CN[chip]);
    // channelofchip++;
    // if (channelofchip==64) {
    // 	chip++;
    // 	channelofchip=0;//change the CN value every 16 bins (channels)
    // }
  }
  isto4->Draw("same");	
  gPad->Update();
  LinesVas(nva, ((int)(nch/nva)));
  gPad->Update();

  /*	
	Char_t gen[200];
	sprintf(gen,"%s",filename);
	TLatex *ladnam=new TLatex(0.2,0.4,gen);
	ladnam->SetNDC();
	ladnam->SetTextSize(0.2);
	ladnam->Draw();
  */

  c->Update();

  c->Print(nameout,outkind);

  if (mute) {
    if (comment) delete comment;
    if (c) delete c;
    if (fram) delete fram;
    if (fram2) delete fram2;
    if (fram3) delete fram3;
    if (fram4) delete fram4;
    if (isto) delete isto;
    if (isto2) delete isto2;
    if (isto3) delete isto3;
    if (isto4) delete isto4;
  }

  count++;
  return 0;
}

void LinesVas(int nva, int nch_for_va) {
  Double_t xmin,ymin,xmax,ymax;
  gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
  TLine *line=new TLine();
  line->SetLineColor(kBlue);
  TText *text=new TText();
  text->SetTextAlign(21); // vert and hor. alignment: horiz centered, top
  text->SetTextColor(kBlue);
  line->SetLineStyle(3);
  for (Int_t va=1; va<=nva; va++) {// first and last separation lines are not drawn
    // line->SetLineStyle((va==10)?1:3);
    // if (va==11) {//this change the color from now on, since the `line` is always the same
    //   line->SetLineColor(kRed); text->SetTextColor(kRed);
    // }
    if (va*nch_for_va>xmin && va*nch_for_va<xmax) line->DrawLine(1+va*nch_for_va,ymin,1+va*nch_for_va,ymax);
    if (va*nch_for_va>xmin && va*nch_for_va<=xmax) text->DrawText((va-0.5)*nch_for_va,ymax+(ymax-ymin)*0.01,Form("%d",va));
  }

  if (line) delete line;
  if (text) delete text;
}

void InitStyle() {
  gROOT->SetStyle("Plain");

  TStyle *myStyle[2], *tempo;
  myStyle[0]=new TStyle("StyleWhite", "white");
  myStyle[1]=new TStyle("StyleBlack", "black");

  tempo=gStyle;
  Int_t linecol, bkgndcol, histcol;

  for(Int_t style=0; style<2; style++) {

    linecol=kWhite*style+kBlack*(1-style);
    bkgndcol=kBlack*style+kWhite*(1-style);
    histcol=kYellow*style+kBlack*(1-style); // was 95

    myStyle[style]->Copy(*tempo);

    myStyle[style]->SetCanvasBorderMode(0);
    myStyle[style]->SetCanvasBorderSize(1);
    myStyle[style]->SetFrameBorderSize(1);
    myStyle[style]->SetFrameBorderMode(0);
    myStyle[style]->SetPadBorderSize(1);
    myStyle[style]->SetStatBorderSize(1);
    myStyle[style]->SetTitleBorderSize(1);
    myStyle[style]->SetPadBorderMode(0);
    myStyle[style]->SetPalette(1,0);
    myStyle[style]->SetPaperSize(20,27);	
    myStyle[style]->SetFuncColor(kRed);
    myStyle[style]->SetFuncWidth(1);
    myStyle[style]->SetLineScalePS(1); 
    myStyle[style]->SetCanvasColor(bkgndcol);
    myStyle[style]->SetAxisColor(linecol,"XYZ");
    myStyle[style]->SetFrameFillColor(bkgndcol);
    myStyle[style]->SetFrameLineColor(linecol);
    myStyle[style]->SetLabelColor(linecol,"XYZ");
    myStyle[style]->SetPadColor(bkgndcol);
    myStyle[style]->SetStatColor(bkgndcol);
    myStyle[style]->SetStatTextColor(linecol);
    myStyle[style]->SetTitleColor(linecol,"XYZ");
    myStyle[style]->SetTitleFillColor(bkgndcol);
    myStyle[style]->SetTitleTextColor(linecol);
    myStyle[style]->SetLineColor(linecol);
    myStyle[style]->SetMarkerColor(histcol);
    myStyle[style]->SetTextColor(linecol);

    myStyle[style]->SetGridColor((style)?13:kBlack);
    myStyle[style]->SetHistFillStyle(1001*(1-style));
    myStyle[style]->SetHistLineColor(histcol);
    myStyle[style]->SetHistFillColor((style)?bkgndcol:kYellow);
    myStyle[style]->SetOptDate(22);
    myStyle[style]->GetAttDate()->SetTextColor(linecol);
    myStyle[style]->GetAttDate()->SetTextAngle(90);
    myStyle[style]->GetAttDate()->SetTextSize(0.01);
    myStyle[style]->SetDateY(1);
    myStyle[style]->SetDateX(1);
  }

  myStyle[1]->cd();

  gROOT->ForceStyle();

  //if (myStyle[0]) delete myStyle[0];
  //if (myStyle[1]) delete myStyle[1];
}
