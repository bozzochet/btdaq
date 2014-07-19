CPAT,RSTALL   .                                                         
      subroutine pribd(dtpr)                                            
       include 'scatter7'                                               
       include 'partblk7'                                               
       common/dnsblk/jokall,timpri,bdens,hdens,edens,t11,t22,t33        
     a                   ,tb11,tb22,tb33                                
     b                   ,tp11,tp22,tp33,hdcent                         
     c                   ,rtsq(2,-5:5),drzsq(2,-5:5)                    
     d                   ,rvel(2,-5:5),yldet(2,-5:5)                    
       dimension xact(0:3)                                              
        if(ida(36).eq.3) then                                           
          ietmin=-3                                                     
          ietmax=3                                                      
          do iet=ietmin,ietmax                                          
           do 19 j=1,2                                                  
            rtsq(j,iet)=0.0                                             
            drzsq(j,iet)=0.0                                            
            yldet(j,iet)=0.0                                            
            rvel(j,iet)=0.0                                             
   19      continue                                                     
          end do                                                        
          iofs=0                                                        
          do 31 kcl=1,nrclst                                            
           do 30 il=iofs+1,iofs+nrbar(kcl)+nrmes(kcl)                   
            if(ityp(il).eq.-9999) go to 30                              
            itpil=ityp(il)                                              
            npil=npidec(il)                                             
            emil=em(il)                                                 
            call oldnew(1,itpil,npil,emil,kmult,ltyp)                   
            jact=0                                                      
            if(kmult.eq.0) then                                         
              if(iabs(ltyp).eq.17.or.ltyp.eq.23) then                   
                jact=2                                                  
               end if                                                   
            end if                                                      
            if(ltyp.gt.0.and.ltyp.le.120) then                          
              if(kspin(ltyp).ge.2) jact=1                               
            end if                                                      
            if(jact.eq.0) go to 30                                      
            do 39 i=0,3                                                 
   39       xact(i)=x(i,il)+qdot(i,il)*dtprop                           
            if(tlast(il).gt.xact(0)) go to 30                           
            identl=issn(il)                                             
            if(identl.le.n1old+n2old.and.nclcnt(il).eq.0)go to 30       
            if(tdtg(identl).gt.xact(0).and.                             
     a                     itdth(2,identl).ne.0) go to 30               
            til=xact(0)                                                 
            zil=xact(3)                                                 
            uetil=til+zil                                               
            detil=til-zil                                               
            if(uetil.lt.0.0.or.detil.lt.0.0) go to 30                   
            if(detil.lt.0.0001) go to 30                                
            etail=0.5*log(uetil/detil)                                  
            ieta=sign(1.0,etail)*nint(abs(etail))                       
            if(ieta.lt.ietmin.or.ieta.gt.ietmax) go to 30               
            yil=0.5*log((p(0,il)+p(3,il))/(p(0,il)-p(3,il)))            
            vrt=0.0                                                     
            rtil=0.0                                                    
            do 38 i=1,2                                                 
             vrt=vrt+p(i,il)*xact(i)                                    
             rtil=rtil+xact(i)**2                                       
   38       continue                                                    
            if(rtil.lt.0.01) then                                       
              vrt=0.0                                                   
            else                                                        
              vrt=vrt/sqrt(rtil)/p(0,il)*cosh(yil)                      
            end if                                                      
            tauil=sqrt(til**2-zil**2)                                   
            zetail=tauil*sinh(yil)                                      
            yldet(jact,ieta)=yldet(jact,ieta)+1.0                       
            rtsq(jact,ieta)=rtsq(jact,ieta)+xact(1)**2+xact(2)**2       
            drzsq(jact,ieta)=drzsq(jact,ieta)+                          
     a                       (xact(3)-zetail)**2                        
            rvel(jact,ieta)=rvel(jact,ieta)+vrt                         
   30      continue                                                     
           iofs=iofs+iclusz(kcl)                                        
   31     continue                                                      
          do iet=ietmin,ietmax                                          
           do 29 j=1,2                                                  
            if(yldet(j,iet).eq.0.0) go to 29                            
            rtsq(j,iet)=rtsq(j,iet)/yldet(j,iet)                        
            drzsq(j,iet)=drzsq(j,iet)/yldet(j,iet)                      
            rvel(j,iet)=rvel(j,iet)/yldet(j,iet)                        
   29      continue                                                     
          end do                                                        
          do iet=ietmin,ietmax                                          
           do 28 j=1,2                                                  
            rtsq(j,iet)=sqrt(rtsq(j,iet))                               
            drzsq(j,iet)=sqrt(drzsq(j,iet))                             
   28      continue                                                     
          end do                                                        
   43  format(1x,f4.1,2x,7(f8.3,1x))                                    
          write(13,43)  timpri,(yldet(1,iet),iet=ietmin,ietmax)         
          write(13,43)  timpri,(yldet(2,iet),iet=ietmin,ietmax)         
          write(13,43)  timpri,(rtsq(1,iet),iet=ietmin,ietmax)          
          write(13,43)  timpri,(rtsq(2,iet),iet=ietmin,ietmax)          
          write(13,43)  timpri,(drzsq(1,iet),iet=ietmin,ietmax)         
          write(13,43)  timpri,(drzsq(2,iet),iet=ietmin,ietmax)         
          write(13,43)  timpri,(rvel(1,iet),iet=ietmin,ietmax)          
          write(13,43)  timpri,(rvel(2,iet),iet=ietmin,ietmax)          
        else if(ida(36).eq.2) then                                      
                 delty=0.7                                              
                 iptpi=0                                                
                 ptpi=0.                                                
                 iptba=0                                                
                 idba=0                                                 
                 inba=0                                                 
                 iba=0                                                  
                 ptba=0.                                                
                 poteng=0.0                                             
                 sumpx=0.0                                              
                 sumetd=0.0                                             
                 avdent=0.0                                             
                 avden3=0.0                                             
                 avden2=0.0                                             
                 avvil=0.0                                              
                 iofs=0                                                 
                 do 21 kcl=1,nrclst                                     
                 do 20 il=iofs+1,iofs+nrbar(kcl)+nrmes(kcl)             
                   if(ityp(il).eq.-9999) go to 20                       
                   yil=0.5*log((p(0,il)+p(3,il))/(p(0,il)-p(3,il)))     
                   if(ityp(il).ge.7.and.ityp(il).le.9) then             
                     if(yil.lt.delty.and.yil.gt.-delty) then            
                        iptpi=iptpi+1                                   
                        ptpi=ptpi+sqrt(p(1,il)**2+p(2,il)**2)           
                     end if                                             
                   else if(kcl.ne.kclst) then                           
                     if(yil.lt.delty.and.yil.gt.-delty) then            
                        if(ityp(il).eq.1.or.ityp(il).eq.2) then         
                          iptba=iptba+1                                 
                          ptba=ptba+sqrt(p(1,il)**2+p(2,il)**2)         
                        end if                                          
                        inba=inba+1                                     
                     end if                                             
                   else if(il.le.iofs+nrbar(kcl)) then                  
                     if(yil.lt.delty.and.yil.gt.-delty) then            
                        vil=vquasi(il,dens2,dens3,denst)                
                        avvil=avvil+vil                                 
                        pvec2=p(1,il)**2+p(2,il)**2+p(3,il)**2          
                        poteng=poteng+p(0,il)-sqrt(em(il)**2+pvec2)     
                        avden3=avden3+dens3                             
                        avden2=avden2+dens2                             
                        avdent=avdent+denst                             
                        if(ityp(il).eq.1.or.ityp(il).eq.2) then         
                          iptba=iptba+1                                 
                          ptba=ptba+sqrt(p(1,il)**2+p(2,il)**2)         
                        end if                                          
                        identl=issn(il)                                 
                        if(ityp(il).le.2.and.(itdth(2,identl).eq.0      
     a                    .or.(itdth(2,identl).ne.0.and.                
     b                    tdtg(identl).lt.x(0,il)+qdot(0,il)*dtpr))     
     b                                                          ) then  
                          inba=inba+1                                   
                        else                                            
     a                  if(ityp(il).le.6.and.iabs(npidec(il)).le.1      
     b                                  .and.(itdth(2,identl).eq.0      
     a                    .or.(itdth(2,identl).ne.0.and.                
     d                    tdtg(identl).lt.x(0,il)+qdot(0,il)*dtpr))     
     b                                                          ) then  
                          idba=idba+1                                   
                        else                                            
                          iba=iba+1                                     
                        end if                                          
                     end if                                             
                   end if                                               
                   if(yil.lt.delty.and.yil.gt.-delty) then              
                     if(abs(p(1,il)).lt.1.e-6) then                     
                     else                                               
                       tangle=abs(p(2,il)/p(1,il))                      
                       if(atan(tangle).lt.0.25*pi) then                 
                         sgn=1.0                                        
                       else                                             
                         sgn=-1.0                                       
                       end if                                           
                       sumetd=sumetd+p(0,il)*sgn                        
                     end if                                             
                   end if                                               
                   if(yil.gt.0.0) then                                  
                     sumpx=sumpx+p(1,il)                                
                   else                                                 
                     sumpx=sumpx-p(1,il)                                
                   end if                                               
   20            continue                                               
                 iofs=iofs+iclusz(kcl)                                  
   21            continue                                               
                 itba=inba+idba+iba                                     
                 if(itba.gt.0) then                                     
                    if(iptba.gt.0) ptba=ptba/iptba                      
                    poteng=poteng/itba                                  
                    avdent=avdent/itba                                  
                    avden3=avden3/itba                                  
                    avden2=avden2/itba                                  
                    avvil=avvil/itba                                    
                 end if                                                 
         call dnscal(0,i1,i2,dens1)                                     
   33  format(1x,f4.1,2x,14(f6.3,1x),1x,f5.2,2x,f8.3,1x,f8.3)           
         write(13,33)  timpri,                                          
     a        bdens,hdens,edens,t11,t22,t33,                            
     b          tb11,tb22,tb33,                                         
     d           ptba,poteng,                                           
     c          tp11,tp22,tp33,                                         
     e          hdcent,sumpx,sumetd                                     
         hdcent=0.0                                                     
         tp11=0.0                                                       
         tp22=0.0                                                       
         tp33=0.0                                                       
       end if                                                           
      return                                                            
      end                                                               
      subroutine dnscal(iswd,i2,i1,dens)                                
      include 'scatter7'                                                
      include 'partblk7'                                                
       common/dnsblk/jokall,timpri,bdens,hdens,edens,t11,t22,t33        
     a                   ,tb11,tb22,tb33                                
     b                   ,tp11,tp22,tp33,hdcent                         
     c                   ,rtsq(2,-5:5),drzsq(2,-5:5)                    
     d                   ,rvel(2,-5:5),yldet(2,-5:5)                    
       dimension qlk(0:3),plk(0:3),xind(0:3)                            
       data ds2 /2.165/                                                 
      dens=0.                                                           
      bdens=0.0                                                         
      hdens=0.0                                                         
      edens=0.0                                                         
      t11=0.0                                                           
      t22=0.0                                                           
      t33=0.0                                                           
      tb11=0.0                                                          
      tb22=0.0                                                          
      tb33=0.0                                                          
      ds2n=(pi*ds2)**1.5                                                
      nrb=nrbar(kclst)                                                  
      nrm=nrmes(kclst)                                                  
      if(iswd.eq.0) then                                                
        do 8 i=1,3                                                      
    8   xind(i)=0.0                                                     
        xind(0)=timpri                                                  
        i1=0                                                            
        i2=0                                                            
        nrt=nrb+nrm                                                     
      else                                                              
        do 9 i=0,3                                                      
    9   xind(i)=x(i,i2)+qdot(i,i2)*dtprop                               
        nrt=nrb                                                         
      end if                                                            
      do 30 l=1,nrt                                                     
       il=iofset+l                                                      
       identl=issn(il)                                                  
       if(iswd.eq.0) then                                               
         itypl=ityp(il)                                                 
         if(itypl.eq.-9999) go to 30                                    
         npidl=npidec(il)                                               
         eml=em(il)                                                     
         call oldnew(1,itypl,npidl,eml,kmultl,ltypl)                    
         ltypa=iabs(ltypl)                                              
         if(kspin(ltypa).ge.2) then                                     
           if(ltypl.gt.0) then                                          
             idb=1                                                      
           else                                                         
             idb=-1                                                     
           end if                                                       
         else                                                           
           idb=0                                                        
         end if                                                         
       else                                                             
         if(il.eq.i2) go to 30                                          
         if(il.eq.i1) go to 30                                          
         if(ityp(il).gt.11.or.                                          
     a      (ityp(il).ge.7.and.ityp(il).le.9)) go to 30                 
       end if                                                           
       if(qdot(0,il).le.0.) then                                        
         dtpr=0.                                                        
       else                                                             
         dtpr=(xind(0)-x(0,il))/qdot(0,il)                              
       end if                                                           
       do 11 i=0,3                                                      
        qlk(i)=-x(i,il)-qdot(i,il)*dtpr+xind(i)                         
   11  continue                                                         
       xbet=0.                                                          
       xsq=0.                                                           
       bsq=0.                                                           
       do 12 i=1,3                                                      
       if(qdot(0,il).gt.0.) then                                        
         plk(i)=qdot(i,il)/qdot(0,il)                                   
       else                                                             
         plk(i)=0.                                                      
       end if                                                           
       xbet=xbet+qlk(i)*plk(i)                                          
       xsq=xsq+qlk(i)*qlk(i)                                            
       bsq=bsq+plk(i)*plk(i)                                            
   12  continue                                                         
       if(bsq.gt.1.e-3) then                                            
         xparsq=xbet*xbet/bsq                                           
       else                                                             
         xparsq=0.                                                      
       end if                                                           
       if(bsq.lt.0.0.or.bsq.ge.1.) then                                 
         write(8,*) 'bsq out of range in dnscal',bsq                    
         go to 30                                                       
       end if                                                           
       gamil=1./sqrt(1.-bsq)                                            
       xtrsq=xsq-xparsq                                                 
       if(sqrt(max(cvalu0,xsq)).gt.3.) go to 30                         
       densil=gamil*exp(-(xtrsq+gamil*gamil*xparsq)/ds2)/ds2n           
       dens=dens+densil                                                 
       if(iswd.eq.0) then                                               
         identl=issn(il)                                                
         if(tlast(il).gt.xind(0)) go to 30                              
         if(tdtg(identl).gt.xind(0).and.itdth(2,identl).ne.0) go to 30  
         if(issn(il).le.n1old+n2old.and.nclcnt(il).eq.0) go to 30       
         hdens=hdens+densil                                             
         edens=edens+p(0,il)*densil                                     
         t11=t11+p(1,il)**2*densil/p(0,il)                              
         t22=t22+p(2,il)**2*densil/p(0,il)                              
         t33=t33+p(3,il)**2*densil/p(0,il)                              
         if(idb.ne.0) then                                              
           bdens=bdens+densil*idb                                       
           tb11=tb11+p(1,il)**2*densil/p(0,il)*idb                      
           tb22=tb22+p(2,il)**2*densil/p(0,il)*idb                      
           tb33=tb33+p(3,il)**2*densil/p(0,il)*idb                      
         end if                                                         
       end if                                                           
   30 continue                                                          
      dens=dens/rho0                                                    
      return                                                            
      end                                                               
      subroutine rstall                                                 
        include 'scatter7'                                              
        include 'partblk7'                                              
      real*4 plund                                                      
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      nlund=0                                                           
      do 1000 l=1,ntotma                                                
       do  999 i=0,3                                                    
        x(i,l)=0.0                                                      
        p(i,l)=0.0                                                      
        qdot(i,l)=0.0                                                   
        itdth(i+1,l)=0                                                  
  999   continue                                                        
        itdth(5,l)=0                                                    
        tlast(l)=0.0                                                    
        tdtg(l)=0.0                                                     
        em(l)=0.0                                                       
        denspp(l)=0.0                                                   
       do  998 i=0,3                                                    
        frfp(i,l)=0.0                                                   
  998   continue                                                        
        ityp(l)=0                                                       
        lastcl(l)=0                                                     
        kstor(l,1)=0                                                    
        ichbig(l)=0                                                     
        npidec(l)=0                                                     
        nclcnt(l)=0                                                     
        issn(l)=0                                                       
        idud(1,l)=0                                                     
        decay(l)=0.0                                                    
        datsto(l)=0.0                                                   
        ylast(l)=0.0                                                    
        zlast(l)=0.0                                                    
 1000 continue                                                          
      write(8,*) 'reset completed'                                      
      return                                                            
      end                                                               
