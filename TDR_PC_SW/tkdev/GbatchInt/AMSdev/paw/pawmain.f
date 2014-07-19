      PROGRAM PAMAIN
*
*        MAIN Program for basic PAW
*
      PARAMETER (NWPAW=20000000)
*
      COMMON/PAWC/PAWCOM(NWPAW)
*
*
      CALL PAW(NWPAW,IWTYP)
*
      CALL KUWHAG
*
      CALL PAEXIT
*
      STOP
      END
      SUBROUTINE QNEXT
      END

