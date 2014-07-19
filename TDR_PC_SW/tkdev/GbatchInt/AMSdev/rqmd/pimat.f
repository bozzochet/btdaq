CPAT,PIMAT    .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine piweg(i1,i2,i3,i4,nop,imixt,                           
     a                                  itag1,itag2,ichanl,srt,icltyp)  
        include 'scatter7'                                              
        include 'partblk7'                                              
      parameter( fstrsq=4.5238)                                         
      parameter( em1=0.138,npd1=0,em2=0.938,npd2=0)                     
      parameter( parama=fstrsq/em1/em1/6./3.141592*em2)                 
      parameter( paramb=20./3.*fstrsq/em1/em1/(5.0674)**2 )             
      parameter( b0im=0.035/em1**4 )                                    
      parameter( dsemax=0.04 )                                          
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 1.09 -- 14-jul-93'/                     
      data pronam/'p i w e g  -- pion  absorption'/                     
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      kyes=0                                                            
      if(srt.le. emnuc+empion.or.srt.gt.1.5) return                     
      if(iqcnvt.ne.0.or.itag1.ne.0.or.itag2.ne.0) then                  
        kyes=1                                                          
      end if                                                            
      ppr= sqrt(((srt*srt-emnuc*emnuc-empion*empion)/                   
     a                                 (2.*emnuc))**2-empion*empion)    
      pcms=pcmsr(srt,emnuc,empion)                                      
      gamd=parama/srt*pcms**3                                           
      sigin=paramb/(srt/emnuc*pcms)*                                    
     a               pcms**2*dsemax/((srt-emdelt)**2+0.25*gamd**2)      
      pimsmx=sigin                                                      
      pilu=sqrt(empion*empion+ppr*ppr)                                  
      tpion=pilu-empion                                                 
      sigsab=1.5*(hc/pi2)**5*20.*pi2/ppr*                               
     a             (1.+0.5*pilu/empion)*b0im*rho0*                      
     b                            exp(-6.*max(cvalu0,tpion-.1))         
      sigin=sigin+sigsab                                                
      if(ranf(0).gt.sigsab/sigin) then                                  
            sigsab=0.                                                   
      end if                                                            
      itp1=ityp(i1)                                                     
      itp2=ityp(i2)                                                     
      call cross1(1,srt,pcms,icltyp,sig,em1,em2,                        
     a                                  itp1,itp2,npd1,npd2)            
      if(kyes.eq.1.or.ida(39).eq.0) then                                
         if(ranf(0).lt.sigin/sig) ichanl=0                              
      else                                                              
        zrand=ranf(0)                                                   
        if(zrand.gt.sigin/sig) then                                     
          gmdlt= 29.*pcms**3/(1.+40.*pcms*pcms)                         
          sigsab=gmdlt/((srt-emdelt)**2+0.25*gmdlt**2)                  
          if(srt.gt.1.3) then                                           
             xsdelt=10.*(hc/pi2)**2*pi2/pcms**2*sigsab*gmdlt            
             if(imixt.eq.1) then                                        
             elseif(imixt.eq.2) then                                    
               xsdelt=0.3333*xsdelt                                     
             else if(imixt.eq.3) then                                   
               xsdelt=0.6667*xsdelt                                     
             end if                                                     
             if(ranf(0)*(sig-sigin).gt.xsdelt) return                   
          end if                                                        
          sigsab=-sigsab                                                
        end if                                                          
        call imsed(srt,pimsed,                                          
     a               pimsmx,sigsab,icltyp,i1,i2,i3,i4,ichanl,nop)       
      end if                                                            
      return                                                            
      end                                                               
      subroutine ndmed(i1,i2,i3,i4,nop,srt,                             
     a                  idcid1,idcid2,itag1,itag2,                      
     b                  icltyp,ityp1,npid1,ityp2,npid2,em1,em2,ichanl)  
        include 'scatter7'                                              
        include 'partblk7'                                              
      if(ida(39).eq.1.and.idcid1.eq.0.and.idcid2.eq.0                   
     a       .and.iabs(npidec(i1)).le.1.and.iabs(npidec(i2)).le.1       
     b            .and.itag1.eq.0.and.itag2.eq.0.and.iqcnvt.eq.0) then  
      else                                                              
        if(ranf(0).gt.1./enhcnd) then                                   
         ichanl=0                                                       
         call rstifo(i1,1)                                              
         call rstifo(i2,2)                                              
        end if                                                          
        return                                                          
      end if                                                            
      if(ityp(i1).ge.3.and.ityp(i1).le.6) then                          
         ia=i1                                                          
         ib=i2                                                          
         emd=em1                                                        
      else if(ityp(i2).ge.3.and.ityp(i2).le.6) then                     
         emd=em2                                                        
         ia=i2                                                          
         ib=i1                                                          
      else                                                              
          call errex('no delta in entrance, abort in ndmed')            
      end if                                                            
      prel=pcmsr(emd,emnuc,empion)                                      
      gamd= 29.*prel**3/(1.+40.*prel*prel)                              
      pimsmx=gamd                                                       
      call imsed(srt,pimsed,pimsmx,sigdum,                              
     a                    icltyp,ia,ib,i3,i4,ichanl,nop)                
      if(ichanl.eq.0) then                                              
         call rstifo(i1,1)                                              
         call rstifo(i2,2)                                              
      end if                                                            
      return                                                            
      end                                                               
      subroutine imsed(srt,pimsed,pimsmx,sigsab,                        
     a                                icltyp,i1,i2,i3,i4,ichanl,nop)    
        include 'scatter7'                                              
        include 'partblk7'                                              
        dimension qlk(0:3),plk(0:3),xind(0:3),p1(0:3),p2(0:3),pil(0:3)  
        dimension prest(0:3),presto(0:3)                                
        data fmrho0,ds2 /0.27,2.165/                                    
        data fstrsq / 4.5238/                                           
       real*4 force,dcoef                                               
       common/blocal/ ngbor(ntotma),force(0:3,ntotma),dcoef(ntotma)     
       common /pimabk/                                                  
     a            ds2n,b0im,parama,paramb                               
        logical*1 ttvers                                                
        data ttvers/ .true./                                            
        cq(dum1)=-5.19*dum1*dum1+15.35*dum1+2.06                        
        ca2(dum2)=1.06*dum2*dum2-6.64*dum2+22.66                        
        ca3(dum3)=-13.46*dum3*dum3+46.17*dum3-20.34                     
        alp(dum4)=0.382*dum4*dum4-1.322*dum4+1.466                      
        bet(dum5)=-0.038*dum5*dum5+0.204*dum5+0.613                     
      if(ttvers) then                                                   
          ds2n=(pi*ds2)**1.5 * rho0                                     
          b0im= 0.035/empion**4                                         
          parama= fstrsq/empion/empion/6./pi*emnuc                      
          paramb= 20./3.*fstrsq/empion/empion*(hc/pi2)**2               
          ttvers=.false.                                                
      end if                                                            
      if(icltyp.le.3) then                                              
         iclas=3                                                        
         i3delt=2*ityp(i1)-9                                            
         if(iabs(i3delt).eq.1) then                                     
            if(ranf(0).gt.0.6667) then                                  
               ichgpi=isign(1,i3delt)                                   
            else                                                        
               ichgpi=0                                                 
            end if                                                      
         else                                                           
            ichgpi=isign(1,i3delt)                                      
         end if                                                         
         if(ityp(i1).eq.3) then                                         
            iqabs=-1                                                    
            itp1n=1                                                     
         else if(ityp(i1).eq.6) then                                    
            iqabs=1                                                     
            itp1n=2                                                     
         else                                                           
            iqabs=0                                                     
            itp1n=ityp(i1)-3                                            
         end if                                                         
         itp2n=ityp(i2)                                                 
      else if(ityp(i1).ge.3.and.ityp(i1).le.6) then                     
         call errex('iclas=1 not allowed in imsed')                     
         iclas=1                                                        
         if(ityp(i1).eq.3) then                                         
            iqabs=-1                                                    
            itp1n=1                                                     
         else if(ityp(i1).eq.6) then                                    
            iqabs=1                                                     
            itp1n=2                                                     
         else                                                           
            iqabs=0                                                     
            itp1n=ityp(i1)-3                                            
         end if                                                         
         iqs=ityp(i2)-1+iqabs                                           
         if(iqs.eq.0.or.iqs.eq.1) then                                  
           itp2n=iqs+1                                                  
           iqabs=0                                                      
         else                                                           
           itp2n=ityp(i2)                                               
         end if                                                         
      else if(ityp(i1).ge.7.and.ityp(i1).le.9) then                     
         iclas=2                                                        
         ichgpi=ityp(i1)-8                                              
         itp1n=-9999                                                    
         iqabs=ityp(i1)-8                                               
         iqs=ityp(i2)-1+iqabs                                           
         if(iqs.eq.0.or.iqs.eq.1) then                                  
           itp2n=iqs+1                                                  
           iqabs=0                                                      
         else                                                           
           if(iqs.eq.2) then                                            
             itp2n=2                                                    
             iqabs=1                                                    
           else if(iqs.eq.-1) then                                      
             itp2n=1                                                    
             iqabs=-1                                                   
           end if                                                       
         end if                                                         
      else                                                              
          call errex('wrong particle type for absorption in imsed')     
      end if                                                            
      if(iabs(iqabs).gt.1) then                                         
          call errex('remaining charge exceeds -1 in imsed')            
      end if                                                            
      i3=0                                                              
      i4=0                                                              
      pimsed=0.                                                         
      pimsqe=0.                                                         
      innch=2                                                           
      nrb=nrbar(kclst)                                                  
      do  9 i=0,3                                                       
    9   xind(i)=x(i,i2)+qdot(i,i2)*dtprop                               
        do 33 i=0,3                                                     
   33   prest(i)=0.                                                     
      do 30 l=1,nrb                                                     
       prop(l)=.false.                                                  
       dcoef(l)=0.                                                      
       il=iofset+l                                                      
       if(ityp(il).ne.1.and.ityp(il).ne.2) go to 30                     
       identl=issn(il)                                                  
       if(itdth(2,identl).ne.0 ) go to 30                               
       if(qdot(0,il).eq.0.) then                                        
         do 10 i=0,3                                                    
   10     pil(i)=p(i,il)                                                
       else                                                             
         do 20 i=0,3                                                    
   20     pil(i)=qdot(i,il)                                             
       end if                                                           
       do 11 i=0,3                                                      
        qlk(i)=x(i,il)-xind(i)                                          
        plk(i)=p(i,il)+p(i,i2)                                          
   11  continue                                                         
       plplin =pil(0)*plk(0)                                            
       dqplin =qlk(0)*plk(0)                                            
       do 12 i=1,3                                                      
        plplin=plplin-pil(i)*plk(i)                                     
        dqplin=dqplin-qlk(i)*plk(i)                                     
   12  continue                                                         
       do 13 i=0,3                                                      
       qlk(i)=qlk(i)- dqplin/plplin *pil(i)                             
   13  continue                                                         
       dqlksq = qlk(0) *qlk(0)                                          
       do 14 i=1,3                                                      
        dqlksq = dqlksq - qlk(i) *qlk(i)                                
   14  continue                                                         
       zlk= dqlksq                                                      
       if(sqrt(max(cvalu0,-zlk)).gt.3.) go to 30                        
       prop(l)=.true.                                                   
       dcoef(l)=exp(zlk/ds2)/ds2n                                       
       if(iclas.eq.3.and.il.eq.i2) go to 30                             
        do 29 i=0,3                                                     
   29   prest(i)=prest(i)+p(i,il)                                       
   30 continue                                                          
      fm=2.*fmrho0                                                      
      fmact=fmrho0                                                      
      if(iclas.eq.3.and.prest(0).eq.0.) then                            
         if(ranf(0).gt.1./enhcnd) ichanl=0                              
         return                                                         
      end if                                                            
      dens=0.                                                           
      cmsen=                                                            
     a   sqrt(prest(0)**2-(prest(1)**2+prest(2)**2+prest(3)**2))        
      niter=0                                                           
   34 niter=niter+1                                                     
      denso=dens                                                        
      cmseno=cmsen                                                      
      cmsen=                                                            
     a   sqrt(prest(0)**2-(prest(1)**2+prest(2)**2+prest(3)**2))        
      if(cmsen.le.0.1) then                                             
        inabs=0                                                         
        go to 140                                                       
      else                                                              
        do 35 i=0,3                                                     
   35   prest(i)=prest(i)/cmsen                                         
      end if                                                            
      dens=0.                                                           
      do 36 i=0,3                                                       
      presto(i)=prest(i)                                                
      prest(i)=0.                                                       
   36 continue                                                          
      inabs=0                                                           
      pnabsp=0.                                                         
      pnabsn=0.                                                         
      do 40 l=1,nrb                                                     
       if(dcoef(l).eq.0.) go to 40                                      
       il=iofset+l                                                      
       pilu=p(0,il)*presto(0)                                           
       do 37 i=1,3                                                      
   37  pilu=pilu-p(i,il)*presto(i)                                      
       pilrst=-(p(0,il)-pilu*presto(0))**2                              
       do 38 i=1,3                                                      
   38  pilrst=pilrst+(p(i,il)-pilu*presto(i))**2                        
       fmlmt2=fm**2                                                     
       if(pilrst.gt.fmlmt2) go to 40                                    
       inabs=inabs+1                                                    
       if(ityp(il).eq.1) then                                           
         pnabsn=pnabsn+1.                                               
       else if(ityp(il).eq.2) then                                      
         pnabsp=pnabsp+1.                                               
       end if                                                           
       dens=dens+dcoef(l)                                               
       if(iclas.eq.3.and.il.eq.i2) go to 40                             
       do 39 i=0,3                                                      
   39  prest(i)=prest(i)+p(i,il)                                        
   40 continue                                                          
  140 continue                                                          
      if(iclas.eq.3.and.inabs.le.1) then                                
         if(ranf(0).gt.1./enhcnd) ichanl=0                              
         return                                                         
      else if(iclas.eq.2.and.inabs.le.1) then                           
         if(sigsab.ge.0.) then                                          
            ichanl=0                                                    
         end if                                                         
         return                                                         
      else if(iclas.eq.1.and.inabs.le.1) then                           
         if(srt.gt.em(i1)+em(i2)) then                                  
            ema=em(i1)                                                  
            emb=em(i2)                                                  
            prel=pcmsr(srt,ema,emb)                                     
            call detbal(srt,prel,icltyp,ityp(i1),npidec(i1),ityp(i2),   
     a                                  npidec(i2),em(i1),em(i2),sigdb) 
            pimsed=sigdb                                                
            go to 51                                                    
         else                                                           
            ichanl=0                                                    
            return                                                      
         end if                                                         
      end if                                                            
      if(niter.lt.5.and.((abs(dens-denso).gt.0.1.and.                   
     a                  dens.gt.0.1) .or.abs(cmsen-cmseno).gt.2.)) then 
        fm=max(dens**0.33333*fmrho0,fmrho0)                             
        go to 34                                                        
      else                                                              
        cmsen=                                                          
     a     sqrt(prest(0)**2-(prest(1)**2+prest(2)**2+prest(3)**2))      
        pilu=p(0,i1)*presto(0)                                          
        do 42 i=1,3                                                     
   42   pilu=pilu-p(i,i1)*presto(i)                                     
        pilrst=-(p(0,i1)-pilu*presto(0))**2                             
        do 43 i=1,3                                                     
   43   pilrst=pilrst+(p(i,i1)-pilu*presto(i))**2                       
        if(iclas.eq.2) then                                             
          xsdelt=0.                                                     
          tpion=sqrt(empion*empion+pilrst)-empion                       
          if(sigsab.gt.0.) then                                         
            sigsw=(hc/pi2)**5*20.*pi2/sqrt(pilrst)*                     
     a             (1.+0.5*pilu/empion)*b0im*dens*rho0*                 
     b                            exp(-6.*max(cvalu0,tpion-.1))         
            if(sigsw.gt.sigsab) then                                    
               write(8,*) 'warning from imsed: sigsw.gt. sigsab:',      
     a                       sigsw,sigsab,tpion,dens                    
            end if                                                      
            rands=ranf(0)                                               
            if(rands.lt.sigsw/sigsab) then                              
              pimsed=pimsmx                                             
              go to 71                                                  
            else                                                        
              ichanl=0                                                  
              return                                                    
            end if                                                      
          else if(sigsab.lt.0.) then                                    
            xsdelt=-sigsab                                              
          end if                                                        
          emd=srt                                                       
          pilu=sqrt(emd*emd+pilrst)                                     
          bdelt=sqrt(pilrst)/pilu                                       
        else if(iclas.eq.1) then                                        
          emd=sqrt(pilu**2-pilrst)                                      
          if(emd.le.emnuc+empion) then                                  
             ichanl=0                                                   
             return                                                     
          end if                                                        
          bdelt=sqrt(pilrst)/pilu                                       
          tpion=sqrt(empion**2+pilrst)-empion                           
          write(8,*) 'imsed delta absorption: tpion=',tpion,bdelt       
        else if(iclas.eq.3) then                                        
          emd=sqrt(pilu**2-pilrst)                                      
          bdelt=sqrt(pilrst)/pilu                                       
          if(emd.le.emnuc+empion) then                                  
              write(8,*) 'imsed-warning:iclas=3, delta mass too small'  
              ichanl=0                                                  
              return                                                    
          end if                                                        
          tpion=sqrt(empion**2+pilrst)-empion                           
        end if                                                          
      end if                                                            
      if(tpion.lt.0.) then                                              
            call errex('imsed:negative kinetic pion energy')            
            ichanl=0                                                    
            return                                                      
      end if                                                            
      chiiso=(pnabsn-pnabsp)/(pnabsn+pnabsp)                            
      xtpion=tpion/empion                                               
      betx=max(cvalu0,bet(xtpion))                                      
      if(iclas.eq.1) then                                               
        vmltp=(1.+0.33333*(2*ityp(i1)-9)*chiiso)                        
      else if(iclas.eq.2.or.iclas.eq.3) then                            
        vmltp=1.5*(0.666667+ichgpi*chiiso*0.22222 +                     
     a                         chiiso**2*(1.-abs(ichgpi)*0.55555))      
      end if                                                            
      vcnvrt=bdelt                                                      
      vrats=0.                                                          
      nn1=0                                                             
      itp=0                                                             
      lstart=int(ranf(0)*nrb)                                           
      do 70 leff=lstart,lstart+nrb-1                                    
        l=mod(leff,nrb)+1                                               
        il=iofset+l                                                     
        if(il.eq.i2) go to 70                                           
        if(.not.prop(l)) go to 70                                       
        itp=il                                                          
        if(lmax.ge.ntotma-nspect) then                                  
          go to 72                                                      
        else                                                            
          itq=ntotma-nspect                                             
          if(iclas.eq.1.or.iclas.eq.3) then                             
            do 73 i=0,3                                                 
   73       p(i,itq)=p(i,i1)                                            
          else                                                          
            do 74 i=0,3                                                 
   74       p(i,itq)=p(i,i1)+p(i,i2)                                    
          end if                                                        
        end if                                                          
        nn1=nn1+1                                                       
        issn(itq)=ntotma                                                
        call frzcor(ratv,itp,itq)                                       
        vrats=vrats+ratv                                                
        if(nn1.eq.3) go to 72                                           
   70  continue                                                         
   72  continue                                                         
      if(nn1.gt.0) then                                                 
         ratv=vrats/nn1                                                 
         vcnvrt=ratv*vcnvrt                                             
      end if                                                            
      if(ida(4).eq.2) then                                              
        if(tpion.gt.0.315) then                                         
           srtact=(emnuc+sqrt(emd**2+pilrst))**2-pilrst                 
           srtact=sqrt(srtact)                                          
           prdn=pcmsr(srtact,emd,emnuc)                                 
           call detbal(srtact,prdn,5,3,0,2,                             
     a                                  0,emd,emnuc,sigdb)              
           sigdb=0.5*vmltp*sigdb                                        
           frctwo=50.*sigdb*vcnvrt*(dens*rho0)/pi2*hc                   
        else                                                            
          catwo=ca2(xtpion)                                             
          frctwo=vmltp*catwo*dens**betx                                 
        end if                                                          
      else                                                              
        srtact=(emnuc+sqrt(emd**2+pilrst))**2-pilrst                    
        srtact=sqrt(srtact)                                             
        prdn=pcmsr(srtact,emd,emnuc)                                    
        call detbal(srtact,prdn,5,3,0,2,                                
     a                                  0,emd,emnuc,sigdb)              
        sigdb=0.5*vmltp*sigdb                                           
        frctwo=50.*sigdb*vcnvrt*(dens*rho0)/pi2*hc                      
      end if                                                            
      if(iclas.eq.1) then                                               
          icltp=icltyp                                                  
      else if(iclas.eq.2.or.iclas.eq.3) then                            
        prbntr=pnabsn/(pnabsn+pnabsp)                                   
        if(ranf(0).lt.prbntr) then                                      
             itp3f=1                                                    
        else                                                            
             itp3f=2                                                    
        end if                                                          
        if(iclas.eq.2) then                                             
             itp1f=ityp(i1)+ityp(i2)-5                                  
             if(itp1f.lt.3.or.itp1f.gt.6) then                          
                call errex('wrong itp1f in imsed')                      
             end if                                                     
        else                                                            
             itp1f=ityp(i1)                                             
        end if                                                          
        icltp=itp1f*(itp1f-1)/2+itp3f                                   
      end if                                                            
      if(ida(4).eq.1) then                                              
        emn=emnuc                                                       
        call wqdnch(icltp,itp1f,itp3f,0,0,srtact,prdn,emd,emn,          
     a                       sigeno,sigel1,sigel2,sigdd1,sigdd2,sigdb)  
        sigsum=sigeno+sigel1+sigel2+sigdd1+sigdd2                       
        pimsqe=0.05*sigsum*vcnvrt*(dens*rho0)/pi2*hc                    
        pimsed=frctwo*1.e-3                                             
      else                                                              
        pimsed=(frctwo+max(cvalu0,ca3(xtpion))*dens**(2.*betx))*1.e-3   
        alpx=max(cvalu0,alp(xtpion))                                    
        pimsqe=max(cvalu0,cq(xtpion))*dens**alpx*1.e-3                  
        if(icltp.eq.17.or.icltp.eq.4 ) then                             
            enlrg=1.5                                                   
        else if(icltp.eq.16.or.icltp.eq.5 ) then                        
            enlrg=0.5                                                   
        else if(icltp.eq.12.or.icltp.eq.7 ) then                        
            enlrg=7./6.                                                 
        else if(icltp.eq.11.or.icltp.eq.8 ) then                        
            enlrg=5./6.                                                 
        else                                                            
           write(8,*) 'wrong icltp in imsed',icltp,iclas                
           call errex('unclear situation')                              
        end if                                                          
        pimsqe=enlrg*pimsqe                                             
      end if                                                            
      prel=pcmsr(emd,emnuc,empion)                                      
      if(iclas.eq.2) then                                               
         pimsed=pimsqe+pimsed                                           
         valmu=(pilu*sqrt(emnuc*emnuc+prel*prel)-                       
     a                   sqrt(emnuc*emnuc+fmact*fmact)*emd)/            
     b                                               (prel*sqrt(pilrst))
         if(dens.lt.1..and.dens.gt.0.01) then                           
             valmu=valmu/dens                                           
         end if                                                         
         valmu=max(min(cvalu1,valmu),-cvalu1)                           
         gamdd=parama/srt*prel**3                                       
         gmims=2.*pimsed                                                
         gmeff=gamdd*0.25*(2.+valmu+valmu**3)+gmims                     
         pimsvf=paramb/srt*emnuc*prel/((srt-emdelt)**2+0.25*gmeff**2)   
         pimsed=pimsvf*pimsed                                           
         pimsqe=pimsvf*pimsqe                                           
         if(xsdelt.gt.0.) then                                          
            gmdlt= 29.*prel**3/(1.+40.*prel*prel)                       
            gmeff=gmdlt*0.25*(2.+valmu+valmu**3)                        
            enhnce=gmeff/xsdelt/((emd-emdelt)**2+0.25*(gmeff+gmims)**2) 
            if(enhnce.gt.1.) then                                       
              write(8,*) 'warning: iclas=2 in imsed, enhnce>1.'         
              write(8,*) srt,icltyp,emd,enhnce,gmdlt,gmeff,valmu        
              return                                                    
            else                                                        
              if(ranf(0).gt.enhnce) ichanl=0                            
              return                                                    
            end if                                                      
         end if                                                         
      else if(iclas.eq.1.or.iclas.eq.3) then                            
        if(iclas.eq.1) then                                             
          pimsed=20.*pimsed/vcnvrt/(dens*rho0) * pi2/hc                 
          pimsqe=20.*pimsqe/vcnvrt/(dens*rho0) * pi2/hc                 
          pimsed=pimsqe+pimsed                                          
        else if(iclas.eq.3) then                                        
            pimsed=pimsed+pimsqe                                        
            gmdlt=pimsmx                                                
            valmu=(pilu*sqrt(emnuc*emnuc+prel*prel)-                    
     a                   sqrt(emnuc*emnuc+fmact*fmact)*emd)/            
     b                                               (prel*sqrt(pilrst))
            valmu=max(min(cvalu1,valmu),-cvalu1)                        
            gmeff=gmdlt*0.25*(2.+valmu+valmu**3)+2.*pimsed              
            enhnce=((emd-emdelt)**2+0.25*gmdlt**2)/                     
     a                      ((emd-emdelt)**2+0.25*gmeff**2)*gmeff/gmdlt 
            if(pimsed.eq.0.) then                                       
               if(ranf(0).gt.1./enhcnd) ichanl=0                        
               return                                                   
            else                                                        
               pimsmx=pimsed*enhcnd/enhnce                              
            end if                                                      
        end if                                                          
      end if                                                            
   51 continue                                                          
      rand=ranf(0)                                                      
      if((rand.gt.pimsed/pimsmx).or.                                    
     a   (ida(4).eq.2.and.ranf(0).lt.pimsqe/pimsed))then                
         ichanl=0                                                       
         return                                                         
      else if(ida(4).eq.1) then                                         
        return                                                          
      end if                                                            
      pimsqe=0.                                                         
      if(iclas.eq.1) then                                               
      elseif(iclas.eq.2) then                                           
          if(pimsqe.gt.0. .and.ranf(0).lt.pimsqe/pimsed) then           
                  innch=3                                               
                  iqabs=-2*itp3f+3                                      
          else                                                          
                  innch=2                                               
          end if                                                        
      else if(iclas.eq.3) then                                          
            if(ranf(0).lt.gmdlt/gmeff) then                             
                  return                                                
            end if                                                      
            if(pimsqe.gt.0. .and.ranf(0).lt.pimsqe/pimsed) then         
                  innch=3                                               
                  iqabs=-2*itp3f+3                                      
            else                                                        
                  innch=2                                               
            end if                                                      
      end if                                                            
   71 continue                                                          
      if(iclas.eq.2.or.iclas.eq.3.or.iqabs.ne.0) then                   
       lstart=int(ranf(0)*nrb)                                          
       do 50 leff=lstart,lstart+nrb-1                                   
        l=mod(leff,nrb)+1                                               
        il=iofset+l                                                     
        if(il.eq.i2) go to 50                                           
        iqs=ityp(il)-1+iqabs                                            
        if(.not.prop(l)) go to 50                                       
        if(iqs.ne.0.and.iqs.ne.1) go to 50                              
        i3=il                                                           
        itp3n=iqs+1                                                     
        go to 52                                                        
   50  continue                                                         
   52  continue                                                         
       if(i3.eq.0) then                                                 
          ichanl=0                                                      
          return                                                        
       end if                                                           
      end if                                                            
      if(i3.eq.0) then                                                  
         nop=2                                                          
      else                                                              
         nop=3                                                          
         lastcl(i3)=ievntn                                              
         if(qdot(0,i3).eq.0.) then                                      
            do 110 i=0,3                                                
  110       pil(i)=p(i,i3)                                              
         else                                                           
            do 120 i=0,3                                                
  120       pil(i)=qdot(i,i3)                                           
         end if                                                         
         do 111 i=0,3                                                   
          qlk(i)=x(i,i3)-xind(i)                                        
          plk(i)=p(i,i3)+p(i,i2)                                        
  111    continue                                                       
         plplin =pil(0)*plk(0)                                          
         dqplin =qlk(0)*plk(0)                                          
         do 112 i=1,3                                                   
          plplin=plplin-pil(i)*plk(i)                                   
          dqplin=dqplin-qlk(i)*plk(i)                                   
  112    continue                                                       
         delts=max(cvalu0,dqplin/plplin)                                
         if(tlast(i3).gt.x(0,i3)) then                                  
            delts=0.                                                    
         else                                                           
            delts=min(delts,(x(0,i3)-tlast(i3))/pil(0))                 
         end if                                                         
         if(qdot(0,i3).eq.0.) then                                      
            do 210 i=0,3                                                
  210       x(i,i3)=x(i,i3)-delts*pil(i)                                
         else                                                           
            do 220 i=0,3                                                
  220       qdot(i,i3)=-qdot(i,i3)*delts/dtprop                         
         end if                                                         
      end if                                                            
      if(innch.eq.3) then                                               
          if(icltp.eq.17.or.icltp.eq.4 ) then                           
            prb1=0.                                                     
          else if(icltp.eq.16.or.icltp.eq.5 ) then                      
            prb1=0.25                                                   
          else if(icltp.eq.12.or.icltp.eq.7 ) then                      
            prb1=1./28.                                                 
          else if(icltp.eq.11.or.icltp.eq.8 ) then                      
            prb1=0.1                                                    
          end if                                                        
          if(ranf(0).lt.prb1) then                                      
             j=2                                                        
          else                                                          
             j=4                                                        
          end if                                                        
          j1new=1                                                       
          j2new=3                                                       
          m=2*(itp1f+itp3f)-12                                          
          if(iabs(m).gt.4.or.iabs(m).gt.j) then                         
             call errex('imsed:innch=3, m out of range')                
          end if                                                        
      end if                                                            
      if(iclas.eq.1) then                                               
         em(i1)=emnuc                                                   
         ia=i1                                                          
         ib=i2                                                          
         em1eff=emnuc                                                   
         em2eff=emnuc                                                   
      else if(iclas.eq.2) then                                          
         em(i1)=0.                                                      
         do 53 i=0,3                                                    
         p(i,i2)=p(i,i1)+p(i,i2)                                        
   53    p(i,i1)=0.                                                     
         ia=i2                                                          
         ib=i3                                                          
         if(innch.eq.3) then                                            
            em1eff=emnuc                                                
            emd=p(0,i2)*p(0,i2)                                         
            do 54 i=1,3                                                 
   54       emd=emd-p(i,i2)*p(i,i2)                                     
            emd=sqrt(emd)                                               
            sact=(p(0,i2)+p(0,i3))**2                                   
            do 55 i=1,3                                                 
   55       sact=sact-(p(i,i2)+p(i,i3))**2                              
            sact=sqrt(sact)                                             
            if(sact.le.2.*emnuc+empion+ekinmi) then                     
               write(8,*) 'iclas=',iclas,'imsed:missing cms energy',    
     a                        ' for chrg. exch. ',sact                  
               ichanl=0                                                 
               return                                                   
            else if(sact.lt.emnuc+emd) then                             
               emd=max(emnuc+empion,0.5*(empion+sact))                  
            end if                                                      
            em2eff=emd                                                  
            em(i3)=emd                                                  
            itag=-50                                                    
            call isonew(j,m,j1new,j2new,m1new,m2new,itag)               
            itp2n=(m1new+3)/2                                           
            itp3n=(m2new+9)/2                                           
         else                                                           
            em1eff=emnuc                                                
            em2eff=emnuc                                                
         end if                                                         
      else if(iclas.eq.3) then                                          
         ia=i1                                                          
         ib=i3                                                          
         if(innch.eq.2) then                                            
            em1eff=emnuc                                                
            em2eff=emnuc                                                
         elseif(innch.eq.3) then                                        
            em1eff=emnuc                                                
            sact=(p(0,i1)+p(0,i3))**2                                   
            do 56 i=1,3                                                 
   56       sact=sact-(p(i,i1)+p(i,i3))**2                              
            sact=sqrt(sact)                                             
            if(sact.le.2.*emnuc+empion+ekinmi) then                     
               write(8,*) 'iclas=',iclas,'imsed:missing cms energy',    
     a                        ' for chrg. exch. ',sact                  
               return                                                   
            else if(sact.lt.emnuc+em(i1)) then                          
               emd=max(emnuc+empion,0.5*(empion+sact))                  
            end if                                                      
            em2eff=emd                                                  
            em(i3)=em2eff                                               
            itag=-50                                                    
            call isonew(j,m,j1new,j2new,m1new,m2new,itag)               
            itp1n=(m1new+3)/2                                           
            itp3n=(m2new+9)/2                                           
         end if                                                         
         em(i1)=emnuc                                                   
      end if                                                            
      ityp(i1)=itp1n                                                    
      npidec(i1)=0                                                      
      ityp(i2)=itp2n                                                    
      npidec(i2)=0                                                      
      if(nop.eq.3) then                                                 
        ityp(i3)=itp3n                                                  
        if(innch.eq.3) then                                             
          npidec(i3)=1                                                  
        else                                                            
          npidec(i3)=0                                                  
        end if                                                          
      end if                                                            
      do 60 i=0,3                                                       
        p1(i)=p(i,ia)                                                   
        p2(i)=p(i,ib)                                                   
   60 continue                                                          
      call onshel(1,isccss,em1eff,em2eff,p1(0),p1(1),p1(2),p1(3),       
     a                                       p2(0),p2(1),p2(2),p2(3))   
      if(isccss.eq.1) then                                              
         do 61 i=0,3                                                    
          p(i,ia)=p1(i)                                                 
          p(i,ib)=p2(i)                                                 
   61    continue                                                       
      else                                                              
         call errex('isccss.ne.1 in imsed,?????????')                   
      end if                                                            
      if(lmax.ge.ntotma-nspect) then                                    
         call errex('no free line in particle array, abort in imsed')   
      else                                                              
         ltest=ntotma-nspect                                            
         do 65 i=0,3                                                    
   65    plk(i)=p1(i)+p2(i)                                             
         cmsen=sqrt(plk(0)**2-(plk(1)**2+plk(2)**2+plk(3)**2))          
         prel=pcmsr(cmsen,em1eff,em2eff)                                
         cthet=1.-2.*ranf(0)                                            
         sthet=sqrt(1.-cthet*cthet)                                     
         phi=pi2*ranf(0)                                                
         p(1,ltest)=prel*cos(phi)*sthet                                 
         p(2,ltest)=prel*sin(phi)*sthet                                 
         p(3,ltest)=prel*cthet                                          
         p(0,ltest)=sqrt(em1eff*em1eff+prel*prel)                       
         bx=plk(1)/plk(0)                                               
         by=plk(2)/plk(0)                                               
         bz=plk(3)/plk(0)                                               
         call trobo(1,ltest,ltest,cvalu0,cvalu0,bx,by,bz)               
         do 69 i=0,3                                                    
         p(i,ia)=p(i,ltest)                                             
         p(i,ib)=plk(i)-p(i,ia)                                         
   69    continue                                                       
      end if                                                            
      return                                                            
      end                                                               
      function  klbar(ifla3,iflb3,iflc3)                                
       if(ifla3.le.0.or.iflb3.le.0.or.iflc3.le.0) then                  
          call errex('abort in klbar')                                  
       end if                                                           
       ifla1=max(ifla3,iflb3)                                           
       iflc1=min(ifla3,iflb3)                                           
       ifla1=max(ifla1,iflc3)                                           
       iflc1=min(iflc1,iflc3)                                           
       iflb1=ifla3+iflb3+iflc3-ifla1-iflc1                              
       if(ifla1.eq.3.and.ifla1.eq.iflc1) then                           
         klbar=70                                                       
       else if(ifla1.eq.3.and.ifla1.eq.iflb1) then                      
         klbar=67+iflc1                                                 
       else if(ifla1.eq.3.and.ifla1.gt.iflb1) then                      
         klbar=41+iflb1+iflc1                                           
       else                                                             
         klbar=58+ifla1+iflb1+iflc1                                     
       end if                                                           
      return                                                            
      end                                                               
      subroutine fill12(ityp1,ityp2,npid1,npid2,itypi,npidi,            
     a                                                  em1,em2,srt)    
        include 'scatter7'                                              
        em1=srt                                                         
        em2=0.                                                          
        ityp1=itypi                                                     
        npid1=npidi                                                     
        ityp2=-9999                                                     
        npid2=0                                                         
      return                                                            
      end                                                               
