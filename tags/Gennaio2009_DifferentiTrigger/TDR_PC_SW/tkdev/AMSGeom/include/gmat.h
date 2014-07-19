//  $Id: gmat.h,v 1.2 2008/07/30 16:51:41 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// Sep 06, 1996 ak. add getnpar and getNumbers functions
//
// Last Edit : Sep 08, 1996, ak. 
//
#ifndef __AMSGMAT__
#define __AMSGMAT__ 
#include "node.h"
#include "typedefs.h"
#include <string.h>
#include "cern.h"
#include "amsdbc.h"
#include "amsgobj.h"
#include "geantnamespace.h"
class AMSgmat : public AMSNode 
{
 
public:
  static void amsmat();
  friend class AMSgtmed;
  AMSgmat (): AMSNode(0),_pamsg4m(0){};
  virtual ~AMSgmat(){delete[] _a;_a=0;delete[] _z;_z=0;delete[] _w;_w=0;}
  AMSgmat (const char name[], 
	   geant a[] , geant z[], geant w[], integer npar, 
	   geant rho, geant radl=0, geant absl=0, geant temp=0 ): 
    _imate(++_GlobalMatI),_npar(npar), _rho(rho), _radl(radl), _absl(absl),AMSNode(0),_pamsg4m(0),_temp(temp){
#ifdef __AMSDEBUG__
    assert(npar>0);
#endif    
    setname(name);
    setid(0);
    _a=new geant[npar];
    _z=new geant[npar];
    _w=new geant[npar];
    UCOPY(a,_a,npar*sizeof(_a[0])/4);
    UCOPY(z,_z,npar*sizeof(_z[0])/4);
    UCOPY(w,_w,npar*sizeof(_w[0])/4);
  }
  static uinteger GetLastMatNo(){return _GlobalMatI;}
  AMSgmat (const char name[], 
	   geant a , geant z,  
	   geant rho, geant radl, geant absl, geant temp=0 ): 
    _imate(++_GlobalMatI), _rho(rho), _radl(radl), _absl(absl),AMSNode(0),_pamsg4m(0),_temp(temp){
    
    setname(name);
    setid(0);
    _npar=1;
    _a=new geant[_npar];
    _a[0]=a;
    _z=new geant[_npar];
    _z[0]=z;
    _w=new geant[_npar];
    _w[0]=1;   
  }
  AMSgmat * next(){return (AMSgmat *)AMSNode::next();}
  AMSgmat * prev(){return (AMSgmat *)AMSNode::prev();}
  AMSgmat * up(){return   (AMSgmat *)AMSNode::up();}
  AMSgmat * down(){return (AMSgmat *)AMSNode::down();}
  static integer debug;
  static AMSgmat * getpmat(const char name[]){return (AMSgmat *)AMSgObj::GTrMatMap.getp(AMSID(name,0));}
#ifdef __DB__
  //+
  friend class AMSgmatD;
  inline integer getnpar() {return _npar;}
  void getNumbers(geant* a, geant* z, geant* w)
  {
    UCOPY (_a, a, sizeof(_a[0])*_npar/4);
    UCOPY (_z, z, sizeof(_z[0])*_npar/4);
    if (_npar > 1) UCOPY (_w, w, sizeof(_w[0])*_npar/4);
    if (_npar < 2) w[0] = 0;
  }
  //-
#endif
  integer getmati(){return _imate;}
  G4Material* getpamsg4m(){return _pamsg4m;}   
protected:
  void _init();
  static uinteger _GlobalMatI;
  AMSgmat (const AMSgmat&);   // do not want cc
  AMSgmat &operator=(const AMSgmat&); // do not want ass
  G4Material* _pamsg4m;
  integer _imate;
  geant * _a;  //   ! atomic num
  geant * _z;  //   ! z
  geant * _w;  //   ! weight in atoms number
  integer _npar; // npar for compaund
  geant _rho; //density
  geant _radl; //X_0
  geant _absl; //Abs l
  geant _ubuf[1];
  geant _temp;
  virtual ostream & print(ostream &)const;
};

class AMSgtmed : public AMSNode 
{
 
public:
  static void amstmed();
  virtual ~AMSgtmed(){delete [] _uwbuf;_uwbuf=0;}
  AMSgtmed (): AMSNode(0),_pgmat(0),_uwbuf(0){};
  AMSgtmed (const char name[], const char matname[], 
	    integer isvol=0 , char yb='N', geant birks[3]=0,
	    integer ifield=1, geant fieldm=20, 
            geant tmaxfd=10, geant stemax=1000, 
            geant deemax=-1, geant epsil=0.001,
            geant stmin=-1):_G4Range(-1 ),_G4EHad(GCCUTS.CUTHAD),_G4EEl(GCCUTS.CUTELE),_G4EPh(GCCUTS.CUTGAM),_G4EHNeu(GCCUTS.CUTNEU),_G4EMu(GCCUTS.CUTMUO),_G4TofMax(GCCUTS.TOFMAX),
			    _itmed(++_GlobalMedI),_isvol(isvol), _ifield(ifield),_fieldm(fieldm),_tmaxfd(tmaxfd),
			    _stemax(stemax),_deemax(deemax),_epsil(epsil),_stmin(stmin),_yb(yb),_nwbuf(0),_uwbuf(0),
			    AMSNode(){
    _pgmat=AMSgmat::getpmat(matname); 
    if(!_pgmat){
      cerr<<"AMSgtmed-ctor-F-NoMaterialFound "<<matname <<" for media "<<name<<endl;
      exit(1);
    }
    setname(name);
    setid(0); 
   
    if(birks)for(int i=0;i<3;i++)_birks[i]=birks[i];
    else     for(int i=0;i<3;i++)_birks[i]=0;
  }
  void setubuf(int nwbuf, geant uwbuf[]);
  float  getubuf(uinteger i){return i<_nwbuf?_uwbuf[i]:0;}
  AMSgtmed * next(){return (AMSgtmed *)AMSNode::next();}
  AMSgtmed * prev(){return (AMSgtmed *)AMSNode::prev();}
  AMSgtmed * up(){return   (AMSgtmed *)AMSNode::up();}
  AMSgtmed * down(){return (AMSgtmed *)AMSNode::down();}
  static integer debug;
  integer getmedia(){return _itmed;}
  static integer gettmed(const char name[]){AMSgtmed *p =(AMSgtmed *)AMSgObj::GTrMedMap.getp(AMSID(name,0));return p?p->_itmed:0;}
  //+
#ifdef __DB__
  friend class AMSgtmedD;
#endif
  char getyb() {return _yb;}
  //-

  static uinteger GetLastMedNo(){return _GlobalMedI;}
  AMSgmat * getpgmat(){ return _pgmat;}
  integer IsSensitive() const {return _isvol?1:0;}
  geant & CUTGAM(){ return _G4EPh;}
  geant & CUTHAD(){ return _G4EHad;}
  geant & CUTNEU(){ return _G4EHNeu;}
  geant & CUTELE(){ return _G4EEl;}
  geant & CUTMUO(){ return _G4EMu;}
  geant & CUTRANGE(){ return _G4Range;}
  geant & TOFMAX(){ return _G4TofMax;}
  void AGSCKOV(integer nument, geant pmom[], geant absl[], geant eff[], geant rindex[], geant rayleigh);
  void     CUTGAM(geant cut){_G4EPh=cut;GSTPAR(_itmed,"CUTGAM",cut);}
  void     CUTHAD(geant cut){_G4EHad=cut;GSTPAR(_itmed,"CUTHAD",cut);}
  void     CUTNEU(geant cut){_G4EHNeu=cut;GSTPAR(_itmed,"CUTNEU",cut);}
  void     CUTELE(geant cut){_G4EEl=cut;GSTPAR(_itmed,"CUTELE",cut);}
  void     CUTMUO(geant cut){_G4EMu=cut;GSTPAR(_itmed,"CUTMUO",cut);}
  void     TOFMAX(geant cut){_G4TofMax=cut;GSTPAR(_itmed,"TOFMAX",cut);}
protected:
  void _init();
  static uinteger _GlobalMedI;
  AMSgtmed (const AMSgtmed&);   // do not want cc
  AMSgtmed &operator=(const AMSgtmed&); // do not want ass
  integer _isvol;
  integer _itmed;
  AMSgmat * _pgmat;
  integer _ifield;
  geant _fieldm;
  geant _tmaxfd;
  geant _stemax;
  geant _deemax;
  geant _epsil;
  geant _stmin;
  integer _nwbuf;
  geant * _uwbuf;
  geant _birks[3];
  char _yb;  
  geant _G4Range;
  geant _G4EHad;
  geant _G4EHNeu;
  geant _G4EEl;
  geant _G4EMu;
  geant _G4EPh;
  geant _G4TofMax;
  virtual ostream & print(ostream &)const;
};
#endif
