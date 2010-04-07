//  $Id: GbatchDefs.C,v 1.1 2008/01/21 21:58:26 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchDefs.cc
///\brief External Gbatch/Fortan calls definitions
/// 
///\date  2007/11/21 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:26 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchDefs.h"
#include "cern.h"

//-------------------------------------
// ZEBRA stuff
//-------------------------------------

struct PAWC_DEF {
  float q[NWPAW];
};

struct GCBANK_DEF {
  float q[NWGEAN];
};

#define GCBANK COMMON_BLOCK(GCBANK,gcbank)
COMMON_BLOCK_DEF(GCBANK_DEF,GCBANK);
GCBANK_DEF GCBANK;

#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;


//-------------------------------------
// FFKEY buffer
//-------------------------------------

#define FFCHAR_LENMAX 132

struct FFCHAR_DEF {
  char buffer[FFCHAR_LENMAX];
};

#define FFCHAR COMMON_BLOCK(FFCHAR,ffchar)
COMMON_BLOCK_DEF(FFCHAR_DEF,FFCHAR);

//-------------------------------------
// Fortran-C interface
//-------------------------------------

#undef GUKINE
#undef GUSTEP
#undef GUOUT
#include "GbatchInterface.h"

extern "C" void gukine_(void)
{
  GbatchInterface::Gukine();
}

extern "C" void gustep_(void)
{
  GbatchInterface::Gustep();
}

extern "C" void guout_(void)
{
  GbatchInterface::Guout();
}

//-------------------------------------
// C-Fortran interface
//-------------------------------------

#include "dcards.h"
#include <cstring>

extern "C" void gstran_(int&, float&, float&);

namespace gbint {
  int ffone(const char *str)
  { 
    // Execute one step of FFGO

    char *buf = FFCHAR.buffer;
    int len = strlen(str), lmax = 110;
    if (len > lmax) len = lmax;

    // Copy FFKEY string to the buffer
    strncpy(buf, str, len);

    // Fill spcace in the rest of buffer
    memset(&buf[len], ' ', lmax-len);
    buf[lmax] = '\0';

    // Execute one step of FFGO
    int istat; 
    FFONE(istat); 
    return istat;
  }

  void gstran(int itrt)
  {
    // Define the stranglet particle
    gstran_(itrt,CCFFKEY.StrCharge,CCFFKEY.StrMass);
  }
}
