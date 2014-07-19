// $Id: TrTrack.C,v 1.35 2008/12/13 15:02:57 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrTrack.C
///\brief Source file of AMSTrTrack class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/20 SH  TkFit support
///\date  2007/12/28 SH  First stable vertion after a refinement
///\date  2008/01/21 SH  Imported to tkdev (test version)
///\date  2008/02/13 SH  Comments are updated
///\date  2008/03/01 AO  Preliminary new data format
///\date  2008/03/12 SH  Getting back to SH version
///\date  2008/07/01 PZ  New Tracker Lib Standalone but to be emebedded
///\date  2008/09/16 PZ  Rename as TrTrackR to be gbatch compatible
///\date  2008/10/23 PZ  Modifications for GBATCH compatibility
///\date  2008/11/05 PZ  New data format to be more compliant
///\date  2008/11/13 SH  Some updates for the new TrRecon
///\date  2008/11/20 SH  A new structure introduced
///$Date: 2008/12/13 15:02:57 $
///
///$Revision: 1.35 $
///
//////////////////////////////////////////////////////////////////////////

#include "Event.h"
#include "TrRecHit.h"
#include "TrTrack.h"
#include "TrField.h"
#include "TkDBc.h"
#include "point.h"
#include "tkdcards.h"

#include <cmath>

ClassImp(AMSTrTrack)

#ifdef __WRITEROOT_
#include "VCon_gbatch.h"
VCon* AMSTrTrack::vcon = new VCon_gb();
#else  
#include "VCon_root.h"
VCon* AMSTrTrack::vcon = new VCon_root();
#endif

geant AMSTrTrack::_TimeLimit = 0;

int AMSTrTrack::NhitHalf     = 4;
int AMSTrTrack::DefaultFitID = AMSTrTrack::kChoutko;

AMSTrTrack::AMSTrTrack()
{
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iMult[i] = -1;
  }
}

AMSTrTrack::AMSTrTrack(int pattern, int nhits, AMSTrRecHit *phit[], int *imult)
  : _Pattern(pattern), _Nhits(nhits), _NhitsX(0), _NhitsY(0), _NhitsXY(0)

{
  for (int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = (phit  && i < _Nhits) ? phit [i] :  0;
    _iMult[i] = (imult && i < _Nhits) ? imult[i] : -1;

    if (phit && i < _Nhits) {
      if (phit[i]->GetXCluster()) _NhitsX++;
      if (phit[i]->GetYCluster()) _NhitsY++;
      if (phit[i]->GetXCluster() && phit[i]->GetYCluster()) _NhitsXY++;
    }
  }

  // These patterns should be filled with SetPatterns()
  _PatternX = _PatternY = _PatternXY = 0;

  DBase[0] = DBase[1] = 0;

  BuildHitsIndex();
}

AMSTrTrack::AMSTrTrack(number theta, number phi, AMSPoint point)
  : AMSlink(0,0), _Pattern(-1), _Nhits(0)
{
  AMSTrTrackPar &par = _TrackPar[kDummy];
  par.FitDone = true;
  par.P0      = point;
  par.Dir     = AMSDir(std::tan(theta)*std::cos(phi), 
		       std::tan(theta)*std::sin(phi), -1);

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
    par.Residual[i] = 0;
  }

  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  DBase[0] = DBase[1] = 0;
}

AMSTrTrack::AMSTrTrack(AMSDir dir, AMSPoint point, number rig, number errig)
  : AMSlink(0,0), _Pattern(-1), _Nhits(0)
{
  AMSTrTrackPar &par = _TrackPar[kDummy];
  par.FitDone  = true;
  par.Rigidity = rig;
  par.ErrRinv  = errig;
  par.P0       = point;
  par.Dir      = dir;

  for(int i = 0; i < trconst::maxlay; i++) {
    _Hits [i] = 0;
    _iHits[i] = _iMult[i] = -1;
    par.Residual[i] = 0;
  }

  _PatternX = _PatternY = _PatternXY = _NhitsX = _NhitsY = _NhitsXY = 0;
  DBase[0] = DBase[1] = 0;
}

AMSTrTrack::~AMSTrTrack()
{
}

void AMSTrTrack::AddHit(AMSTrRecHit *hit, int imult)
{
  VCon* cont2=vcon->GetCont("AMSTrRecHit");
  if (_Nhits < trconst::maxlay) {
    _Hits [_Nhits] = hit;
    _iHits[_Nhits] = cont2->getindex(hit);
    _iMult[_Nhits] = (imult >= 0) ? imult : hit->GetResolvedMultiplicity();
    _Nhits++;
    if (hit->GetXCluster()) _NhitsX++;
    if (hit->GetYCluster()) _NhitsY++;
    if (hit->GetXCluster() && hit->GetYCluster()) _NhitsXY++;
  }
  delete cont2;
}

void AMSTrTrack::BuildHitsIndex()
{
  VCon *cont2 = vcon->GetCont("AMSTrRecHit");
  if (!cont2) return;

  for (int i = 0; i < _Nhits; i++)
    _iHits[i] = cont2->getindex(_Hits[i]);

  delete cont2;
}

AMSPoint AMSTrTrack::GetPentry(int id)
{
  double zent = 0;

  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (hit && TestHitBits(hit->GetLayer(), id)) {
      double zh = hit->GetCoord().z();
      if (zent == 0 || zh > zent) zent = zh;
    }
  }

  // TO_BE_FIXED, interpolation to z=zent
  double xent = 0, yent = 0;

  return AMSPoint(xent, yent, zent);
}

AMSTrRecHit *AMSTrTrack::GetHit(int i) 
{
  if (i < 0 || trconst::maxlay <= i) return 0;
  if (_Hits[i] == 0 && _iHits[i] >= 0) {
    VCon* cont2 = vcon->GetCont("AMSTrRecHit");
    _Hits[i] = (AMSTrRecHit*)cont2->getelem(_iHits[i]);
    delete cont2;
  }

  return _Hits[i];
}

float AMSTrTrack::Fit(int id, int layer, bool update, const float *err, 
		      float mass, float chrg)
{
  if (id < 0) return -1;

  int idf = id&0xf;
  if (idf == kGEANE || idf == kGEANE_Kalman || idf == kChikanian) {
    cerr << "Error in TrTrack::Fit Fitting method not implemented: "
	 << idf << endl;
    return -1;
  }
  if (idf == kDummy) {
    cerr << "Warning in TrTrack::Fit Dummy fit is ignored " << endl;
    return -1;
  }

  // Force to use Linear fit if magnet is off
  if (MAGFIELDFFKEY.magstat == 0) id = (id&0xfff0)+kLinear;
  idf = id&0xf;

  // Select fitting method
  int method = TrFit::CHOUTKO;
  if      (idf == kAlcaraz) method = TrFit::ALCARAZ;
  else if (idf == kLinear)  method = TrFit::LINEAR;
  else if (idf == kCircle)  method = TrFit::CIRCLE;
  else if (idf == kSimple)  method = TrFit::SIMPLE;

  // Set multiple scattering option and assumed mass
  if (id & kMultScat) {
    TrFit::_mscat = 1;
    _TrFit.SetMassChrg(mass, chrg);
  }

  // Sort hits in the ascending order of the layer number
  int idx[trconst::maxlay], nhit = 0;
  for (int i = 0; i < _Nhits; i++) {
    AMSTrRecHit *hit = GetHit(i);
    if (!hit || hit->GetLayer() == layer) continue;
    idx[nhit++] = hit->GetLayer()*10+i;
  }
  std::sort(idx, &idx[nhit-1]);

  // For the half-fitting options    : TO_BE_CHECKED
  int i1 = 0, i2 = nhit;
  if (id & kUpperHalf) { i1 = 0;    i2 = std::min(nhit, NhitHalf); }
  if (id & kLowerHalf) { i2 = nhit; i1 = std::max(0, nhit-NhitHalf); }

  // Set fitting errors
  double errx = (err) ? err[0] : TRFITFFKEY.ErrX;
  double erry = (err) ? err[1] : TRFITFFKEY.ErrY;
  double errz = (err) ? err[2] : TRFITFFKEY.ErrZ;
  double zh0  = 0;
  int hitbits = 0;

  // Fill hit points
  _TrFit.Clear();
  for (int i = i1; i < i2; i++) {
    int j = idx[i]%10;
    AMSTrRecHit *hit = GetHit(j);
    AMSPoint coo = (_iMult[j] >= 0) ? hit->GetCoord(_iMult[j])
                                    : hit->GetCoord();
    _TrFit.Add(coo, (hit->GetStatus() & YONLY) ? 0 : errx,
	            (hit->GetStatus() & XONLY) ? 0 : erry, errz);
    hitbits |= (1 << (trconst::maxlay-hit->GetLayer()));
    if (id != kLinear && j == 0) zh0 = coo.z();
  }

  // Perform fitting
  if (_TrFit.Fit(method) < 0) return -1;

  // Return if fitting values are not to be over written
  if (!update) return _TrFit.GetChisq();

  // Fill fittng parameters
  AMSTrTrackPar &par = _TrackPar[id];
  par.FitDone  = true;
  par.HitBits  = hitbits;
  par.ChisqX   = _TrFit.GetChisqX();
  par.ChisqY   = _TrFit.GetChisqY();
  par.NdofX    = _TrFit.GetNdofX();
  par.NdofY    = _TrFit.GetNdofY();
  par.Chisq    = _TrFit.GetChisq();
  par.Rigidity = _TrFit.GetRigidity();
  par.ErrRinv  = _TrFit.GetErrRinv();

  for (int i = 0, j = 0; i < trconst::maxlay; i++) {
    if (hitbits & (1 << (trconst::maxlay-(i+1)))) {
      par.Residual[i].setp(_TrFit.GetXr(j), _TrFit.GetYr(j), _TrFit.GetZr(j));
      j++;
    }
    else par.Residual[i].setp(0, 0, 0);
  }

  // Interpolate to Z=0
  AMSPoint pnt(0, 0, 0);
  AMSDir   dir(0, 0, 1);
  _TrFit.Interpolate(pnt, dir);
  par.P0  = pnt;
  par.Dir = dir;

  return GetChisq(id);
}

void AMSTrTrack::print(void)
{
 //  std::cout << "AMSContainer:AMSTrTrack 0 Elements: " 
// 	    << _event->NTrTracks << std::endl;
//   for (int i = 0; i < _event->NTrTracks; i++)
//     _event->GetTrTrack(i)->_printEl();
}

void AMSTrTrack::myprint(void)
{
  cout << "Npars= " << _TrackPar.size() << endl;
  for (map<int, AMSTrTrackPar>::iterator it = _TrackPar.begin();
       it != _TrackPar.end(); it++)
    cout << it->first << " " << it->second.FitDone << endl;
/*
  printf("Tracks  \n");
  for (int ii=0;ii<GetNhits();ii++)
    printf(" %d pointer %X  index: %d\n ",ii,(unsigned int)_Hits[ii],_iHits[ii]);
*/
}

void AMSTrTrack::_printEl(std::ostream& ost )
{
  /// FastFit is assumed as Choutko fit
  int id1 = kChoutko;
  int id2 = kChoutko+kMultScat;
  if (!ParExists(id1)) id1 = kLinear;
  if (!ParExists(id2)) id2 = id1;
  if (!ParExists(id1)) return;
  ost << " Pattern " << _Pattern
      << " Rigidity (no MS)" << GetRigidity(id1)
      << " Rigidity (Fast) " << GetRigidity(id2)
      << " Chi2Fast "  << GetChisq(id1)
      << " ThetaFast " << GetTheta(id1)
      << " PhiFast "   << GetPhi  (id1) << std::endl;
}

int AMSTrTrack::DoAdvancedFit()
{
  /*! "Advanced fit" is assumed to perform all the fitting: 
   *   1: 1st harf, 2: 2nd harf, 6: with nodb, 4: "Fast fit" ims=0 and 
   *   5: Juan with ims=1 */

  Fit(kChoutko | kUpperHalf);
  Fit(kChoutko | kLowerHalf);
  Fit(kChoutko);
  Fit(kAlcaraz);

  return AdvancedFitDone();
}

double AMSTrTrack::Interpolate(double zpl, AMSPoint &pnt, 
			       AMSDir &dir, Int_t id)
{
  if (!FitDone(id)) return -1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  dir.setp(0, 0,   1);
  pnt.setp(0, 0, zpl);
  return tprop.Interpolate(pnt, dir);
}

void AMSTrTrack::Interpolate(int nz, double *zpl, 
			     AMSPoint *pvec, AMSDir *dvec, double *lvec,
			     Int_t id)
{
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  tprop.Interpolate(nz, zpl, pvec, dvec, lvec);
}

void AMSTrTrack::interpolate(AMSPoint pnt, AMSDir dir, AMSPoint &P1, 
			     number &theta, number &phi, number &length, 
			     int id)
{
  if (!FitDone(id)) return;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  length = tprop.Interpolate(pnt, dir);
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();
}

bool AMSTrTrack::interpolateCyl(AMSPoint pnt, AMSDir dir, number rad, 
				number idir, AMSPoint &P1, number &theta, 
				number &phi, number &length, int id)
{
  if (!FitDone(id)) return false;

  int sdir = (idir < 0) ? -1 : 1;

  TrProp tprop(GetP0(id), GetDir(id), GetRigidity(id));
  if ((length = tprop.InterpolateCyl(pnt, dir, rad, sdir)) < 0) return false;
  P1     = pnt;
  theta  = dir.gettheta();
  phi    = dir.getphi();

  return true;
}


#include <strstream>

void AMSTrTrack::_printEl(std::string& sout)
{
  char cc[1024];
  strstream ost(cc, 1024);
  _printEl(ost);
  sout.append(cc);
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

