//  $Id: AMSR_AntiClusterReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Anti Counter Cluster Reader class.                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_AntiCluster.h"
#include "AMSR_AntiClusterReader.h"
#include "AMSR_Ntuple.h"


ClassImp(AMSR_AntiClusterReader)


//_____________________________________________________________________________
AMSR_AntiClusterReader::AMSR_AntiClusterReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_AntiClusterReader
   //

   m_Fruits     = new TClonesArray("AMSR_AntiCluster",8,kFALSE);
   m_BranchName = "AntiClusters";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   //
   // Mark that we want to save things
   //
   Save();

}


//_____________________________________________________________________________
void AMSR_AntiClusterReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Make()
{
// Make the branch in result tree
//

   Int_t k;
   ANTICLUS_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkAnticlus;

//........................................................
//....Store clusters in Root ClonesArray
//........................................................
//   m_Ncells    = ncells;
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_AntiClusterReader::Make(): making %d clusters.\n",
	  _ntuple->nanti);
   for (k=0; k<_ntuple->nanti; k++) {
      AddCluster(_ntuple->Antistatus[k],
                 _ntuple->Antisector[k],
                 _ntuple->Antiedep[k],
                &_ntuple->Anticoo[k][0],
                &_ntuple->Antiercoo[k][0]);
   }
/*
{
   float coo[3]={53.8,-10.7,-12.59};
   float ecoo[3]={-1.21,6.1,20.};
  AddCluster(0,16,0.37,&coo[0],&ecoo[0]);
}
*/
   debugger.Print("AMSR_AntiClusterReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

/*
//_____________________________________________________________________________
Bool_t AMSR_AntiClusterReader::Enabled(TObject * obj)
{
   return kTRUE;		// no discrimation on CTC Clusters
}
*/

//_____________________________________________________________________________
void  AMSR_AntiClusterReader::AddCluster(Int_t status, Int_t sector, 
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
  Float_t cooA[3];
  Float_t ercooA[3];
  cooA[0]=coo[0]*cos(coo[1]/180.*3.14159);
  cooA[1]=coo[0]*sin(coo[1]/180.*3.14159);
  cooA[2]=coo[2];
  ercooA[0]=coo[0]*cos(coo[1]/180.*3.14159)*ercoo[1]/180.;
  ercooA[1]=coo[0]*sin(coo[1]/180.*3.14159)*ercoo[1]/180.;
  ercooA[2]=ercoo[2];
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   debugger.Print("--- AMSR_AntiClusterReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSR_AntiCluster(status, sector, 
	signal,cooA, ercooA, ntracks, tracks);

}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_AntiClusterReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






