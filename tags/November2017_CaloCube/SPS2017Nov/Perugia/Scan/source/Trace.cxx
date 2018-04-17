#include "Trace.hh"

ClassImp(Trace);


Trace::Trace() {


  for (int i=0; i<6; i++) {
    x[i]=0.;
    y[i]=0.;
    z[i]=0.;
    xp[i]=0.;
    yp[i]=0.;
    xsa[i]=0.;
    ysa[i]=0.;
  }

  for (int i=0; i<2; i++)
    for (int j=0; j<6; j++) {
        amas_entree[i][j]=-1;
        amas_signal[i][j]=0.;
    };

  dxdz = 0.;
  dydz = 0.;

}


