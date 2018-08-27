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

#include "Calo3DViewer.h"

using namespace std;

Color_t Calo3DViewer::fColor[] = {
			kCyan-7
			,kSpring-9//kGreen
			,kYellow
			,kOrange+8//kRed
			,kPink
			,kViolet//kMagenta
	};

Calo3DViewer::Calo3DViewer(bool adc_mip): fNROW(5),fNCOL(5),fNLAY(18),fNCOLOR(6),fMIP2MeV(20.178),fSIZE(4.0) {
	fAdc_mip = adc_mip;
	fLog10_paletteMin = adc_mip? 1 : -1.; //min signal
	fLog10_paletteMax = adc_mip? 7. : 4.; //max signal
	fCubeSignal_l = vector< vector< vector< TPolyLine* > > > (fNCOL,vector< vector< TPolyLine* > >(fNROW,vector< TPolyLine* >(fNLAY,NULL)));
	fCubeSignal_s = vector< vector< vector< TPolyLine* > > > (fNCOL,vector< vector< TPolyLine* > >(fNROW,vector< TPolyLine* >(fNLAY,NULL)));
	fCubeFrame = vector< vector< vector< TPolyLine* > > > (fNCOL,vector< vector< TPolyLine* > >(fNROW,vector< TPolyLine* >(fNLAY,NULL)));
}


// Inizialize 3D calocube viewer
void Calo3DViewer::Inizialize3Dviewer(TCanvas *c3D) {

	//========================================================
	// set cube image parameters
	//========================================================
	const float size = fSIZE;//3.9;//cm
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

	c3D->Divide(2,1);
	c3D->cd(1);

	float xCanvasLow = -size*fNROW*cos(angle)-0.5;
	float yCanvasLow = -1.*(fNROW*size*sin(angle)+gap)*(fNLAY+1)-0.5 - pal - blabla;
	float xCanvasHigh = size*fNCOL+0.5;
	float yCanvasHigh = 0.+0.5;
	c3D->GetPad(1)->Range(xCanvasLow-margine,
			yCanvasLow-margine,
			xCanvasHigh+margine,
			yCanvasHigh+margine);
	c3D->GetPad(2)->Range(xCanvasLow-margine,
			yCanvasLow-margine,
			xCanvasHigh+margine,
			yCanvasHigh+margine);

	//========================================================
	// initialize cube images
	//========================================================
	fFront = new TBox();
	for(int ic=0;ic<fNCOL;ic++){
		for(int ir=0;ir<fNROW;ir++){
			for(int il=0;il<fNLAY;il++){
				// traslazione
				float dx = ic*size-ir*size*cos(angle);
				float dy = -1*ir*size*sin(angle)-il*(fNROW*size*sin(angle)+gap);
				for(int i=0; i<5; i++)x[i] = x0[i] + dx;
				for(int i=0; i<5; i++)y[i] = y0[i] + dy;
				fCubeSignal_l[ic][ir][il] = NULL;
				fCubeSignal_s[ic][ir][il] = NULL;
				fCubeFrame[ic][ir][il] = new TPolyLine(5, x, y);
				fCubeFrame[ic][ir][il]->SetLineColor(kGray);
				fCubeFrame[ic][ir][il]->SetFillColor(kGray);
				fCubeFrame[ic][ir][il]->SetLineWidth(2);
				c3D->cd(1); fCubeFrame[ic][ir][il]->Draw();
				c3D->cd(2); fCubeFrame[ic][ir][il]->Draw();
				if(ic==0&&ir==fNROW-1&&il==fNLAY-1){
					fFront->SetX1(x0[3]+dx);
					fFront->SetY1(y0[3]+dy-(fNROW*size*sin(angle)));
				}else if(ic==fNCOL-1&&ir==fNROW-1&&il==0){
					fFront->SetX2(x0[2]+dx);
					fFront->SetY2(y0[2]+dy);
				}
				//
			}
		}
	}
	fFront->SetLineStyle(2);
	fFront->SetLineColor(kBlack);
	fFront->SetFillStyle(0);
	c3D->cd(1);   fFront->Draw();
	c3D->cd(2);   fFront->Draw();

	TBox *palette[fNCOLOR];
	float dx = (xCanvasHigh-xCanvasLow)/fNCOLOR;
	float xl = xCanvasLow;
	float yl = yCanvasLow + blabla;
	float xh = xCanvasLow + dx;
	float yh = yCanvasLow + blabla + 0.5*pal;

	for(int ic=0; ic<fNCOLOR; ic++ ){
		palette[ic] = new TBox(xl+ic*dx,yl,xh+ic*dx,yh);
		palette[ic]->SetFillColor(fColor[ic]);
		c3D->cd(1); palette[ic]->Draw();
		c3D->cd(2); palette[ic]->Draw();
	}

	//========================================================
	// create energy axis
	//========================================================
	TGaxis *axis = new TGaxis(xl, yl,
			xl+dx*fNCOLOR, yl,
			pow(10.,fLog10_paletteMin),pow(10.,fLog10_paletteMax),
			fNCOLOR,"G");
	axis->SetLabelOffset(-0.05);
	axis->SetLabelSize(.03);
	axis->SetTitleSize(.03);
	axis->SetTitleOffset(0.3);
	axis->SetTitle(Form("Energy deposit [%s]", fAdc_mip? "ADC" : "MIP"));
	c3D->cd(1); axis->Draw();
	c3D->cd(2); axis->Draw();
}

void Calo3DViewer::Fill3Dviewer(TCanvas *c3D,Int_t nHits_l,Int_t nHits_s, Float_t hitSignal_l[], Float_t hitSignal_s[],
		Int_t hitRow_l[], Int_t hitRow_s[], Int_t hitCol_l[], Int_t hitCol_s[], Int_t hitLayer_l[], Int_t hitLayer_s[]) {

	double sumde_l=0.;
	double sumde_s=0.;

	c3D->cd(1);
	for(int ih=0; ih<nHits_l; ih++){
		sumde_l+=hitSignal_l[ih];
		if(hitSignal_l[ih]<=0)
			cout << ih << " " << hitCol_l[ih]<<" "<<hitRow_l[ih]<<" "<< hitLayer_l[ih]<<" !!!!!!!!!!!!!!!!!!!  "<<hitSignal_l[ih]<<endl;
		hitSignal_l[ih] = TMath::Max((Float_t)0.1,hitSignal_l[ih]);

		// create image
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]] = new TPolyLine();
		fCubeFrame[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Copy(*fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]);
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetFillColor(kWhite);
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetLineColor(kGray+2);
		int colorIndex = (int) (fNCOLOR*(log10(hitSignal_l[ih])-fLog10_paletteMin)/(fLog10_paletteMax-fLog10_paletteMin) );
		Color_t thisColor = kBlack;
		if(colorIndex<0)thisColor = kWhite;
		if(colorIndex>=0 && colorIndex<fNCOLOR)thisColor = fColor[colorIndex];
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->SetFillColor(thisColor);
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Draw("f");
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Draw("");
	};
	fFront->Draw();

	c3D->cd(2);
	for(int ih=0; ih<nHits_s; ih++){
		sumde_s+=hitSignal_s[ih];
		//if(hitSignal_s[ih]<=0)
		//	cout << ih << " " << hitCol_s[ih]<<" "<<hitRow_s[ih]<<" "<< hitLayer_s[ih]<<" !!!!!!!!!!!!!!!!!!!  "<<hitSignal_s[ih]<<endl;
		hitSignal_s[ih] = TMath::Max((Float_t)0.1,hitSignal_s[ih]);

		// create image
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]] = new TPolyLine();
		fCubeFrame[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Copy(*fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]);
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetFillColor(kWhite);
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetLineColor(kGray+2);
		int colorIndex = (int) (fNCOLOR*(log10(hitSignal_s[ih])-fLog10_paletteMin)/(fLog10_paletteMax-fLog10_paletteMin) );
		Color_t thisColor = kBlack;
		if(colorIndex<0)thisColor = kWhite;
		if(colorIndex>=0 && colorIndex<fNCOLOR)thisColor = fColor[colorIndex];
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->SetFillColor(thisColor);
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Draw("f");
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Draw("");
	};
	fFront->Draw();

	c3D->Update();

	cout << "Large "<< endl;
	cout << "\tnHits = " << nHits_l << endl;
	cout << "\tEnergy = " << sumde_l << " MIP " << (fAdc_mip? "" : Form("(%f MeV)", sumde_l*fMIP2MeV)) << endl;
	cout << "Small "<< endl;
	cout << "\tnHits = " << nHits_s << endl;
	cout << "\tEnergy = " << sumde_s << " MIP " << (fAdc_mip? "" : Form("(%f MeV)", sumde_s*fMIP2MeV)) << endl;

}

void Calo3DViewer::DeleteImage(Int_t nHits_l,Int_t nHits_s, Float_t hitSignal_l[], Float_t hitSignal_s[],
		Int_t hitRow_l[], Int_t hitRow_s[], Int_t hitCol_l[], Int_t hitCol_s[], Int_t hitLayer_l[], Int_t hitLayer_s[]) {

	for(int ih=0; ih<nHits_l; ih++){
		fCubeSignal_l[hitCol_l[ih]][hitRow_l[ih]][hitLayer_l[ih]]->Delete();
	}
	for(int ih=0; ih<nHits_s; ih++){
		fCubeSignal_s[hitCol_s[ih]][hitRow_s[ih]][hitLayer_s[ih]]->Delete();
	}
}

