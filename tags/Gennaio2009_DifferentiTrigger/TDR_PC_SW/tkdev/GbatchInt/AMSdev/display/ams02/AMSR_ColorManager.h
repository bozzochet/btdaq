//  $Id: AMSR_ColorManager.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_ColorManager_H
#define AMSR_ColorManager_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_ColorManager                                                       //
//                                                                      //
// class to manage colors used by AMSR_Root event display program.        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

static const int MaxColor = 256;

class AMSR_ColorManager : public TNamed {

protected:
//	struct { Float_t r; Float_t g; Float_t b; } colorTable[MaxColor];
	Float_t red[MaxColor];
	Float_t green[MaxColor];
	Float_t blue[MaxColor];

public:
                     AMSR_ColorManager(char * name="AMSR_ColorManager", char * title="AMS Event Display Color Manager");
   virtual          ~AMSR_ColorManager() {;}
            Int_t    GetColorAt(Int_t i, Float_t *r, Float_t *g, Float_t *b);
           Bool_t    IsValid(Int_t i);
	    Int_t    Migrate(Int_t src, Int_t dest);
	     void    ColorTest(Int_t begin=0, Int_t end=MaxColor-1);

   ClassDef(AMSR_ColorManager, 0)   //helper class to browse AMSR_Root Makers histograms
};

#endif
