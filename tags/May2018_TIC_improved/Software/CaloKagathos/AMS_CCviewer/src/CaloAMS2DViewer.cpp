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

#include "CaloAMS2DViewer.h"

using namespace std;

#define DEBUG false
#define DAMPEHOLE false

double CaloAMS2DViewer::fCCLAYERZ[] = {1,7.6,12.2,17.6,23};
double CaloAMS2DViewer::fCCCUBEX[] = {-8,-4,0,4,8};
double CaloAMS2DViewer::fCCCUBEY[] = {8,4,0,-4,-8};
double CaloAMS2DViewer::fAMSLAYERZ[] = {-54, -50, -24,-20};
double CaloAMS2DViewer::fAMSALIGNX[] = {0, 0, 0, 0};
double CaloAMS2DViewer::fAMSALIGNY[] = {0, 0, 0, 0};
bool CaloAMS2DViewer::fAMSDIRECTIONX[] = {true,true,true,true};
bool CaloAMS2DViewer::fAMSDIRECTIONY[] = {false,false,true,false};
int CaloAMS2DViewer::fAMSTDRNUM[] = {23, 22, 21, 20}; // 10, 12, 13}; // one ladder per layer
double CaloAMS2DViewer::fDAMPELAYERZ[] = {2.95,5.75,9.55,15.3,20.7};
double CaloAMS2DViewer::fDAMPEALIGNX[] = {-6.75, 2.75, -6.75, 2.75, -6.75, 2.75, -6.75, 2.75, -6.75, 2.75}; // 2 ladder for each layer
bool CaloAMS2DViewer::fDAMPEDIRECTIONX[] = {true,true,false,false,true,true,true,true};
int CaloAMS2DViewer::fDAMPETDRNUM[] = {19,18,15,14,11,10,7,6,3,2};

CaloAMS2DViewer::CaloAMS2DViewer(bool adc_mip): fCCNROW(5),fCCNCOL(5),fCCNLAY(5),fCCMIP2MeV(20.178),fCCSIZEXY(3.6),
		fCCSIZEZ(2), fAMSPITCHX(0.0208), fAMSPITCHY(0.0110), fAMSNSTRIPX(384), fAMSNSTRIPY(640), fAMSNLAY(4), fAMSSIZEZ(2), fAMSREBIN(1),
		fAMSXSIDE(1),	fDAMPEPITCHX(0.0242), fDAMPENSTRIP(384), fDAMPENLAY(5), fDAMPESIZEZ(1), fDAMPEREBIN(1), fDAMPEXSIDE(0) {
	fAdc_mip = adc_mip;
	fCubeFrameX = vector< vector< TBox* > > (fCCNLAY,vector< TBox* >(fCCNCOL,NULL));
	fCubeFrameY = vector< vector< TBox* > > (fCCNLAY,vector< TBox* >(fCCNROW,NULL));
	fHAMSOccupancyVect.clear();
	fHDAMPEOccupancyVect.clear();
	fHDAMPEHitVect.clear();
}

// Inizialize 3D calocube viewer
void CaloAMS2DViewer::Inizialize2Dviewer(TCanvas *c2D, TCanvas *cTracker) {

	// set tbox XZ
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNCOL; iBox++) {
			fCubeFrameX[iLayer][iBox] = new TBox(fCCCUBEX[iBox]-fCCSIZEXY/2.,fCCLAYERZ[iLayer]-fCCSIZEZ/2.,
					fCCCUBEX[iBox]+fCCSIZEXY/2.,fCCLAYERZ[iLayer]+fCCSIZEZ/2.);
			fCubeFrameX[iLayer][iBox]->SetFillStyle(0);
			fCubeFrameX[iLayer][iBox]->SetLineColor(kBlack);
			fCubeFrameX[iLayer][iBox]->SetLineWidth(2);
		}
	}
	// set tbox YZ
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNROW; iBox++) {
			fCubeFrameY[iLayer][iBox] = new TBox(fCCCUBEY[iBox]-fCCSIZEXY/2.,fCCLAYERZ[iLayer]-fCCSIZEZ/2.,
					fCCCUBEY[iBox]+fCCSIZEXY/2.,fCCLAYERZ[iLayer]+fCCSIZEZ/2.);
			fCubeFrameY[iLayer][iBox]->SetFillStyle(0);
			fCubeFrameY[iLayer][iBox]->SetLineColor(kBlack);
			fCubeFrameY[iLayer][iBox]->SetLineWidth(2);
		}
	}

	// Calocube 2D histograms, creating binning vectors
	double *binX = new double[fCCNCOL*2];
	double *binY = new double[fCCNROW*2];
	double *binZ = new double[fCCNLAY*2];
	for (int iBin=0; iBin<fCCNCOL; iBin++) {
		binX[iBin*2] = fCCCUBEX[iBin]-fCCSIZEXY/2.;
		binX[iBin*2+1] =fCCCUBEX[iBin]+fCCSIZEXY/2.;
		cout << "binX: " << iBin*2 << ": " << binX[iBin*2];
		cout << " binX: " << iBin*2+1 << ": " << binX[iBin*2+1] << endl;
	}
	for (int iBin=0; iBin<fCCNROW; iBin++) {
	 binY[iBin*2] = fCCCUBEY[fCCNROW-1-iBin]-fCCSIZEXY/2.;
	 binY[iBin*2+1] =fCCCUBEY[fCCNROW-1-iBin]+fCCSIZEXY/2.;
		cout << "binY: " << iBin*2 << ": " << binY[iBin*2];
		cout << " binY: " << iBin*2+1 << ": " << binY[iBin*2+1] << endl;
	}
	for (int iBin=0; iBin<fCCNLAY; iBin++) {
	 binZ[iBin*2] = fCCLAYERZ[iBin]-fCCSIZEZ/2.;
	 binZ[iBin*2+1] =fCCLAYERZ[iBin]+fCCSIZEZ/2.;
		cout << "binZ: " << iBin*2 << ": " << binZ[iBin*2];
		cout << " binZ: " << iBin*2+1 << ": " << binZ[iBin*2+1] << endl;
	}

	h2DCalocubeX = new TH2D("h2DCalocubeX","2D CC Xview",fCCNCOL*2-1,binX,fCCNLAY*2-1,binZ);
	h2DCalocubeX->SetStats(false);
	h2DCalocubeY = new TH2D("h2DCalocubeY","2D CC Yview",fCCNROW*2-1,binY,fCCNLAY*2-1,binZ);
	h2DCalocubeX->SetStats(false);

	// AMS tracker line
	for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
		fAMSLineX.push_back(new TLine(fAMSALIGNX[iLayer]-fAMSPITCHX*fAMSNSTRIPX/2.,fAMSLAYERZ[iLayer],fAMSALIGNX[iLayer]+fAMSPITCHX*fAMSNSTRIPX/2.,fAMSLAYERZ[iLayer]));
		fAMSLineX.back()->SetLineWidth(2);
		fAMSLineY.push_back(new TLine(fAMSALIGNY[iLayer]-fAMSPITCHY*fAMSNSTRIPY/2.,fAMSLAYERZ[iLayer],fAMSALIGNY[iLayer]+fAMSPITCHY*fAMSNSTRIPY/2.,fAMSLAYERZ[iLayer]));
		fAMSLineY.back()->SetLineWidth(2);
	}

	// AMS 2D histograms and creating binning vectors
	double *AMSbinZ = new double[fAMSNLAY*2];
	for (int iBin=0; iBin<fAMSNLAY; iBin++) {
		AMSbinZ[iBin*2] = fAMSLAYERZ[iBin]-fAMSSIZEZ/10.;
		AMSbinZ[iBin*2+1] =fAMSLAYERZ[iBin]+fAMSSIZEZ;
		cout << "AMSbinZ: " << iBin*2 << ": " << AMSbinZ[iBin*2];
		cout << " AMSbinZ: " << iBin*2+1 << ": " << AMSbinZ[iBin*2+1] << endl;
	}
	// x and Y wider axes to take into account alignment: 2000strip*pithc starting from 0
	h2DAMSX = new TH2D("h2DAMSX","2D AMS Xview",2000/fAMSREBIN,-1000*fAMSPITCHX,+1000*fAMSPITCHX,fAMSNLAY*2-1,AMSbinZ);
	h2DAMSX->SetStats(false);
	h2DAMSY = new TH2D("h2DAMSY","2D AMS Yview",2000/fAMSREBIN,-1000*fAMSPITCHY,+1000*fAMSPITCHY,fAMSNLAY*2-1,AMSbinZ);
	h2DAMSX->SetStats(false);

	// DAMPE tracker line
	for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
		fDAMPELineX.push_back(new TLine(fDAMPEALIGNX[iLayer*2]-fDAMPEPITCHX*fDAMPENSTRIP/2.,fDAMPELAYERZ[iLayer],fDAMPEALIGNX[iLayer*2]+fDAMPEPITCHX*fDAMPENSTRIP/2.,fDAMPELAYERZ[iLayer]));
		fDAMPELineX.back()->SetLineWidth(2);
		//fDAMPELineX.back()->SetLineColor(kBlue);
		fDAMPELineX.push_back(new TLine(fDAMPEALIGNX[iLayer*2+1]-fDAMPEPITCHX*fDAMPENSTRIP/2.,fDAMPELAYERZ[iLayer],fDAMPEALIGNX[iLayer*2+1]+fDAMPEPITCHX*fDAMPENSTRIP/2.,fDAMPELAYERZ[iLayer]));
		fDAMPELineX.back()->SetLineWidth(2);
		//fDAMPELineX.back()->SetLineColor(kRed);
	}
	// DAMPE 2D histograms and creating binning vectors
	double *DAMPEbinZ = new double[fDAMPENLAY*2];
	for (int iBin=0; iBin<fDAMPENLAY; iBin++) {
		DAMPEbinZ[iBin*2] = fDAMPELAYERZ[iBin]-fDAMPESIZEZ/10.;
		DAMPEbinZ[iBin*2+1] =fDAMPELAYERZ[iBin]+fDAMPESIZEZ;
		cout << "DAMPEbinZ: " << iBin*2 << ": " << DAMPEbinZ[iBin*2];
		cout << " DAMPEbinZ: " << iBin*2+1 << ": " << DAMPEbinZ[iBin*2+1] << endl;
	}
	// x and Y wider axes to take into account alignment: 2000strip*pithc starting from 0
	h2DDAMPEX = new TH2D("h2DDAMPEX","h2DDAMPEX",1200/fDAMPEREBIN,-700*fDAMPEPITCHX,+500*fDAMPEPITCHX,fDAMPENLAY*2-1,DAMPEbinZ);
	h2DDAMPEX->SetStats(false);

	//Create base histrogmas with maximum and minimum values
	h2DBaseX =  new TH2D("h2DBaseX","2D viewer, X view",1,-14,+14,1,-60,30);
	h2DBaseX->SetStats(false);
	h2DBaseY =  new TH2D("h2DBaseY","2D viewer, Y view",1,-14,+14,1,-60,30);
	h2DBaseY->SetStats(false);

	// set canvas dimensions and draw constant contents.
	c2D->Divide(2);
	c2D->cd(1);
	h2DBaseX->Draw();
	// cube image
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNCOL; iBox++) {
			fCubeFrameX[iLayer][iBox]->Draw("same");
		}
	}
	// ams line
	for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
		fAMSLineX[iLayer]->Draw("same");
	}
	// dampe line
	for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
		fDAMPELineX[iLayer*2]->Draw("same");
		fDAMPELineX[iLayer*2+1]->Draw("same");
	}
	c2D->cd(2);
	h2DBaseY->Draw();
	// cube image
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNROW; iBox++) {
			fCubeFrameY[iLayer][iBox]->Draw("same");
		}
	}
	// ams line
	for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
		fAMSLineY[iLayer]->Draw("same");
	}

	// create 1D tracker histos
	cTracker->Divide(3,2);
	for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
	  stringstream name, title;
	  name << "hDAMPEHit_" << iLayer;
	  title << "DAMPE layer " << iLayer << " tdr1 " << fDAMPETDRNUM[iLayer*2] << " tdr2 " << fDAMPETDRNUM[iLayer*2+1];
	  fHDAMPEHitVect.push_back(new TH1F(name.str().c_str(),title.str().c_str(),
					    1200/fDAMPEREBIN,-700*fDAMPEPITCHX,+500*fDAMPEPITCHX));
	}
}



void CaloAMS2DViewer::Fill2DCCviewer(TCanvas *c2D,Int_t nHits_l,Float_t hitSignal_l[],Int_t hitRow_l[],Int_t hitCol_l[],Int_t hitLayer_l[]) {

  vector< vector < double > > singalXZ(fCCNLAY,vector< double >(fCCNCOL,0));
  vector< vector < double > > singalYZ(fCCNLAY,vector< double >(fCCNROW,0));
  double Etot=0;
  double Etot_5layer=0;
  
  // hit cycle
  for(int iHit=0; iHit<nHits_l; iHit++){
    
    // consistency check
    if(hitSignal_l[iHit]<=0) {
      cout << iHit << " " << hitCol_l[iHit]<<" "<<hitRow_l[iHit]<<" "<< hitLayer_l[iHit]<<" !!!!!!!!!!!!!!!!!!!  "<<hitSignal_l[iHit]<<endl;
      continue;
    }
    Etot += hitSignal_l[iHit];
    
    // fill X,Y vectors
    if (hitCol_l[iHit]<fCCNCOL && hitRow_l[iHit]<fCCNROW && hitLayer_l[iHit]<fCCNLAY) {
      singalXZ[hitLayer_l[iHit]][hitCol_l[iHit]] += hitSignal_l[iHit];
      singalYZ[hitLayer_l[iHit]][hitRow_l[iHit]] += hitSignal_l[iHit];
      Etot_5layer += hitSignal_l[iHit];
    }
  }
  
  // fill histogram XZ
  for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
    for(int iCol=0; iCol<fCCNCOL; iCol++) {
      h2DCalocubeX->Fill(fCCCUBEX[iCol],fCCLAYERZ[iLayer],singalXZ[iLayer][iCol]);
    }
  }
  // fill histogram YZ
  for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
    for(int iRow=0; iRow<fCCNCOL; iRow++) {
      h2DCalocubeY->Fill(fCCCUBEY[iRow],fCCLAYERZ[iLayer],singalYZ[iLayer][iRow]);
    }
  }
  // plot X view
  c2D->cd(1);
  gPad->SetLogz();
  h2DCalocubeX->Draw("same COL");
  gPad->Update();
  // plot Y view
  c2D->cd(2);
  gPad->SetLogz();
  h2DCalocubeY->Draw("same COL");
  gPad->Update();
  
  cout << "Large "<< endl;
  cout << "\tnHits = " << nHits_l << endl;
  cout << "\tEnergy = " << Etot << endl;
  cout << "\tEnergy in 5 layer = " << Etot_5layer << endl;
}

bool CaloAMS2DViewer::ReturnAMSHit(int iCluster, Event *ev, int &tdr, int &side, int &Layer, vector<int> &strip,vector<float> &position, vector<float> &signal) {

  // default val
  tdr=-1;
  side=-1;
  Layer=-1;
  strip.clear();
  position.clear();
  signal.clear();
  // Check cluster
  if (iCluster<ev->GetNClusTot()) {
    Cluster *cl = ev->GetCluster(iCluster);
    tdr = cl->GetTDR();
    side = cl->side;
    int address = cl->address;
    // find layer
    Layer=-1;
    for (int iLadder=0; iLadder < fAMSNLAY; iLadder++) {
    	if (fAMSTDRNUM[iLadder]==tdr) {
    		Layer=iLadder;
    		break;
    	}
    }
    if (Layer==-1)
    	return true;
    //cout << "AMS tdr " << tdr << " side " << side << " address " << address << endl;
    //cout << "Z " << cl->GetZPosition() << " XY " << cl->GetAlignedPosition() << " pitch " << cl->GetPitch(side) << endl;
    //cout << "Singal vector: ";
    for (int iSignal=0; iSignal<cl->length; iSignal++) {
    	//cout << " " << cl->Signal[iSignal];
    	//if(side==0) hVect[iHisto].Fill(address+iSignal,cl->Signal[iSignal]);
    	if(side==fAMSXSIDE) {
	  if (side == 0) strip.push_back(address+iSignal);
	  else strip.push_back(address+iSignal-TMath::Max(fAMSNSTRIPX,fAMSNSTRIPY));
	  position.push_back(((double)(strip.back()) - (fAMSNSTRIPX/2.) )*fAMSPITCHX + fAMSALIGNX[Layer]);//cm
	  signal.push_back(cl->Signal[iSignal]);
    	}
    	else {
	  if (side == 0) strip.push_back(address+iSignal);
	  else strip.push_back(address+iSignal-TMath::Max(fAMSNSTRIPX,fAMSNSTRIPY));
	  position.push_back(((double)(strip.back()) - (fAMSNSTRIPY/2.) )*fAMSPITCHY + fAMSALIGNY[Layer]);//cm
	  signal.push_back(cl->Signal[iSignal]);
	  // if (tdr==20) cout << "Cluster: " <<  iCluster << " tdr " << tdr << " " << side << " length " << cl->length << " address " << address << endl;
    	}
    }
  } else {
  	return false;
  }
  return true;
}

void CaloAMS2DViewer::Fill2DAMSviewer(TCanvas *c2D, Event *ev) {
  // get AMS information
  int tdr, side, layer;
  vector<int> strip;
  vector<float> position;
  vector<float> signal;
  int iCluster=0;
  while(ReturnAMSHit(iCluster,ev,tdr,side,layer,strip,position,signal)) {
    if (signal.size()>0) {
      if (side == fAMSXSIDE) {
	for (unsigned int iPos=0; iPos<position.size(); iPos++) {
	  h2DAMSX->Fill(position[iPos],fAMSLAYERZ[layer],signal[iPos]);
	} 
      } else {
	for (unsigned int iPos=0; iPos<position.size(); iPos++) {
	  h2DAMSY->Fill(position[iPos],fAMSLAYERZ[layer],signal[iPos]);
	}
      }
    }
    iCluster++;
  }

  // plot X view
  c2D->cd(1);
  gPad->SetLogz();
  h2DAMSX->Draw("same COL");
  gPad->Update();
  // plot Y view
  c2D->cd(2);
  gPad->SetLogz();
  h2DAMSY->Draw("same COL");
  gPad->Update();
}

bool CaloAMS2DViewer::ReturnDAMPEHit(int iCluster, Event *ev, int &tdr, int &Ladder, vector<int> &strip,vector<float> &position, vector<float> &signal) {

  // default val
  tdr=-1;
  int side=-1;
  Ladder=-1;
  strip.clear();
  position.clear();
  signal.clear();
  // Check cluster
  if (iCluster<ev->GetNClusTot()) {
    Cluster *cl = ev->GetCluster(iCluster);
    tdr = cl->GetTDR();
    side = cl->side;
    int address = cl->address;
    // find ladder
    if (DEBUG) cout << "Cluster: " <<  iCluster << " tdr " << tdr << " " << side << " length " << cl->length << " address " << address << endl;

    for (int iLadder=0; iLadder < 2*fDAMPENLAY; iLadder++) {
      if (fDAMPETDRNUM[iLadder]==tdr) {
	Ladder=iLadder;
	break;
      }
    }
    if (Ladder==-1) // it is not a dampe ladder
    	return true;

    for (int iSignal=0; iSignal<cl->length; iSignal++) {
    	//cout << " " << cl->Signal[iSignal];
    	//if(side==0) hVect[iHisto].Fill(address+iSignal,cl->Signal[iSignal]);
        if(DAMPEHOLE) { 
	  if(side==fDAMPEXSIDE && ((address+iSignal)<192)) {
	    strip.push_back(address+iSignal);
	    position.push_back(((double)(strip.back()) - (fDAMPENSTRIP/2.) )*fDAMPEPITCHX + fDAMPEALIGNX[Ladder]);//cm
	    signal.push_back(cl->Signal[iSignal]);
	  } else if (side==fDAMPEXSIDE && ((address+iSignal)>=321 && (address+iSignal)<(321+192))) {
	    strip.push_back(address+iSignal-321+192);
	    position.push_back(((double)(strip.back()) - (fDAMPENSTRIP/2.) )*fDAMPEPITCHX + fDAMPEALIGNX[Ladder]);//cm
	    signal.push_back(cl->Signal[iSignal]);
	  }
        } else { 
	  strip.push_back(address+iSignal);
	  position.push_back(((double)(strip.back()) - (fDAMPENSTRIP/2.) )*fDAMPEPITCHX + fDAMPEALIGNX[Ladder]);//cm
	  signal.push_back(cl->Signal[iSignal]);
	}
    }
  } else {
  	return false;
  }
  return true;
}

 void CaloAMS2DViewer::Fill2DDAMPEviewer(TCanvas *c2D,TCanvas *cTracker, Event *ev) {

   // delete 1D histo if is not done by the viewer core
   for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
     fHDAMPEHitVect[iLayer]->Reset("M");
   }

  // get DAMPE information
  int tdr, ladder;
  vector<int> strip;
  vector<float> position;
  vector<float> signal;
  int iCluster=0;
  while(ReturnDAMPEHit(iCluster,ev,tdr,ladder,strip,position,signal)) {
    if (signal.size()>0) {
      for (unsigned int iPos=0; iPos<position.size(); iPos++) {
	h2DDAMPEX->Fill(position[iPos],fDAMPELAYERZ[ladder/2],signal[iPos]);
	fHDAMPEHitVect[ladder/2]->Fill(position[iPos],signal[iPos]);
      }
    }
    iCluster++;
  }
  // plot X view
  c2D->cd(1);
  gPad->SetLogz();
  h2DDAMPEX->Draw("same COL");
  gPad->Update();

  // 1D tracker plots
  for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
    cTracker->cd(iLayer+1);
    fHDAMPEHitVect[iLayer]->Draw();
    gPad->Update();
  }
}

void CaloAMS2DViewer::DeleteImage() {
  cout << "DELCAL" << endl;
  h2DCalocubeX->Reset("M");
  h2DCalocubeY->Reset("M");
  h2DAMSX->Reset("M");
  h2DAMSY->Reset("M");
  h2DDAMPEX->Reset("M");
  for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
    fHDAMPEHitVect[iLayer]->Reset("M");
  }
}

void CaloAMS2DViewer::CreateAMSOccupancy(TCanvas *cAMS) {
	if (fHAMSOccupancyVect.size()==0) {
		for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
			stringstream name, title;
			name << "hAMSOccupancy_" << iLayer;
			title << "AMS layer " << iLayer << " tdr " << fAMSTDRNUM[iLayer];
			fHAMSOccupancyVect.push_back(new TH2F(name.str().c_str(),title.str().c_str(),
					2000/fAMSREBIN,-1000*fAMSPITCHX,+1000*fAMSPITCHX,2000/fAMSREBIN,-1000*fAMSPITCHY,+1000*fAMSPITCHY));
		}

		// Divide Canvas
		float NrowF = sqrt(fAMSNLAY);
		int NrowI = NrowF;
		if (NrowI!=NrowF) {
			NrowI++;
		}
		float NcolF = sqrt(fAMSNLAY);
		int NcolI = NcolF;
		if (NcolI!=NcolF) {
			NcolI++;
		}
		cAMS->Divide(NrowI,NcolI);
	}
}

void CaloAMS2DViewer::CreateDAMPEOccupancy(TCanvas *cDAMPE) {
	if (fHDAMPEOccupancyVect.size()==0) {
		for (int iLayer=0; iLayer<fDAMPENLAY; iLayer++) {
			stringstream name, title;
			name << "hDAMPEOccupancy_" << iLayer;
			title << "DAMPE layer " << iLayer << " tdr1 " << fDAMPETDRNUM[iLayer*2] << " tdr2 " << fDAMPETDRNUM[iLayer*2+1];
			fHDAMPEOccupancyVect.push_back(new TH1F(name.str().c_str(),title.str().c_str(),
					1200/fDAMPEREBIN,-700*fDAMPEPITCHX,+500*fDAMPEPITCHX));
		}

		// Divide Canvas
		float NrowF = sqrt(fDAMPENLAY);
		int NrowI = NrowF;
		if (NrowI!=NrowF) {
			NrowI++;
		}
		float NcolF = sqrt(fDAMPENLAY);
		int NcolI = NcolF;
		if (NcolI!=NcolF) {
			NcolI++;
		}
		cDAMPE->Divide(NrowI,NcolI);
	}
}

void CaloAMS2DViewer::FillAMSOccupancy(Event *ev) {

  //vector for fill histograms
  vector< int > layerVectX;
  vector < vector < float > > positionVectX;
  vector < vector < float > > signalVectX;
  vector< int > layerVectY;
  vector < vector < float > > positionVectY;
  vector < vector < float > > signalVectY;
  
  // get AMS information
  int tdr, side, layer;
  vector<int> strip;
  vector<float> position;
  vector<float> signal;
  int iCluster=0;
  while(ReturnAMSHit(iCluster,ev,tdr,side,layer,strip,position,signal)) {
    if (signal.size()>0) {
      if (side == fAMSXSIDE) {
	layerVectX.push_back(layer);
	positionVectX.push_back(position);
	signalVectX.push_back(signal);
      } else {
	layerVectY.push_back(layer);
	positionVectY.push_back(position);
	signalVectY.push_back(signal);
      }
    }
    iCluster++;
  }
  
  // Fill 2D histogram
  //cout << "!!!!!!!!!!pippo!!!!!!!!!!!!!! " << layerVectX.size() << endl;
  for (unsigned int iVectX=0; iVectX<layerVectX.size(); iVectX++) {
    for (unsigned int iPosX=0; iPosX<positionVectX[iVectX].size(); iPosX++) {
      for (unsigned int iVectY=0; iVectY<layerVectY.size(); iVectY++) {
	for (unsigned int iPosY=0; iPosY<positionVectY[iVectY].size(); iPosY++) {
	  if (layerVectX[iVectX]==layerVectY[iVectY]) {
	    fHAMSOccupancyVect[layerVectX[iVectX]]->Fill(positionVectX[iVectX][iPosX],positionVectY[iVectY][iPosY]);
	    //cout << "Fill " << iVectX << " " << iPosX << " " << iPosY << endl;
	  }
	}
      }
    }
  }
}

void CaloAMS2DViewer::FillDAMPEOccupancy(Event *ev) {

	// get DAMPE information
	int tdr, ladder;
	vector<int> strip;
	vector<float> position;
	vector<float> signal;
	int iCluster=0;
	while(ReturnDAMPEHit(iCluster,ev,tdr,ladder,strip,position,signal)) {
		if (signal.size()>0) {
			for (unsigned int iPos=0; iPos<position.size(); iPos++) {
				fHDAMPEOccupancyVect[ladder/2.]->Fill(position[iPos],signal[iPos]);
			}
		}
		iCluster++;
	}
}

void CaloAMS2DViewer::DrawAMSOccupancy(TCanvas *cAMS) {
	for (unsigned int iVect=0; iVect<fHAMSOccupancyVect.size(); iVect++) {
		cAMS->cd(iVect+1);
		fHAMSOccupancyVect[iVect]->Draw("COLZ");
		gPad->Update();
	}
}

void CaloAMS2DViewer::DrawDAMPEOccupancy(TCanvas *cDAMPE) {
	for (unsigned int iVect=0; iVect<fHDAMPEOccupancyVect.size(); iVect++) {
		cDAMPE->cd(iVect+1);
		fHDAMPEOccupancyVect[iVect]->Draw();
		gPad->Update();
	}
}
