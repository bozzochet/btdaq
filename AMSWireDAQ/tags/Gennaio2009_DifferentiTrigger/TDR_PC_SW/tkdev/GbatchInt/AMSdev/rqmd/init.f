CPAT,INIT     .                                                         
      subroutine initev                                                 
        include 'scatter7'                                              
        include 'partblk7'                                              
      dimension qlk(0:3),plk(0:3),ptot(0:3)                             
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version  rqmd 2.2 --- 21-mar-95'/                    
      data pronam /'i n i t  initializes the particles coordinates'/    
      data ttvers/.true./                                               
      data rhc/0.6/                                                     
      save z01,z02,tfix,x0p,x0t                                         
      real*4  radius(1)                                                 
      equivalence (radius(1),datsto(1))                                 
      real*4 force,dcoef                                                
      common/blocal/ ngbor(ntotma),force(0:3,ntotma),dcoef(ntotma)      
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
       sphepr=4.*(4.*pi/3.)**2                                          
       pfm=hc/(da(3)*sphepr**.333333333)                                
       gampr=1./sqrt(1.-betapr*betapr)                                  
       gamta=1./sqrt(1.-betata*betata)                                  
        n1=n1old                                                        
        iz1=iz1old                                                      
        n2=n2old                                                        
        iz2=iz2old                                                      
        nt=n1+n2                                                        
      if(ida(17).eq.0) then                                             
         rap=da(3)*n1**0.333333333                                      
         rat=da(3)*n2**0.333333333                                      
      else                                                              
         rap=1.19* n1**0.3333-1.61* n1**(-0.3333)                       
         rat=1.19* n2**0.3333-1.61* n2**(-0.3333)                       
      end if                                                            
      if(n1.eq.1) rap=0.                                                
      if(n2.eq.1) rat=0.                                                
       z01=1.+rap                                                       
       z02=-1.-rat                                                      
        delt1=2.0                                                       
        delt2=2.0                                                       
        tfix=1./betann                                                  
     a          * min(z01-gamnn*rap-delt1,-z02-gamnn*rat-delt2)         
        if(tfix.gt.0.) tfix=0.                                          
        x0p=tfix/gamnn +betann*z01                                      
        x0t=tfix/gamnn -betann*z02                                      
      if(da(11).lt.0.) then                                             
          if(da(11).le.-99.0)then                                       
            bmax=rap+rat+3.                                             
          else                                                          
            bmax=-da(11)                                                
          end if                                                        
          x0=bmax*sqrt(ranf(0))/2.                                      
          b=2.*x0                                                       
      else                                                              
          x0=0.5*b                                                      
      end if                                                            
      bplus=x0                                                          
      rhomx1=1./(1.+exp(-rap/da(2)))                                    
      rhomx2=1./(1.+exp(-rat/da(2)))                                    
      if(ida(34).gt.0) then                                             
       if(ref.eq.'lab') then                                            
         pnorm(0)=1.                                                    
         pnorm(1)=0.                                                    
         pnorm(2)=0.                                                    
         pnorm(3)=0.                                                    
       else                                                             
         pnorm(0)=gamnn                                                 
         pnorm(1)=0.                                                    
         pnorm(2)=0.                                                    
         pnorm(3)=gamnn*betann                                          
       end if                                                           
      else                                                              
       if(ref.eq.'lab') then                                            
         pnorm(0)=gamnn                                                 
         pnorm(1)=0.                                                    
         pnorm(2)=0.                                                    
         pnorm(3)=-gamnn*betann                                         
       else                                                             
         pnorm(0)=1.                                                    
         pnorm(1)=0.                                                    
         pnorm(2)=0.                                                    
         pnorm(3)=0.                                                    
       end if                                                           
      end if                                                            
       nprtcl=0                                                         
        pfmold=pfm                                                      
        if(n1.eq.1) pfm=0.                                              
        if(iz1.ge.0) then                                               
            ibas1=0                                                     
            ibas2=0                                                     
        else                                                            
            ibas1=n2                                                    
            ibas2=-n1                                                   
        end if                                                          
        do 106 i =ibas1+1,ibas1+n1                                      
          nprtcl=nprtcl+1                                               
          itry=0                                                        
   31     continue                                                      
          ktry=0                                                        
          itry=itry+1                                                   
          if(rap.eq.0.) then                                            
              r=0.                                                      
          else if(ida(17).eq.0) then                                    
              r=rap*ranf(0)**0.3333                                     
          else                                                          
              r=(rap+2.5)*ranf(0)**0.3333                               
              if(ranf(0).gt.(1./(1.+exp((r-rap)/da(2))))/rhomx1) then   
                 if(itry.gt.200)then                                    
                    write(8,*) 'infinite loop in init,31?'              
                 else                                                   
                    go to 31                                            
                 end if                                                 
              end if                                                    
          end if                                                        
   29     ktry=ktry+1                                                   
          if(ktry.gt.60) then                                           
             go to 31                                                   
          end if                                                        
          cth=1.-2.*ranf(0)                                             
          sth=sqrt(1.-cth**2)                                           
          phi=pi2*ranf(0)                                               
          x(0,i)=x0p                                                    
          x(1,i)=r*sth*cos(phi)+x0                                      
          x(2,i)=r*sth*sin(phi)                                         
          x(3,i)=r*cth+z01                                              
          if(ida(6).eq.1.or.ida(31).eq.1)then                           
           denspp(nprtcl)=0.                                            
           do 506 k=ibas1+1,i-1                                         
            drsq=(x(1,i)-x(1,k))**2+(x(2,i)-x(2,k))**2+                 
     a                                           (x(3,i)-x(3,k))**2     
            if(ida(31).eq.1.and.drsq.lt.rhc*rhc)then                    
               go to 29                                                 
            end if                                                      
            if(ida(6).eq.1)then                                         
              arg=exp(-drsq/alpot)                                      
              denspp(nprtcl)=denspp(nprtcl)+arg                         
              denspp(issn(k))=denspp(issn(k))+arg                       
            end if                                                      
  506      continue                                                     
          end if                                                        
          if(switch(21)) then                                           
            dsq=(x(1,i)+bplus)**2+x(2,i)**2 - rtasq                     
            if(dsq.gt.0.) then                                          
               ityp(i)=-9999                                            
               go to 106                                                
            end if                                                      
          end if                                                        
          lastcl(i)=0                                                   
          ichbig(i)=0                                                   
          issn(i)=nprtcl                                                
          decay(i)=1.e35                                                
          pr=pfm*ranf(0)**(1./3.)                                       
          cth=1.-2.*ranf(0)                                             
          sth=sqrt(1.-cth**2)                                           
          phi=pi2*ranf(0)                                               
          p(0,i)=sqrt(emproj**2+pr**2)                                  
          p(3,i)=pr*cth                                                 
          p(1,i)=pr*sth*cos(phi)                                        
          p(2,i)=pr*sth*sin(phi)                                        
          npidec(i)=0                                                   
          if (iz1.lt.0) then                                            
            itypld=ityppr                                               
            call transl(1,itp,npid,itypld)                              
            ityp(i)=itp                                                 
            npidec(i)=npid                                              
            em(i)=emproj                                                
          else if (i .le. iz1) then                                     
            ityp(i)=2                                                   
            em(i)=emnuc                                                 
          else                                                          
            ityp(i)=1                                                   
            em(i)=emnuc                                                 
          end if                                                        
 106    continue                                                        
        pfm=pfmold                                                      
        if(n2.eq.1) pfm=0.                                              
        do 107 i =n1+1+ibas2,nt+ibas2                                   
          nprtcl=nprtcl+1                                               
          itry=0                                                        
   32     continue                                                      
          ktry=0                                                        
          itry=itry+1                                                   
          if(rat.eq.0.) then                                            
              r=0.                                                      
          else if(ida(17).eq.0) then                                    
              r=rat*ranf(0)**0.3333                                     
          else                                                          
              r=(rat+2.5)*ranf(0)**0.3333                               
              if(ranf(0).gt.(1./(1.+exp((r-rat)/da(2))))/rhomx2) then   
                 if(itry.gt.200)then                                    
                   write(8,*) 'infinite loop in init,32?'               
                 else                                                   
                   go to 32                                             
                 end if                                                 
              end if                                                    
          end if                                                        
   30     ktry=ktry+1                                                   
          if(ktry.gt.60) then                                           
             go to 32                                                   
          end if                                                        
          cth=1.-2.*ranf(0)                                             
          sth=sqrt(1.-cth**2)                                           
          phi=pi2*ranf(0)                                               
          x(0,i)=x0t                                                    
          x(1,i)=r*sth*cos(phi)-x0                                      
          x(2,i)=r*sth*sin(phi)                                         
          x(3,i)=r*cth+z02                                              
          if(ida(34).eq.2) then                                         
             radius(nprtcl)=r                                           
          end if                                                        
          if(ida(6).eq.1.or.ida(31).eq.1)then                           
           denspp(nprtcl)=0.                                            
           do 507 k=n1+1+ibas2,i-1                                      
            drsq=(x(1,i)-x(1,k))**2+(x(2,i)-x(2,k))**2+                 
     a                                           (x(3,i)-x(3,k))**2     
            if(ida(31).eq.1.and.drsq.lt.rhc*rhc)then                    
               go to 30                                                 
            end if                                                      
            if(ida(6).eq.1)then                                         
              arg=exp(-drsq/alpot)                                      
              denspp(nprtcl)=denspp(nprtcl)+arg                         
              denspp(issn(k))=denspp(issn(k))+arg                       
            end if                                                      
  507      continue                                                     
          end if                                                        
          if(switch(21)) then                                           
            dsq=(x(1,i)-bplus)**2+x(2,i)**2 - rprsq                     
            if(dsq.gt.0.) then                                          
               ityp(i)=-9999                                            
               go to 107                                                
            end if                                                      
          end if                                                        
          lastcl(i)=0                                                   
          ichbig(i)=0                                                   
          issn(i)=nprtcl                                                
          decay(i)=1.e35                                                
          pr=pfm*ranf(0)**(1./3.)                                       
          cth=1.-2.*ranf(0)                                             
          sth=sqrt(1.-cth**2)                                           
          phi=pi2*ranf(0)                                               
          p(0,i)=sqrt(emnuc**2+pr**2)                                   
          p(3,i)=pr*cth                                                 
          p(1,i)=pr*sth*cos(phi)                                        
          p(2,i)=pr*sth*sin(phi)                                        
          if ((i-n1-ibas2) .le. iz2) then                               
            ityp(i)=2                                                   
            em(i)=emnuc                                                 
          else                                                          
            ityp(i)=1                                                   
            em(i)=emnuc                                                 
          end if                                                        
          npidec(i)=0                                                   
 107    continue                                                        
      if(switch(11))then                                                
          nrbar(1)=n1                                                   
          do 1052 i =ibas1+1,ibas1+n1                                   
            ininew=0                                                    
 1053       continue                                                    
            ntag=0                                                      
            call paulin(i,ntag)                                         
            if(ntag.lt.0)then                                           
              r=rap*ranf(0)**0.3333                                     
              cth=1.-2.*ranf(0)                                         
              sth=sqrt(1.-cth**2)                                       
              phi=pi2*ranf(0)                                           
              x(3,i)=r*cth+z01                                          
              x(1,i)=r*sth*cos(phi)+x0                                  
              x(2,i)=r*sth*sin(phi)                                     
              pr=pfm*ranf(0)**(1./3.)                                   
              cth=1.-2.*ranf(0)                                         
              sth=sqrt(1.-cth**2)                                       
              phi=pi2*ranf(0)                                           
              p(0,i)=sqrt(emproj**2+pr**2)                              
              p(3,i)=pr*cth                                             
              p(1,i)=pr*sth*cos(phi)                                    
              p(2,i)=pr*sth*sin(phi)                                    
              ininew=ininew+1                                           
              xininw=xininw+1.                                          
              if(ininew.le.15) goto 1053                                
              call errex('nucleon initially in occupied state')         
           end if                                                       
 1052    continue                                                       
          if(iz1.ge.0) then                                             
             iofset=n1                                                  
             nrbar(1)=n2                                                
           else                                                         
             iofset=0                                                   
             nrbar(1)=n2                                                
           end if                                                       
          do 1054 i =1+n1+ibas2,nt+ibas2                                
            ininew=0                                                    
 1055       continue                                                    
            ntag=0                                                      
            call paulin(i,ntag)                                         
            if(ntag.lt.0)then                                           
              r=rat*ranf(0)**0.3333                                     
              cth=1.-2.*ranf(0)                                         
              sth=sqrt(1.-cth**2)                                       
              phi=pi2*ranf(0)                                           
              x(3,i)=r*cth+z02                                          
              x(1,i)=r*sth*cos(phi)-x0                                  
              x(2,i)=r*sth*sin(phi)                                     
              pr=pfm*ranf(0)**(1./3.)                                   
              cth=1.-2.*ranf(0)                                         
              sth=sqrt(1.-cth**2)                                       
              phi=pi2*ranf(0)                                           
              p(0,i)=sqrt(emnuc**2+pr**2)                               
              p(3,i)=pr*cth                                             
              p(1,i)=pr*sth*cos(phi)                                    
              p(2,i)=pr*sth*sin(phi)                                    
              ininew=ininew+1                                           
              xininw=xininw+1.                                          
              if(ininew.le.15)goto1055                                  
              call errex('nucleon initially in occupied state')         
           end if                                                       
 1054    continue                                                       
          nrbar(1)=0                                                    
          iofset=0                                                      
      end if                                                            
      if(ida(6).gt.0) then                                              
          do 21 l=ibas1+1,ibas1+n1                                      
           identl=issn(l)                                               
           if(ida(6).eq.1) then                                         
              denspp(identl)=denspp(identl)/salpot*(n1+1)/max(1,n1)     
           else if(ida(6).eq.2) then                                    
              dr=sqrt((x(1,l)-x0)**2+x(2,l)**2+(x(3,l)-z01)**2)         
              rhol=1./(1.+exp((dr-rap)/da(2)))/rhomx1                   
              denspp(identl)=rhol                                       
           end if                                                       
           expon=denspp(identl)**0.33333                                
           do 23 i=1,3                                                  
             p(i,l)=p(i,l)*expon                                        
  23       continue                                                     
           p(0,l)=sqrt(emproj**2+p(1,l)**2+p(2,l)**2+p(3,l)**2)         
  21      continue                                                      
          do 22 l=1+n1+ibas2,nt+ibas2                                   
           identl=issn(l)                                               
           if(ida(6).eq.1) then                                         
              denspp(identl)=denspp(identl)/salpot*(n2+1)/max(1,n2)     
           else if(ida(6).eq.2) then                                    
              dr=sqrt((x(1,l)+x0)**2+x(2,l)**2+(x(3,l)-z02)**2)         
              rhol=1./(1.+exp((dr-rat)/da(2)))/rhomx2                   
              denspp(identl)=rhol                                       
           end if                                                       
           expon=denspp(identl)**0.33333                                
           do 24 i=1,3                                                  
             p(i,l)=p(i,l)*expon                                        
  24       continue                                                     
           p(0,l)=sqrt(emnuc**2+p(1,l)**2+p(2,l)**2+p(3,l)**2)          
  22      continue                                                      
      end if                                                            
      if(switch(21))then                                                
         il=0                                                           
         n1new=0                                                        
         iz1new=0                                                       
         n2new=0                                                        
         iz2new=0                                                       
         ipro=0                                                         
         do 130  l=1,nt                                                 
            if(ityp(l).eq.-9999) go to 130                              
            if(l-ibas1.le.n1.and.l-ibas1.gt.0) then                     
               inc=1                                                    
               ipro=1                                                   
            else                                                        
               inc=2                                                    
            end if                                                      
            il=il+1                                                     
            call store(il,l)                                            
            if(inc.eq.1) then                                           
              n1new=n1new+1                                             
              if(iz1.ge.0)                                              
     a           iz1new=iz1new+(ityp(il)-1)                             
            else                                                        
              n2new=n2new+1                                             
              iz2new=iz2new+(ityp(il)-1)                                
            end if                                                      
  130    continue                                                       
         if(ipro.eq.0.or.iz1.ge.0) then                                 
            iz1=iz1new                                                  
         end if                                                         
         iz2=iz2new                                                     
         n1=n1new                                                       
         n2=n2new                                                       
         nt=n1+n2                                                       
         if(n1+n2.ne.il) then                                           
            call errex(                                                 
     a         'incorrect number of participants for sw(21) in init')   
         end if                                                         
         if(iz1.ge.0) then                                              
            ibas1=0                                                     
            ibas2=0                                                     
         else                                                           
            ibas1=n2                                                    
            ibas2=-n1                                                   
         end if                                                         
      else                                                              
        call shift(ibas1,n1)                                            
        if(n2.gt.0)  call shift(n1+ibas2,n2)                            
      end if                                                            
      iloop=0                                                           
  199 continue                                                          
      if(n1.le.1) go to 217                                             
      if(n1.eq.2.and.iloop.ge.1)  go to 217                             
      do 210 l=1+ibas1,n1+ibas1                                         
      identl=issn(l)                                                    
      dcoef(l)=0.                                                       
      denspp(identl)=0.                                                 
      force(1,l)=0.                                                     
      chi(l)=0.                                                         
      pp=0.                                                             
      do 211 i=1,3                                                      
  211 pp=pp+p(i,l)*p(i,l)                                               
      force(0,l)=emproj**2+pp                                           
      h(l)=p(0,l)*p(0,l) -(emproj**2 +pp)                               
      do 209 k=1+ibas1,l-1                                              
      identk=issn(k)                                                    
      do 208 i=0,3                                                      
      qlk(i)= x(i,l)-x(i,k)                                             
      plk(i)= p(i,l)+p(i,k)                                             
  208 continue                                                          
      dqlksq =qlk(0) *qlk(0)                                            
      splksq =plk(0) *plk(0)                                            
      dqsplk =qlk(0) *plk(0)                                            
      qplkvc =0.                                                        
      do  207 i=1,3                                                     
      dqlksq =dqlksq - qlk(i) *qlk(i)                                   
      splksq =splksq - plk(i) *plk(i)                                   
      qplkvc =qplkvc + qlk(i) *plk(i)                                   
  207 continue                                                          
      dqsplk =dqsplk - qplkvc                                           
      if(-dqlksq.lt. dstint) then                                       
         zlk=dqlksq - dqsplk*dqsplk/splksq                              
         if(ida(14).eq.1)then                                           
            cofact=-exp(dqlksq/al)*(al/dqlksq) *emnuc**4                
            chi(l)=chi(l) + cofact * dqsplk/sqrt(splksq)                
            chi(k)=chi(k) - cofact * dqsplk/sqrt(splksq)                
            dcoef(l)=dcoef(l)+                                          
     a            cofact*(plk(0)*x(0,k)+qplkvc)/sqrt(splksq)            
            dcoef(k)=dcoef(k)+                                          
     a            cofact*(plk(0)*x(0,l)-qplkvc)/sqrt(splksq)            
            force(1,l)=force(1,l)+cofact*plk(0)/sqrt(splksq)            
            force(1,k)=force(1,k)+cofact*plk(0)/sqrt(splksq)            
         else                                                           
            cofact=-exp(dqlksq/al)*(al/dqlksq)                          
            chi(l)=chi(l) + cofact * dqsplk                             
            chi(k)=chi(k) - cofact * dqsplk                             
            dcoef(l)=dcoef(l)+                                          
     a            cofact*(plk(0)*x(0,k)+qplkvc)                         
            dcoef(k)=dcoef(k)+                                          
     a            cofact*(plk(0)*x(0,l)-qplkvc)                         
            force(1,l)=force(1,l)+(cofact*plk(0))                       
            force(1,k)=force(1,k)+cofact*plk(0)                         
         end if                                                         
         if(iz1.ge.0) then                                              
           denspp(identl)=denspp(identl)+(exp(zlk/alpot))               
           denspp(identk)=denspp(identk)+(exp(zlk/alpot))               
         end if                                                         
      end if                                                            
  209 continue                                                          
  210 continue                                                          
      if(iz1.lt.0) then                                                 
          write(8,*) 'this part is not usable at the moment'            
          call errex('change source text')                              
      end if                                                            
      do 220 l=1+ibas1,n1+ibas1                                         
      identl=issn(l)                                                    
      force(0,l)=force(0,l)+                                            
     a        coef1*denspp(identl)+                                     
     b            coef2*(denspp(identl)/salpot)**gamma                  
      h(l)=h(l)-coef1*denspp(identl)-                                   
     a        coef2*(denspp(identl)/salpot)**gamma                      
      p(0,l)=sqrt(force(0,l))                                           
      if(force(1,l).ne.0.0) then                                        
        x(0,l)=dcoef(l)/force(1,l)                                      
      end if                                                            
  220 continue                                                          
  217 continue                                                          
      if(n2.le.1) go to 317                                             
      if(n2.eq.2.and.iloop.ge.1)  go to 317                             
      do 310 l=n1+1+ibas2,nt+ibas2                                      
      identl=issn(l)                                                    
      dcoef(l)=0.                                                       
      denspp(identl)=0.                                                 
      force(1,l)=0.                                                     
      chi(l)=0.                                                         
      pp=0.                                                             
      do 311 i=1,3                                                      
  311 pp=pp+p(i,l)*p(i,l)                                               
      force(0,l)=emnuc**2+pp                                            
      h(l)=p(0,l)*p(0,l) -(emnuc**2 +pp)                                
      do 309 k=n1+1+ibas2,l-1                                           
      identk=issn(k)                                                    
      do 308 i=0,3                                                      
      qlk(i)= x(i,l)-x(i,k)                                             
      plk(i)= p(i,l)+p(i,k)                                             
  308 continue                                                          
      dqlksq =qlk(0) *qlk(0)                                            
      splksq =plk(0) *plk(0)                                            
      dqsplk =qlk(0) *plk(0)                                            
      qplkvc =0.                                                        
      do  307 i=1,3                                                     
      dqlksq =dqlksq - qlk(i) *qlk(i)                                   
      splksq =splksq - plk(i) *plk(i)                                   
      qplkvc =qplkvc + qlk(i) *plk(i)                                   
  307 continue                                                          
      dqsplk =dqsplk - qplkvc                                           
      if(-dqlksq.lt. dstint) then                                       
         zlk=dqlksq - dqsplk*dqsplk/splksq                              
         if(ida(14).eq.1)then                                           
            cofact=-exp(dqlksq/al)*(al/dqlksq) *emnuc**4                
            chi(l)=chi(l)+cofact*dqsplk/sqrt(splksq)                    
            chi(k)=chi(k)-cofact*dqsplk/sqrt(splksq)                    
            dcoef(l)=dcoef(l)+                                          
     a            cofact*(plk(0)*x(0,k)+qplkvc)/sqrt(splksq)            
            dcoef(k)=dcoef(k)+                                          
     a            cofact*(plk(0)*x(0,l)-qplkvc)/sqrt(splksq)            
            force(1,l)=force(1,l)+cofact*plk(0)/sqrt(splksq)            
            force(1,k)=force(1,k)+cofact*plk(0)/sqrt(splksq)            
         else                                                           
            cofact=-exp(dqlksq/al)*(al/dqlksq)                          
            chi(l)=chi(l)+cofact*dqsplk                                 
            chi(k)=chi(k)-cofact*dqsplk                                 
            dcoef(l)=dcoef(l)+                                          
     a            cofact*(plk(0)*x(0,k)+qplkvc)                         
            dcoef(k)=dcoef(k)+                                          
     a            cofact*(plk(0)*x(0,l)-qplkvc)                         
            force(1,l)=force(1,l)+cofact*plk(0)                         
            force(1,k)=force(1,k)+cofact*plk(0)                         
         end if                                                         
         denspp(identl)=denspp(identl)+(exp(zlk/alpot))                 
         denspp(identk)=denspp(identk)+(exp(zlk/alpot))                 
      end if                                                            
  309 continue                                                          
  310 continue                                                          
      do 320 l=n1+1+ibas2,nt+ibas2                                      
      identl=issn(l)                                                    
      force(0,l)=force(0,l)+                                            
     a     coef1*denspp(identl)+coef2*(denspp(identl)/salpot)**gamma    
      h(l)=h(l)-coef1*denspp(identl)-                                   
     a            coef2*(denspp(identl)/salpot)**gamma                  
      p(0,l)=sqrt(force(0,l))                                           
      if(force(1,l).ne.0.0) then                                        
        x(0,l)=dcoef(l)/force(1,l)                                      
      end if                                                            
  320 continue                                                          
  317 continue                                                          
      iloop=iloop+1                                                     
      if(iloop.lt.9) go to 199                                          
      emefpr=0.                                                         
      do 340 l=1+ibas1,n1+ibas1                                         
      ct  =x(0,l)                                                       
      x(0,l)=gamnn*(ct - betann*x(3,l) )                                
      x(3,l)=gamnn*(x(3,l)- betann*ct  )                                
      p0=p(0,l)                                                         
      emefpr=emefpr+p0                                                  
      p(0,l)=gamnn*(p0- betann*p(3,l) )                                 
      p(3,l)=gamnn*(p(3,l)- betann*p0 )                                 
  340 continue                                                          
      if(n1.gt.0) emefpr=emefpr/n1                                      
      emefta=0.                                                         
      do 341 l=n1+1+ibas2,nt+ibas2                                      
      ct  =x(0,l)                                                       
      x(0,l)=gamnn*(ct+ betann*x(3,l) )                                 
      x(3,l)=gamnn*(x(3,l)+ betann*ct )                                 
      p0=p(0,l)                                                         
      emefta=emefta+p0                                                  
      p(0,l)=gamnn*(p0+ betann*p(3,l) )                                 
      p(3,l)=gamnn*(p(3,l)+ betann*p0 )                                 
  341 continue                                                          
      if(n2.gt.0) emefta=emefta/n2                                      
      write(8,*) 'eff. mass of proj/target=',emefpr,emefta              
      dq3mx=0.                                                          
      do 525 l=1+ibas1,n1+ibas1                                         
      do 524 k=n1+1+ibas2,nt+ibas2                                      
      do 522 i=0,3                                                      
      qlk(i)= x(i,l)-x(i,k)                                             
  522 continue                                                          
      dqlksq =qlk(0) *qlk(0)                                            
      do  523 i=1,3                                                     
      dqlksq =dqlksq - qlk(i) *qlk(i)                                   
  523 continue                                                          
      if(dqlksq.gt.-dstint) then                                        
        dq3=-qlk(3)+sqrt(dstint+dqlksq +qlk(3)*qlk(3))                  
        dq3mx=max(dq3mx,dq3)                                            
      end if                                                            
  524 continue                                                          
  525 continue                                                          
      if(dq3mx.gt.0.) then                                              
          do 526 l=1+ibas1,n1+ibas1                                     
          x(3,l)=x(3,l)+dq3mx                                           
  526     continue                                                      
      end if                                                            
       zmnpr=((z01-rap)+gamnn*dq3mx)/gampr                              
       zmxpr=((z01+rap)+gamnn*dq3mx)/gampr                              
       zmnta=(z02-rat)/gamta                                            
       zmxta=(z02+rat)/gamta                                            
       zproj=0.5*(zmxpr+zmnpr)                                          
       tkoll=(zmxta-zmnpr)/(betapr-betata)                              
       zkoll=zmnpr+betapr*tkoll                                         
       zmxpr=zmxpr-zmnpr                                                
       zmnta=zmnta-zmxta                                                
       zmnpr=0.0                                                        
       zmxta=0.0                                                        
       tcoll=cvalu0                                                     
       zcoll=cvalu0                                                     
      do 739 l=1,nt                                                     
       x(0,l)=x(0,l)-tkoll                                              
       x(3,l)=x(3,l)-zkoll                                              
  739 continue                                                          
      if(ida(27).eq.1)  call initfa                                     
      if(ref.eq.'lab') then                                             
        do 740 l=1,nt                                                   
        ct=x(0,l)                                                       
        x(0,l)=gamnn*(ct-betann*x(3,l))                                 
        x(3,l)=gamnn*(x(3,l)-betann*ct)                                 
        p0=p(0,l)                                                       
        p(0,l)=gamnn*(p0- betann*p(3,l))                                
        p(3,l)=gamnn*(p(3,l)-betann*p0)                                 
  740   continue                                                        
      end if                                                            
       do 741 l=1,nt                                                    
        tlast(l)=x(0,l)                                                 
        identl=issn(l)                                                  
        zlast(identl)=x(3,l)-(pnorm(0)*x(0,l)-pnorm(3)*x(3,l))*pnorm(3) 
        if(identl.le.n1old) then                                        
          ylast(identl)=yproj                                           
        else                                                            
          ylast(identl)=ytarg                                           
        end if                                                          
  741  continue                                                         
      do 818 i=0,3                                                      
  818 pcorr(i)=0.                                                       
      do 819 i=0,3                                                      
  819 psyst(i)=0.                                                       
      do 821 l=1,nt                                                     
      do 820 i=0,3                                                      
      psyst(i)=psyst(i)+p(i,l)                                          
  820 continue                                                          
  821 continue                                                          
      write(8,*) 'total momentum after initialization'                  
      write(8,*) (psyst(i),i=0,3)                                       
      return                                                            
      end                                                               
      subroutine shift(ind,n)                                           
       include 'scatter7'                                               
       include 'partblk7'                                               
       cpx=0.                                                           
       cpy=0.                                                           
       cpz=0.                                                           
       do 100 ie=1,n                                                    
        i=ind+ie                                                        
        cpx=cpx+p(1,i)                                                  
        cpy=cpy+p(2,i)                                                  
        cpz=cpz+p(3,i)                                                  
 100   continue                                                         
       cpx=-cpx/float(n)                                                
       cpy=-cpy/float(n)                                                
       cpz=-cpz/float(n)                                                
       do 101 ie=1,n                                                    
        i=ind+ie                                                        
        p(1,i)=p(1,i)+cpx                                               
        p(2,i)=p(2,i)+cpy                                               
        p(3,i)=p(3,i)+cpz                                               
 101   continue                                                         
      return                                                            
      end                                                               
      subroutine spctat(ianf,jcl,ispc)                                  
        include 'scatter7'                                              
        include 'partblk7'                                              
        logical hit                                                     
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'ver   rqmd7 -- 18-jun-89,18:25:20'/                  
      data pronam /'spctat transfers partcls. to spectator region'/     
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      lloop=ianf                                                        
      idelte=0                                                          
      ibar=ianf+nrbar(jcl)                                              
      iend=ibar+nrmes(jcl)                                              
      do 10 l=ianf+1,iend                                               
          if(ispc.eq.0) then                                            
            ispctr=ispect(l)                                            
            if(ispctr.gt.2) then                                        
             iorig=1                                                    
             do 11 k=ianf+1,iend                                        
               if( hit(iorig,k,l,srt,sig,sigel,thit) ) then             
                  ispctr=0                                              
                  go to 12                                              
               end if                                                   
   11        continue                                                   
             ispctr=mod(ispctr,3)                                       
   12        continue                                                   
            end if                                                      
          else                                                          
             ispctr=0                                                   
             ytl=0.5*alog((p(0,l)-p(3,l))/(p(0,l)+p(3,l)))+ytarg        
             ypl=0.5*alog((p(0,l)+p(3,l))/(p(0,l)-p(3,l)))-yproj        
             if(ytl.lt.1.5) ispctr=2                                    
             if(ypl.lt.1.5.and.ispctr.eq.0) ispctr=1                    
             if(ispctr.eq.0.and.decay(l).ne.1.e+35) ispctr=2            
          end if                                                        
          if(ispctr.ne.0) then                                          
             if(l.le.ibar)then                                          
                 nrbar(jcl)=nrbar(jcl)-1                                
             else                                                       
                 nrmes(jcl)=nrmes(jcl)-1                                
             endif                                                      
             idelte=idelte+1                                            
             nspect=nspect+1                                            
             if(ispctr.eq.1) then                                       
                 nsppr=nsppr+1                                          
             else                                                       
                 nspta=nspta+1                                          
             end if                                                     
             ind=ntotma+1-nspect                                        
             if(ind.le.lmax)                                            
     a             call errex('too many spectators in spctat')          
              call store(ind,l)                                         
              lastcl(ind)=-ispctr*100000-lastcl(ind)                    
              go to 10                                                  
          end if                                                        
          lloop=lloop+1                                                 
          if(idelte.eq.0)go to 10                                       
          do 30 i=0,3                                                   
                qdot(i,lloop)=qdot(i,l)                                 
                x(i,lloop)=x(i,l)                                       
                p(i,lloop)=p(i,l)                                       
   30         continue                                                  
              em(lloop)=em(l)                                           
              ityp(lloop)=ityp(l)                                       
              decay(lloop)=decay(l)                                     
              lastcl(lloop)=lastcl(l)                                   
              issn(lloop)=issn(l)                                       
              npidec(lloop)=npidec(l)                                   
              tlast(lloop)=tlast(l)                                     
              ichbig(lloop)=ichbig(l)                                   
   10 continue                                                          
      if(lloop-ianf.ne.nrbar(jcl)+nrmes(jcl))                           
     a        call errex('particle lost in spectat')                    
      return                                                            
      end                                                               
      function ispect(il)                                               
      include 'scatter7'                                                
      include 'partblk7'                                                
      if(ityp(il).eq.-9999) then                                        
         ispect=0                                                       
         return                                                         
      end if                                                            
      ilike=0                                                           
      dpsq=(x(1,il)-bplus)**2+x(2,il)**2 - rprsq                        
      dtsq=(x(1,il)+bplus)**2+x(2,il)**2 - rtasq                        
      if(dpsq.le.0.) ilike=ilike+1                                      
      if(dtsq.le.0.) ilike=ilike+2                                      
      if(ilike.eq.0) then                                               
         ispect=0                                                       
         return                                                         
      else if(ilike.eq.3) then                                          
         deltzp=x(3,il)-  betapl *x(0,il)                               
         if(deltzp.gt.zmxpr) then                                       
          if(nspta.gt.0) then                                           
            deltz=x(3,il)- betata*x(0,il)                               
            yil=0.5*alog( (p(0,il)-p(3,il))/(p(0,il)+p(3,il)) ) +       
     a                 ytarg                                            
            if(yil.lt.1.1 .and. deltz.ge.zmnta.and.                     
     a                                      deltz.le.zmxta) then        
                 ispect=5                                               
                 return                                                 
            end if                                                      
          end if                                                        
          ispect=0                                                      
          return                                                        
         end if                                                         
         deltzp=x(3,il)-betatl*x(0,il)                                  
         if(deltzp.lt.zmnta) then                                       
          if(nsppr.gt.0) then                                           
            yil=0.5*alog( (p(0,il)+p(3,il))/(p(0,il)-p(3,il)) ) -       
     a                 yproj                                            
            deltz=x(3,il)- betapr*x(0,il)                               
            if(yil.lt.1.1 .and. deltz.ge.zmnpr.and.                     
     a                                      deltz.le.zmxpr) then        
                 ispect=4                                               
                 return                                                 
            end if                                                      
          end if                                                        
         end if                                                         
      else if(ilike.eq.1) then                                          
         deltz=x(3,il)- betapr*x(0,il)                                  
         if(deltz.ge.zmnpr.and.                                         
     a        deltz.le.zmxpr) then                                      
            ispect=1                                                    
            return                                                      
         end if                                                         
      else if(ilike.eq.2) then                                          
         deltz=x(3,il)- betata*x(0,il)                                  
         if(deltz.ge.zmnta.and.                                         
     a        deltz.le.zmxta) then                                      
            ispect=2                                                    
            return                                                      
         end if                                                         
      endif                                                             
      ispect=0                                                          
      return                                                            
      end                                                               
      subroutine initfa                                                 
      include 'scatter7'                                                
      include 'partblk7'                                                
      dimension qlk(0:3),plk(0:3)                                       
      ztamx=zmxta* sqrt(1./(1.-betata*betata))/gamnn                    
      zprmn=zmnpr* sqrt(1./(1.-betapr*betapr))/gamnn                    
      tprop=0.5*(zprmn-ztamx)/betann                                    
      if(n1.eq.1.or.n2.eq.1) then                                       
         tprop=tprop-0.1                                                
      end if                                                            
      if(iz1.ge.0) then                                                 
            ibas1=0                                                     
            ibas2=0                                                     
      else                                                              
            ibas1=n2                                                    
            ibas2=-n1                                                   
      end if                                                            
      do 20 l=1+ibas1,n1+ibas1                                          
       x(3,l)=x(3,l)-betann*(tprop-x(0,l))                              
       x(0,l)=tprop                                                     
   20 continue                                                          
      do 30 l=n1+1+ibas2,nt+ibas2                                       
       x(3,l)=x(3,l)+betann*(tprop-x(0,l))                              
       x(0,l)=tprop                                                     
   30 continue                                                          
      do   23 l=1,nt                                                    
   23 chi(l)=0.                                                         
      do   13 l=1,nt                                                    
      do   12 k=l+1,nt                                                  
        do 10 i=0,3                                                     
         plk(i)=p(i,l)+p(i,k)                                           
         qlk(i)=x(i,l)-x(i,k)                                           
   10   continue                                                        
        dqlksq=0.                                                       
        splksq=plk(0)*plk(0)                                            
        dqsplk=qlk(0)*plk(0)                                            
        do   11 i=1,3                                                   
         dqlksq=dqlksq- qlk(i)*qlk(i)                                   
         splksq=splksq- plk(i)*plk(i)                                   
         dqsplk=dqsplk- qlk(i)*plk(i)                                   
   11   continue                                                        
        if(-dqlksq.lt. dstint) then                                     
          if(-dqlksq.lt.0.000001) then                                  
              dqlksq=-0.000001                                          
          end if                                                        
          if(dqlksq.gt. 0.)then                                         
            call errex('initfa:dqlksq>0. not allowed')                  
          end if                                                        
          if(ida(14).eq.1) then                                         
            cofact=exp(dqlksq/al)*abs(al/dqlksq)*em(l)**2 *em(k)**2     
     a                              * dqsplk/sqrt(splksq)               
          else                                                          
             cofact=exp(dqlksq/al)*abs(al/dqlksq) *dqsplk               
          end if                                                        
          chi(l)=chi(l) + cofact                                        
          chi(k)=chi(k) - cofact                                        
        end if                                                          
   12  continue                                                         
       if(chi(l).gt. 10.)   ichbig(l)=1                                 
   13 continue                                                          
      return                                                            
      end                                                               
      subroutine store(j,k)                                             
      include 'scatter7'                                                
      include 'partblk7'                                                
                    do 111 i=0,3                                        
                     x(i,j)=x(i,k)                                      
                     qdot(i,j)=qdot(i,k)                                
                     p(i,j)=p(i,k)                                      
  111               continue                                            
                    em(j)=em(k)                                         
                    ityp(j)=ityp(k)                                     
                    decay(j)=decay(k)                                   
                    lastcl(j)=lastcl(k)                                 
                    issn(j)=issn(k)                                     
                    npidec(j)=npidec(k)                                 
                    tlast(j)=tlast(k)                                   
                    ichbig(j)=ichbig(k)                                 
      return                                                            
      end                                                               
      function ictype(ityp1,ityp2)                                      
       if(ityp1.gt.ityp2) then                                          
         iii=ityp1                                                      
         jjj=ityp2                                                      
       else                                                             
         iii=ityp2                                                      
         jjj=ityp1                                                      
       end if                                                           
       ictype=(iii*(iii-1))/2+jjj                                       
      return                                                            
      end                                                               
      subroutine emuls(izta,nta)                                        
      zrand=ranf(0)                                                     
      if (zrand.lt..123804) then                                        
        nta=1                                                           
        izta=1                                                          
      elseif (zrand.lt..278765) then                                    
        nta=12                                                          
        izta=6                                                          
      elseif (zrand.lt..316723) then                                    
        nta=14                                                          
        izta=7                                                          
      elseif (zrand.lt..436329) then                                    
        nta=16                                                          
        izta=8                                                          
      elseif (zrand.lt..438570) then                                    
        nta=32                                                          
        izta=16                                                         
      elseif (zrand.lt..697722) then                                    
        nta=80                                                          
        izta=35                                                         
      elseif (zrand.lt..998168) then                                    
        nta=108                                                         
        izta=47                                                         
      else                                                              
        nta=127                                                         
        izta=53                                                         
      endif                                                             
      return                                                            
      end                                                               
