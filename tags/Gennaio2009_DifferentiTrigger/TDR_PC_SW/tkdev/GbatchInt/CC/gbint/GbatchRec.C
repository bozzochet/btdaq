// $Id: GbatchRec.C,v 1.2 2008/01/22 23:41:06 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchRec.cc
///\brief Gbatch interface to access classes in trrec.C
///
///\date  2007/11/29 SH  First test version
///$Date: 2008/01/22 23:41:06 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchRec.h"

#include <iostream>

ClassImp(GbatchRec)

GbatchRec *GbatchRec::_ptr = 0;

GbatchRec::GbatchRec(void)
{
}

GbatchRec::~GbatchRec()
{
}

#include "trrec.h"    // for AMSTrTrack
#include "tkcalls.h"
#include "TrTrack.h"
#include "GbatchRecTmp.h"

GbatchRec *GbatchRec::GetPtr(void)
{
  if (_ptr) return _ptr;

  if (gbtrk::reccode == gbtrk::GBATCH) {
    _ptr = new GbatchRecTmp<AMSTrTrack>;
    std::cout << "GbatchRecTmp<AMSTrTrack> is created" << std::endl;
  }
  else {
    _ptr = new GbatchRecTmp<TrTrack>;
    std::cout << "GbatchRecTmp<TrTrack> is created" << std::endl;
  }

  return _ptr;
}
