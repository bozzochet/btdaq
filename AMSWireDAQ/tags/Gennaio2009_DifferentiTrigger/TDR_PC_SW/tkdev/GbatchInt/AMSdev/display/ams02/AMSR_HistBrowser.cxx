//  $Id: AMSR_HistBrowser.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_HistBrowser                                                      //
//                                                                      //
// helper class to browse AMSR_Root Makers histograms.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TBrowser.h>
#include "AMSR_Root.h"
#include "AMSR_Maker.h"
#include "AMSR_HistBrowser.h"

ClassImp(AMSR_HistBrowser)



//_____________________________________________________________________________
AMSR_HistBrowser::AMSR_HistBrowser() 
                : TNamed("Histograms","AMSR_Root Histograms browser")
{

}

//_____________________________________________________________________________
void AMSR_HistBrowser::Browse(TBrowser *b)
{

//printf("Entering Browse, gATLFast=%x\n",gATLFast);
//printf("gATLFast->Makers()=%x\n",gATLFast->Makers());

  TIter next(gAMSR_Root->Makers());
//printf("after TIter definition\n");
  AMSR_Maker *maker;
  while (maker = (AMSR_Maker*)next()) {
//printf("in loop makers\n");
//printf("b=%x, maker=%x, maker->Histograms()=%x, name=%s\n",b, maker, maker->Histograms(),maker->GetName());
     b->Add(maker->Histograms(),maker->GetName());
   }

}
