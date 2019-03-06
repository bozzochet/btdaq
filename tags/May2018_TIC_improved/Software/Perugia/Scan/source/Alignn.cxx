#include "Alignn.hh"

ClassImp(Alignn);

 Alignn::Alignn() {
  for (int i=0; i<4; i++) z_fixe[i]=0;

  for (int i=0; i<4; i++) {
    x[i]=0.;
    y[i]=0.;
    z[i]=0.;}

  for (int i=0; i<2; i++)
    for (int j=0; j<4; j++) nstrip[i][j]=0;
 
  for (int i=0; i<11; i++)
    for (int j=0; j<4; j++) algpar[i][j]=0.;
 
  for (int i=0; i<11; i++)
    for (int j=0; j<4; j++) 
      for (int k=0; k<8; k++) algpar_fich[k][i][j]=0.;

  }

