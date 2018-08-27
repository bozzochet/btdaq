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

double CaloAMS2DViewer::fCCLAYERZ[] = {2,6,10,14,18};
double CaloAMS2DViewer::fCCCUBEX[] = {8,4,0,-4,-8};
double CaloAMS2DViewer::fCCCUBEY[] = {-8,-4,0,4,8};
double CaloAMS2DViewer::fAMSLAYERZ[] = {-147, -143, -112, -108, -76, -72, -38}; // real distance is 100 cm greather with respect the one here used
double CaloAMS2DViewer::fAMSALIGNX[] = {0, 0, 0, 0, 0, 0, 0};
double CaloAMS2DViewer::fAMSALIGNY[] = {0, 0, 0, 0, 0, 0, 0};
int CaloAMS2DViewer::fAMSTDRNUM[] = {0, 15, 11, 1, 10, 12, 13};

CaloAMS2DViewer::CaloAMS2DViewer(bool adc_mip): fCCNROW(5),fCCNCOL(5),fCCNLAY(5),fCCMIP2MeV(20.178),fCCSIZEXY(3.6),
		fCCSIZEZ(3), fAMSPITCHX(0.011), fAMSPITCHY(0.0208), fAMSNSTRIP(1024), fAMSNLAY(7), fAMSSIZEZ(2), fAMSREBIN(10) {
		fAdc_mip = adc_mip;
		fCubeFrameX = vector< vector< TBox* > > (fCCNLAY,vector< TBox* >(fCCNCOL,NULL));
		fCubeFrameY = vector< vector< TBox* > > (fCCNLAY,vector< TBox* >(fCCNROW,NULL));
}

// Inizialize 3D calocube viewer
void CaloAMS2DViewer::Inizialize2Dviewer(TCanvas *c2D) {

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
		binX[iBin*2] = fCCCUBEX[fCCNROW-1-iBin]-fCCSIZEXY/2.;
		binX[iBin*2+1] =fCCCUBEX[fCCNROW-1-iBin]+fCCSIZEXY/2.;
		cout << "binX: " << iBin*2 << ": " << binX[iBin*2];
		cout << " binX: " << iBin*2+1 << ": " << binX[iBin*2+1] << endl;
	}
	for (int iBin=0; iBin<fCCNROW; iBin++) {
	 binY[iBin*2] = fCCCUBEY[iBin]-fCCSIZEXY/2.;
	 binY[iBin*2+1] =fCCCUBEY[iBin]+fCCSIZEXY/2.;
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
		fAMSLineX.push_back(new TLine(fAMSALIGNX[iLayer]-fAMSPITCHX*fAMSNSTRIP,fAMSLAYERZ[iLayer],fAMSALIGNX[iLayer]+fAMSPITCHX*fAMSNSTRIP,fAMSLAYERZ[iLayer]));
		fAMSLineX.back()->SetLineWidth(2);
		fAMSLineY.push_back(new TLine(fAMSALIGNY[iLayer]-fAMSPITCHY*fAMSNSTRIP,fAMSLAYERZ[iLayer],fAMSALIGNY[iLayer]+fAMSPITCHY*fAMSNSTRIP,fAMSLAYERZ[iLayer]));
		fAMSLineY.back()->SetLineWidth(2);
	}

	// AMS 2D histograms and creating binning vectors
	double *AMSbinZ = new double[fAMSNLAY*2];
	for (int iBin=0; iBin<fAMSNLAY; iBin++) {
		AMSbinZ[iBin*2] = fAMSLAYERZ[iBin]-fAMSSIZEZ;
		AMSbinZ[iBin*2+1] =fAMSLAYERZ[iBin]+fAMSSIZEZ/10.;
		cout << "AMSbinZ: " << iBin*2 << ": " << AMSbinZ[iBin*2];
		cout << " AMSbinZ: " << iBin*2+1 << ": " << AMSbinZ[iBin*2+1] << endl;
	}
	// x and Y wider axes to take into account alignment: 2000strip*pithc starting from 0
	h2DAMSX = new TH2D("h2DAMSX","2D AMS Xview",2000/fAMSREBIN,-1000*fAMSPITCHX,+1000*fAMSPITCHX,fAMSNLAY*2-1,AMSbinZ);
	h2DAMSX->SetStats(false);
	h2DAMSY = new TH2D("h2DAMSY","2D AMS Yview",2000/fAMSREBIN,-1000*fAMSPITCHY,+1000*fAMSPITCHY,fAMSNLAY*2-1,AMSbinZ);
	h2DAMSX->SetStats(false);

	//Create base histrogmas with maximum and minimum values
	h2DBaseX =  new TH2D("h2DBaseX","2D viewer, X view",1,-14,+14,1,-150,20);
	h2DBaseX->SetStats(false);
	h2DBaseY =  new TH2D("h2DBaseX","2D viewer, X view",1,-14,+14,1,-150,20);
	h2DBaseY->SetStats(false);

	// set canvas dimensions
	c2D->Divide(2);
	c2D->cd(1);
	h2DBaseX->Draw();
	// cube image
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNCOL; iBox++) {
			fCubeFrameX[iLayer][iBox]->Draw("same");
		}
	}
	for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
		fAMSLineX[iLayer]->Draw("same");
	}
	c2D->cd(2);
	h2DBaseY->Draw();
	// cube image
	for (int iLayer=0; iLayer<fCCNLAY; iLayer++) {
		for(int iBox=0; iBox<fCCNROW; iBox++) {
			fCubeFrameY[iLayer][iBox]->Draw("same");
		}
	}
	for (int iLayer=0; iLayer<fAMSNLAY; iLayer++) {
		fAMSLineY[iLayer]->Draw("same");
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

void CaloAMS2DViewer::Fill2DAMSviewer(TCanvas *c2D, Event *ev) {

  // Check cluster
  cout << "Check AMS cluster" << endl;
  for (int iCluster=0; iCluster<ev->GetNClusTot(); iCluster++) {
    Cluster *cl = ev->GetCluster(iCluster);
    int tdr = cl->GetTDR();
    int side = cl->side;
    int address = cl->address;
    // find layer
    int iLayer=-1;
    for (int iLadder=0; iLadder < fAMSNLAY; iLadder++) {
    	if (fAMSTDRNUM[iLadder]==tdr) {
    		iLayer=iLadder;
    		break;
    	}
    }
    if (iLayer==-1) {
    	cout << "TDR: " << tdr <<" no layer found, exit!" << endl;
    	exit(1);
    }
    cout << "AMS tdr " << tdr << " side " << side << " address " << address << endl;
    cout << "Z " << cl->GetZPosition() << " XY " << cl->GetAlignedPosition() << " pitch " << cl->GetPitch(side) << endl;
    cout << "Singal vector: ";
    for (int iSignal=0; iSignal<cl->length; iSignal++) {
    	cout << " " << cl->Signal[iSignal];
    	//if(side==0) hVect[iHisto].Fill(address+iSignal,cl->Signal[iSignal]);
    	if(side==0) h2DAMSX->Fill((double)((address+iSignal)-fAMSNSTRIP)*fAMSPITCHX,fAMSLAYERZ[iLayer],cl->Signal[iSignal]);
    	if(side==0) h2DAMSY->Fill((double)((address+iSignal)-fAMSNSTRIP)*fAMSPITCHY,fAMSLAYERZ[iLayer],cl->Signal[iSignal]);
    }
    cout << endl;
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

void CaloAMS2DViewer::DeleteImage() {
	h2DCalocubeX->Reset();
	h2DCalocubeY->Reset();
	h2DAMSX->Reset();
	h2DAMSY->Reset();
}


