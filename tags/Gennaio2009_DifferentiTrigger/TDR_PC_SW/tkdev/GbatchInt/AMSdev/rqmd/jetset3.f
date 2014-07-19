CPAT,JETSET3  .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine ludecy(ip)                                             
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      dimension iflo(4),ifl1(4),pv(10,5),rord(10),ue(3),be(3)           
      common /prinbk/ iprin                                             
                                                                        
      pawt(a,b,c)=sqrt((a**2-(b+c)**2)*(a**2-(b-c)**2))/(2.*a)          
      four(i,j)=p(i,4)*p(j,4)-p(i,1)*p(j,1)-p(i,2)*p(j,2)-p(i,3)*p(j,3) 
                                                                        
      ntry=0                                                            
      nsav=n                                                            
      kfa=iabs(k(ip,2))                                                 
      kfs=isign(1,k(ip,2))                                              
      if(kfa.le.100) then                                               
        idcold=idb(kfa)-1                                               
      else                                                              
        call luiflv(kfa,ifla,iflb,iflc,ksp)                             
        idcold=idb(76+5*ifla+ksp)-1                                     
      endif                                                             
  120 ntry=ntry+1                                                       
      idc=idcold                                                        
  100 rbr=rlu(0)                                                        
  110 idc=idc+1                                                         
      if(rbr.gt.cbr(idc)) goto 110                                      
      mmat=iabs(kdp(4*idc-3))/1000                                      
      if(ntry.gt.1000) then                                             
        mst(24)=mst(24)+1                                               
        mst(25)=5                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      i=n                                                               
      np=0                                                              
      nq=0                                                              
      mbst=0                                                            
      if(mmat.ge.5.and.p(i,4).gt.20.*p(i,5)) mbst=1                     
      do 130 j=1,4                                                      
      pv(1,j)=0.                                                        
  130 if(mbst.eq.0) pv(1,j)=p(ip,j)                                     
      if(mbst.eq.1) pv(1,4)=p(ip,5)                                     
      pv(1,5)=p(ip,5)                                                   
      ps=0.                                                             
      psq=0.                                                            
      nm=0                                                              
                                                                        
      do 140 i1=4*idc-3,4*idc                                           
      kp=mod(kdp(i1),1000)                                              
      if(kp.eq.0) goto 140                                              
      if(iabs(kp).le.100) then                                          
        kfp=kfs*kp                                                      
        if(mod(ktyp(iabs(kp)),10).eq.0) kfp=kp                          
      elseif(iabs(kp).lt.590) then                                      
        kfp=kfs*kp                                                      
        if(kp.eq.500) kfp=kp                                            
      elseif(iabs(kp).eq.590) then                                      
        if(ksp.le.1) kfp=kfs*(-500+iflb)                                
        if(ksp.eq.3) kfp=kfs*(500+10*iflc+iflb)                         
        if(ksp.eq.2.or.ksp.eq.4) kfp=kfs*(500+10*iflb+iflc)             
      elseif(iabs(kp).eq.591) then                                      
        call luifld(-kfs*int(1.+(2.+par(2))*rlu(0)),0,0,kfp,kdump)      
        if(pv(1,5).lt.par(22)+2.*ulmass(2,kfp)) goto 120                
        kfp=mod(kfp,100)+isign(500,kfp)                                 
      elseif(iabs(kp).eq.592) then                                      
        kfp=-kfp                                                        
      endif                                                             
      if(mmat.ge.6.and.mmat.le.8.and.iabs(kfp).ge.500) then             
        nq=nq+1                                                         
        iflo(nq)=mod(kfp,500)                                           
        psq=psq+ulmass(3,iflo(nq))                                      
      elseif(mmat.ge.12.and.np.eq.3) then                               
        nq=nq-1                                                         
        ps=ps-p(i,5)                                                    
        k(i,1)=ip                                                       
        call luifld(mod(kfp,500),0,mod(k(i,2),500),ifldmp,k(i,2))       
        p(i,5)=ulmass(1,k(i,2))                                         
        ps=ps+p(i,5)                                                    
      else                                                              
        i=i+1                                                           
        np=np+1                                                         
        if(iabs(kfp).ge.500) nq=nq+1                                    
        k(i,1)=ip+10000*(nq-2*(nq/2))                                   
        k(i,2)=kfp                                                      
        p(i,5)=ulmass(1+2*(iabs(kfp)/500),kfp)                          
        ps=ps+p(i,5)                                                    
      endif                                                             
  140 continue                                                          
                                                                        
  150 if(mmat.ge.6.and.mmat.le.8) then                                  
        psp=ps                                                          
        cnde=dpar(11)*alog(max((pv(1,5)-ps-psq)/dpar(12),1.1))          
        if(mmat.eq.8) cnde=cnde+dpar(13)                                
  160   ntry=ntry+1                                                     
        if(ntry.gt.1000) then                                           
          mst(24)=mst(24)+1                                             
          mst(25)=5                                                     
          if(mst(23).ge.1) return                                       
        endif                                                           
        gauss=sqrt(-2.*cnde*alog(rlu(0)))*sin(par(72)*rlu(0))           
        nd=0.5+0.5*np+0.25*nq+cnde+gauss                                
        if(nd.lt.np+nq/2.or.nd.lt.2.or.nd.gt.10) goto 160               
        if(mmat.eq.7.and.nd.eq.2) goto 160                              
                                                                        
        do 170 jt=1,4                                                   
  170   ifl1(jt)=iflo(jt)                                               
        if(nd.eq.np+nq/2) goto 190                                      
        do 180 i=n+np+1,n+nd-nq/2                                       
        jt=1+int((nq-1)*rlu(0))                                         
        call luifld(ifl1(jt),0,0,ifl2,k(i,2))                           
  180   ifl1(jt)=-ifl2                                                  
  190   jt=2+2*(nq/4)*int(rlu(0)+0.5)                                   
        if(min(iabs(ifl1(1)),iabs(ifl1(jt))).gt.10.or.(nq.eq.4.and.     
     &  min(iabs(ifl1(3)),iabs(ifl1(6-jt))).gt.10)) goto 160            
        if(max(iabs(ifl1(1)),iabs(ifl1(jt))).gt.100.or.(nq.eq.4.and.    
     &  max(iabs(ifl1(3)),iabs(ifl1(6-jt))).gt.100)) goto 160           
        call luifld(ifl1(1),0,ifl1(jt),ifldmp,k(n+nd-nq/2+1,2))         
        if(nq.eq.4) call luifld(ifl1(3),0,ifl1(6-jt),ifldmp,k(n+nd,2))  
                                                                        
        ps=psp                                                          
        do 200 i=n+np+1,n+nd                                            
        k(i,1)=ip                                                       
        p(i,5)=ulmass(1,k(i,2))                                         
  200   ps=ps+p(i,5)                                                    
        if(ps+dpar(14).gt.pv(1,5)) goto 160                             
                                                                        
      elseif(mmat.eq.5.or.mmat.eq.11) then                              
        ps=ps-p(n+np,5)                                                 
        pqt=(p(n+np,5)+dpar(15))/pv(1,5)                                
        do 210 j=1,5                                                    
        p(n+np,j)=pqt*pv(1,j)                                           
  210   pv(1,j)=(1.-pqt)*pv(1,j)                                        
        nd=np-1                                                         
                                                                        
      else                                                              
        if(np.ge.2.and.ps+dpar(14).gt.pv(1,5)) goto 120                 
        nd=np                                                           
      endif                                                             
                                                                        
      if(nd.eq.1) then                                                  
        do 220 j=1,4                                                    
  220   p(n+1,j)=p(ip,j)                                                
        goto 430                                                        
      endif                                                             
                                                                        
      pv(nd,5)=p(n+nd,5)                                                
      if(nd.eq.2) goto 280                                              
      wtmax=1./dpar(nd-2)                                               
      pmax=pv(1,5)-ps+p(n+nd,5)                                         
      pmin=0.                                                           
      do 230 il=nd-1,1,-1                                               
      pmax=pmax+p(n+il,5)                                               
      pmin=pmin+p(n+il+1,5)                                             
  230 wtmax=wtmax*pawt(pmax,pmin,p(n+il,5))                             
                                                                        
  240 rord(1)=1.                                                        
      do 260 il1=2,nd-1                                                 
      rsav=rlu(0)                                                       
      do 250 il2=il1-1,1,-1                                             
      if(rsav.le.rord(il2)) goto 260                                    
  250 rord(il2+1)=rord(il2)                                             
  260 rord(il2+1)=rsav                                                  
      rord(nd)=0.                                                       
      wt=1.                                                             
      do 270 il=nd-1,1,-1                                               
      pv(il,5)=pv(il+1,5)+p(n+il,5)+(rord(il)-rord(il+1))*(pv(1,5)-ps)  
  270 wt=wt*pawt(pv(il,5),pv(il+1,5),p(n+il,5))                         
      if(wt.lt.rlu(0)*wtmax) goto 240                                   
                                                                        
  280 do 300 il=1,nd-1                                                  
      pa=pawt(pv(il,5),pv(il+1,5),p(n+il,5))                            
      ue(3)=2.*rlu(0)-1.                                                
      phi=par(72)*rlu(0)                                                
      ue(1)=sqrt(1.-ue(3)**2)*cos(phi)                                  
      ue(2)=sqrt(1.-ue(3)**2)*sin(phi)                                  
      do 290 j=1,3                                                      
      p(n+il,j)=pa*ue(j)                                                
  290 pv(il+1,j)=-pa*ue(j)                                              
      p(n+il,4)=sqrt(pa**2+p(n+il,5)**2)                                
  300 pv(il+1,4)=sqrt(pa**2+pv(il+1,5)**2)                              
                                                                        
      do 310 j=1,4                                                      
  310 p(n+nd,j)=pv(nd,j)                                                
      do 340 il=nd-1,1,-1                                               
      do 320 j=1,3                                                      
  320 be(j)=pv(il,j)/pv(il,4)                                           
      ga=pv(il,4)/pv(il,5)                                              
      do 340 i=n+il,n+nd                                                
      bep=be(1)*p(i,1)+be(2)*p(i,2)+be(3)*p(i,3)                        
      do 330 j=1,3                                                      
  330 p(i,j)=p(i,j)+ga*(ga*bep/(1.+ga)+p(i,4))*be(j)                    
  340 p(i,4)=ga*(p(i,4)+bep)                                            
                                                                        
      if(mmat.eq.1) then                                                
        wt=(p(n+1,5)*p(n+2,5)*p(n+3,5))**2-(p(n+1,5)*four(n+2,n+3))**2  
     &  -(p(n+2,5)*four(n+1,n+3))**2-(p(n+3,5)*four(n+1,n+2))**2        
     &  +2.*four(n+1,n+2)*four(n+1,n+3)*four(n+2,n+3)                   
        if(max(wt*dpar(9)/p(ip,5)**6,0.001).lt.rlu(0)) goto 240         
                                                                        
      elseif(mmat.eq.3) then                                            
        if(nm.ne.2) then                                                
          im=mod(k(ip,1),10000)                                         
          if(im.eq.0) goto 360                                          
          do 350 il=max(ip-2,im+1),min(ip+2,n)                          
  350     if(mod(k(il,1),10000).eq.im) nm=nm+1                          
          call luiflv(k(im,2),iflam,iflbm,iflcm,kspm)                   
          if(nm.ne.2.or.kspm.ne.0) goto 360                             
        endif                                                           
        if((p(ip,5)**2*four(im,n+1)-four(ip,im)*four(ip,n+1))**2.le.    
     &  rlu(0)*(four(ip,im)**2-(p(ip,5)*p(im,5))**2)*(four(ip,n+1)**2-  
     &  (p(ip,5)*p(n+1,5))**2)) goto 280                                
  360   nm=0                                                            
                                                                        
      elseif(mmat.ge.11) then                                           
        wt=four(ip,n+1)*four(n+2,n+3)                                   
        if(wt.lt.rlu(0)*p(ip,5)*pv(1,5)**3/dpar(10)) goto 240           
      endif                                                             
                                                                        
      if(mmat.eq.5.or.mmat.eq.11) then                                  
        do 370 j=1,5                                                    
  370   pv(1,j)=pv(1,j)/(1.-pqt)                                        
        nd=nd+1                                                         
      endif                                                             
                                                                        
      if(mmat.eq.5) then                                                
        if(p(n+2,5)**2+p(n+3,5)**2+2.*four(n+2,n+3).ge.                 
     &  (par(22)+ulmass(0,k(n+2,2))+ulmass(0,k(n+3,2)))**2) goto 430    
        k(n+2,1)=ip                                                     
        call luifld(mod(k(n+2,2),500),0,mod(k(n+3,2),500),ifldmp,       
     &  k(n+2,2))                                                       
        p(n+2,5)=ulmass(1,k(n+2,2))                                     
        ps=p(n+1,5)+p(n+2,5)                                            
        pv(2,5)=p(n+2,5)                                                
        mmat=0                                                          
        nd=2                                                            
        goto 280                                                        
      elseif(mmat.eq.11) then                                           
        if(p(n+3,5)**2+p(n+4,5)**2+2.*four(n+3,n+4).ge.                 
     &  (par(22)+ulmass(0,k(n+3,2))+ulmass(0,k(n+4,2)))**2) goto 400    
        k(n+3,1)=ip                                                     
        call luifld(mod(k(n+3,2),500),0,mod(k(n+4,2),500),ifldmp,       
     &  k(n+3,2))                                                       
        p(n+3,5)=ulmass(1,k(n+3,2))                                     
        do 380 j=1,3                                                    
  380   p(n+3,j)=p(n+3,j)+p(n+4,j)                                      
        p(n+3,4)=sqrt(p(n+3,1)**2+p(n+3,2)**2+p(n+3,3)**2+p(n+3,5)**2)  
        ha=p(n+1,4)**2-p(n+2,4)**2                                      
        hb=ha-(p(n+1,5)**2-p(n+2,5)**2)                                 
        hc=(p(n+1,1)-p(n+2,1))**2+(p(n+1,2)-p(n+2,2))**2+               
     &  (p(n+1,3)-p(n+2,3))**2                                          
        hd=(pv(1,4)-p(n+3,4))**2                                        
        he=ha**2-2.*hd*(p(n+1,4)**2+p(n+2,4)**2)+hd**2                  
        hf=hd*hc-hb**2                                                  
        hg=hd*hc-ha*hb                                                  
        hh=(sqrt(hg**2+he*hf)-hg)/(2.*hf)                               
        do 390 j=1,3                                                    
        pcor=hh*(p(n+1,j)-p(n+2,j))                                     
        p(n+1,j)=p(n+1,j)+pcor                                          
  390   p(n+2,j)=p(n+2,j)-pcor                                          
        p(n+1,4)=sqrt(p(n+1,1)**2+p(n+1,2)**2+p(n+1,3)**2+p(n+1,5)**2)  
        p(n+2,4)=sqrt(p(n+2,1)**2+p(n+2,2)**2+p(n+2,3)**2+p(n+2,5)**2)  
        nd=nd-1                                                         
      endif                                                             
                                                                        
  400 if(mmat.ge.11.and.iabs(k(n+1,2)).ge.500) then                     
        pmr=sqrt(max(0.,p(n+1,5)**2+p(n+2,5)**2+2.*four(n+1,n+2)))      
        if(pmr.gt.par(22)+ulmass(0,k(n+1,2))+ulmass(0,k(n+2,2)))        
     &  goto 410                                                        
        call luifld(mod(k(n+1,2),500),0,-isign(1,k(n+1,2)),ifldmp,kf1)  
        call luifld(mod(k(n+2,2),500),0,-isign(1,k(n+2,2)),ifldmp,kf2)  
        psm=ulmass(0,kf1)+ulmass(0,kf2)                                 
        if(mmat.le.12.and.pmr.gt.0.2*par(22)+psm) goto 410              
        if(mmat.eq.13.and.pmr.gt.dpar(14)+psm) goto 410                 
        if(nd.eq.4.or.kfa.eq.11) goto 120                               
        k(n+1,1)=ip                                                     
        call luifld(mod(k(n+1,2),500),0,mod(k(n+2,2),500),ifldmp,       
     &  k(n+1,2))                                                       
        p(n+1,5)=ulmass(0,k(n+1,2))                                     
        k(n+2,2)=k(n+3,2)                                               
        p(n+2,5)=p(n+3,5)                                               
        ps=p(n+1,5)+p(n+2,5)                                            
        pv(2,5)=p(n+3,5)                                                
        mmat=0                                                          
        nd=2                                                            
        goto 280                                                        
      endif                                                             
                                                                        
  410 if(mmat.eq.13) then                                               
        iflo(1)=mod(k(n+1,2),500)                                       
        iflo(2)=mod(k(n+2,2),500)                                       
        k(n+1,1)=k(n+3,1)                                               
        k(n+1,2)=k(n+3,2)                                               
        do 420 j=1,5                                                    
        pv(1,j)=p(n+1,j)+p(n+2,j)                                       
  420   p(n+1,j)=p(n+3,j)                                               
        pv(1,5)=pmr                                                     
        n=n+1                                                           
        np=0                                                            
        nq=2                                                            
        ps=0.                                                           
        psq=ulmass(3,iflo(1))+ulmass(3,iflo(2))                         
        mmat=6                                                          
        goto 150                                                        
      endif                                                             
                                                                        
  430 n=n+nd                                                            
      if(mbst.eq.1) then                                                
        do 440 j=1,3                                                    
  440   be(j)=p(ip,j)/p(ip,4)                                           
        ga=p(ip,4)/p(ip,5)                                              
        do 460 i=nsav+1,n                                               
        bep=be(1)*p(i,1)+be(2)*p(i,2)+be(3)*p(i,3)                      
        do 450 j=1,3                                                    
  450   p(i,j)=p(i,j)+ga*(ga*bep/(1.+ga)+p(i,4))*be(j)                  
  460   p(i,4)=ga*(p(i,4)+bep)                                          
      endif                                                             
      k(ip,1)=k(ip,1)+20000                                             
      return                                                            
      end                                                               
                                                                        
      subroutine luifld(ifl1,ifl2,ifl3,ifl4,kf)                         
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
      common /prinbk/ iprin                                             
      ifla=iabs(ifl1)                                                   
      iflb=iabs(ifl2)                                                   
      iflc=iabs(ifl3)                                                   
      par1=par(1)                                                       
      par2=par(2)                                                       
      par3=par(3)                                                       
      par4=3.*par(4)                                                    
      if(ifla.ge.4.and.ifla.lt.10.and.abs(par(16)-1.).gt.0.1) then      
        par1=2.5*(0.4*par(1))**(1./par(16))                             
        par2=par(2)**(1./par(16))                                       
        par3=par(3)**(1./par(16))                                       
        par4=3.*par(4)**(1./par(16))                                    
      endif                                                             
        par4m=1./(3.*sqrt(par(4)))                                      
                                                                        
      iflg=0                                                            
      ifli=0                                                            
      ifl4=0                                                            
      kf=0                                                              
      mb=1                                                              
      if(ifla.gt.10.or.iflc.gt.10) mb=2                                 
      if(ifla.lt.10.and.iflb.eq.0.and.iflc.eq.0.and.                    
     &(1.+par1)*rlu(0).lt.1.) mb=0                                      
      if(ifla.lt.10.and.iflb.eq.0.and.(iflc+9)/10.eq.1) mb=0            
                                                                        
      if((ifla.gt.100.or.mb.eq.1).and.par(5).gt.0.) then                
        par3m=sqrt(par(3))                                              
        pardm=par(7)/(par(7)+par3m*par(6))                              
        pars0=par(5)*(2.+(1.+par2*par3m*par(7))*(1.+par4m))             
        pars1=par(7)*pars0/(2.*par3m)+par(5)*(par(6)*(1.+par4m)+        
     &  par2*par3m*par(6)*par(7))                                       
        pars2=par(5)*2.*par(6)*par(7)*(par2*par(7)+(1.+par4m)/par3m)    
        parsm=max(pars0,pars1,pars2)                                    
        par4=par4*(1.+parsm)/(1.+parsm/(3.*par4m))                      
      endif                                                             
                                                                        
      if(mb.eq.0.or.ifla.gt.100) then                                   
        if(mb.eq.0) then                                                
          if(iflc.eq.0) ifl4=isign(1+int((2.+par2)*rlu(0)),-ifl1)       
          ifld=max(ifla,iflc+iabs(ifl4))                                
          ifle=min(ifla,iflc+iabs(ifl4))                                
                                                                        
        else                                                            
  100     iflg=mod(ifla,10)+int(rlu(0)+0.5)*((ifla-100)/10-mod(ifla,10))
          iflh=mod(ifla,10)+(ifla-100)/10-iflg                          
          if((iflg.eq.3.and.rlu(0).gt.pardm).or.(iflh.eq.3.and.rlu(0).  
     &    lt.pardm)) then                                               
            ifli=iflg                                                   
            iflg=iflh                                                   
            iflh=ifli                                                   
          endif                                                         
          ifli=1+int((2.+par2*par3m*par(7))*rlu(0))                     
          if((iflh.ne.ifli.and.rlu(0).gt.(1.+par4m)/max(2.,1.+par4m)).  
     &    or.(iflh.eq.ifli.and.rlu(0).gt.2./max(2.,1.+par4m))) goto 100 
          ifld=max(iflg,ifli)                                           
          ifle=min(iflg,ifli)                                           
          ifl4=isign(10*min(ifli,iflh)+max(ifli,iflh)+9*int(rlu(0)+     
     &    1./(1.+par4m))*iabs(ifli-iflh),-ifl1)                         
          mst(33)=ifli                                                  
        endif                                                           
                                                                        
        ksp=int(par(8)+rlu(0))                                          
        if(ifld.eq.3.and.ifld.ne.ifle)                                  
     a                      ksp=int(par(9)+rlu(0))                      
        if(ifld.ge.4) ksp=int(par(10)+rlu(0))                           
        if(ifld.ne.ifle) then                                           
          kf=isign(kfr(8*ksp+ifld)+ifle,(ifl1+ifl3+ifl4)*(2*ifld-7))    
          if(ifla.gt.100.and.ifli.gt.iflg) kf=-kf                       
        else                                                            
          rfr=rlu(0)                                                    
          if(ifld.le.3) kf=23+10*ksp+int(rfr+cfr(6*ksp+2*ifld-1))+      
     &    int(rfr+cfr(6*ksp+2*ifld))                                    
          if(ifld.eq.4) kf=26+10*ksp                                    
          if(ifld.ge.5) kf=78+4*ksp+ifld                                
        endif                                                           
                                                                        
      else                                                              
  110   if(ifla.lt.10.and.iflb.eq.0.and.iflc.eq.0) then                 
          mb=3                                                          
          ifld=ifla                                                     
  120     ifle=1+int((2.+par2*par3)*rlu(0))                             
          iflf=1+int((2.+par2*par3)*rlu(0))                             
          if(ifle.ge.iflf.and.par4.lt.rlu(0)) goto 120                  
          if(ifle.lt.iflf.and.par4*rlu(0).gt.1.) goto 120               
          ifl4=isign(10*ifle+iflf,ifl1)                                 
                                                                        
        elseif(ifla.lt.10.and.iflb.eq.0) then                           
          ifld=ifla                                                     
          ifle=iflc/10                                                  
          iflf=mod(iflc,10)                                             
                                                                        
        elseif(ifla.lt.10) then                                         
          ifld=iflb                                                     
          if(iflc.eq.0) ifl4=isign(1+int((2.+par2)*rlu(0)),ifl1)        
          ifle=ifla+int(rlu(0)+0.5)*(iflc+iabs(ifl4)-ifla)              
          iflf=ifla+iflc+iabs(ifl4)-ifle                                
                                                                        
        else                                                            
          if(iflc.eq.0) ifl4=isign(1+int((2.+par2)*rlu(0)),ifl1)        
          ifld=iflc+iabs(ifl4)                                          
          ifle=ifla/10                                                  
          iflf=mod(ifla,10)                                             
        endif                                                           
                                                                        
        lfr=3+2*((2*(ifle-iflf))/(1+iabs(ifle-iflf)))                   
        if(ifld.ne.ifle.and.ifld.ne.iflf) lfr=lfr+1                     
        wt=cfr(2*lfr+11)+par(11)*cfr(2*lfr+12)                          
        if(mb.eq.1.and.ifle.lt.iflf) wt=wt/3.                           
        if(mb.eq.1.and.iflb.ne.0) wt=0.75*wt                            
        if(mb.eq.3.and.par(5).gt.0.) then                               
          wtdq=pars0                                                    
          if(max(ifle,iflf).eq.3) wtdq=pars1                            
          if(min(ifle,iflf).eq.3) wtdq=pars2                            
          if(ifle.lt.iflf) wtdq=wtdq/(3.*par4m)                         
          if((1.+wtdq)*rlu(0).gt.1.) ifl4=ifl4+isign(100,ifl1)          
          if(ifle.ge.iflf) wt=wt*(1.+wtdq)/(1.+parsm)                   
          if(ifle.lt.iflf) wt=wt*(1.+wtdq)/(1.+parsm/(3.*par4m))        
        endif                                                           
        if(iflb.ne.0.and.wt.lt.rlu(0)) return                           
        if(iflb.eq.0.and.iflc.eq.0.and.wt.lt.rlu(0)) goto 110           
                                                                        
        iflg=max(ifld,ifle,iflf)                                        
        ifli=min(ifld,ifle,iflf)                                        
        iflh=ifld+ifle+iflf-iflg-ifli                                   
        ksp=2+2*int(1.-cfr(2*lfr+11)+(cfr(2*lfr+11)+par(11)*            
     &  cfr(2*lfr+12))*rlu(0))                                          
                                                                        
        if(ksp.eq.2.and.iflg.gt.iflh.and.iflh.gt.ifli) then             
          if(ifle.gt.iflf.and.ifld.ne.iflg) ksp=2+int(0.75+rlu(0))      
          if(ifle.lt.iflf.and.ifld.eq.iflg) ksp=3                       
          if(ifle.lt.iflf.and.ifld.ne.iflg) ksp=2+int(0.25+rlu(0))      
        endif                                                           
                                                                        
        kf=isign(kfr(16*ksp-16+iflg)+kfr(16*ksp-8+iflh)+ifli,ifl1)      
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
