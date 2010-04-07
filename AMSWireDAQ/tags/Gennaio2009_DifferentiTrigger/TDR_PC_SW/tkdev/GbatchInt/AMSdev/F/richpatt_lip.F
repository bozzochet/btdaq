* $Id: richpatt_lip.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*//+LIP pattern tracing routines
* ================================================================         
      subroutine patrace1(phig,cerangh,xdet,ydet,nmiref,nerr,verr)
* ================================================================         
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig   = photon phi angle                               
*                  cerangh = cerenkov angle                                 
*            OUT - xdet   = x-coord of the photon at the PMT plane         
*                  ydet   = y-coord of the photon at the PMT plane         
*                  nmiref = number of reflections on mirror                
*                  ierr   = error flag                                     
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig   = photon phi angle                               
*                  cerang = cerenkov angle                                 
*            OUT - xdet   = x-coord of the photon at the PMT plane         
*                  ydet   = y-coord of the photon at the PMT plane         
*                  nmiref = number of reflections on mirror                
*                  ierr   = error flag                                     
*
*      verr(1:20) : photon error coding
*              10  - backward photon
*              11  - out of RADIATOR but outside mirror acc.
*              12  - out of PLEXI but outside mirror acc.
*              13  - starts PLEXI outside mirror acc.(rad=PLEXI)
*              14  - out of PLEXI+AIRGAP but outside mirror acc.
*              15  - total reflection in RADIATOR-PLEXI transition 
*              16  - total reflection in PLEXI-AIR transition 
*              17  - total reflection in RADIATOR-AIR (no PLEXI)
*              18  - 
*              19  - in NON-ACTIVE detection region 
******************************************************************         
      implicit none
#include "../include/richrec_lipf.h"

      real phdir(3), phpto(3),phdirn(3)                                    
      character*3 CHIN,CHOUT
       integer iter
       integer niter
       data niter/100/
* ... passed parameters
      integer nmiref
      integer nphmax
      parameter(nphmax=10)
      integer nerr,verr(nphmax)
      real xdet,ydet,phig,cerangh
      real *8 eps
* ... local variables
      integer ierr,i,iflfl
      real *8 rpoint

* --- init                                                                 
      nerr   = 0                                                           
      call vfill(verr,nphmax,0)                                                
      nmiref = 0 
*
      xdet = -999.                                                         
      ydet = -999.                                                         
                                
* --- set particle rotation matrix if not already done
      if (irotflg.eq.0) then          
         call patmatr                 
         irotflg = 1                  
      endif                           

* --- function                                                             
* ... photon emission point                                                
      call ucopy(pcervtx,phpto,3)                                          

* ... generate photon on detector frame                                    

      call patphot(phig,cerangh,phdir)                                      

* ... test photon direction: backward?                                     
      if (phdir(3).lt.0.) then                                             
         nerr = nerr + 1                                                   
         verr(nerr) = 10                                                   
         return                                                            
      endif                                                                
* ... photon refraction                                                    

      /* test photon origin medium */

      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then 
         call patints(phpto,phdir,'RADB',ierr)  
         if (ierr.ne.0) then                                             
            nerr = nerr + 1                                              
            verr(nerr) = 11                     
            return                                                       
         endif
         /* test plexiglass support */
         if (HPGL.ne.0.0) then
            chin = chradid 
            call patrefr(chin,'PGL',phdir,ierr) 
            if (ierr.ne.0) then                  
               nerr = nerr + 1                                             
               verr(nerr) = 15                  
               return                                                      
            endif                                                          
            call patints(phpto,phdir,'PGLB',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 12                
               return                                                       
            endif                                                           
            call patrefr('PGL','AIR',phdir,ierr) 
            if (ierr.ne.0) then                  
               nerr = nerr + 1                                             
               verr(nerr) = 16               
               return                                                      
            endif
            call patints(phpto,phdir,'AGAP',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 14               
               return                                                       
            endif                                                           
         else  ! NO PLEXIGLASS 
            chin = chradid 
            call patrefr(chin,'AIR',phdir,ierr)
            if (ierr.ne.0) then                
               nerr = nerr + 1                                             
               verr(nerr) = 17                                             
               return                                                      
            endif                                                          
            call patints(phpto,phdir,'AGAP',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 14  
               return                                                       
            endif                                                           
         endif
      /* photon starting in plexiglass */
      elseif (chradid.eq.'PGL') then
         if (HPGL.eq.0.0) then
          print*,' >>> patrace1: photons starting in PGL when HPGL=0'
          return 
         endif   
         call patints(phpto,phdir,'PGLB',ierr) 
         if (ierr.ne.0) then                                             
            nerr = nerr + 1                                              
            verr(nerr) = 13                
            return                                                       
         endif                                                           
         call patrefr('PGL','AIR',phdir,ierr)
         if (ierr.ne.0) then                 
            nerr = nerr + 1                                             
            verr(nerr) = 16                                             
            return                                                      
         endif                                                          
        call patints(phpto,phdir,'AGAP',ierr)                          
        if (ierr.ne.0) then                                             
           nerr = nerr + 1                                              
           verr(nerr) = 14     
           return                                                       
        endif                                                           
      endif                      

CC    if (nerr.ne.0) return           
                                   
* ... intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)                        
*      print*,'>>>>>>>>>>>>>>>>>> richpatt,phpto',phpto 
* ... propagating photons to pmt matrix  - FLIGHT
       iter=0
      rpoint = sqrt(phpto(1)**2+phpto(2)**2)  
      eps=1.e-5                             
      do while (rpoint.gt.RBMIR+TGMIR*ZBMIRGAP+eps)
        iter=iter+1
        call patints(phpto,phdir,'MIRR',ierr) 
        if(iter.gt.niter)ierr=1
        if (ierr.ne.0) return   
        rpoint = sqrt(phpto(1)**2+phpto(2)**2)                            

        if(phpto(3).gt.(zpmtdet-zbmirgap)) then
          call patints(phpto,phdir,'PMTD',ierr)                             
        else       
          nmiref = nmiref + 1                                               
          if (NMIREF.GT.10)then
           nerr=nerr+1
           RETURN
          endif
          call patrefl(phpto,phdir) 
          call patints(phpto,phdir,'PMTD',ierr)                             
          rpoint = sqrt(phpto(1)**2+phpto(2)**2)                            
        endif 

      enddo

* ... output coordinates                                                   
      xdet = phpto(1)                                                      
      ydet = phpto(2)                                                      
                                                                           
* --- end                                                                  
      return                                                               

      end                                                                       
* ===================================================================
      subroutine patrace_full(phig,cerangh,xdet,ydet,nmiref,nerr,verr)
* ===================================================================
      implicit none
#include "../include/richrec_lipf.h"

      real phdir(3), phpto(3),phdirn(3)                                    
      character*3 CHIN,CHOUT

* ... passed parameters 
      integer nmiref   
      integer nphmax  
      parameter(nphmax=10)
      integer nerr,verr(nphmax)                                                
      real xdet,ydet,phig,cerangh

* ... local variables
      integer ierr,i,iflfl  
      real rpoint   

* --- init                                                                 
      nerr   = 0                                                           
      call vfill(verr,nphmax,0)                                                
      nmiref = 0 ! number of mirror reflections 
*
      xdet = -999.                                                         
      ydet = -999.                                                         
                                
* --- set particle rotation matrix if not already done
      if (irotflg.eq.0) then          
         call patmatr                 
         irotflg = 1                  
      endif                           

* ... photon emission point                                                

      call ucopy(pcervtx,phpto,3)                                          

* ... generate photon on detector frame                                    
      call patphot(phig,cerangh,phdir)                                      

* ... test photon direction: backward?                                     

      if (phdir(3).lt.0.) then                                             
         nerr = nerr + 1                                                   
         verr(nerr) = 10                                                   
         return                                                            
      endif                                                                

* ... photon refraction                                                    

      /* test photon origin medium */

      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then 
         call patints(phpto,phdir,'RADB',ierr) 
          if (ierr.ne.0) then                                             
            nerr = nerr + 1                                              
            verr(nerr) = 11   
            return                                                       
         endif

         /* test plexiglass support */

         if (HPGL.ne.0.0) then
            chin = chradid 
            call patrefr(chin,'PGL',phdir,ierr) 
            if (ierr.ne.0) then                 
               nerr = nerr + 1                                             
               verr(nerr) = 15                                             
               return                                                      
            endif                                                          
            call patints(phpto,phdir,'PGLB',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 12                  
               return                                                       
            endif                                                           
            call patrefr('PGL','AIR',phdir,ierr)
            if (ierr.ne.0) then                 
               nerr = nerr + 1                                             
               verr(nerr) = 16                                             
               return                                                      
            endif
            call patints(phpto,phdir,'AGAP',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 14                
               return                                                       
            endif                                                           
         else
            chin = chradid 
            call patrefr(chin,'AIR',phdir,ierr)
            if (ierr.ne.0) then                
               nerr = nerr + 1                                             
               verr(nerr) = 17                                             
               return                                                      
            endif                                                           
            call patints(phpto,phdir,'AGAP',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 14                
               return                                                       
            endif                                                           
        endif


      elseif (chradid.eq.'PGL') then

      /* photon starting in plexiglass */

       if (HPGL.eq.0.0) then
        print*,'>>> patrace_full:photons starting in PGL when HPGL=0'
        return 
       endif   

         call patints(phpto,phdir,'PGLB',ierr) 
         if (ierr.ne.0) then                                             
            nerr = nerr + 1                                               
            verr(nerr) = 13                    
            return                                                       
         endif                                                           
         call patrefr('PGL','AIR',phdir,ierr)  
         if (ierr.ne.0) then                   
            nerr = nerr + 1                                             
            verr(nerr) = 16                                             
            return                                                      
         endif                                                          
        call patints(phpto,phdir,'AGAP',ierr)                          
        if (ierr.ne.0) then                                             
           nerr = nerr + 1                                              
           verr(nerr) = 14               
           return                                                       
        endif                                                           
      endif                      
                                   
* ... intersect photon with PMT plane

      call patints(phpto,phdir,'PMTD',ierr)                         

* ... propagating photons to pmt matrix                    

* -----> FLIGHT

      rpoint = sqrt(phpto(1)**2+phpto(2)**2)                               
      do while (rpoint.gt.RBMIR+TGMIR*ZBMIRGAP)                                           

        call patints(phpto,phdir,'MIRR',ierr) 
        if (ierr.ne.0) return   
        rpoint = sqrt(phpto(1)**2+phpto(2)**2)                            

        if(phpto(3).gt.(zpmtdet-zbmirgap)) then 
          call patints(phpto,phdir,'PMTD',ierr)                             
        else       ! reflected photon
          nmiref = nmiref + 1                                               
          if (NMIREF.GT.10)then
           nerr=nerr+1
           RETURN
          endif
          call patrefl(phpto,phdir) 
          call patints(phpto,phdir,'PMTD',ierr)                             
          rpoint = sqrt(phpto(1)**2+phpto(2)**2)                            
        endif 

      enddo

         /* EMC hole */                                                       

         IFLFL=0

* EMC HOLE:

         if((abs(phpto(1)).lt.XLIMH .and.        
     +       abs(phpto(2)).lt.YLIMV)) then 
             nerr = nerr + 1                                                
             verr(nerr) = 19 
             goto 10              
         endif 

* EMC INNER CORNERS:

         if(  abs(phpto(1)).lt.XLIMH+pitchx.and.
     +        abs(phpto(1)).gt.XLIMV-pitchx.and.
     +        abs(phpto(2)).lt.YLIMV+pitchy.and.
     +        abs(phpto(2)).gt.YLIMH-pitchy
     +     ) then
            nerr = nerr + 1                                                
            verr(nerr) = 19
             goto 10              
         endif

* OUTSIDE PLANES:

         if(
     +       (abs(phpto(1)).gt.XLIMH+9*PITCHX) 
     +         .or. 
     +       (abs(phpto(2)).gt.YLIMV+9*PITCHY) 
     +     ) then
            nerr = nerr + 1                                                
            verr(nerr) = 19
             goto 10              
         endif

* OUTSIDE DIVISIONS:

         if(
     +       (phpto(2).gt.YLIMV.and.
     +        phpto(1).lt.-DX0 .and.
     +        phpto(1).gt.-JUMP-DX0)    
     +         .OR.
     +       (phpto(2).lt.-YLIMV.and.
     +        phpto(1).gt. DX0.and.
     +        phpto(1).lt. JUMP+DX0)    
     +         .OR.       
     +        ABS(PHPTO(1)).GT.XLIMV.AND.ABS(PHPTO(1)).LT.XLIMH 
     +         .OR.       
     +        ABS(PHPTO(2)).GT.YLIMH.AND.ABS(PHPTO(2)).LT.YLIMV 
     +     ) then
            nerr = nerr + 1                                                
            verr(nerr) = 19
             goto 10              
         endif

         if(
     +       (ABS(PHPTO(1)).gt.XLIMH+8*pitchx.and.
     +        ABS(PHPTO(2)).gt.YLIMH-3*pitchy)
     +         .OR.
     +       (ABS(PHPTO(1)).gt.XLIMH+7*pitchx.and.
     +        ABS(PHPTO(2)).gt.YLIMH-pitchy)
     +         .OR.   
     +       (ABS(PHPTO(2)).gt.YLIMV+8*pitchy.and.
     +        ABS(PHPTO(1)).gt.XLIMV-3*pitchx)
     +         .OR.   
     +       (ABS(PHPTO(2)).gt.YLIMV+7*pitchy.and.
     +        ABS(PHPTO(1)).gt.XLIMV-pitchx)
     +  ) then
            nerr = nerr + 1                                                
            verr(nerr) = 19
             goto 10              
         endif

* CORNER MODULES:

         DO i=1,8
          if(abs(phpto(1)).gt.xlimh+(8-i)*pitchx.and.    
     +       abs(phpto(2)).gt.ylimv+(i-1)*pitchy)then
            nerr = nerr + 1                                                
            verr(nerr) = 19
            goto 10 
          endif 
         enddo

* ... output coordinates                                                   
 10   continue   
      xdet = phpto(1)                                                      
      ydet = phpto(2)                   
* --- end                                                                  
      return                                                               
      end                                                                       
* ==================================================================
      subroutine patphot(phig,cerang,coneg)
* ==================================================================
********************************************************************
*     AIMS       : Get photon direction in RICH-frame generated for 
*                  a given phi angle
*     PARAMETERS :
*                   IN - PHIG   = photon PHI angle
*                        CERANGH = cerenkov angle
*                  OUT - CONEG  = photon direction
********************************************************************
      implicit none

      integer irotflg
      real figrotm(3,3)
      common /matrixf/ irotflg        
      common /matrixc/ figrotm  

      real pconeg(3),coneg(3)           

*... local variables
      integer i,j
      real cerang,phig          


* --- geratriz
      pconeg(1) =  sin(cerang)*cos(phig) 
      pconeg(2) =  sin(cerang)*sin(phig) 
      pconeg(3) =  cos(cerang)

* --- change frame
      call vzero(coneg,3)
      do i=1,3
         do j=1,3
            coneg(i) = coneg(i)+figrotm(i,j)*pconeg(j)  
         enddo
      enddo

      return
      end

* ================================================================
      subroutine patints(p0,v0,CHOPT,ierr)
* ================================================================
******************************************************************
*
*   Intersection of a straight line (photon) with a surface Z=cte
*     
*   PARAMETERS:
*     IN  - P0(3) = straight line point
*           V0(3) = straight line direction
*           CHOPT = defines surface ('RADB','MIRR', PMTD')
*     OUT - P0I(3) = intersection point
*           V0I(3) = normal to the plane at the inteserction point 
*                   (pointing inwards) 
*
******************************************************************
      implicit none 
#include "../include/richrec_lipf.h"
            
      real p0(3), v0(3), p0i(3), v0i(3), pc(3)
      real srtfroot
      character*4 chopt 

*... local variables
      integer ierr 
      real arat,brat,dx,dy,dz,A,B,C,factroot,root_plus,root_minus,
     &     root1,dist,radius

* --- init
      ierr = 0

* ------------------------------------
      IF (CHOPT.EQ.'PMTD') THEN       
* ------------------------------------

* --- intersection with detector plane

      P0I(3) = ZPMTDET
      P0I(1) = P0(1) + V0(1)/V0(3)*(ZPMTDET-P0(3))
      P0I(2) = P0(2) + V0(2)/V0(3)*(ZPMTDET-P0(3))
     
* ------------------------------------  
      ELSEIF (CHOPT.EQ.'MIRR') THEN       
* ------------------------------------  

* --- photon direction
      arat = v0(1)/v0(3)
      brat = v0(2)/v0(3)
      dx   = p0(1)-vtmir(1)
      dy   = p0(2)-vtmir(2) 
      dz   = p0(3)-vtmir(3) 

* --- intersection point
      A      = arat**2 + brat**2 - TGMIR**2
      B      = 2.*( arat*(p0(1)-VTMIR(1)) +
     &              brat*(P0(2)-VTMIR(2)) -
     &              TGMIR**2*(P0(3)-VTMIR(3))
     &            )          
      C      = (P0(1)-VTMIR(1))**2 + 
     &         (P0(2)-VTMIR(2))**2 - 
     &         TGMIR**2*(P0(3)-VTMIR(3))**2

      
*      factroot = 4*(2.*arat*brat*dx*dy - 
*     +           2.*tgmir**2*dz*(arat*dx+brat*dy) +
*     +           arat**2*(dy**2-tgmir**2*dz**2) +
*     +           brat**2*(dx**2-tgmir**2*dz**2) -
*     +           tgmir**2*(dx**2+dy**2))
      factroot=4*(TGMIR**2*(dx**2+dy**2-2*dx*dz*arat+dz**2*arat**2-
     +         2*dy*dz*brat+dz**2*brat**2)
     +         -dy**2*arat**2+2*dx*dy*arat*brat-dx**2*brat**2)


*      print*,'argg ',phig,b**2-4*a*c,factroot,acos(v0(3))*RADDEG

cc      root_plus  = -0.5*B/A+0.5/A*sqrt((B**2-4.*A*C))
cc      root_minus = -0.5*B/A-0.5/A*sqrt((B**2-4.*A*C))

      srtfroot=sqrt(factroot)
      root_plus  = -0.5*B/A+0.5/A*srtfroot
      root_minus = -0.5*B/A-0.5/A*srtfroot
* ... closest intersection
      if (root_minus.lt.0. .and. root_plus.lt.0.) then
         root1 = -min(abs(root_minus),abs(root_plus))
      else
         ierr=1
         return
      endif
* ... intersection point coordinates
      p0i(3) = root1 + p0(3)
      p0i(1) = p0(1) + arat*(p0i(3)-p0(3))
      p0i(2) = p0(2) + brat*(p0i(3)-p0(3))

* ------------------------------------  
      ELSEIF (CHOPT.EQ.'RADB') THEN     
* ------------------------------------  
* --- refraction point                  
*      p0i(3) = HRAD                   
      p0i(3) = ZPMTDET-ZBMIRGAP-HMIR-ZTMIRGAP-HPGL
      dist = (p0i(3) - p0(3))/v0(3)      
      p0i(1) = p0(1) + dist*v0(1)        
      p0i(2) = p0(2) + dist*v0(2)        
      radius = sqrt(p0i(1)**2+p0i(2)**2)  
      if (radius.gt.RTMIR) then         
       ierr = 1                       
       return                         
      endif
* ------------------------------------  
      ELSEIF (CHOPT.EQ.'PGLB') THEN     
* ------------------------------------  
* --- refraction point                  

      p0i(3) = ZPMTDET-ZBMIRGAP-HMIR-ZTMIRGAP
      dist = (p0i(3) - p0(3))/v0(3)      
      p0i(1) = p0(1) + dist*v0(1)        
      p0i(2) = p0(2) + dist*v0(2)        
      radius = sqrt(p0i(1)**2+p0i(2)**2)  
      if (radius.gt.RTMIR) then         
       ierr = 1                       
       return                         
      endif
* ------------------------------------  
      ELSEIF (CHOPT.EQ.'AGAP') THEN     
* ------------------------------------  
* --- refraction point                  
      p0i(3) = ZPMTDET-ZBMIRGAP-HMIR
      dist = (p0i(3) - p0(3))/v0(3)      
      p0i(1) = p0(1) + dist*v0(1)        
      p0i(2) = p0(2) + dist*v0(2)        
      radius = sqrt(p0i(1)**2+p0i(2)**2)  
      if (radius.gt.RTMIR) then         
       ierr = 1                       
       return                         
      endif

* ------------------------------------
      ENDIF 
* ------------------------------------
* --- output intersection point
      call ucopy(p0i,p0,3)
* --- end
      return
      end
* =============================================================
      subroutine patrefr(CHIN,CHOUT,phdir,ierr)
* =============================================================
***************************************************************
*
*     It refracts the photon at the exit of the radiator
*     PARAMETERS:
*     IN     - ierr  = 1 photon not tranmitted due to critical 
*                        angle
*     IN/OUT - phdir = photon direction
***************************************************************
      implicit none
#include "../include/richrec_lipf.h"

      real phdir(3),phdirn(3)
      real vnormal(3)
      character*3 CHIN,CHOUT

*... local variables
      integer ierr,i 
      real costhi,acthi,thi,refindex_1,refindex_2,sthi,sintht,tht,A,B,
     &     phdirnm
*... functions
      real vdot  
      external vdot

* --- init
      ierr = 0
* --- normal to the radiator plane on the general frame 
*    (radiator inward)
      vnormal(1) = 0.
      vnormal(2) = 0.
      vnormal(3) = -1.
* --- incident angle
      costhi = - vdot(phdir,vnormal,3)
      acthi = abs(costhi)
      if (acthi.gt.1.) costhi = costhi/acthi 
      thi    = acos(costhi)
* --- transmission angle
      if (CHIN.eq.'AGL'.or.CHIN.eq.'NAF') refindex_1 = refindex
      if (CHIN.eq.'PGL')                  refindex_1 = pglix 
      if (CHOUT.eq.'AIR')                 refindex_2 = 1.0
      if (CHOUT.eq.'PGL')                 refindex_2 = pglix
*
      sthi = sin(thi)
      sintht = refindex_1/refindex_2*sthi
      if (sintht.ge.1.) then !critical angle    
         ierr = 1                               
         return                                 
      endif                                     
      tht = asin(sintht)                        
* --- new photon direction
      if (thi.ne.0.) then
         A = sintht/sthi
         B = sin(tht-thi)/sthi
         phdirn(1) = A*phdir(1)+B*vnormal(1)
         phdirn(2) = A*phdir(2)+B*vnormal(2)
         phdirn(3) = A*phdir(3)+B*vnormal(3)
      else
         call ucopy(phdir,phdirn,3)
      endif
* --- normalization of new direction
      phdirnm = sqrt(phdirn(1)**2+phdirn(2)**2+phdirn(3)**2)
      do 1 i=1,3
 1       phdirn(i) = phdirn(i)/phdirnm
* --- output
      call ucopy(phdirn,phdir,3)

* --- end
      return
      end
* ===============================================================
      subroutine patrefl(phpto,phdir)
* ===============================================================
*****************************************************************
*
*     It reflects the photon at the mirror 
*     
*      Parameters:
*      IN  - phdir(3) - photon direction
*            phpto(3) = intersection point on mirror
*     OUT  - phdir
*****************************************************************
      implicit none
#include "../include/richrec_lipf.h"

      real phdir(3),vnorm(3),phdir_ref(3),phpto(3)

*... local variables
      integer i 
      real phdirm,vnormm,costhi,phdir_refm
*... functions
      real vdot  
      external vdot

* --- normalize incident angle

      phdirm = sqrt(phdir(1)**2+phdir(2)**2+phdir(3)**2)
      do 1 i=1,3      
 1       phdir(i) = phdir(i)/phdirm

* --- normal vector at the intersection point
      vnorm(1) = -2.*(phpto(1)-vtmir(1))
      vnorm(2) = -2.*(phpto(2)-vtmir(2))
      vnorm(3) =  2.*TGMIR**2*(phpto(3)-vtmir(3))
      vnormm = sqrt(vnorm(1)**2+vnorm(2)**2+vnorm(3)**2)
      do 2 i=1,3
 2       vnorm(i)=vnorm(i)/vnormm
C      print*,'vnorm=',vnorm

* --- incident angle
      costhi = -vdot(phdir,vnorm,3)
C      print*,'thi=',acos(costhi)

* --- reflected direction of the photon
      phdir_ref(1) = phdir(1) + 2*costhi*vnorm(1)
      phdir_ref(2) = phdir(2) + 2*costhi*vnorm(2)
      phdir_ref(3) = phdir(3) + 2*costhi*vnorm(3)
      phdir_refm = sqrt(phdir_ref(1)**2+phdir_ref(2)**2+phdir_ref(3)**2)
      do 3 i=1,3        
 3       phdir_ref(i) = phdir_ref(i)/phdir_refm

* --- output
      call ucopy(phdir_ref,phdir,3)

* --- print
C      costhr = vdot(vnorm,phdir,3)
C      print*,'thr=',acos(costhr)
* --- end
      return
      end
* ===========================================================
      subroutine patmatr
* ===========================================================
*************************************************************
* 
* gets (x,y,z) from (x',y',z') by defining FIGROTM matrix
*
*************************************************************
      implicit none
*
      real pimp,pmom,pthe,pphi,pcoopmt,cerang,pbeta,pchg

      common /liptrk/pimp(3),
     +               pmom,
     +               pthe,
     +               pphi,
     +               pcoopmt(3),
     +               cerang,
     +               pbeta,
     +               pchg 

      integer irotflg
      real figrotm(3,3)
      common /matrixf/ irotflg        
      common /matrixc/ figrotm  

      real paxisx(3),paxisy(3),paxisz(3)
      real axisx(3),axisy(3),axisz(3)

*... local variables
      real paxisxm,  paxisym,  paxiszm  
*... functions
      real vdot  
      external vdot

* --- general frame
      axisx(1) = 1. !X 
      axisx(2) = 0.    
      axisx(3) = 0.    
*
      axisy(1) = 0. !Y 
      axisy(2) = 1.    
      axisy(3) = 0. 
*
      axisz(1) = 0. !Z    
      axisz(2) = 0.    
      axisz(3) = 1.    


* --- particle frame axis
      if (pthe.eq.0.) then
         call ucopy(axisx,paxisx,3)
         call ucopy(axisy,paxisy,3)
         call ucopy(axisz,paxisz,3)
      else
        paxisz(1) = sin(pthe)*cos(pphi)   !Z
        paxisz(2) = sin(pthe)*sin(pphi)
        paxisz(3) = abs(cos(pthe))  
        paxiszm = sqrt(paxisz(1)**2+paxisz(2)**2+paxisz(3)**2)
*
        paxisy(1) =  paxisz(2)             !Y
        paxisy(2) = -paxisz(1) 
        paxisy(3) =  0.
        paxisym = sqrt(paxisy(1)**2+paxisy(2)**2+paxisy(3)**2)
        paxisy(1) =  paxisy(1)/paxisym
        paxisy(2) =  paxisy(2)/paxisym
        paxisym = sqrt(paxisy(1)**2+paxisy(2)**2+paxisy(3)**2)

*
        call cross(paxisy,paxisz,paxisx)  !X
        paxisxm = sqrt(paxisx(1)**2+paxisx(2)**2+paxisx(3)**2)
        paxisx(1) = paxisx(1)/paxisxm
        paxisx(2) = paxisx(2)/paxisxm
        paxisx(3) = paxisx(3)/paxisxm
      endif
C      print*,' ***** particle axis ******'
C      print*,(paxisx(i),i=1,3),paxisxm
C      print*,(paxisy(i),i=1,3),paxisym
C      print*,(paxisz(i),i=1,3),paxiszm

* --- matrix elements
      figrotm(1,1) = vdot(axisx,paxisx,3)            
      figrotm(1,2) = vdot(axisx,paxisy,3)            
      figrotm(1,3) = vdot(axisx,paxisz,3)            
*
      figrotm(2,1) = vdot(axisy,paxisx,3)            
      figrotm(2,2) = vdot(axisy,paxisy,3)            
      figrotm(2,3) = vdot(axisy,paxisz,3)            
*
      figrotm(3,1) = vdot(axisz,paxisx,3)            
      figrotm(3,2) = vdot(axisz,paxisy,3)            
      figrotm(3,3) = vdot(axisz,paxisz,3)            
* 
C      print*,' ***** rotation matrix ******'
C      print*,(figrotm(1,j),j=1,3)
C      print*,(figrotm(2,j),j=1,3)
C      print*,(figrotm(3,j),j=1,3)
C      print*,'*****************************'

      return
      end

***********************************************
************* FUNCTIONS ***********************
***********************************************
* ================================================================
      real function fphotdist(phig)
* ================================================================
******************************************************************
*  It determines the distance crossed by a photon in the radiator
******************************************************************
      implicit none
#include "../include/richrec_lipf.h"

      real par(10)
      common/parfunc/par
      real phpto(3),phdir(3)

      integer ierr
      real thetac,phig,dist2  


* --- init
      fphotdist = -999.
      thetac=par(2)
* --- rotation matrix between detector frame and particle frame             
      if (irotflg.eq.0) then                                                
         call patmatr                                                       
         irotflg = 1                                                        
      endif 

* --- get photon cosine directors
      call patphot(phig,cerang,phdir)
      if (phdir(3).lt.0.) return

* --- get photon intersection point at radiator
      call ucopy(pcervtx,phpto,3)
      call patints(phpto,phdir,'RADB',ierr)
      if (ierr.ne.0) return

* --- set distance acrossed in radiator
      dist2 = (pcervtx(1)-phpto(1))**2 +
     +        (pcervtx(2)-phpto(2))**2 +
     +        (pcervtx(3)-phpto(3))**2 
      fphotdist = sqrt(dist2)

* --- end
      return
      end

      subroutine  pmtpos_flight(IPM,XL,XR,YU,YD,XC,YC,IFLG)
****************************************************************
* AIMS  : get x and y position of pmt  
*(used by DISPLAY and by acceptance) 
* PARAMS:
*        IN - pmtnb  = pmt number
*        OUT- xleft  = X left limit     
*             xright = X right limit
*             yup    = Y up limit     
*             ydown  = Y down limit
*             xcent  = X center
*             ycent  = Y center     
*        IFLG 1 - external contour -first out
*             1 - internal non existing PMT
*             3 - external non existing PMT 
****************************************************************
      implicit none
#include "../include/richrec_lipf.h"

      integer IPM,IFLG,IDUM,IP,IX,IY,IDIF,I,K
      real XL,XR,YU,YD,XC,YC

      IDUM=1
      IFLG=0

      IF(IPM.LT.153) THEN
       IP=IPM 

       IX = MOD(IP,9) +1 
       IY = INT(IP/9.)+1 

       XL = (IX-1)*PITCHX 
       XR = (IX)*PITCHX 
       YD = (IY-1)*PITCHY 
       YU = (IY)*PITCHY

       XL=  XL + XLIMH  
       XR=  XR + XLIMH  
*
       YU= -YLIMH  +  YU 
       YD= -YLIMH  +  YD 

       IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
*
      ELSEIF(IPM.GE.153.AND.IPM.LT.189) THEN

       IP=IPM-153 
       IDIF=IP 
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN  
          IDIF=IDIF-K
        ELSE
          IY=I
          IX=IDIF+1 
          GOTO 11
        ENDIF 
       ENDDO 

 11    XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YD =  (IY-1)*PITCHY
       YU =  (IY)*PITCHY

       XL=XL+XLIMH 
       XR=XR+XLIMH 
       YD=YD+YLIMV 
       YU=YU+YLIMV 

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.189.AND.IPM.LT.342) THEN
* 3RD AND 4TH PLANE
       IP=IPM-189         
       IY = MOD(IP,9) + 1 
       IX = INT(IP/9.)+ 1 

       XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YD =   (IY-1)*PITCHY
       YU =   (IY)*PITCHY

       XL=XL + XLIMV 
       XR=XR + XLIMV 

       IF(IX.GE.10) THEN  
        XL = XL - JUMP 
        XR = XR - JUMP 
       ENDIF 

       YD=YD+YLIMV 
       YU=YU+YLIMV 

       IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1

      ELSEIF(IPM.GE.342.AND.IPM.LT.378) THEN ! 5TH PLANE

       IP=IPM-342         
       IDIF=IP 
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN  
         IDIF=IDIF-K
        ELSE
         IX=I
         IY=IDIF+1 
         goto 12
        endif 
       enddo 

 12    XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YD =   (IY-1)*PITCHY
       YU =   (IY)*PITCHY

       XL=XL- XLIMH  
       XR=XR- XLIMH  
       YD=YD+ YLIMV  
       YU=YU+ YLIMV  

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.378.AND.IPM.LT.531) THEN !  6TH PLANE

       IP=IPM-378

       IX = MOD(IP,9) +1 
       IY = INT(IP/9.)+1 

       XR = -(IX-1)*PITCHX
       XL = -(IX)*PITCHX
       YU = -(IY-1)*PITCHY
       YD = -(IY)*PITCHY

       XL= XL - XLIMH  
       XR= XR - XLIMH 
       YU= YU + YLIMH 
       YD= YD + YLIMH 

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1

      ELSEIF(IPM.GE.531.AND.IPM.LT.567) THEN ! 7TH PLANE(LOW LEFT)

       IP=IPM-531
       IDIF=IP 
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN  
          IDIF=IDIF-K
        ELSE
          IY=I
          IX=IDIF+1 
          GOTO 13
        ENDIF 
       ENDDO 

 13    XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YU =  -(IY-1)*PITCHY
       YD =  -(IY)*PITCHY

       XL=XL-XLIMH 
       XR=XR-XLIMH     
       YD=YD-YLIMV 
       YU=YU-YLIMV 

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.567.AND.IPM.LT.720) THEN !  8TH PLANE(LOWER LEFT)

       IP=IPM-567

       IX = INT(IP/9.)+ 1 
       IY = MOD(IP,9) + 1 

       XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YU = -(IY-1)*PITCHY
       YD = -(IY)*PITCHY

       XL=XL - XLIMV 
       XR=XR - XLIMV 
       IF(IX.GE.10) THEN  
        XL = XL + JUMP 
        XR = XR + JUMP 
       ENDIF 
       YD=YD - YLIMV
       YU=YU - YLIMV

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
      ELSEif(ipm.ge.720.and.ipm.le.755) then
* 10th plane(lower right corner)
       IP=IPM-720
       IDIF=IP
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN  
          IDIF=IDIF-K
        ELSE
          IX=I
          IY=IDIF+1 
          GOTO 14
        ENDIF 
       ENDDO 

 14    XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YU =  -(IY-1)*PITCHY
       YD =  -(IY)*PITCHY

       XL=XL + XLIMH 
       XR=XR + XLIMH 
       YD=YD - YLIMV 
       YU=YU - YLIMV 

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.
     & IP.EQ.25.OR.IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35)IFLG=1
      ELSE 
       IFLG=3
      ENDIF 

      XR=XR+XCPMM
      YU=YU+YCPMM 
      XL=XL+XCPMM
      YD=YD+YCPMM 

      XC =(XL+XR)/2. 
      YC =(YU+YD)/2. 

* --- end
      return
      end

      subroutine  pmtnmb_flight(XPHOT,YPHOT,IPM,IFLG)
********************************************************************
* AIMS  : 
*get pmt nmb and flag (used by pattrace to flag path through matrix)
* PARAMS:
*        IN - XPHOT,YPHOT
*        OUT- IPM  PMT number
*             IFLG  0 - valid PMT
*                   1 - external contour -first out
*                   2 - internal non existing PMT
*                   3 - external non existing PMT (dead areas)
********************************************************************
      implicit none
#include "../include/richrec_lipf.h"

      integer IPM,IFLG,IP,IX,IY,I
      real XPHOT,YPHOT,RPHOT,dxp,dyp

      IP   = -1      
      IPM  = -1
      IFLG =  3

* move to matrix coordinates
*      XPHOT=XPHOT - XCPMM
*      YPHOT=YPHOT - YCPMM

      IF(ABS(YPHOT).LT.YLIMH) THEN  ! 1ST/6TH PLANES (153 PMTS EACH)
       IF(ABS(XPHOT).GT.XLIMH.AND.ABS(XPHOT).LT.XLIMH+9*PITCHX)THEN 

          DXP=ABS(ABS(XPHOT)-XLIMH)
          DYP=ABS(YPHOT+SIGN(YLIMH,XPHOT))

          IX=(INT(DXP/PITCHX)+1)          
          IY=(INT(DYP/PITCHY)+1)         

          IP= (IY-1)*9 + (IX-1)

          IF (XPHOT.GT.0) THEN 
           IFLG =  0   
           IPM  = IP  
          ELSE
           IFLG =  0   
           IPM  = IP + 2*(153+36)
          ENDIF   

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
         ENDIF              

      ELSEIF(
* TOP/BOTTOM PLANES : 3RD/4th/8TH/9th 
     & (ABS(XPHOT).LT.(XLIMV)).AND. 
     &   ABS(YPHOT).GT.YLIMV.AND.
     &   ABS(YPHOT).LT.YLIMV+9*PITCHY) THEN 
             IF((XPHOT.LT.-DX0.AND.YPHOT.GT.0).OR.
     &          (XPHOT.GT. DX0.AND.YPHOT.LT.0))THEN
               DXP=ABS(XPHOT+SIGN(-JUMP+XLIMV,XPHOT))
               IFLG = 0
             ELSEIF((XPHOT.GT.-DX0+JUMP.AND.YPHOT.GT.0).OR.
     &         (XPHOT.LT.DX0-JUMP.AND.YPHOT.LT.0)) THEN
               DXP=ABS(ABS(XPHOT)-XLIMV)
               IFLG = 0
             ENDIF  
             DYP=ABS(ABS(YPHOT)-YLIMV)
*
             IF (IFLG.EQ.0) THEN
               IX=(INT(DXP/PITCHX)+1)          
               IY=(INT(DYP/PITCHY)+1)         
               IP=(IX-1)*9 + (IY-1)

               IF(YPHOT.GT.0) THEN
                 IPM= IP+153+36
               ELSE  
                 IPM= IP+3*(153+36)
               ENDIF 

               IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &              IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &              IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152)
     &              IFLG=1

             ENDIF
* CORNER PLANES 
      ELSEIF(ABS(XPHOT).gt.XLIMH.AND.
     &       ABS(YPHOT).gt.YLIMV) THEN  

        DXP=ABS(ABS(XPHOT)-XLIMH)
        DYP=ABS(ABS(YPHOT)-YLIMV)

        IX=(INT(DXP/PITCHX)+1)          
        IY=(INT(DYP/PITCHY)+1)         
        IP=0


* 2nd/5th planes (36 each)
        IF(XPHOT*YPHOT.GT.0) THEN 
         DO I=1,IY-1
            IP=IP+(8-(I-1))
         ENDDO 
         IP=IP+(IX-1)        
          IF(XPHOT.GT.0) THEN 
           IPM= IP+153
          ELSE
           IPM= IP+3*153+2*36
          ENDIF
* 7nd/10th planes (36 each)
        ELSEIF(XPHOT*YPHOT.LT.0)then    
         DO I=1,IX-1
            IP=IP+(8-(I-1))
         ENDDO 
         IP=IP+(IY-1)        
         IF(XPHOT.GT.0) THEN
           IPM= IP+4*153+3*36
          ELSE
           IPM= IP+2*153+36
          ENDIF
        ENDIF

       IF ((IY-1)+(IX-1).LE.6) THEN
         IFLG=0
*       ELSEIF ((IY-1)+(IX-1).EQ.6) THEN
*         IFLG=1
       ELSE
         IFLG=3
         IPM=-1
       ENDIF
 
       IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSE
       IFLG=3
      ENDIF 


* --- END
      END






























