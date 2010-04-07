CPAT,MSCSTR   .                                                         
      subroutine mscstr(imscst,i1,i2,isccss)                            
      include 'scatter7'                                                
      include 'partblk7'                                                
      dimension p1(0:3),p2(0:3)                                         
      logical  potyes,poy(2)                                            
      dimension iqq(2),ident(2)                                         
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 2.3  -- 05-sep-96'/                     
      data pronam/'module: m s c s t r'/                                
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
           if(switch(6)) then                                           
             isccss=2                                                   
             return                                                     
           end if                                                       
           isccss=0                                                     
           ident1=issn(i1)                                              
           ident2=issn(i2)                                              
           poy(1)=.false.                                               
           poy(2)=.false.                                               
           irturn=0                                                     
           if(.not.potyes(i1,iqq1)) then                                
             if(.not.potyes(i2,iqq2)) then                              
               isccss=2                                                 
               irturn=3                                                 
             else                                                       
               isccss=1                                                 
               irturn=1                                                 
             end if                                                     
           else                                                         
             poy(1)=.true.                                              
             if(.not.potyes(i2,iqq2)) then                              
               irturn=2                                                 
             else                                                       
               poy(2)=.true.                                            
             end if                                                     
           end if                                                       
           if(irturn.ne.0) then                                         
             if(imscst.eq.0) return                                     
             if(mod(irturn,2).eq.1) then                                
               if(iqq1.eq.3.or.iqq1.eq.1) return                        
               if(iqq2.eq.3.or.iqq2.eq.1) return                        
             end if                                                     
             if(irturn.eq.2) then                                       
               if(iqq2.eq.3.or.iqq2.eq.1) return                        
             end if                                                     
           end if                                                       
      iqq(1)=iqq1                                                       
      iqq(2)=iqq2                                                       
      ident(1)=ident1                                                   
      ident(2)=ident2                                                   
      il=i1                                                             
      iloop=1                                                           
 8200 continue                                                          
      pp=0.                                                             
      if(iqq(iloop).eq.1) then                                          
        pilsq=(p(0,il)+frfp(0,ident(iloop)))**2                         
        do 8111 i=1,3                                                   
 8111   pp=pp+(p(i,il)+frfp(i,ident(iloop)))**2                         
      else                                                              
        pilsq=p(0,il)**2                                                
        do 8211 i=1,3                                                   
 8211   pp=pp+p(i,il)**2                                                
      end if                                                            
      pilsq=pilsq-pp                                                    
      if(ityp(il).eq.99.and.npidec(il).eq.1) pilsq=0.                   
      if(poy(iloop)) then                                               
        vil=vquasi(il,densa,densb,densc)                                
      else                                                              
        vil=0.0                                                         
      end if                                                            
      eml=em(il)                                                        
      arg=pilsq-vil                                                     
      if(arg.lt.0.) then                                                
        emnew=-1.                                                       
      else                                                              
        emnew=sqrt(arg)                                                 
      end if                                                            
         itypl=ityp(il)                                                 
         npidl=npidec(il)                                               
         iqql=iqq(iloop)                                                
         newm=newmas(itypl,npidl,iqql,emnew)                            
         if(newm.eq.1) then                                             
            if(iloop.eq.1) isccss=1                                     
            em(il)=emnew                                                
         end if                                                         
 8210 continue                                                          
      if(iloop.eq.1) then                                               
         iloop=2                                                        
         em1eff= sqrt(em(il)*em(il)+vil)                                
         pi1sq=pilsq                                                    
         il=i2                                                          
         go to 8200                                                     
      else                                                              
         em2eff=sqrt(em(il)*em(il)+vil )                                
         pi2sq=pilsq                                                    
      end if                                                            
      if(iqq1.eq.1) then                                                
        call ofshel(1,i1,ident1)                                        
      end if                                                            
      if(iqq2.eq.1) then                                                
        call ofshel(1,i2,ident2)                                        
      end if                                                            
   33 continue                                                          
      if(abs(em1eff-sqrt(pi1sq)).lt.1.e-3.and.                          
     a               abs(em2eff-sqrt(pi2sq)).lt.1.e-3                   
     b              .and.(imscst.eq.0.or.pcorr(0).gt.0.0)               
     c                                            ) then                
         isccss=2                                                       
         go to 44                                                       
      end if                                                            
      do 30 i=0,3                                                       
        p1(i)=p(i,i1)                                                   
        p2(i)=p(i,i2)                                                   
   30 continue                                                          
      if(imscst.ne.0) then                                              
        if(pcorr(0).lt.0.0) then                                        
          p0tot=p1(0)+p2(0)                                             
          p1tot=p1(1)+p2(1)                                             
          p2tot=p1(2)+p2(2)                                             
          p3tot=p1(3)+p2(3)                                             
          deltp0=                                                       
     a       delten(2,p0tot,p1tot,p2tot,p3tot,em1eff,em2eff,srtpr)      
          pcorr(0)=pcorr(0)+deltp0                                      
          p1(0)=p1(0)-0.5*deltp0                                        
          p2(0)=p2(0)-0.5*deltp0                                        
        end if                                                          
      end if                                                            
      call onshel(1,isccss,em1eff,em2eff,p1(0),p1(1),p1(2),p1(3),       
     a                                       p2(0),p2(1),p2(2),p2(3))   
      if(isccss.eq.1) then                                              
       do 31 i=0,3                                                      
         p(i,i1)=p1(i)                                                  
         p(i,i2)=p2(i)                                                  
   31  continue                                                         
       isccss=2                                                         
      end if                                                            
   44 continue                                                          
      if(iqq1.eq.1) then                                                
        do 51 i=0,3                                                     
          p(i,i1)=p(i,i1)-frfp(i,ident1)                                
   51   continue                                                        
        pp=p(0,i1)**2                                                   
        do 52 i=1,3                                                     
   52     pp=pp-p(i,i1)**2                                              
        if(pp.lt.0.3) call ofshel(9,i1,ident1)                          
      end if                                                            
      if(iqq2.eq.1) then                                                
        do 53 i=0,3                                                     
          p(i,i2)=p(i,i2)-frfp(i,ident2)                                
   53   continue                                                        
        pp=p(0,i2)**2                                                   
        do 54 i=1,3                                                     
   54     pp=pp-p(i,i2)**2                                              
        if(pp.lt.0.3) call ofshel(9,i2,ident2)                          
      end if                                                            
      return                                                            
      end                                                               
      logical function jets(ityp1,npid1,em1,ipot)                       
      include 'scatter7'                                                
       npida=iabs(npid1)                                                
       npidr1=-mod(-npid1,1000)                                         
           if((ityp1.le.11.and.(npid1.eq.3.or.npid1.eq.4)).or.          
     a        (ityp1.ge.10.and.ityp1.le.11.and.-npidr1.eq.10).or.       
     b        (ityp1.ge.3.and.ityp1.le.6.and.-npidr1.ge.6.and.          
     c                                       -npidr1.le.9)) then        
             if(em1.lt.emstrm.and.((ityp1.ge.10.and.ityp1.le.11).or.    
     a            (ityp1.ge.3.and.ityp1.le.6)).and.ipot.eq.1) then      
               jets=.false.                                             
             else if(npid1.lt.-1000) then                               
               jets=.false.                                             
             else                                                       
               jets=.true.                                              
             end if                                                     
           else if(ityp1.le.15) then                                    
             if((ityp1.eq.12.and.em1.gt.emkdec).or.                     
     e          (ityp1.eq.13.and.em1.gt.emlamb+empion).or.              
     f          (ityp1.eq.15.and.em1.gt.emlamb+empion).or.              
     g          (ityp1.eq.14.and.em1.gt.emkdec)                         
     h                                         ) then                   
               jets=.true.                                              
             else                                                       
               jets=.false.                                             
             end if                                                     
           else if(ityp1.ge.85.and.ityp1.le.97) then                    
             if((npida.ge.41.and.npida.le.42.and.                       
     b           ityp1.eq.94).or.                                       
     c          (npida.ge.61.and.npida.le.64.and.                       
     d           ityp1.ge.90.and.ityp1.le.92).or.                       
     e          (npida.eq.57.and.                                       
     f          (ityp1.ge.86                                            
     g                                                 )).or.           
     h          ((ityp1.ge.85)                                          
     i             .and.npida.ge.43.and.npida.le.45).or.                
     j          (((ityp1.ge.89.and.ityp1.le.92).or.ityp1.eq.85)         
     k          .and.(npida.eq.46.or.npida.eq.47)).or.                  
     h          ((ityp1.ge.85)                                          
     i             .and.npida.ge.65.and.npida.le.67).or.                
     l          ((ityp1.ge.85.and.ityp1.ne.88).and.                     
     m             npida.ge.68.and.npida.le.69).or.                     
     n          (ityp1.ge.85.and.ityp1.le.94.and.npida.eq.70)) then     
               jets=.true.                                              
             else                                                       
               jets=.false.                                             
             end if                                                     
           else if(ityp1.eq.98) then                                    
             jets=.true.                                                
           else                                                         
              jets=.false.                                              
           end if                                                       
      return                                                            
      end                                                               
      subroutine onshel(iwoher,isccss,em1,em2,p0i1,px1,py1,pz1,         
     a                                                p0i2,px2,py2,pz2) 
        include 'scatter7'                                              
            p0tot= p0i1+p0i2                                            
            p1tot=px1+px2                                               
            p2tot=py1+py2                                               
            p3tot=pz1+pz2                                               
            srt=(p0tot*p0tot-p1tot*p1tot-p2tot*p2tot-p3tot*p3tot)       
            if(srt.le.(em1+em2)**2) then                                
                isccss=0                                                
                return                                                  
            end if                                                      
            srt=sqrt(srt)                                               
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
            if(pr.lt.1.e-9) then                                        
                isccss=0                                                
                return                                                  
            end if                                                      
            prnew=pcmsr(srt,em1,em2)                                    
            pxr=pxr/pr* prnew                                           
            pyr=pyr/pr* prnew                                           
            pzr=pzr/pr* prnew                                           
              pb=bx*pxr+by*pyr+bz*pzr                                   
              ea=sqrt(em1*em1+pxr**2+pyr**2+pzr**2)                     
              gg=(gam-1.)*pb/b2+gam*ea                                  
              px1=gg*bx+pxr                                             
              py1=gg*by+pyr                                             
              pz1=gg*bz+pzr                                             
              pp=px1*px1+py1*py1+ pz1*pz1                               
              arg=em1*em1+pp                                            
              p0i1=sqrt(arg)                                            
              eb=sqrt(em2*em2+pxr**2+pyr**2+pzr**2)                     
              gg=(gam-1.)*(-pb)/b2+gam*eb                               
              px2=gg*bx-pxr                                             
              py2=gg*by-pyr                                             
              pz2=gg*bz-pzr                                             
              pp= px2*px2+py2*py2+ pz2*pz2                              
              p0i2=sqrt(em2*em2+pp)                                     
              p0out=p0i1+p0i2                                           
              if((p0tot/ p0out .gt. 1.01).or.                           
     a                               (p0tot/ p0out .lt. 0.99)) then     
                 write(8,*) p0out,p0tot,'energy after and before coll.' 
                 p3out=pz1+pz2                                          
                 write(8,*) p3out,p3tot,'z-mom. after and before coll.' 
                 call errex('no energy conservation in onshel')         
              end if                                                    
              isccss=1                                                  
      return                                                            
      end                                                               
      function vquasi(ill,densl2,densl3,denst)                          
      include 'scatter7'                                                
      real*8 plk,qlk,dqlksq,splksq,dqsplk,zlk                           
      logical potyes                                                    
      include 'partblk7'                                                
      dimension qlk(0:3),plk(0:3)                                       
      itypl=ityp(ill)                                                   
      npidl=npidec(ill)                                                 
      eml=em(ill)                                                       
      identl=issn(ill)                                                  
      denst=0.                                                          
      densl3=0.                                                         
      densl2=0.                                                         
      vil=0.                                                            
      if(.not.potyes(ill,iqql)) go to 8210                              
      kpotl=kpote(itypl,npidl,iqql)                                     
      do 8209 kk=1,nrbar(kclst)                                         
      ikk=kk+iofset                                                     
      if(ill.eq.ikk) go to 8209                                         
      itypk=ityp(ikk)                                                   
      npidk=npidec(ikk)                                                 
      emk=em(ikk)                                                       
      identk=issn(ikk)                                                  
      if(.not.potyes(ikk,iqqk)) go to 8209                              
           kpotk=kpote(itypk,npidk,iqqk)                                
           kpot=kpotl+kpotk                                             
           do 8208 i=0,3                                                
            qlk(i)=x(i,ill)-x(i,ikk)+(qdot(i,ill)-qdot(i,ikk))*dtprop   
            plk(i)=p(i,ill)+p(i,ikk)                                    
 8208      continue                                                     
           if(iqql.eq.1) then                                           
             do 8206 i=0,3                                              
 8206        plk(i)=plk(i)+frfp(i,identl)                               
           end if                                                       
           if(iqqk.eq.1) then                                           
             do 8205 i=0,3                                              
 8205        plk(i)=plk(i)+frfp(i,identk)                               
           end if                                                       
           dqlksq=qlk(0)*qlk(0)                                         
           splksq=plk(0)*plk(0)                                         
           dqsplk=qlk(0)*plk(0)                                         
           qplkvc=0.                                                    
           do 8207 i=1,3                                                
            dqlksq=dqlksq-qlk(i)*qlk(i)                                 
            splksq=splksq-plk(i)*plk(i)                                 
            qplkvc=qplkvc+qlk(i)*plk(i)                                 
 8207      continue                                                     
           dqsplk=dqsplk-qplkvc                                         
           zlk=dqlksq-dqsplk*dqsplk/splksq                              
           if(-zlk.lt.dstint) then                                      
             pr2=(.25*(splksq-(emk*emk+eml*eml))**2 -                   
     a           emk*emk*eml*eml)/splksq                                
             cofact=exp(zlk/alpot)                                      
             denst=denst+cofact                                         
              pilsq=p(0,ill)*p(0,ill)                                   
              piksq=p(0,ikk)*p(0,ikk)                                   
              do  427 i=1,3                                             
               pilsq=pilsq-p(i,ill)*p(i,ill)                            
               piksq=piksq-p(i,ikk)*p(i,ikk)                            
  427         continue                                                  
              emil=sqrt(max(0.0,pilsq))                                 
              emik=sqrt(max(0.0,piksq))                                 
              srtlk=sqrt(splksq)                                        
              if(srtlk.gt.emil+emik) then                               
               prlk=pcmsr(srtlk,emil,emik)                              
              else                                                      
                prlk=0.0                                                
              end if                                                    
              valk=prlk                                                 
             if((ida(12).eq.1.and.valk.gt.pr2max).or.                   
     a                                    ida(12).eq.2) then            
                if(kpot.eq.2.or.kpot.eq.6.or.                           
     a                                  kpot.eq.3) then                 
                  densl2=densl2+cofact*da(13)*coef3/coef1               
                  densl3=densl3+cofact*da(16)                           
                else if(kpot.eq.1) then                                 
                  densl2=densl2+cofact*da(14)*coef3/coef1               
                  densl3=densl3+cofact                                  
                else                                                    
                   densl2=densl2+cofact*coef3/coef1                     
                   densl3=densl3+cofact                                 
                end if                                                  
             else                                                       
                densl3=densl3+cofact                                    
                densl2=densl2+cofact                                    
             end if                                                     
        end if                                                          
 8209 continue                                                          
      denspp(identl)=densl3                                             
      vil=coef1*densl2+coef2*(densl3/salpot)**gamma                     
      vquasi=vil                                                        
      return                                                            
 8210 continue                                                          
      vquasi=vil                                                        
      return                                                            
      end                                                               
      subroutine chkmsc(betfb)                                          
      include 'scatter7'                                                
      include 'partblk7'                                                
      logical  potyes                                                   
         niop=0                                                         
         nprop=0                                                        
         denslm=0.                                                      
         denstm=0.                                                      
         vilm=0.                                                        
         hilm=0.                                                        
         ptlm=0.                                                        
         edif=0.                                                        
         ntt=nrbar(kclst)                                               
         gamfb=1./sqrt(1.-betfb*betfb)                                  
         do 2130 l=1,ntt                                                
          il=iofset+l                                                   
          if(.not.potyes(il,iqql)) go to 2130                           
          identl=issn(il)                                               
          eml=em(il)                                                    
          pp=0.                                                         
          if(iqql.eq.1) then                                            
            pilsq=(p(0,il)+frfp(0,identl))**2                           
            do 2713 i=1,3                                               
 2713       pp=pp+(p(i,il)+frfp(i,identl))**2                           
          else                                                          
            pilsq=p(0,il)**2                                            
            do 2712 i=1,3                                               
 2712       pp=pp+p(i,il)*p(i,il)                                       
          end if                                                        
          pilsq=pilsq-pp                                                
          vil=vquasi(il,dens2,dens3,denst)                              
          hil=pilsq-em(il)**2-vil                                       
            yl=0.5*log((p(0,il)+p(3,il))/(p(0,il)-p(3,il)))             
              ptl=sqrt(p(1,il)**2+p(2,il)**2)                           
              niop=niop+1                                               
              denslm=denslm+dens3/salpot/r0cor                          
              denstm=denstm+denst/salpot/r0cor                          
              vilm=vilm+vil                                             
              hilm=hilm+abs(hil)                                        
              ptlm=ptlm+ptl                                             
              p0=gamfb*(p(0,il)+betfb*p(3,il))                          
              p3=gamfb*(p(3,il)+betfb*p(0,il))                          
              ed=p0-sqrt(em(il)**2+vil+p3**2+ptl*ptl)                   
              edif=edif+abs(ed)                                         
 2130    continue                                                       
         if(niop.gt.0) then                                             
           write(6,*)'chkmsc:',niop,ntt,'edif=',edif,vilm/niop,hilm/niop
           write(6,*)  't mn dns=',                                     
     b          denstm/niop,denslm/niop                                 
         end if                                                         
      return                                                            
      end                                                               
      logical function potyes(il,iqql)                                  
      include 'scatter7'                                                
      include 'partblk7'                                                
      logical jets                                                      
       data emdumm/ 0.0/                                                
       potyes=.false.                                                   
       iqql=0                                                           
       if(switch(6)) return                                             
       if(il.le.0.or.il.gt.ntotma) then                                 
         call errex('potyes: wrong input')                              
       end if                                                           
       itypl=ityp(il)                                                   
       npidl=npidec(il)                                                 
       call oldnew(1,itypl,npidl,emdumm,kmultl,ltypl)                   
       identl=issn(il)                                                  
       if(itypl.eq.-9999) then                                          
         iqql=3                                                         
         return                                                         
       else if(itdth(2,identl).ne.0.and.                                
     a               tdtg(identl).gt.x(0,il)+qdot(0,il)*dtprop) then    
         iqql=1                                                         
         return                                                         
       else if(kmultl.lt.0) then                                        
         iqql=2                                                         
         return                                                         
       else if(il-iofset.gt.nrbar(kclst)) then                          
         iqql=4                                                         
         return                                                         
       else if(ida(13).eq.1.and.(kmultl.gt.0.or.                        
     a                           ltypl.lt.41.or.                        
     b                           (ltypl.gt.42.and.ltypl.lt.61).or.      
     c                                            ltypl.gt.64)) then    
         iqql=4                                                         
         return                                                         
       else if(ida(13).eq.2.and.                                        
     a                          (ltypl.lt.41.or.                        
     b                           (ltypl.gt.42.and.ltypl.lt.61).or.      
     c                                            ltypl.gt.64)) then    
         iqql=4                                                         
         return                                                         
       end if                                                           
       potyes=.true.                                                    
      return                                                            
      end                                                               
      function newmas(itypl,npidl,iqql,emnew)                           
       include 'scatter7'                                               
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       data emdumm/ 0.0/                                                
       call oldnew(1,itypl,npidl,emdumm,kmultl,ltypl)                   
       newmas=0                                                         
       if(iqql.eq.1) then                                               
       else if(kmultl.eq.0) then                                        
       else if(iabs(ltypl).le.40) then                                  
       else                                                             
         idn=idnbar(ltypl)                                              
         if(idn.gt.0) then                                              
           embm=embmin(idn)+ekinmi                                      
           if(emnew.gt.embm) newmas=1                                   
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function kpote(itypl,npidl,iqql)                                  
       include 'scatter7'                                               
       data emdumm/ 0.0/                                                
       if(ida(35).eq.1) then                                            
         kpote=0                                                        
         return                                                         
       end if                                                           
       call oldnew(1,itypl,npidl,emdumm,kmultl,ltypl)                   
       ltypa=iabs(ltypl)                                                
       if(iqql.eq.1) then                                               
         kpote=3                                                        
       else if(itypl.eq.41.or.itypl.eq.42) then                         
         kpote=0                                                        
       else if(itypl.ge.61.and.itypl.le.64) then                        
         kpote=1                                                        
       else if(kmultl.eq.0) then                                        
         kpote=0                                                        
       else                                                             
         kpote=3                                                        
       end if                                                           
      return                                                            
      end                                                               
      function delten(iswi,p0tot,p1tot,p2tot,p3tot,em1,em2,srtpr)       
       include 'scatter7'                                               
       include 'partblk7'                                               
          srt2=(p0tot*p0tot-p1tot*p1tot-p2tot*p2tot-p3tot*p3tot)        
          deltec=0.3                                                    
          if((iswi.eq.2.or.pcorr(0).lt.-0.05*psyst(0)).and.             
     a                         srt2.gt.(em1+em2+deltec)**2) then        
            delted=min(deltec,sqrt(srt2)-(em1+em2+deltec))              
            delts=min(-pcorr(0),deltec+ranf(0)*delted)                  
            srtpr=sqrt(srt2)-delts                                      
            delten=p0tot-sqrt(p0tot**2+srtpr**2-srt2)                   
          else                                                          
            delten=0.0                                                  
            srtpr=sqrt(srt2)                                            
          end if                                                        
      return                                                            
      end                                                               
