//  $Id: hbook.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $


/*
 Corrections:
 15-Feb-94 GF HGIVE  correct output args
              HGIVEN idem
 18-Feb-94 GF comment headers for routines with more than 10 Arguments
              and break long lines by \ (for the VAX)
 -------------above carried over from previous file on 15-Mar-94
 11-Apr-94 GF remove ALL occurences of ZTRING (these are not used so far)
              correct HOPEN, HROUT calls
 21-Apr-94 GF restore PROTOCCALLSFFUNi lines for functions.
 18-May-94 GF correct passing of external functions, must use ROUTINE
 further comments kept in RCS/CVS
 12-Apr-95 GF delete all non documented routines; this should give a more 
              version for this file.
---------------------------------------
*/

/* used in AMSR_Ntuple.cxx */

PROTOCCALLSFSUB1(HLIMIT,hlimit,INT)
#define HLIMIT(A1)  CCALLSFSUB1(HLIMIT,hlimit,INT,A1)
PROTOCCALLSFSUB6(HROPEN,hropen,INT,STRING,STRING,STRING,PINT,PINT)
#define HROPEN(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HROPEN,hropen,INT,STRING,STRING,STRING,PINT,PINT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB3(HRIN,hrin,INT,INT,INT)
#define HRIN(A1,A2,A3)  CCALLSFSUB3(HRIN,hrin,INT,INT,INT,A1,A2,A3)
PROTOCCALLSFSUB2(HCDIR,hcdir,PSTRING,STRING)
#define HCDIR(A1,A2)  CCALLSFSUB2(HCDIR,hcdir,PSTRING,STRING,A1,A2)
PROTOCCALLSFSUB1(HDELET,hdelet,INT)
#define HDELET(A1)  CCALLSFSUB1(HDELET,hdelet,INT,A1)
PROTOCCALLSFSUB1(HREND,hrend,STRING)
#define HREND(A1)  CCALLSFSUB1(HREND,hrend,STRING,A1)
PROTOCCALLSFSUB3(HGNTF,hgntf,INT,INT,PINT)
#define HGNTF(A1,A2,A3)  CCALLSFSUB3(HGNTF,hgntf,INT,INT,PINT,A1,A2,A3)
PROTOCCALLSFSUB3(HGNT,hgnt,INT,INT,PINT)
#define HGNT(A1,A2,A3)  CCALLSFSUB3(HGNT,hgnt,INT,INT,PINT,A1,A2,A3)
PROTOCCALLSFSUB5(HGNTV,hgntv,INT,STRINGV,INT,INT,PINT)
#define HGNTV(A1,A2,A3,A4,A5)  CCALLSFSUB5(HGNTV,hgntv,INT,STRINGV,INT,INT,PINT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB2(HNOENT,hnoent,INT,PINT)
#define HNOENT(A1,A2)  CCALLSFSUB2(HNOENT,hnoent,INT,PINT,A1,A2)
PROTOCCALLSFSUB4(HBNAME,hbname,INT,STRING,INTV,STRING)
#define HBNAME(A1,A2,A3,A4)  CCALLSFSUB4(HBNAME,hbname,INT,STRING,INTV,STRING,A1,A2,A3,A4)

/* end of hbook subroutines used in AMSR_Ntuple */
