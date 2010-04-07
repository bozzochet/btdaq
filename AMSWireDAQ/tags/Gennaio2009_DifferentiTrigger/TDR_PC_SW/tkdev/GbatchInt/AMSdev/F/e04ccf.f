*  $Id: e04ccf.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      SUBROUTINE E04CCF(N,X,FMIN,EPS,N1,PDSTAR,PSTAR,PBAR,STEP,Y,P,
     *                  FUNCT,MONIT,MAXIT,IFAIL,dummy)
C     MARK 1 RELEASE.  NAG COPYRIGHT 1971
C     MARK 3 REVISED.
C     MARK 4.5 REISSUE. LER-F7
C     MARK 8 REVISED. IER-220 (MAR 1980)
C     MARK 11.5(F77) REVISED. (SEPT 1985.)
C     MARK 13 REVISED. USE OF MARK 12 X02 FUNCTIONS (APR 1988).
C     .. Parameters ..
      CHARACTER*6       SRNAME
      PARAMETER         (SRNAME='E04CCF')
C     .. Scalar Arguments ..
      DOUBLE PRECISION  EPS, FMIN
      INTEGER           IFAIL, MAXIT, N, N1
C     .. Array Arguments ..
      DOUBLE PRECISION  P(N1,N), PBAR(N), PDSTAR(N), PSTAR(N), STEP(N),
     *                  X(N), Y(N1)
C     .. Subroutine Arguments ..
      EXTERNAL          FUNCT, MONIT
C     .. Local Scalars ..
      DOUBLE PRECISION  A, B, C, CENT, COEFF, DERIV, DERIV2, F1, F2, F3,
     *                  FMAX, R, SERROR, X1, X2, X3, XMIN, YDSTAR,
     *                  YMEAN, YSTAR
      INTEGER           H, I, IV, J, K, L, LASTMX, MCOUNT, NCALL, NP1
      integer dummy(1)
C     .. Local Arrays ..
      CHARACTER*1       P01REC(1)
C     .. External Functions ..
      DOUBLE PRECISION  X02AJF
      INTEGER           P01ABF
      EXTERNAL          X02AJF, P01ABF
C     .. Intrinsic Functions ..
      INTRINSIC         ABS, SIGN, SQRT, DBLE
C     .. Executable Statements ..
      FMAX = 1000.D0
      FMIN = 0.D0
      NCALL = 0
      R = 0
      IF (N.LT.1 .OR. EPS.LT.X02AJF()
     *    .OR. N1.NE.N+1 .OR. MAXIT.LT.1 .OR.
     *    (IFAIL.NE.1 .AND. IFAIL.NE.0)) GO TO 1220
   20 DO 300 I = 1, N
         CALL FUNCT(N,X,F1,dummy)
         NCALL = NCALL + 1
         X1 = 0.D0
         COEFF = 1.D0
         DO 80 J = 1, N
            IF (I-J) 60, 40, 60
   40       PSTAR(J) = X(J) + COEFF
            GO TO 80
   60       PSTAR(J) = X(J)
   80    CONTINUE
         CALL FUNCT(N,PSTAR,F2,dummy)
         NCALL = NCALL + 1
         X2 = 1.D0
         PSTAR(I) = PSTAR(I) + COEFF
         CALL FUNCT(N,PSTAR,F3,dummy)
         NCALL = NCALL + 1
         X3 = 2.D0
  100    IF (NCALL.GT.MAXIT) GO TO 1120
         DERIV = (X2-X3)*F1 + (X3-X1)*F2 + (X1-X2)*F3
         IF (ABS(DERIV)-X02AJF()) 140, 120, 120
  120    DERIV2 = DERIV/(X1-X2)/(X2-X3)/(X3-X1)
         IF (DERIV2) 240, 140, 140
  140    IF (F1-F3) 160, 200, 200
  160    IF (X1.LE.-5.0D0) GO TO 180
         F3 = F2
         X3 = X2
         F2 = F1
         X2 = X1
         X1 = X1 - COEFF
         PSTAR(I) = X(I) + X1
         CALL FUNCT(N,PSTAR,F1,dummy)
         NCALL = NCALL + 1
         GO TO 100
  180    XMIN = -5.0D0
         GO TO 280
  200    IF (X3.GE.5.0D0) GO TO 220
         F1 = F2
         X1 = X2
         F2 = F3
         X2 = X3
         X3 = X3 + COEFF
         PSTAR(I) = X(I) + X3
         CALL FUNCT(N,PSTAR,F3,dummy)
         NCALL = NCALL + 1
         GO TO 100
  220    XMIN = 5.0D0
         GO TO 280
  240    XMIN = .5D0*((X2**2-X3**2)*F1+(X3**2-X1**2)*F2+(X1**2-X2**2)
     *          *F3)/DERIV
         IF (XMIN.NE.0.0D0) GO TO 260
         XMIN = 0.1D0
         GO TO 280
  260    IF (ABS(XMIN).LT.0.1D0) XMIN = SIGN(0.1D0,XMIN)
         IF (ABS(XMIN).GT.5.0D0) XMIN = SIGN(5.0D0,XMIN)
  280    STEP(I) = XMIN
  300 CONTINUE
      NP1 = N + 1
      DO 380 I = 1, NP1
         DO 360 J = 1, N
            IF (I-J-1) 320, 340, 320
  320       PSTAR(J) = X(J)
            P(I,J) = X(J)
            GO TO 360
  340       PSTAR(J) = X(J) + STEP(J)
            P(I,J) = X(J) + STEP(J)
  360    CONTINUE
         NCALL = NCALL + 1
         CALL FUNCT(N,PSTAR,Y(I),dummy)
  380 CONTINUE
      A = 1.D0
      B = .5D0
      C = 2.D0
      LASTMX = 0
      MCOUNT = 0
      K = 0
  400 K = K + 1
      FMAX = Y(1)
      FMIN = Y(1)
      H = 1
      L = 1
      DO 480 I = 2, N1
         IF (Y(I)-FMAX) 440, 440, 420
  420    FMAX = Y(I)
         H = I
         GO TO 480
  440    IF (Y(I)-FMIN) 460, 480, 480
  460    FMIN = Y(I)
         L = I
  480 CONTINUE
      IF (LASTMX-H) 580, 500, 580
  500 MCOUNT = MCOUNT + 1
      IF (MCOUNT-5) 600, 520, 600
  520 IF (H-1) 560, 540, 560
  540 H = 2
      FMAX = Y(H)
      GO TO 600
  560 H = 1
      FMAX = Y(H)
      GO TO 600
  580 LASTMX = H
      MCOUNT = 0
  600 CONTINUE
      CALL MONIT(FMIN,FMAX,P,N,N1,NCALL)
      IF (K.EQ.1) GO TO 620
      IF (SERROR-EPS) 1140, 620, 620
  620 IF (NCALL-MAXIT) 640, 1160, 1160
  640 DO 700 J = 1, N
         CENT = 0.D0
         DO 680 I = 1, N1
            IF (I-H) 660, 680, 660
  660       CENT = CENT + P(I,J)
  680    CONTINUE
         PBAR(J) = CENT/DBLE(N)
  700 CONTINUE
C     REFLECTION
      DO 720 I = 1, N
         PSTAR(I) = (1.D0+A)*PBAR(I) - A*P(H,I)
  720 CONTINUE
      CALL FUNCT(N,PSTAR,YSTAR,dummy)
      NCALL = NCALL + 1
      IF (YSTAR-FMIN) 740, 780, 780
C     EXPANSION
  740 DO 760 I = 1, N
         PDSTAR(I) = (1.D0+C)*PSTAR(I) - C*PBAR(I)
  760 CONTINUE
      CALL FUNCT(N,PDSTAR,YDSTAR,dummy)
      NCALL = NCALL + 1
      IF (YDSTAR-YSTAR) 980, 1020, 1020
C     CONTRACTION
  780 DO 820 I = 1, N
         IF (I-H) 800, 820, 800
  800    IF (YSTAR-Y(I)) 1020, 820, 820
  820 CONTINUE
      IF (FMAX-YSTAR) 880, 840, 840
  840 DO 860 I = 1, N
         P(H,I) = PSTAR(I)
  860 CONTINUE
  880 CONTINUE
      DO 900 I = 1, N
         PDSTAR(I) = B*P(H,I) + (1.D0-B)*PBAR(I)
  900 CONTINUE
      CALL FUNCT(N,PDSTAR,YDSTAR,dummy)
      NCALL = NCALL + 1
      IF (YDSTAR-FMAX) 980, 980, 920
  920 DO 960 I = 1, N1
         DO 940 J = 1, N
            PBAR(J) = (P(I,J)+P(L,J))*0.5D0
            P(I,J) = PBAR(J)
  940    CONTINUE
         CALL FUNCT(N,PBAR,Y(I),dummy)
         NCALL = NCALL + 1
  960 CONTINUE
      GO TO 1060
  980 DO 1000 J = 1, N
         P(H,J) = PDSTAR(J)
 1000 CONTINUE
      Y(H) = YDSTAR
      GO TO 1060
 1020 DO 1040 J = 1, N
         P(H,J) = PSTAR(J)
 1040 CONTINUE
      Y(H) = YSTAR
 1060 YMEAN = 0.D0
      SERROR = 0.D0
      DO 1080 I = 1, N1
         YMEAN = YMEAN + Y(I)
 1080 CONTINUE
      YMEAN = YMEAN/DBLE(N+1)
      DO 1100 I = 1, N1
         SERROR = SERROR + (Y(I)-YMEAN)**2
 1100 CONTINUE
      SERROR = SQRT(SERROR/DBLE(N+1))
      GO TO 400
 1120 IV = 2
      GO TO 1240
 1140 IV = 0
      GO TO 1180
 1160 IV = 2
 1180 DO 1200 I = 1, N
         X(I) = P(L,I)
 1200 CONTINUE
      GO TO 1240
 1220 IV = 1
 1240 IFAIL = P01ABF(IFAIL,IV,SRNAME,0,P01REC)
      RETURN
      END
              subroutine rzerowrapper(za,zb,x0,zmin,ierr)  
                common /zminacommon/zmina
                real *8 zmina,zmin
                real *8 za,zb,x0,xfun,r,eps
                external xfun
                eps=0.01
                zmina=zmin
                maxfu=1000 
                call dzero(za,zb,x0,r,eps,maxfu,xfun)
                ierr=0
                if(r.lt.0)ierr=1 
              end
              real *8 function xfun(x,i)
                common /zminacommon/zmina
                real *8 zmina
                real *8 x
                xfun=derfc(x)-zmina
              end
      SUBROUTINE D01AMF(F,BOUND,INF,EPSABS,EPSREL,RESULT,ABSERR,WORK,
     *                  LWORK,IWORK,LIWORK,IFAIL,dummy)
C     MARK 13 RE-ISSUE. NAG COPYRIGHT 1988.
C     BASED ON THE QUADPACK ROUTINE  QAGI.
C
C     D01AMF CALCULATES AN APPROXIMATION TO THE INTEGRAL OF A FUNCTION
C     OVER THE INTERVAL (A,B) , WHERE A AND/OR B IS INFINITE.
C
C     D01AMF ITSELF IS ESSENTIALLY A DUMMY ROUTINE WHOSE FUNCTION IS TO
C     PARTITION THE WORK ARRAYS WORK AND IWORK FOR USE BY D01AMV.
C     WORK IS PARTITIONED INTO 4 ARRAYS EACH OF SIZE LIMIT, WHERE
C     LIMIT = MIN(LWORK/4, LIMIT).
C     IWORK IS A SINGLE ARRAY IN D01AMV OF SIZE LIMIT.
C
C     .. Parameters ..
      integer dummy(1)
      DOUBLE PRECISION  ZERO
      PARAMETER         (ZERO=0.0D0)
      CHARACTER*6       SRNAME
      PARAMETER         (SRNAME='D01AMF')
C     .. Scalar Arguments ..
      DOUBLE PRECISION  ABSERR, BOUND, EPSABS, EPSREL, RESULT
      INTEGER           IFAIL, INF, LIWORK, LWORK
C     .. Array Arguments ..
      DOUBLE PRECISION  WORK(LWORK)
      INTEGER           IWORK(LIWORK)
C     .. Function Arguments ..
c      DOUBLE PRECISION  F
      EXTERNAL          F
C     .. Local Scalars ..
      INTEGER           I, IBL, IEL, IER, IERR, IRL, J, JBL, JEL, JRL,
     *                  K, LAST, LIMIT, NEVAL, NREC
      CHARACTER*1       ORDER
C     .. Local Arrays ..
      CHARACTER*80      REC(2)
C     .. External Functions ..
      INTEGER           P01ABF
      EXTERNAL          P01ABF
C     .. External Subroutines ..
      EXTERNAL          D01AMV, M01DAF, M01EAF
C     .. Intrinsic Functions ..
      INTRINSIC         ABS, MIN
C     .. Executable Statements ..
      IER = IFAIL
C     CHECK THAT MINIMUM REQUIREMENTS ARE MET
      IF (LWORK.LT.4 .OR. LIWORK.LT.1) GO TO 100
      IF (INF.NE.-1 .AND. INF.NE.1 .AND. INF.NE.2) GO TO 120
C     LIMIT = UPPER BOUND ON NUMBER OF SUBINTERVALS
      LIMIT = MIN(LWORK/4,LIWORK)
C     SET UP BASE ADDRESSES FOR WORK ARRAYS
      IBL = LIMIT + 1
      IEL = LIMIT + IBL
      IRL = LIMIT + IEL
C     PERFORM INTEGRATION
      CALL D01AMV(F,BOUND,INF,ABS(EPSABS),ABS(EPSREL),WORK(1),WORK(IBL),
     *            WORK(IEL),WORK(IRL),LIMIT,IWORK,LIMIT,RESULT,ABSERR,
     *            NEVAL,IER,dummy)
C     RE-ORDER THE ELEMENTS OF WORK SO THAT THE RIGHT END-POINTS OF THE
C     SUB-INTERVALS (BLIST), ABSOLUTE ERROR ESTIMATES (ELIST) AND
C     APPROXIMATIONS TO THE INTEGRAL OVER THE SUB-INTERVALS (RLIST)
C     ARE EASILY ACCESSIBLE TO THE USER (SEE D01AMV).
      LAST = IWORK(1)
      IF (IER.LT.6 .AND. LAST.GE.1) THEN
         JBL = LAST
         JEL = 2*LAST
         JRL = 3*LAST
         IF (LAST.LT.LIMIT) THEN
            DO 20 I = 1, LAST
               WORK(JBL+I) = WORK(IBL+I-1)
   20       CONTINUE
            DO 40 I = 1, LAST
               WORK(JEL+I) = WORK(IEL+I-1)
   40       CONTINUE
            DO 60 I = 1, LAST
               WORK(JRL+I) = WORK(IRL+I-1)
   60       CONTINUE
         END IF
C        ZERO THE REMAINING PART OF WORK
         K = 4*LAST + 1
         DO 80 J = K, LWORK
            WORK(J) = ZERO
   80    CONTINUE
C        SORT THE ELEMENTS OF ALIST INTO ASCENDING ORDER USING M01DAF
C        AND M01EAF. ON EXIT FROM M01DAF, IWORK(1), ... ,IWORK(LAST)
C        CONTAIN THE RANKS OF ALIST(1), ... ,ALIST(LAST).
         IERR = 0
         ORDER = 'A'
         CALL M01DAF(WORK(1),1,LAST,ORDER,IWORK,IERR)
         CALL M01EAF(WORK(1),1,LAST,IWORK,IERR)
C        USE IWORK(1), ... ,IWORK(LAST) TO RECOVER THE VALUES OF
C        BLIST(I), ELIST(I) AND RLIST(I) CORRESPONDING TO ALIST(I)
C        AS RETURNED BY M01EAF, WHERE I = 1, 2, ...,LAST.
         JBL = JBL + 1
         CALL M01EAF(WORK(JBL),1,LAST,IWORK,IERR)
         JEL = JEL + 1
         CALL M01EAF(WORK(JEL),1,LAST,IWORK,IERR)
         JRL = JRL + 1
         CALL M01EAF(WORK(JRL),1,LAST,IWORK,IERR)
         IWORK(1) = LAST
      END IF
      IF (IER.NE.0) GO TO 140
      IFAIL = 0
      GO TO 160
C     ERROR 6 = INSUFFICIENT WORKSPACE
  100 IER = 6
      WRITE (REC,FMT=99999) LWORK, LIWORK
      GO TO 140
  120 IER = 6
      WRITE (REC,FMT=99994) INF
  140 NREC = 2
      IF (IER.EQ.1) THEN
         WRITE (REC,FMT=99998) LIMIT, LWORK, LIWORK
      ELSE IF (IER.EQ.2) THEN
         WRITE (REC,FMT=99997) EPSABS, EPSREL
      ELSE IF (IER.EQ.3) THEN
         NREC = 0
      ELSE IF (IER.EQ.4) THEN
         WRITE (REC(1),FMT=99996)
         NREC = 1
      ELSE IF (IER.EQ.5) THEN
         WRITE (REC(1),FMT=99995)
         NREC = 1
      END IF
      IFAIL = P01ABF(IFAIL,IER,SRNAME,NREC,REC)
  160 RETURN
C
99999 FORMAT (' ** On entry, LW.lt.4 or LIW.lt.1:',/'    LW = ',I16,
     *       '  LIW = ',I16)
99998 FORMAT (' ** The maximum number of subdivisions (LIMIT) has been',
     *       ' reached:',/'    LIMIT = ',I16,'   LW = ',I16,'   LIW = ',
     *       I16)
99997 FORMAT (' ** Round-off error prevents the requested tolerance fr',
     *       'om being achieved:',/'    EPSABS = ',1P,D8.1,
     *       '  EPSREL = ',1P,D8.1)
99996 FORMAT (' ** Round-off error is detected in the extrapolation ta',
     *       'ble')
99995 FORMAT (' ** The integral is probably divergent or slowly conver',
     *       'gent')
99994 FORMAT (' ** On entry, INF.ne.-1 and INF.ne.1 and INF.ne.2',/'  ',
     *       '  INF =',I16)
      END


      SUBROUTINE D01AMV(F,BOUND,INF,EPSABS,EPSREL,ALIST,BLIST,ELIST,
     *                  RLIST,LIMIT,IORD,LIORD,RESULT,ABSERR,NEVAL,IER
     + ,dummy)
C     MARK 13 RE-ISSUE. NAG COPYRIGHT 1988.
C     BASED ON QUADPACK ROUTINE  QAGI.
C     ..................................................................
C
C        PURPOSE
C           THE ROUTINE CALCULATES AN APPROXIMATION  RESULT  TO A GIVEN
C           INTEGRAL    I = INTEGRAL OF  F  OVER (BOUND,+INFINITY)
C                    OR I = INTEGRAL OF  F  OVER (-INFINITY,BOUND)
C                    OR I = INTEGRAL OF  F  OVER (-INFINITY,+INFINITY),
C           HOPEFULLY SATISFYING FOLLOWING CLAIM FOR ACCURACY
C           ABS(I-RESULT).LE.MAX(EPSABS,EPSREL*ABS(I)).
C
C        PARAMETERS
C         ON ENTRY
C            F      - REAL
C                     FUNCTION SUBPROGRAM DEFINING THE INTEGRAND
C                     FUNCTION F(X). THE ACTUAL NAME FOR F NEEDS TO BE
C                     DECLARED E X T E R N A L IN THE DRIVER PROGRAM.
C
C            BOUND  - REAL
C                     FINITE BOUND OF INTEGRATION RANGE
C                     (HAS NO MEANING IF INTERVAL IS DOUBLY-INFINITE)
C
C            INF    - REAL
C                     INDICATING THE KIND OF INTEGRATION RANGE INVOLVED
C                     INF = 1 CORRESPONDS TO  (BOUND,+INFINITY),
C                     INF = -1            TO  (-INFINITY,BOUND),
C                     INF = 2             TO (-INFINITY,+INFINITY).
C
C            EPSABS - REAL
C                     ABSOLUTE ACCURACY REQUESTED
C            EPSREL - REAL
C                     RELATIVE ACCURACY REQUESTED
C
C            ALIST,BLIST,ELIST,RLIST
C                   - REAL WORK ARRAYS (FUNCTIONS DESCRIBED BELOW)
C
C            LIMIT  - INTEGER
C                     GIVES AN UPPERBOUND ON THE NUMBER OF SUBINTERVALS
C                     IN THE PARTITION OF (A,B), LIMIT.GE.1.
C
C            IORD   - INTEGER
C                     WORK ARRAY OF DIMENSION LIORD
C
C            LIORD  - INTEGER
C                     LENGTH OF IORD (=LIMIT)
C
C         ON RETURN
C            RESULT - REAL
C                     APPROXIMATION TO THE INTEGRAL
C
C            ABSERR - REAL
C                     ESTIMATE OF THE MODULUS OF THE ABSOLUTE ERROR,
C                     WHICH SHOULD EQUAL OR EXCEED ABS(I-RESULT)
C
C            NEVAL  - INTEGER
C                     NUMBER OF INTEGRAND EVALUATIONS
C
C            IER    - INTEGER
C                     IER = 0 NORMAL AND RELIABLE TERMINATION OF THE
C                             ROUTINE. IT IS ASSUMED THAT THE REQUESTED
C                             ACCURACY HAS BEEN ACHIEVED.
C                   - IER.GT.0 ABNORMAL TERMINATION OF THE ROUTINE. THE
C                             ESTIMATES FOR RESULT AND ERROR ARE LESS
C                             RELIABLE. IT IS ASSUMED THAT THE REQUESTED
C                             ACCURACY HAS NOT BEEN ACHIEVED.
C                     IER = 1 MAXIMUM NUMBER OF SUBDIVISIONS ALLOWED
C                             HAS BEEN ACHIEVED. ONE CAN ALLOW MORE
C                             SUBDIVISIONS BY INCREASING THE DATA VALUE
C                         OF LIMIT IN D01AMV (AND TAKING THE ACCORDING
C                             DIMENSION ADJUSTMENTS INTO ACCOUNT).
C                             HOWEVER, IF THIS YIELDS NO IMPROVEMENT
C                             IT IS ADVISED TO ANALYZE THE INTEGRAND
C                             IN ORDER TO DETERMINE THE INTEGRATION
C                             DIFFICULTIES. IF THE POSITION OF A LOCAL
C                             DIFFICULTY CAN BE DETERMINED (E.G.
C                             SINGULARITY, DISCONTINUITY WITHIN THE
C                             INTERVAL) ONE WILL PROBABLY GAIN FROM
C                             SPLITTING UP THE INTERVAL AT THIS POINT
C                             AND CALLING THE INTEGRATOR ON THE
C                             SUBRANGES. IF POSSIBLE, AN APPROPRIATE
C                             SPECIAL-PURPOSE INTEGRATOR SHOULD BE USED,
C                             WHICH IS DESIGNED FOR HANDLING THE TYPE
C                             OF DIFFICULTY INVOLVED.
C                         = 2 THE OCCURRENCE OF ROUNDOFF ERROR IS
C                             DETECTED, WHICH PREVENTS THE REQUESTED
C                             TOLERANCE FROM BEING ACHIEVED.
C                             THE ERROR MAY BE UNDER-ESTIMATED.
C                         = 3 EXTREMELY BAD INTEGRAND BEHAVIOUR OCCURS
C                             AT SOME POINTS OF THE INTEGRATION
C                             INTERVAL.
C                         = 4 THE ALGORITHM DOES NOT CONVERGE.
C                             ROUNDOFF ERROR IS DETECTED IN THE
C                             EXTRAPOLATION TABLE.
C                             IT IS ASSUMED THAT THE REQUESTED TOLERANCE
C                             CANNOT BE ACHIEVED, AND THAT THE RETURNED
C                             RESULT IS THE BEST WHICH CAN BE OBTAINED.
C                         = 5 THE INTEGRAL IS PROBABLY DIVERGENT, OR
C                             SLOWLY CONVERGENT. IT MUST BE NOTED THAT
C                             DIVERGENCE CAN OCCUR WITH ANY OTHER VALUE
C                             OF IER.
C
C     ..................................................................
C
C     .. Scalar Arguments ..
      integer dummy(1) 
      DOUBLE PRECISION  ABSERR, BOUND, EPSABS, EPSREL, RESULT
      INTEGER           IER, INF, LIMIT, LIORD, NEVAL
C     .. Array Arguments ..
      DOUBLE PRECISION  ALIST(LIMIT), BLIST(LIMIT), ELIST(LIMIT),
     *                  RLIST(LIMIT)
      INTEGER           IORD(LIMIT)
C     .. Function Arguments ..
c      DOUBLE PRECISION  F
      EXTERNAL          F
C     .. Local Scalars ..
      DOUBLE PRECISION  A1, A2, ABSEPS, AREA, AREA1, AREA12, AREA2, B1,
     *                  B2, BOUN, CORREC, DEFAB1, DEFAB2, DEFABS, DRES,
     *                  EPMACH, ERLARG, ERLAST, ERRBND, ERRMAX, ERRO12,
     *                  ERROR1, ERROR2, ERRSUM, ERTEST, OFLOW, RESABS,
     *                  RESEPS, SMALL, UFLOW
      INTEGER           ID, IERRO, IERS, IROFF1, IROFF2, IROFF3, JUPBND,
     *                  K, KSGN, KTMIN, LAST, MAXERR, NERR, NRES, NRMAX,
     *                  NUMRL2
      LOGICAL           EXTRAP, NOEXT
C     .. Local Arrays ..
      DOUBLE PRECISION  RES3LA(3), RLIST2(52)
      CHARACTER*80      REC(2)
C     .. External Functions ..
      DOUBLE PRECISION  X02AJF, X02AMF
      EXTERNAL          X02AJF, X02AMF
C     .. External Subroutines ..
      EXTERNAL          D01AJX, D01AJY, D01AMZ, X04AAF, X04BAF
C     .. Intrinsic Functions ..
      INTRINSIC         ABS, MAX
C     .. Executable Statements ..
C
C            THE DIMENSION OF RLIST2 IS DETERMINED BY THE VALUE OF
C            LIMEXP IN SUBROUTINE D01AJY.
C
C            LIST OF MAJOR VARIABLES
C            -----------------------
C
C           ALIST     - LIST OF LEFT END POINTS OF ALL SUBINTERVALS
C                       CONSIDERED UP TO NOW
C           BLIST     - LIST OF RIGHT END POINTS OF ALL SUBINTERVALS
C                       CONSIDERED UP TO NOW
C           RLIST(I)  - APPROXIMATION TO THE INTEGRAL OVER
C                       (ALIST(I),BLIST(I))
C           RLIST2    - ARRAY OF DIMENSION AT LEAST (LIMEXP+2),
C                       CONTAINING THE PART OF THE EPSILON TABLE
C                       WHICH IS STILL NEEDED FOR FURTHER COMPUTATIONS
C           ELIST(I)  - ERROR ESTIMATE APPLYING TO RLIST(I)
C           MAXERR    - POINTER TO THE INTERVAL WITH LARGEST ERROR
C                       ESTIMATE
C           ERRMAX    - ELIST(MAXERR)
C           ERLAST    - ERROR ON THE INTERVAL CURRENTLY SUBDIVIDED
C                       (BEFORE THAT SUBDIVISION HAS TAKEN PLACE)
C           AREA      - SUM OF THE INTEGRALS OVER THE SUBINTERVALS
C           ERRSUM    - SUM OF THE ERRORS OVER THE SUBINTERVALS
C           ERRBND    - REQUESTED ACCURACY MAX(EPSABS,EPSREL*
C                       ABS(RESULT))
C           *****1    - VARIABLE FOR THE LEFT SUBINTERVAL
C           *****2    - VARIABLE FOR THE RIGHT SUBINTERVAL
C           LAST      - INDEX FOR SUBDIVISION
C           NRES      - NUMBER OF CALLS TO THE EXTRAPOLATION ROUTINE
C           NUMRL2    - NUMBER OF ELEMENTS CURRENTLY IN RLIST2. IF AN
C                       APPROPRIATE APPROXIMATION TO THE COMPOUNDED
C                       INTEGRAL HAS BEEN OBTAINED, IT IS PUT IN
C                       RLIST2(NUMRL2) AFTER NUMRL2 HAS BEEN INCREASED
C                       BY ONE.
C           SMALL     - LENGTH OF THE SMALLEST INTERVAL CONSIDERED UP
C                       TO NOW, MULTIPLIED BY 1.5
C           ERLARG    - SUM OF THE ERRORS OVER THE INTERVALS LARGER
C                       THAN THE SMALLEST INTERVAL CONSIDERED UP TO NOW
C           EXTRAP    - LOGICAL VARIABLE DENOTING THAT THE ROUTINE
C                       IS ATTEMPTING TO PERFORM EXTRAPOLATION. I.E.
C                       BEFORE SUBDIVIDING THE SMALLEST INTERVAL WE
C                       TRY TO DECREASE THE VALUE OF ERLARG.
C           NOEXT     - LOGICAL VARIABLE DENOTING THAT EXTRAPOLATION
C                       IS NO LONGER ALLOWED (TRUE-VALUE)
C
      EPMACH = X02AJF()
      UFLOW = X02AMF()
      OFLOW = 1.0D+00/UFLOW
C
C           TEST ON VALIDITY OF PARAMETERS
C           -----------------------------
C
      IERS = IER
      IER = 0
      NEVAL = 0
      LAST = 0
      RESULT = 0.0D+00
      ABSERR = 0.0D+00
      ALIST(1) = 0.0D+00
      BLIST(1) = 1.0D+00
      RLIST(1) = 0.0D+00
      ELIST(1) = 0.0D+00
      IORD(1) = 0
C
C           FIRST APPROXIMATION TO THE INTEGRAL
C           -----------------------------------
C
C           DETERMINE THE INTERVAL TO BE MAPPED ONTO (0,1).
C           IF INF = 2 THE INTEGRAL IS COMPUTED AS I = I1+I2, WHERE
C           I1 = INTEGRAL OF F OVER (-INFINITY,0),
C           I2 = INTEGRAL OF F OVER (0,+INFINITY).
C
      BOUN = BOUND
      IF (INF.EQ.2) BOUN = 0.0D+00
      CALL D01AMZ(F,BOUN,INF,0.0D+00,1.0D+00,RESULT,ABSERR,DEFABS,
     *            RESABS,dummy)
C
C           TEST ON ACCURACY
C
      LAST = 1
      RLIST(1) = RESULT
      ELIST(1) = ABSERR
      IORD(1) = 1
      DRES = ABS(RESULT)
      ERRBND = MAX(EPSABS,EPSREL*DRES)
      IF ((ABSERR.LE.ERRBND .AND. ABSERR.NE.RESABS)
     *    .OR. ABSERR.EQ.0.0D+00) GO TO 320
      IF (LIMIT.EQ.1) IER = 1
      IF (ABSERR.LE.1.0D+02*EPMACH*DEFABS .AND. ABSERR.GT.ERRBND)
     *    IER = 2
      IF (IER.NE.0) GO TO 320
C
C           INITIALIZATION
C           --------------
C
      RLIST2(1) = RESULT
      ERRMAX = ABSERR
      MAXERR = 1
      AREA = RESULT
      ERRSUM = ABSERR
      ABSERR = OFLOW
      NRMAX = 1
      NRES = 0
      KTMIN = 0
      NUMRL2 = 2
      EXTRAP = .FALSE.
      NOEXT = .FALSE.
      IERRO = 0
      IROFF1 = 0
      IROFF2 = 0
      IROFF3 = 0
      KSGN = -1
      IF (DRES.GE.(1.0D+00-5.0D+01*EPMACH)*DEFABS) KSGN = 1
C
C           MAIN DO-LOOP
C           ------------
C
      DO 200 LAST = 2, LIMIT
C
C           BISECT THE SUBINTERVAL WITH NRMAX-TH LARGEST ERROR ESTIMATE.
C
         A1 = ALIST(MAXERR)
         B1 = 5.0D-01*(ALIST(MAXERR)+BLIST(MAXERR))
         A2 = B1
         B2 = BLIST(MAXERR)
         ERLAST = ERRMAX
         CALL D01AMZ(F,BOUN,INF,A1,B1,AREA1,ERROR1,RESABS,DEFAB1,dummy)
         CALL D01AMZ(F,BOUN,INF,A2,B2,AREA2,ERROR2,RESABS,DEFAB2,dummy)
C
C           IMPROVE PREVIOUS APPROXIMATIONS TO INTEGRAL AND ERROR
C           AND TEST FOR ACCURACY.
C
         AREA12 = AREA1 + AREA2
         ERRO12 = ERROR1 + ERROR2
         ERRSUM = ERRSUM + ERRO12 - ERRMAX
         AREA = AREA + AREA12 - RLIST(MAXERR)
         IF (DEFAB1.EQ.ERROR1 .OR. DEFAB2.EQ.ERROR2) GO TO 40
         IF (ABS(RLIST(MAXERR)-AREA12).GT.1.0D-05*ABS(AREA12)
     *       .OR. ERRO12.LT.9.9D-01*ERRMAX) GO TO 20
         IF (EXTRAP) IROFF2 = IROFF2 + 1
         IF ( .NOT. EXTRAP) IROFF1 = IROFF1 + 1
   20    IF (LAST.GT.10 .AND. ERRO12.GT.ERRMAX) IROFF3 = IROFF3 + 1
   40    RLIST(MAXERR) = AREA1
         RLIST(LAST) = AREA2
         ERRBND = MAX(EPSABS,EPSREL*ABS(AREA))
C
C           APPEND THE NEWLY-CREATED INTERVALS TO THE LIST.
C
         IF (ERROR2.GT.ERROR1) GO TO 60
         ALIST(LAST) = A2
         BLIST(MAXERR) = B1
         BLIST(LAST) = B2
         ELIST(MAXERR) = ERROR1
         ELIST(LAST) = ERROR2
         GO TO 80
   60    ALIST(MAXERR) = A2
         ALIST(LAST) = A1
         BLIST(LAST) = B1
         RLIST(MAXERR) = AREA2
         RLIST(LAST) = AREA1
         ELIST(MAXERR) = ERROR2
         ELIST(LAST) = ERROR1
C
C           CALL SUBROUTINE D01AJX TO MAINTAIN THE DESCENDING ORDERING
C           IN THE LIST OF ERROR ESTIMATES AND SELECT THE SUBINTERVAL
C           WITH NRMAX-TH LARGEST ERROR ESTIMATE (TO BE BISECTED NEXT).
C
   80    CALL D01AJX(LIMIT,LAST,MAXERR,ERRMAX,ELIST,IORD,NRMAX)
         IF (ERRSUM.LE.ERRBND) GO TO 280
C
C           SET ERROR FLAG IN THE CASE THAT THE NUMBER OF SUBINTERVALS
C           EQUALS LIMIT.
C
         IF (LAST.EQ.LIMIT) IER = 1
C
C           TEST FOR ROUNDOFF ERROR AND EVENTUALLY SET ERROR FLAG.
C
         IF (IROFF1+IROFF2.GE.10 .OR. IROFF3.GE.20) IER = 2
         IF (IROFF2.GE.5) IERRO = 3
C
C           SET ERROR FLAG IN THE CASE OF BAD INTEGRAND BEHAVIOUR
C           AT SOME POINTS OF THE INTEGRATION RANGE.
C
         IF (MAX(ABS(A1),ABS(B2)).LE.(1.0D+00+1.0D+03*EPMACH)*(ABS(A2)
     *       +1.0D+03*UFLOW)) IER = 4
         IF (IER.GT.1) GO TO 220
         IF (LAST.EQ.2) GO TO 180
         IF (NOEXT) GO TO 200
         ERLARG = ERLARG - ERLAST
         IF (ABS(B1-A1).GT.SMALL) ERLARG = ERLARG + ERRO12
         IF (EXTRAP) GO TO 100
C
C           TEST WHETHER THE INTERVAL TO BE BISECTED NEXT IS THE
C           SMALLEST INTERVAL.
C
         IF (ABS(BLIST(MAXERR)-ALIST(MAXERR)).GT.SMALL) GO TO 200
         EXTRAP = .TRUE.
         NRMAX = 2
  100    IF (IERRO.EQ.3 .OR. ERLARG.LE.ERTEST) GO TO 140
C
C           THE SMALLEST INTERVAL HAS THE LARGEST ERROR.
C           BEFORE BISECTING DECREASE THE SUM OF THE ERRORS OVER THE
C           LARGER INTERVALS (ERLARG) AND PERFORM EXTRAPOLATION.
C
         ID = NRMAX
         JUPBND = LAST
         IF (LAST.GT.(2+LIMIT/2)) JUPBND = LIMIT + 3 - LAST
         DO 120 K = ID, JUPBND
            MAXERR = IORD(NRMAX)
            ERRMAX = ELIST(MAXERR)
            IF (ABS(BLIST(MAXERR)-ALIST(MAXERR)).GT.SMALL) GO TO 200
            NRMAX = NRMAX + 1
  120    CONTINUE
C
C           PERFORM EXTRAPOLATION.
C
  140    NUMRL2 = NUMRL2 + 1
         RLIST2(NUMRL2) = AREA
         CALL D01AJY(NUMRL2,RLIST2,RESEPS,ABSEPS,RES3LA,NRES)
         KTMIN = KTMIN + 1
         IF (KTMIN.GT.5 .AND. ABSERR.LT.1.0D-03*ERRSUM) IER = 5
         IF (ABSEPS.GE.ABSERR) GO TO 160
         KTMIN = 0
         ABSERR = ABSEPS
         RESULT = RESEPS
         CORREC = ERLARG
         ERTEST = MAX(EPSABS,EPSREL*ABS(RESEPS))
         IF (ABSERR.LE.ERTEST) THEN
            IF (IER.EQ.1) IER = 0
            GO TO 220
         END IF
C
C            PREPARE BISECTION OF THE SMALLEST INTERVAL.
C
  160    IF (NUMRL2.EQ.1) NOEXT = .TRUE.
         IF (IER.EQ.5 .OR. IER.EQ.1) GO TO 220
         MAXERR = IORD(1)
         ERRMAX = ELIST(MAXERR)
         NRMAX = 1
         EXTRAP = .FALSE.
         SMALL = SMALL*5.0D-01
         ERLARG = ERRSUM
         GO TO 200
  180    SMALL = 3.75D-01
         ERLARG = ERRSUM
         ERTEST = ERRBND
         RLIST2(2) = AREA
  200 CONTINUE
      LAST = LIMIT
C
C           SET FINAL RESULT AND ERROR ESTIMATE.
C           ------------------------------------
C
  220 IF (ABSERR.EQ.OFLOW) GO TO 280
      IF ((IER+IERRO).EQ.0) GO TO 260
      IF (IERRO.EQ.3) ABSERR = ABSERR + CORREC
      IF (IER.EQ.0) IER = 3
      IF (RESULT.NE.0.0D+00 .AND. AREA.NE.0.0D+00) GO TO 240
      IF (ABSERR.GT.ERRSUM) GO TO 280
      IF (AREA.EQ.0.0D+00) GO TO 320
      GO TO 260
  240 IF (ABSERR/ABS(RESULT).GT.ERRSUM/ABS(AREA)) GO TO 280
C
C           TEST ON DIVERGENCE
C
  260 IF (KSGN.EQ.(-1) .AND. MAX(ABS(RESULT),ABS(AREA))
     *    .LE.DEFABS*1.0D-02) GO TO 320
      IF (1.0D-02.GT.(RESULT/AREA) .OR. (RESULT/AREA)
     *    .GT.1.0D+02 .OR. ERRSUM.GT.ABS(AREA)) IER = 6
      GO TO 320
C
C           COMPUTE GLOBAL INTEGRAL SUM.
C
  280 RESULT = 0.0D+00
      DO 300 K = 1, LAST
         RESULT = RESULT + RLIST(K)
  300 CONTINUE
      ABSERR = ERRSUM
  320 NEVAL = 30*LAST - 15
      IORD(1) = LAST
      IF (INF.EQ.2) NEVAL = 2*NEVAL
      IF (IER.GT.2) IER = IER - 1
      IF (IER.EQ.3 .AND. IERS.NE.1) THEN
         CALL X04AAF(0,NERR)
         IF (INF.EQ.1) THEN
            A1 = BOUND + (1.0D+00-A1)/A1
            B2 = BOUND + (1.0D+00-B2)/B2
            WRITE (REC,FMT=99999) A1, B2
         ELSE IF (INF.EQ.-1) THEN
            A1 = BOUND - (1.0D+00-A1)/A1
            B2 = BOUND - (1.0D+00-B2)/B2
            WRITE (REC,FMT=99999) A1, B2
         ELSE
            A1 = (1.0D+00-A1)/A1
            B2 = (1.0D+00-B2)/B2
            WRITE (REC,FMT=99998) B2, A1, -A1, -B2
         END IF
         CALL X04BAF(NERR,REC(1))
         CALL X04BAF(NERR,REC(2))
      END IF
      RETURN
C
99999 FORMAT (' ** Extremely bad integrand behaviour occurs around the',
     *       ' subinterval',/'    (',1P,D15.7,' , ',1P,D15.7,' )')
99998 FORMAT (' ** Extremely bad integrand behaviour occurs around one',
     *       ' of the subintervals',/'    (',1P,D15.7,' , ',1P,D15.7,
     *       ' ) or (',1P,D15.7,' , ',1P,D15.7,' )')
      END
      SUBROUTINE D01AMZ(F,BOUN,INF,A,B,RESULT,ABSERR,RESABS,RESASC
     +  ,dummy)
C     MARK 13 RE-ISSUE. NAG COPYRIGHT 1988.
C     BASED ON QUADPACK ROUTINE  QK15I.
C     ..................................................................
C
C           PURPOSE
C              THE ORIGINAL (INFINITE) INTEGRATION RANGE IS MAPPED
C              ONTO THE INTERVAL (0,1) AND (A,B) IS A PART OF (0,1).
C              IT IS THE PURPOSE TO COMPUTE
C              I = INTEGRAL OF TRANSFORMED INTEGRAND OVER (A,B),
C              J = INTEGRAL OF ABS(TRANSFORMED INTEGRAND) OVER (A,B).
C
C           PARAMETERS
C            ON ENTRY
C              F      - REAL
C                       FUNCTION SUBPROGRAM DEFINING THE INTEGRAND
C                       FUNCTION F(X). THE ACTUAL NAME FOR F NEEDS TO BE
C                       DECLARED E X T E R N A L IN THE CALLING PROGRAM.
C
C              BOUN   - REAL
C                       FINITE BOUND OF ORIGINAL INTEGRATION RANGE
C                       (SET TO ZERO IF INF = +2)
C
C              INF    - INTEGER
C                       IF INF = -1, THE ORIGINAL INTERVAL IS
C                                   (-INFINITY,BOUND),
C                       IF INF = +1, THE ORIGINAL INTERVAL IS
C                                   (BOUND,+INFINITY),
C                       IF INF = +2, THE ORIGINAL INTERVAL IS
C                                   (-INFINITY,+INFINITY) AND
C                       THE INTEGRAL IS COMPUTED AS THE SUM OF TWO
C                       INTEGRALS, ONE OVER (-INFINITY,0) AND ONE
C                       OVER (0,+INFINITY).
C
C              A      - REAL
C                       LOWER LIMIT FOR INTEGRATION OVER SUBRANGE
C                       OF (0,1)
C
C              B      - REAL
C                       UPPER LIMIT FOR INTEGRATION OVER SUBRANGE
C                       OF (0,1)
C
C            ON RETURN
C              RESULT - REAL
C                       APPROXIMATION TO THE INTEGRAL I
C                       RESULT IS COMPUTED BY APPLYING THE 15-POINT
C                       KRONROD RULE(RESK) OBTAINED BY OPTIMAL ADDITION
C                       OF ABSCISSAE TO THE 7-POINT GAUSS RULE (RESG).
C
C              ABSERR - REAL
C                       ESTIMATE OF THE MODULUS OF THE ABSOLUTE ERROR,
C                       WHICH SHOULD EQUAL OR EXCEED ABS(I-RESULT)
C
C              RESABS - REAL
C                       APPROXIMATION TO THE INTEGRAL J
C
C              RESASC - REAL
C                       APPROXIMATION TO THE INTEGRAL OF
C                       ABS((TRANSFORMED INTEGRAND)-I/(B-A)) OVER (A,B)
C
C     ..................................................................
C
C           THE ABSCISSAE AND WEIGHTS ARE SUPPLIED FOR THE INTERVAL
C           (-1,1).  BECAUSE OF SYMMETRY ONLY THE POSITIVE ABSCISSAE AND
C           THEIR CORRESPONDING WEIGHTS ARE GIVEN.
C
C           XGK    - ABSCISSAE OF THE 15-POINT KRONROD RULE
C                    XGK(2), XGK(4), ... ABSCISSAE OF THE 7-POINT GAUSS
C                    RULE
C                    XGK(1), XGK(3), ...  ABSCISSAE WHICH ARE OPTIMALLY
C                    ADDED TO THE 7-POINT GAUSS RULE
C
C           WGK    - WEIGHTS OF THE 15-POINT KRONROD RULE
C
C           WG     - WEIGHTS OF THE 7-POINT GAUSS RULE, CORRESPONDING
C                    TO THE ABSCISSAE XGK(2), XGK(4), ...
C                    WG(1), WG(3), ... ARE SET TO ZERO.
C
C     .. Scalar Arguments ..
      integer dummy(1)
      DOUBLE PRECISION  A, ABSERR, B, BOUN, RESABS, RESASC, RESULT
      INTEGER           INF
C     .. Function Arguments ..
c      DOUBLE PRECISION  F
      EXTERNAL          F
C     .. Local Scalars ..
      DOUBLE PRECISION  ABSC, ABSC1, ABSC2, CENTR, DINF, EPMACH, FC,
     *                  FSUM, FVAL1, FVAL2, HLGTH, OFLOW, RESG, RESK,
     *                  RESKH, TABSC1, TABSC2, UFLOW,FV3,FV4
      INTEGER           J
C     .. Local Arrays ..
      DOUBLE PRECISION  FV1(7), FV2(7), WG(8), WGK(8), XGK(8)
C     .. External Functions ..
      DOUBLE PRECISION  X02AJF, X02AMF
      EXTERNAL          X02AJF, X02AMF
C     .. Intrinsic Functions ..
      INTRINSIC         ABS, MAX, MIN
C     .. Data statements ..
      DATA              WG(1)/0.000000000000000000000000000000000D+00/,
     *                  WG(2)/0.129484966168869693270611432679082D+00/,
     *                  WG(3)/0.000000000000000000000000000000000D+00/,
     *                  WG(4)/0.279705391489276667901467771423780D+00/,
     *                  WG(5)/0.000000000000000000000000000000000D+00/,
     *                  WG(6)/0.381830050505118944950369775488975D+00/,
     *                  WG(7)/0.000000000000000000000000000000000D+00/,
     *                  WG(8)/0.417959183673469387755102040816327D+00/
      DATA              XGK(1)/0.991455371120812639206854697526329D+00/,
     *                  XGK(2)/0.949107912342758524526189684047851D+00/,
     *                  XGK(3)/0.864864423359769072789712788640926D+00/,
     *                  XGK(4)/0.741531185599394439863864773280788D+00/,
     *                  XGK(5)/0.586087235467691130294144838258730D+00/,
     *                  XGK(6)/0.405845151377397166906606412076961D+00/,
     *                  XGK(7)/0.207784955007898467600689403773245D+00/,
     *                  XGK(8)/0.000000000000000000000000000000000D+00/
      DATA              WGK(1)/0.022935322010529224963732008058970D+00/,
     *                  WGK(2)/0.063092092629978553290700663189204D+00/,
     *                  WGK(3)/0.104790010322250183839876322541518D+00/,
     *                  WGK(4)/0.140653259715525918745189590510238D+00/,
     *                  WGK(5)/0.169004726639267902826583426598550D+00/,
     *                  WGK(6)/0.190350578064785409913256402421014D+00/,
     *                  WGK(7)/0.204432940075298892414161999234649D+00/,
     *                  WGK(8)/0.209482141084727828012999174891714D+00/
C     .. Executable Statements ..
C
C
C           LIST OF MAJOR VARIABLES
C           -----------------------
C
C           CENTR  - MID POINT OF THE INTERVAL
C           HLGTH  - HALF-LENGTH OF THE INTERVAL
C           ABSC*  - ABSCISSA
C           TABSC* - TRANSFORMED ABSCISSA
C           FVAL*  - FUNCTION VALUE
C           RESG   - RESULT OF THE 7-POINT GAUSS FORMULA
C           RESK   - RESULT OF THE 15-POINT KRONROD FORMULA
C           RESKH  - APPROXIMATION TO THE MEAN VALUE OF THE TRANSFORMED
C                    INTEGRAND OVER (A,B), I.E. TO I/(B-A)
C
      EPMACH = X02AJF()
      UFLOW = X02AMF()
      OFLOW = 1.0D+00/UFLOW
      DINF = MIN(1,INF)
C
      CENTR = 5.0D-01*(A+B)
      HLGTH = 5.0D-01*(B-A)
      TABSC1 = BOUN + DINF*(1.0D+00-CENTR)/CENTR
      CALL F(TABSC1,FVAL1,dummy)
      IF (INF.EQ.2) then
       call f(-TABSC1,fv3,dummy)
       FVAL1 = FVAL1 + Fv3
      endif
      FC = (FVAL1/CENTR)/CENTR
C
C           COMPUTE THE 15-POINT KRONROD APPROXIMATION TO THE INTEGRAL,
C           AND ESTIMATE THE ERROR.
C
      RESG = WG(8)*FC
      RESK = WGK(8)*FC
      RESABS = ABS(RESK)
      DO 20 J = 1, 7
         ABSC = HLGTH*XGK(J)
         ABSC1 = CENTR - ABSC
         ABSC2 = CENTR + ABSC
         TABSC1 = BOUN + DINF*(1.0D+00-ABSC1)/ABSC1
         TABSC2 = BOUN + DINF*(1.0D+00-ABSC2)/ABSC2
         call F(TABSC1,FVAL1,dummy)
         call F(TABSC2,FVAL2,dummy)
         IF (INF.EQ.2) then
          call f(-TABSC1,fv3,dummy)
          call f(-TABSC2,fv4,dummy)
             FVAL1 = FVAL1 + fv3
             FVAL2 = FVAL2 + fv4
         endif
         FVAL1 = (FVAL1/ABSC1)/ABSC1
         FVAL2 = (FVAL2/ABSC2)/ABSC2
         FV1(J) = FVAL1
         FV2(J) = FVAL2
         FSUM = FVAL1 + FVAL2
         RESG = RESG + WG(J)*FSUM
         RESK = RESK + WGK(J)*FSUM
         RESABS = RESABS + WGK(J)*(ABS(FVAL1)+ABS(FVAL2))
   20 CONTINUE
      RESKH = RESK*5.0D-01
      RESASC = WGK(8)*ABS(FC-RESKH)
      DO 40 J = 1, 7
         RESASC = RESASC + WGK(J)*(ABS(FV1(J)-RESKH)+ABS(FV2(J)-RESKH))
   40 CONTINUE
      RESULT = RESK*HLGTH
      RESASC = RESASC*HLGTH
      RESABS = RESABS*HLGTH
      ABSERR = ABS((RESK-RESG)*HLGTH)
      IF (RESASC.NE.0.0D+00 .AND. ABSERR.NE.0.0D+00)
     *    ABSERR = RESASC*MIN(1.0D+00,(2.0D+02*ABSERR/RESASC)**1.5D+00)
      IF (RESABS.GT.UFLOW/(5.0D+01*EPMACH))
     *    ABSERR = MAX((EPMACH*5.0D+01)*RESABS,ABSERR)
      RETURN
      END
