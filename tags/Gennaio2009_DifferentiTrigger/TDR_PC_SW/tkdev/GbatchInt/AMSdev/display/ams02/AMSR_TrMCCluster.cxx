//  $Id: AMSR_TrMCCluster.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TrMC Counter Cluster                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include "AMSR_TrMCCluster.h"
#include "AMSR_Root.h"
#include "AMSR_VirtualDisplay.h"

ClassImp(AMSR_TrMCCluster)


//_____________________________________________________________________________
AMSR_TrMCCluster::AMSR_TrMCCluster(Int_t part, Float_t signal, 
                                Float_t * coo, Float_t * ercoo,
                                Int_t ntracks, TObjArray * tracks)
	: AMSR_3DCluster(coo,ercoo,0,5)
{
   m_Part     = part;
   m_Signal     = signal;
   m_NTracks    = ntracks;
   m_Tracks     = tracks;

   SetDirection(0.0, 0.0);
   SetLineWidth(1);
   SetLineColor(5);             // light blue
   SetFillColor(5);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSR_TrMCCluster(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSR_TrMCCluster::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[90];
   sprintf(info,"TrMC: Particle=%d Signal=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.1f,%5.1f,%5.1f)",
		m_Part, m_Signal, 
		fX, fY, fZ,
		fDx, fDy, fDz );
	//	m_Position[0], m_Position[1], m_Position[2],
	//	m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2] );
   return info;
}

//_____________________________________________________________________________
void AMSR_TrMCCluster::Paint(Option_t *option)
{
  /*
  if (gAMSR_Root->Display()->DrawClusters()) {
    gAMSR_Root->Display()->PaintFruit(this, m_Position, m_ErrPosition, 5, option);
  }
  */

  AMSR_3DCluster::Paint(option);

}

void AMSR_TrMCCluster::Clear(Option_t *option)
{
  m_Part = 0;
  m_Tracks = 0;
}

/*
//______________________________________________________________________________
void AMSR_TrMCCluster::Sizeof3D() const
{
   gAMSR_Root->Display()->SizeFruit();
}
*/
