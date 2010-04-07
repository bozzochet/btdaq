CPAT,N2DEC    .                                                         
      subroutine resdec(ind,lmes,ntag)                                  
        include 'scatter7'                                              
        include 'partblk7'                                              
      real*4 bex,bey,bez,thetlu,philu,prtgam,par,plund                  
      real*4 emmres,resxs                                               
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
      dimension prob(10)                                                
      dimension j1cas(8),j2cas(8)                                       
      dimension j1ome(3),j2ome(3)                                       
      dimension ptot(0:3),pind(0:3)                                     
      real*8 bx,by,bz,b2,gam                                            
      dimension dr(0:3),xind(0:3)                                       
      real*4  xform(0:3)                                                
      logical nstyes,scayes,rhoyes,etayes,sigyes,lamyes,delyes,third,   
     a        casyes,mesyes,omeyes                                      
      logical issers                                                    
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
      logical  potyes                                                   
      data emrmin / 0.282/                                              
      data drind / 0.3/                                                 
      data emnst,widnst,emsca,widsca/1.44,0.200,0.500,0.400/            
      data j1cas /1,1,1,1,2,0,2,0/                                      
      data j2cas /2,0,2,0,1,1,1,1/                                      
      data j1ome /1,1,0/                                                
      data j2ome /1,1,0/                                                
      save  pt2max,ptgau,pt2gau,tavmax,tavgau                           
      character*33 vernum                                               
      character*46 pronam                                               
      data vernum/'version rqmd 2.4 -- 22-nov-96'/                      
      data pronam/'r e s d e c  -- resonance decay'/                    
      logical*1 ttvers                                                  
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
        pt2max=0.40**2                                                  
        ptgau=0.30                                                      
        pt2gau=ptgau**2                                                 
        tavmax=0.16                                                     
        tavgau=0.09                                                     
      end if                                                            
      if(switch(19)) then                                               
         ntag=-1                                                        
         decay(ind)=1.e+35                                              
         return                                                         
      end if                                                            
      id1232=0                                                          
      ievnt=lastcl(ind)                                                 
      zrand=ranf(0)                                                     
      do 20 i=0,3                                                       
      ptot(i)=p(i,ind)                                                  
   20 continue                                                          
      emold= em(ind)                                                    
      pindsq=ptot(0)*ptot(0)                                            
      pvec2=0.0                                                         
      do 25 i=1,3                                                       
   25 pvec2=pvec2+ptot(i)**2                                            
      pindsq=pindsq-pvec2                                               
      eminva=sqrt(pindsq)                                               
      npidol=npidec(ind)                                                
      npold=npidol                                                      
      itind=ityp(ind)                                                   
      itold=itind                                                       
        call oldnew(1,itold,npold,emold,kmult,ltyp)                     
      ident=issn(ind)                                                   
      iqinv=0                                                           
      npabs=iabs(npidol)                                                
      if(itind.ge.3.and.itind.le.6) then                                
        idn=1                                                           
        assign 15 to label2                                             
      else if(itind.ge.10.and.itind.le.11) then                         
        idn=2                                                           
         assign 16 to label2                                            
      elseif(itind.ge.85.and.itind.le.97.and.                           
     a           -npidol.ge.61.and.-npidol.le.64)then                   
        idn=1                                                           
         iqinv=1                                                        
         call qinvrt(itind,npidol,emold)                                
         assign 15 to label2                                            
      elseif(itind.ge.85.and.itind.le.97.and.                           
     a           -npidol.ge.41.and.-npidol.le.42)then                   
        idn=2                                                           
         iqinv=1                                                        
         call qinvrt(itind,npidol,emold)                                
         assign 16 to label2                                            
      elseif(itind.ge.85.and.itind.le.97.and.                           
     a                           npabs.eq.57)then                       
        idn=3                                                           
         if(npidol.eq.-57)then                                          
           iqinv=1                                                      
           call qinvrt(itind,npidol,emold)                              
         end if                                                         
         assign 13 to label2                                            
      elseif(itind.eq.99.and.-npidol.ge.61.and.-npidol.le.64)then       
        idn=1                                                           
         iqinv=1                                                        
         call qinvrt(itind,npidol,emold)                                
         assign 15 to label2                                            
      elseif(                                                           
     a        (npabs.ge.46.and.npabs.le.47).or.                         
     b         (npabs.ge.68.and.npabs.le.69)) then                      
        if(npabs.ge.46.and.npabs.le.47) then                            
          idn=5                                                         
        else                                                            
          idn=7                                                         
        end if                                                          
         if(npidol.lt.0) then                                           
           iqinv=1                                                      
           call qinvrt(itind,npidol,emold)                              
         end if                                                         
         if(npidol.le.47) then                                          
           kmult=min(4,itind-85)                                        
           isoind=1-2*(npidol-46)                                       
         else                                                           
           kmult=5                                                      
           isoind=1-2*(npidol-68)                                       
         end if                                                         
         assign 17 to label2                                            
      elseif(itind.ge.85.and.itind.le.97.and.                           
     a                           npabs.eq.70)then                       
        idn=8                                                           
         if(npidol.eq.-70)then                                          
           iqinv=1                                                      
           call qinvrt(itind,npidol,emold)                              
         end if                                                         
         if(itind+1-85.ne.5) then                                       
           call errex('resdec19:wrong resonance in entrance')           
           kmult=0                                                      
         else                                                           
           kmult=1                                                      
         end if                                                         
         assign 19 to label2                                            
      elseif(itind.ge.85.and.itind.le.97.and.                           
     a                              npabs.le.35)then                    
        idn=0                                                           
         assign 18 to label2                                            
      else                                                              
        write(8,*) ind,issn(ind),itind,npidol                           
        call errex('unknown resonance in resdec')                       
      end if                                                            
      if(potyes(ind,iqqind)) then                                       
        if(idn.lt.0.or.idn.gt.8)                                        
     a    call errex('resdec:abort,wrong idn')                          
        emeff=eminva                                                    
        if(idn.eq.1.or.idn.eq.2) then                                   
          embm=emnuc+empion                                             
        else                                                            
          embm=embmin(idn)+ekinmi                                       
        end if                                                          
        if(emeff.lt.embm) then                                          
          dp0=ptot(0)-sqrt(emold**2+pvec2)                              
          ps005=0.005*psyst(0)                                          
          if(ifndnx.eq.0) then                                          
            emeff=emold                                                 
            if(idn.eq.1.or.idn.eq.2) then                               
            else if(abs(pcorr(0)+dp0).lt.ps005                          
     a                  .or.pcorr(0)*dp0.lt.0.0) then                   
              pcorr(0)=pcorr(0)+dp0                                     
              p(0,ind)=sqrt(emold**2+pvec2)                             
              eminva=emold                                              
            else                                                        
              ntag=-1                                                   
              decay(ind)=decay(ind)+ptot(0)/emold*1.0                   
              return                                                    
            end if                                                      
          else                                                          
            if(abs(pcorr(0)+dp0).lt.ps005                               
     a                  .or.pcorr(0)*dp0.lt.0.0                         
     b                       .or.abs(dp0).lt.0.005) then                
              pcorr(0)=pcorr(0)+dp0                                     
              emeff=emold                                               
              eminva=emold                                              
            else                                                        
              write(8,*) itind,npidol,emeff,emold,issn(ind)             
              call errex('resdec: no phase space for resonance decay')  
            end if                                                      
          end if                                                        
        end if                                                          
      else                                                              
        emeff=eminva                                                    
      end if                                                            
      etayes=.false.                                                    
      delyes=.false.                                                    
      rhoyes=.false.                                                    
      scayes=.false.                                                    
      nstyes=.false.                                                    
      sigyes=.false.                                                    
      lamyes=.false.                                                    
      casyes=.false.                                                    
      omeyes=.false.                                                    
      mesyes=.false.                                                    
      go to label2                                                      
   18 continue                                                          
      mesyes=.true.                                                     
      if(npidol.eq.-18.or.npidol.eq.-19                                 
     a   .or.npidol.eq.-28.or.npidol.eq.-29)then                        
        iqinv=1                                                         
        call qinvrt(itind,npidol,emold)                                 
      end if                                                            
      kmult=itind-84                                                    
      i1=isosp(npabs)                                                   
      i31=isign(1,npidol)*isosp3(npabs)                                 
      identr=idtmes(kmult,npabs)                                        
      emmres=emeff                                                      
      mtry=0                                                            
  182 continue                                                          
      call mesres(2,                                                    
     a   identr,i1,i31,0,0,kfb,kfm,iclcod,isobre,istrin,emmres,resxs,   
     b                                                        reswav)   
      if(kfm.eq.0.or.kfb.eq.0) then                                     
        write(8,*)itind,npidol,kfb,kfm,emeff,identr,i1,i31              
        call errex('resdec18: no decay hadrons found')                  
      end if                                                            
  181 mtry=mtry+1                                                       
      embar=getmas(0,kfb,1,-1.0)                                        
      emmes=getmas(0,kfm,1,-1.0)                                        
      kfba=iabs(kfb)                                                    
      kfma=iabs(kfm)                                                    
      if(identr.eq.14) then                                             
        if(kfma.eq.27.or.kfma.eq.33)                                    
     a  emmes=emmes-0.2                                                 
        if(kfba.eq.27.or.kfba.eq.33)                                    
     a  embar=embar-0.2                                                 
      else if(identr.eq.18) then                                        
        if(kfma.eq.28.or.kfma.eq.29) then                               
          emmes=emmes-0.092                                             
        else if(kfba.eq.28.or.kfba.eq.29) then                          
          embar=embar-0.092                                             
        end if                                                          
      else if(identr.eq.26) then                                        
        if(kfma.eq.28.or.kfma.eq.29) then                               
          emmes=emmes-0.11                                              
        else if(kfba.eq.28.or.kfba.eq.29) then                          
          embar=embar-0.11                                              
        end if                                                          
        if(kfma.eq.27.or.kfma.eq.33) then                               
          emmes=emmes-0.22                                              
        else if(kfba.eq.27.or.kfba.eq.33) then                          
          embar=embar-0.22                                              
        end if                                                          
      else if(identr.eq.30.and.min(embar,emmes).gt.0.2) then            
        if(kfma.eq.27.or.kfma.eq.33) then                               
          emmes=emmes-0.31                                              
        else if(kfba.eq.27.or.kfba.eq.33) then                          
          embar=embar-0.31                                              
        end if                                                          
      else if(identr.eq.34) then                                        
        if(kfma.eq.28.or.kfma.eq.29) then                               
          emmes=emmes-0.32                                              
        else if(kfba.eq.28.or.kfba.eq.29) then                          
          embar=embar-0.32                                              
        end if                                                          
      end if                                                            
      if(mtry.gt.100) then                                              
        assign 182 to label                                             
      else                                                              
        assign 181 to label                                             
      end if                                                            
      if(kfma.eq.27.or.kfma.eq.33) then                                 
        if(emmes.lt.0.3) go to label                                    
      else if(kfma.eq.28.or.kfma.eq.29) then                            
        if(emmes.lt.0.64) go to label                                   
      end if                                                            
      if(kfba.eq.27.or.kfba.eq.33) then                                 
        if(embar.lt.0.3) go to label                                    
      else if(kfba.eq.28.or.kfba.eq.29) then                            
        if(embar.lt.0.64) go to label                                   
      end if                                                            
      if(embar+emmes.ge.emeff) then                                     
        if(mtry.gt.150) then                                            
          write(14,*)itind,npidol,kfb,kfm,emeff,embar,emmes             
          call errex('resdec181:infinite loop')                         
        end if                                                          
        go to label                                                     
      end if                                                            
      embr=embar                                                        
      ipi=0                                                             
      go to 1010                                                        
   19 continue                                                          
      omeyes=.true.                                                     
      mtry=0                                                            
  269 continue                                                          
      probs=0.0                                                         
      kfm=0                                                             
      do 271 ilop=1,3                                                   
       prob(ilop)=dcrato(ilop,kmult,emeff)                              
       probso=probs                                                     
       probs=probs+prob(ilop)                                           
       if(prob(ilop).ne.0.0.and.zrand.gt.probso.and.                    
     a                                   zrand.le.probs) then           
         j1=0                                                           
         m1=0                                                           
         j2=0                                                           
         m2=0                                                           
         j1new=j1ome(ilop)                                              
         j2new=j2ome(ilop)                                              
         itag=0                                                         
         call isocgk(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)          
         if(ilop.eq.3) then                                             
           kfm=24                                                       
         else if(ilop.eq.1.or.ilop.eq.3) then                           
           kfm=-17-(3+m2new)/2                                          
         end if                                                         
         if(ilop.eq.1) then                                             
           kfb=45+(3-m1new)/2                                           
         else if(ilop.eq.2) then                                        
           kfb=67+(3-m1new)/2                                           
         else if(ilop.eq.3) then                                        
           kfb=70                                                       
         end if                                                         
       end if                                                           
  271 continue                                                          
      if(probs.eq.0.0) then                                             
        call errex('resdec19: no phase space for decay')                
      else if(abs(1.-probs).gt.1.e-3) then                              
        call errex('resdec19: probabilities do not add up')             
      else if(zrand.gt.probs) then                                      
        mtry=mtry+1                                                     
        if(mtry.gt.50) call errex('resdec19: mtry.gt.50')               
        zrand=probs                                                     
        go to 269                                                       
      end if                                                            
      if(kfm.eq.0) then                                                 
        call errex('resdec19: no decay hadrons found')                  
      end if                                                            
      embar=getmas(0,kfb,1,-1.0)                                        
      emmes=getmas(0,kfm,1,-1.0)                                        
      if(embar+emmes+ekinmi.ge.emeff) go to 269                         
      embr=embar                                                        
      ipi=0                                                             
      go to 1010                                                        
   17 continue                                                          
      casyes=.true.                                                     
      mtry=0                                                            
  169 continue                                                          
      if(mtry.gt.50) call errex('resdec17: mtry.gt.50')                 
      probs=0.0                                                         
      kfm=0                                                             
      do 171 ilop=1,8                                                   
       prob(ilop)=dcratc(ilop,kmult,emeff)                              
       probso=probs                                                     
       probs=probs+prob(ilop)                                           
       if(prob(ilop).ne.0.0.and.zrand.gt.probso.and.                    
     a                                   zrand.le.probs) then           
         j1=1                                                           
         m1=isoind                                                      
         j2=0                                                           
         m2=0                                                           
         j1new=j1cas(ilop)                                              
         j2new=j2cas(ilop)                                              
         itag=0                                                         
         call isocgk(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)          
         if(ilop.eq.1.or.ilop.eq.3) then                                
           kfm=m2new/2*17+23*(2-iabs(m2new))/2                          
         else if(ilop.eq.2.or.ilop.eq.4) then                           
           kfm=24                                                       
         else if(ilop.eq.5.or.ilop.eq.6.or.ilop.eq.7) then              
           kfm=-17-(3+m2new)/2                                          
         else if(ilop.eq.8) then                                        
           kfm=17+(3-m2new)/2                                           
         end if                                                         
         if(ilop.eq.1.or.ilop.eq.2) then                                
           kfb=45+(3-m1new)/2                                           
         else if(ilop.eq.3.or.ilop.eq.4) then                           
           kfb=67+(3-m1new)/2                                           
         else if(ilop.eq.5) then                                        
           kfb=44-m1new/2                                               
         else if(ilop.eq.6) then                                        
           kfb=57                                                       
         else if(ilop.eq.7) then                                        
           kfb=66-m1new/2                                               
         else if(ilop.eq.8) then                                        
           kfb=70                                                       
         end if                                                         
       end if                                                           
  171 continue                                                          
      if(probs.eq.0.0) then                                             
        call errex('resdec17: no phase space for decay')                
      else if(abs(1.-probs).gt.1.e-3) then                              
        call errex('resdec17: probabilities do not add up')             
      else if(zrand.gt.probs) then                                      
        mtry=mtry+1                                                     
        zrand=probs                                                     
        go to 169                                                       
      end if                                                            
      if(kfm.eq.0) then                                                 
        call errex('resdec17: no decay hadrons found')                  
      end if                                                            
      embar=getmas(0,kfb,1,-1.0)                                        
      emmes=getmas(0,kfm,1,-1.0)                                        
      if(embar+emmes+ekinmi.ge.emeff) go to 169                         
      embr=embar                                                        
      ipi=0                                                             
      go to 1010                                                        
   13 continue                                                          
      if(zrand.le.0.33333) then                                         
          irand=1                                                       
          embar=emsigp                                                  
      else if(zrand.le.0.66667) then                                    
          irand=0                                                       
          embar=emsig0                                                  
      else                                                              
          irand=-1                                                      
          embar=emsigm                                                  
      end if                                                            
      embr=embar                                                        
      emmes=empion                                                      
      npidn=irand                                                       
      ipi= 8-irand                                                      
      in=15                                                             
      go to 1010                                                        
   15 continue                                                          
      third=(ranf(0).gt..666666666)                                     
      nchanl=-npidol/1000                                               
      if(nchanl.ne.1.and.emeff.lt.emnuc+(2.*empion+ekinmi)) npidol=0    
      wdnpi=0.                                                          
      wddpi=0.                                                          
      wdnrho=0.                                                         
      wdspi=0.                                                          
      if(npidol.lt.0) then                                              
         if(emold.le.emnuc+empion) then                                 
            write(8,*) itind,npidol,emold,ind,issn(ind)                 
            call errex('wrong emold in resdec,15 for negat. npidol')    
         end if                                                         
         npidol=-npidol                                                 
         if(npidol.gt.100) then                                         
            iblsgk=1                                                    
            npidol=mod(npidol,100)                                      
         else                                                           
            iblsgk=0                                                    
         end if                                                         
         if(npidol.gt.9) then                                           
            write(8,*) npidol,ind                                       
            call errex('wrong npidol in resdec')                        
         end if                                                         
         if(nchanl.gt.0) then                                           
            if(nchanl.eq.1) then                                        
                 wdnpi=1.                                               
            else if(nchanl.eq.2) then                                   
               if(emeff.gt.emnuc+2.*(empion+ekinmi)) then               
                 wddpi=1.                                               
               else                                                     
                 wdnpi=1.                                               
               end if                                                   
            else if(nchanl.eq.3) then                                   
              if(emeff.gt.emnuc+emrmin+ekinmi) then                     
                wdnrho=1.                                               
              else                                                      
                wdnpi=1.                                                
              end if                                                    
            else if(nchanl.eq.6) then                                   
              if(emeff.gt.emsigm+emka0+ekinmi) then                     
                wdspi=1.                                                
              else                                                      
                wdnpi=1.                                                
              end if                                                    
            else if(nchanl.eq.7) then                                   
              if(emeff.gt.emnuc+2.*empion+ekinmi) then                  
                nstyes=.true.                                           
                wdnpi=1.                                                
              else                                                      
                wdnpi=1.                                                
              end if                                                    
            else                                                        
              call errex('resdec: wrong nchanl in delta branch')        
            endif                                                       
         else                                                           
            wdnspi=dcratd(0,npidol,emeff)                               
            wdnpi=dcratd(1,npidol,emeff)                                
            if(wdnspi.gt.0.0) then                                      
              if(ranf(0).lt.wdnspi/(wdnpi+wdnspi)) nstyes=.true.        
            end if                                                      
            wdnpi=wdnpi+wdnspi                                          
            wddpi=dcratd(2,npidol,emeff)                                
            wdnrho=dcratd(3,npidol,emeff)                               
            if(iblsgk.eq.1) then                                        
               wdspi=0.                                                 
               wddpi=wddpi+dcratd(4,npidol,emeff)                       
            else                                                        
               wdspi=dcratd(4,npidol,emeff)                             
            end if                                                      
         end if                                                         
      else if(npidol.eq.0.or.npidol.eq.1) then                          
         wdnpi=1.                                                       
      else                                                              
         call errex('resdec:dead end in d branch????')                  
      end if                                                            
      if(abs(wdnpi+wddpi+wdnrho+wdspi-1.0).gt.1.e-3) then               
        write(8,*) 'npidol,nchanl,emeff,issn(ind)'                      
        write(8,*) npidol,nchanl,emeff,issn(ind)                        
        write(8,*) 'wdnpi,wddpi,wdnrho,wdspi'                           
        write(8,*)  wdnpi,wddpi,wdnrho,wdspi                            
        call errex('resdec:probabilities do not sum up to 1,dbranch')   
      end if                                                            
      if(npidol.eq.1) id1232=1                                          
      goto (103,104,105,106),itind-2                                    
      write(8,*)'error, select,  resdec , ind,ityp:',ind,itind          
      call errex('fatal error')                                         
  103 continue                                                          
      if((npidol.eq.0).or.(zrand.le.wdnpi+wdnrho)) then                 
         ipi=7                                                          
         in =1                                                          
         if((npidol.gt.0).and.                                          
     a      (zrand.gt.wdnpi).and.                                       
     b      (emeff.gt.emnuc+emrmin+ekinmi)) then                        
              rhoyes=.true.                                             
         end if                                                         
         goto 600                                                       
      else                                                              
          if(zrand.gt.1.- wdspi) then                                   
              if(emeff.gt.emsigm+emka0+ekinmi) then                     
               m1=-3                                                    
               go to 700                                                
              end if                                                    
          end if                                                        
          if(ranf(0).gt.0.6) then                                       
               ipi=7                                                    
               in=4                                                     
          else                                                          
               ipi=8                                                    
               in=3                                                     
          end if                                                        
          go to 800                                                     
      end if                                                            
  104 continue                                                          
      if(npidol.eq.0.or.(zrand.le.wdnpi+wdnrho)) then                   
         if (third) then                                                
            ipi=7                                                       
            in= 2                                                       
         else                                                           
            ipi=8                                                       
            in =1                                                       
         end if                                                         
         if((npidol.gt.0).and.                                          
     a      (zrand.gt.wdnpi).and.                                       
     b      (emeff.gt.emnuc+emrmin+ekinmi)) then                        
              rhoyes=.true.                                             
         end if                                                         
         goto 600                                                       
      else                                                              
          if(zrand.gt.1.- wdspi) then                                   
              if(emeff.gt.emsigm+emkap+ekinmi) then                     
               m1=-1                                                    
               go to 700                                                
              end if                                                    
          end if                                                        
          yrand=ranf(0)                                                 
          if(yrand.le.1./15.) then                                      
               ipi=8                                                    
               in=4                                                     
          else if(yrand.le.0.6) then                                    
               ipi=7                                                    
               in=5                                                     
          else                                                          
               ipi=9                                                    
               in=3                                                     
          end if                                                        
          go to 800                                                     
      end if                                                            
  105 continue                                                          
      if((npidol.eq.0).or.(zrand.le.wdnpi+wdnrho)) then                 
         if (third) then                                                
           ipi=9                                                        
           in =1                                                        
         else                                                           
           ipi=8                                                        
           in =2                                                        
         end if                                                         
         if((npidol.gt.0).and.                                          
     a      (zrand.gt.wdnpi).and.                                       
     b      (emeff.gt.emnuc+emrmin+ekinmi)) then                        
              rhoyes=.true.                                             
         end if                                                         
         goto 600                                                       
      else                                                              
          if(zrand.gt.1.- wdspi) then                                   
              if(emeff.gt.emsigp+emka0+ekinmi) then                     
               m1= 1                                                    
               go to 700                                                
              end if                                                    
          end if                                                        
          yrand=ranf(0)                                                 
          if(yrand.le.1./15.) then                                      
               ipi=8                                                    
               in=5                                                     
          else if(yrand.le.0.6) then                                    
               ipi=9                                                    
               in=4                                                     
          else                                                          
               ipi=7                                                    
               in=6                                                     
          end if                                                        
          go to 800                                                     
      end if                                                            
  106 continue                                                          
      if((npidol.eq.0).or.(zrand.le.wdnpi+wdnrho)) then                 
         ipi=9                                                          
         in =2                                                          
         if((npidol.gt.0).and.                                          
     a      (zrand.gt.wdnpi).and.                                       
     b      (emeff.gt.emnuc+emrmin+ekinmi)) then                        
              rhoyes=.true.                                             
         end if                                                         
         go to 600                                                      
      else                                                              
          if(zrand.gt.1.- wdspi) then                                   
              if(emeff.gt.emsigp+emkap+ekinmi) then                     
               m1= 3                                                    
               go to 700                                                
              end if                                                    
          end if                                                        
          if(ranf(0).gt.0.6) then                                       
               ipi=9                                                    
               in=5                                                     
          else                                                          
               ipi=8                                                    
               in=6                                                     
          end if                                                        
          go to 800                                                     
      end if                                                            
   16 continue                                                          
      znnpi=0.                                                          
      zndpi=0.                                                          
      znnrho=0.                                                         
      znneta=0.                                                         
      znnsca=0.                                                         
      znlpi=0.                                                          
      znspi=0.                                                          
      nchanl=-npidol/1000                                               
      if(nchanl.gt.0) then                                              
            znnpi=1.                                                    
            if(nchanl.eq.1) then                                        
            else if(nchanl.eq.2) then                                   
              if(emeff.gt.emnuc+2.*(empion+ekinmi)) then                
                znnpi=0.                                                
                zndpi=1.                                                
              end if                                                    
            else if(nchanl.eq.3) then                                   
              if(emeff.gt.emnuc+emrmin+ekinmi) then                     
                znnrho=1.                                               
                znnpi=0.                                                
              end if                                                    
            else if(nchanl.eq.4) then                                   
              if(emeff.gt.emnuc+emeta+ekinmi) then                      
                znneta=1.                                               
                znnpi=0.                                                
              end if                                                    
            else if(nchanl.eq.5) then                                   
              if(emeff.gt.emlamb+emka0+ekinmi) then                     
                znlpi=1.                                                
                znnpi=0.                                                
              end if                                                    
            else if(nchanl.eq.6) then                                   
              if(emeff.gt.emsigm+emka0+ekinmi) then                     
                znspi=1.                                                
                znnpi=0.                                                
              end if                                                    
            else                                                        
               call errex('resdec: wrong nchanl in n* branch')          
            endif                                                       
      else if(npidol.lt.0) then                                         
          npidol=-npidol                                                
          if(npidol.gt.100) then                                        
             npidol=mod(npidol,100)                                     
          else                                                          
             if(emeff.gt.emlamb+emka0)then                              
               znlpi=dcratn(5,npidol,emeff)                             
             end if                                                     
             if(emeff.gt.emsigm+emka0)then                              
               znspi=dcratn(6,npidol,emeff)                             
             end if                                                     
          end if                                                        
          znnpi= dcratn(1,npidol,emeff)                                 
          if(emeff.gt.emnuc+emeta)then                                  
                znneta= dcratn(4,npidol,emeff)                          
          else                                                          
                znneta= 0.                                              
          end if                                                        
          if(emeff.gt.emnuc+emrmin)then                                 
                znnrho=dcratn(3,npidol,emeff)                           
                znnsca=dcratn(0,npidol,emeff)                           
                znnrho=znnrho+znnsca                                    
          else                                                          
                znnrho=0.                                               
                znnsca=0.0                                              
          end if                                                        
          if(emeff.gt.emnuc+2.*empion) then                             
                zndpi=dcratn(2,npidol,emeff)                            
          else                                                          
               znnpi=1.                                                 
               zndpi=0.                                                 
          end if                                                        
      else if(npidol.eq.0) then                                         
              znnpi=1.                                                  
              zndpi=0.                                                  
              znneta=0.0                                                
              znnrho=0.                                                 
      else if(npidol.ge.1) then                                         
           call errex('resdec:dead end????')                            
      end if                                                            
      if(abs(znnpi+zndpi+znnrho+znneta+znlpi+znspi-1.0).gt.1.e-3) then  
        write(8,*) 'emeff=',emeff,em(ind),                              
     a                 issn(ind),ityp(ind),npidec(ind)                  
        write(8,*) 'nchanl,npidol=',nchanl,npidol                       
        write(8,*)                                                      
     a    'znnpi,zndpi,znnrho,znneta,znlpi,znspi'                       
        write(8,*)                                                      
     a     znnpi,zndpi,znnrho,znneta,znlpi,znspi                        
        call errex('resdec:probabilities do not sum up to 1,n* branch') 
      end if                                                            
      goto (10,11),itind-9                                              
      write(8,*)'error, select,  resdec , ind,ityp:',ind,itind          
      call errex('fatal error')                                         
   10 continue                                                          
      if(zrand.lt. znnpi+znnrho) then                                   
         if(zrand.ge.znnpi) then                                        
           if(ranf(0).lt.znnsca/znnrho) then                            
             scayes=.true.                                              
           else                                                         
             rhoyes=.true.                                              
           end if                                                       
         end if                                                         
         if(scayes.or.ranf(0).gt.0.666667) then                         
            ipi=8                                                       
            in =1                                                       
         else                                                           
            ipi=7                                                       
            in =2                                                       
         end if                                                         
         go to 600                                                      
      else if(zrand.lt.1.-znneta) then                                  
          zlimit=1.-znneta-znlpi                                        
          if(zrand.gt.zlimit) then                                      
            lamyes=.true.                                               
            ipi=14                                                      
            in= 13                                                      
            iskaon=1                                                    
            npnew=0                                                     
            embar=emlamb                                                
            emmes=emka0                                                 
            go to 500                                                   
          end if                                                        
          zlimit=zlimit-znspi                                           
          if(zrand.gt.zlimit) then                                      
            sigyes=.true.                                               
            ipi=14                                                      
            in= 15                                                      
            xrand=ranf(0)                                               
            if(xrand.lt.0.3333) then                                    
                 embar=emsig0                                           
                 emmes=emka0                                            
                 iskaon=1                                               
                 npnew=0                                                
            else                                                        
                 embar=emsigm                                           
                 emmes=emkap                                            
                 iskaon=2                                               
                 npnew=-1                                               
            endif                                                       
            go to 500                                                   
          end if                                                        
          yrand=ranf(0)                                                 
          if( yrand .lt. 0.166667) then                                 
            ipi=7                                                       
            in =5                                                       
          else if( yrand.lt. 0.5 ) then                                 
            ipi=8                                                       
            in =4                                                       
          else                                                          
            ipi=9                                                       
            in =3                                                       
          end if                                                        
          goto 800                                                      
      else                                                              
          etayes=.true.                                                 
          ipi=12                                                        
          in=itind-9                                                    
          go to 600                                                     
      end if                                                            
   11 continue                                                          
      if(zrand.lt. znnpi+znnrho) then                                   
          if(zrand.ge. znnpi) then                                      
             if(ranf(0).lt.znnsca/znnrho) then                          
               scayes=.true.                                            
             else                                                       
               rhoyes=.true.                                            
             end if                                                     
          end if                                                        
          if(scayes.or.ranf(0).gt.0.666667) then                        
              ipi=8                                                     
              in =2                                                     
          else                                                          
              ipi=9                                                     
              in =1                                                     
          end if                                                        
          go to 600                                                     
      else if(zrand.lt.1.-znneta) then                                  
          zlimit=1.-znneta-znlpi                                        
          if(zrand.gt.zlimit) then                                      
            lamyes=.true.                                               
            embar=emlamb                                                
            emmes=emka0                                                 
            ipi=14                                                      
            in= 13                                                      
            iskaon=2                                                    
            npnew=0                                                     
            go to 500                                                   
          end if                                                        
          zlimit=zlimit-znspi                                           
          if(zrand.gt.zlimit) then                                      
            sigyes=.true.                                               
            ipi=14                                                      
            in= 15                                                      
            xrand=ranf(0)                                               
            if(xrand.lt.0.3333) then                                    
                 embar=emsig0                                           
                 emmes=emkap                                            
                 iskaon=2                                               
                 npnew=0                                                
            else                                                        
                 embar=emsigp                                           
                 emmes=emka0                                            
                 iskaon=1                                               
                 npnew=1                                                
            endif                                                       
            go to 500                                                   
          endif                                                         
          yrand=ranf(0)                                                 
          if( yrand.lt. 0.166667) then                                  
              ipi=9                                                     
              in =4                                                     
          else if( yrand.lt. 0.5 ) then                                 
              ipi=8                                                     
              in =5                                                     
          else                                                          
              ipi=7                                                     
              in =6                                                     
          end if                                                        
          goto 800                                                      
      else                                                              
          etayes=.true.                                                 
          ipi=12                                                        
          in=itind-9                                                    
          go to 600                                                     
      end if                                                            
  500 continue                                                          
      npidn=npnew                                                       
      if(.not.lamyes.and..not.sigyes) then                              
         call errex('hyperon decay channel in resdec is wrong')         
      end if                                                            
      if(emeff.lt. embar+ emmes) then                                   
          write(8,*) emeff,embar+emmes,embar,emmes                      
          call errex('too low effective mass in resdec 500')            
      end if                                                            
      embr=embar                                                        
      go to 1010                                                        
  600 continue                                                          
      if(rhoyes) then                                                   
        if(nstyes) nstyes=.false.                                       
      else if(nstyes) then                                              
        go to 799                                                       
      end if                                                            
      embar=emnuc                                                       
      embr=embar                                                        
      npidn=0                                                           
      if(rhoyes.or.scayes) then                                         
         if(rhoyes) then                                                
           emres=emrho                                                  
           widres=widrho                                                
         else                                                           
           emres=emsca                                                  
           widres=widsca                                                
         end if                                                         
         cr=2.*(emrmin-emres)/widres                                    
         cr1=atan(cr)                                                   
         cr2=pi/2.-cr1                                                  
         emmx=emeff-emnuc-ekinmi                                        
         if(emmx.lt.emrmin)                                             
     a           call errex('too low maximum rho mass in resdec ')      
         xmax=(atan(2.*(emmx-emres)/widres)-cr1)/ cr2                   
         xmax=min(cvalu1,xmax)                                          
         xx=xmax*ranf(0)                                                
         t=tan(xx*cr2)                                                  
         emmes=emres+.5*widres*(cr+t)/(1.-cr*t)                         
      else if(etayes) then                                              
         emmes=emeta                                                    
      else                                                              
          if(eminva.lt.emnuc+empion) then                               
              if(emold.lt.emnuc+empion) then                            
                  write(8,*) 'emeff,issn(ind),ind,em(ind)'              
                  write(8,*) issn(ind),ind,emeff,eminva,em(ind)         
                  write(8,*) emold,(p(i,ind),i=0,3)                     
                  call errex('too low resonance mass in resdec 600')    
              else                                                      
                write(8,*) 'warning: resdec produces off shell nucleon' 
                write(8,*) issn(ind),ind,emeff,eminva,em(ind)           
                embr=sqrt(emnuc**2+(eminva**2-emold**2))                
              end if                                                    
          end if                                                        
          emmes=empion                                                  
      end if                                                            
      go to 1010                                                        
  700 continue                                                          
       nstyes=.false.                                                   
       ipi=14                                                           
       in=15                                                            
       sigyes=.true.                                                    
       j1=3                                                             
       j2=0                                                             
       j1new=2                                                          
       j2new=1                                                          
       m2=0                                                             
       itag=0                                                           
       call isocgk(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)            
       if(itag.eq.-1)                                                   
     a       call errex('isocgk does not allow for delta decay')        
       npid1= m1new/2                                                   
       iskaon= (3+m2new)/2                                              
       npidn=npid1                                                      
       if(npid1.eq.1) then                                              
         embar=emsigp                                                   
       else if(npid1.eq.0) then                                         
         embar=emsig0                                                   
       else                                                             
         embar=emsigm                                                   
       end if                                                           
       if(iskaon.eq.2) then                                             
         emmes=emkap                                                    
       else                                                             
         emmes=emka0                                                    
       end if                                                           
       embr=embar                                                       
       go to 1010                                                       
 799  continue                                                          
         nstyes=.true.                                                  
         if(npidol.eq.9) then                                           
           emres=1.680                                                  
           widres=0.125                                                 
           npidn=-5                                                     
         else if(npidol.eq.8) then                                      
           emres=1.675                                                  
           widres=0.155                                                 
           npidn=-5                                                     
         else if(npidol.eq.5) then                                      
           emres=1.520                                                  
           widres=0.125                                                 
           npidn=-2                                                     
         else                                                           
           emres=emnst                                                  
           widres=widnst                                                
           npidn=-1                                                     
         end if                                                         
         emmin=emnuc+empion+ekinmi                                      
         cr=2.*(emmin-emres)/widres                                     
         cr1=atan(cr)                                                   
         cr2=pi/2.-cr1                                                  
         emmx=emeff-empion-ekinmi                                       
         if(emmx.lt.emmin)                                              
     a           call errex('too low maximum n* mass in resdec')        
         xmax=(atan(2.*(emmx-emres)/widres)-cr1)/ cr2                   
         xmax=min(cvalu1,xmax)                                          
         xx=xmax*ranf(0)                                                
         t=tan(xx*cr2)                                                  
         embar=emres+.5*widres*(cr+t)/(1.-cr*t)                         
         embr=embar                                                     
         emmes=empion                                                   
         in=in+9                                                        
         go to 1010                                                     
 800  continue                                                          
      delyes=.true.                                                     
      npidn=1                                                           
      emmes=empion                                                      
      emmax=emeff-empion-ekinmi                                         
      if(emmax.lt. emnuc+empion)                                        
     a           call errex('too low n* mass in resdec 800')            
          xmax=(atan(2.*(emmax-emdelt)/widdlt)-const1)/                 
     #    const2                                                        
      xmax=min(cvalu1,xmax)                                             
      xx=xmax*ranf(0)                                                   
      t=tan(xx*const2)                                                  
      embar=emdelt+.5*widdlt*(const+t)/(1.-const*t)                     
      embr=embar                                                        
 1010 continue                                                          
      itypl=ipi                                                         
      kpthig=0                                                          
      if(rhoyes) then                                                   
        npidl=2                                                         
      else if(scayes) then                                              
        npidl=1                                                         
      else if(etayes) then                                              
         npidl=2                                                        
      else if(lamyes) then                                              
         npidl=iskaon                                                   
         kpthig=2                                                       
      else if(sigyes) then                                              
         npidl=iskaon                                                   
         kpthig=2                                                       
      else if(casyes.or.mesyes) then                                    
        call transl(1,itypl,npidl,kfm)                                  
        call transl(1,in,npidn,kfb)                                     
      else                                                              
        npidl=0                                                         
      end if                                                            
      if(iqinv.eq.1) then                                               
         call qinvrt(in,npidn,embar)                                    
         call qinvrt(itypl,npidl,emmes)                                 
      end if                                                            
      if(eminva.le.embr+emmes) then                                     
         write(8,*) eminva,emeff,embr,emmes                             
         call errex('no prel found in resdec')                          
      else                                                              
         prel=pcmsr(eminva,embr,emmes)                                  
      end if                                                            
      if(kpthig.eq.2) then                                              
      else if(ida(50).eq.1.and.                                         
     a          itind.le.11.and.mod(itind,10).le.6.and.                 
     b                          npold.ne.0.and.npold.ne.1) then         
        ijet=0                                                          
        nll=0                                                           
        do 14 nl=1,nlund                                                
         kh=mod(k(nl,1),100000)                                         
         ks=k(nl,1)/100000                                              
         if(ks.eq.4) then                                               
           if(kh.eq.ident ) then                                        
             if(ijet.ne.0) call errex('resdec:no unique jetsystem')     
             ijet=1                                                     
             klflag=k(nl,2)                                             
             klchar=k(nl+1,2)                                           
             p0h=plund(nl,4)+plund(nl+1,4)                              
             bxqqq=(plund(nl,1)+plund(nl+1,1))/p0h                      
             byqqq=(plund(nl,2)+plund(nl+1,2))/p0h                      
             bzqqq=(plund(nl,3)+plund(nl+1,3))/p0h                      
             if(klchar.eq.2) then                                       
               em1old=plund(nl,5)                                       
               em2old=plund(nl+1,5)                                     
               if(eminva.gt.em1old+em2old) then                         
                 prold=pcmsr(eminva,em1old,em2old)                      
               else                                                     
                 prold=-1.0                                             
               end if                                                   
             else                                                       
               prold=-1.0                                               
             end if                                                     
             mst(1)=nl                                                  
             mst(2)=nl+1                                                
             bex=-bxqqq                                                 
             bey=-byqqq                                                 
             bez=-bzqqq                                                 
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
         if(ijet.eq.0.or.ijet.gt.2) then                                
           nll=nll+1                                                    
           do 22 i=1,5                                                  
  22       plund(nll,i)=plund(nl,i)                                     
           k(nll,1)=k(nl,1)                                             
           k(nll,2)=k(nl,2)                                             
         else                                                           
           ijet=ijet+1                                                  
         end if                                                         
  14    continue                                                        
        if(ijet.eq.3) then                                              
          if(prold.gt.0.0) then                                         
            taviso=2.*prel*prold                                        
            if(taviso.gt.tavmax) then                                   
              if(ranf(0).le.                                            
     a           (taviso-tavmax)/(taviso-tavgau)) then                  
                kpthig=1                                                
              end if                                                    
            end if                                                      
          else                                                          
            pt2iso=0.6667*prel**2                                       
            if(pt2iso.gt.pt2max) then                                   
              if(ranf(0).le.                                            
     a              (pt2iso-pt2max)/(pt2iso-pt2gau)) then               
                kpthig=1                                                
              end if                                                    
            end if                                                      
          end if                                                        
          if(kpthig.eq.1) then                                          
            nl=nlund-1                                                  
            mst(1)=nl                                                   
            mst(2)=nl                                                   
            thetlu=thetqq                                               
            philu=phiqq                                                 
            izran=0                                                     
  501       izran=izran+1                                               
            if(izran.gt.50) call  errex('resdec:infinite loop at 501?') 
            zrand=ranf(0)                                               
            if(prold.gt.0.0) then                                       
              tact=tavgau*(-log(max(cvalu4,zrand)))                     
              if(tact.gt.4.*prel*prold) go  to 501                      
              ct=1.-0.5*tact/prel/prold                                 
              st=sqrt(1.-ct*ct)                                         
            else                                                        
              ptact=ptgau*sqrt(-log(max(cvalu4,zrand)))                 
              st=ptact/prel                                             
              if(st.gt.1.0) go  to 501                                  
              ct=sqrt(1.-st*st)                                         
            end if                                                      
            plund(nl,4)=0.0                                             
            plund(nl,3)=-ct                                             
            plund(nl,1)=-st                                             
            plund(nl,2)=0.0                                             
            call lurobo(thetlu,philu,0.0,0.0,0.0)                       
            thet=acos(max(-cvalu1,min(cvalu1,plund(nl,3))))             
            ct=cos(thet)                                                
            st=sin(thet)                                                
            phi=atan2(plund(nl,2),plund(nl,1))                          
            mst(1)=0                                                    
            mst(2)=0                                                    
          end if                                                        
          nlund=nlund-2                                                 
        end if                                                          
      end if                                                            
      if(kpthig.ne.1) then                                              
        ct=1.-2.*ranf(0)                                                
        phi=ranf(0)*pi2                                                 
        st=sqrt(1.-ct*ct)                                               
      end if                                                            
      pxrel=prel*cos(phi)*st                                            
      pyrel=prel*sin(phi)*st                                            
      pzrel=prel*ct                                                     
      npion=npion+1                                                     
      iwo=0                                                             
      if(nrmes(kclst)+nrbar(kclst).eq.iclusz(kclst))then                
         if(kclst.eq.itripl .and. kdt.lt.idtcl) then                    
            write(8,*)'warning resdec: iclusz(itripl) too low'          
         end if                                                         
         iwo=1                                                          
         io=0                                                           
         do 45 kcl=1,nrclst                                             
  45     io=io+ iclusz(kcl)                                             
         nrclst=nrclst+1                                                
         nrbar(nrclst)=0                                                
         nrmes(nrclst)=1                                                
         iclusz(nrclst)=1                                               
         nonrel(nrclst)=.false.                                         
         l=io+1                                                         
         lmax=l                                                         
         if(l.ge.ntotma+1-nspect)                                       
     a     call errex('particle number too large,abend in resdec')      
      else if(nrmes(kclst)+nrbar(kclst).lt.iclusz(kclst))then           
         nrmes(kclst)=nrmes(kclst)+1                                    
         l=iofset+nrbar(kclst)+nrmes(kclst)                             
      else                                                              
         call errex('nrbar+nrmes.gt.iclusz  ????')                      
      end if                                                            
      nprtcl=nprtcl+1                                                   
      if(nprtcl.gt.ntotma) then                                         
        call errex('nprtcl>ntotma in resdec, abort')                    
      end if                                                            
      ityp(l)=itypl                                                     
      npidec(l)=npidl                                                   
      em(l)=emmes                                                       
      ichbig(l)=0                                                       
      if(ida(15).ne.0)  ichbig(l)=3                                     
      lastcl(l)=ievnt                                                   
      issn(l)=nprtcl                                                    
      identl=nprtcl                                                     
      tdtg(identl)=0.0                                                  
      itdth(1,identl)=0                                                 
      itdth(2,identl)=0                                                 
      itdth(3,identl)=0                                                 
      itdth(4,identl)=0                                                 
      itdth(5,identl)=0                                                 
      bx=ptot(1)/ptot(0)                                                
      by=ptot(2)/ptot(0)                                                
      bz=ptot(3)/ptot(0)                                                
      b2=dmax1(1.d-6,bx*bx+by*by+bz*bz)                                 
      gam=1./dsqrt(1.-b2)                                               
      pb=bx*pxrel+by*pyrel+bz*pzrel                                     
      ea=sqrt(em(l)*em(l)+pxrel*pxrel+pyrel*pyrel+pzrel*pzrel)          
      gg=(gam-1.)*pb/b2+gam*ea                                          
      p(1,l)=gg*bx+pxrel                                                
      p(2,l)=gg*by+pyrel                                                
      p(3,l)=gg*bz+pzrel                                                
      pp= p(1,l)*p(1,l)+p(2,l)*p(2,l)+p(3,l)*p(3,l)                     
      p(0,l)=sqrt(em(l)*em(l)+pp)                                       
         deltr= drind*sqrt(ranf(0))                                     
         phi=pi2*ranf(0)                                                
         costet=1.-2.*ranf(0)                                           
         sintet=sqrt(1.-costet*costet)                                  
         xrel= deltr* sintet* sin(phi)                                  
         yrel= deltr* sintet* cos(phi)                                  
         zrel= deltr* costet                                            
         xb=bx*xrel+by*yrel+bz*zrel                                     
         gg=(gam-1.)*xb/b2                                              
         dr(0)= gam*xb                                                  
         dr(1)=gg*bx+xrel                                               
         dr(2)=gg*by+yrel                                               
         dr(3)=gg*bz+zrel                                               
      delttr=decay(ind)-x(0,ind)                                        
      if(qdot(0,ind).gt.0.) then                                        
        do 201 i=0,3                                                    
         xind(i)=x(i,ind)+qdot(i,ind)/qdot(0,ind)*delttr                
         x(i,l)=xind(i)+dr(i)                                           
 201    continue                                                        
      else                                                              
         if(abs(delttr).gt.0.001) then                                  
          write(8,*) 'warning from resdec:  qdot(0)=0', ind,issn(ind),  
     a               ityp(ind),npidec(ind),x(0,ind),decay(ind)          
          call errex('resdec: qdot(0)=0 ??????')                        
         end if                                                         
         do 202 i=0,3                                                   
          xind(i)=x(i,ind)                                              
          x(i,l)=xind(i)+ dr(i)                                         
 202     continue                                                       
      end if                                                            
      tlast(l)=x(0,l)                                                   
      ylast(nprtcl)=0.5*log((p(0,l)+p(3,l))/(p(0,l)-p(3,l)))            
      zlast(nprtcl)=x(3,l)-(pnorm(0)*x(0,l)-pnorm(3)*x(3,l))*pnorm(3)   
      pidsq=embr*embr                                                   
      eb=sqrt(pidsq+pxrel*pxrel+pyrel*pyrel+pzrel*pzrel)                
      gg=(gam-1.)*(-pb)/b2+gam*eb                                       
      p(1,ind)=gg*bx-pxrel                                              
      p(2,ind)=gg*by-pyrel                                              
      p(3,ind)=gg*bz-pzrel                                              
      pp= p(1,ind)*p(1,ind)+p(2,ind)*p(2,ind)+p(3,ind)*p(3,ind)         
      p(0,ind)=sqrt(pidsq+pp)                                           
      em(ind)=embar                                                     
      ityp(ind)=in                                                      
      npidec(ind)=npidn                                                 
      lastcl(ind)=ievnt                                                 
      tlast(ind)=xind(0)                                                
      itdth(5,ident)=0                                                  
      startt=x(0,l)                                                     
      call dupdat(l,startt)                                             
      startt=decay(ind)                                                 
      call dupdat(ind,startt)                                           
      if(ifndnx.eq.0.and.ntag.eq.0)then                                 
       if(ida(28).eq.0) then                                            
           isccss=0                                                     
           imscst=2                                                     
           call mscstr(imscst,ind,l,isccss)                             
       else                                                             
           isccss=2                                                     
       end if                                                           
       if(switch(10)) then                                              
            if(id1232.eq.1) ntag=l                                      
            call paulin(ind,ntag)                                       
            if(ntag.gt.0) call errex('ntag>0 not allowed in resdec')    
       end if                                                           
       if(isccss.lt.1) ntag=-1                                          
       if(ntag.lt.0)then                                                
           idm=1                                                        
           call rstifo(ind,idm)                                         
           startt=xind(0)                                               
           call dupdat(ind,startt)                                      
           nprtcl=nprtcl-1                                              
           npion=npion-1                                                
           if(iwo.eq.0) then                                            
              nrmes(kclst)=nrmes(kclst)-1                               
           else                                                         
              nrclst= nrclst-1                                          
           end if                                                       
           ityp(l)=-9999                                                
       end if                                                           
      end if                                                            
      if(ntag.eq.0)then                                                 
         lmes=l                                                         
         do 401 i=0,3                                                   
  401    x(i,ind)=xind(i)                                               
         if(itdth(2,ident).ne.0.and.                                    
     a                      tdtg(ident).gt.xind(0)) then                
           bxres=bx                                                     
           byres=by                                                     
           bzres=bz                                                     
           call qdrot(ind,ident,bxres,byres,bzres)                      
           decay(ind)=tdtg(ident)                                       
           ilogo=itdth(2,ident)                                         
           iexpon=isign(1,ilogo)*mod(iabs(ilogo),100)                   
         else                                                           
           iexpon=0                                                     
         end if                                                         
         ylast(ident)=0.5*log((p(0,ind)+p(3,ind))/(p(0,ind)-p(3,ind)))  
         zlast(ident)=x(3,ind)-(pnorm(0)*x(0,ind)-                      
     a                                 pnorm(3)*x(3,ind))*pnorm(3)      
      if(ida(5).eq.3) then                                              
        ltypa=iabs(ltyp)                                                
        if(ltypa.eq.41.or.ltypa.eq.42.or.ltypa.eq.62                    
     a                                 .or.ltypa.eq.63) then            
          if(kmult.ge.0) then                                           
            ibcode=100*kmult+ltyp                                       
            write(10,1663) ibcode,ptot(0),(-ptot(i),i=1,3),emold        
 1663 format(1x,i4,1x,5(f7.3,1x))                                       
          end if                                                        
        end if                                                          
      end if                                                            
         if(ida(25).eq.1) then                                          
           if(switch(23)) then                                          
            ken= itripl                                                 
           else                                                         
            ken= kdt                                                    
           end if                                                       
           write(15,661)                                                
     a        irun,ken,2,0,2,0,ident*1000+nclcnt(ident),0               
  661      format(1x,i3,1x,i3,1x,i1,1x,i5,1x,i3,1x,i2,1x,i8,1x,i8)      
  662    format(1x,i4,1x,i4,1x,f8.3,1x,2(f7.3,1x),f8.3,1x,              
     a               f7.3,i3,1x,i8,1x,f8.3,2(1x,f7.3),1x,f8.3)          
           if(iexpon.eq.0.or.qdot(0,ind).eq.0.0) then                   
             do 203 i=0,3                                               
  203        xform(i)=xind(i)                                           
           else                                                         
             do 204 i=0,3                                               
  204        xform(i)=xind(i)+qdot(i,ind)/qdot(0,ind)*                  
     a                            (decay(ind)-xind(0))                  
           end if                                                       
           if(iqinv.eq.0) then                                          
             itold=itind                                                
           else                                                         
             call oldat(1,itold,npold,p0old,pxold,pyold,pzold,emold)    
           end if                                                       
           if(npold.lt.-99) then                                        
             npidl=-mod(-npold,100)                                     
           else                                                         
             npidl=npold                                                
           end if                                                       
           write(15,662)itold,npidl,ptot(0),ptot(1),ptot(2),            
     a                                      ptot(3),emold,iexpon,       
     b                                   ident,xind(0),                 
     c                               xind(1),xind(2),xind(3)            
           write(15,662) ityp(ind),npidec(ind),(p(i,ind),i=0,3),        
     a              em(ind),iexpon,issn(ind),xform(0),                  
     b                               xform(1),xform(2),xform(3)         
           write(15,662)ityp(l),npidec(l),(p(i,l),i=0,3),em(l),         
     a                        0,issn(l),x(0,l),                         
     b                               x(1,l),x(2,l),x(3,l)               
           if(ida(36).eq.1) then                                        
              if(ifndnx.eq.0) then                                      
                 call dnscal(1,ind,-1,dens1)                            
                 write(15,663) dens1                                    
              else                                                      
                 write(15,663) 0.                                       
              end if                                                    
           end if                                                       
  663      format(1x,1(f6.3,1x))                                        
           if(ida(38).eq.3.or.ida(38).eq.4) then                        
              if(itind.ge.3.and.itind.le.6.and.                         
     a                 (npold.eq.0.or.npold.eq.1)) then                 
                kstor(identl,1)=7                                       
                kstor(ident,1)=7                                        
              else                                                      
                kstor(identl,1)=3                                       
                kstor(ident,1)=3                                        
              end if                                                    
              if(ida(38).eq.3) then                                     
                do 303 i=1,4                                            
                 stork(identl,i)=x(i-1,l)                               
                 stork(ident,i)=x(i-1,ind)                              
  303           continue                                                
              else                                                      
                do 304 i=1,4                                            
  304           stork(identl,i)=stork(ident,i)                          
              end  if                                                   
              do 403 i=6,8                                              
               stork(identl,i)=p(i-5,l)                                 
               stork(ident,i)=p(i-5,ind)                                
  403         continue                                                  
           else if(ida(38).eq.1) then                                   
             emil=em(l)                                                 
             if(issers(1,ida(38),ityp(l),npidec(l),emil))then           
                   stork(identl,1)=stork(ident,1)                       
                   stork(identl,2)=stork(ident,2)                       
                   stork(identl,3)=stork(ident,3)                       
                   stork(identl,4)=stork(ident,4)                       
                   stork(identl,5)=stork(ident,5)                       
                   kstor(identl,1)=kstor(ident,1)                       
                   kstor(identl,2)=kstor(ident,2)                       
                   kstor(identl,3)=kstor(ident,3)                       
                   kstor(identl,4)=kstor(ident,4)                       
                   kstor(identl,5)=kstor(ident,5)                       
                   kstor(identl,6)=kstor(ident,6)                       
                   kstor(identl,7)=kstor(ident,7)                       
             end if                                                     
           end if                                                       
         end if                                                         
         if(ida(42).eq.1) then                                          
           write(7)                                                     
     &        ityp(ind),issn(ind),npidec(ind),                          
     &        x(0,ind),x(1,ind),x(2,ind),x(3,ind),                      
     &        p(0,ind),p(1,ind),p(2,ind),p(3,ind),em(ind),              
     &        tdtg(ident),itdth(2,ident),itdth(3,ident)                 
           if(iexpon.ne.0) then                                         
             write(7)                                                   
     &        ityp(ind),issn(ind),npidec(ind),                          
     &        (xform(i),i=0,3),                                         
     &        p(0,ind),p(1,ind),p(2,ind),p(3,ind),em(ind),              
     &        0.0,0,0                                                   
           end if                                                       
           write(7)                                                     
     &          ityp(l),issn(l),npidec(l),                              
     &          x(0,l),x(1,l),x(2,l),x(3,l),                            
     &          p(0,l),p(1,l),p(2,l),p(3,l),em(l),                      
     &          0.0,0,0                                                 
         end if                                                         
         if(iwo.eq.0) then                                              
              call propag(ind,l,0)                                      
         else                                                           
              call propag(ind,-2,0)                                     
         end if                                                         
      end if                                                            
      return                                                            
      end                                                               
      real function timelf(widgev,em,px,py,pz)                          
      include 'scatter7'                                                
      if(widgev.lt.0.) then                                             
         timelf=0.                                                      
         return                                                         
      else if(widgev.gt.1.0e+10) then                                   
         timelf=0.                                                      
         return                                                         
      end if                                                            
      wid=widgev*5.0674/da(4)                                           
      p0=sqrt(em**2+px**2+py**2+pz**2)                                  
      gam=p0/em                                                         
      zrand=ranf(0)                                                     
      tau=-log(max(zrand,cvalu2) )/wid                                  
      timelf=gam*tau                                                    
      return                                                            
      end                                                               
      real function dwidth(em)                                          
        include 'scatter7'                                              
       real*4 gamres                                                    
       common /gamrbk/ gamres                                           
      if(em.gt.emnuc+empion) then                                       
         pr=pcmsr(em,emnuc,empion)                                      
      else                                                              
         pr=0.                                                          
      end if                                                            
      dwidth=29.*pr**3/(1.+40.*pr*pr)                                   
      return                                                            
      end                                                               
      function dcratd(i,j,em)                                           
       include 'scatter7'                                               
       real*4 gamres,gemdl,emdl                                         
       common /gamrbk/ gamres                                           
       dimension lres(0:4,9),prob(0:4)                                  
       data lres /1,1,1,1,1,                                            
     a            0,0,3,0,0,                                            
     b            3,3,0,3,3,                                            
     c            0,0,0,3,0,                                            
     d            4,4,2,2,4,                                            
     e            2,2,2,2,2,                                            
     f            2,2,2,2,2,                                            
     g            3,3,3,3,3,                                            
     h            4,4,4,4,4                                             
     i           /                                                      
       dimension gemdl(9)                                               
       data gemdl  /0.25, 0.16, 0.280, 0.15, 0.300, 0.220,              
     a              0.22, 0.25 ,0.24/                                   
       dimension emdl(9)                                                
       data emdl  /1.60, 1.62, 1.700, 1.90, 1.905, 1.910,               
     a              1.92, 1.93 ,1.95/                                   
       dimension dcrat(0:4,9)                                           
       data dcrat /                                                     
     a          0.15,0.35, 0.45, 0.05, 0.00,                            
     b          0.03,0.25, 0.60, 0.12, 0.00,                            
     c          0.00,0.15,.6984, 0.15,.0016,                            
     d          0.40,0.10, 0.05, 0.45, 0.00,                            
     e          0.16,0.10, 0.25, 0.45, 0.04,                            
     f          0.50,0.22, 0.14, 0.14, 0.00,                            
     g          0.40,0.20, 0.333,0.00,0.067,                            
     h          0.19,0.15, 0.36, 0.30, 0.00,                            
     i          0.13,0.55, 0.26, 0.053, 0.007                           
     j                                         /                        
       data prresm / 0.15/                                              
       if(j.le.0.or.j.gt.9) then                                        
         write(8,*) 'dcratd j=',j                                       
         call errex('index j out of range in dcratd')                   
       end if                                                           
       if(i.lt.0.or.i.gt.4) then                                        
         call errex('index i out of range in dcratd')                   
       end if                                                           
       probs=0.                                                         
       do 10 ilop=0,4                                                   
       if(ilop.eq.0)then                                                
         if(j.eq.9) then                                                
           embar=1.680                                                  
         else if(j.eq.8) then                                           
           embar=1.675                                                  
         else if(j.eq.5) then                                           
           embar=1.520                                                  
         else                                                           
           embar=1.440                                                  
         end if                                                         
         emmes=empion                                                   
       else if(ilop.eq.1)then                                           
         embar=emnuc                                                    
         emmes=empion                                                   
       else if(ilop.eq.2)then                                           
         embar=emdelt                                                   
         emmes=empion                                                   
       else if(ilop.eq.3)then                                           
         embar=emnuc                                                    
         emmes=emrho                                                    
       else if(ilop.eq.4)then                                           
         embar=emsigm                                                   
         emmes=emka0                                                    
       end if                                                           
       emres=emdl(j)                                                    
       if(emres.gt.embar+emmes) then                                    
         prres=pcmsr(emres,embar,emmes)                                 
         prres=max(prres,prresm)                                        
       else                                                             
         prres=prresm                                                   
       end if                                                           
       if(em.gt.embar+emmes) then                                       
         pr=pcmsr(em,embar,emmes)                                       
         vfg=(emres/em)*1.2/(1.+0.2*(pr/prres)**(2*lres(ilop,j)))       
       else                                                             
         pr=0.                                                          
         vfg=0.                                                         
       end if                                                           
       prob(ilop)=dcrat(ilop,j)*(pr/prres)**(2*lres(ilop,j)+1)*vfg      
       if(prob(ilop).lt.0.) then                                        
          call errex('neg. probability in dcratd')                      
       end if                                                           
       probs=probs+prob(ilop)                                           
   10  continue                                                         
       gamres=probs*gemdl(j)                                            
       if(probs.le.0.) then                                             
           call errex('probs=0. abort in dcratd')                       
       end if                                                           
       dcratd=prob(i)/probs                                             
      return                                                            
      end                                                               
      function dcratn(i,j,em)                                           
       include 'scatter7'                                               
       real*4 gamres,gemn2,emn2                                         
       common /gamrbk/ gamres                                           
       dimension lres(0:6,10),prob(0:6)                                 
       data lres /                                                      
     a             1,1,1,1,1,1,1,                                       
     b             2,2,0,0,2,2,2,                                       
     c             0,0,2,0,0,0,0,                                       
     d             0,0,2,0,0,0,0,                                       
     e             2,2,2,2,2,2,2,                                       
     f             3,3,1,1,3,3,3,                                       
     g             2,2,2,0,2,2,2,                                       
     h             1,1,1,1,1,1,1,                                       
     i             1,1,1,1,1,1,1,                                       
     j             3,3,3,3,3,3,3                                        
     k           /                                                      
       dimension emn2(10)                                               
       data emn2  /1.44, 1.52, 1.535, 1.65, 1.675, 1.680,               
     a              1.7, 1.71, 1.72, 1.99/                              
       dimension gemn2(10)                                              
       data gemn2  /0.200,0.125, 0.150, 0.15, 0.155, 0.125,             
     a              0.1, 0.11, 0.20, 0.29/                              
       dimension dcrat(0:6,10)                                          
       data dcrat /                                                     
     a         0.200,0.50, 0.15, 0.15, 0.00, 0.00, 0.00,                
     b         0.000,0.55, 0.25, 0.20, 0.00, 0.00, 0.00,                
     c         0.050,0.40, 0.00, 0.05, 0.50, 0.00, 0.00,                
     d         0.080,0.60, 0.05, 0.175,0.015,0.08, 0.00,                
     e         0.014,0.375,0.575,0.025,0.01, 0.001,0.00,                
     f         0.150,0.60, 0.125,0.125,0.00, 0.00, 0.00,                
     g         0.379,0.10, 0.379,0.10, 0.04, 0.002,0.00,                
     h         0.090,0.15, 0.10, 0.20, 0.25, 0.15, 0.06,                
     i         0.100,0.15, 0.10, 0.53, 0.035,0.05, 0.035,               
     j         0.000,0.05, 0.40, 0.43, 0.07, 0.03, 0.02   /             
       data prresm / 0.15/                                              
       data ibug/ 0/                                                    
       if(j.le.0.or.j.gt.10) then                                       
         call errex('index j out of range in dcratn')                   
       end if                                                           
       if(i.lt.0.or.i.gt.6) then                                        
         call errex('index i out of range in dcratn')                   
       end if                                                           
       probs=0.                                                         
       do 10 ilop=0,6                                                   
       if(ilop.eq.0)then                                                
         embar=emnuc                                                    
         emmes=0.60                                                     
       else if(ilop.eq.1)then                                           
         embar=emnuc                                                    
         emmes=empion                                                   
       else if(ilop.eq.2)then                                           
         embar=emdelt                                                   
         emmes=empion                                                   
       else if(ilop.eq.3)then                                           
         embar=emnuc                                                    
         emmes=emrho                                                    
       else if(ilop.eq.4)then                                           
         embar=emnuc                                                    
         emmes=emeta                                                    
       else if(ilop.eq.5)then                                           
         embar=emlamb                                                   
         emmes=emka0                                                    
       else if(ilop.eq.6)then                                           
         embar=emsigm                                                   
         emmes=emka0                                                    
       end if                                                           
       emres=emn2(j)                                                    
       if(emres.gt.embar+emmes) then                                    
         prres=pcmsr(emres,embar,emmes)                                 
         prres=max(prres,prresm)                                        
       else                                                             
         prres=prresm                                                   
       end if                                                           
       if(em.gt.embar+emmes) then                                       
         pr=pcmsr(em,embar,emmes)                                       
         vfg=(emres/em)*1.2/(1.+0.2*(pr/prres)**(2*lres(ilop,j)))       
       else                                                             
         pr=0.                                                          
         vfg=0.                                                         
       end if                                                           
       prob(ilop)=dcrat(ilop,j)*(pr/prres)**(2*lres(ilop,j)+1)*vfg      
       if(prob(ilop).lt.0.) then                                        
           write(8,*) em,j,i,prob(ilop),probs                           
           write(8,*) ilop,prres,pr,emres,embar,emmes                   
          call errex('neg. probability in dcratn')                      
       end if                                                           
       probs=probs+prob(ilop)                                           
   10  continue                                                         
       gamres=probs*gemn2(j)                                            
       if(probs.le.0.) then                                             
           write(8,*) em,j,i,(prob(ilop),ilop=1,4),probs                
           call errex('probs=0. abort in dcratn')                       
       end if                                                           
       dcratn=prob(i)/probs                                             
      return                                                            
      end                                                               
      function dcrato(i,j,em)                                           
       include 'scatter7'                                               
       real*4 pmas,pwid,cfr                                             
       common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)     
       parameter (ichaom=3,irenom=1)                                    
       real*4 gamres,gemom,emom                                         
       common /gamrbk/ gamres                                           
       dimension lres(ichaom,irenom),it1(ichaom),it2(ichaom)            
       data lres /3*3                                                   
     e           /                                                      
       data it1 / 47,69,70                                              
     a          /                                                       
       data it2 / 19,19,24                                              
     a          /                                                       
       data ibegin /0/                                                  
       dimension emom(irenom),gemom(irenom),dcrat(ichaom,irenom)        
       data emom  /2.230 /                                              
       data gemom / 0.036/                                              
       data dcrat /                                                     
     a              0.928, 0.072, 0.000/                                
       dimension prob(ichaom)                                           
       if(j.le.0.or.j.gt.irenom) then                                   
         write(8,*) 'dcrato j=',j                                       
         call errex('index j out of range in dcrato')                   
       end if                                                           
       if(i.lt.1.or.i.gt.ichaom) then                                   
         call errex('index i out of range in dcrato')                   
       end if                                                           
       probs=0.                                                         
       do 10 ilop=1,ichaom                                              
       embar=pmas(it1(ilop))                                            
       emmes=pmas(it2(ilop))                                            
       emres=emom(j)                                                    
       if(emres.gt.embar+emmes) then                                    
          prres=pcmsr(emres,embar,emmes)                                
       else                                                             
         prres=-1.                                                      
       end if                                                           
       if(em.gt.embar+emmes) then                                       
         pr=pcmsr(em,embar,emmes)                                       
         vfg=(emres/em)*1.2/(1.+0.2*(pr/prres)**(2*lres(ilop,j)))       
       else                                                             
         pr=0.                                                          
         vfg=0.                                                         
       end if                                                           
       prob(ilop)=dcrat(ilop,j)*(pr/prres)**(2*lres(ilop,j)+1)*vfg      
       if(prob(ilop).lt.0.) then                                        
          call errex('neg. probability in dcrato')                      
       end if                                                           
       probs=probs+prob(ilop)                                           
   10  continue                                                         
       gamres=probs*gemom(j)                                            
       if(probs.le.0.) then                                             
           write(8,*) 'em=',em,i,j                                      
           call errex('probs=0. abort in dcrato')                       
       end if                                                           
       dcrato=prob(i)/probs                                             
      return                                                            
      end                                                               
      function dcratc(i,j,em)                                           
       include 'scatter7'                                               
       real*4 pmas,pwid,cfr                                             
       common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)     
       real*4 gamres,gemca,emca                                         
       common /gamrbk/ gamres                                           
       dimension lres(8,5),prob(8),it1(8),it2(8)                        
       data it1 / 47,47,69,69,45,57,67,70                               
     a          /                                                       
       data it2 / 17,24,17,24,19,19,19,19                               
     a          /                                                       
       data lres /2*0,2*2,2*0,2*2,                                      
     a            2*2,2*0,2*2,2*0,                                      
     b            2*3,2*1,2*3,2*1,                                      
     c            8*2,                                                  
     d            2*3,2*2,2*3,2*2                                       
     e           /                                                      
       data ibegin /0/                                                  
       dimension emca(5),gemca(5),dcrat(8,5)                            
       data emca  / 1.82,1.835,1.945,2.04,2.130 /                       
       data gemca /                                                     
     a  0.025,0.076,0.084,0.059,0.052/                                  
       data dcrat /                                                     
     a  0.261, 0.944, 0.013, 0.024, 0.300, 0.425, 0.024, 0.048,         
     b  0.604, 0.005, 0.086, 1.451, 0.190, 0.120, 1.451, 2.902,         
     c  0.573, 0.000, 0.145, 1.070, 0.166, 0.102, 0.013, 2.140,         
     d  0.008, 0.029, 0.033, 0.085, 0.691, 0.221, 0.018, 0.169,         
     e  0.278, 0.032, 0.054, 0.000, 0.186, 0.343, 0.107, 0.690/         
       if(j.le.0.or.j.gt.5) then                                        
         write(8,*) 'dcratc j=',j                                       
         call errex('index j out of range in dcratc')                   
       end if                                                           
       if(i.lt.1.or.i.gt.8) then                                        
         call errex('index i out of range in dcratc')                   
       end if                                                           
       probs=0.                                                         
       do 10 ilop=1,8                                                   
       embar=pmas(it1(ilop))                                            
       emmes=pmas(it2(ilop))                                            
       emres=emca(j)                                                    
       if(emres.gt.embar+emmes) then                                    
         prres=pcmsr(emres,embar,emmes)                                 
       else                                                             
         embax=pmas(46)                                                 
         prres=pcmsr(emres,embax,empion)                                
       end if                                                           
       if(em.gt.embar+emmes) then                                       
         pr=pcmsr(em,embar,emmes)                                       
         vfg=(emres/em)*1.2/(1.+0.2*(pr/prres)**(2*lres(ilop,j)))       
       else                                                             
         pr=0.                                                          
         vfg=0.                                                         
       end if                                                           
       prob(ilop)=dcrat(ilop,j)*(pr/prres)**(2*lres(ilop,j)+1)*vfg      
       if(prob(ilop).lt.0.) then                                        
          call errex('neg. probability in dcratc')                      
       end if                                                           
       probs=probs+prob(ilop)                                           
   10  continue                                                         
       gamres=probs*gemca(j)                                            
       if(probs.le.0.) then                                             
           write(8,*) 'em=',em,i,j                                      
           call errex('probs=0. abort in dcratc')                       
       end if                                                           
       dcratc=prob(i)/probs                                             
      return                                                            
      end                                                               
      subroutine bwdist(idn,irand,em,ires,ibrnch,dcrat,bwig)            
       include 'scatter7'                                               
       real*4 gamres,emdl,emn2,emca                                     
       common /gamrbk/ gamres                                           
       dimension prob(10)                                               
       dimension emdl(9),emn2(10),emca(5)                               
       data emdl  /1.60, 1.62, 1.700, 1.90, 1.905, 1.910,               
     a              1.86, 1.93 ,1.95/                                   
       data emn2  /1.44, 1.52, 1.535, 1.65, 1.675, 1.680,               
     a              1.7, 1.71, 1.72, 1.99/                              
       data emca  /1.835, 1.82, 2.04,1.945,2.130 /                      
       dimension jspd(9),jspn(10),jspc(5)                               
       data jspd  /3,1,3,1,5,1,3,5,7/                                   
       data jspn  /1,3,1,1,5,5,3,1,3,7/                                 
       data jspc  /1,3,5,5,7/                                           
       if(irand.gt.0) then                                              
         maxres=ires                                                    
         iofs=ires                                                      
       else                                                             
         ires=-1                                                        
         if(idn.eq.1.or.idn.eq.2) then                                  
           maxres=8+idn                                                 
         else if(idn.eq.5) then                                         
           maxres=5                                                     
         else                                                           
           call errex('bwdist: wrong idn input')                        
         end if                                                         
         iofs=1                                                         
       end if                                                           
       probs=0.                                                         
       do 10 ilop=iofs,maxres                                           
        if(idn.eq.1) then                                               
            emres=emdl(ilop)                                            
            dcrat=dcratd(ibrnch,ilop,em)                                
            jres=jspd(ilop)                                             
        else if(idn.eq.2) then                                          
            dcrat=dcratn(ibrnch,ilop,em)                                
            emres=emn2(ilop)                                            
            jres=jspn(ilop)                                             
        else if(idn.eq.5) then                                          
            dcrat=dcratc(ibrnch,ilop,em)                                
            emres=emca(ilop)                                            
            jres=jspc(ilop)                                             
        end if                                                          
        bwig=(jres+1.)*dcrat*gamres**2/((em-emres)**2+0.25*gamres**2)   
        prob(ilop)=bwig                                                 
        probs=probs+prob(ilop)                                          
   10  continue                                                         
       if(irand.lt.0) then                                              
         if(probs.eq.0.) then                                           
           ires=999                                                     
           dcrat=0.                                                     
           bwig=0.                                                      
           return                                                       
         else                                                           
           zrand=min(ranf(0),0.999)                                     
           do 20 ilop=iofs,maxres                                       
            prob(ilop)=prob(ilop)/probs                                 
            if(zrand.lt.prob(ilop))then                                 
             ires=ilop                                                  
             go to 21                                                   
            else                                                        
             zrand=zrand-prob(ilop)                                     
            end if                                                      
   20     continue                                                      
   21     continue                                                      
         end if                                                         
         if(ires.lt.0) then                                             
            write(8,*)'bwdist',idn,irand,em,ires,ibrnch,dcrat           
            call errex('no resonance found in bwdist?????')             
         end if                                                         
         if(idn.eq.1) then                                              
            emres=emdl(ires)                                            
            dcrat=dcratd(ibrnch,ires,em)                                
            jres=jspd(ires)                                             
         else if(idn.eq.2) then                                         
            dcrat=dcratn(ibrnch,ires,em)                                
            emres=emn2(ires)                                            
            jres=jspn(ires)                                             
         else if(idn.eq.5) then                                         
            dcrat=dcratc(ibrnch,ires,em)                                
            emres=emca(ires)                                            
            jres=jspc(ires)                                             
         end if                                                         
         bwig=(jres+1.)*dcrat*gamres**2/((em-emres)**2+0.25*gamres**2)  
       end if                                                           
      return                                                            
      end                                                               
      function dcr(kmult,kf,em)                                         
       include 'scatter7'                                               
       real*4  dcr                                                      
       real*4 em                                                        
       emr=em                                                           
       dcr=widr(kmult,kf,emr)                                           
      return                                                            
      end                                                               
      function dcrm(kmult,kf,em)                                        
       common /gamrbk/ gamres                                           
       identr=idtmes(kmult,kf)                                          
       call mesres(3,                                                   
     a    identr,0,0,0,0,k1n,k2n,iclcod,isobre,istrin,em,resxs,         
     b                                                        reswav)   
       dcrm=gamres                                                      
      return                                                            
      end                                                               
      function idtmes(kmult,kf)                                         
       kfa=iabs(kf)                                                     
       if(kmult.eq.1) then                                              
         if(kfa.eq.17.or.kfa.eq.23) then                                
           idtmes=11                                                    
         else if(kf.eq.24) then                                         
           idtmes=17                                                    
         else if(kf.eq.25) then                                         
           idtmes=10                                                    
         else if(kfa.eq.18.or.kfa.eq.19) then                           
           idtmes=25                                                    
         else                                                           
           call errex('dcrm1, unknown meson code')                      
         end if                                                         
       else if(kmult.eq.2) then                                         
         if(kfa.eq.17.or.kfa.eq.23) then                                
           idtmes=13                                                    
         else if(kf.eq.24) then                                         
           idtmes=18                                                    
         else if(kf.eq.25) then                                         
           idtmes=20                                                    
         else if(kfa.eq.18.or.kfa.eq.19) then                           
           idtmes=22                                                    
         else if(kfa.eq.27.or.kfa.eq.33) then                           
           idtmes=34                                                    
         else if(kf.eq.34) then                                         
           idtmes=12                                                    
         else if(kf.eq.35) then                                         
           idtmes=20                                                    
         else if(kfa.eq.28.or.kfa.eq.29) then                           
           idtmes=24                                                    
         else                                                           
           call errex('dcrm2, unknown meson code')                      
         end if                                                         
       else if(kmult.eq.3) then                                         
         if(kfa.eq.17.or.kfa.eq.23) then                                
           idtmes=30                                                    
         else if(kf.eq.24) then                                         
           idtmes=14                                                    
         else if(kf.eq.25) then                                         
           idtmes=31                                                    
         else if(kfa.eq.18.or.kfa.eq.19) then                           
           idtmes=26                                                    
         else                                                           
           call errex('dcrm3, unknown meson code')                      
         end if                                                         
       else                                                             
         call errex('dcrm, unknown meson code')                         
       end if                                                           
      return                                                            
      end                                                               
