#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

#include <TROOT.h>
#include <TApplication.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TFile.h>
#include <TTree.h>
#include <TChain.h>
#include <TF1.h>
#include <TH1.h>
#include <TH2.h>
#include <TGraphErrors.h>
#include <THStack.h>
#include <TPaveStats.h>
#include <TGaxis.h>
#include <TGraph.h>
#include <TMath.h>
#include <TFitResult.h>
#include <TVirtualFitter.h>

#include "constants.h"

using namespace std;

const double mpv_shift  = 0.;       // This is the maximum of the convolution of Landau and Gaussian
//const double mpv_shift  = -0.22278298;       // This is the maximum of the Landau distribution only

const char *inputFileName ="";
bool fit_distributions=false;

void HandleInputPar(int argc, char **argv)
{
	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "-i\t\t\tname of input file" << endl;
	usage << "-f\t\t\tfit distributions" << endl;
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while ((c = getopt(argc, argv, "i:fh")) != -1){
		switch (c) {
		case 'i':
			inputFileName = optarg;
			break;
		case 'f':
			fit_distributions = true;
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

Double_t langaufun(Double_t *x, Double_t *par) {

	//Fit parameters:
	//par[0]=Width (scale) parameter of Landau density
	//par[1]=Most Probable (MP, location) parameter of Landau density
	//par[2]=Total area (integral -inf to inf, normalization constant)
	//par[3]=Width (sigma) of convoluted Gaussian function
	//
	//In the Landau distribution (represented by the CERNLIB approximation),
	//the maximum is located at x=-0.22278298 with the location parameter=0.
	//This shift is corrected within this function, so that the actual
	//maximum is identical to the MP parameter.

	// Numeric constants
	Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
	Double_t mpshift  = -0.22278298;       // Landau maximum location

	// Control constants
	Double_t np = 500.0;      // number of convolution steps
	Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

	// Variables
	Double_t xx;
	Double_t mpc;
	Double_t fland;
	Double_t sum = 0.0;
	Double_t xlow,xupp;
	Double_t step;
	Double_t i;

	// MP shift correction
	mpc = par[1] - mpshift * par[0];

	// Range of convolution integral
	xlow = x[0] - sc * par[3];
	xupp = x[0] + sc * par[3];

	step = (xupp-xlow) / (2.*np+1);

	// Convolution integral of Landau and Gaussian by sum
	fland = TMath::Landau(x[0],mpc,par[0]); // /par[0];
	sum += fland * TMath::Gaus(x[0],x[0],par[3]);

	for(i=1.0; i<=np; i++) {
		xx = xlow + (i-.5) * step;
		fland = TMath::Landau(xx,mpc,par[0]); // / par[0];
		sum += fland * TMath::Gaus(x[0],xx,par[3]);

		xx = xupp - (i-.5) * step;
		fland = TMath::Landau(xx,mpc,par[0]); // / par[0];
		sum += fland * TMath::Gaus(x[0],xx,par[3]);
	}

	return (par[0]*par[2] * step * sum * invsq2pi/ (par[3]) );
}

int main(int argc, char **argv) {

	int tmp=1;
	TApplication theApp("App", &tmp, argv);
	HandleInputPar(argc, argv);

	if(access(inputFileName, F_OK ) == -1)
	{
		cout << "No existing input file: Exit..." << endl;
		exit(EXIT_FAILURE);
	}

	gStyle->SetOptStat("emr");
	gStyle->SetOptFit();
	TGaxis::SetMaxDigits(3);

	//Open input file and get histograms
	TFile *file = new TFile(inputFileName, "READ");
	TH1F *henergy[NSENSORS][NLAYERS][NCUBES];

	for(int isensor=0; isensor<NSENSORS; ++isensor)
		for(int ilayer=0; ilayer<NLAYERS; ++ilayer)
			for(int icube=0; icube<NCUBES; ++icube) {
				char name[256];
				sprintf(name, "customCube_%s_%d_%d", isensor==0? "Large" : "Small", ilayer, icube);
				henergy[isensor][ilayer][icube] = (TH1F *) file->Get(name);
				//Look for a TH1F object  with the given name in input file creating a pointer to it
			}

	//Optional : fit distributions with a landau convoluted with a gaussian
	if(fit_distributions) {
		TF1 *func1[NLAYERS][NCUBES];
		TF1 *func2[NLAYERS][NCUBES];

		ofstream ofs("result.dat", ofstream::trunc);
		for(int ilayer=0; ilayer<NLAYERS; ++ilayer)
			for(int icube=0; icube<NCUBES; ++icube) {
				if(ilayer!=0 || icube!=12)
					continue;

				cout << endl;
				cout << "******************************************************************" << endl;
				cout << "Layer " << ilayer << " Cube " << icube << " **********************" << endl;
				cout << "******************************************************************" << endl;

				//LOCATE MAXIMUM
				double range_min=0.5; //Aggiusta manualmente gli estremi per i vari ioni
				double range_max=1.5;
				henergy[0][ilayer][icube]->GetXaxis()->SetRangeUser(range_min, range_max);
				double maximum_bin = henergy[0][ilayer][icube]->GetMaximumBin();
				double maximum_point = henergy[0][ilayer][icube]->GetBinCenter(maximum_bin);
				double integral = henergy[0][ilayer][icube]->Integral(
						henergy[0][ilayer][icube]->FindBin(range_min), henergy[0][ilayer][icube]->FindBin(range_max));
				henergy[0][ilayer][icube]->GetXaxis()->UnZoom();

				cout << endl;
				cout << "Initial Parameters +++++++++++++++++" << endl;
				cout << "Integral = " << integral << endl;
				cout << "Peak position = " << maximum_point << endl;
				cout << "Range = [" << range_min << ", " << range_max << "]" << endl;
				cout << "++++++++++++++++++++++++++++++++++++" << endl;

				//GAUSS FIT
				cout << endl;
				cout << "Gaussian Fit ========================" << endl;
				cout << endl;
				Double_t xmin = range_min;
				Double_t xmax = range_max;
				func1[ilayer][icube] = new TF1(Form("func1_%d_%d", ilayer, icube),"gaus",0., 0.);
				func1[ilayer][icube]->SetParLimits(1,xmin, xmax);
				func1[ilayer][icube]->SetParLimits(1,maximum_point-25., maximum_point+100.);
				func1[ilayer][icube]->SetParameter(0,integral);
				func1[ilayer][icube]->SetParameter(1,maximum_point);
				func1[ilayer][icube]->SetParameter(2,maximum_point*0.2);
				henergy[0][ilayer][icube]->Fit(func1[ilayer][icube],"R","sames",xmin,xmax);
				double mean=func1[ilayer][icube]->GetParameter(1);
				double sigma=func1[ilayer][icube]->GetParameter(2);
				xmin = mean-sigma*1.;
				xmax = mean+sigma*2.;

				//LANGAUSS FIT
				double par[]={0.5*sigma,
						mean,
						integral*henergy[0][ilayer][icube]->GetBinWidth(1)/(TMath::Power(0.5*sigma,2)),
						0.5*sigma};
				func2[ilayer][icube] = new TF1(Form("FitFNC_%s",henergy[0][ilayer][icube]->GetName()),langaufun, 0., 0., 4);
				func2[ilayer][icube]->SetParNames("Width","MP","Area/Width^2","GSigma");
				func2[ilayer][icube]->SetParLimits(0,0.,range_min);
				func2[ilayer][icube]->SetParLimits(1,mean-100., mean+range_min);
				func2[ilayer][icube]->SetParLimits(3,0.,range_min);
				func2[ilayer][icube]->SetParameters(par);

				cout << endl;
				cout << "LanGau Fit 1 ========================" << endl;
				cout << endl;
				henergy[0][ilayer][icube]->Fit(func2[ilayer][icube],"R","SAMES",xmin,xmax);
				func2[ilayer][icube]->GetParameters(par);
				double fit1;
				double fit2;
				fit1 = TMath::Max(par[1]*0.7, range_min);
				fit2 = TMath::Min(par[1]*1.6, range_max);

				cout << endl;
				cout << "LanGau Fit 2 ========================" << endl;
				cout << endl;
				henergy[0][ilayer][icube]->Fit(func2[ilayer][icube],"R","sames",fit1,fit2);
				func2[ilayer][icube]->SetRange(fit1,fit2);
				func2[ilayer][icube]->GetParameters(par);
				double peak_location = func2[ilayer][icube]->GetParameter(1) - mpv_shift * func2[ilayer][icube]->GetParameter(0);
				double peak_error = func2[ilayer][icube]->GetParError(1) - mpv_shift * func2[ilayer][icube]->GetParError(0);

				//printf("%d\t%d\t%.3f\t\t%.3f\n", ilayer, icube, peak_location, peak_error);
				ofs << Form("%d\t%d\t%.3f\t\t%.3f\n", ilayer, icube, peak_location, peak_error);
			}
		ofs.close();
	}

	//Plot histogram
	TCanvas *cresult = new TCanvas("cresult", "", 600, 600);
	cresult->cd();
	for(int isensor=0; isensor<NSENSORS; ++isensor)
		for(int ilayer=0; ilayer<NLAYERS; ++ilayer)
			for(int icube=0; icube<NCUBES; ++icube) {
				if(ilayer!=0 || icube!=12)
					continue;

				henergy[isensor][ilayer][icube]->SetLineColor(kBlue);
				henergy[isensor][ilayer][icube]->SetLineStyle(1);

				henergy[isensor][ilayer][icube]->Draw();
				henergy[isensor][ilayer][icube]->SetTitle("Esempione");
				henergy[isensor][ilayer][icube]->GetXaxis()->SetRangeUser(0., 5.);

				henergy[isensor][ilayer][icube]->GetXaxis()->SetTitle("x");
				henergy[isensor][ilayer][icube]->GetXaxis()->SetTitleSize(0.05);
				henergy[isensor][ilayer][icube]->GetXaxis()->SetTitleOffset(1.05);

				henergy[isensor][ilayer][icube]->GetYaxis()->SetTitle("y");
				henergy[isensor][ilayer][icube]->GetYaxis()->SetTitleSize(0.05);
				henergy[isensor][ilayer][icube]->GetYaxis()->SetTitleOffset(1.05);

				henergy[isensor][ilayer][0]->SetStats(0); //Se vuoi rimuovere il pannello con le statistiche

				//Per fare un esempio stupido su come mettere diversi istogrammi nella stessa finestra
				double norm_curr = henergy[isensor][ilayer][icube-1]->Integral();
				double norm_refe = henergy[isensor][ilayer][icube]->Integral();
				henergy[isensor][ilayer][0]->Scale(norm_refe/norm_curr);
				//Moltiplica il contenuto di ogni bin per il fattore (norm_refe/norm_curr)
				//Questo ti servirà per rinormalizzare i cubi diversi dal 12 al numero di entries del 12
				//(che sarà molto maggiore dato che la statistica è molto magggiore=
				henergy[isensor][ilayer][0]->SetLineColor(kRed);
				henergy[isensor][ilayer][0]->SetLineStyle(1);
				henergy[isensor][ilayer][0]->Draw("SAME"); //Usa l'opzione SAME
				//Fine esempio scemo

				gPad->SetLogy();

				cresult->WaitPrimitive();
				cresult->Update();
				cresult->Print(Form("./img/energy_%s_%d_%d.png", isensor==0? "large" : "small", ilayer, icube));
				cresult->Print(Form("./img/energy_%s_%d_%d.eps", isensor==0? "large" : "small", ilayer, icube));
			}

	return 0;
}
