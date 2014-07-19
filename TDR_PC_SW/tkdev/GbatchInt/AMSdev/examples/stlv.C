///   \example tracc.C
///
#define tracc_cxx
#include "../include/root_RVS.h"
#include "TF1.h"
#include "TH2.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TEventList.h"
#include "TSelector.h"
#include "TChain.h"
#include <TROOT.h>
#include <TTree.h>
/** 
 * This is an example of how to work with AMS Root Files. 
 *  \sa tracc
 */



///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the AMSEventR class. \n

 The following member functions must be called by the user: \n
    void UBegin():       called everytime a loop on the tree starts,
                         a convenient place to create your histograms. \n
    void UProcessFill(): called in the entry loop for all entries \n
    void UTerminate():   called at the end of a loop on the tree,
                         a convenient place to draw/fit your histograms. \n
 
 The following member function is optional: \n
    bool UProcessCut(): called in the entry loop to accept/reject an event \n

   To use this file, try the following session on your Tree T: \n
 Root > T->Process("tracc.C+") -> compile library if not up to date \n
 Root > T->Process("tracc.C++") -> force always compilation \n
 Root > T->Process("tracc.C")  -> non-compiled (interpreted) mode \n
*/

class tracc : public AMSEventR {
   public :

   static  vector<TH1F*>   h1A;   ///< An (alt) way of having array of histos
   tracc(TTree *tree=0){ };

   ~tracc()  {
   }

/// This is necessary if you want to run without compilation as:
///     chain.Process("tracc.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

   /// User Function called before starting the event loop.
   /// Book Histos
   virtual void    UBegin();

   /// User Function called to preselect events (NOT MANDATORY).
   /// Called for all entries.
   virtual bool    UProcessCut();

   /// User Function called to analyze each event.
   /// Called for all entries.
   /// Fills histograms.
   virtual void    UProcessFill();

   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();
};

vector<TH1F*>   tracc::h1A;

//create here pointers to histos and/or array of histos

    TH1F * acc[20]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; 
     TH1F * h1=0;
     TH1F * h2=0;
      TH1F * h3=0;
       TH1F *h4=0;
       TH1F *h5=0;

       



void tracc::UBegin(){
    cout << " Begin calling"<<endl;

   //here create histograms

   float al1=log(0.5);
   float al2=log(8000.);
   int nch=24;
    for(unsigned int i=0;i<h1A.size();i++)delete h1A[i];
        h1A.clear();

 
    for (unsigned int j = 0; j < sizeof(acc)/sizeof(acc[0]); j++) {
      char AccName[255];
      sprintf(AccName,"acc%03d",j);
      if(acc[j])delete acc[j];
      acc[j] = new TH1F(AccName,"acceptance",nch,al1,al2);
//
//    another way of def histos
//

      
      sprintf(AccName,"accv%03d",j);
      h1A.push_back(new TH1F(AccName,"acceptance",nch,al1,al2));         
   }
    char *name="Layers";
   if(h1)delete h1;
   h1=new TH1F(name,name,10,-0.5,9.5);
   if(h2)delete h2;
    h2=new TH1F("a","a",200,0.,1);
    if(h4)delete h4;
    h4=new TH1F("a","a",200,0.,0.5);
  if(h3)delete h3;
    h3=new TH1F("a","a",300,135,165.);
  if(h5)delete h5;
    h5=new TH1F("a","a",200,0.,1);

    cout << " Begin called now"<<endl;
    
   

}

// UProcessCut() is not mandatory
bool tracc::UProcessCut(){
     if (nParticle()!=1) return false;
     return true;
}

void tracc::UProcessFill()
{
   // User Function called for all entries .
   // Entry is the entry number in the current tree.
   // Fills histograms.
    //cout << "Event "<<Event()<<endl;
    Float_t xm=0;
    if(nMCEventg()>0){		
     MCEventgR mc_ev=MCEventg(0);
      xm = log(mc_ev.Momentum);
      acc[0]->Fill(xm,1);
      h1A[0]-> Fill(xm,1);
      cout <<Run()<< "  " <<Event()<< "  "<<mc_ev.Momentum<<"  "<<MCEventg(1).Momentum<<"  "<< MCEventg(1).Coo[2]<<" "<<endl;
      cout <<"  Particle "<<Particle(0).Momentum<<"  "<<NBeta()<<endl;
      for (int k=0;k<nTrTrack();k++){
         cout <<"  tracke "<<k<<" "<<TrTrack(k).IsGood()<<endl;
      }

      h2->Fill(Particle(0).Momentum/mc_ev.Momentum,1);
      float xx=pow(float(fabs(Particle(0).Momentum/mc_ev.Momentum)),float(2.7));
      h4->Fill(xx,1);
     h5->Fill(Particle(0).Momentum/mc_ev.Momentum,xx);
       cout << xx<<endl;
      h3->Fill(MCEventg(1).Coo[2],1);
/*
       int ng=nEcalHit();
      if(ng>0){
        EcalHitR ec=EcalHit(ng-1);
          for(int kk=0;kk<10000000;kk++){
              acc[1]->Fill(xm,1);
           }                                                                                      //cout <<" ne "<<ng<<" "<<ec.Edep<<endl;
}
*/
     if(nParticle()>0){
       int ptrack = Particle(0).iTrTrack();
       int ptrd = Particle(0).iTrdTrack();
       if(NParticle()== 1 && ptrack>=0 && ptrd>=0){ //final if
         acc[1]->Fill(xm,1);
         h1A[1]-> Fill(xm,1);
        int pbeta = Particle(0).iBeta();   
        BetaR *pb =  Particle(0).pBeta();   // another way 
        if(pbeta>=0){			//check beta
	  BetaR BetaI=Beta(pbeta);
          if(fabs(BetaI.Beta) < 2 && pb->Chi2S < 5 && BetaI.Pattern < 4){
	    if(nTrdTrack()<2){
	    Int_t Layer1 =0;
            Int_t Layer2 =0;  
             TrTrackR tr_tr=TrTrack(ptrack);
		 int  ptrh=tr_tr.iTrRecHit(0);			//pht1
   	          Layer1=TrRecHit(ptrh).Layer;
		    
		 
		  ptrh=tr_tr.iTrRecHit(tr_tr.NTrRecHit()-1);			//pht2
   	           Layer2=TrRecHit(ptrh).Layer;

 // alt method

                  for (int k=0;k<tr_tr.NTrRecHit();k++){
                     h1->Fill(tr_tr.pTrRecHit(k)->Layer,1);
                   }
}
}
}
}
}
}
}

void tracc::UTerminate()
{
  
}

