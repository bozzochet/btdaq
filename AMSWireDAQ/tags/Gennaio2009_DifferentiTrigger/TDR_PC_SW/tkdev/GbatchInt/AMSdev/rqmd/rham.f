CPAT,RHAM     .                                                         
      subroutine relham(itrig,ntt,nrpion)                               
        include 'scatter7'                                              
        include 'partblk7'                                              
        include 'storbig'                                               
       common/dnsblk/jokall,timpri,bdens,hdens,edens,t11,t22,t33        
     a                   ,tb11,tb22,tb33                                
     b                   ,tp11,tp22,tp33,hdcent                         
     c                   ,rtsq(2,-5:5),drzsq(2,-5:5)                    
     d                   ,rvel(2,-5:5),yldet(2,-5:5)                    
       dimension v4dum(0:3)                                             
      real*4 force,dcoef                                                
      common/blocal/ ngbor(ntotma),force(0:3,ntotma),dcoef(ntotma)      
      real*8 qlk,plk,dqlksq,splksq,dqsplk,zlk,qtlk                      
      real*8 spkmsq,dqspkm                                              
       logical potyes                                                   
      dimension qlk(0:3),plk(0:3),xrel(0:3)                             
      dimension qtlk(0:3),xind(0:3)                                     
      dimension dphidq(0:3,2),dphidp(0:3,2)                             
      dimension dhldqm(0:3),dhldpm(0:3),dhmdql(0:3),dhmdpl(0:3)         
      dimension dcldqm(0:3),dcldpm(0:3)                                 
      logical clust,test                                                
      character*3  frame                                                
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data dq0max/ 25./                                                 
      data frcrr1/ 0.3/                                                 
      data vernum/'version rqmd 2.3 --10-sep-96'/                       
      data pronam /' r e l h a m  propagates the particles '/           
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'----------------version number report -------------' 
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
        nghbmx=0                                                        
        nttomx=0                                                        
        dq0max=dq0max*dt/0.2                                            
      end if                                                            
      janr=0                                                            
      if(ida(23).ne.0.or.itrig.eq.-1) janr=1                            
      if(ntt.gt.nbarym) then                                            
           call errex('too large baryon number in relham')              
      end if                                                            
      nttot=ntt+nrpion                                                  
      dtprop=0.                                                         
      if(switch(23).or.ida(34).eq.2) then                               
         isimpl=1                                                       
      else                                                              
         isimpl=0                                                       
      end if                                                            
      if((isimpl.eq.0.and.nttot.gt.nclmx).or.                           
     a             (isimpl.eq.1.and.nttot.gt.ntotma)) then              
           call errex('too large particle cluster number in relham')    
      else if(nttot.le.1) then                                          
           write(8,*)'warning:cluster with 1 particle in relham'        
           return                                                       
      end if                                                            
      n1n2s=n1old+n2old                                                 
      ntotm1=nttot-1                                                    
      if(janr.eq.1) then                                                
            ntm1=0                                                      
      else if(ida(8).eq.1) then                                         
         if(nrpion.eq.0) then                                           
            ntm1=ntt-1                                                  
         else                                                           
            ntm1=ntt                                                    
         end if                                                         
      else if(ida(8).eq.3) then                                         
            ntm1=0                                                      
      else                                                              
         ntm1=nttot-1                                                   
      end if                                                            
      ntm2m1=nttot+ntm1                                                 
      ntm2m2=ntm2m1 -1                                                  
      if(ida(8).eq.2) then                                              
         itrig=0                                                        
      else if(ida(8).eq.3) then                                         
         itrig=1                                                        
      end if                                                            
      dtau=dt                                                           
    1 continue                                                          
      if(janr.ne.1) then                                                
       do 6 l=1,ntm2m1                                                  
        do 5 k=1,ntm2m2                                                 
         at(k,l)=0.                                                     
    5   continue                                                        
    6  continue                                                         
      end if                                                            
      do 17 l=1,nttot                                                   
        prop(l)=.true.                                                  
   17 continue                                                          
       do 18 l=1,nttot                                                  
        il=iofset+l                                                     
        ichbig(il)=isign(1,ichbig(il))*mod(iabs(ichbig(il)),1000)       
        if(ida(7).eq.1) then                                            
          ident=issn(il)                                                
          if(itdth(2,ident).ne.0.and.                                   
     a            tdtg(ident).le.x(0,il)) then                          
             tdt1i=tdtg(ident)                                          
             call ofshel(4,il,ident)                                    
             call dupdat(il,tdt1i)                                      
          end if                                                        
        end if                                                          
   18  continue                                                         
      icaltp=0                                                          
      jcaltp=0                                                          
      kcaltp=0                                                          
      if(ida(36).gt.1.and.jokall.eq.0) then                             
        icaltp=1                                                        
        timpra=timpri-0.01                                              
        do 946 l=1,nttot                                                
         do 945 i=1,3                                                   
  945    force(i,l)=0.                                                  
         il=iofset+l                                                    
         if(x(0,il).lt.timpri) kcaltp=1                                 
         rtpote=sqrt(x(1,il)**2+x(2,il)**2)                             
         rlpote=abs(x(3,il))*gamnn                                      
         if(rtpote.lt.frcrr1*rr1.and.rlpote.lt.frcrr1*rr1) then         
           force(0,l)=0.0                                               
         else                                                           
           force(0,l)=-1.0                                              
         end if                                                         
  946   continue                                                        
      end if                                                            
      if(isimpl.eq.1) then                                              
          do 345 l=1,nttot                                              
           il=iofset+l                                                  
           ngbsum(l)=l-1                                                
           nghbor(l)=l                                                  
           identl=issn(il)                                              
           ncolil=nclcnt(identl)                                        
           if(switch(23)) then                                          
             ngbrnr(l)=-1                                               
             if(ncolil.eq.0.and.identl.le.n1n2s) then                   
               do 461 i=0,3                                             
                dhdq(i,l)=0.                                            
  461          continue                                                 
               dhdp(0,l)=2.*p(0,il)                                     
               dhdp(1,l)=0.                                             
               dhdp(2,l)=0.                                             
               if(identl.le.n1old) then                                 
                  dhdp(3,l)=2.*betapr*p(0,il)                           
               else                                                     
                  dhdp(3,l)=2.*betata*p(0,il)                           
               end if                                                   
             else if(itdth(2,identl).ne.0.and.                          
     a                       tdtg(identl).gt.x(0,il)                    
     b                            .and.qdot(0,il).gt.0.0) then          
               versko=p(0,il)/qdot(0,il)                                
               do 400 i=0,3                                             
                dhdq(i,l)=0.                                            
                dhdp(i,l)=2.*qdot(i,il)*versko                          
  400          continue                                                 
             else                                                       
               do 401 i=0,3                                             
                dhdq(i,l)=0.                                            
                dhdp(i,l)=2.*p(i,il)                                    
  401          continue                                                 
             end if                                                     
           else                                                         
             ngbrnr(l)=1                                                
             rhoeff=0.                                                  
             rhoef2=0.                                                  
             do 407 i=0,3                                               
              xind(i)=0.                                                
  407        continue                                                   
             itypl=ityp(il)                                             
             npidl=npidec(il)                                           
             iforc=0                                                    
             if(identl.gt.n1old.and.identl.le.n1n2s                     
     a                   .and.ncolil.eq.0) then                         
             else if(switch(6)) then                                    
             else if(itdth(2,identl).ne.0.and.                          
     a                              tdtg(identl).gt.x(0,il)) then       
             else if(.not.potyes(il,iqql)) then                         
             else                                                       
               iforc=1                                                  
             end if                                                     
             if(iforc.eq.0) then                                        
               if(ncolil.eq.0.and.identl.le.nt) then                    
                 do 462 i=0,3                                           
                  dhdq(i,l)=0.                                          
  462            continue                                               
                 dhdp(0,l)=2.*p(0,il)                                   
                 dhdp(1,l)=0.                                           
                 dhdp(2,l)=0.                                           
                 if(identl.le.n1old) then                               
                  dhdp(3,l)=2.*betapr*p(0,il)                           
                 else                                                   
                  dhdp(3,l)=2.*betata*p(0,il)                           
                 end if                                                 
               else                                                     
                 do 463 i=0,3                                           
                  dhdq(i,l)=0.                                          
                  dhdp(i,l)=2.*p(i,il)                                  
  463            continue                                               
               end if                                                   
             else                                                       
                do 408 i=0,3                                            
                 xind(i)=x(i,il)                                        
  408           continue                                                
                gamta=1./sqrt(1.-betata*betata)                         
                ti1=gamta*(xind(0)-betata*xind(3))                      
                xind(3)=gamta*(xind(3)-betata*xind(0))                  
                xind(0)=ti1                                             
                xind(1)=xind(1)+bplus                                   
                xind(3)=xind(3)-gamta*0.5*(zmxta+zmnta)                 
                drsq=xind(1)**2+xind(2)**2+xind(3)**2                   
                dr=sqrt(drsq)                                           
                rhoeff=1./(exp((dr-rr2)/da(2))+1.)                      
                rhoprf=-1./da(2)*exp((dr-rr2)/da(2))/                   
     a                                        (1.+exp((dr-rr2)/da(2)))  
                splksq=(p(0,il)+emnuc*gamta)**2-(p(1,il)**2+p(2,il)**2  
     a                              +(p(3,il)+emnuc*gamta*betata)**2)   
                if(splksq.gt.(em(il)+emnuc)**2) then                    
                  emil=em(il)                                           
                  ecms=sqrt(splksq)                                     
                  pr2=pcmsr(ecms,emil,emnuc)**2                         
                else                                                    
                  pr2=0.                                                
                end if                                                  
                if(ida(32).eq.0.or.ida(32).eq.1) then                   
                  rcor=1.                                               
                else                                                    
                  rcor=r0cor                                            
                end if                                                  
                rhoef2=rhoeff                                           
                rhoef3=rhoeff                                           
                if(dr.gt.0.001) then                                    
                  xind(0)=0.                                            
                  do 409 i=1,3                                          
                   xind(i)=xind(i)/dr*2.*emnuc*rhoprf*                  
     a                 (0.5*alpha*rhoef2*rcor+beta*gamma/(gamma+1.)*    
     b                                rhoef3*rcor*(rhoef3*rcor)**gm1)   
  409             continue                                              
                  ti1=gamta*(xind(0)+betata*xind(3))                    
                  xind(3)=gamta*(xind(3)+betata*xind(0))                
                  xind(0)=ti1                                           
                else                                                    
                  do 465 i=0,3                                          
  465             xind(i)=0.                                            
                end if                                                  
                do 402 i=0,3                                            
                 dhdq(i,l)=xind(i)                                      
                 dhdp(i,l)=2.*p(i,il)                                   
  402           continue                                                
             end if                                                     
           end if                                                       
  345      continue                                                     
           go to 1809                                                   
      end if                                                            
      ngbsum(1)=0                                                       
      ngbs=0                                                            
      do 350 k=1,nttot                                                  
       if(k.gt.1) then                                                  
          ngbs=ngbsum(k-1)+ngbrnr(k-1)+2                                
          ngbsum(k)=ngbs                                                
          if(ngbsum(k)+nclmx.gt.ngbrmx*nclmx) then                      
             write(8,*) 'k,nttot,ngbsum(k),ngbrmx*nclmx'                
             write(8,*)  k,nttot,ngbsum(k),ngbrmx*nclmx                 
             call errex('array nghbor(1 ... ngbrmx*nclmx) too small')   
          end if                                                        
       end if                                                           
       ngbrnr(k)=0                                                      
       nghbor(ngbs+1)=0                                                 
       ik=iofset+k                                                      
       do  346 l=1,k-1                                                  
         do  346 lnr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)                    
         if(nghbor(lnr).eq.k) then                                      
           ngbrnr(k)=ngbrnr(k)+1                                        
           nghbor(ngbs+ngbrnr(k))=l                                     
         end if                                                         
  346  continue                                                         
       ngbrnr(k)=ngbrnr(k)+1                                            
       nghbor(ngbs+ngbrnr(k))=k                                         
       if(ityp(ik).eq.-9999.or.ichbig(ik).eq.3) then                    
         prop(k)=.false.                                                
         go to 350                                                      
       end if                                                           
       identk=issn(ik)                                                  
       ncolik=nclcnt(identk)                                            
       do  349 l=k+1,nttot                                              
        il=iofset+l                                                     
        if(ityp(il).eq.-9999.or.ichbig(il).eq.3) go to 349              
        identl=issn(il)                                                 
        ncolil=nclcnt(identl)                                           
        if(ida(34).eq.1.and.min(identl,identk).gt.n1old.and.            
     a       max(identk,identl).le.nt.and.max(ncolil,ncolik).eq.0)      
     b                                                       go to 349  
        do  347 i=0,3                                                   
         qlk(i)=x(i,il)-x(i,ik)                                         
         plk(i)=p(i,il)+p(i,ik)                                         
  347   continue                                                        
        dqlksq=qlk(0)*qlk(0)                                            
        splksq=plk(0)*plk(0)                                            
        dqsplk=qlk(0)*plk(0)                                            
        do   348 i=1,3                                                  
         dqlksq=dqlksq-qlk(i)*qlk(i)                                    
         splksq=splksq-plk(i)*plk(i)                                    
         dqsplk=dqsplk-qlk(i)*plk(i)                                    
  348   continue                                                        
         if(dqlksq.lt.0.16)then                                         
            if(mod(ichbig(il),2).ne.1.and.mod(ichbig(ik),2).ne.1) then  
              ichbig(il)=2                                              
              ichbig(ik)=2                                              
            end if                                                      
         end if                                                         
        if(dqlksq.gt.0.) then                                           
          if(qlk(0).gt.0.) then                                         
            prop(l)=.false.                                             
          else                                                          
            prop(k)=.false.                                             
          end if                                                        
        end if                                                          
        dqsq(l*(l-3)/2+k+1)=dqlksq                                      
        spsq(l*(l-3)/2+k+1)=splksq                                      
        dqsp(l*(l-3)/2+k+1)=dqsplk                                      
        if(k.eq.(l-1)) then                                             
           clust=.true.                                                 
        else if((l.eq.nttot).and.(k.eq.1)) then                         
           clust=.true.                                                 
        else                                                            
           clust=.false.                                                
        end if                                                          
        if(-dqlksq.lt.dstint.or.clust) then                             
           ngbrnr(k)=ngbrnr(k)+1                                        
           nghbor(ngbs+ngbrnr(k))=l                                     
        end if                                                          
  349  continue                                                         
  350 continue                                                          
      if(ida(23).ne.0) go to 300                                        
      do 2813 l=1,nttot                                                 
      chi(l)=0.                                                         
      if(.not.prop(l)) go to 2813                                       
      il=iofset+l                                                       
      do 2812 knr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)                       
        k=nghbor(knr)                                                   
      if(k.ge.l) go to 2812                                             
      ik=iofset+k                                                       
      if(.not.prop(k)) go to 2812                                       
      dqlksq=dqsq(l*(l-3)/2+k+1)                                        
      if(-dqlksq.lt.dstint) then                                        
         if(-dqlksq.lt.0.000001) then                                   
             dqlksq=-0.000001                                           
         end if                                                         
         splksq=spsq(l*(l-3)/2+k+1)                                     
         dqsplk=dqsp(l*(l-3)/2+k+1)                                     
         if(dqlksq.gt.0.)then                                           
            call errex('dqlksq>0. not allowed,1')                       
         end if                                                         
         if(ida(14).eq.1) then                                          
            cofact=exp(dqlksq/al)*abs(al/dqlksq)*em(il)**2*em(ik)**2    
     a                              *dqsplk/sqrt(splksq)                
         else                                                           
            cofact=exp(dqlksq/al)*abs(al/dqlksq)*dqsplk                 
         end if                                                         
         if(ichbig(ik).ne.1)then                                        
            chi(l)=chi(l)+cofact                                        
         end if                                                         
         if(ichbig(il).ne.1)then                                        
            chi(k)=chi(k)-cofact                                        
         end if                                                         
      end if                                                            
 2812 continue                                                          
 2813 continue                                                          
        do 3813 l=1,nttot                                               
         il=iofset+l                                                    
         if(.not.prop(l)) go to 3813                                    
         if(ichbig(il).eq.2) then                                       
         else if(ichbig(il).eq.0) then                                  
            if(chi(l).lt.10.)go to 3813                                 
            ichbig(il)=1                                                
         else if(ichbig(il).eq.1) then                                  
            if(chi(l).ge.1.) goto 3813                                  
            ichbig(il)=0                                                
         else if(ichbig(il).eq.3) then                                  
         else                                                           
            call errex('ichbig not allowed in relham')                  
         end if                                                         
 3813   continue                                                        
        do 3814 l=1,nttot                                               
         il=iofset+l                                                    
         if(.not.prop(l)) then                                          
             if(ichbig(il).ne.3) ichbig(il)=1                           
         else if(ichbig(il).eq.1) then                                  
             prop(l)=.false.                                            
             chi(l)=0.                                                  
         else if(ichbig(il).eq.2) then                                  
             ichbig(il)=0                                               
         end if                                                         
 3814   continue                                                        
  300 continue                                                          
      do 310 l=1,nttot                                                  
        il=iofset+l                                                     
        if(.not.prop(l)) then                                           
           nghbor(ngbsum(l)+1)=10000*ngbrnr(l)+nghbor(ngbsum(l)+1)      
           ngbrnr(l)=0                                                  
           go to 310                                                    
        else                                                            
           itypl=ityp(il)                                               
           npidl=npidec(il)                                             
           if(switch(6)) then                                           
               ngbrnr(l)=-ngbrnr(l)                                     
           else if(l.gt.ntt) then                                       
               ngbrnr(l)=-ngbrnr(l)                                     
           else                                                         
             if(potyes(il,iqql)) then                                   
               if(ida(35).gt.1) then                                    
                 ngbor(l)=kpote(itypl,npidl,iqql)                       
               else                                                     
                 ngbor(l)=0                                             
               end if                                                   
             else                                                       
               ngbrnr(l)=-ngbrnr(l)                                     
             end if                                                     
           end if                                                       
        end if                                                          
        if(l.eq.1) go to 310                                            
        if(ida(34).eq.1) go to 310                                      
        iok=0                                                           
        do 308 knr=ngbsum(l)+iabs(ngbrnr(l)),ngbsum(l)+1,-1             
           k=nghbor(knr)                                                
           if(k.ge.l) go to 308                                         
           if(prop(k)) then                                             
              iok=1                                                     
              go to 312                                                 
           end if                                                       
  308   continue                                                        
        do 309  k=l-1,1,-1                                              
          if(prop(k)) then                                              
              ngbrnr(k)=isign(1,ngbrnr(k))*  (iabs(ngbrnr(k))+1)        
              nghbor(ngbsum(k)+iabs(ngbrnr(k)))=l                       
              ngbrnr(l)=isign(1,ngbrnr(l))*  (iabs(ngbrnr(l))+1)        
              nghbor(ngbsum(l)+iabs(ngbrnr(l)))=k                       
              iok=1                                                     
              go to 312                                                 
          end if                                                        
          if(k.eq.1) iok=1                                              
  309   continue                                                        
  312   continue                                                        
        if(iok.ne.1) call errex('wrong iok-value in relham')            
  310 continue                                                          
      if(janr.ne.1) then                                                
        do 981 l=1,ntm1                                                 
           dcoef(l)=0.                                                  
  981   continue                                                        
        if(nttot+ntm1.gt.ntotma) then                                   
            call errex('too small dcoef array in relham')               
        end if                                                          
        if(ida(8).eq.3) then                                            
           do 982 l=ntm1+1,ntm1+ntotm1                                  
             dcoef(l)=chi(l-ntm1)                                       
  982      continue                                                     
        else                                                            
           do 983 l=ntm1+1,ntm1+ntotm1                                  
             dcoef(l)=0.                                                
  983      continue                                                     
        end if                                                          
      end if                                                            
      do 415 l=1,ntt                                                    
      do 414 k=1,ntt                                                    
      dhldzm(k,l)=0.                                                    
  414 continue                                                          
  415 continue                                                          
      do 413 l=1,nttot                                                  
      il=iofset+l                                                       
      identl=issn(il)                                                   
      do 411 k=1,nttot                                                  
      dldqsm(k,l)=0.                                                    
      dldpsm(k,l)=0.                                                    
      dldqpm(k,l)=0.                                                    
  411 continue                                                          
      do 410 i=0,3                                                      
      dchidq(i,l)=0.                                                    
      dchidp(i,l)=0.                                                    
      dhdq(i,l)=0.                                                      
  410 continue                                                          
      if(itdth(2,identl).ne.0.and.                                      
     a           tdtg(identl).gt.x(0,il)                                
     b            .and.qdot(0,il).gt.0.0) then                          
        versko=p(0,il)/qdot(0,il)                                       
        do 708 i=0,3                                                    
         dhdp(i,l)=2.*qdot(i,il)*versko                                 
  708   continue                                                        
      else                                                              
        do 709 i=0,3                                                    
         dhdp(i,l)=2.*p(i,il)                                           
  709   continue                                                        
      end if                                                            
      if((itripl.eq.1.and..not.switch(17))                              
     a                         .or.ida(34).eq.1.or.switch(6)) then      
        ncolil=nclcnt(identl)                                           
        if(ncolil.eq.0.and.identl.le.nt) then                           
           dhdp(1,l)=0.                                                 
           dhdp(2,l)=0.                                                 
           if(identl.le.n1old) then                                     
              dhdp(3,l)=2.*betapr*p(0,il)                               
           else                                                         
              dhdp(3,l)=2.*betata*p(0,il)                               
           end if                                                       
        end if                                                          
      end if                                                            
  413 continue                                                          
      if(janr.eq.1) go to 1808                                          
      do 113 l=1,nttot                                                  
      il=iofset+l                                                       
      do 111 knr=ngbsum(l)+1,ngbsum(l)+iabs(ngbrnr(l))                  
      k=nghbor(knr)                                                     
      if(k.ge.l) go to 111                                              
      ik=iofset+k                                                       
      if(.not.prop(k)) go to 111                                        
      do 99 i=0,3                                                       
      qlk(i)=x(i,il)-x(i,ik)                                            
      plk(i)=p(i,il)+p(i,ik)                                            
   99 continue                                                          
      dqlksq=dqsq(l*(l-3)/2+k+1)                                        
      splksq=spsq(l*(l-3)/2+k+1)                                        
      dqsplk=dqsp(l*(l-3)/2+k+1)                                        
      if(-dqlksq.lt.dstint ) then                                       
         if((dqlksq.lt.0.000001).and.(-dqlksq.lt.0.000001)) then        
           dqlksq=-0.000001                                             
         end if                                                         
         if(ida(14).eq.1) then                                          
            if(dqlksq.gt.1000.) dqlksq=1000.                            
            cofact=exp(dqlksq/al)*abs(al/dqlksq)                        
     a                            *em(il)**2*em(ik)**2/sqrt(splksq)     
            do 1060 i=0,3                                               
            dcdpkl=cofact*(qlk(i)-                                      
     a                  dqsplk/splksq*plk(i))                           
            dcdqkl=-(cofact*(plk(i)+dqsplk*                             
     a                  2.*qlk(i)/al*(1.-(al/dqlksq))))                 
            dchidq(i,l)=dchidq(i,l)-dcdqkl                              
            dchidp(i,l)=dchidp(i,l)+dcdpkl                              
            dchidq(i,k)=dchidq(i,k)-dcdqkl                              
            dchidp(i,k)=dchidp(i,k)-dcdpkl                              
 1060       continue                                                    
            dldqpm(k,l)=cofact                                          
            dldqpm(l,k)=cofact                                          
            cofact=cofact*dqsplk                                        
            dldqsm(k,l)=cofact /al*(1.-al/dqlksq)                       
            dldqsm(l,k)=-dldqsm(k,l)                                    
            dldpsm(k,l)=-0.5*cofact/splksq                              
            dldpsm(l,k)=-dldpsm(k,l)                                    
         else                                                           
            if(dqlksq.gt.1000.) dqlksq=1000.                            
            cofact=exp(dqlksq/al)*abs(al/dqlksq)                        
            do 106 i=0,3                                                
            dcdpkl=cofact*qlk(i)                                        
            dcdqkl=-(cofact*(plk(i)+dqsplk*                             
     a                  2.*qlk(i)/al*(1.-(al/dqlksq))))                 
            dchidq(i,l)=dchidq(i,l)-dcdqkl                              
            dchidp(i,l)=dchidp(i,l)+dcdpkl                              
            dchidq(i,k)=dchidq(i,k)-dcdqkl                              
            dchidp(i,k)=dchidp(i,k)-dcdpkl                              
  106       continue                                                    
            dldqpm(k,l)=cofact                                          
            dldqpm(l,k)=cofact                                          
            cofact=cofact*dqsplk                                        
            dldqsm(k,l)=cofact /al*(1.-al/dqlksq)                       
            dldqsm(l,k)=-dldqsm(k,l)                                    
            dldpsm(k,l)=0.                                              
            dldpsm(l,k)=0.                                              
         end if                                                         
      else                                                              
         if(ida(14).eq.1) then                                          
            glkm4=glk*em(ik)**2*em(il)**2/sqrt(splksq)                  
            do 2006 i=0,3                                               
            dcdpkl=glkm4*(qlk(i)-                                       
     a                  dqsplk/splksq*plk(i)      )                     
            dcdqkl=-glkm4*plk(i)                                        
            dchidq(i,l)=dchidq(i,l)-dcdqkl                              
            dchidp(i,l)=dchidp(i,l)+dcdpkl                              
            dchidq(i,k)=dchidq(i,k)-dcdqkl                              
            dchidp(i,k)=dchidp(i,k)-dcdpkl                              
 2006       continue                                                    
            dldqpm(k,l)=glkm4                                           
            dldqpm(l,k)=glkm4                                           
            dldqsm(k,l)=0.                                              
            dldqsm(l,k)=0.                                              
            glkm4=glkm4*dqsplk                                          
            dldpsm(k,l)=-0.5*glkm4/splksq                               
            dldpsm(l,k)=-dldpsm(k,l)                                    
         else                                                           
            do 1006 i=0,3                                               
            dcdpkl=glk*qlk(i)                                           
            dcdqkl=-glk*plk(i)                                          
            dchidq(i,l)=dchidq(i,l)-dcdqkl                              
            dchidp(i,l)=dchidp(i,l)+dcdpkl                              
            dchidq(i,k)=dchidq(i,k)-dcdqkl                              
            dchidp(i,k)=dchidp(i,k)-dcdpkl                              
 1006       continue                                                    
            dldqpm(k,l)=glk                                             
            dldqpm(l,k)=glk                                             
            dldqsm(k,l)=0.                                              
            dldqsm(l,k)=0.                                              
            dldpsm(k,l)=0.                                              
            dldpsm(l,k)=0.                                              
         end if                                                         
      end if                                                            
  111 continue                                                          
  113 continue                                                          
      if(itrig.eq.0) then                                               
         do 40 l=1,ntotm1                                               
         il=iofset+l                                                    
         do 36 mnr=ngbsum(l)+1,ngbsum(l)+iabs(ngbrnr(l))                
         m=nghbor(mnr)                                                  
         im=iofset+m                                                    
         do 38 knr=ngbsum(m)+1,ngbsum(m)+iabs(ngbrnr(m))                
         kk=nghbor(knr)                                                 
         if(kk.ge.l) go to 38                                           
         k=kk+ntm1                                                      
         ikk=iofset+kk                                                  
         if(.not.prop(kk))go to 38                                      
         wert=0.                                                        
         signum=1.                                                      
         do 435 i=0,3                                                   
         if(kk.eq.m) then                                               
           dphidp(i,1)=dchidp(i,kk)                                     
           dphidq(i,2)=dchidq(i,kk)                                     
         else                                                           
           qlk(i)=x(i,ikk)-x(i,im)                                      
           plk(i)=p(i,ikk)+p(i,im)                                      
           dphidp(i,1)=2.*dldpsm(m,kk)*plk(i)+dldqpm(m,kk)*qlk(i)       
           dphidq(i,2)=-(2.*dldqsm(m,kk)*qlk(i)+dldqpm(m,kk)*plk(i))    
         end if                                                         
         if(l.eq.m) then                                                
           dphidq(i,1)=dchidq(i,l )                                     
           dphidp(i,2)=dchidp(i,l )                                     
         else                                                           
           qlk(i)=x(i,il )-x(i,im)                                      
           plk(i)=p(i,il )+p(i,im)                                      
           dphidq(i,1)=-(2.*dldqsm(m,l )*qlk(i)+dldqpm(m,l )*plk(i))    
           dphidp(i,2)=2.*dldpsm(m,l )*plk(i)+dldqpm(m,l )*qlk(i)       
         end if                                                         
         wert=wert+signum*(                                             
     a         dphidp(i,1)*dphidq(i,1)-                                 
     b         dphidq(i,2)*dphidp(i,2))                                 
         signum=-1.                                                     
  435   continue                                                        
        if(l.le.ntm1) at(k,l)=at(k,l)+wert                              
        if(kk.le.ntm1) at(l+ntm1,kk)=at(l+ntm1,kk)-wert                 
   38   continue                                                        
   36 continue                                                          
   40 continue                                                          
      end if                                                            
 1808 continue                                                          
      do 230 l=1,ntt                                                    
      il=iofset+l                                                       
      identl=issn(il)                                                   
      denspp(identl)=0.                                                 
      do 229 knr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)                        
      k=nghbor(knr)                                                     
      ik=iofset+k                                                       
      if(ngbrnr(ik).le.0) go to 229                                     
      if(k.lt.l) then                                                   
         dqlksq=dqsq(l*(l-3)/2+k+1)                                     
         splksq=spsq(l*(l-3)/2+k+1)                                     
         dqsplk=dqsp(l*(l-3)/2+k+1)                                     
         pr2=(.25*(splksq-(em(ik)*em(ik)+em(il)*em(il)))**2-            
     a           em(ik)*em(ik)*em(il)*em(il))/splksq                    
         zlk=dqlksq-dqsplk*dqsplk/splksq                                
         if(-zlk.lt.dstint) then                                        
           cofact=exp(zlk/alpot)                                        
           identk=issn(ik)                                              
           denspp(identk)=denspp(identk)+cofact                         
           denspp(identl)=denspp(identl)+cofact                         
           ident1=issn(il)                                              
           ident2=issn(ik)                                              
           ncoli1=nclcnt(ident1)                                        
           ncoli2=nclcnt(ident2)                                        
           if(switch(8).and.                                            
     a           ncoli1.eq.0.and.ncoli2.eq.0.and.                       
     b            ((ident1.le.n1old.and.ident2.le.n1old).or.            
     c             (ident1.gt.n1old.and.ident1.le.n1n2s.and.            
     d                 ident2.gt.n1old.and.ident2.le.n1n2s)))then       
           else                                                         
             dhldzm(k,l)=-emnuc*cofact                                  
             dhldzm(l,k)=-emnuc*cofact                                  
           end if                                                       
         end if                                                         
      end if                                                            
  229 continue                                                          
  230 continue                                                          
      do 240 l=1,ntt                                                    
       il=iofset+l                                                      
       identl=issn(il)                                                  
       do 439 mnr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)                       
       m=nghbor(mnr)                                                    
       im=iofset+m                                                      
       if(ngbrnr(im).le.0) go to 439                                    
       if(m.lt.l) then                                                  
          dqlksq=dqsq(l*(l-3)/2+m+1)                                    
          splksq=spsq(l*(l-3)/2+m+1)                                    
          dqsplk=dqsp(l*(l-3)/2+m+1)                                    
          pr2=(.25*(splksq-(em(im)*em(im)+em(il)*em(il)))**2-           
     a           em(im)*em(im)*em(il)*em(il))/splksq                    
          zlk=dqlksq-dqsplk*dqsplk/splksq                               
          if(-zlk.lt.dstint) then                                       
                do 419 i=0,3                                            
                  qlk(i)=x(i,il)-x(i,im)                                
                  plk(i)=p(i,il)+p(i,im)                                
  419           continue                                                
                do 417 i=0,3                                            
                  qtlk(i)=qlk(i)-dqsplk/splksq*plk(i)                   
  417           continue                                                
              eos1=eosc1                                                
              eos2=eosc2                                                
              kpot=ngbor(l)+ngbor(m)                                    
              pilsq=p(0,il)*p(0,il)                                     
              pimsq=p(0,im)*p(0,im)                                     
              do  427 i=1,3                                             
               pilsq=pilsq-p(i,il)*p(i,il)                              
               pimsq=pimsq-p(i,im)*p(i,im)                              
  427         continue                                                  
              emil=sqrt(max(0.0,pilsq))                                 
              emim=sqrt(max(0.0,pimsq))                                 
              srtlm=sqrt(splksq)                                        
              if(srtlm.gt.emil+emim) then                               
               prlm=pcmsr(srtlm,emil,emim)                              
              else                                                      
                prlm=0.0                                                
              end if                                                    
              valm=prlm                                                 
              if((ida(12).eq.1.and.valm.gt.pr2max).or.                  
     a                                    ida(12).eq.2) then            
                if(pr2.gt.pr2max) eos1=eosc3                            
                if(kpot.eq.2.or.kpot.eq.6.or.                           
     a                                     kpot.eq.3) then              
                      eos1=eos1*da(13)                                  
                      eos2=eos2*da(16)                                  
                else if(kpot.eq.1) then                                 
                      eos1=eos1*da(14)                                  
                end if                                                  
              end if                                                    
                identm=issn(im)                                         
                dhldzm(m,l)=0.5*dhldzm(m,l)*                            
     a                      (eos1+eos2*(denspp(identl)/salpot)**gm1)    
                dhldzm(l,m)=0.5*dhldzm(l,m)*                            
     a                      (eos1+eos2*(denspp(identm)/salpot)**gm1)    
                do 438 i=0,3                                            
                  dhldqm(i)=-2.*dhldzm(m,l)*qtlk(i)                     
                  dhdq(i,l)=dhdq(i,l)-dhldqm(i)                         
                  dhmdql(i)=2.*dhldzm(l,m)*qtlk(i)                      
                  dhdq(i,m)=dhdq(i,m)-dhmdql(i)                         
                  dhldpm(i)=-2.*dhldzm(m,l)*dqsplk/splksq*qtlk(i)       
                  dhdp(i,l)=dhdp(i,l)+dhldpm(i)                         
                  dhmdpl(i)=-2.*dhldzm(l,m)*dqsplk/splksq*qtlk(i)       
                  dhdp(i,m)=dhdp(i,m)+dhmdpl(i)                         
  438           continue                                                
                if(icaltp.ne.0) then                                    
                  if(force(0,l).eq.0.0.and.force(0,m).eq.0.0) then      
                    do 432 i=1,3                                        
                     force(i,l)=force(i,l)-dhldqm(i)                    
                     force(i,m)=force(i,m)-dhmdql(i)                    
  432               continue                                            
                  end if                                                
                end if                                                  
                if(janr.ne.1) then                                      
                 do 478 knr=ngbsum(m)+1,ngbsum(m)+ngbrnr(m)             
                   k=nghbor(knr)                                        
                   if(k.eq.nttot) go to 478                             
                   ik=iofset+k                                          
                   if(.not.prop(k)) go to 478                           
                   signum=1.                                            
                   wert=0.                                              
                   do 476 i=0,3                                         
                   if(k.eq.m) then                                      
                    dphidp(i,1)=dchidp(i,k)                             
                    dphidq(i,2)=dchidq(i,k)                             
                   else                                                 
                    qlk(i)=x(i,ik)-x(i,im)                              
                    plk(i)=p(i,ik)+p(i,im)                              
                    dphidp(i,1)=2.*dldpsm(m,k)*plk(i)                   
     a                                   +dldqpm(m,k)*qlk(i)            
                    dphidq(i,2)=-(2.*dldqsm(m,k)*qlk(i)                 
     a                                    +dldqpm(m,k)*plk(i))          
                   end if                                               
                   wert=wert+signum*(                                   
     a                         dphidp(i,1)*dhldqm(i)-                   
     b                               dphidq(i,2)*dhldpm(i))             
                   signum=-1.                                           
  476              continue                                             
                   if(k.le.ntm1.and.l.le.ntm1)                          
     a                           at(l,k)=at(l,k)-wert                   
                   at(k+ntm1,l+ntm1)=at(k+ntm1,l+ntm1)+wert             
  478            continue                                               
                 do 578 knr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)             
                   k=nghbor(knr)                                        
                   if(k.eq.nttot) go to 578                             
                   ik=iofset+k                                          
                   if(.not.prop(k)) go to 578                           
                   signum=1.                                            
                   wert=0.                                              
                   do 576 i=0,3                                         
                   if(k.eq.l) then                                      
                    dphidp(i,1)=dchidp(i,k)                             
                    dphidq(i,2)=dchidq(i,k)                             
                   else                                                 
                    qlk(i)=x(i,ik)-x(i,il)                              
                    plk(i)=p(i,ik)+p(i,il)                              
                    dphidp(i,1)=2.*dldpsm(l,k)*plk(i)                   
     a                                   +dldqpm(l,k)*qlk(i)            
                    dphidq(i,2)=-(2.*dldqsm(l,k)*qlk(i)                 
     a                                    +dldqpm(l,k)*plk(i))          
                   end if                                               
                   wert=wert+signum*(                                   
     a                         dphidp(i,1)*dhmdql(i)-                   
     b                               dphidq(i,2)*dhmdpl(i))             
                   signum=-1.                                           
  576              continue                                             
                   if(k.le.ntm1.and.m.le.ntm1)                          
     a                           at(m,k)=at(m,k)-wert                   
                   at(k+ntm1,m+ntm1)=at(k+ntm1,m+ntm1)+wert             
  578            continue                                               
                end if                                                  
          end if                                                        
       end if                                                           
  439 continue                                                          
  240 continue                                                          
      if(janr.eq.1) go to 1809                                          
         do 2605 l=1,nttot                                              
            il=iofset+l                                                 
            do 2618 knr=ngbsum(l)+1,ngbsum(l)+iabs(ngbrnr(l))           
                k=nghbor(knr)                                           
                if(k.eq.nttot) go to 2618                               
                ik=iofset+k                                             
                if(.not.prop(k)) go to 2618                             
                signum=1.                                               
                wert=0.                                                 
                do 2616 i=0,3                                           
                  if(k.eq.l) then                                       
                    dphidp(i,1)=dchidp(i,k)                             
                    dphidq(i,2)=dchidq(i,k)                             
                  else                                                  
                    qlk(i)=x(i,ik)-x(i,il)                              
                    plk(i)=p(i,ik)+p(i,il)                              
                    dphidp(i,1)=2.*dldpsm(l,k)*plk(i)                   
     a                                   +dldqpm(l,k)*qlk(i)            
                    dphidq(i,2)=-(2.*dldqsm(l,k)*qlk(i)                 
     a                                    +dldqpm(l,k)*plk(i))          
                   end if                                               
                   wert=wert+signum*(                                   
     a                         dphidp(i,1)*dhdq(i,l)-                   
     b                               dphidq(i,2)*dhdp(i,l))             
                   signum=-1.                                           
 2616              continue                                             
                   if(k.le.ntm1.and.l.le.ntm1)                          
     a                           at(l,k)=at(l,k)-wert                   
                   at(k+ntm1,l+ntm1)=at(k+ntm1,l+ntm1) +wert            
 2618        continue                                                   
 2605    continue                                                       
      if(itrig.eq.0) then                                               
         do 30 l=1,min0(ntotm1,ntt)                                     
         il=iofset+l                                                    
         do 26 mnr=ngbsum(l)+1,ngbsum(l)+ngbrnr(l)                      
          m=nghbor(mnr)                                                 
          im=iofset+m                                                   
          do 28 knr=ngbsum(m)+1,ngbsum(m)+ngbrnr(m)                     
            k=nghbor(knr)                                               
            ik=iofset+k                                                 
            if(ngbrnr(k).le.0) go to 28                                 
            if((k.ge.l).and.(k.ne.nttot)) go to 28                      
            wert=0.                                                     
            signum=1.                                                   
            if(l.gt.m) then                                             
              splksq=spsq(l*(l-3)/2+m+1)                                
              dqsplk=dqsp(l*(l-3)/2+m+1)                                
            else if(l.lt.m) then                                        
              splksq=spsq(m*(m-3)/2+l+1)                                
              dqsplk=-dqsp(m*(m-3)/2+l+1)                               
            end if                                                      
            if(k.gt.m) then                                             
              spkmsq=spsq(k*(k-3)/2+m+1)                                
              dqspkm=dqsp(k*(k-3)/2+m+1)                                
            else if(k.lt.m) then                                        
              spkmsq=spsq(m*(m-3)/2+k+1)                                
              dqspkm=-dqsp(m*(m-3)/2+k+1)                               
            end if                                                      
            do 425 i=0,3                                                
             if(l.eq.m) then                                            
                  dphidp(i,1)=dhdp(i,l)                                 
                  dphidq(i,2)=dhdq(i,l)                                 
             else                                                       
                  qlk(i)=x(i,il)-x(i,im)                                
                  plk(i)=p(i,il)+p(i,im)                                
                  qtlk(i)=qlk(i)-dqsplk/splksq*plk(i)                   
                  dphidp(i,1)=-2.*dhldzm(m,l)*dqsplk/splksq*qtlk(i)     
                  dphidq(i,2)=-2.*dhldzm(m,l)*qtlk(i)                   
             end if                                                     
             if(k.eq.m) then                                            
                  dphidq(i,1)=dhdq(i,k)                                 
                  dphidp(i,2)=dhdp(i,k)                                 
              else                                                      
                  qlk(i)=x(i,ik)-x(i,im)                                
                  plk(i)=p(i,ik)+p(i,im)                                
                  qtlk(i)=qlk(i)-dqspkm/spkmsq*plk(i)                   
                  dphidq(i,1)=-2.*dhldzm(m,k)*qtlk(i)                   
                  dphidp(i,2)=-2.*dhldzm(m,k)*dqspkm/spkmsq*qtlk(i)     
              end if                                                    
              wert=wert+signum*(                                        
     a                    dphidp(i,1)*dphidq(i,1)-                      
     b                                   dphidq(i,2)*dphidp(i,2))       
              signum=-1.                                                
  425       continue                                                    
            if(l.le.ntm1) at(l,k+ntm1)=at(l,k+ntm1)+wert                
            if(k.le.ntm1)                                               
     a            at(k,l+ntm1)=at(k,l+ntm1)-wert                        
   28      continue                                                     
   26  continue                                                         
   30 continue                                                          
      end if                                                            
      do 32 l=1,nttot                                                   
         if(.not.prop(l)) then                                          
            if(l.le.ntm1) at(l,l)=1.                                    
            if(l.le.ntotm1) at(ntm1+l,ntm1+l)=1.                        
         end if                                                         
   32 continue                                                          
      lend=ntm2m2+1                                                     
      j1end=ntm2m1+1                                                    
      if(itrig.eq.0) then                                               
         lstart=1                                                       
         j2end=ntm2m2+1                                                 
      else                                                              
         lstart=ntm1+1                                                  
         j2end=ntotm1+1                                                 
      end if                                                            
      iok=0                                                             
      do 520 j=nttot,1,-1                                               
        j1end=j1end-1                                                   
        j2end=j2end-1                                                   
         lend=lend-1                                                    
         bcoef(2*iofset+j1end)=0.                                       
        if(prop(j))then                                                 
           iok=1                                                        
           go to 521                                                    
        end if                                                          
  520 continue                                                          
  521 continue                                                          
      if(iok.eq.0) then                                                 
          write(8,*)'irun,itripl,kdt,kclst,ntt,nrpion'                  
          write(8,*) irun,itripl,kdt,kclst,ntt,nrpion                   
          do 551 l=1,nttot                                              
            il=iofset+l                                                 
            write(8,*) l,'prop=',prop(l),'ichbig=',ichbig(il)           
            do 550 k=1,l-1                                              
              dqlksq=dqsq(l*(l-3)/2+k+1)                                
              write(8,*) l,k,'dqlksq=',dqlksq                           
  550       continue                                                    
  551 continue                                                          
          call errex('only not.prop particles in relham?????')          
       end if                                                           
         do  510 l=lstart,lend                                          
           dcoef(l)=dcoef(l)/at(l,l)                                    
           do  509 j=l+1,j1end                                          
             dcoef(j)=dcoef(j)-at(j,l)*dcoef(l)                         
             if(at(l,j).ne.0.) then                                     
                at(l,j)=-at(l,j)/at(l,l)                                
                do 508 k=l+1,lend                                       
                   at(k,j)=at(k,j)+at(l,j)*at(k,l)                      
  508           continue                                                
             end if                                                     
  509     continue                                                      
  510   continue                                                        
        bcoef(2*iofset+j1end)=1.                                        
        do 514 j=1,j2end                                                
           jj=j1end-j                                                   
           bcoef(2*iofset+jj)=dcoef(jj)                                 
           do 513 k=jj+1,j1end                                          
              bcoef(2*iofset+jj)=bcoef(2*iofset+jj)+                    
     a                      at(jj,k)*bcoef(2*iofset+k)                  
  513      continue                                                     
           bcf=bcoef(2*iofset+jj)                                       
           bcoef(2*iofset+jj)=max(cvalu0,bcf)                           
  514   continue                                                        
      bprmax=0.                                                         
      do 506 j=1,ntm2m1                                                 
        bcf=bcoef(2*iofset+j)                                           
        bprmax=max(bcf,bprmax)                                          
  506 continue                                                          
      if(bprmax.eq.0.) call errex('bprmax=0. in relham')                
      do 507 j=1,ntm2m1                                                 
        bcoef(2*iofset+j)=bcoef(2*iofset+j)/bprmax                      
  507 continue                                                          
      dslsum=0.                                                         
      dslmax=0.                                                         
      nprop=0                                                           
      do 518 l=1,nttot                                                  
        il=iofset+l                                                     
        dsl=p(0,il)*pnorm(0)                                            
        do 519 i=1,3                                                    
  519   dsl=dsl-p(i,il)*pnorm(i)                                        
        dsl=2.*bcoef(2*iofset+ntm1+l)*dsl                               
            if(dsl.ne.0.) nprop=nprop+1                                 
            dslsum=dslsum+dsl                                           
        dslmax=max(dslmax,dsl)                                          
  518 continue                                                          
      if(dslmax.le.0.) call errex('no prop. in relham')                 
      divide=1. / dslmax                                                
      if(nprop.gt.0) then                                               
            if(switch(17)) then                                         
               dslsum=float(nprop)/dslsum                               
            else                                                        
               dslsum=float(nprop)/dslsum                               
            endif                                                       
            divide=dslsum                                               
            if(dslmax*dtau*divide.gt.dq0max) then                       
                 divide=dq0max/(dslmax*dtau)                            
            end if                                                      
      end if                                                            
      do 505 j=1,ntm2m1                                                 
        bcoef(2*iofset+j)=bcoef(2*iofset+j)*divide                      
  505 continue                                                          
      do 503 il=iofset+1,iofset+nttot                                   
        dsl=p(0,il)*pnorm(0)                                            
        do 502 i=1,3                                                    
  502   dsl=dsl-p(i,il)*pnorm(i)                                        
        dsl=0.6667*bcoef(iofset+ntm1+il)*dsl                            
        if(dsl.gt.1.) then                                              
             bcoef(iofset+ntm1+il)=bcoef(iofset+ntm1+il)/dsl            
        end if                                                          
  503 continue                                                          
 1809 continue                                                          
      if(janr.eq.1.and.ida(23).eq.0) then                               
         do 1810 j=1,ntm2m1                                             
          if(j.le.ntm1) then                                            
             bcoef(2*iofset+j)=0.                                       
          else                                                          
             ji=iofset+j-ntm1                                           
             if(.not.prop(ji-iofset)) then                              
                bcoef(2*iofset+j)=0.                                    
             else                                                       
                bcoef(2*iofset+j)=0.5/em(ji)                            
             endif                                                      
          end if                                                        
 1810     continue                                                      
      else if(ida(23).ne.0) then                                        
         teprop=tbegin+kdt*dtau                                         
         do 2853 l=1,nttot                                              
          il=iofset+l                                                   
          tilobs=x(0,il)*pnorm(0)-x(1,il)*pnorm(1)-                     
     a                          x(2,il)*pnorm(2)-x(3,il)*pnorm(3)       
          identl=issn(il)                                               
          ncolil=nclcnt(identl)                                         
          if((itripl.eq.1.and..not.switch(17))                          
     a                         .or.ida(34).gt.0.or.switch(6)) then      
            if(ncolil.eq.0.and.identl.le.nt.and.                        
     a                   (ida(34).ne.2.or.identl.gt.n1old)) then        
              if(identl.le.n1old) then                                  
                pilobs=p(0,il)*pnorm(0)-p(0,il)*betapr*pnorm(3)         
              else                                                      
                pilobs=p(0,il)*pnorm(0)-p(0,il)*betata*pnorm(3)         
              end if                                                    
            else                                                        
              pilobs=p(0,il)*pnorm(0)-p(1,il)*pnorm(1)-                 
     a                          p(2,il)*pnorm(2)-p(3,il)*pnorm(3)       
            end if                                                      
          else                                                          
            pilobs=p(0,il)*pnorm(0)-p(1,il)*pnorm(1)-                   
     a                          p(2,il)*pnorm(2)-p(3,il)*pnorm(3)       
          end if                                                        
          if(ichbig(il).eq.3) then                                      
             bcoef(2*iofset+ntm1+l)=0.                                  
             prop(l)=.false.                                            
          else if(tilobs.gt.teprop) then                                
             ichbig(il)=1                                               
             bcoef(2*iofset+ntm1+l)=0.                                  
             prop(l)=.false.                                            
          else                                                          
             prop(l)=.true.                                             
             ichbig(il)=0                                               
             bcoef(2*iofset+ntm1+l)=0.5*(teprop-tilobs)/dtau/pilobs     
          end if                                                        
 2853   continue                                                        
        do 2852 j=1,ntm1                                                
           bcoef(2*iofset+j)=0.                                         
 2852   continue                                                        
      end if                                                            
      do 943 l=1,nttot                                                  
       il=iofset+l                                                      
       do 942 i=1,4                                                     
         at(i,l)=0.                                                     
         qdot(i-1,il)=0.                                                
  942  continue                                                         
  943 continue                                                          
      do 944 l=1,nttot                                                  
         if(.not.prop(l)) then                                          
             if(bcoef(2*iofset+ntm1+l).ne.0.) then                      
                  write(8,*)'bcoef=',bcoef(2*iofset+ntm1+l)             
                  call errex('bcoef.ne.0. in relham')                   
             end if                                                     
         end if                                                         
  944 continue                                                          
       if(icaltp.ne.0) then                                             
        do 1501 l=1,nttot                                               
         if(force(0,l).lt.0.0) go to 1501                               
         if(ngbrnr(l).le.0) then                                        
           force(0,l)=-1.0                                              
           go to 1501                                                   
         end if                                                         
         il=iofset+l                                                    
         if(x(0,il).lt.timpra) then                                     
           bchl=bcoef(2*iofset+ntm1+l)                                  
           do 1500 i=1,3                                                
 1500      force(i,l)=bchl*force(i,l)                                   
           force(0,l)=1.0                                               
           icaltp=2                                                     
         end if                                                         
 1501   continue                                                        
       end if                                                           
      do 1525 l=1,nttot                                                 
       il=iofset+l                                                      
       lalert=0                                                         
       if(icaltp.eq.0) then                                             
       else if(force(0,l).lt.0.0) then                                  
       else                                                             
         lalert=1                                                       
       end if                                                           
       if(lalert.eq.1) jcaltp=1                                         
       do 1524 mnr=ngbsum(l)+1,ngbsum(l)+iabs(ngbrnr(l))                
        m=nghbor(mnr)                                                   
        im=iofset+m                                                     
       malert=0                                                         
       if(lalert.eq.1) then                                             
         if(m.eq.l) then                                                
         else if(force(0,m).lt.0.0) then                                
         else if(force(0,m).gt.0.0) then                                
           malert=1                                                     
         end if                                                         
       end if                                                           
        if(l.eq.m) then                                                 
           do 1521 i=0,3                                                
            dhldqm(i)=dhdq(i,l)                                         
            dhldpm(i)=dhdp(i,l)                                         
            if(ntm1.gt.0) then                                          
              dcldqm(i)=dchidq(i,l)                                     
              dcldpm(i)=dchidp(i,l)                                     
            end if                                                      
 1521      continue                                                     
        else                                                            
            if((ntm1.eq.0).and.                                         
     a         (ngbrnr(l).le.0.or.ngbrnr(m).le.0))                      
     b                                            go to 1524            
            if(l.gt.m) then                                             
              splksq=spsq(l*(l-3)/2+m+1)                                
              dqsplk=dqsp(l*(l-3)/2+m+1)                                
            else                                                        
              splksq=spsq(m*(m-3)/2+l+1)                                
              dqsplk=-dqsp(m*(m-3)/2+l+1)                               
            end if                                                      
            do 1522 i=0,3                                               
                qlk(i)=x(i,il)-x(i,im)                                  
                plk(i)=p(i,il)+p(i,im)                                  
                if(ntm1.gt.0) then                                      
                   dcldqm(i)=                                           
     a               -(2.*dldqsm(m,l )*qlk(i)+dldqpm(m,l )*plk(i))      
                   dcldpm(i)=                                           
     a                2.*dldpsm(m,l )*plk(i)+dldqpm(m,l )*qlk(i)        
                end if                                                  
 1522       continue                                                    
            if(ngbrnr(l).gt.0.and.ngbrnr(m).gt.0) then                  
              do 1519 i=0,3                                             
                   qtlk(i)=qlk(i)-dqsplk/splksq*plk(i)                  
                   dhldqm(i)=-2.*dhldzm(m,l)*qtlk(i)                    
                   dhldpm(i)=-2.*dhldzm(m,l)*dqsplk/splksq*qtlk(i)      
 1519         continue                                                  
            else                                                        
              do 1528 i=0,3                                             
                   dhldqm(i)=0.                                         
                   dhldpm(i)=0.                                         
 1528         continue                                                  
            end if                                                      
        end if                                                          
        do 1523 i=0,3                                                   
         delt=bcoef(2*iofset+ntm1+l)*dhldqm(i)                          
         if(l.le.ntm1) delt=delt+bcoef(2*iofset+l)*dcldqm(i)            
         at(1+i,m)=at(1+i,m)+delt                                       
         v4dum(i)=delt                                                  
         delt=bcoef(2*iofset+ntm1+l)*dhldpm(i)                          
         if(l.le.ntm1) delt=delt+bcoef(2*iofset+l)*dcldpm(i)            
         qdot(i,im)=qdot(i,im)+delt                                     
 1523   continue                                                        
        if(malert.eq.1) then                                            
          do 1505  i=1,3                                                
 1505     force(i,m)=force(i,m)+v4dum(i)                                
        end if                                                          
 1524  continue                                                         
 1525 continue                                                          
      do 1535 l=1,nttot                                                 
       il=iofset+l                                                      
       if(qdot(0,il).lt.0.) then                                        
         do 1537 i=0,3                                                  
 1537     qdot(i,il)=0.                                                 
       end if                                                           
       do 1533 i=0,3                                                    
        p(i,il)=p(i,il)-at(1+i,l)*dtau                                  
        x(i,il)=x(i,il)+qdot(i,il)*dtau                                 
 1533  continue                                                         
        if(ida(34).eq.2) then                                           
          do 1534 i=0,3                                                 
           psyst(i)=psyst(i)-at(1+i,l)*dtau                             
 1534     continue                                                      
        end if                                                          
 1535 continue                                                          
      if(ida(28).eq.0.and.ida(34).eq.0) then                            
        lanf=0                                                          
        lend=ntt+1                                                      
        lchang=0                                                        
        do 1527 ldum=1,ntt                                              
         if(lchang.eq.0) then                                           
           lchang=1                                                     
           lanf=lanf+1                                                  
           l=lanf                                                       
         else                                                           
           lchang=0                                                     
           lend=lend-1                                                  
           l=lend                                                       
         end if                                                         
         il=iofset+l                                                    
         if(ida(7).eq.1) then                                           
           identl=issn(il)                                              
           if(itdth(2,identl).ne.0.and.                                 
     a                       tdtg(identl).le.x(0,il))then               
             tdt1i=tdtg(identl)                                         
             call ofshel(7,il,identl)                                   
             call dupdat(il,tdt1i)                                      
           end if                                                       
         end if                                                         
         if(.not.potyes(il,iqql)) go to 1527                            
         if(iqql.eq.1)  go to 1527                                      
         vil=vquasi(il,densa,densb,densc)                               
         eml=em(il)                                                     
         emeff2=eml**2+vil                                              
         pilsqo=p(0,il)**2                                              
         pp=0.0                                                         
         do 398 i=1,3                                                   
  398    pp=pp+p(i,il)**2                                               
         pilsqo=pilsqo-pp                                               
         dp0=p(0,il)-sqrt(emeff2+pp)                                    
         if(abs(pilsqo-emeff2).gt.0.4) then                             
           itdo=1                                                       
         else if(abs(vil).lt.0.005.and.                                 
     a      abs(pilsqo-emeff2).gt.0.004) then                           
           itdo=1                                                       
         else                                                           
           itdo=0                                                       
         end if                                                         
         ps005=0.005*psyst(0)                                           
         itypl=ityp(il)                                                 
         npidl=npidec(il)                                               
         emnew=pilsqo-vil                                               
         if(emnew.lt.0.0) then                                          
            emnew=-1.0                                                  
         else                                                           
           emnew=sqrt(emnew)                                            
         end if                                                         
         newm=newmas(itypl,npidl,iqql,emnew)                            
         if(newm.eq.1) then                                             
           em(il)=emnew                                                 
           go to 1527                                                   
         end if                                                         
         if(pcorr(3).eq.0.0) itdo=1                                     
         ptsq=pp-p(3,il)**2                                             
         p3nabs=p(0,il)**2-emeff2-ptsq                                  
         if(p3nabs.lt.0.0) then                                         
           dp3=0.0                                                      
           itdo=2                                                       
         else                                                           
           p3nabs=sqrt(p3nabs)                                          
           dp3=p(3,il)-sign(1.0,p(3,il))*p3nabs                         
         end if                                                         
         if(itdo.eq.1) then                                             
           if(nclcnt(il).eq.0.and.issn(il).le.n1n2s) then               
             dp3=0.0                                                    
             itdo=2                                                     
           end if                                                       
         end if                                                         
         if(abs(pcorr(0)+dp0).gt.2.*ps005) then                         
           if(pcorr(3).eq.0.0.and.itdo.ne.2) itdo=1                     
           if(itdo.eq.1.or.pcorr(3)*dp3.lt.0.0) then                    
           pcorr(3)=pcorr(3)+dp3                                        
           p(3,il)=p(3,il)-dp3                                          
           go to 1527                                                   
         end if                                                         
         end if                                                         
         if(abs(pcorr(0)+dp0).lt.ps005.or.                              
     b                        pcorr(0)*dp0.lt.0.0                       
     c                        .or.itdo.ne.0                             
     d                                              ) then              
           pcorr(0)=pcorr(0)+dp0                                        
           if(itdo.ne.0.and.abs(pcorr(0)).gt.20.*ps005) then            
             write(8,*) 'relham:problem with pcorr'                     
             write(8,*) 'relham:',irun,itripl,kdt,kclst,ntt,nttot       
             write(8,*) pilsqo,emeff2,vil,dp0                           
             write(8,*) issn(il),'particle with wrong.inv.mass'         
             write(8,*) 'p=',(p(i,il),i=0,3)                            
             write(8,*) 'pcorr=',(pcorr(i),i=0,3)                       
             call errex('abort in relham:problem with pcorr')           
           end if                                                       
           p(0,il)=p(0,il)-dp0                                          
         end if                                                         
 1527   continue                                                        
        if(lend-lanf.ne.1) call errex('relham abort:lend/lanf')         
      end if                                                            
      if(icaltp.ne.0) then                                              
        if(kcaltp.eq.0) then                                            
          jokall=1                                                      
        else if(jcaltp.eq.0) then                                       
        else if(icaltp.eq.1) then                                       
          jokall=1                                                      
        else                                                            
          volfac=2.*pi*(frcrr1*rr1)**3/gamnn                            
          do 1547 l=1,nttot                                             
           il=iofset+l                                                  
           if(force(0,l).gt.0.0) then                                   
             if(x(0,il).gt.timpra) then                                 
               do 1546 i=1,3                                            
 1546          v4dum(i)=-force(i,l)/qdot(0,il)*x(i,il)/volfac           
               tp11=tp11+v4dum(1)                                       
               tp22=tp22+v4dum(2)                                       
               tp33=tp33+v4dum(3)                                       
               hdcent=hdcent+1.0/volfac                                 
             end if                                                     
           end if                                                       
 1547     continue                                                      
        end if                                                          
      end if                                                            
      if(kdt.gt.ndt) then                                               
        betafb=0.0                                                      
        call chkmsc(betafb)                                             
      end if                                                            
      do 293 l=1,nttot                                                  
           ngbrnr(l)=iabs(ngbrnr(l))                                    
             if(ngbrnr(l).eq.0) then                                    
                ngbrnr(l)=nghbor(ngbsum(l)+1)/10000                     
                nghbor(ngbsum(l)+1)=mod(nghbor(ngbsum(l)+1),10000)      
             end if                                                     
  293 continue                                                          
      if(ida(33).eq.0.and.kdt.gt.10.and.kdt.ne.idtcl-1) then            
         l=0                                                            
  601   continue                                                        
           l=l+1                                                        
           if(l.gt.nrbar(kclst)+nrmes(kclst)) go to 611                 
           if(.not.prop(l)) go to 610                                   
           if((iabs(ngbrnr(l)).gt.5.and.l.le.nrbar(kclst)).or.          
     a                            iabs(ngbrnr(l)).gt.7) go to 610       
           il=iofset+l                                                  
           if(decay(il).lt. 1.e+10) go to 610                           
           identl=issn(il)                                              
           if(itdth(2,identl).ne.0) go to 610                           
           do 609 knr=ngbsum(l)+1,ngbsum(l)+iabs(ngbrnr(l))             
            k=nghbor(knr)                                               
            if(k.eq.l) go to 609                                        
            ik=iofset+k                                                 
            if(.not.prop(k)) go to 610                                  
            do  605 i=0,3                                               
            qlk(i)=x(i,il)-x(i,ik)                                      
            plk(i)=p(i,il)+p(i,ik)                                      
  605       continue                                                    
            dqlksq=qlk(0)*qlk(0)                                        
            splksq=plk(0)*plk(0)                                        
            dqsplk=qlk(0)*plk(0)                                        
            do  606 i=1,3                                               
            dqlksq=dqlksq-qlk(i)*qlk(i)                                 
            splksq=splksq-plk(i)*plk(i)                                 
            dqsplk=dqsplk-qlk(i)*plk(i)                                 
  606       continue                                                    
            do 826 i=0,3                                                
             qtlk(i)=qlk(i)-dqsplk/splksq*plk(i)                        
  826       continue                                                    
            xtxdot=qtlk(0)*(p(0,il)-p(0,ik))                            
            do 827 i=1,3                                                
             xtxdot=xtxdot-qtlk(i)*(p(i,il)-p(i,ik))                    
  827       continue                                                    
            if(xtxdot.gt.0.)then                                        
               go to 610                                                
            end if                                                      
            zlk=dqlksq-dqsplk*dqsplk/splksq                             
            if(-zlk.lt.dstint) then                                     
               go to 610                                                
            end if                                                      
  609      continue                                                     
            if(ida(28).eq.0.and.ida(34).eq.0.and.l.le.nrbar(kclst))then 
               do 701 ik=iofset+1,iofset+nrbar(kclst)                   
                 if(ik.eq.il) go to 701                                 
                 identk=issn(ik)                                        
                 if(itdth(2,identk).ne.0) go to 701                     
                 if(tdtg(identk).ne.0.) go to 701                       
                imscst=0                                                
                call mscstr(imscst,il,ik,isccss)                        
                 if(isccss.gt.0) go to 702                              
  701          continue                                                 
  702          continue                                                 
            end if                                                      
            io=0                                                        
            do 645 kcl=1,nrclst                                         
                 io=io+iclusz(kcl)                                      
  645       continue                                                    
              nrclst=nrclst+1                                           
              if(l.le.nrbar(kclst)) then                                
                 nrbar(nrclst)=1                                        
                 nrmes(nrclst)=0                                        
              else                                                      
                 nrbar(nrclst)=0                                        
                 nrmes(nrclst)=1                                        
              end if                                                    
              iclusz(nrclst)=1                                          
              nonrel(nrclst)=.false.                                    
              lcl=io+1                                                  
              if(lcl.ge.ntotma+1-nspect) then                           
                  lcl=iofset+iclusz(kclst)                              
                  iclusz(kclst)=iclusz(kclst)-1                         
                  if(iclusz(kclst).lt. nrbar(kclst)+nrmes(kclst))then   
                    call errex('storage overflow lcl.gt.ntotma,relham') 
                  end if                                                
                  do 652 kkcl=0,nrclst-2-kclst                          
                    kcl=nrclst-kkcl                                     
                    nrbar(kcl)=nrbar(kcl-1)                             
                    nrmes(kcl)=nrmes(kcl-1)                             
  652             continue                                              
                  if(l.le.nrbar(kclst)) then                            
                     nrbar(kclst+1)=1                                   
                     nrmes(kclst+1)=0                                   
                  else                                                  
                     nrbar(kclst+1)=0                                   
                     nrmes(kclst+1)=1                                   
                  end if                                                
              else                                                      
                 lmax=lcl                                               
              end if                                                    
              do 646 i=0,3                                              
                x(i,lcl)=x(i,il)                                        
                qdot(i,lcl)=0.0                                         
                p(i,lcl)=p(i,il)                                        
  646         continue                                                  
              em(lcl)=em(il)                                            
              ityp(lcl)=ityp(il)                                        
              decay(lcl)=decay(il)                                      
              lastcl(lcl)=lastcl(il)                                    
              issn(lcl)=issn(il)                                        
              npidec(lcl)=npidec(il)                                    
              ichbig(lcl)=0                                             
              tlast(lcl)=tlast(il)                                      
              ityp(il)=-9999                                            
              decay(il)=1.e+35                                          
              npion=npion+1                                             
  610    continue                                                       
         go to 601                                                      
  611    continue                                                       
      end if                                                            
      ngbrt=ngbrnr(nttot)                                               
      nghbmx=max(nghbmx,ngbsum(nttot)+ngbrt)                            
      nttomx=max(nttomx,nttot)                                          
      return                                                            
      end                                                               
