
//  $Id: mceventg.C,v 1.1 2008/09/11 17:19:22 zuccon Exp $
// Author V. Choutko 24-may-1996
//#undef __ASTRO__ 

#include <limits.h>
#ifdef __DARWIN__
#include <limits.h>   // for FLT_MAX
#include <float.h>    // for FLT_MAX (for Dawin GCC 4)
#else
#include <values.h>   // for FLT_MAX (for Linux)
#endif
#include "mceventg.h"
#include <math.h>
#include "amsdbc.h"
#include "gvolume.h"
#include "amscommonsi.h"
#include "commons.h"
#include "AMSJob.h"
#include "AMSEvent.h"
#include "commons.h"

#include "extC.h"

integer AMSmceventg::_hid=20001;
AMSPoint* AMSmceventg::_r_c;
AMSDir* AMSmceventg::_dir_c;
AMSPoint AMSmceventg::_coorange[2];
AMSPoint AMSmceventg::_dirrange[2];
number   AMSmceventg::_momrange[2];
integer  AMSmceventg::_fixedpoint;
integer  AMSmceventg::_fixedplane;
integer  AMSmceventg::_fixeddir;
integer  AMSmceventg::_fixedmom;
number   AMSmceventg::_albedorate;
number   AMSmceventg::_albedocz; 
number   AMSmceventg::_planesw[6];
int   AMSmceventg::_particle[30];
int   AMSmceventg::_nucleons[30];
geant*   AMSmceventg::_spectra[30];
number   AMSmceventg::_flux[30];





AMSmceventg::AMSmceventg(integer ipart, geant mom, const AMSPoint & coo,
			 const AMSDir & dir, integer nskip):_nskip(nskip),_mom(mom),_coo(coo),_dir(dir){
  init(ipart);
}

void AMSmceventg::gener(){
  // char cdir[256];
//   char *R="R";
//   HCDIR(cdir,R);
//   char hpawc[256]="//PAWC/GEN";
//   char *BLANK=" ";
//   HCDIR (hpawc,BLANK);

  number _gamma=0;
  static integer ini=0;
  integer curp=0;
  number phi;
  number theta;
  geant d(-1);
  _momrange[0]=10;
  _momrange[0]=100;

  if(CCFFKEY.low==5 ){
    if(_momrange[0]<=0 ){
      cerr<<"AMSmceventg::gener-F-LogInfinity "<<endl;
        exit(1);
    }              
    number lrange=log(fabs(_momrange[1]))-log(fabs(_momrange[0]));
    _mom=_momrange[0]*exp(lrange*RNDM(d));
  }
  else return;



  
}




integer AMSmceventg::accept(){
  return 1;
}
integer AMSmceventg::acceptio(){
  if(_coo >= _coorange[0] && _coo <= _coorange[1]){
    if(_fixeddir || (_dir >= _dirrange[0] && _dir<= _dirrange[1])){
      if(_mom>=_momrange[0] && _mom <= _momrange[1]){
	
	return 1;
      }
    }
  }
  return 0;
}

  
void AMSmceventg::gendelay(){

  // Generate a random delay for a track between 0 and 10 microsecs

  geant d(-1);
  _delay=RNDM(d)*10.;

}


void AMSmceventg::init(integer ipart){
  _ipart=ipart;
  _delay = 0.;
  char chn[22]="";
  integer itr=0;
  geant amass=0;
  geant charge=0;
  geant tlife=0;
  geant ubb[1];
  integer one=1;
  GFPART(abs(_ipart)%256,chn,itr,amass,charge,tlife,ubb,one);
  _mass=amass;
  _charge=charge;
}


void AMSmceventg::run(integer ipart){
  geant d(-1);
  init(ipart);
  geant plab[3],vertex[3];
  integer nvert=0;
  integer nt=0;
  do{
    gener();
  }while(!accept());
  // Set seed

  GRNDMQ(_seed[0],_seed[1],0,"G");
  //   cout <<"seed[ "<<_seed[0]<<" "<<_seed[1]<<endl;
  //   cout << "  direction "<<_dir/_dir[2]<<" coo "<<_coo<<endl;
 //  vertex[0]=_coo[0];
//   vertex[1]=_coo[1];
//   vertex[2]=_coo[2];
//   plab[0]=_mom*_dir[0];
//   plab[1]=_mom*_dir[1];
//   plab[2]=_mom*_dir[2];
  _mom=(CCFFKEY.momr[1]-CCFFKEY.momr[0])*RNDM(d) +CCFFKEY.momr[0];
  vertex[0]= (CCFFKEY.coo[1]-CCFFKEY.coo[0])*RNDM(d) +CCFFKEY.coo[0];
  vertex[1]= (CCFFKEY.coo[3]-CCFFKEY.coo[2])*RNDM(d) +CCFFKEY.coo[2];
  vertex[2]= (CCFFKEY.coo[5]-CCFFKEY.coo[4])*RNDM(d) +CCFFKEY.coo[4];



  double th=RNDM(d)*3./180*3.142;
  double ph=RNDM(d)*360./180*3.142;

  _dir[0]= cos(ph)*sin(3.142-th);
  _dir[1]= sin(ph)*sin(3.142-th);
  _dir[2]= cos(3.142-th);
  
  plab[0]=_mom*_dir[0];
  plab[1]=_mom*_dir[1];
  plab[2]=_mom*_dir[2];
  printf("New intial particle : %d\n",_ipart);
  printf("vertex  %f %f %f \n",vertex[0],vertex[1],vertex[2]);
  printf("Plab %f %f %f \n",plab[0],plab[1],plab[2]);
  GSVERT(vertex,0,0,0,0,nvert);
  printf("check nvert1 : %d \n",nvert);
  GSKINE(plab,_ipart,nvert,0,0,nt);
  printf("check nvert2 : %d  ntrack %d \n",nvert,nt );
}

void AMSmceventg::run(){
  geant plab[3],vertex[3];
  integer nvert=0;
  integer nt=0;
  if(acceptio()){
    vertex[0]=_coo[0];
    vertex[1]=_coo[1];
    vertex[2]=_coo[2];
    plab[0]=_mom*_dir[0];
    plab[1]=_mom*_dir[1];
    plab[2]=_mom*_dir[2];
    GSVERT(vertex,0,0,0,0,nvert);
    GSKINE(plab,_ipart,nvert,0,0,nt);
  }   
}

void AMSmceventg::InitSeed(){

  //C
  //C             Initialise the random number generator
  //C
  //      IF(NRNDM(2).NE.0)THEN
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),1,'S')
  //      ELSEIF(NRNDM(1).GT.0)THEN
  //         ISEQ=NRNDM(1)
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'Q')
  //         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'S')
  //      ENDIF
  //
  GCFLAG.NRNDM[0]=_seed[0];
  GCFLAG.NRNDM[1]=_seed[1];
#ifdef __G4AMS__
  if(!MISCFFKEY.G3On){
    long seedl[3]={0,0,0};
    seedl[0]=_seed[0];
    seedl[1]=_seed[1];
    HepRandom::setTheSeeds(seedl);
  }
  else{
#endif
    if(_seed[1] !=0)GRNDMQ(_seed[0],_seed[1],1,"S");
    else if(_seed[0] > 0){
      integer ISEQ=_seed[0];
      GRNDMQ(_seed[0],_seed[1],ISEQ,"Q");
      GRNDMQ(_seed[0],_seed[1],ISEQ,"S");
    }
#ifdef __G4AMS__
  }
#endif
}  

void AMSmceventg::_printEl(ostream &stream){
  geant phi=atan2(_dir[1],_dir[0]);
  geant theta=acos(_dir[2]);
  stream <<" Particle " << _ipart <<" Theta "<<theta<<" Phi "<<phi<<
    " Ridgidity " << _mom/_charge <<endl;
  stream <<" Coo "<<_coo[0]<<" "<<_coo[1]<<" "<<_coo[2]<<endl;
  stream <<" Random numbers "<<_seed[0]<<" "<<_seed[1]<<endl;
}

void AMSmceventg::_copyEl(){

}

void AMSmceventg::_writeEl(){
  
}


extern "C" void getscanfl_(int &scan);
void AMSmceventg::FillMCInfo(){
  static number radl=0;
  static number absl=0;
  static integer init=0;
  static number ECALZ=-FLT_MAX;
  if(!init){
    init=1;
    AMSgvolume *p =AMSJob::gethead()->getgeomvolume(AMSID("PMS1",1));
    if(p){
      ECALZ=p->getcooA(2)*0.95;
      cout <<"AMSmceventg::FillMCInfo-I-LowestZSetTo "<<ECALZ<<endl;
    }
    else{
      cerr<<"AMSmceventg::FillMCInfo-W-NoPMS1VolumeFound"<<endl;
    }
  }

  if(GCTRAK.sleng==0){
    radl=0;
    absl=0;
 
  }
  int scan=0;
  getscanfl_(scan);
  if(GCKINE.itra==1 && scan && GCKINE.ikine==GCKINE.ipart){
    radl+=GCTRAK.step/GCMATE.radl;
    absl+=GCTRAK.step/GCMATE.absl;
    char nvol[5]="EXIT";
    if(GCTRAK.inwvol==1 && GCVOLU.nlevel<500){
      for(int i=0;i<4;i++)nvol[i]=GCVOLU.names[GCVOLU.nlevel-1][i];
      AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
      AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);

    }
    else if(GCTRAK.inwvol==3){
      AMSmctrack* genp=new AMSmctrack(radl,absl,GCTRAK.vect,nvol);
      AMSEvent::gethead()->addnext(AMSID("AMSmctrack",0), genp);
    }
  }
  for (int i=0;i<GCKING.ngkine;i++){
    number mom=(GCKING.gkin[i][0]*GCKING.gkin[i][0]+
		GCKING.gkin[i][1]*GCKING.gkin[i][1]+
		GCKING.gkin[i][2]*GCKING.gkin[i][2]);
    mom=sqrt(mom);
    if(GCKIN3.GPOS[i][2]>ECALZ){
      AMSmceventg* genp=new AMSmceventg(-GCKING.gkin[i][4],mom,
					AMSPoint(GCKIN3.GPOS[i][0],GCKIN3.GPOS[i][1],GCKIN3.GPOS[i][2]),
					AMSDir(GCKING.gkin[i][0],GCKING.gkin[i][1],GCKING.gkin[i][2]));
      AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), genp);
    }
  }

}

integer AMSmceventg::Out(integer status){
  static integer init=0;
  static integer WriteAll=0;
  return (WriteAll || status);
}

bool AMSmceventg::Primary(){
  return _ipart>0 && _ipart<256;
}

void AMSmceventg::endjob(){
//   char cdir[256];
//   char *R="R";
//   HCDIR(cdir,R);
//   char hpawc[256]="//PAWC/GEN";
//   char *BLANK=" ";
//   HCDIR (hpawc,BLANK);
//   if(CCFFKEY.low==2){
//     CMEND();
//   }
//   char hp[9]="//PAWC";
//   HCDIR(hp," ");
//   HCDIR (cdir, " ");
}

void AMSmceventg::SaveSeeds(){
#ifdef __G4AMS__
  if(!MISCFFKEY.G3On){
    GCFLAG.NRNDM[0]=HepRandom::getTheSeeds()[0];
    GCFLAG.NRNDM[1]=HepRandom::getTheSeeds()[1];
  }
  else
#endif
    GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],0,"G");
}

void AMSmceventg::PrintSeeds(ostream & o){
  long tmp[2];
  tmp[0]=GCFLAG.NRNDM[0];
  tmp[1]=GCFLAG.NRNDM[1];
  SaveSeeds();
  o<<GCFLAG.NRNDM[0]<<" "<<GCFLAG.NRNDM[1]<<endl;
  GCFLAG.NRNDM[0]=tmp[0];
  GCFLAG.NRNDM[1]=tmp[1];
}
void AMSmceventg::RestoreSeeds(){
#ifdef __G4AMS__
  if(!MISCFFKEY.G3On){
    long seedl[3]={0,0,0};
    seedl[0]=GCFLAG.NRNDM[0];
    seedl[1]=GCFLAG.NRNDM[1];
    HepRandom::setTheSeeds(seedl);
  }
  else
#endif
    GRNDMQ(GCFLAG.NRNDM[0],GCFLAG.NRNDM[1],1,"S");
}


void AMSmceventg::SetSeed( int seed){

#ifdef __G4AMS__
  if(!MISCFFKEY.G3On){
    HepRandom::setTheSeed(seed);
  }
  else{
#endif
    integer __seed[2];
    GRNDMQ(__seed[0],__seed[1],seed+1,"Q");
    GRNDMQ(__seed[0],__seed[1],seed+1,"S");
#ifdef __G4AMS__
  }

#endif


}



void AMSmctrack::_writeEl(){

}







