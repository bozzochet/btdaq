*  $Id: simul.f,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
        integer *2 iar(10000)
        real *8 xcur
        byte ib(20000)
        equivalence(ib(1),iar(1))
        x=200.
        open(10,file='simul.out',form='unformatted',
     +   status='new') 
        write(*,*)' n event please'
        read(*,*)nmax
        xcur=0       
        do i=1,nmax
         call poissn(x,n,ier)
         if(ier.ne.0)stop 'abort'
         iar(1)=n
         icura=2
         icurb=3
         do j=1,n
          iad=rndm(d)*189000+1
          iar(icura)=iad
          icura=icura+1
          icurb=icurb+2
          do k=1,5
           if(k.ne.3)then
            ivalue=50+4*rnormx(dd)+0.5
            ib(icurb)=ivalue
            icurb=icurb+1
           else
            ivalue=50+4*rnormx(dd)+200*rnormx(dd)+0.5
            icura=icura+1
            iar(icura)=ivalue
            icurb=icurb+2
           endif
          enddo
          icura=icura+2  
         enddo
         write(10)(iar(l),l=1,icura-1)
c         write(*,*)icura,icurb 
           xcur=xcur+icura-1
        enddo
          xcur=xcur/nmax
          write(*,*)' length (bytes) ' ,xcur*2
        close(10)
        end 
        function rnormx(ddmy)
            pi=3.1415926
77           qq=rndm(dummy)
           if(qq.eq.0.)then
c            write(*,*)' rndm eq 0'
            goto 77
           endif
           RGEN1=SQRT(-2.*ALOG(qq))
           RGEN2=2.*PI*Rndm(dummy)
           R1=RGEN1*COS(RGEN2)
           rnormx=r1
           end
