* $Id: richrec_lip.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*// +LIP reconstruction interface
* created        13/03/2003 
* last change    03/12/2003
*
      subroutine richreclip(ievt)
* ==========================================================
*  interface for LIP reconstruction      
* ==========================================================

      implicit none  
*------------
*DECLARATIONS 
*------------
#include "../include/richrec_lipf.h"

      integer ievt

* ..  local variables

      integer i,ih
      integer nbadtrack, nupgtrack,ireflec
      integer nhit_av_r,nhit_av_d 
      real pitch,zphemi,zbrad,htest,ztrial


      integer ifirst
      data ifirst/0/ 

      real dhittrk2,dhittrkass2
      parameter(dhittrkass2=25.) ! cm 
      REAL TRICH 

      integer ipm,ipmmax,nhpm(1000),npmtot
      real pmweigth
       
      double precision thch,phgh,zmirh,dz,thcd,phgd 
      real thc_av_d,thc_av_r,dzhit(16000)

* ---------------------------------------
* INITIALIZATION
* -----------------------------------

      if(ievnumb.ne.ievt) then
         itknumb=1
      else
         itknumb=itknumb+1 
      endif 
      ievnumb=ievt 

      if(ifirst.eq.0) then 
*  matrix placing 
* (according to richid.C => DMECX e DMECY = 1.1)
       pitch=pmt_sidel+pmt_shdtk+pmt_suptk
       pitchx=pitch
       pitchy=pitch
       jump=0.5
       dx0=(pitchx-jump)/2.
       xlimv=emcxlim-0.2
       xlimh=emcxlim+0.9
       ylimh=emcylim-0.7
       ylimv=emcylim+0.4
       xcpmm=0.
       ycpmm=0.
* (mirror aux. var.)
       tgmir   =  (rbmir-rtmir)/hmir  
       vtmir(1)= 0.
       vtmir(2)= 0.
       vtmir(3)= -(rtmir/tgmir-(zpmtdet-hmir-zbmirgap))
       ZBRAD=ZPMTDET-ZBMIRGAP-HMIR-ZTMIRGAP-HPGL  
      endif

*     ---------------------
*     init event variables: 
*     ---------------------


      nbhits_used=0.
      thcrec     =0.
      betarec    =0.
      ebetarec   =999.
      likep      =999.
      chi2beta   =999.
      rprob      =999. 
      
      do i=1,1000 
         nhpm(i)=0
      enddo

      npmtot=0  
      pmweigth=0.

* ... rotation matrix detector->particle frame             

      irotflg=0
      if (irotflg.eq.0) then                                                
         call patmatr                                                       
         irotflg = 1                                                        
      endif 

* ... set radiator 

      if(radtype.eq.1) then
         chradid='AGL'
         zphemi=0.6
      elseif(radtype.eq.2) then
         chradid='NAF'
         zphemi=0.5
      elseif(radtype.eq.0) then 
         return 
      endif  

      CALL TIMED(TRICH)
 
* => RECONSTRUCTION REQUIREMENTS:

      if(nbhits.eq.0)        return

      HTEST=ZBRAD-PIMP(3) 
      if(ABS(HTEST-HRAD).GT.0.1) then
       nbadtrack=nbadtrack+1
       return
      endif  
      if(pthe.gt.90.*degrad) then
       nupgtrack= nupgtrack+1
       return 
      endif  

* =>    BETA RECONSTRUCTION

*... set photon emission vertex
      ztrial=pimp(3)+HRAD*zphemi 

      pcervtx(3) = ZTRIAL                                         
      pcervtx(2) = pimp(2) + tan(pthe)*sin(pphi)*(ZTRIAL-pimp(3)) 
      pcervtx(1) = pimp(1) + tan(pthe)*cos(pphi)*(ZTRIAL-pimp(3)) 

* ... hits associated to track (clean and check once per track)
      call vzero(iflghit,nbhitsmax) 
      nbhits_nass=nbhits
      ipmmax=0  

      do 2 i=1,nbhits
* ... hits associated to particle
        dhittrk2=(hitscoo(1,i)-pcoopmt(1))**2+     
     +           (hitscoo(2,i)-pcoopmt(2))**2
        if(dhittrk2.lt.dhittrkass2) then
         iflghit(i)=1
         nbhits_nass=nbhits_nass-1    
        endif 
* ... clusters (hitshid=10*(16*pmt+pixel)+gain)
        ipm=int(hitshid(i)/160.)
        if(ipm.le.1000) then
         nhpm(ipm)=nhpm(ipm)+1
         ipmmax=max(ipmmax,ipm)
        else 
C         print*,'IPM > 1000',ipm 
        endif
 2    continue

      if(nbhits_nass.lt.3) return 

      do i=1,ipmmax
        if(nhpm(i).gt.0) then
          npmtot=npmtot+1 
         endif 
      enddo   

* ... attempt at clusterization....

      do i=1,ipmmax
       pmweigth= nhpm(i)/(1.*nbhits_nass)
       if(nhpm(i).gt.0) then 
        if(nhpm(i).gt.3.and.pmweigth*nhpm(i).gt.1.1) then 
         do ih=1,nbhits 
          if(iflghit(ih).eq.0.and.int(hitshid(ih)/160.).eq.i) then 
            iflghit(ih)=3 
            nbhits_nass=nbhits_nass-1    
          endif
         enddo 
        endif 
       endif 
      enddo 
      if(nbhits_nass.lt.3) return 

      call richlikebeta_lip(0,ireflec)

      if (ipthetac.ne.0.and.nbushits(ipthetac).gt.0) then

       nbhits_used=nbushits(ipthetac)
       thcrec     =cangrec(ipthetac)  
       likep      =likerec(ipthetac)
       chi2beta   =chi2rec(ipthetac)    
*       ebetarec   =ireflec*1.  ! numero de hits reflectidos
*     rprob      =radtype*1.  ! tipo de radiador 1:AGL/2:NAF
      else
       betarec=0.
      endif

      return
      end

