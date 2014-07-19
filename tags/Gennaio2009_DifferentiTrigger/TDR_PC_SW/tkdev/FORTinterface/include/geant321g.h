//  $Id: geant321g.h,v 1.1 2008/07/30 16:53:58 zuccon Exp $
#ifndef __GEANT321G__
#define __GEANT321G__
#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

/*------------------------------------------------------------------
fortran filename   : gudcay.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUDCAY,gudcay)
#define GUDCAY() CCALLSFSUB0(GUDCAY,gudcay)

/*------------------------------------------------------------------
fortran filename   : gudigi.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUDIGI,gudigi)
#define GUDIGI() CCALLSFSUB0(GUDIGI,gudigi)

/*------------------------------------------------------------------
fortran filename   : gudtim.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gufld.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV)
#define GUFLD(A1,A2)  CCALLSFSUB2(GUFLD,gufld,FLOATV,FLOATV,A1,A2)

/*------------------------------------------------------------------
fortran filename   : guhadr.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUHADR,guhadr)
#define GUHADR() CCALLSFSUB0(GUHADR,guhadr)

/*------------------------------------------------------------------
fortran filename   : guiget.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB3(GUIGET,guiget,INT,INT,INT)
#define GUIGET(A1,A2,A3)  CCALLSFSUB3(GUIGET,guiget,INT,INT,INT,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : guinme.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB4(GUINME,guinme,FLOATV,FLOAT,FLOATV,PINT)
#define GUINME(A1,A2,A3,A4)  CCALLSFSUB4(GUINME,guinme,FLOATV,FLOAT,FLOATV,PINT,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : guinti.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUINTI,guinti)
#define GUINTI() CCALLSFSUB0(GUINTI,guinti)

/*------------------------------------------------------------------
fortran filename   : gukine.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUKINE,gukine)
#define GUKINE() CCALLSFSUB0(GUKINE,gukine)

/*------------------------------------------------------------------
fortran filename   : gunear.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB4(GUNEAR,gunear,INT,INT,FLOATV,INT)
#define GUNEAR(A1,A2,A3,A4)  CCALLSFSUB4(GUNEAR,gunear,INT,INT,FLOATV,INT,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : gunliv.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB3(GUNLIV,gunliv,INT,INT,INT)
#define GUNLIV(A1,A2,A3)  CCALLSFSUB3(GUNLIV,gunliv,INT,INT,INT,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : guout.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUOUT,guout)
#define GUOUT() CCALLSFSUB0(GUOUT,guout)

/*------------------------------------------------------------------
fortran filename   : gupara.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUPARA,gupara)
#define GUPARA() CCALLSFSUB0(GUPARA,gupara)

/*------------------------------------------------------------------
fortran filename   : guphad.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUPHAD,guphad)
#define GUPHAD() CCALLSFSUB0(GUPHAD,guphad)

/*------------------------------------------------------------------
fortran filename   : guplsh.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : guskip.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB1(GUSKIP,guskip,PINT)
#define GUSKIP(A1)  CCALLSFSUB1(GUSKIP,guskip,PINT,A1)

/*------------------------------------------------------------------
fortran filename   : gustep.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUSTEP,gustep)
#define GUSTEP() CCALLSFSUB0(GUSTEP,gustep)

/*------------------------------------------------------------------
fortran filename   : guswim.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB4(GUSWIM,guswim,FLOAT,FLOAT,FLOATV,FLOATV)
#define GUSWIM(A1,A2,A3,A4)  CCALLSFSUB4(GUSWIM,guswim,FLOAT,FLOAT,FLOATV,FLOATV,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : gutrak.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUTRAK,gutrak)
#define GUTRAK() CCALLSFSUB0(GUTRAK,gutrak)

/*------------------------------------------------------------------
fortran filename   : gutrev.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GUTREV,gutrev)
#define GUTREV() CCALLSFSUB0(GUTREV,gutrev)

/*------------------------------------------------------------------
fortran filename   : guview.f
------------------------------------------------------------------*/
/*
#define guview_ELEMS_3          ZTRINGV_NUM(1)
#define guview_ELEMLEN_3        ZTRINGV_NUM(255)
*/

PROTOCCALLSFSUB4(GUVIEW,guview,INT,INT,STRING,INT)
#define GUVIEW(A1,A2,A3,A4)  CCALLSFSUB4(GUVIEW,guview,INT,INT,STRING,INT,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : gvaviv.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gvdcar.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB7(GVDCAR,gvdcar,INT,INT,INT,FLOATV,PFLOAT,PFLOAT,PINT)
#define GVDCAR(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(GVDCAR,gvdcar,INT,INT,INT,FLOATV,PFLOAT,PFLOAT,PINT,A1,A2,A3,A4,A5,A6,A7)

/*------------------------------------------------------------------
fortran filename   : gvdlim.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB6(GVDLIM,gvdlim,INT,INT,INT,PFLOAT,PFLOAT,INT)
#define GVDLIM(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(GVDLIM,gvdlim,INT,INT,INT,PFLOAT,PFLOAT,INT,A1,A2,A3,A4,A5,A6)

/*------------------------------------------------------------------
fortran filename   : gvdphi.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB7(GVDPHI,gvdphi,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT)
#define GVDPHI(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(GVDPHI,gvdphi,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT,A1,A2,A3,A4,A5,A6,A7)

/*------------------------------------------------------------------
fortran filename   : gvdrad.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB8(GVDRAD,gvdrad,INT,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT)
#define GVDRAD(A1,A2,A3,A4,A5,A6,A7,A8)  CCALLSFSUB8(GVDRAD,gvdrad,INT,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT,A1,A2,A3,A4,A5,A6,A7,A8)

/*------------------------------------------------------------------
fortran filename   : gvdthe.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB7(GVDTHE,gvdthe,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT)
#define GVDTHE(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(GVDTHE,gvdthe,INT,INT,FLOATV,FLOATV,PFLOAT,PFLOAT,PINT,A1,A2,A3,A4,A5,A6,A7)

/*------------------------------------------------------------------
fortran filename   : gvgrad.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB4(GVGRAD,gvgrad,FLOATV,FLOATV,INT,PFLOAT)
#define GVGRAD(A1,A2,A3,A4)  CCALLSFSUB4(GVGRAD,gvgrad,FLOATV,FLOATV,INT,PFLOAT,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : gviewf.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB3(GVIEWF,gviewf,FLOATV,FLOATV,INT)
#define GVIEWF(A1,A2,A3)  CCALLSFSUB3(GVIEWF,gviewf,FLOATV,FLOATV,INT,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : gvlong.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB5(GVLONG,gvlong,FLOATV,FLOATV,INT,PFLOAT,PINT)
#define GVLONG(A1,A2,A3,A4,A5)  CCALLSFSUB5(GVLONG,gvlong,FLOATV,FLOATV,INT,PFLOAT,PINT,A1,A2,A3,A4,A5)

/*------------------------------------------------------------------
fortran filename   : gvpsqr.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB3(GVPSQR,gvpsqr,FLOATV,PFLOAT,PINT)
#define GVPSQR(A1,A2,A3)  CCALLSFSUB3(GVPSQR,gvpsqr,FLOATV,PFLOAT,PINT,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : gvrot.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB2(GVROT,gvrot,FLOATV,PFLOAT)
#define GVROT(A1,A2)  CCALLSFSUB2(GVROT,gvrot,FLOATV,PFLOAT,A1,A2)

/*------------------------------------------------------------------
fortran filename   : gvsafe.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gvsign.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gweucl.f
------------------------------------------------------------------*/
/*
#define gweucl_ELEMS_2          ZTRINGV_NUM(1)
#define gweucl_ELEMLEN_2        ZTRINGV_NUM(255)
#define gweucl_ELEMS_3          ZTRINGV_NUM(1)
#define gweucl_ELEMLEN_3        ZTRINGV_NUM(4)
*/

PROTOCCALLSFSUB5(GWEUCL,gweucl,INT,STRING,STRING,INT,INT)
#define GWEUCL(A1,A2,A3,A4,A5)  CCALLSFSUB5(GWEUCL,gweucl,INT,STRING,STRING,INT,INT,A1,A2,A3,A4,A5)

/*------------------------------------------------------------------
fortran filename   : gwork.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB1(GWORK,gwork,PINT)
#define GWORK(A1)  CCALLSFSUB1(GWORK,gwork,PINT,A1)

/*------------------------------------------------------------------
fortran filename   : gwrtre.f
------------------------------------------------------------------*/
/*
#define gwrtre_ELEMS_1          ZTRINGV_NUM(1)
#define gwrtre_ELEMLEN_1        ZTRINGV_NUM(4)
*/

PROTOCCALLSFSUB2(GWRTRE,gwrtre,STRING,INT)
#define GWRTRE(A1,A2)  CCALLSFSUB2(GWRTRE,gwrtre,STRING,INT,A1,A2)

/*------------------------------------------------------------------
fortran filename   : gxclos.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB1(GXCLOS,gxclos,INT)
#define GXCLOS(A1)  CCALLSFSUB1(GXCLOS,gxclos,INT,A1)

/*------------------------------------------------------------------
fortran filename   : gxcont.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXCONT,gxcont)
#define GXCONT() CCALLSFSUB0(GXCONT,gxcont)

/*------------------------------------------------------------------
fortran filename   : gxcs.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXCS,gxcs)
#define GXCS() CCALLSFSUB0(GXCS,gxcs)

/*------------------------------------------------------------------
fortran filename   : gxdraw.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXDRAW,gxdraw)
#define GXDRAW() CCALLSFSUB0(GXDRAW,gxdraw)

/*------------------------------------------------------------------
fortran filename   : gxdz.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXDZ,gxdz)
#define GXDZ() CCALLSFSUB0(GXDZ,gxdz)

/*------------------------------------------------------------------
fortran filename   : gxfca.f
------------------------------------------------------------------*/
/*
#define gxfca_ELEMS_1          ZTRINGV_NUM(1)
#define gxfca_ELEMLEN_1        ZTRINGV_NUM(255)
*/

PROTOCCALLSFSUB4(GXFCA,gxfca,PSTRING,PINT,PINT,INT)
#define GXFCA(A1,A2,A3,A4)  CCALLSFSUB4(GXFCA,gxfca,PSTRING,PINT,PINT,INT,A1,A2,A3,A4)

/*------------------------------------------------------------------
fortran filename   : gxfort.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXFORT,gxfort)
#define GXFORT() CCALLSFSUB0(GXFORT,gxfort)

/*------------------------------------------------------------------
fortran filename   : gxfz.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXFZ,gxfz)
#define GXFZ() CCALLSFSUB0(GXFZ,gxfz)

/*------------------------------------------------------------------
fortran filename   : gxgcon.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXGCON,gxgcon)
#define GXGCON() CCALLSFSUB0(GXGCON,gxgcon)

/*------------------------------------------------------------------
fortran filename   : gxgeom.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXGEOM,gxgeom)
#define GXGEOM() CCALLSFSUB0(GXGEOM,gxgeom)

/*------------------------------------------------------------------
fortran filename   : gxgint.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gxlist.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXLIST,gxlist)
#define GXLIST() CCALLSFSUB0(GXLIST,gxlist)

/*------------------------------------------------------------------
fortran filename   : gxlunf.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB3(GXLUNF,gxlunf,INT,INT,PINT)
#define GXLUNF(A1,A2,A3)  CCALLSFSUB3(GXLUNF,gxlunf,INT,INT,PINT,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : gxobj.f
------------------------------------------------------------------*/
/*
#define gxobj_ELEMS_1          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_1        ZTRINGV_NUM(255)
#define gxobj_ELEMS_2          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_2        ZTRINGV_NUM(255)
#define gxobj_ELEMS_3          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_3        ZTRINGV_NUM(255)
#define gxobj_ELEMS_4          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_4        ZTRINGV_NUM(255)
#define gxobj_ELEMS_5          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_5        ZTRINGV_NUM(255)
#define gxobj_ELEMS_6          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_6        ZTRINGV_NUM(255)
#define gxobj_ELEMS_7          ZTRINGV_NUM(1)
#define gxobj_ELEMLEN_7        ZTRINGV_NUM(255)
*/

PROTOCCALLSFSUB7(GXOBJ,gxobj,STRING,STRING,STRING,PSTRING,PSTRING,STRING,PSTRING)
#define GXOBJ(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(GXOBJ,gxobj,STRING,STRING,STRING,PSTRING,PSTRING,STRING,PSTRING,A1,A2,A3,A4,A5,A6,A7)

/*------------------------------------------------------------------
fortran filename   : gxphys.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXPHYS,gxphys)
#define GXPHYS() CCALLSFSUB0(GXPHYS,gxphys)

/*------------------------------------------------------------------
fortran filename   : gxpick.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXPICK,gxpick)
#define GXPICK() CCALLSFSUB0(GXPICK,gxpick)

/*------------------------------------------------------------------
fortran filename   : gxrz.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXRZ,gxrz)
#define GXRZ() CCALLSFSUB0(GXRZ,gxrz)

/*------------------------------------------------------------------
fortran filename   : gxscah.f
------------------------------------------------------------------*/
/*
#define gxscah_ELEMS_2          ZTRINGV_NUM(1)
#define gxscah_ELEMLEN_2        ZTRINGV_NUM(255)
#define gxscah_ELEMS_3          ZTRINGV_NUM(1)
#define gxscah_ELEMLEN_3        ZTRINGV_NUM(255)
*/

PROTOCCALLSFSUB3(GXSCAH,gxscah,INT,STRING,STRING)
#define GXSCAH(A1,A2,A3)  CCALLSFSUB3(GXSCAH,gxscah,INT,STRING,STRING,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : gxscal.f
------------------------------------------------------------------*/
/*
#define gxscal_ELEMS_2          ZTRINGV_NUM(1)
#define gxscal_ELEMLEN_2        ZTRINGV_NUM(255)
#define gxscal_ELEMS_3          ZTRINGV_NUM(1)
#define gxscal_ELEMLEN_3        ZTRINGV_NUM(255)
*/

PROTOCCALLSFSUB3(GXSCAL,gxscal,PINT,STRING,STRING)
#define GXSCAL(A1,A2,A3)  CCALLSFSUB3(GXSCAL,gxscal,PINT,STRING,STRING,A1,A2,A3)

/*------------------------------------------------------------------
fortran filename   : gxscan.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GXSCAN,gxscan)
#define GXSCAN() CCALLSFSUB0(GXSCAN,gxscan)

/*------------------------------------------------------------------
fortran filename   : gxsi.f
------------------------------------------------------------------*/


/*------------------------------------------------------------------
fortran filename   : gzebra.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB1(GZEBRA,gzebra,INT)
#define GZEBRA(A1)  CCALLSFSUB1(GZEBRA,gzebra,INT,A1)

/*------------------------------------------------------------------
fortran filename   : gzinit.f
------------------------------------------------------------------*/

PROTOCCALLSFSUB0(GZINIT,gzinit)
#define GZINIT() CCALLSFSUB0(GZINIT,gzinit)



struct GCMATE_DEF {
  integer nmat;
  integer namate[5];
  float a;
  float z;
  float dens;
  float radl;
  float absl;


};

#define GCMATE COMMON_BLOCK(GCMATE,gcmate)
COMMON_BLOCK_DEF(GCMATE_DEF,GCMATE);



/*
      PARAMETER (MXGKIN=100)
      COMMON/GCKING/KCASE,NGKINE,GKIN(5,MXGKIN),
     +                           TOFD(MXGKIN),IFLGK(MXGKIN)
      INTEGER       KCASE,NGKINE ,IFLGK,MXPHOT,NGPHOT
      REAL          GKIN,TOFD,XPHOT
C
      PARAMETER (MXPHOT=800)
      COMMON/GCKIN2/NGPHOT,XPHOT(11,MXPHOT)
C
      COMMON/GCKIN3/GPOS(3,MXGKIN)
      REAL          GPOS

*/

struct GCKING_DEF{
int kcase;
int ngkine;
float gkin[100][5];
float tofd[100];
int iflgk[100];
};
#define GCKING COMMON_BLOCK(GCKING,gcking)
COMMON_BLOCK_DEF(GCKING_DEF,GCKING);

// structure for CKOV photons

struct GCKIN2_DEF{
int ngphot;
float xphot[800][11];
};

#define GCKIN2 COMMON_BLOCK(GCKIN2,gckin2)
COMMON_BLOCK_DEF(GCKIN2_DEF,GCKIN2);

//


struct GCKIN3_DEF{
float GPOS[100][3];
};

#define GCKIN3 COMMON_BLOCK(GCKIN3,gckin3)
COMMON_BLOCK_DEF(GCKIN3_DEF,GCKIN3);

struct GCKINE_DEF{
integer ikine;
float pkine[10];
integer itra;
integer istak;
integer ivert;
integer ipart;
integer itrtyp;
integer napart[5];
float amass;
float charge;
float tlife;
float vert[3];
float pvert[4];
integer ipaold;
};
#define GCKINE COMMON_BLOCK(GCKINE,gckine)
COMMON_BLOCK_DEF(GCKINE_DEF,GCKINE);

//      PARAMETER (MAXMEC=30)
//      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
//     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
//     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
//     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
//     + ,NLVSAV,ISTORY

const integer maxmec=30;
struct GCTRAK_DEF{
float vect[7];
float getot;
float gekin;
float vout[7];
integer nmec;
integer lmec[maxmec];
integer namec[maxmec];
integer nstep;
integer maxnst;
float destep;
float destel;
float safety;
float sleng;
float step;
float snext;
float sfield;
float tofg;
float gekrat;
float upwght;
integer ignext;
integer inwvol;
integer istop;
integer igauto;
integer iekbin;
integer ilosl;
integer imull;
integer ingoto;
integer nldown;
integer nlevin;
integer nlvsav;
integer istory;
};

#define GCTRAK COMMON_BLOCK(GCTRAK,gctrak)
COMMON_BLOCK_DEF(GCTRAK_DEF,GCTRAK);



//      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
//     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
//     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
//     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
//     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
//     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
//     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
//     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
//     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
//     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
//     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
//     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
//     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA



struct GCPHYS_DEF{
int IPAIR;
geant SPAIR;
geant SLPAIR;
geant ZINTPA;
geant STEPPA;
int ICOMP;
geant SCOMP;
geant SLCOMP;
geant ZINTCO;
geant STEPCO;
int IPHOT;
geant SPHOT;
geant SLPHOT;
geant ZINTPH;
geant STEPPH;
int IPFIS;
geant SPFIS;
geant SLPFIS;
geant ZINTPF;
geant STEPPF;
int IDRAY;
geant SDRAY;
geant SLDRAY;
geant ZINTDR;
geant STEPDR;
int IANNI;
geant SANNI;
geant SLANNI;
geant ZINTAN;
geant STEPAN;
int IBREM;
geant SBREM;
geant SLBREM;
geant ZINTBR;
geant STEPBR;
int IHADR;
geant SHADR;
geant SLHADR;
geant ZINTHA;
geant STEPHA;
int IMUNU;
geant SMUNU;
geant SLMUNU;
geant ZINTMU;
geant STEPMU;
int IDCAY;
geant SDCAY;
geant SLIFE;
geant SUMLIF;
geant DPHYS1;
int ILOSS;
geant SLOSS;
geant SOLOSS;
geant STLOSS;
geant DPHYS2;
int IMULS;
geant SMULS;
geant SOMULS;
geant STMULS;
geant DPHYS3;
int IRAYL;
geant SRAYL;
geant SLRAYL;
geant ZINTRA;
geant STEPRA;
};
#define GCPHYS COMMON_BLOCK(GCPHYS,gcphys)
COMMON_BLOCK_DEF(GCPHYS_DEF,GCPHYS);


//      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
//     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD 

struct GCTMED_DEF{
integer numed;
integer natmed[5];
integer isvol;
integer ifield;
float fieldm;
float tmaxfd;
float stemax;
float deemax;
float epsil;
float stmin;
float cfield;
float prec;
integer iupd;
integer istpar;
integer numod;
};

#define GCTMED COMMON_BLOCK(GCTMED,gctmed)
COMMON_BLOCK_DEF(GCTMED_DEF,GCTMED);



//COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
struct GCTLIT_DEF{
float THRIND;
float PMIN;
float DP;
float DNDL;
int JMIN;
int ITCKOV;
int IMCKOV;
int NPCKOV;
};

#define GCTLIT COMMON_BLOCK(GCTLIT,gctlit)
COMMON_BLOCK_DEF(GCTLIT_DEF,GCTLIT);

//     COMMON/GCVOLU/NLEVEL,NAMES(15),NUMBER(15),
//     +LVOLUM(15),LINDEX(15),INFROM,NLEVMX,NLDEV(15),LINMX(15),
//     +GTRAN(3,15),GRMAT(10,15),GONLY(15),GLX(3)


     struct GCVOLU_DEF{
integer nlevel;
char names[15][4];
integer number[15];
integer lvolum[15];
integer lindex[15];
integer infrom;
integer nlevmx;
integer nldev[15];
integer linmx[15];
float gtran[15][3];
float grmat[15][10];
char gonly[15][4];
float glx[3];
     };

#define GCVOLU COMMON_BLOCK(GCVOLU,gcvolu)
COMMON_BLOCK_DEF(GCVOLU_DEF,GCVOLU);


struct GCLINK_DEF{
 integer JDIGI;
 integer JDRAW;
 integer JHEAD;
 integer JHITS;
 integer JKINE;
 integer JMATE ;
 integer JPART;
 integer JROTM ;
 integer JRUNG ;
 integer JSET  ;
 integer JSTAK ;
 integer JGSTAT;
 integer JTMED ;
 integer JTRACK;
 integer JVERTX;
 integer JVOLUM;
 integer JXYZ ;
 integer JGPAR;
 integer JGPAR2;
 integer JSKLT;
};


#define GCLINK COMMON_BLOCK(GCLINK,gclink)
COMMON_BLOCK_DEF(GCLINK_DEF,GCLINK);




struct GCFLAG_DEF{
integer IDEBUG;
integer IDEMIN;
integer IDEMAX;
integer ITEST;
integer IDRUN;
integer IDEVT;
integer IEORUN;
integer IEOTRI;
integer IEVENT;
integer ISWIT[10];
integer IFINIT[20];
integer NEVENT;
integer NRNDM[2];
       };


#define GCFLAG COMMON_BLOCK(GCFLAG,gcflag)
COMMON_BLOCK_DEF(GCFLAG_DEF,GCFLAG);

//     COMMON/GCTIME/TIMINT,TIMEND,ITIME,IGDATE,IGTIME
//      INTEGER ITIME,IGDATE,IGTIME
//      REAL TIMINT,TIMEND


struct GCTIME_DEF{
float TIMINT;
float TIMEND;
int ITIME;
int IGDATE;
int IGTIME;
       };


#define GCTIME COMMON_BLOCK(GCTIME,gctime)
COMMON_BLOCK_DEF(GCTIME_DEF,GCTIME);


//      COMMON/GCCUTS/CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
//     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS(5)

struct GCCUTS_DEF{
float CUTGAM;
float CUTELE;
float CUTNEU;
float CUTHAD;
float CUTMUO;
float BCUTE;
float BCUTM;
float DCUTE;
float DCUTM;
float PPCUTM;
float TOFMAX;
float GCUTS[5];
};

#define GCCUTS COMMON_BLOCK(GCCUTS,gccuts)
COMMON_BLOCK_DEF(GCCUTS_DEF,GCCUTS);



//      COMMON /GCSTAK/ NJTMAX, NJTMIN, NTSTKP, NTSTKS, NDBOOK, NDPUSH,
//     +                NJFREE, NJGARB, NJINVO, LINSAV(15), LMXSAV(15)

struct GCSTAK_DEF{
int NJTMAX;
int NJTMIN;
int  NTSTKP;
int  NTSTKS;
int  NDBOOK;
int  NDPUSH;
int NJFREE;
int  NJGARB;
int  NJINVO;
int  LINSAV[15];
int  LMXSAV[15];
};

#define GCSTAK COMMON_BLOCK(GCSTAK,gcstak)
COMMON_BLOCK_DEF(GCSTAK_DEF,GCSTAK);

//     COMMON /GCNUMX/ NALIVE,NTMSTO     
    struct GCNUMX_DEF{
        int NALIVE;
        int NTMSTO;
   };
#define GCNUMX COMMON_BLOCK(GCNUMX,gcnumx)
COMMON_BLOCK_DEF(GCNUMX_DEF,GCNUMX);

#endif
