#include "TLinearFitter.h"

class Event_Fit:public TLinearFitter {
 
  friend class Event;
  public:
  Event_Fit(){TLinearFitter(1);}
  ClassDef(Event_Fit,1);
};
