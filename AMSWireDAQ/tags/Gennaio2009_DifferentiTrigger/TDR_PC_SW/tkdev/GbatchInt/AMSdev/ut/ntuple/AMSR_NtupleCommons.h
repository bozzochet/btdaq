//  $Id: AMSR_NtupleCommons.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef AMSR_NtupleCommons_H
#define AMSR_NtupleCommons_H

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif

#include "cfortran.h"
//#include "hbook.h"

//static const int NWPAW=1000000;
static const int NWPAW=4000000;
struct PAWC_DEF { float hmem[NWPAW]; };
#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);

//************************************************
// start definition of common blocks in AMS Ntuple
// =====               ------------     =========
//-----------------------------------------------

struct EVENTH_DEF {
   int   eventno;
   int   run;
   int   runtype;
   int   time[2];
   int   Rawwords;
   float Rads;
   float Thetas;
   float Phis;
   float Yaws;
   float Pitchs;
   float Rolls;
   float Velocitys;
   float Veltheta;
   float Velphi;
   float Thetam;
   float Phim;
   int   Particles;
   int   Tracks;
   int   Betas;
   int   Charges;
   int   Trrechits;
   int   Trclusters;
   int   Trrawclusters;
   int   Trmcclusters;
   int   Tofclusters;
   int   Tofmcclusters;
   int   Ctcclusters;
   int   Ctcmcclusters;
   int   Antimcclusters;
   int   Anticlusters;
   int   Eventstatus;
};
#define blkEventh COMMON_BLOCK(EVENTH,eventh)
COMMON_BLOCK_DEF(EVENTH_DEF,blkEventh);

static const int NBETA = 150;
struct BETA_DEF {
   int   nbeta;
   int   betastatus[NBETA];
   int   betapattern[NBETA];
   float beta[NBETA];
   float betac[NBETA];
   float betaerror[NBETA];
   float betaerrorc[NBETA];
   float betachi2[NBETA];
   float betachi2s[NBETA];
   int   betantof[NBETA];
   int   betaptof[NBETA][4];
   int   betaptr[NBETA];
};
#define blkBeta COMMON_BLOCK(BETA,beta)
COMMON_BLOCK_DEF(BETA_DEF,blkBeta);

static const int NCHARGE = 150;
struct CHARGE_DEF {
   int   ncharge;
   int   chargestatus[NCHARGE];
   int   chargebetap[NCHARGE];
   int   chargetof[NCHARGE];
   int   chargetracker[NCHARGE];
   float probtof[NCHARGE][4];
   float chintof[NCHARGE][4];
   float probtracker[NCHARGE][4];
   float chintracker[NCHARGE][4];
   float proballtracker[NCHARGE];
   float truntof[NCHARGE];
   float truntofd[NCHARGE];
   float truntracker[NCHARGE];
};
#define blkCharge COMMON_BLOCK(CHARGE,charge)
COMMON_BLOCK_DEF(CHARGE_DEF,blkCharge);

static const int NPART = 100;
struct PARTICLE_DEF {
   int   npart;
   int   pbetap[NPART];
   int   pchargep[NPART];
   int   ptrackp[NPART];
   int   pid[NPART];
   int   pidvice[NPART];
   int   probpid[NPART][2];
   int   fitmom[NPART];
   float pmass[NPART];
   float perrmass[NPART];
   float pmom[NPART];
   float perrmom[NPART];
   float pcharge[NPART];
   float ptheta[NPART];
   float pphi[NPART];
   float thetagl[NPART];
   float phigl[NPART];
   float pcoo[NPART][3];
   int   atcnbcel[NPART][2];
   float atcnbphe[NPART][2];
   int   atcidcel[NPART][2];
   int   atcdispm[NPART][2];
   int   atcdaero[NPART][2];
   int   atcstatu[NPART][2];
   float atcbeta[NPART];
   float cooctc[NPART][2][3];
   float cootof[NPART][4][3];
   float cooanti[NPART][2][3];
   float cootr[NPART][6][3];
};
#define blkParticle COMMON_BLOCK(PARTICLE,particle)
COMMON_BLOCK_DEF(PARTICLE_DEF,blkParticle);

static const int NTOF = 20;
struct TOFCLUST_DEF {
   int   ntof;
   int   Tofstatus[NTOF];
   int   plane[NTOF];
   int   bar[NTOF];
   int   nmemb[NTOF];
   float Tofedep[NTOF];
   float Tofedepd[NTOF];
   float Toftime[NTOF];
   float Tofetime[NTOF];
   float Tofcoo[NTOF][3];
   float Tofercoo[NTOF][3];
};
#define blkTofclust COMMON_BLOCK(TOFCLUST,tofclust)
COMMON_BLOCK_DEF(TOFCLUST_DEF,blkTofclust);

static const int NTOFMC = 200;
struct TOFMCCLU_DEF {
   int   ntofmc;
   int   Tofmcidsoft[NTOFMC];
   float Tofmcxcoo[NTOFMC][3];
   float Tofmctof[NTOFMC];
   float Tofmcedep[NTOFMC];
};
#define blkTofmcclu COMMON_BLOCK(TOFMCCLU,tofmcclu)
COMMON_BLOCK_DEF(TOFMCCLU_DEF,blkTofmcclu);

static const int NTRCL = 200;
struct TRCLUSTE_DEF {
   int   Ntrcl;
   int   Idsoft[NTRCL];
   int   Statust[NTRCL];
   int   Neleml[NTRCL];
   int   Nelemr[NTRCL];
   float Sumt[NTRCL];
   float Sigmat[NTRCL];
   float Meant[NTRCL];
   float Rmst[NTRCL];
   float Errormeant[NTRCL];
   float Amplitude[NTRCL][5];
};
#define blkTrcluste COMMON_BLOCK(TRCLUSTE,trcluste)
COMMON_BLOCK_DEF(TRCLUSTE_DEF,blkTrcluste);

static const int NTRCLMC = 200;
struct TRMCCLUS_DEF {
   int   ntrclmc;
   int   Idsoftmc[NTRCLMC];
   int   Itra[NTRCLMC];
   int   Left[NTRCLMC][2];
   int   Center[NTRCLMC][2];
   int   Right[NTRCLMC][2];
   float ss[NTRCLMC][2][5];
   float xca[NTRCLMC][3];
   float xcb[NTRCLMC][3];
   float xgl[NTRCLMC][3];
   float summc[NTRCLMC];
};
#define blkTrmcclus COMMON_BLOCK(TRMCCLUS,trmcclus)
COMMON_BLOCK_DEF(TRMCCLUS_DEF,blkTrmcclus);

static const int NTRRH = 500;
struct TRRECHIT_DEF {
   int   ntrrh;
   int   px[NTRRH];
   int   py[NTRRH];
   int   statusr[NTRRH];
   int   Layer[NTRRH];
   float hitr[NTRRH][3];
   float ehitr[NTRRH][3];
   float sumr[NTRRH];
   float difosum[NTRRH];
   float cofgx[NTRRH];
   float cofgy[NTRRH];
};
#define blkTrrechit COMMON_BLOCK(TRRECHIT,trrechit)
COMMON_BLOCK_DEF(TRRECHIT_DEF,blkTrrechit);

static const int NTRTR = 200;
struct TRTRACK_DEF {
   int   ntrtr;
   int   trstatus[NTRTR];
   int   pattern[NTRTR];
   int   address[NTRTR];
   int   nhits[NTRTR];
   int   phits[NTRTR][6];
   int   Fastfitdone[NTRTR];
   int   Geanefitdone[NTRTR];
   int   Advancedfitdone[NTRTR];
   float Chi2strline[NTRTR];
   float Chi2circle[NTRTR];
   float Circleridgidity[NTRTR];
   float Chi2fastfit[NTRTR];
   float Ridgidity[NTRTR];
   float Errridgidity[NTRTR];
   float Theta[NTRTR];
   float phi[NTRTR];
   float p0[NTRTR][3];
   float gchi2[NTRTR];
   float gridgidity[NTRTR];
   float gerrridgidity[NTRTR];
   float gtheta[NTRTR];
   float gphi[NTRTR];
   float gp0[NTRTR][3];
   float hchi2[NTRTR][2];
   float Hridgidity[NTRTR][2];
   float Herrridgidity[NTRTR][2];
   float htheta[NTRTR][2];
   float hphi[NTRTR][2];
   float hp0[NTRTR][2][3];
   float fchi2ms[NTRTR];
   float gchi2ms[NTRTR];
   float ridgidityms[NTRTR];
   float gridgidityms[NTRTR];
};
#define blkTrtrack COMMON_BLOCK(TRTRACK,trtrack)
COMMON_BLOCK_DEF(TRTRACK_DEF,blkTrtrack);

static const int NMCG = 20;
struct MCEVENTG_DEF {
   int   nmcg;
   int   nskip[NMCG];
   int   Particle[NMCG];
   float coo[NMCG][3];
   float dir[NMCG][3];
   float momentum[NMCG];
   float mass[NMCG];
   float charge[NMCG];
};
#define blkMceventg COMMON_BLOCK(MCEVENTG,mceventg)
COMMON_BLOCK_DEF(MCEVENTG_DEF,blkMceventg);

static const int NCTCCL = 20;
struct CTCCLUST_DEF {
   int   nctccl;
   int   Ctcstatus[NCTCCL];
   int   Ctclayer[NCTCCL];
   float ctccoo[NCTCCL][3];
   float ctcercoo[NCTCCL][3];
   float ctcrawsignal[NCTCCL];
   float ctcsignal[NCTCCL];
   float ctcesignal[NCTCCL];
};
#define blkCtcclust COMMON_BLOCK(CTCCLUST,ctcclust)
COMMON_BLOCK_DEF(CTCCLUST_DEF,blkCtcclust);

static const int NCTCCLMC = 200;
struct CTCMCCLU_DEF {
   int   nctcclmc;
   int   Ctcmcidsoft[NCTCCLMC];
   float Ctcmcxcoo[NCTCCLMC][3];
   float Ctcmcxdir[NCTCCLMC][3];
   float Ctcstep[NCTCCLMC];
   float ctccharge[NCTCCLMC];
   float ctcbeta[NCTCCLMC];
   float ctcedep[NCTCCLMC];
};
#define blkCtcmcclu COMMON_BLOCK(CTCMCCLU,ctcmcclu)
COMMON_BLOCK_DEF(CTCMCCLU_DEF,blkCtcmcclu);

static const int NANTI = 16;
struct ANTICLUS_DEF {
   int   nanti;
   int   Antistatus[NANTI];
   int   Antisector[NANTI];
   float Antiedep[NANTI];
   float Anticoo[NANTI][3];
   float Antiercoo[NANTI][3];
};
#define blkAnticlus COMMON_BLOCK(ANTICLUS,anticlus)
COMMON_BLOCK_DEF(ANTICLUS_DEF,blkAnticlus);

static const int NANTIMC = 200;
struct ANTIMCCL_DEF {
   int   nantimc;
   int   Antimcidsoft[NANTIMC];
   float Antimcxcoo[NANTIMC][3];
   float Antimctof[NANTIMC];
   float Antimcedep[NANTIMC];
};
#define blkAntimccl COMMON_BLOCK(ANTIMCCL,antimccl)
COMMON_BLOCK_DEF(ANTIMCCL_DEF,blkAntimccl);

static const int NLVL3 = 2;
struct LVL3_DEF {
   int   nlvl3;
   int   Lvl3toftr[NLVL3];
   int   Lvl3antitr[NLVL3];
   int   Lvl3trackertr[NLVL3];
   int   Lvl3ntrhits[NLVL3];
   int   Lvl3npat[NLVL3];
   int   Lvl3pattern[NLVL3][2];
   float Lvl3residual[NLVL3][2];
   float Lvl3time[NLVL3];
   float Lvl3eloss[NLVL3];
};
#define blkLvl3 COMMON_BLOCK(LVL3,lvl3)
COMMON_BLOCK_DEF(LVL3_DEF,blkLvl3);

static const int NLVL1 = 2;
struct LVL1_DEF {
   int   nlvl1;
   int   Lvl1lifetime[NLVL1];
   int   Lvl1flag[NLVL1];
   int   Lvl1tofpatt[NLVL1][4];
   int   Lvl1tofpatt1[NLVL1][4];
   int   Lvl1antipatt[NLVL1];
};
#define blkLvl1 COMMON_BLOCK(LVL1,lvl1)
COMMON_BLOCK_DEF(LVL1_DEF,blkLvl1);

static const int NCTCHT = 50;
struct CTCHIT_DEF {
   int   nctcht;
   int   Ctchitstatus[NCTCHT];
   int   Ctchitlayer[NCTCHT];
   int   ctchitcolumn[NCTCHT];
   int   ctchitrow[NCTCHT];
   float ctchitsignal[NCTCHT];
};
#define blkCtchit COMMON_BLOCK(CTCHIT,ctchit)
COMMON_BLOCK_DEF(CTCHIT_DEF,blkCtchit);

static const int NTRRAW = 300;
struct TRRAWCL_DEF {
   int   ntrraw;
   int   rawaddress[NTRRAW];
   int   rawlength[NTRRAW];
   float s2n[NTRRAW];
};
#define blkTrrawcl COMMON_BLOCK(TRRAWCL,trrawcl)
COMMON_BLOCK_DEF(TRRAWCL_DEF,blkTrrawcl);

static const int NANTIRAW = 32;
struct ANTIRAWC_DEF {
   int   nantiraw;
   int   antirawstatus[NANTIRAW];
   int   antirawsector[NANTIRAW];
   int   antirawupdown[NANTIRAW];
   float antirawsignal[NANTIRAW];
};
#define blkAntirawc COMMON_BLOCK(ANTIRAWC,antirawc)
COMMON_BLOCK_DEF(ANTIRAWC_DEF,blkAntirawc);

static const int NTOFRAW = 20;
struct TOFRAWCL_DEF {
   int   ntofraw;
   int   tofrstatus[NTOFRAW];
   int   tofrplane[NTOFRAW];
   int   tofrbar[NTOFRAW];
   float tofrtovta[NTOFRAW][2];
   float tofrtovtd[NTOFRAW][2];
   float tofrsdtm[NTOFRAW][2];
   float tofreda[NTOFRAW];
   float tofredd[NTOFRAW];
   float tofrtm[NTOFRAW];
   float tofrcoo[NTOFRAW];
};
#define blkTofrawcl COMMON_BLOCK(TOFRAWCL,tofrawcl)
COMMON_BLOCK_DEF(TOFRAWCL_DEF,blkTofrawcl);

//-----------------------------------------------
// end definition of common blocks in AMS Ntuple
// =====               ------------     =========
//************************************************


PROTOCCALLSFSUB3(OPENIT,openit,INT,STRING,STRING)
#define OPEN(lun,file,stat) \
        CCALLSFSUB3(OPENIT,openit,INT,STRING,STRING,lun,file,stat)

PROTOCCALLSFSUB1(CLOSEIT,closeit,INT)
#define CLOSE(lun) CCALLSFSUB1(CLOSEIT,closeit,INT,lun)

PROTOCCALLSFFUN1(INT,EXIST,exist,INT)
#define EXIST(id) CCALLSFFUN1(EXIST,exist,INT,id)

#endif
