//  $Id: extC.h,v 1.1 2008/07/30 16:53:57 zuccon Exp $
// Author V. Choutko 24-may-1996
//
// Oct 06, 1996. ak. add readDB 
// Oct 16, 1996. ak. add readGeomDB, writeGeomDB
// Oct 03, 1997. ak.
//
// Last Edit :  Oct 3, 1997. ak.
//
#include "typedefs.h"
//
extern "C" number rnormx();
extern "C" number sitkfints_(number &,number &);
extern "C" number sitkfint2_(number&,number&,number&,number&,number&);
extern "C" void uglast_();
extern "C" void uginit_();
extern "C" void gustep_();
extern "C" void guout_();
extern "C" void gukine_();


extern "C" void fit_(geant arr[][8],integer fixpar[][8],geant &chi2m,integer &alg,integer &what, geant xf[],geant chi2[][2],geant &rigmin, integer &itermin);
#define FIT fit_
extern "C" void dinv_(int &ndim, double matrix[], int &ndim2, 
		          double ridaux[], int &ifail);
#define DINV dinv_

extern "C" void invertmatrix_(double matrix[], int & dim0, int & dim1, int &ifail);
#define INVERTMATRIX invertmatrix_


extern "C" void initDB();
extern "C" void readDB();
extern "C" void readSetup();
extern "C" void writeSetup();


extern "C" void cminit_();
extern "C" void cmgene_(geant & pmu, geant & themu, geant & phimu, 
                       geant & chargemu, geant & xmu, geant & ymu, geant &zmu);
extern "C" void cmgene2_(geant & pmu, geant & gamma, geant & chmu);
extern "C" void cmend_();
#define CMEND cmend_
#define CMGENE cmgene_
#define CMGENE2 cmgene2_
#define CMINIT cminit_


