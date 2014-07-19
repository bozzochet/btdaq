CPAT,STRINGB  .                                                         
      subroutine qdrot(ind,ident,bx,by,bz)                              
        include 'scatter7'                                              
        include 'partblk7'                                              
       real*4 bex,bey,bez,thetlu,philu                                  
       common /lujets/ nlund,k(2000,2),plund(2000,5)                    
       common/ludat1/mst(40),par(80)                                    
          if(switch(19)) return                                         
          if(itdth(2,ident).eq.0  .or.                                  
     a          tdtg(ident).le.tlast(ind)) return                       
           if(tdtg(ident).eq.decay(ind)) then                           
             idecha=1                                                   
           else                                                         
             idecha=0                                                   
           end if                                                       
           if(bx**2+by**2+bz**2.ge.1.0) then                            
              write(8,*) bx,by,bz,                                      
     a                    bx**2+by**2+bz**2                             
             call errex('qdrot:velocities out of range')                
           end if                                                       
           if(qdot(0,ind).eq.0.) return                                 
           if(ityp(ind).eq.-9999) return                                
           nlund=nlund+3                                                
           if(nlund.gt.2000) call errex('n2dec:nlund>2000')             
           mst(1)=nlund-2                                               
           mst(2)=nlund                                                 
           bex=-bx                                                      
           bey=-by                                                      
           bez=-bz                                                      
           do 298 i=1,4                                                 
            plund(nlund-2,i)=p(mod(i,4),ind)                            
            plund(nlund-1,i)=                                           
     a                       qdot(mod(i,4),ind)/qdot(0,ind)*            
     b                           (tdtg(ident)-tlast(ind))               
            plund(nlund,i)=qdot(mod(i,4),ind)                           
  298      continue                                                     
           call lurobo(0.,0.,bex,bey,bez)                               
           qdotsq=0.0                                                   
           psq=0.0                                                      
           xsq=0.0                                                      
           do 299 i=1,3                                                 
            psq=psq+plund(nlund-2,i)**2                                 
            xsq=xsq+plund(nlund-1,i)**2                                 
            qdotsq=qdotsq+plund(nlund,i)**2                             
  299      continue                                                     
           qdotsq=sqrt(qdotsq)                                          
           psq=sqrt(psq)                                                
           xsq=sqrt(xsq)                                                
           plund(nlund-1,1)=0.0                                         
           plund(nlund-1,2)=0.0                                         
           plund(nlund-1,3)=xsq                                         
           plund(nlund,1)=0.0                                           
           plund(nlund,2)=0.0                                           
           plund(nlund,3)=qdotsq                                        
           bex=-bex                                                     
           bey=-bey                                                     
           bez=-bez                                                     
           if(plund(nlund-2,2).eq.0.0.and.plund(nlund-2,1).eq.0.) then  
             philu=pi2*ranf(0)                                          
           else                                                         
             philu=atan2(plund(nlund-2,2),plund(nlund-2,1))             
           end if                                                       
           thetlu=                                                      
     a       acos(max(-cvalu1,min(cvalu1,plund(nlund-2,3)/psq)))        
           mst(1)=nlund-1                                               
           call lurobo(thetlu,philu,bex,bey,bez)                        
           do 297 i=1,4                                                 
  297      qdot(mod(i,4),ind)=plund(nlund,i)                            
             tdtg(ident)=tlast(ind)+plund(nlund-1,4)                    
             if(idecha.eq.1) decay(ind)=tdtg(ident)                     
           if(ida(38).eq.3.or.ida(38).eq.4) then                        
             do 303 i=1,4                                               
              stork(ident,i)=x(i-1,ind)+                                
     a                       qdot(i-1,ind)/qdot(0,ind)*                 
     b                             (tdtg(ident)-x(0,ind))               
  303        continue                                                   
           end if                                                       
           mst(1)=0                                                     
           mst(2)=0                                                     
           nlund=nlund-3                                                
      return                                                            
      end                                                               
      subroutine trobo(idec,imin,imax,the,phi,bex,bey,bez)              
      include 'scatter7'                                                
      include 'partblk7'                                                
      dimension rot(3,3),pv(3)                                          
      double precision dp(4),dbex,dbey,dbez,dga,dbep,dgabep             
      if(imin.le.0.or.imax.gt.ntotma) then                              
          write(8,*) 'wrong imin,imax in trobo',imin,imax               
          call errex('stop')                                            
      end if                                                            
      if(the**2+phi**2.gt.cvalu4) then                                  
        rot(1,1)=cos(the)*cos(phi)                                      
        rot(1,2)=-sin(phi)                                              
        rot(1,3)=sin(the)*cos(phi)                                      
        rot(2,1)=cos(the)*sin(phi)                                      
        rot(2,2)=cos(phi)                                               
        rot(2,3)=sin(the)*sin(phi)                                      
        rot(3,1)=-sin(the)                                              
        rot(3,2)=0.                                                     
        rot(3,3)=cos(the)                                               
        do 120 i=imin,imax                                              
        do 100 j=1,3                                                    
          if(idec.eq.1) then                                            
             pv(j)=p(j,i)                                               
          else                                                          
             pv(j)=x(j,i)                                               
          end if                                                        
  100   continue                                                        
        do 110 j=1,3                                                    
          if(idec.eq.1) then                                            
            p(j,i)=rot(j,1)*pv(1)+rot(j,2)*pv(2)+rot(j,3)*pv(3)         
          else                                                          
            x(j,i)=rot(j,1)*pv(1)+rot(j,2)*pv(2)+rot(j,3)*pv(3)         
          end if                                                        
  110   continue                                                        
  120   continue                                                        
      endif                                                             
      if(bex**2+bey**2+bez**2.gt.1e-20) then                            
        dbex=bex                                                        
        dbey=bey                                                        
        dbez=bez                                                        
        dga=1d0/dsqrt(1d0-dbex**2-dbey**2-dbez**2)                      
        do 140 i=imin,imax                                              
        if(idec.eq.1) then                                              
           do 130 j=1,4                                                 
  130      dp(j)=p(mod(j,4),i)                                          
        else                                                            
           do 131 j=1,4                                                 
  131      dp(j)=x(mod(j,4),i)                                          
        end if                                                          
        dbep=dbex*dp(1)+dbey*dp(2)+dbez*dp(3)                           
        dgabep=dga*(dga*dbep/(1d0+dga)+dp(4))                           
        if(idec.eq.1) then                                              
           p(1,i)=dp(1)+dgabep*dbex                                     
           p(2,i)=dp(2)+dgabep*dbey                                     
           p(3,i)=dp(3)+dgabep*dbez                                     
           p(0,i)=dga*(dp(4)+dbep)                                      
        else                                                            
           x(1,i)=dp(1)+dgabep*dbex                                     
           x(2,i)=dp(2)+dgabep*dbey                                     
           x(3,i)=dp(3)+dgabep*dbez                                     
           x(0,i)=dga*(dp(4)+dbep)                                      
        end if                                                          
  140   continue                                                        
      endif                                                             
      return                                                            
      end                                                               
      subroutine twodec(                                                
     a                 kay,ifla,iflb,nsave,kt,iwo,                      
     b                                        emjs,bxnew,bynew,bznew)   
      include 'scatter7'                                                
      real*4 bex,bey,bez                                                
      real*4 plund,par,pall(4)                                          
      real*4 zact,emht,emst,emda,emdb                                   
      common /luspbk/ pall,iqqsp,ifirst,ibegin,istart,istfl             
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      common /luopt1/                                                   
     a         iopt(20),kfstr,icqq,mbfix,ideltq,ibbcol,                 
     b         mmult(100),                                              
     a         iprbcq,ilead,iback,ibrtag,icqbar,idlj,klmold,            
     b         klbold,isot,misot,lbreak(100),emst,emda,emdb             
                                                                        
      real*4 xpos(0:5,100)                                              
      common /lupobk/ xpos,ilund                                        
             kay=0                                                      
             ntry=0                                                     
    5        continue                                                   
             ifl1=iflb                                                  
             ifl2=int(0.5+ranf(0))+1                                    
             ifl2=isign(1,iflb)*ifl2                                    
             if(iabs(iflb).lt.10)  ifl2=-ifl2                           
             call luifld(ifl1,0,ifl2,ifdm,kf1)                          
             ifl1=ifla                                                  
             call luifld(ifl1,0,-ifl2,ifdm,kf2)                         
             em1=getmas(0,kf1,1,-1.0)                                   
             em2=getmas(0,kf2,1,-1.0)                                   
             if(em1+em2.ge.emjs) then                                   
               if(ntry.lt.40) go to 5                                   
               return                                                   
             end if                                                     
             lbreak(1)=ifl2                                             
             lbreak(2)=ifla                                             
             kay=1                                                      
             kt=2                                                       
             ibrtag=0                                                   
             nlund=nsave+4                                              
             prel=pcmsr(emjs,em1,em2)                                   
             ct=1.-2.*ranf(0)                                           
             st=sqrt(1.-ct*ct)                                          
             phi=ranf(0)*pi2                                            
             pxrel=prel*cos(phi)*st                                     
             pyrel=prel*sin(phi)*st                                     
             pzrel=prel*ct                                              
             k(nsave+1,1)=20000                                         
             k(nsave+1,2)=0                                             
             k(nsave+2,1)=20000                                         
             k(nsave+2,2)=1                                             
             k(nsave+3,1)=nsave+1                                       
             k(nsave+3,2)=kf1                                           
             k(nsave+4,1)=nsave+2                                       
             k(nsave+4,2)=kf2                                           
             do 10 i=1,3                                                
             plund(nsave+1,i)=0.                                        
             plund(nsave+2,i)=0.                                        
   10        continue                                                   
             plund(nsave+3,1)=pxrel                                     
             plund(nsave+3,2)=pyrel                                     
             plund(nsave+3,3)=pzrel                                     
             plund(nsave+4,1)=-pxrel                                    
             plund(nsave+4,2)=-pyrel                                    
             plund(nsave+4,3)=-pzrel                                    
             plund(nsave+1,4)=emjs                                      
             plund(nsave+1,5)=emjs                                      
             plund(nsave+2,4)=0.                                        
             plund(nsave+2,5)=0.                                        
             plund(nsave+3,4)=sqrt(em1*em1+prel*prel)                   
             plund(nsave+3,5)=em1                                       
             plund(nsave+4,4)=sqrt(em2*em2+prel*prel)                   
             plund(nsave+4,5)=em2                                       
             mmult(1)=0                                                 
             mmult(2)=0                                                 
               ifirst=0                                                 
               ibegin=0                                                 
               istart=0                                                 
               istfl=0                                                  
               iqqsp=0                                                  
               pall(4)=emjs                                             
               do 11 i=1,3                                              
   11          pall(i)=0.0                                              
               iact=nsave+4                                             
               jtact=3                                                  
               call lupos(0.,zact,emht,iact-1,jtact,0,ifl2,iflb)        
               call lupos(0.,zact,emht,iact,4,0,-ifl2,ifla)             
             if(iwo.eq.1) return                                        
             mst(1)=nsave+1                                             
             mst(2)=nsave+6                                             
             do 12 l=1,2                                                
             do 12 ii=1,4                                               
   12        plund(nsave+4+l,ii)=xpos(mod(ii,4),l)                      
             bex=bxnew                                                  
             bey=bynew                                                  
             bez=bznew                                                  
             call lurobo(0.,0.,bex,bey,bez)                             
             mst(1)=0                                                   
            return                                                      
            end                                                         
      subroutine lddec(ind,ntag)                                        
      include 'scatter7'                                                
      dimension ptot(0:3),dr(0:3),qlk(0:3),xold(0:3),xnew(0:3)          
      real*4   xform(0:3)                                               
      dimension p1(0:3)                                                 
      common/ludat1/mst(40),par(80)                                     
      real*4 plund,prtgam,dpar,par,bex,bey,bez                          
      dimension veloc(0:3)                                              
      include 'partblk7'                                                
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
      logical scayes                                                    
      logical issers                                                    
      data drind/ 0.3/                                                  
      if(switch(19)) then                                               
        ntag=-1                                                         
        decay(ind)=1.e+35                                               
        return                                                          
      end if                                                            
      ievnt=lastcl(ind)                                                 
      nsave=nlund                                                       
      ident=issn(ind)                                                   
      itpind=ityp(ind)                                                  
      npidid=npidec(ind)                                                
      if(itpind.eq.8.and.npidid.eq.1) then                              
        if(ranf(0).gt.0.666667) then                                    
          scayes=.true.                                                 
        else                                                            
          scayes=.false.                                                
        end if                                                          
      else                                                              
        scayes=.false.                                                  
      end if                                                            
      call transl(-1,itpind,npidid,kl2)                                 
      ida5=0                                                            
      if(ida(5).ne.0) then                                              
        if(kl2.eq.34.or.kl2.eq.25.or.                                   
     a     (kl2.eq.33.and.npidid.eq.2).or.kl2.eq.35) then               
          ida5=1                                                        
        end if                                                          
      end if                                                            
      if(ida(5).gt.1) then                                              
        if(kl2.eq.24) then                                              
          ida5=1                                                        
        end if                                                          
      end if                                                            
      if(ida5.eq.1) then                                                
        write(10,1663) kl2,p(0,ind),(-p(i,ind),i=1,3),em(ind)           
 1663 format(1x,i4,1x,5(f7.3,1x))                                       
      end if                                                            
      if(iabs(kl2).lt.100)then                                          
        idcold=iabs(kl2)                                                
      else                                                              
        idcold=0                                                        
        call errex('lddec:hadron flavor out of range')                  
      end if                                                            
      nlund=nlund+1                                                     
      if(nlund.ge.2000)                                                 
     a       call errex('lund storage too small, abort in lddec')       
      k(nlund,1)=0                                                      
      k(nlund,2)=kl2                                                    
      pp=p(0,ind)*p(0,ind)                                              
      do 10 i=1,3                                                       
      pp=pp-p(i,ind)*p(i,ind)                                           
  10  plund(nlund,i)=p(i,ind)                                           
      plund(nlund,4)=p(0,ind)                                           
      if(kl2.eq.1) then                                                 
         plund(nlund,5)=0.                                              
      else                                                              
         if(pp.lt.0.) then                                              
             write(8,*) 'lddec: pp.lt.0. pp=',pp,'????????'             
             plund(nlund,5)=0.                                          
         else                                                           
             plund(nlund,5)=sqrt(pp)                                    
         end if                                                         
      end if                                                            
      kfa=iabs(kl2)                                                     
      if(ifndnx.eq.1) then                                              
         idb(23)=-idb(23)                                               
         if(kfa.gt.100) call luiflv(kfa,ifla,iflb,iflc,ksp)             
         if(kfa.gt.100) kfa=76+5*ifla+ksp                               
         if(idb(kfa).ge.1) call luexec                                  
         idb(23)=-idb(23)                                               
         if(nsave+1.eq.nlund) then                                      
            nlund=nsave                                                 
            decay(ind)=1.e+35                                           
            return                                                      
         end if                                                         
         call sveifo(ind,1)                                             
         decay(ind)=x(0,ind)                                            
      else                                                              
         if(kfa.gt.70) then                                             
           write(8,*) 'exotic hadron in lddec',kl2                      
         else                                                           
           if(idb(kfa).lt.0) then                                       
              write(8,*) ident,ityp(ind),npidec(ind)                    
              write(8,*) (itdth(i,ident),i=1,4)                         
              call errex('lddec:stable hadron in decay routine?')       
           end if                                                       
         end if                                                         
         ipnd=nlund                                                     
         call ludecy(ipnd)                                              
      end if                                                            
      call luiflv(kl2,ifla,iflb,iflc,ksp)                               
      if(iflc.eq.0) then                                                
        ldbar=0                                                         
      else                                                              
        ldbar=isign(1,iflc)                                             
        if( (ifla*iflc.lt. 0).or.(ifla*iflb.lt.0))                      
     a      call errex('wrong quark content in subr. lddec, 15')        
      end if                                                            
      delttr=decay(ind)-x(0,ind)                                        
      if(qdot(0,ind).gt.0.) then                                        
        do 207 i=0,3                                                    
  207   veloc(i)=qdot(i,ind)/qdot(0,ind)                                
      else                                                              
        do 208 i=0,3                                                    
  208   veloc(i)=0.0                                                    
        if(abs(delttr).gt.0.001) then                                   
          write(8,*) 'warning:  qdot(0)=0', ind,issn(ind),              
     a               ityp(ind),npidec(ind),x(0,ind),decay(ind)          
          call errex('lddec: qdot(0)=0 ??????')                         
        end if                                                          
      end if                                                            
      do 201 i=0,3                                                      
         x(i,ind)=x(i,ind)+veloc(i)*delttr                              
         xold(i)=x(i,ind)                                               
  201 continue                                                          
      do 69 i=0,3                                                       
   69 ptot(i)=p(i,ind)                                                  
      if(xold(0).lt.tdtg(ident).and.itdth(2,ident).ne.0) then           
         ilogo=itdth(2,ident)                                           
         iexpon=isign(1,ilogo)*mod(iabs(ilogo),100)                     
      else                                                              
         iexpon=0                                                       
      end if                                                            
      lindf=1                                                           
      bx=ptot(1)/ptot(0)                                                
      by=ptot(2)/ptot(0)                                                
      bz=ptot(3)/ptot(0)                                                
      if(em(ind).ne.0.) then                                            
          gam=ptot(0)/em(ind)                                           
      else                                                              
          write(8,*) ind,'p=',(p(i,ind),i=0,3),'em=',em(ind)            
          write(8,*) 'ityp=',ityp(ind),'npid=',npidec(ind)              
          call errex('unstable photon in lddec????')                    
      end if                                                            
      b2=max(cvalu7,bx**2+by**2+bz**2)                                  
      if(ida(25).eq.1.and.ntag.eq.0.and.(ifndnx.eq.0.or.                
     a                                 prtgam(idcold).gt.0.)) then      
           if(switch(23)) then                                          
             ken=itripl                                                 
           else                                                         
             ken=kdt                                                    
           end if                                                       
           ihatot=0                                                     
           do 22 lloop=nsave+1,nlund                                    
            if(k(lloop,1)/10000.gt.1) go to 22                          
            ihatot=ihatot+1                                             
   22      continue                                                     
           write(15,661)                                                
     a      irun,ken,2,0,ihatot,0,issn(ind)*1000+nclcnt(issn(ind)),0    
  661      format(1x,i3,1x,i3,1x,i1,1x,i5,1x,i3,1x,i2,1x,i8,1x,i8)      
  662    format(1x,i4,1x,i4,1x,f8.3,1x,2(f7.3,1x),f8.3,1x,              
     a               f7.3,i3,1x,i8,1x,f8.3,2(1x,f7.3),1x,f8.3)          
           call oldat(1,itold,npold,p0old,pxold,pyold,pzold,emold)      
           write(15,662)itold,npold,p0old,pxold,pyold,pzold,emold,      
     a                            iexpon,ident,xold(0),                 
     b                               xold(1),xold(2),xold(3)            
      end if                                                            
      kl2abs=iabs(kl2)                                                  
      if(ida(50).eq.1.and.                                              
     a         (kl2abs.eq.27.or.kl2abs.eq.28.or.                        
     a          kl2abs.eq.29.or.kl2abs.eq.33.or.kl2abs.eq.35)           
     b                                                    ) then        
        ijet=0                                                          
        nll=0                                                           
        do 14 nl=1,nlund                                                
         kh=mod(k(nl,1),100000)                                         
         ks=k(nl,1)/100000                                              
         if(ks.eq.4) then                                               
           if(kh.eq.ident ) then                                        
             if(ijet.ne.0) call errex('lddec:no unique jetsystem')      
             ijet=1                                                     
             klflag=k(nl,2)                                             
             klchar=k(nl+1,2)                                           
             mst(1)=nl                                                  
             mst(2)=nl+1                                                
             bex=-bx                                                    
             bey=-by                                                    
             bez=-bz                                                    
             call lurobo(0.,0.,bex,bey,bez)                             
             mst(1)=0                                                   
             mst(2)=0                                                   
             pabs=sqrt(plund(nl,1)*plund(nl,1)+                         
     a                   plund(nl,2)*plund(nl,2)+                       
     b                   plund(nl,3)*plund(nl,3)  )                     
             thetqq=acos(max(-cvalu1,min(cvalu1,plund(nl,3)/pabs)))     
             if(plund(nl,2).eq.0.0.and.plund(nl,1).eq.0.) then          
               phiqq=pi2*ranf(0)                                        
             else                                                       
               phiqq=atan2(plund(nl,2),plund(nl,1))                     
             end if                                                     
           end if                                                       
         end if                                                         
  13    continue                                                        
         if(ijet.eq.0.or.ijet.gt.2) then                                
           nll=nll+1                                                    
           do 522 i=1,5                                                 
 522       plund(nll,i)=plund(nl,i)                                     
           k(nll,1)=k(nl,1)                                             
           k(nll,2)=k(nl,2)                                             
         else                                                           
           ijet=ijet+1                                                  
         end if                                                         
  14    continue                                                        
        if(ijet.eq.3) then                                              
          nsave=nsave-2                                                 
          nlund=nlund-2                                                 
          if(ihatot.eq.2) then                                          
            nl=nlund+1                                                  
            mst(1)=nl                                                   
            mst(2)=nl                                                   
            thetlu=thetqq                                               
            philu=phiqq                                                 
            if(klchar.eq.2) then                                        
              zrand=2.*ranf(0)-1.0                                      
              if(zrand.lt.0.) then                                      
                ct=-(-zrand)**0.333333                                  
              else                                                      
                ct=zrand**0.333333                                      
              end if                                                    
            else                                                        
              ct=1.0                                                    
            end if                                                      
            st=sqrt(1.-ct*ct)                                           
            plund(nl,4)=0.0                                             
            plund(nl,3)=ct                                              
            plund(nl,1)=st                                              
            plund(nl,2)=0.0                                             
            call lurobo(thetlu,philu,0.0,0.0,0.0)                       
            emmoth=sqrt(pp)                                             
            emda=plund(nlund-1,5)                                       
            emdb=plund(nlund,5)                                         
            if(emmoth.lt.emda+emdb) then                                
              call errex('lddec: trouble, ida(50)')                     
            else                                                        
              pabs=pcmsr(emmoth,emda,emdb)                              
            end if                                                      
            do 501 j=1,3                                                
             plund(nl,j)=pabs*plund(nl,j)                               
             plund(nl+1,j)=-plund(nl,j)                                 
  501       continue                                                    
            plund(nl,4)=sqrt(emda**2+pabs**2)                           
            plund(nl+1,4)=sqrt(emdb**2+pabs**2)                         
            mst(2)=nl+1                                                 
            bex=bx                                                      
            bey=by                                                      
            bez=bz                                                      
            call lurobo(0.,0.,bex,bey,bez)                              
            do 502 j=1,4                                                
             plund(nlund-1,j)=plund(nl,j)                               
             plund(nlund,j)=plund(nl+1,j)                               
  502       continue                                                    
            mst(1)=0                                                    
            mst(2)=0                                                    
          end if                                                        
        end if                                                          
      end if                                                            
      do 20 lloop=nsave+1,nlund                                         
      ks=k(lloop,1)/10000                                               
      kfm=k(lloop,2)                                                    
      if(ks.gt.1) go to 20                                              
      if(scayes) then                                                   
        if(iabs(kfm).ne.17) then                                        
          call errex('lddec:rho0==>pi+ +pi- expected')                  
        end if                                                          
        kfm=23                                                          
      end if                                                            
      call luiflv(kfm,ifla,iflb,iflc,ksp)                               
      call transl(1,itpl,npidl,kfm)                                     
      if(iflc.eq.0) then                                                
         lnbar=0                                                        
      else                                                              
         lnbar=isign(1,iflc)                                            
      end if                                                            
      lfill=0                                                           
      if(lindf.eq.1.and.((kfm.ne.1.or.lloop.eq.nlund)                   
     a                                 .or.kdt.ge.ndt)) then            
         if(lnbar.eq.ldbar) then                                        
           lfill=1                                                      
         end if                                                         
      end if                                                            
      if(lfill.eq.1) then                                               
          l=ind                                                         
          lindf=0                                                       
      else                                                              
                npion=npion+1                                           
                if((nrmes(kclst)+nrbar(kclst).eq.iclusz(kclst))         
     a            .or.(kfm.eq.1.and.kdt.lt.ndt)) then                   
                   if(kclst.eq.itripl .and. kdt.lt.idtcl                
     b                                         .and. kfm.ne.1) then     
                      call errex('lddec: iclusz(itripl) too low')       
                   end if                                               
                   io=0                                                 
                   do 45 kcl=1,nrclst                                   
  45               io=io+iclusz(kcl)                                    
                   nrclst=nrclst+1                                      
                   nrbar(nrclst)=0                                      
                   nrmes(nrclst)=1                                      
                   iclusz(nrclst)=1                                     
                   nonrel(nrclst)=.false.                               
                   l=io+1                                               
                   lmax=l                                               
                else if(nrmes(kclst)+nrbar(kclst).lt.iclusz(kclst))then 
                   nrmes(kclst)=nrmes(kclst)+1                          
                   l=iofset+nrbar(kclst)+nrmes(kclst)                   
                else                                                    
                   call errex('lddec:nrbar+nrmes.gt.iclusz  ????')      
                end if                                                  
                nprtcl=nprtcl+1                                         
                if(nprtcl.gt.ntotma) then                               
                   call errex('lddec:nprtcl>ntotma, abort')             
                end if                                                  
                issn(l)=nprtcl                                          
      end if                                                            
      if(kdt.le.ndt) then                                               
        lastcl(l)=ievnt                                                 
      else                                                              
        lastcl(l)=-1                                                    
      end if                                                            
      if(l.eq.ind) then                                                 
        nidhad=ident                                                    
      else                                                              
        nidhad=nprtcl                                                   
      end if                                                            
      eml=plund(lloop,5)                                                
      p1(0)=plund(lloop,4)                                              
      do 21 i=1,3                                                       
  21  p1(i)=plund(lloop,i)                                              
      iposit=0                                                          
      if(l.ne.ind) then                                                 
        iposit=1                                                        
        if(iabs(kfm).le.10) then                                        
          iposit=-1                                                     
        end if                                                          
      end if                                                            
      if(iposit.eq.1) then                                              
         deltr=drind*sqrt(ranf(0))                                      
         phi=pi2*ranf(0)                                                
         costet=1.-2.*ranf(0)                                           
         sintet=sqrt(1.-costet*costet)                                  
         xrel=deltr* sintet* sin(phi)                                   
         yrel=deltr* sintet* cos(phi)                                   
         zrel=deltr* costet                                             
         xb=bx*xrel+by*yrel+bz*zrel                                     
         gg=(gam-1.)*xb/b2                                              
         dr(0)=gam*xb                                                   
         dr(1)=gg*bx+xrel                                               
         dr(2)=gg*by+yrel                                               
         dr(3)=gg*bz+zrel                                               
         do 31 i=0,3                                                    
  31     x(i,l)=xold(i)+dr(i)                                           
         wait=100.                                                      
         ixnew=0                                                        
         twait=timelf(wait,eml,p1(1),p1(2),p1(3))                       
  27     continue                                                       
          ixnew=ixnew+1                                                 
          if(ixnew.gt.5) then                                           
              write(8,*) 'ixnew>5 in lddec'                             
              write(8,*) ident                                          
              write(8,*) (p1(i),i=0,3),eml                              
              write(8,*) 'tw=',twait,'xnew(0)=',xnew(0)                 
              do 128 i=0,3                                              
               x(i,l)=xold(i)                                           
 128          continue                                                  
              twait=0.001*p1(0)                                         
          end if                                                        
          do 228 i=0,3                                                  
           xnew(i)=x(i,l)+p1(i)/p1(0)*twait                             
           qlk(i)=xnew(i)-xold(i)                                       
 228      continue                                                      
          irturn=0                                                      
          dqlksq=qlk(0)*qlk(0)                                          
          do 19 i=1,3                                                   
  19       dqlksq=dqlksq-qlk(i)*qlk(i)                                  
          if(ixnew.lt.10) then                                          
            if(dqlksq.le.0..or.qlk(0).le.0.) then                       
                irturn=1                                                
            end if                                                      
          end if                                                        
          if(irturn.eq.1) then                                          
             delqp=qlk(0)*ptot(0)                                       
             pindsq=ptot(0)*ptot(0)                                     
             do 33 i=1,3                                                
              pindsq=pindsq-ptot(i)*ptot(i)                             
              delqp=delqp-qlk(i)*ptot(i)                                
  33         continue                                                   
             val=delqp/pindsq                                           
             arg=(0.01-dqlksq)/pindsq+val*val                           
             if(arg.lt.0.) then                                         
                if(-arg.gt.0.01) then                                   
                   write(8,*) 'arg.lt.0 in lddec,33  ????????',arg      
                   call errex('numerical problem in lddec')             
                end if                                                  
                arg=0.                                                  
             else                                                       
                arg=sqrt(arg)                                           
             end if                                                     
             delts=-val+arg                                             
             do 32 i=0,3                                                
               x(i,l)=xnew(i)+ptot(i)*delts                             
  32         continue                                                   
             twait=0.01                                                 
             go to 27                                                   
          end if                                                        
          do 26 i=0,3                                                   
            x(i,l)=xnew(i)                                              
  26      continue                                                      
      else if(iposit.eq.-1) then                                        
        do 34 i=0,3                                                     
  34    x(i,l)=xold(i)                                                  
      end if                                                            
      tlast(l)=x(0,l)                                                   
      ylast(nidhad)=0.5*log((p1(0)+p1(3))/(p1(0)-p1(3)))                
      zlast(nidhad)=x(3,l)-(pnorm(0)*x(0,l)-pnorm(3)*x(3,l))*pnorm(3)   
      ichbig(l)=0                                                       
      ityp(l)=itpl                                                      
      npidec(l)=npidl                                                   
      em(l)=eml                                                         
      do 28 i=0,3                                                       
   28 p(i,l)=p1(i)                                                      
      kfa=iabs(kfm)                                                     
      if(kfa.lt.100)then                                                
         idc=kfa                                                        
      else                                                              
         idc=76+5*ifla+ksp                                              
      end if                                                            
      if(l.ne.ind.or.kfa.le.16) then                                    
        tdtg(nidhad)=0.0                                                
        itdth(1,nidhad)=0                                               
        itdth(2,nidhad)=0                                               
        itdth(3,nidhad)=0                                               
        itdth(4,nidhad)=0                                               
      end if                                                            
      itdth(5,nidhad)=0                                                 
      if(ntag.eq.0) then                                                
        if(ida(38).eq.3.or.ida(38).eq.4) then                           
          idntl=issn(l)                                                 
          kstor(idntl,1)=kl2                                            
          if(ifndnx.eq.1.and.prtgam(idcold).lt.0.) then                 
            kstor(idntl,1)=0                                            
          end if                                                        
          if(ida(38).eq.3) then                                         
            do 203 i=1,4                                                
  203       stork(idntl,i)=x(i-1,l)                                     
          else                                                          
            do 204 i=1,4                                                
  204       stork(idntl,i)=stork(ident,i)                               
          end  if                                                       
          do 403 i=6,8                                                  
           stork(idntl,i)=p(i-5,l)                                      
  403     continue                                                      
        else if(ida(38).eq.1.or.ida(38).eq.2.or.                        
     a                            ida(38).eq.7) then                    
          if(ida(38).eq.1.or.ida(38).eq.2) then                         
            jeng=1                                                      
          else                                                          
            jeng=0                                                      
          end if                                                        
          emil=em(l)                                                    
          if(issers(jeng,ida(38),ityp(l),npidec(l),emil))then           
                 idntl=issn(l)                                          
                 stork(idntl,1)=stork(ident,1)                          
                 stork(idntl,2)=stork(ident,2)                          
                 stork(idntl,3)=stork(ident,3)                          
                 stork(idntl,4)=stork(ident,4)                          
                 stork(idntl,5)=stork(ident,5)                          
                 kstor(idntl,1)=kstor(ident,1)                          
                 kstor(idntl,2)=kstor(ident,2)                          
                 kstor(idntl,3)=kstor(ident,3)                          
                 kstor(idntl,4)=kstor(ident,4)                          
                 kstor(idntl,5)=kstor(ident,5)                          
                 kstor(idntl,6)=kstor(ident,6)                          
                 kstor(idntl,7)=kstor(ident,7)                          
  888            format(1x,5(f7.3,1x))                                  
  889            format(1x,5(i7,1x),2(i4,1x))                           
  890            format(1x,4(i7,1x),1x,4(f7.3,1x))                      
          end if                                                        
        end if                                                          
      end if                                                            
        if(prtgam(idc).gt.0.) then                                      
          gamact=prtgam(idc)                                            
          decay(l)=x(0,l)+                                              
     a           timelf(gamact,eml,p1(1),p1(2),p1(3))                   
        else                                                            
          decay(l)=1.e+35                                               
        end if                                                          
        iswprp=0                                                        
        if(ida(15).ne.0) then                                           
          if(lnbar.ne.0.or.itdth(2,nidhad).ne.0) then                   
          else                                                          
            do 346 i=0,3                                                
 346        qdot(i,l)=0.                                                
            ichbig(l)=3                                                 
          end if                                                        
        end if                                                          
        if(kdt.le.ndt.and.l.le.iofset+iclusz(kclst).and.ntag.eq.0) then 
          iexpo=0                                                       
          if(xold(0).lt.tdtg(nidhad).and.                               
     a                       itdth(2,nidhad).ne.0) then                 
            call qdrot(l,nidhad,bx,by,bz)                               
            decay(l)=tdtg(nidhad)                                       
            iexpo=iexpon                                                
          end if                                                        
          if(iexpo.eq.0.or.qdot(0,l).eq.0.0) then                       
            do 206 i=0,3                                                
  206       xform(i)=x(i,l)                                             
          else                                                          
            do 202 i=0,3                                                
  202       xform(i)=x(i,l)+qdot(i,l)/qdot(0,l)*                        
     a                          (decay(l)-x(0,l))                       
          end if                                                        
          if(ida(25).eq.1.and.ntag.eq.0.and.(ifndnx.eq.0.or.            
     a                                 prtgam(idcold).gt.0.)) then      
            write(15,662) ityp(l),npidec(l),(p(i,l),i=0,3),em(l),       
     a                     iexpo,issn(l),xform(0),                      
     b                          xform(1),xform(2),xform(3)              
          end if                                                        
          if(ida(42).eq.1.and.ifndnx.eq.0) then                         
            identl=issn(l)                                              
            write(7)                                                    
     &           ityp(l),issn(l),npidec(l),                             
     &           x(0,l),x(1,l),x(2,l),x(3,l),                           
     &           p(0,l),p(1,l),p(2,l),p(3,l),em(l),                     
     &           tdtg(identl),itdth(2,identl),itdth(3,identl)           
            if(iexpo.ne.0) then                                         
              write(7)                                                  
     &           ityp(l),issn(l),npidec(l),                             
     &           (xform(i),i=0,3),                                      
     &           p(0,l),p(1,l),p(2,l),p(3,l),em(l),                     
     &           0.0,0,0                                                
            end if                                                      
          end if                                                        
          call propag(l,-2,iswprp)                                      
        else                                                            
          do 347 i=0,3                                                  
 347      qdot(i,l)=0.                                                  
        end if                                                          
  20   continue                                                         
       if(ida(25).eq.1.and.ntag.eq.0.and.(ifndnx.eq.0.or.               
     a                                 prtgam(idcold).gt.0.)) then      
         if(ida(36).eq.1) then                                          
           if(ifndnx.eq.0) then                                         
             call dnscal(1,ind,-1,dens1)                                
             write(15,663) dens1                                        
           else                                                         
             write(15,663)  0.                                          
           end if                                                       
         end if                                                         
  663    format(1x,1(f6.3,1x))                                          
       end if                                                           
       nlund=nsave                                                      
      return                                                            
      end                                                               
      subroutine  jetcon(inew,nchain,                                   
     a          ema,emb,px,py,pz,emj,itp,npid,identj,bxj,byj,bzj)       
      include 'scatter7'                                                
      real*4 bex,bey,bez,thetlu,philu                                   
      real*4 bextst,beytst,beztst                                       
      real*4 plund,par                                                  
      include 'partblk7'                                                
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      common /seabk/kchain,iofcha,isea(18),psea(0:3,18),isoan,iqqst,    
     a              emsmb                                               
      data lang /5/                                                     
       ijet=0                                                           
       ilist=nlund+1                                                    
       nlu=nlund                                                        
       do 13 nl=1,nlund                                                 
        kh=mod(k(nl,1),100000)                                          
        ks=k(nl,1)/100000                                               
        if(ks.eq.4) then                                                
          if(kh.eq.identj) then                                         
            ilist=nl                                                    
            ijet=1                                                      
          end if                                                        
        end if                                                          
   13  continue                                                         
      call transl(-1,itp,npid,kl2)                                      
       kl2=(-1)**iqcnvt*kl2                                             
       if(ijet.eq.0)then                                                
          nlund=nlund+2                                                 
          if(nlund.ge.2000)                                             
     a       call errex('lund storage too small, abort in string')      
       end if                                                           
       pp=px*px+py*py+pz*pz                                             
       plund(ilist,5)=ema                                               
       plund(ilist+1,5)=emb                                             
       if(mod(inew,2).eq.0) then                                        
         aq=0.5*(1.+sqrt(1.+emj*emj/pp))                                
         plund(ilist,4)=aq*sqrt(pp)                                     
       else                                                             
         aq=0.5/sqrt(pp)*emj                                            
         plund(ilist,4)=0.5*emj                                         
         plund(ilist+1,4)=0.5*emj                                       
       end if                                                           
       plund(ilist,1)=aq*px                                             
       plund(ilist,2)=aq*py                                             
       plund(ilist,3)=aq*pz                                             
       if(mod(inew,2).eq.0) then                                        
         aq=1.-aq                                                       
         plund(ilist+1,4)=abs(aq)*sqrt(pp)                              
       else                                                             
         aq=-aq                                                         
       end if                                                           
       plund(ilist+1,1)=aq*px                                           
       plund(ilist+1,2)=aq*py                                           
       plund(ilist+1,3)=aq*pz                                           
       bex=bxj                                                          
       bey=byj                                                          
       bez=bzj                                                          
       nstrin=0                                                         
       if(nchain.lt.0) then                                             
         nlund=nlund-2                                                  
         kchain=kchain-1                                                
         nchain=nchain+1                                                
       else if(nchain.gt.0) then                                        
         if(mod(inew,2).ne.0)                                           
     a        call errex('jetcon:nchain/inew.ne.0 conflict')            
         zqsum=0.                                                       
         zqbsum=0.                                                      
         kncha=0                                                        
         do 1  incha=1,9                                                
          ncha=9*iofcha+incha                                           
          if(isea(ncha).ne.0) then                                      
            kncha=kncha+1                                               
            zqsum=zqsum+psea(0,ncha)                                    
            zqbsum=zqbsum+psea(3,ncha)                                  
          end if                                                        
    1    continue                                                       
         if(abs(zqsum).gt.1.0.or.abs(zqbsum).gt.1.0)                    
     a       call errex('jetcon: wrong zqsum/zqbsum')                   
         emstcq=sqrt((1.-zqsum)*(1.-zqbsum))*emj                        
         kt=-1                                                          
         call torsts(emstcq,kl2,idum1,idum2,dum1,dum2,kt)               
         if(kt.eq.-1) then                                              
           nchain=nchain-kncha                                          
           do 11 incha=1,9                                              
            ncha=9*iofcha+incha                                         
            isea(ncha)=0                                                
   11      continue                                                     
         else                                                           
           zqsum=1.                                                     
           zqbsum=1.                                                    
           do 5  incha=1,9                                              
            ncha=9*iofcha+incha                                         
            if(isea(ncha).ne.0) then                                    
              zq=psea(0,ncha)                                           
              zqbar=psea(3,ncha)                                        
              if(zq.lt.0.0.or.zq.gt.1.0) then                           
                call errex('jetcon: wrong zq ')                         
              end if                                                    
              if(zqbar.lt.0.0.or.zqbar.gt.1.0) then                     
                call errex('jetcon: wrong zqbar ')                      
              end if                                                    
              iliup=max(nlu,ilist+1)+2*nstrin+1                         
              if(iliup.ge.2000)                                         
     a        call errex('lund storage too small, abort in string')     
              do 2 i=1,4                                                
               plund(iliup,i)=zq*plund(ilist,i)                         
               plund(iliup+1,i)=zqbar*plund(ilist+1,i)                  
    2         continue                                                  
              plund(iliup,5)=0.                                         
              plund(iliup+1,5)=0.                                       
              emsea=(plund(iliup,4)+plund(iliup+1,4))**2                
              do 3 i=1,3                                                
               emsea=emsea-(plund(iliup,i)+plund(iliup+1,i))**2         
    3         continue                                                  
              emsea=sqrt(emsea)                                         
              if(emsea.gt.0.28) then                                    
                nstrin=nstrin+1                                         
                kchain=kchain+1                                         
                psea(0,ncha)=plund(iliup,4)+plund(iliup+1,4)            
                do 6 i=1,3                                              
    6           psea(i,ncha)=plund(iliup,i)+plund(iliup+1,i)            
                zqsum=zqsum-zq                                          
                zqbsum=zqbsum-zqbar                                     
                nlund=nlund+2                                           
                mst(1)=iliup                                            
                mst(2)=iliup+1                                          
                bextst=psea(1,ncha)/psea(0,ncha)                        
                beytst=psea(2,ncha)/psea(0,ncha)                        
                beztst=psea(3,ncha)/psea(0,ncha)                        
                philu=pi2*ranf(0)                                       
                cang=0.10/emsea                                         
                aang=(1.-lang)/((cang+1.)**(1-lang)-cang**(1-lang))     
                dang=-aang/(1.-lang)*cang**(1-lang)                     
                sthet=((1-lang)*                                        
     a                      (ranf(0)-dang)/aang)**(1./(1.-lang))-cang   
                if(sthet.lt.0. .or.sthet.gt.1.0) then                   
                  write(8,*) 'sthet unclear in jetcon',sthet            
                  cthet=cvalu1                                          
                else                                                    
                  cthet=sqrt(cvalu1-sthet)                              
                end if                                                  
                thetlu=acos(cthet)                                      
                if(switch(19)) thetlu=0.                                
                call lurobo(0.,0.,-bextst,-beytst,-beztst)              
                call lurobo(thetlu,philu,bextst,beytst,beztst)          
                call lurobo(0.,0.,bex,bey,bez)                          
                mst(1)=0                                                
                mst(2)=0                                                
                k(iliup,1)=400000+nprtcl+kchain                         
                k(iliup,2)=40                                           
                k(iliup+1,1)=400000                                     
                k(iliup+1,2)=0                                          
              else                                                      
                isea(ncha)=0                                            
                nchain=nchain-1                                         
              end if                                                    
            end if                                                      
    5      continue                                                     
           if(nstrin.gt.0) then                                         
             do 4 i=1,4                                                 
              plund(ilist,i)=zqsum*plund(ilist,i)                       
              plund(ilist+1,i)=zqbsum*plund(ilist+1,i)                  
    4        continue                                                   
             emj=sqrt(zqsum*zqbsum)*emj                                 
             px=plund(ilist,1)+plund(ilist+1,1)                         
             py=plund(ilist,2)+plund(ilist+1,2)                         
             pz=plund(ilist,3)+plund(ilist+1,3)                         
           end if                                                       
         end if                                                         
       end if                                                           
       mst(1)=ilist                                                     
       mst(2)=ilist+1                                                   
       call lurobo(0.,0.,bex,bey,bez)                                   
       mst(1)=0                                                         
       mst(2)=0                                                         
       if(identj.ge.100000) then                                        
         call errex('jetcon:identj>100000 not allowed')                 
       end if                                                           
       k(ilist,1)=400000+identj                                         
       k(ilist,2)=iqqst                                                 
       k(ilist+1,1)=400000                                              
       k(ilist+1,2)=0                                                   
       if(isoan.ne.0) then                                              
         k(ilist+1,2)=isoan                                             
       else if(inew.eq.1) then                                          
         k(ilist+1,2)=2                                                 
       else if(inew.eq.2.or.ijet.eq.1) then                             
         k(ilist+1,2)=3                                                 
       else if(inew.eq.4) then                                          
         k(ilist+1,2)=1                                                 
       end if                                                           
       inew=ijet                                                        
      return                                                            
      end                                                               
      subroutine makjet(ind)                                            
       include 'scatter7'                                               
       include 'partblk7'                                               
       common /seabk/kchain,iofcha,isea(18),psea(0:3,18),isoan,iqqst,   
     a              emsmb                                               
       logical jets                                                     
         ident=issn(ind)                                                
         nchain=0                                                       
         isoan=0                                                        
         iqqst=0                                                        
         emsmb=0.0                                                      
         inew=0                                                         
         emind=em(ind)                                                  
         itpind=ityp(ind)                                               
         npid=npidec(ind)                                               
         if(jets(itpind,npid,emind,0)) then                             
           bx=p(1,ind)/p(0,ind)                                         
           by=p(2,ind)/p(0,ind)                                         
           bz=p(3,ind)/p(0,ind)                                         
           thet=pi*ranf(0)                                              
           phi=pi2*ranf(0)                                              
           pz=cos(thet)                                                 
           px=sin(thet)*cos(phi)                                        
           py=sin(thet)*sin(phi)                                        
           ema=emind                                                    
           emb=0.0                                                      
           call jetcon(inew,nchain,                                     
     a          ema,emb,px,py,pz,emind,itpind,npid,ident,bx,by,bz)      
         end if                                                         
      return                                                            
      end                                                               
      subroutine purglu(ident)                                          
      include 'scatter7'                                                
      real*4 plund                                                      
      include 'partblk7'                                                
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      data ncrit/600/                                                   
       ijet=0                                                           
       nll=0                                                            
       do 13 nl=1,nlund                                                 
        kh=mod(k(nl,1),100000)                                          
        ks=k(nl,1)/100000                                               
        if(ks.eq.4) then                                                
          if(kh.eq.0) then                                              
          else if(kh.eq.ident) then                                     
            ilist=nl                                                    
            ijet=1                                                      
          else if(ijet.ne.2) then                                       
            if(nlund.gt.ncrit) then                                     
              do 18 l=iofset+1,iofset+nrbar(kclst)+nrmes(kclst)         
               if(kh.eq.issn(l).and.ityp(l).eq.-9999) then              
                 if(ityp(l).eq.-9999) then                              
                   ijet=1                                               
                   go to 19                                             
                 else if(decay(l).gt.1.0e+10) then                      
                   ijet=1                                               
                   go to 19                                             
                 end if                                                 
               end if                                                   
   18         continue                                                  
   19         continue                                                  
            end if                                                      
          end if                                                        
        end if                                                          
        if(ijet.eq.0.or.ijet.gt.2) then                                 
          nll=nll+1                                                     
          do 11 i=1,5                                                   
   11     plund(nll,i)=plund(nl,i)                                      
          k(nll,1)=k(nl,1)                                              
          k(nll,2)=k(nl,2)                                              
        else                                                            
          ijet=ijet+1                                                   
        end if                                                          
   13  continue                                                         
       if(ijet.ne.0) nlund=nll                                          
      return                                                            
      end                                                               
