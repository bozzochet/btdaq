#ifndef __AMSNtupleHelper__
#define __AMSNtupleHelper__

#include "../include/root_RVS.h"
class AMSNtupleHelper{
public:
 AMSNtupleHelper(){};
 virtual bool IsGolden(AMSEventR *o){return true;};
 static AMSNtupleHelper *fgHelper;
};
#endif
