// $Id: AlignFitGB.C,v 1.1 2008/04/18 19:41:58 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  AlignFitGB.C
///\brief Source file of AlignFitGB
///
///\date  2007/04/02 SH  First test version
///$Date: 2008/04/18 19:41:58 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "AlignFitGB.h"

#include "TStopwatch.h"

#include <iostream>

extern "C" {
  void alignfit_(float [AlignFitGB::Ndim][AlignFitGB::Nplan], 
		 int   [AlignFitGB::Ndim][AlignFitGB::Nplan], 
		 float &, int &, int &, float [2], 
		 float [AlignFitGB::Nchi2][2], float &, int &);
}

void AlignFitGB::Init(void)
{
  Int_t what = -2;
  Call(0, 0, 0, 0, what, 0, fXchi2, 0, 0);
}

void AlignFitGB::SetHits(Float_t arr[Ndim][Nplan], Int_t prm[Ndim][Nplan])
{
  Int_t what = 0;
  Call(arr, prm, 0, 0, what, 0, 0, 0, 0);
}

void AlignFitGB::Fit(Float_t chi2m, Int_t algo, Float_t xf[2], 
		     Float_t rigmin, Int_t itermin)
{
  TStopwatch timer;
  timer.Start();

  Int_t what = 1;
  Call(0, 0, chi2m, algo, what, xf, fXchi2, rigmin, itermin);
  std::cout << "CpuTime " << timer.CpuTime() << std::endl;
}

void AlignFitGB::Get(Float_t arr[Ndim][8], Int_t what)
{
  Call(arr, 0, 0, 0, what, 0, 0, 0, 0);
}

void AlignFitGB::Call(Float_t arr[Ndim][Nplan], Int_t prm[Ndim][Nplan],
		      Float_t chi2m, Int_t algo, Int_t what, 
		      Float_t xf[2], Float_t xchi2[Nchi2][2],
		      Float_t rigmin, Int_t itermin)
{
  alignfit_(arr, prm, chi2m, algo, what, xf, xchi2, rigmin, itermin);
}
