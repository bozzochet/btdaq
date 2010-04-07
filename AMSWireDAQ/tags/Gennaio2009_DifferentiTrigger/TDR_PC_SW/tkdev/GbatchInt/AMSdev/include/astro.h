//  $Id: astro.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

#ifndef __ASTRO__
#define __ASTRO__

#include "cern.h"
#include <time.h>
#include "io.h"
#include <math.h>
#include "event.h"
#include "mceventg.h" 
#include "amsdbc.h"


class skyposition{
 public:
void RotatePos(integer axis,geant w,geant vec_in[], geant vec_out[]); // rotation around axis, w degrees
void CalcDir(skyposition source,geant & theta,geant & phi); //obtain theta,phi of source in local reference frame
void GetRa(number &Ra) {Ra = RA;};
void GetDec(number &Dec) {Dec = DEC;};
void GetLat(number &GLat) {GLat = GalLat;};
void GetLong(number &GLon) {GLon = GalLong;};
skyposition(geant ThetaS,number PhiS, number alt, time_t time); //for ISS...
skyposition(number RaSource,number DecSource); // constructor for source in table
 private: 
number RA; // right ascension
number DEC; // declination
number GalLat; // galactic latitude
number GalLong; // galactic longitude
number Sidtime(time_t sec);
};

#endif









