*
*  Interface to rqmd  V. Choutko 7-mar-2000
*
            subroutine call_rqmd(a_p,z_p,a_t,z_t,ekin,nout,xpout,ierr)
            include 'scatter7'
            include 'partblk7'
            include 'amsrqmd.h'
            real *4 a_p,z_p,a_t,z_t,ekin
            parameter (maxout=10000)
            parameter (p_mass=0.934)
            real *4 xpout(6,*)
            real *4 xpn(maxout),pid
            real *8 es,egamma,e_dum,ez,gammax,betax
            real *8 ps,dm,px,py,pz,xmass2
            real*8 pxi,pyi,pzi,ezi
            real *8 momentum,energy
            real *8 pk(4)
            integer ixpn(maxout)
            integer iarr(2,maxout)
              npout=-nout
            if(npout.gt.maxout)then
             write(*,*)'call_rqmd-F-wrongarraysize ',npout,maxout
            stop 
            endif
            pid=igepar2(int(a_p),int(z_p),xm)
*            write(*,*)pid
            iagain=0
 10          continue
               nout=0
              if(a_p.lt.a_t+20)then
               n1=a_p
               iz1=z_p
               n2=a_t
               iz2=z_t
               ione=1
              else
               n2=a_p
               iz2=z_p
               n1=a_t
               iz1=z_t
               ione=-1
               energy=ekin*a_p
               momentum=sqrt(energy*(energy+2*a_p*p_mass))
               energy=energy+a_p*p_mass
               betax=momentum/energy
               if(betax.lt.1.)then
                 gammax=1/sqrt(1-betax**2)
               else
                write(*,*)'call-rqmd-E-wrong beta ',betax
                ierr=1
                goto 999
               endif
              endif
              elgev=ekin
              if(elgev.lt.0.1)then
               ierr=2
               write(*,*)'rqmd error no',ierr,elgev
               goto 999
              endif 
              ierror=0
              call rqmd_start  
              call rqmd_event
              call rqmd_endev
              ierr=ierror
              if(ierr.ne.0)goto 999
*
* Transform back to lab frame
*

      ELAB=Ekin                                                              
      EMPROJ=EMNUC                                                            
      PPROJ=SQRT(ELAB*(2.*EMPROJ+ELAB))                         
      S2=(ELAB+EMNUC+EMPROJ)**2-PPROJ**2  
      AVIRT= 0.5*(S2- EMNUC*EMNUC-EMPROJ*EMPROJ)/EMNUC/EMPROJ                 
      IF(AVIRT.LT.1.) THEN                                                     
         WRITE(6,*) 'NUMERICAL ERROR AVIRT.LT.1.   AVIRT=',AVIRT    
         ierr=1
         goto 999                                         
      END IF                                                      
      BETANN=SQRT( (AVIRT-1.)/(AVIRT+1.))                       
      GAMNN= 1./SQRT(1.-BETANN*BETANN)                             
            GAMTRA= GAMNN                                            
            BETTRA=-BETANN                                          
      NTOTAL=N1+N2+npion
c      write(*,*)'npion',npion
      DO  L=1,NTOTAL                                       
C ... TRANSFORM MOMENTA TO REFNEW SYSTEM                          
       P0= GAMTRA*(P(0,L)+ BETTRA*P(3,L) )                          
       P(3,L)= GAMTRA*(P(3,L)+ BETTRA*P(0,L) )                
       P(0,L)=P0                                                
C... CONVERT PROJECTILE DIRECTION (Y(PROJ.) > 0 ).               
       P(3,L)=-P(3,L)                                           
       enddo
*
* Check momentum
*
c       px=0
c       py=0
c       pz=0
c       pp=0
c       do l=1,ntotal
c        pp=pp+p(0,l)
c        px=px+p(1,l)
c        py=py+p(2,l)
c        pz=pz+p(3,l)
c       enddo
c      write(*,*)'a1 ',pp,px,py,pz,pproj*n1  

      do l=1,ntotal
       call transl(-1,ityp(l),npidec(l),kl2)
*
*  k0s/k0l
*
       if(kl2.eq.19)then
           kl2=37
       else if(kl2.eq.-19)then
            kl2=38
       endif
       ipart=l6tog3(kl2)
c       write(*,*)l,ipart,kl2    
c       write(*,*)p(1,l),p(2,l),p(3,l)
      if(ipart.gt.0)then
       nout=nout+1
       if(nout.gt.npout)nout=npout
       xpout(1,nout)=p(1,l)*ione
       xpout(2,nout)=p(2,l)*ione
       xpout(3,nout)=p(3,l)*ione
       xpout(4,nout)=p(0,l)
       if(ione.lt.0)then
           momentum=xpout(3,nout)
           energy=xpout(4,nout)
           xpout(4,nout)=gammax*(energy+betax*momentum)
           xpout(3,nout)=gammax*(+betax*energy+momentum)
       endif
       xpout(5,nout)=ipart
       xpout(6,nout)=em(l)
       if(xpout(6,nout).gt.xpout(4,nout))then
        xpout(4,nout)=xpout(6,nout)**2
        do k=1,3
         xpout(4,nout)=xpout(4,nout)+xpout(k,nout)**2
        enddo
        xpout(4,nout)=sqrt(xpout(4,nout))
       endif
      else
       write(*,*)'l6tog3-E-NoSuchParticle',kl2
      endif
      enddo


*
* Build nuclei
*
         nhad=0
         do i=1,n1+n2
          if(xpout(5,i).eq.14.or.xpout(5,i).eq.13)then
           nhad=nhad+1
           xpn(nhad)=xpout(4,i)
           if(xpout(5,i).eq.14)then
            iarr(1,nhad)=1
           else
            iarr(1,nhad)=0
           endif
           iarr(2,nhad)=0
          endif
         enddo          
         if(nhad.lt.2)goto 999
         
*
* Sort by momentum
*
         call sortzv(xpn,ixpn,nhad,1,1,0)
c         do k=1,nhad
c         write(*,*)xpn(ixpn(k)),xpout(5,ixpn(k)),
c     +   iarr(1,ixpn(k))
c         enddo
*
* Take the seed
*
         
         iseed=1               
         thr=0.01
         
         do while(iseed.lt.nhad)
           do k=1,4
            pk(k)=xpout(k,ixpn(iseed))
           enddo           
          ifound=0
          do i=iseed+1,nhad
           if(iarr(2,i).eq.0)then
           ps=0
           do k=1,3
            ps=ps+(pk(k)+xpout(k,ixpn(i)))**2
           enddo           
           es=pk(4)+xpout(4,ixpn(i))
           es=es**2
           dm=sqrt(es-ps)-
     +      (xpout(6,ixpn(i))+xpout(6,ixpn(iseed)))
c            write(*,*)iseed,i,dm,xpout(4,ixpn(iseed)),
c     +      xpout(4,ixpn(i))
            if(dm.lt.thr)then
              iarr(2,i)=1
              ifound=1
            else if(ifound.eq.0)then
             goto 15
            else
             goto 14
            endif
           endif
          enddo
 14       continue
          iz=iarr(1,iseed)
          ia=1
          do i=iseed+1,nhad
            if(iarr(2,i).eq.1)then
             iz=iz+iarr(1,i)
             ia=ia+1
            endif
          enddo   

          if(ia.gt.1.and.iz.ne.ia.and.iz.ne.0)then
*
* Find closest Ion
*
           ipart=igepar2(ia,iz,xmass)
           iarr(2,iseed)=-1
           iz0=iarr(1,iseed)
           ia0=1
           do k=1,4
            pk(k)=xpout(k,ixpn(iseed))
           enddo           
           do k=1,3
            xpout(k,ixpn(iseed))=0
           enddo
           xpout(4,ixpn(iseed))=xpout(6,ixpn(iseed))
           do l=1,0,-1
           do i=iseed+1,nhad
            if(iarr(2,i).eq.1)then
             if(iarr(1,i).eq.l)then
              if((iz0.lt.iz.and.l.eq.1).or.
     +           (ia0.lt.ia.and.l.eq.0))then
               iarr(2,i)=-1
               do k=1,4
                pk(k)=pk(k)+xpout(k,ixpn(i))
               enddo      
               do k=1,3
               xpout(k,ixpn(i))=0
               enddo
               xpout(4,ixpn(i))=xpout(6,ixpn(i))
               ia0=ia0+1
               iz0=iz0+l     
              else
               iarr(2,i)=0
              endif
             endif
            endif 
           enddo
           enddo
           if(ia0.ne.ia.or.iz0.ne.iz)then
            write(*,*)'call-rqmd-S-erorinbuildngnuclei ',ia,ia0,iz,iz0
            ierr=1
            goto 999 
           endif
           es=pk(4)**2-pk(1)**2-pk(2)**2-pk(3)**2
           egamma=(sqrt(2*es-xmass**2)-xmass)/2
           es=sqrt(es)
           if(egamma.lt.0)egamma=0
           pi=3.1415926
           th=pi*acos(rndm(d))
           ph=2*pi*rndm(d)
           pz=egamma*cos(th)
           px=egamma*sin(th)*cos(ph)
           py=egamma*sin(th)*cos(ph)
           ez=egamma
           pzi=-pz
           pyi=-py
           pxi=-px
           ezi=sqrt(egamma**2+xmass**2)
           gammax=pk(4)/es
           betax=sqrt(1-1/gammax**2)
           Ez     = gammax * (   egamma +betax*PZ)
           PZ    = gammax * (betax*egamma + PZ)
           e_dum=ezi
           Ezi     = gammax * (   e_dum + betax*PZi)
           PZi    = gammax * (betax*e_dum + PZi)
           xmass2=sqrt(ezi**2-pxi**2-pyi**2-pzi**2)
           if(abs(xmass2-xmass).gt.0.01)then
            write(*,*)'error mass calculation ',xmass,xmass2
           endif
*
* Create particles 
*
c           write(*,*)'call_rqmd-I-IonCreated',ipart,ezi
           do i=1,nhad
            if(iarr(2,i).eq.-1)then
             iarr(2,i)=-2
             xpout(1,ixpn(i))=pxi
             xpout(2,ixpn(i))=pyi
             xpout(3,ixpn(i))=pzi
             xpout(4,ixpn(i))=ezi
             xpout(5,ixpn(i))=ipart
             xpout(6,ixpn(i))=xmass2
             goto 16
            endif
           enddo
 16        continue
           if(egamma.gt.0)then
c            write(*,*)'call_rqmd-I-PhotonCreated',ez
           do i=1,nhad
            if(iarr(2,i).eq.-1)then
             iarr(2,i)=-2
             xpout(1,ixpn(i))=px
             xpout(2,ixpn(i))=py
             xpout(3,ixpn(i))=pz
             xpout(4,ixpn(i))=ez
             xpout(5,ixpn(i))=1
             xpout(6,ixpn(i))=0
             goto 17
            endif
           enddo
 17        continue
           endif         
          else
*
* Clean Up
*
           do i=iseed+1,nhad
            if(iarr(2,i).eq.1)then
             iarr(2,i)=0
            endif
           enddo
          endif
 15       continue
         do i=iseed+1,nhad
          if(iarr(2,i).eq.0.and.xpout(4,ixpn(i)).gt.
     +    xpout(6,ixpn(i))+thr)then
           iseed=i
           goto 20
          endif
         enddo
         iseed=nhad
 20      continue
         enddo
c         nrem=0
c         do i=1,nout
c          if(xpout(6,i).eq.xpout(4,i))then
c           nrem=nrem+1
c          endif
c         enddo
c         if(nrem.gt.0)write(*,*)'removed ',nrem,ekin
c      px=0
c      py=0
c      pz=0
c      pp=0
c      do l=1,nout
c      if(xpout(4,l).gt.xpout(6,l))then
c       pp=pp+xpout(4,l)
c       px=px+xpout(1,l)
c       py=py+xpout(2,l)
c       pz=pz+xpout(3,l)
c      endif
c       write(*,*)l,xpout(3,l),xpout(4,l),xpout(5,l)
c      enddo
c      write(*,*)'a2 ',pp,px,py,pz,pproj*n1  
*
* Final cleanup
*
       inout=nout
       do i=1,nout
        if(xpout(4,i)-xpout(6,i).eq.0)then
          do j=nout,i+1,-1
           if(xpout(4,j)-xpout(6,j).gt.0)then
            do k=1,6
             xpout(k,i)=xpout(k,j)
            enddo
            xpout(4,j)=xpout(6,j)
            goto 137
           endif
          enddo
          inout=inout-1
 137      continue
        endif
       enddo
c       write(*,*)' removed ',nout-inout
        nout=inout
*
* Check nonelasticity

        thri=0.95
         stsum=0
         stmax=0           
         do i=1,nout
          s=s+xpout(3,i)
          s4=s4+xpout(4,i)
          st=xpout(4,i)-xpout(6,i)
          if (st.gt.stmax.and.pid.eq.xpout(5,i))stmax=st
          stsum=stsum+st
         enddo
c         write(*,*)'stmax/stsum',stmax/stsum
         if(stmax/stsum.gt.thri)then
           iagain=iagain+1
              if(iagain.gt.99)then
*               ierr=3
               write(*,*)'rqmd error no 3' ,pid,a_p,a_t,
     +         z_p,z_t,ekin,stmax/stsum
              else
                 goto 10        
              endif

         endif
*
 999   end
         
         integer function igepar2(ia,iz,xmass)
 10       continue
          if(iz.eq.1)then
           if(ia.eq.2)then
            igepar2=45
            xmass=1.875613
           else 
            ia=3
            igepar2=46
            xmass=2.80925
           endif
          else if(iz.eq.2)then
           if(ia.eq.3)then
            igepar2=49
            xmass=2.80923
           else 
            ia=4
            igepar2=47
            xmass=3.727417
           endif
          else if(iz.eq.3)then
           if(ia.lt.6)then
            iz=iz-1
            ia=ia-1
            goto 10
           else if(ia.eq.6)then
            igepar2=61
            xmass=5.60305
           else 
            ia=7
            igepar2=62
            xmass=6.53536
           endif
          else if(iz.eq.4)then
           if(ia.lt.7)then
            iz=iz-1
            ia=ia-1
            goto 10
           else if(ia.le.8)then
            ia=7
            igepar2=63
            xmass=6.53622
           else 
            ia=9
            igepar2=64
            xmass=8.39479
           endif
          else if(iz.eq.5)then
           if(ia.lt.10)then
            iz=iz-1
            ia=ia-1
            goto 10
           else if(ia.eq.10)then
            igepar2=65
            xmass=9.32699
           else 
            ia=11
            igepar2=66
            xmass=10.25510
           endif
          else
           ia=ia-iz+6
           iz=6
           if(ia.lt.12)then
            iz=iz-1
            ia=ia-1
            goto 10
           else
            ia=12
            igepar2=67
            xmass=11.17793
           endif
          endif
          
 999      end



      integer function l6tog3(klund)
      PARAMETER (NSEL=32)
      integer IPLUND(NSEL)
      integer klund
      DATA IPLUND/
     + +1, -7, +7, +8, -9, +9, 23, 17, -17, 38, 18, -18, 
     + 42, 41,-41, 37, 24, 57, 43, 44, 45, 46, 47, 34,
     +-42,-57,-43,-44,-45,-46,-47,-34/ 
     
       l6tog3=-1
       DO  IGE=1, NSEL
                  IF(IPLUND(IGE).EQ.Klund) THEN
                     l6tog3=ige
                     goto 999                    
                   ENDIF
       enddo
 999   end
