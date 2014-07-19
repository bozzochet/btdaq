//  $Id: AMSR_SiHit.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Silicon Tracker Hits                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AMSR_SiHit.h"
#include "AMSR_Root.h"
#include "AMSR_VirtualDisplay.h"
#include "Debugger.h"

ClassImp(AMSR_SiHit)


//_____________________________________________________________________________
AMSR_SiHit::AMSR_SiHit(Int_t status, Int_t plane, Int_t px, Int_t py,
                                Float_t * hit, Float_t * errhit, 
                                Float_t ampl, Float_t asym,
                                TObjArray * tracks)
	: AMSR_3DCluster(hit, errhit, 0, 2)
{
   m_Status      = status;
   m_Plane       = plane;
   m_X           = px;
   m_Y           = py;
   for (Int_t i=0; i<3; i++) {
//   m_Position[i]    = hit[i];
     m_ErrPosition[i] = errhit[i];			// the real size
   }
   m_Amplitude   = ampl;
   m_AsymAmpl    = asym;

   m_Tracks      = tracks;

   SetDirection(0.0, 0.0);
						// magnify for visual effect
   TMarker3DBox::SetSize(errhit[0]<0.5?errhit[0]*300.0:errhit[0], errhit[1]*300.0, errhit[2]*300.0);
//   if (m_Plane==1 || m_Plane==4 || m_Plane==5)  // face down
//      TMarker3DBox::SetPosition(hit[0], hit[1], hit[2]-errhit[2]*300.0);
//   else                                         // face up
      TMarker3DBox::SetPosition(hit[0], hit[1], hit[2]+errhit[2]*300.0);

   SetLineWidth(1);
   SetLineColor(4);		// dark blue
   SetFillColor(4);
   SetFillStyle(1001);		// solid filling (not working now....)

   debugger.Print("AMSR_SiHit(%lx,%d bytes): position (%f,%f,%f)+-(%f,%f,%f)\n",
	this, sizeof(*this), hit[0], hit[1], hit[2], errhit[0], errhit[1], errhit[2]);

}

//______________________________________________________________________________
char *AMSR_SiHit::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[64];
   sprintf(info,"Si Layer %d: Ampl=%4.1f, at (%5.1f,%5.1f,%5.1f)+-(%5.3f,%5.3f,%5.3f, asym=%4.1f, status=%x)",
		m_Plane, 		// m_X, m_Y, 
		m_Amplitude, 
		fX, fY, fZ,
		m_ErrPosition[0], m_ErrPosition[1], m_ErrPosition[2],m_AsymAmpl,m_Status);
   return info;
}

//_____________________________________________________________________________
void AMSR_SiHit::Paint(Option_t *option)
{

  /*
  if (gAMSR_Root->Display()->DrawClusters()) {
    Float_t erxyz[3];
    for (Int_t i=0; i<3; i++) 
      erxyz[i] = m_ErrPosition[i] * 100;  // have to enlarge
    gAMSR_Root->Display()->PaintFruit(this, m_Position, erxyz, 4, option);
  }
  */

  AMSR_3DCluster::Paint(option);

}

void AMSR_SiHit::Clear(Option_t *option)
{
  m_Status = -1;
  m_Plane  = m_X = m_Y = 0;
  m_Tracks = 0;
}


/*
//______________________________________________________________________________
void AMSR_SiHit::Sizeof3D() const
{
   gAMSR_Root->Display()->SizeFruit();
}
*/
