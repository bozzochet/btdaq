*  $Id: txyfit.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE TXYFIT(XCOR,YCOR,WTCOR,SPCOR,NPT,NPARXY,RRCOR,IFLAG)
C
C********************************************************************012
C.
C...TXYFIT    FIT CIRCLE TRAJECTORY IN X-Y PROJECTION
C.
C.    OUTPUT: PARAMETERS IN COMMON /TFITRE/
C.
C.------------------------------------------------------------------
C
#include "trfitcl.h"
C
      DIMENSION XCOR(*), YCOR(*), WTCOR(*), SPCOR(*), RRCOR(*)
C
      PARAMETER ( QPI  = 3.14159265 )
      PARAMETER ( QPI2 = 3.14159265 * 2.0 )
C
      RESMAX  = 99.99
      REJMAX  =  0.00
      NHF     = NPT
C
      CALL TCIRCF(XCOR,YCOR,WTCOR,RRCOR,
     $            RESMAX,REJMAX,NHF,1,NPARXY,IFLAG)
C
      IF (IFLAG.LT.0)                               GO TO 1000
C
      CHIXY   = CHITR
      NDFXY   = NGOOD - NPARXY
      BINV3   = 0.
C-- length units are centimeters, need 100 to get GeV
      IF (BTFIEL.NE.0.) BINV3 = 100./ABS(.3*BTFIEL)
      RINV    = ABS(RO)
      PPRINV  = RINV*BINV3
      PPRINV  = AMAX1(1.E-20,PPRINV)
      XCEN    = A
      YCEN    = B
C-
C- CHECK DIRECTION
      IF ( IFVER.GT.0 )  THEN
         DIRTES  = COSF*(XS-XVER) + SINF*(YS-YVER)
      ELSE
         DIRTES  = COSF*(XS     ) + SINF*(YS     )
      END IF
      IF (DIRTES.LT.0.)   THEN
         SIGNR  = -SIGNR
         RO     = -RO
         PHI    = PHI - SIGN(QPI,PHI)
         SINF   = -SINF
         COSF   = -COSF
      END IF
      CHARGE  = SIGNR*SIGN(1.,BTFIEL)
      THET0   = ATAN2(YCOR(IGF)-YCEN,XCOR(IGF)-XCEN)
      THET1   = THET0
      IF (IGF.NE.1)
     $   THET1   = ATAN2( YCOR( 1 )-YCEN, XCOR( 1 )-XCEN )
      IF (ABS(THET1-THET0).GT.QPI) THET0 = THET0 - SIGN(QPI2,THET0)
      SPC     = -SIGNR*R*(THET1-THET0)
      SD      = 0.
C
C- COMPUTE S-COORDINATES
      SPCMIN  =  999999.
      SPCMAX  = -999999.
      THETOL  = THET0
C
      DO 90 IH= 1, NHF
         IF (IH.EQ.1)                                  GO TO 80
         XD     = XCOR(IH)-XCOR(IH-1)
         YD     = YCOR(IH)-YCOR(IH-1)
         STEP   =XD*(RO*(YCOR(IH)-YS)+COSF)- YD*(RO*(XCOR(IH)-XS)-SINF)
         STEP   = STEP*(1. + (1./6.)*(RO*STEP)**2)
         SD     = STEP+SD
         IF (ABS(SD).LT. 0.2*R )                       GO TO 80
         THETIH = ATAN2(YCOR(IH)-YCEN,XCOR(IH)-XCEN)
C- ALLOW FOR PERIODICITY OF THETA ANGLE
C
         IF (ABS(THETIH-THETOL).LT.QPI)                GO TO 70
         THET0  = THET0 - SIGN(QPI2,THETOL)
 70      THETOL = THETIH
         SPC    = -SIGNR*R*(THETIH-THET0)
         SD     = 0.
 80      SPCOR(IH) = SPC + SD
         IF (WTCOR(IH).LE.0.0 )                        GO TO 90
         IF (SPCOR(IH).GT.SPCMAX)  THEN
            SPCMAX = SPCOR(IH)
            IHATE  = IH
         END IF
         IF (SPCOR(IH).LT.SPCMIN)  THEN
            SPCMIN = SPCOR(IH)
            IHATS  = IH
         END IF
 90   CONTINUE
      SPLEN   = SPCMAX - SPCMIN
      IF (IGF.EQ.IHATS)                                GO TO 100
C-
C- STARTING POINT WAS NOT RIGHT - RECOMPUTE
C-
      CALL TPTCIR(XCOR(IHATS),YCOR(IHATS))
C     ---- ------
C-
 100  CONTINUE
      XATS    = XS
      YATS    = YS
      PHIS    = PHI
C
C- ERRORS AT STARTING POINT
C
      S2PPRI  = COVPHR(3)*BINV3**2
      COVPRS  = COVPHR(2)*BINV3
      S2PHIS  = COVPHR(1)
      NOXYZ   = 1
      IF (ABS(COSF).LT..707) NOXYZ = 2
      IF (NOXYZ.EQ.1) S2XOYS = COVDPD(3) / COSF**2
      IF (NOXYZ.EQ.2) S2XOYS = COVDPD(3) / SINF**2
C
      IF (IFVER.EQ.0)                               GO TO 120
C-
C- PROPAGATE TO THE VERTEX
C
      CALL TPTCIR(XVER,YVER)
C     ---- ------
      XATV    = XS
      YATV    = YS
      PHIV    = PHI
      SPV     = (XATS-XS)*COSF + (YATS-YS)*SINF
      SPVER   = SPV*(1.+(1./6)*(RO*SPV)**2) - SPCOR(IHATS)
      DPHISV  = PHIV - PHIS
      IF (ABS(DPHISV).GT.QPI) DPHISV = DPHISV - SIGN(QPI2,DPHISV)
      IF (ABS(DPHISV).GT. .1) SPVER  = SIGNR*R*DPHISV - SPCOR(IHATS)
C- ERRORS AT VERTEX
      S2PHIV  = COVPHR(1)
      COVPRV  = COVPHR(2)*BINV3
      IF (NOXYZ.EQ.1) S2XOYV = COVDPD(3) / COSF**2
      IF (NOXYZ.EQ.2) S2XOYV = COVDPD(3) / SINF**2
  120 CONTINUE
 1000 CONTINUE
      END
