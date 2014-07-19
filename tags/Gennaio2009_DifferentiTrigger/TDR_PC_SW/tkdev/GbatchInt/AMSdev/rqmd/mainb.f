CPAT,MAINB    .                                                         
      subroutine propag(i1,i2,iswprp)                                   
      include 'scatter7'                                                
      include 'partblk7'                                                
      dimension qlk(0:3),plk(0:3)                                       
      dimension pil(0:3)                                                
      common /ierdum/ierr                                               
      if(i1.eq.i2) call errex('i1=i2 not allowed in propag')            
      il=i1                                                             
      ir=i2                                                             
    5 continue                                                          
      if(il.lt.0) go to 11                                              
      ichbl=ichbig(il)                                                  
      ichtag=ichbl/1000                                                 
      if(ichtag.ne.0) then                                              
        call errex('propag:dead end, ichtag.ne.0')                      
      end if                                                            
      ichbl=isign(1,ichbl)*mod(iabs(ichbl),1000)                        
      if(ichbl.eq.1.or.ichbl.eq.3) then                                 
        if(qdot(0,il).ne.0.) then                                       
            write(8,*) 'propag: ichbig=1,qdot(0).ne.0 ',irun,itripl,kdt 
            write(8,*) il,issn(il),ichbig(il),qdot(0,il),               
     a                                               x(0,il),tlast(il)  
        end if                                                          
        go to 11                                                        
      end if                                                            
      if(ityp(il).eq.-9999) go to 11                                    
      identl=issn(il)                                                   
      if(itdth(2,identl).ne.0  .and.                                    
     a        tdtg(identl).gt.tlast(il)) then                           
        do 47 i=0,3                                                     
   47   pil(i)=qdot(i,il)                                               
      else                                                              
        do 48 i=0,3                                                     
   48   pil(i)=p(i,il)                                                  
      end if                                                            
      if(ida(23).ne.0)then                                              
          teprop=tbegin+kdt* dt                                         
          tilobs=x(0,il)*pnorm(0)- x(1,il)*pnorm(1)-                    
     a                          x(2,il)*pnorm(2)- x(3,il)*pnorm(3)      
          pilobs=pil(0)*pnorm(0)-pil(1)*pnorm(1)-                       
     a                       pil(2)*pnorm(2)- pil(3)*pnorm(3)           
          if(tilobs.le.teprop) then                                     
            delts=(teprop-tilobs)/pilobs                                
            fctr=1./pilobs                                              
            do 57 i=0,3                                                 
             qdot(i,il)=pil(i)* fctr                                    
   57       continue                                                    
          else                                                          
            delts=0.                                                    
            do 58 i=0,3                                                 
   58       qdot(i,il)=0.                                               
          end if                                                        
      else                                                              
        if(ityp(il).eq.99.and.(npidec(il).eq.1.or.iabs(npidec(il))      
     a                                                .eq.7)) go to 11  
        chs1=0.                                                         
        chs2=0.                                                         
        deltsm=1.e+30                                                   
        do 10 ik=iofset+1,iofset+ntclst                                 
         if(ik.eq.ir.or.ik.eq.il) go to 10                              
         ichbk=ichbig(ik)                                               
         ichbk=isign(1,ichbk)*mod(iabs(ichbk),1000)                     
         if(ichbk.eq.1.or.ichbk.eq.3) go to 10                          
         if(ityp(ik).eq.-9999) go to 10                                 
         if(ityp(ik).eq.99.and.(npidec(ik).eq.0.or.iabs(npidec(ik))     
     a                                                 .eq.5)) go to 10 
         if(deltsm.eq.0.) go to 10                                      
         do 9 i=0,3                                                     
          qlk(i)=x(i,il)-x(i,ik)                                        
          plk(i)=p(i,il)+p(i,ik)                                        
    9    continue                                                       
         dqlksq=qlk(0)*qlk(0)                                           
         splksq=plk(0)*plk(0)                                           
         dqsplk=qlk(0)*plk(0)                                           
         pisplk=p(0,il)*plk(0)                                          
         delqp=qlk(0)*p(0,il)                                           
         pindsq=p(0,il)*p(0,il)                                         
         do 8  i=1,3                                                    
           dqlksq=dqlksq- qlk(i)*qlk(i)                                 
           splksq=splksq- plk(i)*plk(i)                                 
           dqsplk=dqsplk- qlk(i)*plk(i)                                 
           pisplk=pisplk- p(i,il)*plk(i)                                
           pindsq=pindsq-p(i,il)*p(i,il)                                
           delqp=delqp-qlk(i)*p(i,il)                                   
    8    continue                                                       
         if(dqlksq.gt.0.) then                                          
             if(qlk(0).gt.0.) then                                      
                deltsm=0.                                               
             else                                                       
                go to 10                                                
             end if                                                     
         else                                                           
             val=delqp/pindsq                                           
             arg=- dqlksq/pindsq+val*val                                
             if(arg.lt.0.) then                                         
                if(-arg.gt.0.01) then                                   
                   write(8,*) 'arg.lt.0 in propag,33  ????????',arg     
                   call errex('numerical problem in propag')            
                end if                                                  
                arg=0.                                                  
             else                                                       
                arg=sqrt(arg)                                           
             end if                                                     
             ds=- val+arg                                               
             deltsm=min(deltsm,ds)                                      
         end if                                                         
         if(-dqlksq.gt.dstint) go to 10                                 
         if(-dqlksq.lt.0.000001) dqlksq=-0.000001                       
         if(da(14).eq.1.) then                                          
            cofct1=exp(dqlksq/al)*abs(al/dqlksq)*em(il)**2 *em(ik)**2   
     a                             *dqsplk/sqrt(splksq)                 
            cofct2=exp(dqlksq/al)*abs(al/dqlksq)*em(il)**2 *em(ik)**2   
     a                             *pisplk/sqrt(splksq)                 
         else                                                           
            cofct1=exp(dqlksq/al)*abs(al/dqlksq) *dqsplk                
            cofct2=exp(dqlksq/al)*abs(al/dqlksq) *pisplk                
         end if                                                         
         chs1=chs1+cofct1                                               
         chs2=chs2+cofct2                                               
   10   continue                                                        
        delts=0.                                                        
        if(chs2.ne.0.) delts=max(cvalu0,-chs1/chs2)                     
        delts=min(delts,deltsm)                                         
        if(deltsm.lt.-1.e-3) then                                       
           write(8,*)'propag:ident=',issn(il),'deltsm=',deltsm          
           write(8,*) irun,itripl,kdt,kclst                             
        end if                                                          
        deltt=max(cvalu0,time+dt-tlstev)                                
        if(abs(deltt).lt.1.e-3) then                                    
          fctr=1000.*delts                                              
        else                                                            
          fctr=delts/deltt                                              
        end if                                                          
         tjump=0.                                                       
         ivz=1                                                          
         do 7 i=0,3                                                     
           tjump=tjump+ivz*p(i,il)*pnorm(i)                             
           ivz=-1                                                       
           qdot(i,il)=p(i,il)* fctr                                     
    7    continue                                                       
         tjump=tjump*delts                                              
         if(tjump.gt.3.*dt) then                                        
            fmlt=3.*dt/tjump                                            
            delts=delts*fmlt                                            
            do 16  i=0,3                                                
             qdot(i,il)=qdot(i,il)* fmlt                                
   16       continue                                                    
         end if                                                         
      end if                                                            
      if(switch(19)) then                                               
        qdot(1,il)=0.                                                   
        qdot(2,il)=0.                                                   
        x(0,il)=x(0,il)+pil(0)* delts                                   
        x(3,il)=x(3,il)+pil(3)* delts                                   
      else                                                              
        do 17 i=0,3                                                     
         x(i,il)=x(i,il)+pil(i)*delts                                   
   17   continue                                                        
      end if                                                            
      if(ida(23).eq.0.and.ida(47).eq.0) then                            
        tback=x(0,il)-qdot(0,il)*(time+dt-tlstev)                       
        if(tback+0.01.lt.tlast(il)) then                                
         write(8,*) 'propag: ',il,issn(il)                              
         write(8,*) 'x=',(x(i,il),i=0,3)                                
         write(8,*) 'qdot=',(qdot(i,il),i=0,3)                          
         write(8,*) 'p=',(p(i,il),i=0,3)                                
         write(8,*) 'tlstev,delts,deltt,tlast(il),tback'                
         write(8,*)  tlstev,delts,deltt,tlast(il),tback                 
        end if                                                          
      end if                                                            
   11 continue                                                          
      if(il.eq.i1) then                                                 
        il=i2                                                           
        ir=i1                                                           
        go to 5                                                         
      end if                                                            
      if(iswprp.ne.2) then                                              
        ierr=systim(strt,dummy)                                         
        call cupdat(i1,i2)                                              
        ierr=systim(tend,dummy)                                         
        tmcoll=tmcoll+tend-strt                                         
      end if                                                            
      return                                                            
      end                                                               
      subroutine rend(istop)                                            
         include 'scatter7'                                             
         common /ranfbk/iy                                              
         real*4 plund                                                   
         common /lujets/ nlund,k(2000,2),plund(2000,5)                  
         include 'partblk7'                                             
         include 'storbig'                                              
      write(8,*) 'rend is called for itripl,kdt',itripl,kdt             
      write(12) itripl,kdt                                              
      write(12) iy,nlund                                                
      do 60 l=1,nlund                                                   
       write(12)                                                        
     &    (k(l,i),i=1,2),(plund(l,i),i=1,5)                             
   60 continue                                                          
      write(12)                                                         
     &        n1,iz1,n2,iz2,    nprint,nruns,                           
     &                         inttyp,ieos,iseed,idtcl,                 
     &         elgev,b,dt,sepfp,sepft,cutfct                            
      write(12)                                                         
     &         nt,npion,    kclst,iofset,nprtcl,                        
     &   irun,ierror,nspect,       lmax,ntclst,                         
     &   idcdif,inondf,idifr,inn2df,ibrhig,ievbb,inocol,                
     &   nsppr,nspta,nspnuc,iyy,iqcnvt,ifndnx,nrclst                    
      do 10 l=1,ntotma                                                  
      write(12)                                                         
     &          tdtg(l),(x(i,l),i=0,3),(qdot(i,l),i=0,3),               
     &          (p(i,l),i=0,3),em(l),decay(l),denspp(l),                
     &          tlast(l), (frfp(i,l),i=0,3),datsto(l)                   
      write(12)                                                         
     &          h(l),chi(l),                                            
     &          ylast(l),zlast(l),                                      
     &          iclusz(l),nrbar(l),(itdth(i,l),i=1,5),nrmes(l),         
     &          ityp(l),npidec(l),ichbig(l),nonrel(l),prop(l),          
     &          nclcnt(l),lastcl(l),issn(l),(idud(i,l),i=1,1)           
   10 continue                                                          
      write(12)                                                         
     &              al,sal, alpha,beta,gamma,dstint,glk,coef1,coef2,    
     a                alpot,salpot,eosc3,coef3,                         
     b                eosc1,eosc2,gm1,pr2max,pnorm,elab                 
     &             ,ptotal,psyst,tbegin,emproj,tcoll,zcoll,pcorr        
      write(12)                                                         
     &           ityppr,  ispcod, iqtot,ntbar,ievntn,                   
     a                 iz1old,n1old,iz2old,n2old,                       
     &              zmxpr,zmnpr,zmxta,zmnta,bplus,rprsq,rtasq,betapr,   
     a              betata,yproj,ytarg,betapl,betatl,gamnn,betann,      
     b              rr1,rr2                                             
      write(12)                                                         
     &     tfrmcq,tfrmhd,nfrmcq,nfrmhd                                  
      write(12)                                                         
     &     dtprop,time,tlstev,tnexev,timmax                             
      write(12)                                                         
     &     tmsctr,tmprop,tmupdt,tmpali,tmcoll,                          
     a                       tmabsr                                     
      write(12)                                                         
     &      xblcut, xininw, xfirst,xbldel,xblock,                       
     a               xcnt,npaubl,ncollt,ncoll,                          
     b               iabspi,idcntd,idcntn,idcntr                        
      if(kdt.eq.idtcl) then                                             
        nend=nclmx/10                                                   
        do 44 m=1,nend                                                  
         write(12) (ngbrnr((m-1)*10+l),l=1,10)                          
         write(12) (ngbsum((m-1)*10+l),l=1,10)                          
   44   continue                                                        
        do 45 l=10*nend+1,nclmx                                         
         write(12) ngbsum(l),ngbrnr(l)                                  
   45   continue                                                        
        nend=nclmx*ngbrmx/10                                            
        do 46 m=1,nend                                                  
         write(12) (nghbor((m-1)*10+l),l=1,10)                          
   46   continue                                                        
        do 47 l=10*nend+1,nclmx*ngbrmx                                  
         write(12) nghbor(l)                                            
   47   continue                                                        
      end if                                                            
      if(istop.ne.1) then                                               
        return                                                          
      else                                                              
        stop                                                            
      end if                                                            
      end                                                               
      subroutine rstart                                                 
         include 'scatter7'                                             
         common /ranfbk/iy                                              
         real*4 plund                                                   
         common /lujets/ nlund,k(2000,2),plund(2000,5)                  
         include 'partblk7'                                             
         include 'storbig'                                              
      write(8,*) 'rstart is called for itripl,kdt',itripl,kdt           
      read(11) iy,nlund                                                 
      do 60 l=1,nlund                                                   
       read(11)                                                         
     &    (k(l,i),i=1,2),(plund(l,i),i=1,5)                             
   60 continue                                                          
      read(11)                                                          
     &        n1,iz1,n2,iz2,    nprint,nruns,                           
     &                         inttyp,ieos,iseed,idtcl,                 
     &         elgev,b,dt,sepfp,sepft,cutfct                            
      read(11)                                                          
     &         nt,npion,    kclst,iofset,nprtcl,                        
     &   irun,ierror,nspect,       lmax,ntclst,                         
     &   idcdif,inondf,idifr,inn2df,ibrhig,ievbb,inocol,                
     &   nsppr,nspta,nspnuc,iyy,iqcnvt,ifndnx,nrclst                    
      do 10 l=1,ntotma                                                  
      read(11)                                                          
     &          tdtg(l),(x(i,l),i=0,3),(qdot(i,l),i=0,3),               
     &          (p(i,l),i=0,3),em(l),decay(l),denspp(l),                
     &          tlast(l), (frfp(i,l),i=0,3),datsto(l)                   
      read(11)                                                          
     &          h(l),chi(l),                                            
     &          ylast(l),zlast(l),                                      
     &          iclusz(l),nrbar(l),(itdth(i,l),i=1,5),nrmes(l),         
     &          ityp(l),npidec(l),ichbig(l),nonrel(l),prop(l),          
     &          nclcnt(l),lastcl(l),issn(l),(idud(i,l),i=1,1)           
   10 continue                                                          
      read(11)                                                          
     &              al,sal, alpha,beta,gamma,dstint,glk,coef1,coef2,    
     a                alpot,salpot,eosc3,coef3,                         
     b                eosc1,eosc2,gm1,pr2max,pnorm,elab                 
     &             ,ptotal,psyst,tbegin,emproj,tcoll,zcoll,pcorr        
      read(11)                                                          
     &           ityppr,  ispcod, iqtot,ntbar,ievntn,                   
     a                 iz1old,n1old,iz2old,n2old,                       
     &              zmxpr,zmnpr,zmxta,zmnta,bplus,rprsq,rtasq,betapr,   
     a              betata,yproj,ytarg,betapl,betatl,gamnn,betann,      
     b              rr1,rr2                                             
      read(11)                                                          
     &     tfrmcq,tfrmhd,nfrmcq,nfrmhd                                  
      read(11)                                                          
     &     dtprop,time,tlstev,tnexev,timmax                             
      read(11)                                                          
     &     tmsctr,tmprop,tmupdt,tmpali,tmcoll,                          
     a                       tmabsr                                     
      read(11)                                                          
     &      xblcut, xininw, xfirst,xbldel,xblock,                       
     a               xcnt,npaubl,ncollt,ncoll,                          
     b               iabspi,idcntd,idcntn,idcntr                        
      if(kdt.eq.idtcl) then                                             
        nend=nclmx/10                                                   
        do 44 m=1,nend                                                  
         read(11) (ngbrnr((m-1)*10+l),l=1,10)                           
         read(11) (ngbsum((m-1)*10+l),l=1,10)                           
   44   continue                                                        
        do 45 l=10*nend+1,nclmx                                         
         read(11) ngbsum(l),ngbrnr(l)                                   
   45   continue                                                        
        nend=nclmx*ngbrmx/10                                            
        do 46 m=1,nend                                                  
         read(11) (nghbor((m-1)*10+l),l=1,10)                           
   46   continue                                                        
        do 47 l=10*nend+1,nclmx*ngbrmx                                  
         read(11) nghbor(l)                                             
   47   continue                                                        
      end if                                                            
      rewind(11)                                                        
      return                                                            
      end                                                               
      subroutine cnstst(iw)                                             
         include 'scatter7'                                             
         include 'partblk7'                                             
            ibarys=0                                                    
            ichrgs=0                                                    
            nall=0                                                      
            do 8 i=0,3                                                  
    8       ptotal(i)=0.                                                
            iof=0                                                       
            kcl=0                                                       
   38       kcl=kcl+1                                                   
            if(kcl.le.nrclst) then                                      
                ncl=nrbar(kcl)+nrmes(kcl)                               
            else                                                        
                ncl=nspect                                              
            end if                                                      
            nall=nall+ncl                                               
            do 41 l=iof+1,iof+ncl                                       
               if(ityp(l).eq.-9999) go to 41                            
               call transl(-1,ityp(l),npidec(l),k2l)                    
               ichrgs=ichrgs+luchge(k2l)                                
               k2labs=iabs(k2l)                                         
               if(k2labs.le.90) then                                    
                  if(kspin(k2labs).ge.2) then                           
                     ibarys=ibarys+1*isign(1,k2l)                       
                  endif                                                 
               else                                                     
                  write(8,*) 'warning: kspin not identified'            
                  write(8,*) ityp(l),npidec(l),k2l                      
               endif                                                    
              if(iw.lt.2) go to 41                                      
              do 49 i=0,3                                               
                ptotal(i)=ptotal(i)+p(i,l)                              
   49         continue                                                  
   41       continue                                                    
            if(kcl.le.nrclst) then                                      
              if(kcl.eq.nrclst) then                                    
                iof=ntotma-nspect                                       
              else                                                      
                iof=iof+iclusz(kcl)                                     
              end if                                                    
              go to 38                                                  
            end if                                                      
   40       continue                                                    
            if(nall.ne.nt+npion) then                                   
                write(8,*) 'nall=',nall,'total prtcl.no.=',nt+npion     
                call errex(                                             
     a             'no particle no. conservation for final particles')  
            endif                                                       
            if(ibarys.ne.ntbar)then                                     
                write(8,*) 'ibarys=',ibarys,'total b.no.=',ntbar        
                call errex(                                             
     a                'no baryon no. conservation for final particles') 
            endif                                                       
            if(mod(ichrgs,3).ne.0) then                                 
                write(8,*) 'ichrgs=',ichrgs,'total charge=',iqtot       
                call errex('no integer charge sum for final particles') 
            else                                                        
                ichrgs=ichrgs/3                                         
                if(ichrgs.ne. iqtot) then                               
                   write(8,*) 'ichrgs=',ichrgs,'total charge=',iqtot    
                   call errex(                                          
     a                   'no charge conservation for final particles')  
                end if                                                  
            end if                                                      
            if(iw.lt.2) return                                          
            if(abs(psyst(0)-ptotal(0))/psyst(0).gt.0.015) then          
                write(8,*)(psyst(i),i=0,3),'initial total momentum'     
                write(8,*)(ptotal(i),i=0,3),'final total momentum'      
                call errex('no energy conservation for final particles')
                return                                                  
            end if                                                      
            if((abs(psyst(1)-ptotal(1)).gt.1.0).or.                     
     a         (abs(psyst(2)-ptotal(2)).gt.1.0).or.                     
     b         (abs(psyst(3)).gt.1.e-3.and.                             
     c          abs(psyst(3)-ptotal(3)).gt.1.0 .and.                    
     d          abs(psyst(3)-ptotal(3)).gt.0.015*abs(psyst(3))).or.     
     e         (abs(psyst(3)).le. 1.e-3    .and.                        
     f           abs(psyst(3)-ptotal(3)).gt.1.0 ) ) then                
                  write(8,*)(psyst(i),i=0,3),'initial total momentum'   
                  write(8,*)(ptotal(i),i=0,3),'final total momentum'    
                  call errex(                                           
     a                'no momentum conservation for final particles')   
                return                                                  
            end if                                                      
            if(irun.eq.nruns) then                                      
                write(8,*) 'momentum in the last event:'                
                write(8,*)(psyst(i),i=0,3),'initial total momentum'     
                write(8,*)(ptotal(i),i=0,3),'final total momentum'      
            end if                                                      
      return                                                            
      end                                                               
      subroutine ofshel(isubr,il,ident)                                 
       include 'scatter7'                                               
       include 'partblk7'                                               
                do 17 i=0,3                                             
                  p(i,il)=p(i,il)+frfp(i,ident)                         
   17           continue                                                
                if(isubr.ne.1) then                                     
                   if(isubr.ne.3)  then                                 
                       tdtg(ident)=0.                                   
                       itdth(1,ident)=0                                 
                       itdth(2,ident)=0                                 
                       itdth(4,ident)=0                                 
                   end if                                               
                   do 18 i=0,3                                          
                    psyst(i)=psyst(i)+frfp(i,ident)                     
                    frfp(i,ident)=0.                                    
   18              continue                                             
                end if                                                  
                emsq=p(0,il)*p(0,il)-(p(1,il)*p(1,il)+                  
     a                             p(2,il)*p(2,il)+p(3,il)*p(3,il))     
                if(ida(13).eq.4.and..not.switch(6)) then                
                else if(abs(emsq-em(il)*em(il)).gt.0.008) then          
                   write(8,*) 'isubr=',isubr                            
                   write(8,*) 'psyst=',(psyst(i),i=0,3)                 
                   write(8,*) 'ident=',ident,il,ityp(il),npidec(il)     
                   write(8,*) 'p=',(p(i,il),i=0,3)                      
                   write(8,*) 'frfp=',(frfp(i,ident),i=0,3)             
                   write(8,*) 'emsq=',emsq,'em**2=',em(il)*em(il)       
                   write(8,*) sqrt(emsq),em(il)                         
                   call errex(                                          
     a             'ofshel:wrong mass,conversion of cquark into hadron')
                end if                                                  
      return                                                            
      end                                                               
      subroutine comprs(istor)                                          
      include 'scatter7'                                                
      include 'partblk7'                                                
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 1.08 -- 29-nov-91'/                     
      data pronam /'c o m p r s --- remove absorbed particles '/        
      data ttvers/.true./                                               
      if(ttvers) then                                                   
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
         idelte=0                                                       
         iofbar=iofset+nrbar(kclst)                                     
         do 110 il=iofset+1,iofbar+nrmes(kclst)                         
             if(ityp(il).eq.-9999) then                                 
               idelte=idelte+1                                          
               if(il.gt.iofbar)then                                     
                  nrmes(kclst)=nrmes(kclst)-1                           
               else                                                     
                  nrbar(kclst)=nrbar(kclst)-1                           
               end if                                                   
               go to 110                                                
             end if                                                     
             j=il-idelte                                                
             if(idelte.ne.0) then                                       
                call store(j,il)                                        
             end if                                                     
 110        continue                                                    
            npion=npion-idelte                                          
            if(istor.eq.0) return                                       
            call errex('comprs(1) not ready to run')                    
      return                                                            
      end                                                               
      subroutine errex(text)                                            
      include 'scatter7'                                                
      character*(*) text                                                
      include 'partblk7'                                                
  899 format(1x,'event with random no. initialization:',i12)            
 912  format(3x,'time:',f8.3,2x,i4,  '. time step' )                    
      write(8,*)'errex is called with error message'                    
      write(8,*) 'start value of random integer in initev was ',iyy     
      write(8,*) 'irun,kdt,itripl,kclst,nrbar(kclst),nrmes(kclst)'      
      write(8,*)  irun,kdt,itripl,kclst,nrbar(kclst),nrmes(kclst)       
      write(8,*)text                                                    
      ierror=1                                                          
      if(ida(43).eq.1) then                                             
        write(14,899) iyy                                               
        write(14,912) time,kdt                                          
        write(14,*)'errex is called with error message'                 
        write(14,*)text                                                 
        write(14,*) 'irun,kdt,itripl,kclst,nrbar(kclst),nrmes(kclst)'   
        write(14,*)  irun,kdt,itripl,kclst,nrbar(kclst),nrmes(kclst)    
        if(ida(5).ne.0) write(10,901) -1                                
        if(ida(38).eq.1.or.ida(38).eq.2.or.                             
     a     ida(38).eq.5.or.ida(38).eq.7)  then                          
          write(13,901) -1                                              
        end if                                                          
 901    format(1x,i4)                                                   
       if(ida(42).eq.1) then                                            
         write(7)                                                       
     &           -1,0,0,                                                
     &            10*0.0,0,0                                            
       end if                                                           
        stop                                                            
      end if                                                            
      return                                                            
      end                                                               
      subroutine swap(x1,x2)                                            
      include 'scatter7'                                                
      y=x1                                                              
      x1=x2                                                             
      x2=y                                                              
      return                                                            
      end                                                               
      subroutine iswap(ix1,ix2)                                         
      iy=ix1                                                            
      ix1=ix2                                                           
      ix2=iy                                                            
      return                                                            
      end                                                               
      function clphh(srt,pr,em1,em2,ika1,ika2)                          
       include 'scatter7'                                               
       data avan,kvan /1.2,2/                                           
       if(em1+em2.gt.srt) then                                          
          write(8,*) srt,em1,em2                                        
          call errex('clphh abort:em1+em2>srt')                         
       end if                                                           
       if(ida(53).eq.0) then                                            
         if(pr.lt.1.5)  then                                            
           clphh=1.-exp(-(pr/0.4)**2)                                   
         else                                                           
           clphh=1.                                                     
         end if                                                         
       else if(ida(53).eq.1) then                                       
         clphh=pr**kvan*exp(-avan*sqrt(srt*srt-(em1+em2)**2))           
       else if(ida(53).eq.2) then                                       
         clphh=1.                                                       
       end if                                                           
      return                                                            
      end                                                               
      subroutine ndstar(s,pr,em1,em2,icltyp,itp1,npd1,itp2,npd2,        
     a              icollc,mstr,ichd,ichn,ireco,nscod,strrat,vfspin)    
      include 'scatter7'                                                
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       real*4 pssbar,pr4                                                
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       mstr=0                                                           
       icollc=0                                                         
       pr4=pr                                                           
       if((icltyp.ge.22.and.icltyp.le.23).or.                           
     a         (icltyp.ge.29.and.icltyp.le.30).or.                      
     b    (icltyp.ge.37.and.icltyp.le.38)) then                         
         if(npd1.eq.2) then                                             
           ireco=1                                                      
           vfspin=0.333333                                              
           ichn=3                                                       
           ichd=3                                                       
           icollc=2                                                     
         else                                                           
           ireco=0                                                      
         end if                                                         
         str=1.0                                                        
       else if((icltyp.ge.24.and.icltyp.le.27).or.                      
     a         (icltyp.ge.31.and.icltyp.le.34).or.                      
     b          (icltyp.ge.39.and.icltyp.le.42)) then                   
         if((itp1.gt.6.and.npd1.ne.0).or.                               
     b           (itp2.le.6.and.(npd2.gt.1.or.npd2.lt.0))) then         
           ireco=0                                                      
         else                                                           
           ireco=1                                                      
           vfspin=0.5                                                   
           ichn=2                                                       
           ichd=2                                                       
           icollc=1                                                     
         end if                                                         
         str=1.0                                                        
       else if((icltyp.ge.52.and.icltyp.le.54).or.                      
     a            (icltyp.ge.62.and.icltyp.le.64)) then                 
         if((itp1.le.9.and.npd1.ne.0).or.                               
     a               (itp2.gt.9.and.npd2.ge.0)) then                    
           ireco=0                                                      
         else                                                           
           ireco=1                                                      
           nscod=mod(-npd2,1000)                                        
           if(nscod.eq.1.or.nscod.eq.3.or.                              
     a                 nscod.eq.4.or.nscod.eq.8) then                   
             vfspin=1.0                                                 
           else if(nscod.eq.2.or.nscod.eq.7.or.                         
     a                           nscod.eq.9) then                       
             vfspin=0.5                                                 
           else if(nscod.eq.5.or.nscod.eq.6) then                       
             vfspin=0.3333                                              
           else                                                         
             vfspin=0.25                                                
           end if                                                       
           ichn=-1                                                      
           ichd=0                                                       
         end if                                                         
         str=1.                                                         
       else if((icltyp.eq.104.and.npd1.gt.0).or.                        
     a          (icltyp.gt.120                                          
     b           .and.((npd1.ge.28.and.npd1.le.29).or.                  
     c                  (itp1.eq.14.and.npd1.gt.0))                     
     d           .and.(itp2.eq.13))) then                               
         mstr=1                                                         
         if(em1.gt.0.5.or.em2.gt.1.2) then                              
           ireco=0                                                      
         else                                                           
           ireco=1                                                      
           vfspin=1.0                                                   
           ichn=5                                                       
           ichd=-1                                                      
         end if                                                         
         str=pssbar(ida(55),pr4)                                        
       else if((icltyp.eq.119.and.npd1.gt.0).or.                        
     a          (icltyp.gt.120                                          
     b           .and.((npd1.ge.28.and.npd1.le.29).or.                  
     c                  (itp1.eq.14.and.npd1.gt.0))                     
     d           .and.(itp2.eq.15.or.                                   
     e                 (npd2.ge.65.and.npd2.le.67)                      
     f                                            ))) then              
         mstr=1                                                         
         if(em1.gt.0.5.or.em2.gt.1.2) then                              
           ireco=0                                                      
         else                                                           
           ireco=1                                                      
           vfspin=1.0                                                   
           ichn=6                                                       
           ichd=4                                                       
         end if                                                         
         str=pssbar(ida(55),pr4)                                        
       else if((icltyp.ge.67.and.icltyp.le.72).or.                      
     a         icltyp.eq.76.or.icltyp.eq.77.or.                         
     b         (icltyp.gt.120.and.npd1.eq.25).or.                       
     c         (icltyp.gt.120.and.npd1.eq.34).or.                       
     d         (icltyp.gt.120.and.npd1.eq.35)) then                     
         ichn=-1                                                        
         ichd=-1                                                        
         ireco=0                                                        
         if(icltyp.eq.67.or.icltyp.eq.68) then                          
           if(em1.lt.0.6) then                                          
             ireco=1                                                    
             vfspin=1.0                                                 
             ichn=4                                                     
           end if                                                       
         end if                                                         
         if((ireco.eq.1.and.itp1.eq.12).or.                             
     a                (itp1.eq.99.and.npd1.eq.25)) then                 
           themix=pi2/360.0*anglmx(0,1)                                 
           valmix=0.3333*(cos(themix)-sqrt(2.)*sin(themix))**2          
         else if(npd1.eq.35) then                                       
           valmix=0.                                                    
         else                                                           
           valmix=1.0                                                   
         end if                                                         
         if(npd1.eq.25) valmix=1.-valmix                                
         str=valmix+(1.-valmix)*ssbdif*pssbar(ida(55),pr4)              
       else                                                             
         ireco=0                                                        
         str=0.0                                                        
       end if                                                           
       if(s.le.em1+em2) then                                            
         strrat=0.0                                                     
       else if(str.ne.0.0) then                                         
         prpin=pcmsr(s,emnuc,empion)                                    
         emn=emnuc                                                      
         emp=empion                                                     
         strrat=str*clphh(s,pr,em1,em2,ika1,ika2)/                      
     a            clphh(s,prpin,emn,emp,ikan,ikap)                      
       else                                                             
         write(8,*)'ndstar: wrong icltyp as input',icltyp,              
     a                                    itp1,npd1,itp2,npd2           
         call errex('ndstar: wrong icltyp as input')                    
       end if                                                           
      return                                                            
      end                                                               
      subroutine fillca                                                 
       common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)     
       real*4 gemca(5),dcrat(8,5)                                       
       common /caflbk/dcrat,gemca                                       
       dimension emca(5)                                                
       data emca  /1.835, 1.82, 2.04,1.945,2.130 /                      
       dimension lres(8,5),it1(8),it2(8)                                
       data lres /2*0,2*2,2*0,2*2,                                      
     a            2*2,2*0,2*2,2*0,                                      
     b            2*3,2*1,2*3,2*1,                                      
     c            8*2,                                                  
     d            8*3                                                   
     e           /                                                      
       data it1 / 47,47,69,69,45,57,67,70                               
     a          /                                                       
       data it2 / 17,24,17,24,19,19,19,19                               
     a          /                                                       
       dimension alpsu3(5),acoef1(5),acoef2(5)                          
       data alpsu3 /-0.28,0.72,0.54,-0.16,0.0/                          
       data acoef1 / 5.2, 41.1, 47.7, 22.8, 51.6 /                      
       data acoef2 / 262.0,3.6,16.0,156.0,79.700 /                      
       dimension kconv(5)                                               
       data kconv /2,1,4,3,5/                                           
        do 10 j=1,5                                                     
         kon=kconv(j)                                                   
         emres=emca(j)                                                  
         gemca(kon)=0.0                                                 
         do 11 i=1,8                                                    
          dcrat(i,kon)=0.0                                              
          embar=pmas(it1(i))                                            
          emmes=pmas(it2(i))                                            
          iusual=0                                                      
          if(emres.gt.embar+emmes) then                                 
            prres=pcmsr(emres,embar,emmes)                              
          else                                                          
            embar=pmas(46)                                              
            emmes=pmas(23)                                              
            prres=pcmsr(emres,embar,emmes)                              
            iusual=1                                                    
          end if                                                        
          if(j.le.4) then                                               
            alpha=alpsu3(j)                                             
            acoef=acoef1(j)                                             
            cosu3=s888(i,alpha,acoef)**2                                
            acoef=acoef2(j)                                             
            cosu3=cosu3+s8108(i,acoef)**2                               
          else if(j.eq.5) then                                          
            acoef=acoef1(j)                                             
            cosu3=s1088(i,acoef)**2                                     
            acoef=acoef2(j)                                             
            cosu3=cosu3+s10108(i,acoef)**2                              
          end if                                                        
          gempar=cosu3*prres/emres*prres**(2*lres(i,j))                 
          dcrat(i,kon)=gempar                                           
          if(iusual.eq.1) go to 11                                      
          gemca(kon)=gemca(kon)+gempar                                  
   11    continue                                                       
         if(gemca(kon).eq.0.0) go to 10                                 
         do 12 i=1,8                                                    
   12    dcrat(i,kon)=dcrat(i,kon)/gemca(kon)                           
         gemca(kon)=1.e-3*gemca(kon)                                    
   10  continue                                                         
       do 20 j=1,5                                                      
        write(6,22) (dcrat(i,j),i=1,8)                                  
   22  format(8x,8(f5.3,',',1x))                                        
   20  continue                                                         
       write(6,22) (gemca(j),j=1,5)                                     
      return                                                            
      end                                                               
      function s1088(i,a1088)                                           
       if(i.eq.3.or.i.eq.4.or.i.eq.7.or.i.eq.8) then                    
         s1088=0.0                                                      
       else if(i.eq.1.or.i.eq.2.or.i.eq.5) then                         
         s1088=0.5*a1088                                                
       else if(i.eq.6) then                                             
         s1088=-0.5*a1088                                               
       else                                                             
         call errex('s1088-wrong input')                                
       end if                                                           
      return                                                            
      end                                                               
      function s10108(i,a10108)                                         
       if(i.eq.1.or.i.eq.2.or.i.eq.5.or.i.eq.6) then                    
         s10108=0.0                                                     
       else if(i.eq.3) then                                             
         s10108=a10108/sqrt(8.)                                         
       else if(i.eq.4) then                                             
         s10108=-a10108/sqrt(8.)                                        
       else if(i.eq.7) then                                             
         s10108=a10108/sqrt(2.)                                         
       else if(i.eq.8) then                                             
         s10108=-0.5*a10108                                             
       else                                                             
         call errex('s10108-wrong input')                               
       end if                                                           
      return                                                            
      end                                                               
      function s8108(i,a8108)                                           
       if(i.eq.1.or.i.eq.2.or.i.eq.5.or.i.eq.6) then                    
         s8108=0.0                                                      
       else if(i.eq.3.or.i.eq.4) then                                   
         s8108=-a8108/sqrt(5.)                                          
       else if(i.eq.7) then                                             
         s8108=a8108/sqrt(5.)                                           
       else if(i.eq.8) then                                             
         s8108=a8108*sqrt(0.4)                                          
       else                                                             
         call errex('s8108-wrong input')                                
       end if                                                           
      return                                                            
      end                                                               
      function s888(i,alpha,a888)                                       
       if(i.eq.1) then                                                  
         s888=sqrt(3.)*(2.*alpha-1.)*a888                               
       else if(i.eq.2) then                                             
         s888=(2.*alpha+1.)/sqrt(3.)*a888                               
       else if(i.eq.3.or.i.eq.4.or.i.eq.7.or.i.eq.8) then               
         s888=0.0                                                       
       else if(i.eq.5) then                                             
         s888=sqrt(3.)*a888                                             
       else if(i.eq.6) then                                             
         s888=(4.*alpha-1.)/sqrt(3.)*a888                               
       else                                                             
         call errex('s888-wrong input')                                 
       end if                                                           
      return                                                            
      end                                                               
      subroutine fillom                                                 
       common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)     
       parameter (ichaom=3,irenom=1)                                    
       real*4 gemom(irenom),dcrat(ichaom,irenom)                        
       common /omflbk/dcrat,gemom                                       
       dimension emom(irenom)                                           
       data emom  /2.230 /                                              
       dimension lres(ichaom,irenom),it1(ichaom),it2(ichaom)            
       data lres /3*3                                                   
     e           /                                                      
       data it1 / 47,69,70                                              
     a          /                                                       
       data it2 / 19,19,24                                              
     a          /                                                       
       dimension acoef1(irenom),acoef2(irenom)                          
       data acoef1 /  51.6 /                                            
       data acoef2 /  79.700 /                                          
        do 10 j=1,irenom                                                
         emres=emom(j)                                                  
         kon=j                                                          
         gemom(kon)=0.0                                                 
         do 11 i=1,ichaom                                               
          dcrat(i,kon)=0.0                                              
          embar=pmas(it1(i))                                            
          emmes=pmas(it2(i))                                            
          if(emres.gt.embar+emmes) then                                 
            prres=pcmsr(emres,embar,emmes)                              
          else                                                          
            go to 11                                                    
          end if                                                        
          if(i.eq.1) then                                               
            cosu3=1.0*acoef1(j)**2                                      
          else                                                          
            cosu3=0.5*acoef2(j)**2                                      
          end if                                                        
          gempar=cosu3*prres/emres*prres**(2*lres(i,j))                 
          dcrat(i,kon)=gempar                                           
          gemom(kon)=gemom(kon)+gempar                                  
   11    continue                                                       
         if(gemom(kon).eq.0.0) go to 10                                 
         do 12 i=1,ichaom                                               
   12    dcrat(i,kon)=dcrat(i,kon)/gemom(kon)                           
         gemom(kon)=1.e-3*gemom(kon)                                    
   10  continue                                                         
       do 20 j=1,irenom                                                 
        write(6,22) (dcrat(i,j),i=1,ichaom)                             
   22  format(8x,3(f5.3,',',1x))                                        
   20  continue                                                         
       write(6,22) (gemom(j),j=1,irenom)                                
      return                                                            
      end                                                               
      subroutine hyper(s,pr,em1,em2,                                    
     a                  sig0,sig2,ityp1,npid1,ityp2,npid2)              
       include 'scatter7'                                               
       real*4 cgk20,cgk22                                               
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       real*4 pssbar,pr4                                                
        data sbrsup /1.0/                                               
        pr4=pr                                                          
        sig0st=sig0                                                     
        sig2st=sig2                                                     
        sig0=0.0                                                        
        sig2=0.0                                                        
        itp1=ityp1                                                      
        npd1=npid1                                                      
        itp2=ityp2                                                      
        npd2=npid2                                                      
              emn=emnuc                                                 
              emk=emkap                                                 
              prkan=pcmsr(s,emk,emn)                                    
              if(itp1.ge.7.and.itp1.le.9.and.npd1.eq.0) then            
                vfspin=1.0                                              
                str=1.0                                                 
              else if(itp1.ge.7.and.itp1.le.9.and.npd1.ne.0) then       
                vfspin=0.3333                                           
                str=1.0                                                 
              else if(itp1.ge.98.and.itp1.le.99.and.npd1.eq.34) then    
                vfspin=0.3333                                           
                str=1.0                                                 
              else if(itp1.eq.12.or.                                    
     b              (itp1.ge.98.and.itp1.le.99.and.npd1.eq.25)) then    
                vfspin=1.0                                              
                if(em1.lt.0.6.or.(itp1.eq.99)) then                     
                  themix=pi2/360.0*anglmx(0,1)                          
                  valmix=0.3333*(cos(themix)-sqrt(2.)*sin(themix))**2   
                else                                                    
                  valmix=1.0                                            
                end if                                                  
                if(npd1.eq.25) valmix=1.-valmix                         
                str=valmix+(1.-valmix)*pssbar(ida(55),pr4)              
              else if(itp1.ge.98.and.itp1.le.99.and.npd1.eq.35) then    
                vfspin=0.3333                                           
                str=pssbar(ida(55),pr4)                                 
              else if(itp1.eq.14) then                                  
                vfspin=1.0                                              
                if(npd1.lt.0) then                                      
                  str=1.0                                               
                else                                                    
                  str=pssbar(ida(55),pr4)                               
                end if                                                  
              else if(itp1.ge.98.and.itp1.le.99.and.                    
     a              (iabs(npd1).eq.28.or.iabs(npd1).eq.29)) then        
                vfspin=0.3333                                           
                if(npd1.eq.-28.or.npd1.eq.-29) then                     
                  str=1.0                                               
                else if(npd1.eq.28.or.npd1.eq.29) then                  
                  str=pssbar(ida(55),pr4)                               
                end if                                                  
              else                                                      
                write(8,*) itp1,npd1                                    
                call errex('hyper: unknown ingoing meson')              
              end if                                                    
              if(itp2.le.2) then                                        
              else if(itp2.le.6) then                                   
                vfspin=0.5*vfspin                                       
                jdcod=mod(-npd2,1000)                                   
              else if(itp2.ge.10.and.itp2.le.11) then                   
                nscod=mod(-npd2,1000)                                   
                if(nscod.eq.1.or.nscod.eq.3.or.                         
     a                           nscod.eq.4.or.nscod.eq.8) then         
                else if(nscod.eq.2.or.nscod.eq.7.or.nscod.eq.9) then    
                  vfspin=0.5*vfspin                                     
                else if(nscod.eq.5.or.nscod.eq.6) then                  
                  vfspin=0.3333*vfspin                                  
                else                                                    
                  vfspin=0.25*vfspin                                    
                end if                                                  
              else if(itp2.eq.13) then                                  
                if(em2.lt.1.2) str=sbrsup*str                           
              else if(itp2.eq.15) then                                  
                if(em2.lt.1.2) str=sbrsup*str                           
              else if(itp2.ge.98.and.itp2.le.99.and.                    
     a                 npd2.ge.65.and.npd2.le.67) then                  
                vfspin=0.5*vfspin                                       
                if(npd2.eq.99) str=sbrsup*str                           
              else if(itp2.ge.98.and.itp2.le.99.and.                    
     a                 npd2.ge.46.and.npd2.le.47) then                  
              else if(itp2.ge.98.and.itp2.le.99.and.                    
     a                  npd2.ge.68.and.npd2.le.69) then                 
                vfspin=0.5*vfspin                                       
              else                                                      
                write(8,*) itp2,npd2                                    
                call errex('hyper: unknown ingoing baryon')             
              end if                                                    
              emn=emnuc                                                 
              emk=emkap                                                 
              strrat=str*clphh(s,pr,em1,em2,ika1,ika2)                  
     a                   /clphh(s,prkan,emn,emk,ikan,ikak)              
         call retcgk(itp1,npd1,itp2,npd2,0,cgk20)                       
         call retcgk(itp1,npd1,itp2,npd2,2,cgk22)                       
         vf=strrat*(prkan/pr)**2                                        
         sig0=vf*sig0st*cgk20                                           
         sig2=vf*sig2st*cgk22                                           
      return                                                            
      end                                                               
      subroutine casca(srt,pr,em1,em2,sigstr,sigres,xs1530,             
     a                      inchan,ltp1,kmul1,ltp2,kmul2)               
       include 'scatter7'                                               
       real*4 cgk212                                                    
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       real*4 pssbar,pr4                                                
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       data srtl,ema,emb,incha /2.15,1.318,0.138,1/                     
       data em1530,pr1530,gm1530 /1.533,0.152,0.0095/                   
       data emcrit,sigbwl /2.30,19.56/                                  
       if(kmul2.ne.0.or.kmul1.ne.0) then                                
         if(srt.le.emcrit) then                                         
           sigres=cvalu0                                                
           sigstr=cvalu0                                                
           xs1530=cvalu0                                                
           return                                                       
         else                                                           
           vfexc=1.-(emcrit/srt)**5.0                                   
         end if                                                         
       else                                                             
         vfexc=1.0                                                      
       end if                                                           
       ltp1a=iabs(ltp1)                                                 
       ltp2a=iabs(ltp2)                                                 
       pr4=pr                                                           
       sigstr=0.0                                                       
       sigres=0.0                                                       
       xs1530=0.0                                                       
       idcode=0                                                         
       if(ltp2a.eq.17.or.ltp2a.eq.23) then                              
         if(kmul2.eq.0) then                                            
           vfspin=1.0                                                   
           str=1.0                                                      
           idcode=1                                                     
         else                                                           
           vfspin=0.3333                                                
           str=1.0                                                      
         end if                                                         
       else if(ltp2a.eq.27.or.ltp2a.eq.33) then                         
         vfspin=0.3333                                                  
         str=1.0                                                        
       else if(ltp2.eq.24) then                                         
         vfspin=1.0                                                     
         if(kmul2.eq.0) then                                            
           themix=pi2/360.0*anglmx(0,1)                                 
           valmix=0.3333*(cos(themix)-sqrt(2.)*sin(themix))**2          
           idcode=2                                                     
         else                                                           
           valmix=1.0                                                   
         end if                                                         
         str=valmix+(1.-valmix)*pssbar(ida(55),pr4)                     
       else if(ltp2.eq.25) then                                         
         vfspin=1.0                                                     
         if(kmul2.eq.0) then                                            
           themix=pi2/360.0*anglmx(0,1)                                 
           valmix=0.3333*(cos(themix)-sqrt(2.)*sin(themix))**2          
         else                                                           
           valmix=1.0                                                   
         end if                                                         
         str=(1.-valmix)+valmix*pssbar(ida(55),pr4)                     
       else if(ltp2.eq.34) then                                         
         vfspin=0.3333                                                  
         str=1.0                                                        
       else if(ltp2.eq.35) then                                         
         vfspin=0.3333                                                  
         str=pssbar(ida(55),pr4)                                        
       else if(ltp2a.eq.18.or.ltp2a.eq.19) then                         
         vfspin=1.0                                                     
         if(ltp2.lt.0) then                                             
           if(kmul2.eq.0) idcode=3                                      
           str=1.0                                                      
         else                                                           
           if(kmul2.eq.0) idcode=4                                      
           str=pssbar(ida(55),pr4)                                      
         end if                                                         
       else if(ltp2a.ge.28.and.ltp2a.le.29) then                        
         vfspin=0.3333                                                  
         if(ltp2.lt.0) then                                             
           str=1.0                                                      
         else                                                           
           str=pssbar(ida(55),pr4)                                      
         end if                                                         
       else                                                             
         write(8,*) 'kmul2,ltp2=',kmul2,ltp2                            
         call errex('casca: unknown ingoing meson')                     
       end if                                                           
       if(ltp1.eq.57) then                                              
         if(kmul1.eq.0) idcode=40+idcode                                
       else if(ltp1.ge.43.and.ltp1.le.45) then                          
         if(kmul1.eq.0) idcode=30+idcode                                
       else if(ltp1.ge.65.and.ltp1.le.67) then                          
         vfspin=0.5*vfspin                                              
         if(kmul1.eq.0) idcode=50+idcode                                
       else if(ltp1.ge.46.and.ltp1.le.47) then                          
         if(kmul1.eq.0) idcode=10+idcode                                
       else if(ltp1.ge.68.and.ltp1.le.69) then                          
         vfspin=0.5*vfspin                                              
         if(kmul1.eq.0) idcode=20+idcode                                
       else if(ltp1.eq.70)  then                                        
         vfspin=0.5*vfspin                                              
         if(kmul1.eq.0) idcode=60+idcode                                
       else                                                             
         write(8,*) 'ltp1,kmul1=',ltp1,kmul1                            
         call errex('casca: unknown ingoing baryon')                    
       end if                                                           
       if(idcode.eq.11) then                                            
         inchan=1                                                       
       else if(idcode.eq.12) then                                       
         inchan=2                                                       
       else if(idcode.eq.21) then                                       
         inchan=3                                                       
       else if(idcode.eq.22) then                                       
         inchan=4                                                       
       else if(idcode.eq.33) then                                       
         inchan=5                                                       
       else if(idcode.eq.43) then                                       
         inchan=6                                                       
       else if(idcode.eq.53) then                                       
         inchan=7                                                       
       else if(idcode.eq.64) then                                       
         inchan=8                                                       
       else                                                             
         inchan=100                                                     
       end if                                                           
       if(inchan.eq.1) then                                             
         vfg=(em1530/srt)*1.2/(1.+0.2*(pr/pr1530)**2)                   
         g1530=vfg*(pr/pr1530)**2*gm1530                                
         bwsum=4.*g1530**2/((srt-em1530)**2+0.25*g1530**2)              
         xs1530=0.5*pi*10.0*(hc/pi2)**2/pr**2*bwsum                     
       end if                                                           
       if(inchan.le.8.and.srt.gt.1.46) then                             
         bwsum=0.0                                                      
         do 172 km=1,5                                                  
          call bwdist(5,1,srt,km,inchan,dcrat,bwig)                     
          bwsum=bwsum+bwig                                              
  172    continue                                                       
         sigres=vfspin*0.5*pi*10.0*(hc/pi2)**2/pr**2*bwsum              
       end if                                                           
       if(srt.lt.srtl.and.ida(58).eq.1) then                            
         sigstr=0.0                                                     
       else if(srt.gt.em1530) then                                      
         sigstr=sigbwl*(srtl**2/srt/srt)**da(7)                         
         pin=pcmsr(srt,ema,emb)                                         
         bwsum=0.0                                                      
         do 174 km=1,5                                                  
          call bwdist(5,1,srt,km,incha,dcrat,bwig)                      
          bwsum=bwsum+bwig                                              
  174    continue                                                       
         sigbws=0.5*pi*10.0*(hc/pi2)**2/pin**2*bwsum                    
         sigstr=max(sigstr-sigbws,cvalu0)                               
         strrat=str*clphh(srt,pr,em1,em2,ika1,ika2)                     
     a                   /clphh(srt,pin,ema,emb,ikaa,ikab)              
         sigstr=strrat*(pin/pr)**2*sigstr                               
       else                                                             
         sigstr=0.0                                                     
       end if                                                           
       sigstr=vfexc*sigstr                                              
       call retcgk(99,ltp1,99,ltp2,1,cgk212)                            
       sigres=cgk212*sigres                                             
       sigstr=cgk212*sigstr                                             
       xs1530=cgk212*xs1530                                             
       sigres=sigres+xs1530                                             
      return                                                            
      end                                                               
      logical function issers(jeng,ida38,itp,npid,emin)                 
       include 'scatter7'                                               
       issers=.false.                                                   
       call oldnew(1,itp,npid,emin,kmult,ltyp)                          
       if(jeng.eq.0.and.kmult.ne.0) return                              
       ltypa=iabs(ltyp)                                                 
       if(ida38.eq.1) then                                              
         if(ltypa.eq.18.or.ltypa.eq.19) then                            
           issers=.true.                                                
         end if                                                         
         if(jeng.ne.0) then                                             
           if(ltypa.eq.28.or.ltypa.eq.29) then                          
             issers=.true.                                              
           end if                                                       
         end if                                                         
       else if(ida38.eq.2) then                                         
         if(ltypa.eq.44.or.ltypa.eq.57) then                            
           issers=.true.                                                
         end if                                                         
         if(jeng.ne.0) then                                             
           if((ltypa.ge.43.and.ltypa.le.45).or.                         
     a        (ltypa.ge.46.and.ltypa.le.47).or.                         
     b        (ltypa.ge.65.and.ltypa.le.70)                             
     c                                          ) then                  
             issers=.true.                                              
           end if                                                       
         end if                                                         
       else if(ida38.eq.5) then                                         
         if(ltyp.eq.35) then                                            
           issers=.true.                                                
         end if                                                         
       else if(ida38.eq.7) then                                         
         if(ltypa.eq.46.or.ltypa.eq.47.or.                              
     a            ltypa.eq.68.or.ltypa.eq.69) then                      
           issers=.true.                                                
         end if                                                         
       else                                                             
         call errex('issers:undefined ida38')                           
       end if                                                           
      return                                                            
      end                                                               
      function gamvec(pr,id)                                            
       data radmes/ 1.0/                                                
       dimension asolid(3)                                              
       data asolid /3.5523,2.2583,2.5637/                               
       gctact=(pr*radmes)**2/(1.+(pr*radmes)**2)                        
       iabid=iabs(id)                                                   
       if(iabid.eq.27.or.iabid.eq.33) then                              
         ip=1                                                           
       else if(iabid.eq.28.or.iabid.eq.29) then                         
         ip=2                                                           
       else if(iabid.eq.35) then                                        
         ip=3                                                           
       else                                                             
         call errex('gamvec:unknown resonance')                         
       end if                                                           
       gamvec=asolid(ip)*gctact*pr                                      
      return                                                            
      end                                                               
