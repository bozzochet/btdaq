//  $Id: GbatchRec.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __GbatchRec__
#define __GbatchRec__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchRec.h
///\brief Header file of GbatchRec class
///
///\class GbatchRec
///\brief A Gbatch interface to access classes in trrec.C
///\ingroup gbint
///
/// GbatchRec is a pure virtual class for GbatchRecTmp.
/// It enables us to access track parameters of AMSTrTrack and TkTrack.
/// An instansiace is made at GbatchRec::GetPtr(), where appropreate 
/// track class is selected depending on gbtrk::reccode.
///
///\date  2007/11/29 SH  First test version
///\date  2007/12/09 SH  Second test version (Template version)
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

class GbatchRec : public TObject {

protected:
  static GbatchRec* _ptr;

  GbatchRec(void);              ///< For the singleton class

public:
  virtual ~GbatchRec();

  static GbatchRec *GetPtr(void);

  virtual Int_t    GetNtrack  (void)  const { return 0; }
  virtual Int_t    GetNhits   (Int_t) const { return 0; }
  virtual Double_t GetChisq   (Int_t) const { return 0; }
  virtual Double_t GetRigidity(Int_t, Int_t = 0) const { return 0; }
  virtual Double_t GetPhi     (Int_t, Int_t = 0) const { return 0; }
  virtual Double_t GetTheta   (Int_t, Int_t = 0) const { return 0; }

  /// Set track pointer array, _ptrack, to be implemented
  virtual void SetTracks() = 0;

  ClassDef(GbatchRec, 1)  // Gbatch interface to access classes in trrec.C
};

#endif
