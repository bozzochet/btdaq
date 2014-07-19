
#ifndef _MYHOLDER_
#define _MYHOLDER_
#include "node.h"
#include "cont.h"


//! Temporary class for compatibility
class myholder :public AMSNode{
public:
  static myholder*  Head;
  myholder():AMSNode(){
    Head=this;}
  
  void _init(){};
  static AMSNodeMap EventMap;
  //! sets the inital values for the datacard
  void data();

private:
  void siamsdata();
  void reamsdata();
  void _sitkdata();
  void _retkdata();


};

#endif
