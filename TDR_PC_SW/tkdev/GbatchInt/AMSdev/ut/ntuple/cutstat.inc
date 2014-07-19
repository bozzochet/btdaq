*  $Id: cutstat.inc,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
      INTEGER NCUT
      PARAMETER (NCUT=20)
      COMMON /CUTSTAT/ NPASS(0:NCUT)
