*  $Id: tcircf.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE TCIRCF(X,Y,W,RES,RESMAX,REJMAX,N,M,K,IFLAG)
C
C********************************************************************012
C.
C.    CHI**2 CIRCLE FIT FOR PARAMETERS:
C.
C.    PHI   = ANGLE AT STARTING POINT
C.    DC    = DISTANCE FROM (XR,YR) TO POINT OF CLOSEST
C.            APPROACH (+VE/-VE IF XR,YR IN/OUTSIDE CIRCLE)
C.    DI    = 1/DISTANCE OF POINT (XR,YR) FROM CIRCLE CENTER
C.    XR,YR = REFERENCE POINT NEAR CIRCLE (FIXED POINT)
C.
C.    POINT (XS,YS) IS THE CLOSEST POINT OF APPROACH
C.    TO THE REFERENCE POINT (XR,YR) - USUALLY THE FIRST
C.    MEASURED POINT ACCEPTED FOR THE FIT
C.
C.      X,Y = ARRAYS OF COORDINATES                              (IN)
C.        W = ARRAY OF WEIGHTS  (SEE NOTE 2)                 (IN/OUT)
C.      RES = ARRAY OF RESIDUALS                                (OUT)
C.   RESMAX = MAXIMUM ALLOWABLE RESIDUAL IN NO OF STD'S          (IN)
C.   REJMAX = MAXIMUM ALLOWABLE FRACTION OF REJECTED HITS        (IN)
C.        N = NUMBER OF COORDINATES IN ARRAYS X AND Y            (IN)
C.            IF GT 1000 ON INPUT THEN INTERPRETED AS N+1000
C.            IN THIS CASE THE LAST POINT IS NEVER REJECTED.
C.        M = STEP BETWEEN CONSECUTIVE X,Y,W,RES (=1 IF ALL      (IN)
C.            ARE IN SEPARATE ARRAYS)
C.        K = 3 NORMALLY, 2 IF 2-PARAMETER FIT                   (IN)
C.    IFLAG = 0, FIT OK                                         (OUT)
C.    IFLAG =-1: FIT FAILED - TOO FEW POINTS                    (OUT)
C.          =-2: FIT FAILED - FIT MATRIX NOT POSITIVE DEFINITE
C.
C.    VARIABLES IN COMMON /TCIRCL/ (FROM IMG-PAM):
C.
C.   XR,YR  = COORDINATES OF REFERENCE POINT                    (OUT)
C.   PHI    = DIRECTION ANGLE AT STARTING POINT (-PI<PHI<PI)    (OUT)
C.   DC     = SIGNED DISTANCE BETWEEN (XS,YS) AND (XR,YR)       (OUT)
C.   DI     = 1/DISTANCE BETWEEN (XR,YR) AND CIRCLE CENTER      (OUT)
C.   COVDPD = COVARIANCE MATRIX FOR PHI,DC,DI (6 ELEMENTS)      (OUT)
C.   SINF   = SIN(PHI)                                          (OUT)
C.   COSF   = COS(PHI)                                          (OUT)
C.   SIGNR  = SIGN OF CURVATURE (+VE IF BENDING CLOCK-WISE)     (OUT)
C.   XS,YS  = STARTING POINT ON THE CIRCLE                      (OUT)
C.   A,B,RO = CIRCLE CENTER AND CURVATURE                       (OUT)
C.   COVPHR = COVARIANCE MATRIX FOR PHI,1/R (3 ELEMENTS)        (OUT)
C.   IGF    = SEQUENCE NUMBER OF FIRST POINT USED IN THE FIT    (OUT)
C.   IGL    = SEQUENCE NUMBER OF LAST  POINT USED IN THE FIT    (OUT)
C.   NGOOD  = NUMBER OF POINTS USED IN THE FIT                  (OUT)
C.   CHITR  = CHI SQUARED SUM OF THE FIT                        (OUT)
C.
C.   NOTES: 1. RESIDUALS MUST BE SMALLER THAN .01*R OR SO
C.          2. WEIGHTS ARE SET NEGATIVE FOR REJECTED POINTS
C.          3. ZERO CURVATURE FIT IF K=2
C.                                           AUTHOR: VJK
C.---------------------------------------------------------------
      DOUBLE PRECISION S(10),COXYRR(6),SWINV,SWINV2
C
#include "trfitcl.h"
C
      DIMENSION  X(*), Y(*), W(*), RES(*)
C
      PARAMETER ( QPI  = 3.14159265 )
      PARAMETER ( QPI2 = 3.14159265 * 2.0 )
C-
      NPTOT  = MOD (N,1000)
      IMAX   = M*(NPTOT-1) + 1
      IVEND  = N / 1000
      NREJE  = 0
      NGOOD  = 0
      DO 10 I=1,IMAX,M
      IF (W(I).GT.0.) NGOOD = NGOOD + 1
   10 CONTINUE
      IF (NGOOD.LE.2)                                     GO TO 100
      NREJMX = MAX0 (IFIX(REJMAX*NGOOD),1)
      NREJMX = MIN0 (NREJMX,NGOOD-4)
      WRSLIM = RESMAX**2
   20 CONTINUE
C-
C- INITIALISE FIT
      IMN    = 1
      IMX    = IMAX
      DO 30 I=1,10
   30 S(I)   = 0.D0
      IREF   = 0
   40 CONTINUE
C-
C- COMPUTE SUMS FOR FIT
      DO 50 I=IMN,IMX,M
      IF (W(I).LE.0..AND.IMN.LT.IMX)                      GO TO  50
      IF (IREF.EQ.0) IREF = I
      YP     = Y(I) - Y(IREF)
      XP     = X(I) - X(IREF)
      WX     = W(I) * XP
      WY     = W(I) * YP
      S(1)   = S(1) + W(I)
      S(2)   = S(2) + WX
      S(3)   = S(3) + WY
      S(4)   = S(4) + WX*XP
      S(5)   = S(5) + WX*YP
      S(6)   = S(6) + WY*YP
      IF (K.EQ.2)                                         GO TO  50
      RR     =   .5 * (XP*XP+YP*YP)
      WRR    = W(I) * RR
      S(7)   = S(7) + WRR
      S(8)   = S(8) + WRR*XP
      S(9)   = S(9) + WRR*YP
      S(10)  = S(10)+ WRR*RR
   50 CONTINUE
C-
      IF (S(1).LE.0.)                                     GO TO 200
      SWINV  = 1./S(1)
      SWINV2 = SWINV**2
C-
C- COVARIANCE MATRIX OF MEASUREMENTS X,Y,RR
      COXYRR(1) = SWINV*S(4) - SWINV2*S(2)*S(2)
      COXYRR(2) = SWINV*S(5) - SWINV2*S(2)*S(3)
      COXYRR(3) = SWINV*S(6) - SWINV2*S(3)*S(3)
      COXYRR(4) = SWINV*S(8) - SWINV2*S(2)*S(7)
      COXYRR(5) = SWINV*S(9) - SWINV2*S(3)*S(7)
      COXYRR(6) = SWINV*S(10)- SWINV2*S(7)*S(7)
C-
C- SOLVE PHI
      IF (K.EQ.2) COXYRR(6) = 1.D0
      Y2PHI  = 2.*(COXYRR(2)*COXYRR(6)-COXYRR(4)*COXYRR(5))
      X2PHI  = COXYRR(6)*(COXYRR(1)-COXYRR(3))-COXYRR(4)**2+COXYRR(5)**2
      PHI2   = ATAN2(Y2PHI,X2PHI)
      PHI    = .5*PHI2
      SINF   = SIN(PHI)
      COSF   = COS(PHI)
C-
C- SOLVE DC  AND  DI = 1/(R-DC)
      DI     = (SINF*COXYRR(4)-COSF*COXYRR(5))/COXYRR(6)
      SIGNR  = SIGN(1.,DI)
      DI     = SIGNR*DI
      DC     = SWINV*(DI*S(7)-SIGNR*(SINF*S(2)-COSF*S(3)))
      RO     = SIGNR*DI/(1.+DI*DC)
C-
C- COMPUTE CHI SQUARED AND RESIDUALS
      CHITR  = 0.
      WRSQMX = 0.
      WRSQM1 = 0.
      IGF    = 0
      DO 60 I=1,IMAX,M
      XP     = X(I) - X(IREF)
      YP     = Y(I) - Y(IREF)
      RR     = .5*(XP*XP+YP*YP)
      RES(I) = -DI*RR+SIGNR*(XP*SINF-YP*COSF) + DC
      IF (W(I).LE.0.)                                     GO TO  60
      IF (IGF.EQ.0) IGF = I
                    IGL = I
      WRSQ   = W(I)*RES(I)**2
      CHITR  = CHITR + WRSQ
      IF (WRSQ.GT.WRSQMX) WRSQMX = WRSQ
      IF ( IVEND.GT.0 .AND. I.EQ.IMAX )                   GO TO  60
      IF ( WRSQ.LT.WRSQM1 )                               GO TO  60
      IMX = I
      WRSQM1 = WRSQ
   60 CONTINUE
      IF (WRSQMX.LE.WRSLIM.OR.NREJE.GE.NREJMX)            GO TO  70
C-
C- REFIT IF A POINT WAS REJECTED
      NREJE  = NREJE + 1
      W(IMX) =-W(IMX)
      IF (IMX.EQ.IREF.AND.ABS(DC*DI).GT..01)              GO TO  20
      IMN    = IMX
                                                          GO TO  40
   70 CONTINUE
C-
C- FIT PERFORMED
      NGOOD  = NGOOD - NREJE
C-
C- COVARIANCE MATRIX FOR PHI,DC,DI
      COVDPD(1) =  COSF**2*S(4)+2.*SINF*COSF*S(5)+SINF**2*S(6)
      COVDPD(2) =  SIGNR*(COSF*S(2)+SINF*S(3))
      COVDPD(3) =  S(1)
      COVDPD(4) = -SIGNR*(COSF*S(8)+SINF*S(9))
      COVDPD(5) = -S(7)
      COVDPD(6) =  S(10)
      CALL TRSINV(COVDPD,COVDPD,3)
C     ---- ------
C-
C- FIND RIGHT SIGN OF CURVATURE
      IF (COSF*S(2)+SINF*S(3).GE.0.)                      GO TO  80
      PHI    =  PHI - SIGN(QPI,PHI)
      SINF   = -SINF
      COSF   = -COSF
      RO     = -RO
      SIGNR  = -SIGNR
   80 CONTINUE
C-
C- COVARIANCE MATRIX FOR PHI,1/R
      DDI    =  1./(1.+DC*DI)**2
      DDC    =  -RO**2
      COVPHR(1)= COVDPD(1)
      COVPHR(2)= COVDPD(2)*DDC + COVDPD(4)*DDI
      COVPHR(3)= COVDPD(6)*DDI**2+2.*COVDPD(5)*DDI*DDC+COVDPD(3)*DDC**2
C-
C- STARTING POINT ON THE TRACK
      XS     = X(IREF) - SIGNR*SINF*DC
      YS     = Y(IREF) + SIGNR*COSF*DC
      XR     = X(IREF)
      YR     = Y(IREF)
      IF (RO.NE.0.) R = 1./ABS(RO)
      IF(ABS(RO) .LE. 0.0002) R = 5000.0
      A      = XS + SIGNR*R*SINF
      B      = YS - SIGNR*R*COSF
      IF (IGF.EQ.IREF)                                    GO TO  90
C-
C- PROPAGATE PARAMETERS AND COV MATRIX TO FIRST GOOD POINT
      CALL TPTCIR(X(IGF),Y(IGF))
C     ---- ------
   90 CONTINUE
      IGF    = (IGF+M-1)/M
      IGL    = (IGL+M-1)/M
      IFLAG  = 0
      RETURN
  100 IFLAG  = -1
      RETURN
  200 IFLAG  = -2
      RETURN
      END
