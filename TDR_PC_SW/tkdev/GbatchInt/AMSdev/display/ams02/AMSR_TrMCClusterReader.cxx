//  $Id: AMSR_TrMCClusterReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TrMC Counter Cluster Reader class.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>
#include <iostream.h>
#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_TrMCCluster.h"
#include "AMSR_TrMCClusterReader.h"
#include "AMSR_Ntuple.h"


ClassImp(AMSR_TrMCClusterReader)



//
// struct to read data from tree converted from ntuple
//

static const Int_t maxtrmc=200;

//_____________________________________________________________________________
AMSR_TrMCClusterReader::AMSR_TrMCClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_TrMCClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_TrMCCluster",8,kFALSE);
   m_BranchName = "TrMCClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSR_TrMCClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_TrMCClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;
   TRMCCLUS_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTrmcclus;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_TrMCClusterReader::Make(): making %d clusters.\n",
	  _ntuple->ntrclmc);
   for (k=0; k<_ntuple->ntrclmc; k++) {
      if(_ntuple->Itra[k]!=555)AddCluster(_ntuple->Itra[k],
                 _ntuple->summc[k],
                &_ntuple->xgl[k][0],
                &_ntuple->xcb[k][0]);   //guess the match tentatively
//?? match which ??                &_ntuple.TrMCErCoo[k][0]);
   }

   debugger.Print("AMSR_TrMCClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_TrMCClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_TrMCClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSR_TrMCClusterReader::AddCluster(Int_t part,  
                             Float_t signal, 
                             Float_t * coo, Float_t * ercoo,
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
  // Convert to other coo system
  Float_t ercooA[3];
  ercooA[0]=0.2;
  ercooA[1]=0.2;
  ercooA[2]=signal*20000;
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   debugger.Print("--- AMSR_TrMCClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSR_TrMCCluster(part, 
	signal*10000,coo, ercooA, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSR_TrMCClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_TrMCClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






