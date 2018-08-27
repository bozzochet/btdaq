#include "ViewerCore.h"

ViewerCore::ViewerCore() {
	fCalo3dviewer=NULL;
	fCaloams2dviewer=NULL;
	fC2D=NULL;
	fC3D=NULL;
	fCTracker=NULL;
	fPrevEvent=-1;
	fNeedToDelete=false;
}

ViewerCore &ViewerCore::GetIstance() {
  static ViewerCore instance;
  return instance;
}

void ViewerCore::SetOption(int VCONF,int eventStart,bool adc_mip, bool batch) {
	fEventStart=eventStart;
	fPrevEvent=fEventStart-1;
	fAdc_mip=adc_mip;
	fBatch=batch;
	fVCONF=VCONF;
}
// return 1, both files, 2 CC file, 3 AMS file, 0 no files.
int ViewerCore::OpenRootTrees(string inputCCFileName,string inputAMSFileName) {

	int status=1;
	if (inputCCFileName!="") {
		DataFileHandler::GetIstance().ReadCCFile(inputCCFileName,fAdc_mip);
	} else {
		cout << "No CC file is present, using AMS only." << endl;
		if (status==1) status=3;
		else return 0;
	}
	if (inputAMSFileName!="") {
		DataFileHandler::GetIstance().ReadAMSFile(inputAMSFileName);
	} else {
		cout << "No AMS file is present, using CC only."<< endl;
		if (status==1) status=2;
		else return 0;
	}
	return status;
}

void ViewerCore::CreateCanvas() {
	// Set TCanvas,
	if (fVCONF==1 || fVCONF==3) {
		fC3D	= new TCanvas("c3D","3D event display",600,700);
		fCVect.push_back(fC3D);
	}
	// 2D viewer
	if (fVCONF==1 || fVCONF==2){
		fC2D	= new TCanvas("c2D","2D event display",600,700);
		fCVect.push_back(fC2D);
		fCTracker = new TCanvas("cTracker","Tracker ev-by-ev plots",600,700);
		fCVect.push_back(fCTracker);
	}
}

void ViewerCore::SetCanvasEvent(TCanvas *c3D,TCanvas *c2D) {
	fC3D=c3D;
	fC2D=c2D;
}

void ViewerCore::InizializeViewers() {

	// Calocube 3D viwer
	if (fVCONF==1 || fVCONF==3) {
		fCalo3dviewer = new Calo3DViewer(fAdc_mip);
		fCalo3dviewer->Inizialize3Dviewer(fC3D);
	}
	// 2D viewer
	if (fVCONF==1 || fVCONF==2){
		fCaloams2dviewer = new CaloAMS2DViewer(fAdc_mip);
		fCaloams2dviewer->Inizialize2Dviewer(fC2D,fCTracker);
	}
}

short ViewerCore::DrawEvent(int iEv) {

  if (iEv>DataFileHandler::GetIstance().GetEntries()) {
    cerr << "In ViewerCore::DrowEvent, event > GetEntry, exit" << endl;
    return 0;
  }
  if (fVCONF==1 || fVCONF==3) fC3D->Update();
  if (fVCONF==1 || fVCONF==2) fC2D->Update();
  fPrevEvent=iEv;
  
  cout << " ======================================== Ev "<<iEv<<endl;
  DataFileHandler::GetIstance().GetEntry(iEv);
  
  //==================================
  // selection
  //==================================
  if (!DataFileHandler::GetIstance().Selection()) return 2;
  
  //==================================
  // draw event
  //==================================
  cout << "-------------- EVENT " << iEv << "---------------------" << endl;
  if ((fVCONF==1 || fVCONF==3) && DataFileHandler::GetIstance().IsCCPresent()) fCalo3dviewer->Fill3Dviewer(fC3D,DataFileHandler::GetIstance().fNHits_l,
		        DataFileHandler::GetIstance().fNHits_s,DataFileHandler::GetIstance().fHitSignal_l,DataFileHandler::GetIstance().fHitSignal_s,
			DataFileHandler::GetIstance().fHitRow_l,DataFileHandler::GetIstance().fHitRow_s,DataFileHandler::GetIstance().fHitCol_l,
			DataFileHandler::GetIstance().fHitCol_s,DataFileHandler::GetIstance().fHitLayer_l,DataFileHandler::GetIstance().fHitLayer_s);
  if ((fVCONF==1 || fVCONF==2) && DataFileHandler::GetIstance().IsCCPresent()) fCaloams2dviewer->Fill2DCCviewer(fC2D,DataFileHandler::GetIstance().fNHits_l,
			DataFileHandler::GetIstance().fHitSignal_l,DataFileHandler::GetIstance().fHitRow_l,DataFileHandler::GetIstance().fHitCol_l,
			DataFileHandler::GetIstance().fHitLayer_l);
  if ((fVCONF==1 || fVCONF==2) && DataFileHandler::GetIstance().IsAMSPresent()) fCaloams2dviewer->Fill2DAMSviewer(fC2D,DataFileHandler::GetIstance().fAmsev);
  if ((fVCONF==1 || fVCONF==2) && DataFileHandler::GetIstance().IsAMSPresent()) fCaloams2dviewer->Fill2DDAMPEviewer(fC2D,fCTracker,DataFileHandler::GetIstance().fAmsev);
  fNeedToDelete=true;
  return 1;
}

short ViewerCore::DrawNextEvent() {
	fPrevEvent++;
	return DrawEvent(fPrevEvent);
}

bool ViewerCore::DrawAMSOccupancy() {
	if (!DataFileHandler::GetIstance().IsAMSPresent()) {
		cerr << "In ViewerCore::DrawAMSOccupancy, AMS is not present" << endl;
		return false;
	}

	fCaloams2dviewer->CreateAMSOccupancy(fCAMS);
	int amsNev = DataFileHandler::GetIstance().GetAMSEntries();
	cout << endl << "DrawAMSOccupancy N event: " << amsNev << endl;
	for (int iEv=0; iEv<amsNev; iEv++) {
		if (iEv%1000 == 0) {
			cout << "Processing event: " << iEv << "\xd";
		}
		DataFileHandler::GetIstance().GetAMSEntry(iEv);
		fCaloams2dviewer->FillAMSOccupancy(DataFileHandler::GetIstance().fAmsev);
	}
	cout << endl;
	fCaloams2dviewer->DrawAMSOccupancy(fCAMS);
	return true;
}

bool ViewerCore::DrawDAMPEOccupancy() {
	if (!DataFileHandler::GetIstance().IsAMSPresent()) {
		cerr << "In ViewerCore::DrawDAMPEOccupancy, DAMPE is not present" << endl;
		return false;
	}
	fCaloams2dviewer->CreateDAMPEOccupancy(fCDAMPE);
	int amsNev = DataFileHandler::GetIstance().GetAMSEntries();
	cout << endl << "DrawDAMPEOccupancy N event: " << amsNev << endl;
	for (int iEv=0; iEv<amsNev; iEv++) {
		if (iEv%1000 == 0) {
			cout << "Processing event: " << iEv << "\xd";
			cout.flush();
		}
		DataFileHandler::GetIstance().GetAMSEntry(iEv);
		fCaloams2dviewer->FillDAMPEOccupancy(DataFileHandler::GetIstance().fAmsev);
	}
	cout << endl;
	fCaloams2dviewer->DrawDAMPEOccupancy(fCDAMPE);
	return true;
}


void ViewerCore::DeleteEvent() {
  cout << "DELETE" << endl;
	if ((fVCONF==1 || fVCONF==3) && DataFileHandler::GetIstance().IsCCPresent()) fCalo3dviewer->DeleteImage(DataFileHandler::GetIstance().fNHits_l,
			DataFileHandler::GetIstance().fNHits_s,DataFileHandler::GetIstance().fHitSignal_l,DataFileHandler::GetIstance().fHitSignal_s,
			DataFileHandler::GetIstance().fHitRow_l,DataFileHandler::GetIstance().fHitRow_s,DataFileHandler::GetIstance().fHitCol_l,
			DataFileHandler::GetIstance().fHitCol_s,DataFileHandler::GetIstance().fHitLayer_l,DataFileHandler::GetIstance().fHitLayer_s);
	if ((fVCONF==1 || fVCONF==2)) fCaloams2dviewer->DeleteImage();
	fNeedToDelete=false;
}

void ViewerCore::PrintEvent() {
	if (fVCONF==1 || fVCONF==2)fC2D->Print(Form("2D Event-%i.pdf",fPrevEvent));
	if (fVCONF==1 || fVCONF==3)fC3D->Print(Form("3D Event-%i.pdf",fPrevEvent));
}

ViewerCore::~ViewerCore() {

}


