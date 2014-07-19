*
* $Id: Geant3_utils.F,v 1.1 2008/09/11 17:19:22 zuccon Exp $
*
* $Log: Geant3_utils.F,v $
* Revision 1.1  2008/09/11 17:19:22  zuccon
* Add Gbatch reduced dir
*
* Revision 1.6  2007/01/16 15:08:25  choutko
* *** empty log message ***
*
* Revision 1.5  2006/12/07 09:11:42  choutko
* new ad cross section (fg)
*
* Revision 1.4  2006/11/24 11:04:59  choutko
* corrected tripati cross section (francesca)
*
* Revision 1.3  2005/10/20 14:21:03  choutko
* *** empty log message ***
*
* Revision 1.2  2005/10/20 12:25:25  choutko
* *** empty log message ***
*
* Revision 1.1  2005/05/13 09:12:47  pzuccon
* Split the file amstkshc++.F in the two files Geant3_utils.F TkFit_utils.F
*
* Revision 1.78  2005/05/12           zuccon
*    separate the tracking part from the geant3 part 
*    rename this file Geant3_utils.F
*
* Revision 1.77  2004/01/19 17:15:22  choutko
* add fitting without mf
*
* Revision 1.76  2003/11/12 08:57:39  alcaraz
*        Reverting to previous state. It was a feature, not a bug. Changing
*      then AMSVtx -> ErrMomentum is now the error on 1/Momentum sum.
*
* Revision 1.75  2003/11/12 08:46:33  alcaraz
*       Bug correction: error on PIrigidity was wrongly extracted
*       (it was the error on 1/PIrigidity in older versions)
*
* Revision 1.74  2003/10/30 09:35:02  choutko
* yale algo ++ ( start working)
*
* Revision 1.73  2003/10/29 15:24:43  choutko
* *** empty log message ***
*
* Revision 1.72  2003/03/18 09:12:55  choutko
* write down ascii mfield derivatives while creating mf record
*
* Revision 1.71  2003/02/25 11:08:48  alcaraz
* Updated and improved path integral tracking method (J.Alcaraz, Feb 2003)
* Hoping this time is OK
*
* Revision 1.71  2003/02/25 10:03:00  amscvs
*  Updated and improved path integral tracking method (J.Alcaraz, Feb 2003)
*
* Revision 1.70  2003/02/04 15:02:10  choutko
* geant 3.2.1 compat added + gamma back
*
* Revision 1.69  2003/01/07 18:35:58  jorgec
* Zine bug fixed
* better track of secondaries
*
* Revision 1.68  2002/08/09 16:03:49  choutko
* added be10 (as ipart 114)
*
* Revision 1.67  2002/06/06 14:41:03  choutko
* *** empty log message ***
*
* Revision 1.66  2002/02/26 13:28:17  choutko
* remote client rlated polishing
*
* Revision 1.65  2001/08/01 13:28:49  choutko
* ecal rec released
* trd rec bugs fixed
*
* Revision 1.64  2001/06/26 15:07:22  choutko
* stranglet
*
* Revision 1.63  2001/05/23 14:37:22  choutko
* *** empty log message ***
*
* Revision 1.62  2001/04/27 21:50:07  choutko
* trd alpha version released - see ~/doc/trd.doc on todo/status
*
* Revision 1.61  2001/03/29 14:23:34  choutko
* gheisha/fluka switch fixed
*
* Revision 1.60  2001/01/22 17:32:24  choutko
* prepend 1st line
*
* Revision 1.59  2000/11/22 07:41:25  choumilo
* *** empty log message ***
*
* Revision 1.58  2000/09/05 13:41:00  choutko
* TR radiation g4
*
* Revision 1.57  2000/09/01 14:24:03  choutko
* rndm
*
* Revision 1.2  1998/03/02 15:33:35  japost
*    A significant error on Linux is corrected:
*
*     Two lines of grndm were not defined for any system that was not listed.
*   In such a system (and Linux was one) the behavior of grndm was incorrect.
*
*     A default behavior is now defined for all systems. Any system that has
*   a problem with this behavior (using integer division) must be added to
*   the #ifdef together with IBMALL etc.
*
* Revision 1.1.1.1  1995/10/24 10:21:42  cernlib
* Geant
*  $Id: Geant3_utils.F,v 1.1 2008/09/11 17:19:22 zuccon Exp $
CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.18  by  S.Giani
*-- Author :
CDECK  ID>, ASTDES. 

CDECK  ID>, LSCOMS. 

*-- Author :
CDECK  ID>, ASTDES. 

CDECK  ID>, LSCOMS. 

*-- Author :
CDECK  ID>, ZPINIT. 
*     CMZ :          05/06/92  09.26.29  by
*--   Author :
      SUBROUTINE ZPINIT
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +     ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C     
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C     
      common /seedC/seed
      integer seed
      
*--   Random number seed
*     
      CALL DATIME(IDD,ITT)
      SEED = ITT+IDD*341
      WRITE(*,1045)SEED
 1045 FORMAT(/
     +     20X,'RANDOM NUMBER SEED =',I20,/)
      END
C     DECK  ID>, GUTREV. 
*     CMZ :          05/06/92  09.26.29  by
*--   Author :
      SUBROUTINE GUTREV
      CALL GTREVE
      END
      
      
CDECK  ID>, USTUFF. 
*CMZ :          05/06/92  09.26.57  by
CDECK  ID>, SPECTRA.
c      SUBROUTINE SPECTRA
c***************************************************************
c*
c* read in momentum spectra of H, He, Li, Be, B and C
c* seen at 51 degree by AMS in 2002
c*
c*                 files from G. Guzik
c*
c*                                         Y.F. Wang
c*     Bug fixed V. Shoutko
c**************************************************************
c      real *4 yap(500)
c      data yap/
c     +   1.0000      ,50.000      ,587.00      ,1705.0      ,2841.0
c     +  ,4329.0      ,6163.0      ,8420.0      ,11036.      ,14104.
c     +  ,16461.      ,18793.      ,21135.      ,23615.      ,25994.
c     +  ,28222.      ,30239.      ,32038.      ,33728.      ,35953.
c     +  ,37071.      ,38644.      ,39417.      ,40628.      ,41875.
c     +  ,43109.      ,43891.      ,45431.      ,46059.      ,47021.
c     +  ,47522.      ,48765.      ,49606.      ,50031.      ,50586.
c     +  ,49328.      ,47786.      ,46891.      ,45876.      ,44465.
c     +  ,44208.      ,43229.      ,42566.      ,42312.      ,41732.
c     +  ,41047.      ,40183.      ,39570.      ,38942.      ,38192.
c     +  ,37014.      ,36300.      ,35540.      ,34913.      ,33767.
c     +  ,32913.      ,32231.      ,31837.      ,30946.      ,29737.
c     +  ,28978.      ,28566.      ,27468.      ,27394.      ,26360.
c     +  ,25588.      ,24938.      ,23986.      ,23810.      ,22810.
c     +  ,22046.      ,21684.      ,20946.      ,20522.      ,20061.
c     +  ,19388.      ,18556.      ,18493.      ,17741.      ,17155.
c     +  ,16693.      ,16221.      ,15827.      ,15320.      ,15043.
c     +  ,14496.      ,13985.      ,13647.      ,13109.      ,12875.
c     +  ,12466.      ,12087.      ,11911.      ,11291.      ,10986.
c     +  ,10799.      ,10329.      ,10114.      ,9925.0      ,9591.0
c     +  ,9311.0      ,9198.0      ,9085.0      ,9053.0      ,8950.0
c     +  ,8644.0      ,8771.0      ,8281.0      ,8171.0      ,8150.0
c     +  ,8048.0      ,7864.0      ,7777.0      ,7458.0      ,7544.0
c     +  ,7494.0      ,7135.0      ,6915.0      ,6911.0      ,6819.0
c     +  ,6531.0      ,6370.0      ,6439.0      ,6223.0      ,6116.0
c     +  ,6112.0      ,5948.0      ,5741.0      ,5781.0      ,5508.0
c     +  ,5496.0      ,5243.0      ,5153.0      ,5131.0      ,4980.0
c     +  ,4952.0      ,4763.0      ,4675.0      ,4688.0      ,4367.0
c     +  ,4397.0      ,4196.0      ,4239.0      ,4059.0      ,3917.0
c     +  ,3847.0      ,3920.0      ,3748.0      ,3709.0      ,3643.0
c     +  ,3525.0      ,3382.0      ,3337.0      ,3251.0      ,3231.0
c     +  ,3179.0      ,3001.0      ,3024.0      ,2951.0      ,2858.0
c     +  ,2786.0      ,2795.0      ,2685.0      ,2706.0      ,2621.0
c     +  ,2478.0      ,2390.0      ,2369.0      ,2387.0      ,2289.0
c     +  ,2242.0      ,2164.0      ,2048.0      ,2073.0      ,1993.0
c     +  ,1940.0      ,2011.0      ,1890.0      ,1793.0      ,1736.0
c     +  ,1710.0      ,1720.0      ,1670.0      ,1724.0      ,1589.0
c     +  ,1568.0      ,1529.0      ,1551.0      ,1496.0      ,1430.0
c     +  ,1490.0      ,1350.0      ,1383.0      ,1313.0      ,1308.0
c     +  ,1195.0      ,1233.0      ,1193.0      ,1168.0      ,1127.0
c     +  ,1159.0      ,1060.0      ,1022.0      ,987.00      ,995.00
c     +  ,955.00      ,958.00      ,928.00      ,896.00      ,913.00
c     +  ,876.00      ,817.00      ,793.00      ,796.00      ,818.00
c     +  ,762.00      ,703.00      ,723.00      ,712.00      ,676.00
c     +  ,698.00      ,727.00      ,658.00      ,625.00      ,631.00
c     +  ,593.00      ,569.00      ,553.00      ,541.00      ,544.00
c     +  ,568.00      ,497.00      ,537.00      ,470.00      ,465.00
c     +  ,493.00      ,439.00      ,423.00      ,429.00      ,383.00
c     +  ,392.00      ,405.00      ,411.00      ,372.00      ,358.00
c     +  ,378.00      ,323.00      ,352.00      ,334.00      ,343.00
c     +  ,317.00      ,331.00      ,280.00      ,329.00      ,310.00
c     +  ,291.00      ,302.00      ,280.00      ,252.00      ,268.00
c     +  ,251.00      ,245.00      ,242.00      ,252.00      ,227.00
c     +  ,223.00      ,227.00      ,219.00      ,207.00      ,198.00
c     +  ,194.00      ,192.00      ,152.00      ,174.00      ,167.00
c     +  ,179.00      ,169.00      ,168.00      ,165.00      ,168.00
c     +  ,151.00      ,162.00      ,149.00      ,137.00      ,143.00
c     +  ,138.00      ,142.00      ,122.00      ,128.00      ,111.00
c     +  ,111.00      ,131.00      ,104.00      ,116.00      ,103.00
c     +  ,105.00      ,117.00      ,95.000      ,100.00      ,80.000
c     +  ,105.00      ,91.000      ,78.000      ,97.000      ,89.000
c     +  ,84.000      ,87.000      ,85.000      ,90.000      ,96.000
c     +  ,55.000      ,64.000      ,88.000      ,57.000      ,67.000
c     +  ,76.000      ,72.000      ,54.000      ,42.000      ,62.000
c     +  ,59.000      ,63.000      ,56.000      ,36.000      ,67.000
c     +  ,50.000      ,44.000      ,47.000      ,40.000      ,49.000
c     +  ,39.000      ,43.000      ,47.000      ,32.000      ,38.000
c     +  ,37.000      ,36.000      ,44.000      ,28.000      ,34.000
c     +  ,31.000      ,39.000      ,31.000      ,26.000      ,30.000
c     +  ,34.000      ,36.000      ,28.000      ,32.000      ,24.000
c     +  ,28.000      ,32.000      ,27.000      ,18.000      ,24.000
c     +  ,24.000      ,18.000      ,16.000      ,24.000      ,17.000
c     +  ,14.000      ,21.000      ,16.000      ,23.000      ,21.000
c     +  ,19.000      ,12.000      ,18.000      ,22.000      ,18.000
c     +  ,12.000      ,18.000      ,18.000      ,19.000      ,17.000
c     +  ,12.000      ,13.000      ,10.000      ,16.000      ,12.000
c     +  ,13.000      ,15.000      ,12.000      ,13.000      ,12.000
c     +  ,7.0000      ,17.000      ,11.000      ,12.000      ,10.000
c     +  ,11.000      ,12.000      ,12.000      ,6.0000      ,5.0000
c     +  ,7.0000      ,7.0000      ,5.0000      ,15.000      ,9.0000
c     +  ,7.0000      ,9.0000      ,10.000      ,9.0000      ,10.000
c     +  ,8.0000      ,4.0000      ,13.000      ,5.0000      ,8.0000
c     +  ,9.0000      ,5.0000      ,10.000      ,9.0000      ,4.0000
c     +  ,3.0000      ,4.0000      ,2.0000      ,4.0000      ,2.0000
c     +  ,8.0000      ,5.0000      ,4.0000      ,4.0000      ,7.0000
c     +  ,3.0000      ,7.0000      ,3.0000      ,2.0000      ,2.0000
c     +  ,2.0000      ,2.0000      ,5.0000      ,4.0000      ,1.0000
c     +  ,1.0000      ,3.0000      ,8.0000      ,1.0000      ,4.0000
c     +  ,6.0000      ,2.0000      ,1.0000      ,5.0000      ,2.0000
c     +  ,2.0000      ,7.0000      ,3.0000      ,1.0000      ,1.0000
c     +  ,3.0000      ,2.0000      ,2.0000      ,1.0000      ,2.0000
c     +  ,1.0000      ,4.0000      ,1.0000      ,1.0000      ,3.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,1.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,2.0000
c     +  ,1.0000      ,6.0000      ,1.0000      ,1.0000      ,3.0000
c     +  ,1.0000      ,4.0000      ,3.0000      ,1.0000      ,1.0000
c     +  ,2.0000      ,3.0000      ,1.0000      ,2.0000      ,1.0000
c     +  ,1.0000      ,2.0000      ,1.0000      ,3.0000      ,1.0000
c     +  ,1.0000      ,2.0000      ,2.0000      ,1.0000      ,1.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,1.0000/
c+CDE, FIELD.
c      DIMENSION AREAD1(28),AREAD2(28)
c      CHARACTER*24 FILE1,FILE2
c+SELF, IF=-VAX.
c      DATA FILE1
c     &/'AMS_SS_2002_AC_GEO.LIN'/
c      DATA FILE2
c     &/'AMS_SS_2002_GCR_GEO.LIN'/
c+SELF, IF=VAX.
c      DATA FILE1
c     &/'gener:AMS_SS_2002_AC_GEO.LIN'/
c      DATA FILE2
c     &/'gener:AMS_SS_2002_GCR_GEO.LIN'/
c+SELF.
c      DO I = 1, 28
c       CALL HBOOK1(20000+I,'Spectra',120,1.005,5.005,0.)
c      ENDDO
c      OPEN(10, FILE=
c     +amsdatadir(1:amsdlength)//FILE1, FORM='FORMATTED',STATUS='OLD')
c      OPEN(11, FILE=
c     +amsdatadir(1:amsdlength)//FILE2, FORM='FORMATTED',STATUS='OLD')
c      DO I = 1, 8
c      READ(10,*)
c      READ(11,*)
c      ENDDO
c
c      eold=0
c      DO I = 1,115
c       READ(10,*) E, AREAD1
c       READ(11,*) E, AREAD2
c       AE = ALOG10(E)
c       de=e-eold
c       DO J = 1, 28
c        CALL HFILL(20000+J,AE,0.,(AREAD1(J)+AREAD2(J))*de)
c       ENDDO
c       eold=e
c      ENDDO
c      do j=1,6
cc       ee=hrndm1(20000+j)
c      enddo
c      CLOSE(10)
c      CLOSE(11)
c      call hbook1(30001,'qq',12,0.,6.,0.)
cC     20500. 7600. 3850. 1750. 880. 390. 157. 55. 19. 5. 1.8 1.3
c      call hf1(30001,0.75,12900.)
c      call hf1(30001,1.25,4550.)
c      call hf1(30001,1.75,1810.)
c      call hf1(30001,2.25,846.)
c      call hf1(30001,2.75,376.)
c      call hf1(30001,3.25,177.)
c      call hf1(30001,3.75,67.)
c      call hf1(30001,4.25,29.)
c      call hf1(30001,4.75,7.3)
c      call hf1(30001,5.25,2.7)
c      call hf1(30001,5.75,.4)
c* book & fill ap
c      call hbook1(120001,'ap',500,50.,50050.,0.)
c          bw=100.
c          al=50.
c          bl=50050.
c          do i=1,500
c           r=yap(i)
c           xx=(i-1)*bw+al+bw/2.
c           call hf1(120001,xx,r)
c          enddo
c
c      RETURN
c      END
c     
C     DECK  ID>, ABCR2.  
      subroutine abcr2(i,a_p,z_p,a_t,z_t,p_p,ipart)
      parameter (inmat=1000)
      common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +     sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
      common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
*     
****  G.L. 2/2/00 *********
      
      common /hvsec/ihvse(2)
*     
      real *4 st,se(3),sq(5)
*     
      REAL *4 TRIPA_T
      integer IFLAG_T
*     
*     reduce sigma by 2^2/3=0.63  if stranglet
*     
      if(ipart.eq.113)then
         xfac=0.63
      else
         xfac=1
      endif              
      
      TRIPA_T=0.0
      IFLAG_T=0
*     
*************************
      
      if(i.gt.inmat)then
         write(*,*)'ABCR2 - too many media'
         i=inmat
      endif
      
      sigma_t(i)=0
      call vzero(sigma_el(1,i),3)
      call vzero(sigma_q(1,i),5)
      call abcross(1,a_p,a_t,z_p,z_t,p_p,
     +     st,se,sq)
      sigma_t(i)=sigma_t(i)+st*xfac
      do j=1,3
         sigma_el(j,i)=sigma_el(j,i)+se(j)*xfac
      enddo
      do j=1,5
         sigma_q(j,i)=sq(j)*xfac
      enddo
****  G.L. 2/2/00 *********
*     
      if (ihvse(1).eq.2) then                   
         CALL TRIPA(ipart,P_P,Z_P,A_P,Z_T,A_T,TRIPA_T,IFLAG_T)            
         IF (IFLAG_T.EQ.1) THEN  
            sigma_q(1,i)=0
            sigma_t(i)=TRIPA_T*xfac
         ENDIF
         CALL ANTID2(ipart,A_P,A_T,p_p,BESS_T,IFLAG_T)         
         IF (IFLAG_T.EQ.1) THEN  
            sigma_q(1,i)=0
            sigma_t(i)=BESS_T*xfac
         ENDIF
      endif                     ! ihvse(1).eq.2
*     
****************************
      summ=sigma_q(1,i)+sigma_el(1,i)*xfac
      if(summ.gt.sigma_t(i))then
c     write(*,*)'ABCR2 - E - wrong sigma_tot',i,sigma_t(i),
c     +  sigma_q(1,i),sigma_el(1,i)
         sigma_q(1,i)=0
      endif
      
      
      
      
      end
CDECK  ID>, ANTID2
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                  8/1/2007
c AntiDeuteron inelastic cross section
c                                           by F.Giovacchini  
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      SUBROUTINE ANTID2(ipart,A_P,A_T,P_P,SIGMA_T,IFLAG)
      
      real A_P,A_T
      real P_P
      integer IFLAG       
      real *4 sigma_t

*
*     Input:
*     a_p,a_t,p_p [atom number projectile & target
*     and momentum of projectile [GeV/c]]
*     Output:
*     sigma_T=total inelastic cr_sec [mbarn]

      sigma_T=0.      
      IFLAG=0      
      IF (IPART.NE.145) GOTO 65

*     IPART.EQ.145 =>Anti Deuteron       

      IFLAG=1
 
      CALL ANTIP(ipart,P_P,AP_CROSS)  
      SIGMA_T=AP_CROSS*A_T**(2./3.)
  
      if (sigma_T.lt.0.) sigma_T=0.
      if (sigma_T.eq.0.) IFLAG=0 
 65   CONTINUE
      RETURN
      END 
********************************************************+
CDECK  ID>, ANTIP
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                  06/12/2006
c AntiProton-deuteron inelastic cross section
c                                           by F.Giovacchini  
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      SUBROUTINE ANTIP(ipart,P_P,SIGMA)
      real A_D,A_AP
      real Z_D,Z_AP,r_t,r_p
      real P_P,M_D,M_AP,P_PM,E_k,T1,R_C,D,R,E_cm,C_E,S,dE,B,E_lab
      integer IFLAG       
      real *4 sigma_t
      DATA PI / 3.1415927 /
      DATA r0 / 1.1 /
   
      IF(P_P.LT.10) THEN
         Z_AP=-1   
         A_AP=1
         Z_D=1
         A_D=2
      
*     M_D  is the Deuteron Mass [MeV]
         M_D=A_D*938 
*     M_AP  is the Antiproton Mass [MeV]
         M_AP=938  
         
         r_p=RADIUS(A_D)
         r_t=RADIUS(A_AP)
         
*     P_P  is    DEUTERON'S momentum in "MeV/c"     
         P_PM=P_P*1000.      
*     Kinetc Energy of the antideuteron projectile
*     per NUMBER OF NUCLEONS (dividing by 2=A_P) [GeV/A]
         E_k=(sqrt((P_PM**2)+(M_D**2))-M_D)/A_D 
         E_lab=(E_k*A_D)+M_D         
*     Energy in the centre of mass for the antideuteron-proton system [GeV] 
         E_cm=sqrt((M_AP**2)+(M_D**2)+(2.*M_AP*E_lab))-M_P-M_T
         
         R_C=13.5
         T1=40.
         D=1.55+(0.16/(1.+(exp((500.-E_k)/200.))));
*     // Parameter C_E      for D+X systems:+(exp((500.-E_k)/200.))
         C_E=D*(1-exp(-E_k/T1))-((0.292*exp(-E_k/792.)))
*     dropped:-(*cos(0.229*(E_k**(0.453))));(D*(1-exp(-E_k/T1))
*     //Parameter S  
         S=((A_D**(1./3.))*(A_AP**(1./3.)))/
     +        ((A_D**(1./3.))+(A_AP**(1./3.)))        
*     //Parameter dE       for D+X systems:
         dE=(1.85*S)+(0.16*(S/(E_cm**(1./3.))))-C_E
*     //    Parameter R for D+X systems: 
         R=rp+ru+(1.1*((A_D**(1./3.))+(A_AP**(1./3.)))/(E_cm**(1./3.)))
*     //         Parameter B       for D+X systems:   
         B=(1.44*Z_D*Z_AP)/R
         
         SIGMA=10*PI*(r0**2)*((A_D**(1./3.)+A_AP**(1./3.)+dE)**2)
     +        *(1.-(R_C*B/E_cm))
      ELSE
         IF(P_P.LT.600) THEN
            SIGMA=76.+200.44/P_P
         ELSE 
            SIGMA=76.
         ENDIF
      ENDIF
      RETURN
      END 


CDECK  ID>, TRIPA. 
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                   2/2/2000
c Deuteron total cross section
c                                               by G.LAMANNA  
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      SUBROUTINE TRIPA(ipart,P_P,Z_P,A_P,Z_T,A_T,SIGMA_T,IFLAG)
c

        real A_P,A_T
        integer IFLAG
        real M_P,P_P,M_T,Z_T,Z_P,r_t,r_p
        real E_lab,E_cm,T1,D,C_E,S,dE,R,B,X1,S_L,X_m
        real R_C,tem_p,tem_t        
        real *4 sigma_t
        DATA PI / 3.1415927 /
        DATA r0 / 1.1 /
*        DATA M_P / 1876. /  
        real P_PM
*
*    Input:
*     a_p,a_t,z_p,z_t,p_p [atom number projectile & target,Z p & t
*      and momentom of projectile [MeV/c]]
*     Transform p from GeV/c to Mev/c is mandatory
*
*    Output:
*     sigma_T=total inelastic cr_sec [mbarn]


        sigma_T=0.

        IFLAG=0
        IF (IPART.NE.45) GOTO 66
*       
        IFLAG=1 
*
*       ASSUMING TARGET MASS M_T=A_T * 938.0 MeV/c2 (PROTON MASS)

        M_T=A_T*938.0 
        M_P=A_P*938.0

*
*    P_P  is    DEUTERON'S momentum in                           "MeV/c"           

       P_PM=P_P*1000.

* 
*    E_k  is    DEUTERON'S KINETIC ENERGY 
*               per NUMBER OF NUCLEONS (dividing by 2=A_P) in    "MeV/A"    
*

      E_k=((sqrt((P_PM**2)+(M_P**2))-M_P)/2.)  
*

* ***************     Parameter T1  and R_C  for D+X ....

      T1=23.
      R_C=1.
* ***************     Parameter D for D+X ....

      D=1.65+(0.1/(1.+(exp((500.-E_k)/200.))))


       if (Z_T.eq.1.0) THEN
         if (A_T.lt.2.0.and.A_T.gt.0) then ! d+p becomes p+d

         tem_p=0.0
         tem_t=0.0
         tem_p=A_P
         tem_t=A_T
         A_P=tem_t
         A_T=tem_p

         tem_p=0.0
         tem_t=0.0
         tem_p=M_P
         tem_t=M_T
         M_P=tem_t
         M_T=tem_p

         T1=18.
         R_C=13.5
         D=1.85+(0.16/(1.+(exp((500.-E_k)/200.))))
       endif
      endif
*************************************
* Center of mass energy : E_cm
*
      E_lab=(E_k*A_P)+M_P
*
      
      E_cm=sqrt((M_P**2)+(M_T**2)+(2.*M_T*E_lab))-M_P-M_T
*
*
***
*

* ************        Parameter C_E      for D+X systems:

      C_E=(D*(1-exp(-E_k/T1)))-((0.292*exp(-E_k/792.))*
     +       cos(0.229*(E_k**0.453)))

*
*
* ************        Parameter S        for D+X systems:

        S=((A_P**(1./3.))*(A_T**(1./3.)))/
     +     ((A_P**(1./3.))+(A_T**(1./3.)))  

*
*
* ************        Parameter dE       for D+X systems:

        dE=(1.85*S)+(0.16*(S/(E_cm**(1./3.))))-C_E+(0.91*
     +   ((A_T-2*Z_T)*Z_P/(A_T*A_P)))  

*
*
* ************ radii r_p and r_t         for D+X systems:

              r_p=RADIUS(A_P)
              r_t=RADIUS(A_T)

*
*
* ************         Parameter R       for D+X systems:
        
        R=r_p+r_t+(1.2*((A_P**(1./3.)) +
     +      (A_T**(1./3.)))/(E_cm**(1./3.)))

*
*
* ***********         Parameter B       for D+X systems:

        B=(1.44*Z_P*Z_T)/R

*
* 
************    Optical model multiplier for D+X systems:

        X1=2.83-(3.1*A_T/100.)+(1.7*(A_T**2)/10000.)

        S_L=1.2+1.6*(1-exp(-E_k/15.))
*
*
************************* Parameter X_m :
*
        X_m=1-(X1*(exp(-E_k/(X1*S_L)))) 
*
*
* ***********

       if (A_T.eq.2.0.and.Z_T.eq.1.) THEN     ! d+d 
        R_C=13.5
       endif

       if (A_T.eq.4.0.and.Z_T.eq.2.) THEN     ! d+He4 
        R_C=13.5
       endif

       if (A_T.eq.12.0.and.Z_T.eq.6.) THEN     ! d+C 
        R_C=6.0
       endif



*
*
* ****** FINALLY THE TOTAL CROSS SECTION sigma_T in  "mbarn" : 


c         sigma_T=10.*PI*(r0**2)*(((A_P**(1./3.))+(A_T**(1./3.)
c     +               )+dE)**2)*(1-(R_C*B/E_cm))*X_m
         sigma_T=10.*PI*(r0**2)*(((A_P**(1./3.))+(A_T**(1./3.)
     +               )+dE)**2)*(1-(R_C*B/E_cm))

          if (sigma_T.lt.0.) sigma_T=0.
          if (sigma_T.eq.0.) IFLAG=0


 66       CONTINUE
          RETURN
          END
********************************
*** density of a nucleus in the hard sphere model: rho_av=(rho_Ap + rho_At)/2 
*   where the density of nucleus projectile rho_Ap and nucleus target rho_At 
*   are : 
*
*    rho_Ai=Ai/((4*PI/3)*r_i**3)
*
*   where the radius of the nucleus r_i is
*
*    r_i=1.29*(r_i)_rms
*
*   It is provided by FUNCTION RADIUS
*
*
******************************************************************
*
        FUNCTION RADIUS(A)
        DIMENSION NA(23),RMS(23)
        DATA NA/1,2,3,4,6,7,9,10,11,12,13,14,15,16,17,18,
     +         19,20,22,23,24,25,26/
        DATA RMS/0.85,2.095,1.976,1.671,2.57,2.41,2.519,
     +          2.45,2.42,
     +          2.471,2.440,2.58,2.611,2.730,2.662,
     +          2.727,2.900,
     +          3.040,2.969,2.94,3.075,3.11,3.06/
        FACT=SQRT(5./3.)
        IA=A+0.4
        RADIUS=FACT*(0.84*A**(1./3.)+0.55)
        DO 1 I=1,23
        IF (IA.EQ.NA(I)) GO TO 2
        GO TO 1
 2      RADIUS=FACT*RMS(I)
 1      CONTINUE
        RETURN
        END




cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

CDECK  ID>, ABCROSS.
       subroutine abcross(icas,a_p,a_t,z_pb,z_t,p_p,
     +  sigma_t,sigma_el,sigma_q)
        real *4 sigma_t,sigma_el(3),sigma_q(5)
CDE, ABC.
*
*     Input:
*     icase (normally should be 1)
*     a_p,a_t,z_p,z_t,p_p [atom number projectile & target,Z p & t
*      and momentom of projectile [Gev/c]]
*
*     Output
*
*     sigma_t=total cr_sec [mbarn]
*     sigma_el(3) = sigma elastic [mbarn]
*                   slope [gev/c]^-2
*                   reserved
*     sigma_q(5)  = sigma quasi elastic [mbarn]
*                   sigma q -el (1)
*                   slope    [gev/c]^-2   (1)
*                   slope    [gev/c]^-2   (2)
*                   reserved
*
       real al_d(6)
       data al_d/1.91,1.66,3*0.,1.4/
       data al_par /0.225/
       p_pn=p_p/a_p
       z_p=abs(z_pb)
       if(z_pb.gt.0)then
       snn=sig_pn(1,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(2,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       else
       snn=sig_pn(3,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(4,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       snnel=sig_pn(5,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(6,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       snnel=snnel/a_p/a_t
       endif
       snn=snn/a_p/a_t
       icase=1

       call vzero(sigma_el(1),3)
       call vzero(sigma_q(1),5)
       sigma_t=0
       if(icase.eq.1)then
*
* Factorization
*
        iap=a_p+0.5
        iat=a_t+0.5
       if(iap.ne.2.and.iap.ne.4.and.iap.ne.12)then
        alpha_p=2./3+4./3/a_p**al_par
       else
        inum=iap/2
        alpha_p=al_d(inum)
       endif
       if(iat.ne.2.and.iat.ne.4.and.iat.ne.12)then
        alpha_t=2./3+4./3/a_t**al_par
       else
        inum=iat/2
        alpha_t=al_d(inum)
       endif
       if(a_p.gt.a_t)then
        a_l=a_t
        a_h=a_p
        alpha_l=alpha_t
        alpha_h=alpha_p
       else
        a_l=a_p
        a_h=a_t
        alpha_l=alpha_p
        alpha_h=alpha_t
       endif
        alpha_l=0.5*alpha_l
        alpha_h=0.5*alpha_h
       rr=a_l/a_h
       alpha_hh=rr*alpha_h+(1-rr)*alpha_h**0.5
       sigma_t=snn*a_h**alpha_hh*a_l**alpha_l
       sigma_t_2=144*(a_p**0.33334+a_t**0.33334-1.48)**2*snn/44
c       if(sigma_t_2.gt.sigma_t.and.a_p+a_t.gt.10)then
c        sigma_t=(sigma_t+sigma_t_2)/2.
c       endif
*
* bnn
*
100    continue
       bnn=fb_ab(p_pn)
       ab=0.53
       b_ab=2.2*bnn*(a_t*a_p)**0.53
       cts=0.389*16*3.14159
       dsdt=sigma_t**2/cts
       sigma_el(1)=dsdt/b_ab
       sigma_el(2)=b_ab
       if(sigma_el(1).ge.sigma_t)then
*
* unitarity viol
*
        sigma_el(1)=sigma_t
       endif
       if(z_pb.le.0.and.p_pn.le.20.)then
        iap=a_p+0.5
        iat=a_t+0.5
       if(iap.ne.2.and.iap.ne.4.and.iap.ne.12)then
        alpha_p=2./3+4./3/a_p**al_par
       else
        inum=iap/2
        alpha_p=al_d(inum)
       endif
       if(iat.ne.2.and.iat.ne.4.and.iat.ne.12)then
        alpha_t=2./3+4./3/a_t**al_par
       else
        inum=iat/2
        alpha_t=al_d(inum)
       endif
       if(a_p.gt.a_t)then
        a_l=a_t
        a_h=a_p
        alpha_l=alpha_t
        alpha_h=alpha_p
       else
        a_l=a_p
        a_h=a_t
        alpha_l=alpha_p
        alpha_h=alpha_t
       endif
        alpha_l=0.5*alpha_l
        alpha_h=0.5*alpha_h
       rr=a_l/a_h
       alpha_hh=rr*alpha_h+(1-rr)*alpha_h**0.5
       sigma_el(1)=snnel*a_h**alpha_hh*a_l**alpha_l
       endif
*
* quasi -elastic
* we will assume it follows the total one
*
        bq=14.3+12/a_h
        bq2=9.
        if(a_h.gt.4)then
          dsqdt=11.5*bnn*a_h**0.3333*a_l**alpha_l
          sigma_q(1)=dsqdt/bq+dsqdt/4.5/bq2
          sigma_q(2)=dsqdt/bq
          sigma_q(3)=bq
          sigma_q(4)=bq2
        else
          dsqdt=6.5*bnn*(a_h*a_l)**0.5
          sigma_q(1)=dsqdt/bq
          sigma_q(2)=dsqdt/bq
          sigma_q(3)=bq
          sigma_q(4)=bq2
        endif

       else if(icase.eq.2)then
*
* Geometrical picture
*
       sigma_t=144*(a_p**0.33334+a_t**0.33334-1.48)**2
       if(p_pn.lt.1.5)goto 100
       sigma_i=78*(a_p**0.33334+a_t**0.33334-1.25)**2
       sigma_e=sigma_t-sigma_i
*
* bnn
*

       bnn=fb_ab(p_pn)
       ab=0.53
       b_ab=2.2*bnn*(a_t*a_p)**0.53
       cts=0.389*16*3.14159
       dsdt=sigma_t**2/cts
       sigma_el(1)=sigma_e
       sigma_el(2)=dsdt/sigma_e
       endif
       end
        function fb_ab(p)
         parameter (thr=0.)
        if(p.gt.thr)then
         fb_ab=6.3+1.03*log(p)
        else
         fb_ab=0.
        endif
       end
CDECK  ID>, SIG_PN. 
       function sig_pn(icase,pmom)
*
*  return proton-proton (icase=1) or proton-neutron (icase=2)
*  p-ap icase=3 p-an icase=4
*  total cross section in mbarn vs pmom in Gev/c
*

        if(icase.eq.1)then
* proton
         if(pmom.ge.1.5)then
          sig_pn=48+0.522*(log(pmom))**2-4.51*log(pmom)
         else if(pmom.ge.1.)then
           sig_pn=25*exp(1.23*(pmom-1))
         else if(pmom.ge.0.34)then
           sig_pn=25
         else if (pmom.ge.0.1)then
           sig_pn=25*exp(12.5*(0.34-pmom))
         else
           sig_pn=500.
         endif
        else if(icase.eq.2)then
* neutron
         if(pmom.ge.3.)then
          sig_pn=47.3+0.51*(log(pmom))**2-4.27*log(pmom)
         else if(pmom.ge.1.)then
          sig_pn=40.2+1.5*(pmom-1)
         else if(pmom.ge.0.34)then
          sig_pn=40.2*exp(0.85*(1.-pmom))
         else if(pmom.ge.0.1)then
           sig_pn=70*exp(12.5*(0.34-pmom))
         else
           sig_pn=1400
         endif
        else if(icase.eq.3)then
        sig_pn=38.4+77.6*pmom**(-0.64)+0.26*(log(pmom))**2-
     +  1.2*log(pmom)
        else if(icase.eq.4)then
         sig_pn=133.6*pmom**(-0.7)-1.22*(log(pmom))**2+
     +   13.7*log(pmom)
        else if(icase.eq.5)then
        sig_pn=10.2+52.7*pmom**(-1.16)+0.125*(log(pmom))**2-
     +  1.28*log(pmom)
        else if(icase.eq.6)then
         sig_pn=36.5-11.9*log(pmom)
        else
         write(*,*)'wrong icase'
         stop
        endif
       end
CDECK  ID>, ABSIEL. 
       subroutine absiel(pmom_p,pmom_t,iuu)
*
* To generate elastic scattering
*
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       real *4 pmom_p(5),u(3),pmom_t(6)
       xp=pmom_p(4)
       phi=2*3.14159*rndm(d)
10       q2=-log(rndm(d))/sigma_el(2,iuu)  !q^2

       xm1=pmom_p(5)
       xm2=pmom_t(5)
       call abkin(xm1,xm2,q2,xp,p1,p1_per,ierr)
       if(ierr.ne.0)goto 10
       sint=p1_per/p1         ! sin(theta)
       pmom_p(4)=p1
       sint=min(sint,1.)
       sint=max(sint,-1.)
       t=asin(sint)
       q=sqrt(q2)
       pmom_t(4)=q
       pmom_t(6)=sqrt(q2+pmom_t(5)**2)-pmom_t(5)

       call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
       u(3)=1.
       u(2)=0.
       u(1)=0.
       call rotate(t,2,u(1),u(2),u(3),tst)
       call rotate(phi,3,u(1),u(2),u(3),tst)
       call rotate(th,2,u(1),u(2),u(3),tst)
       call rotate(ph,3,u(1),u(2),u(3),tst)
       pmom_t(1)=pmom_p(1)*xp-u(1)*p1
       pmom_t(2)=pmom_p(2)*xp-u(2)*p1
       pmom_t(3)=pmom_p(3)*xp-u(3)*p1
       ds=pmom_t(1)**2+pmom_t(2)**2+pmom_t(3)**2
       ds=sqrt(ds)
       if(ds.eq.0)ds=1.
       do kk=1,3
        pmom_t(kk)=pmom_t(kk)/ds
       enddo

       call ucopy(u(1),pmom_p(1),3)
       end
CDECK  ID>, ABSIIN. 
       subroutine absiin(a_p,z_p,a_t,z_t,pmom_p,nsec,p_sec,ierr)
*        input
*        pmom_p(6)   u,v,w,ptotal,mass,etotal
*        a_p,t atomic weight
*        z_p,t 
*        output p_sec(6,1000)   // as in gkin
         real *4 p_sec(6,*)
         real *4 u(3),pmom_p(6)
         a_p=int(a_p+0.5)
         ekin=(pmom_p(6)-pmom_p(5))/a_p
         if(z_t.ne.1.or.a_t.ne.1)then
*
*   call to rqmd
*
          call call_rqmd(a_p,z_p,a_t,z_t,ekin,nsec,p_sec,ierr)
*          write(*,*)' rqmd called',a_t,z_t,nsec
         else
          
          call call_fluka(a_p,z_p,a_t,z_t,ekin,
     +    pmom_p(5),nsec,p_sec,ierr)
*          write(*,*)' fluka called',a_t,z_t,nsec
         endif
         if(ierr.ne.0)then
             nsec=0
             goto 999
         endif

*
* Rotate back
*

c         px=0
c         py=0
c         pz=0
c         ptot=pmom_p(4)
         call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
c         u(1)=0
c         u(2)=0
c         u(3)=ptot
c          call rotate(th,2,u(1),u(2),u(3),tst)
c          call rotate(ph,3,u(1),u(2),u(3),tst)
c           write(*,*)u
c        write(*,*)'proj targe ',a_p,z_p,a_t,z_t,ekin
         do i=1,nsec
          call rotate(th,2,p_sec(1,i),p_sec(2,i),p_sec(3,i),tst)
          call rotate(ph,3,p_sec(1,i),p_sec(2,i),p_sec(3,i),tst)
*
* Check conserv
*      
c          px=px+p_sec(1,i)
c          py=py+p_sec(2,i)
c          pz=pz+p_sec(3,i)
c         write(*,*)i,p_sec(3,i),p_sec(4,i),p_sec(5,i)
         enddo
c         write(*,*)'*** secondary *****'
c         write(*,*)px,py,pz,pmom_p(1)*ptot,
c     +    pmom_p(2)*ptot,pmom_p(3)*ptot
          
*
 999     end

CDECK  ID>, ABSIQE. 
       subroutine absiqe(pmom_p,pmom_t,iuu)
*
* Here quasi elastic cross
*
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       real *4 pmom_p(5),u(3),pmom_t(6)
*
*     choose between two exponents.
*
       x1=sigma_q(2,iuu)/sigma_q(1,iuu)
       if(rndm(d).lt.x1)then
        bt=sigma_q(3,iuu)
       else
        bt=sigma_q(4,iuu)
       endif
       xp=pmom_p(4)
       phi=2*3.14159*rndm(d)
10       q2=-log(rndm(d))/bt  !q^2
       xm1=pmom_p(5)
       xm2=pmom_t(5)
       call abkin(xm1,xm2,q2,xp,p1,p1_per,ierr)
       if(ierr.ne.0)goto 10
       sint=p1_per/p1         ! sin(theta)
       pmom_p(4)=p1
       sint=min(sint,1.)
       sint=max(sint,-1.)
       t=asin(sint)
       pmom_t(4)=sqrt(q2)
       q=sqrt(q2)
       pmom_t(6)=sqrt(q2+pmom_t(5)**2)-pmom_t(5)
       call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
       u(3)=1.
       u(2)=0.
       u(1)=0.
       call rotate(t,2,u(1),u(2),u(3),tst)
       call rotate(phi,3,u(1),u(2),u(3),tst)
       call rotate(th,2,u(1),u(2),u(3),tst)
       call rotate(ph,3,u(1),u(2),u(3),tst)
       pmom_t(1)=pmom_p(1)*xp-u(1)*p1
       pmom_t(2)=pmom_p(2)*xp-u(2)*p1
       pmom_t(3)=pmom_p(3)*xp-u(3)*p1
       ds=pmom_t(1)**2+pmom_t(2)**2+pmom_t(3)**2
       ds=sqrt(ds)
       if(ds.eq.0)ds=1.
       do kk=1,3
        pmom_t(kk)=pmom_t(kk)/ds
       enddo
       call ucopy(u(1),pmom_p(1),3)
       end
CDECK  ID>, ABKIN.  
            subroutine abkin(m1,m2,q2,p,p1,p1_per,ierr)
            real *4 m1,m2,p,p1,p1_per
            real *8 g,e,p1d,p1_par,pd,q2d,m1d,m2d
*
*   input:
* m1,m2  - masses of nuclea
* q2 -  q^2
* p -proj momentun
*   output:
* p1 -proj momentum after scatt
* p1_per proj mom perp
*
            q2d=q2
            pd=p
            ierr=0
            m1d=m1
            m2d=m2
            e=sqrt(pd**2+m1d**2)
            g=e+m2
            p1d=g**2+q2d+m2d**2-m1d**2-2*g*sqrt(q2d+m2d**2)
             if(p1d.lt.0)p1d=0.
             p1d=sqrt(p1d)
            p1=p1d
            p1_par=pd/2+p1d/pd/2*p1d-q2d/2/pd
            if(abs(p1d).lt.abs(p1_par))then
             write(*,*)' ABKIN - too big q^2 requested',q2
             ierr=1
            else
             p1_per=sqrt((p1d-p1_par)*(p1d+p1_par))
            endif
            end
CDECK  ID>, ABLORZ. 
      SUBROUTINE ROTATE (A,IAXE,PX,PY,PZ,TEST)

      IMPLICIT NONE

      INTEGER IAXE
      REAL    V(3) , A , PX , PY , PZ
      LOGICAL TEST

      TEST = .FALSE.

      IF (IAXE.EQ.1)THEN
         V(1) = PX
         V(2) = PY*COS(A) - PZ*SIN(A)
         V(3) = PY*SIN(A) + PZ*COS(A)
         TEST = .TRUE.
         GOTO 100
      ENDIF
      IF (IAXE.EQ.2)THEN
         V(1) = PZ*SIN(A) + PX*COS(A)
         V(2) = PY
         V(3) = PZ*COS(A) - PX*SIN(A)
         TEST = .TRUE.
         GOTO 100
      ENDIF
      IF (IAXE.EQ.3)THEN
         V(1) = PX*COS(A) - PY*SIN(A)
         V(2) = PX*SIN(A) + PY*COS(A)
         V(3) = PZ
         TEST = .TRUE.
         GOTO 100
      ENDIF

100   PX = V(1)
      PY = V(2)
      PZ = V(3)

      RETURN
      END
      SUBROUTINE BOOST_ALONG_Z (B , E , PZ , Q)

      IMPLICIT NONE
      REAL    B,G,E,PZ,E_DUM
      LOGICAL Q

      Q = .TRUE.
      IF (ABS(B).GT.1.0.OR.E.LT.ABS(PZ)) THEN
         Q = .FALSE.
         RETURN
      ENDIF

      G = SQRT(1/(1-B**2))
      E_DUM = E
      E     = G * (   E_DUM - B*PZ)
      PZ    = G * (-B*E_DUM + PZ)

      RETURN
      END
      SUBROUTINE CONV_R_P (X,Y,Z,T,P,R)

      IMPLICIT NONE

      LOGICAL  Q
      REAL     X,Y,Z,T,P,R,PI
      PARAMETER (PI = 3.14159)

      Q = .TRUE.
      R = SQRT (X**2 + Y**2 + Z**2)
      IF (ABS(R).LT.1E-30) THEN
         T = 0.0
         P = 0.0
         RETURN
      ENDIF
      T = ACOS (Z/R)
      IF (ABS(X).LT.1E-30) THEN
         P = SIGN (PI/2 ,Y)
      ELSE
         P = ATAN (Y/X)
         IF (X.LT.0.0) P = P + 3.14159
      ENDIF

      RETURN
      END
      SUBROUTINE POL_REC ( P,THE,PHI,PX,PY,PZ )

      IMPLICIT NONE
      REAL     P,THE,PHI , PX,PY,PZ

      PX = P * SIN(THE) * COS(PHI)
      PY = P * SIN(THE) * SIN(PHI)
      PZ = P * COS(THE)

      RETURN
      END
CDECK  ID>, ABDIST. 
       subroutine abdist
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      DOUBLE PRECISION PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      DOUBLE PRECISION EMMU,PMASS,AVO
*
      PARAMETER (PI=3.14159265358979324D0)
      PARAMETER (TWOPI=6.28318530717958648D0)
      PARAMETER (PIBY2=1.57079632679489662D0)
      PARAMETER (DEGRAD=0.0174532925199432958D0)
      PARAMETER (RADDEG=57.2957795130823209D0)
      PARAMETER (CLIGHT=29979245800.D0)
      PARAMETER (BIG=10000000000.D0)
      PARAMETER (EMASS=0.0005109990615D0)
      PARAMETER (EMMU=0.105658387D0)
      PARAMETER (PMASS=0.9382723128D0)
      PARAMETER (AVO=0.60221367D0)
*
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
      parameter (avogad=6.022e23)
      parameter   (avogmb=1.e27/avogad)
      parameter (xm_proton=0.938)

         ZELA  = BIG
         Zine  = BIG
         zqel=big
         a_p=amass/xm_proton
         z_p=abs(charge)
         P_p = SQRT(GEKIN*(GEKIN+2*amass))
         JMA = LQ(JMATE-NMAT)
         NCOMP = ABS(Q(JMA+11))
         DENS = Q(JMA+8)
         JMIXT = LQ(JMA-5)
         IF ( NCOMP .LE. 1) THEN
            a_t = a
            Z_t = z
c            write(*,*)'ncomp ',z_t,a_t,dens
            CALL abcr2(1,a_p,z_p,a_t,z_t,p_p,ipart)
            if(sigma_t(1)-sigma_el(1,1)-sigma_q(1,1).gt.0)then
             zin=a*avogmb/(sigma_t(1)-sigma_el(1,1)-sigma_q(1,1))
            else
             zin=big
            endif
            zel=a*avogmb/sigma_el(1,1)
            if(sigma_q(1,1).gt.0)then
             zqe=a*avogmb/sigma_q(1,1)
            else
             zqe=big
            endif
            IF (ZEL.LT.BIG) THEN
               ZELA = ZEL/DENS
            END IF
            IF (Zin.LT.BIG) THEN
               Zine = Zin/DENS
            END IF
            IF (Zqe.LT.BIG) THEN
               Zqel = Zqe/DENS
            END IF
         ELSE
            zin1 = 0.
            zel1=0
            zqe1=0
            DO 10 K=1,NCOMP
               CALL abcr2(k,a_p,z_p,Q(JMIXT+K),
     +         Q(JMIXT+NCOMP+K),p_p,ipart)
            if(sigma_t(k)-sigma_el(1,k)-sigma_q(1,k).gt.0)then
            zin=q(jmixt+k)*avogmb/(sigma_t(k)-sigma_el(1,k)-
     +      sigma_q(1,k))
            else
            zin=big
            endif
            zel=q(jmixt+k)*avogmb/sigma_el(1,k)
            if(sigma_q(1,k).gt.0)then
            zqe=q(jmixt+k)*avogmb/sigma_q(1,k)
            else
            zqe=big
            endif
               IF (zin.Ge.BIG) THEN
                  ZIN1 = 0.0 + ZIN1
               ELSE
                  ZIN1 = DENS*Q(JMIXT+2*NCOMP+K)/Zin + ZIN1
               END IF
               ABINX(K) = ZIN1
               IF (zqe.Ge.BIG) THEN
                  Zqe1 = 0.0 + Zqe1
               ELSE
                  Zqe1 = DENS*Q(JMIXT+2*NCOMP+K)/Zqe + Zqe1
               END IF
               ABQEX(K) = Zqe1
               IF (zel.Ge.BIG) THEN
                  Zel1 = 0.0 + Zel1
               ELSE
                  Zel1 = DENS*Q(JMIXT+2*NCOMP+K)/Zel + Zel1
               END IF
               ABelX(K) = Zel1
   10       CONTINUE
            ANXNOR = ZIN1
            ELXNOR = ZEL1
            qexnor=zqe1
            IF (ZIN1.GT.0.0) THEN
               ZINE = 1./ZIN1
            END IF
            IF (ZEL1.GT.0.0) THEN
               ZELA = 1./ZEL1
            END IF
            IF (Zqe1.GT.0.0) THEN
               Zqel = 1./Zqe1
            END IF
         END IF
      IF (ZINE.EQ.BIG) THEN
         SINE = 0.0
      ELSE
         SINE = 1./ZINE
      END IF
      IF (ZELA.EQ.BIG) THEN
         SELA = 0.0
      ELSE
         SELA = 1./ZELA
      END IF
      IF (Zqel.EQ.BIG) THEN
         Sqel = 0.0
      ELSE
         Sqel = 1./zqel
      END IF
      FSIG = SINE + SELA +sqel
      IF (FSIG .LE. 0) THEN
         SHADR = BIG
      ELSE
         SHADR = ZINTHA/FSIG
      END IF
c      write(*,*)'shadr',shadr,a_t,z_t,fsig,zintha
  999 CONTINUE
      END

CDECK  ID>, ABUFIN. 
      SUBROUTINE ABUFIN
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCCUTS/CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS(5)
C
      REAL          CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER MXGKIN
      PARAMETER (MXGKIN=100)
      COMMON/GCKING/KCASE,NGKINE,GKIN(5,MXGKIN),
     +                           TOFD(MXGKIN),IFLGK(MXGKIN)
      INTEGER       KCASE,NGKINE ,IFLGK,MXPHOT,NGPHOT
      REAL          GKIN,TOFD,XPHOT
C
      PARAMETER (MXPHOT=800)
      COMMON/GCKIN2/NGPHOT,XPHOT(11,MXPHOT)
C
      COMMON/GCKIN3/GPOS(3,MXGKIN)
      REAL          GPOS
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GSECTI/ AIEL(20),AIIN(20),AIFI(20),AICA(20),ALAM,K0FLAG
      INTEGER K0FLAG
      REAL AIEL,AIIN,AIFI,AICA,ALAM
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       parameter (maxs=200)
       common /aboutC/iseco,e_r(maxs),a_r(maxs)
CDE, FCONS.

      parameter (xm_proton=0.938)
              common /hvsec/ihvse(2)
c      REAL RNDMm(1)
      real *4 pmom_p(6),pmom_t(6)
      parameter (nsecm=1000)
      real *4 p_sec(6,nsecm),v_sec(7),acoo(7)
*
      NP = 0
*
      INT=0
      EKE = GEKIN
      TXI = VECT(4)
      TYI = VECT(5)
      TZI = VECT(6)
      DMOD = 1./SQRT(TXI**2+TYI**2+TZI**2)
      TXI = TXI*DMOD
      TYI = TYI*DMOD
      TZI = TZI*DMOD
      WE  = 1.
      JMA = LQ(JMATE-NMAT)
      NCOMP = ABS (Q(JMA+11))
      AMM = Q(JMA+6)
      JMIXT = LQ(JMA-5)
       POO=SQRT(EKE*(EKE+2*AMass))
c      IF (GEKIN.LE.CUTHAD.or.poo/amass.le.ddcutt)then
      IF (GEKIN.LE.CUTHAD)then
         DESTEP = DESTEP + GEKIN
         GEKIN  = 0.
         GETOT  = AMASS
         VECT(7) = 0.
         ISTOP = 1
         GO TO 110
      ENDIF
*
         pmom_p(1)=txi
         pmom_p(2)=tyi
         pmom_p(3)=tzi
         pmom_p(4)=poo
         pmom_p(5)=amass
         pmom_p(6)=getot
888      continue
c       CALL GRNDM(RNDmM,1)
c      RNDEVT=RNDmM(1)
      RNDEVT=RNDm(dd)
      IF ( RNDEVT .le. Sela/FSIG) THEN

         INT=1
c         iiel=iiel+1
c         write(*,*)'el',iiel,sela/fsig
         IF(NCOMP.LE.1) THEN
            pmom_t(5)=amm*xm_proton
            zmed=z
            CALL absiel(pmom_p,pmom_t,1)
*
* ! here for secondaries --- later
*

         ELSE
c            CALL GRNDM(RNDmM,1)
            RCONT=ELXNOR*RNDm(dd)
            DO 40  I=1,NCOMP
               IF(RCONT.LE.ABELX(I)) GO TO 50
   40       CONTINUE
            I=NCOMP
   50       CONTINUE
            pmom_t(5)=q(jmixt+i)*xm_proton
            zmed=Q(JMIXT+NCOMP+i)
            call absiel(pmom_p,pmom_t,i)
         END IF
        else IF ( RNDEVT .le. (Sela+sqel)/FSIG) THEN
         INT=2
c         iqi=iqi+1
c         write(*,*)iqi,(sela+sqel)/fsig
         IF(NCOMP.LE.1) THEN
            pmom_t(5)=amm*xm_proton
            zmed=z
            CALL absiqe(pmom_p,pmom_t,1)
*
* ! here for secondaries --- later
*

         ELSE
c            CALL GRNDM(RNDMm,1)
            RCONT=qeXNOR*RNDM(dd)
            DO 400  I=1,NCOMP
               IF(RCONT.LE.ABqeX(I)) GO TO 500
  400       CONTINUE
            I=NCOMP
  500       CONTINUE
            zmed=Q(JMIXT+NCOMP+i)
            pmom_t(5)=q(jmixt+i)*xm_proton
            call absiqe(pmom_p,pmom_t,i)
         END IF
      ELSE
c           goto 888
            int=3
CJC            call abinelset(vect,ipart)
            if(ihvse(2).eq.1)then
             ISTOP = 1
             DESTEP = DESTEP + GEKIN
             GEKIN  = 0.
             GETOT  = AMASS
             VECT(7) = 0.
            else if(ihvse(2).eq.0)then
             GO TO 110
            else
*
* here inelastic kinematics will play
*
             IF(NCOMP.LE.1) THEN
               a_t = a
               Z_t = z
               pmom_t(5)=amm*xm_proton
               zmed=z
c               write(*,*)'nc ',a_t,z_t,amm
             ELSE
c              CALL GRNDM(RNDMm,1)
              RCONT=anxnor*RNDM(dd)
c              do i=1,ncomp
c                write(*,*)i,anxnor,abinx(i),Q(JMIXT+I)
c               enddo
              DO   I=1,NCOMP
               IF(RCONT.LE.ABinx(I)) GO TO 501
              enddo
              I=NCOMP
 501          CONTINUE
              a_t=Q(JMIXT+I)
              z_t=Q(JMIXT+NCOMP+I)
              pmom_t(5)=q(jmixt+i)*xm_proton
             END IF
             a_p=amass/xm_proton
             z_p=abs(charge)
             nsec=-nsecm
             call absiin(a_p,z_p,a_t,z_t,pmom_p,nsec,p_sec,ierr)
c             write(*,*)' bamc ',a_t,z_t,nsec
              
              if(ierr.ne.0)then
                DESTEP=DESTEP+GEKIN
                write(*,*)'rqmd error ',ierr,gekin,
     +     ' gev energy released' 
                nsec=0
              endif 
               if(ngkine.ge.mxgkin)then
                write(*,*)'absiin-E-Stack overflowExiting' 
                nsec=0
               endif
             emax=0.
             do i=1,nsec
              if((p_sec(4,i)-p_sec(6,i).gt.cuthad.or.
     +            p_sec(5,i).lt.7)
     +          .and.ngkine.lt.mxgkin)then
               ngkine=ngkine+1
               do j=1,5
                gkin(j,ngkine)=p_sec(j,i)
               enddo
               TOFD(NGKINE)=0.0
               GPOS(1,NGKINE) = VECT(1)
               GPOS(2,NGKINE) = VECT(2)
               GPOS(3,NGKINE) = VECT(3)
               if(ngkine.eq.mxgkin)then
                call gsking(ngkine)
                ngkine=ngkine-1
               endif
               if(p_sec(4,i).gt.emax)then
                emax=p_sec(4,i)
                i_sec=nint(p_sec(5,i))
                do j=1,3
                 v_sec(j)=vect(j)
                 v_sec(3+j)=p_sec(j,i)
                enddo
                v_sec(7)=sqrt(v_sec(4)**2+v_sec(5)**2+v_sec(6)**2)
               endif
              else
               DESTEP = DESTEP + p_sec(4,i)-p_sec(6,i)
              endif
             enddo
             call abinelset(v_sec,i_sec)
             ISTOP = 1
             GEKIN  = 0.
             GETOT  = AMASS
             VECT(7) = 0.
            endif
      endif
   80 IF(INT.Eq.1.or.int.eq.2)then
         VECT(4)=pmom_p(1)
         VECT(5)=pmom_p(2)
         VECT(6)=pmom_p(3)
         VECT(7)=pmom_p(4)
         GETOT= (vect(7)**2+pmom_p(5)**2)**0.5
         gekin=getot-pmom_p(5)
c         if(pmom_t(4)/pmom_t(5)*xm_proton.gt.ddcutt)then
          iseco=min(iseco+1,maxs)
          e_r(iseco)=pmom_t(6)
          a_r(iseco)=pmom_t(5)
         if(pmom_t(6).gt.cuthad)then

*
*        Secondary!
*
c         write(*,*)'ABUFIN- secondary!!!',pmom_t(4),pmom_t(6)
            intt=1
            NGKINE = min(NGKINE + 1,mxgkin)
            IGEPAR = IAFPAR(zmed)
            JPA = LQ(JPART-IGEPAR)
            AGEMAS = Q(JPA+7)
            PGEANT = pmom_t(4)
            GKIN(1,NGKINE)=pmom_t(1)*PGEANT
            GKIN(2,NGKINE)=pmom_t(2)*PGEANT
            GKIN(3,NGKINE)=pmom_t(3)*PGEANT
            GKIN(4,NGKINE)=pmom_t(6)+AGEMAS
            GKIN(5,NGKINE)=IGEPAR
               TOFD(NGKINE)=0.0
               GPOS(1,NGKINE) = VECT(1)
               GPOS(2,NGKINE) = VECT(2)
               GPOS(3,NGKINE) = VECT(3)
*
       endif
      END IF
  110 CONTINUE
      ZINTHA = GARNDM(DUMMY)
      SLHADR = SLENG
      STEPHA = 1.0E10
10000 FORMAT(' **** FLUFIN: Stack overflow, ',I6,' particles lost')
10100 FORMAT(' **** FLUFIN: Stack overflow, ',I6,
     +' heavy particles lost')
  999 END
           function iafpar(zm)
           real *4 ion(54),uw(1)
           character *50 cht
           integer ipart(54)
           data ij /0/
           if(ij.eq.0)then
            ipart(1)=14
            ipart(2)=47
            do i=1,52
             ipart(i+2)=60+i
            enddo
            do i=1,54
             nwb=1
             call gfpart(ipart(i),cht,itr,aa,ion(i),tl,uw,nwb)
            enddo
               ij=1
            endif
            do i=1,54
             if(zm.le.ion(i))then
              iafpar=ipart(i)
              goto 999
             endif
            enddo
            i=54
              iafpar=ipart(i)
999           end
CDECK  ID>, GPIONS. 
      SUBROUTINE GSTRAN(itrt,xcharge,xmass)
C.
C.    *******************************************************************
C       define the stranglet 
C.
      DIMENSION UBUF(1)
      DATA UBUF/0./
*

      CALL GSPART(113,'STRA ',itrt, xmass, xcharge, 1000., UBUF,0)
      end
      SUBROUTINE GPIONS(itrt)
C.
C.    *******************************************************************
C.    *                                                                 *
C.    *  Define a subset of the 'stable' most common elements           *
C.    *    in the Nature                                                *
C.    *                                                                 *
C.    *    ==>Called by : <USER>, UGINIT                                *
C.    *       Author    B.Grosdidier (Strasbourg) *****                 *
C.    *                                                                 *
C.    *******************************************************************
C.
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
      DIMENSION UBUF(1)
      DATA UBUF/0./
c      ITCKOV=1
*
*     The Geant particle identification begin to IPART = 61
      CALL GSPART( 61,'LI6  ',itrt,   5.60305,  3., 1000., UBUF,0)
      CALL GSPART( 62,'LI7  ',itrt,   6.53536,  3., 1000., UBUF,0)
      CALL GSPART( 63,'BE7  ',itrt,   6.53622,  4., 1000., UBUF,0)
      CALL GSPART( 64,'BE9  ',itrt,   8.39479,  4., 1000., UBUF,0)
      CALL GSPART( 114,'BE10  ',itrt,   9.33,    4., 1000., UBUF,0)
      CALL GSPART( 65,'B10  ',itrt,   9.32699,  4., 1000., UBUF,0)
      CALL GSPART( 66,'B11  ',itrt,  10.25510,  5., 1000., UBUF,0)
      CALL GSPART( 67,'C12  ',itrt,  11.17793,  6., 1000., UBUF,0)
      CALL GSPART( 68,'N14  ',itrt,  13.04378,  7., 1000., UBUF,0)
      CALL GSPART( 69,'O16  ',itrt,  14.89917,  8., 1000., UBUF,0)
      CALL GSPART( 70,'F19  ',itrt,  17.69690,  9., 1000., UBUF,0)
      CALL GSPART( 71,'NE20 ',itrt,  18.62284, 10., 1000., UBUF,0)
      CALL GSPART( 72,'NA23 ',itrt,  21.41483, 11., 1000., UBUF,0)
      CALL GSPART( 73,'MG24 ',itrt,  22.34193, 12., 1000., UBUF,0)
      CALL GSPART( 74,'AL27 ',itrt,  25.13314, 13., 1000., UBUF,0)
      CALL GSPART( 75,'SI28 ',itrt,  26.06034, 14., 1000., UBUF,0)
      CALL GSPART( 76,'P31  ',itrt,  28.85188, 15., 1000., UBUF,0)
      CALL GSPART( 77,'S32  ',itrt,  29.78180, 16., 1000., UBUF,0)
      CALL GSPART( 78,'CL35 ',itrt,  32.57328, 17., 1000., UBUF,0)
      CALL GSPART( 79,'AR36 ',itrt,  33.50356, 18., 1000., UBUF,0)
      CALL GSPART( 80,'K39  ',itrt,  36.29447, 19., 1000., UBUF,0)
      CALL GSPART( 81,'CA40 ',itrt,  37.22492, 20., 1000., UBUF,0)
      CALL GSPART( 82,'SC45 ',itrt,  41.87617, 21., 1000., UBUF,0)
      CALL GSPART( 83,'TI48 ',itrt,  44.66324, 22., 1000., UBUF,0)
      CALL GSPART( 84,'V51  ',itrt,  47.45401, 23., 1000., UBUF,0)
      CALL GSPART( 85,'CR52 ',itrt,  48.38228, 24., 1000., UBUF,0)
      CALL GSPART( 86,'MN55 ',itrt,  51.17447, 25., 1000., UBUF,0)
      CALL GSPART( 87,'FE56 ',itrt,  52.10307, 26., 1000., UBUF,0)
      CALL GSPART( 88,'CO59 ',itrt,  54.89593, 27., 1000., UBUF,0)
      CALL GSPART( 89,'NI58 ',itrt,  53.96644, 28., 1000., UBUF,0)
      CALL GSPART( 90,'CU63 ',itrt,  58.61856, 29., 1000., UBUF,0)
      CALL GSPART( 91,'ZN64 ',itrt,  59.54963, 30., 1000., UBUF,0)
      CALL GSPART( 92,'GE74 ',itrt,  68.85715, 32., 1000., UBUF,0)
      CALL GSPART( 93,'SE80 ',itrt,  74.44178, 34., 1000., UBUF,0)
      CALL GSPART( 94,'KR84 ',itrt,  78.16309, 36., 1000., UBUF,0)
      CALL GSPART( 95,'SR88 ',itrt,  81.88358, 38., 1000., UBUF,0)
      CALL GSPART( 96,'ZR90 ',itrt,  83.74571, 40., 1000., UBUF,0)
      CALL GSPART( 97,'MO98 ',itrt,  91.19832, 42., 1000., UBUF,0)
      CALL GSPART( 98,'PD106',itrt,  98.64997, 46., 1000., UBUF,0)
      CALL GSPART( 99,'CD114',itrt, 106.10997, 48., 1000., UBUF,0)
      CALL GSPART(100,'SN120',itrt, 111.68821, 50., 1000., UBUF,0)
      CALL GSPART(101,'XE132',itrt, 122.86796, 54., 1000., UBUF,0)
      CALL GSPART(102,'BA138',itrt, 128.45793, 56., 1000., UBUF,0)
      CALL GSPART(103,'CE140',itrt, 130.32111, 58., 1000., UBUF,0)
      CALL GSPART(104,'SM152',itrt, 141.51236, 62., 1000., UBUF,0)
      CALL GSPART(105,'DY164',itrt, 152.69909, 66., 1000., UBUF,0)
      CALL GSPART(106,'YB174',itrt, 162.02245, 70., 1000., UBUF,0)
      CALL GSPART(107,'W184 ',itrt, 171.34924, 74., 1000., UBUF,0)
      CALL GSPART(108,'PT194',itrt, 180.67513, 78., 1000., UBUF,0)
      CALL GSPART(109,'AU197',itrt, 183.47324, 79., 1000., UBUF,0)
      CALL GSPART(110,'HG202',itrt, 188.13451, 80., 1000., UBUF,0)
      CALL GSPART(111,'PB208',itrt, 193.72907, 82., 1000., UBUF,0)
      CALL GSPART(112,'U238 ',itrt, 221.74295, 92., 1000., UBUF,0)
      CALL GSPART(45,'DEUTERONC   ',itrt,1.875613,+1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(46,'TRITONC     ',itrt,2.80925 ,+1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(47,'ALPHAC      ',itrt,3.727417,+2.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(49,'HE3C        ',itrt,2.80923 ,+2.,1.000000E+15,
     +UBUF,0)
      CALL GSPART( 51,'XRAY$      ',1,0.      , 0.,1.E+15,UBUF,0)

      CALL GAPIONS(ITRT)
*
      END
      SUBROUTINE GAPIONS(ITRT)
C.
C.    *******************************************************************
C.    *                                                                 *
C.    *  Define a subset of the 'stable' most common elements           *
C.    *    in the Nature                                                *
C.    *                                                                 *
C.    *    ==>Called by : <USER>, UGINIT                                *
C.    *       Author    B.Grosdidier (Strasbourg) *****                 *
C.    *                                                                 *
C.    *******************************************************************
C.
      DIMENSION UBUF(1)
      DATA UBUF/0./
*
*     The Geant particle identification begin to IPART = 61
      CALL GSPART( 161,'ALI6  ',ITRT,   5.60305,  -3., 1000., UBUF,0)
      CALL GSPART( 162,'ALI7  ',ITRT,   6.53536,  -3., 1000., UBUF,0)
      CALL GSPART( 163,'ABE7  ',ITRT,   6.53622,  -4., 1000., UBUF,0)
      CALL GSPART( 164,'ABE9  ',ITRT,   8.39479,  -4., 1000., UBUF,0)
      CALL GSPART( 165,'AB10  ',ITRT,   9.32699,  -5., 1000., UBUF,0)
      CALL GSPART( 166,'AB11  ',ITRT,  10.25510,  -5., 1000., UBUF,0)
      CALL GSPART( 167,'AC12  ',ITRT,  11.17793,  -6., 1000., UBUF,0)
      CALL GSPART( 168,'AN14  ',ITRT,  13.04378,  -7., 1000., UBUF,0)
      CALL GSPART( 169,'AO16  ',ITRT,  14.89917,  -8., 1000., UBUF,0)
      CALL GSPART( 170,'AF19  ',ITRT,  17.69690,  -9., 1000., UBUF,0)
      CALL GSPART( 171,'ANE20 ',ITRT,  18.62284, -10., 1000., UBUF,0)
      CALL GSPART( 172,'ANA23 ',ITRT,  21.41483, -11., 1000., UBUF,0)
      CALL GSPART( 173,'AMG24 ',ITRT,  22.34193, -12., 1000., UBUF,0)
      CALL GSPART( 174,'AAL27 ',ITRT,  25.13314, -13., 1000., UBUF,0)
      CALL GSPART( 175,'ASI28 ',ITRT,  26.06034, -14., 1000., UBUF,0)
      CALL GSPART( 176,'AP31  ',ITRT,  28.85188, -15., 1000., UBUF,0)
      CALL GSPART( 177,'AS32  ',ITRT,  29.78180, -16., 1000., UBUF,0)
      CALL GSPART( 178,'ACL35 ',ITRT,  32.57328, -17., 1000., UBUF,0)
      CALL GSPART( 179,'AAR36 ',ITRT,  33.50356, -18., 1000., UBUF,0)
      CALL GSPART( 180,'AK39  ',ITRT,  36.29447, -19., 1000., UBUF,0)
      CALL GSPART( 181,'ACA40 ',ITRT,  37.22492, -20., 1000., UBUF,0)
      CALL GSPART( 182,'ASC45 ',ITRT,  41.87617, -21., 1000., UBUF,0)
      CALL GSPART( 183,'ATI48 ',ITRT,  44.66324, -22., 1000., UBUF,0)
      CALL GSPART( 184,'AV51  ',ITRT,  47.45401, -23., 1000., UBUF,0)
      CALL GSPART( 185,'ACR52 ',ITRT,  48.38228, -24., 1000., UBUF,0)
      CALL GSPART( 186,'AMN55 ',ITRT,  51.17447, -25., 1000., UBUF,0)
      CALL GSPART( 187,'AFE56 ',ITRT,  52.10307, -26., 1000., UBUF,0)
      CALL GSPART( 188,'ACO59 ',ITRT,  54.89593, -27., 1000., UBUF,0)
      CALL GSPART( 189,'ANI58 ',ITRT,  53.96644, -28., 1000., UBUF,0)
      CALL GSPART( 190,'ACU63 ',ITRT,  58.61856, -29., 1000., UBUF,0)
      CALL GSPART( 191,'AZN64 ',ITRT,  59.54963, -30., 1000., UBUF,0)
      CALL GSPART( 192,'AGE74 ',ITRT,  68.85715, -32., 1000., UBUF,0)
      CALL GSPART( 193,'ASE80 ',ITRT,  74.44178, -34., 1000., UBUF,0)
      CALL GSPART( 194,'AKR84 ',ITRT,  78.16309, -36., 1000., UBUF,0)
      CALL GSPART( 195,'ASR88 ',ITRT,  81.88358, -38., 1000., UBUF,0)
      CALL GSPART( 196,'AZR90 ',ITRT,  83.74571, -40., 1000., UBUF,0)
      CALL GSPART( 197,'AMO98 ',ITRT,  91.19832, -42., 1000., UBUF,0)
      CALL GSPART( 198,'APD106',ITRT,  98.64997, -46., 1000., UBUF,0)
      CALL GSPART( 199,'ACD114',ITRT, 106.10997, -48., 1000., UBUF,0)
      CALL GSPART(200,'ASN120',ITRT, 111.68821, -50., 1000., UBUF,0)
      CALL GSPART(201,'AXE132',ITRT, 122.86796, -54., 1000., UBUF,0)
      CALL GSPART(202,'ABA138',ITRT, 128.45793, -56., 1000., UBUF,0)
      CALL GSPART(203,'ACE140',ITRT, 130.32111, -58., 1000., UBUF,0)
      CALL GSPART(204,'ASM152',ITRT, 141.51236, -62., 1000., UBUF,0)
      CALL GSPART(205,'ADY164',ITRT, 152.69909, -66., 1000., UBUF,0)
      CALL GSPART(206,'AYB174',ITRT, 162.02245, -70., 1000., UBUF,0)
      CALL GSPART(207,'AW184 ',ITRT, 171.34924, -74., 1000., UBUF,0)
      CALL GSPART(208,'APT194',ITRT, 180.67513, -78., 1000., UBUF,0)
      CALL GSPART(209,'AAU197',ITRT, 183.47324, -79., 1000., UBUF,0)
      CALL GSPART(210,'AHG202',ITRT, 188.13451, -80., 1000., UBUF,0)
      CALL GSPART(211,'APB208',ITRT, 193.72907, -82., 1000., UBUF,0)
      CALL GSPART(212,'AU238 ',ITRT, 221.74295, -92., 1000., UBUF,0)
      CALL GSPART(145,'ADEUTERONC   ',itrt,1.875613,-1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(146,'ATRITONC     ',itrt,2.80925 ,-1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(147,'AALPHAC      ',itrt,3.727417,-2.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(149,'AHE3C        ',itrt,2.80923 ,-2.,1.000000E+15,
     +UBUF,0)
*
      END

CDECK  ID>, GUPHAD. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE GUPHAD
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
CDE, FCONS.
CDE, XCSIZE.
CDE, XCMATM.
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
C.
C.    ******************************************************************
C.    *                                                                *
C.    *                                                                *
C.    *       GEANT3 user routine called at each step                  *
C.    *       to evaluate the next hadronic interaction point          *
C.    *                                                                *
C.    *                (TATINA) uses R.Barlow cross-sections tables    *
C.    *                (GHEISHA)uses the GHEISHA cross-sections        *
C.    *                                                                *
C.    ******************************************************************
C.
C.
C.    ------------------------------------------------------------------
C.
              common /hvsec/ihvse(2)
      if(ipart.ge.32)then
          if(ihvse(1).eq.1.or.ihvse(1).eq.2)call abdist
      else
        if(ihadr.ge.3)then
          CALL GPGHEI
        else
         call fldist
        endif
      endif
C
      END
CDECK  ID>, GUHADR. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE GUHADR
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
CDE, FCONS.
C.
C.    ******************************************************************
C.    *                                                                *
C.    *                                                                *
C.    *       GEANT3 user routine called when a hadronic process       *
C.    *       has been selected in the current step.                   *
C.    ******************************************************************
C.
C.
C.    ------------------------------------------------------------------
C.
              common /hvsec/ihvse(2)
      if(ipart.ge.32)then
       if(ihvse(1).eq.1.or.ihvse(1).eq.2)call abufin
      else
        if(ihadr.ge.3)then
          CALL GHEISH
        else
          call flufin
        endif
      endif
      RETURN
      END
CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :

CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :


CDECK  ID>, DUMMY.  
      SUBROUTINE QNEXT
      END
      subroutine npq
      common /quest/iquest(100)
      iquest(10)=64000
      end

      subroutine abinelset(acoo,ipart)
         implicit none
         real acoo(7),coo(7)
         common /abinelseti/iset,coo
         integer iset,i,ipart
         data iset/0/
         if(iset.ne.0)then
c           write(*,*)'abinelsetlogicerroralreadyset'
         else
         do i=1,7
          coo(i)=acoo(i)
         enddo
CJC         endif
         iset=ipart
         endif
c         iset=iset-1
       end
       subroutine abinelget(isett,acoo)
         implicit none
         integer iset,isett,i
         real coo(7),acoo(7)
         common /abinelseti/iset,coo
         isett=iset
         do i=1,7
          acoo(i)=coo(i)
         enddo
       end         
       subroutine abinelclear
         implicit none
         integer iset,isett,i
         real coo(7),acoo(7)
         common /abinelseti/iset,coo
         do i=1,7
          coo(i)=0
         enddo
         iset=0
       end         
      SUBROUTINE GPHYSI
C.
C.    ******************************************************************
C.    *                                                                *
C     *       Initialise material constants for all the physics        *
C.    *       mechanisms used by GEANT                                 *
C.    *                                                                *
C.    *    ==>Called by : <USER>, UGINIT                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcphys.inc"
#include "geant321/gccuts.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcjloc.inc"
#include "geant321/gclist.inc"
#include "geant321/gcmulo.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcmate.inc"
#include "geant321/gcnum.inc"
#include "geant321/gconsp.inc"
#include "geant321/gctime.inc"
#include "geant321/gctrak.inc"
#include "geant321/gcunit.inc"
      DIMENSION CUTS(10),UCUT(10),MECA(5,13)
      EQUIVALENCE (CUTS(1),CUTGAM),(MECA(1,1),IPAIR)
      CHARACTER*4 DNAME,KCUT(10)
      CHARACTER*20 CHTITL
      LOGICAL NUCRIN
C.
C.    ------------------------------------------------------------------
C.
C              Write RUN parameters, version numbers and CUTS
C
      WRITE(CHMAIL,10000)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10200)GVERSN,IGDATE,IGTIME
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10300)IDRUN
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10400)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10500)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10600)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
C
C              Get the version number of the original INIT structure
C
      OLDGVE=BIG
*
*        Set NUMOLD to 0 to force recalculation of
*        pointers in the tracking routines
      NUMOLD=0
      IF(JRUNG.NE.0)THEN
         OLDGVE =  Q(JRUNG+21)
         IQ(JRUNG+11)=IGDATE
         IQ(JRUNG+12)=IGTIME
         Q(JRUNG+21)=GVERSN
         Q(JRUNG+22)=ZVERSN
C
         DNAME='INIT'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+11),IQ(JRUNG+12), Q(JRUNG+
     +   21), Q(JRUNG+22)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='KINE'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+13),IQ(JRUNG+14), Q(JRUNG+
     +   23), Q(JRUNG+24)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='HITS'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+15),IQ(JRUNG+16), Q(JRUNG+
     +   25), Q(JRUNG+26)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='DIGI'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+17),IQ(JRUNG+18), Q(JRUNG+
     +   27), Q(JRUNG+28)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         IF(NRNDM(1).EQ.0.AND.NRNDM(2).EQ.0) THEN
*
*             The random number sequence has not been explicitely
*             initialised via a data card. See whether we can initialise
*             it with the words 19/20 of the JRUNG data structure.
            IF(IQ(JRUNG+19).NE.0.OR.IQ(JRUNG+20).NE.0) THEN
               NRNDM(1) = IQ(JRUNG+19)
               NRNDM(2) = IQ(JRUNG+20)
               CALL GRNDMQ(NRNDM(1), NRNDM(2), 0, 'S')
            ENDIF
         ENDIF
         CALL GRNDMQ(IQ(JRUNG+19), IQ(JRUNG+20), 0, 'G')
         WRITE(CHMAIL,10900) IQ(JRUNG+19), IQ(JRUNG+20)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,11000)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
      ENDIF
C
C             Create energy loss and cross-section banks
C
      IF(NEKBIN.LE.0.OR.NEKBIN.GT.199)NEKBIN=100
      IF(EKMIN.GE.EKMAX.OR.EKMIN.LE.0.)THEN
         EKMIN=1.E-6
         EKMAX=1.E+4
      ENDIF
      IF(NEKBIN.EQ.90.AND.EKMIN.EQ.1.E-5.AND.EKMAX.EQ.1.E4)THEN
         EKMIN=1.E-6
         EKMAX=1.E+4
         NEKBIN=100
      ENDIF
      NEK1=NEKBIN+1
      EKINV=1./LOG10(EKMAX/EKMIN)
      EKBIN(1)=LOG10(EKMIN)
      ELOW(1)=EKMIN
      GEKA=NEKBIN*EKINV
      GEKB=1.-GEKA*EKBIN(1)
      DO 10 I=2,NEK1
         EL=EKBIN(1)+(I-1)/GEKA
         EKBIN(I)=EL
         ELOW(I)=10.**EL
   10 CONTINUE
      ILOW=0
      IF(NMATE.LE.0)GO TO 999
      IF(JMATE.LE.0)GO TO 999
      IF(JTMED.LE.0)GO TO 999
C
      IF(IQ(JTMED-1).LT.40) THEN
         NPUSH=40-IQ(JTMED-1)
         CALL MZPUSH(IXCONS,JTMED,0,NPUSH,'I')
      END IF
      Q(JTMED+31)=ILABS
      Q(JTMED+32)=ISYNC
      Q(JTMED+33)=ISTRA
*
*             If Landau fluctuations activated, cancel delta rays
      KLOS=Q(JTMED+21)
      IF (KLOS .EQ. 0) Q(JTMED+15) = 0.
      IF (KLOS .EQ. 2) THEN
         Q(JTMED+ 8)=9999.
         Q(JTMED+ 9)=9999.
         Q(JTMED+15)=0.
      ENDIF
*
* If Cerenkov generation is on, activate Light absorbtion unless
* explicitely switched off by the user
*
      KLABS=Q(JTMED+31)
      IF(ITCKOV.NE.0) THEN
         IF(KLABS.EQ.-1) THEN
            Q(JTMED+31)=1
         ENDIF
      ENDIF
      Q(JTMED+31)=MAX(Q(JTMED+31),0.)
*
*             If BCUTE,BCUTM,DCUTE,DCUTM,PPCUTM not initialized (=BIG)
*             Set them to CUTGAM,CUTGAM,CUTELE,CUTELE respectively
*
      IF(Q(JTMED+ 6).GT.0.9*BIG)Q(JTMED+ 6)=Q(JTMED+1)
      IF(Q(JTMED+ 7).GT.0.9*BIG)Q(JTMED+ 7)=Q(JTMED+1)
      IF(Q(JTMED+ 8).GT.0.9*BIG)Q(JTMED+ 8)=Q(JTMED+2)
      IF(Q(JTMED+ 9).GT.0.9*BIG)Q(JTMED+ 9)=Q(JTMED+2)
      IF(Q(JTMED+10).GT.0.9*BIG)Q(JTMED+10)=0.010
      IF(Q(JTMED+10).LT.4.*EMASS)Q(JTMED+10)=4.*EMASS
C
      DO 20 K=1,10
   20 CALL GEVKEV(Q(JTMED+K),UCUT(K),KCUT(K))
      WRITE(CHMAIL,10800)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11200)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11300) (UCUT(K),KCUT(K),K=1,3)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11400) (UCUT(K),KCUT(K),K=4,5)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11500) (UCUT(K),KCUT(K),K=6,7)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11600) (UCUT(K),KCUT(K),K=8,10)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11700) (Q(JTMED+K),K=11,13)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11800) (Q(JTMED+K),K=14,16)
      CALL GMAIL(0,0)
      IF(Q(JTMED+18).EQ.3.) THEN
         NUCRIN = .FALSE.
         Q(JTMED+18)=3.
      ELSE
         NUCRIN = .FALSE.
      ENDIF
      WRITE(CHMAIL,11900) (Q(JTMED+K),K=17,19)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12000) (Q(JTMED+K),K=20,22)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12100) Q(JTMED+23),Q(JTMED+31),Q(JTMED+32)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12110) Q(JTMED+33)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      IF(NUCRIN) THEN
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,12800)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,12900)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
      ENDIF
*
* ***    Here we clean up the old cross section tables if any
      DO 40  IMA=1,NMATE
         JMA=LQ(JMATE-IMA)
         IF(JMA.NE.0) THEN
            DO 30  J=1,20
               IF(LQ(JMA-J).NE.0.AND.J.NE.5) THEN
                  CALL MZDROP(IXCONS,LQ(JMA-J),'L')
               ENDIF
   30       CONTINUE
         ENDIF
   40 CONTINUE
*
* *** Call initialisation of the phtotelectric effect constants
      CALL GPHINI
      DO 180 ITM=1,NTMED
         JTM=LQ(JTMED-ITM)
         IF(JTM.LE.0) GO TO 180
         NL=10-IQ(JTM-2)
         IF(NL.GT.0)THEN
            CALL MZPUSH(IXCONS,JTM,NL,0,'I')
            JTM=LQ(JTMED-ITM)
         ENDIF
*         IF(IQ(JTM-1).LT.40) THEN
*           NPUSH=40-IQ(JTM-1)
*           CALL MZPUSH(IXCONS,JTM,0,NPUSH,'I')
*           JTM=LQ(JTMED-ITM)
*         ENDIF
         ISVOL = Q(JTM + 7)
         IFIELD = Q(JTM + 8)
         FIELDM = Q(JTM + 9)
         TMAXFD = Q(JTM + 10)
         STEMAX = Q(JTM + 11)
         DEEMAX = Q(JTM + 12)
         EPSIL = Q(JTM + 13)
         STMIN = Q(JTM + 14)
         IF (TMAXFD.LE.0..OR. (IGAUTO.NE.0.AND.TMAXFD.GT.20.)) THEN
            TMAXFD=20.
            Q(JTM+10) = TMAXFD
         ENDIF
         NMAT = Q(JTM+6)
         JMA = LQ(JMATE-NMAT)
         IF(JMA.LE.0)THEN
            WRITE(CHMAIL,12200)NMAT,ITM
            CALL GMAIL(1,1)
            GO TO 180
         ENDIF
C
C=====>       Get material parameters
C
         A=Q(JMA+6)
         Z=Q(JMA+7)
         DENS=Q(JMA+8)
         RADL=Q(JMA+9)
         IF (Z.LT.1.) THEN
            DEEMAX=0.
            STMIN =0.
            JTP=LQ(JTM)
            IF(JTP.EQ.0) THEN
               CALL MZBOOK(IXCONS,JTP,JTM,0,'TCUT',0,0,40,3,0)
               IQ(JTP-5)=ITM
               DO 50 I=1,23
                  Q(JTP+I)=Q(JTMED+I)
   50          CONTINUE
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ELSEIF(IQ(JTP-1).LT.40) THEN
               NPUSH=40-IQ(JTP-1)
               CALL MZPUSH(IXCONS,JTP,0,NPUSH,'I')
               JTP=LQ(JTM)
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ENDIF
C
C=====>     decay and synch. rad. in vacuum
C
            DO 60 I=11,23
               Q(JTP+I)=0.
   60       CONTINUE
            Q(JTP+20) = Q(JTMED+20)
            Q(JTP+31) = 0.
            Q(JTP+32) = Q(JTMED+32)
            Q(JTP+33) =0.
         ENDIF
C
C=====>       Get tracking medium parameters
C
         JTP=JTMED
         IF(LQ(JTM).NE.0)JTP=LQ(JTM)
         IF(JTP.NE.JTMED)THEN
            IF(IQ(JTP-1).LT.40) THEN
               NPUSH=40-IQ(JTP-1)
               CALL MZPUSH(IXCONS,JTP,0,NPUSH,'I')
               JTP=LQ(JTM)
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ENDIF
            KLOS=Q(JTP+21)
            IF (KLOS .EQ. 2) THEN
               Q(JTP+ 8)=9999.
               Q(JTP+ 9)=9999.
               Q(JTP+15)=0.
            ENDIF
*
* If Cerenkov generation is on, activate Light absorbtion unless
* explicitely switched off by the user
*
            KLABS=Q(JTP+31)
            IF(ITCKOV.NE.0) THEN
               IF(KLABS.EQ.-1) THEN
                  Q(JTP+31)=1
               ENDIF
            ENDIF
            Q(JTP+31)=MAX(Q(JTP+31),0.)
            IF(Q(JTP+ 6).GT.0.9*BIG)Q(JTP+ 6)=Q(JTP+1)
            IF(Q(JTP+ 7).GT.0.9*BIG)Q(JTP+ 7)=Q(JTP+1)
            IF(Q(JTP+ 8).GT.0.9*BIG)Q(JTP+ 8)=Q(JTP+2)
            IF(Q(JTP+ 9).GT.0.9*BIG)Q(JTP+ 9)=Q(JTP+2)
            IF(Q(JTP+10).GT.0.9*BIG)Q(JTP+10)=0.010
            IF(Q(JTP+10).LT.4.*EMASS)Q(JTP+10)=4.*EMASS
*
            CALL UHTOC(IQ(JTM+1),4,CHTITL,20)
            LAST=LNBLNK(CHTITL)
            IF(LAST.GT.0) THEN
               IF(CHTITL(LAST:LAST).EQ.'$') LAST=LAST-1
               IF(LAST.LT.20) CHTITL(LAST+1:20)=' '
            ENDIF
*
            DO 70 K=1,10
   70       CALL GEVKEV(Q(JTP+K),UCUT(K),KCUT(K))
            WRITE(CHMAIL,10100)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12300)ITM,CHTITL
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12400)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11300) (UCUT(K),KCUT(K),K=1,3)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11400) (UCUT(K),KCUT(K),K=4,5)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11500) (UCUT(K),KCUT(K),K=6,7)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11600) (UCUT(K),KCUT(K),K=8,10)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11700) (Q(JTP+K),K=11,13)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11800) (Q(JTP+K),K=14,16)
            CALL GMAIL(0,0)
            IF(Q(JTP+18).EQ.3.) THEN
               NUCRIN = .FALSE.
               Q(JTP+18)=3.
            ELSE
               NUCRIN = .FALSE.
            ENDIF
            WRITE(CHMAIL,11900) (Q(JTP+K),K=17,19)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12000) (Q(JTP+K),K=20,22)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12100) Q(JTP+23),Q(JTP+31),Q(JTP+32)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12110) Q(JTP+33)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,10100)
            CALL GMAIL(0,0)
            IF(NUCRIN) THEN
               WRITE(CHMAIL,10100)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,12800)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,12900)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,10100)
               CALL GMAIL(0,0)
            ENDIF
         ENDIF
C
         DO 80 I=1,10
            CUTS(I)=Q(JTP+I)
   80    CONTINUE
         DO 90 I=1,13
            MECA(1,I)=Q(JTP+10+I)
   90    CONTINUE
         ILABS=Q(JTP+10+21)
         ISYNC=Q(JTP+10+22)
         ISTRA=Q(JTP+10+23)
C
         IF(ILOW.EQ.0)THEN
            DO 100 I=1,10
               IF(Q(JTP+I).LT.EKMIN)THEN
                  WRITE(CHMAIL,12500)
                  CALL GMAIL(1,1)
                  ILOW=1
               ENDIF
  100       CONTINUE
         ENDIF
C
C             Check consistency of different tracking media
C             referencing the same material
C
         DO 120 ITM2=ITM+1,NTMED
            JTM2=LQ(JTMED-ITM2)
            IF(JTM2.NE.0)THEN
               NMAT2=Q(JTM2+6)
               IF(NMAT2.EQ.NMAT)THEN
                  JTP2=JTMED
                  IF(LQ(JTM2).NE.0)JTP2=LQ(JTM2)
                  IF(JTP.NE.JTP2)THEN
                     IF(JTP2.NE.JTMED)THEN
                        KLOS=Q(JTP2+21)
                        IF (KLOS .EQ. 2) THEN
                           Q(JTP2+ 8)=9999.
                           Q(JTP2+ 9)=9999.
                           Q(JTP2+15)=0.
                        ENDIF
                        IF(Q(JTP2+ 6).GT.0.9*BIG)Q(JTP2+ 6)=Q(JTP2+1)
                        IF(Q(JTP2+ 7).GT.0.9*BIG)Q(JTP2+ 7)=Q(JTP2+1)
                        IF(Q(JTP2+ 8).GT.0.9*BIG)Q(JTP2+ 8)=Q(JTP2+2)
                        IF(Q(JTP2+ 9).GT.0.9*BIG)Q(JTP2+ 9)=Q(JTP2+2)
                        IF(Q(JTP2+10).GT.0.9*BIG)Q(JTP2+10)=0.010
                        IF(Q(JTP2+10).LT.4.*EMASS)Q(JTP2+10)=4.*EMASS
                     ENDIF
                     DO 110 I=6,10
                        IF(Q(JTP+I).NE.Q(JTP2+I))THEN
                           WRITE(CHMAIL,12600)NMAT
                           CALL GMAIL(1,0)
                           WRITE(CHMAIL,12700)ITM,ITM2
                           CALL GMAIL(0,1)
                           GO TO 120
                        ENDIF
  110                CONTINUE
                  ENDIF
               ENDIF
            ENDIF
  120    CONTINUE
         IF (DEEMAX.LT.0.) THEN
            IF(ISVOL.EQ.0)THEN
               DEEMAX=0.25
               IF(RADL.GT.2.)DEEMAX=0.25-0.2/SQRT(RADL)
            ELSE
               DEEMAX = 0.2/SQRT(RADL)
            ENDIF
         ENDIF
         IF(OLDGVE.LT.3.15.OR.STEMAX.LE.0.) THEN
*
*       Before version 3.15 there was no STEMAX, so we put it to BIG
            STEMAX=BIG
         ENDIF
         Q(JTM+11) = STEMAX
         Q(JTM+12) = DEEMAX
C
*
*       It can happen that several tracking media refer to the
*       same material. In this case we do not fill the cross section
*       tables more than once. But we still fill the banks of the
*       tracking medium.
         IF(LQ(JMA-1).NE.0) GOTO 160
         NPUSH=20-IQ(JMA-2)
         IF(NPUSH.GT.0)THEN
            CALL MZPUSH(IXCONS,JMA,NPUSH,0,'I')
            JTM=LQ(JTMED-ITM)
            JMA=LQ(JMATE-NMAT)
         ENDIF
*
*     Energy loss and cross-section tables
         IF(ISTRA.EQ.0) THEN
         CALL MZBOOK(IXCONS,LBANK,JMA, -1,'MAEL',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA, -2,'MAMU',0,0, NEK1,3,0)
         ELSE
         CALL MZBOOK(IXCONS,LBANK,JMA, -1,'MAEL',0,0,3*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA, -2,'MAMU',0,0,2*NEK1,3,0)
         ENDIF
         CALL MZBOOK(IXCONS,LBANK,JMA, -3,'MAAL',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JPROB,JMA, -4,'MAPR',0,0, 40,3,0)
         CALL MZBOOK(IXCONS,JPHOT,JMA, -6,'MAPH',2,2, NEK1,3,0)
         CALL MZBOOK(IXCONS,JANNI,JMA, -7,'MAAN',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JCOMP,JMA, -8,'MACO',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JBREM,JMA, -9,'MABR',0,0,3*NEK1,3,0)
         CALL MZBOOK(IXCONS,JPAIR,JMA,-10,'MAPA',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,JDRAY,JMA,-11,'MADR',0,0,3*NEK1,3,0)
*
* *** Special case for heavy materials, photo-fission
         IF(A.GE.230..AND.A.LE.240..AND.IPFIS.NE.0)THEN
            CALL MZBOOK(IXCONS,JPFIS,JMA,-12,'MAPF',0,0,2*NEK1,3,0)
         ENDIF
*
* *** Rayleigh effect
         CALL MZBOOK(IXCONS,JRAYL,JMA,-13,'MARA',0,0,2*NEK1,3,0)
*
* *** Muon nuclear interactions
         IF(IMUNU.EQ.0)THEN
            JMUNU=0
         ELSE
            CALL MZBOOK(IXCONS,JMUNU,JMA,-14,'MAMN',0,0,NEK1,3,0)
         ENDIF
*
* *** stopping range
         CALL MZBOOK(IXCONS,LBANK,JMA,-15,'MASE',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA,-16,'MASM',0,0,2*NEK1,3,0)
*
* *** Special for photeffect
         CALL GPHXSI
*
* *** coefficients for energy loss
         CALL MZBOOK(IXCONS,LBANK,JMA,-17,'MACE',0,0,6*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA,-18,'MACM',0,0,6*NEK1,3,0)
*
* *** auxiliary tables for integration of dE/dx
         CALL GWORK(NEKBIN*4)
*
         DO 130 JWORK=1, NEKBIN*4
            WS(JWORK) = 0.
  130    CONTINUE
*
* *** Straggling for thin layers, if in effect
         IF(ISTRA.GT.0) THEN
            CALL MZBOOK(IXCONS,JTSTRA,JMA,-19,'MAST',2,2,1,3,0)
#if defined(CERNLIB_ASHO)
            IF(ISTRA.EQ.2) THEN
               CALL MZBOOK(IXCONS,JTASHO,JMA,-20,'MASH',0,0,106,3,0)
            ENDIF
#endif
         ENDIF
*
         DO 140 J=1,20
            JB=LQ(JMA-J)
            IF(JB.NE.0)IQ(JB-5)=NMAT
  140    CONTINUE
C
         JPROB=LQ(JMA-4)
         JMIXT=LQ(JMA-5)
         JPFIS=LQ(JMA-12)
*
* *** Fill above tables (energy losses,cross-sections,stopping ranges)
*
         CALL GPROBI
C
         DO 150 IEKBIN=1,NEK1
C
            CALL GDRELA
            CALL GBRELA
            CALL GPRELA
C
            CALL GPHOTI
            CALL GRAYLI
            CALL GANNII
            CALL GCOMPI
            CALL GBRSGA
            CALL GPRSGA
            CALL GDRSGA
            CALL GMUNUI
            CALL GPFISI
  150    CONTINUE
*
*           Stopping ranges
*
         CALL GRANGI
*
*           Energy loss coefficients
*
         CALL GCOEFF
* *** The table for the energy loss in thin gas layers if the tracking
*     media is defined as such
*
         IF(ISTRA.GT.0) THEN
            CALL GSTINI
#if defined(CERNLIB_ASHO)
            IF (ISTRA.EQ.2) THEN
               CALL GIASHO
            ENDIF
#endif
         ENDIF
*
* *** Multiple scattering,energy-loss and mag.field steps
  160    DO 170 J=1,2
            IF(LQ(JTM-J).NE.0) THEN
               CALL MZDROP(IXCONS,LQ(JTM-J),'L')
            ENDIF
  170    CONTINUE
         CALL MZBOOK(IXCONS,LBANK,JTM, -1,'MUEL',0,0,NEK1+2,3,0)
         IQ(LBANK-5)=ITM
         CALL MZBOOK(IXCONS,LBANK,JTM, -2,'MUMU',0,0,NEK1+2,3,0)
         IQ(LBANK-5)=ITM
         CALL GMULOF
C
  180 CONTINUE
*
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10400)
      CALL GMAIL(0,2)
C
10000 FORMAT(
     +'1************************************************************')
10100 FORMAT(
     +' *                                                          *')
10200 FORMAT(
     +' *    G E A N T  Version',F7.4,'      DATE/TIME',I7.6,'/',
     +   I4.4,2X,'*')
10300 FORMAT(
     +' *                      R U N  ',I5,10X,'              *')
10400 FORMAT(
     +' ************************************************************')
10500 FORMAT(
     +' *      Data structure   Date   Time    GVERSN    ZVERSN    *')
10600 FORMAT(
     +' *      --------------   ----   ----    ------    ------    *')
10700 FORMAT(' *',11X,A,6X,I7.6,2X,I4.4,3X,F7.4,2X,F7.2,5X,'*')
10800 FORMAT(
     +' *----------------------------------------------------------*')
10900 FORMAT(' *     Random number seeds: ',3X,I10,3X,I10,6X,'*')
11000 FORMAT(
     +' *     --------------------                                 *')
11100 FORMAT(
     +' *              Standard TPAR for this run are              *')
11200 FORMAT(
     +' *              ------------------------------              *')
11300 FORMAT(
     +' *  CUTGAM=',F6.2,A4,'  CUTELE=',F6.2,A4,'  CUTNEU=',F6.2,A4,1X,
     + '*')
11400 FORMAT(
     +' *  CUTHAD=',F6.2,A4,'  CUTMUO=',F6.2,A4,20X,'*')
11500 FORMAT(
     +' *  BCUTE =',F6.2,A4,'  BCUTM =',F6.2,A4,20X,'*')
11600 FORMAT(
     +' *  DCUTE =',F6.2,A4,'  DCUTM =',F6.2,A4,'  PPCUTM=',F6.2,A4,1X,
     + '*')
11700 FORMAT(
     +' *  IPAIR =',F10.0,'  ICOMP =',F10.0,'  IPHOT =',F10.0,1X,'*')
11800 FORMAT(
     +' *  IPFIS =',F10.0,'  IDRAY =',F10.0,'  IANNI =',F10.0,1X,'*')
11900 FORMAT(
     +' *  IBREM =',F10.0,'  IHADR =',F10.0,'  IMUNU =',F10.0,1X,'*')
12000 FORMAT(
     +' *  IDCAY =',F10.0,'  ILOSS =',F10.0,'  IMULS =',F10.0,1X,'*')
12100 FORMAT(
     +' *  IRAYL =',F10.0,'  ILABS =',F10.0,'  ISYNC =',F10.0,1X,'*')
12110 FORMAT(
     +' *  ISTRA =',F10.0, 39X,                                  '*')
 
12200 FORMAT(' ***** GPHYSI error, Material Nr=',I3,
     + ' referenced by Medium Nr=',I3)
12300 FORMAT(
     +' *     Special TPAR for TMED',I4,3X,A,5X,'*')
12400 FORMAT(
     +' *     -------------------------                            *')
12500 FORMAT(' ***** GPHYSI error, CUTS must be',
     + ' greater than EKMIN KeV *****')
12600 FORMAT(' ***** GPHYSI error for material nr ',I4)
12700 FORMAT(7X,'Tracking medium NR',I4,' and',I4,
     +' have different parameters')
12800 FORMAT(
     +' *  IHADR=3 not supported any more. GHEISHA will handle     *')
12900 FORMAT(
     +' *  hadronic interactions for the above tracking medium     *')
  999 END


*
*
       subroutine xdummy(io)
       end

       subroutine getscanfl(iscan)
#include "geant321/gcscan.inc"
       if(SCANFL)then
        iscan=1
       else
        iscan=0
       endif
       end

