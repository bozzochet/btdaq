//  $Id: AMSR_MCParticle.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include <iostream.h>
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSR_Root.h"
#include "AMSR_Track.h"
#include "AMSR_MCParticle.h"
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

ClassImp(AMSR_MCParticle)


/*
//_____________________________________________________________________________
AMSR_MCParticle::AMSR_MCParticle(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}
*/

//_____________________________________________________________________________




void AMSR_MCParticle::SetHelix(){

   Double_t Bfield = -0.9;
   Double_t P[3];
   Double_t V[3];
   Double_t Range[2]={-100.,100.};
   Double_t Axis[3]={-1,0,0};
   P[0]=m_Position[0];
   P[1]=m_Position[1];
   P[2]=m_Position[2];
   float rig=m_Charge!=0?m_Momentum/m_Charge:1000000;
   V[0]=rig *m_Dir[0];
   V[1]=rig *m_Dir[1];
   V[2]=rig*m_Dir[2];

  THelix::SetHelix(P,V,0.3*Bfield/100,Range,kHelixX,Axis);

   SetLineColor(5);
   SetLineWidth(1);

   return;
}


//_____________________________________________________________________________
void AMSR_MCParticle::Paint(Option_t *option)
{
   // Paint a particle... just paint its helix part
   //

   Color_t c = this->GetLineColor();
   Width_t w = this->GetLineWidth();
   Int_t   n = this->GetN();
   Float_t * p = this->GetP();
   //debugger.Print("::::: AMSR_MCParticle::Paint() called: color = %d, width = %d.\n", c, w);
   //debugger.Print("::::: %d points, coordinates:", n);
   //for (Int_t i=0; i<n; i++) debugger.Print(" (%f,%f,%f) ", p[i*3], p[i*3+1], p[i*3+2]);
   THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSR_MCParticle::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[255];
   info[0]='\0';
   sprintf(info,"GID=%d  m=%4.1f, p=%4.1f,  charge=%4.1f",
		 m_GeantID, m_Mass, m_Momentum, m_Charge);

   return info;

}

