//  $Id: AMSR_Particle.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Root Particle class                                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSR_Root.h"
#include "AMSR_Track.h"
#include "AMSR_Particle.h"
#ifndef ROOT_TClonesArray
#include <TClonesArray.h>
#endif

ClassImp(AMSR_Particle)


/*
//_____________________________________________________________________________
AMSR_Particle::AMSR_Particle(Int_t status, Int_t pattern)
{
   m_Status     = status;
   m_Pattern    = pattern;
}
*/

//_____________________________________________________________________________




void AMSR_Particle::SetHelix(){

   Double_t Bfield = -0.9;	// in minus-x direction of AMS
   Double_t P[3];
   Double_t V[3];
   Double_t Range[2]={-130.,130.};
   Double_t Axis[3]={-1,0,0};
   P[0]=m_Position[0];
   P[1]=m_Position[1];
   P[2]=m_Position[2];
   Double_t bend;
   if(m_Charge)bend = m_Momentum/m_Charge;
   else bend=100000000;
//   if (m_beta<0) bend = -bend;  //included in m_Theta and m_Phi already
   V[0] = bend * TMath::Sin(m_Theta) * TMath::Cos(m_Phi);
   V[1] = bend * TMath::Sin(m_Theta) * TMath::Sin(m_Phi);
   V[2] = bend * TMath::Cos(m_Theta);
     THelix::SetHelix(P,V,0.3*Bfield/100,Range,kHelixX,Axis);
   



   SetLineColor(2);
   SetLineWidth(1);

   return;
}


//_____________________________________________________________________________
void AMSR_Particle::Paint(Option_t *option)
{
   // Paint a particle... just paint its helix part
   //

   Color_t c = this->GetLineColor();
   Width_t w = this->GetLineWidth();
   Int_t   n = this->GetN();
   Float_t * p = this->GetP();
   //debugger.Print("::::: AMSR_Particle::Paint() called: color = %d, width = %d.\n", c, w);
   //debugger.Print("::::: %d points, coordinates:", n);
   //for (Int_t i=0; i<n; i++) debugger.Print(" (%f,%f,%f) ", p[i*3], p[i*3+1], p[i*3+2]);
   THelix::Paint(option);

}

//_____________________________________________________________________________
char * AMSR_Particle::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[255];
   info[0]='\0';
   sprintf(info,"GID=%d  m=%4.1f+-%4.2g, p=%4.1f+-%4.2g, pat=%d, status=%x, beta=%4.2f+-%4.2g, betapat=%d, charge=%4.1f",
		 m_GeantID, m_Mass, m_ErrMass, m_Momentum, m_ErrMomentum,
                 m_pattern,m_trstatus,m_beta,m_errbeta,m_betapattern,m_Charge);

   debugger.Print("AMSR_Particle::GetObjectInfo():  %s\n", info);
   return info;

}

