//  $Id: astro.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $


#include "typedefs.h" 
#include "astro.h"
#include "io.h"
#include "cern.h"
#include <time.h>
#include <math.h>
#include "event.h"
#include "mceventg.h" 
#include "amsdbc.h"
#include "ntuple.h"

number skyposition::Sidtime(time_t sec){  
  number tu = (floor(float(sec)/86400)+25567.5)/36525; // julian centuries since 1900 Epoch until beginning of julian day. sec is seconds since UNIX Epoch 1970
  //number l = 365.24219879-6.14E-6*tu; // tropical year length (days)
  number l = 365.2422; // tropical year length (days)
  number dt = sec-86400*(floor(float(sec)/86400)); //seconds since beginning of julian day
  number dthedt = (1+1/l)/240; // sidereal time variation (degrees per second). 240 = (360 degrees)/(86400 solar seconds) 
  number tg0 = 99.69098329 + 36000.76893*tu + 3.8708E-4*pow(tu,2); // sidereal time at beginning of julian day in degrees. 2nd term maybe not needed.
  number tg = tg0+(dt*dthedt); // sidereal time in degrees  
  return tg;
}

skyposition::skyposition(geant thetast, number phist, number alt, time_t sec){
  /// the commented parts are for the case of geodetic inputs thetast,phist
  //number f =  1/298.3 ; // oblateness 
  //geant ae = 6378.15E5; // eq radius (cm) 
  //number geolat = atan(pow(1-f,2)*tan(thetast)); // geodetic latitude (radians)
  //number radc = ae*sqrt((1-f*(2-f))/(1-f*(2-f)*pow(cos(geolat),2)));
  //alt=alt-ae;
  //number radius = sqrt(pow(radc,2)+pow(alt,2)+2*radc*alt*cos(thetast-geolat));

  RA = fmod((Sidtime(sec)*AMSDBc::twopi/360)-(AMSDBc::twopi-phist),AMSDBc::twopi); // right ascension (radians)
  //DEC = geolat + asin((alt/radius)*sin(thetast-geolat)); // declination (radians) 
  DEC = thetast;
  geant equ[3],gal[3],tmp1[3],tmp2[3];
  equ[0]=cos(RA)*cos(DEC);
  equ[1]=sin(RA)*cos(DEC);
  equ[2]=sin(DEC);
  RotatePos(3,282.85,equ,tmp1);
  RotatePos(1,62.9,tmp1,tmp2);
  RotatePos(3,-33,tmp2,gal);
  GalLat = asin(gal[2]); 
  GalLong = atan2(gal[1],gal[0]);
  if(GalLong<0) GalLong=AMSDBc::twopi+GalLong;
}
 
skyposition::skyposition(number RaSource, number DecSource):
  RA(RaSource),DEC(DecSource){  // to fill variables from a table              
  geant equ[3],gal[3],tmp1[3],tmp2[3];
  equ[0]=cos(RaSource)*cos(DecSource);
  equ[1]=sin(RaSource)*cos(DecSource);
  equ[2]=sin(DecSource);
  RotatePos(3,282.85,equ,tmp1);
  RotatePos(1,62.9,tmp1,tmp2);
  RotatePos(3,-33,tmp2,gal);
  GalLat = asin(gal[2]); 
  GalLong = atan2(gal[1],gal[0]);
  if(GalLong<0) GalLong=AMSDBc::twopi+GalLong;
}    

void skyposition::RotatePos(integer axis,geant w,geant vec_in[3], geant vec_out[3]){ // rotate [vec_in] around [axis;x=1,y=2,z=3] by [w] degrees anticlockwise
  switch(axis)
    {
    case 1:
      vec_out[0]=vec_in[0];
      vec_out[1]=vec_in[1]*cos(w*AMSDBc::twopi/360)+vec_in[2]*sin(w*AMSDBc::twopi/360);
      vec_out[2]=-vec_in[1]*sin(w*AMSDBc::twopi/360)+vec_in[2]*cos(w*AMSDBc::twopi/360);
      break;
    case 2:
      vec_out[1]=vec_in[1];
      vec_out[0]=vec_in[0]*cos(w*AMSDBc::twopi/360)+vec_in[2]*sin(w*AMSDBc::twopi/360);
      vec_out[2]=-vec_in[0]*sin(w*AMSDBc::twopi/360)+vec_in[2]*cos(w*AMSDBc::twopi/360);
      break;
    case 3:
      vec_out[2]=vec_in[2];
      vec_out[0]=vec_in[0]*cos(w*AMSDBc::twopi/360)+vec_in[1]*sin(w*AMSDBc::twopi/360);
      vec_out[1]=-vec_in[0]*sin(w*AMSDBc::twopi/360)+vec_in[1]*cos(w*AMSDBc::twopi/360);
      break;
    }
  return;
}

void skyposition::CalcDir(skyposition source,geant & theta,geant & phi){
  number decs,ras,raf,decf,pole,geoth,geoph;
  source.GetRa(ras); // get source coordinates
  source.GetDec(decs); 
  
  geant vtheta=AMSEvent::gethead()->getveltheta(); // get veltheta,velphi
  geant vphi=AMSEvent::gethead()->getvelphi();
  AMSEvent::gethead()->GetGeographicCoo(pole,geoth,geoph);
  geant alt=AMSEvent::gethead()->GetStationRad();// get StationRad(altitude)
  vphi=fmod(vphi-(pole-AMSmceventg::Orbit.PolePhiStatic)+AMSDBc::twopi,AMSDBc::twopi); //correct velphi for Earth rotation
  time_t abstime=AMSEvent::gethead()->gettime(); // get absolute time since UNIX Epoch

  skyposition forwardpos(vtheta,vphi,alt,abstime); // calculate celestial position towards which ISS advances (X_lvlh)
  forwardpos.GetRa(raf);  
  forwardpos.GetDec(decf);  

  geant zams[3],xams[3],yams[3],sourcecoo[3],sourceproj[3];
  
  sourcecoo[0] = cos(ras)*cos(decs);
  sourcecoo[1] = sin(ras)*cos(decs);
  sourcecoo[2] = sin(decs);
  zams[0] = cos(RA)*cos(DEC); // Z_ams=-Z_lvlh ; AMS pointing direction
  zams[1] = sin(RA)*cos(DEC);
  zams[2] = sin(DEC);  
  yams[0] = -cos(raf)*cos(decf); // Y_ams=-X_lvlh
  yams[1] = -sin(raf)*cos(decf);
  yams[2] = -sin(decf);  
  xams[0] = yams[1]*zams[2]-zams[1]*yams[2]; // X_ams=Y_ams x Z_ams
  xams[1] = -(yams[0]*zams[2]-zams[0]*yams[2]);
  xams[2] = yams[0]*zams[1]-zams[0]*yams[1];
  sourceproj[0] = xams[0]*sourcecoo[0]+xams[1]*sourcecoo[1]+xams[2]*sourcecoo[2]; // projection of sourcecoo on X_ams-Y_ams plane to get phi
  sourceproj[1] = yams[0]*sourcecoo[0]+yams[1]*sourcecoo[1]+yams[2]*sourcecoo[2];
  sourceproj[2] = 0;

  theta = acos(zams[0]*sourcecoo[0]+zams[1]*sourcecoo[1]+zams[2]*sourcecoo[2]);
  phi = fmod(atan2(sourceproj[1],sourceproj[0])+AMSDBc::twopi,AMSDBc::twopi);
}













