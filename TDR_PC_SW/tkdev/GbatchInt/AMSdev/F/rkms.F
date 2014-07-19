*  $Id: rkms.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_rig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
*---------------------------------------------------------------------
* A.Chikanian, Yale, 2003
* Reconstruct track's kinematical parameters with full (nondiagonal)
* Covar.Error Metrix due to Multipole Scattering in Si tracking planes.
* Supposed to be used when the Mult.Scatt. is dominate.
*---------------------------------------------------------------------
* Input:   npo        - # points
*          npl        - plane #, associated with each point
*          xyz(3,npl) - 3d coordinates
*          dxyz(npl)  - errors (intrinsic Si planes resolution)
*          ipa        - particle Geant ID#
*          Rini       - initial Rigidity value (try the best !!!)

* Output:  Out(*)     - reconstructed parameters
*          Out(1-3)   - x, y, z 
*          Out(4-5)   - Theta, Phi
*          Out(6)     - Rigidity
*          Out(7)     - Chi2/D.F.
*          Out(8)     - not used
*          Out(9)     - err(1/Rigidity)=err(R)/(R*R)
*---------------------------------------------------
      implicit none
*     -------------------
#include "rkms.h"
*     -------------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
*-------------------------
      integer npo,npl(npo),ipa,i
      real xyz(3,npo),dxyz(3,npo)
      real x(NPma),y(NPma),z(NPma),dx(NPma),dy(NPma)
      real Rini,Out(9),Chi2,R,dR,Rin
      double precision Xout(5),Eout(5)
*---------------------------------------------------
      do i=1,npo
         x(i)= xyz(1,i)
         y(i)= xyz(2,i)
         z(i)= xyz(3,i)
        dx(i)=dxyz(1,i)
        dy(i)=dxyz(2,i)
      enddo
*     =================================================================
*     WARNING:  Xi2 depends on Rini  !!! Try the best possible Rini
*            OR do additional minimization loop with corrected Err.Matr.
*            (works, but time consumming)!!!
      Rin=Rini
      if(abs(Rini).lt.0.3.or.abs(Rini).gt.333.)
     +                                 call R_init(npo,x,y,z,Rin)
c      write(6,'(a3,8i8.2)')'np0',npo ! deb
c      write(6,'(a3,8i8.2)')'npl',npl ! deb
c      write(6,'(a3,8f8.2)')'  z',  z ! deb
*     ==========================================================
      call  rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rin,Xout,Eout,Chi2)
*     ==========================================================
      R = dsqrt(Xout(1)**2+Xout(2)**2+Xout(3)**2)
      dR=dsqrt((Xout(1)*Eout(1))**2 + (Xout(2)*Eout(2))**2
     +        +(Xout(3)*Eout(3))**2)/R
*     =============================================
      Out(1) = Xout(4)                            ! x
      Out(2) = Xout(5)                            ! y
      Out(3) = z(1)                               ! z
      Out(4) = acos(sngl(Xout(3))/R)              ! Theta
      Out(5) = atan2(sngl(Xout(2)),sngl(Xout(1))) ! Phi
      Out(6) = R*sigcha                           ! Rigidity*ChargeSign
      Out(7) = Chi2                               ! Chi2
      Out(8) = 0.                                 ! Not Used
      Out(9) = dR/(R*R)                           ! err(1/Rig)
*     =============================================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,Er,Chi2) ! MINUIT
*   A.Chikanian, Yale, 2003
*--------------------------------------------------------------------
* Input:   npo   - # points
*          npl   - plane #, associated with each point
*          x(npl),y(npl),z(npl) - 3d coordinates
*          dx(npl),dy(npl) - errors
*          ipa   - particle Geant #
*          Rini  - initial Rigidity value
* Output:  Xout  -  x, y, Rx, Ry, Rz
*          Er    - dx,dy,dRx,dRy,dRz
*          Chi2  - chi2/D.F.
*-----------------------------------------
      implicit none !!! commenting helps with realy STUPID problem (see next).
c      real     FunMi !!! Required by STUPID PAW, but doesn't like it !!!????
      External FunMi
      double precision Par(5),St(5),arglis(3),Xout(5),Er(5),bnd1,bnd2
      double precision                        Xbuf(5),Chi2B
*--------------------------------------------------------------------------
*               dPx     dPy    dPz(GeV) dx   dy(cm)  Initial MIGRAD steps 
      data St/.000100,.000100,.002000,.0005,.0005/   ! 8.98*min
* see:/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/RKMSFIT/derivstep.opt
*--------------------------------------------------------------------------
      double precision zero
      data             zero/0.d0/
      double precision grad(5),A
      data             grad/5*0.D0/
      integer i,j,it,ivar,ierflg,na   ,Npa
      data                       na/0/,Npa/5/ ! # of parameters
      character*10 CHNAM
      data         CHNAM/' '/                 ! parameter names

      integer npo,npl(npo),NitMa   ,iercou
      data                 NitMa/3/,iercou/0/

      integer ipa,iflag
      real x(npo),y(npo),z(npo),dx(npo),dy(npo)
      integer lun8
      data    lun8/67/
*---------------------------------------------
#include "rkms.h"
c#include "geant321/iounit.inc"
c there is no type definition (integer ...) in  iounit.inc
*-------------------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha

      real z1
      real Rini,Chi2,Rad
*-------------------------------
      logical first
      data    first/.true./
      save first,iercou,CHNAM,zero,npa,na,St,grad

      if(first) then ! ----------------- first
        first = .false.
#if defined __IFC
        open(unit=lun8,form='formatted',file='/dev/null',
     +  status='unknown',shared,err=100)
#else
       open(unit=lun8,form='formatted',file='/dev/null',
     +  status='unknown',err=100)
#endif

 100    continue
        Call MNINIT(5,lun8,lun8) ! Read, Write, Save
        na = 1             ! # of arguments
        arglis(1)=-1.      ! suppr. output
        Call MNEXCM(FunMI,'SET PRINT',arglis,na,ierflg,0)
        If(ierflg.ne.0) write(6,*)'MNEXCM: Print ierflg=',ierflg
        arglis(1)= 1.
        Call MNEXCM(FunMI,'SET GRAD',arglis,na,ierflg,0) ! Gradients
        If(ierflg.ne.0) write(6,*)'MNEXCM: Grad ierflg=',ierflg
*  Errors calculation !!!### Not sure that it works !!!???
        arglis(1)=1.
        Call MNEXCM(FunMI,'SET ERRo',arglis,na,ierflg,0)
        If(ierflg.ne.0) write(6,*)'MNEXCM: ERRo ierflg=',ierflg
      endif          ! ----------------- end first

      if(npo.lt.3) return !!!

      z1=z(1) ! if so Par(4)=x(1), Par(5)=y(1)
      rad = sqrt((x(2)-x(1))**2+(y(2)-y(1))**2+(z(2)-z(1))**2)

*--  refreshing hits information in common/rkms_1/...
      npoc=npo
      do i=1,NPma
        if(i.le.npo) then
          xc(i)=x(i)
          yc(i)=y(i)
          zc(i)=z(i)
        else
          xc(i)=-999.
          yc(i)=-999.
          zc(i)=-999.
        endif
      enddo
*=============================
*#include "../RKMSFIT/debug1.h"
*=============================
* First TWO passes in loop below are sign of charge checking
* Third pass is iteration with right MS-matrix (mostly for correct Xi2)
      rkstep=5. ! cm
      DO it=1,NitMa
* Initial values
        if(it.eq.NitMa) rkstep=2. ! cm
        if(it.le.2) then
                      sigcha= 1.
          if(it.eq.2) sigcha=-1.
          Par(1)=Rini*(x(2)-x(1))/rad                     ! Px0
          Par(2)=Rini*(y(2)-y(1))/rad                     ! Py0
          Par(3)=Rini*(z(2)-z(1))/rad                     ! Pz0
          Par(4)=x(1) + (z1-z(1))/(z(2)-z(1))*(x(2)-x(1)) !  x0
          Par(5)=y(1) + (z1-z(1))/(z(2)-z(1))*(y(2)-y(1)) !  y0
        else
          do j=1,Npa
            Par(j)=Xout(j)
          enddo
        endif
*---------------------------- Initial values setup
        do i=1,Npa
          Call MNPARM(i,CHNAM,Par(i),St(i),zero,zero,IERFLG)! Init.Val.Setup
          If(ierflg.ne.0) write(6,*)'### MNPARM: i,ierflg=',i,ierflg
        enddo
*----------------------------------------------- Minimization  
*       ======================================+
        call wxy_matr(npo,npl,dx,dy,ipa,Rini) ! Full Matr.
*       ======================================+
*=============================
*#include "../RKMSFIT/debug2.h"
*=============================
*       ================================================
        na=0  ! # of arguments in arglis (MNEXCM)      !
        Call MNEXCM(FunMI,'MIGRAD',arglis,na,ierflg,0) !
*       ================================================
cer        if(ierflg.ne.0.and.iercou.le.1) then        !
cer          write(6,*)'MNEXCM: ierflg=',ierflg,iercou ! always 4 !!!???
cer          iercou=iercou+1                           !
cer        endif                                       !

        do i=1,Npa                                          ! Parameters
          Call MNPOUT(i,CHNAM,Xout(i),Er(i),bnd1,bnd2,ivar) ! Extraction
        enddo

        Chi2=0.
        if(2*npo-npa.gt.0) Chi2=Dhi2/(2.*npo-npa)      ! Chi2/d.f.

        if(it.eq.1) then
          Chi2B=Chi2
          do i=1,Npa
            Xbuf(i)=Xout(i)
          enddo
        endif
        if(it.eq.2.and.chi2.gt.Chi2B) then
          sigcha=1.
          do i=1,Npa
            Xout(i)=Xbuf(i)
          enddo
        endif
      ENDDO !                                          ! NitMa 
*=============================
*#include "../RKMSFIT/debug3.h"
*=============================
      end

*--------1---------2---------3---------4---------5---------6---------7-- FCN
      Subroutine FunMI(Npa,grad,Fu,par,iflag) ! ,futil)
*   A.Chikanian, Yale, Aug,2003
*-----------------------------------
      implicit none
      integer Npa,iflag,k
      double precision Fu,Fun,grad(npa),par(npa)
*-------------------------------------------
c      write(6,'(    a3,5f10.5)')'Par',par ! debug ###
      if(iflag.eq.2) call FunDer(Npa,par,grad)
      Fu = Fun(Npa,par)
c      write(6,*)'iflag,Fu',iflag,Fu       ! debug ###
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision Function Fun(Npa,par)
*   A.Chikanian, Yale, Aug,2003
*-----------------------------------
      implicit none
      integer Npa,i
      double precision chi2su,par(Npa)
*-------------------------
#include "rkms.h"
*-------------------------
      double precision pin(3),xin(3),xt(NPma),yt(NPma) ! r8

      pin(1) = par(1) ! Px or cx
      pin(2) = par(2) ! Py or cy
      pin(3) = par(3) ! Pz or R
      xin(1) = par(4) ! x
      xin(2) = par(5) ! y
      xin(3) = zc(1)  ! z

      do i=1,Npma     !***   ???
        xt(i)=-999.   !**************
        yt(i)=-999.   !*** let it be !!!
      enddo           !*****************
*     ==========================
      call rk_trk(pin,xin,xt,yt)
      Fun = chi2su(xt,yt)
*     ==========================
*=============================
*#include "../RKMSFIT/debug4.h"
*=============================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine FunDer(npa,par,der)
*   A.Chikanian, Yale, Aug,2003
*---   numerical calculation of Fun and derivatives.
*--------------------------------------------------
      implicit none
      integer npa,j
      double precision f1,f2,x0,dp,Fun,par(npa),der(npa)
*==================================================
      double precision dp0(5),xx(5)                !
      data dp0/.000005,.000005,.000100,.0002,.0002/! Opt.Steps
*                 GeV     GeV     GeV    cm    cm  !
*==================================================
      do j=1,npa
        xx(j)=par(j)
      enddo
      do j=1,npa
        dp=dp0(j) ! /2.
        x0=xx(j)
        xx(j)=x0-dp
        f1=Fun(npa,xx)
        xx(j)=x0+dp
        f2=Fun(npa,xx)
        der(j) = .5*(f2-f1)/dp
        xx(j)=x0
      enddo
c      write(6,'(a6,5(1pe12.4))')' der  ',der ! debug ###
c      write(6,'(a6,5(1pe12.4))')'   x  ',xp  ! debug ###
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision function Chi2Su(x0,y0)
*   A.Chikanian, Yale, Feb,2003
*---------------------------------------------
* Input    x0     - x-coord. for current iteration
*          y0     - y-coord. for current iteration
* Output:  Chi2Su - chi2 value
*---------------------------------------------
      implicit none
      integer i,j
      double precision xi,yi
*-------------------------
#include "rkms.h"
*-------------------------*  xc,yc - hits coordinates
      double precision x0(NPma),y0(NPma) ! r8
      Chi2Su=wxy(1,1)*((xc(1)-x0(1))**2+(yc(1)-y0(1))**2)
      do i=2,npoc
        xi=xc(i)-x0(i)
        yi=yc(i)-y0(i)
        Chi2Su=Chi2Su+wxy(i,i)*(xi**2+yi**2)
        if(i.lt.npoc) then
        do j=i+1,npoc
          Chi2Su=Chi2Su+2.*wxy(j,i)*(xi*(xc(j)-x0(j))+yi*(yc(j)-y0(j)))
        enddo
        endif
      enddo
      Dhi2=Chi2Su
      if(Chi2Su.ge.0.) return
*=============================
c#include "../RKMSFIT/debug5.h"
*=============================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rk_trk(pin,xin,x0,y0)
*---------------------------------------------------
* Runge-Kutta integration for particle in Magn.field
* A.Chikanian, Yale, Feb.,2003
*----------------------------------------------------
*  Input:  pin(3) - initial Rigidity Px,Py,Pz (GeV/c)
*          xin(3) - initial position  x, y, z (cm)
*  Output: pou(3) -   final Rigidity Px,Py,Pz (GeV/c)
*          xou(3) -   final position  x, y, z (cm)
*-------------------------------------------------
      implicit none
      integer i,j,Nst,np
*------------------------
#include "rkms.h"
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
*==================================
*#include "../RKMSFIT/instedgeant.h"
*==================================
      double precision xm,pin(3),xin(3),x0(NPma),y0(NPma) ! r8
      double precision step,vect(7),vout(7),Stot,vni      ! r8
*                           vect(7) is  x,y,z,px,py,pz,P
*                                   or  x,y,z,cx,cy,cz,P
*                                       1 2 3  4  5  6 7
      data Stot/150./ ! Max.range(cm) in Path Length
*-------------------------------------------------------
      logical first
      data    first/.true./
      double precision w
*-------------------------------------------------------
      step=rkstep
      Nst=int(Stot/step) ! Max.# of steps
      vni=1.d0
      if(zc(1).lt.zc(2)) vni=-1.d0 ! back tracking
cxy3 ---
      vect(7)=dsqrt(pin(1)**2+pin(2)**2+pin(3)**2)   ! r8
      do i=1,3
        vect(i  )=xin(i)         !  xyz
        vect(i+3)=pin(i)/vect(7) ! cxyz
      enddo
*----------------------------------------------- Runge Kutta
      np = 1
      do i=1,Nst
        call DRKUTA(SigCha,STEP,VECT,VOUT)    ! r8
          DO j=1,2 !!!! fixed close planes problem(AMS02)
        if(vni*vect(3).ge.vni*zc(np).and.vni*vout(3).lt.vni*zc(np))then
          w=(zc(np)-vout(3))/(vect(3)-vout(3))
          x0(np) = vect(1)*w + vout(1)*(1.d0-w)
          y0(np) = vect(2)*w + vout(2)*(1.d0-w)
c          write(6,'(a3,i2,a3,8f10.5)')' np',np,' x0',x0 ! debug ###
c          write(6,'(   5x,a3,8f10.5)')         ' y0',y0 ! debug ###
          if(np.eq.npoc) return  !!!!!!!!! Done
          np = np+1
        endif
          ENDDO  !### fixing closed planes probl.(or run with small STEP)
        do j=1,7 ! x,y,z,cx,cy,cz,P
          vect(j)=vout(j)
        enddo
      enddo ! Nst 
*==============================
*#include "../RKMSFIT/debug5a.h"
*==============================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine wxy_matr(npo,npl,dx,dy,ipa,R)   !
*-------------------------------------------------
* Preparing Cov.Err.Matrix with Mult.Scattering
* A.Chikanian Feb,2003.
*------------------------------------------------------------
* Missing planes carring by putting dX,dY --> INF.(~22cm)
* COS(Th) - is INDIV. for each planes
* Symmetry of all matrix assumed
*===========================================================
* INP: npo      - # of x,y - hits
*      npl(npo) - array with plane # associated with hits
*    dx,dy(npo) - errors of x,y
*          ipa  - particle ID (Geant)
*            R  - Rigidity (gess)
*===========================================================
      implicit none
*------------------------
#include "rkms.h"
c#include "w8test.h"
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
*-------------------------------
      real dZ(NPma,NPma)
      real wzn(Npma),wzi(Npma)
      integer ipa
      real R,RR,Rmin
      data      Rmin/0.300/ ! GeV

      real xHC
      data xHC/1./ ! Amount of HanyComb mater.in Si-pl.r.l.units  

      integer npo,i,j,k,ii,jj,is,ier,nmspl
      integer ir(10),ifail,jfat
      integer npl(NPma),lw(NPma),mw(NPma)
      real dx(npo),dy(npo)
      real dms(NPma,NPma),ww(NPma,NPma)
      real dTh0,dTc2,oco2(NPma),E,be,P
      integer itr,nw
      real ama,cha,tl,ub
      character*20 pname
c      real det              !## MINV
c      integer ll(10),mm(10) !## MINV

      integer np3
ccc      real w1,wfirst,sq2
*-----------------------
      logical first
      data    first/.true./
      save    first,dz,dTh0,dms,NMSpl !ccc,sq2

      if(first) then
        first=.false.
*========
*========
        do i=1,NPma-1  ! ------------------------------  dZ
          dz(i,i)=0.
          do j=i+1,NPma
            dz(j,i)=trkZ0(j)-trkZ0(i)
            dz(i,j)=dz(j,i)
          enddo
        enddo
        dz(NPma,NPma)=0.
        dTh0 = sqrt(2.)*13.6e-3*sqrt(zz0)*(1.+.038*alog(zz0))! KEEP IT
ccc        sq2=sqrt(1.+xHC)*(1.+0.038*alog(1.+xHC)
ccc     +                    /(1.+0.038*alog((1.+xHC)*zz0)))
        write(6,*)'xHC,sq2',xHC !ccc,sq2 ! debug
      endif ! -------------------- first
 
*========================================= checkimg # of planes > 3 
      if(npo.lt.3) then
        write(6,*)'### wxy_matr: # of planes',npo
        return
      endif
*================================== Initializatiob to 0
      do i=1,NPma*NPma
        dms(i,1)=0.
        ww (i,1)=0.
        wxy(i,1)=0.
        if(i.le.NPma) oco2(i)=0.
      enddo

*================================== MS-matrix building
      if(npl(1).gt.npl(npo)) then
        np3=-1
ccc        wfirst=1.
      else
        np3=1
cccc        wfirst=sq2
ccc        wfirst=1.
      endif
      ii=2
      do i=npl(1)+np3,npl(npo),np3
        jj=ii
        do j=i,npl(npo),np3
          do k=npl(1),i-np3,np3
ccc            w1=1.
ccc            if(k.eq.npl(1)) w1=wfirst
            dms(jj,ii)=dms(jj,ii)+dz(k,i)*dz(k,j) !ccc*w1
          enddo
          jj=jj+1
        enddo
        ii=ii+1
      enddo

*====================================
      call dmshc02(npo,npl,xHC,dZ,dMS) ! Honycomb contrib.in MS-matrix 
c      call dmshc02(npo,npl,xHC) ! ,dZ,dMS) ! debug
*===========================================
      NMSpl = iabs(npl(npo)-npl(1)+np3) ! # Mult.Scatt.planes
      call gfpart(ipa,pname,itr,ama,cha,tl,ub,nw)
      RR=max(R,Rmin)
      P  =  RR*cha
      E  =  sqrt(P**2+ama**2)
      be =  P/E ! or be from TOF
      dTc2 = (dTh0/(be*RR))**2 ! space factor 2 in dTh0
      charge = cha
c      sigcha = 1.D0
c      if(cha.lt.0.) sigcha = -1.D0 ! for GRKUTA 
*=============================
*#include "../RKMSFIT/debug7.h"
*=============================
*     ________________________________ Diagonal elements

      ww(1,1) = dY(1)**2 + dX(1)**2 ! dms(1,1)=0.
      ii=1
      do i=2,npo !_____________________Diagonal elements
        if(iabs(npl(i)-npl(i-1)).gt.1) then
          do is=1,iabs(npl(i)-npl(i-1))-1
            ii=ii+1
            oco2(ii)=1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                                     /dz(npl(i),npl(i-1))**2
            ww(ii,ii)=dms(ii,ii)*dTc2 + 1000. ! dX,Y~22cm if missing plane
          enddo
        endif
        ii=ii+1
        oco2(ii)=1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                               /dz(npl(i),npl(i-1))**2
        ww(ii,ii)=dms(ii,ii)*dTc2*oco2(ii)**2 + dY(i)**2 + dX(i)**2
      enddo
*=============================
*#include "../RKMSFIT/debug8.h"
*=============================
      do i=3,NMSpl !___________________Nondiagonal elements
        do j=2,i-1
          ww(i,j)=dms(i,j)*dTc2*oco2(i)*oco2(j)
c          ww(j,i)=ww(i,j) !  ## MINV
        enddo
      enddo
*=============================
*#include "../RKMSFIT/debug9.h"
*=============================

      call rsinv(NMSpl,ww,NPma,ifail) ! F012 Symm.Matr.Inversion
      if(ifail.ne.0) write(6,*)'### rsinv: ifail=',ifail
c      call minv(ww,NMSpl,det,ll,mm)
*==============================
*#include "../RKMSFIT/debug10.h"
*==============================
*--------------------- Matr.Shrinking
      wxy(1,1)=ww(1,1)
      do j=2,npo
        jj=iabs(npl(j)-npl(1))+1
        do i=j,npo
          wxy(i,j)=ww(iabs(npl(i)-npl(1)+np3),jj)
        enddo
      enddo
*==============================
*#include "../RKMSFIT/debug11.h"
*==============================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine dmshc02(npo,npl,xHC,dZ,zz)
c      subroutine dmshc02(npo,npl,xHC)    ! deb
*-------------------------------------------------
* Calculate contribution from HonyComb in Cov.Err.Matrix
* A.Chikanian Aug,2004.
*===========================================================
* INP: npo      - # of x,y - hits
*      npl(npo) - array with plane # associated with hits
*      xHC      - thicknes of Honycomb (in r.l.of Si)
*       dZ      - matrix dZ(i,j)=Z0(i)-Z0(j)
* OUT:  zz      - Error metrix due to MS (1/2 if filed)
*===========================================================
      implicit none
*------------------------
#include "rkms.h"
c#include "w8test.h"
*-------------------------------------------------------------------
      integer np3,k1,k2,i1,npla,kk,mm,i0
      real zz(NPma,NPma)
*=========
c#include "/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/debug1_zz8.h"
*=========
      real     dZ(NPma,NPma)
c      save     dZ                    ! deb
      integer npl(NPma)
      integer npo,i,j,k,ii,jj
      real xHC ! Honycomb thickness between planes (2-3,4-5,6-7) in Si r.l.
cf         xHC - par. is not necessuary because =1 (but let it be).
cf-- could be usefull in case xHC # 1
cf      real Whc0,Wth(3)! HanyComb Thetta weight (see N.B. p.69)
cf      save Whc0      
cf      logical first
cf      data    first/.true./
cf      save    first
*-------------------------
cf      if(first) then
cf        first=.false.
cf        whc0=    xHC*(1.+0.038*alog(   xHC)/(1.+0.038*alog(zz0)))**2
cf        write(6,*)'=== wHC0',wHC0 ! debug
cf      endif ! first
*------------------------------------------
cf      do i=1,3 ! = (NPma-1)/2
cf        Wth(i)=Whc0
cf      enddo
*=========
c#include "/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/debug2_zz8.h"
*=========

      if(npl(1).lt.npl(npo)) then ! Down
        np3=1
        i0=0
        i1=mod(npl(1),2)
      else                        ! Up
        np3=-1
        i0=1
        i1=mod(npl(1)+1,2)
      endif
      k1=(npl(1)+np3)/2
      npla=iabs(npl(npo)-npl(1))+1

*---------------------------------------------------------------
*    (ik,lj)=dZ(i,k)/sqrt(3)+dZ(l,j)
*    (2k,2k+1 , 2k+1,j) = (dZ(2*k  ,2*k+1)/sqrt(3.)+dZ(2*k+1,i))
*                       w*(dZ(2*k+1,  j  )/sqrt(3.)+dZ(2*k+1,j))
*---------------------------------------------------------------
      do ii=2+i1,npla
        i=npl(1)+(ii-1)*np3
        k2=(i-1)/2+i0  ! 1,2 or 3
c        write(6,*)'ii,i,k1,k2',ii,i,k1,k2          ! debug
        do jj=ii,npla
          j=npl(1)+(jj-1)*np3
c          zz(jj,ii)=0.                             ! debug
c          kkk=0                                    ! debug
          do mm=k1,k2,np3
            k=2*mm+i0
            kk=k+np3
            zz(jj,ii)=zz(jj,ii)+(dZ(k,kk)/sqrt(3.)+dZ(kk,i))
     +                         *(dZ(k,kk)/sqrt(3.)+dZ(kk,j))
cf     +               *Wth(mm)*(dZ(k,kk)/sqrt(3.)+dZ(kk,j))
*=========
c#include "/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/debug3_zz8.h"
*=========
          enddo ! k
c          zz(ii,jj)=zz(jj,ii)
        enddo  ! j
      enddo   ! i
*=========
c#include "/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/debug4_zz8.h"
*=========
      end

*------------ /group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/drkuta.f
* modified by A.Chikanian Aug.26,2003
* EVERYTHING switched to DOUBLE PRECISION
*----------------------------------------------------------------------
CDECK  ID>, GRKUTA. 
*CMZ :  3.21/02 29/03/94  15.41.23  by  S.Giani
*-- Author :
      SUBROUTINE DRKUTA (CHARGE,STEP,VECT,VOUT)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *  Runge-Kutta method for tracking a particle through a magnetic *
C.    *  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of     *
C.    *  Standards, procedure 25.5.20)                                 *
C.    *                                                                *
C.    *  Input parameters                                              *
C.    *       CHARGE    Particle charge                                *
C.    *       STEP      Step size                                      *
C.    *       VECT      Initial co-ords,direction cosines,momentum     *
C.    *  Output parameters                                             *
C.    *       VOUT      Output co-ords,direction cosines,momentum      *
C.    *  User routine called                                           *
C.    *       CALL GUFLD(X,F) (GUFIELD with real*8 param.)             *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GUSWIM                               *
C.    *       Authors    R.Brun, M.Hansroul  *********                 *
C.    *                  V.Perevoztchikov (CUT STEP implementation)    *
C.    *                                                                *
C.    *                                                                *
C.    ******************************************************************
C.
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DOUBLE PRECISION CHARGE, STEP, VECT(*), VOUT(*), F(4)
      DOUBLE PRECISION XYZT(3), XYZ(3), X, Y, Z, XT, YT, ZT
      DOUBLE PRECISION SECXS(4),SECYS(4),SECZS(4),HXP(3)
      EQUIVALENCE (X,XYZ(1)),(Y,XYZ(2)),(Z,XYZ(3)),
     +            (XT,XYZT(1)),(YT,XYZT(2)),(ZT,XYZT(3))
*
      PARAMETER (MAXIT = 1992, MAXCUT = 11)
      PARAMETER (EC=2.9979251D-4,DLT=1D-4,DLT32=DLT/32)
      PARAMETER (ZERO=0, ONE=1, TWO=2, THREE=3)
      PARAMETER (THIRD=ONE/THREE, HALF=ONE/TWO)
      PARAMETER (PISQUA=.986960440109D+01)
      PARAMETER      (IX=1,IY=2,IZ=3,IPX=4,IPY=5,IPZ=6)
*.
*.    ------------------------------------------------------------------
*.
*             This constant is for units CM,GEV/C and KGAUSS
*
      ITER = 0
      NCUT = 0
      DO 10 J=1,7
         VOUT(J)=VECT(J)
   10 CONTINUE
      PINV   = EC * CHARGE / VECT(7)
      TL = 0.
      H      = STEP
*
*
   20 REST  = STEP-TL
      IF (ABS(H).GT.ABS(REST)) H = REST
      CALL DUFLD(VOUT,F)
*
*             Start of integration
*
      X      = VOUT(1)
      Y      = VOUT(2)
      Z      = VOUT(3)
      A      = VOUT(4)
      B      = VOUT(5)
      C      = VOUT(6)
*
      H2     = HALF * H
      H4     = HALF * H2
      PH     = PINV * H
      PH2    = HALF * PH
      SECXS(1) = (B * F(3) - C * F(2)) * PH2
      SECYS(1) = (C * F(1) - A * F(3)) * PH2
      SECZS(1) = (A * F(2) - B * F(1)) * PH2
      ANG2 = (SECXS(1)**2 + SECYS(1)**2 + SECZS(1)**2)
      IF (ANG2.GT.PISQUA) GO TO 40
      DXT    = H2 * A + H4 * SECXS(1)
      DYT    = H2 * B + H4 * SECYS(1)
      DZT    = H2 * C + H4 * SECZS(1)
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
*
*              Second intermediate point
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.H) GO TO 30

      CALL DUFLD(XYZT,F)
      AT     = A + SECXS(1)
      BT     = B + SECYS(1)
      CT     = C + SECZS(1)
*
      SECXS(2) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(2) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(2) = (AT * F(2) - BT * F(1)) * PH2
      AT     = A + SECXS(2)
      BT     = B + SECYS(2)
      CT     = C + SECZS(2)
      SECXS(3) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(3) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(3) = (AT * F(2) - BT * F(1)) * PH2
      DXT    = H * (A + SECXS(3))
      DYT    = H * (B + SECYS(3))
      DZT    = H * (C + SECZS(3))
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
      AT     = A + TWO*SECXS(3)
      BT     = B + TWO*SECYS(3)
      CT     = C + TWO*SECZS(3)
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.2.*ABS(H)) GO TO 30

      CALL DUFLD(XYZT,F)
*
      Z      = Z + (C + (SECZS(1) + SECZS(2) + SECZS(3)) * THIRD) * H
      Y      = Y + (B + (SECYS(1) + SECYS(2) + SECYS(3)) * THIRD) * H
      X      = X + (A + (SECXS(1) + SECXS(2) + SECXS(3)) * THIRD) * H
*
      SECXS(4) = (BT*F(3) - CT*F(2))* PH2
      SECYS(4) = (CT*F(1) - AT*F(3))* PH2
      SECZS(4) = (AT*F(2) - BT*F(1))* PH2
      A      = A+(SECXS(1)+SECXS(4)+TWO * (SECXS(2)+SECXS(3))) * THIRD
      B      = B+(SECYS(1)+SECYS(4)+TWO * (SECYS(2)+SECYS(3))) * THIRD
      C      = C+(SECZS(1)+SECZS(4)+TWO * (SECZS(2)+SECZS(3))) * THIRD
*
      EST    = ABS(SECXS(1)+SECXS(4) - (SECXS(2)+SECXS(3)))
     ++        ABS(SECYS(1)+SECYS(4) - (SECYS(2)+SECYS(3)))
     ++        ABS(SECZS(1)+SECZS(4) - (SECZS(2)+SECZS(3)))
*
      IF (EST.GT.DLT .AND. ABS(H).GT.1.E-4) GO TO 30
      ITER = ITER + 1
      NCUT = 0
*               If too many iterations, go to HELIX
      IF (ITER.GT.MAXIT) GO TO 40
*
      TL = TL + H
      IF (EST.LT.(DLT32)) THEN
         H = H*TWO
      ENDIF
      CBA    = ONE/ SQRT(A*A + B*B + C*C)
      VOUT(1) = X
      VOUT(2) = Y
      VOUT(3) = Z
      VOUT(4) = CBA*A
      VOUT(5) = CBA*B
      VOUT(6) = CBA*C
      REST = STEP - TL
      IF (STEP.LT.0.) REST = -REST
      IF (REST .GT. 1.E-5*ABS(STEP)) GO TO 20
*
      GO TO 999
*
**              CUT STEP
   30 NCUT = NCUT + 1
*               If too many cuts , go to HELIX
      IF (NCUT.GT.MAXCUT)       GO TO 40
      H = H*HALF
      GO TO 20
*
**              ANGLE TOO BIG, USE HELIX
   40 F1  = F(1)
      F2  = F(2)
      F3  = F(3)
      F4  = SQRT(F1**2+F2**2+F3**2)
      RHO = -F4*PINV
      TET = RHO * STEP
      IF(TET.NE.0.) THEN
         HNORM = ONE/F4
         F1 = F1*HNORM
         F2 = F2*HNORM
         F3 = F3*HNORM
*
         HXP(1) = F2*VECT(IPZ) - F3*VECT(IPY)
         HXP(2) = F3*VECT(IPX) - F1*VECT(IPZ)
         HXP(3) = F1*VECT(IPY) - F2*VECT(IPX)

         HP = F1*VECT(IPX) + F2*VECT(IPY) + F3*VECT(IPZ)
*
         RHO1 = ONE/RHO
         SINT = SIN(TET)
         COST = TWO*SIN(HALF*TET)**2
*
         G1 = SINT*RHO1
         G2 = COST*RHO1
         G3 = (TET-SINT) * HP*RHO1
         G4 = -COST
         G5 = SINT
         G6 = COST * HP

         VOUT(IX) = VECT(IX) + (G1*VECT(IPX) + G2*HXP(1) + G3*F1)
         VOUT(IY) = VECT(IY) + (G1*VECT(IPY) + G2*HXP(2) + G3*F2)
         VOUT(IZ) = VECT(IZ) + (G1*VECT(IPZ) + G2*HXP(3) + G3*F3)

         VOUT(IPX) = VECT(IPX) + (G4*VECT(IPX) + G5*HXP(1) + G6*F1)
         VOUT(IPY) = VECT(IPY) + (G4*VECT(IPY) + G5*HXP(2) + G6*F2)
         VOUT(IPZ) = VECT(IPZ) + (G4*VECT(IPZ) + G5*HXP(3) + G6*F3)
*
      ELSE
         VOUT(IX) = VECT(IX) + STEP*VECT(IPX)
         VOUT(IY) = VECT(IY) + STEP*VECT(IPY)
         VOUT(IZ) = VECT(IZ) + STEP*VECT(IPZ)
*
      ENDIF
*
  999 END

*---------1---------2---------3---------4---------5---------6---------7--
      Subroutine DUFLD(xyz,Field)
      implicit none
      double precision xyz(3),Field(3)
      real   x(3),F(3)

      x(1)=xyz(1)
      x(2)=xyz(2)
      x(3)=xyz(3)
      call GUFLD(x,F)
      Field(1)=F(1)
      Field(2)=F(2)
      Field(3)=F(3)
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine R_init(npo,x,y,z,Rc)
*--- calculate Initial Rigidity by circular fit
*----------------------------------------------
* Input:   npo - # points
*          x(npl),y(npl),z(npl) - 3d coordinates
* Output:  Rc  - reconstructed Rigidity by fiting Circle
*-------------------------------------------------------
      implicit none
      integer npo
      integer cfit,utcfit
      real x(npo),y(npo),z(npo)
*-------------------------------------------
#include "rkms.h"
*-------------------------------------
      real gauss,Fu,Bx
      external   Fu
      real Chi2,Rc,sgn,Rad,Rx,Ry,w1(NPma)
      data                       w1/NPma*1./
      real Sl,dSl,AA,dAA,Cor,Ch2L  ! for linear fit

      logical first
      data    first/.true./
      save first,w1,Bx

      if(first) then
        first = .false.
        Bx=gauss(Fu,trkz0(1),trkZ0(NPma),1.e-3)/(trkZ0(NPma)-trkZ0(1))! <Bx>
        write(6,*)'=== R_init: < Bx > =',Bx
      endif ! first

      if(npo.lt.3) return                            !!!!
      cfit = utcfit(z,y,w1,npo,sgn,Rad,Rx,Ry,Chi2)   ! CIRCLE fit
c      cfit = utcfit(z,y,w1,min(npo,4),sgn,Rad,Rx,Ry,Chi2)   ! CIRCLE fit
      if(cfit.ne.0) write(6,*)'### utcfit: cfit.ne.0 !!!'
      Rc = 0.0003*Bx*Rad ! *sgn                      ! ragidity (GeV)
      call LinFit(z,x,w1,npo,Sl,dSl,AA,dAA,Cor,Ch2L) ! Liniar fit
      Rc = abs(Rc)*sqrt(Sl**2+1.)                    ! in space
      end

*--------1---------2---------3---------4---------5---------6---------7--
      Real function Fu(z) ! For integration Mag.Field by Gauss
*     --------------------------------------------------------
      Implicit none
      real x,y,z
      data x/0./,y/0./
      real xyz(3)   ,fld(3)
      data xyz/3*0./,fld/3*0./
      save xyz,fld

      xyz(3)=z
      call Gufld(xyz,fld)
      Fu = fld(1)
      end

*=======================================================================
      Subroutine LinFit(X,Y,SigmaY,Npts,B,SigmaB,A,SigmaA,R,Chi2)
*                       <--- input ---> <------- output ------->
*     A.Chikanian (Yale University)
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*     The least-squares fit to a line ( y = A + B*x ) through points X,Y
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*      The input & output are REAL*4 variables. 
*      This version handle the high precision requirements.
*      All internal calculations used the REAL*8 variables only.
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*   Input:  X, Y   -  1d arrays with points coordinates
*           SigmaY -  1d array with Y errors
*           Npts   -  # of points
*   Output: A      -  intercept
*           SigmaA -  error on intercept
*           B      -  slope
*           SigmaB -  error on slope
*           R      -  correlation coefficient <A,B>/SigmaA/SigmaB
*           Chi2   -  chi square
*        if Chi2 = -2.  number of points with SIGMAY>0. less then 2
*                  -4.  singularity
* ----------------------------------------------------------------------
      Implicit NONE

* input variables
      Integer          Npts
      Real*4           X(Npts),Y(Npts),SigmaY(Npts)
* output variables
      Real*4           A,SigmaA,B,SigmaB,R,Chi2
* internal variables
      Real*8           Sum,SumX,SumY,SumX2,SumXY,Weight,Det
      Integer          Np,I

      Sum   = 0.
      SumX  = 0.
      SumY  = 0.
      SumX2 = 0.
      SumXY = 0.

      Np = 0
      Do I=1,Npts
        If(SigmaY(I).gt.1.E-15) Then
          Np = Np + 1
          Weight = 1./SigmaY(I)**2
          Sum   = Sum   + Weight
          SumX  = SumX  + Weight*X(I)
          SumY  = SumY  + Weight*Y(I)
          SumX2 = SumX2 + Weight*X(I)**2
          SumXY = SumXY + Weight*X(I)*Y(I)
        EndIf
      EndDo

      Chi2 = -2.                       !
      If(Np.le.1) Return               ! not enough points

      Det=Sum*SumX2-SumX**2
      Chi2 = -4.                       !
      if(Det.le.0.) Return             ! singularity checking

      B=(SumXY*Sum -SumY *SumX)/Det    ! Slope
      A=(SumY*SumX2-SumXY*SumX)/Det    ! Intercept
      SigmaB= dsqrt(Sum/Det)           ! Slope Error
      SigmaA= dsqrt(SumX2/Det)         ! Intercept Error
      R     = -SumX/Det/SigmaB/SigmaA  ! correlation coeff.

      Chi2=0.
      do i=1,Npts
        if(SigmaY(i).gt.1.e-15)
     +     Chi2=Chi2+((A+B*X(i)-Y(i))/SigmaY(i))**2
      enddo

      END

*---  /group/yaug1/prj/shik/MYLIB/GENERAL/circle_fit.f
*===========================================================================
* shik: SGN is not realyable !!! (Why?, see next line)
*       ASSUMED ORIGIN OF TRACK in the (0,0), if so no problem with SIG !!!!
*       But still: WHAT DOES mean SGN in case of full circle ?????

* it is probably not right in case dX not eq. dY
* for example Xav = Sum(Wx(i)*X(i))/Sum(Wx(i)),
*     but not Xav = Sum( W(i)*X(i))/Sum( W(i)) !!!!!!!!!!!!!!! 
* unic w(i) is only possible in case dX(i)=dY(i)
*===========================================================================


* $Id: rkms.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
* $Log: rkms.F,v $
* Revision 1.1.1.1  2007/11/13 09:56:10  zuccon
* First Import
*
* Revision 1.5  2006/02/16 13:21:28  choutko
* work around g77 shared
*
* Revision 1.4  2005/11/17 14:29:32  choutko
* correct /dev/null statement
*
* Revision 1.3  2005/09/22 09:09:15  choutko
* :
*
* Revision 1.2  2003/10/30 09:35:02  choutko
* yale algo ++ ( start working)
*
* Revision 1.2  1998/01/27 00:35:54  fisyak
* Split sources
*
      INTEGER FUNCTION UTCFIT(XCIR,YCIR,WCIR,NCIR,
     >                           SGN,R0,ACENT,BCENT,CHISQ)
*>-----------------------------------------------------------------

* UTCFIT - circle fit

* DESCRIPTION:
* Subroutine that fits circle parameters using algorithm
* described by Chernov and Oskov in Computer Physics
* Communications.

* INPUT ARGUMENTS:
* OUTPUT ARGUMENTS:
* AUTHOR:
*<----------------------------------------------------------------
*
* Modified: 13-NOV-1997 by R.Bossingham
*           Eliminate unused variables.

        IMPLICIT DOUBLE PRECISION (A-H , O-Z)
        INTEGER ITRY, NTRY, NCIR, IERR

        REAL    SGN,R0,ACENT,BCENT,CHISQ
        REAL    XCIR(*),YCIR(*),WCIR(*)

        DOUBLE PRECISION  LAMDA,KAPPA,KAPPA1
        COMMON /CIRCOM/ XAV   ,YAV   ,WSUM  ,RSCALE,COSROT,SINROT,
     +                XXAV  ,XYAV  ,YYAV  ,XRRAV ,YRRAV ,RRRRAV,
     +                ALPHA ,BETA  ,KAPPA

        DATA DBL1 / 1.0D0/,DBL2 / 2.0D0/, DBL4/ 4.0D0/, DBL64/6.4D1/
        DATA NTRY / 5/
        Save DBL1,DBL2,DBL4,DBL64,NTRY ! Ch

*-->  FIRST CALCULATE THE H MATRIX AND OTHER THINGS

        UTCFIT = 0
        IERR = 0

        CALL UTCMAT(XCIR,YCIR,WCIR,NCIR)

*-->  USE THESE TO GET THE COEFFICIENTS OF THE 4-TH ORDER POLYNIMIAL
*-->  DON'T PANIC - THE THIRD ORDER TERM IS ZERO !

        XRRXRR= XRRAV  * XRRAV
        YRRYRR= YRRAV  * YRRAV
        RRRRM1= RRRRAV - DBL1
        XXYY  = XXAV   * YYAV

        C0  =          RRRRM1*XXYY - XRRXRR*YYAV - YRRYRR*XXAV
        C1  =        - RRRRM1      + XRRXRR      + YRRYRR   - DBL4*XXYY
        C2  =   DBL4 + RRRRM1                               - DBL4*XXYY
        C4  = - DBL4

*-->  COEFFICIENTS OF THE DERIVATIVE - USED IN NEWTON-RAPHSON ITERATIONS

        C2D =   DBL2 * C2
        C4D =   DBL4 * C4

*-->  0'TH VALUE OF LAMDA - LINEAR INTERPOLATION BETWEEN P(0) & P(YYAV)

CCCC  LAMDA = YYAV * C0 / (C0 + YRRSQ * (XXAV-YYAV))
        LAMDA = 0.0D0
        DLAMDA= 0.0D0

        CHISCL= WSUM * RSCALE**2
        DLAMAX= 0.001 / CHISCL

        DO 30 ITRY = 1,NTRY

            P = C0 + LAMDA * (C1 + LAMDA * (C2 + LAMDA * LAMDA * C4 ))
            PD = (C1 + LAMDA * (C2D + LAMDA * LAMDA * C4D))

            DLAMDA= - P / PD
            LAMDA = LAMDA + DLAMDA

            IF (DABS(DLAMDA).LT.DLAMAX) GO TO 35

   30   CONTINUE

        ITRY = NTRY

   35   CONTINUE

        CHISQ = CHISCL * LAMDA

        DCHISQ= CHISCL * DLAMDA

*-->  NOW CALCULATE THE MATRIX ELEMENTS FOR ALPHA, BETA & KAPPA

        H11   = XXAV  -     LAMDA
        H14   = XRRAV
        H22   = YYAV  -     LAMDA
        H24   = YRRAV
        H34   = DBL1   + DBL2*LAMDA
        IF(H11.EQ.0.0D0.OR.H22.EQ.0.0D0) THEN
           UTCFIT = 1
C          write(6,*) 'Problems in the UTCFIT' ! Ch
           RETURN
        ENDIF

        ROOTSQ= (H14/H11)**2 + DBL4*H34

        IF (DABS(H22).GE.DABS(H24)) THEN
            RATIO = H24/H22
            ROOTSQ=        RATIO**2  + ROOTSQ
            KAPPA = DBL1 / DSQRT(ROOTSQ)
            BETA  = - RATIO * KAPPA
        ELSE
            RATIO = H22/H24
            ROOTSQ= DBL1 + (RATIO**2) * ROOTSQ
            BETA  = DBL1 / DSQRT(ROOTSQ)
            IF (H24   .GT. 0.0) BETA  = - BETA
            KAPPA = - RATIO * BETA
        ENDIF

        ALPHA = - (H14/H11) * KAPPA

*-->  transform these into the lab coordinate system
*-->  first get kappa and back to real dimensions

        KAPPA1 = KAPPA / RSCALE
        DBR0  =   0.5D0 / KAPPA1

*-->  next rotate alpha and beta and scale

        ALPHAR=     (COSROT * ALPHA - SINROT * BETA)*DBR0
        BETAR =     (SINROT * ALPHA + COSROT * BETA)*DBR0

*-->  then translate by (xav,yav)

        ACENT=-( ALPHAR - XAV)
        BCENT=-( BETAR  - YAV)
        R0   = DBR0

        IF(YRRAV .GT. 0) SGN = -1.0
        IF(YRRAV .LT. 0) SGN =  1.0

        RETURN
        END

* $Id: rkms.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
* $Log: rkms.F,v $
* Revision 1.1.1.1  2007/11/13 09:56:10  zuccon
* First Import
*
* Revision 1.5  2006/02/16 13:21:28  choutko
* work around g77 shared
*
* Revision 1.4  2005/11/17 14:29:32  choutko
* correct /dev/null statement
*
* Revision 1.3  2005/09/22 09:09:15  choutko
* :
*
* Revision 1.2  2003/10/30 09:35:02  choutko
* yale algo ++ ( start working)
*
* Revision 1.2  1998/01/27 00:35:54  fisyak
* Split sources
*
      SUBROUTINE UTCMAT(xcir,ycir,wcir,ncir)
*>----------------------------------------------------------------------
* UTCMAT - auxiliary routine for the circle fitting
*<----------------------------------------------------------------------

*      calculates weighted averages needed                    *
*      for circle fitting, shifts, rotates and scales coordi  *
*      nate system so the whole procedure is simpler          *

      IMPLICIT DOUBLE PRECISION   (A-H , O-Z)
      DOUBLE PRECISION   KAPPA
      INTEGER NCIR,I

      COMMON /CIRCOM/ XAV   ,YAV   ,WSUM  ,RSCALE,COSROT,SINROT,
     >                XXAV  ,XYAV  ,YYAV  ,XRRAV ,YRRAV ,RRRRAV,
     >                ALPHA ,BETA  ,KAPPA
      real xcir(*),ycir(*),wcir(*)

*-->  CIRCLH CALCULATES AVERAGES USED IN THE H MATRIX OF THE CIRCLE FIT
*-->  H IS CALCULATED IN THE COORDINATE SYSTEM WHERE :
*-->  <X>    = 0
*-->  <Y>    = 0
*-->  <XY>   = 0
*-->  <R**2> = 1

*-->  IN THIS COORDINATE SYSTEM ONLY 5 ELEMENTS OF H ARE NEITHER 0 OR 1
*-->  THEY ARE : <X**2>, <Y**2>, <X R**2>, <Y R**2> & <R**4>
*-->  <X>  AND <Y>  BEFORE TRANSLATION ARE NEEDED TO GET BACK TO THE LAB
*-->  COSROT & SINROT ARE NEEDED TO GET BACK TO THE LAB
*-->  SIGN(Q) CAN BE DETERMINED WITH THE SIGNS CHOSEN FOR CROT & SINROT
*-->  WSUM   IS REQUIRED TO SCALE THE CHISQUARE
*-->  RSCALE IS REQUIRED TO SCALE KAPPA and CHISQUARE

*-->  FIRST CALCULATE <X> AND <Y> IN THE ORIGINAL CORRDINATE SYSTEM

        WSUM  = 0.0D0

        XAV   = 0.0D0
        YAV   = 0.0D0

        DO 10 I = 1,NCIR

            WI = WCIR(I)
            XI = XCIR(I)
            YI = YCIR(I)

            WSUM = WSUM + WI

            XAV = XAV + WI * XI
            YAV = YAV + WI * YI

   10   CONTINUE

        XAV   = XAV    / WSUM
        YAV   = YAV    / WSUM

*-->  CALCULATE <X**2>, <XY>, AND <Y**2> WITH <X> = 0, & <Y> = 0

        XXAV  = 0.0D0
        XYAV  = 0.0D0
        YYAV  = 0.0D0

        DO 20 I = 1,NCIR

            WI = WCIR(I)
            XI = XCIR(I)
            YI = YCIR(I)

            XI = XI - XAV
            YI = YI - YAV

            WIXI = WI * XI
            WIYI = WI * YI

            XXAV = XXAV + WIXI*XI
            XYAV = XYAV + WIXI*YI
            YYAV = YYAV + WIYI*YI

   20   CONTINUE

        XXAV  = XXAV   / WSUM
        XYAV  = XYAV   / WSUM
        YYAV  = YYAV   / WSUM

*-->  ROTATE COORDINATES SO THAT <XY> = 0

*-->  SIGN(C**2 - S**2) = SIGN(XXAV - YYAV) >
*-->  &                                     > ==> NEW : (XXAV-YYAV) > 0
*-->  SIGN(S) = SIGN(XYAV)                  >

        A     = DABS( XXAV - YYAV )
        B     = 4.0 * XYAV * XYAV

        ASQPB = A**2 + B
        RASQPB= DSQRT(ASQPB)

        SPLUS  = 1.0D0 + A / RASQPB
        SMINUS = B / (ASQPB * SPLUS)

        SPLUS = DSQRT(0.5 * SPLUS )
        SMINUS= DSQRT(0.5 * SMINUS)

*-->  FIRST REQUIRE : SIGN(C**2 - S**2) = SIGN(XXAV - YYAV)

        IF (XXAV .LE.YYAV ) THEN
           COSROT= SMINUS
           SINROT= SPLUS
        ELSE
           COSROT= SPLUS
           SINROT= SMINUS
        ENDIF

*-->  REQUIRE : SIGN(S) = SIGN(XYAV) * SIGN(C) (ASSUMING SIGN(C) > 0)

        IF (XYAV  .LT. 0.0D0) SINROT = - SINROT

*-->  WE NOW HAVE THE SMALLEST ANGLE THAT GUARANTEES <X**2> > <Y**2>
*-->  TO GET THE SIGN OF THE CHARGE RIGHT, THE NEW X-AXIS MUST POINT
*-->  OUTWARD FROM THE ORGIN.  WE ARE FREE TO CHANGE SIGNS OF BOTH
*-->  COSROT AND SINROT SIMULTANEOUSLY TO ACCOMPLISH THIS.

*-->  CHOOSE SIGN OF C WISELY TO BE ABLE TO GET THE SIGN OF THE CHARGE

        IF((COSROT*XAV + SINROT*YAV).GT. 0.0D0) GO TO 35

        COSROT= - COSROT
        SINROT= - SINROT

   35   CONTINUE

*-->  NOW GET <R**2> AND RSCALE= SQRT(<R**2>)

        RRAV  = XXAV  + YYAV
        RSCALE= DSQRT(RRAV)

        XXAV  = 0.0D0
        YYAV  = 0.0D0
        XRRAV = 0.0D0
        YRRAV = 0.0D0
        RRRRAV= 0.0D0
        XYAV  = 0.0D0

        DO 40 I = 1,NCIR

            WI = WCIR(I)
            XOLD = XCIR(I)
            YOLD = YCIR(I)

            XOLD = XOLD - XAV
            YOLD = YOLD - YAV

*-->  ROTATE SO THAT <XY> = 0 & DIVIDE BY RSCALE SO THAT <R**2> = 1

            XI = ( COSROT * XOLD + SINROT * YOLD ) / RSCALE
            YI = ( - SINROT * XOLD + COSROT * YOLD ) / RSCALE

            XIXI = XI * XI
            YIYI = YI * YI
            RIRI = XIXI + YIYI

            WIRIRI= WI * RIRI

            XYAV = XYAV + WI * XI * YI
            XXAV = XXAV + WI * XIXI
            YYAV = YYAV + WI * YIYI

            XRRAV = XRRAV + WIRIRI * XI
            YRRAV = YRRAV + WIRIRI * YI
            RRRRAV= RRRRAV + WIRIRI * RIRI

   40   CONTINUE

*-->  DIVIDE BY WSUM TO MAKE AVERAGES

        XXAV  = XXAV   / WSUM
        YYAV  = YYAV   / WSUM
        XRRAV = XRRAV  / WSUM
        YRRAV = YRRAV  / WSUM
        RRRRAV= RRRRAV / WSUM
        XYAV  = XYAV   / WSUM

        RETURN
        END
