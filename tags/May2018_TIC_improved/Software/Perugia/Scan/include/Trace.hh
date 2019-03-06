#ifndef Trace_hh
#define Trace_hh

#include "TObject.h"
#include "TClonesArray.h"

class Trace:public TObject{

public:

  /*! coordonnes des traces 
     X Horizontal (Saleve a Jura) 
     Y Vertical   (haut a bas) 
     Z direction du faisceau vers +Z)
  */
  double x[6]; // avec alignment    
  double y[6]; 
  double z[6];
  double xsa[6]; // sans alignment positions centree    
  double ysa[6]; 
  double xp[6]; // positions en pistes    
  double yp[6]; 
  int amas_entree[2][6]; // les places des amas du trace dans la liste Cls
  float amas_signal[2][6]; // les signaux des amas
  float dxdz, dydz; // les divergences du faisceau

  Trace();
  //! Default destructor
  ~Trace(){};
  // mise-a-jour des centres du faisceau

  ClassDef(Trace,1)
};



#endif
