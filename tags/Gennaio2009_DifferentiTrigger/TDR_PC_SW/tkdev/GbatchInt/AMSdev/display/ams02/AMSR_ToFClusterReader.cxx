//  $Id: AMSR_ToFClusterReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster Reader class.                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <iostream.h>
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_ToFCluster.h"
#include "AMSR_ToFClusterReader.h"
#include "AMSR_Ntuple.h"

/*
#include "ATLFMCMaker.h"

const Float_t kPI  = TMath::Pi();
const Float_t k2PI = 2*kPI;
const Int_t kMAXCELLS = 2000;
const Int_t kMAXCLU   = 1000;
*/

const Int_t maxtof=200;
ClassImp(AMSR_ToFClusterReader)


//_____________________________________________________________________________
AMSR_ToFClusterReader::AMSR_ToFClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_ToFClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_ToFCluster",maxtof,kFALSE);
   m_BranchName = "ToFClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}


//_____________________________________________________________________________
void AMSR_ToFClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_ToFClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;
   TOFCLUST_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTofclust;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_ToFClusterReader::Make(): making %d clusters.\n",
	  _ntuple->ntof);
   for (k=0; k<_ntuple->ntof; k++) {
   debugger.Print("AMSR_ToFClusterReader::Make(): making cluster %d %d.\n",
	  k,_ntuple->plane[k]);

      AddCluster(_ntuple->Tofstatus[k],
                 _ntuple->plane[k],
                 _ntuple->bar[k],
                 _ntuple->Tofedep[k],
                 _ntuple->Toftime[k],
                 _ntuple->Tofetime[k],
                &_ntuple->Tofcoo[k][0],
                &_ntuple->Tofercoo[k][0]);
   }
/*
{
   float coo[3]={-4.165,-63.25,-65.8};
   float ecoo[3]={2.6,3.46,0.};

   AddCluster(0,4,1,0.83,1.24e-8,1.e-8,&coo[0],&ecoo[0]);
}
{
   float coo[3]={-23.5,-51.75,-64.35};
   float ecoo[3]={2.6,3.46,0.};

   AddCluster(0,4,2,0.45,1.64e-8,1.e-8,&coo[0],&ecoo[0]);
}
*/
   debugger.Print("AMSR_ToFClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_ToFClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_ToFClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation among ToF clusters
}
*/


//_____________________________________________________________________________
void  AMSR_ToFClusterReader::AddCluster(Int_t status, Int_t plane, Int_t bar,
                                Float_t energy, Float_t time, Float_t ertime,
                                Float_t * coo, Float_t * ercoo,
                                Int_t ncells, Int_t npart,
                                Int_t ntracks, TObjArray * tracks)
{
//            Add a new cluster to the list of clusters

 //Note the use of the "new with placement" to create a new cluster object.
 //This complex "new" works in the following way:
 //   clusters[i] is the value of the pointer for cluster number i in the TClonesArray
 //   if it is zero, then a new cluster must be generated. This typically
 //   will happen only at the first events
 //   If it is not zero, then the already existing object is overwritten
 //   by the new cluster parameters.
 // This technique should save a huge amount of time otherwise spent
 // in the operators new and delete.

  //cout << "ene "<<energy<<endl;
   Float_t cooA[3];
   Float_t ercooA[3];
   cooA[0]=coo[0];
   cooA[1]=coo[1];
   cooA[2]=coo[2];
   ercooA[0]=ercoo[0];
   ercooA[1]=ercoo[1];
//   ercooA[2]=energy/2;
   ercooA[2]=sqrt(energy/2);

   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   new(clusters[m_Nclusters++]) AMSR_ToFCluster(status, plane, bar, 
	energy, time, ertime, cooA, ercooA, ncells, npart, ntracks, tracks);
}

//_____________________________________________________________________________
void AMSR_ToFClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_ToFClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






