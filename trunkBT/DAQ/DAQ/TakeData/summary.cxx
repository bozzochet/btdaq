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

#define NJINF 24 //maximum number of JINF

void LinesVas();
void InitStyle();

using namespace std;

class ConfPars{

public:
  char CALPATH[512];
  unsigned int refmask;
  ConfPars();
};

ConfPars::ConfPars() {
	sprintf(CALPATH,"./Calibrations");
	refmask=0;
}

ConfPars* CPars;

bool mute=false;

void ShowHelp(char *cmd);
void RefMask(ConfPars *CPars, int run_number, int jinfnum, const char *nameprefixin);
Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, char *nameout, const char *outkind);
Int_t Summary(char *filename, char *nameout, const char *outkind);
int SummaryComplete(char *dir, int run_number, int jinfnum, const char *outkind, const char *nameprefix);

//--------------------------------------------------------------------------------------------
//                              Here comes the main...
//-------------------------------------------------------------------------------------------
int main(int argc, char **argv) {
  
  int ret=0;

  TString _name = argv[0];
  if (_name.Contains("MUTE")) mute=true;

  CPars= new ConfPars();
  //-------------------------------------------------
  //              COMMANDS
  //-------------------------------------------------
  if (argc<2) {
    ret=1;
    printf("Too few arguments: %s <run #> [type of file](everything for pdf, \"ps\" for PostScript)\n", argv[0]);
    printf("                              [name prefix](default is \"\")\n");
    printf("                              [dir](default is \"./Calibrations\")\n");
  }
  else if (argc==2) {
    for (int ii=0;ii<NJINF;ii++) {
      RefMask(CPars, atoi(argv[1]), ii, "");
      ret=SummaryComplete(CPars->CALPATH, atoi(argv[1]), ii, "pdf", "");
    }
  }
  if (argc>2) {
    char type[512];
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

  delete CPars;
  return 0;
}

void RefMask(ConfPars *CPars, int run_number, int jinfnum, const char *nameprefixin) {
  CPars->refmask=0; 
  char calfileprefix[850];
  char prefixtemp[255];
  char prefix[300];
  sprintf(prefixtemp, "%s", nameprefixin);
  if (prefixtemp[0]!=0) sprintf(prefix, "%s_", prefixtemp);
  else sprintf(prefix, "%s", prefixtemp);
  sprintf(calfileprefix,"%s/%s%06d", CPars->CALPATH, prefix, run_number);
  
  char calfilename[1024];
  
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

	char calfileprefix[600];
	char nameouttemp[750];
	char nameout[700];
	char nameprefixtemp[512];
	char systemcommand[750];

	if (nameprefix[0]!=0) sprintf(nameprefixtemp,"summary-%s_",nameprefix);
	else sprintf(nameprefixtemp,"summary-%s",nameprefix);	
	sprintf(calfileprefix,"%s/%s%02d_%06d", dir, nameprefixtemp, jinfnum, run_number);
	printf("%s\n", calfileprefix);
	sprintf(nameout,"%s.cal.%s", calfileprefix, outkind);

	sprintf(systemcommand,"evince %s &",nameout);

	for (int ii=0;ii<24;ii++){
		//    LPRINTF("%d : %d\n", ii, CPars->refmask&(1<<ii)); //only for debug
		if(CPars->refmask&(1<<ii)) {
			tdrcount++;
		}
	}
	printf("We have %d file of calibrations for run #%d, Jinf #%d, in the directory \"%s\" with prefix name \"%s\"\n",tdrcount, run_number, jinfnum, dir, nameprefix);

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
	struct stat buff;
	if (!mute) {
	  if (stat(nameout,&buff)==0) system(systemcommand);
	}
	return ret;
}

Int_t Summary(char *dir, int run_number, int jinfnum, int tdr_number, const char *nameprefixin, char *nameout, const char *outkind) {
	int ret=0;

	char calfileprefix[700];
	Char_t filename[1024];
	char nameprefixintemp[512];

	if (nameprefixin[0]!=0) sprintf(nameprefixintemp,"%s_",nameprefixin);
	else sprintf(nameprefixintemp,"%s",nameprefixin);
	sprintf(calfileprefix,"%s/%s%06d", dir, nameprefixintemp, run_number);
	sprintf(filename,"%s_%02d%02d.cal", calfileprefix,jinfnum, tdr_number);
	ret=Summary(filename, nameout, outkind);
	
	return ret;
}

Int_t Summary(char *filename, char *nameout, const char *outkind) {

	InitStyle();

	gROOT->SetStyle("StyleWhite");
	//  gStyle->SetPaperSize(27,20);

	Double_t dum;
	Double_t CN[16];
	Int_t va[1024];//red but not used
	Int_t chofva[1024];//red but not used
	Int_t ch[1024];//red but not used
	Double_t ped[1024];
	float rsig[1024];
	float sig[1024];
	int status[1024];
	float dumf;
	TCanvas *c;

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

	// reading the common noise
	for (Int_t i=0; i<16; i++) {
		fscanf(calfil,"%lf,%lf,%lf",&dum,&dum,&CN[i]);
		//		printf("ho letto: %lf,%lf,%lf\n",dum,dum,CN[i]);
	}

	// reading channels
	for (Int_t ii=0;ii<1024;ii++) {
		fscanf(calfil,"%d %d %d %lf %f %f %f %d",&ch[ii],&va[ii],&chofva[ii],&ped[ii],&rsig[ii],&sig[ii],&dumf,&status[ii]);
		//		printf("Ho letto: %d %d %d %lf %f %f %f %d\n",ch[ii],va[ii],chofva[ii],ped[ii],rsig[ii],sig[ii],dumf,status[ii]);
	}

	fclose(calfil);

	c=new TCanvas(Form("c%d",0),Form("%s_%d",filename,0),0*100,0*10,600,400);
	comment->Draw();
	c->Update();
	c->Divide(2,2);
	c->SetFillStyle(0);
	c->cd(1);
	gPad->SetFillStyle(0);

	c->cd(1);
	TH2F *fram=new TH2F("fram",Form("%s: pedestals",filename),1024,1,1024,2000,0,2000);
	fram->SetStats(0);
	fram->Draw();
	TH1F *isto=new TH1F("isto",Form("%s: pedestals",filename),1024,1,1024);
	for (Int_t ii=0;ii<1024;ii++) isto->SetBinContent(ii,ped[ii]);
	isto->Draw("same");
	gPad->Update();
	LinesVas();
	gPad->Update();	

	c->cd(2);
	gPad->SetFillStyle(0);
	TH2F *fram2=new TH2F("fram2",Form("%s: sigma",filename),1024,1,1024,31,0,30);
	fram2->SetStats(0);
	fram2->Draw();
	TH1F *isto2=new TH1F("isto2",Form("%s: sigma",filename),1024,1,1024);  
	for (int ii=0;ii<1024;ii++) isto2->SetBinContent(ii,sig[ii]);
	isto2->Draw("same");
	gPad->Update();
	LinesVas();
	gPad->Update();

	c->cd(3);
	gPad->SetFillStyle(0);
	TH2F *fram3=new TH2F("fram3",Form("%s: sigma raw",filename),1024,1,1024,51,0,50);
	fram3->SetStats(0);
	fram3->Draw();
	TH1F *isto3=new TH1F("isto3",Form("%s: sigma raw",filename),1024,1,1024);
	for (int ii=0;ii<1024;ii++) isto3->SetBinContent(ii,rsig[ii]);
	isto3->Draw("same");
	gPad->Update();
	LinesVas();
	gPad->Update();

	c->cd(4);
	gPad->SetFillStyle(0);
	TH2F *fram4=new TH2F("fram4",Form("%s: strip status",filename),1024,1,1024,11,0,10);
	fram4->SetStats(0);
	fram4->Draw();
	TH1F *isto4=new TH1F("isto4",Form("%s: strip status",filename),1024,1,1024);	
	// int chip=0;
	// int channelofchip=0;
	for (int jj=0;jj<1024;jj++) {
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
	LinesVas();
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

	return 0;
}

void LinesVas() {
	Double_t xmin,ymin,xmax,ymax;
	gPad->GetRangeAxis(xmin,ymin,xmax,ymax);
	TLine *line=new TLine();
	line->SetLineColor(kBlue);
	TText *text=new TText();
	text->SetTextAlign(21); // vert and hor. alignment: horiz centered, top
	text->SetTextColor(kBlue);
	//line->SetLineStyle(3);
	for (Int_t va=1; va<17; va++) {// fisrt and last sep. lines are not drawn
		line->SetLineStyle((va==10)?1:3);
		if (va==11) { line->SetLineColor(kRed); text->SetTextColor(kRed); }
		if (va*64>xmin && va*64<xmax) line->DrawLine(1+va*64,ymin,1+va*64,ymax);
		if (va*64>xmin && va*64<=xmax) text->DrawText((va-0.5)*64,ymax+(ymax-ymin)*0.01,Form("%d",va));
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
