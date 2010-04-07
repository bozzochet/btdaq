//  $Id: AMSR_ToFCluster.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <iostream.h>
#include "AMSR_ToFCluster.h"
#include "AMSR_Root.h"
#include "AMSR_VirtualDisplay.h"

ClassImp(AMSR_ToFCluster)


//_____________________________________________________________________________
AMSR_ToFCluster::AMSR_ToFCluster(Int_t status, Int_t plane, Int_t bar,
			     Float_t energy, Float_t time, Float_t ertime,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ncells, Int_t nparticles,
                             Int_t ntracks, TObjArray * tracks)
	: AMSR_3DCluster(coo, ercoo, 0, 1)
{
   m_Status     = status;
   m_Plane      = plane;
   m_Bar        = bar;
   m_Energy     = energy;
   m_Time       = time;
   m_ErTime     = ertime;
// for ( int i=0; i<3; i++) {
//   m_Position[i]    = coo[i];
//   m_ErrPosition[i] = ercoo[i];
// }
   m_Ncells  = ncells;
   m_Nparticles = nparticles;
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(3);
   SetLineColor(3);             // light green
   SetFillColor(3);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSR_ToFCluster::AMSR_ToFCluster(): position (%f,%f,%f)+-(%f,%f,%f)\n",
	coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSR_ToFCluster::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[64];
   sprintf(info,"ToF S%dB%d: time=%3.1f+-%3.1f, Edep=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Plane, m_Bar,  m_Time*1.e9, m_ErTime*1.e9,m_Energy,
		fX, fY, fZ,
		fDx, fDy, fDz );
   return info;
}

//_____________________________________________________________________________
void AMSR_ToFCluster::Paint(Option_t *option)
{

  /*
  if (gAMSR_Root->Display()->DrawClusters()) {
    gAMSR_Root->Display()->PaintFruit(this, m_Position, m_ErrPosition, 2, option);
  }
  */

  AMSR_3DCluster::Paint(option);
}

void AMSR_ToFCluster::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = m_Bar = 0;
  m_Energy = 0;
  m_Tracks = 0;
}


/*
//______________________________________________________________________________
void AMSR_ToFCluster::Sizeof3D() const
{
   gAMSR_Root->Display()->SizeFruit();
}
*/
