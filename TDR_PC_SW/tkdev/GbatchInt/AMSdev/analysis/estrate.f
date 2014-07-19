*  $Id: estrate.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      subroutine estrate(theta1,phi1,phigr1,phizero1,
     +  pitch1,yaw1,roll1)
*
*     input   
*     phigr1 phi of Greenwich (degree) ( 0 usually)
*     theta1 theta (degree, 90==pole)     
*     pheta1 pheta (degree)     
*     phizero1 orbit phi with theta=0 (degree)
*     pitch1 pitch (degree)
*     yaw1   yaw   (degree)
*     roll1  roll  (degree)
      integer init
      real modul
           real aa(4),zz(4)
           data aa/1.,4.,12.,1./
           data zz/1.,2.,6.,1./
           double precision t,p,sth,cl
           double precision amsxg(3),amsyg(3),amszg(3)
           double precision amsx(3),amsy(3),amsz(3),axis(3)
           double precision c1,c2,c3,s1,s2,s3
           double precision n1,n2,n3,l1,l2,l3,m1,m2,m3
           double precision _dir(3)

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
             phigr=phigr1/180.*3.1415926
             thetam=78.6/180.*3.1415926
             phim=290.6/180.*3.1415926
             theta=theta1/180.*3.1415926
             phi=phi1/180.*3.1415926-phigr
             pitch=pitch1/180.*3.1415926
             yaw=yaw11/180.*3.1415926
             roll=roll1/180.*3.1415926
             phizero=phizero1/180.*3.1415926
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
              ntry=10000 
*
* Magnetic medirian
*

                 uv=vm*wp-wm*vp
                 vv=wm*up-wp*um                      
                 wv=um*vp-vm*up
*
* AMS coordinates
*


                 amszg(1)=up 
                 amszg(2)=vp 
                 amszg(3)=wp 
                 xmir=51.8/180.*3.1415926
                 amsyg(1)=-sin(xmir)*cos(phizero)
                 amsyg(2)=-sin(xmir)*sin(phizero)
                 amsyg(3)=cos(xmir)
                 amsxg(1)=amsyg(2)*amszg(3)-amsyg(3)*amszg(2) 
                 amsxg(2)=amsyg(3)*amszg(1)-amsyg(1)*amszg(3) 
                 amsxg(3)=amsyg(1)*amszg(2)-amsyg(2)*amszg(1) 
                 
                 cp=cos(pitch)  
                 sp=sin(pitch) 
                 cy=cos(yaw) 
                 sy=sin(yaw) 
                 cr=cos(roll) 
                 sr=sin(roll) 
                 l1=cy*cp
                 m1=-sy
                 n1=cy*sp
                 l2=cr*sy*cp-sr*sp
                 m2=cr*cy
                 n2=cr*sy*sp+sr*cp
                 l3=-sr*sy*cp-cr*sp
                 m3=-sr*cy
                 n3=-sr*sy*sp+cr*cp
                 amsx(1)=l1*amsxg(1)+m1*amsyg(1)+n1*amszg(1) 
                 amsx(2)=l1*amsxg(2)+m1*amsyg(2)+n1*amszg(2) 
                 amsx(3)=l1*amsxg(3)+m1*amsyg(3)+n1*amszg(3) 
                 amsy(1)=l2*amsxg(1)+m2*amsyg(1)+n2*amszg(1) 
                 amsy(2)=l2*amsxg(2)+m2*amsyg(2)+n2*amszg(2) 
                 amsy(3)=l2*amsxg(3)+m2*amsyg(3)+n2*amszg(3) 
                 amsz(1)=l3*amsxg(1)+m3*amsyg(1)+n3*amszg(1) 
                 amsz(2)=l3*amsxg(2)+m3*amsyg(2)+n3*amszg(2) 
                 amsz(3)=l3*amsxg(3)+m3*amsyg(3)+n3*amszg(3) 
                 write(*,*)amsx  
                 write(*,*)amsy  
                 write(*,*)amsz  


              do k=1,ntry



*
*  Particle 
*


               the=acos(rndm(d)**0.5)
               phe=twopi*rndm(d)
               if(passok(the,phe).eq.1)then




                 _dir(1)=sin(the)*cos(phe)
                 _dir(2)=sin(the)*sin(phe)
                 _dir(3)=cos(the)
*
* Goto global system from AMS one
*

                 ue=_dir(1)*amsx(1)+_dir(2)*amsy(1)+_dir(3)*amsz(1) 
                 ve=_dir(1)*amsx(2)+_dir(2)*amsy(2)+_dir(3)*amsz(2) 
                 we=_dir(1)*amsx(3)+_dir(2)*amsy(3)+_dir(3)*amsz(3) 




                                 
                 cth=ue*uv+ve*vv+we*wv
                 p=52.5*cl**4/(sqrt(1.d0-cth*cl**3)+1)**2
                 pt=p
                 ee=hrndm1(7000+l*100+3)/1000.
                 xmom=sqrt(ee*(ee+2*0.938))*aa(l)/zz(l)
                 if(xmom.gt.pt)then                                                
                   rate=rate+1+inter*(www(xmom)-1)
                 endif
               endif  
             enddo
             rate=rate/ntry*xsum
             call hf1(7000+l*100+6,rate,1.)
             estrat=estrat+rate*3.1415926**2*0.7**2 
          
             enddo
             write(*,*)'rate ',theta1,phi1,estrat

      end



             function www(xmom)
              if(xmom.lt.20)then
               www=0.77+0.35*xmom-0.66e-2*xmom**2
              else
               www=5.13
              endif
              www=www*0.88
             end


              function passok(theta,phi)
*
*  Cos theta isotropic distribution results
*
*
         implicit double precision (a-z)
         integer i,n
         real  d,rndm,theta,phi,passok
         pi=atan2(0.d0,-1.d0)  ! pi
c         read(*,*) r1,r2,l ! 1st rad 2nd rad and distance btw them
         r1=0.7
         r2=0.7
         l=1.5
         big=1/2.*r1  ! want have some 2 % syst
         n=2e2**2/0.02**2/100   ! want have something  2-3 % stat
c         write(*,*)big,n
         r1=r1**2
         r2=r2**2
*
*  1st plane position center 0,0,0
*
*
*  2nd plane position center 0,0,-l
*                    
       do i=1,1
*
* source on the first plane
*
      
 777    x0=rndm(d)*r1**0.5
        y0=rndm(d)*r1**0.5
        if(x0**2+y0**2.gt.r1)goto 777
        z0=0
*
       nx=cos(phi)*sin(theta)
       ny=sin(phi)*sin(theta)
       nz=cos(theta)   
*
* first plane inters
*
       tg1=z0/abs(nz)   ! hemisphere
       x1=x0+nx*tg1
       y1=y0+ny*tg1
       if(x1**2+y1**2.le.r1)then
       
*
* second plane inters
*

       tg2=(z0+l)/abs(nz) ! hemisphere
       x2=x0+nx*tg2
       y2=y0+ny*tg2
       if(x2**2+y2**2.le.r2.and.y1*y2.lt.0)then
c       if(x2**2+y2**2.le.r2)then
         passok=1
       else 
        passok=0
       endif

      endif
      enddo
       end
