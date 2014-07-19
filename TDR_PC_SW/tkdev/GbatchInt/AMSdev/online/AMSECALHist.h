#include "AMSHist.h"

class AMSECALHist : public AMSHist{
public:
AMSECALHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){};
void Book();
void ShowSet(int );
void Fill(AMSNtupleR *ntuple);
};

