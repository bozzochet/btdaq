      SUBROUTINE TKFITA(NPOINTS, XM, EXM, IPLANEZ,
     + IMSCAT, XMASS, OUTPAR, CHI2, NDOF, IERR)
*********************************************************************
*
*     Fitting NPOINTS (consecutive) (maximum 8 points)
*
*     Input Coordinates XM(i,j), i=1,3, j=1, NPOINTS
*     Error to Input Coordinates EXM(i,j), i=1,3, j=1, NPOINTS
*
*     If IMSCAT=0 no multiple scattering error is assumed
*     If IMSCAT<>0 the multiple scattering uncertainty is accounted for
*
*     XMASS is the assumed mass for the particle (GeV)
*
*     Fitted parameters OUTPAR(k), k=1,5
*     OUTPAR(1) = x coordinate at first plane (cm)
*     OUTPAR(2) = y coordinate at first plane (cm)
*     OUTPAR(3) = u_x, component of direction at first plane
*     OUTPAR(4) = u_y, component of direction at first plane
*                 (u_z = -sqrt(1-u_x**2-u_y**2) )
*     OUTPAR(5) = 1/rigidity (1/GV)
*     OUTPAR(6) = sigma(1/rigidity)   
*
*     CHI2 is the chi-square of the fit
*     NDOF is the number of degrees of freedom (=2*npoints-5)
*
*     IERR=0 if everything was OK
*
********************************************
*
*     Things to play with (lengths are in cm):
*        WLEN = number of radiation lengths traversed for each plane
*
*     The multiple scattering uses the formula:
*        (Deviation_rms_x)**2 = (Deviation_rms_y)**2 =
*          (0.0118/(rigidity(GV)*beta))**2 * (WLAD/RLEN)/cos(theta)
*
********************************************
*
*     Written for AMS-01, J. Alcaraz (1999)
*     Updated and improved for AMS-02, J. Alcaraz (February 2003)
*
*********************************************************************
*
      PARAMETER (NHITS=8, NITER=25)
      DIMENSION AXX(3,NHITS), AV(3,NHITS), XM(3,NHITS), XP(2,NHITS), 
     + AL(NHITS),exm(3,nhits),iplanez(nhits)
      DIMENSION D(2*NHITS,5), IDAUX(4*NHITS)
      DIMENSION OUTPAR(6)
      DIMENSION ABSUZ(NHITS) 
      DOUBLE PRECISION ER2(2*NHITS,2*NHITS),DD(5,5), DX(5)
 
      DIMENSION WLEN(NHITS)
C OLD, underestimaed
C      DATA WLEN /0.,3.2E-3,6.4E-3,3.2E-3,6.4E-3,3.2E-3,6.4E-3, 0./
C From test beam:
C        Ladder:    300 um silicon + 50 um kapton + ~3 um de metal: 3.74e-3 X0
C        Shielding: 100 um kapton + ~12 um metal: 1.88e-3 X0
C        Support:  1mm carbon fiber + 9mm carton: 6.35e-3 X0
C      DATA WLEN /0.,11.97e-3,17.59e-3,0.0,17.59e-3,0.0,17.59e-3,11.97e-3/
C AMS02:
C        Ladder:    300 um silicon + 50 um kapton + ~3 um de metal: 3.74e-3 X0
C        Shielding: 100 um kapton + ~12 um metal: 1.88e-3 X0
C        Support:   10mm Al Honeycomb: 1.67e-3 X0
      DATA WLEN /0.,7.29e-3,12.91e-3,0.0,12.91e-3,0.0,12.91e-3,7.29e-3/
      data tol/1.e-5/
      data prec /5.e-3/ ! precision of the algorithm in MV
*
#      include "mfield.h"
*
***---------------------------------------------------------------
*
c      write(*,*)imscat,xmass,iplanez,exm
      IERR = 1

      IF (NPOINTS.LT.3 .OR. NPOINTS.GT.nhits) THEN
        GOTO 999
      ENDIF

      IF (magstat.LE.0) IMSCAT = 0

      do 20 ipl = 1, NPOINTS
        if (ipl.EQ.1 .or. magstat.le.0) then
          DO i=1,3
            axx(i,ipl) = 0.
            av(i,ipl) = 0.
          END DO
        else
          CALL PATH_INTEGRAL (XM(1,ipl-1), XM(1,ipl)
     >               , axx(1,ipl), av(1,ipl) )
        endif
c        print*,'ipl, axx= ', ipl, (axx(i,ipl),i=1,3)
c        print*,'ipl, av= ', ipl, (av(i,ipl),i=1,3)
20    CONTINUE

C Initial parameters
      OUTPAR(1) = XM(1,1)
      OUTPAR(2) = XM(2,1)
      unorm = sqrt(  (XM(1,2)-XM(1,1))**2
     >             + (XM(2,2)-XM(2,1))**2
     >             + (XM(3,2)-XM(3,1))**2)
      OUTPAR(3) = (XM(1,2)-XM(1,1))/UNORM
      OUTPAR(4) = (XM(2,2)-XM(2,1))/UNORM
      OUTPAR(5) = 0.
c      print*,'First: OUTpar= ', OUTpar
      DO J = 1, NPOINTS
        IF (J.EQ.1) then
          AL(1) = 0.
        ELSE
          AL(J) = sqrt(  (XM(1,J)-XM(1,J-1))**2
     >                    +(XM(2,J)-XM(2,J-1))**2
     >                    +(XM(3,J)-XM(3,J-1))**2)
        ENDIF
      END DO
c        print*,' AL= ', AL

C Iterative procedure
      do 30 iter = 1, NITER

C Fetch information from previous iteration
        outpar5=outpar(5)
        DO J = 1, NPOINTS
          IF (J.EQ.1) then
            ux = outpar(3)
            uy = outpar(4)
          ELSE
            ux = ux + outpar5*av(1,J-1)
            uy = uy + outpar5*av(2,J-1)
          ENDIF
          IF (ux**2+uy**2.lt.1.) THEN
            uz = -sqrt(1.-ux**2-uy**2)
            ABSUZ(J) = abs(uz)
          ELSE
* Non-sense
           GOTO 999
          ENDIF

        END DO

C F and G matrices
        CALL VZERO (D, 2*NHITS*5)
        DO J = 1, NPOINTS
          jx = j
          jy = j+NPOINTS
          D(JX,1) = 1.
          D(JY,2) = 1.
          DO K = 1, J
            D(JX,3) = D(JX,3) + AL(K)
            D(JY,4) = D(JY,4) + AL(K)
            IF (magstat.gt.0) THEN
             DO L = 1, K
              IF (L.EQ.K) THEN
                D(JX,5) = D(JX,5) + AL(K)*AL(K)*axx(1,k)
                D(JY,5) = D(JY,5) + AL(K)*AL(K)*axx(2,k)
              ELSE
                D(JX,5) = D(JX,5) + AL(K)*AL(L)*av(1,l)
                D(JY,5) = D(JY,5) + AL(K)*AL(L)*av(2,l)
              ENDIF
             END DO
            ENDIF
          END DO
        END DO

C Multiple scattering
        IF (IMSCAT.NE.0) THEN
          IF (OUTPAR(5).NE.0.) THEN
c  PBETAI2 is 1/(p*beta)**2
            xinv2=(OUTPAR(5)/2.997E-4)**2
            PBETAI2 = xinv2*(1+xmass**2*xinv2)
          ELSE
            PBETAI2 = 0.
          ENDIF
        ELSE
          PBETAI2 = 0.
        ENDIF

C Covariance matrix
        CALL VZERO (ER2, 2*NHITS*2*NHITS*2)
        DO J = 1, NPOINTS
          jx = j
          jy = j+NPOINTS
          DO L = 1, J
            lx = l
            ly = l + NPOINTS
            IF (j.EQ.l) THEN
              ER2(jx,lx) = ER2(jx,lx) + Exm(1,jx)**2
              ER2(jy,ly) = ER2(jy,ly) + Exm(2,jx)**2
            ENDIF

C Multiple scattering
            IF (IMSCAT.NE.0) THEN
              DO M = 2, MIN(J,L)-1
                AL_JM = 0.
                DO K = M+1, J
                  AL_JM = AL_JM + AL(K)
                END DO
                AL_LM = 0.
                DO K = M+1, L
                  AL_LM = AL_LM + AL(K)
                END DO
C    add radiation lengths in case of missing measurements
                WL = 0.
                DO IZ = IPLANEZ(M-1)+1, IPLANEZ(M)
                  WL = WL + WLEN(IZ)
                END DO
                ER2(jx,lx) = ER2(jx,lx) 
     +            + AL_JM*AL_LM*0.0118**2*PBETAI2*WL/ABSUZ(M)
                ER2(jy,ly) = ER2(jy,ly) 
     +            + AL_JM*AL_LM*0.0118**2*PBETAI2*WL/ABSUZ(M)
              END DO
            ENDIF
            ER2(lx,jx) = ER2(jx,lx) 
            ER2(ly,jy) = ER2(jy,ly) 
          END DO
        END DO

C Inverse of the covariance matrix S=V**{-1}
        CALL DINV (2*NPOINTS, ER2, 2*NHITS, IDAUX, IFAIL)
        IF (IFAIL.LT.0) THEN
          PRINT*,' ER2 Covariance Matrix is Singular !!!'
          outpar(5)=0
          GOTO 999
        ENDIF

C F*S_x*x + G*S_y*y
        DO J = 1, 5
          DX(J) = 0.D0
          DO L = 1, NPOINTS
           DO M = 1, NPOINTS
            DX(J) = DX(J) + D(L,J)*ER2(L,M)*XM(1,M)
            DX(J) = DX(J) + D(L+NPOINTS,J)*ER2(L+NPOINTS,M+NPOINTS)
     >               *XM(2,M)
           END DO
          END DO
        END DO

C (F*S_x*F + G*S_y*G)
        IF (magstat.gt.0) THEN
          IDIM_EFF = 5
          CALL VZERO (DD, 2*5*5)
        ELSE
          IDIM_EFF = 4
          CALL VZERO (DD, 2*5*5)
          DD(5,5) = 1.D0
        ENDIF
        DO J = 1, IDIM_EFF
          DO K = 1, IDIM_EFF
            DO L = 1, NPOINTS
             DO M = 1, NPOINTS
              DD(J,K) = DD(J,K) + D(L,J)*ER2(L,M)*D(M,K)
              DD(J,K) = DD(J,K) + D(L+NPOINTS,J)
     >                 *ER2(L+NPOINTS,M+NPOINTS)*D(M+NPOINTS,K)
             END DO
            END DO
          END DO
c        PRINT*,' J, DD= ', J, (DD(J,i),i=1,5)
        END DO
        
C (F*S_x*F + G*S_y*G)**{-1}
        CALL DINV (5, DD, 5, IDAUX, IFAIL)
        IF (IFAIL.LT.0) THEN
          PRINT*,' DD Covariance Matrix is Singular !!!'
          outpar(5)=0
          GOTO 999
        ENDIF

C Solution
        DO K = 1, IDIM_EFF
          OUTPAR(K) = 0.
          DO I = 1, IDIM_EFF
            OUTPAR(K) = OUTPAR(K) + DD(K,I)*DX(I)
          END DO
        END DO
c        PRINT*,' OUTpar= ', OUTpar

C Convergence criteria
        IF (magstat.LE.0) GOTO 998
        deltatol = abs(outpar5-outpar(5))/2.997e-4
        deltaprec = deltatol*(outpar(5)/2.997e-4)**2
        IF (     IMSCAT.EQ.0 
     >      .OR. DELTATOL.LT.tol
     >      .OR. DELTAPREC.LT.prec
     >                             ) THEN
          GOTO 998
        ENDIF

30    continue
C Too many iterations and no convergence
      write(*,*)'tkfita-Error-TooManyIterations'
C Final Chi2
      NDOF = 2*NPOINTS-IDIM_EFF
      CHI2 = 0.
      DO L = 1, NPOINTS
        XL = XM(1,L)
        YL = XM(2,L)
        DO K = 1, 5
          XL = XL - D(L,K)*OUTPAR(K)
          YL = YL - D(L+NPOINTS,K)*OUTPAR(K)
        END DO
        DO J = 1, NPOINTS
          XJ = XM(1,J)
          YJ = XM(2,J)
          DO K = 1, 5
            XJ = XJ - D(J,K)*OUTPAR(K)
            YJ = YJ - D(J+NPOINTS,K)*OUTPAR(K)
          END DO
          CHI2 = CHI2 + XL*SNGL(ER2(L,J))*XJ
     >                + YL*SNGL(ER2(L+NPOINTS,J+NPOINTS))*YJ
        END DO
      END DO
C Convert rigidity to GV
      OUTPAR(5)=OUTPAR(5)/2.997E-4
      OUTPAR(6)=SQRT(DD(5,5))/2.997E-4
      ierr=2
      goto 999

998   continue
C Everything OK
c      write(*,*)'qq ',iter
C Final Chi2
      NDOF = 2*NPOINTS-IDIM_EFF
      CHI2 = 0.
      DO L = 1, NPOINTS
        XL = XM(1,L)
        YL = XM(2,L)
        DO K = 1, IDIM_EFF
          XL = XL - D(L,K)*OUTPAR(K)
          YL = YL - D(L+NPOINTS,K)*OUTPAR(K)
        END DO
        DO J = 1, NPOINTS
          XJ = XM(1,J)
          YJ = XM(2,J)
          DO K = 1, IDIM_EFF
            XJ = XJ - D(J,K)*OUTPAR(K)
            YJ = YJ - D(J+NPOINTS,K)*OUTPAR(K)
          END DO
          CHI2 = CHI2 + XL*SNGL(ER2(L,J))*XJ
     >                + YL*SNGL(ER2(L+NPOINTS,J+NPOINTS))*YJ
        END DO
      END DO
C Convert rigidity to GV
      IF (magstat.GT.0) THEN
            OUTPAR(5)=OUTPAR(5)/2.997E-4
            OUTPAR(6)=SQRT(DD(5,5))/2.997E-4
      ELSE 
            OUTPAR(5)=0.
            OUTPAR(6)=0.
      ENDIF
      IERR = 0

999   CONTINUE
*
      END
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      SUBROUTINE PATH_INTEGRAL (P1, P2, axx, av)
************************************************************
      REAL P1(3), P2(3), alpha, axx(3), av(3)
      REAL X(3), U(3), B(3)
*
      PARAMETER (NTOT=20)
*
***---------------------------------------------------------------
*
      CALL VZERO (axx, 3)
      CALL VZERO (av, 3)

      UNORM = (P2(1)-P1(1))**2+(P2(2)-P1(2))**2+(P2(3)-P1(3))**2
      IF (UNORM.LE.0.) GOTO 999
      UNORM = sqrt(UNORM)

c      print*,' P1= ', P1
c      print*,' P2= ', P2

      do l = 0, NTOT
        alpha = float(l)/ntot

        if (l.eq.0.or.l.eq.ntot) then
           fact = 1.
        else if (mod(l,2).eq.1) then
           fact = 4.
        else
           fact = 2.
        endif

        do i = 1, 3
          X(i) = P1(i) + alpha*(P2(i)-P1(i))
          U(i) = (P2(i)-P1(i))/UNORM
        end do
c        print*, ' l, X, U= ', l, X, U

        CALL GUFLD (X, B)
c        print*, ' l, b= ', l, b
        axx(1) = axx(1) + fact*
     >           (1.-alpha)*(u(2)*b(3)-u(3)*b(2))
        axx(2) = axx(2) + fact*
     >           (1.-alpha)*(u(3)*b(1)-u(1)*b(3))
        axx(3) = axx(3) + fact*
     >           (1.-alpha)*(u(1)*b(2)-u(2)*b(1))
        av(1) = av(1) + fact*(u(2)*b(3)-u(3)*b(2))
        av(2) = av(2) + fact*(u(3)*b(1)-u(1)*b(3))
        av(3) = av(3) + fact*(u(1)*b(2)-u(2)*b(1))
      end do

      axx(1) = axx(1)/3./NTOT
      axx(2) = axx(2)/3./NTOT
      axx(3) = axx(3)/3./NTOT
      av(1) = av(1)/3./NTOT
      av(2) = av(2)/3./NTOT
      av(3) = av(3)/3./NTOT

999   CONTINUE
      
*                                  END PATH_INTEGRAL
      END
*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      SUBROUTINE INVERTMATRIX (A, N0, N1, IFAIL)
*
* a) Simplified calling sequence to invert a square matrix. 
*    It calls DINV.
* b) If A is not symmetric and the subroutine is called from C++ code, 
*    one has to interchange rows and columns before calling it.
*
* Input: 
*        A              double precision array containing the A matrix
*        N0             dimension of A is N0 x N0 in the definition
*        N1             We want the N1 x N1 submatrix of A
*        IFAIL          <0 in case of problems (singularities,...)
*
*                                         J. Alcaraz (2003)
*******************************************************************
      IMPLICIT NONE
      INTEGER IFAIL, N0, N1
      DOUBLE PRECISION A(N0*N0)
      INTEGER IDAUX(2*N0)
*
***---------------------------------------------------------------
*
      CALL DINV (N0, A, N1, IDAUX, IFAIL)

999   CONTINUE
*                                            END INVERTMATRIX
      END
