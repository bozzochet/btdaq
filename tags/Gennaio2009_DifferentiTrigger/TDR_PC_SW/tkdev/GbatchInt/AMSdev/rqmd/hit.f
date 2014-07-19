CPAT,HIT      .                                                         
      logical function hit(iorig,i1,i2,srt,sig,sigel,thit)              
        include 'scatter7'                                              
        dimension ifind(6)                                              
        include 'partblk7'                                              
        include 'sigmasx'                                               
        common /ierdum/ierr                                             
        dimension plk(0:3),qlk(0:3),qtlk(0:3),xdot(0:3),icoll(70)       
        data ifind/  1,  0, 0 ,0  ,0  ,  0/                             
        data icoll / 16*0,1,1,1,3*0,1,1,                                
     a         1,0,1,1,1,3*0,3*1,0,1,1,2*0,7*2,9*0,2,3*0,10*2/          
       ikey=0                                                           
       if(i1.eq.i2) goto 22                                             
       itp1=ityp(i1)                                                    
       itp2=ityp(i2)                                                    
       if(itp1.eq.-9999) goto 22                                        
       if(itp2.eq.-9999) goto 22                                        
       ident1=issn(i1)                                                  
       ident2=issn(i2)                                                  
       if(mod(ida(40),2).eq.1) then                                     
         deltz=zlast(ident1)-zlast(ident2)                              
         delty=ylast(ident1)-ylast(ident2)                              
         if(deltz*delty.gt.3.0) go to 22                                
         if(min(deltz,delty).gt.1.0) goto22                             
         if(max(deltz,delty).lt.-1.0) goto22                            
       end if                                                           
       if(ichbig(i1).eq.3) go to 22                                     
       if(ichbig(i2).eq.3) go to 22                                     
       if(ida(23).ne.0) then                                            
         if(qdot(0,i1).eq.0.0.or.qdot(0,i2).eq.0.0) go to 22            
       end if                                                           
      if(lastcl(i1).eq.lastcl(i2).and.lastcl(i1).ne.0) go to 22         
      npid1= npidec(i1)                                                 
      npid2= npidec(i2)                                                 
      if(itp1.ge.85.and.itp1.le.99) then                                
        if(itp1.lt.98) call substi(itp1,npid1)                          
        if(itp1.ge.98) then                                             
          if(iabs(npid1).gt.70.or.                                      
     a            icoll(iabs(npid1)).eq.0) then                         
            go to 22                                                    
          end if                                                        
        end if                                                          
      end if                                                            
      if(itp2.ge.85.and.itp2.le.99) then                                
        if(itp2.lt.98) call substi(itp2,npid2)                          
        if(itp2.ge.98) then                                             
          if(iabs(npid2).gt.70.or.                                      
     a            icoll(iabs(npid2)).eq.0) then                         
            go to 22                                                    
          end if                                                        
        end if                                                          
      end if                                                            
       do 18 i=0,3                                                      
       plk(i)= p(i,i1)+p(i,i2)                                          
       qlk(i)= x(i,i1)-x(i,i2)                                          
   18 continue                                                          
      dqlksq= qlk(0)*qlk(0)                                             
      splksq= plk(0)*plk(0)                                             
      dqsplk= qlk(0)*plk(0)                                             
      plkvc2=0.                                                         
      do 19 i=1,3                                                       
      dqlksq= dqlksq-qlk(i)*qlk(i)                                      
      dqsplk= dqsplk-qlk(i)*plk(i)                                      
      plkvc2= plkvc2+plk(i)*plk(i)                                      
   19 continue                                                          
      splksq=splksq-plkvc2                                              
      if(ida(2).eq.1.and.splksq.lt.16.) go to 22                        
      arg=(-dqlksq+dqsplk*dqsplk/splksq)*da(8)                          
      if(arg.lt.0.) then                                                
         write(8,*) 'hit: arg=',arg,'  ???????????'                     
         write(8,*) dqlksq,dqsplk,splksq                                
         write(8,*) issn(i1),issn(i2)                                   
         write(8,*) (p(i,i1),i=0,3),(x(i,i1),i=0,3)                     
         write(8,*)  (p(i,i2),i=0,3),(x(i,i2),i=0,3)                    
         arg=0.                                                         
      end if                                                            
      dtc=x(0,i1)-tlast(i1)                                             
      dtmax1=min(qdot(0,i1)*dt,dtc)                                     
      dtc=x(0,i2)-tlast(i2)                                             
      dtmax2=min(qdot(0,i2)*dt,dtc)                                     
      if(iorig.eq.0) then                                               
         if(sqrt(arg)-                                                  
     a       (plk(0)+sqrt(plkvc2))/sqrt(splksq)*(dtmax1+dtmax2)         
     b                                              .gt.5.5) then       
            go to 22                                                    
         end if                                                         
      end if                                                            
       if(ikey.eq.2) then                                               
          ihopp=1                                                       
       end if                                                           
      do 20 i=0,3                                                       
      xdot(i)= qdot(i,i1)-qdot(i,i2)                                    
      qlk(i)= qlk(i)- xdot(i) *dt                                       
   20 continue                                                          
      dqlksq= qlk(0)*qlk(0)                                             
      dqsplk= qlk(0)*plk(0)                                             
      do 25 i=1,3                                                       
      dqlksq= dqlksq-qlk(i)*qlk(i)                                      
      dqsplk= dqsplk-qlk(i)*plk(i)                                      
   25 continue                                                          
       if(ikey.eq.2) then                                               
          ihopp=2                                                       
       end if                                                           
      zlk= dqlksq- dqsplk*dqsplk/splksq                                 
      do 26 i=0,3                                                       
      qtlk(i)= qlk(i)- dqsplk/splksq*plk(i)                             
   26 continue                                                          
      xdotsp= xdot(0)*plk(0)                                            
      xdotsq=xdot(0)*xdot(0)                                            
      xtxdot= qtlk(0)*xdot(0)                                           
      do 27 i=1,3                                                       
      xdotsp= xdotsp-xdot(i)*plk(i)                                     
      xdotsq=xdotsq-xdot(i)*xdot(i)                                     
      xtxdot= xtxdot-qtlk(i)*xdot(i)                                    
   27 continue                                                          
      if(xtxdot.lt.0.) go to 22                                         
       if(ikey.eq.2) then                                               
          ihopp=3                                                       
       end if                                                           
      denum=xdotsp*xdotsp/splksq-xdotsq                                 
      if(denum.lt.1.e-5) go to 22                                       
       if(ikey.eq.2) then                                               
          ihopp=4                                                       
       end if                                                           
      dtau= xtxdot/denum                                                
      if(dtau.gt.dt.and.iorig.eq.0) go to 22                            
       if(ikey.eq.2) then                                               
          ihopp=5                                                       
       end if                                                           
      if(dtau.lt.0.) then                                               
        write(8,*)'numerical error in hit dtau=',dtau,' .lt.0.'         
        dtau=0.                                                         
      end if                                                            
      ttrue=time+dtau                                                   
      if(ttrue.le.tlstev) go to 22                                      
       if(ikey.eq.2) then                                               
          ihopp=6                                                       
       end if                                                           
      tcol1=x(0,i1)+qdot(0,i1)*(dtau-dt)                                
      tcol2=x(0,i2)+qdot(0,i2)*(dtau-dt)                                
      if((tlast(i1).ne.0..and.tcol1.lt.tlast(i1)).or.                   
     a            (tlast(i2).ne.0..and.tcol2.lt.tlast(i2)))then         
          go to 22                                                      
      end if                                                            
       if(ikey.eq.2) then                                               
          ihopp=7                                                       
       end if                                                           
      do 30 i=0,3                                                       
      qlk(i)= qlk(i)+xdot(i)*dtau                                       
   30 continue                                                          
      dqlksq= qlk(0)*qlk(0)                                             
      dqsplk= qlk(0)*plk(0)                                             
      do 35 i=1,3                                                       
      dqlksq= dqlksq-qlk(i)*qlk(i)                                      
      dqsplk= dqsplk-qlk(i)*plk(i)                                      
   35 continue                                                          
      zlk=dqlksq-dqsplk*dqsplk/splksq                                   
      bsq=-zlk*da(8)                                                    
      s=splksq                                                          
      srt=sqrt(splksq)                                                  
      iii=max0(itp1,itp2)                                               
      jjj=min0(itp1,itp2)                                               
      icltyp=(iii*(iii-1))/2+jjj                                        
       if((icltyp.ge.3571).and.(icltyp.le.4950)) then                   
           icltyp=maxtyp+1                                              
       end if                                                           
      if((icltyp.le.0).or.(icltyp.gt.maxtyp+1) ) then                   
        call errex('wrong icltyp in hit')                               
      end if                                                            
      if(splksq.gt.25.) then                                            
         xm2=1.6                                                        
      else                                                              
         xm2=seacut(icltyp)*seacut(icltyp)                              
      end if                                                            
      if(icltyp.le.3) then                                              
      else if(icltyp.ge.79.and.icltyp.le.80) then                       
      else if(icltyp.ge.91.and.icltyp.le.92) then                       
      else                                                              
        if(srt-em(i1)-em(i2).lt.0.4) then                               
           xm2=3.*xm2*0.4/max(0.5*cvalu3,srt-em(i1)-em(i2))             
        end if                                                          
      end if                                                            
      ncoli1=nclcnt(ident1)                                             
      ncoli2=nclcnt(ident2)                                             
      if(nt.gt.2.and.((itripl.eq.1.and..not.switch(17))                 
     a              .or.ida(34).gt.0.or.switch(23).or.switch(6)).and.   
     b                                (ncoli1.eq.0.or.ncoli2.eq.0))then 
         ifreez=1                                                       
         xm2=1.5*xm2                                                    
      else                                                              
         ifreez=0                                                       
      end if                                                            
      if(bsq.gt.xm2) go to 22                                           
       if(ikey.eq.2) then                                               
          ihopp=8                                                       
       end if                                                           
      if(icltyp.le.3.and.s.lt.(em(i1)+em(i2)+ekinmi)**2) then           
        goto 22                                                         
      end if                                                            
       if(ikey.eq.2) then                                               
          ihopp=9                                                       
       end if                                                           
        pi1sq=p(0,i1)*p(0,i1)                                           
        do 38 i=1,3                                                     
   38   pi1sq=pi1sq-p(i,i1)*p(i,i1)                                     
        pi2sq=p(0,i2)*p(0,i2)                                           
        do 39 i=1,3                                                     
   39   pi2sq=pi2sq-p(i,i2)*p(i,i2)                                     
        if(srt.le.sqrt(pi1sq)+sqrt(pi2sq)) then                         
           write(8,*) 'hit warning: sqrt(s)<em1+em2(eff)'               
           write(8,*) 'hit', issn(i1),srt,sqrt(pi1sq)                   
           write(8,*) 'hit', issn(i2),srt,sqrt(pi2sq)                   
           goto 22                                                      
        end if                                                          
        prel=pcmsr(srt,sqrt(pi1sq),sqrt(pi2sq))                         
        em1=em(i1)                                                      
        em2=em(i2)                                                      
        if(em1+em2+ekinmi.gt.srt) go to 22                              
        enlrge=1.0                                                      
        if(ifreez.eq.1) then                                            
          call frzcor(enlrge,i1,i2)                                     
        else if(.not.switch(6)) then                                    
          pfree=pcmsr(srt,em1,em2)                                      
          enlrge=pfree/prel                                             
        end if                                                          
        sigact=10.*pi*bsq                                               
        if(ida(40).eq.2.or.ida(40).eq.3) then                           
           sig=sigact/enlrge                                            
        else                                                            
           call cross1(1,srt,prel,icltyp,sig,em1,em2,                   
     a                           itp1,itp2,npid1,npid2)                 
           if(sig.eq.0.) go to 22                                       
           sigact=sigact/enlrge                                         
           if(sigact.gt.sig) goto 22                                    
        end if                                                          
      hit=.true.                                                        
      thit=ttrue                                                        
      return                                                            
   22 hit=.false.                                                       
      return                                                            
      end                                                               
      function pcmsr(a,b,c)                                             
      include 'scatter7'                                                
      if(a.ge.b+c) then                                                 
        pcmsr=sqrt((a**2-(b+c)**2)*(a**2-(b-c)**2))/(2.*a)              
      else                                                              
        pcmsr=0.                                                        
        idiv=0                                                          
        adiv=1./idiv                                                    
        write(8,*) 'adiv=',adiv                                         
        write(8,*) 'illegal call of pcmsr'                              
      end if                                                            
      return                                                            
      end                                                               
      subroutine hit34(iorig,i1,i2,sig,thit,iclyes)                     
        include 'scatter7'                                              
        include 'partblk7'                                              
        logical hit                                                     
        dimension xind(0:3),pind(0:3),plk(0:3),qlk(0:3)                 
        real*4 bxj,byj,bzj                                              
        real*4 plund,par                                                
        common /lujets/ nlund,k(2000,2),plund(2000,5)                   
        common/ludat1/mst(40),par(80)                                   
       real*4 radius(1)                                                 
       equivalence (radius(1),datsto(1))                                
        iclyes=0                                                        
        if(iorig.ne.0) then                                             
         write(8,*) 'iorig=',iorig                                      
         call errex('iorig not allowed in hit34')                       
        end if                                                          
        if(ida(34).ne.2) then                                           
         write(8,*) 'ida(34)=',ida(34)                                  
         call errex('ida(34).ne.2 not allowed in hit34')                
        end if                                                          
        if(ichbig(i1).eq.3) return                                      
        if(qdot(0,i1).eq.0.) return                                     
        do 8 i=0,3                                                      
         pind(i)=p(i,i1)                                                
         xind(i)=x(i,i1)-qdot(i,i1)*dt                                  
    8   continue                                                        
        gamta=1./sqrt(1.-betata*betata)                                 
        ti1=gamta*(xind(0)-betata*xind(3))                              
        xind(3)=gamta*(xind(3)-betata*xind(0))                          
        xind(0)=ti1                                                     
        p0i1=gamta*(pind(0)-betata*pind(3))                             
        pind(3)=gamta*(pind(3)-betata*pind(0))                          
        pind(0)=p0i1                                                    
        drsq=(xind(1)+bplus)**2+xind(2)**2+                             
     a         (xind(3)-gamta*0.5*(zmxta+zmnta))**2                     
        dr=sqrt(drsq)                                                   
        vlab=sqrt(pind(1)**2+pind(2)**2+pind(3)**2)/pind(0)             
        sigmax=400.                                                     
        rhoeff=rho0/(exp((dr-rr2)/da(2))+1.)                            
        if(rhoeff.lt.0.05*rho0) return                                  
        dpmax= 0.1*sigmax*rhoeff*dt*vlab                                
        zrand=ranf(0)                                                   
        if(zrand.gt.dpmax) return                                       
        i2=0                                                            
        radmin=10000.                                                   
        do 10 ib=iofset+1,iofset+nrbar(kclst)+nrmes(kclst)              
           if(prop(ib)) go to 10                                        
           if (i1.eq.ib) goto 10                                        
           if(ichbig(ib).eq.3) go to 10                                 
           if(lastcl(i1).eq.lastcl(ib).and.lastcl(i1).ne.0) go to 10    
           identb=issn(ib)                                              
           ncolib=nclcnt(identb)                                        
           if(identb.gt.n1.and.identb.le.nt.and.ncolib.eq.0) then       
           else                                                         
               go to 10                                                 
           end if                                                       
           drel=abs(radius(identb)-dr)                                  
           if(drel.lt.radmin) then                                      
             radmin=drel                                                
             i2=ib                                                      
           end if                                                       
   10   continue                                                        
        if(i2.eq.0) return                                              
        itp1=ityp(i1)                                                   
        itp2=ityp(i2)                                                   
        if(itp1.eq.-9999) return                                        
        if(itp2.eq.-9999) return                                        
        ncoli1=nclcnt(issn(i1))                                         
        ncoli2=nclcnt(issn(i2))                                         
        npid1=npidec(i1)                                                
        npid2=npidec(i2)                                                
        iii=max0(itp1,itp2)                                             
        jjj=min0(itp1,itp2)                                             
        icltyp=(iii*(iii-1))/2+jjj                                      
        if((icltyp.ge.3571).and.(icltyp.le.4950)) then                  
           icltyp=maxtyp+1                                              
       end if                                                           
       if((icltyp.le.0).or.(icltyp.gt.maxtyp+1)) then                   
         call errex('wrong icltyp in hit34')                            
       end if                                                           
       do 18 i=0,3                                                      
       plk(i)= p(i,i1)+p(i,i2)                                          
   18 continue                                                          
      splksq= plk(0)*plk(0)                                             
      do 19 i=1,3                                                       
      splksq=splksq-plk(i)*plk(i)                                       
   19 continue                                                          
      if(ida(2).eq.1.and.splksq.lt.16.) return                          
      if(nt.gt.2.and.((itripl.eq.1.and..not.switch(17))                 
     a             .or.ida(34).gt.0.or.switch(23).or.switch(6)).and.    
     b                              (ncoli1.eq.0.or.ncoli2.eq.0))then   
         ifreez=1                                                       
      else                                                              
         ifreez=0                                                       
      end if                                                            
      s=splksq                                                          
      if (splksq.lt.(em(i1)+em(i2)+ekinmi)**2) return                   
        srt=sqrt(s)                                                     
        pi1sq=p(0,i1)*p(0,i1)                                           
        do 38 i=1,3                                                     
   38   pi1sq=pi1sq-p(i,i1)*p(i,i1)                                     
        pi2sq=p(0,i2)*p(0,i2)                                           
        do 39 i=1,3                                                     
   39   pi2sq=pi2sq-p(i,i2)*p(i,i2)                                     
        prel=pcmsr(srt,sqrt(pi1sq),sqrt(pi2sq))                         
        em1=em(i1)                                                      
        em2=em(i2)                                                      
        call cross1(1,srt,prel,icltyp,sig,em1,em2,                      
     a                           itp1,itp2,npid1,npid2)                 
        enlrge=1.                                                       
        if(ifreez.eq.1) call frzcor(enlrge,i1,i2)                       
      sig=enlrge*sig                                                    
      if(dpmax.gt.1.) then                                              
         sigmax=sigmax/dpmax                                            
      end if                                                            
      if(sig.gt.sigmax) then                                            
         write(8,*) 'hit34 warning:sig>sigmax'                          
      else                                                              
         if(ranf(0).gt.sig/sigmax) return                               
      end if                                                            
      i3=0                                                              
      drmin=10000.                                                      
      do 50 ib=iofset+1,iofset+nrbar(kclst)+nrmes(kclst)                
       if(prop(ib)) go to 50                                            
       if (i1.eq.ib) goto 50                                            
       if(ichbig(ib).eq.3) go to 50                                     
       identb=issn(ib)                                                  
       ncolib=nclcnt(identb)                                            
       if(identb.gt.n1.and.identb.le.nt.and.ncolib.eq.0) then           
       else                                                             
        go to 50                                                        
       end if                                                           
       do 48 i=0,3                                                      
       plk(i)= p(i,i1)+p(i,ib)                                          
       qlk(i)= x(i,i1)-x(i,ib)-(qdot(i,i1)-qdot(i,ib))*dt               
   48 continue                                                          
       dqlksq= qlk(0)*qlk(0)                                            
       splksq= plk(0)*plk(0)                                            
       dqsplk= qlk(0)*plk(0)                                            
       plkvc2=0.                                                        
       do 49 i=1,3                                                      
        dqlksq= dqlksq-qlk(i)*qlk(i)                                    
        dqsplk= dqsplk-qlk(i)*plk(i)                                    
        plkvc2= plkvc2+plk(i)*plk(i)                                    
   49  continue                                                         
       splksq=splksq-plkvc2                                             
       zlk= dqlksq- dqsplk*dqsplk/splksq                                
       if(sqrt(-zlk).lt.drmin) then                                     
         i3=ib                                                          
         drmin=sqrt(-zlk)                                               
       end if                                                           
   50 continue                                                          
      if(i3.eq.0) then                                                  
         write(8,*) 'i3=0 in hit34 '                                    
         return                                                         
      end if                                                            
      do 51 i=0,3                                                       
   51 x(i,i3)=x(i,i2)                                                   
      do 52 i=0,3                                                       
       xind(i)=x(i,i1)-0.5*qdot(i,i1)*dt                                
       pind(i)=qdot(i,i1)                                               
       qdot(i,i2)=0.                                                    
       plk(i)=p(i,i1)+p(i,i2)                                           
   52 continue                                                          
      tlast(i2)=0.                                                      
      tlast(i1)=x(0,i1)-qdot(0,i1)*dt-0.1                               
      bxj=plk(1)/plk(0)                                                 
      byj=plk(2)/plk(0)                                                 
      bzj=plk(3)/plk(0)                                                 
      nsave=nlund                                                       
      if(nsave+2.gt.2000) then                                          
         call errex('nsave too large in hit34')                         
      end if                                                            
      mst1=mst(1)                                                       
      mst2=mst(2)                                                       
      mst(1)=nsave+1                                                    
      mst(2)=nsave+2                                                    
      nlund=nlund+2                                                     
      do 53 i=1,3                                                       
       plund(nsave+1,i)=pind(i)                                         
       plund(nsave+2,i)=xind(i)                                         
   53 continue                                                          
      plund(nsave+1,4)=pind(0)                                          
      plund(nsave+2,4)=xind(0)                                          
      call lurobo(0.,0.,-bxj,-byj,-bzj)                                 
      costet=1-2.*ranf(0)                                               
      sintet=sqrt(1.-costet*costet)                                     
      phi=pi2*ranf(0)                                                   
      xind(1)=costet                                                    
      xind(2)=sintet*sin(phi)                                           
      xind(3)=sintet*cos(phi)                                           
      xprel=0.                                                          
      prelsq=0.                                                         
      do 55 i=1,3                                                       
      xprel=xprel+plund(nsave+1,i)*xind(i)                              
      prelsq=prelsq+plund(nsave+1,i)**2                                 
   55 continue                                                          
      xnorm=0.                                                          
        do 56 i=1,3                                                     
         xind(i)=xind(i)-xprel/prelsq*plund(nsave+1,i)                  
         xnorm=xnorm+xind(i)**2                                         
   56   continue                                                        
      sigt=min(sig,10.*pi*seacut(icltyp)**2)                            
      dr2=0.99*sigt/pi*0.1*(ranf(0))**0.5                               
      do 57 i=1,3                                                       
       xind(i)=xind(i)/sqrt(xnorm/dr2)                                  
       plund(nsave+2,i)=plund(nsave+2,i)+xind(i)                        
   57 continue                                                          
      call lurobo(0.,0.,bxj,byj,bzj)                                    
      do 58 i=1,3                                                       
       x(i,i2)=plund(nsave+2,i)                                         
   58 continue                                                          
      x(0,i2)=plund(nsave+2,4)                                          
      nlund=nsave                                                       
      mst(1)=mst1                                                       
      mst(2)=mst2                                                       
      iorig=0                                                           
      if(hit(iorig,i1,i2,srt,sig,sigel,thit)) then                      
         iclyes=1                                                       
      else                                                              
         call errex('hit=f in hit34')                                   
      end if                                                            
      return                                                            
      end                                                               
      subroutine frzcor(enlrge,i1,i2)                                   
           include 'scatter7'                                           
           include 'partblk7'                                           
          enlrge=1.                                                     
          ident1=issn(i1)                                               
          ncoli1=nclcnt(ident1)                                         
          ident2=issn(i2)                                               
          ncoli2=nclcnt(ident2)                                         
          identa=min(ident1,ident2)                                     
          identb=ident1+ident2-identa                                   
          if(identa.eq.ident1) then                                     
            ncola=ncoli1                                                
            ncolb=ncoli2                                                
          else                                                          
            ncola=ncoli2                                                
            ncolb=ncoli1                                                
          end if                                                        
          if(ident1.le.nt.and.ident2.le.nt) then                        
            if(ident1.le.n1.and.ident2.le.n1) then                      
              if(ncoli1+ncoli2.eq.0) then                               
                identb=identa                                           
              else if(ncolb.eq.0) then                                  
                identa=identb                                           
                identb=ident1+ident2-identa                             
              end if                                                    
            else if(ident1.gt.n1.and.ident2.gt.n1) then                 
              if(ncoli1+ncoli2.eq.0) then                               
                identb=identa                                           
              else if(ncola.eq.0) then                                  
                identa=identb                                           
                identb=ident1+ident2-identa                             
              end if                                                    
            end if                                                      
          else if(identa.gt.nt) then                                    
            identb=identa                                               
          else                                                          
            if(identa.gt.n1) then                                       
              identa=identb                                             
              identb=ident1+ident2-identa                               
            end if                                                      
          end if                                                        
          if(identa.eq.identb) then                                     
          else if(identa.eq.ident1) then                                
            ia=i1                                                       
            ib=i2                                                       
            ncola=ncoli1                                                
            ncolb=ncoli2                                                
          else                                                          
            ia=i2                                                       
            ib=i1                                                       
            ncola=ncoli2                                                
            ncolb=ncoli1                                                
          end if                                                        
          if(identa.eq.identb) then                                     
          else if(ncolb.eq.0.and.identb.le.nt.and.identb.gt.n1+1) then  
            bx=p(1,ia)/p(0,ia)                                          
            by=p(2,ia)/p(0,ia)                                          
            bz=p(3,ia)/p(0,ia)                                          
            b2= max(cvalu7,bx*bx+by*by+bz*bz)                           
            gam=1./sqrt(1.-b2)                                          
            pb=bx*p(1,ib)+by*p(2,ib)+bz*p(3,ib)                         
            gg=(gam-1.)*pb/b2-gam*p(0,ib)                               
            p1=gg*bx+p(1,ib)                                            
            p2=gg*by+p(2,ib)                                            
            p3=gg*bz+p(3,ib)                                            
            p0=gam*(p(0,ib)-pb)                                         
            pabs=sqrt(p1**2+p2**2+p3**2)                                
            veff=pabs/p0                                                
            if(veff.lt.0. .or.veff.ge.1.) then                          
                veff=min(cvalu1,max(cvalu0,veff))                       
            end if                                                      
            if(ncola.eq.0.and.identa.le.n1) then                        
               vcomp=(betata*betata+betapr*betapr                       
     a                         -2.*betata*betapr)/(1.-betata*betapr)**2 
               vcomp=sqrt(vcomp)                                        
            else                                                        
               gamta=1./(1.-betata*betata)                              
               pb=bz*betata*gamta                                       
               gg=(gam-1.)*pb/b2-gam*gamta                              
               p1=gg*bx                                                 
               p2=gg*by                                                 
               p3=gg*bz+betata*gamta                                    
               p0=gam*(gamta-pb)                                        
               pabs=sqrt(p1**2+p2**2+p3**2)                             
               vcomp=pabs/p0                                            
               if(vcomp.lt.0. .or.vcomp.gt.1.) then                     
                  vcomp=min(cvalu1,max(cvalu0,vcomp))                   
               end if                                                   
            end if                                                      
            enlrge=veff/vcomp                                           
          else if(ncola.eq.0.and.identa.le.n1) then                     
            bx=p(1,ib)/p(0,ib)                                          
            by=p(2,ib)/p(0,ib)                                          
            bz=p(3,ib)/p(0,ib)                                          
            b2=max(cvalu7,bx*bx+by*by+bz*bz)                            
            gam=1./sqrt(1.-b2)                                          
            pb=bx*p(1,ia)+by*p(2,ia)+bz*p(3,ia)                         
            gg=(gam-1.)*pb/b2-gam*p(0,ia)                               
            p1=gg*bx+p(1,ia)                                            
            p2=gg*by+p(2,ia)                                            
            p3=gg*bz+p(3,ia)                                            
            p0=gam*(p(0,ia)-pb)                                         
            pabs=sqrt(p1**2+p2**2+p3**2)                                
            veff=pabs/p0                                                
            if(veff.lt.0. .or.veff.gt.1.) then                          
                veff=min(cvalu1,max(cvalu0,veff))                       
            end if                                                      
               gamta=1./(1.-betapr*betapr)                              
               pb=bz*betapr*gamta                                       
               gg=(gam-1.)*pb/b2-gam*gamta                              
               p1=gg*bx                                                 
               p2=gg*by                                                 
               p3=gg*bz+betapr*gamta                                    
               p0=gam*(gamta-pb)                                        
               pabs=sqrt(p1**2+p2**2+p3**2)                             
               vcomp=pabs/p0                                            
               if(vcomp.lt.0. .or.vcomp.gt.1.) then                     
                  vcomp=min(cvalu1,max(cvalu0,vcomp))                   
               end if                                                   
            enlrge=veff/vcomp                                           
          end if                                                        
      return                                                            
      end                                                               
      subroutine substi(ityp,npid)                                      
       if(ityp.lt.85.or.ityp.gt.97)                                     
     a       call errex('wrong input in substi')                        
       npida=iabs(npid)                                                 
       if(npid.ge.61.and.npid.le.64) then                               
         ityp=7-mod(npid,60)                                            
         npid=3                                                         
       else if(npida.eq.23.or.npida.eq.17.or.                           
     a                         npida.eq.33.or.npida.eq.27) then         
         ityp=8+isign(1,npid)*(mod(npida,10)-3)/4                       
         if(npida.eq.23.or.npida.eq.17) then                            
           npid=3                                                       
         else                                                           
           npid=4                                                       
         end if                                                         
       else if(npid.ge.41.and.npid.le.42) then                          
         ityp=12-mod(npid,40)                                           
         npid=3                                                         
       else if(-npid.ge.41.and.-npid.le.42) then                        
       else if(-npid.ge.61.and.-npid.le.64) then                        
       else if(npid.eq.24) then                                         
         ityp=12                                                        
         npid=3                                                         
       else if(npid.eq.57) then                                         
         ityp=13                                                        
         npid=0                                                         
       else if(npida.ge.18.and.npida.le.19) then                        
         ityp=14                                                        
         npid=isign(1,npid)*(20-npida)                                  
       else if(npid.ge.43.and.npid.le.45) then                          
         ityp=15                                                        
         npid=44-npid                                                   
       else                                                             
         ityp=98                                                        
       end if                                                           
      return                                                            
      end                                                               
      subroutine subst2(idir,ityp,npid)                                 
       if(idir.eq.1) then                                               
         npida=-npid                                                    
         if(ityp.ge.3.and.ityp.le.6) then                               
           if(npida.ge.1.and.npida.le.10) then                          
             npid=67-ityp                                               
             ityp=84+npida                                              
           end if                                                       
         else if(ityp.ge.10.and.ityp.le.11) then                        
           if(npida.ge.1.and.npida.le.10) then                          
             npid=52-ityp                                               
             ityp=84+npida                                              
           end if                                                       
         end if                                                         
       else                                                             
         npida=npid                                                     
         if(ityp.ge.85.and.ityp.le.97) then                             
           if(npid.ge.61.and.npid.le.64) then                           
             npid=-(ityp-84)                                            
             ityp=67-npida                                              
           else if(npid.ge.41.and.npid.le.42) then                      
             npid=-(ityp-84)                                            
             ityp=52-npida                                              
           end if                                                       
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function redcqi(imeso,iexpon)                                     
       include 'scatter7'                                               
        if(imeso.eq.1) then                                             
          vcq=2.                                                        
        else if(imeso.eq.0) then                                        
          vcq=3.                                                        
        else                                                            
          call errex('redcqi:wrong imeso')                              
        end if                                                          
        iqid=mod(iabs(iexpon),100)                                      
        iqid1=iqid/10                                                   
        iqid2=mod(iqid,10)                                              
        if(iqid1.eq.1.or.iqid1.eq.2) then                               
          rqsum=1.0                                                     
        else if(iqid1.eq.3) then                                        
          rqsum=0.6                                                     
        else                                                            
          rqsum=0.                                                      
        end if                                                          
        if(iqid2.eq.3) then                                             
          rqsum=rqsum+0.6                                               
        else                                                            
          rqsum=rqsum+1.0                                               
        end if                                                          
        redcqi=rqsum/vcq                                                
      return                                                            
      end                                                               
