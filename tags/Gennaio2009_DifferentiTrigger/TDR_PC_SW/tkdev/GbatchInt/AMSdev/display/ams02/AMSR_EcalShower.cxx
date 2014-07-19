//  $Id: AMSR_EcalShower.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Track class                                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"

#include "AMSR_EcalShower.h"

ClassImp(AMSR_EcalShower)


//_____________________________________________________________________________

//_____________________________________________________________________________
void AMSR_EcalShower::SetHelix()
{
   Double_t Bfield = -0.9;	// in minus-x direction of AMS
   Double_t P0[3];
   Double_t V0[3];
   Double_t Axis[3]={-1,0,0};
   Double_t Range[2]={125,165};
     P0[0]=m_FP0[0];
     P0[1]=m_FP0[1];
     P0[2]=m_FP0[2];
     V0[0]=m_Energy * TMath::Sin(m_FTheta) * TMath::Cos(m_FPhi);
     V0[1]=m_Energy * TMath::Sin(m_FTheta) * TMath::Sin(m_FPhi);
     V0[2]=m_Energy * TMath::Cos(m_FTheta);
     THelix::SetHelix(P0,V0,0.3*Bfield/10000,Range,kHelixX,Axis);

   SetLineColor(12);	// dark grey
   int lw=pow(m_Energy,0.5);
   if(lw<1)lw=1;
   SetLineWidth(lw);

   return;
}

//_____________________________________________________________________________
void AMSR_EcalShower::Paint(Option_t *option)
{
  // Paint an AMS track 
  //

  // special treatment on dummy tracks which has status word 17 (a la Vitaly)
  //
  //if ( m_Status != 17 )
     THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSR_EcalShower::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[255];
   sprintf(info,"#%d, (energy,theta,phi)=(%4.1lf+-%4.1lf,%4.1f,%4.1f), Status=%d, Chi2=%5.1f", m_ID,m_Energy,m_ErrEnergy,m_FTheta,m_FPhi,m_Status,m_FChi2);

   return info;

}

