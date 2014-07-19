//  $Id: GbatchRecTmp.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __GbatchRecTmp__
#define __GbatchRecTmp__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchRecTmp.h
///\brief Header file of GbatchRecTmp class
///
///\class GbatchRecTmp
///\brief A template class to access reconstructed tracks
///\ingroup gbint
///
/// GbatchRecTmp is a template class inherited from GbatchRec.
/// It enables us to access track parameters of AMSTrTrack and TkTrack.
/// An instansiace is made at GbatchRec::GetPtr(), where appropreate 
/// track class is selected depending on gbtrk::reccode.
///
///\date  2007/12/09 SH  First test version
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchRec.h"

template <class TRACK>
class GbatchRecTmp : public GbatchRec {

protected:
  /// Maximum number of tracks
  enum { NTRK = 10 };

  Int_t   _ntrack;        //!< Number of tracks reconstructed
  TRACK  *_ptrack[NTRK];  //!< Array of AMSTrTrack pointers

public:
  GbatchRecTmp(void);
 ~GbatchRecTmp();

  Int_t    GetNtrack  (void)  const { return _ntrack; }
  Int_t    GetNhits   (Int_t) const;
  Double_t GetChisq   (Int_t) const;
  Double_t GetRigidity(Int_t, Int_t method = 0) const;
  Double_t GetPhi     (Int_t, Int_t icase  = 0) const;
  Double_t GetTheta   (Int_t, Int_t icase  = 0) const;

  void SetTracks();                 ///< Set track pointer array, _ptrack

protected:
  TRACK *GetTrack(Int_t i) const;   ///< Get track pointer at i
};

template <class TRACK>
GbatchRecTmp<TRACK>::GbatchRecTmp(void)
{
}

template <class TRACK>
GbatchRecTmp<TRACK>::~GbatchRecTmp()
{
}

template <class TRACK>
Int_t GbatchRecTmp<TRACK>::GetNhits(Int_t i) const
{
  // Get number of hits in the track at i
  return (GetTrack(i)) ? _ptrack[i]->getnhits() : 0;
}

template <class TRACK>
Double_t GbatchRecTmp<TRACK>::GetChisq(Int_t i) const
{
  // Get chisquare of the track at i
  return (GetTrack(i)) ? _ptrack[i]->getchi2() : 0;
}

template <class TRACK>
Double_t GbatchRecTmp<TRACK>::GetRigidity(Int_t i, Int_t method) const
{
  // Get rigidity of the track at i
  return (GetTrack(i)) ? _ptrack[i]->getrid(method) : 0;
}

template <class TRACK>
Double_t GbatchRecTmp<TRACK>::GetPhi(Int_t i, Int_t icase) const
{
  // Get phi of the track at i
  return (GetTrack(i)) ? _ptrack[i]->getphi(icase) : 0;
}

template <class TRACK>
Double_t GbatchRecTmp<TRACK>::GetTheta(Int_t i, Int_t icase) const
{
  // Get theta of the track at i
  return (GetTrack(i)) ? _ptrack[i]->gettheta(icase) : 0;
}

template <class TRACK>
void GbatchRecTmp<TRACK>::SetTracks(void)
{
  // Set track pointer array

  _ntrack = 0;

  for (Int_t i = 0; i < TKDBc::npat() && _ntrack < NTRK; i++) {
    AMSContainer *p = AMSEvent::gethead()->getC("AMSTrTrack", i);
    if(!p || !TKDBc::patallow(i)) continue;

    TRACK *trk = (TRACK *)p->gethead();
    if (!trk) continue;

    _ptrack[_ntrack++] = trk;
  }
}

template <class TRACK>
TRACK *GbatchRecTmp<TRACK>::GetTrack(Int_t i) const
{
  // Get track at i
  return (_ptrack && 0 <= i && i < _ntrack) ? _ptrack[i] : 0;
}

#endif
