//  $Id: gamma.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author G.LAMANNA 13-Sept-2002

#ifndef __AMSTRGAMMA__
#define __AMSTRGAMMA__


#include "typedefs.h"
#include <iostream.h>
#include <math.h>
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "trid.h"
#include "link.h"
#include "commons.h"
#include "cont.h"
#include "event.h"
#include <iomanip>
#include <vector>





class AMSTrTrackGamma: public AMSlink{
protected:



AMSTrTrack * _pntTrL;
AMSTrTrack * _pntTrR;

AMSTrRecHit * _PRight[trconst::maxlay];
AMSTrRecHit * _PLeft[trconst::maxlay];
//
//integer _GammaStatus;
integer _NhRight; // 
integer _NhLeft;
//
integer _FastFitDoneR;
integer _GeaneFitDoneR;
integer _FastFitDoneL;
integer _GeaneFitDoneL;
//
AMSPoint _HRIGH[trconst::maxlay];
AMSPoint _EHRIGH[trconst::maxlay];
AMSPoint _HLEFT[trconst::maxlay];
AMSPoint _EHLEFT[trconst::maxlay];
////////
//!
number _Chi2FastFitLR;
//!
number _Chi2FastFitR;
number _RidgidityR;
number _ErrRidgidityR;
number _ThetaR;
number _PhiR;
AMSPoint _P0R;
//...
number _Chi2FastFitL;
number _RidgidityL;
number _ErrRidgidityL;
number _ThetaL;
number _PhiL;
AMSPoint _P0L;
////
number _GChi2R;
number _GRidgidityR;    //  Ridgidity or momentum if (geanefitdone != 14)
number _GErrRidgidityR; //  err of  see above
number _GThetaR;
number _GPhiR;
AMSPoint _GP0R;
//
number _GChi2L;
number _GRidgidityL;    //  Ridgidity or momentum if (geanefitdone != 14)
number _GErrRidgidityL; //  err of  see above
number _GThetaL;
number _GPhiL;
AMSPoint _GP0L;
///////
number _Chi2MSR;
number _GChi2MSR;//JUAN
number _RidgidityMSR;
number _GRidgidityMSR;//JUAN
number _GThetaMSR;
number _GPhiMSR;
AMSPoint _GP0MSR;
//...
number _Chi2MSL;
number _GChi2MSL;//JUAN
number _RidgidityMSL;
number _GRidgidityMSL;//JUAN
number _GThetaMSL;
number _GPhiMSL;
AMSPoint _GP0MSL;
/////
number _PGAMM;
number _ErrPGAMM;   //error  to pgam
number _MGAM;
AMSPoint _VE1;
AMSPoint _VE2;
number _PhTheta;
number _PhPhi;
number _Gacosd;

//added by VC

AMSPoint _Vertex;
number _TrackDistance;
integer _Charge;

/*
// to fulfill the AMSTrTrack members
uinteger _AddressLR;
integer _PatternLR;
integer _AdvancedFitDoneLR;
number _DbaseLR[2];
number _Chi2StrLineLR;
number _Chi2CircleLR;
number _CircleRidgidityLR;
number _HChi2LR[2];
number _HRidgidityLR[2];
number _HErrRidgidityLR[2];
number _HThetaLR[2];
number _HPhiLR[2];
AMSPoint _HP0LR[2];
*/


void _printEl(ostream & stream){ stream << " Pattern ************************" <<endl;}
void _copyEl();
void _writeEl();
void _crHitLR();
inline  AMSPoint  getHleft(int i, int dir=0){return _HLEFT[dir==0?i:_NhLeft-1-i];}
inline  AMSPoint  getEHleft(int i){return _EHLEFT[i];}
inline  AMSPoint  getHright(int i, int dir=0){return _HRIGH[dir==0?i:_NhRight-1-i];}
inline  AMSPoint  getEHright(int i){return _EHRIGH[i];}
//_____-
static void _LeftRight(vector<double>, integer , number);
//-----
public:
static void _CheckZOrder(double re[], double ze[], double ce[], integer &);
static void _CheckInvReswrtZ(double re[], double ze[], double ce[], integer &);
static void _Combi(integer&, integer, integer);
static void _Averes(number&, number&, vector<double>, integer, integer);
static void _Cente(number&, number&, vector<double>, integer, integer, number, number);
static vector<double> _TK1YRemoveTRD2TOF(vector<double>, integer);
static void _LSQP2(integer FLPAT[], vector<double> H[], integer, integer);
static void SetBestResLR(integer FLPAT[], vector<double> H[], integer, integer, integer);
static void _LookOneEight(integer FLPAT[], vector<double> H[]);
static void _SingleCommonHit(integer FLPAT[], vector<double> H[]);
static void _SingleHit(integer FLPAT[], double CE[], integer );
inline static void _intercept(double X1,double Y1,double Z1,double X2,double Y2,double Z2,double& MX,double& QX,double& MY,double& QY){
  // slope in XZ : MX
    MX = (X2-X1)/(Z2-Z1);
  // intercept in XZ : QX 
    QX = X1- (Z1*(X2-X1))/(Z2-Z1); 
  // slope in YZ : MY
   MY = (Y2-Y1)/(Z2-Z1);
  // intercept in YZ : QY 
    QY = Y1- (Z1*(Y2-Y1))/(Z2-Z1);}
inline static void _DISTANCE(double X,double Y,double Z,double MX,double QX,double MY,double QY,double& Str,double& Circ){        
   Str = QX + MX*Z - X/sqrt(1+pow(MX,2));
   Circ= QY + MY*Z - Y/sqrt(1+pow(MY,2));} 

static integer build(integer refit=0);
AMSTrTrackGamma (integer nhitL, integer nhitR, AMSTrRecHit* phL[], AMSTrRecHit* phR[], integer stat);
//proviamo
AMSTrTrackGamma (number Chi2, number Rigi, number Erigi, number Thetaff, number Phiff, AMSPoint X0);
number Fit(integer i=0, integer ipart=3);
void PAIR2GAMMA(int&, int&);
void addtracks(int&);
void _MyVertex(double n_L[],double n_R[]);
void LR_RES_STUDY3(integer [], double [], int & );
static void dlsqp2me(integer M, double v[], double w[], double & a0, double & a1, double & a2, double & chi);
static void dlinearme(integer M, double v[], double w[], double & a0, double & a1, double & chi);
double _vdmax(double v[], int size);
double _vdmin(double v[], int size);
void interpolate(int which, AMSPoint  pnt, AMSDir  dir,  AMSPoint & P1, 
                 number & theta, number & phi, number & length);
//
static void RecoXLeft(double X2P, double X3P, double DeltaRecoTop, double DeltaRecoBottom, 
		      int ifi, int ila, double RegStr0, int & lkfil, double & VAMAXL);
static void RecoXRight(double X2P, double X3P, double DeltaRecoTop, double DeltaRecoBottom, 
		      int ifi, int ila, double RegStr0, int & lkfir,double & VAMAXR);

static void RecoLeftRight(int & refitting, integer FLPAT[], double SLOPEf, double INTERf,double x_starf,
                   double z_starf,int fbotf,int ftopf,
                   double firR,double lasR,double firL,double lasL,
                   int fir_planeR, int fir_planeL,
                   int las_planeR,int las_planeL,double & slr, double & qlr);


 static void ResetXhits(int & resetting,double,double);
//
void HITRESEARCH(int pla, double RES_REF, AMSPoint P_0L2, AMSPoint P_0R2);
static integer Out(integer);
integer getcharge() const{return _Charge;}
AMSTrTrack * getpleft() const {return _pntTrL;}
AMSTrTrack * getpright() const {return _pntTrR;}

number getmass() const {return _MGAM;}
number getmom() const{return _PGAMM;}
number gettheta() const{return _PhTheta;}
number getphi() const{ return _PhPhi;}
number geterrmom() const{ return _ErrPGAMM;} 
AMSPoint getvert()const {return _Vertex;}
~AMSTrTrackGamma(){};
AMSTrTrackGamma *  next(){return (AMSTrTrackGamma*)_next;}
//
void getFFParam(number&  Chi2, number& Rigi, number&  Erigi, 
number&  Thetaff, number & Phiff, AMSPoint&  X0)const ; 

void _ConstructGamma(int method);


#ifdef __WRITEROOT__
  friend class VertexR;
#endif

};




class XZLine_TOF{
protected:
  int fbot,ftop;
  double x_star,z_star;
  double SLOPE,INTER;
  int* LL;
  int* NN;
public:
 XZLine_TOF(int* mul,int* nm, double&, number&);
 inline void intercept(double X1,double Y1,double Z1,double X2,double Y2,double Z2,double& MX,double& QX,double& MY,double& QY){
  // slope in XZ : MX
    MX = (X2-X1)/(Z2-Z1);
  // intercept in XZ : QX 
    QX = X1- (Z1*(X2-X1))/(Z2-Z1); 
  // slope in YZ : MY
   MY = (Y2-Y1)/(Z2-Z1);
  // intercept in YZ : QY 
    QY = Y1- (Z1*(Y2-Y1))/(Z2-Z1);}
 void getTofMul(int, int mul[], double&);
 void getEcalMul(int, int nn[], number&);
 void TopSplash(double&);
 void Check_TRD_TK1(int,vector<double>,int jj[]);
 void makeEC_out(number&, int&);
 void Lines_Top_Bottom(int&);
 void LastCheckTRDX(double& , double& , double& );
 void getParRoadXZ(int& bott, int& topp, double& x_ss, double& z_ss, double& SLL, double& INTT)const;
};

class MYlfit{
 protected:
  double a,b,delta,VAR;
  double* x;
  double* y;
  int L;
  double sigma(double* r);
  double sigma(double* r, double* s);
 public:
  MYlfit(int M, double* xp, double* yp);
  void make_a();
  void make_b();
  void make_VAR();
  void display();
  double _getA();
  double _getB(); 
  double _getVAR(); 
};


class MYlsqp2{
 protected:
  double a,b,c,sdw;
  double n1[3],n2[3],n3[3],n4[3];
  double al,be,ga,rho,eps,bi;
  int L;
  double* x;
  double* y;
  double sigma(double* r);
  double sigma(double* r, double* s);
  double sigma(double* r, double* s, double* t);
  double sigma(double* r, double* s, double* t, double* u);
 public:
  MYlsqp2( int M, double* xp, double* yp);
  void make_param();
  void make_a();
  void make_b();
  void make_c();
  void make_SDW();
  double _getA();
  double _getB();
  double _getC();
  double _getSDW(); 
};

#endif
