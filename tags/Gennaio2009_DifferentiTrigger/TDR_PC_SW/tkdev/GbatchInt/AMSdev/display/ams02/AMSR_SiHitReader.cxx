//  $Id: AMSR_SiHitReader.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Tracker Hit Reader class.                                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <TRandom.h>

#include <TClonesArray.h>
//#include <TMCParticle.h>

#include "AMSR_Root.h"
#include "AMSR_SiHit.h"
#include "AMSR_SiHitReader.h"
#include "AMSR_Ntuple.h"

static const Int_t MaxSiHits = 500;

ClassImp(AMSR_SiHitReader)


//_____________________________________________________________________________
AMSR_SiHitReader::AMSR_SiHitReader(const char *name, const char *title)
                 : AMSR_Maker(name,title)
{
   // Construct an AMSR_SiHitReader
   //

   m_Fruits     = new TClonesArray("AMSR_SiHit",MaxSiHits,kFALSE);
   m_BranchName = "SiliconHits";
   m_Nclusters  = 0;
//   m_Ncells     = 0;

   DrawUsedHitsOnly = kFALSE;

   //
   // Mark that we want to save things
   //
   Save();

}

//_____________________________________________________________________________
void AMSR_SiHitReader::Finish()
{
// Function called when maker for all events have been called
// close histograms... etc.

}

//_____________________________________________________________________________
void AMSR_SiHitReader::Make()
{
// Make the branch in result tree
//

   Int_t k;
   TRRECHIT_DEF *_ntuple = (gAMSR_Root->GetNtuple())->m_BlkTrrechit;


//........................................................
//....Store clusters in Root ClonesArray
//........................................................
   m_Nclusters = 0;		// it will be accumulated by AddCluster()
   debugger.Print("AMSR_SiHitReader::Make(): making %d clusters.\n",
	  _ntuple->ntrrh);
   for (k=0; k<_ntuple->ntrrh; k++) {
      AddCluster(_ntuple->statusr[k],
                 _ntuple->Layer[k],
                 _ntuple->px[k],
                 _ntuple->py[k],
                &_ntuple->hitr[k][0],
                &_ntuple->ehitr[k][0],
                 _ntuple->sumr[k],
                 _ntuple->difosum[k]);
   }

   debugger.Print("AMSR_SiHitReader::Make(): %d clusters made.\n", m_Nclusters);
}

//_____________________________________________________________________________
void AMSR_SiHitReader::PrintInfo()
{
   AMSR_Maker::PrintInfo();
}

//_____________________________________________________________________________
Bool_t AMSR_SiHitReader::Enabled(TObject * obj)
{
  //
  // a hit with status 32 have a track passing through
  //
  AMSR_SiHit * hit = (AMSR_SiHit *) obj;
  Int_t status = hit->GetStatus();
  // debugger.Print("SiHit status = %d\n", status);
  if ( DrawUsedHitsOnly && !(status & 32) ) return kFALSE;
  else                                    return kTRUE;
}

//_____________________________________________________________________________
void  AMSR_SiHitReader::AddCluster(Int_t status, Int_t plane, Int_t px, Int_t py,
                             Float_t * hit, Float_t * errhit,
                             Float_t ampl, Float_t asym,
                             TObjArray * tracks)
{
Float_t errhitA[3];
Float_t hitA[3];
errhitA[0]=errhit[0];
errhitA[1]=errhit[1];
//errhitA[2]=ampl/10000;
errhitA[2]=sqrt(ampl)/1000;
hitA[0]=hit[0];
hitA[1]=hit[1];
//hitA[2]=hit[2]+ampl/30;
hitA[2]=hit[2];
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

   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   debugger.Print("--- AMSR_SiHitReader: will add a cluster at %0lx\n", clusters[m_Nclusters]);
   new(clusters[m_Nclusters++]) AMSR_SiHit(status, plane, px, py, 
	hitA, errhitA, ampl, asym, tracks);
}

//_____________________________________________________________________________
void AMSR_SiHitReader::RemoveCluster(const Int_t cluster)
{
   TClonesArray &clusters = *(TClonesArray*)m_Fruits;
   clusters.RemoveAt(cluster);
   clusters.Compress();
   m_Nclusters--;
}

//_____________________________________________________________________________
void AMSR_SiHitReader::Clear(Option_t *option)
{
//    Reset Cluster Maker

   m_Nclusters  = 0;
//   m_Ncells     = 0;
   if (m_Fruits) m_Fruits->Clear();
}






