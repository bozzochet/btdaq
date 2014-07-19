CPAT,DUPDAT   .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine dupdat(ind,startt)                                     
        include 'scatter7'                                              
        include 'partblk7'                                              
      dimension pind(0:3)                                               
       common /dtestb/ ammag                                            
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 2.2  -- 17-mar-95'/                     
      data pronam/'d u p d a t  --  decay update'/                      
      data ttvers/.true./                                               
      if(ttvers) then                                                   
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      lfin=0                                                            
      itp1=ityp(ind)                                                    
      npid1=npidec(ind)                                                 
      if(itp1.eq.-9999) then                                            
        decay(ind)=1.e+35                                               
        return                                                          
      else if(itp1.eq.98.and.npid1.eq.40) then                          
        decay(ind)=startt                                               
        return                                                          
      end if                                                            
      em1=em(ind)                                                       
      do  9 i=0,3                                                       
    9 pind(i)=p(i,ind)                                                  
      emeff=pind(0)*pind(0)                                             
      do 10 i=1,3                                                       
   10 emeff=emeff-pind(i)*pind(i)                                       
      emeff=sqrt(emeff)                                                 
      ammag=gamind(kerr,itp1,npid1,lfin,em1,emeff)                      
      if(kerr.eq.-100) then                                             
        write(8,*) ind,issn(ind),itp1,npid1,em1,emeff                   
        call errex('dupdat:kerr=-100')                                  
      else if(ammag.ne.cvalu0) then                                     
        deltt=timelf(ammag,emeff,pind(1),pind(2),pind(3))               
        decay(ind)=startt+deltt                                         
      else                                                              
        decay(ind)= 1.e+35                                              
      end if                                                            
      return                                                            
      end                                                               
      function widr(kmult,npid,em)                                      
       include 'scatter7'                                               
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       real*4 gemla(11),gemsi(11),                                      
     a        gemca(11),gemsis(11),gemcas(11),gemom(11)                 
       equivalence (gemla(1),gambar(1,3))                               
       equivalence (gemsi(1),gambar(1,4))                               
       equivalence (gemca(1),gambar(1,5))                               
       equivalence (gemsis(1),gambar(1,6))                              
       equivalence (gemcas(1),gambar(1,7))                              
       equivalence (gemom(1),gambar(1,8))                               
       real*4 gamres                                                    
       common /gamrbk/ gamres                                           
       real*4 prtgam                                                    
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
       real*4 dcrm                                                      
       npida=iabs(npid)                                                 
       if(kmult.lt.0.or.kmult.gt.11) then                               
         call errex('wrong input in widr: kmult unknown')               
       end if                                                           
       if(kmult.eq.0) then                                              
         if(npida.eq.0.or.npida.gt.70) then                             
           call errex('wrong input in widr: npida unknown')             
         else                                                           
           widr=prtgam(npida)                                           
         end if                                                         
       else if(npida.eq.41.or.npida.eq.42) then                         
         ientry=kmult                                                   
         dummy=dcratn(1,ientry,em)                                      
         widr=gamres                                                    
       else if(npida.ge.61.and.npida.le.64) then                        
         ientry=kmult                                                   
         dummy=dcratd(1,ientry,em)                                      
         widr=gamres                                                    
       else if(npida.eq.57) then                                        
         widr=gemla(kmult)                                              
       else if(npida.ge.43.and.npida.le.45) then                        
         widr=gemsi(kmult)                                              
       else if(npida.eq.46.or.npida.eq.47) then                         
         if(kmult.eq.2.or.kmult.eq.3.or.                                
     a                     kmult.eq.4.or.kmult.eq.9) then               
           ientry=min(4,kmult-1)                                        
           dummy=dcratc(1,ientry,em)                                    
           widr=gamres                                                  
         else                                                           
           widr=gemca(kmult)                                            
         end if                                                         
       else if(npida.ge.65.and.npida.le.67) then                        
         widr=gemsis(kmult)                                             
       else if(npida.eq.68.or.npida.eq.69) then                         
         if(kmult.eq.4) then                                            
           ientry=5                                                     
           dummy=dcratc(1,ientry,em)                                    
           widr=gamres                                                  
         else                                                           
           widr=gemcas(kmult)                                           
         end if                                                         
       else if(npida.eq.70) then                                        
         widr=gemom(kmult)                                              
       else                                                             
         widr=dcrm(kmult,npid,em)                                       
       end if                                                           
      return                                                            
      end                                                               
      function gamind(kerr,itp1,npid1,lfin,em1,emeff)                   
       include 'scatter7'                                               
       real*4 prtgam                                                    
       common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)    
       real*4 gamres                                                    
       common /gamrbk/ gamres                                           
       kerr=0                                                           
               if(itp1.eq.-9999) then                                   
                 gamind=cvalu0                                          
               else if(itp1.le.2) then                                  
                 gamind=cvalu0                                          
               else if((itp1.ge.7.and. itp1.le.9).and.                  
     a                              (npid1.eq.0))then                   
                 gamind=cvalu0                                          
               else if(itp1.eq.12.and.em1.lt.0.6)then                   
                 gamind=cvalu0                                          
               else if(itp1.eq.13.and.em1.lt.1.2)then                   
                 gamind=cvalu0                                          
               else if(itp1.eq.15.and.em1.le.1.2)then                   
                 gamind=cvalu0                                          
               else if(itp1.eq.14.and.em1.le.0.5)then                   
                 gamind=cvalu0                                          
               else if(itp1.le.6) then                                  
                   if(npid1.eq.3.or.npid1.eq.4) then                    
                     gamind=gamcon(emeff)                               
                   else if(npid1.eq.1.or.npid1.eq.0) then               
                     gamind=dwidth(emeff)                               
                   else                                                 
                     kmult=mod(-npid1,100)                              
                     dummy=dcratd(1,kmult,emeff)                        
                     gamind=gamres                                      
                   end if                                               
               else if(itp1.ge.7.and.itp1.le.9) then                    
                   if(npid1.eq.3.or.npid1.eq.4) then                    
                     gamind=gamcon(emeff)                               
                   else if(npid1.eq.2) then                             
                     gamind=widrho                                      
                   else if(npid1.eq.1) then                             
                     gamind=-1.0                                        
                   end if                                               
               else if(itp1.ge.10.and.itp1.le.11) then                  
                   if(npid1.eq.3.or.npid1.eq.4) then                    
                     gamind=gamcon(emeff)                               
                   else                                                 
                     kmult=mod(-npid1,100)                              
                     dummy=dcratn(1,kmult,emeff)                        
                     gamind=gamres                                      
                   end if                                               
               else if(itp1.ge.12.and.itp1.le.15) then                  
                     gamind=gamcon(emeff)                               
               else if(itp1.ge.85.and.itp1.le.97) then                  
                 kmult=itp1-84                                          
                 gamind=widr(kmult,npid1,emeff)                         
                 if(gamind.le.0.0) then                                 
                   kerr=-100                                            
                 end if                                                 
               else if(itp1.eq.98) then                                 
                     gamind=gamcon(emeff)                               
               else if(itp1.eq.99) then                                 
                     kl2=npid1                                          
                     kfa=iabs(kl2)                                      
                     if(kfa.lt.100)then                                 
                         idc=kfa                                        
                     else                                               
                         call luiflv(kfa,ifla,iflb,iflc,ksp)            
                         idc=76+5*ifla+ksp                              
                     end if                                             
                     if(prtgam(idc).gt.0.) then                         
                       gamind=prtgam(idc)                               
                     else                                               
                       gamind=cvalu0                                    
                     end if                                             
               else                                                     
                 write(8,*) 'itp1=',itp1                                
                 call errex(' gamind: this type not implemented')       
               end if                                                   
      return                                                            
      end                                                               
      function gamcon(emeff)                                            
       include 'scatter7'                                               
       data decstr  /0.0/                                               
       if(decstr.eq.0.0) then                                           
         if(emctst.le.2.) then                                          
           decstr=1000.                                                 
         else                                                           
           decstr=log(2.0)/(emctst-2.)                                  
         end if                                                         
       end if                                                           
       if(ida(18).eq.2) then                                            
         deccst=decstr*(emeff-2.)                                       
         if(deccst.lt.20.) then                                         
           prblif=exp(-deccst)                                          
         else                                                           
           prblif=0.                                                    
         end if                                                         
         if(ranf(0).lt.prblif) then                                     
           gamcon=abs(gmprop)                                           
         else                                                           
           gamcon=gmprop                                                
         end if                                                         
       else if(ida(18).eq.1.and.emeff.lt.emctst) then                   
         gamcon=abs(gmprop)                                             
       else                                                             
         gamcon=gmprop                                                  
       end if                                                           
      return                                                            
      end                                                               
      subroutine oldnew(jwhat,ityp,npidec,emin,kmult,ltype)             
       include 'scatter7'                                               
       if(jwhat.ne.0) then                                              
         kmult=-2                                                       
         call transl(-1,ityp,npidec,ltype)                              
         if(ityp.eq.-9999) then                                         
           kmult=0                                                      
         else if(ityp.eq.1.or.ityp.eq.2) then                           
           kmult=0                                                      
         else if((ityp.ge.7.and. ityp.le.9).and.                        
     a                              npidec.eq.0)then                    
           kmult=0                                                      
         else if(ityp.eq.12.and.npidec.eq.2                             
     a                         .and.emin.lt.0.60) then                  
           kmult=0                                                      
         else if(ityp.eq.13.and.emin.lt.1.2)then                        
           kmult=0                                                      
         else if(ityp.eq.15.and.emin.lt.1.2)then                        
           kmult=0                                                      
         else if(ityp.eq.14.and.emin.lt.0.5)then                        
           kmult=0                                                      
         else if(ityp.le.6) then                                        
           if(npidec.eq.3.or.npidec.eq.4) then                          
             kmult=-1                                                   
           else if(npidec.eq.1.or.npidec.eq.0) then                     
             kmult=0                                                    
           else                                                         
             kmult=mod(-npidec,100)                                     
           end if                                                       
         else if(ityp.ge.7.and.ityp.le.9) then                          
           if(npidec.eq.3.or.npidec.eq.4) then                          
             kmult=-1                                                   
           else if(npidec.eq.2) then                                    
             kmult=0                                                    
           else if(npidec.eq.1) then                                    
             kmult=-1                                                   
           else if(npidec.eq.0) then                                    
             kmult=0                                                    
           end if                                                       
         else if(ityp.ge.10.and.ityp.le.11) then                        
           if(npidec.eq.3.or.npidec.eq.4) then                          
             kmult=-1                                                   
           else                                                         
              kmult=mod(-npidec,100)                                    
           end if                                                       
         else if(ityp.ge.12.and.ityp.le.15) then                        
           kmult=-1                                                     
         else if(ityp.ge.85.and.ityp.le.95) then                        
           kmult=ityp-84                                                
         else if(ityp.eq.98) then                                       
           kmult=-1                                                     
         else if(ityp.eq.99) then                                       
           kmult=0                                                      
         else                                                           
           write(8,*) 'ityp=',ityp                                      
           call errex('oldnew: this type not implemented')              
         end if                                                         
         if(kmult.lt.-1.or.kmult.gt.11)                                 
     a                   call errex('oldnew: wrong kmult')              
       else                                                             
         call transl(1,ityp,npidec,ltype)                               
         if(kmult.gt.0) then                                            
           if(kmult.gt.13) then                                         
             call errex('oldnew 0:kmult out of range')                  
           end if                                                       
           if(iabs(ltype).le.40) then                                   
             ityp=84+kmult                                              
             npidec=ltype                                               
           else                                                         
             if(ityp.eq.99) then                                        
               ityp=84+kmult                                            
             else if(ityp.eq.13) then                                   
               ityp=84+kmult                                            
               npidec=57                                                
             else if(ityp.eq.15) then                                   
               ityp=84+kmult                                            
               npidec=44-npidec                                         
             else if(ityp.le.6) then                                    
               npidec=-kmult                                            
               if(ityp.eq.1.or.ityp.eq.2) then                          
                 ityp=ityp+9                                            
               end if                                                   
             else                                                       
               write(8,*) ityp,npidec                                   
               call errex('oldnew 0, wrong ityp for baryon')            
             end if                                                     
           end if                                                       
         else if(kmult.eq.0) then                                       
         else if(kmult.eq.-1) then                                      
           if(ityp.eq.99) then                                          
             ityp=98                                                    
           else if(ityp.gt.0.and.ityp.le.15) then                       
             if(ityp.le.6) then                                         
               npidec=3                                                 
               if(ityp.eq.1.or.ityp.eq.2) then                          
                 ityp=ityp+9                                            
               end if                                                   
             else if(ityp.le.9) then                                    
               if(npidec.eq.0) then                                     
                 npidec=3                                               
               else                                                     
                 npidec=4                                               
               end if                                                   
             end if                                                     
           else                                                         
             call errex('oldnew 0, kmult=-1, wrong ityp')               
           end if                                                       
         else                                                           
           call errex('oldnew:kmult out of range')                      
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      subroutine hprop(kmult,ltype,emin,em,gamma)                       
       include 'scatter7'                                               
       dimension jentry(70)                                             
       data jentry /                                                    
     a  16*-1,1,4,4,-1,                                                 
     b   2*-1,1,2,3,-1,5,8,8,3*-1,5,6,7,5*-1,                           
     c   2*2,3*4,2*5,9*-1,3,3*-1,4*1,3*6,2*7,8                          
     d             /                                                    
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       if(kmult.lt.-1.or.kmult.gt.11)                                   
     a      call errex('hprop: wrong input')                            
       ltypa=iabs(ltype)                                                
       if(ltypa.eq.0.or.ltypa.gt.70)                                    
     a      call errex('hprop: wrong input')                            
       jent=jentry(ltypa)                                               
       if(jent.le.0)                                                    
     a      call errex('hprop: wrong input')                            
       if(kmult.ge.0) then                                              
         if(ltypa.gt.40) then                                           
           em=embarr(kmult,jent)                                        
           gamma=gambar(kmult,jent)                                     
         else                                                           
           em=emmesr(kmult,jent)                                        
           gamma=gammes(kmult,jent)                                     
         end if                                                         
       else                                                             
         em=emin                                                        
         gamma=abs(gmprop)*emin                                         
       end if                                                           
       if(em.le.0.0)                                                    
     a      call errex('hprop: wrong output')                           
      return                                                            
      end                                                               
