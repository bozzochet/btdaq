*  $Id: egzav.F,v 1.1 2008/09/11 17:19:22 zuccon Exp $
      SUBROUTINE CMINIT
*     =================================================================
*
************************************************************************
*                                                                      *
*     Routine to initialize the Cosmic Muon Package                    *
*                                                                      *
*       Input :  RADIUS - radius of the spherical surface on which     *
*                         the muons are generated                      *
*                EMIN - minimum energy of the muon (both for 1st and   *
*                                                   2nd pass)          *
*                ZMIN - maximum zenith angle (in degrees) only for     *
*                         2nd pass                                     *
*                CHOPT - 'G' - Generate - 1st pass                     *
*                        'T' - Track - 2nd pass                        *
*                        'N' - Generate without weight (weight = 1)    *
*                        'S' - Generate muons for SIGEL3 Standard input*
*                                                                      *
*       Output: GENMU - true if the muon should be generated           *
*               TRAMU   true                       tracked             *
*                                                                      *
************************************************************************
*
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*
      CHARACTER CHOP,CHOP1
      CHARACTER *2 CHOPT
C
      LOGICAL GENMU, TRAMU
*
***   Load constants to be used in the program
*
      EMININ=0.1
      EMINOU=0.1
      ZMIN=360
      RADIUS=300
      CALL INCONS
*
***   Decode character option
*
       
      STAND = .FALSE.
      GENER = .FALSE.
      TRACK = .FALSE.
      READ = .FALSE.
      WRITE1 = .FALSE.
      WRITE2 = .FALSE.
      NOWEGH = .TRUE.
      IWRIT = 0
      CHOPT='GN'
      LOPT = LEN(CHOPT)
      IF (LOPT.GE.1) THEN
         DO 1 I=1,LOPT
         CHOP = CHOPT(I:I)
         IF (CHOP.EQ.'G') GENER = .TRUE.
         IF (CHOP.EQ.'T') TRACK = .TRUE.
         IF (CHOP.EQ.'R') READ = .TRUE.
         IF (CHOP.EQ.'N') NOWEGH = .TRUE.
         IF (CHOP.EQ.'W') THEN
            IF (I.GE.LOPT) THEN
               PRINT 9931
               STOP
            ENDIF
            CHOP1 = CHOPT(I+1:I+1)
            IF (CHOP1.EQ.'1') THEN
               IWRIT = 1
               WRITE1 = .TRUE.
            ELSE IF (CHOP1.EQ.'2') THEN
               WRITE2 = .TRUE.
               IWRIT = 2
            ELSE IF (CHOP1.EQ.'3') THEN
               WRITE1 = .TRUE.
               WRITE2 = .TRUE.
               IWRIT = 3
            ELSE
               PRINT 9931
9931           FORMAT(/2X,'W-OPTION IS NOT FOLLOWED BY',
     F                ' CHARACTER 1 OR 2 OR 3 - STOP'/)
               STOP
            ENDIF
         ENDIF
         IF (CHOP.EQ.'S') THEN
            STAND = .TRUE.
         ENDIF
1        CONTINUE
      ENDIF
      GENMU = GENER
      TRAMU = TRACK
*
**    Cross-check options
*
      IF (WRITE1.AND..NOT.GENER) THEN
         PRINT 9932
9932  FORMAT(/2X,'YOU WANT WRITE OUT 1ST PASS MUONS WITHOUT',
     F            ' GENERATING THEM - STOP'/)
         STOP
      ENDIF
      IF ((WRITE2.AND..NOT.TRACK).OR.(STAND.AND..NOT.TRACK)) THEN
         PRINT 9933
9933  FORMAT(/2X,'YOU WANT WRITE OUT 2ND PASS MUONS WITHOUT',
     F            ' TRACKING THEM - STOP'/)
         STOP
      ENDIF
      IF (READ.AND.GENER) THEN
         PRINT 9934
9934  FORMAT(/2X,'YOU WANT GENERATE 1ST PASS MUONS AND AT THE',
     F            ' SAME TIME READ THEM FROM FILE - STOP'/)
         STOP
      ENDIF
      IF (READ.AND..NOT.TRACK) THEN
         PRINT 9935
9935  FORMAT(/2X,'YOU WANT READ 1ST PASS MUONS BUT ',
     F            'DO NOT WANT TO TRACK THEM - STOP'/)
         STOP
      ENDIF
*
***   Open files
*
*      LUN1 = LUNI1
*      LUN2 = LUNI2
*      IF (STAND) CALL INIGEN (NAME2)
*      LUN2 = LUFZL3
*      IF (WRITE1)
*     I   OPEN(UNIT=LUN1,FILE=NAME1,STATUS='UNKNOWN')
*      IF (WRITE2)
*     I   OPEN(UNIT=LUN2,FILE=NAME2,STATUS='UNKNOWN')
*      IF (READ)
*     I   OPEN(UNIT=LUN1,FILE=NAME1,STATUS='OLD')
*
***   Run conditions
*
      EMINRU = EMININ
      EMNARU = EMINOU
      RSURRU = RADIUS
      ANGCRU = ZMIN
*
***   Prepare flux calculation
*
      IF (GENER) THEN
         CALL INFLUX
         CALL INIFLU
      ENDIF
*
***   Read header and Print initial conditions
*
      IF (GENER) CALL INITEG
      IF (TRACK)  THEN
         CALL INITET
         NACCRU = 0
         MGENST = 0
         MANGST = 0
         MENEST = 0
         MDETST = 0
      ENDIF
*
***   Book histograms to monitor
*
      IF (GENER) CALL INITHG
      IF (TRACK) CALL INITHT
*
      END
      SUBROUTINE CMGENE(PMU,TheMU,PHIMU,CHARMU,xmu,ymu,zmu)
*     =================================================================
*
************************************************************************
*                                                                      *
*     Routine to Generate Cosmic Muons                                 *
*                                                                      *
*        Output : PMU - momentum of the muon                           *
*                 COSTMU - cosine of the muon zenith angle             *
*                 PHIMU - azimuthal angle of the muon                  *
*                 CHARMU - charge of the muon                          *
*                 XMU,YMU,ZMU - coordinates of the muon on the sphere  *
*                               ZMU is pointing to the vertical        *
*                 WEIMU - weight of the muon                           *
*                                                                      *
************************************************************************
C
C --- Generation of cosmic muons according to dN/dE/dO = dN/dE.dN/dO
C     where E is the energy, O is the solid angle: dO = d(cos(TH))dFI
C     TH is the zenith angle, i.e. angle w.r.t. vertical direction
C     FI is the azimuth
C --- dN/dE is taken from the measured vertical flux (see e.g.
C     O.C.Allkofer et al, Phys.Letters 36B (1971) 425)
C --- dN/dO is approximated by - (cos(TH))**gam(E), where gam(E) is
C     a decreasing funtion of E (2.0 at 10 Gev, -0.7 at 1000 GeV)
C
C --- This generation is fairly close to the flux parametrized by
C     A.Dar, Phys.Rev.Letters 51 (1983) 227. The difference is taken
C     into account by the associated weight-factor
C
C --- The above parametrization (A.Dar) is close to the measured
C     cosmic muon spectrum within 10-20 % at small TH but it may
C     differ from it considerably at high TH (even 30-50%). However,
C     at high zenith angles different measurements differ about the
C     same amount (see e.g. O.C.Allkofer et al. Cosmic Rays on
C     Earth, Fachinformations-Zentrum GMBH Karlsruhe, ISSN 0344-
C     8401, 1984, Nr 25-1) so the uncertainty of the parametrisation
C     does not exceed that of the experimental knowledge.
C
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2

      PARAMETER (NARG=2)
      DIMENSION WG(NARG),NA(NARG),AWG(100)
      DOUBLE PRECISION XLNOR,YLNOR,ZLNOR
*   Data to decide which charge of the muon
      DATA CHMIN /0.4386/
*   Data to be used for generation without weight
      DATA WMAX/2./
      data numbev /0/
*
***   Event number
*
      NUMBEV = numbev+1
*
***   Generate azimuthal angle  FIMU
*
 11   CONTINUE
         FIMU = TWOPI * RNDM(Q)
         PHIDEG = FIMU*DEGRAD
*
***   Generate energy EMU
*

         R = RNDM(Q)
*
**    Energy is greater than the highest grid-value
*
      IF (R.LE.FLUNMU(NFLUMU)) THEN
         ALF = ENERMU(NFLUMU)*FLUDMU(NFLUMU)/FLUIMU(NFLUMU)+1.
         ALF1 = 1./(1.-ALF)
         DR = 1. - R / FLUNMU(NFLUMU)
         EMU = ENERMU(NFLUMU)*DR**ALF1
         GOTO 6
      ENDIF
*
         DO 1 I = 2,NFLUMU
         J = NFLUMU - I + 1
         DR = FLUNMU(J) - R
         IF (DR.GT.0.) THEN
            GOTO 2
         ELSEIF (DR.EQ.0.) THEN
            GOTO 4
         ENDIF
1        CONTINUE
2        CONTINUE
         DR = DR/(FLUNMU(J)-FLUNMU(J+1))
         ALF1 = 1. + ALOG(FLUDMU(J)/FLUDMU(J+1))/
     .               ALOG(ENERMU(J)/ENERMU(J+1))
         EN1 = ENERMU(J+1)**ALF1
         EN2 = ENERMU( J )**ALF1
         EMU = (EN1-(EN1-EN2)*DR)**(1./ALF1)
         GOTO 6
4     CONTINUE
*
**    Energy is at the grid-point
*
      EMU = ENERMU(J)
6     CONTINUE
*
***   Generate zenith angle THMU
*
**    Linear interpolation of the logarithm of the energy
*        to find the exponent
*
      AEMU = ALOG10(EMU)
      CALL LININT(XGAMM,YGAMM,NGAMM,AEMU,GAMMA)
C      CALL HFILL(3,GAMMA,0.,1.)
      GAM1 = 1.+GAMMA
      IF (GAM1.LT.0.1) GAM1 = 0.1
61    CONTINUE
      CSTH = RNDM(Q)**(1./GAM1)
      THMU = ACOS(CSTH)
C      CALL HFILL(2,CSTH,0.,1.)
*
***   Weight to correct zenith angle distribution
*
      NA(1) = NTH
      NA(2) = NGAMM
      WG(1) = CSTH
      WG(2) = AEMU
      CALL UCOPY(COSTH(1),AWG(1),NTH)
      CALL UCOPY(XGAMM(1),AWG(NTH+1),NGAMM)
      WEIGHT = FINT(NARG,WG,NA,AWG,WEIGMU)
C MWG ((
      IF(WEIGHT.LT.0.0)THEN
        PRINT*,'CMGENE> Warning: FINT returns negative weight:'
        PRINT*,'  FINT(',WG,')=',WEIGHT
        GOTO 11
      ENDIF
C MWG ))
C      CALL HFILL(5,WEIGHT,0.,1.)
      IF (NOWEGH) THEN
         WRAND = WMAX*RNDM(Q)
         IF (WRAND.GT.WEIGHT) GOTO 61
         WEIGHT = 1.
      ENDIF
      WEIMU = WEIGHT
c      CALL HFILL(101,EMU,0.,WEIGHT)
c      CALL HFILL(102,CSTH,0.,WEIGHT)
       CALL HFILL(110,CSTH,EMU,WEIGHT)
c      CALL HFILL(109,CSTH,PHIDEG,1.)
*
***   Now generate the charge of the muon
*        It is assumed, that the +/- ratio is constant = 1.28
*
      NCHMU = 1
      IF (RNDM(Q).LT.CHMIN) NCHMU = -1
*
***   Choose the position of the spherical surface element.
*            It is chosen according to the flux-factor,
*            i.e. the probability of its position is
*            proportional to the cosine of the angle between
*            the muon direction and the normal of the surface.
*            Moreover, the surface element should be located
*            at the upper hemisphere
*
**    Theta,phi in the muon-system (i.e. z-axis along the muon)
*
7     CONTINUE
      FINOR = TWOPI*RNDM(Q)
      CSTHNO = SQRT(RNDM(Q))
      THNOR = ACOS(CSTHNO)
*
**    Transform to the labor (where muon is given by THMU,FIMU)
*
      ZNOR = CSTHNO
      XNOR = SIN(THNOR)
      YNOR = XNOR*SIN(FINOR)
      XNOR = XNOR*COS(FINOR)
*
      SFMU = SIN(FIMU)
      CFMU = COS(FIMU)
      STMU = SIN(THMU)
      CTMU = CSTH
*
**    Rotate by two Euler angles: 1st around x-axis by THMU
*                                 2nd    "   z-axis by PI/2-FIMU
*
      XLNOR =   XNOR*SFMU + (   YNOR*CTMU + ZNOR*STMU)*CFMU
      YLNOR = - XNOR*CFMU + (   YNOR*CTMU + ZNOR*STMU)*SFMU
      ZLNOR =                 - YNOR*STMU + ZNOR*CTMU
      COSUMU = XLNOR*STMU*CFMU+YLNOR*STMU*SFMU+ZLNOR*CTMU
      IF (ABS(COSUMU-CSTHNO).GT.1.E-2) PRINT 1526,COSUMU,CSTHNO
1526  FORMAT(/2X,'... TRANSFORMATION TO LAB NOT CORRECT... ',
     F       2F12.5/)
c*
c***   Now generate starting coordinates of the muon
c*
      XMU = RSURRU*XLNOR
      YMU = RSURRU*YLNOR
      ZMU = RSURRU*ZLNOR
c      CALL HFILL(201,XMU,YMU,1.)
c      CALL HFILL(202,XMU,ZMU,1.)
c      CALL HFILL(203,YMU,ZMU,1.)
*
***   Muon-vector:
*
      PMU = EMU
      TheMU =  acos(CSTH)
      PHIMU = FIMU
      CHARMU = NCHMU
*
***   Write out the muon (if requested)
*
*      IF (WRITE1)
*     W    WRITE(LUN1,100) PMU,COSTMU,PHIMU,CHARMU,XMU,YMU,ZMU,WEIMU
100   FORMAT(3E14.6,F5.1,4E14.6)
*
      END
      SUBROUTINE CMGENE2(PMU,gamma,charmu)
*     =================================================================
*
*
*      
*
************************************************************************
*                                                                      *
*     Routine to Generate Cosmic Muons                                 *
*                                                                      *
*        Output : PMU - momentum of the muon                           *
*                 COSTMU - cosine of the muon zenith angle             *
*                 PHIMU - azimuthal angle of the muon                  *
*                 CHARMU - charge of the muon                          *
*                 XMU,YMU,ZMU - coordinates of the muon on the sphere  *
*                               ZMU is pointing to the vertical        *
*                 WEIMU - weight of the muon                           *
*                                                                      *
************************************************************************
C
C --- Generation of cosmic muons according to dN/dE/dO = dN/dE.dN/dO
C     where E is the energy, O is the solid angle: dO = d(cos(TH))dFI
C     TH is the zenith angle, i.e. angle w.r.t. vertical direction
C     FI is the azimuth
C --- dN/dE is taken from the measured vertical flux (see e.g.
C     O.C.Allkofer et al, Phys.Letters 36B (1971) 425)
C --- dN/dO is approximated by - (cos(TH))**gam(E), where gam(E) is
C     a decreasing funtion of E (2.0 at 10 Gev, -0.7 at 1000 GeV)
C
C --- This generation is fairly close to the flux parametrized by
C     A.Dar, Phys.Rev.Letters 51 (1983) 227. The difference is taken
C     into account by the associated weight-factor
C
C --- The above parametrization (A.Dar) is close to the measured
C     cosmic muon spectrum within 10-20 % at small TH but it may
C     differ from it considerably at high TH (even 30-50%). However,
C     at high zenith angles different measurements differ about the
C     same amount (see e.g. O.C.Allkofer et al. Cosmic Rays on
C     Earth, Fachinformations-Zentrum GMBH Karlsruhe, ISSN 0344-
C     8401, 1984, Nr 25-1) so the uncertainty of the parametrisation
C     does not exceed that of the experimental knowledge.
C
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2

      PARAMETER (NARG=2)
      DIMENSION WG(NARG),NA(NARG),AWG(100)
      DOUBLE PRECISION XLNOR,YLNOR,ZLNOR
*   Data to decide which charge of the muon
      DATA CHMIN /0.4386/
*   Data to be used for generation without weight
      DATA WMAX/2./
*
***   Event number
*
      NUMBEV = numbev+1
*
*
***   Generate energy EMU
*

         R = RNDM(Q)
*
**    Energy is greater than the highest grid-value
*
      IF (R.LE.FLUNMU(NFLUMU)) THEN
         ALF = ENERMU(NFLUMU)*FLUDMU(NFLUMU)/FLUIMU(NFLUMU)+1.
         ALF1 = 1./(1.-ALF)
         DR = 1. - R / FLUNMU(NFLUMU)
         EMU = ENERMU(NFLUMU)*DR**ALF1
         GOTO 6
      ENDIF
*
         DO 1 I = 2,NFLUMU
         J = NFLUMU - I + 1
         DR = FLUNMU(J) - R
         IF (DR.GT.0.) THEN
            GOTO 2
         ELSEIF (DR.EQ.0.) THEN
            GOTO 4
         ENDIF
1        CONTINUE
2        CONTINUE
         DR = DR/(FLUNMU(J)-FLUNMU(J+1))
         ALF1 = 1. + ALOG(FLUDMU(J)/FLUDMU(J+1))/
     .               ALOG(ENERMU(J)/ENERMU(J+1))
         EN1 = ENERMU(J+1)**ALF1
         EN2 = ENERMU( J )**ALF1
         EMU = (EN1-(EN1-EN2)*DR)**(1./ALF1)
         GOTO 6
4     CONTINUE
*
**    Energy is at the grid-point
*
      EMU = ENERMU(J)
6     CONTINUE
*
**    Linear interpolation of the logarithm of the energy
*        to find the exponent
*
      AEMU = ALOG10(EMU)
      CALL LININT(XGAMM,YGAMM,NGAMM,AEMU,GAMMA)
*
***   Now generate the charge of the muon
*        It is assumed, that the +/- ratio is constant = 1.28
*
      NCHMU = 1
      IF (RNDM(Q).LT.CHMIN) NCHMU = -1
      CHARMU = NCHMU
      PMU=EMU
*
      END
CCDECK,DNDPTH.
      FUNCTION DNDPTH(P,THETA)
*     ========================
*
***   P (momentum) given in GeV, THETA (zenith angle) given in
*       rad the function calculates the differential distribution
*       of the cosmic muons at sea-level. The distribution is
*       given in 1./(GeV*cm**2*sec*sterad)
*     This parametrization is due to A.Dar, Phys.Rev.Letters
*       51 (1983) 227. The computer code is provided by B.Gindl
*       and R.Mount
*
      REAL G1(6),G2(6),B(4),SQMASS(4),
     *     CTAU(4),AMASS(4)
      EXTERNAL AKM
      DATA G1/0.04190,0.03014,0.007354,0.002633,0.2028,0.1147/
      DATA G2/0.00655,-0.01019,-0.006456,-0.003179,0.2801,-0.0986/
      DATA CTAU/7.804, 7.804, 3.709, 3.709/
      DATA SQMASS/2*0.019479,2*0.24371/
      DATA AMASS/2*0.139567, 2*0.493669/
      DATA B/1.,1.,0.632,0.632/
      DATA ALPHA0/2.06E-04/, AMASSM/0.10566/
      DATA ALAMB0/1200/,H0/6300/,CTAUMU/658.68/
      SQMASM=AMASSM**2
      ALAMBF=10300/COS(THETA)
        ENERGY=SQRT(P**2+SQMASM)
        EBAR=ENERGY+ALPHA0*(ALAMBF-ALAMB0)/2.
        E0=ENERGY+ALPHA0*(ALAMBF-ALAMB0)
        GAMMU = COS(THETA)*CTAUMU/(AMASSM*H0)
        SMU = (ALAMB0/ALAMBF)**(1.0/(GAMMU*EBAR))
        DFMU=0.
        DO 30 M=1,4
          ALPHAM=SQMASS(M)/(SQMASS(M)-SQMASM)
          GM=G1(M)+G2(M)/SQRT(E0)
          GN=G1(5)+G1(6)+(G2(5)+G2(6))/SQRT(E0)
          GMAT=GM/(1.-GN)
          GMAT=GMAT*1.15
          GAMMAM=COS(THETA)*CTAU(M)/(AMASS(M)*H0)
          BE0=SQMASS(M)/SQMASM*E0
          DFMU=DFMU+GMAT*B(M)*ALPHAM*SMU*
     &    (AKM(GAMMAM,E0)-AKM(GAMMAM,BE0))
   30   CONTINUE
        DNDPTH = DFMU
      END
*
      FUNCTION AKM(GAMMAM,E)
      EXPO=2.67
      C=1.6
      AKM=C*E**(-EXPO)/(EXPO+(EXPO+1)*GAMMAM*E)
      END
CCDECK,ENDRUN.
         SUBROUTINE CMEND
C        =================
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
*
*     Energy points where the 0 deg zenith angle data were obtained
*
      DATA ENER/0.2,0.4,0.8,1.0,1.5,2.0,3.0,5.0,7.0,10.0,
     &15.,20.,30.,50.,70.,100.,150.,200.,300.,500.,700.,1000./
*
***   0 deg zenith angle differential xsections obtained by Allkofer
*        et al. Phys. Letters 36B (1971) 425
*
      DATA DDIF/0.373E-02,0.372E-02,0.310E-02,0.279E-02,0.214E-02,
     &0.167E-02,0.106E-02,0.497E-03,0.273E-03,0.133E-03,0.540E-04,
     &0.270E-04,0.959E-05,0.236E-05,0.892E-06,0.304E-06,0.851E-07,
     &0.335E-07,0.870E-08,0.152E-08,0.471E-09,0.134E-09/
*
*     INTEGRATION OF THE DAR PARAMETRIZATION OVER THE ANGLE
*     IN COS(TH)-BIN OF  0.001
*
      DATA DARIN1/
     D  0.4340E-02, 0.4551E-02, 0.4516E-02, 0.4363E-02, 0.3840E-02,
     D  0.3292E-02, 0.2388E-02, 0.1328E-02, 0.8104E-03, 0.4415E-03,
     D  0.2018E-03, 0.1099E-03, 0.4380E-04, 0.1252E-04, 0.5220E-05,
     D  0.1986E-05, 0.6332E-06, 0.2741E-06, 0.8152E-07, 0.1688E-07,
     D  0.5835E-08, 0.1859E-08/
*
C
C ---  Print histogrammes, statistics, etc...
C
*
***   Calculate the time (in sec) to which the generated
*               number of muons corresponds
*
**    Flux (= Mu/(cm**2*sec)) corresponding to the minimum
*          energy as calculated by the MC integration
*
      XFIR = HIJ(110,1,NCTHAB-ICLOAB)
      XTOT = HSUM(110)+OVUSUM(110)
      XTOTC = HIJ(110,1,0)+HIJ(110,1,NCTHAB+1)
      DO 501 I = 1,NCTHAB
501   XTOTC = XTOTC + HIJ(110,1,I)
      IF (XFIR.GT.1.E-20) THEN
         FLUX = XTOT/XFIR*DLCTAB*DELEAB*FLUFAB
         FLUXC = XTOTC/XFIR*DLCTAB*FLUFAB
         DFLUX = SQRT(1./XFIR+1./XTOT)*FLUX
         DFLUXC = SQRT(1./XFIR+1./XTOTC)*FLUXC
C         PRINT 102,XTOT,XFIR,DLCTAB,DELEAB,FLUFAB,ICLOAB
102      FORMAT(/2X,'( XTOT,XFIR,DLCTAB,DELEAB,FLUFAB,ICLOAB',2E12.4,
     F        2F10.2,E12.4,I4,' )'/)
C         PRINT 101,FLUX,DFLUX,EMINRU
101      FORMAT(//2X,'CALCULATED TOTAL MU-FLUX ',
     F       'BY MC-INTEGRATING THE PARAMETRIZATION OF A.DAR'/
     F        6X,'FLUX = ',E12.4,' +/-',E12.4,' (CM**2*SEC)-1 FOR ',
     F        F6.1,' GEV MINIMUM ENERGY'/)
         E1 = EMINRU + DELEAB/2.
C         PRINT 1011,E1,FLUXC,DFLUXC,EMINRU
1011     FORMAT(//2X,'CALCULATED  MU-FLUX AT ',F6.1,' ENERGY ',
     F       'BY MC-INTEGRATING THE PARAMETRIZATION OF A.DAR'/
     F        6X,'FLUX = ',E12.4,' +/-',E12.4,' (CM**2*SEC)-1 FOR ',
     F        F6.1,' GEV MINIMUM ENERGY'/)
      PRINT 104,FLUIMU(1)
104   FORMAT(/2X,'INTERPOLATED TOTAL MU FLUX FROM DAR PARAMETRIZATION',
     F        E12.4/)
         FLUMI = FALKMU*TWOPI
         FLUM1 = FLUMI/2.
C         FLUMA = FLUMI/(HSTATI(3,1,'HIST',1)+1.)
C         PRINT 103,FLUMI,FLUM1,FLUMA
103      FORMAT(/22X,'FOR COMPARISON :'
     F          /2X,'INTEGRATED FLUX BY EXTRAPOLATING FROM THE',
     F           ' 0 DEG MEASUREMENT OF ALLKOFER ET AL.'
     F   /14X,E12.4,' (ISOTROPIC) ',
     F           E12.4,' (COS(THMU)**1 EMISSION)',E12.4,
     F   ' (COS(THMU)**AVER.GAM)'//)
      ELSE
C         PRINT 110
110   FORMAT(//2X,'NOT ENOUGH EVENTS GENERATED TO OBTAIN FLUX'//)
      ENDIF
*
**    Total intensity (in sec-1) corresponding to the volume and EMINRU
*        (2pi instead of 4pi, since muons are generated only in the
*         upward hemisphere !)
*
      FLUX = FLUIMU(1)
      TOTINT = TWOPI*RSURRU**2*FLUX
*
*********************************************************************
      TOTINT = TOTINT/2.
*
*     The total intensity should be devided by 2 since the effective
*        area of the hemisphere on which one integrates the flux is
*        n o t  2*pi*r**2   but only  pi*r**2, i.e. the area of the
*        circle which is perpendicular to the muons. (E.Nagy, Feb.96)
*********************************************************************
**    Elapsed time (in sec) corresponding to the generated
*        muons
*
      TIMERU = NUMBEV/TOTINT
      TIMHO = TIMERU/3600.
      PRINT 120, NUMBEV,RSURRU,EMINRU,TIMHO,TIMERU,TOTINT
120   FORMAT(//30X,'====== END OF GENERATION - STATISTICS ======'//
     F       2X,I10,' MUONS WERE GENERATED ON A SPHERICAL SURFACE OF',/
     F       ,' RADIUS',F7.2,' CM ABOVE',F7.2,'GEV ENERGY'//
     F       2X,'THIS NUMBER CORRESPONDS AT THE SEA-LEVEL',/
     F       ,' TO ',E15.7,' HOURS (',E15.7,' SEC) COLLECTING TIME',
     F       //,
     F       2X,'I.E. TO',E12.4,' MUON/SEC INTENSITY'//)
*
      END
CDECK,FLUINT.
         FUNCTION FLUINT(E1,E2,F1,F2)
C        ============================
C
C --- Given E1 and E2 lower and upper energy limits and F1 and F2 the
C        corresponding differential distribution dN/dE = a*E**-alf
C        at E1 and E2. Then FLUINT = Integral dN/dE from E1 to E2
C
         FLUINT = (E1*F1-E2*F2)/(-ALOG(F1/F2)/ALOG(E1/E2)-1.)
C
         RETURN
         END
CDECK,INCONS.
         SUBROUTINE INCONS
C        =================
C
C --- Initializes constants needed
C
C
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      PI=ATAN(1.)*4.
      TWOPI=2.0*PI
      PIBY2=PI/2.0
      DEGRAD=180./PI
      RADDEG=1./DEGRAD
C
         END
CDECK,INFLUX.
      SUBROUTINE INFLUX
*     =================
*
*     Routine determines the exponents YGAMM of the
*        cos(THMU)**YGAMM(EMU) generator distribution,
*        where THMU is the muon zenith angle and EMU is
*        its energy (= momentum), by fitting the
*        cos(THMU)**YGAMM(EMU) function to the parametrization
*        of A.Dar (see F/N DNDPTH). The routine also
*        calculates the weight-factors WEIGMU(THMU,EMU), which
*        take into account the deviation of the generation
*        (by cos(THMU)**YGAMM(EMU)) from the above mentioned para-
*        metrization.
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
      PARAMETER (NP=2,NP1=3)
      DIMENSION ETHET(NTH,22),IENER(NGAMM),ERRETH(NTH)
      DIMENSION FCOSTH(NTH)
      DIMENSION SIGTH(NP),COVTH(NP1),STTH(NP),PMITH(NP),PMATH(NP)
      DOUBLE PRECISION PARTH,FUNTH
      COMMON/MYPAR/PARTH(NP)
      EXTERNAL FUNTH
      DATA IENER/8,10,12,14,16,18,19,20,21,22/
      DATA VMX/0./,RELERR/0.01/
*
*
*     Energy points where the 0 deg zenith angle data were obtained
*
      DATA ENER/0.2,0.4,0.8,1.0,1.5,2.0,3.0,5.0,7.0,10.0,
     &15.,20.,30.,50.,70.,100.,150.,200.,300.,500.,700.,1000./
*
***   0 deg zenith angle differential xsections obtained by Allkofer
*        et al. Phys. Letters 36B (1971) 425
*
      DATA DDIF/0.373E-02,0.372E-02,0.310E-02,0.279E-02,0.214E-02,
     &0.167E-02,0.106E-02,0.497E-03,0.273E-03,0.133E-03,0.540E-04,
     &0.270E-04,0.959E-05,0.236E-05,0.892E-06,0.304E-06,0.851E-07,
     &0.335E-07,0.870E-08,0.152E-08,0.471E-09,0.134E-09/
*
*     INTEGRATION OF THE DAR PARAMETRIZATION OVER THE ANGLE
*     IN COS(TH)-BIN OF  0.001
*
      DATA DARIN1/
     D  0.4340E-02, 0.4551E-02, 0.4516E-02, 0.4363E-02, 0.3840E-02,
     D  0.3292E-02, 0.2388E-02, 0.1328E-02, 0.8104E-03, 0.4415E-03,
     D  0.2018E-03, 0.1099E-03, 0.4380E-04, 0.1252E-04, 0.5220E-05,
     D  0.1986E-05, 0.6332E-06, 0.2741E-06, 0.8152E-07, 0.1688E-07,
     D  0.5835E-08, 0.1859E-08/
*
C        CALL HDELET(0)
*
* MWG:Initial parameter values
      PAR1=1.3
      PAR2=2.5
* MWG
*
      DO 60 ICTH = 1,NTH
      CTHETA = ICTH*0.1
      COSTH(ICTH) = CTHETA
      THETA = ACOS(CTHETA)
*
      DO 50 IE=1,22
      PMU = ENER(IE)
      DFMU = DNDPTH(PMU,THETA)
      ETHET(ICTH,IE) = DFMU
50    CONTINUE
60    CONTINUE
*
***   Now fit angular distribution to cos(th)**gam
*
**       angular distribution for fixed energy
*
      DO 70 I = 1,NGAMM
      PRINT 698,I
698   FORMAT(//2X,I2,'th round of fitting:',/,
     F         2X,'----------------------')
      IE = IENER(I)
       PRINT 699,ENER(IE)
699   FORMAT(/,2X,'ANGULAR DISTRIBUTION AND FIT TO COS(TH)**P FOR ',
     F       F10.1,'  ENERGY'/)
      CALL HBOOK1(IE,' F(COS(TH)) $',NTH,0.05,1.05,VMX)
      CALL HPAK(IE,ETHET(1,IE))
      DO 700 J = 1,NTH
700   ERRETH(J) = ETHET(J,IE)*RELERR
      CALL HPAKE(IE,ERRETH)
      CON = 1./ (HI(IE,10) + 0.000001) / 2.
      CALL HOPERA(IE,'+   ',IE,IE,CON,CON)
      CALL HIDOPT (IE, 'ERRO')
      CALL HIDOPT (IE, 'PERR')
* MWG PARTH(1) = 1.
* MWG PARTH(2) = 0.
* MWG:Take fit results of last step as start values for next step
      PARTH(1)=PAR1
      PARTH(2)=PAR2
* MWG
      STTH(1) = 0.5
      STTH(2) = 1.
      PMITH(1) = 0.
      PMATH(1) = 2.
      PMITH(2) = -3.
      PMATH(2) = 4.
       CALL HPRINT (IE)
      CALL HFITL(IE,FUNTH,NP,PARTH,CHI2,1,SIGTH,COVTH,STTH,PMITH,PMATH)
*     CALL HFITS(IE,FUNTH,NP,PARTH,CHI2,1,SIGTH)
      CALL HIDOPT (IE, 'PFUN')
      CALL HUNPAK(IE,FCOSTH,'HIST',1)
      PAR1 = PARTH(1)
      PAR2 = PARTH(2)
*
      XGAMM(I) = ALOG10(ENER(IE))
      YGAMM(I) = PAR2
*
      PRINT 702, PAR1,PAR2
702   FORMAT(2X,'NORMALISATION ',F10.3,'  EXPONENT ',F10.3)
      DO 701 J = 1, NTH
      X = J*0.1
      FITTH = FUNTH(X)
C      DEL = (FITTH-FCOSTH(J))/(FITTH+FCOSTH(J))*2.
      DEL = FCOSTH(J)/FITTH
      FCOSTH(J) = DEL
      WEIGMU(J,I) = DEL
701   CONTINUE
      CALL HDELET(IE)
      CALL HBOOK1(IE,' REL-DEV $',NTH,0.05,1.05,VMX)
      CALL HPAK(IE,FCOSTH)
       CALL HPRINT(IE)
      CALL HDELET(IE)
70    CONTINUE
*
      END
*
      DOUBLE PRECISION FUNCTION FUNTH(X)
      PARAMETER (NP=2)
      DOUBLE PRECISION PARTH
      COMMON/MYPAR/PARTH(NP)
      FUNTH = PARTH(1)*DEXP(PARTH(2)*ALOG(X))
      END
CDECK,INIFLU.
C
         SUBROUTINE INIFLU
C        =================
C

C --- Routine calculates the normalized integral fluxes to be used for
C     the energy generation above the minimum energy EMIN the user may
C     specify between 0.2 and 1000. GeV. The routine uses the vertical
C     cosmic muon spectrum of Allkofer et al (see S/R INTFLU).
C
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
*
*     Energy points where the 0 deg zenith angle data were obtained
*
      DATA ENER/0.2,0.4,0.8,1.0,1.5,2.0,3.0,5.0,7.0,10.0,
     &15.,20.,30.,50.,70.,100.,150.,200.,300.,500.,700.,1000./
*
***   0 deg zenith angle differential xsections obtained by Allkofer
*        et al. Phys. Letters 36B (1971) 425
*
      DATA DDIF/0.373E-02,0.372E-02,0.310E-02,0.279E-02,0.214E-02,
     &0.167E-02,0.106E-02,0.497E-03,0.273E-03,0.133E-03,0.540E-04,
     &0.270E-04,0.959E-05,0.236E-05,0.892E-06,0.304E-06,0.851E-07,
     &0.335E-07,0.870E-08,0.152E-08,0.471E-09,0.134E-09/
*
*     INTEGRATION OF THE DAR PARAMETRIZATION OVER THE ANGLE
*     IN COS(TH)-BIN OF  0.001
*
      DATA DARIN1/
     D  0.4340E-02, 0.4551E-02, 0.4516E-02, 0.4363E-02, 0.3840E-02,
     D  0.3292E-02, 0.2388E-02, 0.1328E-02, 0.8104E-03, 0.4415E-03,
     D  0.2018E-03, 0.1099E-03, 0.4380E-04, 0.1252E-04, 0.5220E-05,
     D  0.1986E-05, 0.6332E-06, 0.2741E-06, 0.8152E-07, 0.1688E-07,
     D  0.5835E-08, 0.1859E-08/
*
      DLCTAB = 0.05
      DELEAB = 1.
      NENEAB = 50
      NCTHAB = 10
*
      EMIN = EMINRU
*
***   Book histogram to calculate the absolute flux-value
*
      VMX = 0.0
      EMAX = EMINRU + NENEAB*DELEAB
      CTHMN = 1. - NCTHAB*DLCTAB
      CALL HBOOK2(110,' EMU.VS.COS(THMU) $',
     H            NCTHAB,CTHMN,1.,NENEAB,EMINRU,EMAX,VMX)
      CALL HIDOPT(110,'TABL')
*
**    Calculate the differential xsection d2N/(dE.dcos(THMU)) in the
*         1st E and cos(THMU) bin
      E1 = EMINRU + DELEAB/2.
*
**    Evaluate the xsection around cos(th) = 0.7, i.e. where the
*        weight is the closest to 1.
*
      CLOSM = 1.E+10
      DO 10 I = 1,NCTHAB
      NN = I - 1
      CTHL = 1. - DLCTAB/2.-NN*DLCTAB
      CLOS = ABS(CTHL-0.7)
      IF (CLOS.LT.CLOSM) THEN
         CTHLM = CTHL
         ICLOAB = NN
         CLOSM = CLOS
      ENDIF
10    CONTINUE
      THL = ACOS(CTHLM)
      FLUFAB = TWOPI*DNDPTH(E1,THL)
C
C ::: Define the energy points for the generation (ENERMU,FLUDMU,FLUIMU
C     being the energy points, the corresponding differential and nor-
C     malized integral flux (probabilities)
*
***   Integrate the energy flux
*
      CALL INTFLU
*
***   First point
*
**    Interpolate the energy point
*
      DO 1 I = 1,NENER
      IF (EMIN.LT.ENER(I)) THEN
         GOTO  2
      ELSEIF (EMIN.EQ.ENER(I)) THEN
         ENERMU(1) = EMIN
         ISTRT = I+1
         FLUDMU(1) = DARIN1(I)
         FLUIMU(1) = DARIN2(I)
         FALKMU = DINT(I)
         GOTO  21
      ENDIF
1     CONTINUE
      GOTO 999
2     CONTINUE
      ISTRT = I
      ENERMU(1) = EMIN
*
**    For the first flux-point interpolate linearly on the log-log plot
*
      IF (ISTRT.EQ.1) THEN
         I1 = 1
         I2 = 2
      ELSE
         I1 = ISTRT-1
         I2 = ISTRT
      ENDIF
      ALF = ALOG(DARIN1(I1)/DARIN1(I2))/ALOG(ENER(I1)/ENER(I2))
      A = DARIN1(I1)/ENER(I1)**ALF
      FLUDMU(1) = A*EMIN**ALF
      FLUIMU(1) = DARIN2(ISTRT) + FLUINT(EMIN,ENER(ISTRT),FLUDMU(1),
     F                                 DARIN1(ISTRT))
      AALF = ALOG(DDIF(I1)/DDIF(I2))/ALOG(ENER(I1)/ENER(I2))
      AA = DDIF(I1)/ENER(I1)**AALF
      FDALK = AA*EMIN**AALF
      FALKMU = DINT(ISTRT) + FLUINT(EMIN,ENER(ISTRT),FDALK,
     F                                 DDIF(ISTRT))
*
**    Total flux taking into account EMIN
*
21    CONTINUE
      ISTRMU = ISTRT
      TOTFMU = FLUIMU(1)
*
***   Now for the other energy points straightforward
*
      DO 3 J = ISTRT,NENER
      I = 2 + J - ISTRT
      ENERMU(I) = ENER(J)
      FLUDMU(I) = DARIN1(J)
      FLUIMU(I) = DARIN2(J)
3     CONTINUE
      NFLUMU = 2 + NENER - ISTRT
C         PRINT 100
100      FORMAT(//10X,'ENERGY   DIFFERENTIAL  INTEGRATED TOTAL'//)
C
         DO 4  I=1,NFLUMU
C         PRINT 104,I,ENERMU(I),FLUDMU(I),FLUIMU(I)
104      FORMAT(2X,I4,2X,F10.2,4X,2E15.5)
4        CONTINUE
C
*
***   Get normalized intregral flux for randomization
*
C         PRINT 101
101      FORMAT(//10X,'ENERGY   DIFFERENTIAL  NORMALIZED TOTAL'//)
C
         DO 5  I=1,NFLUMU
         FLUNMU(I) = FLUIMU(I)/TOTFMU
C         PRINT 104,I,ENERMU(I),FLUDMU(I),FLUNMU(I)
5        CONTINUE
C
      RETURN
999   CONTINUE
      PRINT 9991,EMIN
9991  FORMAT(/2X,'MINIMUM ENERGY OF THE GENERATION ',F10.2,
     F       '  IS TOO BIG - STOP!'/)
      STOP
         END
CDECK,INITEG.
      SUBROUTINE INITEG
*     =================
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
**    Total intensity (in sec-1) corresponding to the volume and EMINRU
*        (2pi instead of 4pi, since muons are generated only in the
*         upward hemisphere !)
*
      FLUX = FLUIMU(1)
      TOTINT = TWOPI*RSURRU**2*FLUX
      FLUXIN = FLUX
      TOTMIN = TOTINT
*
      PRINT 100,EMINRU,RSURRU,FLUX,TOTINT
100   FORMAT(//2X,'COSMIC MUONS ARE GENERATED WITH MINIMUM ENERGY OF',
     F       F7.2,' GEV AT SEA-LEVEL'/2X,'ON A SPHERE OF RADIUS OF',
     F       F7.2,' CM.'/2X,'THE MINIMUM ENERGY CORRESPONDS TO A ',
     F       'MUON-FLUX OF',E12.4,' MUON/CM2/S '/2X,'THE VOLUME ',
     F       'DEFINED BY THE RADIUS RESULTS IN AN INTENSITY OF',E12.4,
     F       ' MUONS/SEC'//)
*
*      IF (WRITE1) WRITE (LUN1,101) EMINRU,RSURRU,FLUXIN,TOTMIN
101   FORMAT(4E15.5)
*
      END
CDECK,INITHG.
         SUBROUTINE INITHG
*        =================
*
***   Books histograms to monitor generation
*
         VMX = 0.
C
         CALL HBOOK1(101,' INITIAL MUON MOMENTUM WEIGHTED$',
     ,               100,0.,200.,VMX)
         CALL HIDOPT (101, 'LOGY')
C         CALL HBOOK1(2,' INITIAL COS(TH) OF MUON UNWEIGHTED$',
C     ,               20,0.,1.,VMX)
         CALL HBOOK1(102,' INITIAL COS(TH) OF MUON WEIGHTED$',
     ,               20,0.,1.,VMX)
      CALL HBOOK2(109,' GEND MUON FI.VS.CT IN MUONSYSTEM$',
     ,            50,0.,1.,72,0.,360.,VMX)
      CALL HBPRO(109,VMX)
C         CALL HBOOK1(3,' EXPONENT OF COS(TH) OF MUON$',
C     ,               20,-1.,3.,VMX)
C         CALL HBOOK1(5,' WEIGHT FREQUENCY$',
C     ,               50,0.,5.,VMX)
      CALL HBOOK2(201,' GENERATED MUON-IMPACT X-Y $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
      CALL HBOOK2(202,' GENERATED MUON-IMPACT X-Z $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
      CALL HBOOK2(203,' GENERATED MUON-IMPACT Y-Z $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
      END
CDECK,INTFLU.
C
         SUBROUTINE INTFLU
C        =================
C
C --- Routine integrates the differential flux assuming it has the
C     form of dN/dE = a*E**-alf, where E is the energy, a and alf are
C     experimental parameters
C
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
*     Energy points where the 0 deg zenith angle data were obtained
*
      DATA ENER/0.2,0.4,0.8,1.0,1.5,2.0,3.0,5.0,7.0,10.0,
     &15.,20.,30.,50.,70.,100.,150.,200.,300.,500.,700.,1000./
*
***   0 deg zenith angle differential xsections obtained by Allkofer
*        et al. Phys. Letters 36B (1971) 425
*
      DATA DDIF/0.373E-02,0.372E-02,0.310E-02,0.279E-02,0.214E-02,
     &0.167E-02,0.106E-02,0.497E-03,0.273E-03,0.133E-03,0.540E-04,
     &0.270E-04,0.959E-05,0.236E-05,0.892E-06,0.304E-06,0.851E-07,
     &0.335E-07,0.870E-08,0.152E-08,0.471E-09,0.134E-09/
*
*     INTEGRATION OF THE DAR PARAMETRIZATION OVER THE ANGLE
*     IN COS(TH)-BIN OF  0.001
*
      DATA DARIN1/
     D  0.4340E-02, 0.4551E-02, 0.4516E-02, 0.4363E-02, 0.3840E-02,
     D  0.3292E-02, 0.2388E-02, 0.1328E-02, 0.8104E-03, 0.4415E-03,
     D  0.2018E-03, 0.1099E-03, 0.4380E-04, 0.1252E-04, 0.5220E-05,
     D  0.1986E-05, 0.6332E-06, 0.2741E-06, 0.8152E-07, 0.1688E-07,
     D  0.5835E-08, 0.1859E-08/
*
**    Last bin of energy integral is obtained assuming an a*E**-alf
*        behaviour of DARIN1
*
      ALF = ALOG(DARIN1(NENER-1)/DARIN1(NENER))/ALOG(ENER(NENER-1)/
     A           ENER(NENER))
      DARIN2(NENER) = - DARIN1(NENER)*ENER(NENER)/(1.+ALF)
*
      DINT(NENER) = 5.23E-8
C
C
C ::: Integrate the flux
C
         DO 1 J = 1,NENER-1
         I = NENER - J
         DINT(I) = DINT(I+1) + FLUINT(ENER(I),ENER(I+1),
     ,               DDIF(I),DDIF(I+1))
         DARIN2(I) = DARIN2(I+1) + FLUINT(ENER(I),ENER(I+1),
     ,               DARIN1(I),DARIN1(I+1))
1        CONTINUE
C
      PRINT 100
100   FORMAT(//20X,' ---- FLUXES USED IN THIS PROGRAM ----'//
     F    2X,'(DIFFERENTIAL (AT 0 DEG) IS MEASURED BY ALLKOFER ET AL.',
     F ' PHYS.LETT.36B (1971) 425'/2X,'INTEGRAL IS INTEGRATED FROM',
     F ' THE DIFFERENTIAL BY THE PROGRAM ASSUMING E**-ALF FORM'/
     F 2X,'PARAMETRIZED IS THE DIFFERENTIAL (AT 0 DEG) GIVEN BY A.DAR ',
     F 'PHYS.REV.LETT. 51 (1983) 227',
     F /2X,'PARAM_INTEG IS DAR-S PARAMETRIZATION INTEGRATED OVER ALL',
     F ' ANGLES'/2X,'PARAM_D_INTEG IS PARAM_INTEG INTEGRATED OVER'
     F ,' ENERGIES ASSUMING E**-ALF DEPENDENCE)'
     F //7X,' ENERGY    DIFFERENTIAL       INTEGRAL   ',
     F    '     PARAMETRIZED     PARAM_INTEG     PARAM_D_INTEG'//)
C
         DO 2 I = 1,NENER
         DPARM = DNDPTH(ENER(I),0.)
         PRINT 101,  ENER(I),DDIF(I),DINT(I),DPARM,DARIN1(I),DARIN2(I)
101      FORMAT(2X,F10.2,5(4X,E13.4))
2        CONTINUE
       PRINT 102
102   FORMAT(//2X,'FOR COMPARISON : MEASURED TOTAL FLUX ABOVE 0.35 GEV'
     F/19X,'BY ALLKOFER ET AL. COSMIC RAYS ON EARTH,ISSN 0344-8401'/19X,
     F       'J2 = (1.90 +/- 0.12).E-2'//)
*
         END
CDECK,LININT.
      SUBROUTINE LININT(A,F,N,X,Y)
*     ============================
*
***   Routine interpolates linearly
*
*     Input: A - abscissa points
*            F - ordinate points
*            N - number of points
*            X - abscissa value where the interpolation
*                should be done
*    Output: F - the interpolated value of the function
*
      DIMENSION A(N),F(N)
*
      IF (X.LT.A(1)) THEN
         I = 2
         GOTO 2
      ELSEIF (X.EQ.A(1)) THEN
         I = 1
         GOTO 3
      ENDIF
      DO 1 I=2,N
      IF (X.LT.A(I)) THEN
         GOTO 2
      ELSEIF (X.EQ.A(I)) THEN
         GOTO 3
      ENDIF
1     CONTINUE
2     CONTINUE
      Y = F(I) + (F(I)-F(I-1))/(A(I)-A(I-1))*(X-A(I))
      RETURN
3     Y = F(I)
      END
CDECK,OVUSUM.
      FUNCTION OVUSUM(ID)
*     ===================
*
***   Routine calculates the sum total of overflow of a 2-dim histo
*
      CHARACTER TIT * 80
      CALL HGIVE(ID,TIT,NX,DUM2,DUM3,NY,DUM4,DUM5,NDUM1,IDUM1)
*
      OVUSUM = 0.
      DO 1 I=0,NX+1
1     OVUSUM = OVUSUM + HIJ(ID,I,0) + HIJ(ID,I,NY+1)
      DO 2 J = 1,NY
2     OVUSUM = OVUSUM + HIJ(ID,0,J) + HIJ(ID,NX+1,J)
*
      END
CDECK,ABSDET.
      SUBROUTINE ABSDET(X,E,RIN,ROU,ZIN,ZOU,D,PIN,ALDET)
*     ==================================================
*
***   Calculates approximate energy loss in a detector modul
*
**    The modul is simplified by consisting of a
*         'barrel' - 2 concentric cylindres of inner and outer
*                    radius, RIN,ROU and halflength ZIN, and an
*         'end-cap' - disk of radius ROU and width of ZOU-ZIN
*
**    The track of momentum PIN is approximated by a straight line
*         with starting point X and direction E (unit-vector)
*
**    D is the density (g/cm**3) of the modul
*
      DIMENSION X(3),E(3)
*
***   Path of the track inside the detector
*
      ALDET = 0.
      IF (X(1)*X(1)+X(2)*X(2)+X(3)*X(3).GE.ROU*ROU+ZOU*ZOU)
     I    RETURN
      CALL CYLLIN(X(1),X(2),E(1),E(2),ROU,N,ALO,ALD)
      IF (N.EQ.0) RETURN
      AZO = (SIGN(1.,E(3))*ZOU-X(3))/E(3)
      IF (AZO.LT.0) RETURN
      ALMAX = AMIN1(ALO,AZO)
      IF (N.EQ.2) THEN
         ALMAX = AMIN1(ALD,AZO)-ALO
      ENDIF
      CALL CYLLIN(X(1),X(2),E(1),E(2),RIN,N,ALI,ALD)
      IF (N.EQ.0) THEN
         ALDET = ALMAX
         GOTO 77
      ENDIF
      AZI = (SIGN(1.,E(3))*ZIN-X(3))/E(3)
      IF (AZI.LT.0.) THEN
         ALDET = ALMAX
         GOTO 77
      ENDIF
      IF (N.EQ.1) THEN
         ALDET = ALMAX-AMIN1(ALI,AZI)
      ELSE
         ALDET = ALMAX-AMIN1(ALD,AZI)+AMIN1(ALI,AZI)
      ENDIF
77    CONTINUE
*
***   Energy loss
*
      DD = D*ALDET
      CALL ELOS(PIN,PIN,DD)
      RETURN
*
      END
CDECK,ABSEAR.
      SUBROUTINE ABSEAR(X,E,PIN)
*     ==========================
*
***   Routine calculates the absorbtion of the muons by the Earth
*
*     The Earth surface is represented by a horizontal plane
*     The experimental hall is represented by a horizontal cylinder
*        centered around the beam-line
*     3 experimental shafts are included, 2 of them are situated at both
*        end of the experimental hall, the 3rd one at the machine
*        by-pass. Each shaft is schematised by a vertical cylinder.
*        The largest shaft includes a  horizontal concret layer which
*        will hold the offices.
*
*     All coordinates are given in cm and in the L3 frame, i.e.
*        (0,0,0) = intersection point
*        x - towards the LEP centre, y - vertical, z - e- beam direction
*
      DIMENSION X(3),E(3)
      DIMENSION ACON(4),ICON(4)
*
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*
      DATA SMALL/1.E-10/
**    Level of the Earth surface
      DATA YSURF/4454./
**    Central cavity - radius, offset of centre, wall upstream e- beam
      DATA RCH/1070./,YOFCH/500./,ZCH/-1890./
**    Shafts - offsets of centre, radius
      DATA ZOFS1/2290./,RS1/1150./
      DATA XOFS2/-370./,ZOFS2/-4880./,RS2/600./
      DATA XOFS3/2100./,RS3/450./
**    Concrete layer in shaft 1 - lower and upper level
      DATA YCON1/2010./,YCON2/2180./
**    Density of Earth and of concret (in g/cm**2)
      DATA DEART/2.6/,DCONC/2.5/
*
      ALCON = 0.
*
***   Distance from the Earth-surface
*
      EY = E(2)
      IF (EY.LT.SMALL) EY = SMALL
      ALS = (YSURF-X(2))/EY
      IF (ALS.LT.0) GOTO 991
*
***   Central hall
*
      CALL CYLLIN(X(1),X(2)-YOFCH,E(1),EY,RCH,N,ALC,ALCC)
      IF (N.EQ.0) ALC = 0.
      IF (N.EQ.2) ALC = ALCC
      IF (ALC.GT.ALS) GOTO 993
      EZ = E(3)
      IF (ABS(E(3)).LT.SMALL) EZ = SMALL
      IF (EZ.LT.0.) THEN
*
***   In the direction of the positrons
*
         ALW = (ZCH-X(3))/EZ
         IF (ALW.LT.ALC) THEN
            ALX = ALW
            NSH = 1
            GOTO 2
         ELSE
            ALX = ALC
            NSH = 2
            GOTO 2
         ENDIF
*
**    Test 2nd shaft
*
2        CONTINUE
         XP = X(3) - ZOFS2
         YP = X(1) - XOFS2
         EXP = EZ
         EYP = E(1)
         RS = RS2
         GOTO 10
*
**    Test 3rd shaft
*
3        CONTINUE
         XP = X(3)
         YP = X(1) - XOFS3
         EXP = EZ
         EYP = E(1)
         RS = RS3
         NSH = 1
10       CONTINUE
*
***   Test a vertical shaft outside
*
         CALL CYLLIN(XP,YP,EXP,EYP,RS,N,AL1,AL2)
         IF (N.EQ.0) THEN
            IF (E(1).GT.0..AND.NSH.EQ.2) GOTO 3
            ALE = ALS-ALX
            GOTO 90
         ELSEIF (N.EQ.2) THEN
            IF (ALX.GT.AL1) GOTO 994
            IF (AL1.GT.ALS) THEN
               ALE = ALS - ALX
               GOTO 90
            ELSE
               IF (AL2.GT.ALS) THEN
                  ALE = AL1 - ALX
                  GOTO 90
               ELSE
                  ALE = ALS-AL2 + AL1-ALX
                  GOTO 90
               ENDIF
            ENDIF
         ELSE
            GOTO 995
         ENDIF
      ELSE
*
***   In the direction of the electrons (big shaft)
*
*
**    Test 1st (big) shaft
*
         CALL CYLLIN(X(3)-ZOFS1,X(1),EZ,E(1),RS1,N,AL11,AL12)
         IF (N.EQ.0) THEN
           IF(E(1).GT.0.) THEN
*
**    Test 3rd shaft
*
            CALL CYLLIN(X(3),X(1)-XOFS3,EZ,E(1),RS3,N,AL31,AL32)
            IF (N.EQ.0) THEN
               ALE = ALS-ALC
               GOTO 90
            ELSEIF (N.EQ.2) THEN
               IF (ALC.GT.AL31) GOTO 994
               IF (AL31.GT.ALS) THEN
                  ALE = ALS - ALC
                  GOTO 90
               ELSE
                  IF (AL32.GT.ALS) THEN
                     ALE = AL31 - ALC
                     GOTO 90
                  ELSE
                     ALE = ALS-AL32 + AL31-ALC
                     GOTO 90
                  ENDIF
               ENDIF
            ELSE
               GOTO 995
            ENDIF
           ELSE
               ALE = ALS-ALC
               GOTO 90
           ENDIF
         ELSEIF (N.EQ.2) THEN
            IF (AL11.GE.ALS) THEN
               ALE = ALS-ALC
               GOTO 90
            ELSE
               ALE = 0.
               IF (AL11.GE.ALC) ALE = ALE + AL11-ALC
               IF (AL12.LE.ALS) ALE = ALE + ALS-AL12
            ENDIF
         ELSE
            GOTO 995
         ENDIF
C      PRINT 1526,NUMBEV
1526  FORMAT(2X,'ABSEAR: EVENT NUMBER FOR CONCRET SLAB -',I6)
*
**    Concret slab
*
      ACON(1) = AL11
      ACON(2) = AL12
      ACON(3) = (YCON1-X(2))/EY
      ACON(4) = (YCON2-X(2))/EY
      CALL SORTZV(ACON,ICON,4,1,0,0)
      IF ((ICON(1)+1)/2.EQ.(ICON(2)+1)/2) GOTO 90
*
**    Muon traversed the slab
*
      ALCON = ACON(ICON(3))-ACON(ICON(2))
*
      ENDIF
*
90    CONTINUE
*
***   Calculate energy loss in the Earth and in the concret slab
*
      IF (ALCON.GT.0.) THEN
         D = ALCON*DCONC
         CALL ELOS(PIN,PIN,D)
      ENDIF
      IF (PIN.GT.0.AND.ALE.GT.0.) THEN
         D = ALE*DEART
         CALL ELOS(PIN,PIN,D)
      ENDIF
*
***   Histograms
*
C      IF (ALCON.GT.0.) CALL HFILL(15,ALCON,0.,1.)
      IF (ALE.LT.0.) THEN
          PRINT 1527,NUMBEV
1527      FORMAT(2X,'NEGATIVE LENGTH IN EVENT : ',I10/)
      ENDIF
C      CALL HFILL(14,ALE,0.,1.)
C      CALL HFILL(141,ALE,0.,1.)
      RETURN
*
991   CONTINUE
      PRINT 9911
9911  FORMAT(/2X,'ABSEAR: DISTANCE TO SURFACE NEGATIVE! - STOP'/)
      PRINT 9912,X,E
9912  FORMAT(2X,'X AND E',2X,3F7.1,4X,3F7.4)
      STOP
993   CONTINUE
      PRINT 9931
9931  FORMAT(/2X,'ABSEAR: DISTANCE IN CENTRAL HALL NEGATIVE! - STOP'/)
      PRINT 9912,X,E
      STOP
994   CONTINUE
      PRINT 9941
9941  FORMAT(/2X,'ABSEAR: SHAFT 2 OR 3 ARE INSIDE CENTRAL HALL! STOP'/)
      PRINT 9912,X,E
      PRINT 9942,NSH,ALX,ALC,ALW,AL1,AL31
9942  FORMAT(2X,'NSH,ALX,ALC,ALW,AL1,AL31',I3,5E13.4)
      STOP
995   CONTINUE
      PRINT 9951
9951  FORMAT(/2X,'ABSEAR: ONLY 1 SOLUTION IN SHAFTS! - STOP'/)
      PRINT 9912,X,E
      STOP
      END
CDECK,CMTRAC.
      SUBROUTINE CMTRAC(NUMU,PMU,COSTMU,PHIMU,CHARMU,XMU,YMU,ZMU,
     ,                  WEIMU,IEOF)
*     ============================================================
*
************************************************************************
*                                                                      *
*     Routine to track the muon through the Earth and part             *
*       of the L3 detector down to the surface of generation           *
*       to test if the muon survive the traversal                      *
*                                                                      *
*       Input :   NUMU - original serial number of the generated muon  *
*                 PMU - momentum of the muon                           *
*                 COSTMU - cosine of the muon zenith angle             *
*                 PHIMU - azimuthal angle of the muon                  *
*                 CHARMU - charge of the muon                          *
*                 XMU,YMU,ZMU - coordinates of the muon on the sphere  *
*                               ZMU is pointing to the vertical        *
*                 WEIMU - weight of the muon                           *
*                                                                      *
*       Output :  IEOF - 1 if E.o.f. read otherwise 0                  *
*                                                                      *
************************************************************************

C
C --- Fast tracking of muon in the L3 detector and in the shaft
C
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
         DIMENSION MSTATI(10)
         EQUIVALENCE (MSTATI(1),MGENST)
*
         DATA RBIHCA,RBOHCA,ZWIHCA,ZWOHCA,DENHCA
     /       /  89.,  220.,  100.,  235.,  7.5 /
         DATA RBIBGO,RBOBGO,ZWIBGO,ZWOBGO,DENBGO
     /       /  53.,  72.,   64.5,  64.5,  7.1 /
         DATA RBICOI,RBOCOI,ZWICOI,ZWOCOI,DENCOI
     /       /  617., 717.,  643.,  643.,  2.7 /
         DATA RBIYOK,RBOYOK,ZWIYOK,ZWOYOK,DENYOK
     /       / 717.,  817.,  643.,  709.,  7.87 /
*
***   Read in the muon if second pass
*
      IOEF = 0
*      IF (READ)
*     R  READ(LUN1,100,END=99)PMU,COSTMU,PHIMU,CHARMU,XMU,YMU,ZMU,WEIMU
100   FORMAT(3E14.6,F5.1,4E14.6)
*
***   Generation number
*
      NUMBEV = NUMU
*
**    Acceptance flag for SIGEL3 output
*
      IACCSI = 0
C
C --- MUON GENERATED
C
         CHMUSI = CHARMU
         WEIGSI = WEIMU
         MGENST = MGENST + 1
C
C
         THC = ACOS(COSTMU)
         THDEG = THC*DEGRAD
         PHIDEG = PHIMU*DEGRAD
C
C --- CUT ON ZENITH ANGLE
C
         IF(THDEG.GT.ANGCRU) GOTO 999
C
C --- MUON SURVIVED ZENITH ANGLE (THETA) CUT
C
         MANGST = MANGST + 1
C
C
C --- ROTATE TO THE L-3 SYSTEM
C
         PTRA = PMU
         CALL COROT(THC,PHIMU,THD,PHD)
C
C --- UNIT VECTOR OF MUON DIRECTION
C
         ETRA(1)=SIN(THD)
         ETRA(2)=ETRA(1)*SIN(PHD)
         ETRA(1)=ETRA(1)*COS(PHD)
         ETRA(3)=COS(THD)
C
C --- MUON IMPACT POINT ON A CIRCLE AT THE BOTTOM OF THE MASTER SURFACE
C
         AL      = ZMU/ETRA(2)
         XTRA(2) =   0.
         XTRA(1) =   XMU - AL * ETRA(1)
         XTRA(3) = - YMU - AL * ETRA(3)
*
***   Acceptance test with a user defined volume
*
*                                               |
* ACCTST: RT radius of test cylinder         <--|
*         ZT half length of test cylinder    <--|
*
         CALL ACCTST (XTRA,ETRA,ALTST)
         IF (ALTST.LE.0.) GOTO 999
*
***   Calculate coordinates on the Muon Chamber.
*
      CALL SIGINT(XTRA,ETRA,PTRA,1)
      IF (IACCSI.EQ.0) GOTO 999
      IACCSI = 0
C
C ---  ABSORBTION DUE TO THE EARTH
C
         CALL ABSEAR(XTRA,ETRA,PTRA)
         IF(PTRA.LT.EMNARU) GOTO 999
         PLOS = PMU - PTRA
         CALL HFILL (106,PLOS,0.,1.)
C
C --- MUON SURVIVED ABSORBTION IN EARTH
C
         MENEST = MENEST + 1
C
         IF(PTRA.LT.EMNARU) GOTO 999
C
C ---  Muon survived energy loss in the magnet (yoke+coil)
C
C
C --- CHECKING ABSORBTION
C
C ... RETURN YOKE
C
         CALL ABSDET(XTRA,ETRA,RBIYOK,RBOYOK,ZWIYOK,ZWOYOK,DENYOK,
     ,               PTRA,ALYOK)
C         CALL HFILL(11,ALYOK,0.,1.)
         IF(PTRA.LT.EMNARU) GOTO 999
C
C ... MAGNET COIL
C
         CALL ABSDET(XTRA,ETRA,RBICOI,RBOCOI,ZWICOI,ZWOCOI,DENCOI,
     ,               PTRA,ALCOI)
C         CALL HFILL(13,ALCOI,0.,1.)
         IF(PTRA.LT.EMNARU) GOTO 999
*
      NACCRU = NACCRU + 1
*
**    Dump out the muon for SIGEL3
*
      IACCSI = 1
      CALL SIGINT(XTRA,ETRA,PTRA,2)
*
***   Plot the tracked muon momentum
*
         PLOS = PMU - PTRA
         CALL HFILL(108,PLOS,0.,1.)
         CALL HFILL(113,PTRA,0.,WEIMU)
         CALL HFILL(114,COSTMU,0.,WEIMU)
         CALL HFILL(115,COSTMU,PHIDEG,1.)
*
***   Look for absorbtion up-to the surface of generation
*
C
C ... HADRON CALORIMETER
C
         CALL ABSDET(XTRA,ETRA,RBIHCA,RBOHCA,ZWIHCA,ZWOHCA,DENHCA,
     ,               PTRA,ALHCA)
C         CALL HFILL(12,ALHCA,0.,1.)
         IF(PTRA.LT.EMNARU) GOTO 999
C
C ... BGO
C
         CALL ABSDET(XTRA,ETRA,RBIBGO,RBOBGO,ZWIBGO,ZWOBGO,DENBGO,
     ,               PTRA,ALBGO)
C         CALL HFILL(16,ALBGO,0.,1.)
         IF(PTRA.LT.EMNARU) GOTO 999
*
         IF (ALBGO.GT.0.01) THEN
            MDETST = MDETST + 1
*
***   Plot the accepted muon momentum
*
            CALL HFILL(11,ALYOK,0.,1.)
            CALL HFILL(13,ALCOI,0.,1.)
            CALL HFILL(12,ALHCA,0.,1.)
            CALL HFILL(16,ALBGO,0.,1.)
            PLOS = PMU - PTRA
            CALL HFILL (107,PLOS,0.,1.)
*
            CALL HFILL(103,PTRA,0.,WEIMU)
            CALL HFILL(104,COSTMU,0.,WEIMU)
            CALL HFILL(105,COSTMU,PHIDEG,1.)
            CALL HFILL(301,XMU,YMU,1.)
            CALL HFILL(302,XMU,ZMU,1.)
            CALL HFILL(303,YMU,ZMU,1.)
C           CALL ABSDET(XTRA,-ETRA,RBIBGO,RBOBGO,ZWIBGO,ZWOBGO,DENBGO,
C    ,                  PTRA,ALBGO1)
C           CALL HFILL(161,ALBGO1,0.,1.)
         END IF
C
999      CONTINUE
         RETURN
99    CONTINUE
*
***   End of file
*
      IEOF = 1
*
      END
CDECK,COROT.
      SUBROUTINE COROT(THC,PHC,THD,PHD)
*     =================================
*
***   Rotation of the coordinates from the cosmic-ray system:
*            z - vertical, x and y are orthogonal to z
*                                  to  the LEP system:
*            x - pointing towards the LEP centre,
*            y - vertical, z - direction of the e- beam
*
**    The change of the coordinate system corresponds to:
*            x --> x, -y --> z, z --> y
*
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
      XD=SIN(THC)*COS(PHC)
      YD=COS(THC)
      ZD=-SIN(THC)*SIN(PHC)
      THD=ACOS(ZD)
      PHD=ATAN2(YD,XD)
      IF(PHD.LT.0.) PHD=PHD+TWOPI
      RETURN
      END
CDECK,CYLLIN.
      SUBROUTINE CYLLIN(X,Y,EX,EY,R,NSOL,AL1,AL2)
*     ===========================================
*
***   Routine calculates the intersection of a cylinder
*        with a straight line
*     The cylinder is centered at x,y = 0 and is infinite in z
*        it has a radius of R
*     The straight line is given by a point (X,Y) and a by the
*        2 first component (EX,EY) of a unit-vector (EX,EY,EZ)
*     NSOL is the number of intersection in the +ve direction
*     If NSOL.ne.0, the routine calculates the distances, AL1
*        and AL2 (in increasing order) form the point (X,Y,Z)
*        to the intersection
*
      DOUBLE PRECISION DET,A,B,C,AM,YY
*
      NSOL = 0
      AL1 = 0.
      AL2 = 0.
*
      IF (ABS(EX).LT.1.E-5) THEN
         IF (ABS(EY).LT.1.E-5) GOTO 999
         X1 = Y
         Y1 = X
         EX1 = EY
         EY1 = EX
      ELSE
         X1 = X
         Y1 = Y
         EX1 = EX
         EY1 = EY
      ENDIF
      AM = EY1/EX1
      YY = Y1-AM*X1
      A = 1.+AM*AM
      B = 2.*AM*YY
      C = YY*YY-R*R
      DET = B*B - 4.*A*C
      IF (DET.LT.0.) RETURN
*
      DET = DSQRT(DET)
      XS1 = (-B + DET)/(2.*A)
      XS2 = (-B - DET)/(2.*A)
*
      AL1 = (XS1 - X1)/EX1
      AL2 = (XS2 - X1)/EX1
      ALN = AMIN1(AL1,AL2)
      ALX = AMAX1(AL1,AL2)
      IF (ALX.LT.0) GOTO 998
      IF (ALN.LT.0) THEN
         NSOL = 1
         AL1 = ALX
         RETURN
      ENDIF
      NSOL = 2
      AL1 = ALN
      AL2 = ALX
      RETURN
998   CONTINUE
C      PRINT 9981
9981  FORMAT(/2X,'ERROR IN CYLLIN - BOTH LENGTHS ARE NEGATIVE !'/)
      RETURN
999   CONTINUE
      PRINT 9991
9991  FORMAT(/2X,'BOTH EX AND EY ARE TOO SMALL - CYLLIN CANNOT WORK!'/)
      END
CDECK,CYLLI1.
      SUBROUTINE CYLLI1(X,Y,EX,EY,R,NSOL,AL1,AL2)
*     ===========================================
*
***   Routine calculates the intersection of a cylinder
*        with a straight line
*     The cylinder is centered at x,y = 0 and is infinite in z
*        it has a radius of R
*     The straight line is given by a point (X,Y) and a by the
*        2 first component (EX,EY) of a unit-vector (EX,EY,EZ)
*     NSOL is the number of intersection
*     If NSOL.ne.0, the routine calculates the distances, AL1
*        and AL2 (in increasing order) form the point (X,Y,Z)
*        to the intersection
*     Contrary to S/R CYLLIN this routines allows also negative
*        lengths
*
      DOUBLE PRECISION DET,A,B,C,AM,YY
*
      NSOL = 0
      AL1 = 0.
      AL2 = 0.
*
      IF (ABS(EX).LT.1.E-5) THEN
         IF (ABS(EY).LT.1.E-5) GOTO 999
         X1 = Y
         Y1 = X
         EX1 = EY
         EY1 = EX
      ELSE
         X1 = X
         Y1 = Y
         EX1 = EX
         EY1 = EY
      ENDIF
      AM = EY1/EX1
      YY = Y1-AM*X1
      A = 1.+AM*AM
      B = 2.*AM*YY
      C = YY*YY-R*R
      DET = B*B - 4.*A*C
      IF (DET.LT.0.) RETURN
*
      DET = DSQRT(DET)
      XS1 = (-B + DET)/(2.*A)
      XS2 = (-B - DET)/(2.*A)
*
      AL1 = (XS1 - X1)/EX1
      AL2 = (XS2 - X1)/EX1
      ALN = AMIN1(AL1,AL2)
      ALX = AMAX1(AL1,AL2)
      NSOL = 2
      AL1 = ALN
      AL2 = ALX
      RETURN
999   CONTINUE
      PRINT 9991
9991  FORMAT(/2X,'BOTH EX AND EY ARE TOO SMALL - CYLLIN CANNOT WORK!'/)
      END
CDECK,ELOS.
      SUBROUTINE ELOS(EIN,EOUT,D)
C     ===========================
C
C     ESTIMATES ENERGY LOSS FOR COSMIC RAYS.
C     EIN,EOUT IN GEV. D IN GR*CM**-2
C     MENON & MURTHY. PROGRESS IN ELEMENTARY PARTICLE AND COSMIC RAY
C     PHYSICS. VOL9.
C
      EMEV=1000.*EIN
      DO 100 I=1,0.1*D
      EX=EMEV**2/(EMEV+10.9E3)
      DE=1.88+0.0766*ALOG(EX/106.)+3.6E-6*EMEV
      EMEV=EMEV-10.*DE
      IF(EMEV.LT.0.) GOTO 200
100   CONTINUE
      EOUT=0.001*EMEV
      RETURN
200   CONTINUE
      EOUT=0.
      RETURN
      END
CDECK,INITET.
      SUBROUTINE INITET
*     =================
*
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
      DIMENSION ICODE(8), PARAM(4)
*
      IF (READ) THEN
*         READ (LUN1,102) EMINRU,RSURRU,FLUXIN,TOTMIN
102   FORMAT(4E15.5)
*
**    Total intensity (in sec-1) corresponding to the volume and EMINRU
*
*
      PRINT 100,EMINRU,RSURRU,FLUXIN,TOTMIN
100   FORMAT(//2X,'COSMIC MUONS GENERATED WITH MINIMUM ENERGY OF',
     F       F7.2,' GEV AND'/2X,'ON A SURFACE OF A SPHERE OF RADIUS OF',
     F       F7.2,' CM WILL BE TRACKED FROM THE EARTH SURFACE'/
     F       2X,'TO THE SPHERE CENTERED AROUND THE I-2 INTERSECTION.'
     F       /2X,'THE MINIMUM ENERGY CORRESPONDS TO A ',
     F       'MUON-FLUX OF',E12.4,' MUON/CM2/S '/2X,'THE VOLUME ',
     F       'DEFINED BY THE RADIUS RESULTS IN AN INTENSITY OF',E12.4,
     F       ' MUONS/SEC'//)
*
      ENDIF
      PRINT 101,EMNARU,ANGCRU
101   FORMAT(/2X,'MUONS ARE ACCEPTED WITH MINIMUM ENERGY OF',E12.4,/
     F       2X,'MAXIMUM ZENITH ANGLE OF',F6.2,' DEGREES'/)
*
*      IF (WRITE2) WRITE(LUN2,102) EMNARU,ANGCRU,FLUXIN,TOTMIN
*      IF (STAND) THEN
*      CALL UCTOH ('COSMIC_MUON.V110.880700         ',ICODE,4,32)
*         NPARA = 3
*         PARAM(2) = EMNARU
*         PARAM(3) = ANGCRU
*         PARAM(1) = RSURRU
*         CALL HEADER (LUN2, ICODE, NPARA, PARAM, TOTMIN)
*      ENDIF
*
      END
CDECK,INITHT.
         SUBROUTINE INITHT
*        =================
*
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
***   Books histograms to monitor tracking.
*
         VMX = 0.
C
         IF (.NOT.GENER)
     C      CALL HBOOK1(101,' INITIAL MUON MOMENTUM WEIGHTED$',
     ,               100,0.,100.,VMX)
         IF (.NOT.GENER) CALL HIDOPT (101, 'LOGY')
         CALL HBOOK1(113,' TRACKED MUON MOMENTUM WEIGHTED$',
     ,               100,0.,100.,VMX)
         CALL HIDOPT(113,'LOGY')
         CALL HBOOK1(114,' TRACKED COS(TH) OF MUON WEIGHTED$',
     ,               20,0.,1.,VMX)
         CALL HBOOK2(115,' TRKD MUON FI.VS.CT IN MUONSYSTEM$',
     ,               50,0.,1.,72,0.,360.,VMX)
         CALL HBPRO(115,VMX)
         CALL HBOOK1(103,' ACCEPTED MUON MOMENTUM WEIGHTED$',
     ,               100,0.,100.,VMX)
         CALL HIDOPT (103, 'LOGY')
         IF (.NOT.GENER)
     C   CALL HBOOK1(102,' INITIAL COS(TH) OF MUON WEIGHTED$',
     ,               20,0.,1.,VMX)
         CALL HBOOK1(104,' ACCEPTED COS(TH) OF MUON WEIGHTED$',
     ,               20,0.,1.,VMX)
      CALL HBOOK2(301,' ACCEPTED MUON-IMPACT X-Y $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
      CALL HBOOK2(302,' ACCEPTED MUON-IMPACT X-Z $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
      CALL HBOOK2(303,' ACCEPTED MUON-IMPACT Y-Z $',
     ,            50,-200.,200.,50,-200.,200.,VMX)
C        CALL HBOOK1(14,' LENGTH IN EARTH$',50,0.,100.,VMX)
C        CALL HBOOK1(141,' LENGTH IN EARTH$',50,0.,100.,VMX)
C        CALL HBOOK1(15,' LENGTH IN CONCRET$',50,0.,100.,VMX)
         CALL HBOOK1(106,' ENERGY LOSS IN EARTH$',80,0.,40.,VMX)
         CALL HBOOK1(11,' LENGTH IN YOKE$',100,-50.,150.,VMX)
         CALL HBOOK1(13,' LENGTH IN COIL$',100,-50.,150.,VMX)
         CALL HBOOK1(108,' ENERGY LOSS UP TO MU. CH.$',80,0.,40.,VMX)
         CALL HBOOK1(12,' LENGTH IN HCAL$',100,-50.,150.,VMX)
         CALL HBOOK1(16,' LENGTH IN BGO$',100,-50.,150.,VMX)
         CALL HBOOK1(107,' TOTAL ENERGY LOSS$',80,0.,40.,VMX)
C        CALL HBOOK1(161,' LENGTH IN BGO DOWN$',100,-50.,150.,VMX)
         CALL HBOOK2(105,' ACCD MUON FI.VS.CT IN MUONSYSTEM$',
     ,               50,0.,1.,72,0.,360.,VMX)
         CALL HBPRO(105,VMX)
      END
CDECK,SIGINT.
      SUBROUTINE SIGINT(X,E,P,MODE)
*     =============================
*
***   Tracked muon vector to be transferred to SIGEL
*       Coordinates are given at the entrance to the muon chamber
*       Direction is downwards (i.e. py is negative)
*       Momentum is reduced by absorbtion in the earth and magnet
*
      COMMON / MEVENT / NUMBEV
      COMMON / ABSFLU / DLCTAB,DELEAB,NENEAB,NCTHAB,FLUFAB,ICLOAB
      COMMON / COSTRA / PTRA, ETRA(3), XTRA(3)
      COMMON / INTENS / FLUXIN, TOTMIN
      COMMON / MUSPEC / ENERMU(30), FLUDMU(30), FLUIMU(30),
     .   FLUNMU(30), NFLUMU, ISTRMU, FALKMU
      PARAMETER (NENER = 22)
      COMMON / MUFLUX / DINT(NENER), DARIN2(NENER)
      DIMENSION ENER(NENER), DDIF(NENER), DARIN1(NENER)
      PARAMETER (NTH = 10, NGAMM = 10)
      COMMON / MUANG / XGAMM(NGAMM), YGAMM(NGAMM), WEIGMU(NTH,NGAMM),
     .                 COSTH(NTH)
      COMMON / NUMCON / PI, TWOPI, PIBY2, DEGRAD, RADDEG
      COMMON / RUNCON / EMINRU, RSURRU, NGENRU, TIMERU,
     .                  NACCRU, EMNARU, ANGCRU
      COMMON / SIGELI / XCMUSI(3), PCMUSI(3), CHMUSI, WEIGSI, IACCSI
      COMMON / STATIS / MGENST, MANGST, MENEST, MDETST
      LOGICAL GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
      COMMON / SWITCH /GENER,TRACK,READ,WRITE1,WRITE2,NOWEGH,STAND
*      COMMON / UNITS / LUN1, LUN2
*
      DIMENSION X(3),E(3)
*
      DATA RMUCH/615./,ZMUCH/590./
*
      GOTO (10,20),MODE
10    CONTINUE
*
      IACCSI = 1
      CALL CYLLI1(X(1),X(2),E(1),E(2),RMUCH,N,AL1,AL2)
      IF (N.EQ.0) GOTO 99
      AZO = (SIGN(1.,E(3))*ZMUCH-X(3))/E(3)
      AL = AMIN1(AL2,AZO)
      DO 1 I = 1,3
1     XCMUSI(I) = X(I) + AL*E(I)
      IF (AL.LT.0.) CALL UCOPY(XCMUSI(1),X(1),3)
      RETURN
*
20    CONTINUE
*
      DO 2 I = 1,3
2     PCMUSI(I) = - P*E(I)
*
      IF (WRITE2) THEN
*         WRITE(LUN2,100) NUMBEV,(XCMUSI(I),I=1,3),(PCMUSI(I),I=1,3)
*     ,             ,CHMUSI,WEIGSI
100      FORMAT(I10,6E15.5,F7.1,E15.5)
         IF (NUMBEV.LE.20) THEN
            PRINT 101
101         FORMAT(/2X,'OUTPUT TO SIGEL3')
C            PRINT 100, NUMBEV,(XCMUSI(I),I=1,3),(PCMUSI(I),I=1,3)
C     ,             ,CHMUSI,WEIGSI
         ENDIF
      ENDIF
*      IF (STAND) CALL UEVENT
      RETURN
*
99    CONTINUE
      IACCSI = 0
C      PRINT 991,NUMBEV,X,E
991   FORMAT(/2X,'SIGINT: TRACK DOES NOT CROSS THE MUON CHAMBER'
     F      ,' IN EVENT',I10,' WITH COORDINATES',3F7.2,3F8.4/)
      END
CDECK,ACCTST.
      SUBROUTINE ACCTST(X,E,ALT)
*     ==========================
*
*                                               |
* ACCTST: RT radius of test cylinder         <--|
*         ZT half length of test cylinder    <--|
*
***   Tests acceptance of the muon in a cylinder enclosing the interesting d
*     detector component.
*
**    Purpose: effective Sigel output
*
      DIMENSION X(3),E(3)
*
*     bgo barrel:    (radius of generation surface: 120 cm)
C     DATA RT/85./,ZT/84./
*     scintillators: (radius of generation surface: 180 cm)
C     DATA RT/105./,ZT/152./
*     scintillators test: (radius of generation surface: 200 cm)
C     DATA RT/115./,ZT/165./
*     scintillators test: (radius of generation surface: 320 cm)
      DATA RT/250./,ZT/200./
*     muon chambers: (radius of generation surface: 850 cm)
C     DATA RT/615./,ZT/585./
*
      ALT = 0.
      CALL CYLLI1(X(1),X(2),E(1),E(2),RT,N,AL1,AL2)
      IF (N.EQ.0) RETURN
      ALZ = (SIGN(1.,E(3))*ZT - X(3)) /E(3)
      ALT = AMIN1(AL2,ALZ)
      RETURN
*

      END
      subroutine f02eafw(xma,xwr)
       implicit none
       real *8 xma(3,3),xwr(3),tmp
       integer ifail,n,i,j,lda,lwork,ldz
       parameter (lda=3)
       parameter (lwork=3*64)
       real *8 wi(lda),wr(lda),z(lda,lda),xmb(lda,lda)
       real *8 work(lwork)
       real *8 eps
       logical matv
       real *8 ima(lda,lda),rdb(lda,lda),idb(lda,lda),beta(lda),
     + vr(lda,lda),vi(lda,lda)
       integer iter(lda)
       eps=0
       n=3
       ldz=3
       do i=1,lda
        do j=1,lda
         ima(i,j)=0
         rdb(i,j)=0
         idb(i,j)=0
       enddo
      enddo
      do i=1,3
       rdb(i,i)=1
      enddo
       ifail=-1
       matv=.false.
#ifndef __ICC__
       call SSTEQR('N',n,xma,wr,z,ldz,work,ifail)
#else 
       call f02gjf(n,xma,n,ima,n,rdb,n,idb,n,eps,wr,wi,beta,
     +  matv,vr,n,vi,n,iter,ifail)
#endif
       if(ifail.eq.0)then       
       do i=1,3
        xwr(i)=sqrt(wr(i)**2+wi(i)**2)
       enddo
       do i=1,3
        do j=i,3
         if(xwr(i).lt.xwr(j))then
           tmp=xwr(i)
           xwr(i)=xwr(j)
           xwr(j)=tmp
         endif
        enddo
       enddo
       endif
       end
