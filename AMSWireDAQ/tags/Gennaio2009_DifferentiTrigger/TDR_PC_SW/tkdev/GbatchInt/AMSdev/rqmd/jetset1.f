CPAT,JETSET1  .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine lupart(ip,kf,pe,the,phi)                               
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      if(mst(19).ge.1) call lulist(-1)                                  
      if(ip.ge.mst(30)-5-mst(31)) mst(26)=1                             
      ir=max(ip,1)                                                      
      k(ir,1)=0                                                         
      k(ir,2)=kf                                                        
      if(mst(9).eq.0) p(ir,5)=ulmass(1,kf)                              
      p(ir,4)=max(pe,p(ir,5))                                           
      pa=sqrt(p(ir,4)**2-p(ir,5)**2)                                    
      p(ir,1)=pa*sin(the)*cos(phi)                                      
      p(ir,2)=pa*sin(the)*sin(phi)                                      
      p(ir,3)=pa*cos(the)                                               
      n=ir                                                              
      if(ip.eq.0) call luexec                                           
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lu1jet(ip,ifl,iflj,ifli,pe,the,phi)                    
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      if(iabs(ifl).gt.100) mst(26)=2                                    
      ir=max(iabs(ip),1)                                                
      call lupart(ir,ifl+isign(500,2*ifl+1),pe,the,phi)                 
      if(ip.lt.0) k(n,1)=10000                                          
                                                                        
      if(iflj.ne.0.or.ifli.ne.0) then                                   
        if(iabs(iflj).gt.10.or.iabs(ifli).gt.10.or.(iflj.ne.0.and.      
     &  iabs(ifl).lt.10).or.ifl*iflj.lt.0.or.(ifli.ne.0.and.ifl.eq.0).  
     &  or.ifl*(10-iabs(ifl))*ifli.gt.0) mst(26)=2                      
        n=ir+1                                                          
        k(n,1)=60000+ir                                                 
        k(n,2)=isign(600+10*iabs(iflj)+iabs(ifli),iflj+ifli)            
        do 100 j=1,5                                                    
  100   p(n,j)=0.                                                       
      endif                                                             
      if(ip.eq.0) call luexec                                           
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lu2jet(ip,ifl1,ifl2,ecm)                               
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      if(iabs(ifl1).gt.100.or.iabs(ifl2).gt.100.or.(ifl1.eq.0.and.      
     &ifl2.ne.0).or.(ifl1.ne.0.and.ifl2.eq.0).or.ifl1*(10-              
     &iabs(ifl1))*ifl2*(10-iabs(ifl2)).gt.0) mst(26)=2                  
      ir=max(iabs(ip),1)                                                
      if(mst(9).eq.0) p(ir,5)=ulmass(2,ifl1)                            
      if(mst(9).eq.0) p(ir+1,5)=ulmass(2,ifl2)                          
      if(ecm.le.p(ir,5)+p(ir+1,5)) mst(26)=4                            
      pe1=0.5*(ecm+(p(ir,5)**2-p(ir+1,5)**2)/ecm)                       
      mst(9)=mst(9)+1                                                   
      call lupart(ir,ifl1+isign(500,2*ifl1+1),pe1,0.,0.)                
      k(n,1)=10000                                                      
      call lupart(ir+1,ifl2+isign(500,2*ifl2+1),ecm-pe1,par(71),0.)     
      mst(9)=mst(9)-1                                                   
      if(ip.eq.0) call luexec                                           
                                                                        
      if(ip.lt.0) then                                                  
        k(ir+2,1)=k(ir+1,1)                                             
        k(ir+2,2)=k(ir+1,2)                                             
        do 100 j=1,5                                                    
  100   p(ir+2,j)=p(ir+1,j)                                             
        do 110 i=ir+1,ir+3,2                                            
        k(i,1)=70000+i-1                                                
        k(i,2)=1000+i-1                                                 
        p(i,3)=0.                                                       
        p(i,4)=0.                                                       
  110   p(i,5)=0.                                                       
        p(ir+1,1)=ir+2                                                  
        p(ir+1,2)=ir+2                                                  
        p(ir+3,1)=ir                                                    
        p(ir+3,2)=ir                                                    
        n=n+2                                                           
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lu3jet(ip,ifl1,ifl3,ecm,x1,x3)                         
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      if(iabs(ifl1).gt.100.or.iabs(ifl3).gt.100.or.(ifl1.eq.0.and.      
     &ifl3.ne.0).or.(ifl1.ne.0.and.ifl3.eq.0).or.ifl1*(10-              
     &iabs(ifl1))*ifl3*(10-iabs(ifl3)).gt.0) mst(26)=2                  
      ir=max(ip,1)                                                      
      if(mst(9).eq.0) p(ir,5)=ulmass(2,ifl1)                            
      if(mst(9).eq.0) p(ir+1,5)=ulmass(2,0)                             
      if(mst(9).eq.0) p(ir+2,5)=ulmass(2,ifl3)                          
      if(0.5*x1*ecm.le.p(ir,5).or.0.5*(2.-x1-x3)*ecm.le.p(ir+1,5).or.   
     &0.5*x3*ecm.le.p(ir+2,5)) mst(26)=4                                
      pa1=sqrt((0.5*x1*ecm)**2-p(ir,5)**2)                              
      pa2=sqrt((0.5*(2.-x1-x3)*ecm)**2-p(ir+1,5)**2)                    
      pa3=sqrt((0.5*x3*ecm)**2-p(ir+2,5)**2)                            
                                                                        
      cthe2=(pa3**2-pa1**2-pa2**2)/(2.*pa1*pa2)                         
      if(abs(cthe2).ge.1.001) mst(26)=4                                 
      if(abs(cthe2).le.1.001) cthe2=max(-1.,min(1.,cthe2))              
      the2=-acos(cthe2)                                                 
      cthe3=(pa2**2-pa1**2-pa3**2)/(2.*pa1*pa3)                         
      if(abs(cthe3).ge.1.001) mst(26)=4                                 
      if(abs(cthe3).le.1.001) cthe3=max(-1.,min(1.,cthe3))              
      the3=acos(cthe3)                                                  
      mst(9)=mst(9)+1                                                   
      call lupart(ir,ifl1+isign(500,2*ifl1+1),0.5*x1*ecm,0.,0.)         
      k(n,1)=10000                                                      
      call lupart(ir+1,500,0.5*(2.-x1-x3)*ecm,the2,0.)                  
      k(n,1)=10000                                                      
      call lupart(ir+2,ifl3+isign(500,2*ifl3+1),0.5*x3*ecm,the3,0.)     
      mst(9)=mst(9)-1                                                   
      if(ip.eq.0) call luexec                                           
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lu4jet(ip,ifl1,ifl2,ifl3,ifl4,ecm,x1,x2,x4,x12,x14)    
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      if(iabs(ifl1).gt.100.or.iabs(ifl2).gt.100.or.iabs(ifl3).gt.100.   
     &or.iabs(ifl4).gt.100) mst(26)=2                                   
      if(ifl2.eq.0.and.(ifl3.ne.0.or.(ifl1.eq.0.and.ifl4.ne.0).or.      
     &(ifl1.ne.0.and.ifl4.eq.0).or.ifl1*(10-iabs(ifl1))*ifl4*           
     &(10-iabs(ifl4)).gt.0)) mst(26)=2                                  
      if(ifl2.ne.0.and.(ifl1.eq.0.or.ifl1*(10-iabs(ifl1))*ifl2*         
     &(10-iabs(ifl2)).gt.0.or.(ifl3.eq.0.and.ifl4.ne.0).or.             
     &(ifl3.ne.0.and.ifl4.eq.0).or.ifl3*(10-iabs(ifl3))*ifl4*           
     &(10-iabs(ifl4)).gt.0)) mst(26)=2                                  
      ir=max(ip,1)                                                      
      if(mst(9).eq.0) p(ir,5)=ulmass(2,ifl1)                            
      if(mst(9).eq.0) p(ir+1,5)=ulmass(2,ifl2)                          
      if(mst(9).eq.0) p(ir+2,5)=ulmass(2,ifl3)                          
      if(mst(9).eq.0) p(ir+3,5)=ulmass(2,ifl4)                          
      if(0.5*x1*ecm.le.p(ir,5).or.0.5*x2*ecm.le.p(ir+1,5).or.           
     &0.5*(2.-x1-x2-x4)*ecm.le.p(ir+2,5).or.0.5*x4*ecm.le.              
     &p(ir+3,5)) mst(26)=4                                              
      pa1=sqrt((0.5*x1*ecm)**2-p(ir,5)**2)                              
      pa2=sqrt((0.5*x2*ecm)**2-p(ir+1,5)**2)                            
      pa3=sqrt((0.5*(2.-x1-x2-x4)*ecm)**2-p(ir+2,5)**2)                 
      pa4=sqrt((0.5*x4*ecm)**2-p(ir+3,5)**2)                            
                                                                        
      x24=x1+x2+x4-1.-x12-x14+(p(ir+2,5)**2-p(ir,5)**2-                 
     &p(ir+1,5)**2-p(ir+3,5)**2)/ecm**2                                 
      cthe4=(x1*x4-2.*x14)*ecm**2/(4.*pa1*pa4)                          
      if(abs(cthe4).ge.1.002) mst(26)=4                                 
      if(abs(cthe4).le.1.002) cthe4=max(-1.,min(1.,cthe4))              
      the4=acos(cthe4)                                                  
      cthe2=(x1*x2-2.*x12)*ecm**2/(4.*pa1*pa2)                          
      if(abs(cthe2).ge.1.002) mst(26)=4                                 
      if(abs(cthe2).le.1.002) cthe2=max(-1.,min(1.,cthe2))              
      the2=acos(cthe2)                                                  
      cthe3=-(pa1+pa2*cthe2+pa4*cthe4)/pa3                              
      if(abs(cthe3).ge.1.002) mst(26)=4                                 
      if(abs(cthe3).le.1.002) cthe3=max(-1.,min(1.,cthe3))              
      the3=acos(cthe3)                                                  
      sgn=(-1.)**int(rlu(0)+0.5)                                        
      cphi2=((x2*x4-2.*x24)*ecm**2-4.*pa2*cthe2*pa4*cthe4)/             
     &(4.*pa2*sin(the2)*pa4*sin(the4))                                  
      if(abs(cphi2).ge.1.05) mst(26)=4                                  
      if(abs(cphi2).le.1.05) cphi2=max(-1.,min(1.,cphi2))               
      phi2=sgn*acos(cphi2)                                              
      cphi3=-(pa2*sin(the2)*cphi2+pa4*sin(the4))/(pa3*sin(the3))        
      if(abs(cphi3).ge.1.05) mst(26)=4                                  
      if(abs(cphi3).le.1.05) cphi3=max(-1.,min(1.,cphi3))               
      phi3=-sgn*acos(cphi3)                                             
                                                                        
      mst(9)=mst(9)+1                                                   
      call lupart(ir,ifl1+isign(500,2*ifl1+1),0.5*x1*ecm,0.,0.)         
      k(n,1)=10000                                                      
      call lupart(ir+1,ifl2+isign(500,2*ifl2+1),0.5*x2*ecm,the2,phi2)   
      if(ifl2.eq.0) k(n,1)=10000                                        
      call lupart(ir+2,ifl3+isign(500,2*ifl3+1),0.5*(2.-x1-x2-x4)*ecm,  
     &the3,phi3)                                                        
      k(n,1)=10000                                                      
      call lupart(ir+3,ifl4+isign(500,2*ifl4+1),0.5*x4*ecm,the4,0.)     
      mst(9)=mst(9)-1                                                   
      if(ip.eq.0) call luexec                                           
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      function klu(i,j)                                                 
      common /lujets/ n,k(2000,2),p(2000,5)                             
                                                                        
      klu=0                                                             
      if(i.lt.0.or.j.le.0) return                                       
      if(i.eq.0.and.j.le.1) then                                        
        klu=n                                                           
      elseif(i.eq.0) then                                               
        do 100 i1=1,n                                                   
        if(j.eq.2.and.k(i1,1).lt.20000) klu=klu+1                       
        if(j.eq.3.and.k(i1,1).lt.20000) klu=klu+luchge(k(i1,2))         
        if(j.ne.3.or.k(i1,1)/10000.ne.6) goto 100                       
        if(k(i1-1,1).lt.20000) klu=klu+luchge(k(i1,2))                  
  100   if(j.eq.4.and.k(i1,1).lt.40000.and.iabs(k(i1,2)).ge.500) klu=   
     &  klu+1                                                           
                                                                        
      elseif(j.le.2) then                                               
        klu=k(i,j)                                                      
      elseif(j.le.4) then                                               
        if(j.eq.3) klu=luchge(k(i,2))                                   
                                                                        
        if(j.eq.4) klu=mod(k(i,1),10000)                                
      elseif(j.le.7) then                                               
        i2=i                                                            
        i1=i                                                            
  110   klu=klu+1                                                       
        i3=i2                                                           
        i2=i1                                                           
        i1=mod(k(i1,1),10000)                                           
        if(i1.gt.0.and.k(i1,1).lt.40000) goto 110                       
        if(j.eq.6) klu=i2                                               
        if(j.eq.7) then                                                 
          klu=0                                                         
          do 120 i1=i2+1,i3                                             
  120     if(mod(k(i1,1),10000).eq.i2.and.k(i1,1).lt.40000) klu=klu+1   
        endif                                                           
                                                                        
      elseif(j.le.9) then                                               
        if(j.eq.8.and.k(i,1).lt.60000.and.iabs(k(i,2)).lt.500) klu=     
     &  k(i,2)                                                          
        if(j.eq.9) klu=1000                                             
        if(j.eq.9.and.k(i,1).lt.60000.and.iabs(k(i,2)).ge.500) klu=     
     &  mod(k(i,2),500)                                                 
                                                                        
      elseif(j.le.13) then                                              
        if(k(i,1).lt.60000) klu=k(i,2)                                  
        if(j.ge.11.and.k(i,1).ge.20000) klu=0                           
        kfa=iabs(k(i,2))                                                
        if(j.ge.12.and.(kfa.eq.8.or.kfa.eq.10.or.kfa.eq.12.or.kfa.eq.   
     &  14)) klu=0                                                      
        if(j.ge.13.and.luchge(kfa).eq.0) klu=0                          
                                                                        
      elseif(j.eq.14) then                                              
        call luiflv(k(i,2),ifla,iflb,iflc,ksp)                          
        if(ksp.ge.0) klu=ifla                                           
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      function plu(i,j)                                                 
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      dimension psum(4)                                                 
                                                                        
      plu=0.                                                            
      if(i.lt.0.or.j.le.0.or.(i.eq.0.and.j.gt.6)) return                
      if(i.eq.0.and.j.le.4) then                                        
        do 100 i1=1,n                                                   
  100   if(k(i1,1).lt.20000) plu=plu+p(i1,j)                            
      elseif(i.eq.0.and.j.eq.5) then                                    
        do 110 j1=1,4                                                   
        psum(j1)=0.                                                     
        do 110 i1=1,n                                                   
  110   if(k(i1,1).lt.20000) psum(j1)=psum(j1)+p(i1,j1)                 
        plu=sqrt(max(0.,psum(4)**2-psum(1)**2-psum(2)**2-psum(3)**2))   
      elseif(i.eq.0) then                                               
        do 120 i1=1,n                                                   
        if(k(i1,1)/10000.ne.6) goto 120                                 
        if(k(i1-1,1).lt.20000) plu=plu+luchge(k(i1,2))/3.               
  120   if(k(i1,1).lt.20000) plu=plu+luchge(k(i1,2))/3.                 
      elseif(j.le.5) then                                               
        plu=p(i,j)                                                      
                                                                        
      elseif(j.le.12) then                                              
        if(j.eq.6) plu=luchge(k(i,2))/3.                                
        if(j.eq.7.or.j.eq.8) plu=p(i,1)**2+p(i,2)**2+p(i,3)**2          
        if(j.eq.9.or.j.eq.10) plu=p(i,1)**2+p(i,2)**2                   
        if(j.eq.11.or.j.eq.12) plu=p(i,5)**2+p(i,1)**2+p(i,2)**2        
        if(j.eq.8.or.j.eq.10.or.j.eq.12) plu=sqrt(plu)                  
                                                                        
      elseif(j.le.16) then                                              
        if(j.le.14) plu=ulangl(p(i,3),sqrt(p(i,1)**2+p(i,2)**2))        
        if(j.ge.15) plu=ulangl(p(i,1),p(i,2))                           
        if(j.eq.14.or.j.eq.16) plu=plu*180./par(71)                     
                                                                        
      elseif(j.le.19) then                                              
        pmr=0.                                                          
        if(j.eq.17) pmr=p(i,5)                                          
        if(j.eq.18) pmr=ulmass(0,17)                                    
        pr=max(1e-20,pmr**2+p(i,1)**2+p(i,2)**2)                        
        plu=sign(alog(min((sqrt(pr+p(i,3)**2)+abs(p(i,3)))/sqrt(pr),    
     &  1e20)),p(i,3))                                                  
                                                                        
      elseif(j.le.25) then                                              
        if(j.eq.20) plu=2.*sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2)/par(75)  
        if(j.eq.21) plu=2.*p(i,3)/par(75)                               
        if(j.eq.22) plu=2.*sqrt(p(i,1)**2+p(i,2)**2)/par(75)            
        if(j.eq.23) plu=2.*p(i,4)/par(75)                               
        if(j.eq.24) plu=(p(i,4)+p(i,3))/par(75)                         
        if(j.eq.25) plu=(p(i,4)-p(i,3))/par(75)                         
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lurobo(the,phi,bex,bey,bez)                            
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      dimension rot(3,3),pv(3)                                          
      double precision dp(4),dbex,dbey,dbez,dga,dbep,dgabep             
                                                                        
      imax=n                                                            
      if(mst(2).gt.0) imax=mst(2)                                       
      if(the**2+phi**2.gt.1e-20) then                                   
        rot(1,1)=cos(the)*cos(phi)                                      
        rot(1,2)=-sin(phi)                                              
        rot(1,3)=sin(the)*cos(phi)                                      
        rot(2,1)=cos(the)*sin(phi)                                      
        rot(2,2)=cos(phi)                                               
        rot(2,3)=sin(the)*sin(phi)                                      
        rot(3,1)=-sin(the)                                              
        rot(3,2)=0.                                                     
        rot(3,3)=cos(the)                                               
        do 120 i=max(1,mst(1)),imax                                     
        if(mod(k(i,1)/10000,10).ge.6) goto 120                          
        do 100 j=1,3                                                    
  100   pv(j)=p(i,j)                                                    
        do 110 j=1,3                                                    
  110   p(i,j)=rot(j,1)*pv(1)+rot(j,2)*pv(2)+rot(j,3)*pv(3)             
  120   continue                                                        
      endif                                                             
                                                                        
      if(bex**2+bey**2+bez**2.gt.1e-20) then                            
        dbex=bex                                                        
        dbey=bey                                                        
        dbez=bez                                                        
        dga=1d0/dsqrt(1d0-dbex**2-dbey**2-dbez**2)                      
        do 140 i=max(1,mst(1)),imax                                     
        if(mod(k(i,1)/10000,10).ge.6) goto 140                          
        do 130 j=1,4                                                    
  130   dp(j)=p(i,j)                                                    
        dbep=dbex*dp(1)+dbey*dp(2)+dbez*dp(3)                           
        dgabep=dga*(dga*dbep/(1d0+dga)+dp(4))                           
        p(i,1)=dp(1)+dgabep*dbex                                        
        p(i,2)=dp(2)+dgabep*dbey                                        
        p(i,3)=dp(3)+dgabep*dbez                                        
        p(i,4)=dga*(dp(4)+dbep)                                         
  140   continue                                                        
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luedit(medit)                                          
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
                                                                        
      if(medit.ge.0.and.medit.le.3) then                                
        imax=n                                                          
        if(mst(2).gt.0) imax=mst(2)                                     
        mnot=0                                                          
        i1=max(1,mst(1))-1                                              
        do 120 i=max(1,mst(1)),imax                                     
        if(mnot.eq.1.and.k(i,1)/20000.eq.3) goto 100                    
        mnot=0                                                          
        if(k(i,1).ge.40000) goto 120                                    
        if(medit.ge.1.and.k(i,1).ge.20000) goto 120                     
        kfa=iabs(k(i,2))                                                
        if(medit.ge.2.and.(kfa.eq.8.or.kfa.eq.10.or.kfa.eq.12.or.       
     &  kfa.eq.14)) goto 120                                            
        if(medit.ge.3.and.kfa.le.499.and.luchge(kfa).eq.0) goto 120     
        if(kfa.ge.500) mnot=1                                           
                                                                        
  100   i1=i1+1                                                         
        k(i1,1)=10000*(k(i,1)/10000)                                    
        k(i1,2)=k(i,2)                                                  
        do 110 j=1,5                                                    
  110   p(i1,j)=p(i,j)                                                  
  120   continue                                                        
        n=i1                                                            
                                                                        
      elseif(medit.eq.-1) then                                          
        if(2*n.ge.mst(30)) then                                         
          mst(26)=1                                                     
          return                                                        
        endif                                                           
        do 130 i=1,n                                                    
        k(mst(30)-i,1)=k(i,1)                                           
        k(mst(30)-i,2)=k(i,2)                                           
        do 130 j=1,5                                                    
  130   p(mst(30)-i,j)=p(i,j)                                           
        mst(31)=n                                                       
                                                                        
      elseif(medit.eq.-2) then                                          
        do 140 i=1,mst(31)                                              
        k(i,1)=k(mst(30)-i,1)                                           
        k(i,2)=k(mst(30)-i,2)                                           
        do 140 j=1,5                                                    
  140   p(i,j)=p(mst(30)-i,j)                                           
        n=mst(31)                                                       
                                                                        
      elseif(medit.eq.-3) then                                          
        i1=0                                                            
        do 150 i=1,n                                                    
        kh=mod(k(i,1),10000)                                            
        if(kh.ge.1) then                                                
          if(k(kh,1)/20000.eq.2) kh=0                                   
        endif                                                           
        if(k(i,1).ge.60000) kh=0                                        
        if(kh.ne.0) goto 160                                            
        i1=i1+1                                                         
  150   if(k(i,1)/20000.eq.1) k(i,1)=k(i,1)-20000                       
  160   n=i1                                                            
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lulist(mlist)                                          
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      common /ludat4/ chag(50),chaf(100)                                
      character chag*4,chaf*4,chap*8,chan*8,chad(4)*8                   
      dimension ps(6)                                                   
                                                                        
      if((mlist.ge.0.and.mlist.le.2).or.(mlist.ge.10.and.mlist.le.12))  
     &then                                                              
        if(mlist.le.2) write(mst(20),1000)                              
        if(mlist.ge.10) write(mst(20),1100)                             
        imax=n                                                          
        if(mst(2).gt.0) imax=mst(2)                                     
        do 100 i=max(1,mst(1)),imax                                     
        call luname(k(i,2),chap)                                        
        mlc=0                                                           
        if(k(i,1)/20000.eq.1) mlc=1                                     
        if(mlc.eq.1.and.iabs(k(i,2)).ge.500) mlc=2                      
        if(k(i,1)/20000.eq.2) mlc=k(i,1)/10000-1                        
        if(mlc.ne.0) chap(8:8)=chag(36)(mlc:mlc)                        
        if(k(i,1).ge.70000) mlc=10                                      
        if(mlist.le.2.and.mlc.lt.10) write(mst(20),1200) i,             
     &  mod(k(i,1),10000),chap,(p(i,j),j=1,5)                           
        if(mlist.ge.10.and.mlc.lt.10) write(mst(20),1300) i,k(i,1),     
     &  k(i,2),chap,(p(i,j),j=1,5)                                      
        if(mlist.le.2.and.mlc.eq.10) write(mst(20),1400) i,k(i,1),      
     &  k(i,2),(p(i,j),j=1,5)                                           
  100   if(mlist.ge.10.and.mlc.eq.10) write(mst(20),1500) i,k(i,1),     
     &  k(i,2),(p(i,j),j=1,5)                                           
                                                                        
        if(mlist.eq.1.or.mlist.eq.11) then                              
          do 110 j=1,6                                                  
  110     ps(j)=plu(0,j)                                                
          if(mlist.eq.1) write(mst(20),1600) ps(6),(ps(j),j=1,5)        
          if(mlist.eq.11) write(mst(20),1700) ps(6),(ps(j),j=1,5)       
        elseif(mlist.eq.2.or.mlist.eq.12) then                          
          do 120 i=n+1,n+mst(3)                                         
          if(mlist.eq.2) write(mst(20),1400) i,k(i,1),k(i,2),           
     &    (p(i,j),j=1,5)                                                
  120     if(mlist.eq.12) write(mst(20),1500) i,k(i,1),k(i,2),          
     &    (p(i,j),j=1,5)                                                
        endif                                                           
                                                                        
      elseif(mlist.eq.3) then                                           
        write(mst(20),1800)                                             
        kf=max(1,mst(1))-1                                              
  130   kf=kf+1                                                         
        write(mst(20),1900)                                             
                                                                        
  140   call luname(kf,chap)                                            
        call luname(-kf,chan)                                           
        kfa=kf                                                          
        if(kf.gt.100) call luiflv(kf,ifla,iflb,iflc,ksp)                
        if(kf.gt.100) kfa=100+ifla                                      
        pm=ulmass(0,kf)                                                 
        kty=ktyp(kfa)                                                   
        if(kty.lt.10) write(mst(20),2000) kf,chap,chan,kty,pm           
        if(kty.ge.10) write(mst(20),2000) kf,chap,chan,kty,pm,          
     &  pwid(2*(kty/10)-1),pwid(2*(kty/10))                             
                                                                        
        if(kf.gt.100.and.(mst(2).le.0.or.kf.lt.mst(2))) then            
          call luiflv(kf+1-50*(kf/392),ifla1,iflb1,iflc1,ksp1)          
          if(ifla1.eq.ifla) kf=kf+1                                     
          if(ifla1.eq.ifla) goto 140                                    
          kfa=76+5*ifla+ksp                                             
        endif                                                           
                                                                        
        if(idb(kfa).eq.0) goto 170                                      
        idc=idb(kfa)-1                                                  
  150   idc=idc+1                                                       
        mmat=iabs(kdp(4*idc-3))/1000                                    
        if(idc.eq.idb(kfa)) br=100.*cbr(idc)                            
        if(idc.ne.idb(kfa)) br=100.*(cbr(idc)-cbr(idc-1))               
        do 160 j=1,4                                                    
  160   call luname(mod(kdp(4*idc-4+j),1000),chad(j))                   
        write(mst(20),2100) idc,mmat,br,(chad(j),j=1,4)                 
        if(cbr(idc).le.0.99999) goto 150                                
  170   if((mst(2).le.0.and.kf.lt.392).or.(mst(2).gt.0.and.kf.lt.       
     &  mst(2))) goto 130                                               
                                                                        
      elseif(mlist.eq.4) then                                           
        write(mst(20),2200)                                             
        ifl=max(0,mst(1))-1                                             
  180   ifl=ifl+1                                                       
        if(ifl.gt.0.and.mod(ifl-1,10).ge.8) goto 180                    
        call luname(ifl+500,chap)                                       
        call luname(-ifl-500,chan)                                      
        pmc=ulmass(2,ifl)                                               
        pma=ulmass(3,ifl)                                               
        kty=ktyp(100+max(ifl/10,mod(ifl,10)))                           
        if(kty.lt.10) write(mst(20),2300) ifl+500,ifl,chap,chan,kty,    
     &  pmc,pma                                                         
        if(kty.ge.10) write(mst(20),2300) ifl+500,ifl,chap,chan,kty,    
     &  pmc,pma,pwid(2*(kty/10)-1),pwid(2*(kty/10))                     
        if((mst(2).le.0.and.ifl.lt.88).or.(mst(2).gt.0.and.ifl.lt.      
     &  mst(2).and.(mod(ifl,10).ne.8.or.mst(2)-ifl.ge.3))) goto 180     
                                                                        
      elseif(mlist.eq.5) then                                           
        write(mst(20),2400)                                             
        do 190 l=1,20                                                   
  190   write(mst(20),2500) l,mst(l),mst(l+20),par(l),par(l+20),        
     &  par(l+40),par(l+60),dpar(l)                                     
                                                                        
      elseif(mlist.eq.-1) then                                          
        write(mst(20),2600)                                             
        mst(19)=0                                                       
      endif                                                             
                                                                        
 1000 format(///20x,'event listing'//5x,'i     ori   part/jet',7x,      
     &'px',9x,'py',9x,'pz',9x,'e',10x,'m'/)                             
 1100 format(///20x,'event listing (extended)'//5x,'i  k(i,1)  k(i,2)', 
     &3x,'part/jet',7x,'p(i,1)',7x,'p(i,2)',7x,'p(i,3)',7x,'p(i,4)',    
     &7x,'p(i,5)'/)                                                     
 1200 format(2x,i4,1x,i7,3x,a8,5(1x,f10.3))                             
 1300 format(2x,i4,2(1x,i7),3x,a8,5(1x,f12.5))                          
 1400 format(2x,i4,1x,i7,4x,i7,5(1x,f10.3))                             
 1500 format(2x,i4,2(1x,i7),11x,5(1x,f12.5))                            
 1600 format(10x,'sum:',6(1x,f10.3))                                    
 1700 format(16x,'sum:',6(1x,f12.5))                                    
 1800 format(///20x,'particle data table'//4x,'kf    particle   ',      
     &'antipart  ktyp      mass     width     w-cut'/19x,               
     &'idc    mat    b.r.   decay products')                            
 1900 format(10x)                                                       
 2000 format(1x,i5,4x,a8,3x,a8,1x,i5,3(1x,f9.3))                        
 2100 format(17x,i5,3x,'(',i2,')',1x,f7.1,4(3x,a8))                     
 2200 format(///20x,'parton/jet data table'//4x,'kf   ifl     parton',  
     &'    antipar  ktyp    m-cons    m-c.a.     width     w-cut')      
 2300 format(/1x,i5,1x,i5,4x,a8,3x,a8,1x,i4,4(1x,f9.3))                 
 2400 format(///20x,'parameter value table'//5x,'l',4x,'mst(l)',        
     &3x,'&(l+20)',7x,'par(l)',6x,'&(l+20)',6x,'&(l+40)',6x,            
     &'&(l+60)',6x,'dpar(l)'/)                                          
 2500 format(1x,i5,2(1x,i9),5(1x,f12.4))                                
 2600 format(///20x,'the lund monte carlo - jetset version 6.2'/        
     &23x,'last date of change : 30 march 1986')                        
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luupda(mupda,lfn)                                      
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      common /ludat4/ chag(50),chaf(100)                                
      character chag*4,chaf*4,cli*72,cut*12,cwr*12,csa*12,cre*12        
                                                                        
      if(mupda.eq.1) then                                               
        do 120 kf=1,120                                                 
        if(idb(kf).eq.0) then                                           
          ndc=0                                                         
        else                                                            
          idc=idb(kf)-1                                                 
  100     idc=idc+1                                                     
          if(cbr(idc).le.0.99999) goto 100                              
          ndc=idc+1-idb(kf)                                             
          if(kf.ge.2) then                                              
            if(idb(kf).eq.idb(kf-1)) ndc=-1                             
          endif                                                         
        endif                                                           
        kty=ktyp(kf)-10*(ktyp(kf)/10)                                   
        pwi=0.                                                          
        pcu=0.                                                          
        if(ktyp(kf).ge.10) pwi=pwid(2*(ktyp(kf)/10)-1)                  
        if(ktyp(kf).ge.10) pcu=pwid(2*(ktyp(kf)/10))                    
        if(kf.le.100) write(lfn,1000) kf,ndc,kty,pmas(kf),pwi,pcu,      
     &  chaf(kf)                                                        
        if(kf.gt.100) write(lfn,1000) kf,ndc,kty,pmas(kf),pwi,pcu       
        do 110 idc=idb(kf),idb(kf)+ndc-1                                
        mmat=iabs(kdp(4*idc-3))/1000                                    
  110   write(lfn,1100) cbr(idc),mmat,(mod(kdp(4*idc-4+j),1000),j=1,4)  
  120   continue                                                        
                                                                        
      elseif(mupda.eq.2) then                                           
        do 130 i=1,60                                                   
  130   pwid(i)=0.                                                      
        do 140 i=1,400                                                  
  140   cbr(i)=0.                                                       
        do 150 i=1,1600                                                 
  150   kdp(i)=0                                                        
        iwis=0                                                          
        idbs=0                                                          
        do 170 kf=1,120                                                 
        if(kf.le.100) read(lfn,1000) kfa,ndc,ktyp(kf),pmas(kf),pwi,pcu, 
     &  chaf(kf)                                                        
        if(kf.gt.100) read(lfn,1000) kfa,ndc,ktyp(kf),pmas(kf),pwi,pcu  
        if(pwi.ge.0.0005) then                                          
          pwid(2*iwis+1)=pwi                                            
          pwid(2*iwis+2)=pcu                                            
          iwis=iwis+1                                                   
          ktyp(kf)=ktyp(kf)+10*iwis                                     
        endif                                                           
        if(ndc.eq.0) then                                               
          idb(kf)=0                                                     
        elseif(ndc.eq.-1) then                                          
          idb(kf)=idb(kf-1)                                             
        else                                                            
          idb(kf)=idbs+1                                                
          do 160 idc=idbs+1,idbs+ndc                                    
          read(lfn,1100) cbr(idc),mmat,(kdp(4*idc-4+j),j=1,4)           
  160     kdp(4*idc-3)=kdp(4*idc-3)+isign(1000*mmat,kdp(4*idc-3))       
          idbs=idbs+ndc                                                 
        endif                                                           
  170   continue                                                        
                                                                        
      elseif(mupda.eq.3) then                                           
        do 210 ic=1,12                                                  
        ne=120                                                          
        if(ic.eq.3) ne=60                                               
        if(ic.eq.5.or.ic.eq.6) ne=200                                   
        if(ic.ge.7.and.ic.le.11) ne=320                                 
        if(ic.eq.12) ne=100                                             
        cli=' '                                                         
        if(ic.eq.1) cli(7:16)='data ktyp/'                              
        if(ic.eq.2) cli(7:16)='data pmas/'                              
        if(ic.eq.3) cli(7:16)='data pwid/'                              
        if(ic.eq.4) cli(7:15)='data idb/'                               
        if(ic.eq.5) cli(7:28)='data (cbr(j),j=1,200)/'                  
        if(ic.eq.6) cli(7:30)='data (cbr(j),j=201,400)/'                
        if(ic.eq.7) cli(7:28)='data (kdp(j),j=1,320)/'                  
        if(ic.eq.8) cli(7:30)='data (kdp(j),j=321,640)/'                
        if(ic.eq.9) cli(7:30)='data (kdp(j),j=641,960)/'                
        if(ic.eq.10) cli(7:31)='data (kdp(j),j=961,1280)/'              
        if(ic.eq.11) cli(7:32)='data (kdp(j),j=1281,1600)/'             
        if(ic.eq.12) cli(7:16)='data chaf/'                             
        lct=16                                                          
        if(ic.eq.4) lct=15                                              
        if(ic.eq.5.or.ic.eq.7) lct=28                                   
        if(ic.eq.6.or.ic.eq.8.or.ic.eq.9) lct=30                        
        if(ic.eq.10) lct=31                                             
        if(ic.eq.11) lct=32                                             
        csa='start'                                                     
        do 200 ie=1,ne                                                  
        if(ic.eq.1) write(cut,1200) ktyp(ie)                            
        if(ic.eq.2) write(cut,1300) pmas(ie)                            
        if(ic.eq.3) write(cut,1300) pwid(ie)                            
        if(ic.eq.4) write(cut,1200) idb(ie)                             
        if(ic.eq.5) write(cut,1300) cbr(ie)                             
        if(ic.eq.6) write(cut,1300) cbr(200+ie)                         
        if(ic.eq.7) write(cut,1200) kdp(ie)                             
        if(ic.eq.8) write(cut,1200) kdp(320+ie)                         
        if(ic.eq.9) write(cut,1200) kdp(640+ie)                         
        if(ic.eq.10) write(cut,1200) kdp(960+ie)                        
        if(ic.eq.11) write(cut,1200) kdp(1280+ie)                       
        if(ic.eq.12) cut=chaf(ie)                                       
        cwr=' '                                                         
        la=1                                                            
        lb=1                                                            
        do 180 ll=1,12                                                  
        if(cut(13-ll:13-ll).ne.' ') la=13-ll                            
  180   if(cut(ll:ll).ne.' ') lb=ll                                     
        lon=1+lb-la                                                     
        cwr(1:lon)=cut(la:lb)                                           
        if(ic.eq.12) then                                               
          cut=cwr                                                       
          cwr(1:lon+2)=''''//cut(1:lon)//''''                           
          lon=lon+2                                                     
        elseif(ic.eq.2.or.ic.eq.3.or.ic.eq.5.or.ic.eq.6) then           
          lon=lon+1                                                     
  190     lon=lon-1                                                     
          if(cwr(lon:lon).eq.'0') goto 190                              
          if(lon.eq.1) cwr(1:2)='0.'                                    
          if(lon.eq.1) lon=2                                            
        endif                                                           
        if(cwr.ne.csa) then                                             
          iag=1                                                         
          csa=cwr                                                       
        else                                                            
          lex=lon+1                                                     
          if(iag.ge.2) lex=lon+3                                        
          if(iag.ge.10) lex=lon+4                                       
          if(iag.ge.100) lex=lon+5                                      
          lct=lct-lex                                                   
          iag=iag+1                                                     
          write(cre,1200) iag                                           
          lex=1                                                         
          if(iag.ge.10) lex=2                                           
          if(iag.ge.100) lex=3                                          
          cut=cwr                                                       
          cwr(1:lex+1+lon)=cre(13-lex:12)//'*'//cut(1:lon)              
          lon=lon+lex+1                                                 
        endif                                                           
        if(lct+lon.gt.70) then                                          
          cli(lct+1:72)=' '                                             
          write(lfn,1400) cli                                           
          cli=' '                                                       
          cli(6:6)='&'                                                  
          lct=6                                                         
        endif                                                           
        cli(lct+1:lct+lon)=cwr(1:lon)                                   
        lct=lct+lon+1                                                   
        if(ie.lt.ne) cli(lct:lct)=','                                   
  200   if(ie.eq.ne) cli(lct:lct)='/'                                   
  210   write(lfn,1400) cli                                             
      endif                                                             
                                                                        
 1000 format(3i5,3f12.5,2x,a4)                                          
 1100 format(5x,f12.5,5i5)                                              
 1200 format(i12)                                                       
 1300 format(f12.5)                                                     
 1400 format(a72)                                                       
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luexec                                                 
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      common/prinbk/ iprin                                              
      dimension psum(2,5)                                               
                                                                        
      if(mst(19).ge.1) call lulist(-1)                                  
      mst(2)=0                                                          
      mst(3)=0                                                          
      nerr=mst(24)                                                      
      mst(25)=0                                                         
      mst(32)=0                                                         
      mst(34)=mst(34)+1                                                 
      par(75)=0.                                                        
      do 100 i=1,n                                                      
  100 if(mod(k(i,1),10000).eq.0) par(75)=par(75)+p(i,4)                 
                                                                        
      do 110 i=1,2                                                      
      do 110 j=1,5                                                      
  110 psum(i,j)=0.                                                      
      icon=0                                                            
      do 130 i=1,n                                                      
      if(k(i,1).ge.20000) goto 130                                      
      do 120 j=1,4                                                      
  120 psum(1,j)=psum(1,j)+p(i,j)                                        
      psum(1,5)=psum(1,5)+luchge(k(i,2))                                
      if(i.eq.n) goto 130                                               
      if(k(i+1,1)/10000.eq.6) psum(1,5)=psum(1,5)+luchge(k(i+1,2))      
  130 continue                                                          
                                                                        
      call luprep                                                       
      mst(1)=0                                                          
      if(mst(23).eq.1.and.mst(26).ne.0.and.mst(35).lt.5) then           
        mst(35)=mst(35)+1                                               
        if(mst(26).ne.3)                                                
     a  write(mst(20),1000) mst(34)                                     
        if(mst(26).eq.1) write(mst(20),1100)                            
        if(mst(26).eq.2) write(mst(20),1200)                            
        if(mst(26).eq.4) write(mst(20),1400)                            
        mst(26)=0                                                       
      endif                                                             
                                                                        
      ip=0                                                              
  140 ip=ip+1                                                           
      if(k(ip,1).ge.20000) then                                         
      elseif(iabs(k(ip,2)).lt.500) then                                 
        kfa=iabs(k(ip,2))                                               
        if(kfa.gt.100) call luiflv(kfa,ifla,iflb,iflc,ksp)              
        if(kfa.gt.100) kfa=76+5*ifla+ksp                                
        if(mst(7).eq.1.and.idb(kfa).ge.1) call ludecy(ip)               
      elseif(iabs(k(ip,2)).le.600) then                                 
        mos=min(mst(5),2)                                               
        if(mos.eq.2.and.mst(6).gt.0) mos=3                              
        if(mst(5).eq.1.and.k(ip,1).lt.10000) mos=2                      
        if(mos.eq.1) call lusysj(ip)                                    
        if(mos.eq.2) call luonej(ip)                                    
        if(mos.eq.3) call lucons(ip)                                    
        if(mos.eq.2) icon=1                                             
        if(mos.eq.3.and.(mst(6).le.0.or.mod(mst(6),5).eq.0)) icon=1     
      endif                                                             
                                                                        
      if(n.ge.mst(30)-20-mst(31).and.ip.lt.n.and.mst(24).eq.nerr)       
     &then                                                              
        mst(24)=mst(24)+1                                               
        mst(25)=1                                                       
      endif                                                             
      if((mst(23).eq.1.and.mst(24).gt.nerr).or.(mst(23).ge.2.and.       
     &mst(24).ge.2)) then                                               
        write(mst(20),1500) mst(24),mst(34)                             
        if(mst(25).eq.1) write(mst(20),1100)                            
        if(mst(25).eq.2) write(mst(20),1200)                            
        if(mst(25).eq.3) write(mst(20),1300)                            
        if(mst(25).eq.4) write(mst(20),1600)                            
        if(mst(25).eq.5) write(mst(20),1700)                            
      endif                                                             
      if((mst(23).eq.1.and.mst(24).ge.5).or.(mst(23).ge.2.and.          
     &mst(24).ge.2)) then                                               
        write(mst(20),1800)                                             
        mst(1)=0                                                        
        mst(2)=0                                                        
        call lulist(11)                                                 
        stop                                                            
      elseif(mst(23).ge.1.and.mst(24).gt.nerr) then                     
        return                                                          
      endif                                                             
      if(ip.lt.n) goto 140                                              
                                                                        
      do 160 i=1,n                                                      
      if(k(i,1).ge.20000) goto 160                                      
      do 150 j=1,4                                                      
  150 psum(2,j)=psum(2,j)+p(i,j)                                        
      psum(2,5)=psum(2,5)+luchge(k(i,2))                                
      if(i.eq.n) goto 160                                               
      if(k(i+1,1)/10000.eq.6) psum(2,5)=psum(2,5)+luchge(k(i+1,2))      
  160 continue                                                          
      pdev=(abs(psum(2,1)-psum(1,1))+abs(psum(2,2)-psum(1,2))+          
     &abs(psum(2,3)-psum(1,3))+abs(psum(2,4)-psum(1,4)))/               
     &(1.+abs(psum(2,4))+abs(psum(1,4)))                                
      if(icon.eq.0.and.(pdev.gt.par(74).or.abs(psum(2,5)-psum(1,5)).    
     &gt.0.25)) then                                                    
        mst(24)=mst(24)+1                                               
        mst(25)=6                                                       
      endif                                                             
      if((mst(23).eq.1.and.mst(24).gt.nerr).or.(mst(23).ge.2.and.       
     &mst(24).ge.2)) then                                               
        write(mst(20),1500) mst(24),mst(34)                             
        write(mst(20),1900) ((psum(i,j),j=1,4),psum(i,5)/3.,i=1,2)      
      endif                                                             
      if((mst(23).eq.1.and.mst(24).ge.5).or.(mst(23).ge.2.and.          
     &mst(24).ge.2)) then                                               
        write(mst(20),1800)                                             
        call lulist(11)                                                 
        stop                                                            
      endif                                                             
                                                                        
 1000 format(/5x,'warning  mst(26) flag was set at luexec ',            
     &'call no',i8,'; error type is')                                   
 1100 format(5x,'1: not enough memory available in commonblock lujets') 
 1200 format(5x,'2: unphysical flavour setup of jet system')            
 1300 format(5x,'3: not enough energy available in jet system ',        
     &'(string fragmentation)')                                         
 1400 format(5x,'4: inconsistent kinematics for definition of jet ',    
     &'configuration')                                                  
 1500 format(/5x,'warning  error no',i2,' has occured in luexec ',      
     &'call no',i8,'; error type is')                                   
 1600 format(5x,'4: not enough energy available in jet system ',        
     &'(independent fragmentation)')                                    
 1700 format(5x,'5: no kinematically allowed decays are found for ',    
     &'this particle')                                                  
 1800 format(5x,'execution will be stopped after printout of ',         
     &'event listing')                                                  
 1900 format(5x,'6: momentum, energy and/or charge were not conserved'/ 
     &5x,'sum of',9x,'px',11x,'py',11x,'pz',11x,'e',8x,'charge'/        
     &5x,'before',2x,4(1x,f12.5),1x,f8.2/5x,'after',3x,4(1x,f12.5),1x,  
     &f8.2)                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luprep                                                 
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      dimension ps(5),pc(5),ue(3)                                       
                                                                        
      ns=n                                                              
      do 120 iqg=1,2                                                    
      do 120 i=1,ns-1                                                   
      if(k(i+1,1)/10000.ne.7.or.k(i,1).ge.20000.or.iabs(k(i,2)).lt.     
     &500.or.(iqg.eq.1.and.iabs(k(i,2)).eq.500)) goto 120               
                                                                        
      kcs=(3-isign(1,k(i,2)*(510-iabs(k(i,2)))))/2                      
      ia=i                                                              
      nl=0                                                              
  100 nl=nl+1                                                           
      if(nl.gt.2*ns) then                                               
        mst(26)=2                                                       
        return                                                          
      endif                                                             
      if(k(ia,1).lt.20000) then                                         
        n=n+1                                                           
        if(n.ge.mst(30)-5-mst(31)) then                                 
          mst(26)=1                                                     
          return                                                        
        endif                                                           
        k(n,1)=10000                                                    
        if(nl.ge.2.and.iabs(k(ia,2)).gt.500) k(n,1)=0                   
        k(n,1)=k(n,1)+max(0,k(ia+1,2)-1000)                             
        k(n,2)=k(ia,2)                                                  
        do 110 j=1,5                                                    
  110   p(n,j)=p(ia,j)                                                  
        k(ia,1)=k(ia,1)+20000                                           
        if(k(n,1).lt.10000) goto 120                                    
      endif                                                             
                                                                        
      ib=ia                                                             
      if(p(ib+1,kcs+2).gt.0.5) then                                     
        ia=nint(p(ib+1,kcs+2))                                          
        p(ib+1,kcs+2)=-p(ib+1,kcs+2)                                    
        mm=0                                                            
      else                                                              
        if(p(ib+1,kcs).lt.0.5) kcs=3-kcs                                
        ia=nint(p(ib+1,kcs))                                            
        p(ib+1,kcs)=-p(ib+1,kcs)                                        
        mm=1                                                            
      endif                                                             
      if(ia.le.0.or.ia.gt.min(ns,mst(30)-mst(31))) then                 
        mst(26)=2                                                       
        return                                                          
      endif                                                             
      if(nint(p(ia+1,1)).eq.ib.or.nint(p(ia+1,2)).eq.ib) then           
        if(mm.eq.1) kcs=3-kcs                                           
        if(nint(p(ia+1,kcs)).ne.ib) kcs=3-kcs                           
        p(ia+1,kcs)=-p(ia+1,kcs)                                        
      else                                                              
        if(mm.eq.0) kcs=3-kcs                                           
        if(nint(p(ia+1,kcs+2)).ne.ib) kcs=3-kcs                         
        p(ia+1,kcs+2)=-p(ia+1,kcs+2)                                    
      endif                                                             
      if(ia.ne.i) goto 100                                              
      k(n,1)=k(n,1)-10000                                               
  120 continue                                                          
                                                                        
      if(mst(21).ge.1) then                                             
        i1=0                                                            
        do 140 i=1,n                                                    
        ks=k(i,1)/10000                                                 
        if(ks.eq.7.or.(mst(21).ge.3.and.ks.ge.2.and.ks.le.5)) goto 140  
        if(ks.ge.2.and.i.lt.n.and.k(i+1,1)/10000.eq.7) then             
          if(mst(21).ge.2.and.ks.ne.6) goto 140                         
          if(ks.le.3.and.i.gt.mst(1)) goto 140                          
          if(ks.le.3) k(i,1)=40000                                      
        endif                                                           
        i1=i1+1                                                         
        k(i1,1)=k(i,1)                                                  
        k(i1,2)=k(i,2)                                                  
        do 130 j=1,5                                                    
  130   p(i1,j)=p(i,j)                                                  
  140   continue                                                        
        n=i1                                                            
      endif                                                             
                                                                        
      if(mst(12).le.0) goto 310                                         
      ns=n                                                              
  150 nsin=n-ns                                                         
      pdm=1.+par(22)                                                    
      ic=0                                                              
      do 200 i=1,ns                                                     
      if(k(i,1).ge.20000.or.iabs(k(i,2)).lt.500) goto 200               
      if(k(i,1).ge.10000.and.ic.eq.0) then                              
        nsin=nsin+1                                                     
        ic=i                                                            
        do 160 j=1,4                                                    
  160   ps(j)=p(i,j)                                                    
        ps(5)=ulmass(0,k(i,2))                                          
      elseif(k(i,1).ge.10000) then                                      
        do 170 j=1,4                                                    
  170   ps(j)=ps(j)+p(i,j)                                              
      elseif(ic.ne.0) then                                              
        do 180 j=1,4                                                    
  180   ps(j)=ps(j)+p(i,j)                                              
        ps(5)=ps(5)+ulmass(0,k(i,2))                                    
        pd=sqrt(max(0.,ps(4)**2-ps(1)**2-ps(2)**2-ps(3)**2))-ps(5)      
        if(pd.lt.pdm) then                                              
          pdm=pd                                                        
          do 190 j=1,5                                                  
  190     pc(j)=ps(j)                                                   
          icl=ic                                                        
          icu=i                                                         
        endif                                                           
        ic=0                                                            
      endif                                                             
  200 continue                                                          
      if(pdm.ge.par(22).or.nsin.eq.1) goto 310                          
                                                                        
      pcm=sqrt(max(0.,pc(4)**2-pc(1)**2-pc(2)**2-pc(3)**2))             
      k(n+1,1)=icl                                                      
      k(n+2,1)=icu                                                      
      if(k(icl+1,1)/10000.eq.6.or.(icu.lt.n.and.k(icu+1,1)/10000.       
     &eq.6)) then                                                       
        goto 310                                                        
      elseif(iabs(k(icl,2)).gt.500) then                                
        if(mod(k(icl,2),500)*mod(k(icu,2),500)*(510-iabs(k(icl,2)))*    
     &  (510-iabs(k(icu,2))).ge.0) goto 310                             
  210   call luifld(mod(k(icl,2),500),0,0,ifln,k(n+1,2))                
        if(iabs(ifln).ge.100.or.(iabs(ifln).gt.10.and.iabs(k(icu,2)).   
     &  gt.510)) goto 210                                               
        call luifld(mod(k(icu,2),500),0,-ifln,ifldmp,k(n+2,2))          
      else                                                              
        if(iabs(k(icu,2)).ne.500) goto 310                              
  220   call luifld(1+int((2.+par(2))*rlu(0)),0,0,ifln,kdump)           
        if(iabs(ifln).ge.100) goto 220                                  
        call luifld(ifln,0,0,iflm,k(n+1,2))                             
        if(iabs(iflm).ge.100) goto 220                                  
        call luifld(-ifln,0,-iflm,ifldmp,k(n+2,2))                      
      endif                                                             
      p(n+1,5)=ulmass(1,k(n+1,2))                                       
      p(n+2,5)=ulmass(1,k(n+2,2))                                       
      if(p(n+1,5)+p(n+2,5)+dpar(14).ge.pcm) goto 260                    
                                                                        
      if(pcm.ge.0.02*pc(4)) then                                        
        pa=sqrt((pcm**2-(p(n+1,5)+p(n+2,5))**2)*(pcm**2-                
     &  (p(n+1,5)-p(n+2,5))**2))/(2.*pcm)                               
        ue(3)=2.*rlu(0)-1.                                              
        phi=par(72)*rlu(0)                                              
        ue(1)=sqrt(1.-ue(3)**2)*cos(phi)                                
        ue(2)=sqrt(1.-ue(3)**2)*sin(phi)                                
        do 230 j=1,3                                                    
        p(n+1,j)=pa*ue(j)                                               
  230   p(n+2,j)=-pa*ue(j)                                              
        p(n+1,4)=sqrt(pa**2+p(n+1,5)**2)                                
        p(n+2,4)=sqrt(pa**2+p(n+2,5)**2)                                
        mst1s=mst(1)                                                    
        mst(1)=n+1                                                      
        n=n+2                                                           
        call lurobo(0.,0.,pc(1)/pc(4),pc(2)/pc(4),pc(3)/pc(4))          
        mst(1)=mst1s                                                    
      else                                                              
        np=0                                                            
        do 240 i=icl,icu                                                
  240   if(k(i,1).lt.20000) np=np+1                                     
        ha=p(icl,4)*p(icu,4)-p(icl,1)*p(icu,1)-p(icl,2)*p(icu,2)-       
     &  p(icl,3)*p(icu,3)                                               
        if(np.ge.3.or.ha.le.1.25*p(icl,5)*p(icu,5)) goto 260            
        hd1=0.5*(p(n+1,5)**2-p(icl,5)**2)                               
        hd2=0.5*(p(n+2,5)**2-p(icu,5)**2)                               
        hr=sqrt(max(0.,((ha-hd1-hd2)**2-(p(n+1,5)*p(n+2,5))**2)/        
     &  (ha**2-(p(icl,5)*p(icu,5))**2)))-1.                             
        hc=p(icl,5)**2+2.*ha+p(icu,5)**2                                
        hk1=((p(icu,5)**2+ha)*hr+hd1-hd2)/hc                            
        hk2=((p(icl,5)**2+ha)*hr+hd2-hd1)/hc                            
        do 250 j=1,4                                                    
        p(n+1,j)=(1.+hk1)*p(icl,j)-hk2*p(icu,j)                         
  250   p(n+2,j)=(1.+hk2)*p(icu,j)-hk1*p(icl,j)                         
        n=n+2                                                           
      endif                                                             
      goto 290                                                          
                                                                        
  260 if(iabs(k(icl,2)).gt.510.and.iabs(k(icu,2)).gt.510) then          
        goto 310                                                        
      elseif(iabs(k(icl,2)).gt.500) then                                
        call luifld(mod(k(icl,2),500),0,mod(k(icu,2),500),              
     &  ifldmp,k(n+1,2))                                                
      else                                                              
        ifln=1+int((2.+par(2))*rlu(0))                                  
        call luifld(ifln,0,-ifln,ifldmp,k(n+1,2))                       
      endif                                                             
      p(n+1,5)=ulmass(1,k(n+1,2))                                       
                                                                        
      ir=0                                                              
      ha=0.                                                             
      do 270 i=1,n                                                      
      if(k(i,1).ge.20000.or.(i.ge.icl.and.i.le.icu).or.                 
     &(iabs(k(i,2)).lt.500.and.i.le.ns)) goto 270                       
      pcr=pc(4)*p(i,4)-pc(1)*p(i,1)-pc(2)*p(i,2)-pc(3)*p(i,3)           
      if(pcr.gt.ha) then                                                
        ir=i                                                            
        ha=pcr                                                          
      endif                                                             
  270 continue                                                          
                                                                        
      hb=pcm**2+ha                                                      
      hc=p(n+1,5)**2+ha                                                 
      hd=p(ir,5)**2+ha                                                  
      hk2=0.5*(hb*sqrt(((hb+hc)**2-4.*(hb+hd)*p(n+1,5)**2)/             
     &(ha**2-(pcm*p(ir,5))**2))-(hb+hc))/(hb+hd)                        
      hk1=(0.5*(p(n+1,5)**2-pcm**2)+hd*hk2)/hb                          
      do 280 j=1,4                                                      
      p(n+1,j)=(1.+hk1)*pc(j)-hk2*p(ir,j)                               
  280 p(ir,j)=(1.+hk2)*p(ir,j)-hk1*pc(j)                                
      n=n+1                                                             
                                                                        
  290 do 300 i=icl,icu                                                  
  300 if(k(i,1).le.20000.and.iabs(k(i,2)).ge.500) k(i,1)=k(i,1)+20000   
      if(n.lt.mst(30)-5-mst(31)) goto 150                               
                                                                        
  310 np=0                                                              
      kfn=0                                                             
      kfs=0                                                             
      do 320 j=1,5                                                      
  320 ps(j)=0.                                                          
      do 350 i=1,n                                                      
      if(k(i,1).ge.20000.or.iabs(k(i,2)).lt.500) goto 350               
      np=np+1                                                           
      if(iabs(k(i,2)).gt.500) then                                      
        kfn=kfn+1                                                       
        kfs=kfs+isign(1,k(i,2)*(510-iabs(k(i,2))))                      
        if(n.gt.i.and.k(i+1,1)/10000.eq.6) kfs=kfs+isign(1,             
     &  mod(k(i+1,2),10))                                               
        ps(5)=ps(5)+ulmass(0,k(i,2))                                    
      endif                                                             
      do 330 j=1,4                                                      
  330 ps(j)=ps(j)+p(i,j)                                                
      if(k(i,1).lt.10000) then                                          
        if(np.ne.1.and.(kfn.eq.1.or.kfn.ge.3.or.kfs.ne.0)) mst(26)=2    
        if(np.ne.1.and.ps(4)**2-ps(1)**2-ps(2)**2-ps(3)**2.lt.(par(22)+ 
     &  ps(5))**2) mst(26)=3                                            
        np=0                                                            
        kfn=0                                                           
        kfs=0                                                           
        do 340 j=1,5                                                    
  340   ps(j)=0.                                                        
      endif                                                             
  350 continue                                                          
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine lucons(ip)                                             
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      dimension ps1(4),ps2(4),nfl(3),ifet(3),iflf(3),te(3),td(3,3)      
                                                                        
      ntry=0                                                            
  100 ntry=ntry+1                                                       
      if(ntry.gt.200) then                                              
        mst(24)=mst(24)+1                                               
        mst(25)=4                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      do 110 j=1,3                                                      
      nfl(j)=0                                                          
      ifet(j)=0                                                         
  110 iflf(j)=0                                                         
      if(ntry.eq.1) then                                                
        do 120 j=1,4                                                    
  120   ps1(j)=0.                                                       
        in=ip-1                                                         
        njet=0                                                          
  130   in=in+1                                                         
        if(in.gt.min(n,mst(30)-mst(31))) then                           
          mst(24)=mst(24)+1                                             
          mst(25)=2                                                     
          if(mst(23).ge.1) return                                       
        endif                                                           
        if(k(in,1).ge.20000.or.iabs(k(in,2)).lt.500) goto 130           
        njet=njet+1                                                     
        do 140 j=1,4                                                    
  140   ps1(j)=ps1(j)+p(in,j)                                           
        if(k(in,1).ge.10000.or.(mst(6).le.4.and.n.gt.in.and.            
     &  k(in+1,1)/10000.eq.1)) goto 130                                 
        nsys=1+in-ip                                                    
        mst(1)=ip                                                       
        mst(2)=in                                                       
        call lurobo(0.,0.,-ps1(1)/ps1(4),-ps1(2)/ps1(4),-ps1(3)/ps1(4)) 
                                                                        
        if(n+nsys.ge.mst(30)-5-mst(31)) then                            
          mst(24)=mst(24)+1                                             
          mst(25)=1                                                     
          if(mst(23).ge.1) return                                       
        endif                                                           
        ecm=0.                                                          
        do 150 i=ip,in                                                  
        if(k(i,1).lt.20000.and.iabs(k(i,2)).ge.500) ecm=ecm+p(i,4)      
        do 150 j=1,5                                                    
  150   p(n+1+i-ip,j)=p(i,j)                                            
        n=n+nsys                                                        
        nsav=n                                                          
      else                                                              
        n=nsav                                                          
        do 160 i=ip,in                                                  
        if(k(i,1).ge.100000) k(i,1)=k(i,1)-120000                       
        do 160 j=1,5                                                    
  160   p(i,j)=p(nsav+i-in,j)                                           
      endif                                                             
                                                                        
      if(mst(6).ge.10.and.ntry.eq.1.and.njet.ge.3) then                 
        phi=ulangl(p(ip,1),p(ip,2))                                     
        call lurobo(0.,-phi,0.,0.,0.)                                   
        the=ulangl(p(ip,3),p(ip,1))                                     
        call lurobo(-the,0.,0.,0.,0.)                                   
        chi=ulangl(p(ip+1,1),p(ip+1,2))                                 
        call lurobo(0.,-chi,0.,0.,0.)                                   
        nbal=0                                                          
  170   nbal=nbal+1                                                     
        do 180 j1=1,3                                                   
        te(j1)=0.                                                       
        do 180 j2=1,3                                                   
  180   td(j1,j2)=0.                                                    
        do 200 i=ip,in                                                  
        if(k(i,1).ge.20000.or.iabs(k(i,2)).lt.500) goto 200             
        pa=sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2)                          
        ten=min(1.,pa/par(18))                                          
        if(iabs(k(i,2)).eq.500) ten=par(17)*ten                         
        do 190 j1=1,3                                                   
        te(j1)=te(j1)+ten*p(i,j1)/pa                                    
        td(j1,j1)=td(j1,j1)+ten*p(i,4)/pa                               
        do 190 j2=1,3                                                   
  190   td(j1,j2)=td(j1,j2)-ten*p(i,4)*p(i,j1)*p(i,j2)/pa**3            
  200   continue                                                        
        if(te(1)**2+te(2)**2+te(3)**2.lt.1e-3) goto 220                 
        if(nbal.ge.mst(13)) goto 100                                    
        do 210 jl=1,2                                                   
        do 210 j1=jl+1,3                                                
        te(j1)=te(j1)-(td(j1,jl)/td(jl,jl))*te(jl)                      
        do 210 j2=jl+1,3                                                
  210   td(j1,j2)=td(j1,j2)-(td(j1,jl)/td(jl,jl))*td(jl,j2)             
        te(3)=te(3)/td(3,3)                                             
        te(2)=(te(2)-td(2,3)*te(3))/td(2,2)                             
        te(1)=(te(1)-td(1,2)*te(2)-td(1,3)*te(3))/td(1,1)               
        ter=1.+sqrt(te(1)**2+te(2)**2+te(3)**2)                         
        call lurobo(0.,0.,-te(1)/ter,-te(2)/ter,-te(3)/ter)             
        goto 170                                                        
      endif                                                             
                                                                        
  220 mst(1)=0                                                          
      mst(2)=0                                                          
      kfsum=0                                                           
      do 230 i=ip,in                                                    
      kfa=iabs(k(i,2))                                                  
      if(k(i,1).ge.20000.or.kfa.lt.500) goto 230                        
      if(kfa.ge.501) kfsum=kfsum+isign(1,k(i,2)*(510-kfa))              
      ifl=mod(kfa,10)                                                   
      if(ifl.ne.0.and.ifl.le.3) nfl(ifl)=nfl(ifl)+isign(1,k(i,2))       
      ifl=mod(kfa,100)/10                                               
      if(ifl.ne.0.and.ifl.le.3) nfl(ifl)=nfl(ifl)+isign(1,k(i,2))       
      ifl=mod(iabs(k(i+1,2)),10)                                        
      if(n.gt.i.and.k(i+1,1)/10000.eq.6.and.ifl.ne.0) then              
        kfsum=kfsum+isign(1,ifl)                                        
        nfl(ifl)=nfl(ifl)+isign(1,k(i+1,2))                             
      endif                                                             
      call luonej(i)                                                    
      k(i,1)=k(i,1)+100000                                              
  230 continue                                                          
      if(kfsum.ne.0) then                                               
        mst(24)=mst(24)+1                                               
        mst(25)=2                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      if(mod(mst(6),5).ne.0.and.n-nsav.lt.2) goto 100                   
                                                                        
      if(mst(6).ge.10.and.ntry.eq.1.and.njet.ge.3) then                 
        do 240 j=1,4                                                    
        ps2(j)=0.                                                       
        do 240 i=ip,in                                                  
  240   if(k(i,1).ge.100000) ps2(j)=ps2(j)+p(i,j)                       
        mst(1)=ip                                                       
        mst(2)=in                                                       
        call lurobo(0.,0.,-ps2(1)/ps2(4),-ps2(2)/ps2(4),-ps2(3)/ps2(4)) 
        phir=ulangl(p(ip,1),p(ip,2))                                    
        call lurobo(0.,-phir,0.,0.,0.)                                  
        ther=ulangl(p(ip,3),p(ip,1))                                    
        call lurobo(-ther,0.,0.,0.,0.)                                  
        chir=ulangl(p(ip+1,1),p(ip+1,2))                                
        call lurobo(0.,-chir,0.,0.,0.)                                  
        call lurobo(0.,chi,0.,0.,0.)                                    
        call lurobo(the,phi,0.,0.,0.)                                   
        mst(1)=nsav+1                                                   
        mst(2)=0                                                        
        call lurobo(0.,0.,-ps2(1)/ps2(4),-ps2(2)/ps2(4),-ps2(3)/ps2(4)) 
        call lurobo(0.,-phir,0.,0.,0.)                                  
        call lurobo(-ther,0.,0.,0.,0.)                                  
        call lurobo(0.,-chir,0.,0.,0.)                                  
        call lurobo(0.,chi,0.,0.,0.)                                    
        call lurobo(the,phi,0.,0.,0.)                                   
        mst(1)=0                                                        
      endif                                                             
                                                                        
      if(mod(mst(6),10).ne.0) then                                      
        do 250 i=nsav+1,n                                               
        call luiflv(k(i,2),ifla,iflb,iflc,ksp)                          
        if(iabs(ifla).le.3) nfl(iabs(ifla))=nfl(iabs(ifla))-isign(1,    
     &  ifla)                                                           
        if(iabs(iflb).le.3) nfl(iabs(iflb))=nfl(iabs(iflb))-isign(1,    
     &  iflb)                                                           
  250   if(iflc.ne.0) nfl(iabs(iflc))=nfl(iabs(iflc))-isign(1,iflc)     
        nreq=(iabs(nfl(1))+iabs(nfl(2))+iabs(nfl(3))-iabs(nfl(1)+       
     &  nfl(2)+nfl(3)))/2+iabs(nfl(1)+nfl(2)+nfl(3))/3                  
        if(nreq.eq.0) goto 330                                          
                                                                        
        nrem=0                                                          
  260   irem=0                                                          
        p2min=ecm**2                                                    
        do 270 i=nsav+1,n                                               
        p2=p(i,1)**2+p(i,2)**2+p(i,3)**2                                
        if(k(i,1).lt.100000.and.p2.lt.p2min) irem=i                     
  270   if(k(i,1).lt.100000.and.p2.lt.p2min) p2min=p2                   
        if(irem.eq.0) goto 100                                          
        k(irem,1)=k(irem,1)+100000                                      
        call luiflv(k(irem,2),ifla,iflb,iflc,ksp)                       
        if(iabs(ifla).ge.4) k(irem,1)=k(irem,1)+100000                  
        if(iabs(ifla).ge.4) goto 260                                    
        nfl(iabs(ifla))=nfl(iabs(ifla))+isign(1,ifla)                   
        nfl(iabs(iflb))=nfl(iabs(iflb))+isign(1,iflb)                   
        if(iflc.ne.0) nfl(iabs(iflc))=nfl(iabs(iflc))+isign(1,iflc)     
        nrem=nrem+1                                                     
        nreq=(iabs(nfl(1))+iabs(nfl(2))+iabs(nfl(3))-iabs(nfl(1)+       
     &  nfl(2)+nfl(3)))/2+iabs(nfl(1)+nfl(2)+nfl(3))/3                  
        if(nreq.gt.nrem) goto 260                                       
        do 280 i=nsav+1,n                                               
  280   if(k(i,1).ge.200000) k(i,1)=k(i,1)-200000                       
                                                                        
  290   nfet=2                                                          
        if(nfl(1)+nfl(2)+nfl(3).ne.0) nfet=3                            
        if(nreq.lt.nrem) nfet=1                                         
        if(iabs(nfl(1))+iabs(nfl(2))+iabs(nfl(3)).eq.0) nfet=0          
        do 300 j=1,nfet                                                 
        ifet(j)=1+(iabs(nfl(1))+iabs(nfl(2))+iabs(nfl(3)))*rlu(0)       
        iflf(j)=isign(1,nfl(1))                                         
        if(ifet(j).gt.iabs(nfl(1))) iflf(j)=isign(2,nfl(2))             
  300   if(ifet(j).gt.iabs(nfl(1))+iabs(nfl(2))) iflf(j)=isign(3,nfl(3))
        if(nfet.eq.2.and.(ifet(1).eq.ifet(2).or.iflf(1)*iflf(2).gt.0))  
     &  goto 290                                                        
        if(nfet.eq.3.and.(ifet(1).eq.ifet(2).or.ifet(1).eq.ifet(3).or.  
     &  ifet(2).eq.ifet(3).or.iflf(1)*iflf(2).lt.0.or.iflf(1)*iflf(3).  
     &  lt.0.or.iflf(1)*(nfl(1)+nfl(2)+nfl(3)).lt.0)) goto 290          
        if(nfet.eq.0) iflf(1)=1+int((2.+par(2))*rlu(0))                 
        if(nfet.eq.0) iflf(2)=-iflf(1)                                  
        if(nfet.eq.1) iflf(2)=isign(1+int((2.+par(2))*rlu(0)),-iflf(1)) 
        if(nfet.le.2) iflf(3)=0                                         
        call luifld(iflf(1),iflf(3),iflf(2),ifldmp,kf)                  
        if(kf.eq.0) goto 290                                            
        do 310 j=1,max(2,nfet)                                          
  310   nfl(iabs(iflf(j)))=nfl(iabs(iflf(j)))-isign(1,iflf(j))          
        npos=min(1+int(rlu(0)*nrem),nrem)                               
        do 320 i=nsav+1,n                                               
        if(k(i,1).ge.100000) npos=npos-1                                
        if(k(i,1).lt.100000.or.npos.ne.0) goto 320                      
        k(i,1)=k(i,1)-100000                                            
        k(i,2)=kf                                                       
        p(i,5)=ulmass(1,k(i,2))                                         
        p(i,4)=sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2+p(i,5)**2)            
  320   continue                                                        
        nrem=nrem-1                                                     
        nreq=(iabs(nfl(1))+iabs(nfl(2))+iabs(nfl(3))-iabs(nfl(1)+       
     &  nfl(2)+nfl(3)))/2+iabs(nfl(1)+nfl(2)+nfl(3))/3                  
        if(nrem.gt.0) goto 290                                          
      endif                                                             
                                                                        
  330 if(mod(mst(6),5).ne.0.and.mod(mst(6),5).ne.4) then                
        do 340 j=1,3                                                    
        ps2(j)=0.                                                       
        do 340 i=nsav+1,n                                               
  340   ps2(j)=ps2(j)+p(i,j)                                            
        ps2(4)=ps2(1)**2+ps2(2)**2+ps2(3)**2                            
        pds=0.                                                          
        do 350 i=nsav+1,n                                               
        if(mod(mst(6),5).eq.1) pds=pds+p(i,4)                           
        if(mod(mst(6),5).eq.2) pds=pds+sqrt(p(i,5)**2+(ps2(1)*p(i,1)+   
     &  ps2(2)*p(i,2)+ps2(3)*p(i,3))**2/ps2(4))                         
  350   if(mod(mst(6),5).eq.3) pds=pds+1.                               
        do 370 i=nsav+1,n                                               
        if(mod(mst(6),5).eq.1) pdm=p(i,4)                               
        if(mod(mst(6),5).eq.2) pdm=sqrt(p(i,5)**2+(ps2(1)*p(i,1)+       
     &  ps2(2)*p(i,2)+ps2(3)*p(i,3))**2/ps2(4))                         
        if(mod(mst(6),5).eq.3) pdm=1.                                   
        do 360 j=1,3                                                    
  360   p(i,j)=p(i,j)-ps2(j)*pdm/pds                                    
  370   p(i,4)=sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2+p(i,5)**2)            
                                                                        
      elseif(mod(mst(6),5).eq.4) then                                   
        do 380 i=n+1,n+nsys                                             
        k(i,1)=0                                                        
        do 380 j=1,5                                                    
  380   p(i,j)=0.                                                       
        do 400 i=nsav+1,n                                               
        ir1=k(i,1)                                                      
        ir2=n+1+ir1-ip                                                  
        k(ir2,1)=k(ir2,1)+1                                             
        pls=(p(i,1)*p(ir1,1)+p(i,2)*p(ir1,2)+p(i,3)*p(ir1,3))/          
     &  (p(ir1,1)**2+p(ir1,2)**2+p(ir1,3)**2)                           
        do 390 j=1,3                                                    
  390   p(ir2,j)=p(ir2,j)+p(i,j)-pls*p(ir1,j)                           
        p(ir2,4)=p(ir2,4)+p(i,4)                                        
  400   p(ir2,5)=p(ir2,5)+pls                                           
        hss=0.                                                          
        do 410 i=n+1,n+nsys                                             
  410   if(k(i,1).ne.0) hss=hss+p(i,4)/(ecm*(0.8*p(i,5)+0.2))           
        do 430 i=nsav+1,n                                               
        ir1=k(i,1)                                                      
        ir2=n+1+ir1-ip                                                  
        pls=(p(i,1)*p(ir1,1)+p(i,2)*p(ir1,2)+p(i,3)*p(ir1,3))/          
     &  (p(ir1,1)**2+p(ir1,2)**2+p(ir1,3)**2)                           
        do 420 j=1,3                                                    
  420   p(i,j)=p(i,j)-p(ir2,j)/k(ir2,1)+(1./(p(ir2,5)*hss)-1.)*pls*     
     &  p(ir1,j)                                                        
  430   p(i,4)=sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2+p(i,5)**2)            
      endif                                                             
                                                                        
      if(mod(mst(6),5).ne.0) then                                       
        pms=0.                                                          
        pes=0.                                                          
        pqs=0.                                                          
        do 440 i=nsav+1,n                                               
        pms=pms+p(i,5)                                                  
        pes=pes+p(i,4)                                                  
  440   pqs=pqs+p(i,5)**2/p(i,4)                                        
        if(pms.ge.ecm) goto 100                                         
        neco=0                                                          
  450   neco=neco+1                                                     
        fac=(ecm-pqs)/(pes-pqs)                                         
        pes=0.                                                          
        pqs=0.                                                          
        do 470 i=nsav+1,n                                               
        do 460 j=1,3                                                    
  460   p(i,j)=fac*p(i,j)                                               
        p(i,4)=sqrt(p(i,1)**2+p(i,2)**2+p(i,3)**2+p(i,5)**2)            
        pes=pes+p(i,4)                                                  
  470   pqs=pqs+p(i,5)**2/p(i,4)                                        
        if(neco.lt.10.and.abs(ecm-pes).gt.2e-6*ecm) goto 450            
      endif                                                             
                                                                        
      do 480 i=ip,in                                                    
  480 if(k(i,1).ge.100000) k(i,1)=k(i,1)-100000                         
      mst(1)=ip                                                         
      mst(2)=in                                                         
      call lurobo(0.,0.,ps1(1)/ps1(4),ps1(2)/ps1(4),ps1(3)/ps1(4))      
      mst(1)=nsav+1                                                     
      mst(2)=0                                                          
      call lurobo(0.,0.,ps1(1)/ps1(4),ps1(2)/ps1(4),ps1(3)/ps1(4))      
      mst(1)=0                                                          
      do 490 i=nsav+1,n                                                 
      k(i-nsys,1)=k(i,1)                                                
      k(i-nsys,2)=k(i,2)                                                
      do 490 j=1,5                                                      
  490 p(i-nsys,j)=p(i,j)                                                
      n=n-nsys                                                          
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luonej(ip)                                             
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      dimension iflf(3),iflo(2),pxo(2),pyo(2),zo(2),wo(2)               
                                                                        
      mst(1)=n+1                                                        
      iflf(1)=mod(k(ip,2),500)                                          
      iflf(2)=0                                                         
      iflf(3)=0                                                         
      iflo(2)=0                                                         
      wf=p(ip,4)+sqrt(p(ip,1)**2+p(ip,2)**2+p(ip,3)**2)                 
      zj=0.                                                             
      zi=0.                                                             
                                                                        
  100 if(iflf(1).eq.0.and.mst(5).le.2) then                             
        ns=2                                                            
        i=n+1                                                           
        k(i,1)=ip                                                       
        call luifld(int(1.+(2.+par(2))*rlu(0)),0,0,iflo(1),kdump)       
        call luifld(-int(1.+(2.+par(2))*rlu(0)),0,0,iflo(2),kdump)      
        if(iabs(iflo(2)).lt.100) iflo(2)=-mod(iflo(1),100)              
        if(iabs(iflo(2)).gt.100) iflo(1)=-mod(iflo(2),100)              
        call luifld(iflo(1),0,0,ifl1,k(i,2))                            
        iflo(1)=-ifl1                                                   
        p(i,5)=ulmass(1,k(i,2))                                         
        call luptdi(iflo(1),pxo(1),pyo(1))                              
        call luptdi(iflo(2),pxo(2),pyo(2))                              
        pr=p(i,5)**2+(pxo(1)+pxo(2))**2+(pyo(1)+pyo(2))**2              
        prdiv=rlu(0)*pr                                                 
        do 110 jt=1,2                                                   
        call luzdis(iflo(jt),0,0.6*((2-jt)*pr+(2*jt-3)*prdiv),zo(jt))   
  110   wo(jt)=0.5*(1.-zo(jt))*wf                                       
                                                                        
        p(i,1)=-(pxo(1)+pxo(2))                                         
        p(i,2)=-(pyo(1)+pyo(2))                                         
        p(i,3)=0.25*(zo(1)+zo(2))*wf-pr/((zo(1)+zo(2))*wf)              
        p(i,4)=0.25*(zo(1)+zo(2))*wf+pr/((zo(1)+zo(2))*wf)              
        if(mst(5).ge.2.and.mst(6).ge.0.and.p(i,3).lt.0.) i=i-1          
        n=i                                                             
                                                                        
      elseif(iflf(1).eq.0.and.(mst(5).eq.3.or.mst(5).eq.4)) then        
        ns=1                                                            
        if(mst(5).eq.4) mst(32)=1                                       
        iflo(1)=int(1.+(2.+par(2))*rlu(0))*(-1)**int(rlu(0)+0.5)        
        call luptdi(93,pxo(1),pyo(1))                                   
        wo(1)=wf                                                        
                                                                        
      elseif(iflf(1).eq.0.and.mst(5).ge.5) then                         
        ns=2                                                            
        if(mst(5).eq.6) mst(32)=1                                       
        iflo(1)=int(1.+(2.+par(2))*rlu(0))*(-1)**int(rlu(0)+0.5)        
        iflo(2)=-iflo(1)                                                
        call luptdi(93,pxo(1),pyo(1))                                   
        pxo(2)=-pxo(1)                                                  
        pyo(2)=-pyo(1)                                                  
        wo(1)=wf*rlu(0)**(1./3.)                                        
        wo(2)=wf-wo(1)                                                  
                                                                        
      else                                                              
        ns=1                                                            
        iflo(1)=iflf(1)                                                 
        call luptdi(93,pxo(1),pyo(1))                                   
        wo(1)=wf                                                        
                                                                        
        if(mod(mst(10),2).eq.1.and.iabs(iflf(1)).gt.10) then            
          ifla=iflf(1)/10                                               
          iflb=iflf(1)-10*ifla                                          
          iflf(2)=ifla+int(rlu(0)+0.5)*(iflb-ifla)                      
          if(n.gt.ip.and.k(ip+1,1)/10000.eq.6.and.iabs(k(ip+1,2)).ge.   
     &    610) iflf(2)=mod(k(ip+1,2)/10,10)                             
          iflo(1)=ifla+iflb-iflf(2)                                     
          call luzdis(0,1,0.,zj)                                        
          if(n.gt.ip.and.k(ip+1,1)/10000.eq.6) p(ip+1,1)=zj             
        endif                                                           
                                                                        
        if(n.gt.ip.and.k(ip+1,1)/10000.eq.6) iflf(3)=mod(k(ip+1,2),10)  
        if(iflf(3).ne.0) then                                           
          call luzdis(0,2+(90+iabs(iflf(1)))/100,0.,zi)                 
          if(iabs(iflf(1)).gt.10.and.mod(mst(10),2).eq.1) zi=zi*zj      
          p(ip+1,3)=zi                                                  
        endif                                                           
      endif                                                             
                                                                        
      do 140 jt=1,ns                                                    
  120 i=n                                                               
      lrk=0                                                             
      ifl1=iflo(jt)                                                     
      iflj=iflf(2)                                                      
      ifli=iflf(3)                                                      
      if(iflj.eq.0) then                                                
        px1=pxo(jt)                                                     
        py1=pyo(jt)                                                     
        pxj=0.                                                          
        pyj=0.                                                          
      else                                                              
        call luptdi(94,pxr,pyr)                                         
        px1=0.5*pxo(jt)+pxr                                             
        py1=0.5*pyo(jt)+pyr                                             
        pxj=0.5*pxo(jt)-pxr                                             
        pyj=0.5*pyo(jt)-pyr                                             
        if(n.gt.ip.and.k(ip+1,1)/10000.eq.6) p(ip+1,2)=0.               
      endif                                                             
      if(iflf(3).ne.0) p(ip+1,4)=0.                                     
      w=wo(jt)                                                          
                                                                        
  130 i=i+1                                                             
      if(i.ge.mst(30)-5-mst(31)) then                                   
        mst(24)=mst(24)+1                                               
        mst(25)=1                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      lrk=lrk+1                                                         
      k(i,1)=ip                                                         
      call luptdi(ifl1,px2,py2)                                         
      mqj=0                                                             
      mqi=0                                                             
                                                                        
      if(iflj.ne.0.or.ifli.ne.0) then                                   
        prji=par(37)**2+(px1+px2)**2+(py1+py2)**2                       
        call luzdis(ifl1,iflj+ifli,prji,z)                              
        if(iflj.ne.0.and.(1.-z)*w.le.zj*wf) mqj=1                       
        if(mqj.eq.1.and.iabs(ifl1).gt.10) goto 120                      
        if(mqj.eq.1.and.lrk.eq.1) ifl1=iflf(1)                          
        if(ifli.ne.0.and.(1.-z)*w.le.zi*wf) mqi=1                       
        if(mqi.eq.1.and.iabs(ifl1).gt.100) goto 120                     
      endif                                                             
      call luifld(ifl1,mqj*iflj,mqi*ifli,ifl2,k(i,2))                   
      if(k(i,2).eq.0) goto 120                                          
      p(i,5)=ulmass(1,k(i,2))                                           
      pr=p(i,5)**2+(px1+px2+mqj*pxj)**2+(py1+py2+mqj*pyj)**2            
                                                                        
      if(iflj.eq.0.and.ifli.eq.0) then                                  
        call luzdis(ifl1,0,pr,z)                                        
      elseif(mst(4).eq.1.or.mst(4).eq.3) then                           
        gamji=(1.+par(35))/par(36)                                      
        zbc=(pr-prji-z*gamji+prji/z)/(2.*gamji)                         
        z=sqrt(zbc**2+pr/gamji)-zbc                                     
      endif                                                             
      p(i,1)=px1+px2+mqj*pxj                                            
      p(i,2)=py1+py2+mqj*pyj                                            
      p(i,3)=0.5*(z*w-pr/(z*w))                                         
      p(i,4)=0.5*(z*w+pr/(z*w))                                         
      if(mod(mst(6),10).gt.0.and.lrk.eq.1.and.max(mod(iabs(iflf(1)),    
     &10),iabs(iflf(1))/10).ge.4.and.p(i,3).le.0.001) then              
        if(w.ge.p(i,5)+0.5*par(22)) goto 120                            
        p(i,3)=0.0001                                                   
        p(i,4)=sqrt(pr)                                                 
        z=p(i,4)/w                                                      
      endif                                                             
      if(mst(5).ge.2.and.mst(6).ge.0.and.p(i,3).lt.0.) i=i-1            
      if(i.eq.n+lrk.and.mqj*n.gt.ip.and.k(ip+1,1)/10000.eq.6)           
     &p(ip+1,2)=i                                                       
      if(i.eq.n+lrk.and.mqi.eq.1) p(ip+1,4)=i                           
                                                                        
      ifl1=-ifl2                                                        
      if(mqi.eq.1) call luifld((-1)**int(rlu(0)+0.5),0,0,ifl1,kdump)    
      if(mqj.eq.1) iflj=0                                               
      if(mqi.eq.1) ifli=0                                               
      px1=-px2                                                          
      py1=-py2                                                          
      w=(1.-z)*w                                                        
      if(mst(10).le.1.and.w.gt.par(21)) goto 130                        
      if(mst(10).ge.2.and.(iflj.ne.0.or.ifli.ne.0)) goto 130            
  140 n=i                                                               
                                                                        
      if(mod(mst(6),5).eq.4.and.mst(1).eq.n+1) wf=wf+0.1*par(22)        
      if(mod(mst(6),5).eq.4.and.mst(1).eq.n+1) goto 100                 
      the=ulangl(p(ip,3),sqrt(p(ip,1)**2+p(ip,2)**2))                   
      phi=ulangl(p(ip,1),p(ip,2))                                       
      call lurobo(the,phi,0.,0.,0.)                                     
      mst(1)=0                                                          
      mst(32)=0                                                         
      k(ip,1)=k(ip,1)+20000                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
                                                                        
      function ulmass(mmass,kf)                                         
      common /ludat1/ mst(40),par(80)                                   
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
                                                                        
      kfa=iabs(kf)                                                      
      ulmass=0.                                                         
      kty=0                                                             
                                                                        
      if(mmass.le.1.and.kfa.le.100) then                                
        ulmass=pmas(kfa)                                                
        kty=ktyp(kfa)/10                                                
                                                                        
      elseif(mmass.le.1.and.kfa.lt.500) then                            
        call luiflv(kfa,ifla,iflb,iflc,ksp)                             
        if(ksp.eq.2.and.ifla.eq.iflb) then                              
          ifld=ifla                                                     
          ifla=iflc                                                     
          iflc=ifld                                                     
        endif                                                           
        pma=pmas(100+ifla)                                              
        pmb=pmas(100+iabs(iflb))                                        
        pmc=pmas(100+iflc)                                              
        if(ksp.le.1) ulmass=pmas(113)+pma+pmb+pmas(115)*pmas(101)**2*   
     &  cfr(25+ksp)/(pma*pmb)                                           
        if(ksp.ge.2) ulmass=pmas(114)+pma+pmb+pmc+pmas(116)*            
     &  pmas(101)**2*(cfr(21+3*ksp)/(pma*pmb)+cfr(22+3*ksp)/(pma*pmc)+  
     &  cfr(23+3*ksp)/(pmb*pmc))                                        
        kty=ktyp(100+ifla)/10                                           
                                                                        
      else                                                              
        kfa=mod(kfa,100)                                                
        if(kfa.ge.1.and.kfa.le.10) then                                 
          ulmass=pmas(100+kfa)                                          
          if(mmass.eq.3) ulmass=ulmass-pmas(111)                        
          kty=ktyp(100+kfa)/10                                          
        elseif(kfa.gt.10) then                                          
          ksp=0                                                         
          if(kfa/10.ge.mod(kfa,10)) ksp=1                               
          pma=pmas(100+kfa/10)                                          
          pmb=pmas(100+mod(kfa,10))                                     
          ulmass=pma+pmb                                                
          if(mmass.eq.3) ulmass=ulmass-pmas(112)+pmas(116)*pmas(101)**2*
     &    cfr(25+ksp)/(pma*pmb)                                         
          kty=ktyp(100+max(kfa/10,mod(kfa,10)))/10                      
        endif                                                           
      endif                                                             
                                                                        
      if(mst(8).eq.1.and.mmass.ge.1.and.kty.ge.1) ulmass=ulmass+0.5*    
     &pwid(2*kty-1)*tan((2.*rlu(0)-1.)*atan(2.*pwid(2*kty)/             
     &pwid(2*kty-1)))                                                   
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luptdi(ifl,px,py)                                      
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      ifla=iabs(ifl)                                                    
      pt=par(12)*sqrt(-alog(max(1e-10,rlu(0))))                         
      if(mst(32).eq.1) pt=par(13)*pt                                    
      if(ifla.ge.4.and.ifla.lt.10.and.abs(par(16)-1.).gt.0.1) pt=       
     &sqrt(par(16))*pt                                                  
      if(ifla.eq.93.and.mst(11).eq.1) pt=par(14)*pt                     
      if(ifla.eq.93.and.mst(11).ne.1) pt=0.                             
      if(ifla.eq.94) pt=par(15)*pt/par(12)                              
      phi=par(72)*rlu(0)                                                
      px=pt*cos(phi)                                                    
      py=pt*sin(phi)                                                    
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luzdis(ifl1,ifl3,pr,z)                                 
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      ifla=max(mod(iabs(ifl1),100)/10,mod(iabs(ifl1),10))               
      if(ifla.ne.0.and.(mst(4).eq.1.or.(mst(4).eq.3.and.ifla.le.3)))    
     &then                                                              
        fa=par(31)                                                      
        fb=par(32)*pr                                                   
        if(ifl3.ne.0) fa=par(35)                                        
        if(ifl3.ne.0) fb=par(36)*pr                                     
        if(mst(32).eq.1) fa=par(33)                                     
        if(mst(32).eq.1) fb=par(34)*pr                                  
        if(fa.le.0.01) zmax=min(1.,fb)                                  
        if(fa.gt.0.01.and.abs(fa-1.)/fb.le.0.01) zmax=fb/(1.+fb)+       
     &  (1.-fa)*fb**2/(1.+fb)**3                                        
        if(fa.gt.0.01.and.abs(fa-1.)/fb.gt.0.01) zmax=0.5*(1.+fb-       
     &  sqrt((1.-fb)**2+4.*fa*fb))/(1.-fa)                              
        if(zmax.lt.0.1) zdiv=2.75*zmax                                  
        if(zmax.gt.0.85) zdiv=zmax-0.6/fb**2+(fa/fb)*alog((0.01+fa)/fb) 
  100   z=rlu(0)                                                        
        idiv=1                                                          
        fpre=1.                                                         
        if(zmax.lt.0.1) then                                            
          if(1..lt.rlu(0)*(1.-alog(zdiv))) idiv=2                       
          if(idiv.eq.1) z=zdiv*z                                        
          if(idiv.eq.2) z=zdiv**z                                       
          if(idiv.eq.2) fpre=zdiv/z                                     
        elseif(zmax.gt.0.85) then                                       
          if(1..lt.rlu(0)*(fb*(1.-zdiv)+1.)) idiv=2                     
          if(idiv.eq.1) z=zdiv+alog(z)/fb                               
          if(idiv.eq.1) fpre=exp(fb*(z-zdiv))                           
          if(idiv.eq.2) z=zdiv+z*(1.-zdiv)                              
        endif                                                           
        if(z.le.fb/(50.+fb).or.z.ge.1.) goto 100                        
        fval=(zmax/z)*exp(fb*(1./zmax-1./z))                            
        if(fa.gt.0.01) fval=((1.-z)/(1.-zmax))**fa*fval                 
        if(fval.lt.rlu(0)*fpre) goto 100                                
                                                                        
      elseif(ifl1.ne.0) then                                            
        fc=par(40+ifla)                                                 
        if(mst(32).eq.1) fc=par(49)                                     
        if(ifl3.ne.0) fc=par(50)                                        
  110   z=rlu(0)                                                        
        if(fc.ge.0..and.fc.le.1.) then                                  
          if(fc.gt.rlu(0)) z=1.-z**(1./3.)                              
        elseif(fc.gt.-1.) then                                          
          if(-4.*fc*z*(1.-z)**2.lt.rlu(0)*((1.-z)**2-fc*z)**2) goto 110 
        else                                                            
          if(fc.gt.0.) z=1.-z**(1./fc)                                  
          if(fc.lt.0.) z=z**(-1./fc)                                    
        endif                                                           
                                                                        
      else                                                              
  120   z=rlu(0)**(1./(1.+max(par(49+2*ifl3),par(50+2*ifl3))))          
        if((1.-z)**min(par(49+2*ifl3),par(50+2*ifl3)).lt.rlu(0))        
     &  goto 120                                                        
        if(par(50+2*ifl3).gt.par(49+2*ifl3)) z=1.-z                     
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luiflv(kf,ifla,iflb,iflc,ksp)                          
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
                                                                        
      kfa=iabs(kf)                                                      
      kfs=isign(1,kf)                                                   
      ifla=0                                                            
      iflb=0                                                            
      iflc=0                                                            
                                                                        
      ksp=-1                                                            
      if((kfa.ge.17.and.kfa.le.26).or.kfa.eq.37.or.kfa.eq.38.or.        
     &(kfa.ge.83.and.kfa.le.86).or.kfa.ge.101) ksp=0                    
      if((kfa.ge.27.and.kfa.le.36).or.(kfa.ge.87.and.kfa.le.90).or.     
     &kfa.ge.123) ksp=1                                                 
      if((kfa.ge.41.and.kfa.le.56).or.kfa.ge.145) ksp=2                 
      if((kfa.ge.57.and.kfa.le.60).or.kfa.ge.241) ksp=3                 
      if((kfa.ge.61.and.kfa.le.80).or.kfa.ge.293) ksp=4                 
      if(kfa.ge.393) ksp=-1                                             
                                                                        
      if((kfa.ge.23.and.kfa.le.26).or.(kfa.ge.33.and.kfa.le.36).or.     
     &(kfa.ge.83.and.kfa.le.90)) then                                   
        if(kfa.le.40) ifla=kfa-22-10*ksp                                
        if(kfa.ge.80) ifla=kfa-78-4*ksp                                 
        iflb=-ifla                                                      
      elseif(kfa.eq.37.or.kfa.eq.38) then                               
        ifla=isign(3,(-1)**int(rlu(0)+0.5))                             
        iflb=isign(2,-ifla)                                             
      elseif(ksp.eq.0.or.ksp.eq.1) then                                 
  100   ifla=ifla+1                                                     
        if(ifla.lt.8.and.kfr(8*ksp+ifla+1).lt.kfa) goto 100             
        iflb=-(kfa-kfr(8*ksp+ifla))                                     
        if(ifla.le.3) iflb=-iflb                                        
        if(ifla.le.3) ifla=-ifla                                        
                                                                        
      elseif(ksp.ge.2.and.ksp.le.4) then                                
  110   ifla=ifla+1                                                     
        if(ifla.lt.8.and.kfr(16*ksp+ifla-15).lt.kfa) goto 110           
  120   iflb=iflb+1                                                     
        if(iflb.lt.8.and.kfr(16*ksp+iflb-7).lt.kfa-kfr(16*ksp+ifla-16)) 
     &  goto 120                                                        
        iflc=kfa-kfr(16*ksp+ifla-16)-kfr(16*ksp+iflb-8)                 
      endif                                                             
                                                                        
      ifla=kfs*ifla                                                     
      iflb=kfs*iflb                                                     
      iflc=kfs*iflc                                                     
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      function luchge(kf)                                               
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
                                                                        
      kfa=iabs(kf)                                                      
      luchge=0                                                          
                                                                        
      if(kfa.le.100) then                                               
        kty=mod(ktyp(kfa),10)                                           
        if(kty.ge.1) luchge=3*kty-6                                     
                                                                        
      elseif(kfa.lt.500) then                                           
        call luiflv(kfa,ifla,iflb,iflc,ksp)                             
        luchge=3*mod(ktyp(100+ifla),10)-16+                             
     &  (3*mod(ktyp(100+iabs(iflb)),10)-16)*isign(1,iflb)               
        if(iflc.ne.0) luchge=luchge+3*mod(ktyp(100+iflc),10)-16         
                                                                        
      elseif(kfa.le.600) then                                           
        if(mod(kfa,10).ne.0) luchge=3*mod(ktyp(100+mod(kfa,10)),10)-16  
        if(kfa.gt.510) luchge=luchge+3*mod(ktyp(50+kfa/10),10)-16       
                                                                        
      elseif(kfa.le.700) then                                           
        if(mod(kfa,10).ne.0) luchge=3*mod(ktyp(100+mod(kfa,10)),10)-16  
      endif                                                             
                                                                        
      luchge=luchge*isign(1,kf)                                         
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      subroutine luname(kf,chau)                                        
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
      common /ludat4/ chag(50),chaf(100)                                
      character chau*8,chag*4,chaf*4                                    
                                                                        
      chau=chag(1)//chag(1)                                             
      kfa=iabs(kf)                                                      
      kfs=isign(1,kf)                                                   
                                                                        
      if(kfa.eq.0) then                                                 
      elseif(kfa.le.100) then                                           
        chau=chaf(kfa)//chag(27+kfs*mod(ktyp(kfa),10))                  
      elseif(kfa.lt.500) then                                           
        call luiflv(kfa,ifla,iflb,iflc,ksp)                             
        if(iflc.eq.0) chau=chag(10+ifla)(1:1)//chag(10-iflb)(1:2)//     
     &  chag(35-ksp)(1:1)//chag(27+kfs*(luchge(kfa)/3+2))               
        if(iflc.ne.0) chau=chag(10+ifla)(1:1)//chag(10+iflb)(1:1)//     
     &  chag(10+iflc)(1:1)//chag(30+ksp)(1:1)//chag(27+kfs*             
     &  (luchge(kfa)/3+2))                                              
                                                                        
      elseif(kfa.lt.590) then                                           
        ifla=max(kfa/10-50,kfa-10*(kfa/10))                             
        iflb=min(kfa/10-50,kfa-10*(kfa/10))                             
        if(iflb.eq.0) chau=chag(10+kfs*ifla)//chag(22)                  
        ksp=32                                                          
        if(kfa/10-50.lt.ifla) ksp=33                                    
        if(iflb.ne.0) chau=chag(10+ifla)(1:1)//chag(10+kfs*iflb)(1:2)// 
     &  chag(ksp)(1:1)//chag(22)                                        
      elseif(kfa.le.600) then                                           
        chau=chag(kfa-571)//chag(22)                                    
                                                                        
      elseif(kfa.lt.700) then                                           
        ifla=isign(kfa/10-60,kf)                                        
        iflb=isign(mod(kfa,10),kf)                                      
        if(ifla.ne.0) chau(1:4)=chag(10+ifla)(1:2)//chag(37)(1:2)       
        if(iflb.ne.0) chau(5:8)=chag(10+iflb)(1:2)//chag(38)(1:2)       
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      function ulangl(x,y)                                              
      common /ludat1/ mst(40),par(80)                                   
                                                                        
      ulangl=0.                                                         
      r=sqrt(x**2+y**2)                                                 
      if(r.lt.1e-20) return                                             
      if(abs(x)/r.lt.0.8) then                                          
        ulangl=sign(acos(x/r),y)                                        
      else                                                              
        ulangl=asin(y/r)                                                
        if(x.lt.0..and.ulangl.ge.0.) then                               
          ulangl=par(71)-ulangl                                         
        elseif(x.lt.0.) then                                            
          ulangl=-par(71)-ulangl                                        
        endif                                                           
      endif                                                             
                                                                        
      return                                                            
      end                                                               
                                                                        
                                                                        
      function rlu(idum)                                                
      rlu=ranf(0)                                                       
      return                                                            
      end                                                               
