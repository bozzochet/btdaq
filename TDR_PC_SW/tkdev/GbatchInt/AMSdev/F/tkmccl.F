*  $Id: tkmccl.F,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
           double precision function sitkfint2(a,b,av,dav,dlx)
            real *8 a,b,av,dav,dlx,dlmax,dlmin
            real *8 fint_l,fint_r,fint_c,sitkfints
            if(abs(dav)/dlx.le.0.01)then
             sitkfint2=sitkfints(a-av,b-av)
            else
              dlmin=av-abs(dav)
              dlmax=av+abs(dav)
              if(a.le.dlmin.and.b.ge.dlmax)then
                sitkfint2=fint_c(a,b,dlmin,dlmax)
              else if(a.le.dlmin.and.b.le.dlmin)then
                sitkfint2=fint_l(a,b,dlmin,dlmax)
              else if(a.ge.dlmax.and.b.ge.dlmax)then
                sitkfint2=fint_r(a,b,dlmin,dlmax)
              else if(a.le.dlmin.and.b.le.dlmax)then
                ds=(b-dlmin)/(dlmax-dlmin)
                sitkfint2=fint_c(a,b,dlmin,b)*ds
                ds=(dlmax-b)/(dlmax-dlmin)
                sitkfint2=sitkfint2+fint_l(a,b,b,dlmax)*ds
              else if(a.ge.dlmin.and.b.ge.dlmax)then
                ds=(dlmax-a)/(dlmax-dlmin)
                sitkfint2=fint_c(a,b,a,dlmax)*ds
                ds=(a-dlmin)/(dlmax-dlmin)
                sitkfint2=sitkfint2+fint_r(a,b,dlmin,a)*ds
              else
                ds=(b-a)/(dlmax-dlmin)
                sitkfint2=fint_c(a,b,a,b)*ds
                ds=(a-dlmin)/(dlmax-dlmin)
                sitkfint2=sitkfint2+fint_r(a,b,dlmin,a)*ds
                ds=(dlmax-b)/(dlmax-dlmin)
                sitkfint2=sitkfint2+fint_l(a,b,b,dlmax)*ds
              endif
            endif
           end
           double precision function sitkfints(a,b)
            real *8 fdiff,a,b
            if(a.ge.0.and. b.ge.0)then
             sitkfints=fdiff(a,0)-fdiff(b,0)
            else if(a.le.0.and.b.le.0)then
             sitkfints=fdiff(b,0)-fdiff(a,0)
            else
             sitkfints=2*fdiff(0.d0,0)-fdiff(a,0)-fdiff(b,0)
            endif
           end
           function fint_c(a,b,c,d)
           double precision fint_c,fdiff,a,b,c,d
             fint_c=2*fdiff(0.d0,0)-(fdiff(b-d,1)-fdiff(b-c,1))/(d-c)
     +       +(fdiff(a-d,1)-fdiff(a-c,1))/(d-c)                   
           end
           function fint_r(a,b,c,d)
             double precision fint_r,fdiff,a,b,c,d
             fint_r=(-fdiff(b-d,1)+fdiff(b-c,1)+
     +       fdiff(a-d,1)-fdiff(a-c,1))/(d-c)
           end
           function fint_l(a,b,c,d)
             double precision fint_l,fdiff,a,b,c,d
             fint_l=(-fdiff(b-d,1)+fdiff(b-c,1)+
     +       fdiff(a-d,1)-fdiff(a-c,1))/(d-c)
           end
           double precision function fdiff(a,ialpha)
           common /trmcffkey/alpha,beta            
            integer init
            data init/0/
            double precision xl,a
             if(init.eq.0)then
              init=1
              if(alpha.eq.0.and.beta.eq.0)then
                write(*,*)
     +          'tkmccl.f /trmcffkey/alpha & beta zeros smth wrong stop'
                stop
              endif
             endif
              xl=abs(a)*alpha
            if(xl.gt.70)then
             fdiff=0
            else
             fdiff=beta*exp(-xl)
             if(ialpha.ne.0)fdiff=fdiff/alpha
            endif
           end
c           subroutine rnormx(r1)
c            pi=3.1415926
c77           qq=rndm(dummy)
c           if(qq.eq.0.)then
cc            write(*,*)' rndm eq 0'
c            goto 77
c           endif
c           RGEN1=SQRT(-2.*ALOG(qq))  
c           RGEN2=2.*PI*Rndm(dummy)    
c           R1=RGEN1*COS(RGEN2)   
c           end 
c         SUBROUTINE poissn(AMU,N,IERR)
c         DATA AMU0 /-12345.67/, AMAX /85./ 
c         IERR=0
c         IF(AMU .LE. 0) THEN
c          IERR=1
c          J=0
c         ELSEIF(AMU .GT. AMAX) THEN
c          CALL RNORMX(R)
c          J=R*SQRT(AMU)+AMU+0.5
c         ELSE
c          IF(AMU .NE. AMU0) THEN
c           AMU0=AMU
c           EMU=EXP(-AMU)
c          ENDIF
c          P=1
c          J=-1
c 1        J=J+1
c          P=P*rndm(d)
c          IF(P .GT. EMU) GO TO 1
c         ENDIF
c         N=J
c         END
c
