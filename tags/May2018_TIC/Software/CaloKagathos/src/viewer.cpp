#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

#include <TApplication.h>
#include <TFile.h>
#include <TH1F.h>
#include <TH2F.h>
#include <TF1.h>
#include <TGraphErrors.h>
#include <TROOT.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TLegend.h>
#include <TPolyLine.h>
#include <TGaxis.h>

#include "constants.h"
#include "functions.h"
#include "structures.h"

using namespace std;

const int NROW = 5;
const int NCOL = 5;
const int NLAY = 18;

const double MIP2MeV = 20.178; //30.995 MeV energy release by 50 GeV muons 
			       //ignoring losses different from ionization (bremmstrahlung, pair production and photonuclear)
			       //and delta-ray production

//command line variables
const char *inputFileName = "";
int eventStart = 0;
bool adc_mip=false;
bool batch=false;

void HandleInputPar(int argc, char **argv)
{
	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "CALOCUBE OPTIONS" << endl;
	usage << "-i\t\t\tname of the input file" << endl;
	usage << "-r\t\t\tstart event number" << endl;
	usage << "-a\t\t\tview energy deposit in adc instead of mip" << endl;
	usage << "-b\t\t\tset batch (disable interactive)" << endl;
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while ((c = getopt(argc, argv, "i:r:abh")) != -1){
		switch (c) {
		case 'i':
			inputFileName = optarg;
			break;
		case 'r':
			eventStart = atoi(optarg);
			break;
		case 'a':
			adc_mip = true;
			break;
		case 'b':
			batch = true;
			break;
		case 'h':
			cout << usage.str().c_str() << endl;
			exit(0);
			break;
		default:
			cout << usage.str().c_str() << endl;
			exit(0);
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv){

	HandleInputPar(argc, argv);
	gROOT->Reset();

	int tmp=1;
	TApplication theApp("App",&tmp,argv);

	if(access(inputFileName, F_OK ) == -1)
	{
		cout << "No existing input file: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	TFile *inputFile = new TFile(inputFileName, "READ");
	TTree *tree = (TTree *) inputFile->Get("data");

	if(!tree)
	{
		cout << "No existing tree file: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	EventInfo eventInfo;
	Int_t casisTime;
	Float_t tCorr;
	tree->SetBranchAddress("eventInfo", &(eventInfo.trigger));
	tree->SetBranchAddress("eventFlag", &(eventInfo.good));
	tree->SetBranchAddress("casisTime", &casisTime);
	tree->SetBranchAddress("tCorr", &tCorr);

	Int_t nHits_l;
	Float_t hitSignal_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitRow_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitCol_l[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitLayer_l[NCASISCHIPS*NCASISCHANNELS];
	if(adc_mip) {
		tree->SetBranchAddress("LADC.nHits",&nHits_l);
		tree->SetBranchAddress("LADC.hitSignal",&hitSignal_l);
		tree->SetBranchAddress("LADC.hitRow",&hitRow_l);
		tree->SetBranchAddress("LADC.hitCol",&hitCol_l);
		tree->SetBranchAddress("LADC.hitLayer",&hitLayer_l);
	} else {
		tree->SetBranchAddress("LMIP.nHits",&nHits_l);
		tree->SetBranchAddress("LMIP.hitSignal",&hitSignal_l);
		tree->SetBranchAddress("LMIP.hitRow",&hitRow_l);
		tree->SetBranchAddress("LMIP.hitCol",&hitCol_l);
		tree->SetBranchAddress("LMIP.hitLayer",&hitLayer_l);
	}

	Int_t   nHits_s;
	Float_t hitSignal_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitRow_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitCol_s[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitLayer_s[NCASISCHIPS*NCASISCHANNELS];
	if(adc_mip) {
		tree->SetBranchAddress("SADC.nHits",&nHits_s);
		tree->SetBranchAddress("SADC.hitSignal",&hitSignal_s);
		tree->SetBranchAddress("SADC.hitRow",&hitRow_s);
		tree->SetBranchAddress("SADC.hitCol",&hitCol_s);
		tree->SetBranchAddress("SADC.hitLayer",&hitLayer_s);
	} else {
		tree->SetBranchAddress("SMIP.nHits",&nHits_s);
		tree->SetBranchAddress("SMIP.hitSignal",&hitSignal_s);
		tree->SetBranchAddress("SMIP.hitRow",&hitRow_s);
		tree->SetBranchAddress("SMIP.hitCol",&hitCol_s);
		tree->SetBranchAddress("SMIP.hitLayer",&hitLayer_s);
	}

	//========================================================
	// set cube image parameters
	//========================================================

	const float size = SIZE;//3.9;//cm
	const float angle = 0.8;//0.785;//rad
	const float gap = 1.;//cm
	float x[5],y[5];
	float x0[5],y0[5];
	x0[0]=0.;
	y0[0]=0.;
	x0[1]=x0[0]+size;
	y0[1]=y0[0];
	x0[2]=x0[1]-size*cos(angle);
	y0[2]=y0[1]-size*sin(angle);
	x0[3]=x0[2]-size;
	y0[3]=y0[2];
	x0[4]=x0[0];
	y0[4]=y0[0];

	//========================================================
	// set canvas dimensions
	//========================================================
	const float blabla = 10;//cm // title
	const float pal = 10;//cm // palette
	const float margine = 2.;

	TCanvas *cc = new TCanvas("cc","Event display",800,900);
	cc->Divide(2,1);
	cc->cd(1);

	float xCanvasLow = -size*NROW*cos(angle)-0.5;
	float yCanvasLow = -1.*(NROW*size*sin(angle)+gap)*(NLAY+1)-0.5 - pal - blabla;
	float xCanvasHigh = size*NCOL+0.5;
	float yCanvasHigh = 0.+0.5;
	cc->GetPad(1)->Range(xCanvasLow-margine,
			yCanvasLow-margine,
			xCanvasHigh+margine,
			yCanvasHigh+margine);
	cc->GetPad(2)->Range(xCanvasLow-margine,
			yCanvasLow-margine,
			xCanvasHigh+margine,
			yCanvasHigh+margine);

	//========================================================
	// initialize cube images
	//========================================================
	TPolyLine *cubeSignal_l[NCOL][NROW][NLAY];
	TPolyLine *cubeSignal_s[NCOL][NROW][NLAY];
	TPolyLine *cubeFrame[NCOL][NROW][NLAY];
	TBox* front = new TBox();
	for(int ic=0;ic<NCOL;ic++){
		for(int ir=0;ir<NROW;ir++){
			for(int il=0;il<NLAY;il++){
				// traslazione
				float dx = ic*size-ir*size*cos(angle);
				float dy = -1*ir*size*sin(angle)-il*(NROW*size*sin(angle)+gap);
				for(int i=0; i<5; i++)x[i] = x0[i] + dx;
				for(int i=0; i<5; i++)y[i] = y0[i] + dy;
				cubeSignal_l[ic][ir][il] = NULL;
				cubeSignal_s[ic][ir][il] = NULL;
				cubeFrame[ic][ir][il] = new TPolyLine(5, x, y);
				cubeFrame[ic][ir][il]->SetLineColor(kGray);
				cubeFrame[ic][ir][il]->SetFillColor(kGray);
				cubeFrame[ic][ir][il]->SetLineWidth(2);
				cc->cd(1); cubeFrame[ic][ir][il]->Draw();
				cc->cd(2); cubeFrame[ic][ir][il]->Draw();
				if(ic==0&&ir==NROW-1&&il==NLAY-1){
					front->SetX1(x0[3]+dx);
					front->SetY1(y0[3]+dy-(NROW*size*sin(angle)));
				}else if(ic==NCOL-1&&ir==NROW-1&&il==0){
					front->SetX2(x0[2]+dx);
					front->SetY2(y0[2]+dy);
				}
				//
			}
		}
	}
	front->SetLineStyle(2);
	front->SetLineColor(kBlack);
	front->SetFillStyle(0);
	cc->cd(1);   front->Draw();
	cc->cd(2);   front->Draw();

	const Int_t NCOLOR = 6;
	Color_t color[] = {
			kCyan-7
			,kSpring-9//kGreen
			,kYellow
			,kOrange+8//kRed
			,kPink
			,kViolet//kMagenta
	};
	float log10_paletteMin = adc_mip? 1 : -1.; //min signal
	float log10_paletteMax = adc_mip? 7. : 4.; //max signal
	TBox *palette[NCOLOR];
	float dx = (xCanvasHigh-xCanvasLow)/NCOLOR;
	float xl = xCanvasLow;
	float yl = yCanvasLow + blabla;
	float xh = xCanvasLow + dx;
	float yh = yCanvasLow + blabla + 0.5*pal;

	for(int ic=0; ic<NCOLOR; ic++ ){
		palette[ic] = new TBox(xl+ic*dx,yl,xh+ic*dx,yh);
		palette[ic]->SetFillColor(color[ic]);
		cc->cd(1); palette[ic]->Draw();
		cc->cd(2); palette[ic]->Draw();
	}

	//========================================================
	// create energy axis
	//========================================================
	TGaxis *axis = new TGaxis(xl, yl,
			xl+dx*NCOLOR, yl,
			pow(10.,log10_paletteMin),pow(10.,log10_paletteMax),
			NCOLOR,"G");
	axis->SetLabelOffset(-0.05);
	axis->SetLabelSize(.03);
	axis->SetTitleSize(.03);
	axis->SetTitleOffset(0.3);
	axis->SetTitle(Form("Energy deposit [%s]", adc_mip? "ADC" : "MIP"));
	cc->cd(1); axis->Draw();
	cc->cd(2); axis->Draw();

	////////////////////////////////////////////////////////
	// start loop over events
	////////////////////////////////////////////////////////

	cout << " N.Events "<<tree->GetEntries()<<endl;

	for(int iev=0; iev<tree->GetEntries(); iev++){

		if(iev<eventStart)
			continue;

		cc->Update();

		cout << " ======================================== Ev "<<iev<<endl;
		tree->GetEntry(iev);

		//==================================
		// selection
		//==================================
		bool GOOD = true;

		if(eventInfo.trigger!=PHYSICSTRIGGER) GOOD = false;
		if(eventInfo.good!=GOODEVENTFLAG) GOOD = false;

		//Event selection
		if(nHits_l<15) GOOD = false;
		//if(nHits_l<50) GOOD = false;
		if(!GOOD)continue;

		//==================================
		// draw event
		//==================================

		double sumde_l=0.;
		double sumde_s=0.;

		cc->cd(1);
		for(int ih=0; ih<nHits_l; ih++){
			sumde_l+=hitSignal_l[ih];
			if(hitSignal_l[ih]<=0)
				cout << ih << " " << hitCol_l[ih]<<" "<<hitRow_l[ih]<<" "<< hitLayer_l[ih]<<" !!!!!!!!!!!!!!!!!!!  "<<hitSignal_l[ih]<<endl;
			hitSignal_l[ih] = TMath::Max((Float_t)0.1,hitSignal_l[ih]);

			// create image
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]] = new TPolyLine();
			cubeFrame[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Copy(*cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]);
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetFillColor(kWhite);
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetLineColor(kGray+2);
			int colorIndex = (int) (NCOLOR*(log10(hitSignal_l[ih])-log10_paletteMin)/(log10_paletteMax-log10_paletteMin) );
			Color_t thisColor = kBlack;
			if(colorIndex<0)thisColor = kWhite;
			if(colorIndex>=0 && colorIndex<NCOLOR)thisColor = color[colorIndex];
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetFillColor(thisColor);
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Draw("f");
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Draw("");
		};
		front->Draw();

		cc->cd(2);
		for(int ih=0; ih<nHits_s; ih++){
			sumde_s+=hitSignal_s[ih];
			//if(hitSignal_s[ih]<=0)
			//	cout << ih << " " << hitCol_s[ih]<<" "<<hitRow_s[ih]<<" "<< hitLayer_s[ih]<<" !!!!!!!!!!!!!!!!!!!  "<<hitSignal_s[ih]<<endl;
			hitSignal_s[ih] = TMath::Max((Float_t)0.1,hitSignal_s[ih]);

			// create image
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]] = new TPolyLine();
			cubeFrame[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Copy(*cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]);
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetFillColor(kWhite);
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetLineColor(kGray+2);
			int colorIndex = (int) (NCOLOR*(log10(hitSignal_s[ih])-log10_paletteMin)/(log10_paletteMax-log10_paletteMin) );
			Color_t thisColor = kBlack;
			if(colorIndex<0)thisColor = kWhite;
			if(colorIndex>=0 && colorIndex<NCOLOR)thisColor = color[colorIndex];
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetFillColor(thisColor);
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Draw("f");
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Draw("");
		};
		front->Draw();

		cc->Update();
	
		cout << "Large "<< endl;
		cout << "\tnHits = " << nHits_l << endl;
		cout << "\tEnergy = " << sumde_l << " MIP " << (adc_mip? "" : Form("(%f MeV)", sumde_l*MIP2MeV)) << endl;
		cout << "Small "<< endl;
		cout << "\tnHits = " << nHits_s << endl;
		cout << "\tEnergy = " << sumde_s << " MIP " << (adc_mip? "" : Form("(%f MeV)", sumde_s*MIP2MeV)) << endl;

		//==================================
		// actions
		//==================================
		if(!batch){
			char what_to_do;
			cout << "[q]uit [p]rint [any-key]next" << endl;
			cin >> what_to_do;
			if(what_to_do == 'q')break;
			if(what_to_do == 'p'){
				cout << "(( PRINTING EVENT ))"<<endl;
				cc->Print(Form("Event-%i.png",iev));
				cc->Print(Form("Event-%i.eps",iev));
			}
		}else{
		}

		//==================================
		// delete image
		//==================================

		for(int ih=0; ih<nHits_l; ih++){
			cubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Delete();
		}
		for(int ih=0; ih<nHits_s; ih++){
			cubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Delete();
		}

	}

	cc->Close();

	return 0;
}
