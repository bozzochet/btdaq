*  $Id: mfield.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      subroutine readmfield
#      include "mfield.h"
       real hxy(3,3),x1(3)
      if(iniok.eq.0)then
*       
          write(*,*)'Opening field file...'
c          OPEN(21,FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
          OPEN(21,FILE=amsdatadir(1:amsdlength)//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.
          do k=1,nz
           do i=1,nx
            do j=1,ny
             read(21,*)x(i),y(j),z(k),bx(i,j,k),by(i,j,k),bz(i,j,k)
             x(i)=x(i)
             y(j)=y(j)
             z(k)=z(k)
             bx(i,j,k)=bx(i,j,k)/1000. *fac
             by(i,j,k)=by(i,j,k)/1000. *fac
             bz(i,j,k)=bz(i,j,k)/1000. *fac
            enddo
           enddo
          enddo
          na(1)=nx
          na(2)=ny
          na(3)=nz
          call ucopy(x(1),xyz(1),nx)
          call ucopy(y(1),xyz(1+nx),ny)
          call ucopy(z(1),xyz(1+nx+ny),nz)
*         read(21,*)isec
*         read(21,*)imin
*         read(21,*)ihour
*         read(21,*)iday
*         read(21,*)imon
*         read(21,*)iyear
          iniok=1
          close(21)
          write(*,*)'Closing field file...'
*
* Now calculate field derivatives
*
          call timex(st0)
          write(*,*)'Calculating field derivatives...'
          do i=1,nx
           do j=1,ny
             do k=1,nz
              x1(1)=x(i)
              x1(2)=y(j)
              x1(3)=z(k)
              call tkfldc(x1,hxy)
              bdx(i,j,k,1)=hxy(1,1)
              bdy(i,j,k,1)=hxy(2,1)
              bdz(i,j,k,1)=hxy(3,1)
              bdx(i,j,k,2)=hxy(1,2)
              bdy(i,j,k,2)=hxy(2,2)
              bdz(i,j,k,2)=hxy(3,2)
              brx=bx(i,j,k)*hxy(1,1)+by(i,j,k)*hxy(1,2)+
     +        bz(i,j,k)*hxy(1,3)
              bry=bx(i,j,k)*hxy(2,1)+by(i,j,k)*hxy(2,2)+
     +        bz(i,j,k)*hxy(2,3)
              brz=bx(i,j,k)*hxy(3,1)+by(i,j,k)*hxy(3,2)+
     +        bz(i,j,k)*hxy(3,3)
              brxm=hxy(1,1)**2+hxy(1,2)**2+hxy(1,3)**2
              brym=hxy(2,1)**2+hxy(2,2)**2+hxy(2,3)**2
              brzm=hxy(3,1)**2+hxy(3,2)**2+hxy(3,3)**2
              bb=sqrt(bx(i,j,k)**2+by(i,j,k)**2+bz(i,j,k)**2)
              brm=sqrt(brxm+brym+brzm)
              if(bb.gt.0)then
                br=sqrt(brx**2+bry**2+brz**2)/bb
              else
                br=0
              endif
              write(31,*)x1,bx(i,j,k),by(i,j,k),bz(i,j,k),br,brm
             enddo
           enddo
          enddo
          call timex(st1)
          write(*,*)'Finished in ',st1-st0, ' sec'
c          call rmf2
      else if(iniok.eq.-1)then

*
*  special procedure for 2L  Permanent Magnet
*
*       
          xlfac=2
          write(*,*)'Special ...  Opening field file...'
c          OPEN(21,FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
          OPEN(21,FILE=amsdatadir(1:amsdlength)//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.1
          do k=1,nz
           do i=1,nx
            do j=1,ny
             read(21,*)x(i),y(j),z(k),bx(i,j,k),by(i,j,k),bz(i,j,k)
             x(i)=x(i)
             y(j)=y(j)
             z(k)=z(k)*xlfac
             bx(i,j,k)=bx(i,j,k)/1000. *fac
             by(i,j,k)=by(i,j,k)/1000. *fac
             bz(i,j,k)=bz(i,j,k)/1000. *fac
            enddo
           enddo
          enddo
          na(1)=nx
          na(2)=ny
          na(3)=nz
          call ucopy(x(1),xyz(1),nx)
          call ucopy(y(1),xyz(1+nx),ny)
          call ucopy(z(1),xyz(1+nx+ny),nz)
*         read(21,*)isec
*         read(21,*)imin
*         read(21,*)ihour
*         read(21,*)iday
*         read(21,*)imon
*         read(21,*)iyear
          iniok=1
          close(21)
          write(*,*)'Closing field file...'
*
* Now calculate field derivatives
*
          call timex(st0)
          write(*,*)'Calculating field derivatives...'
          do i=1,nx
           do j=1,ny
             do k=1,nz
              x1(1)=x(i)
              x1(2)=y(j)
              x1(3)=z(k)
              call tkfldc(x1,hxy)
              bdx(i,j,k,1)=hxy(1,1)
              bdy(i,j,k,1)=hxy(2,1)
              bdz(i,j,k,1)=hxy(3,1)
              bdx(i,j,k,2)=hxy(1,2)
              bdy(i,j,k,2)=hxy(2,2)
              bdz(i,j,k,2)=hxy(3,2)
              brx=bx(i,j,k)*hxy(1,1)+by(i,j,k)*hxy(1,2)+
     +        bz(i,j,k)*hxy(1,3)
              bry=bx(i,j,k)*hxy(2,1)+by(i,j,k)*hxy(2,2)+
     +        bz(i,j,k)*hxy(2,3)
              brz=bx(i,j,k)*hxy(3,1)+by(i,j,k)*hxy(3,2)+
     +        bz(i,j,k)*hxy(3,3)
              brxm=hxy(1,1)**2+hxy(1,2)**2+hxy(1,3)**2
              brym=hxy(2,1)**2+hxy(2,2)**2+hxy(2,3)**2
              brzm=hxy(3,1)**2+hxy(3,2)**2+hxy(3,3)**2
              bb=sqrt(bx(i,j,k)**2+by(i,j,k)**2+bz(i,j,k)**2)
              brm=sqrt(brxm+brym+brzm)
              if(bb.gt.0)then
                br=sqrt(brx**2+bry**2+brz**2)/bb
              else
                br=0
              endif
*              write(31,*)x1,bx(i,j,k),by(i,j,k),bz(i,j,k),br,brm
             enddo
           enddo
          enddo
          call timex(st1)
          write(*,*)'Finished in ',st1-st0, ' sec'
c          call rmf2
      endif
      end

c      subroutine rmf2
c#      include "mfield.h"
c*
c* Initialization for E01
c*
c          write(*,*)'x ',x
c          write(*,*)'y ',y
c          write(*,*)'z ',z
c          write(*,*)'Calculating E01GTF parameters'
c        call timex(st0)
c      do k=1,nz
c       do j=1,ny
c        do i=1,nx
c         l=i+(j-1)*nx+(k-1)*nx*ny
c         xe(l)=x(i)
c         ye(l)=y(j)
c         ze(l)=z(k)
c         bxe(l)=bx(i,j,k)         
c         bye(l)=by(i,j,k)         
c         bze(l)=bz(i,j,k)         
c        enddo
c       enddo
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      write(*,*)'x started'
c      call e01tgf(mmax,xe,ye,ze,bxe,nw,nq,iqx,liq,rqx,lrq,ifail)
c      write(*,*)'x finished'
c      do i=1,mmax
c        tmp1=rqx(i)
c        tmp2=rqx(i)-tmp1
c        rqxr(1,i)=tmp1
c        rqxr(2,i)=tmp2
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      call e01tgf(mmax,xe,ye,ze,bye,nw,nq,iqy,liq,rqy,lrq,ifail)
c      write(*,*)'y finished'
c      do i=1,mmax
c        tmp1=rqy(i)
c        tmp2=rqy(i)-tmp1
c        rqyr(1,i)=tmp1
c        rqyr(2,i)=tmp2
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      call e01tgf(mmax,xe,ye,ze,bze,nw,nq,iqz,liq,rqz,lrq,ifail)
c      do i=1,mmax
c        tmp1=rqz(i)
c        tmp2=rqz(i)-tmp1
c        rqzr(1,i)=tmp1
c        rqzr(2,i)=tmp2
c      enddo
c          call timex(st1)
c          write(*,*)'Finished in ',st1-st0, ' sec'
c      aa=0.1
c      call hbook1(-701,'dr',100,-aa,aa,0.)
c      call hbook2(-702,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-703,'dr',100,-aa,aa,0.)
c      call hbook2(-704,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-705,'dr',100,-aa,aa,0.)
c      call hbook2(-706,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-801,'dr',100,-aa,aa,0.)
c      call hbook2(-802,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-803,'dr',100,-aa,aa,0.)
c      call hbook2(-804,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-805,'dr',100,-aa,aa,0.)
c      call hbook2(-806,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-901,'dr',100,-aa,aa,0.)
c      call hbook2(-902,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-903,'dr',100,-aa,aa,0.)
c      call hbook2(-904,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-905,'dr',100,-aa,aa,0.)
c      call hbook2(-906,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      end

c      subroutine rmf3
c#      include "mfield.h"
c*
c* Initialization for E01
c*
c      do k=1,nz
c       do j=1,ny
c        do i=1,nx
c         l=i+(j-1)*nx+(k-1)*nx*ny
c         xe(l)=x(i)
c         ye(l)=y(j)
c         ze(l)=z(k)
c         bxe(l)=bx(i,j,k)         
c         bye(l)=by(i,j,k)         
c         bze(l)=bz(i,j,k)         
c        enddo
c       enddo
c      enddo
c      do i=1,mmax
c        tmp1=rqxr(1,i)
c        tmp2=rqxr(2,i)
c        rqx(i)=tmp1
c        rqx(i)=rqx(i)+tmp2
c        tmp1=rqyr(1,i)
c        tmp2=rqyr(2,i)
c        rqy(i)=tmp1
c        rqy(i)=rqy(i)+tmp2
c        tmp1=rqzr(1,i)
c        tmp2=rqzr(2,i)
c        rqz(i)=tmp1
c        rqz(i)=rqz(i)+tmp2
c      enddo
c      end
      SUBROUTINE GUFLD(VV,B)
*     ======================
#      include "mfield.h"
      data init/0/ 
      data hint/0,0,0/
      DIMENSION B(3),VV(3),AVV(3)
        if(abs(vv(1)).gt.x(nx).or.abs(vv(2)).gt.y(ny).or.
     +    abs(vv(3)).gt.z(nz))then
            call vzero(b(1),3)
            goto 999
        ENDIF
#ifdef __G4AMSU__
        b(1)=7
        b(2)=0
        b(3)=0
        goto 999
#endif

C
        sx=sign(1.,vv(1))
        sy=sign(1.,vv(2))
        sz=sign(1.,vv(3))
        do i=1,2
          avv(i)=abs(vv(i))
        enddo
*
* Special trick if need
*        
        CALL BZCORR(xfac)
        avv(3)=abs(vv(3))*xfac
c        if(iniok.eq.1)then
c          b(1) = FINT(3,avv(1),NA(1),xyz(1),Bx(1,1,1))
c          b(2) = FINT(3,avv(1),NA(1),xyz(1),By(1,1,1))
c          b(3) = FINT(3,avv(1),NA(1),xyz(1),Bz(1,1,1))
          call fint_my(3,avv(1),NA(1),xyz(1),
     +    Bx(1,1,1),By(1,1,1),Bz(1,1,1),b(1),hint(1))
c        else
c         u(1)=avv(1)
c         v(1)=avv(2)
c         w(1)=avv(3)
c         ifail=-1
c         call  e01thf(mmax,xe,ye,ze,bxe,iqx,liq,rqx,lrq,1,
c     +   u,v,w,q,qx,qy,qz,ifail)
c         if(ifail.ne.0)then
c          write(*,*)' e01thf failed on x ',ifail
c          b(1) = FINT(3,avv(1),NA(1),xyz(1),Bx(1,1,1))
c         else
cc          dr=b(1)-q(1)
cc          call hf1(-701,dr,1.)
cc          dr=q(1)
cc          call hf2(-702,b(1),dr,1.)
c          b(1) = q(1)
c         endif
c         ifail=-1
c         call  e01thf(mmax,xe,ye,ze,bye,iqy,liq,rqy,lrq,1,
c     +   u,v,w,q,qx,qy,qz,ifail)
c         if(ifail.ne.0)then
c          write(*,*)' e01thf failed on y ',ifail
c          b(2) = FINT(3,avv(1),NA(1),xyz(1),By(1,1,1))
c         else
cc          dr=b(2)-q(1)
cc          call hf1(-801,dr,1.)
cc          dr=q(1)
cc          call hf2(-802,b(2),dr,1.)
c          b(2) = q(1)
c         endif
c         ifail=-1
c         call  e01thf(mmax,xe,ye,ze,bze,iqz,liq,rqz,lrq,1,
c     +   u,v,w,q,qx,qy,qz,ifail)
c         if(ifail.ne.0)then
c          write(*,*)' e01thf failed on z ',ifail
c          b(3) = FINT(3,avv(1),NA(1),xyz(1),Bz(1,1,1))
c         else
cc          dr=b(3)-q(1)
cc          call hf1(-901,dr,1.)
cc          dr=q(1)
cc          call hf2(-902,b(3),dr,1.)
c          b(3) = q(1)
c         endif
c        endif
        if(magstat.gt.0)then
         CALL BTEMPCOR (factor)
         b(1) = b(1) * factor
         b(2) = b(2)*sx*sy * factor
         b(3) = b(3)*sx*sz * factor
        else
         b(1)=0
         b(2)=0
         b(3)=0
        endif

 999    END



      SUBROUTINE TKFLD(XX,HXY)
*     ========================
#      include "mfield.h"
       
          real xx(3),axx(3),hxy(3,3)
         if(abs(xx(1)).gt.x(nx).or.abs(xx(2)).gt.y(ny).or.
     +    abs(xx(3)).gt.z(nz))then
           call vzero (hxy(1,1),9)
           goto 999
          endif
#ifdef __G4AMSU__
           call vzero (hxy(1,1),9)
           goto 999
#endif          

          sx=sign(1.,xx(1))
          sy=sign(1.,xx(2))
          sz=sign(1.,xx(3))
          do i=1,2
            axx(i)=abs(xx(i))
          enddo
          CALL BZCORR(xfac)
          axx(3)=abs(xx(3))*xfac
c          if(iniok.eq.1)then

c           hxy(1,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1))
c           hxy(2,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,1))
c           hxy(3,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,1))
c           hxy(1,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2))
c           hxy(2,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,2))
c           hxy(3,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,2))
c           call fint_my(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1),
c     +     Bdy(1,1,1,1),Bdz(1,1,1,1),hxy(1,1),hint(1))
c           call fint_my(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2),
c     +     Bdy(1,1,1,2),Bdz(1,1,1,2),hxy(1,2),hint(1))
           call fint_my_2(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1),
     +     Bdy(1,1,1,1),Bdz(1,1,1,1),Bdx(1,1,1,2),
     +     Bdy(1,1,1,2),Bdz(1,1,1,2),hxy(1,1),hint(1))
c          else
c           u(1)=axx(1)
c           v(1)=axx(2)
c           w(1)=axx(3)
c           ifail=-1
c           call  e01thf(mmax,xe,ye,ze,bxe,iqx,liq,rqx,lrq,1,
c     +     u,v,w,q,qx,qy,qz,ifail)
c           if(ifail.ne.0)then
c            write(*,*)' e01thf failed on xd ',ifail
c            hxy(1,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1))
c            hxy(1,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2))
c          else
cc           d1=qx(1)
cc           d2=qy(1)
cc           call hf1(-703,d1-hxy(1,1),1.)
cc           call hf1(-705,d2-hxy(1,2),1.)
cc           call hf2(-704,d1,hxy(1,1),1.)
cc           call hf2(-706,d2,hxy(1,2),1.)
c           hxy(1,1)=qx(1)
c           hxy(1,2)=qy(1)
c          endif
c          ifail=-1
c          call  e01thf(mmax,xe,ye,ze,bye,iqy,liq,rqy,lrq,1,
c     +    u,v,w,q,qx,qy,qz,ifail)
c          if(ifail.ne.0)then
c           write(*,*)' e01thf failed on yd ',ifail
c            hxy(2,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,1))
c            hxy(2,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,2))
c          else
cc            d1=qx(1)
cc            d2=qy(1)
cc            call hf1(-803,d1-hxy(2,1),1.)
cc            call hf1(-805,d2-hxy(2,2),1.)
cc            call hf2(-804,d1,hxy(2,1),1.)
cc            call hf2(-806,d2,hxy(2,2),1.)
c           hxy(2,1)=qx(1)
c           hxy(2,2)=qy(1)
c          endif
c          ifail=-1
c          call  e01thf(mmax,xe,ye,ze,bze,iqz,liq,rqz,lrq,1,
c     +    u,v,w,q,qx,qy,qz,ifail)
c          if(ifail.ne.0)then
c           write(*,*)' e01thf failed on zd ',ifail
c            hxy(3,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,1))
c            hxy(3,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,2))
c          else
cc            d1=qx(1)
cc            d2=qy(1)
cc            call hf1(-903,d1-hxy(3,1),1.)
cc            call hf1(-905,d2-hxy(3,2),1.)
cc            call hf2(-904,d1,hxy(3,1),1.)
cc            call hf2(-906,d2,hxy(3,2),1.)
c           hxy(3,1)=qx(1)
c           hxy(3,2)=qy(1)
c          endif
c         endif

          if(magstat.gt.0)then
           CALL BTEMPCOR (factor)
           hxy(1,1) = hxy(1,1) * sx * factor
           hxy(2,1) = hxy(2,1) * sy * factor
           hxy(3,1) = hxy(3,1) * sz * factor
           hxy(1,2) = hxy(1,2) * sy * factor
           hxy(2,2) = hxy(2,2) * sx * factor
           hxy(3,2) = hxy(3,2) * sx*sy*sz * factor
          else
           hxy(1,1) = 0
           hxy(2,1) = 0
           hxy(3,1) = 0
           hxy(1,2) = 0
           hxy(2,2) = 0
           hxy(3,2) = 0
          endif
999       end



