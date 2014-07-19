
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root TrdTrack class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include "AMSR_TrdTrack.h"

ClassImp(AMSR_TrdTrack)


//_____________________________________________________________________________
AMSR_TrdTrack::AMSR_TrdTrack(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}

//_____________________________________________________________________________
void AMSR_TrdTrack::SetHelix()
{
   Double_t Bfield = 0.0001;	// in minus-x direction of AMS
   Double_t P0[3];
   Double_t V0[3];
   Double_t Axis[3]={-1,0,0};
   Double_t Range[2]={-160.,-80.};
     P0[0]=m_Coo[0];
     P0[1]=m_Coo[1];
     P0[2]=m_Coo[2];
     V0[0]=TMath::Sin(m_Theta) * TMath::Cos(m_Phi);
     V0[1]=TMath::Sin(m_Theta) * TMath::Sin(m_Phi);
     V0[2]=TMath::Cos(m_Theta);
     THelix::SetHelix(P0,V0,0.3*Bfield/100,Range,kHelixX,Axis);

   SetLineColor(7);	// Cyan
   SetLineWidth(1);

   return;
}

//_____________________________________________________________________________
void AMSR_TrdTrack::Paint(Option_t *option)
{
  // Paint an AMS TrdTrack 
  //

  // special treatment on dummy TrdTracks which has status word 17 (a la Vitaly)
  //
  //if ( m_Status != 17 )
     THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSR_TrdTrack::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[255];
   sprintf(info,"#%d, (theta,phi)=(%4.1f,%4.1f), pattern=%d", m_ID,
		  m_Theta, 
		 m_Phi, 
		 m_Pattern);

   return info;

}

