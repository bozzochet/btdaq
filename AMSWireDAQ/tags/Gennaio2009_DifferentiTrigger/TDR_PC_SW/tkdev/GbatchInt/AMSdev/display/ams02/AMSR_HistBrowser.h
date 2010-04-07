//  $Id: AMSR_HistBrowser.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_HistBrowser_H
#define AMSR_HistBrowser_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_HistBrowser                                                       //
//                                                                      //
// helper class to browse AMSR_Root Makers histograms.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class AMSR_HistBrowser : public TNamed {

public:
                     AMSR_HistBrowser();
   virtual          ~AMSR_HistBrowser() {;}
   virtual void      Browse(TBrowser *b);
   Bool_t            IsFolder() {return kTRUE;}

   ClassDef(AMSR_HistBrowser, 0)   //helper class to browse AMSR_Root Makers histograms
};

#endif
