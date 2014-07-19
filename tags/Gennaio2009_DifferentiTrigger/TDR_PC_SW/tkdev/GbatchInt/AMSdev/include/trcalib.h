//  $Id: trcalib.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 4-mar-1997

#ifndef __AMSTRCALIB__
#define __AMSTRCALIB__
#include "typedefs.h"
#include  "commons.h"
#include "link.h"
#include "point.h"
#include "particle.h"
#include "trid.h"
class AMSmceventg;
using namespace amsprotected;
const integer nrho=64;
class PSStr_def{
  public:
    integer Layer;
    integer Ladder;
    integer Half;
    integer Side;
    integer Strip;
    geant Ped;
    geant Sigma;
    geant BadCh;
    geant SigmaRaw;
    geant Rho[nrho];
};
class TrAlig_def{
  public:
    integer Pattern;
    integer Alg;
    geant FCN;
    geant FCNI;
    geant Pfit;
    geant Pfitsig;
    geant Coo[6][3];
    geant Angle[6][3];
};
class TrCalib_def{
  public:
    integer Layer;
    integer Ladder;
    integer Half;
    integer Side;
    integer Strip;
    geant Ped;
    geant Sigma;
    geant BadCh;
    geant CmnNoise;
};
class AMSTrCalibPar{
  protected:
    AMSPoint _Coo;
    AMSPoint _Angles;
    AMSDir _Dir[3];
    void _a2m();
  public:
    AMSTrCalibPar(){};
    AMSTrCalibPar(const AMSPoint & coo, const AMSPoint & angles);
    AMSPoint  getcoo()const {return _Coo;}
    AMSPoint  getang()const {return _Angles;}
    void setcoo(const AMSPoint & o) {_Coo=o;}
    void setpar(const AMSPoint & coo, const AMSPoint & angle);
    AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
    AMSDir &  setmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
    void updmtx(){_a2m();}
    AMSTrCalibPar  operator +(const AMSTrCalibPar &o){
      return AMSTrCalibPar(_Coo+o._Coo,_Angles+o._Angles);
    }         
    AMSTrCalibPar  operator -(const AMSTrCalibPar &o){
      return AMSTrCalibPar(_Coo-o._Coo,_Angles-o._Angles);
    }         
    AMSTrCalibPar  operator *(const AMSTrCalibPar &o){
      return AMSTrCalibPar(_Coo*o._Coo,_Angles*o._Angles);
    }         
    AMSTrCalibPar  operator /(number o){
      return AMSTrCalibPar(_Coo/o,_Angles/o);
    }
    void sqr();


    friend ostream &operator << (ostream &o, const  AMSTrCalibPar &b )
    {return o<<" "<<b._Coo<<" "<<b._Angles<<" "<<b._Dir[0]<<" "<<b._Dir[1]<<" "<<b._Dir[2];}

};

const integer tkcalpat=5;
class AMSTrCalibration: public AMSlink{
  protected:
    integer _Pattern;
    void _copyEl(){}
    void _printEl(ostream & stream){ stream << " Pattern " << _Pattern<<endl;}
    void _writeEl(){}
  public:
    AMSTrCalibration(integer pattern=0):_Pattern(pattern){}
};


class AMSTrCalibData{
  protected:
    integer _NHits;
    AMSPoint * _Hits;
    AMSPoint * _EHits;
    geant _InvBeta;
    geant _ErrInvBeta;
    geant _InvRigidity;
    geant _ErrInvRigidity;
    static integer patpoints[nalg][tkcalpat];
    static integer patconf[nalg][tkcalpat][6];
  public:
    AMSTrCalibData():_NHits(0),_Hits(0),_EHits(0),_InvBeta(0),_ErrInvBeta(0),
		     _InvRigidity(0),_ErrInvRigidity(0){};
    integer Init(AMSBeta * pbeta, AMSTrTrack *ptrack, AMSmceventg *pgen, 
	integer pattern, integer alg);
    integer Select(integer alg, integer pattern);
    integer PatternMatch(integer patcal,integer pattr);
    friend class AMSTrCalibFit;
    ~AMSTrCalibData(){ delete [] _Hits; delete[] _EHits;}
};

class AMSTrCalibFit{
  protected:
    integer _PlaneNo[36];
    integer _ParNo[36];
    integer _NoActivePar;
    integer  _Pattern; 
    integer _PositionData;
    integer _PositionIter;
    integer _NData;
    integer _NIter;
    AMSTrCalibData * _pData;
    integer _Algorithm;
    integer _flag;    // 
    number _tmp;
    number _tmppav;
    number _tmppsi;
    integer _NLad;    // Ladder No
    integer _Pid;   // presumed particleid 
    number* _fcn;   // pointer to fcns;
    number* _fcnI;   // pointer to fcns;
    number *_pfit;  //pointer to fitterd mom
    number *_pfits;  //pointer to fitterd mom sigma
    AMSTrCalibPar * _pParC[6];       // pointer to fitted current par
    static  AMSTrCalibFit  * _pCalFit[nalg][tkcalpat];
    static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
    {};
    static void alfun(integer & n, number xc[], number & fc, AMSTrCalibFit * ptr);
  public:
    AMSTrCalibFit();
    AMSTrCalibFit(integer pattern, integer data, integer iter, integer alg, integer ipart);
    integer Test(int i=0);
    void Fit();
    void Anal();
    static void SAnal();
    static AMSTrCalibFit * getHead(integer alg, integer pat)
    {return pat>=0 && pat<tkcalpat && alg>=0 && alg<nalg ? _pCalFit[alg][pat]:0;}
    static void setHead(integer alg, integer pat, AMSTrCalibFit * ptr);
    integer getlayer(integer c);
    ~AMSTrCalibFit();

};

class AMSTrIdCalib : public AMSTrIdSoft{
  protected:

    static integer * _Count;
    static geant * _BadCh;
    static number  * _ADC;
    static number *_ADCMax;
    static number * _ADC2;
    static number * _ADCRho[nrho];
    static number * _ADC2Raw;
    static number * _ADCRaw;
    static integer  _CmnNoiseC[10][trid::ms];
    static geant  _CmnNoise[10][trid::ms];
    static void _calc();
    static void _hist();
    static void _update();
    static void _clear();
    static time_t _BeginTime;
    static time_t _CurTime;
    static uinteger _CurRun;

  public:
    static void getaverage();
    static void addonemorecalib();
    static void offmonhist();
    static void printbadchanlist();
    static integer CalcBadCh(integer half, integer side);
    static uinteger getrun(){return _CurRun;}
    AMSTrIdCalib():AMSTrIdSoft(){};
    AMSTrIdCalib(const AMSTrIdSoft & o):AMSTrIdSoft(o){};
    static void initcalib();
    static void check(integer forcedw=0);
    static void ntuple(integer s);
    static void buildpreclusters( AMSTrIdSoft & idd, integer len, geant id[]);
    static void buildSigmaPed(integer n, int16u* p);
    static void buildSigmaPedA(integer n, int16u* p);
    static void buildSigmaPedB(integer n, int16u* p);
    inline integer getcount() const {return _Count[getchannel()];}
    geant getcmnnoise() const {return _CmnNoise[_VANumber][_addr];}
    void updcmnnoise(geant cmn){(_CmnNoise[_VANumber][_addr])+=cmn*cmn;}
    void updcmnnoiseC(){(_CmnNoiseC[_VANumber][_addr])++;}
    void updADC(geant ped){(_ADC[getchannel()])+=ped;}
    number getADC(){return _ADC[getchannel()];}
    number getADCRaw(){return _ADCRaw[getchannel()];}
    geant getBadCh(){return _BadCh[getchannel()];}
    void updBadCh(){_BadCh[getchannel()]++;}
    void updADC2(geant ped){(_ADC2[getchannel()])+=ped*ped;}
    void updADCRaw(geant ped){(_ADCRaw[getchannel()])+=ped;}
    void updADC2Raw(geant ped){(_ADC2Raw[getchannel()])+=ped*ped;}
    void updADCRho(geant ped1, geant ped2, integer k){(_ADCRho[k][getchannel()])+=ped1*ped2;}
    void updADCMax(geant ped){if(_ADCMax[getchannel()]<ped)
      _ADCMax[getchannel()]=ped;}
    void updcounter(){(_Count[getchannel()])++;}
};



#endif
