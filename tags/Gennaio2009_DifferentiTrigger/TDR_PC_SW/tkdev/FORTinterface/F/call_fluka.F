*
*  Interface to fluka  V. Choutko 26-may-2000
*
            subroutine call_fluka(a_p,z_p,a_t,z_t,ekin,
     +      p_mass,nout,xpout,ierr)
            real *4 a_p,z_p,a_t,z_t,ekin,p_mass,t_mass
            parameter (maxout=10000)
            real *4 xpout(6,*)

            real *8 energy,beta,gamma,momentum,
     +      e_p,p_p,e_t,p_t

#include "geant321/dimpar.inc"
#include "geant321/gtflag.inc"
#include "geant321/gcflag.inc"
 
#if !defined(CERNLIB_SINGLE)
#include "geant321/finuct.inc"
#endif
#include "geant321/finuc.inc"
      REAL RNDM(1)
#if !defined(CERNLIB_SINGLE)
#include "geant321/mapat.inc"
#endif
#include "geant321/mapa.inc"
#if !defined(CERNLIB_SINGLE)
#include "geant321/part2t.inc"
#endif
#include "geant321/part2.inc"
#if !defined(CERNLIB_SINGLE)
#include "geant321/comcont.inc"
#endif
#include "geant321/comcon.inc"
#if !defined(CERNLIB_SINGLE)
#include "geant321/fheavyt.inc"
#endif
#include "geant321/fheavy.inc"
#include "geant321/paprop.inc"
#if !defined(CERNLIB_SINGLE)
#include "geant321/papropt.inc"
#endif
#include "geant321/gfkdis.inc"
#if !defined(CERNLIB_SINGLE)
      DOUBLE PRECISION POO,EKE,TXI,TYI,TZI,AMM,WE,ONE,PGEANT,DMOD
#endif
      PARAMETER (ONE=1)
      DIMENSION IGTOFL(49),IFLTOG(39),IHVTOG(6),ZSAMP(50)
      real GMASSP(39),GMASSH(6)
      DATA IGTOFL / 0, 0, 0, 0, 0, 0,23,13,14,12, 15,16, 8, 1, 2,19, 0,
     +17,21,22, 20, 34, 36, 38, 9,18, 31, 32, 33, 35, 37, 39, 17*0/
           
      DATA IFLTOG /14,15, 3, 2, 4, 4, 1,13,25, 5, 6,10, 8, 9,11,12,18,
     +26,16,21, 19,20, 7, 7*0, 27, 28, 29, 22, 30, 23, 31, 24, 32/
      DATA IHVTOG /13,14,45,46,49,47/

      data gmassp/2*0.93827,2*0.000511,2*0,
     + 0,2*0.93956,2*0.10565,0.4976,2*0.13957,2*0.4937,2*1.116,
     + 0.4976,1.197,1.189,1.193,0.135,7*0.,
     + 1.189,1.193,1.197,2*1.1315,2*1.32,2*1.672/
      data gmassh/0.93956,0.93827,1.8756,2.809,2.809,3.7274/
*
      NP = 0
      NPHEAV = 0

            ierr=0
c            if(a_t.ge.1.5.or.z_t.ge.1.5)then
c             write(*,*)'call_fluka-F-WrongTarget ',a_t,p_t
c             stop
c            endif
      IF (IFINIT(5) .EQ. 0) CALL FLINIT
      INT=0
      IJ=IGTOFL(14)
      t_mass=am(ij)
*
*  First swap projectile/target
*
      energy=ekin*a_p
      momentum=sqrt(energy*(energy+2*p_mass))
      energy=energy+p_mass
      beta=momentum/energy
      if(beta.lt.1.)then
       gamma=1/sqrt(1-beta**2)
      else
       write(*,*)'call-fluka-E-wrong beta ',beta
       ierr=1
       goto 999
      endif
*
* boost along Z
*      
      
      e_t=gamma*t_mass
      p_t=gamma*(-beta*t_mass)
c      e_p=gamma*(energy-beta*momentum)
c      p_p=gamma*(-beta*energy+momentum)  
      we=1.
      TXI=0
      TYI=0
      TZI=-1
      POO=abs(p_t)
      EKE=sqrt(p_t**2+t_mass**2)-t_mass
            AMSS(1)   = a_p
            MSSNUM(1) = 0
            ZTAR(1)   = z_p
            RHO(1)    = 1
      CALL EVENTV(IJ,POO,EKE,TXI,TYI,TZI,WE,1)
         NSTAK1 = MIN(NP,maxout)
         IF(NP.GT.NSTAK1) THEN
            WRITE(*,10000) NP-nstak1
         ENDIF
         NOUT=0
         DO 90  K=1,NSTAK1
            NOUT = NOUT + 1
            IF (KPART(K) .EQ. 24 .OR. KPART(K) .EQ. 25) THEN
               KPART(K) = 19
               CALL GRNDM(RNDM,1)
               IF (RNDM(1) .GT. 0.5) KPART(K) = 12
            END IF
            IGEPAR = IFLTOG(KPART(K))
            AGEMAS = GMASSP(KPART(K))
            PGEANT = SQRT(TKI(K)*(TKI(K)+2*AGEMAS))
            xpout(1,NOUT)=CXR(K)*PGEANT
            xpout(2,NOUT)=CYR(K)*PGEANT
            xpout(3,NOUT)=CZR(K)*PGEANT
            xpout(4,NOUT)=TKI(K)+AGEMAS
            xpout(5,NOUT)=IGEPAR
            xpout(6,NOUT)=agemas
   90    CONTINUE

         NSTAK2 = MIN(NPHEAV,maxout-NOUT)
         IF(NPHEAV.GT.NSTAK2) THEN
            WRITE(*,10100) NPHEAV-NSTAK2
         ENDIF
         DO 100 K=1,NSTAK2
            NOUT = NOUT + 1
            IGEPAR = IHVTOG(KHEAVY(K))
            AGEMAS = GMASSH(KHEAVY(K))
            PGEANT = SQRT(TKHEAV(K)*(TKHEAV(K)+2*AGEMAS))
            xpout(1,NOUT)=CXHEAV(K)*PGEANT
            xpout(2,NOUT)=CYHEAV(K)*PGEANT
            xpout(3,NOUT)=CZHEAV(K)*PGEANT
            xpout(4,NOUT)=TKHEAV(K)+AGEMAS
            xpout(5,NOUT)=IGEPAR
            xpout(6,NOUT)=agemas
  100    CONTINUE

*
*  transform Back
*
         do i=1,nout
           momentum=xpout(3,i)
           energy=xpout(4,i)
           xpout(4,i)=gamma*(energy+beta*momentum)
           xpout(3,i)=gamma*(+beta*energy+momentum)  
         enddo
      
10000 FORMAT(' **** call_fluka: Stack overflow, ',I6,' particles lost')
10100 FORMAT(' **** call_fluka: Stack overflow, ',I6,
     +' heavy particles lost')
 999  end
