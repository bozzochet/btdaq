CPAT,COLL     .                                                         
      subroutine coll(ind)                                              
        include 'amsrqmd.h'
        include 'scatter7'                                              
        logical absrb                                                   
        include 'partblk7'                                              
        common /ioutbk/i1,i2,i3,i4,nop,ichanl,iexpn1,iexpn2             
     a                 ,idcid1,idcid2,iqsq                              
        common /routbk/xold1(0:3),xold2(0:3),srt,bxtot,bytot,bztot      
        real*4  xform(0:3),pform(0:3)                                   
        dimension xvold(0:3)                                            
        common /ierdum/ierr                                             
        common /prpsto/ kmulo1,ltypo1,kmulo2,ltypo2                     
        data  iruact,itprn,dtprn /0,0,1.0/                              
       real*4 plund                                                     
       common /lujets/ nlund,k(2000,2),plund(2000,5)                    
       data ncrit/600/                                                  
       i1=i1c(ind)                                                      
       i2=i2c(ind)                                                      
       ident1=issn(i1)                                                  
       ident2=issn(i2)                                                  
       idud(1,ident1)=idud(1,ident1)-1                                  
       idud(1,ident2)=idud(1,ident2)-1                                  
       if(itdth(5,ident1).lt.0) then                                    
         itdth(5,ident1)=0                                              
         itdth(5,ident2)=0                                              
         iqsq=1                                                         
         dtceff=datsto(ident1)-tlast(i1)                                
         datsto(ident1)=datsto(ident2)                                  
         if(dtceff.lt.0.0) call errex('coll:wrong dtceff')              
       else                                                             
         iqsq=0                                                         
       end if                                                           
       tnexev=ctime(ind)                                                
       if(tlstev.gt.tnexev) then                                        
         write(lun8,*)'??? tlstev>tnexev',tnexev,tlstev                    
         i1c(ind)=-1                                                    
         return                                                         
       end if                                                           
       tlstev=tnexev                                                    
       if(tnexev.gt.timmax) then                                        
         write(lun8,*)'coll??? tnexev>timmax',tnexev,timmax                
         i1c(ind)=-1                                                    
         return                                                         
       end if                                                           
       dtprop=tnexev-(time+dt)                                          
       absrb=.false.                                                    
       ntag12=0                                                         
       ncoli1=nclcnt(ident1)                                            
       ncoli2=nclcnt(ident2)                                            
       itagdl=0                                                         
       if(switch(8)) then                                               
         if(ncoli1.eq.0.and.ncoli2.eq.0) then                           
           if((ident1.le.n1old.and.ident2.le.n1old).or.                 
     a         (ident1.gt.n1old.and.ident1.le.n1old+n2old.and.          
     b             ident2.gt.n1old.and.ident2.le.n1old+n2old))then      
             xfirst=xfirst+1.                                           
             i1c(ind)=-1                                                
             return                                                     
           end if                                                       
         end if                                                         
       end if                                                           
      if(ida(47).eq.1) then                                             
        if((itdth(1,ident1).ne.0.and.                                   
     a      itdth(2,ident1).eq.0).or.                                   
     b              (itdth(1,ident2).ne.0  .and.                        
     c               itdth(2,ident2).eq.0)) then                        
              i1c(ind)=-1                                               
              return                                                    
        end if                                                          
      end if                                                            
      do 162 i=0,3                                                      
       xold1(i)=x(i,i1)+qdot(i,i1)*dtprop                               
       xold2(i)=x(i,i2)+qdot(i,i2)*dtprop                               
  162 continue                                                          
      isubr1=0                                                          
      isubr2=0                                                          
      if(ida(15).ne.0) then                                             
        if(ident1.gt.n1old+n2old) then                                  
          if(tdtg(ident1).le.xold1(0)) then                             
            i1c(ind)=-1                                                 
            return                                                      
          end if                                                        
        end if                                                          
        if(ident2.gt.n1old+n2old) then                                  
          if(tdtg(ident2).le.xold2(0)) then                             
            i1c(ind)=-1                                                 
            return                                                      
          end if                                                        
        end if                                                          
      end if                                                            
      if(ida(7).eq.1) then                                              
        il=i1                                                           
        ident=ident1                                                    
 1214   continue                                                        
        isubr=0                                                         
        if(itdth(2,ident).ne.0)then                                     
          if(tdtg(ident).le.x(0,il)+qdot(0,il)*dtprop) then             
            isubr=6                                                     
            startt=tdtg(ident)                                          
            call dupdat(il,startt)                                      
          else                                                          
            isubr=1                                                     
          end if                                                        
          call ofshel(isubr,il,ident)                                   
        end if                                                          
        isubr2=isubr                                                    
        if(il.eq.i1) then                                               
            il=i2                                                       
            ident=ident2                                                
            isubr1=isubr                                                
            go to 1214                                                  
        end if                                                          
      end if                                                            
            em1=em(i1)                                                  
            npid1=npidec(i1)                                            
            ityp1=ityp(i1)                                              
            em2=em(i2)                                                  
            npid2=npidec(i2)                                            
            ityp2=ityp(i2)                                              
      call oldnew(1,ityp1,npid1,em1,kmulo1,ltypo1)                      
      call oldnew(1,ityp2,npid2,em2,kmulo2,ltypo2)                      
            isubs1=0                                                    
            if(ityp1.ge.85.and.ityp1.le.97) then                        
              isubs1=1                                                  
              call substi(ityp1,npid1)                                  
            end if                                                      
            isubs2=0                                                    
            if(ityp2.ge.85.and.ityp2.le.97) then                        
              isubs2=1                                                  
              call substi(ityp2,npid2)                                  
            end if                                                      
            icltyp=ictype(ityp1,ityp2)                                  
            p0i1=p(0,i1)                                                
            p0i2=p(0,i2)                                                
            pz1=p(3,i1)                                                 
            pz2=p(3,i2)                                                 
            px1=p(1,i1)                                                 
            px2=p(1,i2)                                                 
            py1=p(2,i1)                                                 
            py2=p(2,i2)                                                 
            pi1sqo=p0i1*p0i1-px1*px1-py1*py1-pz1*pz1                    
            pi2sqo=p0i2*p0i2-px2*px2-py2*py2-pz2*pz2                    
            p0tot=p0i1+p0i2                                             
            p1tot=px1+px2                                               
            p2tot=py1+py2                                               
            p3tot=pz1+pz2                                               
            bxtot=p1tot/p0tot                                           
            bytot=p2tot/p0tot                                           
            bztot=p3tot/p0tot                                           
            srt=sqrt(p0tot*p0tot-p1tot*p1tot-p2tot*p2tot-p3tot*p3tot)   
              prel=pcmsr(srt,sqrt(pi1sqo),sqrt(pi2sqo))                 
              velrat=1.0                                                
              enlrge=1./da(8)                                           
              if(nt.gt.2.and.((itripl.eq.1.and..not.switch(17))         
     a              .or.ida(34).gt.0.or.switch(6)).and.                 
     b                                (ncoli1.eq.0.or.ncoli2.eq.0))then 
                call frzcor(enlfrz,i1,i2)                               
                enlrge=enlrge*enlfrz                                    
              else if(.not.switch(6).and.srt.gt.em1+em2) then           
                pfree=pcmsr(srt,em1,em2)                                
                enlrge=enlrge*pfree/prel                                
              end if                                                    
            if((ida(40).eq.2.or.ida(40).eq.3).and.ida(34).ne.2          
     a                                           .and.iqsq.eq.0) then   
              icltp=min(maxtyp+1,icltyp)                                
              call cross1(1,srt,prel,icltp,sigt,em1,em2,                
     a                                  ityp1,ityp2,npid1,npid2)        
              sigsto=csig(ind)                                          
              if(sigsto.gt.sigt) then                                   
                i1c(ind)=-1                                             
                if(isubr1.eq.1) then                                    
                  do 67 i=0,3                                           
   67              p(i,i1)=p(i,i1)-frfp(i,ident1)                       
                end if                                                  
                if(isubr2.eq.1) then                                    
                  do 68 i=0,3                                           
   68              p(i,i2)=p(i,i2)-frfp(i,ident2)                       
                end if                                                  
                return                                                  
              end if                                                    
            end if                                                      
            deltz=zlast(ident1)-zlast(ident2)                           
            delty=ylast(ident1)-ylast(ident2)                           
            if(delty.gt.1.0) then                                       
              if(deltz.gt.1.0) then                                     
               write(lun8,*) deltz,delty,deltz*delty,'zy warning'          
              end if                                                    
            end if                                                      
            if(abs(deltz).le.3.0) deltz=0.                              
            if(abs(delty).le.1.6) delty=0.                              
            if(deltz*delty.gt.3.0) then                                 
             write(lun8,*) deltz,delty,deltz*delty,'zy warning'            
             write(lun8,*) ityp1,ityp2,ident1,ident2                       
            end if                                                      
       call sveifo(i1,1)                                                
       call sveifo(i2,2)                                                
       call contnt(ltypo1,ltypo2,istra1,istra2,                         
     a                         imeso1,imeso2,ibry1,ibry2,iann)          
       iqcnvt=0                                                         
       if((ibry1.lt.0.or.ibry2.lt.0).and.ibry1*ibry2.ge.0)then          
         call qinvrt(ityp1,npid1,em1)                                   
         call qinvrt(ityp2,npid2,em2)                                   
         icltyp=ictype(ityp1,ityp2)                                     
         iqcnvt=1                                                       
       else if(ibry1*ibry2.lt.0)then                                    
         if(isubs1.eq.1.and.ityp1.ge.85.and.ityp1.le.97) ityp1=98       
         if(isubs2.eq.1.and.ityp2.ge.85.and.ityp2.le.97) ityp2=98       
         icltyp=ictype(ityp1,ityp2)                                     
       end if                                                           
       ierr=systim(strt,dummy)                                          
       idcid1=0                                                         
       idcid2=0                                                         
       call scattr(srt,p0tot,p1tot,p2tot,p3tot,enlrge,                  
     &             p0i1,px1,py1,pz1, p0i2,px2,py2,pz2, em1,em2,         
     &                 pi1sqo,pi2sqo,dtau1,dtau2,prbhd1,prbhd2,         
     &                 dtceff,ityp1,npid1,ityp2,npid2,iqsq,             
     &                 icltyp,iexpn1,iexpn2,itagdl,itag1,itag2,         
     &                 i1,i2,i3,i4, idcid1,idcid2, isubr1,isubr2,       
     &                  kdiffr,nchain,ichanl,ibbban,nop,absrb)          
       jcolt=0                                                          
       if(ichanl.gt.0) then                                             
         call coltim(ichanl,ind,ident1,ident2,i1,i2,iqsq,               
     a                  itag1,itag2,nop,idcid1,idcid2,dtau1,dtau2,      
     b                                              prbhd1,prbhd2)      
         if(ibbban.eq.1.and.ichanl.gt.0) then                           
             if(ida(49).eq.3) then                                      
               pbarpa=0.05                                              
               pbarpb=0.6                                               
               sig0n=120.                                               
               srt0=2.*emnuc                                            
               rppbbw=(pbarpa*srt0)**2/                                 
     a                      ((srt**2-srt0**2)**2+(pbarpa*srt0)**2)      
               rppbto=pbarpb+rppbbw                                     
               sann=sig0n*(srt0/srt)**2*rppbto                          
               if(ranf(0).lt.rppbbw/rppbto) then                        
                 ichanl=-3                                              
                 itdth(4,ident1)=ident2                                 
                 if(ityp(i1).eq.-9999) itdth(4,ident1)=-itdth(4,ident1) 
                 itdth(4,ident2)=ident1                                 
                 if(ityp(i2).eq.-9999) itdth(4,ident2)=-itdth(4,ident2) 
                 zrand=ranf(0)                                          
                 deltt=-log(max(zrand,cvalu2))/                         
     a                        (da(21)*pbarpa*5.0674/da(4))              
                 deltt=deltt*p0tot/srt                                  
               else                                                     
                 ibbban=0                                               
               end if                                                   
             else                                                       
               ibbban=0                                                 
             end if                                                     
         else if(ibbban.eq.1) then                                      
           ibbban=0                                                     
         end if                                                         
         if(ichanl.lt.0) jcolt=1                                        
       end if                                                           
       if(ichanl.gt.0) then                                             
         if(iabs(isubr1).eq.1) then                                     
           do 97 i=0,3                                                  
            if(isubr1.eq.-1) then                                       
              p(i,i1)=p(i,i1)-frfp(i,ident1)                            
            else                                                        
              psyst(i)=psyst(i)+frfp(i,ident1)                          
              frfp(i,ident1)=0.                                         
            end if                                                      
   97      continue                                                     
         end if                                                         
         if(iabs(isubr2).eq.1) then                                     
           do 98 i=0,3                                                  
            if(isubr2.eq.-1) then                                       
              p(i,i2)=p(i,i2)-frfp(i,ident2)                            
            else                                                        
              psyst(i)=psyst(i)+frfp(i,ident2)                          
              frfp(i,ident2)=0.                                         
            end if                                                      
   98      continue                                                     
         end if                                                         
       end if                                                           
       if(ierror.eq.1) return                                           
       if(ichanl.le.0) nop=2                                            
       if(idcid1.ne.0.or.idcid2.ne.0) ntag12=10                         
       if(iqcnvt.eq.1) ntag12=10                                        
       if(nop.gt.2)  ntag12=10                                          
       ierr=systim(tend,dummy)                                          
       tmsctr=tmsctr+tend-strt                                          
       if(switch(4).or.ntag12.gt.0.or.ichanl.le.0) then                 
         ntag1=0                                                        
         ntag2=0                                                        
       else                                                             
         ierr=systim(strt,dummy)                                        
         ntag1=0                                                        
         ntag2=0                                                        
         call paulin(i1,ntag1)                                          
         call paulin(i2,ntag2)                                          
         if(ierror.eq.1) return                                         
         ierr=systim(tend,dummy)                                        
         tmpali=tmpali+tend-strt                                        
       end if                                                           
       ntag12=ntag1+ntag2+ntag12                                        
       i1c(ind)=0                                                       
       if(ntag12.lt.0.or.ichanl.le.0) then                              
         ichanl=0                                                       
         if(ntag1.lt.0.or.ntag2.lt.0) then                              
           ncoll=ncoll+1                                                
           xblock=xblock+1.                                             
           npaubl=npaubl+1                                              
         end if                                                         
         call rstifo(i1,1)                                              
         call rstifo(i2,2)                                              
         if(ibbban.eq.1) then                                           
           decay(i1)=xold1(0)+deltt                                     
           decay(i2)=xold2(0)+deltt                                     
         end if                                                         
         if(isubr1.eq.-1.or.                                            
     a       (isubr1.eq.1.and.jcolt.eq.1)) then                         
           do 94 i=0,3                                                  
            p(i,i1)=p(i,i1)-frfp(i,ident1)                              
   94      continue                                                     
         end if                                                         
         if(isubr2.eq.-1.or.                                            
     a       (isubr2.eq.1.and.jcolt.eq.1)) then                         
           do 93 i=0,3                                                  
            p(i,i2)=p(i,i2)-frfp(i,ident2)                              
   93      continue                                                     
         end if                                                         
         if(nop.gt.2) then                                              
           npion=npion-1                                                
           if(i3.gt.iofset+iclusz(kclst))then                           
                nrclst=nrclst-1                                         
           else                                                         
                nrmes(kclst)=nrmes(kclst)-1                             
           end if                                                       
           if(nop.ge.4) then                                            
              do  510 i4a=i4,i4+nop-4                                   
               npion=npion-1                                            
               if(i4a.gt.iofset+iclusz(kclst))then                      
                   nrclst=nrclst-1                                      
               else                                                     
                   nrmes(kclst)=nrmes(kclst)-1                          
               end if                                                   
  510         continue                                                  
           end if                                                       
         end if                                                         
       else if(ichanl.gt.0) then                                        
         call xtupdt                                                    
         if(isubs1.eq.1.or.isubs2.eq.1) then                            
           if(isubs1.eq.1) then                                         
             call oldat(                                                
     a            1,itol1,npol1,p0ol1,pxol1,pyol1,pzol1,emol1)          
             call transl(-1,itol1,npol1,kfb)                            
             call transl(-1,ityp(i1),npidec(i1),kfa)                    
             if(ichanl.eq.1.or.(abs(emol1-em(i1)).lt.1.e-4              
     a                                  .and.kfa.eq.kfb)) then          
               ityp(i1)=itol1                                           
               npidec(i1)=npol1                                         
             end if                                                     
           end if                                                       
           if(isubs2.eq.1) then                                         
             call oldat(                                                
     a            2,itol2,npol2,p0ol2,pxol2,pyol2,pzol2,emol2)          
             call transl(-1,itol2,npol2,kfb)                            
             call transl(-1,ityp(i2),npidec(i2),kfa)                    
             if(ichanl.eq.1.or.(abs(emol2-em(i2)).lt.1.e-4              
     a                                  .and.kfa.eq.kfb)) then          
               ityp(i2)=itol2                                           
               npidec(i2)=npol2                                         
             end if                                                     
           end if                                                       
         end if                                                         
         imscst=1                                                       
         if(ida(28).eq.0) then                                          
           if(imeso1.eq.0) then                                         
             call mscstr(imscst,i1,i2,isccss)                           
           else                                                         
             call mscstr(imscst,i2,i1,isccss)                           
           end if                                                       
         end if                                                         
         ierr=systim(strt,dummy)                                        
         startt=x(0,i1)                                                 
         call dupdat(i1,startt)                                         
         startt=x(0,i2)                                                 
         call dupdat(i2,startt)                                         
         if(nchain.gt.0) then                                           
           if(idcid1.ne.0) decay(i1)=x(0,i1)                            
           if(idcid2.ne.0) decay(i2)=x(0,i2)                            
         end if                                                         
         if((kdiffr.eq.2.or.ichanl.eq.1)                                
     a             .and.itdth(2,ident1).ne.0.and.                       
     b                      tdtg(ident1).gt.tlast(i1)) then             
           decay(i1)=tdtg(ident1)                                       
         end if                                                         
         if((kdiffr.eq.1.or.ichanl.eq.1)                                
     a             .and.itdth(2,ident2).ne.0.and.                       
     b                      tdtg(ident2).gt.tlast(i2)) then             
           decay(i2)=tdtg(ident2)                                       
         end if                                                         
         if(nop.ge.3) then                                              
           startt=x(0,i3)                                               
           call dupdat(i3,startt)                                       
           if(nop.ge.4) then                                            
             do 512 i4a=i4,i4+nop-4                                     
             startt=x(0,i4a)                                            
  512        call dupdat(i4a,startt)                                    
           end if                                                       
         end if                                                         
         if(ida(4).eq.2.and.itagdl.eq.1) then                           
           if(ityp(i1).ge.3.and.ityp(i1).le.6) then                     
             decay(i1)=x(0,i1)                                          
           else if(ityp(i2).ge.3.and.ityp(i2).le.6) then                
             decay(i2)=x(0,i2)                                          
           end if                                                       
         end if                                                         
         call output(1)                                                 
         i1a=i1                                                         
         i2a=i2                                                         
         if(idcid1.ne.0.and.decay(i1).eq.x(0,i1)) then                  
           i1a=-2                                                       
         else                                                           
           if(itdth(2,ident1).ne.0.and.                                 
     a              tdtg(ident1).gt.tlast(i1)) then                     
             call qdrot(i1,ident1,bxtot,bytot,bztot)                    
             if(ida(42).eq.1) then                                      
               do 501 i=0,3                                             
                pform(i)=p(i,i1)+frfp(i,ident1)                         
                xform(i)=x(i,i1)+qdot(i,i1)/qdot(0,i1)*                 
     a                             (tdtg(ident1)-x(0,i1))               
  501          continue                                                 
               write(lun7)                                                 
     &              ityp(i1),issn(i1),npidec(i1),                       
     &              (xform(i),i=0,3),                                   
     &              (pform(i),i=0,3),em(i1),                            
     &              0.0,0,0                                             
             end if                                                     
           end if                                                       
         end if                                                         
         if(idcid2.ne.0.and.decay(i2).eq.x(0,i2)) then                  
           i2a=-2                                                       
         else                                                           
           if(itdth(2,ident2).ne.0.and.                                 
     a               tdtg(ident2).gt.tlast(i2)) then                    
             call qdrot(i2,ident2,bxtot,bytot,bztot)                    
             if(ida(42).eq.1) then                                      
               do 502 i=0,3                                             
                pform(i)=p(i,i2)+frfp(i,ident2)                         
                xform(i)=x(i,i2)+qdot(i,i2)/qdot(0,i2)*                 
     a                             (tdtg(ident2)-x(0,i2))               
  502          continue                                                 
               write(lun7)                                                 
     &              ityp(i2),issn(i2),npidec(i2),                       
     &              (xform(i),i=0,3),                                   
     &              (pform(i),i=0,3),em(i2),                            
     &              0.0,0,0                                             
             end if                                                     
           end if                                                       
         end if                                                         
         if(i1a.gt.0.or.i2a.gt.0) then                                  
           call propag(i1a,i2a,0)                                       
         end if                                                         
         if(nop.eq.3) then                                              
           if(i3.le.iofset+iclusz(kclst))call propag(i3,-2,0)           
         else if(nop.ge.4) then                                         
           if(i3.le.iofset+iclusz(kclst).and.                           
     a                       i4.le.iofset+iclusz(kclst)) then           
                  call propag(i3,i4,0)                                  
           else if(i3.le.iofset+iclusz(kclst)) then                     
                 call propag(i3,-2,0)                                   
           end if                                                       
           if(nop.gt.4) then                                            
              do 516 i4a=i4+1,i4+nop-4                                  
  516         call propag(i4a,-2,0)                                     
           end if                                                       
         end if                                                         
         if(ityp(i1).gt.0.and.itdth(2,ident1).ne.0  ) then              
           if(tdtg(ident1).gt.tlast(i1)) then                           
             if(i1a.lt.0) then                                          
               do  168 i=0,3                                            
  168          x(i,i1)=x(i,i1)-qdot(i,i1)*dtprop                        
             end if                                                     
           else                                                         
             tdtg(ident1)=0.0                                           
             itdth(1,ident1)=0                                          
             itdth(2,ident1)=0                                          
             itdth(4,ident1)=0                                          
           end if                                                       
         end if                                                         
         if(ityp(i2).gt.0.and.itdth(2,ident2).ne.0  ) then              
           if(tdtg(ident2).gt.tlast(i2)) then                           
             if(i2a.lt.0) then                                          
               do  169 i=0,3                                            
  169          x(i,i2)=x(i,i2)-qdot(i,i2)*dtprop                        
             end if                                                     
           else                                                         
             tdtg(ident2)=0.0                                           
             itdth(1,ident2)=0                                          
             itdth(2,ident2)=0                                          
             itdth(4,ident2)=0                                          
           end if                                                       
         end if                                                         
         ibb=min(1,ispect(i1))+min(1,ispect(i2)) +1                     
         if(ibb.le.0.or.ibb.gt.3) then                                  
            write(lun8,*) 'wrong ibb=',ibb                                 
         else                                                           
            xcnt(ibb)=xcnt(ibb)+1.                                      
         end if                                                         
         if(ida(50).eq.1)  then                                         
           ipur=0                                                       
           if(ityp(i1).eq.-9999) then                                   
             if(kspin(iabs(ltypo1)).eq.1) ipur=i1                       
           else if(ityp(i2).eq.-9999) then                              
             if(kspin(iabs(ltypo2)).eq.1) ipur=i2                       
           end if                                                       
           if(ipur.ne.0) then                                           
             idntpu=issn(ipur)                                          
             call purglu(idntpu)                                        
           end if                                                       
         end if                                                         
         if(nlund.gt.ncrit)  call purglu(0)                             
         if(absrb) then                                                 
           if(nop.ge.3) iabspi=iabspi+1                                 
         else if(icltyp.gt.3.and.iabs(ityp(i1)).le.2.and.               
     a                                      iabs(ityp(i2)).le.2) then   
               if(nop.eq.2) then                                        
                  iabspi=iabspi+1                                       
               else if(nop.eq.3.and.iabs(ityp(i3)).le.2) then           
                  iabspi=iabspi+1                                       
               end if                                                   
         end if                                                         
       end if                                                           
       ierr=systim(tend,dummy)                                          
       tmupdt=tmupdt+tend-strt                                          
      return                                                            
      end                                                               
      subroutine coltim(ichanl,ind,ident1,ident2,i1,i2,iqsq,            
     a                  itag1,itag2,nop,idcid1,idcid2,dtau1,dtau2,      
     b                                              prbhd1,prbhd2)      
        include 'amsrqmd.h'
       include 'scatter7'                                               
       include 'partblk7'                                               
       dimension ibeqlc(2)                                              
       dimension iblock(2)                                              
        if(ida(57).eq.0) return                                         
        if(idcid1.ne.0.and.idcid2.ne.0) return                          
        if(itag1.ne.0.and.itag2.ne.0) return                            
        if(ityp(i1).eq.-9999.or.ityp(i2).eq.-9999) return               
        if(iqsq.eq.1) return                                            
        if(nop.ne.2) return                                             
        if(itag1.eq.0)then                                              
          jpar1=ident1                                                  
        else                                                            
          jpar1=mod(itdth(3,ident1),100000)                             
        end if                                                          
        if(itag2.eq.0)then                                              
          jpar2=ident2                                                  
        else                                                            
          jpar2=mod(itdth(3,ident2),100000)                             
        end if                                                          
        if(jpar1.gt.n1old.and.jpar1.le.n1old+n2old) then                
          jpar1=1                                                       
        else if(jpar1.gt.0.and.jpar1.le.n1old) then                     
          jpar1=-1                                                      
        else                                                            
          jpar1=0                                                       
        end if                                                          
        if(jpar2.gt.n1old.and.jpar2.le.n1old+n2old) then                
          jpar2=1                                                       
        else if(jpar2.gt.0.and.jpar2.le.n1old) then                     
          jpar2=-1                                                      
        else                                                            
          jpar2=0                                                       
        end if                                                          
        if(jpar1.eq.0.and.jpar2.eq.0) return                            
        ibeqlc(1)=0                                                     
        ibeqlc(2)=0                                                     
        iblock(1)=0                                                     
        iblock(2)=0                                                     
        kblock=0                                                        
        do 100 jjj=1,nument                                             
         if(i1c(jjj).ne.i1.and.i2c(jjj).ne.i1.and.                      
     a      i1c(jjj).ne.i2.and.i2c(jjj).ne.i2) go to 100                
         if(i1c(jjj).eq.i1.and.i2c(jjj).eq.i2) go to 100                
         if(i1c(jjj).eq.i1.or.i2c(jjj).eq.i1) then                      
           if(itag1.ne.0.or.idcid1.ne.0) go to 100                      
           if(dtau1.eq.cvalu0) go to 100                                
           if(jpar1.eq.0) go to 100                                     
           if(ibeqlc(1).eq.1) go to 100                                 
           if(iblock(1).eq.1) go to 100                                 
           if(prbhd1.eq.cvalu0) go to 100                               
           if(ranf(0).gt.prbhd1) go to 100                              
           jpara=jpar1                                                  
           ia=i1                                                        
           ib=i1c(jjj)+i2c(jjj)-i1                                      
           dtaua=dtau1                                                  
           ii=1                                                         
         else if(i1c(jjj).eq.i2.or.i2c(jjj).eq.i2) then                 
           if(itag2.ne.0.or.idcid2.ne.0) go to 100                      
           if(dtau2.eq.cvalu0) go to 100                                
           if(jpar2.eq.0) go to 100                                     
           if(ibeqlc(2).eq.1) go to 100                                 
           if(iblock(2).eq.1) go to 100                                 
           if(prbhd2.eq.cvalu0) go to 100                               
           if(ranf(0).gt.prbhd2) go to 100                              
           jpara=jpar2                                                  
           ia=i2                                                        
           ib=i1c(jjj)+i2c(jjj)-i2                                      
           dtaua=dtau2                                                  
           ii=2                                                         
         end if                                                         
         if(ib.le.0) go to 100                                          
         identb=issn(ib)                                                
         if(identb.le.n1old+n2old) then                                 
           jparb=identb                                                 
         else                                                           
           jparb=mod(itdth(3,identb),100000)                            
         end if                                                         
         if(jparb.gt.n1old.and.jparb.le.n1old+n2old) then               
           jparb=1                                                      
         else if(jparb.gt.0.and.jparb.le.n1old) then                    
           jparb=-1                                                     
         else                                                           
           jparb=0                                                      
         end if                                                         
         if(jpara*jparb.ne.-1) go to 100                                
         dtinv2=qdot(0,ia)**2                                           
         do 33 i=1,3                                                    
   33    dtinv2=dtinv2-qdot(i,ia)**2                                    
         if(dtinv2.le.0.0) then                                         
           if(ctime(jjj).lt.ctime(ind)) then                            
             call errex('coltim:dtinv2.le.0.0')                         
           else                                                         
             write(lun8,*) 'coltim warning:dtinv2.le.0.0'                  
           end if                                                       
           ibeqlc(ii)=1                                                 
           write(lun8,*) dtinv2,i1c(jjj),i2c(jjj)                          
           write(lun8,*) ia,issn(ia),ident1,ident2,jjj,ind                 
           write(lun8,*) 'qdot=',(qdot(i,ia),i=0,3)                        
           write(lun8,*) ctime(jjj),ctime(ind)                             
           go to 100                                                    
         end if                                                         
         dtinv2=dtinv2*(ctime(jjj)-ctime(ind))**2                       
         if(sqrt(dtinv2).lt.dtaua) then                                 
           call oldat(ii,itola,npola,p0ola,pxola,pyola,pzola,emola)     
           if(ityp(ib).eq.0) then                                       
             write(lun8,*) ib,issn(ib),jjj,ia,ident1,ident2,ii             
             write(lun8,*) ii,'coltim:',                                   
     a            itola,npola,ityp(ib),npidec(ib)                       
           end if                                                       
           call transl(-1,itola,npola,ltypa)                            
           call transl(-1,ityp(ib),npidec(ib),ltypb)                    
           call contnt(                                                 
     a            ltypa,ltypb,istra1,istra2,                            
     b                           imeso1,imeso2,ibry1,ibry2,iann)        
           icqflv=itdth(2,identb)                                       
           if(icqflv.ne.0) then                                         
             tcolb=x(0,ib)+qdot(0,ib)*                                  
     a               (ctime(jjj)-(time+dt))                             
             if(tdtg(identb).gt.tcolb) icqflv=0                         
           end if                                                       
           redcqf=1.0                                                   
           if(icqflv.ne.0) redcqf=redcqi(imeso2,icqflv)                 
           sighrd=sigasy(1,imeso1,istra1,imeso2,istra2)                 
           if(ranf(0).lt.sighrd/csig(jjj)) then                         
             iblock(ii)=1                                               
               kblock=1                                                 
               go to 200                                                
           end if                                                       
         end if                                                         
  100   continue                                                        
  200   continue                                                        
        if(kblock.eq.1) ichanl=-2                                       
      return                                                            
      end                                                               
      subroutine output(iwoher)                                         
        include 'amsrqmd.h'
       include 'scatter7'                                               
       include 'partblk7'                                               
        common /ioutbk/i1,i2,i3,i4,nop,ichanl,iexpn1,iexpn2             
     a                 ,idcid1,idcid2,iqsq                              
       common /routbk/xold1(0:3),xold2(0:3),srt,bxtot,bytot,bztot       
       logical issers                                                   
       dimension xvold(0:3)                                             
       ident1=issn(i1)                                                  
       iexpn1=itdth(2,ident1)                                           
       ident2=issn(i2)                                                  
       iexpn2=itdth(2,ident2)                                           
       if(ida(25).eq.1) then                                            
         if(nop.eq.1) then                                              
           if(ityp(i1).eq.-9999) then                                   
            ind1=i2                                                     
            do 659 i=0,3                                                
  659       xold1(i)=xold2(i)                                           
           else if(ityp(i2).eq.-9999) then                              
            ind1=i1                                                     
           end if                                                       
         end if                                                         
         if(switch(23)) then                                            
            ken=itripl                                                  
         else                                                           
            ken=kdt                                                     
         end if                                                         
         write(lun15,661) irun,ken,1,ncollt+ncoll,nop,ichanl,              
     a                    issn(i1)*1000+nclcnt(ident1),                 
     b                    issn(i2)*1000+nclcnt(ident2)                  
  661    format(1x,i3,1x,i3,1x,i1,1x,i5,1x,i3,1x,i2,1x,i8,1x,i8)        
  662    format(1x,i4,1x,i4,1x,f8.3,1x,2(f7.3,1x),f8.3,1x,              
     a               f7.3,i3,1x,i8,1x,f8.3,2(1x,f7.3),1x,f8.3)          
                                                                        
         call oldat(1,itol1,npol1,p0ol1,pxol1,pyol1,pzol1,emol1)        
         if(-npol1.gt.99) then                                          
             npol1=-mod(-npol1,100)                                     
         end if                                                         
         iexpo1=isign(1,iexpn1)*mod(iabs(iexpn1),100)                   
         write(lun15,662)                                                  
     a      itol1,npol1,p0ol1,pxol1,pyol1,pzol1,emol1,iexpo1            
         call oldat(                                                    
     a          2,itol2,npol2,p0ol2,pxol2,pyol2,pzol2,emol2)            
         if(-npol2.gt.99) then                                          
           npol2=-mod(-npol2,100)                                       
         end if                                                         
         iexpo2=isign(1,iexpn2)*mod(iabs(iexpn2),100)                   
         write(lun15,662)                                                  
     a          itol2,npol2,p0ol2,pxol2,pyol2,pzol2,emol2,iexpo2        
         if(nop.eq.1) then                                              
           if(-npidec(ind1).gt.99) then                                 
             npidl=-mod(-npidec(ind1),100)                              
           else                                                         
             npidl=npidec(ind1)                                         
           end if                                                       
           if(ind1.eq.i1)  iexpo=iexpo1                                 
           if(ind1.eq.i2)  iexpo=iexpo2                                 
           write(15,662)ityp(ind1),npidl,(p(i,ind1),i=0,3),             
     a          em(ind1),iexpo,issn(ind1),xold1(0),                     
     b                  xold1(1),xold1(2),xold1(3)                      
         else                                                           
           if(-npidec(i1).gt.99) then                                   
             npidl=-mod(-npidec(i1),100)                                
           else                                                         
             npidl=npidec(i1)                                           
           end if                                                       
           if(-ityp(i1).eq.9999) then                                   
             itypl=9999                                                 
           else                                                         
             itypl=ityp(i1)                                             
           end if                                                       
           write(lun15,662) itypl,npidl,(p(i,i1),i=0,3),                   
     a                em(i1),iexpo1,issn(i1),xold1(0),                  
     b                        xold1(1),xold1(2),xold1(3)                
           if(-npidec(i2).gt.99) then                                   
             npidl=-mod(-npidec(i2),100)                                
           else                                                         
             npidl=npidec(i2)                                           
           end if                                                       
           if(-ityp(i2).eq.9999) then                                   
             itypl=9999                                                 
           else                                                         
             itypl=ityp(i2)                                             
           end if                                                       
           write(lun15,662) itypl,npidl,(p(i,i2),i=0,3),                   
     a              em(i2),iexpo2,issn(i2),xold2(0),                    
     b                        xold2(1),xold2(2),xold2(3)                
         end if                                                         
         if(nop.gt.2) then                                              
           write(lun15,662) ityp(i3),npidec(i3),(p(i,i3),i=0,3),           
     a                    em(i3),0,issn(i3),x(0,i3),                    
     b                        x(1,i3),x(2,i3),x(3,i3)                   
         end if                                                         
         if(nop.ge.4) then                                              
           do 513 i4a=i4,i4+nop-4                                       
            write(lun15,662)                                               
     a         ityp(i4a),npidec(i4a),(p(i,i4a),i=0,3),                  
     b             em(i4a),0,issn(i4a),x(0,i4a),                        
     c                   x(1,i4a),x(2,i4a),x(3,i4a)                     
  513      continue                                                     
         end if                                                         
         if(ida(36).eq.1) then                                          
            call dnscal(1,i1,i2,dens1)                                  
            call dnscal(1,i2,i1,dens2)                                  
            write(lun15,663) dens1,dens2                                   
         end if                                                         
       end if                                                           
       emi1=em(i1)                                                      
       emi2=em(i2)                                                      
       if(ida(38).eq.3.or.ida(38).eq.4) then                            
         kstor(ident1,1)=1                                              
                  kstor(ident2,1)=1                                     
                  do 203 i=1,4                                          
  203             stork(ident1,i)=xold1(i-1)                            
                  do 303 i=6,8                                          
  303             stork(ident1,i)=p(i-5,i1)                             
                  do 204 i=1,4                                          
  204             stork(ident2,i)=xold2(i-1)                            
                  do 304 i=6,8                                          
  304             stork(ident2,i)=p(i-5,i2)                             
                  if(nop.gt.2) then                                     
                    ident3=issn(i3)                                     
                    kstor(ident3,1)=1                                   
                    do 205 i=1,4                                        
  205               stork(ident3,i)=x(i-1,i3)                           
                    do 305 i=6,8                                        
  305               stork(ident3,i)=p(i-5,i3)                           
                  end if                                                
                  if(nop.gt.3) then                                     
                    ident4=issn(i4)                                     
                    kstor(ident4,1)=1                                   
                    do 206 i=1,4                                        
  206               stork(ident4,i)=x(i-1,i4)                           
                    do 306 i=6,8                                        
  306               stork(ident4,i)=p(i-5,i4)                           
                  end if                                                
       else if((ida(38).eq.1.or.ida(38).eq.2.or.                        
     a            ida(38).eq.5.or.ida(38).eq.7)                         
     b                                     .and.ichanl.gt.1) then       
           if(ida(38).eq.1.or.ida(38).eq.2) then                        
             jeng=1                                                     
             keng=1                                                     
           else if(ida(38).eq.5) then                                   
             jeng=0                                                     
             keng=0                                                     
           else if(ida(38).eq.7) then                                   
             if(nop.ge.2) then                                          
               jeng=1                                                   
             else                                                       
               jeng=0                                                   
             end if                                                     
             keng=1                                                     
           end if                                                       
           if(issers(jeng,ida(38),itol1,npol1,emol1).and.               
     a     .not.issers(jeng,ida(38),ityp(i1),npidec(i1),emi1).and.      
     b     .not.issers(jeng,ida(38),ityp(i2),npidec(i2),emi2))then      
             write(lun13,888) ident1,0,itol1,npol1,                        
     a                               p0ol1,-pxol1,-pyol1,-pzol1         
             write(lun13,889) (stork(ident1,i),i=1,5)                      
             write(lun13,890) (kstor(ident1,i),i=1,7)                      
             isser1=1                                                   
           else                                                         
             isser1=0                                                   
           end if                                                       
           if(issers(jeng,ida(38),itol2,npol2,emol2).and.               
     a     .not.issers(jeng,ida(38),ityp(i1),npidec(i1),emi1).and.      
     b     .not.issers(jeng,ida(38),ityp(i2),npidec(i2),emi2))then      
             write(lun13,888) ident2,0,itol2,npol2,                        
     a                          p0ol2,-pxol2,-pyol2,-pzol2              
             write(lun13,889) (stork(ident2,i),i=1,5)                      
             write(lun13,890) (kstor(ident2,i),i=1,7)                      
             isser2=1                                                   
           else                                                         
             isser2=0                                                   
           end if                                                       
           if((.not.issers(keng,ida(38),itol1,npol1,emol1).and.         
     a        .not.issers(keng,ida(38),itol2,npol2,emol2)).or.          
     b         isser1+isser2.ne.0.or.                                   
     c     (.not.issers(keng,ida(38),ityp(i1),npidec(i1),emi1).and.     
     d      .not.issers(keng,ida(38),ityp(i2),npidec(i2),emi2)))then    
                   stork(ident1,1)=srt                                  
                   stork(ident1,2)=emol1                                
                   stork(ident1,3)=emol2                                
                   stork(ident1,4)=em(i1)                               
                   stork(ident1,5)=em(i2)                               
                   kstor(ident1,1)=itol1                                
                   kstor(ident1,2)=npol1                                
                   kstor(ident1,3)=itol2                                
                   kstor(ident1,4)=npol2                                
                   kstor(ident1,5)=ichanl                               
                   kstor(ident1,6)=itdth(2,ident1)                      
                   kstor(ident1,7)=itdth(2,ident2)                      
                   stork(ident2,1)=srt                                  
                   stork(ident2,2)=emol1                                
                   stork(ident2,3)=emol2                                
                   stork(ident2,4)=em(i1)                               
                   stork(ident2,5)=em(i2)                               
                   kstor(ident2,1)=itol1                                
                   kstor(ident2,2)=npol1                                
                   kstor(ident2,3)=itol2                                
                   kstor(ident2,4)=npol2                                
                   kstor(ident2,5)=ichanl                               
                   kstor(ident2,6)=itdth(2,ident1)                      
                   kstor(ident2,7)=itdth(2,ident2)                      
           end if                                                       
           if(nop.ge.3) then                                            
                     ident3=issn(i3)                                    
                     stork(ident3,1)=srt                                
                     stork(ident3,2)=emol1                              
                     stork(ident3,3)=emol2                              
                     stork(ident3,4)=em(i1)                             
                     stork(ident3,5)=em(i2)                             
                     kstor(ident3,1)=itol1                              
                     kstor(ident3,2)=npol1                              
                     kstor(ident3,3)=itol2                              
                     kstor(ident3,4)=npol2                              
                     kstor(ident3,5)=ichanl                             
                     kstor(ident3,6)=itdth(2,ident1)                    
                     kstor(ident3,7)=itdth(2,ident2)                    
           end if                                                       
           if(nop.ge.4) then                                            
                    do 515 i4a=i4,i4+nop-4                              
                     ident4=issn(i4a)                                   
                     stork(ident4,1)=srt                                
                     stork(ident4,2)=emol1                              
                     stork(ident4,3)=emol2                              
                     stork(ident4,4)=em(i1)                             
                     stork(ident4,5)=em(i2)                             
                     kstor(ident4,1)=itol1                              
                     kstor(ident4,2)=npol1                              
                     kstor(ident4,3)=itol2                              
                     kstor(ident4,4)=npol2                              
                     kstor(ident4,5)=ichanl                             
                     kstor(ident4,6)=itdth(2,ident1)                    
                     kstor(ident4,7)=itdth(2,ident2)                    
  515               continue                                            
           end if                                                       
           if(isser1.eq.1) then                                         
             if(nop.ge.2) then                                          
               kphid=-2                                                 
             else                                                       
               kphid=-1                                                 
             end if                                                     
             write(lun13,888) ident1,kphid,itol1,npol1,                    
     a                             p0ol1,-pxol1,-pyol1,-pzol1           
             write(lun13,889) (stork(ident1,i),i=1,5)                      
             write(lun13,890) (kstor(ident1,i),i=1,7)                      
           end if                                                       
           if(isser2.eq.1) then                                         
             if(nop.ge.2) then                                          
               kphid=-2                                                 
             else                                                       
               kphid=-1                                                 
             end if                                                     
             write(lun13,888) ident2,kphid,itol2,npol2,                    
     a                        p0ol2,-pxol2,-pyol2,-pzol2                
             write(lun13,889) (stork(ident2,i),i=1,5)                      
             write(lun13,890) (kstor(ident2,i),i=1,7)                      
           end if                                                       
  888      format(1x,4(i7,1x),1x,4(f7.3,1x))                            
  889      format(1x,5(f7.3,1x))                                        
  890      format(1x,5(i7,1x),2(i4,1x))                                 
       end if                                                           
  663  format(1x,2(f6.3,1x))                                            
       if(ida(42).eq.1) then                                            
           write(7)                                                     
     &              ityp(i1),issn(i1),npidec(i1),                       
     &              x(0,i1),x(1,i1),x(2,i1),x(3,i1),                    
     &              p(0,i1),p(1,i1),p(2,i1),p(3,i1),em(i1),             
     &              tdtg(ident1),itdth(2,ident1),itdth(3,ident1)        
           write(lun7)                                                     
     &              ityp(i2),issn(i2),npidec(i2),                       
     &              x(0,i2),x(1,i2),x(2,i2),x(3,i2),                    
     &              p(0,i2),p(1,i2),p(2,i2),p(3,i2),em(i2),             
     &              tdtg(ident2),itdth(2,ident2),itdth(3,ident2)        
           if(nop.gt.2) then                                            
             write(lun7)                                                   
     &              ityp(i3),issn(i3),npidec(i3),                       
     &              x(0,i3),x(1,i3),x(2,i3),x(3,i3),                    
     &              p(0,i3),p(1,i3),p(2,i3),p(3,i3),em(i3),             
     &              0.0,0,0                                             
             if(nop.ge.4) then                                          
               do 517 i4a=i4,i4+nop-4                                   
                write(lun7)                                                
     &                 ityp(i4a),issn(i4a),npidec(i4a),                 
     &                 x(0,i4a),x(1,i4a),x(2,i4a),x(3,i4a),             
     &                 p(0,i4a),p(1,i4a),p(2,i4a),p(3,i4a),             
     &                 em(i4a),0.0,0,0                                  
  517          continue                                                 
             end if                                                     
           end if                                                       
       end if                                                           
      return                                                            
      end                                                               
      subroutine xtupdt                                                 
        include 'amsrqmd.h'
       include 'scatter7'                                               
       include 'partblk7'                                               
       include 'amsrqmd.h'
       common /ioutbk/i1,i2,i3,i4,nop,ichanl,iexpn1,iexpn2              
     a                 ,idcid1,idcid2,iqsq                              
       common /routbk/xold1(0:3),xold2(0:3),srt,bxtot,bytot,bztot       
       ident1=issn(i1)                                                  
       ident2=issn(i2)                                                  
       itdt31=itdth(3,ident1)                                           
       itdt32=itdth(3,ident2)                                           
         nclcnt(ident1)=nclcnt(ident1)+1                                
         nclcnt(ident2)=nclcnt(ident2)+1                                
         ncoll=ncoll+1                                                  
         itdth(2,ident1)=iexpn1                                         
         itdth(2,ident2)=iexpn2                                         
         itdth(5,ident1)=0                                              
         itdth(5,ident2)=0                                              
         if(iexpn1.eq.0) then                                           
           tdtg(ident1)=0.0                                             
           itdth(1,ident1)=0                                            
           itdth(4,ident1)=0                                            
         end if                                                         
         if(iexpn2.eq.0) then                                           
           tdtg(ident2)=0.0                                             
           itdth(1,ident2)=0                                            
           itdth(4,ident2)=0                                            
         end if                                                         
         if(ichanl.gt.1) then                                           
           if(itdt31.ne.0) then                                         
             itd1=mod(itdt31,100000)                                    
           else                                                         
             itd1=ident1                                                
           end if                                                       
           if(itdt32.ne.0) then                                         
             itd2=mod(itdt32,100000)                                    
           else                                                         
             itd2=ident2                                                
           end if                                                       
           itdth(3,ident1)=100000*itd2+itd1                             
           itdth(3,ident2)=100000*itd1+itd2                             
         end if                                                         
         do 163 i=0,3                                                   
          x(i,i1)=xold1(i)                                              
          x(i,i2)=xold2(i)                                              
  163    continue                                                       
         tlast(i1)=xold1(0)                                             
         tlast(i2)=xold2(0)                                             
         if(iexpn1.eq.0) then                                           
           do  164 i=0,3                                                
  164      qdot(i,i1)=0.                                                
         end if                                                         
         if(iexpn2.eq.0) then                                           
           do  165 i=0,3                                                
  165      qdot(i,i2)=0.                                                
         end if                                                         
         if(p(0,i1).gt.0.01) then                                       
           ylast(ident1)=0.5*log((p(0,i1)+p(3,i1))/                     
     a                               (p(0,i1)-p(3,i1)))                 
         end if                                                         
         zlast(ident1)=x(3,i1)-(pnorm(0)*x(0,i1)-                       
     a                                 pnorm(3)*x(3,i1))*pnorm(3)       
         if(p(0,i2).gt.0.01) then                                       
           ylast(ident2)=0.5*log((p(0,i2)+p(3,i2))/                     
     a                                (p(0,i2)-p(3,i2)))                
         end if                                                         
         zlast(ident2)=x(3,i2)-(pnorm(0)*x(0,i2)-                       
     a                                 pnorm(3)*x(3,i2))*pnorm(3)       
         if(iqcnvt.eq.1) then                                           
           emi1=em(i1)                                                  
           emi2=em(i2)                                                  
           call qinvrt(ityp(i1),npidec(i1),emi1)                        
           call qinvrt(ityp(i2),npidec(i2),emi2)                        
           if((ityp(i1).eq.98.and.idcid1.eq.0).or.                      
     a            (ityp(i2).eq.98.and.idcid2.eq.0)) then                
             call errex('coll: charge reversed string, no init.')       
           end if                                                       
         end if                                                         
         if(nop.ge.3) then                                              
           if(iqcnvt.eq.1) then                                         
             em3=em(i3)                                                 
             call qinvrt(ityp(i3),npidec(i3),em3)                       
           end if                                                       
           if(x(0,i3).eq.0.) then                                       
             do  166 i=0,3                                              
              x(i,i3)=x(i,i1)                                           
              qdot(i,i3)=0.                                             
  166        continue                                                   
           else                                                         
             do  265 i=0,3                                              
              x(i,i3)=x(i,i3)+qdot(i,i3)*dtprop                         
              qdot(i,i3)=0.                                             
  265        continue                                                   
           end if                                                       
           tlast(i3)=x(0,i3)                                            
           ident3=issn(i3)                                              
           tdtg(ident3)=0.0                                             
           itdth(1,ident3)=0                                            
           itdth(2,ident3)=0                                            
           if(iabs(npidec(i3)).ne.40) then                              
             itdth(3,ident3)=0                                          
           end if                                                       
           itdth(5,ident3)=0                                            
           itdth(4,ident3)=0                                            
           ylast(ident3)=0.5*log((p(0,i3)+p(3,i3))/                     
     a                                (p(0,i3)-p(3,i3)))                
           zlast(ident3)=x(3,i3)-(pnorm(0)*x(0,i3)-                     
     a                        pnorm(3)*x(3,i3))*pnorm(3)                
           if(nop.ge.4) then                                            
             do 511 i4a=i4,i4+nop-4                                     
              if(iqcnvt.eq.1) then                                      
                em4=em(i4a)                                             
                call qinvrt(ityp(i4a),npidec(i4a),em4)                  
              end if                                                    
              if(x(0,i4a).eq.0.) then                                   
                do  167 i=0,3                                           
                 x(i,i4a)=x(i,i2)                                       
                 qdot(i,i4a)=0.                                         
  167           continue                                                
              else                                                      
                do  266 i=0,3                                           
                 x(i,i4a)=x(i,i4a)+qdot(i,i4a)*dtprop                   
                 qdot(i,i4a)=0.                                         
  266           continue                                                
              end if                                                    
              tlast(i4a)=x(0,i4a)                                       
              ident4=issn(i4a)                                          
              tdtg(ident4)=0.0                                          
              itdth(1,ident4)=0                                         
              itdth(2,ident4)=0                                         
              if(iabs(npidec(i4a)).ne.40) then                          
                itdth(3,ident4)=0                                       
              end if                                                    
              itdth(5,ident4)=0                                         
              itdth(4,ident4)=0                                         
              ylast(ident4)=0.5*log((p(0,i4a)+p(3,i4a))/                
     a                                         (p(0,i4a)-p(3,i4a)))     
              zlast(ident4)=x(3,i4a)-(pnorm(0)*x(0,i4a)-                
     a                                pnorm(3)*x(3,i4a))*pnorm(3)       
  511        continue                                                   
           end if                                                       
         end if                                                         
      return                                                            
      end                                                               
