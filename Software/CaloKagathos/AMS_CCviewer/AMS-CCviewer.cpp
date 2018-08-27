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
#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <RQ_OBJECT.h>

#include "ViewerGUI.h"
#include "ViewerCore.h"

using namespace std;

//command line variables
string inputCCFileName = "";
string inputAMSFileName = "";
int eventStart = 0;
bool adc_mip=true; // true -> use ADC, false -> use MIP (small diodes are not well calibrated so far)
bool batch=false;
int VCONF=1;

void HandleInputPar(int argc, char **argv)
{
	stringstream usage;
	usage.clear();
	usage << endl;
	usage << "Usage:" << endl << endl;
	usage << "CALOCUBE OPTIONS" << endl;
	usage << "-C\t\t\tname of the input CC file" << endl;
	usage << "-A\t\t\tname of the input AMS file" << endl;
	usage << "-r\t\t\tstart event number" << endl;
	usage << "-a\t\t\tview energy deposit in adc instead of mip" << endl;
	usage << "-b\t\t\tset batch (disable interactive)" << endl;
	usage << "-V\t\t\tviewer configurations: 1-> draw all, 2->2D AMS-Calocube, 3->3D Calocube" << endl;
	usage << "-h\t\t\thelp" << endl << endl;

	int c;

	while ((c = getopt(argc, argv, "A:V:C:r:abh")) != -1){
		switch (c) {
		case 'V':
			VCONF = atoi(optarg);
			if (VCONF<1 || VCONF>3) {
				cerr << "viewer configurations is not allowed (1,2,3), exit" << endl;
				exit(EXIT_FAILURE);
			}
			break;
		case 'C':
			inputCCFileName = optarg;
			break;
		case 'A':
			inputAMSFileName = optarg;
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

	int tmp=1;
	TApplication theApp("App",&tmp,argv);

	gROOT->Reset();

	if (inputCCFileName=="" && inputAMSFileName=="") {
		cerr << "No imput files found, exit" << endl;
		exit(EXIT_FAILURE);
	}

	// Set option
	ViewerCore::GetIstance().SetOption(VCONF,eventStart,adc_mip,batch);
	// open files and trees
	int status = ViewerCore::GetIstance().OpenRootTrees(inputCCFileName,inputAMSFileName);
	if (status <1) {
		cerr << "Status of ViewerCore < 1, exit!" << endl;
		exit(EXIT_FAILURE);
	}
	// create canvas
	if (batch) {
		ViewerCore::GetIstance().CreateCanvas();
	}
	// main GUI class
	ViewerGUI* gui=NULL;
	if (!batch) {
		gui = new ViewerGUI(gClient->GetRoot(),600,600,VCONF);
		ViewerCore::GetIstance().SetCanvasEvent(gui->GetC3D(),gui->GetC2D());
		ViewerCore::GetIstance().SetCanvasAMS(gui->GetCAMS());
		ViewerCore::GetIstance().SetCanvasDAMPE(gui->GetCDAMPE());
		ViewerCore::GetIstance().SetCanvasTracker(gui->GetCTracker());
	}

	// Create viewer
	ViewerCore::GetIstance().InizializeViewers();

	if (batch) {
		for(int iev=eventStart; iev<DataFileHandler::GetIstance().GetEntries(); iev++){
			//break;
			if (ViewerCore::GetIstance().DrawNextEvent()==0) break;
			if (ViewerCore::GetIstance().DrawNextEvent()==2) continue;

			//==================================
			// actions (if batch)
			//==================================
			char what_to_do;
			cout << "[q]uit [p]rint [any-key]next" << endl;
			cin >> what_to_do;
			if(what_to_do == 'q')break;
			if(what_to_do == 'p'){
				ViewerCore::GetIstance().PrintEvent();
			}
			ViewerCore::GetIstance().DeleteEvent();
		}
	} else {
	   theApp.Run();
	}

	return 0;
}
