//  $Id: ctc.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// V. Choutko 26-sep-96
//
// Sep  26, 1996.  ak.  V1.27a, CTC extract from the file particle.h
//
#ifndef __AMSCTC__
#define __AMSCTC__
#include "link.h"
#include "trrec.h"
#include <tofrec.h>
#include "beta.h"
#include "charge.h"

class CTC {

private:
 number _Signal;
 number _Beta;
 number _ErrorBeta;
 AMSPoint _Coo; 

public:

CTC()  {_Coo[0]=0;_Coo[1]=0;_Coo[2]=0;}
CTC(number signal, number beta, number error, AMSPoint coo):
  _Signal(signal), _Beta(beta), _ErrorBeta(error), _Coo(coo){}

number getsignal()const {return _Signal;}
number getbeta()const {return _Beta;}
number geterbeta()const {return _ErrorBeta;}
AMSPoint getcoo()const {return _Coo;}

};


#endif
