CPAT,SCATTR   .                                                         
      subroutine scattr(srt,p0tot,p1tot,p2tot,p3tot,enlrge,             
     &                    p0i1,px1,py1,pz1, p0i2,px2,py2,pz2, em1,em2,  
     &                     pi1sqo,pi2sqo,dtau1,dtau2,prbhd1,prbhd2,     
     &                     dtceff,ityp1,npid1,ityp2,npid2,iqsq,         
     &                    icltyp,iexpn1,iexpn2,itagdl,itag1,itag2,      
     &                    i1,i2,i3,i4,idcid1,idcid2,isubr1,isubr2,      
     &                        kdiffr,nchain,ichanl,ibbban,nop,absrb)    
        include 'scatter7'                                              
        include 'sigmasx'                                               
        include 'partblk7'                                              
      real*4 plund,par,thetlu,philu                                     
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
        common /ierdum/ierr                                             
       common /prpsto/ kmulo1,ltypo1,kmulo2,ltypo2                      
      common /seabk/kchain,iofcha,isea(18),psea(0:3,18),isoan,iqqst,    
     a              emsmb                                               
      real*4 angmxv,angeff                                              
      real*4 tboard                                                     
      common /su3mix/ angmxv,angeff                                     
      logical  jets,string                                              
      logical anpla                                                     
      dimension sigin(mxchan)                                           
      logical absrb                                                     
      data  nchan /0/                                                   
      dimension pv(10,5),rord(10),ue(3),be(3),pl(10,5),dpar(8)          
      data dpar /2.,5.,15.,60.,250.,1500.,1.2e+4,1.2e+5  /              
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 2.4  -- 30-jan-98'/                     
      data pronam/'module: s c a t t r'/                                
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      dtau1=0.0                                                         
      dtau2=0.0                                                         
      emda=0.0                                                          
      emdb=0.0                                                          
       iabsrb=0                                                         
            kdiffr=0                                                    
            nchain=0                                                    
            kchain=0                                                    
            nop=2                                                       
            isubr1=-isubr1                                              
            isubr2=-isubr2                                              
            itagdl=0                                                    
            bx=(p1tot)/(p0tot)                                          
            by=(p2tot)/(p0tot)                                          
            bz=(p3tot)/(p0tot)                                          
            gam=1./sqrt(1.-bx**2-by**2-bz**2)                           
            b2=max(cvalu7,bx**2+by**2+bz**2)                            
            pb=bx*px1+by*py1+bz*pz1                                     
            gg=(gam-1.)*pb/b2-gam*p0i1                                  
            pzr=gg*bz+pz1                                               
            pxr=gg*bx+px1                                               
            pyr=gg*by+py1                                               
            pr=sqrt(pxr*pxr+pyr*pyr+pzr*pzr)                            
            if(pr.lt.1.e-3) then                                        
                ichanl=-1                                               
                return                                                  
            end if                                                      
            x01=x(0,i1) +qdot(0,i1)*dtprop                              
            x02=x(0,i2) +qdot(0,i2)*dtprop                              
            x0=x01-x02                                                  
            x1=x(1,i1)-x(1,i2) +(qdot(1,i1)-qdot(1,i2))*dtprop          
            x2=x(2,i1)-x(2,i2) +(qdot(2,i1)-qdot(2,i2))*dtprop          
            x3=x(3,i1)-x(3,i2) +(qdot(3,i1)-qdot(3,i2))*dtprop          
            ident1=issn(i1)                                             
            ident2=issn(i2)                                             
            iexpn1=itdth(2,ident1)                                      
            iexpn2=itdth(2,ident2)                                      
            itag1=0                                                     
            itag2=0                                                     
            if(iexpn1.ne.0.and.tdtg(ident1).gt.x01) itag1=1             
            if(iexpn2.ne.0.and.tdtg(ident2).gt.x02) itag2=1             
       cost=pzr/pr                                                      
       iswave=0                                                         
       ibbban=0                                                         
       intmod=0                                                         
       itdt31=itdth(3,ident1)                                           
       itdt32=itdth(3,ident2)                                           
       icltp=min(icltyp,maxtyp+1)                                       
       call cross1(1,srt,pr,icltp,sig,em1,em2,                          
     a                                ityp1,ityp2,npid1,npid2)          
       xb=bx*x1+by*x2+bz*x3                                             
       gg=(gam-1.)*xb/b2-gam*x0                                         
       x1=gg*bx+x1                                                      
       x2=gg*by+x2                                                      
       x3=gg*bz+x3                                                      
       qsqcms=x1*x1+x2*x2+x3*x3                                         
       sigqsq=10.*pi*qsqcms                                             
       if(enlrge.gt.0.0) sigqsq=sigqsq/enlrge                           
       if(iqsq.eq.1) then                                               
         sigqsq=0.0                                                     
       end if                                                           
       if(sigqsq.gt.sig) then                                           
         ichanl=0                                                       
         return                                                         
       end if                                                           
       emsmb=em1+em2                                                    
       isoan=0                                                          
       iqqst=0                                                          
       if(icltyp.le.3.and.itag1.eq.0.and.itag2.eq.0) then               
         ipin=0                                                         
       else                                                             
         ipin=2                                                         
       end if                                                           
       call oldnew(1,ityp1,npid1,em1,kmult1,ltyp1)                      
       call oldnew(1,ityp2,npid2,em2,kmult2,ltyp2)                      
       if(kmult1.gt.0) then                                             
         if(kmulo1.ne.kmult1)                                           
     a   call errex('scattr:kmulo1.ne.kmult1')                          
       else if(kmult1.lt.0) then                                        
         if(kmulo1.eq.0)                                                
     a   call errex('scattr:kmulo1.eq.0')                               
       else                                                             
         if(kmulo1.ne.0)  then                                          
           write(8,*) ityp(i1),npidec(i1),em(i1)                        
           write(8,*) ityp1,npid1,em1,kmult1,ltypo1,kmulo1              
         end if                                                         
         if(kmulo1.ne.0)                                                
     a   call errex('scattr:kmulo1.ne.0')                               
       end if                                                           
       if(kmult2.gt.0) then                                             
         if(kmulo2.ne.kmult2)                                           
     a   call errex('scattr:kmulo2.ne.kmult2')                          
       else if(kmult2.lt.0) then                                        
         if(kmulo2.eq.0)                                                
     a   call errex('scattr:kmulo2.eq.0')                               
       else                                                             
         if(kmulo2.ne.0)                                                
     a   call errex('scattr:kmulo2.ne.0')                               
       end if                                                           
       if(iabs(ltyp1).ne.iabs(ltypo1).or.                               
     a                     iabs(ltyp2).ne.iabs(ltypo2)) then            
         write(8,*) ltyp1,ltypo1,ltyp2,ltypo2                           
         call errex('scattr:ltyp.ne.ltypo')                             
       end if                                                           
       call contnt(ltyp1,ltyp2,istra1,istra2,                           
     a                         imeso1,imeso2,ibry1,ibry2,iann)          
       if(iann.gt.0) then                                               
         if(iann.gt.4) call errex('scattr:unexpected iann')             
         icltp=min(icltyp,maxtyp+1)                                     
         call cross1(3,srt,pr,icltp,sigbas,em1,em2,                     
     a                                       ityp1,ityp2,npid1,npid2)   
         if(ranf(0).gt.sigbas/sig) then                                 
           absrb=.true.                                                 
           sigqsq=sigqsq*(sig-sigbas)/sig                               
           sig=sig-sigbas                                               
         else                                                           
           sigqsq=sigqsq*sigbas/sig                                     
           sig=sigbas                                                   
         end if                                                         
       end if                                                           
       if(icltyp.gt.maxtyp) then                                        
         if(icltyp.lt.4754.or.icltyp.gt.4950) then                      
           write(8,*)' invalid type in  scattr  : ',icltyp              
           call errex( 'fatal error')                                   
         end if                                                         
       end if                                                           
       idecis=1                                                         
            ityp1o=ityp1                                                
            ityp2o=ityp2                                                
            npid1o=npid1                                                
            npid2o=npid2                                                
            em1old=em1                                                  
            em2old=em2                                                  
            pxrold=pxr                                                  
            pyrold=pyr                                                  
            pzrold=pzr                                                  
       jesant=0                                                         
       ievntn=ievntn+1                                                  
       lastcl(i1)=ievntn                                                
       lastcl(i2)=ievntn                                                
       s=srt                                                            
       if(iqsq.eq.1) then                                               
         sigel=sig                                                      
       else if(absrb) then                                              
         sigel=0.0                                                      
       else if(n1.eq.1.and.iz1.eq.1.and.n2.eq.1.and.ncollt.eq.0         
     a                                       .and.ncoll.eq.0) then      
         if(sig.lt.0.1) then                                            
           write(8,*) 'sig seems to be 0?????',sig                      
         else                                                           
           if(irun.le.10) write(8,*)                                    
     b              'warning:hd. ncln. coll.,only inel. scattering'     
         end if                                                         
         call cross1(0,srt,pr,icltyp,sigel,em1, em2,                    
     a                               ityp1,ityp2,npid1,npid2)           
         sigqsq=sigqsq*(sig-sigel)/sig                                  
         sig=sig-sigel                                                  
         sigel=0.0                                                      
       else                                                             
         icltp=min(icltyp,maxtyp+1)                                     
         call cross1(0,srt,pr,icltp,sigel,em1, em2,                     
     a                               ityp1,ityp2,npid1,npid2)           
       end if                                                           
       if(.not.absrb.and.(itag1.eq.1.or.itag2.eq.1)) then               
         redcqf=1.0                                                     
         if(itag1.eq.1) redcqf=redcqf*redcqi(imeso1,iexpn1)             
         if(itag2.eq.1) redcqf=redcqf*redcqi(imeso2,iexpn2)             
         if(ranf(0).gt.redcqf) then                                     
           ichanl=0                                                     
           return                                                       
         end if                                                         
       end if                                                           
       if(ranf(0).gt.sigel/sig) goto 3000                               
 2000  continue                                                         
       ichanl=1                                                         
       if(switch(15)) return                                            
       if(itag1.ne.0.or.itag2.ne.0) then                                
         ichanl=-1                                                      
         return                                                         
       end if                                                           
       if(srt.le.em1+em2) then                                          
         ichanl=-1                                                      
         return                                                         
       end if                                                           
       if((ityp1.ge.3.and.ityp1.le.6).or.                               
     a         (ityp1.ge.10.and.ityp1.le.11)) then                      
         if(npid1.lt.-100) npid1=-mod(-npid1,100)                       
       end if                                                           
       if((ityp2.ge.3.and.ityp2.le.6).or.                               
     a         (ityp2.ge.10.and.ityp2.le.11)) then                      
         if(npid2.lt.-100) npid2=-mod(-npid2,100)                       
       end if                                                           
       goto 5000                                                        
 3000  continue                                                         
       nscop=0                                                          
       call cross2(srt,pr,em1,em2,npid1,npid2,icltyp,                   
     &               istra1,istra2,imeso1,imeso2,ibry1,ibry2,           
     &            ityp1,ityp2,sig,sigel,sigin,icnum,nscop,nonstr)       
       if(icnum.le.1) then                                              
         ichanl=0                                                       
         return                                                         
       end if                                                           
       xssum=0.                                                         
       do 5 ic=1,icnum-1                                                
        if(sigin(ic).lt.0.) then                                        
          write(8,*)  i1,i2,ityp(i1),ityp(i2),ic,sigin(ic)              
          call errex('negative cross section in channel ic')            
        end if                                                          
        xssum=xssum+sigin(ic)                                           
    5  continue                                                         
       sum=0.0                                                          
       rand=ranf(0)                                                     
       do 10 i=2,icnum                                                  
        ic=i-1                                                          
        sum=sum+sigin(ic)                                               
        if(rand.le.sum/xssum) goto 3100                                 
   10  continue                                                         
       ichanl=0                                                         
       goto 2000                                                        
 3100  continue                                                         
       if(absrb.and.ic.gt.2) then                                       
         absrb=.false.                                                  
       end if                                                           
       ilmass=0                                                         
       ikmass=0                                                         
       immass=0                                                         
       board=tboard(ityp1,ityp2)                                        
  302  continue                                                         
       cmeff=srt                                                        
       if(absrb) then                                                   
       else if((switch(15)).or.(icltyp.gt.3.and.ic.eq.2)                
     a                                .or.srt.gt.board) then            
          if(.not.switch(6)) then                                       
            deltp0=delten(1,p0tot,p1tot,p2tot,p3tot,em1,em2,cmeff)      
            if(cmeff.gt.srt) cmeff=srt                                  
          end if                                                        
          ic=2                                                          
          call lmass(kdiffr,itag1,itag2,iexpn1,iexpn2,nchain,ipin,      
     a      sig,sigel,srt,pr,em1,em2,cmeff,ityp1,ityp2,iann,            
     b      kmulo1,kmulo2,istra1,istra2,imeso1,imeso2,ibry1,ibry2,      
     c      ltyp1,ltyp2,iflvch,idecis,npid1,npid2,ic,pi1sqo,pi2sqo)     
          ilmass=1                                                      
       else if(icltyp.le.3.and.(itag1.eq.1.or.itag2.eq.1).and.          
     a                                                ic.le.nscop) then 
          ic=2                                                          
          call lmass(kdiffr,itag1,itag2,iexpn1,iexpn2,nchain,ipin,      
     a      sig,sigel,srt,pr,em1,em2,cmeff,ityp1,ityp2,iann,            
     b      kmulo1,kmulo2,istra1,istra2,imeso1,imeso2,ibry1,ibry2,      
     c      ltyp1,ltyp2,iflvch,idecis,npid1,npid2,ic,pi1sqo,pi2sqo)     
          ilmass=1                                                      
       else if(ic.gt.nonstr) then                                       
         if(imeso1.eq.1.and.imeso2.eq.1) immass=1                       
         if(immass.eq.1) then                                           
           if(itag1.eq.1.or.itag2.eq.1) then                            
             xsect=0.0                                                  
           else                                                         
             call mesmes(jesant,                                        
     a        srt,pr,xsect,em1,em2,idum,                                
     b        kmulo1,ltyp1,kmulo2,ltyp2,kmuln1,ltypn1,kmuln2,ltypn2,    
     c                                               iswave,0)          
           end if                                                       
           sigqsq=sigqsq*sigin(3)/sig                                   
           if(sigqsq.lt.xsect) then                                     
             if(xsect.gt.sigin(3)) then                                 
               write(8,*) 'sig,sigel,sigin(2),sigin(3),xsect'           
               write(8,*) sig,sigel,sigin(2),sigin(3),xsect             
               write(8,*)                                               
     a            srt,pr,xsect,em1,em2,ityp1,npid1,ityp2,npid2          
               write(8,*)'scattr:xsect.gt.sigin(3)'                     
             end if                                                     
             if(ida(48).eq.1) then                                      
               call mesmes(jesant,                                      
     a          srt,pr,xsect,em1,em2,ic,kmulo1,                         
     b          ltyp1,kmulo2,ltyp2,kmuln1,ltypn1,kmuln2,ltypn2,         
     c                                                 iswave,1)        
               if(ic.ne.0) then                                         
                 call oldnew(0,ityp1,npid1,em1,kmuln1,ltypn1)           
                 call oldnew(0,ityp2,npid2,em2,kmuln2,ltypn2)           
               end if                                                   
             else                                                       
               ic=0                                                     
             end if                                                     
           else                                                         
             ic=0                                                       
           end if                                                       
         else                                                           
           call kmass(srt,pr,em1,em2,em3,em4,ityp1,ityp2,ityp3,ityp4,   
     a      icnum,npid1,npid2,npid3,npid4,ic,icltyp,nop,nscop,nonstr)   
           ikmass=1                                                     
         end if                                                         
       else                                                             
         if(srt.lt.snnlim.or.icltyp.gt.3) then                          
           call cmass(srt,pr,em1,em2,ityp1,ityp2,iqcnvt,                
     a                                          icltyp,npid1,npid2,ic)  
         else                                                           
           call lmass(kdiffr,itag1,itag2,iexpn1,iexpn2,nchain,ipin,     
     a      sig,sigel,srt,pr,em1,em2,cmeff,ityp1,ityp2,iann,            
     b      kmulo1,kmulo2,istra1,istra2,imeso1,imeso2,ibry1,ibry2,      
     c      ltyp1,ltyp2,iflvch,idecis,npid1,npid2,ic,pi1sqo,pi2sqo)     
           if(ic.gt.0) ic=2                                             
           ilmass=1                                                     
         end if                                                         
       end if                                                           
       if(ic.eq.-1) then                                                
         go to 2000                                                     
       else if(ic.eq.-2) then                                           
         if(ilmass.eq.1) then                                           
          call errex('scattr error:ilmass=1,ic=-2 disabled')            
         else if(immass.eq.1) then                                      
          jesant=1                                                      
         else                                                           
          call errex('scattr error:ilmass.ne.1 and immass.ne.1')        
         end if                                                         
         ic=2                                                           
         intmod=2                                                       
       else if(ic.eq.-3) then                                           
         if(ilmass.ne.1) call errex('ic=-3,scattr error:ilmass.ne.1')   
         if(ibry1.lt.0) then                                            
            call antibb(ic,ityp1,ityp2,npid1,npid2,em1,em2,s)           
         else                                                           
            call antibb(ic,ityp2,ityp1,npid2,npid1,em2,em1,s)           
         end if                                                         
         if(ic.eq.-3) then                                              
            jesant=1                                                    
            ic=1                                                        
            intmod=3                                                    
         end if                                                         
       end if                                                           
       if(itag1.eq.1.or.itag2.eq.1) then                                
         if(absrb.and.intmod.eq.0) intmod=1                             
         if(ikmass.eq.1) intmod=2                                       
         if(immass.eq.1) intmod=2                                       
         iqid1=0                                                        
         iqid2=0                                                        
         if(ilmass.eq.1.and.intmod.eq.0.and.iflvch.eq.0) then           
         else if(intmod.eq.1) then                                      
           if(itag1.eq.1) iqid1=isign(1,iexpn1)*                        
     a                         (-1)**iqcnvt*mod(iabs(iexpn1),100)       
           if(itag2.eq.1) iqid2=isign(1,iexpn2)*                        
     a                         (-1)**iqcnvt*mod(iabs(iexpn2),100)       
           if((itag1.eq.1.and.iqid1.gt.0                                
     a                               .and.imeso1.eq.1).or.              
     b        (itag2.eq.1.and.iqid2.gt.0                                
     c                               .and.imeso2.eq.1)) then            
             ic=0                                                       
             absrb=.false.                                              
           end if                                                       
         else if(intmod.eq.2) then                                      
           ic=0                                                         
         else if(intmod.eq.0.and.ic.ne.0) then                          
           jqcnvt=iqcnvt                                                
           if(itag1.eq.1) call newcqe(ityp1,npid1,iexpn1,jqcnvt)        
           if(itag2.eq.1) call newcqe(ityp2,npid2,iexpn2,jqcnvt)        
         else if(intmod.eq.3) then                                      
           iexpn1=0                                                     
           iexpn2=0                                                     
         end if                                                         
       end if                                                           
       if(ic.eq.0) then                                                 
         ichanl=0                                                       
         return                                                         
       end if                                                           
       if(absrb) then                                                   
         nop=1                                                          
         ichanl=2                                                       
         ic=1                                                           
         sigqsq=sigqsq*sigin(2)/sig                                     
         ierr=systim(strt,dummy)                                        
         if(imeso1.eq.1) then                                           
           call absorb(i1,i2,i3,i4,nop,itagdl,jesant,iann,              
     a                 iqqst,isoan,ityp1,npid1,ityp2,npid2,             
     b                 itag1,iqid1,itag2,iqid2,ichanl,icltyp,           
     c                 ltyp1,kmulo1,ltyp2,kmulo2,                       
     c                 sigqsq,sig,em1,em2,pr,pi1sqo,pi2sqo,srt)         
         else if(imeso2.eq.1) then                                      
           call absorb(i2,i1,i3,i4,nop,itagdl,jesant,iann,              
     a                 iqqst,isoan,ityp2,npid2,ityp1,npid1,             
     b                 itag2,iqid2,itag1,iqid1,ichanl,icltyp,           
     c                 ltyp2,kmulo2,ltyp1,kmulo1,                       
     c                 sigqsq,sig,em2,em1,pr,pi2sqo,pi1sqo,srt)         
         else                                                           
           call errex('scattr: absorb is called without m ingoing')     
         end if                                                         
         iqqst=(-1)**iqcnvt*iqqst                                       
         if(nop.eq.1.and.ichanl.le.1) then                              
           call rstifo(i1,1)                                            
           call rstifo(i2,2)                                            
         else if(nop.eq.1.and.ichanl.eq.2) then                         
           emsto1=em(i1)                                                
           emsto2=em(i2)                                                
           iabsrb=1                                                     
           if(imeso1.eq.1) then                                         
             call fill12(                                               
     a                 ityp2,ityp1,npid2,npid1,ityp(i2),npidec(i2),     
     b                                                 em2,em1,srt)     
           else                                                         
             call fill12(                                               
     a                 ityp1,ityp2,npid1,npid2,ityp(i1),npidec(i1),     
     b                                                 em1,em2,srt)     
           end if                                                       
         end if                                                         
         ierr=systim(tend,dummy)                                        
         tmabsr=tmabsr+tend-strt                                        
         if(intmod.eq.1) then                                           
           if(itag2.eq.1.and.ityp(i1).eq.-9999) then                    
             iexpn2=(iexpn2/100)*100+(-1)**iqcnvt*iqid2                 
           else if(itag1.eq.1.and.ityp(i2).eq.-9999) then               
             iexpn1=(iexpn1/100)*100+(-1)**iqcnvt*iqid1                 
           end if                                                       
         end if                                                         
         if(jesant.eq.1) then                                           
         else if(ichanl.eq.1) then                                      
           go to 2000                                                   
         else if(ichanl.eq.0) then                                      
           return                                                       
         else if(nop.eq.1.and.ichanl.eq.2) then                         
           jesant=1                                                     
         else                                                           
           isubr1=-isubr1                                               
           isubr2=-isubr2                                               
           return                                                       
         end if                                                         
       end if                                                           
       if(nop.lt.4) then                                                
         em4=0.                                                         
         npid4=0                                                        
         ityp4=0                                                        
       end if                                                           
       if(nop.lt.3) then                                                
         em3=0.                                                         
         npid3=0                                                        
         ityp3=0                                                        
       end if                                                           
       ichanl=ic+1                                                      
       if(nop.gt.2) go to 6000                                          
 5000  continue                                                         
       if(srt.lt.em1+em2) then                                          
         write(8,*)'scattr: icltyp,ityp1,ityp2,ic,s,em1,em2'            
         write(8,*) icltyp,ityp1,ityp2,ic,srt,em1,em2                   
         if(nchain.ne.0) then                                           
           call errex('too large masses after scattering')              
         else                                                           
           write(8,*) 'too large masses after scattering'               
         end if                                                         
       end if                                                           
       if(ichanl.eq.1.and.ida(3).eq.0) then                             
         pi1sq=pi1sqo                                                   
         pi2sq=pi2sqo                                                   
       else                                                             
         pi1sq=em1*em1                                                  
         pi2sq=em2*em2                                                  
       end if                                                           
       prold=pr                                                         
       ema=sqrt(pi1sq)                                                  
       emb=sqrt(pi2sq)                                                  
       pr=pcmsr(srt,ema,emb)                                            
       if(iswave.eq.1) then                                             
         ida37=ida(37)                                                  
         ida(37)=1                                                      
       end if                                                           
       icthmx=0                                                         
   67  continue                                                         
       if(ichanl.eq.1) then                                             
         emm=em1+em2                                                    
         call angel(sig,emm,s,pr,icltyp,phisc,cthet)                    
         if(abs(cthet).eq.1) then                                       
            ichanl=-1                                                   
            return                                                      
         end if                                                         
       else if(jesant.eq.1) then                                        
         cthet=1.                                                       
         phisc=0.                                                       
       else                                                             
         call angin(s,pr,prold,icltyp,phisc,cthet)                      
       end if                                                           
       if(switch(19)) cthet=1.                                          
       nop=2                                                            
       if(iqsq.eq.1) then                                               
         cthmax=2.0*(dtceff*pr*pi2/hc)**2                               
         if(cthmax.gt.1.0) then                                         
           cthmax=sqrt(1.-1/cthmax)                                     
         else                                                           
           write(8,*) 'scattr warning:dtceff',dtceff,cthmax,pr*pi2/hc   
           cthmax=1.0                                                   
         end if                                                         
         write(15,*) 'dtceff,cthmax,cthet',dtceff,cthmax,cthet,pr,s     
         if(abs(cthet).gt.cthmax) then                                  
            icthmx=icthmx+1                                             
            if(icthmx.lt.50) go to 67                                   
         end if                                                         
       end if                                                           
       if(iswave.eq.1) then                                             
         ida(37)=ida37                                                  
         phisc=pi                                                       
       end if                                                           
       thetr=acos(max(-cvalu1,min(cvalu1,cost)))                        
       if(pxr.eq.0.0.and.pyr.eq.0.0) then                               
         phir=cvalu0                                                    
       else                                                             
         phir=atan2(pyr,pxr)                                            
       end if                                                           
       thetlu=-thetr                                                    
       philu=-phir                                                      
       nlund=nlund+1                                                    
       if(nlund.gt.2000) call errex('scattr abort:nlund')               
       mst(1)=nlund                                                     
       mst(2)=nlund                                                     
       plund(nlund,1)=x1                                                
       plund(nlund,2)=x2                                                
       plund(nlund,3)=x3                                                
       plund(nlund,4)=x0                                                
       call lurobo(0.,philu,0.,0.,0.)                                   
       call lurobo(thetlu,0.,0.,0.,0.)                                  
       if(plund(nlund,2).eq.0.0.and.                                    
     a                    plund(nlund,1).eq.0.0) then                   
         phib=cvalu0                                                    
       else                                                             
         phib=atan2(plund(nlund,2),plund(nlund,1))                      
       end if                                                           
       phisc=phib+phisc                                                 
       sthet=sqrt(1.-cthet**2)                                          
       plund(nlund,3)=pr*cthet                                          
       plund(nlund,1)=pr*sthet*cos(phisc)                               
       plund(nlund,2)=pr*sthet*sin(phisc)                               
       thetlu=-thetlu                                                   
       philu=-philu                                                     
       call lurobo(thetlu,philu,0.,0.,0.)                               
       pxr=plund(nlund,1)                                               
       pyr=plund(nlund,2)                                               
       pzr=plund(nlund,3)                                               
       nlund=nlund-1                                                    
       p(1,i1)=pxr                                                      
       p(2,i1)=pyr                                                      
       p(3,i1)=pzr                                                      
       p(0,i1)=sqrt(pi1sq+pr*pr)                                        
       p(1,i2)=-pxr                                                     
       p(2,i2)=-pyr                                                     
       p(3,i2)=-pzr                                                     
       p(0,i2)=sqrt(pi2sq+pr*pr)                                        
       call trobo(1,i1,i1,cvalu0,cvalu0,bx,by,bz)                       
       call trobo(1,i2,i2,cvalu0,cvalu0,bx,by,bz)                       
 6000  continue                                                         
       if(nop.gt.2) then                                                
                pv(1,1)= p1tot                                          
                pv(1,2)= p2tot                                          
                pv(1,3)= p3tot                                          
                pv(1,4)= p0tot                                          
                pv(1,5)=srt                                             
                nd=nop                                                  
                pl(1,5)=em1                                             
                pl(2,5)=em2                                             
                if(nd.ge.3 ) pl(3,5)= em3                               
                if(nd.eq.4 ) pl(4,5)= em4                               
                ps=0.                                                   
                do 6001 nn=1,nd                                         
 6001           ps=ps+ pl(nn,5)                                         
                if(ps.gt. srt -ekinmi) then                             
                   write(8,*) 'nop=',nop                                
                   write(8,*) 'ps=',ps,em1,em2,em3,em4                  
                   write(8,*) 'srt-ekinmi=',srt-ekinmi                  
                   write(8,*) ityp1,npid1,ityp2,npid2,                  
     a                            ityp3,npid3,ityp4,npid4               
                   call errex('too high mass sum in scattr')            
                end if                                                  
                pv(nd,5)= pl(nd,5)                                      
                if(nd.eq.2) go to 280                                   
                wtmax=1./dpar(nd-2)                                     
                pmax=pv(1,5)-ps+pl(nd,5)                                
                pmin=0.                                                 
                do 230 il=nd-1,1,-1                                     
                pmax=pmax+pl(il,5)                                      
                pmin=pmin+pl(il+1,5)                                    
                plil5=pl(il,5)                                          
  230           wtmax=wtmax*pcmsr(pmax,pmin,plil5)                      
  240           rord(1)=1.                                              
                do 260 il1=2,nd-1                                       
                rsav=ranf(0)                                            
                do 250 il2=il1-1,1,-1                                   
                if(rsav.le.rord(il2)) goto 260                          
  250           rord(il2+1)=rord(il2)                                   
  260           rord(il2+1)=rsav                                        
                rord(nd)=0.                                             
                wt=1.                                                   
                do 270 il=nd-1,1,-1                                     
                pv(il,5)=pv(il+1,5)+pl(il,5)+                           
     a                       (rord(il)-rord(il+1))*(pv(1,5)-ps)         
                plil5=pl(il,5)                                          
  270           wt=wt*pcmsr(pv(il,5),pv(il+1,5),plil5)                  
                if(wt.lt.ranf(0)*wtmax) goto 240                        
  280           do 300 il=1,nd-1                                        
                plil5=pl(il,5)                                          
                pa=pcmsr(pv(il,5),pv(il+1,5),plil5)                     
                ue(3)=2.*ranf(0)-1.                                     
                phi=pi2*ranf(0)                                         
                ue(1)=sqrt(1.-ue(3)**2)*cos(phi)                        
                ue(2)=sqrt(1.-ue(3)**2)*sin(phi)                        
                do 290 j=1,3                                            
                pl(il,j)=pa*ue(j)                                       
  290           pv(il+1,j)=-pa*ue(j)                                    
                pl(il,4)=sqrt(pa**2+pl(il,5)**2)                        
  300           pv(il+1,4)=sqrt(pa**2+pv(il+1,5)**2)                    
                do 310 j=1,4                                            
  310           pl(nd,j)=pv(nd,j)                                       
                do 340 il=nd-1,1,-1                                     
                do 320 j=1,3                                            
  320           be(j)=pv(il,j)/pv(il,4)                                 
                ga=pv(il,4)/pv(il,5)                                    
                do 340 i=il,nd                                          
                bep=be(1)*pl(i,1)+be(2)*pl(i,2)+be(3)*pl(i,3)           
                do 330 j=1,3                                            
  330           pl(i,j)=pl(i,j)+ga*(ga*bep/(1.+ga)+pl(i,4))*be(j)       
  340           pl(i,4)=ga*(pl(i,4)+bep)                                
                do 350 i=1,3                                            
                p(i,i1)=pl(1,i)                                         
  350           p(i,i2)=pl(2,i)                                         
                p(0,i1)=pl(1,4)                                         
                p(0,i2)=pl(2,4)                                         
                do 370 lloop=3,nd                                       
                npion=npion+1                                           
                if(nrmes(kclst)+nrbar(kclst).eq.iclusz(kclst))then      
                   if(kclst.eq.itripl .and. kdt.lt.idtcl) then          
                    write(8,*)'warning scattr: iclusz(itripl) too low'  
                   end if                                               
                   io=0                                                 
                   do 45 kcl=1,nrclst                                   
  45               io=io+ iclusz(kcl)                                   
                   nrclst=nrclst+1                                      
                   nrbar(nrclst)=0                                      
                   nrmes(nrclst)=1                                      
                   iclusz(nrclst)=1                                     
                   nonrel(nrclst)=.false.                               
                   l=io+1                                               
                   lmax=l                                               
                   if(l.ge.ntotma+1-nspect)                             
     a          call errex('particle number too large,abend in scattr') 
                else if(nrmes(kclst)+nrbar(kclst).lt.iclusz(kclst))then 
                   nrmes(kclst)=nrmes(kclst)+1                          
                   l=iofset+nrbar(kclst)+nrmes(kclst)                   
                else                                                    
                   call errex('nrbar+nrmes.gt.iclusz  ????')            
                end if                                                  
                nprtcl=nprtcl+1                                         
                issn(l)=nprtcl                                          
                decay(l)=1.e+35                                         
                if(lloop.eq.3) i3=l                                     
                if(lloop.eq.4) i4=l                                     
                x(0,l)=0.                                               
                do 360 i=1,3                                            
  360           p(i,l)=pl(lloop,i)                                      
                p(0,l)= pl(lloop,4)                                     
                em(l)= pl(lloop,5)                                      
                ichbig(l)=0                                             
  370           continue                                                
                ityp(i3)=ityp3                                          
                npidec(i3)=npid3                                        
                em(i3)=em3                                              
                lastcl(i3)=ievntn                                       
                if(nop.eq.4) then                                       
                   ityp(i4)=ityp4                                       
                   npidec(i4)=npid4                                     
                   em(i4)=em4                                           
                   lastcl(i4)=ievntn                                    
                end if                                                  
       end if                                                           
       if(nop.eq.2.and.ema.ne.0.0.and.emb.ne.0.0                        
     a                          .and.min(ema,emb).lt.2.0) then          
                prz=pr*abs(cthet)                                       
                e1old=sqrt(pi1sqo+prold*prold)                          
                dplz1=abs(e1old-prold                                   
     a                         -(sqrt(ema*ema+pr*pr)-prz))              
                if(dplz1.lt.1.e-10) then                                
                  write(8,*) 'scattr warning:dplz1=0'                   
                  write(8,*) ichanl,e1old,prold,prz,ema                 
                  dplz1=1.e-10                                          
                end if                                                  
                dtau1=0.5/dplz1*da(15)                                  
                dtau1=dtau1/e1old*sqrt(pi1sqo)*hc/pi2                   
                e2old=sqrt(pi2sqo+prold*prold)                          
                dplz2=abs(e2old-prold                                   
     a                         -(sqrt(emb*emb+pr*pr)-prz))              
                if(dplz2.lt.1.e-10) then                                
                  write(8,*) 'scattr warning:dplz2=0'                   
                  write(8,*) ichanl,e2old,prold,prz,emb                 
                  dplz2=1.e-10                                          
                end if                                                  
                dtau2=0.5/dplz2*da(15)                                  
                dtau2=dtau2/e2old*sqrt(pi2sqo)*hc/pi2                   
                prbhd1=1.                                               
                prbhd2=1.                                               
       end if                                                           
       p0out = p(0,i1)+p(0,i2)                                          
       if(nop.ge.3) p0out=p0out + p(0,i3)                               
       if(nop.eq.4) p0out=p0out + p(0,i4)                               
       if((p0tot/p0out.gt.1.01).or.                                     
     a                           (p0tot/ p0out.lt.0.99)) then           
                    arg=(srt*srt-(pi1sq+pi2sq))**2-4.*                  
     a                                                pi1sq*pi2sq       
                    prel = 0.5/srt*sqrt(arg)                            
                    call errex('no energy conservation in collision')   
       end if                                                           
       if(nop.gt.2) then                                                
         if(isubr1.eq.-1) then                                          
           emoff=(p(0,i1)-frfp(0,ident1))**2-                           
     a                ((p(1,i1)-frfp(1,ident1))**2+                     
     b                      (p(2,i1)-frfp(2,ident1))**2+                
     c                             (p(3,i1)-frfp(3,ident1))**2)         
           if(emoff.lt.em1*em1)  isubr1=1                               
         end if                                                         
         if(isubr2.eq.-1) then                                          
           emoff=(p(0,i2)-frfp(0,ident2))**2-                           
     a                ((p(1,i2)-frfp(1,ident2))**2+                     
     b                      (p(2,i2)-frfp(2,ident2))**2+                
     c                             (p(3,i2)-frfp(3,ident2))**2)         
           if(emoff.lt.em2*em2)  isubr2=1                               
         end if                                                         
       else if(nop.eq.2) then                                           
         if(jesant.eq.1) then                                           
           if(isubr1.eq.-1) isubr1=1                                    
           if(isubr2.eq.-1) isubr2=1                                    
         end if                                                         
         em1n=em1                                                       
         em2n=em2                                                       
         if(isubr1.eq.-1) then                                          
           emoff=(p(0,i1)-frfp(0,ident1))**2-                           
     a                ((p(1,i1)-frfp(1,ident1))**2+                     
     b                      (p(2,i1)-frfp(2,ident1))**2+                
     c                             (p(3,i1)-frfp(3,ident1))**2)         
           if(emoff.lt.em1*em1) then                                    
              isubr1=1                                                  
           else                                                         
              em1n=sqrt(emoff)                                          
           end if                                                       
         end if                                                         
         if(isubr2.eq.-1) then                                          
            emoff=(p(0,i2)-frfp(0,ident2))**2-                          
     a                ((p(1,i2)-frfp(1,ident2))**2+                     
     b                      (p(2,i2)-frfp(2,ident2))**2+                
     c                             (p(3,i2)-frfp(3,ident2))**2)         
           if(emoff.lt.em2*em2) then                                    
              isubr2=1                                                  
           else                                                         
              em2n=sqrt(emoff)                                          
           end if                                                       
         end if                                                         
         if(ityp1.le.11.and.mod(ityp1,10).le.6) then                    
             if(idecis.eq.0.and.npid1.eq.3) npid1=4                     
         end if                                                         
         if(ityp2.le.11.and.mod(ityp2,10).le.6) then                    
             if(idecis.eq.0.and.npid2.eq.3) npid2=4                     
         end if                                                         
         if(ida(50).eq.1)  then                                         
           call plasto(em1,jesant,ityp1,npid1,istor)                    
           if(istor.eq.1) then                                          
             pxrel=pxrold                                               
             pyrel=pyrold                                               
             pzrel=pzrold                                               
             emda=em1old                                                
             emdb=em2old                                                
             if(jesant.eq.0) then                                       
               inew=0                                                   
             else                                                       
               inew=1                                                   
             end if                                                     
             call jetcon(inew,nchan,                                    
     a                      emda,emdb,pxrel,pyrel,pzrel,em1,ityp1,      
     a                                        npid1,ident1,bx,by,bz)    
           end if                                                       
           call plasto(em2,jesant,ityp2,npid2,istor)                    
           if(istor.eq.1) then                                          
             pxrel=-pxrold                                              
             pyrel=-pyrold                                              
             pzrel=-pzrold                                              
             emda=em1old                                                
             emdb=em2old                                                
             if(jesant.eq.0) then                                       
               inew=0                                                   
             else                                                       
               inew=1                                                   
             end if                                                     
             call jetcon(inew,nchan,                                    
     a                      emda,emdb,pxrel,pyrel,pzrel,em2,ityp2,      
     a                                        npid2,ident2,bx,by,bz)    
           end if                                                       
         end if                                                         
         string=jets(ityp1,npid1,em1,0)                                 
         if(string.or.nchain.gt.0) then                                 
            if(string) idcid1=1                                         
            if(jesant.eq.0) then                                        
               pxrel=pxr                                                
               pyrel=pyr                                                
               pzrel=pzr                                                
               if(isubr1.eq.-1)then                                     
                 kt=-1                                                  
                 call transl(-1,ityp1,npid1,k2)                         
                 call torsts(em1n,k2,idum1,idum2,dum1,dum2,kt)          
                 if(kt.eq.-1) then                                      
                    em1n=em1                                            
                    isubr1=1                                            
                 end if                                                 
               end if                                                   
               if(isubr1.eq.-1) then                                    
                 ptb=bx*frfp(1,ident1)+                                 
     a                  by*frfp(2,ident1)+bz*frfp(3,ident1)             
                 gg=(gam-1.)*ptb/b2-gam*frfp(0,ident1)                  
                 frp1=(gg*bx+frfp(1,ident1))                            
                 frp2=(gg*by+frfp(2,ident1))                            
                 frp3=(gg*bz+frfp(3,ident1))                            
                 pxrel=pxrel-frp1                                       
                 pyrel=pyrel-frp2                                       
                 pzrel=pzrel-frp3                                       
                 if(nchain.gt.0) then                                   
                   p0old=sqrt(em1**2+pr*pr)                             
                   p0new=sqrt(em1n**2+pxrel**2+pyrel**2+pzrel**2)       
                   frp0=p0old-p0new                                     
                 end if                                                 
               end if                                                   
               iofcha=0                                                 
               iback=0                                                  
  401          continue                                                 
               emstor=em1n                                              
               inew=0                                                   
               if(mod(kdiffr,2).eq.1) then                              
                  inew=4                                                
               else if(ida(20).eq.0) then                               
                 if(ibry1.ne.0.and.kmulo1.ne.0) inew=2                  
               end if                                                   
               call jetcon(inew,nchain,                                 
     a                      emda,emdb,pxrel,pyrel,pzrel,em1n,ityp1,     
     a                                  npid1,ident1,bx,by,bz)          
               if(iback.eq.-1) then                                     
                 nchain=-nchain                                         
               end if                                                   
               if(iabs(nchain).gt.0) then                               
                 if(em1n.ne.emstor.and.isubr1.eq.-1) then               
                     pxon=pxrel+frp1                                    
                     pyon=pyrel+frp2                                    
                     pzon=pzrel+frp3                                    
                     p0on=sqrt(em1n**2+pxrel**2+pyrel**2+pzrel**2)+     
     a                    frp0                                          
                     em1sq=p0on**2-(pxon**2+pyon**2+pzon**2)            
                     if(em1sq.gt.0.) then                               
                       em1tst=sqrt(em1sq)                               
                       kt=-1                                            
                       call torsts(em1tst,k2,idum1,idum2,dum1,dum2,kt)  
                     else                                               
                       kt=-1                                            
                     end if                                             
                     if(kt.eq.-1) then                                  
                       iback=-1                                         
                       nchain=-nchain                                   
                       do 402 n=9,1,-1                                  
                        if(isea(n).ne.0) then                           
                          isea(n)=0                                     
                          p0new=sqrt(em1n**2+pxrel**2+pyrel**2+         
     a                                         pzrel**2)+psea(0,n)      
                          pxrel=pxrel+psea(1,n)                         
                          pyrel=pyrel+psea(2,n)                         
                          pzrel=pzrel+psea(3,n)                         
                          em1n=p0new**2-(pxrel**2+pyrel**2+pzrel**2)    
                          if(em1n.lt.em1sq) then                        
                           call errex(                                  
     a                      'scattr: string mass < after adding chain') 
                          else                                          
                           em1n=sqrt(em1n)                              
                          end if                                        
                          em1=sqrt((p0on+psea(0,n))**2-                 
     a                        ((pxon+psea(1,n))**2+(pyon+psea(2,n))**2+ 
     b                                            (pzon+psea(3,n))**2)) 
                          go to 401                                     
                        end if                                          
  402                  continue                                         
                       call errex('abend in scattr at 402-loop')        
                     else                                               
                       em1=sqrt(em1sq)                                  
                     end if                                             
                 else                                                   
                     pxon=pxrel                                         
                     pyon=pyrel                                         
                     pzon=pzrel                                         
                     p0on=sqrt(em1n**2+pxon**2+pyon**2+pzon**2)         
                     if(isubr1.eq.-1) then                              
                       pxon=pxon+frp1                                   
                       pyon=pyon+frp2                                   
                       pzon=pzon+frp3                                   
                       p0on=p0on+frp0                                   
                     else                                               
                       em1=em1n                                         
                     end if                                             
                 end if                                                 
                 p(1,i1)=pxon                                           
                 p(2,i1)=pyon                                           
                 p(3,i1)=pzon                                           
                 p(0,i1)=p0on                                           
                 call trobo(1,i1,i1,cvalu0,cvalu0,bx,by,bz)             
               end if                                                   
            else                                                        
               inew=1                                                   
               emda=em1old                                              
               emdb=em2old                                              
               call jetcon(inew,nchain,                                 
     a                      emda,emdb,pxrold,pyrold,pzrold,em1,ityp1,   
     a                                  npid1,ident1,bx,by,bz)          
            end if                                                      
            if(inew.eq.1.and.string)idcid1=2                            
         end if                                                         
         string=jets(ityp2,npid2,em2,0)                                 
         if(string.or.nchain.gt.0) then                                 
            if(string) idcid2=1                                         
            if(jesant.eq.0) then                                        
               pxrel=-pxr                                               
               pyrel=-pyr                                               
               pzrel=-pzr                                               
               if(isubr2.eq.-1)then                                     
                 kt=-1                                                  
                 call transl(-1,ityp2,npid2,k2)                         
                 call torsts(em2n,k2,idum1,idum2,dum1,dum2,kt)          
                 if(kt.eq.-1) then                                      
                    em2n=em2                                            
                    isubr2=1                                            
                 end if                                                 
               end if                                                   
               if(isubr2.eq.-1) then                                    
                 ptb=bx*frfp(1,ident2)+                                 
     a                  by*frfp(2,ident2)+bz*frfp(3,ident2)             
                 gg=(gam-1.)*ptb/b2-gam*frfp(0,ident2)                  
                 frp1=(gg*bx+frfp(1,ident2))                            
                 frp2=(gg*by+frfp(2,ident2))                            
                 frp3=(gg*bz+frfp(3,ident2))                            
                 pxrel=pxrel-frp1                                       
                 pyrel=pyrel-frp2                                       
                 pzrel=pzrel-frp3                                       
                 if(nchain.gt.0) then                                   
                   p0old=sqrt(em2**2+pr*pr)                             
                   p0new=sqrt(em2n**2+pxrel**2+pyrel**2+pzrel**2)       
                   frp0=p0old-p0new                                     
                 end if                                                 
               end if                                                   
               iofcha=1                                                 
               iback=0                                                  
  403          continue                                                 
               emstor=em2n                                              
               inew=0                                                   
               if(kdiffr.ge.2) then                                     
                  inew=4                                                
               else if(ida(20).eq.0) then                               
                 if(ibry2.ne.0.and.kmulo2.ne.0) inew=2                  
               end if                                                   
               call jetcon(inew,nchain,                                 
     a                      emda,emdb,pxrel,pyrel,pzrel,em2n,           
     a                         ityp2,npid2,ident2,bx,by,bz)             
               if(iback.eq.-1) then                                     
                 nchain=-nchain                                         
               end if                                                   
               if(iabs(nchain).gt.0) then                               
                 if(em2n.ne.emstor.and.isubr2.eq.-1) then               
                     pxon=pxrel+frp1                                    
                     pyon=pyrel+frp2                                    
                     pzon=pzrel+frp3                                    
                     p0on=sqrt(em2n**2+pxrel**2+pyrel**2+pzrel**2)+     
     a                    frp0                                          
                     em2sq=p0on**2-(pxon**2+pyon**2+pzon**2)            
                     if(em2sq.gt.0.) then                               
                       em2tst=sqrt(em2sq)                               
                       kt=-1                                            
                       call torsts(em2tst,k2,idum1,idum2,dum1,dum2,kt)  
                     else                                               
                       kt=-1                                            
                     end if                                             
                     if(kt.eq.-1) then                                  
                       iback=-1                                         
                       nchain=-nchain                                   
                       do 404 n=18,10,-1                                
                        if(isea(n).ne.0) then                           
                          isea(n)=0                                     
                          p0new=sqrt(em2n**2+pxrel**2+pyrel**2+         
     a                                        pzrel**2)+psea(0,n)       
                          pxrel=pxrel+psea(1,n)                         
                          pyrel=pyrel+psea(2,n)                         
                          pzrel=pzrel+psea(3,n)                         
                          em2n=p0new**2-(pxrel**2+pyrel**2+pzrel**2)    
                          if(em2n.lt.em2sq) then                        
                           call errex(                                  
     a                      'scattr: string mass < after adding chain') 
                          else                                          
                            em2n=sqrt(em2n)                             
                          end if                                        
                          em2=sqrt((p0on+psea(0,n))**2-                 
     a                        ((pxon+psea(1,n))**2+(pyon+psea(2,n))**2+ 
     b                                            (pzon+psea(3,n))**2)) 
                          go to 403                                     
                        end if                                          
  404                  continue                                         
                       call errex('abend in scattr at 404-loop')        
                     else                                               
                       em2=sqrt(em2sq)                                  
                     end if                                             
                 else                                                   
                     pxon=pxrel                                         
                     pyon=pyrel                                         
                     pzon=pzrel                                         
                     p0on=sqrt(em2n**2+pxon**2+pyon**2+pzon**2)         
                     if(isubr2.eq.-1) then                              
                       pxon=pxon+frp1                                   
                       pyon=pyon+frp2                                   
                       pzon=pzon+frp3                                   
                       p0on=p0on+frp0                                   
                     else                                               
                       em2=em2n                                         
                     end if                                             
                 end if                                                 
                 p(1,i2)=pxon                                           
                 p(2,i2)=pyon                                           
                 p(3,i2)=pzon                                           
                 p(0,i2)=p0on                                           
                 call trobo(1,i2,i2,cvalu0,cvalu0,bx,by,bz)             
               end if                                                   
            else                                                        
               inew=1                                                   
               emda=em1old                                              
               emdb=em2old                                              
               call jetcon(inew,nchain,                                 
     a                      emda,emdb,-pxrold,-pyrold,-pzrold,em2,      
     a                                   ityp2,npid2,ident2,bx,by,bz)   
            end if                                                      
            if(inew.eq.1.and.string)idcid2=2                            
         end if                                                         
         nchast=nchain                                                  
         if(nchain.gt.0) then                                           
           if(nop.ne.2) then                                            
             call errex('scattr:nop=2 expected')                        
           end if                                                       
           atsea=pi*da(17)**2/nchain                                    
           rtsea=sqrt(atsea/pi)                                         
                                                                        
           lstart=iofset+nrbar(kclst)+nrmes(kclst)+1                    
           lend=lstart+nchain-1                                         
           do 405 n=1,18                                                
            if(isea(n).ne.0) then                                       
               nchain=nchain-1                                          
               nop=nop+1                                                
               npion=npion+1                                            
               if(nrmes(kclst)+nrbar(kclst).ge.iclusz(kclst))then       
                   call errex('scattr:no space for sea chain')          
               else                                                     
                   nrmes(kclst)=nrmes(kclst)+1                          
                   l=iofset+nrbar(kclst)+nrmes(kclst)                   
               end if                                                   
               if(nop.eq.3) i3=l                                        
               if(nop.eq.4) i4=l                                        
               do 406 i=0,3                                             
  406          p(i,l)=psea(i,n)                                         
               if(n.le.9) then                                          
                do 407 i=0,3                                            
                 x(i,l)=x(i,i1)+qdot(i,i1)*dtprop                       
                 qdot(i,l)=0.                                           
  407           continue                                                
               else                                                     
                do 408 i=0,3                                            
                 x(i,l)=x(i,i2)+qdot(i,i2)*dtprop                       
                 qdot(i,l)=0.                                           
  408           continue                                                
               end if                                                   
               tlast(l)=x(0,l)                                          
               ichbig(l)=0                                              
               lastcl(l)=ievntn                                         
               nprtcl=nprtcl+1                                          
               issn(l)=nprtcl                                           
               tdtg(nprtcl)=0.0                                         
               itdth(2,nprtcl)=0                                        
               if(itdt31.eq.0) then                                     
                 ktdt31=ident1                                          
               else                                                     
                 ktdt31=mod(itdt31,100000)                              
               end if                                                   
               if(itdt32.eq.0) then                                     
                 ktdt32=ident2                                          
               else                                                     
                 ktdt32=mod(itdt32,100000)                              
               end if                                                   
               if(n.le.9) then                                          
                 identb=ktdt31                                          
                 identa=ktdt32                                          
               else                                                     
                 identa=ktdt31                                          
                 identb=ktdt32                                          
               end if                                                   
               itdth(3,nprtcl)=identa*100000+identb                     
               itdth(4,nprtcl)=0                                        
               ityp(l)=98                                               
               npidec(l)=40                                             
               pp=psea(0,n)**2                                          
               do 409 i=1,3                                             
  409          pp=pp-psea(i,n)**2                                       
               em(l)=sqrt(pp)                                           
               denspp(l)=rtsea                                          
                                                                        
            end if                                                      
  405      continue                                                     
           call trobo(1,lstart,lend,cvalu0,cvalu0,bx,by,bz)             
         end if                                                         
         if(nchain.ne.0) then                                           
           call errex('scattr:nchain.ne.0')                             
         end if                                                         
         nchain=nchast                                                  
       end if                                                           
       ityp(i1)=ityp1                                                   
       npidec(i1)=npid1                                                 
       em(i1)=em1                                                       
       ityp(i2)=ityp2                                                   
       npidec(i2)=npid2                                                 
       em(i2)=em2                                                       
       if(iabsrb.eq.1) then                                             
         em(i1)=emsto1                                                  
         em(i2)=emsto2                                                  
       end if                                                           
       if(intmod.eq.3) ibbban=1                                         
       if(ida(4).gt.0.and.icltyp.le.3.and.ilmass.eq.0.and.              
     a    srt.lt.snnlim.and.(ichanl.eq.2.or.ichanl.eq.3)) then          
         call ndmed(i1,i2,i3,i4,nop,srt,                                
     a                idcid1,idcid2,itag1,itag2,                        
     b                icltyp,ityp1,npid1,ityp2,npid2,em1,em2,ichanl)    
         if(ichanl.eq.0) then                                           
           if(isubr1.eq.1) isubr1=-1                                    
           if(isubr2.eq.1) isubr2=-1                                    
         end if                                                         
       end if                                                           
       if(nop.eq.2.and.min(ityp(i2),ityp(i1)).eq.-9999) then            
         nop=1                                                          
       end if                                                           
      return                                                            
      end                                                               
      function tboard(ityp1,ityp2)                                      
      include 'sigmasx'                                                 
      itp1=ityp1                                                        
      itp2=ityp2                                                        
      if ((itp1.eq.14.or.itp2.eq.14).or.                                
     &    (itp1.eq.13.and.itp2.ge.7.and.itp2.le.9).or.                  
     &    (itp2.eq.13.and.itp1.ge.7.and.itp1.le.9).or.                  
     &    (itp1.eq.15.and.itp2.ge.7.and.itp2.le.9).or.                  
     &    (itp2.eq.15.and.itp1.ge.7.and.itp1.le.9)) then                
                                         tboard=tblhig-0.46             
      else                                                              
     &if (((itp1.le.6.or.(itp1.ge.10.and.itp1.le.11)).and.              
     &    ((itp2.ge.7.and.itp2.le.9).or.itp2.eq.12)) .or.               
     &    ((itp2.le.6.or.(itp2.ge.10.and.itp2.le.11)).and.              
     &    ((itp1.ge.7.and.itp1.le.9).or.itp1.eq.12) )) then             
                                          tboard=tblhig-0.938           
      else                                                              
           tboard=tblhig                                                
      end if                                                            
      return                                                            
      end                                                               
      subroutine  newcqe(ityp1,npid1,iexpn1,iqcn)                       
               call transl(-1,ityp1,npid1,kf1)                          
               call luiflv(kf1,ifa,ifb,ifc,ksp)                         
               iqid1=mod(iexpn1,100)                                    
               iabiq=iabs(iqid1)                                        
               if1=iabiq/10                                             
               if2=iabiq-10*if1                                         
               ido=1                                                    
               if(iabiq.gt.10) then                                     
                 if(if1.eq.iabs(ifa)) then                              
                    if(if2.eq.iabs(ifb).or.if2.eq.iabs(ifc)) ido=0      
                 else if(if1.eq.iabs(ifb)) then                         
                    if(if2.eq.iabs(ifa).or.if2.eq.iabs(ifc)) ido=0      
                 else if(if1.eq.iabs(ifc)) then                         
                    if(if2.eq.iabs(ifa).or.if2.eq.iabs(ifb)) ido=0      
                 end if                                                 
               else                                                     
                 if2=isign(1,iqid1)* if2 *(-1)**iqcn                    
                 if(if2.eq.ifa.or.if2.eq.ifb.or.if2.eq.ifc.or.          
     a                    (ifa+ifb+ifc.eq.0.and.iabs(if2).ne.3)) ido=0  
               end if                                                   
               if(ido.eq.1) then                                        
                   call bele(ifla,iflb,kf1)                             
                   ifla=(-1)**iqcn*ifla                                 
                   iflb=(-1)**iqcn*iflb                                 
                   if(iabiq.gt.10) then                                 
                      iexpn1=(iexpn1/100)*100 +iflb                     
                   else                                                 
                      if(isign(1,iqid1).eq.isign(1,ifla)) then          
                          iexpn1=(iexpn1/100)*100 +ifla                 
                      else if(isign(1,iqid1).eq.isign(1,iflb)) then     
                          iexpn1=(iexpn1/100)*100 +iflb                 
                          if(iabs(iflb).gt.10 ) then                    
                             write(8,*) iexpn1,ifla,iflb,iqcn,kf1       
                             call errex('unclear situation in newcqe')  
                          end if                                        
                      else                                              
                          write(8,*) iexpn1,ifla,iflb,iqcn,kf1          
                          call errex('garbage in newcqe')               
                      end if                                            
                   end if                                               
               end if                                                   
      return                                                            
      end                                                               
      subroutine plasto(em1,jesant,ityp1,npid1,istor)                   
       include 'scatter7'                                               
       logical jets                                                     
        istor=0                                                         
        if(ityp1.le.11.and.mod(ityp1,10).le.6.and.                      
     a                                npid1.ne.0.and.npid1.ne.1         
     b                        .and..not.jets(ityp1,npid1,em1,0)) then   
          istor=1                                                       
        else if(((ityp1.ge.7.and.ityp1.le.9.and.npid1.eq.2).or.         
     a           (npid1.eq.35.and.ityp1.eq.99).or.                      
     b      (iabs(npid1).ge.28.and.iabs(npid1).le.29.and.ityp1.eq.99))  
     c                                                           ) then 
                                                                        
          istor=1                                                       
        end if                                                          
      return                                                            
      end                                                               
