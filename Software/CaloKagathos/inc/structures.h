#include <stdio.h>
#include <stdlib.h>
#include <TROOT.h>
#include <TH1.h>

#include "constants.h"
#include "IOfile.h"

using namespace std;

#ifndef _STRUCTURES
#define _STRUCTURES

//#define EUGENIO

// ==========================================
// ============== STRUCTURES ================
// ==========================================

struct EventInfo {
	Int_t trigger;
	Int_t good;

	void Reset() {
		trigger = 0;
		good = 0;
	}
};

struct calibParam{
	double ped[NCASISCHIPS][NCASISCHANNELS];
	double sig[NCASISCHIPS][NCASISCHANNELS];
	unsigned int nEvents[NCASISCHIPS][NCASISCHANNELS];
	double alphaLarge[NCASISCHIPS4CN];
	double alphaSmall[NCASISCHIPS4CN];

	void Reset(){
		for (int iCasis = 0; iCasis < NCASISCHIPS; iCasis++) {
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS; iCasisChannel++) {
				ped[iCasis][iCasisChannel] = 0.;
				sig[iCasis][iCasisChannel] = 0.;// ORRORE!!!!1e7;
				nEvents[iCasis][iCasisChannel] = 0;
			}
		}
		for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {
			alphaLarge[iCasis]=0.;
			alphaSmall[iCasis]=0.;
		}
	}
	//
	void SetPedestal( TH1F* h ){
		if(h && h->GetNbinsX() == NCASISCHIPS*NCASISCHANNELS){
			for(int i=0 ; i< h->GetNbinsX(); i++){
				ped[(int)(i/NCASISCHANNELS)][i%NCASISCHANNELS]=h->GetBinContent(i+1);
			}
		}else{
			cout << "calibParam::SetPed( TH1F* "<<h<<" ) --- wrong num.channels"<<endl;
		}
	}
	//
	void SetSigma( TH1F* h ){
		if(h && h->GetNbinsX() == NCASISCHIPS*NCASISCHANNELS){
			for(int i=0 ; i< h->GetNbinsX(); i++){
				sig[(int)(i/NCASISCHANNELS)][i%NCASISCHANNELS]=h->GetBinContent(i+1);
			}
		}else{
			cout << "calibParam::SetSig( TH1F* "<<h<<" ) --- wrong num.channels"<<endl;
		}
	}
	//
	void SetAlpha(  TH1F* hl, TH1F* hs ){
		if(hl && hl->GetNbinsX() == NCASISCHIPS4CN){
			for(int i=0 ; i< hl->GetNbinsX(); i++){
				alphaLarge[i]=hl->GetBinContent(i+1);
			}

		}else{
			cout << "calibParam::SetAlpha( TH1F* "<<hl<<", TH1F* "<<hs<<") --- wrong num.channels"<<endl;
		}
		if(hs && hs->GetNbinsX() == NCASISCHIPS4CN){
			for(int i=0 ; i< hs->GetNbinsX(); i++){
				alphaSmall[i]=hs->GetBinContent(i+1);
			}

		}else{
			cout << "calibParam::SetAlpha( TH1F* "<<hl<<", TH1F* "<<hs<<") --- wrong num.channels"<<endl;
		}
	}
};

struct CommonNoise{
	Float_t commonNoise[NCASISCHIPS4CN];
	Int_t nChannels[NCASISCHIPS4CN];
	Double_t disc[NCASISCHIPS4CN];//signal of disconnected channel
	Double_t CN[NCASISCHIPS4CN];//signal of diodeCN channel
	Double_t commonNoiseLarge[NCASISCHIPS4CN];//large sensor signals mean
	Int_t nChannelsLarge[NCASISCHIPS4CN];
	Double_t commonNoiseSmall[NCASISCHIPS4CN];//small sensor signals mean
	Int_t nChannelsSmall[NCASISCHIPS4CN];
	Double_t CNLarge[NCASISCHIPS4CN];//evaluated
	Double_t CNSmall[NCASISCHIPS4CN];//evaluated

	/// -------------------------------------
	void Reset(){

		for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++){
			nChannels[iCasis] = 0;
			commonNoise[iCasis] = 0.;
			disc[iCasis] = 0.;
			CN[iCasis] = 0.;
			nChannelsLarge[iCasis] = 0;
			commonNoiseLarge[iCasis] = 0.;
			nChannelsSmall[iCasis] = 0;
			commonNoiseSmall[iCasis] = 0.;
			CNLarge[iCasis] = 0.;
			CNSmall[iCasis] = 0.;
		}
	}//end Reset()
	/// -------------------------------------
	void Compute(IOfile*inputFile, calibParam* paramTemp, double cnRMS = 0.){
		for (int iCasis = 0; iCasis < NCASISCHIPS4CN; iCasis++) {
			int iHydra = iCasis/2;
			int nC = 0;
			double cnTemp = 0.;
			double cn = 0.;

			int nCLarge = 0;
			double cnTempLarge = 0.;
			double cnLarge = 0.;
			int nCSmall = 0;
			double cnTempSmall = 0.;
			double cnSmall = 0.;

			// evaluate averages over instrumented channesl
			// --------------------
			// FIRST estimation ( )
			// --------------------
			//loop over instrumented channels
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {

				int iHydraChannel = iCasisChannel+(14*(iCasis%2));

				if(!GoodChannel(iHydra, iHydraChannel))
					continue;

				double val = (double)inputFile->GetCCUBEdata(iHydra, iHydraChannel)
		    						- paramTemp->ped[iHydra][iHydraChannel];
				int sensor = GetSensor(iHydra,iHydraChannel);
				if( sensor!=0 && sensor !=1 )continue;//not instrumented

				//total noise
				double tn = paramTemp->sig[iHydra][iHydraChannel] * paramTemp->sig[iHydra][iHydraChannel];
				tn +=  cnRMS * cnRMS; //given as input
				tn = sqrt(tn);
				if(fabs(val) > tCut * tn )continue;//skip the channel

				cnTemp += (double)val;
				nC++;

				if(sensor==0){
					cnTempLarge += (double)val;
					nCLarge++;
				}
				if(sensor==1){
					cnTempSmall += (double)val;
					nCSmall++;
				}
			}// endl loop over channels
			if (nCLarge > 0)cnTempLarge /= (double)nCLarge;
			if (nCSmall > 0)cnTempSmall /= (double)nCSmall;
			if (nC > 0)cnTemp /= (double)nC;
			// --------------------
			// SECOND estimation ( )
			// --------------------
			nC = 0;
			nCLarge = 0;
			nCSmall = 0;
			for (int iCasisChannel = 0; iCasisChannel < NCASISCHANNELS4CN; iCasisChannel++) {

				int iHydraChannel = iCasisChannel+(14*(iCasis%2));

				if(!GoodChannel(iHydra, iHydraChannel))
					continue;

				double val = (double)inputFile->GetCCUBEdata(iHydra, iHydraChannel)
		    						- paramTemp->ped[iHydra][iHydraChannel];
				int sensor = GetSensor(iHydra,iHydraChannel);

				if( sensor!=0 && sensor !=1 )continue;//not instrumented
				//random noise
				double rn = paramTemp->sig[iHydra][iHydraChannel];
#ifdef EUGENIO
				if(sensor==0) {
					if(abs(val-cnTempLarge) > tCut * rn )continue;
				} else if (sensor==1) {
					if(abs(val-cnTempSmall) > tCut * rn )continue;
				}
#else
				if(fabs(val-cnTemp) > tCut * rn )continue;
#endif

				cn += (double)val;
				nC++;

				if(sensor==0){
					cnLarge += (double)val;
					nCLarge++;
				}
				if(sensor==1){
					cnSmall += (double)val;
					nCSmall++;
				}
			}//end loop over channels
			if (nCLarge > 0)cnLarge /= (double)nCLarge;
			if (nCSmall > 0)cnSmall /= (double)nCSmall;
			if (nC > 0)cn /= (double)nC;

			commonNoiseLarge[iCasis] = cnLarge;
			nChannelsLarge[iCasis]   = nCLarge;
			commonNoiseSmall[iCasis] = cnSmall;
			nChannelsSmall[iCasis]   = nCSmall;
			commonNoise[iCasis] = cn;
			nChannels[iCasis]   = nC;

			int iHydraChannel_disc = (iCasis%2 ? 27 : 0 );
#if defined(SPS2017Aug) || defined(SPS2017Nov)
			if(iCasis==49) { //Chip 24 CNb broken

			}
#endif
			disc[iCasis]= (double)inputFile->GetCCUBEdata(iHydra, iHydraChannel_disc)
								- paramTemp->ped[iHydra][iHydraChannel_disc];;
			int iHydraChannel_CN = (iCasis%2 ? 20 : 1 );
#if defined(SPS2017Aug) || defined(SPS2017Nov)
			if(iCasis==49) { //Chip 24 CNb broken
				iHydraChannel_CN = 1;
			}
#endif
			CN[iCasis]=(double)inputFile->GetCCUBEdata(iHydra, iHydraChannel_CN)
								- paramTemp->ped[iHydra][iHydraChannel_CN];
			CNLarge[iCasis] = paramTemp->alphaLarge[iCasis] * CN[iCasis];
			CNSmall[iCasis] = paramTemp->alphaSmall[iCasis] * CN[iCasis];
		}//endl loop over chips
	}//end Compute()
	/// -------------------------------------
	double GetCN(int iCasis, int iCasisChannel, int model=0){
		int iHydraChannel = iCasisChannel+(14*(iCasis%2));
		int iHydra = iCasis/2;
		int sensor = GetSensor(iHydra,iHydraChannel);

		switch(model){
		case 0:
			return commonNoise[iCasis];
		case 1:
			if (sensor==0)    return commonNoiseLarge[iCasis];
			else if(sensor==1)return commonNoiseSmall[iCasis];
			else              return 0.;
		case 2:
			if (sensor==0)     return CNLarge[iCasis];
			else if (sensor==1)return CNSmall[iCasis];
			else if (sensor==1000)  return CNLarge[iCasis];
			else if (sensor==1001)	return CNSmall[iCasis];
			else if (sensor==1000000)    return CNLarge[iCasis];
			else if (sensor==1000001)	 return CNSmall[iCasis];
			else              return 0.;
		}
		return 0.;
	};//end GetCN()
};

struct Hits{
	Int_t   nHits;
	Int_t   hitRow[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitCol[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitLayer[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitGain[NCASISCHIPS*NCASISCHANNELS];
	Int_t   hitSat[NCASISCHIPS*NCASISCHANNELS];
	Float_t hitSignal[NCASISCHIPS*NCASISCHANNELS];

	void Reset(){
		nHits=0;
		for(int i=0; i<NCASISCHIPS*NCASISCHANNELS; i++){
			hitSignal[i]=0.;
			hitRow[i]=0;
			hitCol[i]=0;
			hitLayer[i]=0;
			hitGain[i]=0;
			hitSat[i]=0;
		}
	}

	void Print(){
		for(int ihit=0; ihit<nHits; ++ihit)
			printf("\tHit %d\tLayer %d\tRow %d\tColoumn %d\tSignal %.3f\n",
					ihit, hitLayer[ihit], hitRow[ihit], hitCol[ihit], hitSignal[ihit]);
	}
};

struct Shower{
	int   nHitsLayer[NLAYERS];   //n.hits per layer
	float signalLayer[NLAYERS];  //tot.signal per layer
	float signalHitM[NLAYERS];   //max.signal per layer
	float signalHitC[NLAYERS];   //signal along the central column
	// shower maximum
	int layerHitMax;    //layer of max-hit
	int cubeHitMax;     //cube of max-hit
	float signalHitMax; //signal ofmax-hit
	float totSignal;	//total signal
	int layerStart;

	void Reset(){
		totSignal      = 0.;
		layerStart       = NLAYERS;
		layerHitMax    = -1;
		cubeHitMax     = -1;
		signalHitMax   = 0.;
		for(int il=0; il<NLAYERS; il++){
			signalLayer[il]=0.;
			signalHitM[il]=0.;
			nHitsLayer[il]=0;
			signalHitC[il]=0;
		}
	}
};

#endif
