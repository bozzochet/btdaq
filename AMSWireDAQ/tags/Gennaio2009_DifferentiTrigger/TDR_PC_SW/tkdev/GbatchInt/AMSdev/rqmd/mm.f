         common /multi/nout,xpout
         common /input/a_t,z_t,a_p,z_p,pid,ekin,ierror
         real *4 xpout(6,1000)
         real *4 xpn(1000)
         real *8 es,egamma,e_dum,ez,gamma,beta
         real *8 ps,dm,px,py,pz,xmass2
         real*8 pxi,pyi,pzi,ezi
         real *8 pk(4)
         integer ixpn(1000)
         integer iarr(2,1000)
         common /pawc/hm(1000000)
         maxout=1000
         call hlimit(1000000)
         call seed(-1)
         call hropen(3,'output','out.hbk','NPQ',1024,iostat)
         call hbook1(10,'Inelasticity',500,0.,1.,0.)
         call hbnt(1,'my ntuple',' ')
         call hbname(1,'Input',a_t,
     +   'at,zt,ap,zp,pidp,ekin,ierror')
         call hbname(1,'Multi',nout,
     +   'nout[0,1000],xpout(6,nout)')
         write(*,*)'give z_t,a_t // pin // nc '
         read(*,*)z_t,a_t
         read (*,*)pin
         read(*,*)nc
         ic=0
 10      continue
         ic=ic+1
         write(*,*)'ic ',ic
         r=rndm(d)
c         z_p=int(rndm(d)*12)
c         a_p=int(z_p*2+rndm(d)*4)
c         z_t=int(rndm(d)*12)
c         a_t=int(z_t*2+rndm(d)*4)
         z_p=26
         a_p=58
         pid=87
         ekin=(pin**2+a_p**2*0.938**2)**0.5-a_p*0.938
         ekin=ekin/a_p
         
c         z_t=1
c         a_t=1
         
         write(*,*)'**********'
         write(*,*)a_p,z_p
         write(*,*)a_t,z_t
         write(*,*)ekin
c         a_p=12
c         z_p=6
c         a_t=12
c         z_t=6
c         read(*,*)ekin
         nout=-maxout
         if(a_t.eq.1)then
          call call_fluka(a_p,z_p,a_t,z_t,ekin,0.938*a_p,
     +    nout,xpout,ierror)
         else
          call call_rqmd(a_p,z_p,a_t,z_t,ekin,nout,xpout,ierror)
         endif
         s=0
         s4=0
         stsum=0
         stmax=0           
         do i=1,nout
          s=s+xpout(3,i)
          s4=s4+xpout(4,i)
          st=xpout(4,i)-xpout(6,i)
          if (st.gt.stmax)stmax=st
          stsum=stsum+st
          write(*,*)i,xpout(3,i),xpout(4,i),xpout(5,i),xpout(6,i)
         enddo
         write(*,*)'sum',s,s4,stsum,stmax,stmax/stsum
         call hf1(10,stmax/stsum,1.)
         call hfnt(1)
c         read(*,*)i1
         if(ierror.ne.0)then
           write(*,*)'ierror ',ekin
         endif
         if(ic.lt.nc)goto 10
          call HROUT (0, ICYCL, ' ')
          call  HREND ('output')
          close(3)
         end

             function rndm(dummy)
             data i/0/
             real *4 r(2)
             if(i.eq.0)then
              i=1
              call grndmq(  556776976, 462072869,1,'S')
c              call grndmq(1,1,2,' ')
             endif
             call grndm(r,1)
             rndm=r(1)

c             i=i+1
c             write(*,*)i,r(1)
              end
