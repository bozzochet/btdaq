CPAT,RESFOR   .                                                         
      subroutine absorb(ipi,id,i3,i4,nop,itagdl,jesant,iann,            
     a                      iqqst,isoan,ityp1,npid1,ityp2,npid2,        
     b                      itag1,iqid1,itag2,iqid2,ichanl,icltyp,      
     c                      ltype1,kmult1,ltype2,kmult2,                
     c                      sigqsq,xstot,em1,em2,prel,pi1sq,pi2sq,cme)  
       include 'scatter7'                                               
       include 'partblk7'                                               
       include 'sigmasx'                                                
       dimension prob(3)                                                
       dimension sigch(5)                                               
       common /xspibk/ xspin1,xspin3,wnorm1,wnorm3,pow12,pow32,         
     a                xsbgl1,xsbgl3,xs1218,xs32pn,xs12pn                
       real*4 cgk212,cgk232,cgk2d                                       
       real*4  emstr,prbas,embaa,emmea,embab,emmeb,ptx,pty,emda,emdb    
       real*4  regsup,prcms,proia,proib,prlpi4,prspi4,pcms4,pr0r,spw    
       common /resbk/ resfrm                                            
       data ikhyp, iahyp /0,0/                                          
        parameter (ixsect=5)                                            
      logical  potyes                                                   
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version  rqmd 2.3 -- 23-sep-96'/                     
      data pronam/'a b s o r b  -- resonance formation '/               
      data ttvers/.true./                                               
       if(ttvers) then                                                  
         write(8,*)'--------------- version number report -------------'
         write(8,*)pronam                                               
         write(8,*)vernum                                               
         write(8,*)'---------------------------------------------------'
         ttvers=.false.                                                 
         resfrm=0.                                                      
         if(wnorm1.eq.0.0.or.wnorm3.eq.0.0) then                        
             call errex('absorb abort: wnorm not initialized')          
         end if                                                         
       end if                                                           
       jome=0                                                           
       jcas=0                                                           
       jhyp=0                                                           
       jpin=0                                                           
       jpins=0                                                          
       jnds=0                                                           
       if(iann.eq.1) then                                               
         jnds=1                                                         
         mflt=0                                                         
       else if(iann.eq.2) then                                          
         jhyp=1                                                         
         mflt=1                                                         
       else if(iann.eq.3) then                                          
         jcas=1                                                         
         mflt=2                                                         
       else if(iann.eq.4) then                                          
         jome=1                                                         
         mflt=3                                                         
       else                                                             
        call errex('absorb:iann.ne.1-4 not  implemented')               
       end if                                                           
       if(itag1.ne.0.or.itag2.ne.0) then                                
         ichanl=0                                                       
         return                                                         
       end if                                                           
       kestim=0                                                         
       ida53=ida(53)                                                    
       ida55=ida(55)                                                    
       kl1=ltype1                                                       
       kl2=ltype2                                                       
       kmul1=kmult1                                                     
       if(kmul1.eq.-1) kmul1=9                                          
       kfinms=isign(1,kl1)*(iabs(kl1)+100*kmul1)                        
       kmul2=kmult2                                                     
       if(kmul2.eq.-1) kmul2=14                                         
       kfinba=kl2+100*kmul2                                             
       if(kmul1.eq.9.or.kmul2.eq.14) then                               
         irehev=1                                                       
       else                                                             
         irehev=0                                                       
       end if                                                           
       mfld=0                                                           
       if(ltype1.eq.24.or.ltype1.eq.25) then                            
         if(imspin(kfinms)/10.eq.3) mfld=3                              
       end if                                                           
       if(itag1.ne.0.or.itag2.ne.0) then                                
         kswi=2                                                         
       else                                                             
         kswi=1                                                         
       end if                                                           
       imesp=iabs(ltype1)+100*kmul1                                     
       ibasp=kl2+100*kmul2                                              
       call retcgk(99,ltype1,j1,m1,-1,cgk2d)                            
       call retcgk(99,ltype2,j2,m2,-1,cgk2d)                            
       m=m1+m2                                                          
       jtm1=m                                                           
       jt2=0                                                            
       jtm2=0                                                           
       if(cme.gt.1.5.or.iann.gt.1.or.em1.gt.0.2.or.                     
     a                         itag1.ne.0.or.itag2.ne.0) then           
         call luiflv(kl1,ifla1,iflb1,iflc1,ksp)                         
         call luiflv(kl2,ifla2,iflb2,iflc2,ksp)                         
         if(iflc1*iflc2.ne.0) then                                      
           write(8,*) ltype1,kmult1,ltype2,kmult2                       
           call errex('absorb:mb annihilation, but wrong types')        
         end if                                                         
         ivac=0                                                         
         if(iflc1.eq.0)then                                             
           if(ifla1+iflb1.eq.0) then                                    
             ivac=1                                                     
             ifla3=ifla2                                                
             iflb3=iflb2                                                
             iflc3=iflc2                                                
             zrand=ranf(0)                                              
             if(zrand.lt.0.33333) then                                  
               call iswap(ifla3,iflb3)                                  
             else if(zrand.lt.0.66667) then                             
               call iswap(ifla3,iflc3)                                  
             else                                                       
               call iswap(iflb3,iflc3)                                  
             end if                                                     
             if(ranf(0).lt.0.5) call iswap(ifla3,iflb3)                 
             if(iabs(ifla1).ne.3.and.ifla3.eq.3) then                   
               if(iann.le.1) then                                       
                 call iswap(ifla3,iflc3)                                
               else if(iann.eq.2) then                                  
                 if(iflc3.lt.3) then                                    
                   call iswap(ifla3,iflc3)                              
                 else                                                   
                   call iswap(ifla3,iflb3)                              
                 end if                                                 
               end if                                                   
             else if(iabs(ifla1).eq.3.and.ifla3.ne.3) then              
               if(iann.eq.1.or.iann.eq.2) then                          
                 if(iflc3.eq.3) then                                    
                   call iswap(ifla3,iflc3)                              
                 else                                                   
                   call iswap(ifla3,iflb3)                              
                 end if                                                 
               end if                                                   
             end if                                                     
           else                                                         
             if(ifla1*ifla2.gt.0) then                                  
               call iswap(ifla1,iflb1)                                  
             end if                                                     
             if(ifla1.eq.-ifla2) then                                   
               ifla3=iflb1                                              
               iflb3=iflb2                                              
               iflc3=iflc2                                              
             else if(ifla1.eq.-iflb2) then                              
               ifla3=iflb1                                              
               iflb3=ifla2                                              
               iflc3=iflc2                                              
             else if(ifla1.eq.-iflc2) then                              
               ifla3=iflb1                                              
               iflb3=ifla2                                              
               iflc3=iflb2                                              
             else                                                       
               ichanl=0                                                 
               write(8,*) ltype1,kmult1,ltype2,kmult2,                  
     a                      issn(ipi),issn(id)                          
               write(8,*) 'absorb:mb-ann, no common q/qbar-pair'        
               return                                                   
             end if                                                     
           end if                                                       
         else                                                           
           call errex('absorb: meson not in first position')            
         end if                                                         
         if(itag1.eq.1.or.itag2.eq.1) then                              
           if(iqid1.gt.0)                                               
     a     call errex('absorb: no const.antiquark in meson')            
           kcoun=0                                                      
           krejec=0                                                     
           iflban=ifla2+iflb2+iflc2-iflb3-iflc3                         
           if(itag1.eq.1) then                                          
             irid2=0                                                    
             if(ivac.eq.1) then                                         
               if(ifla3.ne.-iqid1) krejec=1                             
             end if                                                     
           else                                                         
             irid2=10*(ifla1+iflb1+iflban)                              
           end if                                                       
           if(itag2.eq.1) then                                          
             iflcq1=iqid2/10                                            
             iflcq2=mod(iqid2,10)                                       
             if(iflcq1.eq.0) then                                       
               if(itag1.eq.1) then                                      
                 krejec=1                                               
               end if                                                   
               if(ifla2.eq.iflcq2) then                                 
                 iflbvq=iflb2                                           
                 iflbwq=iflc2                                           
               else if(iflb2.eq.iflcq2) then                            
                 iflbvq=ifla2                                           
                 iflbwq=iflc2                                           
               else if(iflc2.eq.iflcq2) then                            
                 iflbvq=ifla2                                           
                 iflbwq=iflb2                                           
               else                                                     
                 call errex('absorb:where is the cq in qqq?')           
               end if                                                   
               iqid2=irid2/10                                           
             else                                                       
               iflbvq=ifla2+iflb2+iflc2-iflcq1-iflcq2                   
               if(iflbvq.ne.ifla2.and.iflbvq.ne.iflb2                   
     a                            .and.iflbvq.ne.iflc2) then            
                 call errex('absorb:what is the vq in qqq?')            
               end if                                                   
               iflbwq=0                                                 
               if(iflban.eq.iflcq1) then                                
                 iqid2=irid2+iflcq2                                     
               else                                                     
                 iqid2=irid2+iflcq1                                     
               end if                                                   
             end if                                                     
             if(iflbvq.eq.iflbwq) then                                  
               dkre=2.                                                  
             else                                                       
               dkre=1.                                                  
             end if                                                     
             if(iflbvq.eq.iflban.or.iflbwq.eq.iflban) then              
               if(ifla2.eq.iflban) kcoun=1                              
               if(iflb2.eq.iflban) kcoun=kcoun+1                        
               if(iflc2.eq.iflban) kcoun=kcoun+1                        
               if(kcoun.eq.0)                                           
     a         call errex('absorb:annihilated quark not recovered')     
               if(ranf(0).gt.(kcoun-dkre)/float(kcoun)) krejec=1        
             end if                                                     
           end if                                                       
           if(krejec.eq.1) then                                         
             ichanl=0                                                   
             return                                                     
           end if                                                       
         end if                                                         
       end if                                                           
       if(jnds.eq.1) then                                               
         if(ityp1.ge.7.and.ityp1.le.9) then                             
           if(npid1.eq.0) then                                          
             if(ityp2.le.2) then                                        
               jpin=1                                                   
               jnds=0                                                   
             else if(ityp2.ge.10.and.ityp2.le.11) then                  
               jpins=1                                                  
               jnds=0                                                   
             end if                                                     
           end if                                                       
           if(jpin.eq.1) then                                           
             if(icltyp.eq.22.or.icltyp.eq.38) then                      
                imixt=1                                                 
                rat3=1.                                                 
                rat1=0.                                                 
             else if(icltyp.eq.37.or.icltyp.eq.23) then                 
                imixt=2                                                 
                rat3=0.333                                              
                rat1=0.667                                              
             elseif(icltyp.eq.29.or.icltyp.eq.30) then                  
                imixt=3                                                 
                rat3=0.667                                              
                rat1=0.333                                              
             end if                                                     
             if((ida(4).eq.1.or.ida(4).eq.2).and.                       
     a                                 jpin.eq.1) then                  
               call piweg(ipi,id,i3,i4,nop,imixt,                       
     a                              itag1,itag2,ichanl,cme,icltyp)      
               if(ida(4).eq.2) itagdl=1                                 
               if(ichanl.eq.0.or.nop.ne.1) return                       
             end if                                                     
             ichrgp=ityp1-8                                             
             ichrgb=ityp2-1                                             
           end if                                                       
         end if                                                         
       end if                                                           
       srt=cme                                                          
       if(em1+em2.ge.srt) then                                          
         ichanl=0                                                       
         return                                                         
       end if                                                           
       pcms=prel                                                        
       pcms4=pcms                                                       
       emstr=srt                                                        
       em(ipi)=0.0                                                      
       em(id)=srt                                                       
       do  25 i=0,3                                                     
        p(i,id)=p(i,id)+p(i,ipi)                                        
   25  continue                                                         
       ityp(ipi)=-9999                                                  
       decay(ipi)=1.e+35                                                
       em2nur=2.8+mflt*0.15                                             
       if(jpin+jpins+jnds+jhyp+jcas+jome.ne.1)                          
     a       call errex('error in absorb:wrong ingoing particles')      
       if(jome.eq.1) then                                               
       else if(jcas.eq.1) then                                          
         if(srt.le.2.0.and.(itag1.ne.0.or.itag2.ne.0)) then             
           ichanl=0                                                     
           return                                                       
         end if                                                         
         call casca(srt,pcms,em2,em1,sigstr,sigres,xs1530,              
     a                   inchan,ltype2,kmult2,ltype1,kmult1)            
         if(sigres+sigstr.eq.cvalu0) then                               
           ichanl=0                                                     
           return                                                       
         end if                                                         
         emcasc=1.318                                                   
         if(srt.le.emcasc+empion) then                                  
           ichanl=0                                                     
           return                                                       
         end if                                                         
         xsann=xsesti(5,srt,pcms,strdum,iredu,mdum,icodum)              
         if(ida55.ne.0.and.kswi.eq.1.and.iabs(m).eq.1                   
     a                            .and.sigstr.gt.cvalu0) then           
           mbfix=0                                                      
           ichoba=5                                                     
           idntre=93                                                    
           fnlrga=0.                                                    
           ltyper=(idntre-m)/2                                          
           call bele(iflfa,iflfb,ltyper)                                
           prbas=pcmsr(srt,emcasc,empion)                               
           emda=emcasc                                                  
           emdb=empion                                                  
           jt1=1                                                        
           jt1new=2                                                     
           jt2new=1                                                     
           idntbc=93                                                    
           itag=0                                                       
           call isocgk(jtm1,jt1,jtm2,jt2,jt1new,jt2new,                 
     a                                       jtm1nw,jtm2nw,itag)        
           klmold=jtm1nw/2*17+23*(2-iabs(jtm1nw))/2                     
           klbold=(idntbc-jtm2nw)/2                                     
           ktry=0                                                       
  326      ktry=ktry+1                                                  
           f1ia=1.0                                                     
           f2ia=1.0                                                     
           kfbaa=0                                                      
           if(ranf(0).gt.em2nur/srt) then                               
             imany=1                                                    
           else                                                         
             call bresdc(imany,                                         
     a              iflfb,iflfa,ichoba,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbaa,kfmea,kmltba,kmltma,                          
     d              iflb,ifla,ifl3,kdfr,embaa,emmea,ptx,pty)            
           end if                                                       
           if(imany.eq.0) then                                          
             proia=prcms(emstr,embaa,emmea)                             
             kfbria=kfbaa+100*kmltba                                    
             kfmsia=isign(1,kfmea)*(iabs(kfmea)+100*kmltma)             
             f1ia=                                                      
     a         regsup(ida55,kfmsia,kfbria,klmold,klbold,ifl3,kex,       
     b                          prbas,proia,emstr,pr0r,spw,iscd)        
           end if                                                       
           enlrga=1.                                                    
           if(kfbaa.ne.0) then                                          
             if(irehev.eq.1) then                                       
               f2ia=0.0                                                 
             else                                                       
               f2ia=                                                    
     a         regsup(ida55,kfinms,kfinba,kfmsia,kfbria,mfld,kex,       
     b                         pcms4,proia,emstr,pr0r,spw,iscd)         
             end if                                                     
             enlrga=f2ia/f1ia                                           
           end if                                                       
           fnlrga=fnlrga+enlrga                                         
           if(ktry.eq.1) then                                           
             sigupl=1.2*(1.+(prbas**2/0.64))*sigstr+sigres              
             if(sigqsq.gt.sigupl) kestim=2                              
             if(sigqsq.le.sigres) kestim=1                              
           end if                                                       
           if(kestim.ne.1.and.ktry.le.ixsect)  go to 326                
           fnlrg=fnlrga/ktry                                            
           sigstr=fnlrg*sigstr                                          
         end if                                                         
         sigmb=sigres+sigstr                                            
         if(xsann.lt.sigmb) then                                        
           write(8,*) 'warning: absorb326:',xsann,sigmb                 
           write(8,*) ltype1,kmult1,ltype2,kmult2                       
           write(8,*) 'srt=',srt,pcms,sigstr,xsann                      
         end if                                                         
         if(sigqsq.gt.sigmb) then                                       
           ichanl=0                                                     
           return                                                       
         end if                                                         
         if(iabs(m).ne.1)                                               
     a   call errex('absorb:cascade w wrong i3 component')              
         if(ranf(0).gt.sigres/sigmb) then                               
           ktry=0                                                       
   46      continue                                                     
           ktry=ktry+1                                                  
           if(ktry.gt.20) call errex('absorb 46-abort')                 
           mbaf=mbafnd(7,srt)                                           
           if(mbaf.gt.0) then                                           
             itypb=84+mbaf                                              
             if(itypb.eq.88) go to 46                                   
             npidb=22+(93-m)/2                                          
           else                                                         
             itypb=98                                                   
             npidb=(93-m)/2                                             
           end if                                                       
           jesant=1                                                     
           isoan=10                                                     
         else if(xs1530.ne.cvalu0                                       
     a                   .and.ranf(0).le.xs1530/sigres) then            
           itypb=99                                                     
           npidb=(137-m)/2                                              
         else                                                           
           call bwdist(5,-1,srt,km,inchan,dcrat,bwig)                   
           if(km.le.0.or.km.gt.5) then                                  
             call errex('absorb: wrong random km from bwdist')          
           end if                                                       
           if(km.le.4) then                                             
             itypb=85+km                                                
             if(itypb.eq.89) itypb=93                                   
             npidb=(93-m)/2                                             
           else                                                         
             itypb=88                                                   
             npidb=22+(93-m)/2                                          
           end if                                                       
         end if                                                         
         ityp(id)=itypb                                                 
         npidec(id)=npidb                                               
         if(kestim.eq.2)                                                
     a     write(8,*) '326:srt=',srt,ltype1,ltype2,em1,em2              
       else if(jhyp.eq.1) then                                          
         srtpar=2.50                                                    
         if(srt.le.2.0.and.(itag1.ne.0.or.itag2.ne.0)) then             
           ichanl=0                                                     
           return                                                       
         end if                                                         
         if(itag1.ne.0.or.itag2.ne.0) then                              
           isoan=10                                                     
         else                                                           
     a   if((icltyp.eq.92.or.icltyp.eq.93).and.em1.lt.0.5) then         
           isoan=5                                                      
         else if((icltyp.ge.85.and.icltyp.le.87).or.                    
     a                  (icltyp.ge.112.and.icltyp.le.114)) then         
           if(em1.lt.0.2.and.em2.le.1.2.and.srt.lt.srtpar) then         
             isoan=6                                                    
           else                                                         
             isoan=10                                                   
           end if                                                       
         else                                                           
           isoan=10                                                     
         end if                                                         
         if(srt.le.emnuc+emkap) then                                    
           ichanl=0                                                     
           return                                                       
         end if                                                         
         if(isoan.ne.5.and.srt.le.1.50) then                            
           ichanl=0                                                     
           return                                                       
         end if                                                         
         prkan=pcmsr(srt,emnuc,emkap)                                   
         call cross1(1,srt,prkan,93,xstkmp,emnuc,emkap,2,14,0,-2)       
         call cross1(1,srt,prkan,92,xstkmn,emnuc,emkap,1,14,0,-2)       
         call cross1(2,srt,prkan,93,xsekmp,emnuc,emkap,2,14,0,-2)       
         call cross1(2,srt,prkan,92,xsekmn,emnuc,emkap,1,14,0,-2)       
         rlimit=min(cvalu1,(4.0/srt**2)**da(7))                         
         sigkmp=(xstkmp-xsekmp)*rlimit                                  
         sigkmn=(xstkmn-xsekmn)*rlimit                                  
         delxsp=0.0                                                     
         delxsm=0.0                                                     
         do 27 itot=0,2,2                                               
         do 27 inum=1,3                                                 
          sigcha=piynkb(1,-1,1,1,itot,inum,srt,prkan)                   
          if(srt.ge.srtpar) sigcha=cvalu0                               
          delxsp=delxsp+sigcha                                          
          if(inum.ne.2) delxsm=delxsm+2.*sigcha                         
   27    continue                                                       
         sigkmp=sigkmp-delxsp                                           
         sigkmn=sigkmn-delxsm                                           
         sig2=sigkmn                                                    
         sig0=2.*sigkmp-sigkmn                                          
         if(sig2.lt.0.) sig2=0.                                         
         if(sig0.lt.0.) sig0=0.                                         
         sigsum=0.0                                                     
         if(isoan.ne.10) then                                           
           do 28 inu=1,5                                                
   28      sigch(inu)=0.0                                               
           do 29 itot=0,2,2                                             
           do 29 inu=1,5                                                
            inum=inu                                                    
            if(isoan.eq.6) inum=-inum                                   
            if(inu.le.4) then                                           
              sigcha=piynkb(j1,m1,j2,m2,itot,inum,srt,pcms)             
              if(inu.eq.4) sigcha=2.*sigcha                             
              if(srt.ge.srtpar.and.inu.le.3) then                       
                sigcha=cvalu0                                           
              end if                                                    
            else if(isoan.eq.5.and.itot.eq.2.and.                       
     a                      iabs(m1+m2).eq.2) then                      
              if(srt.gt.2.0) then                                       
                sigcha=xsekmn*(2./srt)**5                               
              else                                                      
                sigcha=xsekmn                                           
              end if                                                    
            else                                                        
              sigcha=cvalu0                                             
            end if                                                      
            if(sigcha.eq.cvalu0) go to 29                               
            sigch(inu)=sigcha                                           
            sigsum=sigsum+sigcha                                        
   29      continue                                                     
         end if                                                         
         if(isoan.ne.5) then                                            
           mbfix=-5                                                     
           prbas=prkan                                                  
           emda=emnuc                                                   
           emdb=emkap                                                   
           ichoba=3                                                     
           jt1=2                                                        
           jt1new=1                                                     
           jt2new=1                                                     
           idntmc=-37                                                   
           idntbc=83                                                    
           ltyper=(88-m)/2                                              
           fnlrga=0.                                                    
           ichobb=4                                                     
           fnlrgb=0.                                                    
           if(srt.le.1.5) then                                          
             allow=0.                                                   
           else                                                         
             call bele(iflfa,iflfb,ltyper)                              
             itag=0                                                     
             call isocgk(jtm1,jt1,jtm2,jt2,jt1new,jt2new,               
     a                                       jtm1nw,jtm2nw,itag)        
             klmold=(idntmc-jtm1nw)/2                                   
             klbold=(idntbc-jtm2nw)/2                                   
             ktry=0                                                     
   26        ktry=ktry+1                                                
             f1ia=1.0                                                   
             f2ia=1.0                                                   
             kfbaa=0                                                    
             f1ib=1.0                                                   
             f2ib=1.0                                                   
             f2sig0=0.0                                                 
             f2kao0=0.0                                                 
             f2sig2=0.0                                                 
             f2lam2=0.0                                                 
             f2kao2=0.0                                                 
             kfbab=0                                                    
             if(ida55.ne.0.and.kswi.eq.1) then                          
               if(m.eq.0) then                                          
                 if(ranf(0).gt.em2nur/srt) then                         
                   imana=1                                              
                   imanb=1                                              
                 else                                                   
                   call bresdc(imana,                                   
     a              iflfb,iflfa,ichoba,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbaa,kfmea,kmltba,kmltma,                          
     d              iflb,ifla,ifl3,kdfr,embaa,emmea,ptx,pty)            
                   call bresdc(imanb,                                   
     a              iflfb,iflfa,ichobb,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbab,kfmeb,kmltbb,kmltmb,                          
     d              iflb,ifla,jfl3,kdfr,embab,emmeb,ptx,pty)            
                 end if                                                 
                 if(imana.eq.0) then                                    
                   proia=prcms(emstr,embaa,emmea)                       
                   kfbria=kfbaa+100*kmltba                              
                   kfmsia=isign(1,kfmea)*(iabs(kfmea)+100*kmltma)       
                   f1ia=                                                
     a             regsup(ida55,kfmsia,kfbria,klmold,klbold,ifl3,kex,   
     b                          prbas,proia,emstr,pr0r,spw,iscd)        
                 end if                                                 
  111  format(2x,a5,2x,3(f6.3,1x),6(i6,1x))                             
                 if(imanb.eq.0) then                                    
                   proib=prcms(emstr,embab,emmeb)                       
                   kfbrib=kfbab+100*kmltbb                              
                   kfmsib=isign(1,kfmeb)*(iabs(kfmeb)+100*kmltmb)       
                   f1ib=                                                
     a             regsup(ida55,kfmsib,kfbrib,klmold,klbold,jfl3,kex,   
     b                          prbas,proib,emstr,pr0r,spw,iscd)        
                 end if                                                 
               else                                                     
                 if(ranf(0).gt.em2nur/srt) then                         
                   imanb=1                                              
                 else                                                   
                   call bresdc(imanb,                                   
     a              iflfb,iflfa,ichobb,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbab,kfmeb,kmltbb,kmltmb,                          
     d              iflb,ifla,jfl3,kdfr,embab,emmeb,ptx,pty)            
                 end if                                                 
                 if(imanb.eq.0) then                                    
                   proib=prcms(emstr,embab,emmeb)                       
                   kfbrib=kfbab+100*kmltbb                              
                   kfmsib=isign(1,kfmeb)*(iabs(kfmeb)+100*kmltmb)       
                   f1ib=                                                
     a             regsup(ida55,kfmsib,kfbrib,klmold,klbold,jfl3,kex,   
     b                          prbas,proib,emstr,pr0r,spw,iscd)        
                 end if                                                 
               end if                                                   
             end if                                                     
             itm1=m                                                     
             it2=0                                                      
             itm2=0                                                     
             it2new=2                                                   
             enlrga=1.                                                  
             if(kfbaa.ne.0) then                                        
               if(irehev.eq.1) then                                     
                 f2ia=0.0                                               
               else                                                     
                 f2ia=                                                  
     a           regsup(ida55,kfinms,kfinba,kfmsia,kfbria,mfld,kex,     
     b                            pcms4,proia,emstr,pr0r,spw,iscd)      
               end if                                                   
               enlrga=f2ia                                              
               if(                                                      
     b            kmltba.eq.0.and.kmltma.eq.0.and.                      
     c            kfbaa.ge.65.and.kfbaa.le.67.and.                      
     d            (iabs(kfmea).eq.23.or.iabs(kfmea).eq.17))then         
                 prspi=pcmsr(srt,emsig0,empion)                         
                 prspi4=prspi                                           
                 clpsi=clphh(srt,prspi,emsig0,empion,ikasi,ikapi)       
                 clpkao=clphh(srt,prkan,emnuc,emkap,ikan,ikap)          
                 embout=embaa                                           
                 emmout=emmea                                           
                 prout=pcmsr(srt,embout,emmout)                         
                 clpout=clphh(srt,prout,embout,emmout,ika1,ika2)        
                 if(isoan.eq.6.or.irehev.eq.1) then                     
                   f2kao0=0.                                            
                 else                                                   
                   f2kao0=regsup(ida55,kfinms,kfinba,                   
     a                            klmold,klbold,mfld,kex,               
     b                        pcms4,prbas,emstr,pr0r,spw,iscd)          
                 end if                                                 
                 it1=0                                                  
                 it1new=2                                               
                 itag=0                                                 
                 call isocgk(itm1,it1,itm2,it2,it1new,it2new,           
     a                                     itm1nw,itm2nw,itag)          
                 kfpi=itm2nw/2*17+23*(2-iabs(itm2nw))/2                 
                 kfy=45+(3-itm1nw)/2                                    
                 if(irehev.eq.1) then                                   
                   f2sig0=0.0                                           
                 else                                                   
                   f2sig0=regsup(ida55,kfinms,kfinba,kfpi,kfy,          
     a                     mfld,kex,pcms4,prspi4,emstr,pr0r,spw,iscd)   
                 end if                                                 
                 if(srt.ge.srtpar) then                                 
                   f2sig0=0.0                                           
                 end if                                                 
                 enlrga=enlrga+0.5/clpout*                              
     a                          (clpsi*f2sig0+clpkao*f2kao0)            
               end if                                                   
               enlrga=enlrga/f1ia                                       
             end if                                                     
             enlrgb=1.                                                  
             if(kfbab.ne.0) then                                        
               if(irehev.eq.1) then                                     
                 f2ib=0.0                                               
               else                                                     
                 f2ib=                                                  
     a           regsup(ida55,kfinms,kfinba,kfmsib,kfbrib,mfld,kex,     
     b                        pcms4,proib,emstr,pr0r,spw,iscd)          
               end if                                                   
               enlrgb=f2ib                                              
               if(                                                      
     b            kmltbb.eq.0.and.kmltmb.eq.0.and.                      
     c            kfbab.ge.65.and.kfbab.le.67.and.                      
     d            (iabs(kfmeb).eq.23.or.iabs(kfmeb).eq.17))then         
                 prlpi=pcmsr(srt,emlamb,empion)                         
                 prlpi4=prlpi                                           
                 clpla=clphh(srt,prlpi,emlamb,empion,ikal,ikapi)        
                 prspi=pcmsr(srt,emsig0,empion)                         
                 prspi4=prspi                                           
                 clpsi=clphh(srt,prspi,emsig0,empion,ikasi,ikapi)       
                 clpkao=clphh(srt,prkan,emnuc,emkap,ikan,ikap)          
                 embout=embab                                           
                 emmout=emmeb                                           
                 prout=pcmsr(srt,embout,emmout)                         
                 clpout=clphh(srt,prout,embout,emmout,ika1,ika2)        
                 if(isoan.eq.6.or.irehev.eq.1) then                     
                   f2kao2=0.                                            
                 else                                                   
                   f2kao2=regsup(ida55,kfinms,kfinba,                   
     a                            klmold,klbold,mfld,kex,               
     b                            pcms4,prbas,emstr,pr0r,spw,iscd)      
                 end if                                                 
                 it1=2                                                  
                 it1new=0                                               
                 itag=0                                                 
                 call isocgk(itm1,it1,itm2,it2,it1new,it2new,           
     a                                       itm1nw,itm2nw,itag)        
                 kfpi=itm2nw/2*17+23*(2-iabs(itm2nw))/2                 
                 kfy=57                                                 
                 if(irehev.eq.1) then                                   
                   f2lam2=0.0                                           
                 else                                                   
                   f2lam2=regsup(ida55,kfinms,kfinba,kfpi,kfy,          
     a                     mfld,kex,pcms4,prlpi4,emstr,pr0r,spw,iscd)   
                 end if                                                 
                 it1new=2                                               
                 itag=0                                                 
                 call isocgk(itm1,it1,itm2,it2,it1new,it2new,           
     a                                       itm1nw,itm2nw,itag)        
                 kfpi=itm2nw/2*17+23*(2-iabs(itm2nw))/2                 
                 kfy=45+(3-itm1nw)/2                                    
                 if(irehev.eq.1) then                                   
                   f2sig2=0.0                                           
                 else                                                   
                   f2sig2=regsup(ida55,kfinms,kfinba,kfpi,kfy,          
     b                     mfld,kex,pcms4,prspi4,emstr,pr0r,spw,iscd)   
                 end if                                                 
                 if(srt.ge.srtpar) then                                 
                   f2sig2=0.0                                           
                   f2lam2=0.0                                           
                 end if                                                 
                 enlrgb=enlrgb+0.5/clpout*                              
     a                       (clpsi*f2sig2+clpla*f2lam2                 
     b                                   +clpkao*f2kao2)                
               end if                                                   
               enlrgb=enlrgb/f1ib                                       
             end if                                                     
             fnlrga=fnlrga+enlrga                                       
             fnlrgb=fnlrgb+enlrgb                                       
             if(ktry.eq.1) then                                         
               call hyper(srt,pcms,em1,em2,                             
     a                   sig0,sig2,ityp1,npid1,ityp2,npid2)             
               xsann=xsesti(4,srt,pcms,strdum,iredu,mdum,icodum)        
               sigupl=1.3*(1.+(prbas**2/0.64))*(prkan/pcms)**2*         
     a                xstkmp                                            
               if(srt.gt.2.0.and.sigqsq.gt.sigupl) kestim=2             
               if(sigqsq.le.sigsum) kestim=1                            
             end if                                                     
             allow=xsann                                                
             if(allow.eq.0.0) then                                      
               call errex('absorb: allow uncalculable')                 
             else                                                       
               allow=((fnlrga*sig0+fnlrgb*sig2)/ktry+sigsum)/allow      
             end if                                                     
             if(ida55.ne.0.and.kswi.eq.1.and.                           
     a                  kestim.ne.1.and.ktry.le.ixsect)  go to 26       
             if(allow.gt.1.0) then                                      
               write(8,*) 'warning: absorb26:',allow                    
               write(8,*) ltype1,kmult1,ltype2,kmult2                   
               write(8,*) 'srt=',srt,pcms,allow*xsann,xsann             
             end if                                                     
           end if                                                       
           if(sigqsq.gt.allow*xsann) then                               
             ichanl=0                                                   
             return                                                     
           else                                                         
             xsann=allow*xsann                                          
             sig0=fnlrga/ktry*sig0                                      
             sig2=fnlrgb/ktry*sig2                                      
           end if                                                       
         else                                                           
           if(m.eq.0) then                                              
             sig0=0.5*sig0                                              
             sig2=0.5*sig2                                              
           else                                                         
             sig0=0.                                                    
           end if                                                       
           xsann=xstot                                                  
           if(xsann.le.0.) call errex('absorb:kbar n,xsann wrong')      
         end if                                                         
         if(isoan.ne.10) then                                           
           if(srt.lt.1.52.and.isoan.eq.5) xsann=sigsum                  
           if(ranf(0).lt.sigsum/xsann) then                             
             zrand=ranf(0)                                              
             icnu=4                                                     
             do 30 inu=1,5                                              
              if(zrand.gt.sigch(inu)/sigsum) then                       
                zrand=zrand-sigch(inu)/sigsum                           
              else                                                      
                icnu=inu                                                
                go to 31                                                
              end if                                                    
   30        continue                                                   
   31        continue                                                   
             if(icnu.eq.1) then                                         
               sig0=0.0                                                 
               sig2=1.0                                                 
               isoan=8                                                  
             else if(icnu.eq.2) then                                    
               sig2=0.0                                                 
               sig0=1.0                                                 
               if(isoan.eq.5) then                                      
                 isoan=9                                                
               else                                                     
                 isoan=7                                                
               end if                                                   
             else if(icnu.eq.3) then                                    
               sig0=0.0                                                 
               sig2=1.0                                                 
               if(isoan.eq.5) then                                      
                 isoan=9                                                
               else                                                     
                 isoan=7                                                
               end if                                                   
             else if(icnu.eq.4) then                                    
               sig2=0.0                                                 
               sig0=1.0                                                 
               isoan=7                                                  
             else if(icnu.eq.5) then                                    
               sig2=1.0                                                 
               sig0=0.0                                                 
               isoan=7                                                  
             end if                                                     
           end if                                                       
         end if                                                         
         if(sig0.eq.0.0.and.sig2.eq.0.0) then                           
           ichanl=0                                                     
           return                                                       
         else if(srt.le.1.5.and.                                        
     a           (isoan.eq.10.or.isoan.eq.5)) then                      
           ichanl=0                                                     
           return                                                       
         else if(ranf(0).lt.sig0/(sig0+sig2)) then                      
           itypid=13                                                    
           npidid=0                                                     
           ibar=3                                                       
         else                                                           
           if(iabs(m).gt.2) then                                        
               call errex('absorb:wrong charge for sigma* baryon')      
           end if                                                       
           itypid=15                                                    
           npidid=m/2                                                   
           ibar=4                                                       
         end if                                                         
         mbaf=mbafnd(ibar,srt)                                          
         if(mbaf.gt.0) then                                             
           itypid=84+mbaf                                               
           if(ibar.eq.3) then                                           
             npidid=57                                                  
           else                                                         
             npidid=44-m/2                                              
           end if                                                       
         end if                                                         
         ityp(id)=itypid                                                
         npidec(id)=npidid                                              
         if(kestim.eq.2)                                                
     a     write(8,*) ' 26:srt=',srt,ltype1,ltype2,em1,em2              
         jesant=1                                                       
         iahyp=iahyp+1                                                  
         if(iabs(iflb3).ne.3.and.iabs(iflc3).ne.3) ikhyp=ikhyp+1        
       else if(jnds.eq.1.or.jpins.eq.1) then                            
         if(srt.le.emnuc+empion) then                                   
           ichanl=0                                                     
           return                                                       
         end if                                                         
         call ndstar(                                                   
     a          srt,pcms,em1,em2,icltyp,ityp1,npid1,ityp2,npid2,        
     b          icollc,mstr,ichd,ichn,ireco,nscod,strrat,vfspin)        
         xsann=xsesti(1,srt,pcms,strrat,ireco,mstr,icollc)              
         prpin=pcmsr(srt,emnuc,empion)                                  
         dxsmb1=0.0                                                     
         dxsmb3=0.0                                                     
         sigmb3=0.                                                      
         sigmb1=0.                                                      
         if(srt.le.max(1.8,2.0)) then                                   
           ida4=ida(4)                                                  
           ida(4)=0                                                     
           call cross1(1,srt,prpin,38,sig3t,emnuc,empion,2,9,0,0)       
           call cross1(1,srt,prpin,23,sig1t,emnuc,empion,2,7,0,0)       
           ida(4)=ida4                                                  
         end if                                                         
         jreco=0                                                        
         if(ireco.eq.0)  jreco=1                                        
         vfflav=strrat                                                  
         bmigst=0.0                                                     
         if(jpins.eq.1) then                                            
           jentry=0                                                     
           if(ireco.eq.0) then                                          
           else if(nscod.eq.1) then                                     
             mtry=0                                                     
  264        continue                                                   
             mtry=mtry+1                                                
             if(mtry.gt.50) then                                        
                call errex('absorb 264:mtry>50')                        
             end if                                                     
             call bwdist(1,-1,srt,jentry,ichd,zdnp,bwig)                
             if(jentry.eq.3.or.jentry.eq.5.or.                          
     b               jentry.eq.8.or.jentry.eq.9) go to 264              
             if(jentry.eq.999) jentry=0                                 
           else if(nscod.eq.2) then                                     
             jentry=5                                                   
           else if(nscod.eq.5) then                                     
             jentry=8                                                   
           else if(nscod.eq.6) then                                     
             jentry=9                                                   
           end if                                                       
           if(jentry.ne.0) then                                         
             if(nscod.eq.1) then                                        
               jent1=1                                                  
               jent2=9                                                  
             else                                                       
               jent1=jentry                                             
               jent2=jentry                                             
             end if                                                     
             bmigs=0.0                                                  
             do 265 jent=jent1,jent2                                    
              if(nscod.eq.1.and.(jent.eq.3.or.jent.eq.5                 
     b           .or.jent.eq.8.or.jent.eq.9)) go to 265                 
              call bwdist(1,1,srt,jent,ichd,zdnp,bmig)                  
  265        continue                                                   
             bmigs=bmigs+bmig                                           
           else                                                         
             bmigs=0.0                                                  
           end if                                                       
         else                                                           
           bmigs=0.0                                                    
           do 267 jent=1,9                                              
            if(jent.ge.6.and.jent.le.8.and.                             
     a         (ichd.ne.4.or.jent.ne.7)                                 
     a                                        ) then                    
              call bwdist(1,1,srt,jent,1,zdnp,bmig)                     
              bmigst=bmigst+bmig                                        
            else if(jreco.eq.1.or.ichd.lt.0) then                       
            else                                                        
              call bwdist(1,1,srt,jent,ichd,zdnp,bmig)                  
              bmigs=bmigs+bmig                                          
            end if                                                      
  267      continue                                                     
         end if                                                         
         if(srt.lt.2.0) then                                            
           if(srt.lt.1.8) then                                          
             gamdb=29.*prpin**3/(1.+40.*prpin**2)                       
             siginb=20.*pi*(hc/2./pi)**2/                               
     a           prpin**2*gamdb**2/((srt-emdelt)**2+0.25*gamdb**2)      
           else                                                         
             siginb=0.0                                                 
           end if                                                       
           sig3t=max(cvalu0,sig3t-siginb-xsbgl3)                        
           bmigsp=0.0                                                   
           do 268 jent=1,9                                              
            call bwdist(1,1,srt,jent,1,zdnp,bmig)                       
            bmigsp=bmigsp+bmig                                          
  268      continue                                                     
           vfak=(prpin/pcms)**2*sig3t/bmigsp                            
         else                                                           
           vfak=0.5*pi*10.0*(hc/pi2)**2/pcms**2/wnorm3                  
         end if                                                         
         if(ireco.ne.0) sigmb3=vfak*vfspin*bmigs                        
         if(irehev.eq.0) dxsmb3=vfak*vfflav*bmigst                      
         bmigs=0.0                                                      
         bmigst=0.0                                                     
         do 269 jent=1,10                                               
          if(jent.eq.10) then                                           
            call bwdist(2,1,srt,jent,1,znnp,bmig)                       
            bmigst=bmigst+bmig                                          
          else if(jreco.eq.1.or.ichn.le.0) then                         
          else                                                          
            call bwdist(2,1,srt,jent,ichn,znnp,bmig)                    
            bmigs=bmigs+bmig                                            
          end if                                                        
  269    continue                                                       
         if(srt.lt.1.8) then                                            
           bmigsp=0.0                                                   
           do 270 jent=1,10                                             
            call bwdist(2,1,srt,jent,1,znnp,bmig)                       
            bmigsp=bmigsp+bmig                                          
  270      continue                                                     
           sig1t=max(cvalu0,1.5*sig1t-0.5*sig3t-xsbgl1)                 
           vfak=(prpin/pcms)**2*sig1t/bmigsp                            
         else                                                           
           vfak=0.5*pi*10.0*(hc/pi2)**2/pcms**2/wnorm1                  
         end if                                                         
         if(ireco.ne.0) sigmb1=vfak*vfspin*bmigs                        
         if(irehev.eq.0)  dxsmb1=vfak*vfflav*bmigst                     
         if(ida(54).eq.1) then                                          
           call hprop(kmult1,ltype1,em1,emr1,gamm1)                     
           call hprop(kmult2,ltype2,em2,emr2,gamm2)                     
           freson=1.                                                    
           if(gamm1.gt.0.0) then                                        
             emmx=srt-em2                                               
             argat=2./gamm1*(emmx-emr1)                                 
             freson=freson/(0.5+atan(argat)/pi)                         
           end if                                                       
           if(gamm2.gt.0.0) then                                        
             emmx=srt-em1                                               
             argat=2./gamm2*(emmx-emr2)                                 
             freson=freson/(0.5+atan(argat)/pi)                         
           end if                                                       
           sigmb1=freson*sigmb1                                         
           sigmb3=freson*sigmb3                                         
         end if                                                         
         sigmb3=sigmb3+dxsmb3                                           
         sigmb1=sigmb1+dxsmb1                                           
         call retcgk(99,ltype1,99,ltype2,1,cgk212)                      
         call retcgk(99,ltype1,99,ltype2,3,cgk232)                      
         sigmb1=cgk212*sigmb1                                           
         sigmb3=cgk232*sigmb3                                           
         dxsmb1=cgk212*dxsmb1                                           
         dxsmb3=cgk232*dxsmb3                                           
         vf=strrat*(prpin/pcms)**2                                      
         if(srt.gt.1.8.and.cgk212.ne.0.0) then                          
           apin=(4.0/1.8**2)**da(7)                                     
           scon1=cgk212*vf*xspin1*                                      
     a              ((4.0/srt**2)**da(7)-apin*(1.8/srt)**pow12)         
         else                                                           
           scon1=0.0                                                    
         end if                                                         
         if(srt.gt.2.0.and.cgk232.ne.0.0) then                          
           bpin=(4.0/2.0**2)**da(7)                                     
           scon3=cgk232*vf*xspin3*                                      
     a              ((4.0/srt**2)**da(7)-bpin*(2.0/srt)**pow32)         
         else                                                           
           scon3=0.0                                                    
         end if                                                         
         if(scon1.gt.cvalu0) then                                       
           if(ida55.ne.0.and.kswi.eq.1.and.iabs(m).eq.1) then           
             mbfix=-11                                                  
             prbas=prpin                                                
             emda=emnuc                                                 
             emdb=empion                                                
             ichoba=2                                                   
             jt1=1                                                      
             jt1new=2                                                   
             jt2new=1                                                   
             idntre=83                                                  
             idntbc=83                                                  
             fnlrga=0.                                                  
             ltyper=(idntre-m)/2                                        
             call bele(iflfa,iflfb,ltyper)                              
             itag=0                                                     
             call isocgk(jtm1,jt1,jtm2,jt2,jt1new,jt2new,               
     a                                       jtm1nw,jtm2nw,itag)        
             klmold=jtm1nw/2*17+23*(2-iabs(jtm1nw))/2                   
             klbold=(idntbc-jtm2nw)/2                                   
             ktry=0                                                     
  126        ktry=ktry+1                                                
             f1ia=1.0                                                   
             f2ia=1.0                                                   
             kfbaa=0                                                    
             if(ranf(0).gt.em2nur/srt) then                             
               imany=1                                                  
             else                                                       
               call bresdc(imany,                                       
     a              iflfb,iflfa,ichoba,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbaa,kfmea,kmltba,kmltma,                          
     d              iflb,ifla,ifl3,kdfr,embaa,emmea,ptx,pty)            
             end if                                                     
             if(imany.eq.0) then                                        
               proia=prcms(emstr,embaa,emmea)                           
               kfbria=kfbaa+100*kmltba                                  
               kfmsia=isign(1,kfmea)*(iabs(kfmea)+100*kmltma)           
               f1ia=                                                    
     a         regsup(ida55,kfmsia,kfbria,klmold,klbold,ifl3,kex,       
     b                        prbas,proia,emstr,pr0r,spw,iscd)          
             end if                                                     
             enlrga=1.                                                  
             if(kfbaa.ne.0) then                                        
               if(irehev.eq.1) then                                     
                 f2ia=0.0                                               
               else                                                     
                 f2ia=                                                  
     a           regsup(ida55,kfinms,kfinba,kfmsia,kfbria,mfld,kex,     
     b                            pcms4,proia,emstr,pr0r,spw,iscd)      
               end if                                                   
               enlrga=f2ia/f1ia                                         
             end if                                                     
             fnlrga=fnlrga+enlrga                                       
             if(ktry.eq.1) then                                         
               sigupl=1.0*(1.+(prbas**2/0.64))*(scon1+scon3)            
     a                                         +sigmb1+sigmb3           
               if(sigqsq.gt.sigupl) kestim=2                            
               if(sigqsq.le.sigmb1+sigmb3) kestim=1                     
             end if                                                     
             if(kestim.ne.1.and.ktry.le.ixsect)  go to 126              
             fnlrg=fnlrga/ktry                                          
             scon1=fnlrg*scon1                                          
             if(xsann.lt.scon1) then                                    
               write(8,*) 'warning: absorb126:',fnlrg                   
               write(8,*) ltype1,kmult1,ltype2,kmult2                   
               write(8,*) 'srt=',srt,pcms,fnlrg*scon3,xsann             
             end if                                                     
           end if                                                       
         end if                                                         
         if(scon3.gt.cvalu0) then                                       
           if(ida55.ne.0.and.kswi.eq.1.and.iabs(m).le.3) then           
             mbfix=-11                                                  
             prbas=prpin                                                
             emda=emnuc                                                 
             emdb=empion                                                
             ichoba=1                                                   
             jt1=3                                                      
             jt1new=2                                                   
             jt2new=1                                                   
             idntre=125                                                 
             idntbc=83                                                  
             fnlrga=0.                                                  
             ltyper=(idntre-m)/2                                        
             call bele(iflfa,iflfb,ltyper)                              
             itag=0                                                     
             call isocgk(jtm1,jt1,jtm2,jt2,jt1new,jt2new,               
     a                                       jtm1nw,jtm2nw,itag)        
             klmold=jtm1nw/2*17+23*(2-iabs(jtm1nw))/2                   
             klbold=(idntbc-jtm2nw)/2                                   
             ktry=0                                                     
  226        ktry=ktry+1                                                
             f1ia=1.0                                                   
             f2ia=1.0                                                   
             kfbaa=0                                                    
             if(ranf(0).gt.em2nur/srt) then                             
               imany=1                                                  
             else                                                       
               call bresdc(imany,                                       
     a              iflfb,iflfa,ichoba,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbaa,kfmea,kmltba,kmltma,                          
     d              iflb,ifla,ifl3,kdfr,embaa,emmea,ptx,pty)            
             end if                                                     
             if(imany.eq.0) then                                        
               proia=prcms(emstr,embaa,emmea)                           
               kfbria=kfbaa+100*kmltba                                  
               kfmsia=isign(1,kfmea)*(iabs(kfmea)+100*kmltma)           
               f1ia=                                                    
     a         regsup(ida55,kfmsia,kfbria,klmold,klbold,ifl3,kex,       
     b                           prbas,proia,emstr,pr0r,spw,iscd)       
             end if                                                     
             enlrga=1.                                                  
             if(kfbaa.ne.0) then                                        
               if(irehev.eq.1) then                                     
                 f2ia=0.0                                               
               else                                                     
                 f2ia=                                                  
     a           regsup(ida55,kfinms,kfinba,kfmsia,kfbria,mfld,kex,     
     b                           pcms4,proia,emstr,pr0r,spw,iscd)       
               end if                                                   
               enlrga=f2ia/f1ia                                         
             end if                                                     
             fnlrga=fnlrga+enlrga                                       
             if(ktry.eq.1) then                                         
               sigupl=1.0*(1.+(prbas**2/0.64))*scon3+scon1              
     a                                         +sigmb1+sigmb3           
               if(sigqsq.gt.sigupl) kestim=2                            
               if(sigqsq.le.sigmb1+sigmb3+scon1) kestim=1               
             end if                                                     
             if(kestim.ne.1.and.ktry.le.ixsect)  go to 226              
             fnlrg=fnlrga/ktry                                          
             scon3=fnlrg*scon3                                          
             if(xsann.lt.scon3) then                                    
               write(8,*) 'warning: absorb226:',fnlrg                   
               write(8,*) ltype1,kmult1,ltype2,kmult2                   
               write(8,*) 'srt=',srt,pcms,fnlrg*scon3,xsann             
             end if                                                     
           end if                                                       
         end if                                                         
         sigr3=sigmb3+scon3                                             
         sigr1=sigmb1+scon1                                             
         if(sigr1+sigr3.gt.xsann) then                                  
           write(8,*) 'absorb: estimated xs_ann to low'                 
           write(8,*) issn(ipi),issn(id),srt                            
           write(8,*) ltype1,kmult1,ltype2,kmult2                       
           write(8,*) sigr1,sigr3,xsann                                 
         else                                                           
           if(sigqsq.gt.(sigr1+sigr3)) then                             
             ichanl=0                                                   
             return                                                     
           end if                                                       
         end if                                                         
         if(srt.gt.2.0) then                                            
           resfrm=resfrm+1.                                             
         end if                                                         
         icharg=(m1+m2+1)/2                                             
         if(sigr1.eq.0.0.and.sigr3.eq.0.0) then                         
           ichanl=0                                                     
           return                                                       
         else if(ranf(0).lt.sigr1/(sigr1+sigr3)) then                   
           ityp(id)=icharg+10                                           
           if(ranf(0).lt.sigmb1/sigr1) then                             
             if(ranf(0).le.dxsmb1/sigmb1) then                          
               jentry=10                                                
             else                                                       
               call bwdist(2,-1,srt,jentry,ichn,znnp,bwig)              
             end if                                                     
             isoan=mbtage(2,jentry,srt,jesant)                          
             npidec(id)=-jentry                                         
           else                                                         
             npidec(id)=3                                               
             jesant=1                                                   
             isoan=10                                                   
           end if                                                       
         else if(sigr3.gt.0.0) then                                     
           ityp(id)=icharg+4                                            
           if(ranf(0).lt.sigmb3/sigr3) then                             
             if(jpins.eq.1) then                                        
             else if(ranf(0).le.dxsmb3/sigmb3) then                     
               probs=0.                                                 
               do 272 jent=6,8                                          
                call bwdist(1,1,srt,jent,1,zdnp,bwig)                   
                prob(jent-5)=bwig/zdnp                                  
                probs=probs+prob(jent-5)                                
  272          continue                                                 
               zrand=ranf(0)                                            
               do 273 j=1,3                                             
                jentry=5+j                                              
                prob(j)=prob(j)/probs                                   
                if(j.gt.1) prob(j)=prob(j-1)                            
                if(zrand.lt.prob(j)) go to 274                          
  273          continue                                                 
  274          continue                                                 
             else                                                       
               call bwdist(1,-1,srt,jentry,ichd,zdnp,bwig)              
             end if                                                     
             isoan=mbtage(1,jentry,srt,jesant)                          
             npidec(id)=-jentry                                         
           else                                                         
             npidec(id)=3                                               
             jesant=1                                                   
             isoan=10                                                   
           end if                                                       
         end if                                                         
         if(kestim.eq.2)                                                
     a     write(8,*) '226:srt=',srt,ltype1,ltype2,em1,em2              
       else if(jpin.eq.1) then                                          
         icharg=ichrgb+ichrgp                                           
         ida4=ida(4)                                                    
         ida(4)=0                                                       
         if(srt.le.emnuc+empion) then                                   
           ichanl=0                                                     
           return                                                       
         end if                                                         
         prpin=pcmsr(srt,emnuc,empion)                                  
         call cross1(1,srt,prpin,38,sig3t,emnuc,empion,2,9,0,0)         
         sigtpn=sig3t*rat3                                              
         sig3=sigtpn                                                    
         if(imixt.gt.1) then                                            
           if(srt.lt.emmnn2) then                                       
             sig1t=0.                                                   
           else                                                         
             call cross1(1,srt,prpin,23,xspimp,emnuc,empion,2,7,0,0)    
             sig1t=max(cvalu0,1.5*xspimp-0.5*sig3t)                     
           endif                                                        
           sigtpn=sigtpn+sig1t*rat1                                     
         endif                                                          
         ida(4)=ida4                                                    
         if(sigtpn.le.1.e-10) then                                      
           write(8,*) 'warning: sigtpn.lt. 1.e-10 in absorb',sigtpn     
           rat3=1.                                                      
           rat1=0.                                                      
         else                                                           
           rat3=sig3/sigtpn                                             
           rat1=1.-rat3                                                 
         end if                                                         
         ilocms=1                                                       
         xsann=xspin3*(4./srt**2)**da(7)                                
         if(em(id).gt.emnuc+2.*empion) then                             
           ida4=ida(4)                                                  
           ida(4)=0                                                     
           call cross1(2,srt,prpin,38,xspipe,emnuc,empion,2,9,0,0)      
           ida(4)=ida4                                                  
           if(srt.lt.emmnd2) then                                       
             zdnpi=1.                                                   
           else                                                         
            zdnpi=0.0                                                   
            bwigs=0.0                                                   
            do 66 jent=1,9                                              
             call bwdist(1,1,srt,jent,1,zdnp,bwig)                      
             zdnpi=zdnpi+bwig*zdnp                                      
             bwigs=bwigs+bwig                                           
   66       continue                                                    
            zdnpi=zdnpi/bwigs                                           
            sigbws=0.5*pi*10.0*(hc/pi2)**2/prpin**2*bwigs               
            if(srt.gt.2.0)then                                          
              probbw=sigbws/wnorm3/xsann                                
              zdnpi=probbw*zdnpi                                        
            else                                                        
              probbw=1.0                                                
            end if                                                      
            call bwdist(1,-1,srt,jentry,1,zdnp,bwig)                    
           end if                                                       
           if(zdnpi.eq.1.) then                                         
             spinds=0.                                                  
           else                                                         
             spinds=(sig3t-xspipe)/(1.-zdnpi)                           
           end if                                                       
           if(spinds.gt.1.08*sig3t) then                                
             write(8,*) 'srt,sig3t,spinds',srt,sig3t,spinds             
             write(8,*) 'xspipe,zdnpi=',xspipe,zdnpi                    
             call errex('absorb:spinds.gt.sig3t')                       
           end if                                                       
           if(spinds.gt.sig3t) spinds=sig3t                             
           if(spinds.lt.0.) spinds=0.                                   
           spind=sig3t-spinds                                           
           if(srt.gt.2.80) then                                         
             xsdelt=0.0                                                 
           else if(srt.gt.1.50) then                                    
             gmdlt= 29.*prpin**3/(1.+40.*prpin*prpin)                   
             sigsab=gmdlt/((srt-emdelt)**2+0.25*gmdlt**2)               
             xsdelt=10.*(hc/pi2)**2*pi2/prpin**2*sigsab*gmdlt           
           else                                                         
             xsdelt=spind                                               
           end if                                                       
           xselb3=max(cvalu0,spind-xsdelt)                              
           if(srt.gt.2.00) then                                         
             sigpar=(sig3t-xselb3)*xs32pn*2./srt/sig3t                  
           end if                                                       
         else                                                           
           ilocms=0                                                     
         end if                                                         
         if(ranf(0).le.rat3) then                                       
           ityp(id)=4+icharg                                            
           if(ilocms.eq.0) then                                         
             npidec(id)=1                                               
           else                                                         
             if(ranf(0).le.spind/sig3t) then                            
               if(ranf(0).lt.xsdelt/spind) then                         
                 npidec(id)=1                                           
               else                                                     
                 ichanl=1                                               
               end if                                                   
             else                                                       
               if(srt.gt.2.0.and.ranf(0).gt.probbw)then                 
                 jesant=1                                               
                 isoan=11                                               
                 npidec(id)=3                                           
               else                                                     
                 isoan=mbtage(1,jentry,srt,jesant)                      
                 npidec(id)=-jentry                                     
               end if                                                   
             end if                                                     
           end if                                                       
         else                                                           
           if(icharg.ne.0.and.icharg.ne.1)                              
     a              call errex('wrong charge in absorb')                
           ityp(id)=10+icharg                                           
           if(ilocms.ne.0) then                                         
             if(sig1t.lt.1.e-10) then                                   
               call errex('sig1t=0. in absorb?????')                    
             end if                                                     
             ida4=ida(4)                                                
             ida(4)=0                                                   
             call cross1(2,srt,prpin,23,xspime,emnuc,empion,2,7,0,0)    
             ida(4)=ida4                                                
             reltp=(xspipe+2.*xselb3)/sig3t                             
             reltm=xspime/xspimp                                        
             if(imixt.eq.2) then                                        
               rat3mp=rat3                                              
             else                                                       
               rat3mp=1./(2.*sig1t/sig3t+1.)                            
             end if                                                     
             xelbg=reltm-0.33333*rat3mp*reltp                           
             if(xelbg.lt.0.0) then                                      
               if(xelbg.lt.-0.02) then                                  
                 write(8,*) srt,xelbg,reltm,rat3mp,reltp                
                 write(8,*)                                             
     a              '3/2 channel provides too much el. xs, absorb'      
                 xelbg=0.0                                              
               end if                                                   
             end if                                                     
             znnpi=0.0                                                  
             bwigs=0.0                                                  
             do 67 jent=1,10                                            
              call bwdist(2,1,srt,jent,1,znnp,bwig)                     
              znnpi=znnpi+bwig*znnp                                     
              bwigs=bwigs+bwig                                          
   67        continue                                                   
             sigbws=0.5*pi*10.0*(hc/pi2)**2/prpin**2*bwigs              
             znnpi=znnpi/bwigs                                          
             if(srt.le.1.8) then                                        
               probbw=1.0                                               
               xsann=sig1t-xsbgl1                                       
             else if(srt.le.2.0) then                                   
               xsann=xs1218+(srt-1.8)/0.2*(xspin1-xs1218)               
               probbw=sigbws/wnorm1/xsann                               
               if(probbw.gt.1.0) then                                   
                 write(8,*) 'absorb-warning: probbw=',probbw            
                 probbw=1.0                                             
               end if                                                   
             else                                                       
               xsann=xspin1*(4.0/srt**2)**da(7)                         
               probbw=sigbws/wnorm1/xsann                               
             end if                                                     
             if(probbw.gt.1.001) then                                   
               write(8,*) probbw,sigbws,wnorm1,xsann,srt,icltyp         
               sigbws=0.5*pi*10.0*(hc/pi2)**2/prpin**2*bwigs            
               write(8,*) prpin,bwigs,sigbws                            
               call errex('absorb: probbw>1')                           
             end if                                                     
             znnpi=probbw*znnpi                                         
             rat1mp=1.0-rat3mp                                          
             xelbg=(xelbg-0.66667*rat1mp*znnpi)/                        
     a                          (1.-0.66667*znnpi)/rat1mp               
             sigr12=(1.-xelbg)*sig1t                                    
             if(srt.gt.2.00) sigr12=sigr12*xs12pn*2./srt/sig1t          
             if(xelbg.lt.-0.05.or.xelbg.gt.1.02) then                   
               if(srt.gt.1.55) then                                     
                 write(8,*) srt,xelbg,                                  
     a            'no consistent i=1/2 background el. xs, absorb'       
               end if                                                   
             end if                                                     
             if(ranf(0).le.xelbg) then                                  
               ichanl=1                                                 
             else                                                       
               call bwdist(2,-1,srt,jentry,1,znnp,bwig)                 
               if(srt.gt.1.8.and.                                       
     a             ranf(0).gt.probbw) then                              
                 jesant=1                                               
                 isoan=11                                               
                 npidec(id)=3                                           
               else                                                     
                 isoan=mbtage(2,jentry,srt,jesant)                      
                 npidec(id)=-jentry                                     
               end if                                                   
             end if                                                     
           else                                                         
             call errex('too low cms energy,n* branch  absorb?????')    
           end if                                                       
         end if                                                         
       else                                                             
         call errex('absorb:dead end')                                  
       end if                                                           
       if(jesant.eq.1) then                                             
         iqqst=iqqsto(ityp(id),npidec(id),iflb3,iflc3)                  
         isoan=isign(1,ltype1)*(isoan+imesp*100)                        
         isoan=isign(1,isoan)*(iabs(isoan)+ibasp*100000)                
       end if                                                           
      return                                                            
      end                                                               
      function iqqsto(ityp,npidec,iflb3,iflc3)                          
       call transl(-1,ityp,npidec,kf)                                   
       ltry=0                                                           
  701  call bele(jfla,jfl,kf)                                           
       jflb=mod(jfl,10)                                                 
       jflc=jfl/10                                                      
       if(jflb.eq.iflb3.and.jflc.eq.iflc3) then                         
         iqqsto=jfl                                                     
       else if(jflc.eq.iflb3.and.jflb.eq.iflc3) then                    
         iqqsto=jfl                                                     
       else if(ltry.lt.50) then                                         
         ltry=ltry+1                                                    
         go to 701                                                      
       else                                                             
        write(8,*) 'iqqsto warning:no qq found',iflb3,iflc3,kf          
        iqqsto=jfl                                                      
       end if                                                           
       return                                                           
       end                                                              
      function  mbtage(idn,jentry,srt,jesant)                           
       include 'scatter7'                                               
       if(jentry.gt.0) then                                             
         if(idn.eq.1) then                                              
           if(jentry.ge.6.and.jentry.le.9) then                         
             jesant=1                                                   
             zrand=ranf(0)                                              
             wdnpi=dcratd(1,jentry,srt)                                 
             wdnspi=wdnpi+dcratd(0,jentry,srt)                          
             if(jentry.eq.7) then                                       
               wdnsk=wdnspi+dcratd(4,jentry,srt)                        
               mbtage=15                                                
             else                                                       
               wdnsk=wdnspi                                             
               mbtage=13                                                
             end if                                                     
             zrand=ranf(0)                                              
             if(zrand.le.wdnpi) then                                    
               mbtage=12                                                
             else if(zrand.le.wdnspi) then                              
               mbtage=14                                                
             else if(zrand.le.wdnsk) then                               
               mbtage=16                                                
             end if                                                     
           else                                                         
             mbtage=0                                                   
           end if                                                       
         else if(idn.eq.2) then                                         
           if(jentry.eq.10) then                                        
             jesant=1                                                   
             zrand=ranf(0)                                              
             znnpi=dcratn(1,jentry,srt)                                 
             if(zrand.le.znnpi) then                                    
               mbtage=12                                                
             else                                                       
               mbtage=13                                                
             end if                                                     
           else                                                         
             mbtage=0                                                   
           end if                                                       
         else                                                           
           call errex('undefined idn in mbtage')                        
         end if                                                         
       else                                                             
         mbtage=0                                                       
       end if                                                           
      return                                                            
      end                                                               
      function xsesti(icho,s,pr,strrat,ireco,mstr,icollc)               
       include 'scatter7'                                               
       common /xspibk/ xspin1,xspin3,wnorm1,wnorm3,pow12,pow32,         
     a                xsbgl1,xsbgl3,xs1218,xs32pn,xs12pn                
       if(icho.le.2) then                                               
          prpin=pcmsr(s,emnuc,empion)                                   
          vf=(prpin/pr)**2*strrat                                       
          if(s.gt.1.8) then                                             
            xsesti=2.0*vf*xspin1*(4.0/s**2)**da(7)                      
            if(s.lt.2.2) xsesti=1.5*xsesti                              
            if(s.lt.2.0.and.(icollc.eq.1.or.icollc.eq.2))               
     a                                        xsesti=1.7*xsesti         
          else                                                          
            if(ireco.eq.1) then                                         
              if(mstr.eq.0) then                                        
                xsesti=(prpin/pr)**2*50.0                               
              else                                                      
                xsesti=(prpin/pr)**2*10.0                               
              end if                                                    
            else                                                        
              xsesti=(prpin/pr)**2*10.0                                 
            end if                                                      
          end if                                                        
          if(pr.lt.0.1) xsesti=2.*xsesti                                
       else if(icho.le.4) then                                          
         prkan=pcmsr(s,emnuc,emkap)                                     
         vf=(prkan/pr)**2                                               
         if(s.gt.1.8) then                                              
           xsesti=vf*50.0*(4.0/s**2)**da(7)                             
         else                                                           
           xsesti=vf*50.0                                               
           if(s.lt.1.6) xsesti=xsesti*4.0                               
         end if                                                         
         if(xsesti.gt.400.) xsesti=400.0                                
       else if(icho.le.6) then                                          
         if(s.le.2.5) then                                              
           xsesti=100.0                                                 
         else                                                           
           xsesti=25.0/pr/pr                                            
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function imspin(kfin)                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       common/ludat1/mst(40),par(80)                                    
       kf=kfin                                                          
       kfa=iabs(kf)                                                     
       kmult=kfa/100                                                    
       if(kfa.gt.100) kfa=mod(kfa,100)                                  
       if(kfa.le.0.or.kfa.gt.70) then                                   
         call errex('imspin: kfa out of range')                         
       end if                                                           
       kfred=isign(1,kf)*kfa                                            
       call luiflv(kfred,ifla,iflb,iflc,ksp)                            
       if(kfred.eq.24.or.kfred.eq.25) then                              
         if(kmult.eq.0) then                                            
           themix=par(72)/360.0*anglmx(0,1)                             
           valmix=0.3333*(cos(themix)-sqrt(2.)*sin(themix))**2          
           if(kfred.eq.25) valmix=1.-valmix                             
           if(ranf(0).gt.valmix) then                                   
             ifla=3                                                     
             iflb=-3                                                    
           else                                                         
             ifla=1                                                     
             iflb=-1                                                    
           end if                                                       
         else                                                           
           if(kfred.eq.25) then                                         
             ifla=3                                                     
             iflb=-3                                                    
           else                                                         
             ifla=1                                                     
             iflb=-1                                                    
           end if                                                       
         end if                                                         
       end if                                                           
       if(kmult.eq.0) then                                              
         if(ksp.eq.0.or.ksp.eq.1) then                                  
           imspin=ksp                                                   
         else                                                           
           call errex('imspin: wrong ksp')                              
         end if                                                         
       else                                                             
         if(kmult.gt.3) then                                            
           imspin=9                                                     
         else if(ksp.eq.0.or.ksp.eq.1) then                             
           imspin=kmult+1                                               
         else                                                           
           call errex('imspin: wrong ksp')                              
         end if                                                         
       end if                                                           
       if(imspin.ge.0) then                                             
         if(ifla.eq.3.or.iflb.eq.3) then                                
           if(ifla+iflb.ne.0) then                                      
             imspin=imspin+10                                           
           else                                                         
             imspin=imspin+30                                           
           end if                                                       
         else if(ifla.eq.-3.or.iflb.eq.-3) then                         
           imspin=imspin+20                                             
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function ibspin(ityp,npidec,kfin)                                 
       if(kfin.eq.0) then                                               
         call transl(-1,ityp,npidec,kf)                                 
       else                                                             
         kf=kfin                                                        
       end if                                                           
       kfa=iabs(kf)                                                     
       kmult=kfa/100                                                    
       if(kfa.gt.100) kfa=mod(kfa,100)                                  
       if(kmult.eq.0) then                                              
         ibspin=0                                                       
       else                                                             
         ibspin=1                                                       
       end if                                                           
       if(kfa.eq.57.or.                                                 
     a    (kfa.ge.43.and.kfa.le.45)) ibspin=ibspin+10                   
      return                                                            
      end                                                               
