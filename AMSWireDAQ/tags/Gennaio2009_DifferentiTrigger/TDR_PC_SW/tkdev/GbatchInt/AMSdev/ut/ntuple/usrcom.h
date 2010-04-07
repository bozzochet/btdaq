//  $Id: usrcom.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef USRCOM_H
#define USRCOM_H

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif

#include "cfortran.h"

static const int MXVAR=222;
static const int MXCOL=20;
struct USRCOM_DEF {
   int  nvar;
   char varlis[MXVAR][MXCOL];
};
#define usrCom COMMON_BLOCK(USRCOM,usrcom)
COMMON_BLOCK_DEF(USRCOM_DEF,usrCom);

PROTOCCALLSFFUN1(INT,USRCUT,usrcut,INT)
#define USRCUT(A1) CCALLSFFUN1(USRCUT,usrcut,INT,A1)

#endif

