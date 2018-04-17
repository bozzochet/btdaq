#include "SClass.hh"

ClassImp(SClass);

 SClass::SClass() {
 fX=-1.;
 fY=-1.;}

void SClass:: Print() const {
  cout << "fX = " << fX << " fY = " << fY << endl;
}
