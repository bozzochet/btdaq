//  $Id: AlignFitGB.h,v 1.1 2008/04/18 19:41:58 haino Exp $
#ifndef __AlignFitGB__
#define __AlignFitGB__

//////////////////////////////////////////////////////////////////////////
///
///\file  AlignFitGB.h
///\brief Header file of AlignFitGB class
///
///\class AlignFitGB
///\brief Alignment fit
///\ingroup gbint
///
///\date  2007/04/02 SH  First test version
///$Date: 2008/04/18 19:41:58 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "TObject.h"

using namespace std;

class AlignFitGB : public TObject {

public:
  enum { Nplan = 8, Nlad = 15, Nhalf = 2, Npar = 2, 
	 Ndim = 11, Narr = 7,  Nchi2 = 10000, MaxCases = 300000,
	 Nbuf = Nplan*Narr*MaxCases };

  Float_t  fXchi2[Nchi2][Npar];

  AlignFitGB(void) {}
 ~AlignFitGB() {}

  void Init(void);

  void SetHits(Float_t arr[Ndim][8], Int_t prm[Ndim][8]);

  void Fit(Float_t chi2m, Int_t algo, Float_t xf[2], 
	   Float_t rigmin, Int_t itermin);

  void Get(Float_t arr[Ndim][8], Int_t what);

  static void Call(Float_t arr[Ndim][8], Int_t prm[Ndim][8],
		   Float_t chi2m, Int_t algo, Int_t what, 
		   Float_t xf[2], Float_t xchi2[Nchi2][2],
		   Float_t rigmin, Int_t itermin);

  ClassDef(AlignFitGB, 1)  // Alignment fit by Gbatch
};

#endif
