// $Id: TrTrack.C,v 1.24 2008/10/23 10:12:39 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTrack.C
///\brief Source file of AMSTrTrack class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/20 SH  TkFit support
///\date  2007/12/28 SH  First stable vertion after a refinement
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/03/01 AO  Preliminary new data format
///\date  2008/03/12 SH  Getting back to SH version
///\date  2008/04/02 SH  Temporally optimization for non-B runs
///$Date: 2008/10/23 10:12:39 $
///
///$Revision: 1.24 $
///
//////////////////////////////////////////////////////////////////////////

#include "Event.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrFit.h"
#include "TrField.h"
#include "TkDBc.h"
#include "point.h"
#include "tkdcards.h"
//#include "commons.h"

#include <cmath>
#ifdef __DARWIN__
#include <limits.h>   // for FLT_MAX
#include <float.h>    // for FLT_MAX (for Dawin GCC 4)
#else
#include <values.h>   // for FLT_MAX (for Linux)
#endif
ClassImp(AMSTrTrack)

geant AMSTrTrack::_TimeLimit=0;

 void AMSTrTrack::BaseInit(){
//   memset(FitDone,1,NFITTYPE*sizeof(FitDone[0]));
   _GeaneFitDone=0;
   _AdvancedFitDone=0;
 }

// int AMSTrTrack::DoFit(char* fitname){
//   int index=GetFitIndex(fitname);
//   if(index<0) return -1;

//   TrFit fit();
//   int fitdone=fit->dofit;
//   if(!fitdone) return -2;

//   int index2=_chi2.size();
//   FitDone[index]=index2;
  
//   _chi2x.pushback(fit.GetChisqX());
//   _chi2y.pushback(fit.GetChisqY());
  
  

// }


// int AMSTrTrack::GetFitIndex(char* fitname){
//   int index=-1;
//   string str1(fitname);
//   for (int ii=0;ii<str1.size();ii++)
//     str1.at(ii)=tolower(str1.at(ii));

//   for (int ii=0;ii<NFITTYPE;ii++)
//     if(str1.compare(FitMethods[ii])){
//       index=ii;
//       break;
//     }
//   return index;
// }


AMSTrTrack::AMSTrTrack(AMSDir dir, AMSPoint point, number rig, number errig):
  AMSlink(0,0),_Pattern(-1),_Nhits(0),_GeaneFitDone(0),
  _AdvancedFitDone(1),_Rigidity(rig),_ErrRinv(errig),_Chi2FastFit(1000000)
{
  for(int i=0;i<trconst::maxlay;i++)
    _Hits[i]=0;
  
  _Theta=dir.gettheta();
  _Phi=dir.getphi();
  _P0x=point[0];
  _P0y=point[1];
  _P0z=point[2];
} 
  
AMSTrTrack::AMSTrTrack(number theta, number phi, AMSPoint point):
  AMSlink(0,0),_Pattern(-1),_Nhits(0),_GeaneFitDone(0),
  _AdvancedFitDone(1),_Rigidity(10000000),_ErrRinv(10000000),
  _Chi2FastFit(1000000)
{
  for(int i=0;i<trconst::maxlay;i++)
    _Hits[i]=0;
  
  _Theta=theta;
  _Phi=phi;
  _P0x=point[0];
  _P0y=point[1];
  _P0z=point[2];

}



AMSTrTrack::AMSTrTrack(void) : _Pattern(-1), _Nhits(-1) 
{
  BaseInit();
}

AMSTrTrack::AMSTrTrack(int pattern, int nhits, AMSTrRecHit *phit[], int *imult)
  : _Pattern(pattern), _Nhits(nhits)
{
  BaseInit();
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit  && i < _Nhits) ? phit [i] :  0;
    _iMult[i] = (imult && i < _Nhits) ? imult[i] : -1;
  }
}

AMSTrTrack::~AMSTrTrack()
{
}

AMSTrRecHit *AMSTrTrack::GetHit(int i) const
{
  return (0 <= i && i < trconst::maxlay) ?  _Hits[i] : 0;
}

double AMSTrTrack::GetChisq(void) const
{
  return (_NdofX+_NdofY > 0) ? (_ChisqX+_ChisqY)/(_NdofX+_NdofY) : 0;
}

double AMSTrTrack::GetChisqWoMS(void) const
{
  return (_NdofX+_NdofY > 0) ? (_ChisqXWoMS+_ChisqYWoMS)/(_NdofX+_NdofY) : 0;
}


double AMSTrTrack::SimpleFit(void)
{
  double err[3] = { 0.03, 0.03, 0.03 };
  return SimpleFit(err);
}

double AMSTrTrack::SimpleFit(double *ehit)
{
  TrFit fit;
  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit) continue;
    AMSPoint coo = hit->GetCoord(_iMult[i]);
    fit.Add((ehit[0] > 0) ? coo[0] : 0, 
	    (ehit[1] > 0) ? coo[1] : 0, coo[2], ehit[0], ehit[1], ehit[2]);
  }

  int method = TrFit::SIMPLE;
  if (MAGFIELDFFKEY.magstat == 0) method = TrFit::LINEAR;

  double chisq = -1;

  // X-only fit
  if (method == TrFit::LINEAR && ehit[1] <= 0) {
    fit.LinearFit(1);
    chisq = fit.GetChisqX();
  }
  // Y-only fit
  else if (method == TrFit::LINEAR && ehit[0] <= 0) {
    fit.LinearFit(2);
    chisq = fit.GetChisqY();
  }
  else chisq = fit.Fit(method);

  if (chisq < 0)
    _ChisqXWoMS = _ChisqYWoMS = FLT_MAX;
  else {
    _ChisqXWoMS = fit.GetChisqX();
    _ChisqYWoMS = fit.GetChisqY();

    _NdofX = fit.GetNdofX();
    _NdofY = fit.GetNdofY();

    _RigidityWoMS = 2.997E-4/fit.GetParam(4);
  }

  return GetChisqWoMS();
}


double AMSTrTrack::Fit(int fits, int layer, TrFit *trfit)
{
  // fit =0  fit pattern
  // fit =1  fit 1st part if pat=0,1,2,3, ... etc  
  // fit =2  fit 2nd half if pat=0,1,2,3  ... etc + interpolate to beg of 1st
  // fit =3  Geanefit pattern
  // fit =4  fast fit with ims=0
  // fit =5  Juans fit with ims=1
  //
  // For now, only fit =0,4 is supported, ipart is ignored
  //
  // The specified layer is excluded in the fitting
  // If trfit is not null, fitting parameters are left in trfit
  // and data members are not updated

  if (fits != 0 && fits != 4) return FLT_MAX;

  int fit = abs(fits);
  TrFit::_ims = (fit == 4) ? 0 : 1;

  double zh0 = 0;

  int update = 0;
  if (!trfit) {
    trfit = new TrFit;
    update = 1;
  }

  trfit->Clear();
  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;
    AMSPoint coo = hit->GetCoord(_iMult[i]);
    trfit->Add(coo[0], coo[1], coo[2], TRFITFFKEY.ErrX, TRFITFFKEY.ErrY, TRFITFFKEY.ErrZ);
    if (i == 0) zh0 = coo[2];
  }

  int method = TrFit::NORMAL;
  if (MAGFIELDFFKEY.magstat == 0) method = TrFit::LINEAR;
    
  int ifail = 0;
  if (trfit->Fit(method) < 0) ifail = 1;

  if (!update) return (ifail) ? FLT_MAX 
		              : trfit->GetChisqX()+trfit->GetChisqY();

  // Set fitting residual
  for (int i = 0; !ifail && i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit) continue;
    hit->SetResidual(AMSPoint(trfit->GetXr(i), trfit->GetYr(i), 0));
  }

  _ChisqX = (ifail) ? FLT_MAX : trfit->GetChisqX();
  _ChisqY = (ifail) ? FLT_MAX : trfit->GetChisq()-_ChisqX;//trfit->GetChisqY();
  _NdofX  = trfit->GetNdofX();
  _NdofY  = trfit->GetNdofY();

  _Rigidity = (trfit->GetParam(4) != 0) ? 1./trfit->GetParam(4) : 1.e10;
  _ErrRinv  = trfit->GetErig();
  _Theta = std::acos(-1/std::sqrt(1+trfit->GetParam(1)*trfit->GetParam(1)
				   +trfit->GetParam(3)*trfit->GetParam(3)));
  _Phi = std::atan2(-trfit->GetParam(3),-trfit->GetParam(1));
  _P0x = trfit->GetParam(0);
  _P0y = trfit->GetParam(2);
  _P0z = zh0;

  delete trfit;

  return (ifail) ? FLT_MAX : _ChisqX+_ChisqY;//GetChisq();
}

void AMSTrTrack::print(void)
{
 //  std::cout << "AMSContainer:AMSTrTrack 0 Elements: " 
// 	    << _event->NTrTracks << std::endl;
//   for (int i = 0; i < _event->NTrTracks; i++)
//     _event->GetTrTrack(i)->_printEl();
}

void AMSTrTrack::_printEl(std::ostream& ost )
{
  ost << " Pattern " << _Pattern 
	    << " Rigidity (no MS)" << _RigidityWoMS
	    << " Rigidity (Fast) " << _Rigidity 
	    << " Chi2Fast " << _ChisqX+_ChisqY
	    << " ThetaFast " <<_Theta << " PhiFast " << _Phi << std::endl;
}

void AMSTrTrack::_printEl(std::string& ost )
{
  char cc[280];
  sprintf(cc, " Pattern %02X\n  Rigidity (no MS) %f\n Rigidity (Fast) %f\n Chi2Fast %f\n ThetaFast %f\n PhiFast %f\n",
	 _Pattern,_RigidityWoMS,_Rigidity ,_ChisqX+_ChisqY,_Theta,_Phi);
  ost.append(cc);
}

#include "amsgobj.h"

AMSTrTrackError::AMSTrTrackError(char * name){
  if(name){
    integer n=strlen(name)+1;
    if(n>255)n=255;
    strncpy(msg,name,n);
  }
  AMSgObj::BookTimer.stop("TrTrack",1);
  AMSgObj::BookTimer.stop("RETKEVENT",1);
  AMSgObj::BookTimer.stop("REAMSEVENT",1);
  //  AMSEvent::gethead()->seterror(2);
}
char * AMSTrTrackError::getmessage(){return msg;}

