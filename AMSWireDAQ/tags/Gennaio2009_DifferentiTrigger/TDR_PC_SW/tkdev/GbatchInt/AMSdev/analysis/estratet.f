*  $Id: estratet.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      subroutine estratet(phi1,time)
*
*     input   
*     pheta1 pheta (degree)     
*     time in sec from beg of orbit (theta=0) 
*

      integer init
      real modul
           real aa(4),zz(4)
           data aa/1.,4.,12.,1./
           data zz/1.,2.,6.,1./
           double precision t,p,sth,cl

      data init/0/  
      if(init.eq.0)then
        init=1
        modul=350.
        call hbook1(7101 ,'interstellar p',1000,50.,100050.,0.)
        call hbook1(7201 ,'interstellar He',1000,50.,100050.,0.)
        call hbook1(7102 ,'modulated p',1000,50.,100050.,0.)
        call hbook1(7202 ,'modulated He',1000,50.,100050.,0.)
        call hbook1(7103 ,'modulated p',1000,50.,100050.,0.)
        call hbook1(7203 ,'modulated He',1000,50.,100050.,0.)
          call hbook1(7106,'Rate H',100,0.,5000.,0.)
          call hbook1(7206,'Rate He',100,0.,600.,0.)
          pi=3.1415926
          twopi=2*pi
          fguz=1.
          bw=100
          al=50.
          bl=100050.
          do i=1,1000
*
*  Proton
*
           xkin=(i-1)*bw+al+bw/2
           xt=xkin/1000+0.938
           beta=sqrt(1-0.938**2/xt**2)
           xmom=beta*xt
           yy=1.5e4/beta/xmom**2.74/fguz
           call hf1(7101,xkin,yy)
           xt=(xkin+modul)/1000+0.938
           beta=sqrt(1-0.938**2/xt**2)
           xmom=beta*xt
           yy=1.5e4/beta/xmom**2.74/fguz
           yy=yy*(xkin**2+2*938*xkin)/
     +     ((xkin+modul)**2+2*938*(xkin+modul))
           call hf1(7102,xkin,yy)
           call hf1(7103,xkin,yy)
*
*  He
*
           xkin=(i-1)*bw+al+bw/2
           a=4
           am=0.938*a
           amm=am*1000
           z=2
           xk=xkin*a
           xt=xk/1000+am
           beta=sqrt(1-am**2/xt**2)
           xmom=beta*xt/z
           yy=.5e4/beta/xmom**2.68/fguz
           call hf1(7201,xkin,yy)
           xkm=xk+z*modul
           xt=xkm/1000+am
           beta=sqrt(1-am**2/xt**2)
           xmom=beta*xt/z
           yy=.5e4/beta/xmom**2.68/fguz
           yy=yy*(xk**2+2*amm*xk)/
     +     (xkm**2+2*amm*xkm)
           call hf1(7202,xkin,yy)
           call hf1(7203,xkin,yy)
          enddo

      endif
             estrat=0
             thetam=78.6/180.*3.1415926
             phim2=290.6/180.*3.1415926
             theta2=0;
             phi2=phi1/180.*3.1415926
*
* Estimate phi,theta after time sec
*
             twopi=2*3.1415926
             aspeed=twopi/90.8/60
             espeed=twopi/24/60/60
             phim=phim2+espeed*time;
             if(phim>twopi)phim=phim-twopi
             t2=(tan(51.8/180*3.1415926))**2
             philocal=0; 
             philocal=philocal+aspeed*time
             do while(philocal>twopi)
              philocal=philocal-twopi
             enddo
             phi=atan2(sin (philocal),cos(philocal)*sqrt(1+t2))
             theta=atan(t2**0.5*sin(phi))
             phi=phi+phi2
             if(phi>twopi)phi=phi-twopi
             if(phi<0)phi=phi+twopi
             inter=0
             do l=1,2
              xsum=hsum(7000+l*100+2)/10.
              um=sin(pi/2-thetam)*cos(phim)
              vm=sin(pi/2-thetam)*sin(phim)
              wm=cos(pi/2-thetam)
              up=sin(pi/2-theta)*cos(phi)
              vp=sin(pi/2-theta)*sin(phi)                                          
              wp=cos(pi/2-theta)
              cts=um*up+vm*vp+wm*wp
              xl=acos(cts)
              cl=abs(sin(xl))
              rate=0
              ntry=1000 
              do k=1,ntry
               the=acos(rndm(d))
               phe=twopi*rndm(d)
               ue=sin(the)*cos(phe)
               ve=sin(the)*sin(phe)
               we=cos(the)
               uv=vm*wp-wm*vp
               vv=wm*up-wp*um                                                    
               wv=um*vp-vm*up
               cth=ue*uv+ve*vv+we*wv
               p=52.5*cl**4/(sqrt(1.d0-cth*cl**3)+1)**2
               pt=p
               ee=hrndm1(7000+l*100+3)/1000.
               xmom=sqrt(ee*(ee+2*0.938))*aa(l)/zz(l)
              if(xmom.gt.pt)then                                                
               rate=rate+1+inter*(www(xmom)-1)
              endif
             enddo
             rate=rate/ntry*xsum
             call hf1(7000+l*100+6,rate,1.)
             estrat=estrat+rate 
          
             enddo
             write(*,*)'rate ',theta*180/3.141592,phi*180/3.141592,
     +        phim*180/3.1415926,estrat

      end



             function www(xmom)
              if(xmom.lt.20)then
               www=0.77+0.35*xmom-0.66e-2*xmom**2
              else
               www=5.13
              endif
              www=www*0.88
             end
