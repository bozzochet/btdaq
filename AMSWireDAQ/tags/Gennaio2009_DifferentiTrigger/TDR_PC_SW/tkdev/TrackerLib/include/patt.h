#ifndef _patt_H_
#define _patt_H_
#include "typedefs.h"
#include "TkDBc.h"

class PATT{
public:
  static uinteger _Npat;
  static integer * _patconf[trconst::maxlay];
  static integer * _patpoints;
  static integer * _patmiss[trconst::maxlay];
  static integer  _patd[trconst::maxlay]; 
  static integer * _patallow;
  static integer * _patallow2;



  PATT();
  ~PATT();
  void init(int atrefffekypar=1);
  void InitPattern();
  void clear();

  
  static integer patmiss(uinteger pat, uinteger lay){return _patmiss[lay][pat];}
  static integer patconf(uinteger pat, uinteger lay){return _patconf[lay][pat];}
  static integer patconf3(uinteger pat, uinteger lay){return _patconf[lay][pat]-1;}
  static integer patd(uinteger lay){return _patd[lay];}
  static integer patallow(uinteger pat){return pat<_Npat?_patallow[pat]:-1;}
  static integer patallowFalseX(uinteger pat){return pat<_Npat?_patallow2[pat]:-1;}
  static integer ambig(uinteger pat);
  static integer patpoints(uinteger pat){return pat<_Npat?_patpoints[pat]:-1;}
  static uinteger npat(){return _Npat;}
private:
  uinteger factorial(uinteger n);


};

extern PATT patt;
#endif
