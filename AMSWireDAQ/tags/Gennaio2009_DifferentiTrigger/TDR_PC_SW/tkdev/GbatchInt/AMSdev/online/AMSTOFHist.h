//  $Id: AMSTOFHist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSTOFHIST__
#define __AMSTOFHIST__
#include "AMSHist.h"
#include "AMSHist.h"

const Float_t toftrange[3]={120,120,120};//mins,hours,days

class AMSTOFHist : public AMSHist{
public:
AMSTOFHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};



#endif
