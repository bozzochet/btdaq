//  $Id: GbatchDefs.h,v 1.1 2008/01/21 21:58:33 haino Exp $
#ifndef __GbatchDefs__
#define __GbatchDefs__

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchDefs.h
///\brief External Gbatch/Fortan calls definitions
///
///\date  2007/11/21 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:33 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#define mipsFortran
#include "cfortran.h"

//-------------------------------------
// ZEBRA stuff
//-------------------------------------

const int NWGEAN = 13000000;
const int NWPAW  = 1200000;

//-------------------------------------
// Fortran calls
//-------------------------------------

PROTOCCALLSFSUB0(GFFGOI,gffgoi)
PROTOCCALLSFSUB0(GFFGOF,gffgof)
PROTOCCALLSFSUB1(FFONE, ffone, PINT)

#define GFFGOI()  CCALLSFSUB0(GFFGOI,gffgoi)
#define GFFGOF()  CCALLSFSUB0(GFFGOF,gffgof)
#define FFONE(A1) CCALLSFSUB1(FFONE,ffone,PINT,A1)

//-------------------------------------
// C-Fortran interface
//-------------------------------------

/// A namespace which includes Gbatch/Geant3 fortran interface functions
namespace gbint {
  int  ffone (const char *);    ///< Call one step of FFGO
  void gstran(int);             ///< Define the stranglet particle
}

#endif
