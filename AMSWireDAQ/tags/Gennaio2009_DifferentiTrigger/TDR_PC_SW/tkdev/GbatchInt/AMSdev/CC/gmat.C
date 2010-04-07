//  $Id: gmat.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V.Choutko.
// modified by E.Choumilov 20.06.96. - add some TOF materials.
// modified by E.Choumilov 1.10.99. - add some ECAL materials.
#include "gmat.h"
#include "snode.h"
#include "amsgobj.h"
#include "job.h"
#include "richdbc.h"
#ifdef __G4AMS__
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UnitsTable.hh"
#include <strstream>
#endif
integer AMSgmat::debug=0;
void AMSgmat::_init(){
#ifdef __AMSDEBUG__
  for( AMSNode *p=this->prev();p;p=p->prev()){
    if(!strcmp(getname(),p->getname())){
      cerr<<"AMSgmat-F-MaterialalrdyExists "<<getname()<<endl;
      exit(1);
    }
  }
#endif
#ifdef __G4AMS__
if(MISCFFKEY.G3On){
#endif
   geant *a=new geant[_npar];
   geant *z=new geant[_npar];
   geant *w=new geant[_npar];
   for(int i=0;i<_npar;i++){
     a[i]=_a[i];
     w[i]=_w[i];
     z[i]=_z[i];
   }
   
  if(_npar == 1)   GSMATE(_imate,_name,a[0],z[0],_rho,_radl,_absl,_ubuf,1);

  else    GSMIXT(_imate,_name,a,z,_rho,-_npar,w);
   delete []a;
   delete []z;
   delete []w;
#ifdef __G4AMS__
}
if(MISCFFKEY.G4On){
 if(_npar==1 ){
  if(_temp==0)_pamsg4m= new G4Material(G4String(_name),_z[0],_a[0]*g/mole,_rho*g/cm3);
  else _pamsg4m= new G4Material(G4String(_name),_z[0],_a[0]*g/mole,_rho*g/cm3,kStateGas,_temp*kelvin,1.e-18*_rho*g/cm3/universe_mean_density*_temp*kelvin*pascal); 
 }
 else{
  char namz[255];
  ostrstream ost(namz,sizeof(namz));
  _pamsg4m= new G4Material(G4String(_name),_rho*g/cm3,_npar);
  for(int i=0;i<_npar;i++){
   ost.clear();
   ost.seekp(0);
   ost<<_name<<int(_z[i])<<ends;
   G4String name(namz);
   int natoms=int(_w[i]);
   _pamsg4m->AddElement(new G4Element(name," ",_z[i],_a[i]*g/mole),natoms);
  }
 }
}
#endif
}

      
ostream & AMSgmat::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSMATE" << endl);
}
integer AMSgtmed::debug=1;
void AMSgtmed::_init(){
#ifdef __AMSDEBUG__
  for( AMSNode *p=this->prev();p;p=p->prev()){
//    cout <<getname() <<" "<<p->getname()<<endl;
    if(!strcmp(getname(),p->getname())){
      cerr<<"AMSgtmed-F-MediaalrdyExists "<<getname()<<endl;
      exit(1);
    }
  }
#endif
  GSTMED(_itmed,_name,_pgmat->getmati(),_isvol,_ifield,_fieldm,_tmaxfd,
  _stemax,_deemax,_epsil,_stmin,_uwbuf,_nwbuf);
  if(_yb=='Y'){
   GSTPAR(_itmed,"BIRK1",_birks[0]);
   GSTPAR(_itmed,"BIRK2",_birks[1]);
   GSTPAR(_itmed,"BIRK3",_birks[2]);
  }
  if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd && _uwbuf[1]>1){
    CUTGAM(1.01e-6);
//   GSTPAR(_itmed,"CUTGAM",1.01e-6);
  }
  if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd && _uwbuf[1]>0 && _uwbuf[2]==3){
   if(IsSensitive() ){
     CUTELE(20.01e-6);
//    GSTPAR(_itmed,"CUTELE",20.01e-6);
    GSTPAR(_itmed,"DCUTE",1.8e-6);
    GSTPAR(_itmed,"STRA",1);
   }
  }
  else if(_nwbuf && _uwbuf[0]==TRDMCFFKEY.g3trd  && _uwbuf[2]==3){
   if(IsSensitive()){
    CUTELE(35.01e-6);
//    GSTPAR(_itmed,"CUTELE",35.01e-6);
   }
  }
}      
void AMSgtmed::setubuf(int nwbuf,geant ubuf[]){
 _nwbuf=nwbuf;
 if(_nwbuf){
   _uwbuf=new geant[_nwbuf];
   if(_uwbuf){
    for(int i=0;i<_nwbuf;i++)_uwbuf[i]=ubuf[i];
   }
 }
}
ostream & AMSgtmed::print(ostream & stream)const{
return(AMSID::print(stream)  <<  " GSTMED" << endl);
}

uinteger AMSgmat::_GlobalMatI=0;
uinteger AMSgtmed::_GlobalMedI=0;
//-------------------------------------------------------------------------
void AMSgmat::amsmat(){
static AMSgmat mat;
mat.setname("Materials:");
AMSJob::gethead()->addup(&mat);
mat.add (new AMSgmat("HYDROGEN",1.01, 1.,0.0708,865.,790.));
//mat.add (new AMSgmat("DEUTERIUM",  2.01,1.,0.162 ,757.,342.));
mat.add (new AMSgmat("HELIUM",  4.  , 2.,0.125 ,755.,478.));
//mat.add (new AMSgmat("LITHIUM",  6.94, 3.,0.534 ,155.,120.6));
//mat.add (new AMSgmat("BERILLIUM",  9.01, 4., 1.848 ,35.3,36.7));
mat.add (new AMSgmat("CARBON", 12.01, 6., 2.265 ,18.8,49.9));
//mat.add (new AMSgmat("NITROGEN", 14.01, 7.,0.808 ,44.5,99.4));
//mat.add (new AMSgmat("NEON", 20.18,10., 1.207 , 24.,74.9));
         // Half density alum
mat.add (new AMSgmat("ALUMINIUM",26.98, 13., 1.35, 17.8, 74.4));
mat.add (new AMSgmat("IRON", 55.85,26., 7.87  ,1.76,17.1));
mat.add (new AMSgmat("COPPER", 63.54,29.,8.96  ,1.43,14.8));
mat.add (new AMSgmat("TUNGSTEN",183.85,74., 19.3  ,0.35,10.3));
mat.add (new AMSgmat("TUNGSTENTRD",183.85,74., 19.3  ,0.35,10.3));
mat.add (new AMSgmat("LEAD",207.19,82., 11.35 ,0.56,18.5));
mat.add (new AMSgmat("URANIUM",238.03,92., 18.95 ,0.32,12. ));
mat.add (new AMSgmat("AIR",14.61,7.3, 0.001205,30423.24,67500.));
mat.add (new AMSgmat("MYLAR",  8.7   ,4.5,  1.39  ,28.7, 43.));
mat.add (new AMSgmat("MYLARTRD",  8.7   ,4.5,  1.39  ,28.7, 43.));
mat.add (new AMSgmat("VACUUM",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
mat.add (new AMSgmat("VACUUMTRD",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
geant a[]={20.18,12.01,1.01,0};
geant z[]={10.,6.,1.,0};
geant w[]={4.,1.,4.,0};
//mat.add (new AMSgmat("TPC GAS",a,z,w,3,0.8634e-3));
a[0]=39.95;a[1]=12.01;a[2]=1.01;
z[0]=18.;  z[1]=6.;   z[2]=1.;
w[0]=1.;   w[1]=2.;   w[2]=6.;
//mat.add (new AMSgmat("DRIFT GAS ",a,z,w,3,1.568e-3));
a[0]=12.01;a[1]=28.1;
z[0]=6.;  z[1]=14.;
w[0]=2.66;w[1]=1.;
mat.add (new AMSgmat("SICARBON",a,z,w,2,2.286));
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("SCINT",a,z,w,2,1.032));
mat.add (new AMSgmat("SILICON",28.09,14.,2.32,9.36,45.));
a[0]=144.24;a[1]=55.85;a[2]=10.8;
z[0]=60.;   z[1]=26.;  z[2]=5.;
w[0]=30.;   w[1]=69.;  w[2]=1.;
mat.add (new AMSgmat("MAGNET",a,z,w,3,7.45));

// WLS for CTC (same polystiren as for TOF scint)
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("WLS",a,z,w,2,1.03));
//
// AL honeycomb structure for CTC (as for TOF now !) :
mat.add (new AMSgmat( "AL-HONEYC2",26.98, 13., 0.04, 600., 2660.));
//
// Teflon(C2F4) cover for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON1",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for CTC :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON2",a,z,w,2, 2.2));
//
// Teflon(C2F4) separators for ATC rho=0.3 gr/cm^3 :
a[0]=12.;a[1]=19.;
z[0]=6.; z[1]=9.;
w[0]=2.; w[1]=4.;
mat.add (new AMSgmat( "TEFLON3",a,z,w,2, 0.3));
//
// Si-aerogel(SiO4H4) for CTC :
a[0]=28.; a[1]=16.; a[2]=1.;
z[0]=14.; z[1]=8.;  z[2]=1.;
w[0]=1.;  w[1]=4.;  w[2]=4.;
mat.add (new AMSgmat( "SIAEROGEL",a,z,w,3, 0.24));
//
// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
a[0]=28.; a[1]=16.; a[2]=12.; a[3]=1.;
z[0]=14.; z[1]=8.;  z[2]=6.; z[3]=1.;
w[0]=1.;  w[1]=2.;  w[2]=3.; w[3]=9.;
mat.add (new AMSgmat( "MEWAEROGEL",a,z,w,4, 0.125));

// Si-aerogel(SiO2(CH3)3) for CTC-MEW :
{
  geant a[]={209.,72.61,16.};
  geant z[]={83.,32.,8.};
  geant w[]={4.,3.,12.};
   mat.add (new AMSgmat( "BGO",a,z,w,3, 7.1));
}


// AL honeycomb structure for Tracker (as for TOF now !) :
mat.add (new AMSgmat( "AL-HONEYC-Tr",26.98, 13., 0.04, 600., 2660.));

// Foam structure for Tracker  :
mat.add (new AMSgmat( "FOAM",12.01, 6., 0.1 , 425.82, 900.));
//




{

  // RICH materials by Carlos Delgado (CIEMAT) based on the official
  // rich simulation by Fernando Barao at al.


{ // Aerogel  
  geant a[]={28.09,16.0};
  geant z[]={14.,8.};
  geant w[]={1.,2.};

#ifdef __AMSDEBUG__
  cout << "RICH aerogel density "<<RICHDB::aerogel_density() <<endl;
#endif

  mat.add(new AMSgmat("RICH_AEROGEL",a,z,w,2,RICHDB::aerogel_density()));
  //  mat.add(new AMSgmat("RICH_AEROGEL",a,z,w,2,.7368));  
} 
// Aerogel density extracted from J Phys D 27(1994)414
// Case: Pure aerogel

{
  geant a[]={22.98977,18.99984};
  geant z[]={11.,9.};
  geant w[]={1.,1.};

  mat.add(new AMSgmat("RICH_NaF",a,z,w,2,2.558));
}



{ // Mirrors: plexiglass: Used for the light guides
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("RICH_MIRRORS",a,z,w,3,1.16));
}

{ // PMTs
  geant a[]={28.,16.};
  geant z[]={14.,8.};
  geant w[]={1.,2.};
  mat.add (new AMSgmat("PMT_WINDOW",a,z,w,2,2.64));
}


{ // Absorber walls: plexiglass too... C5H8O2
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("RICH_WALLS",a,z,w,3,1.16));

  mat.add(new AMSgmat("RICH_SOLG",a,z,w,3,1.16));

  // Carbon fiber for the aerogel support structure

  mat.add (new AMSgmat("RICH_CARBONF", 12.01, 6., 1.3 , 33., 66.));

  // Magnetic shielding
  mat.add (new AMSgmat("RICH_BSHIELD",55.85,26.,0.787,17.6,168.));

  // NEW! Outer mirror

  mat.add (new AMSgmat("RICH_MIRROR",29.98,13.,1.35,17.8,74.4));
}
}
//-------------------------------------------
// ---> materials for TOF/ANTI:
//

mat.add (new AMSgmat("VACUUMTOFA",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
{
// ---> TOF-scintillator(C8H8):
  geant a[]={12.01,1.01};
  geant z[]={6.,1.};
  geant w[]={8.,8.};
  mat.add (new AMSgmat("TOFSCINT",a,z,w,2,1.032));
}
{
// ---> AL honeycomb structure for TOF :
geant relden=0.085/2.7;
mat.add (new AMSgmat( "TOF_AL_HONEYC",26.98,13.,2.7*relden, 8.9/relden,39.4/relden));
// ---> effective material for PMT-boxes (low dens.(1:10) iron):
      relden=0.787/7.87;//(1:10)
mat.add (new AMSgmat("TOF_LOW_DENS_Fe",55.85,26.,7.87*relden,1.76/relden,16.8/relden));
// ---> low density(1.8 eff) carbon (carb.fiber) for TOF covers :
      relden=1.8/2.265;
mat.add (new AMSgmat("TOFCARBONF",12.01,6.,2.265*relden,18.8/relden,38.1/relden));
// ---> low density(1.8 eff) carbon (carb.fiber) for ANTI supp.tube :
mat.add (new AMSgmat("ANTICARBONF",12.01,6.,2.265*relden,18.8/relden,38.1/relden));
}
{  
// ---> LG-plexiglass(C5H8O2):
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("TOFLGPLEX",a,z,w,3,1.16));
}

//-------------------------------------------
// ---> materials for ECAL:
//
// Light lead for ECAL test :
{
mat.add (new AMSgmat("LIGHTLEAD",207.19,82., 6.36 ,1.,32.5));
//
a[0]=12.;a[1]=1.;
z[0]=6.; z[1]=1.;
w[0]=8.; w[1]=8.;
mat.add (new AMSgmat("ECSCINT",a,z,w,2,1.032));
}
//--------
{
//---> Effective material for ECAL PMT-boxes-frame (low dens.AL):
  geant relden=0.91/2.7;//relat(to true AL) density 
  mat.add (new AMSgmat("LOW_DENS_AL_2",26.98,13.,2.7*relden,8.9/relden,39.4/relden));
//
//---> AL honeycomb structure for ECAL:
  relden=0.266/2.7;//relat(to true AL) density 
  mat.add (new AMSgmat("EC-AL-HONEYC",26.98,13.,2.7*relden,8.9/relden,39.4/relden));
//
//---> Al-plates(NOW glue !) for ECAL superlayer structure:
//  relden=0.5;//relat(to true AL) density 
//  mat.add (new AMSgmat("ECALPALUM",26.98,13.,2.7*relden,8.9/relden,39.4/relden));
  mat.add (new AMSgmat("ECALPALUM",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));// this
//should be glue, but for simplicity/speed reasons i use vacuum, because NOW thickness is small
//
//---> Eff.lead for ECAL(to compensate extra weight puzzle):
  relden=0.938;//relat(to true Pb) density 
  mat.add (new AMSgmat("ECLEAD",207.19,82., 11.35*relden ,0.56/relden,18.5/relden));
}
//--------
{ // Fiber wall(cladding+glue, ~ plexiglass):  
  geant a[]={12.01,1.01,16.0};
  geant z[]={6.,1.,8.};
  geant w[]={5.,8.,2.};

  mat.add(new AMSgmat("ECFPLEX",a,z,w,3,1.16));
}
//------------------------------------------


{
 // TRD Materials by V. Choutko - probably wrong ones

  // Honeycomb for top and bottom plates

geant rho=0.032;
mat.add (new AMSgmat( "TRDHC",26.98, 13., rho, 24./rho, 106/rho));

  // Honeycomb for side walls
rho=0.050;
mat.add (new AMSgmat( "TRDHCWall",26.98, 13., rho, 24./rho, 106/rho));


 // CF  (2.25 % X0)
rho=1.65;
mat.add (new AMSgmat("TRDCarbonFiber", 12.01, 6., rho , 42.7/rho, 86.3/rho));

// FiberGlass  
{
geant a[]={28.08,16.};
geant z[]={14.,8.};
geant w[]={1,2};
rho =1.3 ;  //probably wrong one, needs check with Joe
mat.add (new AMSgmat("TRDFiberGlass",a,z,w,2,rho));
}
 // RohaCell
rho=0.04;
mat.add (new AMSgmat("TRDRohaCell", 12.01, 6., rho , 42.7/rho, 86.3/rho));

  mat.add (new AMSgmat("TRDALUMINIUM",26.98, 13., 2.7, 8.9, 39.4));

// Gas (Xe/CO2) (80/20) 1.1% X0

{
   geant z[]={54.,6.,8.};
   geant a[]={131.3,12.,16.};
   geant w[]={8,2,4};
   mat.add (new AMSgmat("XECO2_80/20",a,z,w,3,5.1e-3));
}
// TRDFoam
//   just plain carbon with 0.08 0.4% X0

rho=0.08;
mat.add (new AMSgmat("TRDFoam", 12.01, 6., rho , 42.7/rho, 86.3/rho));

// TRD tube walls (kapton)  --> using mylar for the moment (1% X0)

//TRD Radiator (polypropylene )  6% X0

{
   geant z[]={6.,1.};
   geant a[]={12.,1.};
   geant w[]={1,2};
   mat.add (new AMSgmat("TRDRadiator",a,z,w,2,0.06));
}

}

// SRD Materials - almost certainly wrong
{
  // Wave length shifter -pure scintillator ?

   // Xtall (YAlO3)
  geant z[]={39,13,8};
  geant a[]={88.9,27.,16.};
  geant w[]={1,1,3};
  mat.add (new AMSgmat("YAlO3",a,z,w,3,5.55));

}
//--------------------------------
//  Magnet materials:
{
  mat.add (new AMSgmat("MVACUUM",1.01,1., 1.e-21,1.E+22,1.E+22,0.1));
  mat.add (new AMSgmat("MALUMINIUM",26.98, 13., 2.7, 8.9, 39.4));// norm.dens.AL
  mat.add (new AMSgmat("MCOILMAT",26.98, 13., 2.7, 8.9, 39.4));// tempor as above
}
//--------------------------------
//  Radiator+shield/crates/TOF-TRD_interf/USS materials:
{
  geant rraden=0.33/2.7;//relat(to norm.AL) density of radiator(with embedded tubes)
  mat.add (new AMSgmat("RADALUMIN1",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL for crate-rad.
}
{
  geant rraden=0.51/2.7;//relat(to norm.AL) density of radiator(with embedded tubes)
  mat.add (new AMSgmat("RADALUMIN2",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL for tracker-rad.
}
{
  geant rraden=0.34;//relat(to norm.AL) density of TOF honeycomb support legs
  mat.add (new AMSgmat("TFSUPALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL for supports
}
{
  geant rraden=0.68/2.7;//(final) Relat(to norm.AL) density of M-structure frame and M-legs
  mat.add (new AMSgmat("MSFALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=1.27/2.7;//(final) Relat(to norm.AL) density of M-structure top-brackets
  mat.add (new AMSgmat("MSBRALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant a[]={28.08,26.98,63.55};//Si+Al+Cu
  geant z[]={14.,13.,29.};
  geant w[]={10.,50.,40.};//prop. by weight
  geant craden=0.49;// abs. density of crate(g/cm3)
  mat.add (new AMSgmat("CRATEMAT1",a,z,w,3,craden));
}
{
  geant rraden=0.49;//(final)relat(to norm.AL) density of bar-1 type of USS
  mat.add (new AMSgmat("US1BALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=0.38;//(final)relat(to norm.AL) density of bar-2 type of USS
  mat.add (new AMSgmat("US2BALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=0.64;//(final)relat(to norm.AL) density of bar-3 type of USS
  mat.add (new AMSgmat("US3BALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=0.454;//(final)Relat(to norm.AL) density of bar-4 type of USS
  mat.add (new AMSgmat("US4BALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=0.65;//(final)Relat(to norm.AL) density of frame of USS lower part
  mat.add (new AMSgmat("US5BALUMIN",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
{
  geant rraden=0.005;//Relat(to norm.AL) density of ISS "tube" near AMS attachment
  mat.add (new AMSgmat("ISSTUBEALU",26.98, 13., 2.7*rraden,
                                   8.9/rraden, 39.4/rraden));// low dens.AL 
}
//-------------------------------
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPMATE(0);
#endif

AMSgObj::GTrMatMap.map(mat);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
cout <<"AMSgmat::amsmat-I-TotalOf "<<GetLastMatNo()<<" materials defined"<<endl;
}



//------------------------------------------------------------------------------



void AMSgtmed::amstmed(){


// NowMedia

static AMSgtmed tmed;
tmed.setname("TrackingMedia:");
AMSJob::gethead()->addup(&tmed);


tmed.add (new AMSgtmed("AIR","AIR",0));
tmed.add (new AMSgtmed("MAGNET","MAGNET",0));
AMSgtmed * rich_pvac;
{
// vacuum has to be trd aware
 geant uwbuf[5];
 integer nwbuf=5;
#ifdef __G4AMS__
if (MISCFFKEY.G4On){
 uwbuf[0]=0;
}
else{
#endif
 uwbuf[0]=TRDMCFFKEY.g3trd;
#ifdef __G4AMS__
}
#endif
  uwbuf[1]=TRDMCFFKEY.mode;
  uwbuf[3]=0;
  uwbuf[4]=0;
AMSgtmed * pvac=new AMSgtmed("VACUUM","VACUUMTRD",0);  
  rich_pvac=pvac;   // DO NOT MODIFY
 uwbuf[2]=6;
 pvac->setubuf(nwbuf,uwbuf);
 tmed.add (pvac );
 pvac=new AMSgtmed("COPPERTRD","COPPER",0);
 uwbuf[2]=10;
 pvac->setubuf(nwbuf,uwbuf);
 tmed.add (pvac );
 pvac=new AMSgtmed("TUNGSTENTRD","TUNGSTEN",0);
 uwbuf[2]=12;
 pvac->setubuf(nwbuf,uwbuf);
 tmed.add (pvac );
}


tmed.add (new AMSgtmed("1/2ALUM","ALUMINIUM",0));
tmed.add (new AMSgtmed("USSALLOY","ALUMINIUM",0));
tmed.add (new AMSgtmed("ACTIVE_SILICON","SILICON",1));
tmed.add (new AMSgtmed("NONACTIVE_SILICON","SILICON"));
tmed.add (new AMSgtmed("CARBON","CARBON",0));
tmed.add (new AMSgtmed("ELECTRONICS","CARBON",0));
//
//----------------------------------------------------
// TOF/ANTI-media:
//
tmed.add (new AMSgtmed("TOF_VAC","VACUUMTOFA",0));// fill all gaps
{
//geant birks[]={1.,0.013,9.6e-6};
//geant birks[]={2.,0.0012,-6.4e-7};
geant birks[]={0.,0.0011,0.52};//model "0" - pars from TOF test-beam; using atan
//tmed.add (new AMSgtmed("TOF_SCINT","TOFSCINT",1,'Y',birks));
tmed.add (new AMSgtmed("TOF_SCINT","TOFSCINT",1,'Y',birks,1,5,10,
                       -0.25, -1, 0.001, -0.05));
}
//(for tof_scint.: max_step=0.25cm/autom, min_step=0.05cm/autom )
//
{
//geant birks[]={1.,0.013,9.6e-6};
//geant birks[]={2.,0.0012,-6.4e-7};
geant birks[]={0.,0.0011,0.52};
tmed.add (new AMSgtmed("ANTI_SCINT","TOFSCINT",1,'Y',birks));
}
//
tmed.add (new AMSgtmed("TOF_HONEYCOMB","TOF_AL_HONEYC",0));
tmed.add (new AMSgtmed("TOF_PMT_BOX","TOF_LOW_DENS_Fe",0));
tmed.add (new AMSgtmed("TOF_SC_COVER","TOFCARBONF",0));
tmed.add (new AMSgtmed("TOF_LG","TOFLGPLEX",0));
//
tmed.add (new AMSgtmed("ANTI_WRAP","MYLAR",0));//  tempor. mylar
tmed.add (new AMSgtmed("ANTI_SUPTB","ANTICARBONF",0));//  tempor. carb.fiber
//----------------------------------------------------
tmed.add (new AMSgtmed("IRON","IRON",0));
//
tmed.add (new AMSgtmed("CTC_WLS","WLS",1));
//
tmed.add (new AMSgtmed("CTC_HONEYCOMB","AL-HONEYC2",0));
//
tmed.add (new AMSgtmed("CTC_WALL","TEFLON1",0));
//
tmed.add (new AMSgtmed("CTC_SEP","TEFLON2",0));
//
tmed.add (new AMSgtmed("ATC_PTFE","TEFLON3",1));
//
tmed.add (new AMSgtmed("CTC_AEROGEL","SIAEROGEL",1));
//
tmed.add (new AMSgtmed("CTC_DUMMYMED","VACUUM",0));// fill all gaps inside CTC
//
tmed.add (new AMSgtmed("ATC_AEROGEL","MEWAEROGEL",1));
//
//
tmed.add (new AMSgtmed("TUNGSTEN","TUNGSTEN",0));
//
{
geant birks[]={1.,0.013,9.6e-6};
tmed.add (new AMSgtmed("BGO","BGO",1,'Y',birks));
}
int ip=27;
tmed.add (new AMSgtmed("Tr_Honeycomb","AL-HONEYC-Tr",0));
tmed.add (new AMSgtmed("Tr_Foam","FOAM",0));

tmed.add (new AMSgtmed("ATC_PTAE","TEFLON3",0));//30
tmed.add (new AMSgtmed("TOF_PMT_WINDOW","PMT_WINDOW",1));
//---------------
// RICH media

{
  // Cerenkov photon properties for the radiator

  geant index[RICmaxentries];
  geant abs_l[RICmaxentries];
  geant p[RICmaxentries];
  geant dummy[RICmaxentries];
  integer iw;
  
  // Recompute some tables

  RICHDB::mat_init();

  for(iw=0;iw<RICHDB::entries;iw++)
    {
      p[iw]=2*3.1415926*197.327e-9/RICHDB::wave_length[iw]; // Photon momentum in GeV
      dummy[iw]=1.;
    }

// Radiator

  geant my_step=RICHDB::max_step();
  AMSgtmed * pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH RAD",
           "RICH_AEROGEL",0,'N',0,1,20,10,my_step));   //32
  pgtmed->AGSCKOV(RICHDB::entries,p,RICHDB::abs_length,dummy,RICHDB::index,RICHDB::rad_clarity);
  RICHDB::agl_media=pgtmed->getmedia();

  // NaF (Secondary radiator)
  pgtmed=(AMSgtmed*)  tmed.add (new AMSgtmed("RICH NAF",
					     "RICH_NaF",0));
  pgtmed->AGSCKOV(RICHDB::entries,p,RICHDB::naf_abs_length,dummy,
		  RICHDB::naf_index_table,0.);
  RICHDB::naf_media=pgtmed->getmedia();

// PMT window
  
//  my_step*=(1-1/1.458/1.458)/(1-1/RICHDB::rad_index/RICHDB::rad_index); // To be moved to RICH common
    pgtmed= (AMSgtmed*)  tmed.add (new AMSgtmed("RICH PMTS",
    "PMT_WINDOW",1)); //35 
  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=1.e-10;     //changed by VC 7-sep-2000
      index[iw]=RICHDB::pmtw_index;
    }
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);


// Solid light-guides. They only work if __HOLLOW__ is undefined in richdbc.h 
  pgtmed= (AMSgtmed*) tmed.add (new AMSgtmed("RICH SOLG",
					     "RICH_SOLG",0));
  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=RICHDB::lg_abs[iw];
      index[iw]=RICHDB::lg_index[iw];
    }
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);


//  geant xustep=200.;
  geant xustep=800.;
  for(iw=0;iw<RICHDB::entries;iw++){
//    xustep=xustep+(800.-200.)/geant(RICHDB::entries);
    xustep=xustep-(800.-200.)/geant(RICHDB::entries);    
    p[iw]=2*3.1415926*197.327e-9/xustep;
  }


// RICH MOTHER VOLUME

  pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH VACUUM","VACUUM",0));  // not absorber
  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=1e10;
      index[iw]=1;
    }
    pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);
    rich_pvac->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);


// LG mirrors

  pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH MIRRORS","RICH_MIRRORS",0));//33
  for(iw=0;iw<RICHDB::entries;iw++)
    abs_l[iw]=1.-0.92; // Reflectivity=92%
  index[0]=0;          // This behaves like metal
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);
  
// Outer mirror

pgtmed= (AMSgtmed*)  tmed.add (new AMSgtmed("RICH MIRROR","RICH_MIRROR",0));
  for(iw=0;iw<RICHDB::entries;iw++)
    abs_l[iw]=1.-RICmireff; // Reflectivity=85%   
  index[0]=0;          // The mirror is a metal
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);


// Aerogel support structure

  pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH CARBON","RICH_CARBONF",0));
  for(iw=0;iw<RICHDB::entries;iw++){
      abs_l[iw]=1.; 
      index[iw]=0.;}
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);

// Absorber

  pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH WALLS","RICH_WALLS",0));
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);

// B shields

   pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH SHIELD","RICH_BSHIELD",0));
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0); 

// Electronics

   pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH GLUE","RICH_WALLS",0));
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);

  // PORON
  pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("RICH PORON","VACUUM",0));
  pgtmed->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);

// transparent vaccum

  for(iw=0;iw<RICHDB::entries;iw++)
    {
      abs_l[iw]=1e10;
      index[iw]=1;
    }
  rich_pvac->AGSCKOV(RICHDB::entries,p,abs_l,dummy,index,0);  

}
//---------------
//  ECAL media
//
{
//geant birks[]={1.,0.013,9.6e-6};
//geant birks[]={2.,0.0012,-6.4e-7};
geant birks[]={0.,0.0011,0.52};
tmed.add (new AMSgtmed("EC_EFFRAD","LIGHTLEAD",0));// eff.radiator for fast sim(not sens !!!)
//
AMSgtmed * pgtmed= (AMSgtmed*)tmed.add (new AMSgtmed("EC_RADIATOR","ECLEAD",0,'N',birks,2,20.,10.,1000.,
                                    -1.,0.001,-1.)); // simplif.tracking in magn.f
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_RADIATOR
pgtmed->CUTELE(ECMCFFKEY.cutge);
//
pgtmed=(AMSgtmed*)tmed.add (new AMSgtmed("EC_FCORE","ECSCINT",1,'Y',birks));
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_FIBER-core
pgtmed->CUTELE(ECMCFFKEY.cutge);
//
pgtmed=(AMSgtmed*)tmed.add (new AMSgtmed("EC_FWALL","ECFPLEX",0)); 
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_FIBER-cladd+glue
pgtmed->CUTELE(ECMCFFKEY.cutge);
//
pgtmed=(AMSgtmed*)tmed.add (new AMSgtmed("EC_ELBOX","LOW_DENS_AL_2",0));
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_ELBOX
pgtmed->CUTELE(ECMCFFKEY.cutge);
//
pgtmed=(AMSgtmed*)tmed.add (new AMSgtmed("EC_HONEYC","EC-AL-HONEYC",0));
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_HONEYC
pgtmed->CUTELE(ECMCFFKEY.cutge);
//
pgtmed=(AMSgtmed*)tmed.add (new AMSgtmed("EC_AL_PLATE","ECALPALUM",0));
pgtmed->CUTGAM(ECMCFFKEY.cutge);// special cuts for EC_AL_PLATE(now glue !)
pgtmed->CUTELE(ECMCFFKEY.cutge);
} 
//----------------

{ // TRD Media by V. Choutko  seems to be also wrong
geant uwbuf[5];
integer nwbuf=5;
#ifdef __G4AMS__
if (MISCFFKEY.G4On){
 uwbuf[0]=TRDMCFFKEY.g3trd;
}
else{
#endif
uwbuf[0]=TRDMCFFKEY.g3trd;
#ifdef __G4AMS__
}
#endif
uwbuf[1]=TRDMCFFKEY.mode;
uwbuf[3]=0;
uwbuf[4]=0;
tmed.add (new AMSgtmed("TRDHC","TRDHC",0));
tmed.add (new AMSgtmed("TRDHCWall","TRDHCWall",0));
tmed.add (new AMSgtmed("TRDCarbonFiber","TRDCarbonFiber",0));
tmed.add (new AMSgtmed("TRDFiberGlass","TRDFiberGlass",0));
tmed.add (new AMSgtmed("TRDALUMINIUM","TRDALUMINIUM",0));
tmed.add (new AMSgtmed("TRDRohaCell","TRDRohaCell",0));


uwbuf[2]=3;
if(TRDMCFFKEY.mode<3){
 uwbuf[3]=1;   //dont really need it but...
}

AMSgtmed * pgas=new AMSgtmed("TRDGas","XECO2_80/20",1);
pgas->setubuf(nwbuf,uwbuf);
tmed.add (pgas);

AMSgtmed *pfoam = new AMSgtmed("TRDFoam","TRDFoam",0);
uwbuf[2]=2;
uwbuf[3]=0;
pfoam->setubuf(nwbuf,uwbuf);
tmed.add (pfoam);

AMSgtmed * pwall=new AMSgtmed("TRDCapton","MYLARTRD",0);
uwbuf[2]=2;
uwbuf[3]=0;
pwall->setubuf(nwbuf,uwbuf);
tmed.add (pwall);

uwbuf[2]=1;
if(TRDMCFFKEY.mode<3){
 uwbuf[3]=TRDMCFFKEY.cor;
 uwbuf[4]=0;
}
else{
 uwbuf[3]=TRDMCFFKEY.alpha;
 uwbuf[4]=TRDMCFFKEY.beta;
}
AMSgtmed * prad=new AMSgtmed("TRDRadiator","TRDRadiator",1);
prad->setubuf(nwbuf,uwbuf);
tmed.add (prad);



}

{
  //SRD Media
tmed.add (new AMSgtmed("SRDWLS","SCINT",1));
tmed.add (new AMSgtmed("SRDXtall","YAlO3",1));
tmed.add (new AMSgtmed("SRDPMT","PMT_WINDOW",1));

}
//--------------------
{
// ---> AMS02 Magnet Media:
//
geant cutge=MAGSFFKEY.ecutge;// increased EgammaEelectron cut
AMSgtmed * magmed=(AMSgtmed*)tmed.add (new AMSgtmed("MVACMED","MVACUUM",0));
  magmed->CUTGAM(cutge);
  magmed->CUTELE(cutge);
//
magmed=(AMSgtmed*)tmed.add (new AMSgtmed("MVCASEMED","MALUMINIUM",0));
  magmed->CUTGAM(cutge);
  magmed->CUTELE(cutge);
//
magmed=(AMSgtmed*)tmed.add (new AMSgtmed("MCOILMED","MCOILMAT",0));
  magmed->CUTGAM(cutge);
  magmed->CUTELE(cutge);
//
magmed=(AMSgtmed*)tmed.add (new AMSgtmed("MLHVMED","MALUMINIUM",0));
  magmed->CUTGAM(cutge);
  magmed->CUTELE(cutge);
//
magmed=(AMSgtmed*)tmed.add (new AMSgtmed("MLHEMED","HELIUM",0));
  magmed->CUTGAM(cutge);
  magmed->CUTELE(cutge);
//
}
//--------------------
{
// ---> AMS02 Radiator+shield/crate/TOF-TRD_interf/USS Media:
//
geant cutge=MAGSFFKEY.ecutge;// increased EgammaEelectron cut
AMSgtmed * radmed=(AMSgtmed*)tmed.add (new AMSgtmed("RADMED1","RADALUMIN1",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("RADMED2","RADALUMIN2",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("CRATEMED1","CRATEMAT1",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("TFSUPMED1","TFSUPALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("MSFMED1","MSFALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("MSBRMED","MSBRALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("US1BMED","US1BALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("US2BMED","US2BALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("US3BMED","US3BALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("US4BMED","US4BALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("US5BMED","US5BALUMIN",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
//
radmed=(AMSgtmed*)tmed.add (new AMSgtmed("ISSTUBEMED","ISSTUBEALU",0));
  radmed->CUTGAM(cutge);
  radmed->CUTELE(cutge);
}
//--------------------

AMSgObj::GTrMedMap.map(tmed);
#ifdef __AMSDEBUG__
if(AMSgtmed::debug)AMSgObj::GTrMedMap.print();
#endif
#ifdef __AMSDEBUG__
if(AMSgmat::debug)GPTMED(0);
#endif
cout <<"AMSgmat::amstmed-I-TotalOf "<<GetLastMedNo()<<" media defined"<<endl;
}
//-----------------------------------------------------------------------------

void AMSgtmed::AGSCKOV(integer nument, geant pmom[], geant absl[], geant eff[], geant rindex[], geant rayleigh){

#ifdef __G4AMS__
if(MISCFFKEY.G4On){
 if(nument>0){
    number *  _pmom=new number[nument];
    number *  _absl=new number[nument];
    number *  _refl=new number[nument];
    number *  _eff=new number[nument];
    number *  _rindex=new number[nument];
    number *  _rayl=new number[nument];
    bool ord=true;
    if(pmom[0]>pmom[nument-1]){
     cerr<<" AMSgtmed::AGSCKOV-W-MomentumArrayNotOrderedFor "<<getname()<<endl;
     ord=false;
    }
    int i;
    for (i=0;i<nument;i++){
     int j=ord?i:nument-1-i;
     _pmom[j]=pmom[i]*GeV;
     _rindex[j]=rindex[i];
     if(rindex[0]>0)_absl[j]=absl[i]*cm;
     else {
       _refl[j]=1-absl[i];   //reflectivity
       _absl[j]=DBL_MAX;
     }
     _eff[j]=eff[i];
     if(rayleigh>0){
      _rayl[j]=1/rayleigh* pow(1.2398E-9/pmom[i],4.)*cm;
     }
     else _rayl[j]=DBL_MAX;
    }
  G4MaterialPropertiesTable *pr = new G4MaterialPropertiesTable();
   pr->AddProperty("ABSLENGTH",_pmom,_absl,nument);
   pr->AddProperty("EFFICIENCY",_pmom,_eff,nument);
  if(rindex[0]>0){
   // Case dielectric
   pr->AddProperty("RINDEX",_pmom,_rindex,nument);
   pr->AddProperty("RAYLEIGH",_pmom,_rayl,nument);
  }
  else{
   // Case Metal
   pr->AddProperty("REFLECTIVITY",_pmom,_refl,nument);
  }
   _pgmat->getpamsg4m()->SetMaterialPropertiesTable(pr);
   delete[] _pmom;
   delete[] _absl;
   delete[] _eff;
   delete[] _rindex;
   delete[] _rayl;
   delete[] _refl;
 }
}
else
#endif
GSCKOV(_itmed,nument,pmom,absl,eff,rindex);
}





