#include "AMSHist.h"

class AMSRICHHist : public AMSHist{
public:
AMSRICHHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};
