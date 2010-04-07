*  $Id: ggckov.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*
* $Id: ggckov.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*
* $Log: ggckov.F,v $
* Revision 1.1.1.1  2007/11/13 09:56:10  zuccon
* First Import
*
* Revision 1.3  2007/07/24 18:19:13  mdelgado
* Added tile refractive index inhomogeneity in RICH simulation/reconstruction
*
* Revision 1.2  2001/01/22 17:32:24  choutko
* prepend 1st line
*
* Revision 1.1  2000/09/19 07:48:03  mdelgado
* Geant3 problem with large number of Cerenkov photons solved.
*
* Revision 1.1.1.1  1995/10/24 10:21:41  cernlib
* Geant
*
*
#include "geant321/pilot.h"
*CMZ :  3.21/02 29/03/94  15.41.23  by  S.Giani
*-- Author :
      SUBROUTINE GGCKOV
C.
C.    ******************************************************************
C.    *                                                                *
C.    *   This routine is called for each tracking step of a charged   *
C.    *   particle in a radiator. A Poisson-distributed number of      *
C.    *   photons is generated according to the Cherenkov formula,     *
C.    *   distributed evenly along the track segment and uniformly     *
C.    *   azimuth w.r.t. the particle direction. The parameters are    *
C.    *   then transformed into the Master Reference System, and they  *
C.    *   are added to the particle stack.                             *
C.    *                                                                *
C.    *   ==>Called by : GTMUON, GTHADR, GTELEC                        *
C.    *      Authors     R.Jones, F.Carminati ********                 *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcjloc.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcunit.inc"
#include "geant321/gctrak.inc"
#include "geant321/gckine.inc"
#include "geant321/gcking.inc"
#include "geant321/gconsp.inc"

*
      REAL RPHOT(3)
      LOGICAL ROTATE
      PARAMETER (RFACT=369.81E9)

*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
      integer testradiator              ! Rich Added: test if NUMED is a RICH radiator
      real getphotons                   ! Rich Added: get a photon using RICH rad. parameters
      real getmomentum                  ! Rich Added: sample photon momentum
      real mean,ppos(3)
      real mystep
*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

*
*   ------------------------------------------------------------------
*
* *** See whether we generate at least one photon
*
C     THRIND = GETOT/VECT(7)
C     IF(Q(JINDEX+NPCKOV).LT.THRIND) THEN
C        GO TO 999
C     ELSEIF(Q(JINDEX+1).GE.THRIND) THEN
C        PMIN  = Q(JTCKOV+2)
C        DP    = Q(JTCKOV+NPCKOV+1)-PMIN
C        GE    = Q(JCURIN+NPCKOV)
C        JMIN  = 1
C     ELSE
C        JMIN = 1
C        JMAX = NPCKOV
C  10    JMED = (JMIN+JMAX)/2
C        IF(Q(JINDEX+JMED).LT.THRIND) THEN
C           JMIN = JMED
C        ELSE
C           JMAX = JMED
C        ENDIF
C        IF(JMAX-JMIN.GT.1) GO TO 10
C        RATIO =
C    +   (THRIND-Q(JINDEX+JMIN))/(Q(JINDEX+JMIN+1)-Q(JINDEX+JMIN))
C        RATI1 = 1.-RATIO
C        PMIN  = Q(JTCKOV+JMIN+1)*RATI1+Q(JTCKOV+JMIN+2)*RATIO
C        DP    = Q(JTCKOV+NPCKOV+1)-PMIN
C        GEMIN = Q(JCURIN+JMIN)*RATI1+Q(JCURIN+JMIN+1)*RATIO
C        GE    = Q(JCURIN+NPCKOV)-GEMIN
C     ENDIF
C     DNDL = RFACT*(CHARGE**2)*(DP-GE*THRIND**2)

*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> RICH ADDED
      IF(testradiator(NUMED).ne.0) then
*     Get number of photons to be generated
         THRIND = GETOT/VECT(7)         

c$$$         if(ignext.ne.0) then
c$$$            mystep=STEP-PREC
c$$$            ppos(1)=VECT(1)-PREC*VECT(4)
c$$$            ppos(2)=VECT(2)-PREC*VECT(5)
c$$$            ppos(3)=VECT(3)-PREC*VECT(6)
c$$$         else
c$$$            mystep=STEP
c$$$            ppos(1)=VECT(1)
c$$$            ppos(2)=VECT(2)
c$$$            ppos(3)=VECT(3)
c$$$         endif
c$$$         mean=getphotons(CHARGE,THRIND,ppos,mystep)
         mean=getphotons(CHARGE,THRIND,VECT,STEP)
         IF(mean.eq.0) goto 999
         
         CALL GPOISS(mean,NGPHOT,1)

         IF(NGPHOT.EQ.0) GOTO 999
         
         IF(NGPHOT.GT.MXPHOT) THEN
            WRITE(CHMAIL,10000) NGPHOT-MXPHOT
10000       FORMAT(' **** GGCKOV Overflow in the photon stack, ',I10,
     +           ' photons are lost')
            CALL GMAIL(0,0)
            NGPHOT = MXPHOT    
         ENDIF
         
* Prepare rotation
         CALL GFANG(VECT(4),COSTH,SINTH,COSPH,SINPH,ROTATE)
         
         DO J=1,NGPHOT
            CALL GRNDM(RPHOT, 2)
            
*     Distribute the photons in origin
            IF(IGNEXT.NE.0) THEN
               DS=(STEP-PREC)*RPHOT(1)+PREC
            ELSE
               DS = STEP*RPHOT(1)
            ENDIF
            XPHOT(1,J) = VECT(1)-VECT(4)*DS
            XPHOT(2,J) = VECT(2)-VECT(5)*DS
            XPHOT(3,J) = VECT(3)-VECT(6)*DS
            XPHOT(11,J)= TOFG+(STEP-DS)*GETOT/(VECT(7)*CLIGHT)

            ppos(1)=XPHOT(1,J)
            ppos(2)=XPHOT(2,J)
            ppos(3)=XPHOT(3,J)

* Sample the momentum; this seems to give problems
            PPHOT=getmomentum(RINDEX)  ! This has to be checked

* Assign direction
            COST   = THRIND/RINDEX          ! 1/n/beta
            SINT2  = (1.-COST)*(1.+COST)
            SINT = SQRT(SINT2)
            PHI  = TWOPI*RPHOT(2)
            SINP = SIN(PHI)
            COSP = COS(PHI)
            XPHOT(4,J) = SINT*COSP
            XPHOT(5,J) = SINT*SINP
            XPHOT(6,J) = COST
            XPHOT(7,J) = PPHOT
            XPHOT(8,J) = COST*COSP
            XPHOT(9,J) = COST*SINP
            XPHOT(10,J) = -SINT
*     
            IF(ROTATE) THEN
               CALL GDROT(XPHOT(8,J),COSTH,SINTH,COSPH,SINPH)
               CALL GDROT(XPHOT(4,J),COSTH,SINTH,COSPH,SINPH)
            ENDIF
         ENDDO

         GOTO 999
      endif
*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

      IF(ITRTYP.NE.4.AND.ITRTYP.NE.8) CALL GNCKOV
      CALL GPOISS(DNDL*STEP,NGPHOT,1)
      IF(NGPHOT.EQ.0) THEN
         GO TO 999
      ELSEIF(NGPHOT.GT.MXPHOT) THEN
        WRITE(CHMAIL,11000) NGPHOT-MXPHOT
11000   FORMAT(' **** GGCKOV Overflow in the photon stack, ',I10,
     +         ' photons are lost')
        CALL GMAIL(0,0)
        NGPHOT = MXPHOT    
      ENDIF
*
* ***  Set up rotation to Particle frame
*
      CALL GFANG(VECT(4),COSTH,SINTH,COSPH,SINPH,ROTATE)
*
* ***  Distribute the photons in origin, direction, momentum
      COSMX  = THRIND/Q(JINDEX+NPCKOV)
      SINMX2 = (1.-COSMX)*(1.+COSMX)
      DO 40 J=1,NGPHOT
         CALL GRNDM(RPHOT, 1)
         IF(IGNEXT.NE.0) THEN
            DS=(STEP-PREC)*RPHOT(1)+PREC
         ELSE
            DS = STEP*RPHOT(1)
         ENDIF
         XPHOT(1,J) = VECT(1)-VECT(4)*DS
         XPHOT(2,J) = VECT(2)-VECT(5)*DS
         XPHOT(3,J) = VECT(3)-VECT(6)*DS
         XPHOT(11,J)= TOFG+(STEP-DS)*GETOT/(VECT(7)*CLIGHT)
*
* *** Sample the momentum of the photon
   20    CALL GRNDM(RPHOT, 3)
         PPHOT=PMIN+RPHOT(1)*DP
*
* *** Find in which bin we are
         KMIN = JMIN
         KMAX = NPCKOV
   30    KMED = (KMIN+KMAX)/2
         IF(Q(JTCKOV+1+KMED).LT.PPHOT) THEN
            KMIN = KMED
         ELSE
            KMAX = KMED
         ENDIF
         IF(KMAX-KMIN.GT.1) GOTO 30
         RATIO = (PPHOT-Q(JTCKOV+1+KMIN))/
     +           (Q(JTCKOV+KMIN+2)-Q(JTCKOV+1+KMIN))
         RATI1  = (1.-RATIO)
*
* *** Find the density function value corresponding to the
* *** momentum sampled
         RINDEX = Q(JINDEX+KMIN)*RATI1+Q(JINDEX+KMIN+1)*RATIO
         COST   = THRIND/RINDEX
         SINT2  = (1.-COST)*(1.+COST)
*
* *** Perform hit-and-miss
         IF(RPHOT(2)*SINMX2.GT.SINT2) GO TO 20
         SINT = SQRT(SINT2)
         PHI  = TWOPI*RPHOT(3)
         SINP = SIN(PHI)
         COSP = COS(PHI)
         XPHOT(4,J) = SINT*COSP
         XPHOT(5,J) = SINT*SINP
         XPHOT(6,J) = COST
         XPHOT(7,J) = PPHOT
         XPHOT(8,J) = COST*COSP
         XPHOT(9,J) = COST*SINP
         XPHOT(10,J) = -SINT
*
         IF(ROTATE) THEN
            CALL GDROT(XPHOT(8,J),COSTH,SINTH,COSPH,SINPH)
            CALL GDROT(XPHOT(4,J),COSTH,SINTH,COSPH,SINPH)
         ENDIF
   40 CONTINUE
  999 END









