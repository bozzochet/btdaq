*  $Id: tszfit.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE TSZFIT(WLOC,SPCOR,ZCOR,WZCOR,NHTKD,NPARSZ,ZRCOR,IFLASZ)
C
C********************************************************************012
C.
C...TSZFIT          FIT A TRACK IN ZS PROJECTION
C.
C.  I/O VIA COMMON BLOCKS TKDCDE,IMGCON,TFITRE.
C.
C.  NOTE: HIT REJECTION IS SUSPENDED, IF EITHER
C.        -CHI-SQUARED PROBABILITY IMPROVES LESS THAN BY FACTOR PRATMN
C.        -OR MORE THAN FRACTION REJMAX OF HITS WOULD BE REJECTED
C.------------------------------------------------------------------
C
#include "trfitcl.h"
       DIMENSION  SPCOR(*), ZCOR(*), WZCOR(*), ZRCOR(*), WLOC(*)
       DIMENSION  S(6)
       DOUBLE PRECISION  S,ZOR,ZSL,DETER,YLOC,ZLOC,WLOC
C-- STATEMENT FUNCTIONS
       YLOC(I) = WZCOR(I) *(SPCOR(I) - SP0)
       ZLOC(I) = WZCOR(I) * ZCOR(I)
C-- REJECTION OF POINTS AVAILABLE BUT NOT USED NOW
      RESMAX = 99.99
      REJMAX =  0.00
C
      IWANT  = 0
      NDFZ   = 0
      NHTKMN = 3
      NHINIT = 0
      IH1    = 1
      IH2    = NHTKD
      SP0    = SPCOR(IHATS)
C
      DO  5 I = IH1,IH2
CCC   YLOC(I) = WZCOR(I) *(SPCOR(I) - SP0)
CCC   ZLOC(I) = WZCOR(I) * ZCOR(I)
      WLOC(I) = WZCOR(I)
      IF ( WLOC(I).GT.0.0 )    NHINIT = NHINIT + 1
    5 CONTINUE
      IF (NHINIT.LT.NHTKMN)                        GO TO 200
      NHTAKE = NHINIT
C
      DO 10 I=1,6
   10 S(I) = 0.
C-
C- VERTEX CONTRIBUTION TO 2 PARAMETER FIT
C-
      IF ( IFVER.GT.0 )  THEN
         ZVX  = ZVER
         S(1) = 1./ZVERR**2
         S(2) = S(1)*ZVX
         S(3) = S(1)*SP0
         S(4) = S(1)*ZVX**2
         S(5) = S(1)*SP0*ZVX
         S(6) = S(1)*SP0**2
      END IF
C-
C- COMPUTE SUMS
      DO 30  IH = IH1,IH2
         IF ( WLOC(IH).LE.0.0    )                   GO TO  30
         S(1) = S(1) + WLOC(IH)**2
         S(2) = S(2) + WLOC(IH)*ZLOC(IH)
         S(3) = S(3) + WLOC(IH)*YLOC(IH)
         S(4) = S(4) + ZLOC(IH)**2
         S(5) = S(5) + YLOC(IH)*ZLOC(IH)
         S(6) = S(6) + YLOC(IH)**2
   30 CONTINUE
C-
   40 CONTINUE
      IF (NPARSZ.EQ.2)  THEN
C- TWO PARAMETER FIT
         DETER  = S(1)*S(6) - S(3)**2
         IF (DETER.LE.0.)                             GO TO 200
         ZOR    = (S(2)*S(6)-S(3)*S(5))/DETER
         ZSL    = (S(1)*S(5)-S(2)*S(3))/DETER
         S2ZOR  =  S(6)/DETER
         S2ZSL  =  S(1)/DETER
         CHI    =  S(4) - ZOR*S(2) - ZSL*S(5)
      END IF
C
      IF (NPARSZ.EQ.1)  THEN
C- ONE PARAMETER FIT
         ZOR     = ZVER
         ZSL     = (S(5) - ZOR*S(3))/S(6)
         S2ZOR   = ZVERR**2
         S2ZSL   = 1./S(6)
         CHI     =  S(4) - ZOR*S(2) - ZSL*S(5)
         CHI     =   CHI + ZOR*(ZOR*S(1)-S(2)+ZSL*S(3))
      END IF
C-
C- COMPUTE RESIDUALS AND FIND WORST HIT
      RESM    = 0.
      DO 80  IH = 1,NHTKD
         RES     = ZLOC(IH)-ZOR*WZCOR(IH)-ZSL*YLOC(IH)
         ZRCOR(IH) = RES
         IF ( WZCOR(IH).NE.0.0 ) ZRCOR(IH) = RES / WZCOR(IH)
         IF ( WLOC(IH).LE.0.0 )                       GO TO  80
         RES     = ABS(RES)
         IF ( RES.LT.RESM )                           GO TO  80
         RESM    = RES
         IHM     = IH
   80 CONTINUE
C-
C- SEE IF ACCEPTABLE
      IF (RESM.LT.RESMAX )                         GO TO  90
C- ARE THERE ENOUGH POINTS LEFT?
      IF (NHTAKE.LE.NHTKMN)                        GO TO  90
      REFRAC = FLOAT(NHINIT-NHTAKE+1)/NHINIT
C- ALREADY TOO MANY POINTS REJECTED?
      IF (REFRAC.GT.REJMAX)                        GO TO  90
C-
C- REJECT WORST HIT AND REFIT
      NHTAKE = NHTAKE - 1
      S(1) = S(1) - WLOC(IHM)**2
      S(2) = S(2) - WLOC(IHM)*ZLOC(IHM)
      S(3) = S(3) - WLOC(IHM)*YLOC(IHM)
      S(4) = S(4) - ZLOC(IHM)**2
      S(5) = S(5) - YLOC(IHM)*ZLOC(IHM)
      S(6) = S(6) - YLOC(IHM)**2
      WLOC(IHM) = -1.0
      GO                      TO                          40
C
   90 CONTINUE
      NREJPR = 100*(NHINIT-NHTAKE)/NHINIT
C-
C- DEFINE ALSO VERTEX RESIDUAL
c      IHVX   = NHTKD + 1
c      ZRCOR(IHVX) = ZOR*WZCOR(IHVX)
c      IF ( IFVER.GT.0 ) THEN
c         IF ( NPARSZ.EQ.2 ) ZRCOR(IHVX) = ZOR/ZVERR
c      ENDIF
C-
      ZOPAR  = ZOR
      ZSPAR  = ZSL
      ZOESQ  = S2ZOR
      ZSESQ  = S2ZSL
      CHIZ   = CHI
      NDFZ   = NHTAKE - NPARSZ
      IWANT  = 1
  200 CONTINUE
      END
