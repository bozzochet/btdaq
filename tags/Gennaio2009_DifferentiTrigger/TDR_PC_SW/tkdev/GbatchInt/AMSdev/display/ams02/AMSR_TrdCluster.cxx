//  $Id: AMSR_TrdCluster.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS TRD Cluster                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Debugger.h"
#include <iostream.h>
#include "AMSR_TrdCluster.h"
#include "AMSR_Root.h"
#include "AMSR_VirtualDisplay.h"

ClassImp(AMSR_TrdCluster)


//_____________________________________________________________________________
AMSR_TrdCluster::AMSR_TrdCluster(Float_t* coo, Float_t* ercoo)
	: AMSR_3DCluster(coo, ercoo, 0, 1)
{

   SetDirection(0.0, 0.0);
   SetLineWidth(3);
   SetLineColor(46);  // Dark red   
   SetFillColor(46);
   SetFillStyle(1001);          // solid filling (not working now....)

   debugger.Print("AMSR_TrdCluster::AMSR_TrdCluster(): position (%f,%f,%f)+-(%f,%f,%f)\n",
	coo[0], coo[1], coo[2], ercoo[0], ercoo[1], ercoo[2]);
}

//______________________________________________________________________________
char *AMSR_TrdCluster::GetObjectInfo(Int_t px, Int_t py) const
{
   static char info[64];
   sprintf(info,"Trd");
   return info;
}

//_____________________________________________________________________________
void AMSR_TrdCluster::Paint(Option_t *option)
{

  /*
  if (gAMSR_Root->Display()->DrawClusters()) {
    gAMSR_Root->Display()->PaintFruit(this, m_Position, m_ErrPosition, 2, option);
  }
  */

  AMSR_3DCluster::Paint(option);
}

void AMSR_TrdCluster::Clear(Option_t *option)
{
//
}


/*
//______________________________________________________________________________
void AMSR_TrdCluster::Sizeof3D() const
{
   gAMSR_Root->Display()->SizeFruit();
}
*/
