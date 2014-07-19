#ifndef  __VCon__
#define  __VCon__
#include "link.h"

class VCon{

public:


  VCon();
  virtual ~VCon(){}

  virtual  VCon*  GetCont(char* name){ return 0; }

  virtual int      getnelem(){ return 0; }
  virtual void     eraseC(){}
  virtual AMSlink* getelem(int ii){ return 0; }
  virtual int      getindex(AMSlink*){ return 0; }
  virtual void     addnext(AMSlink* aa){}
  // remove next element !!!
  virtual void     removeEl(AMSlink* prev, integer restore=1) {}
};

#endif


