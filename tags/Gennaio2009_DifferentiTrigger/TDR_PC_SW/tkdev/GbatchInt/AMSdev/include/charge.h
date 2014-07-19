//  $Id: charge.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// V. Choutko 5-june-96
//
// July 12, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. implementation of friend class
//
// Last Edit Oct  10, 1996.  ak. 
//
#ifndef __AMSCHARGE__
#define __AMSCHARGE__
#include "link.h"
#include "tofdbc02.h"
#include "tkdbc.h"
#include "trrec.h"
#include "beta.h"
#include "richrec.h"
const int TrackerMaxHits=trconst::maxlay;
const int maxzin=AMSChargConst::MaxZTypes;
const number powmx=50.;
class AMSCharge: public AMSlink{
protected:
  AMSBeta * _pbeta;      // pointer to beta --> then to track & tof
  AMSRichRing * _pring;  // pointer to RICH ring

 integer _ChargeTOF;
 integer _ChargeTracker;
 integer _ChargeRich;
 number _TrMeanTracker;
 number _TrMeanTOF;
 number _TrMeanTOFD;
 number  _ProbTOF[maxzin];       //prob  to be e,p,He,...,F
 number  _LkhdTOF[maxzin];       //loglikhd value for e,p,...
 integer _IndxTOF[maxzin];       //index 0,...,9 from most to least prob charge
 integer _iTOF;                   //index of most probable charge
 number  _ProbTracker[maxzin];   //prob  to be e,p,He,...,F
 number  _LkhdTracker[maxzin];   //loglikhd value for e,p,...
 integer _IndxTracker[maxzin];   //index 0,...,9 from most to least prob charge
 integer _iTracker;               //index of most probable charge
 number  _ProbAllTracker;         //prob of maximum charge using all tracker clusters
 number  _ProbRich[maxzin];      //prob  to be e,p,He,...,F
 number  _LkhdRich[maxzin];      //loglikhd value for e,p,...
 integer _IndxRich[maxzin];      //index 0,...,9 from most to least prob charge
 integer _iRich;                  //index of most probable charge

 integer _sortlkhd(integer sort);
 number _probcalc(int mode, int fit, int nhittyp[],number lkhd[]);
 number _probrich(number expRich, number useRich, number lkhrich[]);
 void _copyEl();
 void _printEl(ostream & stream){ stream << " ChargeTOF "<<_ChargeTOF<<" ChargeTracker "<<_ChargeTracker<<" ProbTOF "<<_ProbTOF[0]<<" "<<_ProbTOF[1]<<" "<<_ProbTOF[2]<<" "<<_ProbTOF[3]<<" "<<_ProbTOF[4]<<" "<<_ProbTOF[5]<<" "<<_ProbTOF[6]<<" "<<" ProbTracker "<<_ProbTracker[0]<<" "<<_ProbTracker[1]<<" "<<_ProbTracker[2]<<" "<<_ProbTracker[3]<<" "<<_ProbTracker[4]<<" "<<_ProbTracker[5]<<" "<<_ProbTracker[6]<<" "<<endl;}
 void _writeEl();
   static integer _chargeTracker[maxzin];
   static integer _chargeTOF[maxzin];
   static integer _chargeRich[maxzin];
public:
  AMSCharge *  next(){return (AMSCharge*)_next;}
  AMSCharge(): AMSlink(),  _pbeta(0),_pring(0),_iTracker(0),_ProbAllTracker(0),
_ChargeTracker(0),_TrMeanTracker(0){}
  AMSCharge(AMSBeta * pbeta, AMSRichRing * pring, number trtr, number trtof): AMSlink(),
  _pbeta(pbeta),_pring(pring),_TrMeanTracker(trtr),_TrMeanTOF(trtof),_TrMeanTOFD(trtof),
_iTracker(0),_ProbAllTracker(0),_ChargeTracker(0){}
  integer getvotedcharge(int & index);
  inline number getrmeantrk(){return _TrMeanTracker/2;}
  inline number getrmeantof(){return _TrMeanTOF;}
  number getprobcharge(integer charge);
  integer getchargeTOF()const{return _ChargeTOF;}
  integer getchargeTracker()const{return _ChargeTracker;}
  integer getchargeRich()const{return _ChargeRich;}
  AMSBeta * getpbeta()const{return _pbeta;}
  AMSRichRing * getpring()const{return _pring;}
  static void addnext(AMSBeta* pbeta, AMSRichRing* pring, integer nhitTOF, integer nhitTracker, 
  AMSTOFCluster *pTOFc[], number EdepTOF[TOF2GC::SCLRS], AMSTrCluster  *pTrackerc[trconst::TrkTypes-1][TrackerMaxHits], number EdepTracker[trconst::TrkTypes-1][TrackerMaxHits], number trtr, number trtof, number expRich, number useRich);
  int FitTOF(int toffit, number beta, int bstatus, int nhitTOF, AMSTOFCluster *pTOFc[], number etof[TOF2GC::SCLRS]);
  int FitTracker(int trkfit, number beta, int bstatus, int nhitTracker, AMSTrCluster *pTrackerc[trconst::TrkTypes-1][TrackerMaxHits], number etrk[trconst::TrkTypes-1][TrackerMaxHits]);
  int FitRich(int richfit, number expRich, number useRich);
  static number resmax(number x[],int ntot,int refit,number rescut,int &imax,number &mean,number &trres,number &trmax);
  static void lkhcalc(int mode, number beta, int nhit, number ehit[], int typeh[], number lkh[]);
  static integer build(integer refit=0);
  static integer ind2charge(int idet, int ind);
  static void print();
friend class AMSJob;
#ifdef __WRITEROOT__
friend class ChargeR;
#endif
};


class AMSChargeI{
public:
AMSChargeI();
private:
static integer _Count;
};
static AMSChargeI cI;


#endif
