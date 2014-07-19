//  $Id: beta.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// V. Choutko 5-june-96
//
// July 10, 1996.  ak  add _ContPos and functions get/setNumbers;
// Oct  04, 1996.  ak  _ContPos is moved to AMSLink
// Oct  10, 1996.  ak. friend class implementation
//
#ifndef __AMSBETA__
#define __AMSBETA__
#include "link.h"
#include "tofrec02.h"
#include "trrec.h"
#include "point.h"
#include "commons.h"
class AMSBeta: public AMSlink{
protected:
 AMSTOFCluster * _pcluster[4];
 AMSTrTrack * _ptrack;
 integer _Pattern;
 number _Beta ;
 number _InvErrBeta;
 number _BetaC ;
 number _InvErrBetaC;
 number _Chi2;
 number _Chi2Space;
 void _printEl(ostream & stream);
 void _copyEl();
 void _writeEl();
 number betacorr(number zint,number z0, number part);
 static integer patpoints[npatb];
 static integer patconf[npatb][4];

public:
 ~AMSBeta();
 AMSBeta(): AMSlink(),_ptrack(0){for(int i=0;i<4;i++)_pcluster[i]=0;}
 AMSBeta(integer pattern, AMSTOFCluster * pcluster[4], AMSTrTrack * ptrack, number chi2s): 
         AMSlink(),_Pattern(pattern),_Chi2Space(chi2s),_ptrack(ptrack)
         {for(int i=0;i<4;i++)_pcluster[i]=pcluster[i];}
 AMSBeta(const AMSBeta & o): 
         AMSlink(o._status,o._next),_Pattern(o._Pattern),_Beta(o._Beta),
         _InvErrBeta(o._InvErrBeta),_Chi2(o._Chi2),_Chi2Space(o._Chi2Space),_ptrack(o._ptrack)
         {for(int i=0;i<4;i++)_pcluster[i]=o._pcluster[i];}
AMSBeta *  next(){return (AMSBeta*)_next;}
number getchi2()const{return _Chi2;}
number getchi2S()const{return _Chi2Space;}
number getbeta()const{return (checkstatus(AMSDBc::BAD)|| BETAFITFFKEY.MassFromBetaRaw)?_Beta:_BetaC;}
number getebeta()const{return (checkstatus(AMSDBc::BAD)|| BETAFITFFKEY.MassFromBetaRaw)?_InvErrBeta:_InvErrBetaC;}
integer getpattern()const{return _Pattern;}
AMSTrTrack * getptrack()const {return _ptrack;}
AMSTOFCluster * getpcluster(integer i){return i>=0 && i<4? _pcluster[i]:0;}
 void SimpleFit(integer nhit, number sleng[]);
static integer _addnext(integer pat, integer nhit, number sleng[],
AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number theta, number chi2s);
static integer _addnext(integer pat, integer nhit, number sleng[],
AMSTOFCluster *ptr[], AMSTrTrack * ptrack, number chi2s);
static AMSPoint Distance(AMSPoint coo, AMSPoint ecoo, AMSTrTrack *ptr,
                       number & sleng, number & theta);
static integer build(integer refit=0);
static void print();
#ifdef __WRITEROOT__
 friend class BetaR;
#endif
};

#endif
