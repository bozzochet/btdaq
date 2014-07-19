#ifndef  __VCon_gb__
#define  __VCon_gb__
#include "VCon.h"
#include "cont.h"

class VCon_gb :public VCon{

public:
  AMSContainer * con;
  VCon_gb(AMSContainer * cc=0);
  ~VCon_gb(){}


  VCon*     GetCont(char * name);
  int      getnelem();
  void      eraseC();
  int      getindex(AMSlink* aa);
  AMSlink* getelem(int ii);
  void     addnext(AMSlink* aa);


};


#endif
