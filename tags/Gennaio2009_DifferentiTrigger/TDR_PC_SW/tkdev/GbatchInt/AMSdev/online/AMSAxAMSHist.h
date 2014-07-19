//  $Id: AMSAxAMSHist.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef __AMSAxAMSHIST__
#define __AMSAxAMSHIST__
#include "AMSHist.h"

class AMSAxAMSHist : public AMSHist{
public:
AMSAxAMSHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};


#endif
