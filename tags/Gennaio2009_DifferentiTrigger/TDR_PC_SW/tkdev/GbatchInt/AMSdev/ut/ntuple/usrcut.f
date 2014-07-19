*  $Id: usrcut.f,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
      SUBROUTINE USRINIT
**************************************************************************
* Do user initialization such as histgram booking
**************************************************************************
      INCLUDE 'cutstat.inc'
*
*-- Reset the counters for cuts
      DO I=0,NCUT
        NPASS(I) = 0
      ENDDO
*
*-- Book histgrams
C      CALL HBOOK1(11,  'NPART', 12, -1, 5, 0.)
C      CALL HBOOK1(12,  'NTRTR', 12, -1, 5, 0.)
C      CALL HBOOK1(13,  'NBETA', 12, -1, 5, 0.)
C      CALL HBOOK1(103, '[b](v/c)', 120, -1.2, 1.2, 0.)
* 
       call HROPEN(44,'output','posn.hbk','NP',1024,iostat)      
      RETURN
      END

C**------------- BLOCK DATA VARDATA ---------------------------------C
      BLOCK DATA VARDATA
*------------------------------------------------------------
* put the variable names below which are used in the function
*
* In case of NVAR=0, all variables will be read
*-----------------------------------------------------------
      INCLUDE 'usrcom.inc'
*-user may change following
      DATA NVAR/0/, VARLIS(1)/'EVENTSTATUS'/
C      DATA NVAR/5/,VARLIS/
C     &  'NBETA','BETA', 'NPART', 'NTRTR', 'NHITS', 217*' '/
*-end of user change
       END

      INTEGER FUNCTION USRCUT(IOPT)
*      REAL FUNCTION USRCUT()
**************************************************************************
* USER's CUT here
*
* Input:
*        IOPT  < 0, do initialization
*              = 0, event processing
*              > 0, do termination such as printing
*
* Return:
*        1, survives     
*        0, fails
*
* Usage:
*        1) In "BLOCK DATA VARDATA", change/add the list of variables in
*           VARS which are used in the function, NVAR is the total number
*           of variables used. Set NVAR=0 to enable reading all variables
*        2) set the value of this function, 1=survive, 0=fail
*
**************************************************************************
      
      INCLUDE 'cwncom.inc'
      INCLUDE 'usrcom.inc'
      INCLUDE 'cutstat.inc'
       integer aprf
      INTEGER IOPT
*
      USRCUT = 0
*
      IF (IOPT .LT. 0) THEN
         CALL USRINIT
         GOTO 990
      ELSE IF (IOPT .GT. 0) THEN
         CALL USREND
         GOTO 990
      ELSE If (IOPT .NE. 0) THEN
         GOTO 990
      ENDIF
*
*-impose cuts here
*
      NPASS(0) = NPASS(0) + 1
      IPASS    = 0


*
*-- Comment the following in case you do not want to write selectd events
*--  into new ntuples
      icut=aprf(-1)
      USRCUT =icut
      if(usrcut.ne.0)then
        notzero=notzero+1
      else
         izero=izero+1
      endif
       if(mod(izero,100000).eq.1)write(*,*)izero,notzero
 990  CONTINUE

      RETURN
      END


      SUBROUTINE USREND
**************************************************************************
* Do user termination such as printing.
**************************************************************************
      INCLUDE 'cutstat.inc'
*
      PRINT *,'Total events processed =',NPASS(0)
      DO I=1,NCUT
        WRITE(6,101) I, NPASS(I)
      ENDDO
      
      call  HCDIR ('//output',' ')
      ICYCL=0
      CALL HROUT (0, ICYCL, ' ')
      CALL HREND ('output')
      close(44)
C      CALL HPRINT(0)
       

      RETURN
 101  FORMAT(2X,'Events survived after cut-',I2,' =',I8)
      END



      integer FUNCTION aprf(isig)
      INCLUDE 'cwncom.inc'
         integer patmiss(6,22)
         data patmiss/                0,0,0,0,0,0,   ! 123456  0
     +                                0,0,0,0,0,5,   ! 12346   1
     +                                0,0,0,0,0,4,   ! 12356   2
     +                                0,0,0,0,0,3,   ! 12456   3
     +                                0,0,0,0,0,2,   ! 13456   4
     +                                0,0,0,0,0,6,   ! 12345   5
     +                                0,0,0,0,0,1,   ! 23456   6
     +                                0,0,0,0,5,6,   ! 1234    7
     +                                0,0,0,0,4,6,   ! 1235    8
     +                                0,0,0,0,4,5,   ! 1236    9
     +                                0,0,0,0,3,6,   ! 1245   10
     +                                0,0,0,0,3,5,   ! 1246   11
     +                                0,0,0,0,3,4,   ! 1256   12
     +                                0,0,0,0,2,6,   ! 1345   13
     +                                0,0,0,0,2,5,   ! 1346   14
     +                                0,0,0,0,2,4,   ! 1356   15
     +                                0,0,0,0,2,3,   ! 1456   16
     +                                0,0,0,0,1,6,   ! 2345   17
     +                                0,0,0,0,1,5,   ! 2346   18
     +                                0,0,0,0,1,4,   ! 2356   19
     +                                0,0,0,0,1,3,   ! 2456   20
     +                                0,0,0,0,1,2/   ! 3456   21

      integer pm
      logical cut
      aprf=0
      
*
*    Basic cuts
*
c      call hf1(2,float(npart),1.)
       if(npart.gt.0.and.pcharge(1).eq.1.and.
     + ptrackp(1).gt.0.and.pmom(1).lt.0)aprf=1
*
      END



      function xsign(x)
       xsign=1.
       if(x.lt.0)xsign=-1.
      end      
      function frp(x)
       frp=9*sqrt(0.42e-6*exp(-x*0.037)+0.46e-7/x/x)
      end
      function frt(x)
       frt=9*sqrt(0.2e-7*exp(-x*0.037)+0.46e-7/x/x)
      end
