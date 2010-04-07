      subroutine alignfit(arr,prm,chi2m,algo,what,
     +     xf,xchi2,rigmin,itermin)
      implicit none
      integer nplan,m,algo,what
      integer nlad,nhalf,npar,iter,miter,nmin,maxcases
      parameter(nlad=15)
      parameter (nhalf=2)
      parameter (nplan=8)
      parameter (npar=2)
      integer prm(nplan,*)
#ifdef __CORBA__
      parameter (maxcases=2000)
#else
#ifdef __AMSPARALLEL__
      parameter (maxcases=2500000)
#else
c      parameter (maxcases=300000)
      parameter (maxcases=1000000)
#endif
#endif
      real rig,rigmin
      integer itermin
      real xf(2)
      real xchi2(2,10000)
      real cool(3,nplan,nlad,nhalf),s1,s2,s3
      common /arrcommon/ncases,array,fixpar,cool
      real arr(nplan,*),chi2m
      real array(nplan,7,maxcases)  ! x,y,z,lad,half,ex,ey
      integer fixpar(nplan,nlad,nhalf,6) ! -1 fixed 0 one for all ladders 1 free
      real sigma(nplan,2),z(nplan)
      integer ncases,i,j,k,l,ntot,npt,ialgo
      real out(9),hits(3,nplan),xsigma(3,nplan),normal(3,nplan),
     + outa(7,nplan),res(2,nplan)
      integer layer(nplan),ilayer(nplan),ims,ialg,ifail
      double precision dd(nplan,nlad,nhalf,npar),ch(2)
      double precision ddxx(nplan,nlad,nhalf,npar)
      double precision ddxy(nplan,nlad,nhalf,npar) 
      double precision xnx(nplan,nlad,nhalf,npar)
      double precision xnx2(nplan,nlad,nhalf,npar)
      double precision xny(nplan,nlad,nhalf,npar)
      double precision xynx(nplan,nlad,nhalf,npar)
      double precision xny2(nplan,nlad,nhalf,npar)
      double precision d(2),dy(nplan),d3,cooloc(3),sr,cr,sp,cp,sy,cy
      double precision param(nplan+1,nlad,nhalf,3)
      double precision phi(nplan+1,nlad,nhalf,npar)
      double precision pitch(nplan+1,nlad,nhalf,npar)
      double precision roll(nplan+1,nlad,nhalf,npar)
      double precision aver(nplan,nlad,nhalf,npar)
      double precision averx(nplan,nlad,nhalf,npar)
      double precision averxy(nplan,nlad,nhalf,npar)
      double precision aver2(nplan,nlad,nhalf,npar)
      double precision averz(nplan,nlad,nhalf,npar)
      double precision averxz(nplan,nlad,nhalf,npar)
      double precision mtx(3,3),a1,a2,c,e,parav,nparav
      double precision naver(nplan,nlad,nhalf,npar)
      double precision x,y,xy,x2,y2,nx,a,b,comp,dyz,dz2,dxz
      double precision chi2,chi2a,chi2max,chi2o,tol,dz,dy2,rmax
      common /paramcommon/param,phi,roll,pitch
#ifdef __AMSPARALLEL__
      include "omp_lib.h"
#endif
      if(what.eq.-2)then
        ncases=0
        call vzero(xchi2,2*10000)
        return
      else if(what.eq.0.and.ncases.lt.maxcases)then
         do i=1,nplan
           do j=1,7
            if(arr(i,4).ne.0)then
            if(j.le.3)then
             cool(j,i,arr(i,4),arr(i,5))=arr(i,7+j)
            endif
            if(j.le.6)then
              fixpar(i,arr(i,4),arr(i,5),j)=prm(i,j)
            endif
            endif
            array(i,j,ncases+1)=arr(i,j)
           enddo
         enddo
         ncases=ncases+1
         return
       else if(what.gt.1)then
*
*       get pars back...
*
         do j=what/2,nlad
          do k=mod(what,2)+1,nhalf
          do i=1,nplan
           do l=1,3
             arr(i,l)=param(i,j,k,l)
             arr(i,4)=pitch(i,j,k,1)
             arr(i,5)=phi(i,j,k,2)
             arr(i,6)=roll(i,j,k,1)
             arr(i,7)=j
             arr(i,8)=k
             arr(i,l+8)=cool(l,i,j,k)
           enddo
          enddo
          what=what+1
c          write(*,*)j,k
          return
         enddo
        enddo 
        return
       else if(abs(what).eq.1)then
       else
        return
       endif
      write(*,*)' ncases ',ncases
      chi2max=chi2m
      rmax=0.1
      nmin=20
      iter=0
c      miter=100
      miter=20
       chi2o=10000000
c       tol=2.e-3
       tol=1.e-3
       if(what.eq.1)then
      do i=nplan+1,nplan+1
       do j=1,nlad
         do k=1,nhalf
         do l=1,npar
           param(i,j,k,l)=0
           param(i,j,k,3)=0
           pitch(i,j,k,l)=0
           roll(i,j,k,l)=0
           phi(i,j,k,l)=0
         enddo
        enddo
       enddo
      enddo
      endif
      do i=1,nplan
       do j=1,nlad
         do k=1,nhalf
         do l=1,npar
c           param(i,j,k,l)=0
c           param(i,j,k,3)=0
c           pitch(i,j,k,l)=0
c           roll(i,j,k,l)=0
c           phi(i,j,k,l)=0
           aver(i,j,k,l)=0
           averx(i,j,k,l)=0
           averxy(i,j,k,l)=0
           averz(i,j,k,l)=0
           averxz(i,j,k,l)=0
           aver2(i,j,k,l)=0
           naver(i,j,k,l)=0
           dd(i,j,k,l)=0
           ddxx(i,j,k,l)=0
           ddxy(i,j,k,l)=0
           xnx(i,j,k,l)=0
           xynx(i,j,k,l)=0
           xnx2(i,j,k,l)=0
           xny(i,j,k,l)=0
           xny2(i,j,k,l)=0
         enddo
        enddo
      enddo
      enddo
 10   continue
      chi2a=0
      ntot=0
#ifdef __AMSPARALLEL__
       call omp_set_num_threads(omp_get_num_procs())
c       call omp_set_num_threads(1)
       !$OMP PARALLEL   DEFAULT(NONE) PRIVATE(ims,
     + ialgo,npt,layer,
     +m,normal,j,ilayer,cooloc,sy,cy,cp,sp,cr,sr,mtx,s1,s2,
     +d3,s3,d,k,hits,xsigma,out,outa,x,y,x2,y2,nx,xy,
     +a,b,sigma,res,dy,z,chi2,rig,ch,i,l)
     + SHARED(param,roll,pitch,phi,iter,array,
     +cool,ncases,chi2max,rigmin,
     +itermin,rmax,algo,xchi2) 
     +  shared(dd,ddxx,ddxy,xnx,xnx2,xynx,xny,xny2,
     + naver,averz,averxz,aver,averxy,aver2,ntot,chi2a,averx)
#endif
      !$OMP DO   
      do i=1,ncases
c        do m=1,nplan
c           if(array(m,4,i).ne.0)then
c             z(m)=array(m,3,i)+param(m,array(m,4,i),array(m,5,i),3)
c           endif
c        enddo
        if(mod(algo,2).eq.0)then
              ims=0
              ialgo=1
              do m=1,nplan
               normal(1,m)=0
               normal(2,m)=0
               normal(3,m)=-1
              enddo
              npt=0
              do j=1,nplan     
               ilayer(j)=0
               if(array(j,4,i).ne.0)then
                npt=npt+1
                layer(npt)=j
                ilayer(j)=npt
          cooloc(1)=array(j,1,i)
          cooloc(2)=array(j,2,i)
          cooloc(3)=array(j,3,i)
          sy=sin(phi(j,array(j,4,i),array(j,5,i),2))
          cy=cos(phi(j,array(j,4,i),array(j,5,i),2))
          cp=cos(pitch(j,array(j,4,i),array(j,5,i),1))
          sp=sin(pitch(j,array(j,4,i),array(j,5,i),1))
          cr=cos(roll(j,array(j,4,i),array(j,5,i),1))
          sr=sin(roll(j,array(j,4,i),array(j,5,i),1))
            mtx(1,1)=cy*cp
            mtx(2,1)=-sy
            mtx(3,1)=cy*sp
            mtx(1,2)=cr*sy*cp-sr*sp
            mtx(2,2)=cr*cy
            mtx(3,2)=cr*sy*sp+sr*cp
            mtx(1,3)=-sr*sy*cp-cr*sp
            mtx(2,3)=-sr*cy
            mtx(3,3)=-sr*sy*sp+cr*cp
            s1=array(j,1,i)-cool(1,j,array(j,4,i),array(j,5,i))
            s2=array(j,2,i)-cool(2,j,array(j,4,i),array(j,5,i))
            d3=mtx(1,3)*s1+mtx(2,3)*s2
            s3=array(j,3,i)-cool(3,j,array(j,4,i),array(j,5,i))+d3
            d(1)=mtx(1,1)*s1+mtx(2,1)*s2+mtx(3,1)*s3
            d(2)=mtx(1,2)*s1+mtx(2,2)*s2+mtx(3,2)*s3
                do k=1,2
                 hits(k,npt)=d(k)+cool(k,j,array(j,4,i),array(j,5,i))+
     +           param(j,array(j,4,i),array(j,5,i),k)
                 xsigma(k,npt)=array(j,k+5,i)
                enddo
                 hits(3,npt)=array(j,3,i)+
     +           param(j,array(j,4,i),array(j,5,i),3)+d3
                 xsigma(3,npt)=xsigma(1,npt)
               endif
               enddo
              out(1)=1./1000000.
             call tkfitg(npt,hits,xsigma,normal,5,ialgo,ims,layer,out)
#ifdef  __AMSPARALLEL__
             if(out(7).ne.out(7))then
              write(*,*)' nan ',out
             endif
#endif
             call tkgetres(outa,nplan)
             if(i.le.0)then
              write(*,*)i
              write(*,*)npt
              write(*,*)'hits' ,hits
              write(*,*)'sigma' ,xsigma
              write(*,*)'normal ',normal
              write(*,*)'out ',out
              write(*,*)'outa ',outa
             endif
        endif

       do l=1,2
       x=0
       y=0
       x2=0
       y2=0
       nx=0
       xy=0 
       do j=1,nplan
        if(array(j,4,i).ne.0)then
          cooloc(1)=array(j,1,i)
          cooloc(2)=array(j,2,i)
          cooloc(3)=array(j,3,i)
          sy=sin(phi(j,array(j,4,i),array(j,5,i),2))
          cy=cos(phi(j,array(j,4,i),array(j,5,i),2))
          cp=cos(pitch(j,array(j,4,i),array(j,5,i),1))
          sp=sin(pitch(j,array(j,4,i),array(j,5,i),1))
          cr=cos(roll(j,array(j,4,i),array(j,5,i),1))
          sr=sin(roll(j,array(j,4,i),array(j,5,i),1))
            mtx(1,1)=cy*cp
            mtx(2,1)=-sy
            mtx(3,1)=cy*sp
            mtx(1,2)=cr*sy*cp-sr*sp
            mtx(2,2)=cr*cy
            mtx(3,2)=cr*sy*sp+sr*cp
            mtx(1,3)=-sr*sy*cp-cr*sp
            mtx(2,3)=-sr*cy
            mtx(3,3)=-sr*sy*sp+cr*cp
            s1=array(j,1,i)-cool(1,j,array(j,4,i),array(j,5,i))
            s2=array(j,2,i)-cool(2,j,array(j,4,i),array(j,5,i))
            d3=mtx(1,3)*s1+mtx(2,3)*s2
            s3=array(j,3,i)-cool(3,j,array(j,4,i),array(j,5,i))+d3
          d(1)=mtx(1,1)*s1+mtx(2,1)*s2+mtx(3,1)*s3
          d(2)=mtx(1,2)*s1+mtx(2,2)*s2+mtx(3,2)*s3
            z(j)=array(j,3,i)+param(j,array(j,4,i),array(j,5,i),3)+d3
          dy(j)=d(l)+cool(l,j,array(j,4,i),array(j,5,i))+
     +    param(j,array(j,4,i),array(j,5,i),l)
          y=y+dy(j)
          xy=xy+dy(j)*z(j)
          y2=y2+dy(j)**2
          x=x+z(j)
          nx=nx+1
          x2=x2+z(j)**2
        endif
       enddo
        if(nx.gt.nplan-3)then
         chi2=0
         x=x/nx
         y=y/nx
         xy=xy/nx
         x2=x2/nx
         y2=y2/nx
         x2=(x2-x*x)
         a=(xy-x*y)/x2
         b=y-a*x
         do j=1,nplan
          sigma(j,l)=array(j,5+l,i)
            if(mod(algo,2).eq.1)then
             res(l,j)=(dy(j)-b-a*z(j)) 
            else
             res(1,j)=-(outa(1,ilayer(j))-hits(1,ilayer(j)))
             res(2,j)=-(outa(2,ilayer(j))-hits(2,ilayer(j)))
            endif
          if(array(j,4,i).ne.0)then
           chi2=chi2+(res(l,j))**2/sigma(j,l)**2
          endif
         enddo
         rig=10000
         if(mod(algo,2).eq.0)then
           chi2=out(7)
           rig=abs(out(6))
          ch(1)=chi2
          ch(2)=chi2
        else
         ch(l)=chi2
        endif
c          if(i.le.1)write(*,*)'chi2 ',i,iter,chi2
c        if(i.le.10)then
c          write(*,*)'chi2 ',i,iter,chi2
c        endif
        if(l.eq.2.and.i.le.10000)then
          if(iter.eq.0)then
            xchi2(1,i)=ch(1)+ch(2)
         endif
            xchi2(2,i)=ch(1)+ch(2)
        endif
        if((chi2.lt.chi2max.and.rig.gt.rigmin).or.iter.lt.itermin)then
       !$OMP  CRITICAL
         do j=1,nplan
          if(mod(algo,2).eq.0)then
             a=outa(3+l,ilayer(j))/outa(6,ilayer(j))
          endif
          if(array(j,4,i).ne.0.and.abs(res(l,j)).lt.rmax)then
         dd(j,array(j,4,i),array(j,5,i),l)=
     +        dd(j,array(j,4,i),array(j,5,i),l)+res(l,j)
         ddxx(j,array(j,4,i),array(j,5,i),l)=
     +        ddxx(j,array(j,4,i),array(j,5,i),l)+
     +       (res(l,j))*a*array(j,l,i)
         ddxy(j,array(j,4,i),array(j,5,i),l)=
     +        ddxy(j,array(j,4,i),array(j,5,i),l)+
     +       (res(l,j))*a*array(j,3-l,i) 
         xnx(j,array(j,4,i),array(j,5,i),l)=
     +        xnx(j,array(j,4,i),array(j,5,i),l)+(a*array(j,l,i))
         xnx2(j,array(j,4,i),array(j,5,i),l)=
     +        xnx2(j,array(j,4,i),array(j,5,i),l)+(a*array(j,l,i))**2
        xynx(j,array(j,4,i),array(j,5,i),l)=
     +        xynx(j,array(j,4,i),array(j,5,i),l)+(a*array(j,l,i))*
     +        +(a*array(j,3-l,i))
         xny(j,array(j,4,i),array(j,5,i),l)=
     +        xny(j,array(j,4,i),array(j,5,i),l)+(a*array(j,3-l,i))
         xny2(j,array(j,4,i),array(j,5,i),l)=
     +        xny2(j,array(j,4,i),array(j,5,i),l)+(a*array(j,3-l,i))**2


         naver(j,array(j,4,i),array(j,5,i),l)=
     +   naver(j,array(j,4,i),array(j,5,i),l)+1

         averz(j,array(j,4,i),array(j,5,i),1)=
     +   averz(j,array(j,4,i),array(j,5,i),1)+a

         averz(j,array(j,4,i),array(j,5,i),2)=
     +   averz(j,array(j,4,i),array(j,5,i),2)+a*a

         averxz(j,array(j,4,i),array(j,5,i),1)=
     +   averxz(j,array(j,4,i),array(j,5,i),1)+a*res(l,j)

         averxz(j,array(j,4,i),array(j,5,i),2)=
     +   averxz(j,array(j,4,i),array(j,5,i),2)+res(l,j)
 
         aver(j,array(j,4,i),array(j,5,i),l)=
     +   aver(j,array(j,4,i),array(j,5,i),l)+res(l,j)
 
         averx(j,array(j,4,i),array(j,5,i),l)=
     +   averx(j,array(j,4,i),array(j,5,i),l)+array(j,3-l,i)

         averxy(j,array(j,4,i),array(j,5,i),l)=
     +   averxy(j,array(j,4,i),array(j,5,i),l)+(res(l,j))*
     +   array(j,3-l,i)

         aver2(j,array(j,4,i),array(j,5,i),l)=
     +   aver2(j,array(j,4,i),array(j,5,i),l)+array(j,3-l,i)**2

         endif
         enddo
         ntot=ntot+1
         chi2a=chi2a+chi2
       !$OMP END CRITICAL
         endif
         endif
c           write(*,*)'BOKE',l,chi2,chi2a,chi2a/ntot,ntot
c         if(mod(ntot,100).eq.1)then
c           write(*,*)'l ',l,chi2,chi2a,chi2a/ntot,ntot
c         endif
      enddo 
      enddo
      !$OMP END DO 
      !$OMP END PARALLEL
      chi2a=chi2a/ntot
      write(*,*)'iter ',iter,chi2a,ntot,nmin
      do i=1,nplan
       do j=1,nlad
         do k=1,nhalf
         do l=1,npar
            if(naver(i,j,k,l).gt.nmin)then
           dd(i,j,k,l)=dd(i,j,k,l)/naver(i,j,k,l)
           ddxx(i,j,k,l)=ddxx(i,j,k,l)/naver(i,j,k,l)
           ddxy(i,j,k,l)=ddxy(i,j,k,l)/naver(i,j,k,l)
           xnx(i,j,k,l)=xnx(i,j,k,l)/naver(i,j,k,l)
           xnx2(i,j,k,l)=xnx2(i,j,k,l)/naver(i,j,k,l)
           xynx(i,j,k,l)=xynx(i,j,k,l)/naver(i,j,k,l)
           xny(i,j,k,l)=xny(i,j,k,l)/naver(i,j,k,l)
           xny2(i,j,k,l)=xny2(i,j,k,l)/naver(i,j,k,l)
            aver(i,j,k,l)=aver(i,j,k,l)/naver(i,j,k,l)
            averx(i,j,k,l)=averx(i,j,k,l)/naver(i,j,k,l)
            averxy(i,j,k,l)=averxy(i,j,k,l)/naver(i,j,k,l)
            averz(i,j,k,l)=averz(i,j,k,l)/
     +      (naver(i,j,k,1)+naver(i,j,k,2))
            averxz(i,j,k,l)=averxz(i,j,k,l)/
     +      (naver(i,j,k,1)+naver(i,j,k,2))
            aver2(i,j,k,l)=aver2(i,j,k,l)/naver(i,j,k,l)
            param(i,j,k,l)=param(i,j,k,l)-aver(i,j,k,l)

c           if (i.eq.1) then
c           write(*,*)'BOKE',i,j,k,l,
c     +          averxz(i,j,k,l),aver2(i,j,k,l),
c     +          param(i,j,k,l)
c           endif

            if(l.eq.2)then
            phi(i,j,k,2)=phi(i,j,k,2)-
     +      atan((averxy(i,j,k,2)-aver(i,j,k,2)*
     +      averx(i,j,k,2)-averxy(i,j,k,1)+aver(i,j,k,1)*
     +      averx(i,j,k,1))/(aver2(i,j,k,2)-averx(i,j,k,2)**2+
     +      aver2(i,j,k,1)-averx(i,j,k,1)**2))

c           if (i.eq.1) then
c           write(*,*)'BOKE',i,j,k,l,phi(i,j,k,2)
c           endif

            endif
            if(fixpar(i,j,k,l).eq.0)then
              param(i,j,k,l)=0
            endif
            if(iter.eq.0)then
c            write(*,*)'blia ',i,j,k,l,naver(i,j,k,l),param(i,j,k,l),
c     +       phi(i,j,k,l)
           endif
           endif
c           aver(i,j,k,l)=0
c           averxy(i,j,k,l)=0
c           averx(i,j,k,l)=0
c           aver2(i,j,k,l)=0
c           naver(i,j,k,l)=0
         enddo
            if(naver(i,j,k,1).gt.nmin)then
           a1=ddxx(i,j,k,1)-dd(i,j,k,1)*xnx(i,j,k,1)+
     +        ddxy(i,j,k,2)-dd(i,j,k,2)*xny(i,j,k,2)
           a2=ddxx(i,j,k,2)-dd(i,j,k,2)*xnx(i,j,k,2)+
     +        ddxy(i,j,k,1)-dd(i,j,k,1)*xny(i,j,k,1)
           a=xnx2(i,j,k,1)-xnx(i,j,k,1)**2+
     +       xny2(i,j,k,2)-xny(i,j,k,2)**2
           b=xynx(i,j,k,1)-xnx(i,j,k,1)*xny(i,j,k,1)+
     +       xynx(i,j,k,2)-xnx(i,j,k,2)*xny(i,j,k,2)
           c=b
           e=xnx2(i,j,k,2)-xnx(i,j,k,2)**2+
     +       xny2(i,j,k,1)-xny(i,j,k,1)**2
          roll(i,j,k,1)=roll(i,j,k,1)-(a*a2-c*a1)/(a*e-b*c)
           pitch(i,j,k,1)=pitch(i,j,k,1)-(a1*e-a2*b)/(a*e-b*c)
           param(i,j,k,3)=param(i,j,k,3)+(averxz(i,j,k,1)-
     +     averxz(i,j,k,2)*averz(i,j,k,1))/
     +     (averz(i,j,k,2)-averz(i,j,k,1)**2)
          endif
           if(fixpar(i,j,k,3).eq.0)then
              param(i,j,k,3)=0
           endif
           if(fixpar(i,j,k,4).eq.0)then
             pitch(i,j,k,1)=0
           endif
         if(fixpar(i,j,k,5).eq.0)then
             phi(i,j,k,1)=0
              phi(i,j,k,2)=0
           endif
         if(fixpar(i,j,k,6).eq.0)then
             roll(i,j,k,1)=0
           endif

         if(iter.eq.0)then
            l=2
            if (averz(i,j,k,l).ne.0) then
c            write(*,*)'blia3 ',i,j,k,param(i,j,k,l), 
c     +     averz(i,j,k,2),averz(i,j,k,1)
         endif
           endif
           averz(i,j,k,1)=0
           averz(i,j,k,2)=0
           averxz(i,j,k,1)=0
           averxz(i,j,k,2)=0
           do l=1,2
           dd(i,j,k,l)=0
           ddxx(i,j,k,l)=0
           ddxy(i,j,k,l)=0
           xnx(i,j,k,l)=0
           xnx2(i,j,k,l)=0
           xynx(i,j,k,l)=0
           xny(i,j,k,l)=0
           xny2(i,j,k,l)=0
           enddo
        enddo
      enddo
         do l=1,6
         parav=0
         nparav=0
       do j=1,nlad
         do k=1,nhalf
           if(fixpar(i,j,k,l).eq.-1)then
             if(l.le.2)then
               parav=parav+param(i,j,k,l)*naver(i,j,k,l)
               nparav=nparav+naver(i,j,k,l)
             else if(l.eq.3)then
               parav=parav+param(i,j,k,3)*
     +         (naver(i,j,k,1)+naver(i,j,k,2))
               nparav=nparav+(naver(i,j,k,1)+naver(i,j,k,2))
             else if(l.eq.4)then
               parav=parav+pitch(i,j,k,1)*naver(i,j,k,1)
               nparav=nparav+naver(i,j,k,1)
             else if(l.eq.5)then
               parav=parav+phi(i,j,k,2)*naver(i,j,k,2)
               nparav=nparav+naver(i,j,k,2)
             else if(l.eq.6)then
               parav=parav+roll(i,j,k,1)*naver(i,j,k,1)
               nparav=nparav+naver(i,j,k,1)
             endif    
          endif
         enddo
        enddo
       do j=1,nlad
         do k=1,nhalf
           if(fixpar(i,j,k,l).eq.-1)then
             if(l.le.3)then
               param(i,j,k,l)=parav/(nparav+1)
c                 write(*,*)i,l,parav/(nparav+1),nparav
             else if(l.eq.4)then
               pitch(i,j,k,1)=parav/(nparav+1)
             else if(l.eq.5)then
               phi(i,j,k,2)=parav/(nparav+1)
             else if(l.eq.6)then
               roll(i,j,k,1)=parav/(nparav+1)
             endif
           endif
         enddo
        enddo  
       enddo 
        do j=1,nlad
         do k=1,nhalf
         do l=1,npar
          aver(i,j,k,l)=0
           averxy(i,j,k,l)=0
           averx(i,j,k,l)=0
           aver2(i,j,k,l)=0
           naver(i,j,k,l)=0
         enddo
        enddo
       enddo 
     
      enddo
      if(iter.eq.0)then
        xf(1)=chi2a
      endif
        xf(2)=chi2a
      if(abs(chi2a-chi2o).gt.tol.and.iter.lt.miter.and.
     +  chi2a.le.chi2a)then
       iter=iter+1
       chi2o=chi2a
       goto 10
      endif
      do i=1,nplan
       do j=1,nlad
         do k=1,nhalf
         do l=1,npar
c            write(*,*)i,j,k,l,param(i,j,k,l),phi(i,j,k,l)
         enddo
        enddo
      enddo
      enddo
      end                        
