      function ajustp(x)
      common/pawpar/par(12)
*
      parameter(nphmx=8)           ! maximo numero de fotones que 
*                                  ! contribuiran a nuestra distribucion
      real cp(0:nphmx)          
      real cv(0:nphmx,0:nphmx)
      real f(0:nphmx,0:nphmx)
      real ajust(0:nphmx)
*
      real arl(0:nphmx,0:nphmx)
      real ars(0:nphmx,0:nphmx)
      common/mypar/arl,ars
*
      data rl1old/0./,rs1old/0./,rlvold/0./,rsvold/0./
      data nupd/0/

      data ini/0/,poisson/0./
*
* ADC distribution
*   par(1)  -> Number of events
*   par(2)  -> Average number of photons per event (poisson)
* Pedestal characterization: pdf: (1-f)*G1+f*G2
*   par(3)  -> f: fraction of gaussians
*   par(4)  -> G1: pedestal (adc)
*   par(5)  -> G1: sigma pedestal (adc)
*   par(6)  -> G2: pedestal (adc)
*   par(7)  -> G2: sigma pedestal (adc)
* Single Photoelectron response: pdf: (1-f)*F1+f*F_low
*   par(8)  -> f: fraction of low gain response (transmitance?)
*   par(9)  -> F1: Lambda1
*   par(10) -> F1: S1
*   par(11) -> F_low: Lambda_low
*   par(12) -> F_low: S_Low
*
* Initialization ****************************************************
*
      pi=2.*acos(0.)
      do i=1,12
        par(i)=abs(par(i))
      enddo
*
      totsta=par(1)
      avph  =par(2)
      r21p  =par(3)
      ped   =par(4)
      sgped =par(5)
      ped2  =par(6)
      sgped2=par(7)
      t     =par(8)
      rl1   =par(9)
      rs1   =par(10)
      rlv   =par(11)
      rsv   =par(12)
****  x0f   =par(13)
      x0f=0.
*
* Escogemos el numero de fotones que contribuyen segun sea avph:
*
      if(ini.eq.0) then
         ini=1
         nphtbu=4                ! ajuste como minimo con 4 fotones
         do i=4,nphmx
*         print*,'i=',i
          poisson=(exp(-avph)*(avph)**i)/fact(i)
          if(poisson.lt.0.0001) then
             nphtbu=i
             goto 10
          else
             nphtbu=nphmx        ! ajuste como maximo con nphmx fotones
          endif
*         print*,'# Number of photons to be used = ',nphtbu
*         print*,'# Poisson probability = ',poisson
         enddo
10       continue 
         print*,'################################################'
         print*,'# Poisson probability = ',poisson,'         #'
         print*,'################################################'
         print*,'# Number of photons to be used = ',nphtbu,'    #'
         print*,'################################################'
      endif
*
* compute and store rl.rs for each (iph,nv) combination
*
      if(rl1.ne.rl1old.or.rs1.ne.rs1old
     >.or.rlv.ne.rlvold.or.rsv.ne.rsvold) then
        nupd=nupd+1

        rl=rl1
        rs=rs1
        call getrmursg(rl,rs,av,sg)
        g1=av
        sg1=sg

        rl=rlv
        rs=rsv
        call getrmursg(rl,rs,av,sg)
        gv=av
        sgv=sg

        do iph=1,nphtbu
          do nv=0,iph
            nph=iph-nv
            if(g1.gt.0..and.sg1.gt.0..and.gv.gt.0..and.sgv.gt.0.) then

              av=nph*g1+nv*gv
              sg=sqrt(nph*sg1**2+nv*sgv**2)
              call getrlrs(av,sg,rl,rs)
              arl(iph,nv)=rl
              ars(iph,nv)=rs
      
            else
              arl(iph,nv)=-1.
              ars(iph,nv)=-1.
            endif
          enddo
        enddo
        rl1old=rl1
        rs1old=rs1
        rlvold=rlv
        rsvold=rsv
      endif
*
* Coeficients *******************************************************
*
      x0=ped
*
      do iph=0,nphtbu
        cp(iph)=exp(-avph)*avph**iph/fact(iph)
        do nv=0,iph
          nph=iph-nv
          cv(iph,nv)=(fact(iph)/fact(nv)/fact(nph))*t**nv*(1.-t)**nph
        enddo
      enddo
*
* Functions   ********************************************************
*
* iph=0 (Pedestals -> Gauss)
*
      av=ped
      sg=sgped
      f01=exp(-0.5*((x-av)/sg)**2)/sqrt(2.*pi)/sg
      av=ped2
      sg=sgped2
      f02=exp(-0.5*((x-av)/sg)**2)/sqrt(2.*pi)/sg

      f(0,0)=(1.-r21p)*f01+r21p*f02
*
* iph>0 (PMT response -> Poisson)
*
      xp=x-ped+x0f
      do iph=1,nphtbu
        do nv=0,iph
          nph=iph-nv
          rl= arl(iph,nv)
          rs= ars(iph,nv)
          f(iph,nv)=Pdens(xp,rl,rs)
        enddo
      enddo
*
* PDF value **********************************************************
*
      ajustot=0.
      do iph=0,nphtbu
        ajust(iph)=0.
        do nv=0,iph
          nph=iph-nv
          ajust(iph)=ajust(iph)+cv(iph,nv)*f(iph,nv)
        enddo
        ajust(iph)=cp(iph)*ajust(iph)
        ajustot=ajustot+ajust(iph)
      enddo

      ajustp = totsta*ajustot

      end
*
*
*
      function fact(n)
      integer n
      fact=1.
      if(n.gt.0) then
        do i=1,n
          fact=fact*i
        enddo
      endif
      end
*
*
*
**********************************************************
*     Normalised probability density p(x | rl,rs)
*       input: rl,rs    (lambda,S free parameters)
*                        S>0,   
*                        lambda>0.1 due to normalisation approx.
*              x         >0   
*---------------------------------------------------------
      function Pdens(x,rl,rs)
      dimension a(4),b(4),c(4),rcutl(4)
      data a/  1.000000,  1.003300,  0.882630,  0.838100/
      data b/ -0.250000, -0.315530, -0.047686,  0.035362/
      data c/  0.500000,  0.500000,  0.581300,  0.636520/
      data rcutl/ 10., 2., 0.7, 0.1/

      Pdens = 0.
      if(x.le.0..or.rl.lt.0..or.rs.lt.0.) goto 999

      ind = 0
      do i=4,1,-1
        if(rl.gt.rcutl(i)) ind = i
      end do

      if(ind.gt.0.) then
        fnorm0= a(ind)*(rl+b(ind))**c(ind)
        fnorm  = exp(rl)*rs*fnorm0
        w  = x/rs
        Fw = w*alog(rl) - algamma(w) - 0.5*alog(w)
        Pdens = exp(fw)/fnorm
      endif

999   end
*
*
*
**********************************************************
* Gamma function -----------------------------------------
      function algamma(x)
      data twopi/6.283185307178/

      if(x.le.20.) then
        algamma = alog(gamma(x))
      else
        w = x - 1.
        f2 = 1. + 1./(12.*w) + 1./(288.*w**2)
        f1 = w*alog(w) - w + 0.5*alog(twopi*w) + alog(f2)
        algamma = f1
      end if
      end
*
*
*
**********************************************************
*     Get      rmu, rsg  (output: mu,sigma)
*     given     rl, rs   (input:  lambda,s)
*---------------------------------------------------------
      subroutine getrmursg(rl,rs,rmu,rsg)
      dimension am(6),bm(6),cm(6),as(6),bs(6),cs(6)
      dimension rlcut(6)
      data am/ 1.00020, 1.00040, 0.980390, 0.927950, 1.26620, 1.47110/
      data bm/ 0.50781, 0.51928, 0.588250, 0.720830, 0.34056, 0.11002/
      data cm/ 0.99993, 0.99961, 1.005400, 1.023800, 0.82253, 0.59419/
      data as/ 1.00120, 1.00180, 1.002190, 0.965210, 0.90980, 0.95240/
      data bs/-0.02468,-0.03665,-0.051227, 0.046977, 0.18242, 0.06553/
      data cs/ 0.49976, 0.49967, 0.499870, 0.515730, 0.54253, 0.44432/
      data rlcut/ 15.0,    10.0,      7.5,      1.0,     0.5,     0.1/

        rmu = -1.
        rsg = -1.

        do i=6,1,-1
          if(rl.gt.rlcut(i)) then
            rmu0 = rl+bm(i)
            rsg0 = rl+bs(i)
            if(rmu0.lt.0. .or. rsg0.lt.0.) go to 1000
            rmu  = rs*am(i)*rmu0**cm(i)
            rsg  = rs*as(i)*rsg0**cs(i)
 1000     end if
        end do

      end

**********************************************************
*     GET       rl, rs   (output: lambda,s)
*     given    rmu, rsg  (input:  mu,sigma)
*---------------------------------------------------------
      subroutine getrlrs(rmu,rsg,rl,rs)
      dimension am(6),bm(6),cm(6),as(6),bs(6),cs(6)
      dimension rlcut(6)
      data am/ 1.00020, 1.00040, 0.980390, 0.927950, 1.26620, 1.47110/
      data bm/ 0.50781, 0.51928, 0.588250, 0.720830, 0.34056, 0.11002/
      data cm/ 0.99993, 0.99961, 1.005400, 1.023800, 0.82253, 0.59419/
      data as/ 1.00120, 1.00180, 1.002190, 0.965210, 0.90980, 0.95240/
      data bs/-0.02468,-0.03665,-0.051227, 0.046977, 0.18242, 0.06553/
      data cs/ 0.49976, 0.49967, 0.499870, 0.515730, 0.54253, 0.44432/
      data rlcut/ 15.0,    10.0,      7.5,      1.0,     0.5,     0.1/
      data   acc/ 0.001/

        rl = -1.
        rs = -1.

        step = 0.1
        x = rlcut(6) - step

        f00   = (as(6)*rmu)/(am(6)*rsg)
        fv0   = (x+bm(6))**cm(6)/(x+bs(6))**cs(6)
        ftst0 = fv0 - f00

 1000   x = x + step
        if(x.gt.50.) go to 1100
        do i=6,1,-1
          if(x.ge.rlcut(i)) k=i
        end do
        f0   = (as(k)*rmu)/(am(k)*rsg)
        fv   = (x+bm(k))**cm(k)/(x+bs(k))**cs(k)
        ftst = fv - f0
        if(ftst*ftst0.lt.0.) then
          if(step.lt.acc) then
             rl = x - step/2.
             go to 1100
          end if
          x    = x - step
          step = step/10.
        end if
        go to 1000
 1100   if(rl.ge.rlcut(6)) then
          rs = rmu/(am(k)*(rl+bm(k))**cm(k))
        end if
 
        return
        end
