*$Id: richlikebeta_lip.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*+LIP  velocity reconstruction 

       subroutine richlikebeta_lip(iproc,ireflec)
*=============================================================
* beta reconstruction steering subroutine                      
*=============================================================
      implicit none

*  DECLARATIONS 
* ==============
#include "../include/richrec_lipf.h"

      integer iproc,ireflec

* .. functions       
      real fliketot_phihint, brentmod1
      external fliketot_phihint,brentmod1
* ..  local variables 
      integer i,j,nn,iter
      real cangrectol
      real fliketot_buf, thetac_buf, xthet, testval , thetac_hint
      real thetbrack1, thetbrack2, thetbrack3, cangrecmin,
     & chi2_evt,prob_evt
     
*  INITIALIZATION 
* ================

* ..  reconstruction variables and parameters (angles in radians): 
      call vfill(cangrec,ntherecmax,-999.)
      ipthetac = 0
      ntherec=0       
      chi2rec_fit=999.
      nbushits_fit=0
      ireflec=0  
      cangrectol = degrad*0.01 
      if (chradid.eq.'AGL') then
         cangrecup  = acos(1./refindex)*1.05
         cangreclow = 4.0*DEGRAD 
         PHISTEP    = 10.0       
         THCSTEP    = 0.5
         FSIGMA2    = 0.3**2     
         FBACKGR    = 0.2        
      elseif (chradid.eq.'NAF' .or. chradid.eq.'PGL') then
         cangrecup  = acos(1./refindex)*1.05
         cangreclow = 20.*degrad 
         PHISTEP   =  5.     
         THCSTEP    = 0.5
         FSIGMA2   =  0.7**2 
         FBACKGR   =  0.1    
      endif

      FDISTAN   =  100.                
      CHI2HTCUT=  (4*sqrt(fsigma2))**2 

      if (iproc.lt.0) return 

* //////////////////////////////////////////////////////////////
* ==> RECONSTRUCT CERENKOV ANGLE 
* //////////////////////////////////////////////////////////////


* => find minimal solution => phi(hit), best thc

      fliketot_buf = 9999.
      thetac_buf=0.        

      do xthet=cangrecup,cangreclow,-THCSTEP*DEGRAD
          testval = fliketot_phihint(xthet)
          if(testval.lt.fliketot_buf) then
            fliketot_buf=testval   
            thetac_buf=xthet  
          endif 
      enddo   

*  => THC minimization
      thetac_hint= thetac_buf  
      thetbrack2 = thetac_buf
      thetbrack1 = thetac_hint-THCSTEP*DEGRAD
      thetbrack3 = thetac_hint+THCSTEP*DEGRAD

      chi2_evt = brentmod1(thetbrack1,thetbrack2,thetbrack3,               
     +fliketot_phihint,cangrectol,cangrecmin,iter)    

      prob_evt = fliketot_phihint(cangrecmin)

C      print*,'final >>>>>>>>> thetac.flike.buf',
C     & raddeg*cangrecmin,prob_evt

* => store reconstructed values       

      if (nbushits_fit.gt.0) then
         ipthetac = 1
         ntherec  = 1
         cangrec(ipthetac)  = cangrecmin
         likerec(ipthetac)  = prob_evt 
         chi2rec(ipthetac)  = chi2rec_fit
         nbushits(ipthetac) = nbushits_fit
         do nn=1,nbushits_fit
            ipushits(ipthetac,nn) = ipushits_fit(nn)
            iflghit(ipushits_fit(nn)) = 2
         enddo                              
         do nn=1,nbhits
            chi2hit(ipthetac,nn)    = chi2hit_fit(nn)
            nbminshit(ipthetac,nn)  = nbminshit_fit(nn)
            phihit(ipthetac,nn)     = phihit_fit(nn)
            ireflechit(ipthetac,nn) = ireflechit_fit(nn)
            if(ireflechit_fit(nn).gt.0) ireflec=ireflec+1 
C            print*,'xhit,yhit,chi2',
C     & ireflechit_fit(nn),hitscoo(1,nn),
C     & hitscoo(2,nn), chi2hit_fit(nn),iflghit(nn)
         enddo 


         if (ipthetac.ne.0) then
            betarec = 1./refindex/cos(cangrec(ipthetac))
         else
            betarec = -999.
         endif

      endif

      return
      end

      real function fliketot_phihint(cangrecx)
************************************************************
* last upDATE: 03/12/2003
*
* calculates Likelihood function <- chi2
*
*   a numerical method is used 
*   starting from the (THC,PHI(hits)) obtained previously
************************************************************
      implicit none 

*  DECLARATIONS
* ==============
#include "../include/richrec_lipf.h"
     
      real cangrecx      

* .. local variables

      integer ievbuff,itkbuff   
      data ievbuff,itkbuff /0,0/
      save ievbuff,itkbuff

      integer i,nmaxiter,nsteps,ierr,iter
      real dxdet,dydet,dzdet,dxpar,dypar 
      real phibrack1, phibrack2, phibrack3, f1,f2,f3,
     &     funcphi1,funcphi2,funcphi3
      real phitol, phphimin, chi2_hit

*      real residual_max 
*      parameter(residual_max=20)    
*      integer nbhits_near 
*      save nbhits_near

      real gcte, fgaussct 
      real phihint(nhitmax)  
      save phihint

* ..  functions

      real brentmod2, fchi2hit
      external brentmod2, fchi2hit
 
*  INITIALIZATION
* ================

      fliketot_phihint = 0.0
      nbushits_fit = 0

* --- pass variable
      hypthc = cangrecx
      if (hypthc.eq.0.0) hypthc = 0.001

* ... tolerance for determination minimum
      phitol = 0.01*degrad
      nmaxiter = 25
* ... chi2
      chi2rec_fit = 0.0


*  find likelihood
* =================

      if (nbhits.eq.0          .or.
     &    hypthc.lt.cangreclow .or.
     &    hypthc.gt.cangrecup   
     &    ) then
         fliketot_phihint = 1.E+8
         goto 99
      endif

      do 1 i=1,nbhits
       if (iflghit(i).ne.0) then
            chi2hit_fit(i) = 999999.
            goto 1
       endif

* ...... current hit              

         ihit = i                 
         curhit(1) = hitscoo(1,i) 
         curhit(2) = hitscoo(2,i) 
         curhit(3) = hitscoo(3,i)
         
*        1ST CALL to fliketot_phihint ...
         IF (ievnumb.ne.ievbuff.or.itknumb.ne.itkbuff) THEN
* ========> Get phihint if phi not found 
           dxdet = hitscoo(1,i)-pcervtx(1)            
           dydet = hitscoo(2,i)-pcervtx(2)            
           dzdet = hitscoo(3,i)-pcervtx(3)
           if(pthe*RADDEG.ne.0.)then
            dxpar = - cos(pthe)*cos(pphi)*dxdet 
     +              - cos(pthe)*sin(pphi)*dydet 
     +              + sin(pthe)*dzdet
            dypar =   sin(pphi)*dxdet 
     +              - cos(pphi)*dydet 
            phihint(i) = atan2(dypar,dxpar)
            if(phihint(i).lt.0.)phihint(i)=phihint(i)+twopi
           else 
            phihint(i) = atan2(dydet,dxdet)            
            if(phihint(i).lt.0.)phihint(i)=phihint(i)+twopi
           endif

* use after MAT!!
* conversion to patrace1 frame of reference (used in fit) 
* Z points up,  x is the same , y changes sign 
**           phihit_fit(i)=twopi-phihit_fit(i) 
**          if(phihit_fit(i).gt.twopi) phihit_fit(i)=phihit_fit(i)-twopi 
**          if(phihit_fit(i).lt.0)     phihit_fit(i)=phihit_fit(i)+twopi 

         else
          phihint(i)=phihit_fit(i)        
         ENDIF ! END OF 1ST CALL !      

* ..     build bracket by hand

         phibrack1 = phihint(i)- 20.*DEGRAD
         phibrack2 = phihint(i)
         phibrack3 = phihint(i)+ 20.*DEGRAD
         f1        = fchi2hit(phibrack1)
         f2        = fchi2hit(phibrack2)
         f3        = fchi2hit(phibrack3)
         if((f1-f2).lt.0..or.(f2-f3).gt.0)then

* ..     function bracketing 

         phibrack1 = phihint(i)-0.5*DEGRAD
         phibrack2 = phibrack1 +1.0*DEGRAD           
         call minbrackphi(phibrack1,phibrack2,phibrack3,
     &                    funcphi1,funcphi2,funcphi3,
     &                    fchi2hit,nmaxiter,nsteps,ierr)        
           if (ierr.ne.0)goto 1
         endif

         if(fchi2hit(phibrack2).gt.1.e7)goto 1

* ..     find the minimal phi of the hit 

         chi2_hit = brentmod2(phibrack1,phibrack2,phibrack3,
     + fchi2hit,phitol,phphimin,iter)
 
         if (int(phphimin/twopi).gt.0)
     & phphimin = phphimin - int(phphimin/twopi)*twopi
         if(phphimin.lt.0.)
     & phphimin = phphimin + (int(phphimin/twopi)+1)*twopi

       phihit_fit(i)     = phphimin                  
       chi2hit_fit(i)    = chi2_hit
       ireflechit_fit(i) = nmirefhit

* .. LIKELIHOOD function for the hit

       gcte   = 1./sqrt(twopi*fsigma2)

       fgaussct = -log( 
     +                   (1.-FBACKGR)*gcte*
     +                   exp(-0.5*chi2hit_fit(i)/FSIGMA2)+
     +                   FBACKGR/FDISTAN
     +                  )

       fliketot_phihint = fliketot_phihint + fgaussct

* .. nb hits within a chi2 cut

       if (chi2hit_fit(i).lt.CHI2HTCUT) then
             nbushits_fit               = nbushits_fit + 1 
             ipushits_fit(nbushits_fit) = i
             chi2rec_fit                = chi2rec_fit + chi2hit_fit(i)   
       endif 
 1    continue

      if(fliketot_phihint.eq.0..and.nbushits_fit.eq.0)then
        fliketot_phihint = 1.E+8        
      endif

      if (ievnumb.ne.ievbuff)then
         ievbuff = ievnumb
      endif
      if (itkbuff.ne.itknumb) then
         itkbuff = itknumb
      endif

 99   continue

      return
      end

*=======================================================
      real function fchi2hit(phphi)
*=======================================================
      implicit none 
*----------------
* DECLARATIONS
*----------------
#include "../include/richrec_lipf.h"
      real phphi 
* .. local 
      integer nerr,nmiref 
      integer nermax  
      parameter(nermax=10)
      integer verr(nermax)
      real cangrecx,xdet,ydet
*----------------
* INITIAIZATION
*----------------
      fchi2hit = 0.
      nerr=0
      cangrecx = hypthc
*------------------
* .. GO ...
*------------------
      CALL PATRACE1(phphi,cangrecx,xdet,ydet,nmiref,nerr,verr) 
      nmirefhit = nmiref
      if (nerr.ne.0) then
         fchi2hit = 1.E+8
         return
      endif
      fchi2hit = (curhit(1)-xdet)**2 + (curhit(2)-ydet)**2
      return
      end
*###################################################################*
*                                                                   *
*               MODIFIED SUBROUTINES                                *
*                                                                   *
*###################################################################*
* ===================================================================
      real function brentmod1(ax,bx,cx,f,tol,xmin,iter)                      
* ===================================================================         
      implicit none
      INTEGER ITMAX
*      REAL brentmod1
      REAL ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f    
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.    
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
          if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      pause 'brentmod1 exceed maximum iterations'
3     xmin=x
      brentmod1=fx
      return
      END
* ================================================================
      FUNCTION brentmod2(ax,bx,cx,f,tol,xmin,iter)
* ================================================================
      implicit none
      INTEGER ITMAX
      REAL brentmod2,ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f    
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.    
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
        if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      pause 'brentmod2 exceed maximum iterations'
3     xmin=x
      brentmod2=fx
      return
      END
********************************************************
      subroutine minbrackthe(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
********************************************************************
*     this routine makes the bracketing of a minimum of the function 
*
*     extracted from NUMERICAL RECIPES in Fortran
*     Chapter. 10: Function Minimization
* ------------------------------------------------------------------
* the output bracket is given by ax,bx,cx where ax<bx<cx (or ax>bx>cx)
* and f(ax)>f(bx) and f(cx)>f(bx)
*
*
*********************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nmax
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nmax)then
        r=(bx-ax)*(fb-fc)
        q=(bx-cx)*(fb-fa)
        u=bx-((bx-cx)*q-(bx-ax)*r)/(2.*sign(max(abs(q-r),TINY),q-r))
        ulim=bx+GLIMIT*(cx-bx)
        if((bx-u)*(u-cx).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            ax=bx
            fa=fb
            bx=u
            fb=fu
            return
          else if(fu.gt.fb)then
            cx=u
            fc=fu
            return
          endif
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        else if((cx-u)*(u-ulim).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            bx=cx
            cx=u
            u=cx+GOLD*(cx-bx)
            fb=fc
            fc=fu
            fu=func(u)
          endif
        else if((u-ulim)*(ulim-cx).ge.0.)then
          u=ulim
          fu=func(u)
        else
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        endif
        ax=bx
        bx=cx
        cx=u
        fa=fb
        fb=fc
        fc=fu
        else
          ierr=1
          return
        endif
        goto 1 
      endif

      return
      END
*===================================================================
      subroutine minbrackphi(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
********************************************************************
*   this routine makes the bracketing of a minimum of the function.
*
*   extracted from NUMERICAL RECIPES in Fortran
*   Chapter. 10: Function Minimization
*
* ------------------------------------------------------------------
*   Here no quadratic interpolation is done
* 
*   Borges 23/9/2002
********************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nmax
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1      
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nmax)then
          u=cx+GOLD*(cx-bx)
          fu=func(u)
          ax=bx
          bx=cx
          cx=u
          fa=fb
          fb=fc
          fc=fu
        else
          ierr=1
          return
        endif
        goto 1
      endif

      return
      END



















