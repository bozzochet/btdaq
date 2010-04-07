//  $Id: bcorr.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "event.h"
#include "bcorr.h"
#include "dcards.h"
extern "C" void bzcorr_(float& factor){
 factor=MISCFFKEY.BZCorr;
}
extern "C" void btempcor_(float& factor) {

// Default in absence of corrections
  factor = 1.;
  if(!MISCFFKEY.BTempCorrection)return;

// evaluate temperature
  float temp=MagnetVarp::getmeanmagnetmtemp();
  if (temp<=0. || temp>=50.) return;

// apply the correction
  const float dBdT=0.13e-3;
  const float TETH=185;
  factor = 1.-dBdT*(temp-TETH);

//  static int oldevent = -1;
//  if (AMSEvent::gethead()->getEvent() != oldevent) {
//    oldevent = AMSEvent::gethead()->getEvent();
//    time_t utime = AMSEvent::gethead()->gettime();
//    cout << "Event= " << oldevent << " Time= " << utime;
//    cout << " MagTemp= " << temp << " BtempCor= " << factor << endl;
//  }

}

MagnetVarp::MagnetTemperature MagnetVarp::mgtt;
