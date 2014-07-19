*  $Id: trafit.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE TRAFIT
     $      (IFIT,X,Y,WXY,Z,SSZ,NPT,RESXY,RESSZ,IFLAG,SPCOR,WORK,
     +       SCHIXY,SCHIZ,xmom,exmom,p0,theta,phit)
C
C********************************************************************012
C.
C
C
C
C
C.
C.      THE TRACK IS FITTED SEPARATELY
C.         AS A CIRCLE IN THE XY PROJECTION (TXYFIT) AND
C.         AS A  LINE  IN THE ZS PROJECTION (TSZFIT).
C.
C.  parameters :
C.    (in)      TID         track identifier
C.    (in)      X   (npt)   X-coordinates   in cm
C.    (in)      Y   (npt)   Y-coordinates   in cm
C.    (in)      WXY (npt)   weights = 1/sigma**2  in the XY-projection
C.    (in)      Z   (npt)   Z-coordinates   in cm
C.    (in)      SSZ (npt)   weights = 1/sigma     in the SZ-projection
C.    (in)      NPT         number of points to be fitted
C.    (in)      NPARXY      number of parameters in xy fit
C.                          3 = circle fit,  2 = line fit
C.    (out)     RESXY (npt) residuals from the XY fit       in cm
C.    (out)     RESSZ (npt) residuals from the SZ line fit  in cm
C.    (out)     IFLAG       error flag,  0 = OK
C.    (out)     SPCOR (npt) track length, transfer from xy to sz fit
C.      -       WORK  (2*npt)  work space for sz fit - double space !!!
C.
C. For correct error calculation, give weights correctly:
C.    for xy points:  weight = 1 / sigma**2
C.    for  z points:  weight = 1 / sigma      (not squared)
C. Residuals are given in cm for both fits, chisq in (resi/sigma)**2
C.----------------------------------------------------------------------
C
#include "trfitcl.h"
C
       DIMENSION  X(*), Y(*), WXY(*), Z(*), SSZ(*), p0(*)
       DIMENSION  RESXY(*), RESSZ(*), SPCOR(*)
       double precision WORK(*)
C
        nparxy=3
       IFLAG    = 0
       call getfield(BTFIEL)
C       BTFIEL=-0.15*0.95
C
C--       FIRST FIT IN XY PROJECTION
C
       iflaxy=0
       iflasz=0
       tid=1
       if(ifit.eq.0 .or.ifit.eq. 1)
     +       CALL TXYFIT(X,Y,WXY,SPCOR,NPT,3,RESXY,IFLAXY)
C
       IF ( IFLAXY .NE. 0 )                    GO TO 800
C
C--        AND THEN IN Z-S PROJECTION
C
      NPARSZ  = 2
C
       if(ifit.eq.0 .or.ifit.eq. 2)       
     +      CALL TSZFIT(work,SPCOR,Z,SSZ,NPT,NPARSZ,RESSZ,IFLASZ)
C
      IF ( IFLASZ .NE. 0 )                       GO TO 800
C
      TDIP    = ZSPAR
      CDIP    = 1./SQRT(1.+TDIP**2)
      DIP     = ATAN(TDIP)
      S2DIP   = CDIP**4 * ZSESQ
      PINV    = PPRINV*CDIP
C
C--       SAVE FIT RESULTS IN TKD, TE, TP FOR ASSOCIATED TRACK
C
      ZATV    = ZOPAR
      S2ZAV   = ZOESQ
      ZATS    = ZATV + (SPVER+SPCOR(IHATS))*TDIP
C- CORRELATION TERM NEEDED TOO
      S2ZAS   = ZOESQ + ZSESQ * (SPVER+SPCOR(IHATS))**2
C
      TPARAM( 1) = TID
      TPARAM( 2) = 0.0
      TPARAM( 3) = 0.0
      TPARAM( 4) = SPLEN/CDIP
      TPARAM( 5) = NDFXY+NPARXY
      TPARAM( 6) = XATS
      TPARAM( 7) = YATS
      TPARAM( 8) = ZATS
      TPARAM( 9) = PINV * CHARGE
      TPARAM(10) = DIP
      TPARAM(11) = PHIS
      TPARAM(12) = PHIV
      TPARAM(13) = XATV
      TPARAM(14) = YATV
      TPARAM(15) = ZATV
C
      TPERR( 1) = CHIZ
      TPERR( 2) = NDFZ
      TPERR( 3) = NPARSZ
      TPERR( 4) = NOXYZ
      TPERR( 5) = S2XOYV
      TPERR( 6) = S2ZAV
      TPERR( 7) = S2PPRI*CDIP**2 + S2DIP*(PINV*TDIP)**2
      TPERR( 8) = S2DIP
      TPERR( 9) = S2PHIV
      TPERR(10) = COVPRV*CDIP
      TPERR(11) = CHIXY
      TPERR(12) = NDFXY
      TPERR(13) = NPARXY
      TPERR(14) = S2PHIS
      TPERR(15) = S2XOYS
      TPERR(16) = S2ZAS
      TPERR(17) = COVPRS*CDIP
      p0(1)=tparam(6)
      p0(2)=tparam(7)
      p0(3)=tparam(8)
      exmom=tperr(7)
      if(dip.gt.0)then
          theta=3.1415926-dip
          phit=-3.1415926/2.-phis
          if(phit.lt.-3.1415926)phit=2*3.1415926+phit
       else
          theta=3.1415926+dip
          phit=phis-3.1415926/2.
       endif
      if(pinv.ne.0) then
       xmom=1./pinv/charge
       erxy=sqrt(1/abs(wxy(1)))
       ertot=sqrt(0.001**2+(erxy*pinv)**2)
       xfacxy=erxy/ertot
       SCHIXY=CHIXY*xfacxy
       erz=1/ssz(1)
       ertot=sqrt(0.0025**2+(erz*pinv)**2)
       xfacz=erz/ertot
       SCHIZ=CHIZ*xfacz
c       write(*,*)erz,erxy,xfacxy,xfacz,xmom
      else
       xmom=1000000.
       schixy=1000000.
       schiz=1000000.
      endif
C
      GO TO 900
 800  CONTINUE
      IFLAG     = 1000 * IFLAXY + IFLASZ
C
 900  CONTINUE
      END

      subroutine getfield(bt)
       integer init/0/
       real b/0/
       real x(3),bb(3)
       save b
       save init
       if(init.eq.0)then
        init=1
        b=0
        do i=1,11
         do j=1,11
          do k=1,11
           x(1)=6*(i-6)
           x(2)=6*(j-6)
           x(3)=8*(k-6)
           call gufld(x,bb)
           b=b+(bb(1)**2+bb(2)**2+bb(3)**2)**0.5
          enddo
         enddo
        enddo
        b=b/11/11/11
        write(*,*)'getfield-AverageMagField ',-b/10 , ' Tesla'
       endif
       bt=-b/10
      end    
