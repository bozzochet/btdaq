CPAT,LMASS    .                                                         
      subroutine lmass(kdiffr,itag1,itag2,iexpn1,iexpn2,nchain,ipin,    
     a      sigt,sigel,cme,pr,em1,em2,cmeff,ityp1,ityp2,iann,           
     b      kmult1,kmult2,istra1,istra2,imeso1,imeso2,ibry1,ibry2,      
     c      ltyp1,ltyp2,iflvch,idecis,npid1,npid2,ic,pi1sqo,pi2sqo)     
       include 'scatter7'                                               
       include 'partblk7'                                               
       include 'sigmasx'                                                
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       real*4 pmas,pwid,cfr                                             
       common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)     
       real*4 cgk2                                                      
       real*4 cqpfrc                                                    
       character*33 vernum                                              
       character*46 pronam                                              
       logical*1 ttvers                                                 
       data vernum/'version rqmd 2.3  -- 23-sep-96'/                    
       data pronam/'module: l m a s s lets hadrons collide        '/    
       data ttvers/.true./                                              
       if(ttvers) then                                                  
         write(8,*)'--------------- version number report -------------'
         write(8,*)pronam                                               
         write(8,*)vernum                                               
         write(8,*)'---------------------------------------------------'
         ttvers=.false.                                                 
       end if                                                           
       iflvch=0                                                         
       if(cme.lt.em1+em2+ekinmi) then                                   
          ic=0                                                          
          return                                                        
       end if                                                           
       em1o=em1                                                         
       em2o=em2                                                         
       if(ibry1*ibry2.lt.0) then                                        
         if(ida(19).eq.0) then                                          
           snew=cme                                                     
         else                                                           
           snew=2.*sqrt(emnuc**2+pr**2)                                 
         end if                                                         
         sann=sppban(snew)                                              
         if(sigt-sigel.lt.sann-2.) then                                 
           write(8,*)  ityp1,npid1,ityp2,npid2,sigt,sigel,snew          
           write(8,*)  sann                                             
           write(8,*) 'sigt-sigel.lt.sann in lmass???????'              
         end if                                                         
         if(ranf(0).lt.sann/(sigt-sigel)) then                          
           if(ida(49).eq.2) then                                        
             ic=0                                                       
           else                                                         
             if(ida(19).eq.2) then                                      
               cgk2s=0.0                                                
               do 12 i=0,2                                              
                call retcgk(ityp1,npid1,ityp2,npid2,i,cgk2)             
                cgk2s=cgk2s+cgk2                                        
   12          continue                                                 
               if(cgk2s.gt.1.001)                                       
     a           call errex('lmass: wrong sum of cgcoefficients')       
               if(ranf(0).gt.cgk2s) then                                
                 ic=0                                                   
               else                                                     
                 ic=-3                                                  
               end if                                                   
             else                                                       
               ic=-3                                                    
             end if                                                     
           end if                                                       
           return                                                       
         end if                                                         
       end if                                                           
       assign 11 to label                                               
       if(ipin.eq.0.and.                                                
     a            ((ida(26).eq.0.and.cme.le.tblhig).or.                 
     b                           (ida(26).gt.0.and.cme.le.snnlim)))then 
         idecis=0                                                       
       else if(ibry1.eq.1.and.ibry2.eq.1) then                          
         if(ida(26).eq.0.and.cme.le.tblhig)  idecis=0                   
       end if                                                           
       if(switch(6)) then                                               
         srt=cme                                                        
       else                                                             
         srt=cmeff                                                      
       end if                                                           
       call emexc(ltyp1,em1,epdmin,epmin,em1min)                        
       call emexc(ltyp2,em2,etdmin,etmin,em2min)                        
       ltyp1a=iabs(ltyp1)                                               
       ltyp2a=iabs(ltyp2)                                               
       iresex=0                                                         
       ibrod=ibry1*ibry2                                                
       if(ibrod.ge.2) then                                              
         if(istra1+istra2.gt.1) iresex=1                                
       else if(ibrod.lt.0) then                                         
         iresex=1                                                       
       else if(ibry1.eq.0.and.ibry2.eq.0) then                          
       else if(ibrod.eq.0) then                                         
         if(iann.eq.0) iresex=1                                         
       end if                                                           
       pplus=srt                                                        
       pminus=srt                                                       
       if(srt.le.epmin+etmin) then                                      
         ic=0                                                           
         return                                                         
       end if                                                           
       noexc1=0                                                         
       noexc2=0                                                         
       pl=pcmsr(srt,epmin,etmin)                                        
       plsq=pl*pl                                                       
       p1pl=sqrt(epmin**2+plsq)+pl                                      
       p1mi=sqrt(epmin**2+plsq)-pl                                      
       p2mi=sqrt(etmin**2+plsq)+pl                                      
       p2pl=sqrt(etmin**2+plsq)-pl                                      
       if(srt.le.etdmin+epdmin) then                                    
         em1new=em1min                                                  
         em2new=em2min                                                  
         prbas=pcmsr(srt,em1min,em2min)                                 
         go to label                                                    
       end if                                                           
       if(iresex.eq.0) then                                             
         if(srt.gt.epdmin+em2min) then                                  
           prbas=pcmsr(srt,epdmin,em2min)                               
         else                                                           
           prbas=cvalu0                                                 
         end if                                                         
         if(srt.gt.etdmin+em1min) then                                  
           prtes=pcmsr(srt,etdmin,em1min)                               
           prbas=max(prbas,prtes)                                       
         end if                                                         
       else                                                             
         prbas=pcmsr(srt,em1min,em2min)                                 
       end if                                                           
       if(kmult1.ge.0.and.kmult2.ge.0.and.                              
     a                 srt.gt.etdmin+epdmin+ekinmi) then                
         pdfrle=((etdmin+epdmin)/srt)**2                                
         if(ranf(0).lt.pdfrle) then                                     
           if(kmult1.eq.0.or.kmult2.eq.0) then                          
             if(ranf(0).gt.0.5) then                                    
               noexc1=1                                                 
               em1new=em1min                                            
               embase=etdmin                                            
               emcase=em1min                                            
             else                                                       
               noexc2=1                                                 
               em2new=em2min                                            
               embase=epdmin                                            
               emcase=em2min                                            
             end if                                                     
             mtry=0                                                     
   19        mtry=mtry+1                                                
             if(mtry.gt.50)                                             
     a       call errex('lmass19:mtry abort')                           
             emdase=srt-(ekinmi+emcase)                                 
             if(emdase.le.embase) call errex('lmass19 abort')           
             emtes=sqrt(embase**2+ranf(0)*(emdase**2-embase**2))        
             prtes=pcmsr(srt,emcase,embase)                             
             if(ranf(0).gt.                                             
     a               (pcmsr(srt,emcase,emtes)/prtes)**2) go to 19       
             if(noexc1.eq.1) then                                       
               em2new=emtes                                             
             else                                                       
               em1new=emtes                                             
             end if                                                     
             go to 11                                                   
           else                                                         
             noexc1=2                                                   
             noexc2=2                                                   
           end if                                                       
         end if                                                         
       end if                                                           
       go to 60                                                         
   11  continue                                                         
       em1=em1new                                                       
       em2=em2new                                                       
       npink1=0                                                         
       npink2=0                                                         
       ltypa1=ltyp1                                                     
       ltypa2=ltyp2                                                     
       if(ltyp1a.eq.41.or.ltyp1a.eq.42.or.                              
     a       (ltyp1a.ge.61.and.ltyp1a.le.64)) then                      
         if(ltyp1a.le.42) then                                          
           ihad1=2                                                      
         else                                                           
           ihad1=1                                                      
         end if                                                         
         if(em1.ge.epdmin) then                                         
           kmuln1=mbafnd(ihad1,em1)                                     
           if(ltyp1.gt.0.and.idecis.eq.0.and.kmuln1.gt.0)               
     a                                       npink1=-100                
         else                                                           
           kmuln1=0                                                     
         end if                                                         
       else                                                             
         if(ibry1.ne.0) then                                            
           ihad1=idnbar(ltyp1a)                                         
         else                                                           
           ihad1=idnmes(ltyp1a)                                         
         end if                                                         
         if(em1.ge.epdmin) then                                         
           if(ibry1.ne.0) then                                          
             if(ihad1.gt.0) then                                        
               kmuln1=mbafnd(ihad1,em1)                                 
             else                                                       
               call errex('lmass: ibry/ibar dead end')                  
             end if                                                     
           else                                                         
             if(ihad1.gt.0) then                                        
               kmuln1=mmefnd(ihad1,em1)                                 
               if(kmuln1.eq.9999) then                                  
                 kmuln1=0                                               
                 em1=em1min                                             
               end if                                                   
             else                                                       
               call errex('lmass: imes dead end')                       
             end if                                                     
           end if                                                       
         else                                                           
           kmuln1=0                                                     
         end if                                                         
       end if                                                           
       if(kmuln1.eq.-3) kmuln1=-1                                       
       if(ltyp2a.eq.41.or.ltyp2a.eq.42.or.                              
     a       (ltyp2a.ge.61.and.ltyp2a.le.64)) then                      
         if(ltyp2a.le.42) then                                          
           ihad2=2                                                      
         else                                                           
           ihad2=1                                                      
         end if                                                         
         if(em2.ge.etdmin) then                                         
           kmuln2=mbafnd(ihad2,em2)                                     
           if(ltyp2.gt.0.and.idecis.eq.0.and.kmuln2.gt.0)               
     a                                       npink2=-100                
         else                                                           
           kmuln2=0                                                     
         end if                                                         
       else                                                             
         if(ibry2.ne.0) then                                            
           ihad2=idnbar(ltyp2a)                                         
         else                                                           
           ihad2=idnmes(ltyp2a)                                         
         end if                                                         
         if(em2.ge.etdmin) then                                         
           if(ibry2.ne.0) then                                          
             if(ihad2.gt.0) then                                        
               kmuln2=mbafnd(ihad2,em2)                                 
             else                                                       
               call errex('lmass: ibry/ibar dead end')                  
             end if                                                     
           else                                                         
             if(ihad2.gt.0) then                                        
               kmuln2=mmefnd(ihad2,em2)                                 
               if(kmuln2.eq.9999) then                                  
                 kmuln2=0                                               
                 em2=em2min                                             
               end if                                                   
             else                                                       
               call errex('lmass: imes dead end')                       
             end if                                                     
           end if                                                       
         else                                                           
           kmuln2=0                                                     
         end if                                                         
       end if                                                           
       if(kmuln2.eq.-3) kmuln2=-1                                       
       if(ibry1.eq.2.and.itag1.ne.0) then                               
         if(kmuln1.ge.0.and.em1.lt.2.0) then                            
           em1=em1o                                                     
           em2=em2o                                                     
           ic=0                                                         
           return                                                       
         end if                                                         
       end if                                                           
       if(ibry2.eq.2.and.itag2.ne.0) then                               
         if(kmuln2.ge.0.and.em2.lt.2.0) then                            
           em1=em1o                                                     
           em2=em2o                                                     
           ic=0                                                         
           return                                                       
         end if                                                         
       end if                                                           
       if(kmuln1.ge.0.and.kmuln2.ge.0.and.nchain.eq.0                   
     a                                .and.kdiffr.eq.0) then            
         igfix=0                                                        
         jgfix=1                                                        
         mtry=0                                                         
   40    continue                                                       
         mtry=mtry+1                                                    
         if(ibry1.ne.0) then                                            
           minw=0                                                       
           maxw=mxbsta(ihad1)                                           
           if(noexc1.eq.1) then                                         
             maxw=0                                                     
             minw=0                                                     
           end if                                                       
   41      if(embarr(maxw,ihad1)+em2min.gt.srt) then                    
             maxw=maxw-1                                                
             if(maxw.lt.0) then                                         
               maxw=0                                                   
             else                                                       
               go to 41                                                 
             end if                                                     
           end if                                                       
           if(minw.gt.maxw) maxw=minw                                   
           kmula1=kresb(igfix,ihad1,minw,maxw,ltyp1a,em1a)              
         else                                                           
           minw=0                                                       
           maxw=mxmsta(ihad1)                                           
           if(noexc1.eq.1) then                                         
             maxw=0                                                     
             minw=0                                                     
           end if                                                       
   42      if(emmesr(maxw,ihad1)+em2min.gt.srt) then                    
             maxw=maxw-1                                                
             if(maxw.lt.0) then                                         
               maxw=0                                                   
             else                                                       
               go to 42                                                 
             end if                                                     
           end if                                                       
           if(minw.gt.maxw) maxw=minw                                   
           kmula1=kresm(1,igfix,ihad1,minw,maxw,ltyp1a,em1a)            
         end if                                                         
         if(ibry2.ne.0) then                                            
           minw=0                                                       
           maxw=mxbsta(ihad2)                                           
           if(noexc2.eq.1) then                                         
             maxw=0                                                     
             minw=0                                                     
           end if                                                       
   43      if(embarr(maxw,ihad2)+em1min.gt.srt) then                    
             maxw=maxw-1                                                
             if(maxw.lt.0) then                                         
               maxw=0                                                   
             else                                                       
               go to 43                                                 
             end if                                                     
           end if                                                       
           if(minw.gt.maxw) maxw=minw                                   
           kmula2=kresb(igfix,ihad2,minw,maxw,ltyp2a,em2a)              
         else                                                           
           minw=0                                                       
           maxw=mxmsta(ihad2)                                           
           if(noexc2.eq.1) then                                         
             maxw=0                                                     
             minw=0                                                     
           end if                                                       
   44      if(emmesr(maxw,ihad2)+em1min.gt.srt) then                    
             maxw=maxw-1                                                
             if(maxw.lt.0) then                                         
               maxw=0                                                   
             else                                                       
               go to 44                                                 
             end if                                                     
           end if                                                       
           if(minw.gt.maxw) maxw=minw                                   
           kmula2=kresm(1,igfix,ihad2,minw,maxw,ltyp2a,em2a)            
         end if                                                         
         if(mtry.gt.50) then                                            
           kmula1=kmult1                                                
           kmula2=kmult2                                                
           em1a=em1o                                                    
           em2a=em2o                                                    
           go to 45                                                     
         else                                                           
           if(iresex.eq.0) then                                         
             if(kmula1.eq.0.and.kmula2.eq.0) go to 40                   
           end if                                                       
         end if                                                         
         if(em1a+em2a.ge.srt) go to 40                                  
         prnew=pcmsr(srt,em1a,em2a)                                     
         if(ranf(0).gt.(prnew/prbas)**2) go to 40                       
         if(noexc1.eq.2.and.kmula1.eq.0) go to 40                       
         if(noexc2.eq.2.and.kmula2.eq.0) go to 40                       
         if(kmula1.gt.0) then                                           
           emmax=srt-em2a                                               
           em1a=getmas(kmula1,ltyp1a,jgfix,emmax)                       
           if(em1a.lt.0.0) go to 40                                     
         end if                                                         
         if(kmula2.gt.0) then                                           
           emmax=srt-em1a                                               
           em2a=getmas(kmula2,ltyp2a,jgfix,emmax)                       
           if(em2a.lt.0.0) go to 40                                     
         end if                                                         
         if((ltyp1.ne.ltyp2.or.kmult1.ne.kmult2).and.                   
     a       ltypa1.eq.ltypa2.and.kmula1.eq.kmula2) then                
           if(ranf(0).gt.0.5) then                                      
             kmula1=kmult1                                              
             kmula2=kmult2                                              
             em1a=em1o                                                  
             em2a=em2o                                                  
           end if                                                       
         end if                                                         
   45    continue                                                       
         kmuln1=kmula1                                                  
         kmuln2=kmula2                                                  
         em1=em1a                                                       
         em2=em2a                                                       
       end if                                                           
       call oldnew(0,ityp1,npid1,em1,kmuln1,ltyp1)                      
       call oldnew(0,ityp2,npid2,em2,kmuln2,ltyp2)                      
       if(npink1.ne.0) npid1=npid1+npink1                               
       if(npink2.ne.0) npid2=npid2+npink2                               
      return                                                            
   60 continue                                                          
      xplus=1.0                                                         
      xmins=1.0                                                         
      if(itag1.ne.0) then                                               
        if(iabs(iexpn1).gt.10) then                                     
          cqpf1=cqpfrc(1)                                               
          xplus=cqpf1                                                   
        else                                                            
          cqpf1=1.0                                                     
        end if                                                          
      else if(ibry1.eq.0) then                                          
        cqpf1=cqpfrc(1)                                                 
      else                                                              
        cqpf1=cqpfrc(2)                                                 
      end if                                                            
      if(itag2.ne.0) then                                               
        if(iabs(iexpn2).gt.10) then                                     
          cqpf2=cqpfrc(1)                                               
          xmins=cqpf2                                                   
        else                                                            
          cqpf2=1.0                                                     
        end if                                                          
      else if(ibry2.eq.0) then                                          
        cqpf2=cqpfrc(1)                                                 
      else                                                              
        cqpf2=cqpfrc(2)                                                 
      end if                                                            
       ltry=0                                                           
       if(ida(22).eq.3) then                                            
         call diffra(ityp1,npid1,ityp2,npid2,icdl,                      
     a             istra1,istra2,imeso1,imeso2,ibry1,ibry2,             
     b                                             srt,em1,em2)         
         if(icdl.eq.1) then                                             
           kdiffr=1                                                     
         else if(icdl.eq.2) then                                        
           kdiffr=2                                                     
         end if                                                         
       end if                                                           
  201  ltry=ltry+1                                                      
       if(ltry.gt.50) then                                              
         em1new=em1min                                                  
         em2new=em2min                                                  
         go to label                                                    
       end if                                                           
       p1plus=xplus*p1pl                                                
       p1mins=p1mi                                                      
       p2plus=p2pl                                                      
       p2mins=xmins*p2mi                                                
       zprand=ranf(0)                                                   
       zmrand=ranf(0)                                                   
       p1mins=p1mins*(p2mins/p1mins)**zmrand                            
       p2plus=p2plus*(p1plus/p2plus)**zprand                            
       p1plus=pplus-p2plus                                              
       p2mins=pminus-p1mins                                             
       em1new=sqrt(p1plus*p1mins)                                       
       em2new=sqrt(p2plus*p2mins)                                       
       if(em1new.lt.epmin.or.em2new.lt.etmin) go to 201                 
       if(kdiffr.eq.2) then                                             
         pminw=(em1min*em1min)/p1plus                                   
         delpm=p1mins-pminw                                             
         p1mins=pminw                                                   
         p2mins=p2mins+delpm                                            
         em1new=em1min                                                  
         em2new=sqrt(p2plus*p2mins)                                     
         if(em2new.lt.etdmin) go to 201                                 
       else if(kdiffr.eq.1) then                                        
         pplnw=(em2min*em2min)/p2mins                                   
         delpp=p2plus-pplnw                                             
         p2plus=pplnw                                                   
         p1plus=p1plus+delpp                                            
         em2new=em2min                                                  
         em1new=sqrt(p1plus*p1mins)                                     
         if(em1new.lt.epdmin) go to 201                                 
       else if(em1new.lt.epdmin.or.em2new.lt.etdmin) then               
         go to 201                                                      
       end if                                                           
       if(em1new+em2new.ge.srt-ekinmi) go to 201                        
       if(kdiffr.gt.0) then                                             
       else if(ida(44).eq.1) then                                       
         call chains(nchain,p1plus,p1mins,p2plus,p2mins,                
     a                      epdmin,etdmin,pplus,pminus,                 
     b                      em1red,em2red,em1new,em2new,srt)            
       end if                                                           
       go to label                                                      
      end                                                               
      subroutine antibb(ic,ityp1,ityp2,npid1,npid2,em1,em2,srt)         
      include 'scatter7'                                                
      dimension ip(3),iq(3)                                             
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 1.09 -- 14-jul-93'/                     
      data pronam/'module: a n t i b b - - - anti-b b collision  '/     
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      call transl(-1,ityp1,npid1,kl1)                                   
      ichrgs=luchge(kl1)                                                
      call transl(-1,ityp2,npid2,kl2)                                   
      ichrgs=ichrgs+luchge(kl2)                                         
      if(mod(ichrgs,3).ne.0) then                                       
         call errex('no integer charge in antibb')                      
      else                                                              
         ichrgs=ichrgs/3                                                
      end if                                                            
      if(iabs(ichrgs).gt.1) then                                        
         ic=0                                                           
         return                                                         
      end if                                                            
      call luiflv(kl1,ifl1a,ifl1b,ifl1c,ksp)                            
      call luiflv(kl2,ifl2a,ifl2b,ifl2c,ksp)                            
      if(-ifla1.gt.-iflb1) call iswap(ifla1,iflb1)                      
      if(-iflb1.gt.-iflc1) call iswap(iflc1,iflb1)                      
      if(-ifla1.gt.-iflb1) call iswap(ifla1,iflb1)                      
      if(ifla2.gt.iflb2) call iswap(ifla2,iflb2)                        
      if(iflb2.gt.iflc2) call iswap(iflc2,iflb2)                        
      if(ifla2.gt.iflb2) call iswap(ifla2,iflb2)                        
      ssbarf=1.0                                                        
      iqstor=ifl1a                                                      
      ipstor=-iqstor                                                    
      iq(1)=ifl1a                                                       
      iq(2)=ifl1b                                                       
      iq(3)=ifl1c                                                       
      ip(1)=ifl2a                                                       
      ip(2)=ifl2b                                                       
      ip(3)=ifl2c                                                       
      do 10 i=1,3                                                       
      do 10 j=1,3                                                       
       if(ip(i)+iq(j).eq.0) then                                        
         if(ida(19).eq.2.and.iq(j).eq.3) then                           
           ssbarf=ssbarf*0.2                                            
         end if                                                         
         ip(i)=0                                                        
         iq(j)=0                                                        
       end if                                                           
   10 continue                                                          
      izero=0                                                           
      do 20 j=1,3                                                       
       if(ip(j).eq.0) then                                              
         izero=izero+1                                                  
       else                                                             
         ipstor=ip(j)                                                   
       end if                                                           
   20 continue                                                          
      do 30 j=1,3                                                       
       if(iq(j).eq.0) then                                              
         izero=izero+1                                                  
       else                                                             
         iqstor=iq(j)                                                   
       end if                                                           
   30 continue                                                          
      if(ida(19).eq.2.and.ssbarf.lt.1.0) then                           
        if(ranf(0).gt.ssbarf) then                                      
        write(8,*) 'antibb:ssbar annihilation is suppressed',kl1,kl2    
        ic=0                                                            
        return                                                          
        end if                                                          
      end if                                                            
      if(izero.lt.4) then                                               
        write(8,*) 'antibb: annihilation is blocked for ',kl1,kl2       
        ic=0                                                            
        return                                                          
      end if                                                            
      if(iqstor*ipstor.ge.0) then                                       
         write(8,*) kl1,kl2,iqstor,ipstor                               
         call errex('error in antibb encountered')                      
      end if                                                            
      if(ifl1c.gt.0) then                                               
      else                                                              
         iqst=iqstor                                                    
         iqstor=ipstor                                                  
         ipstor=iqst                                                    
      end if                                                            
      if(max(iqstor,-ipstor).le.2) then                                 
            ityp1=8+ ichrgs                                             
            npid1=3                                                     
      else if(min(iqstor,-ipstor).eq.3) then                            
            ityp1=98                                                    
            npid1=35                                                    
      else if(iqstor.eq.1.and.-ipstor.eq.3) then                        
            ityp1=14                                                    
            npid1=2                                                     
      else if(iqstor.eq.2.and.-ipstor.eq.3) then                        
            ityp1=14                                                    
            npid1=1                                                     
      else if(iqstor.eq.3.and.-ipstor.eq.1) then                        
            ityp1=14                                                    
            npid1=-2                                                    
      else if(iqstor.eq.3.and.-ipstor.eq.2) then                        
            ityp1=14                                                    
            npid1=-1                                                    
      else                                                              
         call errex('undefined iqstor/ipstor pair in antibb')           
      end if                                                            
         em1=srt                                                        
         em2=0.                                                         
         ityp2=-9999                                                    
         npid2=0                                                        
      return                                                            
      end                                                               
      subroutine  diffra(ityp1,npid1,ityp2,npid2,icdl,                  
     a             istra1,istra2,imeso1,imeso2,ibry1,ibry2,             
     b                                          srt,em1,em2)            
      include 'scatter7'                                                
      logical difyes                                                    
      real*4 a,b,en,c,d                                                 
      tblfit(plab,a,b,en,c,d)=a+b*plab**en+c*(log(plab))**2+            
     a                              d*log(plab)                         
      sigsd(adum)=0.68*log(0.6+0.1*adum*adum)                           
      ntry=0                                                            
      icdl=0                                                            
      if(max(ityp1,ityp2).gt.2) then                                    
        cv12=12.                                                        
        if(srt.lt.max(cv12,3.*(em1+em2))) return                        
      end if                                                            
      srtact=srt-0.15*(istra1+istra2)                                   
      if(srtact.lt.2.6) then                                            
        return                                                          
      end if                                                            
      sigsdp=sigsd(srtact)                                              
      imeso=imeso1+imeso2                                               
      if(max(ityp1,ityp2).gt.2) then                                    
         sigt12=40.* 0.6666**imeso *                                    
     a          (1.-0.4*istra1/(3.-imeso1))*(1.-0.4*istra2/(3.-imeso2)) 
         sige12=sigt12**1.5 *( 10./40.**1.5 )                           
         sigtp1=40.* 0.6666**imeso1 * (1.-0.4*istra1/(3.-imeso1))       
         sigep1=sigtp1**1.5 *( 10./40.**1.5 )                           
         sigtp2=40.* 0.6666**imeso2 * (1.-0.4*istra2/(3.-imeso2))       
         sigep2=sigtp2**1.5 *( 10./40.**1.5 )                           
         sigsd1=(sige12*sigsdp*sigtp1)/(sigep1*40.)                     
         sigsd2=(sige12*sigsdp*sigtp2)/(sigep2*40.)                     
      else                                                              
         sigsd1=sigsdp                                                  
         sigsd2=sigsd1                                                  
      end if                                                            
      if(srtact.gt.4.) then                                             
        ppr=sqrt(((srtact*srtact                                        
     a                    -2.*emnuc*emnuc)/(2.*emnuc))**2-emnuc*emnuc)  
        sigt=tblfit(ppr,45.60,219.0,-4.23,0.410,-3.41)                  
        sigel=tblfit(ppr,11.20,25.5,-1.12,0.151,-1.62)                  
        sigin=sigt-sigel                                                
      else                                                              
        sigin=30.                                                       
      end if                                                            
      idifrc=0                                                          
    1 continue                                                          
      difyes=.false.                                                    
        if(ranf(0).gt.0.5) then                                         
         if(ranf(0).lt.sigsd1/sigin) then                               
            difyes=.true.                                               
            idifrc=1                                                    
         else if(ranf(0).lt. sigsd2/sigin) then                         
            difyes=.true.                                               
            idifrc=2                                                    
         end if                                                         
        else                                                            
         if(ranf(0).lt. sigsd2/sigin) then                              
            difyes=.true.                                               
            idifrc=2                                                    
         else if(ranf(0).lt. sigsd1/sigin) then                         
            difyes=.true.                                               
            idifrc=1                                                    
         end if                                                         
        end if                                                          
        if(difyes) then                                                 
          if(idifrc.eq.1) then                                          
             icdl=2                                                     
          else if(idifrc.eq.2) then                                     
             icdl=1                                                     
          end if                                                        
          if(ida(46).eq.1) then                                         
            ntry=ntry+1                                                 
            icdl=0                                                      
            if(ntry.gt.50) then                                         
               call errex('ntry>50 in diffra')                          
            end if                                                      
            go to 1                                                     
          end if                                                        
        else                                                            
          icdl=3                                                        
        end if                                                          
      return                                                            
      end                                                               
      function sppban(srt)                                              
        include 'scatter7'                                              
      data pbarpa,pbarpb,sig0n/0.05,0.6,120. /                          
      srt0=2.*emnuc                                                     
      sppban=sig0n*(srt0/srt)**2*(pbarpb+(pbarpa*srt0)**2/              
     a                           ((srt**2-srt0**2)**2+(pbarpa*srt0)**2))
      return                                                            
      end                                                               
      subroutine chains(nchain,p1plus,p1mins,p2plus,p2mins,             
     a                  epdmin,etdmin,pplus,pminus,                     
     b                  em1red,em2red,em1new,em2new,srt)                
        include 'scatter7'                                              
        common /seabk/kchain,iofcha,isea(18),psea(0:3,18),isoan,iqqst,  
     a                emsmb                                             
        dimension sigch(10)                                             
        data emsect / 0.28/                                             
        real*8 srtact                                                   
        real*8 achain                                                   
        save gamres                                                     
        data  emcstq,emgex,emscal,acoef /0.06,0.06,0.8,2.06/            
        data ibegin /0/                                                 
        if(ibegin.eq.0) then                                            
          ibegin=1                                                      
          gamres=acoef*pcmsr(emscal,empion,empion)                      
        endif                                                           
        nchain=0                                                        
        if(srt.le.epdmin+etdmin+0.8) return                             
        if(em1new.le.epdmin.or.em2new.le.etdmin) return                 
        srtact=srt                                                      
        do 8 n=1,10                                                     
    8   sigch(n)=0.                                                     
        do 10 k=1,30                                                    
         ak=achain(k,srtact)                                            
         do 9 n=1,10                                                    
          if(k.ge.n) then                                               
            fack=float(k)                                               
            do 6 km=k-1,k-(n-1),-1                                      
    6        fack=fack*km                                               
             facn=1.                                                    
             do 7 nm=2,n                                                
    7         facn=facn*nm                                              
              sigch(n)=sigch(n)+(-1)**(n+1)*fack/facn*ak                
          end if                                                        
    9    continue                                                       
   10   continue                                                        
        st=0.                                                           
        do 12 n=1,10                                                    
         st=st+sigch(n)                                                 
   12   continue                                                        
        schm1=0.                                                        
        schp1=0.                                                        
        zrand=ranf(0)                                                   
        nstor=0                                                         
        do 13 n=1,10                                                    
         sn=sigch(n)/st                                                 
         schp1=schp1+sn                                                 
         if(zrand.ge.schm1.and.zrand.lt.schp1) then                     
            nstor=n                                                     
         end if                                                         
         schm1=schp1                                                    
   13   continue                                                        
        if(nstor.le.1) return                                           
        do 18 n=1,18                                                    
   18   isea(n)=0                                                       
        mtry=0                                                          
        emsum=0.                                                        
        em1n=em1new                                                     
        em2n=em2new                                                     
        nmax=1                                                          
        nno=0                                                           
        do 14 n=2,nstor                                                 
         ispr=0                                                         
         do 16 kcross=1,2                                               
          if(kcross.eq.1) then                                          
            nact=n-nno-1                                                
          else                                                          
            nact=n-nno+8                                                
          end if                                                        
          ltry=0                                                        
  141     continue                                                      
          ltry=ltry+1                                                   
          if(mtry.gt.20000) then                                        
            call errex('chains: mtry abort')                            
          end if                                                        
          zprand=ranf(0)                                                
          zmrand=ranf(0)                                                
          ipowsq=5                                                      
          ipowg=5                                                       
          if(kcross.eq.2) then                                          
            pplow=emgex*pplus/srt                                       
            pmlow=emcstq*pminus/srt                                     
            ipow1=ipowg                                                 
            ipow2=ipowsq                                                
          else                                                          
            pplow=emcstq*pplus/srt                                      
            pmlow=emgex*pminus/srt                                      
            ipow1=ipowsq                                                
            ipow2=ipowg                                                 
          end if                                                        
          if(p1plus.le.pplow.or.p2mins.le.pmlow) then                   
            emsch=0.0                                                   
          else                                                          
            pp=pplow*(p1plus/pplow)**zprand                             
            if(ranf(0).gt.((p1plus-pp)/(p1plus-pplow))**ipow1)then      
              ltry=ltry-1                                               
              mtry=mtry+1                                               
              go to 141                                                 
            end if                                                      
            pm=pmlow*(p2mins/pmlow)**zmrand                             
            if(ranf(0).gt.((p2mins-pm)/(p2mins-pmlow))**ipow2)then      
              ltry=ltry-1                                               
              mtry=mtry+1                                               
              go to 141                                                 
            end if                                                      
            emsch=sqrt(pp*pm)                                           
          end if                                                        
          if(emsch.gt.emsect) then                                      
            if(emsch.lt.emscal) then                                    
              gamact=acoef*pcmsr(emsch,empion,empion)                   
              shpbw=0.25*gamres*gamact/                                 
     a                         ((emsch-emscal)**2+0.25*gamact**2)       
              if(ranf(0).gt.shpbw) then                                 
                ltry=51                                                 
              end if                                                    
            end if                                                      
          else                                                          
            ltry=51                                                     
          end if                                                        
          if(ltry.eq.51) then                                           
            pp=0.0                                                      
            pm=0.0                                                      
            emsch=0.0                                                   
          end if                                                        
          dpm=pp*p1mins/(p1plus-pp)                                     
          dpp=pm*p2plus/(p2mins-pm)                                     
          em1nsq=max(cvalu0,p1plus-(pp+dpp))*(p1mins+dpm)               
          em2nsq=(p2plus+dpp)*max(cvalu0,p2mins-(pm+dpm))               
          if(emsch.gt.emsect.and.em1nsq.gt.epdmin**2                    
     a                       .and.em2nsq.gt.etdmin**2) then             
            em1n=sqrt(em1nsq)                                           
            em2n=sqrt(em2nsq)                                           
            p1plus=p1plus-(pp+dpp)                                      
            p1mins=p1mins+dpm                                           
            p2mins=p2mins-(pm+dpm)                                      
            p2plus=p2plus+dpp                                           
            emsum=emsum+emsch                                           
            nchain=nchain+1                                             
            isea(nact)=1                                                
            psea(0,nact)=pp                                             
            psea(3,nact)=pm                                             
          else                                                          
            if(ltry.lt.50) go to 141                                    
            ispr=ispr+1                                                 
          end if                                                        
  16     continue                                                       
         if(ispr.eq.2) nno=nno+1                                        
         nmax=n-nno                                                     
   14    continue                                                       
        emsum=emsum+em1n+em2n                                           
        if(emsum.gt.srt) then                                           
          write(8,*) em1n,em2n,emsum,srt                                
          call errex('emsum>srt in chains')                             
        end if                                                          
        em1red=sqrt(p1plus*p1mins)                                      
        em2red=sqrt(p2plus*p2mins)                                      
        do 19 n=2,nmax                                                  
         if(isea(n-1).ne.0) then                                        
           p1mins=p1mins+psea(3,n-1)                                    
           p1plus=p1plus+psea(0,n-1)                                    
         end if                                                         
         if(isea(n+8).ne.0) then                                        
           p2plus=p2plus+psea(3,n+8)                                    
           p2mins=p2mins+psea(0,n+8)                                    
         end if                                                         
   19   continue                                                        
        do 17 n=2,nmax                                                  
         if(isea(n-1).ne.0) then                                        
          psea(0,n-1)=psea(0,n-1)/p1plus                                
          psea(3,n-1)=psea(3,n-1)/p1mins                                
         end if                                                         
         if(isea(n+8).ne.0) then                                        
          psea(0,n+8)=psea(0,n+8)/p2mins                                
          psea(3,n+8)=psea(3,n+8)/p2plus                                
         end if                                                         
   17   continue                                                        
        em1n=sqrt(p1plus*p1mins)                                        
        em2n=sqrt(p2plus*p2mins)                                        
        if(em1n.le.epdmin.or.em2n.le.etdmin) then                       
          nchain=0                                                      
        else if(srt-(em1n+em2n).lt.ekinmi) then                         
          write(8,*) 'chains, no kin.energy',srt,nchain                 
          nchain=0                                                      
        else                                                            
          em1new=em1n                                                   
          em2new=em2n                                                   
        end if                                                          
        if(em1new+em2new.gt.srt) then                                   
          write(8,*) em1new,em2new,srt                                  
          call errex('em1new+em2new>srt in chains')                     
        end if                                                          
      return                                                            
      end                                                               
      function achain(k,srt)                                            
       implicit real*8(a-h,o-z)                                         
       data  pi /3.141526/                                              
       data vkapp,c1,c2, c3,c4 /                                        
     a               0.5, 32.86,0.093,1.07,0.1 /                        
       if(k.lt.1) then                                                  
          write(8,*) 'k<1 not allowed in a(k)'                          
          call errex('wrong k in achain')                               
       end if                                                           
       fack=1.                                                          
       do 4 km=2,k                                                      
    4  fack=fack*km                                                     
       ak=c1*(srt*srt)**c2*                                             
     a ((-(1.+vkapp)*c1*(srt*srt)**c2)/(4.*pi*(c3+c4*log(srt*srt))))    
     b                                                       **(k-1)/   
     c                                   k/fack                         
       achain=ak                                                        
      return                                                            
      end                                                               
      subroutine emexc(kf,em1,epdmin,epmin,em1min)                      
       include 'scatter7'                                               
       kfa=iabs(kf)                                                     
       if(kfa.eq.0.or.kfa.gt.70)  call errex('wrong kfa in emexc')      
       em1min=getmas(0,kfa,1,-1.0)                                      
       if(kfa.eq.17.or.kfa.eq.23.or.kfa.eq.24) then                     
         epdmin=emrhdc                                                  
         epmin=epdmin-dltmas                                            
       else if(kfa.eq.18.or.kfa.eq.19.or.kfa.eq.25) then                
         epdmin=emkdec                                                  
         epmin=epdmin-dltmas                                            
       else                                                             
         if(em1min.gt.em1) em1min=em1                                   
         epdmin=em1min+dltmas                                           
         epmin=em1min                                                   
       end if                                                           
      return                                                            
      end                                                               
