* $Id: gffgof.F,v 1.1 2008/01/21 21:58:29 haino Exp $
********** ADDED by SH **********
*
* Last half of GFFGO
*
*   21/Nov/2007 SH  First test version
*   26/Nov/2007 SH  First stable version
*
********** ADDED by SH **********
*
#include "geant321/pilot.h"
*CMZ :  3.21/04 22/02/95  08.50.24  by  S.Ravndal
*-- Author :
      SUBROUTINE GFFGOF
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Routine to define and read GEANT/FFREAD data cards       *
C.    *      If user data cards have been defined via FFKEY            *
C.    *      they will be read as well                                 *
C.    *                                                                *
C.    *    ==>Called by : <USER>, UGINIT                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcphys.inc"
#include "geant321/gccuts.inc"
#include "geant321/gcflag.inc"
#include "geant321/gckine.inc"
#include "geant321/gclist.inc"
#include "geant321/gcparm.inc"
#include "geant321/gcstak.inc"
#include "geant321/gctime.inc"
#include "geant321/gctrak.inc"
#include "geant321/gcmulo.inc"
#include "geant321/gcscan.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcopti.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcrz.inc"
C.
C.    ------------------------------------------------------------------
C.
      IABAN = NINT(DPHYS1)
C
C             Get some parameters from the data cards
C
      CALL GETNUM (LHSTA ,NHSTA)
      CALL GETNUM (LGET  ,NGET)
      CALL GETNUM (LSAVE ,NSAVE)
      CALL GETNUM (LRGET ,NRGET)
      CALL GETNUM (LRSAVE,NRSAVE)
      CALL GETNUM (LSETS ,NSETS)
      CALL GETNUM (LPRIN ,NPRIN)
      CALL GETNUM (LGEOM ,NGEOM)
      CALL GETNUM (LVIEW ,NVIEW)
      CALL GETNUM (LPLOT ,NPLOT)
      CALL GETNUM (LSTAT ,NSTAT)
      CALL GETNUM (ISLIST,NSLIST)
*
      IF(ILABS.NE.0) THEN
         IF(ITCKOV.NE.0) ILABS=1
      ENDIF
*
      IF (NINT(DPHYS1).NE.IABAN) THEN
          DPHYS1 = IABAN
      END IF
*
      IF(MODTET.LT.1.OR.MODTET.GT.MAXMDT) THEN
         WRITE(LOUT,10000) MODTET
10000    FORMAT(' MODTET = ',I2,' out of range - 1 assumed')
         MODTET=1
      ENDIF
      TETMIN = TETMID(MODTET)
      TETMAX = TETMAD(MODTET)
      IF(IDEVT.GT.0)IDEVT=IDEVT-1
C
C             Set IDEBUG flag for initialisation phase
C
      IF(IDEMIN.LT.0.AND.IDEMAX.GE.0)THEN
         IDEMIN=-IDEMIN
         IDEBUG=1
      ENDIF
C
C             Initialise the random number generator
C
      IF(NRNDM(2).NE.0)THEN
         CALL GRNDMQ(NRNDM(1),NRNDM(2),1,'S')
      ELSEIF(NRNDM(1).GT.0)THEN
         ISEQ=NRNDM(1)
         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'Q')
         CALL GRNDMQ(NRNDM(1),NRNDM(2),ISEQ,'S')
      ENDIF
C
  999 END
