#ifndef CERNLIB_TYPE
#define CERNLIB_TYPE
#endif
        SUBROUTINE SIMTRD(g3trd)
        implicit none
C
C...TR-simulation main program
C
#include    "geant321/gctrak.inc"
#include "geant321/gcbank.inc"
#include "geant321/gckine.inc"
#include "geant321/gcking.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcvolu.inc"
#include "geant321/gccuts.inc"
#include "geant321/gconst.inc"
#include "geant321/gcsets.inc"
#include "geant321/gclist.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcnum.inc"
#include "trdf.h"
*--
        integer g3trd
      DIMENSION ETR(1000),STR(1000),VECTS(7)
      real etr,str,vects,gamma,eloss
      integer ntr,jtm,n
*        DATA COR/0.48/                      !... for Polypr Foam (Airex)
*         DATA COR/0.68/                      !... for Polypr Fibre (Fleece)
C...



         integer nwbuf
C...


*         some how we have to get also ions 
        IF(ABS(CHARGE).EQ.0)        RETURN
C...
        if(step.le.0)               RETURN

        GAMMA = GETOT/AMASS
        IF(GAMMA.LE.100.)           RETURN


        NTR            = 0


*
*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 99
      IF (NUMED.LE.0)GO TO 99
      IF (NUMED.GT.NTMED)GO TO 99
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 99
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 99
      if(Q(JTM+14+1).ne.g3trd.or.
     +(Q(JTM+14+2).gt.2.or.Q(JTM+14+2).lt.0))goto 99
      if(Q(JTM+14+3).eq.3.and.inwvol.eq.1)then
       CALL GENTRDI(ABS(CHARGE),GAMMA,STEP,NTR,ETR,STR)
      else if(Q(JTM+14+3).eq.1)then
       CALL GENTRD(ABS(CHARGE),GAMMA,Q(JTM+14+4),STEP,NTR,ETR,STR)
      else
       goto 99
      endif



C...
C..    TR-Photon generated means NTR =|= 0
C.
        IF(NTR.EQ.0) RETURN

c        write(6,'(A,I8,I6,F12.3)') 'SIMTRD: IDEVT,NTR,GAMMA'
c     >                                     ,IDEVT,NTR,GAMMA

c        write(*,*)'generated: ',ntr,Q(JTM+14+3),Q(JTM+14+4),vect(3)
        CALL UCOPY(VECT,VECTS,7)
        ELOSS          = 0
        DO 1 N         = 1,NTR


        NGKINE         = NGKINE+1
        GKIN(1,NGKINE) = ETR(N)*VECTS(4)
        GKIN(2,NGKINE) = ETR(N)*VECTS(5)
        GKIN(3,NGKINE) = ETR(N)*VECTS(6)
        GKIN(4,NGKINE) = ETR(N)
        if(Q(JTM+14+2).lt.2)then
         GKIN(5,NGKINE) = 48.
        else
         GKIN(5,NGKINE) = 1.
        endif
c        write(*,*)'gen2 ',n,etr(n),str(n),vects(3)
        GPOS(1,NGKINE) = VECTS(1)-STR(N)*VECTS(4)
        GPOS(2,NGKINE) = VECTS(2)-STR(N)*VECTS(5)
        GPOS(3,NGKINE) = VECTS(3)-STR(N)*VECTS(6)
        VECT(1)        = VECTS(1)-STR(N)*VECTS(4)
        VECT(2)        = VECTS(2)-STR(N)*VECTS(5)
        VECT(3)        = VECTS(3)-STR(N)*VECTS(6)
        TOFD (NGKINE)  = 0.
        IFLGK(NGKINE)  = 0
*        IFLGK(NGKINE)  = 1
        ELOSS          = ELOSS+ETR(N)



         CALL GSKING(NGKINE)




         NGKINE         = NGKINE-1
1       CONTINUE
C... Energy primary particle correction
        CALL UCOPY(VECTS,VECT,7)
        GETOT          = GETOT-ELOSS
        GEKIN          = GEKIN-ELOSS
        VECT(7)        = SQRT(GEKIN*(GEKIN+2.*AMASS))
        CALL GEKBIN
C...
 99     CONTINUE

        E N D


C-------------------------------------------------------
        FUNCTION SRADIUS (VECT,STEP,Q)
        implicit none
C
C... *** Calculate the min.distance of the track to wire ***
C
        DIMENSION VECT(7),R1(3),R1D(3),R2D(3),A(2)
        real step,q,vect,r1,r1d,r2d,a,s,sradius
C...if step = 0
C
        IF(STEP.NE.0.AND.Q.NE.0.) GO TO 2
        CALL GMTOD(VECT,R1D,1)
1       SRADIUS = SQRT(R1D(1)**2+R1D(2)**2)
*   STEP=0 used by SPACTRD
C
        RETURN
C
C...if step.ne.0.
C
2       R1(1)    = VECT(1)-STEP*VECT(4)
        R1(2)    = VECT(2)-STEP*VECT(5)
        R1(3)    = VECT(3)-STEP*VECT(6)
C
        CALL GMTOD(R1  ,R1D,1)
        CALL GMTOD(VECT,R2D,1)
        A(1)     = (R2D(1)-R1D(1))/STEP
        A(2)     = (R2D(2)-R1D(2))/STEP
        S        = -(R1D(1)*A(1)+R1D(2)*A(2))/(A(1)**2+A(2)**2)
        IF(S.LE.0.  ) GO TO 1
        IF(S.LT.STEP) GO TO 3
        SRADIUS  = SQRT(R2D(1)**2+R2D(2)**2)
        RETURN
C
3       SRADIUS  = SQRT((R1D(1)+S*A(1))**2+(R1D(2)+S*A(2))**2)
        RETURN
        E N D
C----------------------------------------------------------
        SUBROUTINE SPACTRD (VECT,STEP,DESTEP,NF,NUMS,Q)
        implicit none
C...
#       include "trdf.h"
C...
        DIMENSION VECT(7),VEC(3)
        real vect,step,destep,q,r,sradius,st,vec
        integer nf,nums,ih,n
C...
        IF(DESTEP.EQ.0) RETURN
        IF(Q.NE.0..AND.NCLESTEP.NE.0) GO TO 1
        R     = SRADIUS(VECT,0.,Q)
*        IH    = MIN(50,MAX(1,INT(R/.004)))
        IH=MIN(100,MAX(1,INT(SRADIUS(VECT,0.,Q)/.003)))
        SPTRD(IH,NUMS,NF) = SPTRD(IH,NUMS,NF)+DESTEP
*        PRINT *,'1,ih,nf',ih,nf,DESTEP,SPTRD(IH,NUMS,NF)
        RETURN
C...
1       DO 2 N = 1,NCLESTEP
        ST     = CLESTEP(2,N)
        VEC(1) = VECT(1)-VECT(4)*ST
        VEC(2) = VECT(2)-VECT(5)*ST
        VEC(3) = VECT(3)-VECT(6)*ST
*        IH     = MIN(50,MAX(1,INT(SRADIUS(VEC,0.,Q)/.004)))
        IH=MIN(100,MAX(1,INT(SRADIUS(VEC,0.,Q)/.003)))
        SPTRD(IH,NUMS,NF) = SPTRD(IH,NUMS,NF)+CLESTEP(1,N)
C...
2         CONTINUE
C...
        RETURN
        END
C
C ************************************************************
C *       PROGRAM FOR DE/DX-SIMULATION WITHIN STRAWS         *
C ************************************************************
C
        SUBROUTINE SIMDE(g3trd)
        implicit none
C
C... *** Simulation energy deposition within straw ***
C
#include    "geant321/gctrak.inc"
#include    "geant321/gckine.inc"
#include    "geant321/gcvolu.inc"
#include    "geant321/gccuts.inc"
#include     "geant321/gcbank.inc"
#include     "geant321/gcnum.inc"
#include     "geant321/gctmed.inc"
#include     "geant321/gcflag.inc"
#include     "trdf.h"
      integer g3trd,nwbuf,jtm
      real gamma,de,derand

*>>>> T.S. 2/2003  Variables for Delta-Rays
#include    "geant321/gcking.inc"
#include     "geant321/gconsp.inc"
      INTEGER      Mdr,Ndr,i
      PARAMETER   (Mdr=10)  ! max. number of delta-rays per step
      REAL         Edr(Mdr), Sdr(Mdr)
      REAL         PMODdr, THTdr, PHIdr, RNDM(1)
      CHARACTER*4  CMECH
      INTEGER      IMECH
      EQUIVALENCE (CMECH,IMECH)
*<<<< -----------------------------------------
 



      IF(ISTOP      .NE.0 .OR.STEP         .LE.0.    ) RETURN

*>>>> T.S. 2/2003   keep GEANT3 DESTEP for IONS !!!
*      IF(ABS(CHARGE).EQ.0.)                            RETURN
      IF(ABS(CHARGE).NE.1.)                            RETURN
*<<<< -----------


*
*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 99
      IF (NUMED.LE.0)GO TO 99
      IF (NUMED.GT.NTMED)GO TO 99
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 99
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 99
      if(Q(JTM+14+1).ne.g3trd.or.
     + Q(JTM+14+2).ne.0
     + .or.Q(JTM+14+3).ne.3)goto 99     ! 3 trd-gas
      
        
c        IF(NADE.NE.'GTRL'.AND.NADE.NE.'GTRS')            RETURN
C...
        GETOT   = GETOT+DESTEP
        GEKIN   = GEKIN+DESTEP
        GAMMA   = GETOT/AMASS
C...
        DE      = DERAND(GAMMA,DCUTE,STEP,Ndr,Edr,Sdr)
       
*>>>>>> T.S. 2/2003 ------------------
        if( Ndr.gt.0 ) 
     >   WRITE(6,'(A,I8,3I6)') 
     >           'SIMDE: IDEVT,Ndr,NGKINE,MXGKIN'
     >                  ,IDEVT,Ndr,NGKINE,MXGKIN

        Do i=1,MIN(Ndr,Mdr)    ! put Delta-Rays on stack   

          PMODdr = sqrt(Edr(i)**2+2*Edr(i)*EMASS)
          THTdr  = acos(sqrt(Edr(i)/(Edr(i)+2*EMASS)))
          CALL GRNDM(RNDM,1)
          PHIdr = TWOPI*RNDM(1)

c          write(6,*)'SIMDE DRAY: i,Edr,PMODdr,THTdr,PHIdr,Sdr,TWOPI,X,Y,Z',
c     &      i,Edr(i),PMODdr,THTdr,PHIdr,Sdr(i),VECT(1),VECT(2),VECT(3)

          if( NGKINE.lt.MXGKIN ) then
            NGKINE         = NGKINE+1
            GKIN(1,NGKINE) = PMODdr*(VECT(4)*sin(THTdr)*cos(PHIdr))
            GKIN(2,NGKINE) = PMODdr*(VECT(5)*sin(THTdr)*sin(PHIdr))
            GKIN(3,NGKINE) = PMODdr*(VECT(6)*cos(THTdr))
            GKIN(4,NGKINE) = Edr(i)
            GKIN(5,NGKINE) = 3.
            
            GPOS(1,NGKINE) = VECT(1)-Sdr(i)*VECT(4)
            GPOS(2,NGKINE) = VECT(2)-Sdr(i)*VECT(5)
            GPOS(3,NGKINE) = VECT(3)-Sdr(i)*VECT(6)
            
            TOFD(NGKINE)  = 0.
            IFLGK(NGKINE) = 0
            CMECH         = 'DRAY'
            KCASE         = IMECH
            
            GEKIN = GEKIN - Edr(i)
            GETOT = GETOT - Edr(i)
          endif
        Enddo
*<<<<<< ---------------------------------


        IF(GEKIN-DE.GT.CUTELE) THEN

          GEKIN   = GEKIN-DE
          GETOT   = GETOT-DE
          DESTEP  = DE
          DESTEL  = DESTEP
          VECT(7) = SQRT(GEKIN*(GEKIN+2.*AMASS))

        ELSE ! Stop particle

          NCLESTEP = 0
          ISTOP   = 2
          DESTEP  = GEKIN
          DESTEL  = DESTEP
          GEKIN   = 0.
          GETOT   = AMASS
          VECT(7) = 0.

        ENDIF

        CALL GEKBIN
 99     RETURN
        E N D
*CMZ :          15/08/94  09.46.59  by  Valeri Saveliev
*-- Author :
C------------------------------------------------------------
        FUNCTION DERAND (GAMMA,ECUT,STEP,Ndr,Edr,Sdr)
C
C... ***  Simulation of energy loss straggling within straw
C         with energy smaller then ECUT ***
C
         implicit none

*>>>>> T.S. 2/2003 Variable CHARGE in GCKINE
#include "geant321/gckine.inc"
*<<<<< -------------------------------------

#include "trdf.h"
C
C NCLESTEP     - Total number clusters,
C CLESTEP(1,#) - energy of the # cluster,
C CLESTEP(2,#) - coordinate of the # cluster.
C
        DIMENSION ANGL(200),RNDM(2)
        real angl,rndm,gamma,ecut,step,gaml,derand,ec,dgg
        real anmin,anmax,dan,amu,r,ee,e
        integer i,k,n,ier,k1,k2

*>>>> T.S. 2/2003  
*     Generator Gas-Density     should be a DATA-CARD  !!!!!!!
      REAL        Gdens 
      PARAMETER  (Gdens=0.00446)    !    default 4.46 g/cm**3

*     variables for Delta-Rays:
      INTEGER     Mdr,Ndr
      PARAMETER  (Mdr=10)  ! max. number of delta-rays per step
      REAL        Edr(Mdr), Sdr(Mdr)

      REAL        Pecut    ! should be DATA-CARD !!!!!
      PARAMETER  (Pecut=-0.65) ! default 65% of ionisations above DCUTE are ignored
*                       +0.73            73% of ... reappear as Dray on second.stack

*     variables for cubic interpolation:
      INTEGER     ifail,j,m,l,ir(4)    
      REAL        A(4,4),X(4),B(4),GAMLN(4)
      REAL        RVMPY
      EXTERNAL    RVMPY

C...
        Ndr = 0
*<<<< -----------------------------------------

        NCLESTEP = 0
        EC       = 1.E9*ECUT
        DERAND   = 0.
        IF(STEP.LE.0.) RETURN
C...
        GAML     = ALOG(GAMMA)
        IF(GAML.GE.GAME(21)) THEN
        CALL UCOPY(ANL(1,21),ANGL,NWE)
        GO TO 4
        END IF
C...
        DO 1 I  = 1,21
        IF(GAML.LE.GAME(I)) GO TO 2
1         CONTINUE
2       I       = MIN(21,MAX(I,2))

*>>>>>> T.S. 2/2003
        ifail = 0
        IF (I.GT.2 .AND. I.LT.20) then    ! cubic interpolation

          DO j=1,4
            m = (I-2) + (j-1)
            GAMLN(j) = GAML**(j-1)
            DO l =1,4
              A(j,l) = game(m)**(l-1)
            ENDDO
          ENDDO

          CALL RINV(4,A,4,IR,ifail)

          DO K  = 1,NWE
            DO j=1,4
              m = (I-2) + (j-1)
              B(j) = ANL(K,m)
            ENDDO
            CALL VFILL(X,4,0.)
            CALL VMATR(B,A,X,4,4)
            ANGL(K) = RVMPY(4,X(1),X(2),GAMLN(1),GAMLN(2))
          ENDDO

        ENDIF
	
	IF(I.le.2 .OR. I.ge.20 .OR. ifail.ne.0) THEN  ! linear interpolation
	
          DGG     = (GAML-GAME(I-1))/(GAME(I)-GAME(I-1))
          DO K  = 1,NWE
            ANGL(K) = ANL(K,I-1)+(ANL(K,I)-ANL(K,I-1))*DGG
          ENDDO

        ENDIF
*<<<<<  -----------------------------------------------



C...
4       ANMIN   = EXP(ANGL(NWE))
        ANMAX   = EXP(ANGL( 1 ))
C...
        DAN     = ANMAX-ANMIN

*>>>>>  T.S. 2/2003
        AMU     = DAN * STEP
     >                * Gdens/0.00446  ! dEdX density corr. (nominal: 0.00446 g/cm**3)
     >                * CHARGE**2   ! for charge<>1 particles
*<<<<<  -------------------------


        CALL POISSN(AMU,N,IER)
        IF(N.LE.0.) RETURN
        DO 8 I  = 1,N
        K1      = 1
        K2      = NWE
        CALL GRNDM(RNDM,2)
        R       = ALOG(RNDM(1)*DAN+ANMIN)
6       K       = (K1+K2)/2
        IF(R.NE.ANGL(K)) GO TO 7
        EE      = EXP(WE(K))
*>>>        DERAND  = DERAND+EE
        GO TO 81
7       IF(R.GT.ANGL(K)) K2 = K
        IF(R.LT.ANGL(K)) K1 = K
        IF(K2-K1.GT.1  ) GO TO 6
C...
        E       = WE(K2)+(R-ANGL(K2))*(WE  (K1)-WE  (K2))/
     +                                (ANGL(K1)-ANGL(K2))
        EE      = EXP(E)

*>>>>>> T.S. 2/2003
 81     CALL GRNDM(RNDM,1)
        IF( EE.GT.EC .AND. RNDM(1).le.abs(Pecut) ) THEN   ! new delta-ray

          if( Pecut.gt.0.      ! we want secondary Drays
     >        .and. Ndr.lt.Mdr ) then
            Ndr = Ndr + 1
            Edr(Ndr) = EE/1.e9
            CALL GRNDM(RNDM,1)
            Sdr(Ndr) = RNDM(1)*STEP
          endif

        ELSE
C
C..fill clusters bank
C
          NCLESTEP = MIN(4000,NCLESTEP+1)
          CLESTEP(1,NCLESTEP) = EE*1.E-9
          CLESTEP(2,NCLESTEP) = STEP*RNDM(2)
C
C...Total energy calculation
C
          DERAND  = DERAND+EE

        ENDIF
*<<<<<< -----------------------
8       CONTINUE

C...
        DERAND  = DERAND*1.E-9
        RETURN
        END
*CMZ :          16/06/99  14.57.04  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C---------------------------------------------------------
        FUNCTION SIMNOIS (E)
C
C... *** Electronic noise simulation ***
C
        implicit none
        real  simnois,fwhm,e,ee,spacechar,r1
        DATA FWHM/.27/


      REAL AMVEC
      DATA AMVEC / 100.0 /
      INTEGER NRVEC

C
C...Space charge effect simulation
C
        EE      = E*1.E6
        SIMNOIS = SPACECHAR(EE)
C...
        IF(FWHM.LE.0.) GO TO 1

*        CALL RANNOR(R1,R2)
         CALL GPOISS(AMVEC,NRVEC,1)     ! based on RANECU
         R1 = (real(NRVEC)-AMVEC)/SQRT(AMVEC)

        SIMNOIS = SIMNOIS+R1*FWHM/2.35*SQRT(6.*SIMNOIS)
1       SIMNOIS = SIMNOIS*1.E-6
        RETURN
        END


*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C--------------------------------------------------------
        FUNCTION SPACECHAR (E)
C
C... *** Space charge effect simulation (Sosnovcev gas gain
C        for mixture 50%Xe+25%CO2+25%CF4 ***
C
        implicit none
        real e,spacechar
        SPACECHAR = E
        IF(E.LE.2.) RETURN
        SPACECHAR = AMAX1(.2*E,E*(1.0314-.0157*E))
        RETURN
        END



*CMZ :          03/09/99  20.32.51  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C
C *********************************************************
C *      TR-photon absorbtion                             *
C *********************************************************
C
        SUBROUTINE TRPHOTON(g3trd)
        implicit none
C
C... *** TR-photon absorbtion simulation ***
C
#include "geant321/gctrak.inc"
#include "geant321/gctmed.inc"
#include "geant321/gckine.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcbank.inc"
#include "geant321/gcking.inc"
#include "geant321/gcvolu.inc"
#include "geant321/gcnum.inc"
#include "trdf.h"

C...
      integer nwbuf,jtm,im,g3trd
      real stea,sabs
C...


*      Volume Types:
*      0: (Default) Convert TRD-Geantino to ordinary Photon
*      1: (RADIAT.) if absorbed then 4 else continue tracking
*      2: (TBWALL)  like 1
*      3: (GAS)     if absorbed then (new K,L TRD);4 else cont. tr.
*      4: (WIRE)    Stop TRD-Geantino   DESTEP=Etrd
*      5: (AIR)     like 1
*     10: (Cu)      like 1

*      DATA NAMM/'TRLB','TRSB','STRA','GTRL','GTRS','WIRE'/
*      DATA IMM /    1 ,    1 ,    2 ,    3 ,    3 ,    4 /
C...
c       DATA NAMM/'TRAD','STFA','TBMU','TBWL','TBML','TBGS','TBWI',
c     >           'STFM','STFS','STFF','TCLY','TRDU','TRDL',
c     >           'TBSS','TBSW'/
c       DATA IMM /    1,     1,     2,     2,     2,     3,     4,
c     >               5,    10,     2,     5,     5,     5,
c     >               2,     2 /
cC...

C...


*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 1
      IF (NUMED.LE.0)GO TO 1
      IF (NUMED.GT.NTMED)GO TO 1
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 1
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 1
      if(Q(JTM+14+1).ne.g3trd.or.
     +(Q(JTM+14+2).gt.1.or.Q(JTM+14+2).lt.0)) goto 1


        IM=Q(JTM+14+3)

*>>>>>> T.S. 2/2003
        CALL DIFFSCAT(IM) !  DDIS process

C       continue with TR-Photon-absorption...

        IF( IPART.NE.48 .OR.ISTOP.NE.0 .OR. STEP.LE.0. ) RETURN
C...
*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

        goto 2

 1      continue 
C
C... stop Geantino and apparition Gamma
C
        IF( IPART.NE.48 .OR.ISTOP.NE.0 .OR. STEP.LE.0. ) RETURN

c        WRITE(6,'(A,I8,2I6)') 'TRPHOTON: TR->Y',IDEVT,NGKINE,MXGKIN

*         write(*,*)'absorbed somewhere ',gekin,vect(1),vect(2),vect(3)
        NGKINE         = NGKINE+1
        GKIN(1,NGKINE) = GEKIN*VECT(4)
        GKIN(2,NGKINE) = GEKIN*VECT(5)
        GKIN(3,NGKINE) = GEKIN*VECT(6)
        GKIN(4,NGKINE) = GEKIN
        GKIN(5,NGKINE) = 1.
*>>
        GPOS(1,NGKINE) = VECT(1)
        GPOS(2,NGKINE) = VECT(2)
        GPOS(3,NGKINE) = VECT(3)
*<<
        TOFD  (NGKINE) = 0.
        IFLGK (NGKINE) = 0
*        IFLGK (NGKINE) = 1
        CALL GSKING(NGKINE)
***
        NGKINE         = NGKINE-1

        ISTOP   = 1
        DESTEP  = 0.
        DESTEL  = 0.
        GEKIN   = 0.
        GETOT   = 0.
        VECT(7) = 0.
        CALL GEKBIN
        RETURN

C... stop TR-photon
2      continue 
       IF(IM.NE.4) GO TO 4

3       IF(IM.EQ.3) CALL EPHTRD(GEKIN)



        ISTOP   = 2
        DESTEP  = GEKIN
        DESTEL  = DESTEP
        GEKIN   = 0.
        GETOT   = 0.
        VECT(7) = 0.
c         write(*,*)'photon absorbed in ',Q(JTM+14+3),GEKIN,DESTEP
        CALL GEKBIN
        RETURN

C... Absorbtion simulation
4        STEA    = SABS(GEKIN,STEP,IM)
         IF(STEA.GT.STEP) then
c         write(*,*)'passed  trd ',im,gekin,vect(1),vect(2),vect(3)
            RETURN
         endif
c         write(*,*)'absorbed in trd ',im,gekin,vect(1),vect(2),vect(3)
         VECT(1) = VECT(1)+(STEA-STEP)*VECT(4)
         VECT(2) = VECT(2)+(STEA-STEP)*VECT(5)
         VECT(3) = VECT(3)+(STEA-STEP)*VECT(6)
         GO TO 3
C...
         E N D





*CMZ :          03/05/99  23.29.46  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C------------------------------------------------------------
        SUBROUTINE GENTRD (QCH,GAMMA,COR,STEP,NTR,ETR,STR)
C
C... *** Simulation TR-photon in STEP-length radiator ***
C
C  QCH       - abs charge
C  GAMMA     - Lorentz factor,
C  COR       - correction factor,
C  NTR       - number   TR photons,
C  ETR       - energy   TR photons (GeV),
C  STR       - distance of each photon.
C
          implicit none
#include "trdf.h"
C...
        DIMENSION ETR(1000),STR(1000),DNWMG(81)
        DIMENSION RNDM(2)
        real etr,str,dnwmg,step,cor,gamma
        real rndm,qch
        real gamtab,anmin,anmax,gl,dgg,dan,am
        integer ntr,jg,nfoil,i,jg1,n,jtab,ier
        integer k1,k2,k
        real r,e
        DATA GAMTAB/0./,DNWMG/81*0./,ANMIN/0./,ANMAX/0./
        DATA JG/0/,GL/0./

*>>>>>> T.S. 2/2003
*       variables for cubic interpolation:
        INTEGER j,l,m,ir(4),ifail
        REAL GAMLN(4),A(4,4),X(4),B(4)
        REAL        RVMPY
        EXTERNAL    RVMPY
*<<<<<< T.S. 2/2003

C...
        NTR     = 0
        NFOIL   = STEP/(FOIL+GAS)

c        WRITE(6,'(A,I6,4F12.6)')
c     >  ' GENTRD: NFOIL,STEP,FOIL,GAS, GAMMA '
c     >   ,NFOIL,STEP,FOIL,GAS,GAMMA

        IF(GAMMA.LT.EXP(GINWM(1)).OR.NFOIL.LE.0 ) RETURN
        IF(ABS(GAMMA-GAMTAB).LE.5.              ) GO TO 6
C...
        GAMTAB  = GAMMA
        GL      = ALOG(GAMMA)
*       print *,'*gentrd*',gamma,gl,ginwm(1),ginwm(20)
*       print *,'*gentrd*',FOIL,GAS,STEP
        IF(GL.LT.GINWM(20)) GO TO 3
        DO 2 I  = 1,81
2       DNWMG(I)= DNM(I,20)
        JG      = 20
        GO TO 5
C...
3       JG      = JTAB(GL,20,GINWM)
        JG1     = JG+1

*>>>>>> T.S. 2/2003 >>>>>>>>>>>>>>
        DO j=1,4
          if(     JG.le. 2 ) then
            m = j
          elseif( JG.lt.18 ) then
            m = (JG-2) + (j-1)
          else
            m = j + 16
          endif
          GAMLN(j) = GL**(j-1)
          do l =1,4
            A(j,l) = GINWM(m)**(l-1)
          enddo
        ENDDO
        CALL RINV(4,A,4,IR,ifail)

        IF( ifail.ne.0 ) then ! Matrix-Inversion failed - using lin.approx.

          DGG     = (GL-GINWM(JG))/(GINWM(JG1)-GINWM(JG))
          DO I  = 1,81
            DNWMG(I)= (DNM(I,JG1)-DNM(I,JG))*DGG+DNM(I,JG)
            write(6,'(A,I3,E12.4)') 'DNWMG(I)= ',I,DNWMG(K)
          ENDDO

        ELSE

          DGG     = (GL-GINWM(JG))/(GINWM(JG1)-GINWM(JG))

          DO K  = 1,81
            DO j=1,4
                IF (JG.le.2) then
                  m = j
                elseif (JG.gt.2.and.JG.lt.18) then
                  m = (JG-2) + (j-1)
                else
                  m = j + 16
                endif
                B(j) = DNM(K,m)
*                 write(6,*)'j,m,B(j)',j,m,B(j)
            ENDDO
            CALL VFILL(X,4,0.)
            CALL VMATR(B,A,X,4,4)
            DNWMG(K) = RVMPY(4,X(1),X(2),GAMLN(1),GAMLN(2))
c            write(6,'(A,I3,E12.4)') 'DNWMG(K)= ',k,DNWMG(K)
          ENDDO
        ENDIF

*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	
C...
5       ANMIN   = EXP(DNWMG(81))
        ANMAX   = EXP(DNWMG(1 ))
C...
6       DAN     = ANMAX-ANMIN
        AM      = COR*DAN*NFOIL*QCH**2
C...
C...... DANGER
*       AM = 0.
        CALL POISSN(AM,N,IER)

c        WRITE(6,'(A,F12.6,I6,2F12.6,I6)')
c     >  'GENTRD: DAN,NFOIL,COR,AM,N ',DAN,NFOIL,COR,AM,N

        IF(N.LE.0) RETURN
C...
        N       = MIN(N,1000)
        DO 10 I = 1,N
        K1      = 1
        K2      = 81
        CALL GRNDM(RNDM,2)
        R       = ALOG(RNDM(1)*DAN+ANMIN)
7       K       = (K1+K2)/2
        IF(R.NE.DNWMG(K)) GO TO 8
        E       = EXP(WTR(K))
        K2      = K
        GO TO 9
8       IF(R.GT.DNWMG(K)) K2 = K
        IF(R.LT.DNWMG(K)) K1 = K
        IF(K2-K1.GT.1) GO TO 7
C...
        E       = WTR(K2)+(R-DNWMG(K2))*(WTR  (K1)-WTR  (K2))/
     +                                  (DNWMG(K1)-DNWMG(K2))
        E       = EXP(E)
C...
9       NTR         = NTR+1
        ETR(NTR)    = E*1.E-6
        STR(NTR)    = STEP*RNDM(2)
10      CONTINUE
C...
        RETURN
        E N D









*CMZ :          18/11/95  19.45.02  by  Valeri Saveliev
*-- Author :
C---------------------------------------------------------
        SUBROUTINE GENTRDI (QCH,GAMMA,STEP,NTR,ETR,STR)
C
C... *** TR-generation within straw ***
C
         implicit none
#include "trdf.h"
C...
        DIMENSION RNDM(2),ETR(1000),STR(1000)
        real QCH,GAMMA,STEP,ETR,STR
        integer ntr
        real gaml,ant,wmi,rndm,wma,w0,s1,s2,g2,a0,e,e2
        DATA GAML/0./,ANT/0./
C...
        NTR     = 0
        WMI     =       TDATA(2,3)*2. E-3
        WMA     = GAMMA*TDATA(2,4)*.25E-3
        IF(WMA.LE.WMI) RETURN
        CALL GRNDM(RNDM,2)
C...
        IF(ABS(GAMMA-GAML).GT.5.) THEN
        GAML    = GAMMA
        W0      = (WMA+WMI)/2.
        S1      = (1.E-3*TDATA(2,3)/W0)**2
        S2      = (1.E-3*TDATA(2,4)/W0)**2
        G2      = 1./GAMMA**2
        A0      = 2.323E-3*((S1+S2+2.*G2)/(S1-S2)*
     +            ALOG((G2+S1)/(G2+S2))-2.)
        ANT     = A0*ALOG(WMA/WMI)
        END IF
C...
        IF(RNDM(1).GT.ANT) RETURN
        E        = WMI*(WMA/WMI)**(RNDM(2))
        IF(E.GT.5.) RETURN
        NTR      = 1
        ETR(1)   = E*1.E-6
        STR(1)   = STEP
        RETURN
        E N D
*CMZ :          11/08/99  20.26.17  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C----------------------------------------------------------
        SUBROUTINE EPHTRD (E0)
C
C... *** Simulate res within straw in case photon absorb ***
C
C...Kinematics of generated secondaries
C
            implicit none
            real e0
#include    "geant321/gckine.inc"
#include    "geant321/gcking.inc"
#include    "geant321/gctrak.inc"
#include    "geant321/gcvolu.inc"
#include    "geant321/gcflag.inc"
             

C...

        DIMENSION RNDM(3)
        real rndm
        real ef,pm,pl,pk,e2
        real eabs(3)
        data EABS/34.56,4.8,0.6/

*>>>>>> T.S. 2/2003
        CHARACTER*4  Cmech
        INTEGER      Imech
        EQUIVALENCE (Cmech,Imech)
*<<<<<< --------------------------

C...




        Cmech = '    '

        EF     = E0*1.E6
        CALL FRA(EF,PM,PL,PK,EABS)
        CALL GRNDM(RNDM,2)
C...
*
* Bug Fixed V. Choutko 29.02.2000
*
        IF(PM+PL+PK.eq.0.or.RNDM(1).LT.PM)goto 999

        if(RNDM(1).GT.(PM+PL)) then
*
* K
*
         IF(RNDM(2).GT.0.88) goto 999   ! no fluo
         E2=(EABS(1)-EABS(2))*1.e-6
         CMECH = 'Xe-K'

        else
*
* L
*
         IF(RNDM(2).GT..11)        goto 999       ! no fluo
         E2     = (EABS(2)-EABS(3))*1.e-6
         CMECH = 'Xe-L'
        endif

        E0=E0-E2


C
C... Simulation new photon
C
c        WRITE(6,'(A,A5,I8,2I6)') 'EPHTRD:',Cmech,IDEVT,NGKINE,MXGKIN

        NGKINE   = NGKINE+1
        CALL ISOTR(GKIN(1,NGKINE))
        GKIN(1,NGKINE) = GKIN(1,NGKINE)*E2
        GKIN(2,NGKINE) = GKIN(2,NGKINE)*E2
        GKIN(3,NGKINE) = GKIN(3,NGKINE)*E2
        GKIN(4,NGKINE) = E2
        GKIN(5,NGKINE) = 48.
        TOFD  (NGKINE) = 0.
        IFLGK (NGKINE) = 0
*        IFLGK (NGKINE) = 1
*>>
        GPOS(1,NGKINE) = VECT(1)
        GPOS(2,NGKINE) = VECT(2)
        GPOS(3,NGKINE) = VECT(3)
*<<
        KCASE          = IMECH

        CALL GSKING(NGKINE)


***

        NGKINE         = NGKINE-1
 999    continue
c        write(*,*)'xe interacton ',ef,e0,e2
        if(e0.lt.0)then
         write(*,*)' bug********************* '
        endif

C...
       E N D

*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C---------------------------------------------------------
        SUBROUTINE FRA (E,PM,PL,PK,EABS)
        real EABS(3)
         
C
C... *** Calculate fraction different mode of interaction
C        photons with K,L,M-electron shell of XE
C        PM,PL,PK - propability to interact with M,L,K-shells ***
C
C
C...real photon
C
        IF(E.GT.EABS(1))then
         PK      = .83
         PL      = .145
         PM      = .025
        else if(E.GT.EABS(2))then
         PK=0
         PL      = .84
         PM      = .16
        else if(E.GT.EABS(3))then
         PK=0
         PL=0
         PM=1
        else
         PK=0
         PL=0
         PM=0
        endif
        E N D
*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C------------------------------------------------------
        SUBROUTINE ISOTR(P)
cC
cC... *** Simulation isotropic direction ***
cC
c        DIMENSION P(3),RNDM(3)
cC...
c1       CALL GRNDM(RNDM,3)
c        DO 2 K = 1,3
c        P(K)   = RNDM(K)-.5
c2         CONTINUE
cC...
c        SP     = P(1)**2+P(2)**2+P(3)**2
c        IF(SP.GT..25) GO TO 1
c        SQSP   = SQRT(SP)
c        DO 3 K = 1,3
c        P(K)   = P(K)/SQSP
c3         CONTINUE
c        RETURN
*
* would like to speed up a bit (VC)
*
*
        
        implicit none
        real P(3),rnd(2)
        real ph,st,ct,sp,cp
        real twopi
        data twopi/6.28318530/
        call grndm(rnd,2)
        ct=2*rnd(1)-1
        ph=twopi*rnd(2)
        st=sqrt(1-ct**2)
        sp=sin(ph)
        cp=cos(ph)
        p(1)=cp*st
        p(2)=sp*st
        p(3)=ct
        END

*CMZ :          28/04/99  18.59.14  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C---------------------------------------------------------
        FUNCTION SABS (E0,STEP,IM)
        implicit none
C...
#include "trdf.h"
C...
      
        DIMENSION RNDM(1)
        real rndm,sabs,e0,step,e,am,amuf
        integer im
C...
        E      = E0*1.E6              ! E in keV
        IF(     IM.EQ. 1) then
          AM = (AMUF(E,1)*FOIL+AMUF(E,2)*GAS)/(GAS+FOIL) !RAD
        else IF(IM.EQ. 2)then
          AM = AMUF(E, 3)   ! Straw
        else IF(IM.EQ. 3)then
          AM = AMUF(E, 4)   ! Gas
        else IF(IM.EQ. 5) then
          AM = AMUF(E, 2)   ! Air
        else IF(IM.EQ. 6) then
          SABS=1.e10        ! Vacuum
          goto 999 
        else IF(IM.EQ. 8) then
          AM = AMUF(E, 8)   ! Parylene-C   T.S. 2/2003
        else IF(IM.EQ. 9) then
          AM = AMUF(E, 9)   ! Aluminum     T.S. 2/2003
        else IF(IM.EQ.10) then
          AM = AMUF(E,10)   ! Copper
        else
         SABS=0
         write(*,*) 'sabs error IM=',IM
         goto 999
        endif
C...
        CALL GRNDM(RNDM,1)
        SABS   = -1./AM * ALOG(AMAX1(1.E-20,RNDM(1)))
c        write(*,*)' mean path length for ',im,1./am,e,step,sabs

 999    RETURN
        E N D
*CMZ :          03/05/99  23.40.28  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C----------------------------------------------------------
        FUNCTION A M U F (X,IM)
C
*       X:  E in keV
*       IM: Material
*        1  Radiator      \
*        2  Air            |   from DESY2.DAT
*        3  Straw Wall     |
*        4  Gas           /

*        8  Parylene-C    \
*        9  Aluminum       |   coded below
*       10  Copper        /

#include "trdf.h"
       

        INTEGER    NBIN_cu
        PARAMETER (NBIN_cu=12)
        REAL       RHO_cu, MU_cu(NBIN_cu), EBIN_cu(NBIN_cu)
        PARAMETER (RHO_cu=8.96)                 ! g/cm**3
        DATA MU_cu   / 1.0e4, 4.2e3, 2.1e3, 720.0, 330.0, 180.0,
     >                 110.0,  51.0, 210.0,  73.0,  34.0,  11.0 /
        DATA EBIN_cu /   1.0,   1.5,   2.0,   3.0,   4.0,   5.0,
     >                   6.0,   8.0,  10.0,  15.0,  20.0,  30.0 /

*       Parylene_C (T.S. 10-2002)
*       http://www-cxro.lbl.gov/tmp/xray3170.dat
        INTEGER    NBIN_pc
        PARAMETER (NBIN_pc=12)
        REAL       RHO_pc, MU_pc(NBIN_pc), EBIN_pc(NBIN_pc)
        PARAMETER (RHO_pc=1.29)                 ! g/cm**3
        DATA MU_pc   / 3.7e3, 1.2e3, 540.0, 204.0, 850.0, 187.0,
     >                  50.0,  26.0,   8.0,   3.5,   1.2,   0.5 /
        DATA EBIN_pc /   1.0,   1.5,   2.0,   2.8,  2.85,   5.0,
     >                   8.0,  10.0,  15.0,  20.0,  30.0,  50.0 /

*       Aluminium (T.S. 10-2002):
*       http://physics.nist.gov/PhysRefData/XrayMassCoef/ElemTab/z13.html
        INTEGER    NBIN_al
        PARAMETER (NBIN_al=12)
        REAL       RHO_al, MU_al(NBIN_al), EBIN_al(NBIN_al)
        PARAMETER (RHO_al=2.70)                 ! g/cm**3
        DATA MU_al   / 1.2e3,  3.6e2, 4.0e3, 2.3e3, 788.0, 193.0,
     >                  50.3,   26.2,   8.0,   3.4,   1.1,   0.4 /
        DATA EBIN_al /   1.0,   1.55,  1.56,   2.0,   3.0,   5.0,
     >                   8.0,   10.0,  15.0,  20.0,  30.0,  50.0 /

C...
        IF(IM.GE.1.AND.IM.LE.4) THEN   ! from generated Tables:

          AMUF = TABLOG(X,30,WT,AMUTT(1,IM))*TDATA(1,IM)

        ELSEIF( IM.EQ.10 ) THEN        ! Copper

          DO i=NBIN_cu,1,-1
            IF( X.gt.EBIN_cu(i) ) GOTO 10
          ENDDO
          IF( i.lt.1 ) i=1
 10       AMUF = MU_cu(i)*RHO_cu
**          WRITE(6,'(A,F12.1,I4,F12.1)') 'AMUF Cu: ', X,i,1.e4/AMUF


*>>>>>> T.S. 2/2003
        ELSEIF( IM.EQ.8 ) THEN        ! Parylene-C (PC)

          DO i=NBIN_pc,1,-1
            IF( X.gt.EBIN_pc(i) ) GOTO 8
          ENDDO
  8       IF( i.eq.NBIN_pc ) THEN
            AMUF = MU_pc(i)*RHO_pc
          ELSEIF( i.le.1 ) THEN
            AMUF = MU_pc(1)*RHO_pc
          ELSE ! linlog extrapolation T.S.
            AMUL = (X-EBIN_pc(i)) * (log(MU_pc(i+1))-log(MU_pc(i)))
     >                            / (  EBIN_pc(i+1) -  EBIN_pc(i) )
            AMUF = exp(AMUL) * MU_pc(i)*RHO_pc
          ENDIF

**          WRITE(6,'(A,F12.1,I4,F12.1)') 'AMUF pc: ', X,i,AMUF

        ELSEIF( IM.EQ.9 ) THEN        ! ALUMINUM

          DO i=NBIN_al,1,-1
            IF( X.gt.EBIN_al(i) ) GOTO 9
          ENDDO
  9       IF( i.eq.NBIN_al ) THEN
            AMUF = MU_al(i)*RHO_al
          ELSEIF( i.le.1 ) THEN
            AMUF = MU_al(1)*RHO_al
          ELSE ! linlog extrapolation T.S.
            AMUL = (X-EBIN_al(i)) * (log(MU_al(i+1))-log(MU_al(i)))
     >                            / (  EBIN_al(i+1) -  EBIN_al(i) )
            AMUF = exp(AMUL) * MU_al(i)*RHO_al
          ENDIF

**          WRITE(6,'(A,F12.1,I4,F12.1)') 'AMUF al: ', X,i,AMUF
*<<<<<< -------------------------------------------------------

        ELSE                           ! This should not happen

          PRINT *,'ERR AMUF  IM=',IM
          STOP

        ENDIF

C...
        RETURN
        E N D


*CMZ :          18/02/99  14.40.26  by  Valeri Saveliev
*-- Author :
C----------------------------------------------------------
        integer FUNCTION   J T A B (X,N,XM)
C
*       DIMENSION XM(20)
        DIMENSION XM(30)
C...
*       WRITE(6,*)' N= ',N
        IF(X.GT.XM(1)) GO TO 1
        JTAB  = 1
        RETURN
1       IF(X.LT.XM(N)) GO TO 2
        JTAB  = N-1
        RETURN
2       J1    = 1
        J2    = N
3       J     = (J1+J2)/2
        IF(X.NE.XM(J)) GO TO 4
        JTAB  = J
        RETURN
4       IF(X.LT.XM(J)) J2 = J
        IF(X.GT.XM(J)) J1 = J
        IF(J2-J1.GT.1) GO TO 3
        JTAB  = J1
        RETURN
        E N D
*CMZ :          18/02/99  14.40.01  by  Valeri Saveliev
*-- Author :
C-----------------------------------------------------------
        FUNCTION T A B L O G (X,N,XM,YM)
C
C... *** EXTRAC FROM TABLE BY USE LOG-LINEAR EXTRAPOLATION ***
C
*       DIMENSION XM(20),YM(20)
        DIMENSION XM(30),YM(30)
        INTEGER K,K1,K2,N
C...
        TABLOG = 0.
        IF(N.LE.0) RETURN
        K1     = 1
        K2     = N
1       K      = INT((K1+K2)/2.)
*       WRITE(6,*)' K= ',X,K,K1,K2
        IF(X.NE.XM(K)) GO TO 2
        TABLOG = YM(K)
        RETURN
2       IF(X.LT.XM(K)) K2 = K
        IF(X.GT.XM(K)) K1 = K
        IF(K2-K1.GT.1) GO TO 1
        Y1     = ALOG(YM(K1))
        Y2     = ALOG(YM(K2))
        X1     = ALOG(XM(K1))
        X2     = ALOG(XM(K2))
C...
        X0     = ALOG(X)
        Y      = Y1+(Y2-Y1)/(X2-X1)*(X0-X1)
        TABLOG = EXP(Y)
        RETURN
        E N D
*CMZ :          19/03/99  13.30.03  by  Valeri Saveliev
*-- Author :
C----------------------------------------------------------
        SUBROUTINE INDETRA
        implicit none
C
C... *** Input information from DETRA1,DETRA2,DETRA3 files ***
C
#include "trdf.h"
#include "mfield.h"
         real ac,zc,roc
         integer nnwe,k,i,ig

*>>>>>>> T.S. 2/2003  Xe/CO2 Gas-Density should be DATA-CARD !!!! 
         REAL       Gdens
         PARAMETER (Gdens=0.00446) ! Default 0.00446 g/cm**3
*<<<<<<< ---------------------------
C
C
C...read mumber of collision per unit length for gas of straw
C

         OPEN (21,FILE=amsdatadir(1:amsdlength)//'trd.aachen.v2.dat',
     +          FORM='FORMATTED',STATUS='OLD')


         READ (21,*) AC,ZC,ROC,NWE,WE,ANL,GAME
         READ (21,*) TDATA,WT,AMUTT,FOIL,GAS,GINWM,DNM,WTR
         READ (21,*) YTSYN,FTSYN
         CLOSE(21)
         CLOSE(21)
*        print *,'*AC  *',ac
*        print *,'*ZC  *',zc
*        print *,'*ROC *',roc
*        print *,'*NWE *',nwe
*        print *,'*WE  *',we
*        print *,'*ANL *',anl
*        print *,'*GAME*',game
C-----------------------------------------------------------------------
C...


*        WRITE(6,'(A14,21E10.3)') 'GAME', (GAME(i),i=1,21)
*        WRITE(6,'(A10,21E10.3)') 'WE'
        DO K=1,NWE
*          WRITE(6,'(E10.3,TR4,21E10.3)') WE(K),(ANL(K,i),i=1,21)
        ENDDO

        NNWE     = 0
        DO 2 K   = 1,NWE
        WE(K)    = ALOG(WE(K))
        DO 1 I   = 1,21
        IF(ANL(K,I).LE.0.) GO TO 3
        ANL(K,I) = ALOG(ANL(K,I))
1         CONTINUE
        NNWE     = NNWE+1
2         CONTINUE
3       NWE      = NNWE
C...
        DO 4 I   = 1,21
        GAME(I)  = ALOG(GAME(I))
4         CONTINUE
C
C...read TR and absorbtion tables
C
*        print *,'*TDATA*',tdata
*        print *,'*WT   *',wt
*        print *,'*AMUTT*',amutt
c         print *,'*FOIL *',foil
c         print *,'*GAS  *',gas
*        print *,'*GINWM*',ginwm
*        print *,'*DNM  *',dnm
*        print *,'*WTR  *',wtr
C...

*        WRITE(6,'(A14,20E10.3)') 'GINWM', (GINWM(i),i=1,20)
*        WRITE(6,'(A10)')         'WTR  '
        DO k=1,81
*          WRITE(6,'(E10.3,TR4,20E10.3)') WTR(k),(DNM(k,i),i=1,20)
        ENDDO


        DO 5 IG   = 1,20
        GINWM(IG) = ALOG(GINWM(IG))
        DO 5 K    = 1,81
        IF(IG.EQ.1) WTR(K) = ALOG(WTR(K))
        DNM(K,IG) = ALOG(DNM(K,IG))
5         CONTINUE

*>>>>>> T.S. 2/2003
        TDATA(1,4) = Gdens         ! XE/CO2 NOMINAL GAS DENSITY 0.00446 g/cm**3
*<<<<<< ------------------


C
C...read tables for the synchrotron radiation simulation
C
*        print *,'*YTSIN*',ytsin
*        print *,'*FTSYN*',ftsin

C...
        RETURN
        E N D
*CMZ :          02/08/99  19.30.02  by  AMS TRD: Kirn,Raupach,Siedenburg


*>>>>>>>>>>>>>>>>>>>>>>>>new routine DIFFSCAT             T.S. 3/2003

*CMZ :          04/03/2003  14.00.57  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :    AMS TRD: Kirn,Raupach,Siedenburg   13/02/2003

      SUBROUTINE DIFFSCAT(IM)

      IMPLICIT NONE

#include "geant321/gctrak.inc"
#include "geant321/gcbank.inc"
#include "geant321/gckine.inc"
#include "geant321/gcking.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcvolu.inc"
#include "geant321/gccuts.inc"
#include "geant321/gcsets.inc"
#include "geant321/gclist.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcnum.inc"
#include "trdf.h"

      INTEGER IM

      REAL       Epi
      PARAMETER (Epi=0.05)  ! Pion Energy 50 MeV

      REAL     X0,X,dens,sq3a,RNDM(1)
      INTEGER  I

      INTEGER      IMECH
      CHARACTER*4  CMECH
      EQUIVALENCE (IMECH,CMECH)

*--------------------------------------------------------------


      IF( STEP.le.0 )                      RETURN
      IF( IPART.ne.14 .and. IPART.ne.15 )  RETURN  ! p,pbar only
      IF( GEKIN.le.90.0 )                  RETURN  ! above 90 GeV

C Determine which material and the density of it

      if(     IM.eq. 1) then   ! Radiator
        dens = 0.06            ! density in g/cm**3
        sq3a = 2.2             ! A**(1/3)
      elseif( IM.eq. 2) then   ! Straw-Wall (Kapton)
        dens = TDATA(1,3)
        sq3a = 2.3
      elseif( IM.eq. 3.) then  ! Gas (Xe/CO2)
        dens = TDATA(1,4)
        sq3a = 4.5
      elseif( IM.eq. 4) then   ! Anode-Wire (Tungsten)
        dens = 19.3
        sq3a = 5.686
      elseif( IM.eq. 5) then   ! Air (-> Vacuum)
        GOTO 999
      elseif( IM.eq. 6) then   ! Vacuum
        GOTO 999
      elseif( IM.eq. 8) then   ! Parylene-C
        dens = 1.29
        sq3a = 2.3
      elseif( IM.eq. 9) then   ! Aluminum
        dens = 2.70
        sq3a = 2.999
      elseif( IM.eq.10) then   ! Copper
        dens = 8.96
        sq3a = 3.990
      else
        write(6,*)'DIFFSCAT: unknown medium IM ',IM
        GOTO 999
      endif		

C     DDIS.Int.Length in cm for A,rho, proton-energy GEKIN/GeV
*            NIL   A**1/3   rho    scale-factor-parametrization
      X0 =   35.0 * sq3a / dens * (exp(80.-15.*log(GEKIN))+2.5)

      CALL GRNDM(RNDM,1)

      X = -X0*log(RNDM(1))

      IF( STEP.gt.X ) THEN             ! DDIS Interaction

         WRITE(6,'(A,I8,I3,3F8.1,TR4,3F10.2)')
     >   'DIFFSCAT: IDEVT,IM,GEKIN/GeV,X/mu,STEP/mu,POS/cm '
     >                     ,IDEVT,IM,GEKIN,X*1e4,STEP*1e4
     >                     ,(VECT(I),I=1,3)

        CMECH = 'DDIS'
        KCASE = IMECH

        DO I = 1,2
          IF( NGKINE.lt.MXGKIN ) THEN
            NGKINE         = NGKINE+1
            GKIN(1,NGKINE) = (VECT(4))
            GKIN(2,NGKINE) = (VECT(5))
            GKIN(3,NGKINE) = (VECT(6))
            GKIN(4,NGKINE) = Epi
            GKIN(5,NGKINE) = 7+I  ! 8=PI+ 9=PI-
           
           
            GPOS(1,NGKINE) = VECT(1)
            GPOS(2,NGKINE) = VECT(2)
            GPOS(3,NGKINE) = VECT(3)
           
            TOFD(NGKINE)  = 0.
            IFLGK(NGKINE) = 0
           
            GEKIN = GEKIN - Epi
            GETOT = GETOT - Epi
          ENDIF
        ENDDO	

      ENDIF

 999  CONTINUE
      RETURN
      END

