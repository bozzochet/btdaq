//  $Id: AMSGenHist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSGENHIST__
#define __AMSGENHIST__
#include "AMSHist.h"

#include "AMSHist.h"

class AMSGenHist : public AMSHist{
public:
AMSGenHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};



#endif
