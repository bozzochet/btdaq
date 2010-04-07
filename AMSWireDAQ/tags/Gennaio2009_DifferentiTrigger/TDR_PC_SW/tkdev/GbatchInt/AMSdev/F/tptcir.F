*  $Id: tptcir.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE TPTCIR(XP,YP)
C
C********************************************************************012
C.
C.    PROPAGATE CIRCLE PARAMETERS AND COVARIANCE MATRIX TO
C.    CLOSEST POINT OF APPROACH AT POINT (XP,YP) NEAR CIRCLE
C.
C.    OLD XS,YS,PHI,DC,DI,COVDPD AT (XR,YR) ARE PROPAGATED TO NEW
C.    VALUES AT (XP,YP). (SEE DOCUMENTATION IN D=TCIRCF FOR
C.    THESE QUANTITIES).
C.------------------------------------------------------------------
#include "trfitcl.h"
      DIMENSION CSAV(6),DER(9)
      DATA DER /4*0.,1.,4*0./
C-
      XPR    = XP - XR
      YPR    = YP - YR
      ALFA   = YPR*COSF - XPR*SINF
      BETA   = XPR*COSF + YPR*SINF
      DIRA   = 1.+2.*SIGNR*DI*ALFA+DI**2*(XPR**2+YPR**2)
      DIRA   = 1./SQRT(DIRA)
      DIN    = DI*DIRA
      APUA   = DIRA*(SIGNR*ALFA*DI+1.)
      APUB   = DIRA* SIGNR*BETA
      IF (APUA.LE.0.) APUC = (APUA-1.)/DI**2
      IF (APUA.GT.0.) APUC = -APUB**2/(APUA+1.)
C-
C- PROPAGATE THE COVARIANCE MATRIX
      DER(1) =  APUA*DIRA
      DER(3) = -APUB*DIRA
      DER(4) =  APUB
      DER(6) =  APUC
      DER(7) =  APUB*DIN**2
      DER(9) =  APUA*DIRA**2
      CALL UCOPY(COVDPD,CSAV,6)
      CALL TRASAT(DER,CSAV,COVDPD,3,3)
C     ---- ------
      ROODIN = 1./(DIRA*(1.+DI*DC))
      COVPHR(1) = COVDPD(1)
      COVPHR(2) =-COVDPD(2)*RO**2 + COVDPD(4)*ROODIN**2
C-
C- COMPUTE NEW QUANTITIES AT (XP,YP)
      XPS    = XPR + SIGNR*DC*SINF
      YPS    = YPR - SIGNR*DC*COSF
      RR     = .5*(XPS**2 + YPS**2)
      RAT    = 2./(1.+ROODIN)
      DC     = SIGNR*RAT*(-RO*RR+XPS*SINF-YPS*COSF)
      IF (DI.EQ.0.)                           GO TO  10
      XPC    = XPR - SIGNR*SINF/DI
      YPC    = YPR + SIGNR*COSF/DI
      PHI    = ATAN2(-SIGNR*XPC,SIGNR*YPC)
      SINF   = -SIGNR*XPC*DIN
      COSF   =  SIGNR*YPC*DIN
      DI     = DIN
   10 CONTINUE
      XS     = XP - SIGNR*SINF*DC
      YS     = YP + SIGNR*COSF*DC
      XR     = XP
      YR     = YP
C-
      RETURN
      END
