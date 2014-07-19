//  $Id: gsen.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#ifndef __AMSGSEN__
#define __AMSGSEN__
#include "gvolume.h"
class AMSgSen : public AMSgvolume{
public:
integer getsenstrip(AMSPoint&);  
AMSPoint str2pnt(number cofgx,number cofgy);
//void tkidco(char, number,AMSPoint&, AMSDir&); //char='x' or 'y' 
static AMSPoint tkidfn(const AMSPoint&,const AMSPoint&,const AMSDir&,const AMSDir&);
  AMSgSen * next(){return (AMSgSen *)AMSNode::next();}
  AMSgSen * prev(){return (AMSgSen *)AMSNode::prev();}
  AMSgSen * up(){return   (AMSgSen *)AMSNode::up();}
  AMSgSen * down(){return (AMSgSen *)AMSNode::down();}
// static integer getlayer(integer idsoft){return idsoft%10;}
// static integer getladder(integer idsoft){return (idsoft/10)%100;}
// static integer getsensor(integer idsoft){return (idsoft/1000)%100;}
// static integer getside(integer idsoft){return (idsoft/1000)%10;}
};
#endif
