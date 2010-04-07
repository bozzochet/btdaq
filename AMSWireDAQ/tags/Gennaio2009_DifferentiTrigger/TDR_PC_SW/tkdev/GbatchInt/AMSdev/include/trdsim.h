//  $Id: trdsim.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef __AMSTRDSIM__
#define __AMSTRDSIM__

#include "trdid.h"
#include "mccluster.h"
#include "commons.h"
#include "link.h"
#include "cont.h"

namespace trdsim{
const uinteger numvawf=32;
}
using trdsim::numvawf;

class AMSTRDRawHit: public AMSlink {
AMSTRDIdSoft _id;  
uinteger _Amp;
protected:
void _init(){};
void _copyEl();
void _printEl(ostream & stream);
void _writeEl();
static bool _HardWareCompatibilityMode;

public:

 static number delay_sf_table[numvawf];
 static number va_waveform[numvawf];
static number delay_sf(number usec);
 static void init_delay_table();
static void lvl3CompatibilityAddress(int16u address,uinteger &udr, uinteger & ufe, uinteger & ute,uinteger & chan);
static bool & HardWareCompatibilityMode(){return _HardWareCompatibilityMode;}
AMSTRDRawHit( const AMSTRDIdSoft & id,uinteger amp):AMSlink(),_id(id),_Amp(amp){};
uinteger getid() const {return _id.cmpt();}
static integer Out(integer status);
AMSTRDIdSoft & getidsoft()  {return _id;}
number  Amp(){return number(_Amp)/TRDMCFFKEY.f2i;} 
integer operator < (AMSlink & o) const {
  AMSTRDRawHit * p= dynamic_cast<AMSTRDRawHit*>(&o);
 if(_HardWareCompatibilityMode){
  return !p || _id.gethaddr()<p->_id.gethaddr();
 }
 else{
  if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
  else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0
;
 
  else return !p || _id.cmpta() < p->_id.cmpta();
 }
}

 AMSTRDRawHit *  next(){return (AMSTRDRawHit*)_next;}   
//interfce with lvl3
integer lvl3format(int16 * ptr, integer nmax);
//interface with mc
static void sitrddigi();
static void sitrdnoise();



//interface withdaq

static void builddaq(int n, int16u* p);
static void buildraw(int i, int n, int16u*p);
#ifdef __WRITEROOT__
 friend class TrdRawHitR;
#endif
};


#endif
