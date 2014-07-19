
#ifndef __LIPCOMMONS__
#define __LIPCOMMONS__
#include "typedefs.h"
// GEANT part
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"
//+LIP
// commons

// geometry
class LIPGEO_DEF{
 public:
  // dimensions

  geant ztoprad_ams;
  geant zpmtdet_c;
  geant hmir_c;
  geant rtmir_c;
  geant rbmir_c;
  geant emcxlim_c;
  geant emcylim_c;
  geant ztmirgap_c;
  geant zbmirgap_c;
  geant reflec_c;
  // light guides
  geant lg_length_c;
  geant lg_height_c;
  // pmt
  geant pmt_suptk_c;
  geant pmt_shdtk_c;
  geant pmt_sidel_c;
};
#define LIPGEO COMMON_BLOCK(LIPGEO,lipgeo)
COMMON_BLOCK_DEF(LIPGEO_DEF,LIPGEO);

// radiator
class LIPRAD_DEF{
 public:

  // radiator

  geant hrad_c;
  geant refindex_c;
  geant clarity_c;
  int radtype_c;
  geant rrad_c;
  geant ltile_c;
  int nabslen_c;
  geant labsrad_c[44];
  // foil
  geant hpgl_c;
  geant pglix_c;
};
#define LIPRAD COMMON_BLOCK(LIPRAD,liprad)
COMMON_BLOCK_DEF(LIPRAD_DEF,LIPRAD);


// hit parameters

class LIPDAT_DEF{
 public:
  int     nbhitsmax_c;
  int     nbhits_c;
  int     hitsadc_c[16000];
  geant   hitsnpe_c[16000];
  geant   hitscoo_c[16000][3];
  int     hitshid_c[16000];
};
#define LIPDAT COMMON_BLOCK(LIPDAT,lipdat)
COMMON_BLOCK_DEF(LIPDAT_DEF,LIPDAT);

// track parameters
class LIPTRK_DEF{
 public:
  geant pimp_c[3];
  geant pmom_c;
  geant pthe_c;
  geant pphi_c;
  geant pcoopmt_c[3];
  geant cerang_c;
  geant pbeta_c;
  geant pchg_c;
};
#define LIPTRK COMMON_BLOCK(LIPTRK,liptrk)
COMMON_BLOCK_DEF(LIPTRK_DEF,LIPTRK);

// output parameters
class LIPVAR_DEF{
public:
int   liphused;
geant lipthc;
geant lipbeta;
geant lipebeta;
geant liplikep;
geant lipchi2;
geant liprprob;
};
#define LIPVAR COMMON_BLOCK(LIPVAR,lipvar)
COMMON_BLOCK_DEF(LIPVAR_DEF,LIPVAR);
// ENDofLIP


#endif
