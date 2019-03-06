#include "TLinearFitter.h"

class Track_Fit:public TLinearFitter {
 
  friend class Align;
  public:
  Track_Fit(){TLinearFitter(1);}
  ClassDef(Track_Fit,1);
};
