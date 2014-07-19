* $Id: fint_my.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*
* $Log: fint_my.F,v $
* Revision 1.1.1.1  2007/11/13 09:56:10  zuccon
* First Import
*
* Revision 1.1  2002/01/11 16:09:42  choutko
* lift after gprof
*
* Revision 1.1.1.1  1996/02/15 17:48:36  mclareni
* Kernlib
*
*
#include "kernnum/pilot.h"
          subroutine  FINT_my(NARG,ARG,NENT,ENT,
     +    TABLE1,table2,table3,
     +    fint,lhint)
C
C   INTERPOLATION ROUTINE. AUTHOR C. LETERTRE.
C   MODIFIED BY B. SCHORR, 1.07.1982.
C
          INTEGER   NENT(9)
          REAL      ARG(9),ENT(9),TABLE1(9),table2(9),table3(9)
          real fint(3)
          integer lhint(3)
          INTEGER   INDEX(32)
          REAL      WEIGHT(32)
          LOGICAL   MFLAG,    RFLAG
          FINT(1)  =  0.
          FINT(2)  =  0.
          FINT(3)  =  0.
          IF(NARG .LT. 1  .OR.  NARG .GT. 5)  GOTO 300
          LMAX      =  0
          ISTEP     =  1
          KNOTS     =  1
          INDEX(1)  =  1
          WEIGHT(1) =  1.
          DO 100    N  =  1, NARG
             X     =  ARG(N)
             NDIM  =  NENT(N)
             LOCA  =  LMAX
             LMIN  =  LMAX + 1
             LMAX  =  LMAX + NDIM
             IF(NDIM .GT. 2)  GOTO 10
             IF(NDIM .EQ. 1)  GOTO 100
             H  =  X - ENT(LMIN)
             IF(H .EQ. 0.)  GOTO 90
             ISHIFT  =  ISTEP
             IF(X-ENT(LMIN+1) .EQ. 0.)  GOTO 21
             ISHIFT  =  0
             ETA     =  H / (ENT(LMIN+1) - ENT(LMIN))
             GOTO 30
  10         LOCB  =  LMAX + 1
             if(lhint(n).gt.0)then
              if(x.gt.ent(lhint(n)).and.x.lt.ent(lhint(n)+1))then 
               loca=lhint(n)-1
               locb=loca+2
              else
               lhint(n)=0
              endif 
             endif
  11         LOCC  =  (LOCA+LOCB) / 2
             IF(X-ENT(LOCC))  12, 20, 13
  12         LOCB  =  LOCC
             GOTO 14
  13         LOCA  =  LOCC
  14         IF(LOCB-LOCA .GT. 1)  GOTO 11
             LOCA    =  MIN0( MAX0(LOCA,LMIN), LMAX-1 )
             lhint(n)=loca
             ISHIFT  =  (LOCA - LMIN) * ISTEP
             ETA     =  (X - ENT(LOCA))/ (ENT(LOCA+1) - ENT(LOCA))
             GOTO 30
  20         ISHIFT  =  (LOCC - LMIN) * ISTEP
  21         DO 22  K  =  1, KNOTS
                INDEX(K)  =  INDEX(K) + ISHIFT
  22            CONTINUE
             GOTO 90
  30         DO 31  K  =  1, KNOTS
                INDEX(K)         =  INDEX(K) + ISHIFT
                INDEX(K+KNOTS)   =  INDEX(K) + ISTEP
                WEIGHT(K+KNOTS)  =  WEIGHT(K) * ETA
                WEIGHT(K)        =  WEIGHT(K) - WEIGHT(K+KNOTS)
  31            CONTINUE
             KNOTS  =  2*KNOTS
  90         ISTEP  =  ISTEP * NDIM
 100         CONTINUE
          DO 200    K  =  1, KNOTS
             I  =  INDEX(K)
             FINT(1)  =  FINT(1) + WEIGHT(K) * TABLE1(I)
             FINT(2)  =  FINT(2) + WEIGHT(K) * TABLE2(I)
             FINT(3)  =  FINT(3) + WEIGHT(K) * TABLE3(I)
 200         CONTINUE
          RETURN
 300      CALL KERMTR('E104.1',LGFILE,MFLAG,RFLAG)
          IF(MFLAG) THEN
             IF(LGFILE .EQ. 0) THEN
                WRITE(*,1000) NARG
             ELSE
                WRITE(LGFILE,1000) NARG
             ENDIF
          ENDIF
          IF(.NOT. RFLAG) CALL ABEND
          RETURN
1000      FORMAT( 7X, 24HFUNCTION FINT ... NARG =,I6,
     +              17H NOT WITHIN RANGE)
          END
          subroutine  FINT_my_2(NARG,ARG,NENT,ENT,TABLE1,table2,table3,
     +     table4,table5,table6,
     +    fint,lhint)
C
C   INTERPOLATION ROUTINE. AUTHOR C. LETERTRE.
C   MODIFIED BY B. SCHORR, 1.07.1982.
C
          INTEGER   NENT(9)
          REAL      ARG(9),ENT(9),TABLE1(9),table2(9),table3(9),
     +    table4(9),table5(9),table6(9)
          real fint(6)
          integer lhint(3)
          INTEGER   INDEX(32)
          REAL      WEIGHT(32)
          LOGICAL   MFLAG,    RFLAG
          FINT(1)  =  0.
          FINT(2)  =  0.
          FINT(3)  =  0.
          FINT(4)  =  0.
          FINT(5)  =  0.
          FINT(6)  =  0.
          IF(NARG .LT. 1  .OR.  NARG .GT. 5)  GOTO 300
          LMAX      =  0
          ISTEP     =  1
          KNOTS     =  1
          INDEX(1)  =  1
          WEIGHT(1) =  1.
          DO 100    N  =  1, NARG
             X     =  ARG(N)
             NDIM  =  NENT(N)
             LOCA  =  LMAX
             LMIN  =  LMAX + 1
             LMAX  =  LMAX + NDIM
             IF(NDIM .GT. 2)  GOTO 10
             IF(NDIM .EQ. 1)  GOTO 100
             H  =  X - ENT(LMIN)
             IF(H .EQ. 0.)  GOTO 90
             ISHIFT  =  ISTEP
             IF(X-ENT(LMIN+1) .EQ. 0.)  GOTO 21
             ISHIFT  =  0
             ETA     =  H / (ENT(LMIN+1) - ENT(LMIN))
             GOTO 30
  10         LOCB  =  LMAX + 1
             if(lhint(n).gt.0)then
              if(x.gt.ent(lhint(n)).and.x.lt.ent(lhint(n)+1))then 
               loca=lhint(n)-1
               locb=loca+2
              else
               lhint(n)=0
              endif 
             endif
  11         LOCC  =  (LOCA+LOCB) / 2
             IF(X-ENT(LOCC))  12, 20, 13
  12         LOCB  =  LOCC
             GOTO 14
  13         LOCA  =  LOCC
  14         IF(LOCB-LOCA .GT. 1)  GOTO 11
             LOCA    =  MIN0( MAX0(LOCA,LMIN), LMAX-1 )
             lhint(n)=loca 
             ISHIFT  =  (LOCA - LMIN) * ISTEP
             ETA     =  (X - ENT(LOCA)) / (ENT(LOCA+1) - ENT(LOCA))
             GOTO 30
  20         ISHIFT  =  (LOCC - LMIN) * ISTEP
  21         DO 22  K  =  1, KNOTS
                INDEX(K)  =  INDEX(K) + ISHIFT
  22            CONTINUE
             GOTO 90
  30         DO 31  K  =  1, KNOTS
                INDEX(K)         =  INDEX(K) + ISHIFT
                INDEX(K+KNOTS)   =  INDEX(K) + ISTEP
                WEIGHT(K+KNOTS)  =  WEIGHT(K) * ETA
                WEIGHT(K)        =  WEIGHT(K) - WEIGHT(K+KNOTS)
  31            CONTINUE
             KNOTS  =  2*KNOTS
  90         ISTEP  =  ISTEP * NDIM
 100         CONTINUE
          DO 200    K  =  1, KNOTS
             I  =  INDEX(K)
             FINT(1)  =  FINT(1) + WEIGHT(K) * TABLE1(I)
             FINT(2)  =  FINT(2) + WEIGHT(K) * TABLE2(I)
             FINT(3)  =  FINT(3) + WEIGHT(K) * TABLE3(I)
             FINT(4)  =  FINT(4) + WEIGHT(K) * TABLE4(I)
             FINT(5)  =  FINT(5) + WEIGHT(K) * TABLE5(I)
             FINT(6)  =  FINT(6) + WEIGHT(K) * TABLE6(I)
 200         CONTINUE
          RETURN
 300      CALL KERMTR('E104.1',LGFILE,MFLAG,RFLAG)
          IF(MFLAG) THEN
             IF(LGFILE .EQ. 0) THEN
                WRITE(*,1000) NARG
             ELSE
                WRITE(LGFILE,1000) NARG
             ENDIF
          ENDIF
          IF(.NOT. RFLAG) CALL ABEND
          RETURN
1000      FORMAT( 7X, 24HFUNCTION FINT ... NARG =,I6,
     +              17H NOT WITHIN RANGE)
          END
