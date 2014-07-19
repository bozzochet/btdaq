CPAT,STRIST   .                                                         
      subroutine strist                                                 
      include 'scatter7'                                                
      real*4 plund,par                                                  
      real*4 bex,bey,bez,thetlu,philu                                   
      real*4 embang,bpli,emtrue,sigptr                                  
      logical reject                                                    
      parameter (kbpmax=750)                                            
      logical icross(2,kbpmax),newbp(kbpmax)                            
      dimension ptot(0:3),xbang(0:3)                                    
      dimension pq(0:3),pqbar(0:3)                                      
      dimension p1(0:3),p2(0:3),psum(0:3),ptsum(2)                      
      include 'partblk7'                                                
      parameter (mxs=200)                                               
      parameter (mxs1=mxs+1)                                            
      common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout                
      dimension pqt(0:3,mxs)                                            
      dimension  isingl(mxs),jqpart(mxs),iflstr(2,mxs),psingl(2)        
      dimension  kstar(mxs),kende(mxs)                                  
      real*4  barwgh,para11                                             
      dimension  iqqkf(2,mxs),mqflav(2,mxs)                             
      common /rop2bk/ kparnt(3,mxs),                                    
     a   xori(0:3,mxs),pqu(0:3,mxs),pqb(0:3,mxs)                        
      logical nghbor                                                    
      parameter (kbpmd2=kbpmax/2+1)                                     
      real*4 xhad(4,kbpmd2)                                             
      logical manip                                                     
      common /rop3bk/manip(mxs)                                         
      dimension xlcrop(2),plcrop(2),plcfor(2),pspent(2),ps(2)           
      dimension                                                         
     a                iflv(2,mxs),jord(mxs),iord(2,mxs),                
     b                ipdgr(2,mxs1),iqdgr(2,mxs1),                      
     c                pt(2,mxs),plc(2,mxs),                             
     c                ptstor(2,mxs),jflv(2,mxs),plcone(2,mxs),          
     d                korig(mxs),korsto(mxs)                            
      real*4 xlcbp,xlcmx,equark,ptr                                     
      real*4 dquark(2,kbpmax)                                           
      dimension jetzhd(2,kbpmax)                                        
      common /ropblk/                                                   
     a                ivlc(2,kbpmax),lord(2,kbpmax),                    
     b                ipact(2,kbpmax),iqact(2,kbpmax),ipiq(kbpmax),     
     c                iquark(2,kbpmax),icomb(2,kbpmax),                 
     d                xlcbp(2,kbpmax),xlcmx(2,kbpmax),                  
     e                equark(2,kbpmax),ptr(2,2,kbpmax)                  
      real*4 atrmax,emqcns,emq,ems,prbrop,qmass,atrel,tnsmax,dndx4m     
      parameter (iprbr1=16,iprbr2=iprbr1*(iprbr1+3)/2)                  
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      common /bplibk/ libpmx,bpli(2,kbpmax)                             
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      real*4 angmxv,angeff                                              
      common /su3mix/ angmxv,angeff                                     
      logical ttvers                                                    
      data xcutf /2.5 /                                                 
      data  para11 /0.15/                                               
      data qqdqqs /0.6667/                                              
      data ttvers/.true./                                               
      save  gprd                                                        
      data nsupp,ssupp,qsupp/ 3*0/                                      
      dimsu3(ipd,iqd)=0.5*(ipd+1.0)*(iqd+1.0)*(ipd+iqd+2.0)             
      casop(ipd,iqd)=0.333333*(ipd-iqd)**2+ipd+iqd+ipd*iqd              
      if(ttvers) then                                                   
        ttvers=.false.                                                  
        if(tprd.gt.1.e-30) then                                         
           gprd=1./tprd*hc/pi2                                          
        else                                                            
           gprd=1.e+29                                                  
        end if                                                          
      end if                                                            
       if(iroin.eq.1.or.iroin.eq.0) return                              
       if(iroin.eq.-1) then                                             
         nstrin=1                                                       
         iroin=1                                                        
       else                                                             
         nstrin=0                                                       
       end if                                                           
       nmult=0                                                          
       if(para11/par(11).gt.1.0) then                                   
         call errex('strist problem with para11')                       
       end if                                                           
       par2=par(2)                                                      
       par11=par(11)                                                    
       atre=da(17)**2/0.49                                              
       if(iroin.gt.mxs) then                                            
         call errex('iroin>mxs in strist, abort')                       
         return                                                         
       end if                                                           
       do 30 i=0,3                                                      
        psum(i)=0.0                                                     
        ptot(i)=0.0                                                     
        xbang(i)=0.0                                                    
   30  continue                                                         
       do 40 iro=1,iroin                                                
        manip(iro)=.false.                                              
        ksta=lstar(iro)                                                 
        ke=lende(iro)                                                   
        kstar(iro)=ksta                                                 
        kende(iro)=ke                                                   
        do 26 i=0,3                                                     
   26   pqt(i,iro)=0.0                                                  
        do 28 ik=ksta,ke                                                
         do 27 i=0,3                                                    
   27    pqt(i,iro)=pqt(i,iro)+p(i,ik)                                  
   28   continue                                                        
        do 29 i=0,3                                                     
         ptot(i)=ptot(i)+pqu(i,iro)+pqb(i,iro)                          
         psum(i)=psum(i)+pqt(i,iro)                                     
   29   continue                                                        
   40  continue                                                         
       betax=psum(1)/psum(0)                                            
       betay=psum(2)/psum(0)                                            
       betaz=psum(3)/psum(0)                                            
       bex=-betax                                                       
       bey=-betay                                                       
       bez=-betaz                                                       
       nsave=nlund                                                      
       if(nlund+2*iroin+1.ge.2000) then                                 
         call errex('strist:nlund>2000, abort')                         
       else                                                             
         nlund=nlund+2*iroin                                            
         nl=nsave                                                       
         do 42 iro=1,iroin                                              
          plund(nl+1,4)=pqu(0,iro)                                      
          plund(nl+2,4)=pqb(0,iro)                                      
          do 41 i=1,3                                                   
           plund(nl+1,i)=pqu(i,iro)                                     
           plund(nl+2,i)=pqb(i,iro)                                     
   41     continue                                                      
          nl=nl+2                                                       
   42    continue                                                       
       end if                                                           
       mst(1)=nsave+1                                                   
       mst(2)=nlund                                                     
       call lurobo(0.,0.,bex,bey,bez)                                   
       pabmax=0.0                                                       
       iwo=0                                                            
       do 31 nl=nsave+1,nlund,2                                         
        pabs=sqrt((plund(nl,4)+plund(nl+1,4))**2                        
     a            -((plund(nl,1)+plund(nl+1,1))**2+                     
     b                  (plund(nl,2)+plund(nl+1,2))**2+                 
     c                          (plund(nl,3)+plund(nl+1,3))**2))        
        if(pabs.gt.pabmax) then                                         
           pabmax=pabs                                                  
           iwo=nl                                                       
        end if                                                          
   31  continue                                                         
       nmax=nlund                                                       
       nlund=nlund+1                                                    
       do 11 i=1,4                                                      
   11  plund(nlund,i)=0.0                                               
       do  8 nl=nsave+1,nmax,2                                          
        cthet1=0.0                                                      
        cthet2=0.0                                                      
        pqabs=0.0                                                       
        pqbabs=0.0                                                      
        do  9 i=1,3                                                     
         cthet1=cthet1+plund(nl,i)*plund(iwo,i)                         
         cthet2=cthet2+plund(nl+1,i)*plund(iwo,i)                       
         pqabs=pqabs+plund(nl,i)**2                                     
         pqbabs=pqbabs+plund(nl+1,i)**2                                 
    9   continue                                                        
        pqabs=sqrt(pqabs)                                               
        pqbabs=sqrt(pqbabs)                                             
        cthet1=cthet1/pabmax/pqabs                                      
        cthet2=cthet2/pabmax/pqbabs                                     
        if(nl.eq.iwo.or.nl+1.eq.iwo) then                               
          if(nl.eq.iwo) then                                            
            ivz1=1                                                      
            ivz2=-1                                                     
          else                                                          
            ivz1=-1                                                     
            ivz2=1                                                      
          end if                                                        
        else if(cthet1.gt.cthet2) then                                  
         ivz1=1                                                         
         ivz2=-1                                                        
        else                                                            
         ivz2=1                                                         
         ivz1=-1                                                        
        end if                                                          
        do 12 i=1,4                                                     
   12   plund(nlund,i)=plund(nlund,i)+ivz1*                             
     a                 plund(nl,i)+ivz2*plund(nl+1,i)                   
   8   continue                                                         
       pabs=sqrt(plund(nlund,1)*plund(nlund,1)+                         
     a                  plund(nlund,2)*plund(nlund,2)+                  
     b                          plund(nlund,3)*plund(nlund,3))          
       thetr=acos(max(-cvalu1,min(cvalu1,plund(nlund,3)/pabs)))         
       if(plund(nlund,2).eq.0.0.and.                                    
     a                         plund(nlund,1).eq.0.0) then              
            phir=cvalu0                                                 
       else                                                             
            phir=atan2(plund(nlund,2),plund(nlund,1))                   
       end if                                                           
       thetlu=-thetr                                                    
       philu=-phir                                                      
       kbaryo=0                                                         
       kstran=0                                                         
       kcharg=0                                                         
       nlund=nsave+3                                                    
       mst(1)=nsave+1                                                   
       mst(2)=nlund                                                     
       do 51 iro=1,iroin                                                
        plund(nsave+1,4)=pqb(0,iro)                                     
        do  44 i=1,3                                                    
   44   plund(nsave+1,i)=pqb(i,iro)                                     
        plund(nsave+2,4)=pqu(0,iro)                                     
        do 45 i=1,3                                                     
   45   plund(nsave+2,i)=pqu(i,iro)                                     
        plund(nlund,4)=pqt(0,iro)                                       
        do 43 i=1,3                                                     
   43   plund(nlund,i)=pqt(i,iro)                                       
        call lurobo(0.,0.,bex,bey,bez)                                  
        call lurobo(0.,philu,0.,0.,0.)                                  
        call lurobo(thetlu,0.,0.,0.,0.)                                 
        pqbar(0)=plund(nsave+1,4)                                       
        do 46 i=1,3                                                     
   46   pqbar(i)=plund(nsave+1,i)                                       
        pq(0)=plund(nsave+2,4)                                          
        do 47 i=1,3                                                     
   47   pq(i)=plund(nsave+2,i)                                          
        if(pq(3)*pqbar(3).lt.0.) then                                   
          if(pq(3).gt.0.) then                                          
            iturn=1                                                     
          else                                                          
            iturn=-1                                                    
          end if                                                        
          if(kende(iro).gt.kstar(iro)) then                             
            iflv(2,iro)=0                                               
          else                                                          
            iflv(2,iro)=1                                               
          end if                                                        
        else                                                            
          if(pq(3).gt.pqbar(3)) then                                    
            iturn=1                                                     
          else                                                          
            iturn=-1                                                    
          end if                                                        
          iflv(2,iro)=1                                                 
        end if                                                          
        iflv(1,iro)=iturn                                               
        mpar1=kparnt(1,iro)                                             
        mpar2=kparnt(2,iro)                                             
        if(iturn.eq.-1) then                                            
          kparnt(1,iro)=-mpar1                                          
          kparnt(2,iro)=-mpar2                                          
        end if                                                          
        do 38 i=1,2                                                     
   38   ptstor(i,iro)=plund(nlund,i)                                    
        pplus=plund(nlund,4)+plund(nlund,3)                             
        pminus=plund(nlund,4)-plund(nlund,3)                            
        plcone(1,iro)=pplus                                             
        plcone(2,iro)=pminus                                            
   51  continue                                                         
       do 15 iro=1,iroin                                                
        if(iflv(2,iro).eq.0) go to 15                                   
        jdir=0                                                          
        mpar1=kparnt(1,iro)                                             
        mpar2=kparnt(2,iro)                                             
        if(mpar1.lt.0.or.mpar2.lt.0) then                               
          mpar1=-mpar1                                                  
          mpar2=-mpar2                                                  
          call iswap(mpar1,mpar2)                                       
        end if                                                          
        if(mpar1.gt.n1old+n2old.and.mpar2.gt.n1old+n2old) go to 15      
        do 14 iroeff=1,iroin                                            
         if(iflv(2,iroeff).ne.0) go to 14                               
         if(iro.eq.iroeff) go to 14                                     
         npar1=kparnt(1,iroeff)                                         
         npar2=kparnt(2,iroeff)                                         
         if(npar1.lt.0.or.npar2.lt.0) then                              
           npar1=-npar1                                                 
           npar2=-npar2                                                 
           call iswap(npar1,npar2)                                      
         end if                                                         
         if(mpar1.gt.n1old.and.mpar1.le.n1old+n2old) then               
           if(npar1.gt.n1old.and.npar1.le.n1old+n2old) then             
             jdir=jdir+1                                                
           else if(npar1.le.n1old) then                                 
             jdir=jdir-1                                                
           end if                                                       
           if(npar2.gt.n1old.and.npar2.le.n1old+n2old) then             
             jdir=jdir-1                                                
           else if(npar2.le.n1old) then                                 
             jdir=jdir+1                                                
           end if                                                       
         else if(mpar1.le.n1old) then                                   
           if(npar1.gt.n1old.and.npar1.le.n1old+n2old) then             
             jdir=jdir-1                                                
           else if(npar1.le.n1old) then                                 
             jdir=jdir+1                                                
           end if                                                       
           if(npar2.gt.n1old.and.npar2.le.n1old+n2old) then             
             jdir=jdir+1                                                
           else if(npar2.le.n1old) then                                 
             jdir=jdir-1                                                
           end if                                                       
         end if                                                         
         if(mpar2.gt.n1old.and.mpar2.le.n1old+n2old) then               
           if(npar2.gt.n1old.and.npar2.le.n1old+n2old) then             
             jdir=jdir+1                                                
           else if(npar2.le.n1old) then                                 
             jdir=jdir-1                                                
           end if                                                       
           if(npar1.gt.n1old.and.npar1.le.n1old+n2old) then             
             jdir=jdir-1                                                
           else if(npar1.le.n1old) then                                 
             jdir=jdir+1                                                
           end if                                                       
         else if(mpar2.le.n1old) then                                   
           if(npar2.gt.n1old.and.npar2.le.n1old+n2old) then             
             jdir=jdir-1                                                
           else if(npar2.le.n1old) then                                 
             jdir=jdir+1                                                
           end if                                                       
           if(npar1.gt.n1old.and.npar1.le.n1old+n2old) then             
             jdir=jdir+1                                                
           else if(npar1.le.n1old) then                                 
             jdir=jdir-1                                                
           end if                                                       
         end if                                                         
   14   continue                                                        
        if(jdir.lt.0) then                                              
          kparnt(1,iro)=-kparnt(1,iro)                                  
          kparnt(2,iro)=-kparnt(2,iro)                                  
          iflv(1,iro)=-iflv(1,iro)                                      
        end if                                                          
   15  continue                                                         
       nlund=nsave+1                                                    
       mst(1)=nlund                                                     
       mst(2)=nlund                                                     
       do 160 iro=1,iroin                                               
       do 160 jt=1,2                                                    
  160  iflstr(jt,iro)=0                                                 
       do 16 iro=1,iroin                                                
        do 19 igo=1,2                                                   
         if(kparnt(3,iro).eq.-1) go to 16                               
         ifrgn1=0                                                       
         ifrgn2=0                                                       
         mpar1=kparnt(1,iro)                                            
         mpar2=kparnt(2,iro)                                            
         if(mpar1.lt.0.or.mpar2.lt.0) then                              
           mpar1=-mpar1                                                 
           mpar2=-mpar2                                                 
           call iswap(mpar1,mpar2)                                      
         end if                                                         
         pyes1=0.0                                                      
         pyes2=0.0                                                      
         pno1=0.0                                                       
         pno2=0.0                                                       
         nyes1=0                                                        
         nyes2=0                                                        
         nno1=0                                                         
         nno2=0                                                         
         kdabei=0                                                       
         do 17 iroeff=1,iroin                                           
          if(iro.eq.iroeff) go to 17                                    
          if(kparnt(3,iroeff).eq.-1) go to 17                           
          npar1=kparnt(1,iroeff)                                        
          npar2=kparnt(2,iroeff)                                        
          if(npar1.lt.0.or.npar2.lt.0) then                             
            npar1=-npar1                                                
            npar2=-npar2                                                
            call iswap(npar1,npar2)                                     
          end if                                                        
          kforb=0                                                       
          if(mpar1.ne.npar1.and.mpar1.ne.npar2.and.                     
     a            mpar2.ne.npar1.and.mpar2.ne.npar2) then               
            kdabei=1                                                    
            pno1=pno1+plcone(1,iroeff)                                  
            pno2=pno2+plcone(2,iroeff)                                  
          else if(mpar1+mpar2.eq.npar1+npar2) then                      
            kforb=1                                                     
            nyes1=nyes1+1                                               
            nyes2=nyes2+1                                               
            pyes1=pyes1+plcone(1,iroeff)                                
            pyes2=pyes2+plcone(2,iroeff)                                
          else                                                          
            kdabei=1                                                    
            if(mpar1.eq.npar1) then                                     
              pyes1=pyes1+plcone(1,iroeff)                              
            else                                                        
              pno1=pno1+plcone(1,iroeff)                                
            end if                                                      
            if(mpar2.eq.npar2) then                                     
              pyes2=pyes2+plcone(2,iroeff)                              
            else                                                        
              pno2=pno2+plcone(2,iroeff)                                
            end if                                                      
          end if                                                        
          if(kforb.eq.0) then                                           
            if(iflstr(1,iroeff).eq.0) then                              
              nno1=nno1+1                                               
              if(igo.eq.2.and.nno1.eq.nfrgn1) ifrgn1=iroeff             
            else                                                        
              nyes1=nyes1+1                                             
            end if                                                      
            if(iflstr(2,iroeff).eq.0) then                              
              nno2=nno2+1                                               
              if(igo.eq.2.and.nno2.eq.nfrgn2) ifrgn2=iroeff             
            else                                                        
              nyes2=nyes2+1                                             
            end if                                                      
          end if                                                        
   17    continue                                                       
         psum1=pyes1+pno1                                               
         psum2=pyes2+pno2                                               
         if(igo.eq.1) then                                              
           if(kdabei.eq.0) then                                         
             kparnt(3,iro)=-1                                           
           else                                                         
             nfrgn1=min(int(ranf(0)*nno1)+1,nno1)                       
             nfrgn2=min(int(ranf(0)*nno2)+1,nno2)                       
           end if                                                       
         else if(psum1.eq.cvalu0.or.psum2.eq.cvalu0) then               
           kparnt(3,iro)=-1                                             
         else                                                           
           if(ranf(0).gt.pno1/psum1) then                               
             jcut1=1                                                    
           else                                                         
             jcut1=0                                                    
           end if                                                       
           if(ranf(0).gt.pno2/psum2) then                               
             jcut2=1                                                    
           else                                                         
             jcut2=0                                                    
           end if                                                       
           if(jcut1+jcut2.eq.0) go to 16                                
           if(ifrgn1.gt.iroin.or.ifrgn2.gt.iroin)                       
     a     call errex('strist:ifrgn problem')                           
           if(ifrgn1.eq.0) then                                         
             kparnt(3,iro)=-1                                           
             go to 16                                                   
           else                                                         
             plcmx1=plcone(1,ifrgn1)                                    
           end if                                                       
           if(ifrgn2.eq.0) then                                         
             kparnt(3,iro)=-1                                           
             go to 16                                                   
           else                                                         
             plcmx2=plcone(2,ifrgn2)                                    
           end if                                                       
           lb=kstar(iro)                                                
           le=kende(iro)                                                
   49      continue                                                     
           if(jcut1.eq.1) then                                          
             if(plcone(1,iro).le.plcmx1) jcut1=0                        
           end if                                                       
           if(jcut2.eq.1) then                                          
             if(plcone(2,iro).le.plcmx2) jcut2=0                        
           end if                                                       
           if(jcut1+jcut2.eq.0) then                                    
             go to 48                                                   
           else if(jcut1*jcut2.eq.1) then                               
             if(ranf(0).gt.0.5) then                                    
               jt=1                                                     
             else                                                       
               jt=2                                                     
             end if                                                     
           else if(jcut1.eq.1) then                                     
             jt=1                                                       
           else                                                         
             jt=2                                                       
           end if                                                       
           if(jt.eq.1) then                                             
             iflstr(1,ifrgn1)=1                                         
           else                                                         
             iflstr(2,ifrgn2)=1                                         
           end if                                                       
           if((kparnt(1,iro).gt.0.and.jt.eq.1).or.                      
     a        (kparnt(2,iro).lt.0.and.jt.eq.2)) then                    
             lact=lb                                                    
           else                                                         
             lact=le                                                    
           end if                                                       
           if(lb.gt.le) then                                            
             kparnt(3,iro)=-1                                           
             go to 16                                                   
           end if                                                       
           do 69 i=1,4                                                  
   69      plund(nlund,i)=p(mod(i,4),lact)                              
           call lurobo(0.,0.,bex,bey,bez)                               
           call lurobo(0.,philu,0.,0.,0.)                               
           call lurobo(thetlu,0.,0.,0.,0.)                              
           ppnlu=plund(nlund,4)+plund(nlund,3)                          
           pmnlu=plund(nlund,4)-plund(nlund,3)                          
           if(plcone(1,iro)-ppnlu.le.plcmx1) jcut1=0                    
           if(plcone(2,iro)-pmnlu.le.plcmx2) jcut2=0                    
           if(jcut1+jcut2.eq.0)  go to 48                               
           plcone(1,iro)=plcone(1,iro)-ppnlu                            
           plcone(2,iro)=plcone(2,iro)-pmnlu                            
           ptstor(1,iro)=ptstor(1,iro)-plund(nlund,1)                   
           ptstor(2,iro)=ptstor(2,iro)-plund(nlund,2)                   
           if(lact.eq.lb) then                                          
             lb=lb+1                                                    
           else                                                         
             le=le-1                                                    
           end if                                                       
           go to 49                                                     
   48      continue                                                     
           kb=kstar(iro)                                                
           ke=kende(iro)                                                
           if(lb.ne.kb) then                                            
             if(kparnt(3,iro).eq.40) kparnt(3,iro)=0                    
             iroout=iroout+1                                            
             if(iroout.gt.mxs) call errex('strist48-abort')             
             kstar(iroout)=kb                                           
             kende(iroout)=lb-1                                         
             manip(iroout)=.false.                                      
           end if                                                       
           if(le.ne.ke) then                                            
             if(kparnt(3,iro).eq.40) kparnt(3,iro)=0                    
             iroout=iroout+1                                            
             if(iroout.gt.mxs) call errex('strist48-abort')             
             kstar(iroout)=le+1                                         
             kende(iroout)=ke                                           
             manip(iroout)=.false.                                      
           end if                                                       
           kstar(iro)=lb                                                
           kende(iro)=le                                                
         end if                                                         
   19   continue                                                        
   16  continue                                                         
       nlund=nsave                                                      
       if(kparnt(3,1).eq.-1) then                                       
         write(8,*) 'early return'                                      
         iroout=iroin                                                   
         return                                                         
       end if                                                           
       do 57 iro=1,iroin                                                
        if(kparnt(3,iro).eq.-1) go to 57                                
        ksta=kstar(iro)                                                 
        ke=kende(iro)                                                   
        ibend=0                                                         
        istend=0                                                        
        iqend=0                                                         
        ksta=kstar(iro)                                                 
        ke=kende(iro)                                                   
        nflav=1                                                         
        ik=ksta                                                         
   58   continue                                                        
        iqtun=nclcnt(issn(ik))                                          
        if(iqtun.gt.100) iqtun=-mod(iqtun,100)                          
        call transl(-1,ityp(ik),npidec(ik),kf1)                         
        istat=0                                                         
        if(nflav.eq.1) then                                             
           call fndq(kf1,ifle1,iqtun,istat)                             
           kqr=ifle1                                                    
        else                                                            
           ifle1=iqtun                                                  
           kql=ifle1                                                    
        end if                                                          
        if(istat.eq.-1) then                                            
          go to 50                                                      
        else                                                            
          iqend=iqend+lchar(ifle1)                                      
          ifle1a=mod(iabs(ifle1),10)                                    
          ifle1b=iabs(ifle1)/10                                         
          if(ifle1a.eq.3.or.ifle1b.eq.3) then                           
            if(ifle1a.eq.3)                                             
     a      istend=istend+isign(1,ifle1)                                
            if(ifle1b.eq.3)                                             
     a      istend=istend+isign(1,ifle1)                                
          end if                                                        
          if(iabs(ifle1).gt.10) then                                    
            ibend=ibend+isign(2,ifle1)                                  
          else                                                          
            ibend=ibend+isign(1,ifle1)                                  
          end if                                                        
        end if                                                          
        if(nflav.eq.1) then                                             
          ik=ke                                                         
          nflav=2                                                       
          go to 58                                                      
        end if                                                          
        ichrgs=0                                                        
        ibarys=0                                                        
        istras=0                                                        
        do 61 l=ksta,ke                                                 
         call transl(-1,ityp(l),npidec(l),k2l)                          
         call luiflv(k2l,ifla1,iflb1,iflc1,ksp)                         
         if(iabs(ifla1).eq.3) istras=istras+isign(1,ifla1)              
         if(iabs(iflb1).eq.3) istras=istras+isign(1,iflb1)              
         if(iabs(iflc1).eq.3) istras=istras+isign(1,iflc1)              
         ichrgs=ichrgs+luchge(k2l)                                      
         k2labs=iabs(k2l)                                               
         if(k2labs.le.90) then                                          
            if(kspin(k2labs).ge.2) then                                 
              ibarys=ibarys+3*isign(1,k2l)                              
            endif                                                       
         else                                                           
            call errex('strist:unidentified hadron')                    
         endif                                                          
   61   continue                                                        
        if(istras.ne.istend.or.                                         
     a        ichrgs.ne.iqend.or.                                       
     b            ibarys.ne.ibend) then                                 
          write(8,*) 'kqr/kql=',kqr,kql                                 
          write(8,*) 'charge:',ichrgs,iqend                             
          write(8,*) 'baryno:',ibarys,ibend                             
          write(8,*) 'strang:',istras,istend                            
          write(8,*) 'strist:flavor does not add up correctly'          
          go to 50                                                      
        end if                                                          
        kbaryo=kbaryo+ibend                                             
        kstran=kstran+istend                                            
        kcharg=kcharg+iqend                                             
        nmult=nmult+1                                                   
        manip(iro)=.true.                                               
        iturn=iflv(1,iro)                                               
        if(iturn.eq.-1) then                                            
         jflv(2,iro)=kqr                                                
         jflv(1,iro)=kql                                                
        else                                                            
         jflv(1,iro)=kqr                                                
         jflv(2,iro)=kql                                                
        end if                                                          
        korsto(iro)=kparnt(3,iro)                                       
        do 102 i=0,3                                                    
  102   xbang(i)=xbang(i)+xori(i,iro)                                   
   50   continue                                                        
        if(.not.manip(iro)) then                                        
          kstar(iro)=lstar(iro)                                         
          kende(iro)=lende(iro)                                         
          jflv(1,iro)=0                                                 
          jflv(2,iro)=0                                                 
          do 56 iroa=iroin+1,iroout                                     
           if(kstar(iroa).ge.kstar(iro).and.                            
     a        kende(iroa).le.kende(iro)) then                           
             kstar(iroa)=0                                              
             kende(iroa)=0                                              
           end if                                                       
   56     continue                                                      
        end if                                                          
   57  continue                                                         
       nlund=nsave                                                      
       if(nmult.le.1.and.(nmult.eq.0.or.nstrin.eq.0)) then              
         do 191 iro=2,iroin                                             
  191    manip(iro)=.false.                                             
         iroout=iroin                                                   
         return                                                         
       end if                                                           
       do 106 i=0,3                                                     
  106  xbang(i)=xbang(i)/nmult                                          
  444  format(3x,a18,1x,14(i3,1x))                                      
       do 68 iro=1,iroout                                               
        if(manip(iro)) go to 68                                         
        ksta=kstar(iro)                                                 
        if(ksta.eq.0) go to 68                                          
        ke=kende(iro)                                                   
        do 67 ik=ksta,ke                                                
        do 67 i=0,3                                                     
   67   psum(i)=psum(i)-p(i,ik)                                         
   68  continue                                                         
       ppltot=0.                                                        
       pmitot=0.                                                        
       do 21 i=1,2                                                      
   21  ptsum(i)=0.0                                                     
       if(kbpmd2.lt.iroin) then                                         
         call errex('strist70:kbpmd2.lt.iroin')                         
       end if                                                           
       ransm1=0.0                                                       
       ransm2=0.0                                                       
       do 70 iro=1,iroin                                                
        xhad(1,iro)=0.0                                                 
        xhad(2,iro)=0.0                                                 
        if(.not.manip(iro))go to 70                                     
        xhad(1,iro)=ranf(0)                                             
        xhad(2,iro)=ranf(0)                                             
        ransm1=ransm1+xhad(1,iro)                                       
        ransm2=ransm2+xhad(2,iro)                                       
        ppltot=ppltot+plcone(1,iro)                                     
        pmitot=pmitot+plcone(2,iro)                                     
        do 62 i=1,2                                                     
   62   ptsum(i)=ptsum(i)+ptstor(i,iro)                                 
   70  continue                                                         
       do 270 iro=1,iroin                                               
        if(.not.manip(iro))go to 270                                    
        ptstor(1,iro)=xhad(1,iro)/ransm1*ptsum(1)                       
        ptstor(2,iro)=xhad(2,iro)/ransm2*ptsum(2)                       
  270  continue                                                         
       nlund=nlund+1                                                    
       mst(1)=nlund                                                     
       mst(2)=mst(1)                                                    
       do 71 i=1,2                                                      
   71  plund(nlund,i)=ptsum(i)                                          
       ps(1)=ppltot                                                     
       ps(2)=pmitot                                                     
       emtrue=sqrt(ps(1)*ps(2))                                         
       plund(nlund,4)=0.5*(ppltot+pmitot)                               
       plund(nlund,3)=0.5*(ppltot-pmitot)                               
       bex=-bex                                                         
       bey=-bey                                                         
       bez=-bez                                                         
       thetlu=-thetlu                                                   
       philu=-philu                                                     
       call lurobo(thetlu,philu,bex,bey,bez)                            
       ptot(0)=plund(nlund,4)                                           
       do 72 i=1,3                                                      
   72  ptot(i)=plund(nlund,i)                                           
       nlund=nlund-1                                                    
       if((abs(ptot(0)-psum(0)).gt.0.015).or.                           
     a        (abs(ptot(1)-psum(1)).gt.0.015).or.                       
     b            (abs(ptot(2)-psum(2)).gt.0.015).or.                   
     c               (abs(ptot(3)-psum(3)).gt.0.015)) then              
        write(8,166) '   ',iroin,'psum=',psum                           
        write(8,166) '   ',iroin,'ptot=',ptot                           
  166   format(1x,a6,i4,1x,a6,1x,4(f8.4,1x))                            
        call errex('strist:72 no energy/mom. conservation')             
       end if                                                           
       nbsstr=0                                                         
       nbqstr=0                                                         
       nhstri=0                                                         
       kadel=0                                                          
       do 171 iro=1,iroin                                               
        if(.not.manip(iro)) go to 171                                   
        nhstri=nhstri+1+kende(iro)-kstar(iro)                           
        do 179 il=kstar(iro),kende(iro)                                 
         iqbr=nclcnt(issn(il))                                          
         if(iqbr.gt.100) iqbr=-(iqbr-100)                               
         if(il.eq.kende(iro)) then                                      
         else if(iabs(iqbr).le.2) then                                  
           nbqstr=nbqstr+1                                              
         else if(iabs(iqbr).eq.3) then                                  
           nbsstr=nbsstr+1                                              
         end if                                                         
         call transl(-1,ityp(il),npidec(il),kf)                         
         if(iabs(kf).eq.18.or.iabs(kf).eq.28.or.                        
     a        iabs(kf).eq.19.or.iabs(kf).eq.29) then                    
           kadel=kadel+1                                                
         end if                                                         
  179   continue                                                        
  171  continue                                                         
       ndif1=iroin-nmult                                                
       nmulol=nmult                                                     
       mloop=0                                                          
       mlinit=1                                                         
  100  continue                                                         
       if(mloop.gt.30) then                                             
         write(8,*) 'strist call no success:mloop.gt.30,return'         
         do 192 iro=2,iroin                                             
  192    manip(iro)=.false.                                             
         iroout=iroin                                                   
         return                                                         
       end if                                                           
       do 173 iro=1,iroin                                               
        mqflav(1,iro)=1                                                 
        mqflav(2,iro)=1                                                 
        if(.not.manip(iro)) go to 173                                   
        do 172 i=1,2                                                    
         pt(i,iro)=ptstor(i,iro)                                        
         plc(i,iro)=plcone(i,iro)                                       
         iflv(i,iro)=jflv(i,iro)                                        
         mqflav(i,iro)=0                                                
  172   continue                                                        
        if(korsto(iro).eq.40) then                                      
          if(iabs(iflv(1,iro)).lt.10) then                              
            iflqew=isign(1,iflv(1,iro))*(int(ranf(0)*(2.+par2))+1)      
            iflv(1,iro)=iflqew                                          
            iflv(2,iro)=-iflqew                                         
          end if                                                        
        else                                                            
          do  63 iroa=1,iro-1                                           
           if(.not.manip(iroa)) go to 63                                
           if(korsto(iroa).eq.40) go to 63                              
           if(iabs(iflv(1,iroa)).gt.10.or.                              
     a             iabs(iflv(2,iroa)).gt.10) go to 63                   
           if(iflv(1,iro)+iflv(1,iroa).eq.0) then                       
             iflqew=isign(1,iflv(1,iro))*(int(ranf(0)*(2.+par2))+1)     
             iflv(1,iro)=iflqew                                         
             iflv(1,iroa)=-iflqew                                       
           end if                                                       
           if(iflv(2,iro)+iflv(2,iroa).eq.0) then                       
             iflqew=isign(1,iflv(2,iro))*(int(ranf(0)*(2.+par2))+1)     
             iflv(2,iro)=iflqew                                         
             iflv(2,iroa)=-iflqew                                       
           end if                                                       
           if(iflv(2,iro)+iflv(1,iroa).eq.0) then                       
             iflqew=isign(1,iflv(2,iro))*(int(ranf(0)*(2.+par2))+1)     
             iflv(2,iro)=iflqew                                         
             iflv(1,iroa)=-iflqew                                       
           end if                                                       
           if(iflv(1,iro)+iflv(2,iroa).eq.0) then                       
             iflqew=isign(1,iflv(1,iro))*(int(ranf(0)*(2.+par2))+1)     
             iflv(1,iro)=iflqew                                         
             iflv(2,iroa)=-iflqew                                       
           end if                                                       
   63     continue                                                      
        end if                                                          
  173  continue                                                         
       izr=0                                                            
       qqspin=1.0                                                       
       nsingl=0                                                         
       psingl(1)=0.0                                                    
       psingl(2)=0.0                                                    
       mrop=2*iroin                                                     
       par(11)=para11                                                   
       ip=0                                                             
       iq=0                                                             
       do 73 iro=1,iroin                                                
        if(.not.manip(iro)) go to 73                                    
        jqpart(iro)=0                                                   
        jetzhd(1,iro)=0                                                 
        jetzhd(2,iro)=0                                                 
        iqqkf(1,iro)=0                                                  
        iqqkf(2,iro)=0                                                  
        iflq=iflv(1,iro)                                                
        iflr=iflv(2,iro)                                                
        if((iflq.gt.0.and.iflq.lt.10).or.                               
     a                         (iflq.lt.-10)) then                      
          if(iflq.lt.-10) iqqkf(1,iro)=-1                               
          if(iflr.gt.10) iqqkf(2,iro)=-1                                
          ppl1q=dimsu3(ip+1,iq)                                         
          if(ip.gt.0.and.iflq.gt.0.and.-iflr.gt.0) then                 
            pm1qp1=dimsu3(ip-1,iq+1)                                    
          else                                                          
            pm1qp1=0.0                                                  
          end if                                                        
          if(iq.gt.0) then                                              
            pqmi1=dimsu3(ip,iq-1)                                       
          else                                                          
            pqmi1=0.0                                                   
          end if                                                        
          izr=izr+1                                                     
          if(mlinit.eq.1)  dquark(1,izr)=ranf(0)                        
          pmlsum=pm1qp1+ppl1q+pqmi1                                     
          if(dquark(1,izr).lt.pm1qp1/pmlsum) then                       
            iof=int(ranf(0)*iro)+1                                      
            iyes=0                                                      
            do 74 iroant=iof+1,iof+iro-1                                
             iroa=mod(iroant,iro-1)+1                                   
             if(.not.manip(iroa)) go to 74                              
             ifla=iflv(1,iroa)                                          
             iflb=iflv(2,iroa)                                          
             if(ifla.gt.0.and.ifla.lt.10) then                          
              if(-iflb.gt.0.and.-iflb.lt.10) then                       
                  iyes=1                                                
                  goto 75                                               
              end if                                                    
             end if                                                     
   74       continue                                                    
   75       continue                                                    
            if(iyes.eq.1) then                                          
              ip=ip-1                                                   
              iq=iq+1                                                   
              jqpart(iroa)=iro                                          
              do 115 j=1,2                                              
               if(iro.eq.0) then                                        
                 if(plc(j,iroa).gt.plc(j,iro)) then                     
                   iqqkf(j,iroa)=iroa                                   
                 else                                                   
                   iqqkf(j,iroa)=iro                                    
                 end if                                                 
               end if                                                   
               plc(j,iroa)=plc(j,iroa)+plc(j,iro)                       
               pt(j,iroa)=pt(j,iroa)+pt(j,iro)                          
               pt(j,iro)=0.0                                            
               plc(j,iro)=0.0                                           
               iflv(j,iro)=0                                            
               mqflav(j,iro)=1                                          
               mqflav(j,iroa)=1                                         
               if(j.eq.1) then                                          
                 ifl1=ifla                                              
                 ifl2=iflq                                              
               else                                                     
                 ifl1=iflb                                              
                 ifl2=iflr                                              
               end if                                                   
               if(iqqkf(j,iroa).eq.iroa) then                           
                 iflv(j,iroa)=10*ifl1+ifl2                              
               else if(iqqkf(j,iroa).eq.iro) then                       
                 iflv(j,iroa)=10*ifl2+ifl1                              
               else                                                     
                 iflv(j,iroa)=kqqret(ifl1,ifl2)                         
                 if(ifl1.ne.ifl2) qqspin=qqspin*qqdqqs                  
               end if                                                   
  115         continue                                                  
            else                                                        
              ip=ip+1                                                   
            end if                                                      
          else if(dquark(1,izr).lt.(pqmi1+pm1qp1)/pmlsum) then          
            iof=int(ranf(0)*iro)+1                                      
            iyes=0                                                      
            do 174 iroant=iof+1,iof+iro-1                               
             iroa=mod(iroant,iro-1)+1                                   
             if(.not.manip(iroa)) go to 174                             
             if(plc(1,iroa).eq.0.0) go to 174                           
             ifla=iflv(1,iroa)                                          
             iflb=iflv(2,iroa)                                          
             if(itrip(ifla)*itrip(iflq).eq.10.and.                      
     a                              iabs(ifla*iflq).lt.100) then        
               if(itrip(iflb)*itrip(iflr).eq.10.and.                    
     a                              iabs(iflb*iflr).lt.100) then        
                 iyes=1                                                 
                 goto 175                                               
               end if                                                   
             end if                                                     
  174       continue                                                    
  175       continue                                                    
            if(iyes.eq.1) then                                          
              if(max(iro,iroa).ge.1000)                                 
     a        call errex('strist:isingl array overflow')                
              isingl(nsingl+1)=iro+jqpart(iro)*1000                     
              isingl(nsingl+2)=iroa+jqpart(iroa)*1000                   
              do 65 j=1,2                                               
               iflst1=iflv(j,iro)                                       
               iflst2=iflv(j,iroa)                                      
               iroact=0                                                 
               if(iabs(iflst1).gt.10) then                              
                 iroact=iro                                             
               else if(iabs(iflst2).gt.10) then                         
                 iroact=iroa                                            
               end if                                                   
               iswa=0                                                   
               if(iroact.gt.0) then                                     
                 if(iabs(iflst1).lt.10) then                            
                   call iswap(iflst1,iflst2)                            
                   iswa=1                                               
                 end if                                                 
                 if(iqqkf(j,iro).gt.0.or.iqqkf(j,iroa).gt.0) then       
                   ifls11=iflst1/10                                     
                   if(ifls11+10*ifls11.ne.iflst1) then                  
                     qqspin=qqspin*qqdqqs                               
                   end if                                               
                 end if                                                 
                 iflcom=0                                               
                 ichar=0                                                
   24            continue                                               
                 if(iflcom.ne.0) then                                   
                   jro=0                                                
                   mof=int(ranf(0)*(mrop+1))+1                          
                   do 324 ma=mof+1,mof+mrop                             
                    mb=mod(ma,mrop)+1                                   
                    if(mb.gt.iroin) then                                
                      mb=mb-iroin                                       
                      lt=2                                              
                    else                                                
                      lt=1                                              
                    end if                                              
                    if(mqflav(lt,mb).ne.0) go to 324                    
                    if(mb.eq.iro.or.mb.eq.iroa) go to 324               
                    if(iflcom.eq.-iflv(lt,mb)) then                     
                      jro=mb                                            
                      mt=lt                                             
                      ifln=isign(1,iflcom)*(int(ranf(0)*(2.+par2))+1)   
                      go to 325                                         
                    end if                                              
  324              continue                                             
  325              continue                                             
                 end if                                                 
                 if(ichar.eq.1.and.jro.gt.0) then                       
                   iflm=ifln                                            
                 else                                                   
                   iflm=iflst2                                          
                 end if                                                 
                 barw=barwgh(para11,iflst1,iflm)                        
                 if(mlinit.eq.0) then                                   
                   if(ranf(0).gt.barw) then                             
                     iflcom=iflst2                                      
                     ichar=1                                            
                     go to 24                                           
                   end if                                               
                 else                                                   
                   qqspin=qqspin*barw                                   
                 end if                                                 
                 call luifld(iflst1,0,iflm,ifldum,kf)                   
                 jetzhd(j,iro)=kf                                       
                 if(ichar.eq.1.and.jro.gt.0) then                       
                   iflv(mt,jro)=-ifln                                   
                   iflst2=ifln                                          
                 end if                                                 
               end if                                                   
               mqflav(j,iro)=1                                          
               mqflav(j,iroa)=1                                         
               if(iswa.eq.1) then                                       
                 call iswap(iflst1,iflst2)                              
               end if                                                   
               iflstr(j,nsingl+1)=iflst1                                
               iflstr(j,nsingl+2)=iflst2                                
               psingl(j)=psingl(j)+plc(j,iro)+plc(j,iroa)               
               plc(j,iro)=0.0                                           
               pt(j,iro)=0.0                                            
               iflv(j,iro)=0                                            
               plc(j,iroa)=0.0                                          
               pt(j,iroa)=0.0                                           
               iflv(j,iroa)=0                                           
   65         continue                                                  
              nsingl=nsingl+2                                           
              iq=iq-1                                                   
            else                                                        
              ip=ip+1                                                   
            end if                                                      
          else                                                          
              ip=ip+1                                                   
          end if                                                        
        else                                                            
          if(iflq.gt.10) iqqkf(1,iro)=-1                                
          if(iflr.lt.-10) iqqkf(2,iro)=-1                               
          qpl1p=dimsu3(ip,iq+1)                                         
          if(iq.gt.0.and.-iflq.gt.0.and.iflr.gt.0) then                 
            qm1pp1=dimsu3(ip+1,iq-1)                                    
          else                                                          
            qm1pp1=0.0                                                  
          end if                                                        
                                                                        
          if(ip.gt.0) then                                              
            pmi1q=dimsu3(ip-1,iq)                                       
          else                                                          
            pmi1q=0.0                                                   
          end if                                                        
          izr=izr+1                                                     
          if(mlinit.eq.1)  dquark(1,izr)=ranf(0)                        
          pmlsum=qm1pp1+qpl1p+pmi1q                                     
          if(dquark(1,izr).lt.qm1pp1/pmlsum) then                       
            iof=int(ranf(0)*iro)+1                                      
            iyes=0                                                      
            do 76 iroant=iof+1,iof+iro-1                                
             iroa=mod(iroant,iro-1)+1                                   
             if(.not.manip(iroa)) go to 76                              
             ifla=iflv(1,iroa)                                          
             iflb=iflv(2,iroa)                                          
             if(-ifla.gt.0.and.-ifla.lt.10) then                        
              if(iflb.gt.0.and.iflb.lt.10) then                         
                  iyes=1                                                
                  goto 77                                               
              end if                                                    
             end if                                                     
   76       continue                                                    
   77       continue                                                    
            if(iyes.eq.1) then                                          
              iq=iq-1                                                   
              ip=ip+1                                                   
              jqpart(iroa)=iro                                          
              do 116 j=1,2                                              
               if(iro.eq.0) then                                        
                 if(plc(j,iroa).gt.plc(j,iro)) then                     
                   iqqkf(j,iroa)=iroa                                   
                 else                                                   
                   iqqkf(j,iroa)=iro                                    
                 end if                                                 
               end if                                                   
               plc(j,iroa)=plc(j,iroa)+plc(j,iro)                       
               pt(j,iroa)=pt(j,iroa)+pt(j,iro)                          
               pt(j,iro)=0.0                                            
               plc(j,iro)=0.0                                           
               iflv(j,iro)=0                                            
               mqflav(j,iro)=1                                          
               mqflav(j,iroa)=1                                         
               if(j.eq.1) then                                          
                 ifl1=ifla                                              
                 ifl2=iflq                                              
               else                                                     
                 ifl1=iflb                                              
                 ifl2=iflr                                              
               end if                                                   
               if(iqqkf(j,iroa).eq.iroa) then                           
                 iflv(j,iroa)=10*ifl1+ifl2                              
               else if(iqqkf(j,iroa).eq.iro) then                       
                 iflv(j,iroa)=10*ifl2+ifl1                              
               else                                                     
                 iflv(j,iroa)=kqqret(ifl1,ifl2)                         
                 if(ifl1.ne.ifl2) qqspin=qqspin*qqdqqs                  
               end if                                                   
  116         continue                                                  
            else                                                        
              iq=iq+1                                                   
            end if                                                      
          else if(dquark(1,izr).lt.(pmi1q+qm1pp1)/pmlsum) then          
            iof=int(ranf(0)*iro)+1                                      
            iyes=0                                                      
            do 176 iroant=iof+1,iof+iro-1                               
             iroa=mod(iroant,iro-1)+1                                   
             if(.not.manip(iroa)) go to 176                             
             if(plc(1,iroa).eq.0.0) go to 176                           
             ifla=iflv(1,iroa)                                          
             iflb=iflv(2,iroa)                                          
             if(itrip(ifla)*itrip(iflq).eq.10.and.                      
     a                              iabs(ifla*iflq).lt.100) then        
               if(itrip(iflb)*itrip(iflr).eq.10.and.                    
     a                              iabs(iflb*iflr).lt.100) then        
                 iyes=1                                                 
                 goto 177                                               
               end if                                                   
             end if                                                     
  176       continue                                                    
  177       continue                                                    
            if(iyes.eq.1) then                                          
              if(max(iro,iroa).ge.1000)                                 
     a        call errex('strist:isingl array overflow')                
              isingl(nsingl+1)=iro+jqpart(iro)*1000                     
              isingl(nsingl+2)=iroa+jqpart(iroa)*1000                   
              do 66 j=1,2                                               
               iflst1=iflv(j,iro)                                       
               iflst2=iflv(j,iroa)                                      
               iroact=0                                                 
               if(iabs(iflst1).gt.10) then                              
                 iroact=iro                                             
               else if(iabs(iflst2).gt.10) then                         
                 iroact=iroa                                            
               end if                                                   
               iswa=0                                                   
               if(iroact.gt.0) then                                     
                 if(iabs(iflst1).lt.10) then                            
                   call iswap(iflst1,iflst2)                            
                   iswa=1                                               
                 end if                                                 
                 if(iqqkf(j,iro).gt.0.or.iqqkf(j,iroa).gt.0) then       
                   ifls11=iflst1/10                                     
                   if(ifls11+10*ifls11.ne.iflst1) then                  
                     qqspin=qqspin*qqdqqs                               
                   end if                                               
                 end if                                                 
                 iflcom=0                                               
                 ichar=0                                                
   25            continue                                               
                 if(iflcom.ne.0) then                                   
                   jro=0                                                
                   mof=int(ranf(0)*(mrop+1))+1                          
                   do 326 ma=mof+1,mof+mrop                             
                    mb=mod(ma,mrop)+1                                   
                    if(mb.gt.iroin) then                                
                      mb=mb-iroin                                       
                      lt=2                                              
                    else                                                
                      lt=1                                              
                    end if                                              
                    if(mqflav(lt,mb).ne.0) go to 326                    
                    if(mb.eq.iro.or.mb.eq.iroa) go to 326               
                    if(iflcom.eq.-iflv(lt,mb)) then                     
                      jro=mb                                            
                      mt=lt                                             
                      ifln=isign(1,iflcom)*(int(ranf(0)*(2.+par2))+1)   
                      go to 327                                         
                    end if                                              
  326              continue                                             
  327              continue                                             
                 end if                                                 
                 if(ichar.eq.1.and.jro.gt.0) then                       
                   iflm=ifln                                            
                 else                                                   
                   iflm=iflst2                                          
                 end if                                                 
                 barw=barwgh(para11,iflst1,iflm)                        
                 if(mlinit.eq.0) then                                   
                   if(ranf(0).gt.barw) then                             
                     iflcom=iflst2                                      
                     ichar=1                                            
                     go to 25                                           
                   end if                                               
                 else                                                   
                   qqspin=qqspin*barw                                   
                 end if                                                 
                 call luifld(iflst1,0,iflm,ifldum,kf)                   
                 jetzhd(j,iro)=kf                                       
                 if(ichar.eq.1.and.jro.gt.0) then                       
                   iflv(mt,jro)=-ifln                                   
                   iflst2=ifln                                          
                 end if                                                 
               end if                                                   
               mqflav(j,iro)=1                                          
               mqflav(j,iroa)=1                                         
               if(iswa.eq.1) then                                       
                 call iswap(iflst1,iflst2)                              
               end if                                                   
               iflstr(j,nsingl+1)=iflst1                                
               iflstr(j,nsingl+2)=iflst2                                
               psingl(j)=psingl(j)+plc(j,iro)+plc(j,iroa)               
               plc(j,iro)=0.0                                           
               pt(j,iro)=0.0                                            
               iflv(j,iro)=0                                            
               plc(j,iroa)=0.0                                          
               pt(j,iroa)=0.0                                           
               iflv(j,iroa)=0                                           
   66         continue                                                  
              nsingl=nsingl+2                                           
              ip=ip-1                                                   
            else                                                        
              iq=iq+1                                                   
            end if                                                      
          else                                                          
            iq=iq+1                                                     
          end if                                                        
        end if                                                          
   73  continue                                                         
       par(11)=par11                                                    
       mlinit=0                                                         
       if(ranf(0).gt.qqspin)  go to 100                                 
       mlinit=1                                                         
       mloop=mloop+1                                                    
       if(ip.gt.iprbr1.or.iq.gt.iprbr1) then                            
         write(8,*) 'strist warning:'                                   
         write(8,*)                                                     
     a     'fieldstrength larger than (ip>iprbr1,iq.le.ip):',ip,iq      
       end if                                                           
       ndif2=nmulol-(ip+iq)                                             
       nd1=0                                                            
       nd2=0                                                            
       nm=0                                                             
       do 78 iro=1,iroin                                                
        if(.not.manip(iro)) then                                        
          nd1=nd1+1                                                     
        else if(plc(1,iro).eq.0.0) then                                 
          nd2=nd2+1                                                     
        else                                                            
          nm=nm+1                                                       
          do  87 jt=1,2                                                 
           pt(jt,nm)=pt(jt,iro)                                         
           iflv(jt,nm)=iflv(jt,iro)                                     
           plc(jt,nm)=plc(jt,iro)                                       
           mqflav(jt,nm)=mqflav(jt,iro)                                 
           iqqkf(jt,nm)=iqqkf(jt,iro)                                   
           if(iqqkf(jt,nm).gt.0) then                                   
             iroa=iqqkf(jt,nm)                                          
             if(iroa.gt.iroin)                                          
     a       call errex('strist:iqqkf value out of range')              
             plc(jt,nm)=plc(jt,nm)-plcone(jt,iroa)                      
             psingl(jt)=psingl(jt)+plcone(jt,iroa)                      
           end if                                                       
   87     continue                                                      
        end if                                                          
   78  continue                                                         
       do 101  nsi=1,nsingl-1,2                                         
        iro1=mod(isingl(nsi),1000)                                      
        do 101 jt=1,2                                                   
         iqqkf(jt,mxs+1-nsi)=jetzhd(jt,iro1)                            
  101  continue                                                         
       if(nd1.ne.ndif1) then                                            
          call errex('strist:nd1.ne.ndif1')                             
       else if(nd2.ne.ndif2) then                                       
          write(8,*) 'nd2=',nd2,'ndif2=',ndif2                          
          call errex('strist:nd2.ne.ndif2')                             
       else if(nm.ne.ip+iq) then                                        
          call errex('strist:nm.ne.ip+iq')                              
       end if                                                           
       nmult=ip+iq                                                      
       ipmax=ip                                                         
       iqmax=iq                                                         
c      write(8,*) 'nmult,ipmax,iqmax=',nmult,ip,iq                      
       do 80 nm=1,nmult                                                 
        iord(1,nm)=1                                                    
        iord(2,nm)=1                                                    
        do 79 mn=1,nm-1                                                 
         if(plc(1,nm).gt.plc(1,mn))then                                 
          iord(1,nm)=iord(1,nm)+1                                       
         else                                                           
          iord(1,mn)=iord(1,mn)+1                                       
         end if                                                         
         if(plc(2,nm).gt.plc(2,mn))then                                 
          iord(2,nm)=iord(2,nm)+1                                       
         else                                                           
          iord(2,mn)=iord(2,mn)+1                                       
         end if                                                         
   79   continue                                                        
   80  continue                                                         
       do 81 nm=1,nmult                                                 
        jord(nm)=iord(1,nm)                                             
   81  continue                                                         
       isum=0                                                           
       do 82 nm=1,nmult                                                 
        i=jord(nm)                                                      
        isum=isum+i                                                     
        if(i.lt.1.or.i.gt.nmult) then                                   
           call errex('strist:iord problem')                            
        end if                                                          
        iord(1,i)=nm                                                    
   82  continue                                                         
       if(isum.ne.nmult*(nmult+1)/2) then                               
           call errex('strist 82:iord problem')                         
       end if                                                           
       do 83 nm=1,nmult                                                 
        jord(nm)=iord(2,nm)                                             
   83  continue                                                         
       isum=0                                                           
       do 84 nm=1,nmult                                                 
        i=jord(nm)                                                      
        isum=isum+i                                                     
        if(i.lt.1.or.i.gt.nmult) then                                   
           call errex('strist:iord problem')                            
        end if                                                          
        iord(2,i)=nm                                                    
   84  continue                                                         
       if(isum.ne.nmult*(nmult+1)/2) then                               
           call errex('strist 84:iord problem')                         
       end if                                                           
       ipdgr(1,1)=ipmax                                                 
       iqdgr(1,1)=iqmax                                                 
       ipdgr(2,1)=iqmax                                                 
       iqdgr(2,1)=ipmax                                                 
       nstep=1                                                          
       do 96 nm=1,nmult                                                 
        nstep=nstep+1                                                   
        do 97 jt=1,2                                                    
         if(itrip(iflv(jt,iord(jt,nm))).eq.1) then                      
           iqdgr(jt,nstep)=iqdgr(jt,nstep-1)-1                          
           ipdgr(jt,nstep)=ipdgr(jt,nstep-1)                            
         else if(itrip(iflv(jt,iord(jt,nm))).eq.10) then                
           ipdgr(jt,nstep)=ipdgr(jt,nstep-1)-1                          
           iqdgr(jt,nstep)=iqdgr(jt,nstep-1)                            
         end if                                                         
   97   continue                                                        
   96  continue                                                         
       if(ipdgr(1,nstep).ne.0.or.ipdgr(2,nstep).ne.0.or.                
     a        iqdgr(1,nstep).ne.0.or.iqdgr(2,nstep).ne.0) then          
         write(8,*) 'ipdgr(1)=',(ipdgr(1,i),i=1,nstep)                  
         write(8,*) 'iqdgr(1)=',(iqdgr(1,i),i=1,nstep)                  
         write(8,*) 'ipdgr(2)=',(ipdgr(2,i),i=1,nstep)                  
         write(8,*) 'iqdgr(2)=',(iqdgr(2,i),i=1,nstep)                  
         call errex('strist:chromoelectric field not confined?')        
       end if                                                           
         do 95 jt=1,2                                                   
          plcprv=0.0                                                    
          do 90 nm=1,nmult                                              
   90     dquark(jt,nm)=0.0                                             
          do 92 nm=1,nmult-1                                            
           tnsave=0.75*casop(ipdgr(jt,nm),iqdgr(jt,nm))/(nmult-nm+1)    
           xlcfbp=(plc(jt,iord(jt,nm))-plcprv)/tnsave                   
           if(xlcfbp.le.0.0) then                                       
             call errex(                                                
     a           'strist91:decreasing series of quark momenta????')     
           end if                                                       
           plcprv=plc(jt,iord(jt,nm))                                   
           do 91 nn=nm,nmult                                            
            nnpls1=nn+1                                                 
            tnsaft=0.75*(casop(ipdgr(jt,nn),iqdgr(jt,nn))-              
     a                     casop(ipdgr(jt,nnpls1),iqdgr(jt,nnpls1)))    
            if(tnsaft.le.0.0) then                                      
              call errex(                                               
     a              'strist91:increasing chromoelectric field????')     
            end if                                                      
            dquark(jt,iord(jt,nn))=dquark(jt,iord(jt,nn))+              
     a                                   xlcfbp*(tnsaft-tnsave)         
   91      continue                                                     
   92     continue                                                      
          dqsum=0.0                                                     
          do 94 nm=1,nmult                                              
           plc(jt,iord(jt,nm))=plc(jt,iord(jt,nm))+                     
     a                             dquark(jt,iord(jt,nm))               
           dqsum=dqsum+dquark(jt,iord(jt,nm))                           
   94     continue                                                      
          if(abs(dqsum).gt.1.e-3) then                                  
             call errex(                                                
     a           'strist91:plc increments do not add up to 0????')      
          end if                                                        
   95    continue                                                       
      if(nmult.gt.0) then                                               
         xlcrop(1)=plc(1,iord(1,nmult))                                 
         xlcrop(2)=plc(2,iord(2,nmult))                                 
       else                                                             
         xlcrop(1)=0.0                                                  
         xlcrop(2)=0.0                                                  
       end if                                                           
       tnsmax=0.75*casop(ipmax,iqmax)                                   
       plcrop(1)=xlcrop(1)*tnsmax                                       
       plcrop(2)=xlcrop(2)*tnsmax                                       
       embang=sqrt(plcrop(1)*plcrop(2))                                 
       pspent(1)=0.0                                                    
       pspent(2)=0.0                                                    
       nstep=1                                                          
       ibpfor=0                                                         
       do 86 nm=1,nmult                                                 
        nstep=nstep+1                                                   
        call luptdi(0,ptx,pty)                                          
        do 85 jt=1,2                                                    
         ibpfor=ibpfor+1                                                
         jr=3-jt                                                        
         iquark(jt,ibpfor)=0                                            
         iquark(jr,ibpfor)=iflv(jt,iord(jt,nm))                         
         ptr(jr,1,ibpfor)=0.5*pt(1,iord(jt,nm))                         
         ptr(jr,2,ibpfor)=0.5*pt(2,iord(jt,nm))                         
         ptr(jr,1,ibpfor)=ptr(jr,1,ibpfor)+(3-2*jt)*ptx                 
         ptr(jr,2,ibpfor)=ptr(jr,2,ibpfor)+(3-2*jt)*pty                 
         ptr(jt,1,ibpfor)=0.                                            
         ptr(jt,2,ibpfor)=0.                                            
         tnsdif=0.75*(casop(ipdgr(jt,nstep-1),iqdgr(jt,nstep-1))-       
     a                     casop(ipdgr(jt,nstep),iqdgr(jt,nstep)))      
         if(tnsdif.le.0.0) then                                         
           call errex('strist:increasing chromoelectric field????')     
         end if                                                         
         xlcbp(jt,ibpfor)=plc(jt,iord(jt,nm))/tnsdif                    
         lloop=0                                                        
   89    lloop=lloop+1                                                  
         if(lloop.gt.100) then                                          
           call errex('strist89:lloop>100')                             
         end if                                                         
         if(xlcbp(jt,ibpfor).lt.xlcbp(jt,max(1,ibpfor-2))) then         
           fract=0.1*ranf(0)                                            
           do 88 ibpf=jt,ibpfor-2,2                                     
            nmold=(ibpf+1)/2                                            
            pfract=fract*plc(jt,iord(jt,nmold))                         
            tnsold=plc(jt,iord(jt,nmold))/xlcbp(jt,ibpf)                
            pspent(jt)=pspent(jt)+pfract                                
            plc(jt,iord(jt,nmold))=plc(jt,iord(jt,nmold))-pfract        
            xlcbp(jt,ibpf)=plc(jt,iord(jt,nmold))/tnsold                
            xlcmx(jt,ibpf)=xlcbp(jt,ibpf)                               
            plc(jt,iord(jt,nm))=plc(jt,iord(jt,nm))+pfract              
   88      continue                                                     
           xlcbp(jt,ibpfor)=plc(jt,iord(jt,nm))/tnsdif                  
           go to 89                                                     
         end if                                                         
         if(xlcbp(jt,ibpfor).gt.xlcrop(jt)+0.01) then                   
           write(8,*) xlcbp(jt,ibpfor),xlcrop(jt),jt,ibpfor,nstep       
           call errex('strist:wrong quark turning point')               
         end if                                                         
         xlcbp(jr,ibpfor)=0.0                                           
         xlcmx(jt,ibpfor)=xlcbp(jt,ibpfor)                              
         xlcmx(jr,ibpfor)=0.0                                           
         plcfor(jt)=(xlcrop(jt)-xlcbp(jt,ibpfor))*tnsdif                
         plcfor(jr)=0.0                                                 
         pspent(jt)=pspent(jt)+plcfor(jt)                               
         newbp(ibpfor)=.false.                                          
   85   continue                                                        
   86  continue                                                         
       if(abs(plcrop(1)+psingl(1)-pspent(1)-ps(1)).gt.0.05.or.          
     a      abs(plcrop(2)+psingl(2)-pspent(2)-ps(2)).gt.0.05) then      
         write(8,*) 'ps=',ps                                            
         write(8,*) 'pspent=',pspent                                    
         write(8,*) 'psingl=',psingl                                    
         write(8,*) 'plcrop=',plcrop                                    
         call errex('strist:wrong formal turning point momentum?')      
       end if                                                           
       ibptru=ibpfor                                                    
       atrel=atre                                                       
       if(ipmax+iqmax.gt.0) then                                        
         call bplist(ipmax,iqmax,embang,emtrue,atrel)                   
       else                                                             
         libpmx=0                                                       
       end if                                                           
       if(ibpfor+libpmx.gt.kbpmax) then                                 
         do 98 iro=1,iroin                                              
   98    manip(iro)=.false.                                             
         write(8,*) 'libpmx,ibpfor,kbpmax=',                            
     a                     libpmx,ibpfor,kbpmax                         
         call errex('strist:kbpmax-arrays have too small dimension')    
       end if                                                           
       do 99 ibpl=1,libpmx                                              
        ibptru=ibptru+1                                                 
        xlcbp(1,ibptru)=plcrop(1)/embang*bpli(1,ibpl)                   
        xlcbp(2,ibptru)=plcrop(2)/embang*bpli(2,ibpl)                   
        xlcmx(1,ibptru)=xlcbp(1,ibptru)                                 
        xlcmx(2,ibptru)=xlcbp(2,ibptru)                                 
        ptr(1,1,ibptru)=0.0                                             
        ptr(1,2,ibptru)=0.0                                             
        ptr(2,1,ibptru)=0.0                                             
        ptr(2,2,ibptru)=0.0                                             
        newbp(ibptru)=.true.                                            
        iquark(1,ibptru)=0                                              
        iquark(2,ibptru)=0                                              
   99  continue                                                         
       do 110 ibp=1,ibptru                                              
        do 108 jt=1,2                                                   
         icomb(jt,ibp)=0                                                
         ivlc(jt,ibp)=0                                                 
         ipact(jt,ibp)=0                                                
         iqact(jt,ibp)=0                                                
         lord(jt,ibp)=1                                                 
         equark(jt,ibp)=0.0                                             
         dquark(jt,ibp)=0.0                                             
  108   continue                                                        
        do 109 jbp=1,ibp-1                                              
         if(xlcbp(1,ibp).gt.xlcbp(1,jbp))then                           
          lord(1,ibp)=lord(1,ibp)+1                                     
         else if(xlcbp(1,ibp).eq.xlcbp(1,jbp))then                      
           if(xlcbp(2,ibp).gt.xlcbp(2,jbp))then                         
              lord(1,ibp)=lord(1,ibp)+1                                 
           else if(xlcbp(2,ibp).eq.xlcbp(2,jbp))then                    
              call errex(                                               
     a         'strist:two identical break point positions, abort')     
           else                                                         
              lord(1,jbp)=lord(1,jbp)+1                                 
           end if                                                       
         else                                                           
          lord(1,jbp)=lord(1,jbp)+1                                     
         end if                                                         
         if(xlcbp(2,ibp).gt.xlcbp(2,jbp))then                           
          lord(2,ibp)=lord(2,ibp)+1                                     
         else if(xlcbp(2,ibp).eq.xlcbp(2,jbp))then                      
           if(xlcbp(1,ibp).gt.xlcbp(1,jbp))then                         
              lord(2,ibp)=lord(2,ibp)+1                                 
           else if(xlcbp(1,ibp).eq.xlcbp(1,jbp))then                    
              call errex(                                               
     a         'strist:two identical break point positions, abort')     
           else                                                         
              lord(2,jbp)=lord(2,jbp)+1                                 
           end if                                                       
         else                                                           
          lord(2,jbp)=lord(2,jbp)+1                                     
         end if                                                         
  109   continue                                                        
  110  continue                                                         
       ibptot=0                                                         
       do 200 nm=1,nmult                                                
       do 150 ibp=1,ibptru                                              
        if(max(lord(1,ibp),lord(2,ibp)).gt.ibptru.or.                   
     a                  min(lord(1,ibp),lord(2,ibp)).lt.0) then         
          call errex('strist:wrong lord value')                         
        else if(lord(1,ibp).eq.0) then                                  
          go to 150                                                     
        else if(icomb(1,ibp).ne.0.and.                                  
     a                       icomb(2,ibp).ne.0) then                    
          go to 150                                                     
        end if                                                          
        if(nm.eq.1) then                                                
          if(ibp.le.ibpfor) then                                        
           jt=2-mod(ibp,2)                                              
           jr=3-jt                                                      
           ivord=(ibp-jt)/2                                             
           ivlc(jr,ibp)=ivord                                           
           ivlc(jt,ibp)=nmult                                           
           ipact(jt,ibp)=0                                              
           iqact(jt,ibp)=0                                              
           ipact(jr,ibp)=iqdgr(jt,ivord+1)                              
           iqact(jr,ibp)=ipdgr(jt,ivord+1)                              
          else                                                          
           ipact(1,ibp)=ipdgr(1,1)+ipact(1,ibp)                         
           iqact(1,ibp)=iqdgr(1,1)+iqact(1,ibp)                         
           ipact(2,ibp)=ipdgr(2,1)+ipact(2,ibp)                         
           iqact(2,ibp)=iqdgr(2,1)+iqact(2,ibp)                         
           do 119 jbp=1,ibpfor                                          
            if(lord(1,ibp).gt.lord(1,jbp).and.                          
     a            lord(2,ibp).gt.lord(2,jbp)) then                      
              jt=2-mod(jbp,2)                                           
              jr=3-jt                                                   
              itrjt=itrip(iquark(jr,jbp))                               
              ivlc(1,ibp)=ivlc(1,ibp)+1                                 
              ivlc(2,ibp)=ivlc(2,ibp)+1                                 
              if(itrjt.eq.10) then                                      
                 ipact(jt,ibp)=ipact(jt,ibp)-1                          
                 iqact(jr,ibp)=iqact(jr,ibp)-1                          
              else                                                      
                 iqact(jt,ibp)=iqact(jt,ibp)-1                          
                 ipact(jr,ibp)=ipact(jr,ibp)-1                          
              end if                                                    
              if(ipact(1,ibp).lt.0.or.iqact(1,ibp).lt.0.or.             
     a                ipact(2,ibp).lt.0.or.iqact(2,ibp).lt.0) then      
               lord(1,ibp)=0                                            
               lord(2,ibp)=0                                            
               go to 150                                                
              end if                                                    
            end if                                                      
  119      continue                                                     
          end if                                                        
        end if                                                          
        ivord1=ivlc(1,ibp)                                              
        ivord2=ivlc(2,ibp)                                              
        ivord=min(ivord1,ivord2)                                        
        if(.not.newbp(ibp)) then                                        
        else if(ivord+1.gt.nm) then                                     
        else                                                            
          if(ivord+1.lt.nm) then                                        
            call errex(                                                 
     a         'strist conflict:new break point in past field region')  
          else                                                          
            if(ibp.le.ibpfor) then                                      
            else                                                        
               if(ivord1.ne.ivord2) then                                
                 call errex('strist:ambiguous field for break point')   
               end if                                                   
               ipa=ipact(1,ibp)                                         
               iqa=iqact(1,ibp)                                         
               ipiq(ibp)=100*ipa+iqa                                    
               ibptot=max(ibptot,ibp)                                   
               if(iqa.ne.ipact(2,ibp).or.                               
     a                  ipa.ne.iqact(2,ibp)) then                       
                    call errex(                                         
     a                 'strist:field for new break not well defined')   
               end if                                                   
               newbp(ibp)=.false.                                       
               if(ipa.lt.0.or.iqa.lt.0) then                            
                 lord(1,ibp)=0                                          
                 lord(2,ibp)=0                                          
                 go to 150                                              
               end if                                                   
               atrel=atre                                               
               if(reject(ipa,iqa,atrel)) then                           
                 lord(1,ibp)=0                                          
                 lord(2,ibp)=0                                          
                 go to 150                                              
               end if                                                   
               call flavor(ipa,iqa,ifl)                                 
               iquark(1,ibp)=ifl                                        
               iquark(2,ibp)=-ifl                                       
               if(par(12).gt.0.) then                                   
                  kqqbar=(isign(1,(ipa-iqa)*ifl)+3)/2                   
                  par12=par(12)                                         
                  par(12)=sigptr(ipa,iqa,kqqbar)                        
                  call luptdi(ifl,ptr(1,1,ibp),ptr(1,2,ibp))            
                  ptr(2,1,ibp)=-ptr(1,1,ibp)                            
                  ptr(2,2,ibp)=-ptr(1,2,ibp)                            
                  par(12)=par12                                         
               else                                                     
                  ptr(1,1,ibp)=0.0                                      
                  ptr(1,2,ibp)=0.0                                      
                  ptr(2,1,ibp)=0.0                                      
                  ptr(2,2,ibp)=0.0                                      
               end if                                                   
               jbpcrl=0                                                 
               jbpcrr=0                                                 
               xdifmn=xlcrop(1)                                         
               ydifmn=xlcrop(2)                                         
               do 125 jbp=1,ibptru                                      
                ivor2=ivlc(2,jbp)                                       
                if(ibp.eq.jbp) then                                     
                   go to 125                                            
                else if(lord(1,jbp).lt.lord(1,ibp)) then                
                   go to 124                                            
                else if(lord(1,jbp).eq.0) then                          
                   go to 124                                            
                else if(ivor2+1.gt.nm) then                             
                  go to 124                                             
                else if(icomb(2,jbp).ne.0) then                         
                  go to 124                                             
                else if(iquark(2,jbp).eq.0) then                        
                  go to 124                                             
                end if                                                  
                if(lord(2,ibp).gt.lord(2,jbp).and.                      
     a                xlcmx(2,jbp).lt.xlcbp(2,ibp).and.                 
     b                 xlcmx(1,ibp).lt.xlcbp(1,jbp)) then               
                  xdif=xlcbp(1,jbp)-xlcbp(1,ibp)                        
                  if(xdif.lt.xdifmn) then                               
                     xdifmn=xdif                                        
                     jbpcrl=jbp                                         
                  end if                                                
                end if                                                  
  124           continue                                                
                ivor1=ivlc(1,jbp)                                       
                if(lord(2,jbp).lt.lord(2,ibp)) then                     
                   go to 125                                            
                else if(lord(1,jbp).eq.0) then                          
                   go to 125                                            
                else if(ivor1+1.gt.nm) then                             
                  go to 125                                             
                else if(icomb(1,jbp).ne.0) then                         
                  go to 125                                             
                else if(iquark(1,jbp).eq.0) then                        
                  go to 125                                             
                end if                                                  
                if(lord(1,ibp).gt.lord(1,jbp).and.                      
     a                xlcmx(1,jbp).lt.xlcbp(1,ibp).and.                 
     b                 xlcmx(2,ibp).lt.xlcbp(2,jbp)) then               
                  ydif=xlcbp(2,jbp)-xlcbp(2,ibp)                        
                  if(ydif.lt.ydifmn) then                               
                     ydifmn=ydif                                        
                     jbpcrr=jbp                                         
                  end if                                                
                end if                                                  
  125          continue                                                 
              if(jbpcrl.eq.0) then                                      
                call errex('strist:no jbpcrl found')                    
              end if                                                    
              if(jbpcrr.eq.0) then                                      
                call errex('strist:no jbpcrr found')                    
              end if                                                    
               jt=1                                                     
               jr=2                                                     
               itrjt=itrip(iquark(jt,ibp))                              
               do 145 jbp=ibp+1,ibptru                                  
                if(lord(1,ibp).lt.lord(1,jbp).and.                      
     a                   lord(2,ibp).lt.lord(2,jbp)) then               
                  ivlc(1,jbp)=ivlc(1,jbp)+1                             
                  ivlc(2,jbp)=ivlc(2,jbp)+1                             
                  if(itrjt.eq.1) then                                   
                    ipact(jt,jbp)=ipact(jt,jbp)-1                       
                    iqact(jr,jbp)=iqact(jr,jbp)-1                       
                  else                                                  
                    iqact(jt,jbp)=iqact(jt,jbp)-1                       
                    ipact(jr,jbp)=ipact(jr,jbp)-1                       
                  end if                                                
                end if                                                  
                if(ivlc(1,jbp)+1.gt.nmult) then                         
                   lord(1,jbp)=0                                        
                   lord(2,jbp)=0                                        
                end if                                                  
  145          continue                                                 
            end if                                                      
          end if                                                        
        end if                                                          
  150  continue                                                         
       do 151 ibp=1,ibptru                                              
        icross(1,ibp)=.false.                                           
        icross(2,ibp)=.false.                                           
        if(iquark(2,ibp).eq.0) then                                     
          icross(2,ibp)=.true.                                          
        end if                                                          
        if(iquark(1,ibp).eq.0) then                                     
          icross(1,ibp)=.true.                                          
        end if                                                          
        if(icomb(1,ibp).ne.0) then                                      
          icross(1,ibp)=.true.                                          
        end if                                                          
        if(icomb(2,ibp).ne.0) then                                      
          icross(2,ibp)=.true.                                          
        end if                                                          
        if(lord(1,ibp).eq.0) then                                       
          icross(1,ibp)=.true.                                          
          icross(2,ibp)=.true.                                          
        end if                                                          
        if(ivlc(1,ibp)+1.gt.nm) then                                    
          icross(1,ibp)=.true.                                          
        end if                                                          
        if(ivlc(2,ibp)+1.gt.nm) then                                    
          icross(2,ibp)=.true.                                          
        end if                                                          
  151  continue                                                         
       do 170 ibp=1,ibptru                                              
        ivord=ivlc(1,ibp)                                               
        if(lord(1,ibp).eq.0) then                                       
          go to 170                                                     
        else if(ivord+1.gt.nm) then                                     
          go to 170                                                     
        else if(icomb(1,ibp).ne.0) then                                 
          go to 170                                                     
        else if(iquark(1,ibp).eq.0) then                                
          if(ibp.gt.ibpfor) then                                        
             call errex(                                                
     a           'strist:breakpoint without rightmoving quark')         
          end if                                                        
          go to 170                                                     
        end if                                                          
        jbpcrs=0                                                        
        xdifmn=xlcrop(1)+1.0                                            
        do 155 jbp=1,ibptru                                             
         ivor2=ivlc(2,jbp)                                              
         if(jbp.eq.ibp) then                                            
           go to 155                                                    
         else if(lord(1,jbp).lt.lord(1,ibp)) then                       
           go to 155                                                    
         else if(lord(1,jbp).eq.0) then                                 
           go to 155                                                    
         else if(ivor2+1.gt.nm) then                                    
           go to 155                                                    
         else if(icomb(2,jbp).ne.0) then                                
           go to 155                                                    
         else if(iquark(2,jbp).eq.0) then                               
           if(jbp.gt.ibpfor) then                                       
              call errex(                                               
     a           'strist:breakpoint without leftmoving quark')          
           end if                                                       
           go to 155                                                    
         end if                                                         
         if(lord(2,ibp).gt.lord(2,jbp).and.                             
     a         xlcmx(2,jbp).lt.xlcbp(2,ibp).and.                        
     b            xlcmx(1,ibp).lt.xlcbp(1,jbp)) then                    
           xdif=xlcbp(1,jbp)-xlcmx(1,ibp)                               
           if(xdif.lt.xdifmn) then                                      
             xdifmn=xdif                                                
             jbpcrs=jbp                                                 
           end if                                                       
         end if                                                         
  155   continue                                                        
        if(jbpcrs.eq.0) then                                            
          write(8,*) 'ibp=',ibp                                         
          call errex('strist:no jbpcrs found')                          
        else                                                            
          dif1=xdifmn                                                   
          dif2=xlcbp(2,ibp)-xlcmx(2,jbpcrs)                             
          xlcmx(1,ibp)=xlcbp(1,jbpcrs)                                  
          xlcmx(2,jbpcrs)=xlcbp(2,ibp)                                  
          icross(1,ibp)=.true.                                          
          if(icross(2,jbpcrs)) then                                     
            call errex('strist:left jbpcrs quark trajektory used up')   
          end if                                                        
          icross(2,jbpcrs)=.true.                                       
        end if                                                          
        jbp=jbpcrs                                                      
        iq1i=iquark(1,ibp)                                              
        iq2j=iquark(2,jbp)                                              
        itr1i=itrip(iq1i)                                               
        itr2j=itrip(iq2j)                                               
        ivor2=ivlc(2,jbp)                                               
        if(itr1i.eq.1) then                                             
          ipi=0                                                         
          iqi=1                                                         
        else                                                            
          ipi=1                                                         
          iqi=0                                                         
        end if                                                          
        tnsdi=0.75*(casop(ipact(1,ibp),iqact(1,ibp))-                   
     a               casop(iqact(1,ibp)-ipi,ipact(1,ibp)-iqi))          
        if(itr2j.eq.1) then                                             
          ipi=0                                                         
          iqi=1                                                         
        else                                                            
          ipi=1                                                         
          iqi=0                                                         
        end if                                                          
        tnsdj=0.75*(casop(ipact(2,jbp),iqact(2,jbp))-                   
     a               casop(iqact(2,jbp)-ipi,ipact(2,jbp)-iqi))          
        equark(1,ibp)=equark(1,ibp)+tnsdi*dif1                          
        equark(2,jbp)=equark(2,jbp)+tnsdj*dif2                          
        ivlc(1,ibp)=ivlc(1,ibp)+1                                       
        ivlc(2,jbp)=ivlc(2,jbp)+1                                       
        if(itr2j.eq.10) then                                            
          ipact(1,ibp)=ipact(1,ibp)-1                                   
        else                                                            
          iqact(1,ibp)=iqact(1,ibp)-1                                   
        end if                                                          
        if(itr1i.eq.10) then                                            
          ipact(2,jbp)=ipact(2,jbp)-1                                   
        else                                                            
          iqact(2,jbp)=iqact(2,jbp)-1                                   
        end if                                                          
        iuseup=0                                                        
        if(itr1i.eq.10) then                                            
          irmain=iqact(1,ibp)                                           
        else if(itr1i.eq.1) then                                        
          irmain=ipact(1,ibp)                                           
        end if                                                          
        if(itr2j.eq.10) then                                            
          irmain=irmain*iqact(2,jbp)                                    
        else if(itr2j.eq.1) then                                        
          irmain=irmain*ipact(2,jbp)                                    
        end if                                                          
        if(ivord+1.eq.nmult) then                                       
          if(ivor2+1.ne.nmult) then                                     
            call errex('strist:ivord.ne.ivor2')                         
          end if                                                        
          icomb(1,ibp)=jbp                                              
          icomb(2,jbp)=ibp                                              
        else if(itr1i*itr2j.eq.10.and.                                  
     a             ranf(0).le.1./(1.+irmain)) then                      
          icomb(1,ibp)=jbp                                              
          icomb(2,jbp)=ibp                                              
          iuseup=1                                                      
        else                                                            
          if(itr1i.eq.10.and.                                           
     a                   iqact(1,ibp).eq.0) then                        
             icomb(1,ibp)=jbp                                           
             icomb(2,jbp)=ibp                                           
             iuseup=1                                                   
          else if(itr1i.eq.1.and.                                       
     a                       ipact(1,ibp).eq.0) then                    
             icomb(1,ibp)=jbp                                           
             icomb(2,jbp)=ibp                                           
             iuseup=1                                                   
          else if(itr2j.eq.10.and.                                      
     a                       iqact(2,jbp).eq.0) then                    
             icomb(1,ibp)=jbp                                           
             icomb(2,jbp)=ibp                                           
             iuseup=1                                                   
          else if(itr2j.eq.1.and.                                       
     a                       ipact(2,jbp).eq.0) then                    
             icomb(1,ibp)=jbp                                           
             icomb(2,jbp)=ibp                                           
             iuseup=1                                                   
          end if                                                        
        end if                                                          
        if(icomb(1,ibp).ne.0) then                                      
          if(itr1i*itr2j.ne.10) then                                    
            call errex('strist:two quarks do not match for hadron')     
          end if                                                        
          lor1=lord(1,jbp)                                              
          lor2=lord(2,ibp)                                              
          if(iuseup.eq.1) then                                          
            do 169 kbp=ibp+1,ibptru                                     
             if(lor1.lt.lord(1,kbp).and.                                
     a                   lor2.lt.lord(2,kbp)) then                      
               if(min(ivlc(1,kbp),ivlc(2,kbp)).le.nm) then              
                 call errex(                                            
     a            'strist:break point in lc of xpoint,wrong ivlc')      
               end if                                                   
               ivlc(1,kbp)=ivlc(1,kbp)-1                                
               ivlc(2,kbp)=ivlc(2,kbp)-1                                
               if(itr1i.eq.10) then                                     
                 iqact(1,kbp)=iqact(1,kbp)+1                            
               else                                                     
                 ipact(1,kbp)=ipact(1,kbp)+1                            
               end if                                                   
               if(itr2j.eq.10) then                                     
                 iqact(2,kbp)=iqact(2,kbp)+1                            
               else                                                     
                 ipact(2,kbp)=ipact(2,kbp)+1                            
               end if                                                   
             end if                                                     
  169       continue                                                    
          end if                                                        
        end if                                                          
  170  continue                                                         
       do 180 ibp=1,ibptru                                              
        if(.not.icross(1,ibp).or..not.icross(2,ibp)) then               
           call errex('strist:icross=f afterwards')                     
        end if                                                          
  180  continue                                                         
  200  continue                                                         
       do 140 ibp=1,ibpfor                                              
        if(iquark(1,ibp).eq.0) then                                     
          jt=2                                                          
        else                                                            
          jt=1                                                          
        end if                                                          
        iqibp=iquark(jt,ibp)                                            
        if(iabs(iqibp).lt.10) go to 140                                 
        jr=3-jt                                                         
        par(11)=para11                                                  
        jbp=icomb(jt,ibp)                                               
        iqjbp=iquark(jr,jbp)                                            
        if(iabs(iqjbp).gt.10.and.jbp.le.ibpfor) then                    
          lbp=0                                                         
          do 131 kbp=1+mod(jr-1,2),ibpfor-mod(jr,2),2                   
           if(kbp.eq.ibp) go to 131                                     
           iqkbp=iquark(jt,kbp)                                         
           if(iabs(iqkbp).gt.10) go to 131                              
           if(itrip(iqibp).ne.itrip(iqkbp)) go to 131                   
           if(iabs(iquark(jr,icomb(jt,kbp))).gt.10) go to 131           
           lbp=kbp                                                      
           go to 128                                                    
  131     continue                                                      
  128     continue                                                      
          if(lbp.eq.0) then                                             
            go to 100                                                   
          else                                                          
            iquark(jt,ibp)=iqkbp                                        
            iquark(jt,lbp)=iqibp                                        
          end if                                                        
        end if                                                          
  140  continue                                                         
       do 201  nsi=1,nsingl-1,2                                         
        ibpl=0                                                          
        ibpr=0                                                          
        ibptr=ibptru                                                    
  202   continue                                                        
        if(ibptru.eq.0) then                                            
          ibptru=1                                                      
          ibptr=1                                                       
        else if(lord(1,ibptr).eq.0) then                                
        else if(ibptr.gt.ibpfor+1) then                                 
          ibptr=ibptr-1                                                 
          go to 202                                                     
        else if(ibptru.lt.kbpmax) then                                  
          ibptru=ibptru+1                                               
          ibptr=ibptru                                                  
        else                                                            
          call errex('strist 201 no array element free')                
        end if                                                          
        if(ibpr.eq.0) then                                              
          ibpr=ibptr                                                    
          lord(1,ibpr)=-1                                               
          lord(2,ibpr)=-2                                               
          go to 202                                                     
        else                                                            
          ibpl=ibptr                                                    
          lord(1,ibpl)=-1                                               
          lord(2,ibpl)=-2                                               
        end if                                                          
        iro1=mod(isingl(nsi),1000)                                      
        iro2=mod(isingl(nsi+1),1000)                                    
        iro3=isingl(nsi)/1000                                           
        if(iro3.eq.0) iro3=isingl(nsi+1)/1000                           
        equark(1,ibpr)=plcone(1,iro1)+plcone(1,iro2)                    
        if(iro3.gt.0)                                                   
     a  equark(1,ibpr)=equark(1,ibpr)+plcone(1,iro3)                    
        equark(2,ibpr)=plcone(2,iro1)+plcone(2,iro2)                    
        if(iro3.gt.0)                                                   
     a  equark(2,ibpr)=equark(2,ibpr)+plcone(2,iro3)                    
        equark(1,ibpl)=0.0                                              
        equark(2,ibpl)=0.0                                              
        call luptdi(94,pxr,pyr)                                         
        ptr(1,1,ibpr)=pxr+ptstor(1,iro1)                                
        ptr(1,2,ibpr)=pyr+ptstor(2,iro1)                                
        if(isingl(nsi).gt.1000) then                                    
          ptr(1,1,ibpr)=ptr(1,1,ibpr)+ptstor(1,iro3)                    
          ptr(1,2,ibpr)=ptr(1,2,ibpr)+ptstor(2,iro3)                    
        end if                                                          
        ptr(2,1,ibpr)=-pxr+ptstor(1,iro2)                               
        ptr(2,2,ibpr)=-pyr+ptstor(2,iro2)                               
        if(isingl(nsi+1).gt.1000) then                                  
          ptr(2,1,ibpr)=ptr(2,1,ibpr)+ptstor(1,iro3)                    
          ptr(2,2,ibpr)=ptr(2,2,ibpr)+ptstor(2,iro3)                    
        end if                                                          
        ptr(1,1,ibpl)=0.0                                               
        ptr(1,2,ibpl)=0.0                                               
        ptr(2,1,ibpl)=0.0                                               
        ptr(2,2,ibpl)=0.0                                               
        xlcbp(1,ibpr)=equark(1,ibpr)                                    
        xlcbp(2,ibpr)=0.0                                               
        xlcmx(1,ibpr)=xlcbp(1,ibpr)                                     
        xlcmx(2,ibpr)=sqrt(ptr(1,1,ibpr)**2+ptr(1,2,ibpr)**2)/          
     a                 xlcbp(1,ibpr)                                    
        xlcbp(2,ibpl)=equark(2,ibpr)                                    
        xlcbp(1,ibpl)=0.0                                               
        xlcmx(2,ibpl)=xlcbp(2,ibpl)                                     
        xlcmx(1,ibpl)=sqrt(ptr(2,1,ibpr)**2+ptr(2,2,ibpr)**2)/          
     a                 xlcbp(2,ibpl)                                    
        do 203 jt=1,2                                                   
         icomb(jt,ibpr)=ibpl                                            
         icomb(jt,ibpl)=ibpr                                            
         iquark(jt,ibpr)=iflstr(jt,nsi)                                 
         iquark(3-jt,ibpl)=iflstr(jt,nsi+1)                             
         ibp=0                                                          
         if(iabs(iflstr(jt,nsi)).gt.10) then                            
           ibp=ibpr                                                     
           jtact=jt                                                     
         else if(iabs(iflstr(jt,nsi+1)).gt.10) then                     
           ibp=ibpl                                                     
           jtact=3-jt                                                   
         end if                                                         
         if(ibp.ne.0) then                                              
           jetzhd(jtact,ibp)=iqqkf(jt,mxs+1-nsi)                        
           if(iabs(jetzhd(jtact,ibp)).le.1) then                        
             call errex('strist:iqqkf  is empty')                       
           end if                                                       
         end if                                                         
         dquark(jt,ibpr)=0.0                                            
         dquark(jt,ibpl)=0.0                                            
  203   continue                                                        
        ipiq(ibpr)=100                                                  
        ipiq(ibpl)=1                                                    
  201  continue                                                         
       do 205  ibp=1,ibpfor                                             
        io=(ibp+mod(ibp,2))/2                                           
        jt=1                                                            
        if(mod(ibp,2).eq.1) jt=2                                        
        jr=3-jt                                                         
        if(iquark(jt,ibp).eq.0)                                         
     a  call errex('strist205: empty iquark')                           
        nm=iord(jr,io)                                                  
        if(iqqkf(jr,nm).le.0) go to 205                                 
        iqibp=iquark(jt,ibp)                                            
        if(iabs(iqibp).lt.10)                                           
     a  call errex('strist205: no diquark')                             
        ibpl=0                                                          
        ibpr=0                                                          
        ibptr=ibptru                                                    
  204   continue                                                        
        if(ibptru.eq.0) then                                            
          ibptru=1                                                      
          ibptr=1                                                       
        else if(lord(1,ibptr).eq.0) then                                
        else if(ibptr.gt.ibpfor+1) then                                 
          ibptr=ibptr-1                                                 
          go to 204                                                     
        else if(ibptru.lt.kbpmax) then                                  
          ibptru=ibptru+1                                               
          ibptr=ibptru                                                  
        else                                                            
          call errex('strist205 no array element free')                 
        end if                                                          
        if(ibpr.eq.0) then                                              
          ibpr=ibptr                                                    
          lord(1,ibpr)=-1                                               
          lord(2,ibpr)=-1                                               
          go to 204                                                     
        else                                                            
          ibpl=ibptr                                                    
          lord(1,ibpl)=-1                                               
          lord(2,ibpl)=-1                                               
        end if                                                          
        iro1=iqqkf(jr,nm)                                               
        if(iro1.gt.iroin)                                               
     a  call errex('strist205: iro1 out of range')                      
        equark(jr,ibpr)=plcone(jr,iro1)                                 
        equark(jt,ibpr)=0.0                                             
        ifl1=iqibp/10                                                   
        ifl2=iqibp-10*ifl1                                              
        if(iqibp.lt.0) then                                             
          isig=-1                                                       
        else                                                            
          isig=1                                                        
        end if                                                          
        ipa=ipdgr(jr,io)+isig                                           
        iqa=iqdgr(jr,io)-isig                                           
        if(ipa.lt.0.or.iqa.lt.0)                                        
     a  call errex('strist205:ipa/iqa problem')                         
        mtry=0                                                          
  225   continue                                                        
        mtry=mtry+1                                                     
        if(mtry.gt.50) call errex('strist225 abort')                    
        call flavor(ipa,iqa,ifl3)                                       
        if(iabs(ifl3).gt.10) go to 225                                  
        ifl3=-ifl3                                                      
        if(ifl3*iqibp.lt.0) go to 225                                   
        if(ifl2.ne.ifl3.and.ranf(0).gt.qqdqqs) go to 225                
        iquark(jr,ibpr)=ifl1                                            
        iquark(jt,ibpr)=0                                               
        iquark(jt,ibp)=kqqret(ifl2,ifl3)                                
        iquark(jt,ibpl)=-ifl3                                           
        iquark(jr,ibpl)=0                                               
        if(par(12).gt.0.) then                                          
          kqqbar=(isign(1,(ipa-iqa)*ifl3)+3)/2                          
          par12=par(12)                                                 
          par(12)=sigptr(ipa,iqa,kqqbar)                                
          call luptdi(ifl,pxr,pyr)                                      
          par(12)=par12                                                 
        else                                                            
          pxr=0.0                                                       
          pyr=0.0                                                       
        end if                                                          
        ptr(jr,1,ibpr)=pxr                                              
        ptr(jr,2,ibpr)=pyr                                              
        ptr(jt,1,ibp)=-pxr+ptr(jt,1,ibp)                                
        ptr(jt,2,ibp)=-pyr+ptr(jt,2,ibp)                                
        xlcbp(jr,ibpr)=xlcbp(jr,ibp)+0.5*equark(jr,ibpr)                
        xlcbp(jt,ibpr)=0.0                                              
        xlcmx(jr,ibpr)=xlcbp(jr,ibpr)                                   
        xlcmx(jt,ibpr)=sqrt(ptr(jr,1,ibpr)**2+ptr(jr,2,ibpr)**2)/       
     a                 (0.5*equark(jr,ibpr))                            
        icomb(jr,ibpr)=ibpl                                             
        icomb(jt,ibpr)=0                                                
        icomb(jt,ibpl)=ibpr                                             
        icomb(jr,ibpl)=0                                                
        do 206 jn=1,2                                                   
         xlcbp(jn,ibpl)=xlcbp(jn,ibpr)                                  
         xlcmx(jn,ibpl)=xlcmx(jn,ibpr)                                  
         ptr(jt,jn,ibpr)=0.0                                            
         ptr(1,jn,ibpl)=0.0                                             
         ptr(2,jn,ibpl)=0.0                                             
         equark(jn,ibpl)=0.0                                            
         dquark(jn,ibpr)=0.0                                            
         dquark(jn,ibpl)=0.0                                            
  206   continue                                                        
        ipiq(ibpr)=100*ipa+iqa                                          
        ipiq(ibpl)=100*ipa+iqa                                          
  205  continue                                                         
       pplall=0.0                                                       
       pmiall=0.0                                                       
       do 210  ibp=1,ibptru                                             
        if(lord(1,ibp).lt.0) then                                       
          do 224 jt=1,2                                                 
           if(iabs(iquark(jt,ibp)).gt.10) then                          
             jbp=icomb(jt,ibp)                                          
             jetzhd(3-jt,jbp)=-1                                        
             kf1=jetzhd(jt,ibp)                                         
             if(iabs(kf1).le.40) then                                   
               call errex('strist204,no baryon')                        
             end if                                                     
             bpli(jt,ibp)=getmas(0,kf1,0,-1.0)                          
           else                                                         
             jetzhd(jt,ibp)=0                                           
             bpli(jt,ibp)=0.0                                           
           end if                                                       
  224     continue                                                      
        else                                                            
          jetzhd(1,ibp)=0                                               
          jetzhd(2,ibp)=0                                               
          bpli(1,ibp)=0.0                                               
          bpli(2,ibp)=0.0                                               
        end if                                                          
        pplall=pplall+equark(1,ibp)+dquark(1,ibp)                       
        pmiall=pmiall+equark(2,ibp)+dquark(2,ibp)                       
  210  continue                                                         
       if(abs(pplall-ps(1)).gt.0.003.or.                                
     a      abs(pmiall-ps(2)).gt.0.003) then                            
          write(8,*) pplall,pmiall,ps(1),ps(2)                          
          call errex('strist:no energy/mom. conservation, 210')         
       end if                                                           
       kbreak=0                                                         
       ibpsto=0                                                         
  220  ibpsto=ibpsto+1                                                  
       ibp=ibpsto                                                       
  221  continue                                                         
        if(ibp.gt.ibptru) go to 222                                     
        if(ibp.le.ibpfor) then                                          
          if(iquark(1,ibp).eq.0) then                                   
            jt=2                                                        
          else                                                          
            jt=1                                                        
          end if                                                        
          iqibp=iquark(jt,ibp)                                          
          if(iabs(iqibp).lt.10) go to 220                               
          jr=3-jt                                                       
          par(11)=para11                                                
          jbp=icomb(jt,ibp)                                             
          iqjbp=iquark(jr,jbp)                                          
          if(iabs(iqjbp).gt.10.and.jbp.le.ibpfor) then                  
            call errex('strist:unexpected qq-qbqb configuration')       
          else if(                                                      
     a       jbp.le.ibpfor.or.                                          
     b       (jbp.gt.ibpfor.and.jbp.le.ibptot.and.                      
     c        (jetzhd(1,jbp).eq.-1.or.jetzhd(2,jbp).eq.-1))) then       
            mtry=0                                                      
            iqibpb=0                                                    
            iqqibp=iqibp                                                
  211       continue                                                    
            mtry=mtry+1                                                 
            if(mtry.gt.100) then                                        
              call errex('strist211:mtry>100')                          
            end if                                                      
            if(ranf(0).gt.barwgh(para11,iqqibp,iqjbp)) then             
              iqibpb=0                                                  
              mof=int(ranf(0)*(nmult+1))                                
              do 209 ma=mof+1,mof+nmult                                 
               nbp=2*(mod(ma,nmult)+1)                                  
               if(jt.eq.2) nbp=nbp-1                                    
               if(nbp.le.0.or.nbp.gt.ibpfor) then                       
                 call errex('strist:loop209 problem')                   
               end if                                                   
               if(iquark(jt,nbp).eq.0) then                             
                 call errex('strist:loop209 problem')                   
               else                                                     
                 iqnbp=iquark(jt,nbp)                                   
               end if                                                   
               if(iabs(iqnbp).gt.10) go to 209                          
               if(jetzhd(jt,nbp).ne.0) go to 209                        
               iqibp1=iqibp/10                                          
               iqibp2=iqibp-10*iqibp1                                   
               if(iqibp1.eq.iqibp2) then                                
                 if(iqibp1+iqnbp.ne.0) go to 209                        
                 iqibpa=iqibp1                                          
               else                                                     
                 if(iqibp1.eq.-iqnbp) then                              
                   iqibpa=iqibp2                                        
                 else if(iqibp2.eq.-iqnbp) then                         
                   iqibpa=iqibp1                                        
                 else                                                   
                   go to 209                                            
                 end if                                                 
               end if                                                   
  207          iqibpb=isign(1,iqibpa)*(int(ranf(0)*(2.+par2))+1)        
               if(iqibpb.ne.iqibpa.and.ranf(0).gt.qqdqqs) then          
                 go to 207                                              
               end if                                                   
               go to 208                                                
  209         continue                                                  
              if(mtry.gt.1.and.iqibp.eq.0) then                         
                call errex('strist 208,iqibp problem')                  
              end if                                                    
  208         continue                                                  
              if(iqibpb.ne.0) then                                      
                iqqibp=kqqret(iqibpa,iqibpb)                            
                go to 211                                               
              end if                                                    
            end if                                                      
            if(iqibpb.ne.0) then                                        
              iquark(jt,ibp)=iqqibp                                     
              iquark(jt,nbp)=-iqibpb                                    
            end if                                                      
            call luifld(iqqibp,0,iqjbp,ifldum,kf1)                      
            jetzhd(1,jbp)=-1                                            
            jetzhd(2,jbp)=-1                                            
            jetzhd(jt,ibp)=kf1                                          
            bpli(jt,ibp)=getmas(0,kf1,0,-1.0)                           
          else if(jbp.gt.ibpfor.and.jbp.le.ibptot) then                 
            if(jetzhd(1,jbp).eq.-1.or.jetzhd(2,jbp).eq.-1) then         
              call errex('strist:dead branch')                          
            else                                                        
              ipbrk=ipiq(jbp)                                           
              iqbrk=mod(ipbrk,100)                                      
              ipbrk=ipbrk/100                                           
              if(ipbrk.gt.iprbr1) ipbrk=iprbr1                          
              if(iqbrk.gt.iprbr1) iqbrk=iprbr1                          
              iquabr=iquark(1,jbp)                                      
              if(iqbrk.gt.ipbrk) then                                   
                iquabr=-iquabr                                          
                ientry=iqbrk*(iqbrk+1)/2+ipbrk                          
              else                                                      
                ientry=ipbrk*(ipbrk+1)/2+iqbrk                          
              end if                                                    
              if(itrip(iquabr).eq.1) then                               
                ike=1                                                   
              else                                                      
                ike=2                                                   
              end if                                                    
              if(prbrop(2*ike-1,ientry).eq.0.0) then                    
                write(8,*)jt,ipiq(jbp),jbp,ike,iquark(jt,jbp),ientry    
                call errex('strist abort when generating qqbar')        
              end if                                                    
              par(2)=prbrop(ike*2,ientry)/prbrop(2*ike-1,ientry)        
              call luifld(iqibp,0,0,ifld,kf1)                           
            end if                                                      
            jetzhd(jt,ibp)=kf1                                          
            bpli(jt,ibp)=getmas(0,kf1,0,-1.0)                           
            iquark(jr,jbp)=ifld                                         
            iquark(jt,jbp)=-ifld                                        
            jetzhd(1,jbp)=-1                                            
            jetzhd(2,jbp)=-1                                            
            par(2)=par2                                                 
          end if                                                        
          par(11)=par11                                                 
          go to 220                                                     
        end if                                                          
        if(iquark(1,ibp).eq.0) go to 220                                
        if(lord(1,ibp).eq.0) go to 220                                  
        kbreak=kbreak+1                                                 
        if(nlund+kbreak+ibpfor/2.gt.2000) then                          
          call errex('strist:jetset array too small')                   
        end if                                                          
  212   continue                                                        
        iqlibp=iquark(2,ibp)                                            
        if(iabs(iqlibp).lt.10) go to 220                                
        jbpl=icomb(2,ibp)                                               
        jbpr=icomb(1,ibp)                                               
        if(lord(1,ibp).lt.0.and.lord(2,jbpl).lt.0) go to 220            
        if(lord(2,ibp).lt.0.and.lord(1,jbpr).lt.0) go to 220            
        jqlibp=iquark(1,jbpl)                                           
        jqribp=iquark(2,jbpr)                                           
        if(iabs(jqlibp).gt.10.or.                                       
     a          iabs(jqribp).gt.10) then                                
          if((jbpl.lt.ibp.and.jbpl.gt.ibpfor.                           
     a                and.iabs(jqlibp).gt.10).or.                       
     b       (jbpr.lt.ibp.and.jbpr.gt.ibpfor.                           
     c                and.iabs(jqribp).gt.10)) then                     
            call errex('strist:lower entry contains diquark')           
          else                                                          
            ipbrk=ipiq(ibp)                                             
            iqbrk=mod(ipbrk,100)                                        
            ipbrk=ipbrk/100                                             
            ltry=0                                                      
  214       continue                                                    
            ltry=ltry+1                                                 
            if(ltry.gt.100) then                                        
              call errex('strist214:ltry>100')                          
            end if                                                      
            call flavor(ipbrk,iqbrk,ifl)                                
            if(itrip(-iqlibp).ne.itrip(ifl)) then                       
               go to 214                                                
            else if(iabs(ifl).lt.10) then                               
               iquark(1,ibp)=ifl                                        
               iquark(2,ibp)=-ifl                                       
            else                                                        
               go to 214                                                
            end if                                                      
          end if                                                        
          go to 212                                                     
        else                                                            
          par1=par(1)                                                   
          par3=par(3)                                                   
          par4=par(4)                                                   
          par5=par(5)                                                   
          if(ranf(0).gt.0.5) then                                       
            igolr=1                                                     
            if(jbpr.le.ibpfor.or.                                       
     a         jetzhd(1,jbpr).eq.-1.or.                                 
     b                 jetzhd(2,jbpr).eq.-1) igolr=2                    
          else                                                          
            igolr=2                                                     
            if(jbpl.le.ibpfor.or.                                       
     a         jetzhd(1,jbpl).eq.-1.or.                                 
     b                 jetzhd(2,jbpl).eq.-1) igolr=1                    
          end if                                                        
          if(igolr.eq.2) then                                           
            iflqq=-iqlibp                                               
            iflq=iquark(2,jbpr)                                         
            jetzhd(1,jbpr)=-1                                           
            jetzhd(2,jbpr)=-1                                           
          else                                                          
            iflqq=iqlibp                                                
            iflq=iquark(1,jbpl)                                         
            jetzhd(1,jbpl)=-1                                           
            jetzhd(2,jbpl)=-1                                           
          end if                                                        
          ipbrk=ipiq(ibp)                                               
          iqbrk=mod(ipbrk,100)                                          
          ipbrk=ipbrk/100                                               
          if(ipbrk.gt.iprbr1) ipbrk=iprbr1                              
          if(iqbrk.gt.iprbr1) iqbrk=iprbr1                              
          iquabr=-iqlibp                                                
          if(iqbrk.gt.ipbrk) then                                       
            iquabr=-iquabr                                              
            ientry=iqbrk*(iqbrk+1)/2+ipbrk                              
          else                                                          
            ientry=ipbrk*(ipbrk+1)/2+iqbrk                              
          end if                                                        
          if(iquabr.gt.0) then                                          
            rnons=2.*prbrop(1,ientry)/                                  
     a                        (2.*prbrop(1,ientry)+prbrop(2,ientry))    
            pqs=1.-rnons                                                
            pss=pqs*pqs                                                 
          else                                                          
            call errex('strist: qq attracted to wrong side')            
          end if                                                        
          par(1)=10.0                                                   
          par(3)=1.0                                                    
          par(4)=1.0                                                    
          par(5)=0.0                                                    
          par(11)=1.0                                                   
          ktry=0                                                        
  213     continue                                                      
          ktry=ktry+1                                                   
          if(ktry.gt.500) then                                          
            call errex('strist213:ktry>200')                            
          end if                                                        
          par(2)=1.0                                                    
          call luifld(iflq,0,0,iflnew,kfb)                              
          if(iabs(iflnew).gt.10) then                                   
           if1new=mod(iabs(iflnew),10)                                  
           if2new=iabs(iflnew)/10                                       
          else                                                          
            go to 213                                                   
          end if                                                        
          if(if1new.eq.3.and.if2new.eq.3) then                          
            if(ranf(0).gt.pss) go to 213                                
          else if(if1new.eq.3.or.if2new.eq.3) then                      
            if(ranf(0).gt.pqs) go to 213                                
          end if                                                        
          bpli(3-igolr,ibp)=getmas(0,kfb,0,-1.0)                        
          jetzhd(3-igolr,ibp)=kfb                                       
          if(igolr.eq.2) then                                           
            iquark(1,ibp)=iflnew                                        
            iquark(2,ibp)=-iflnew                                       
          else                                                          
            iquark(2,ibp)=iflnew                                        
            iquark(1,ibp)=-iflnew                                       
          end if                                                        
          par(1)=par1                                                   
          par(3)=par3                                                   
          par(4)=par4                                                   
          jbp=jbpl                                                      
          ifle=isign(1,iqlibp)*iabs(iflnew)                             
          jt=1                                                          
          jr=2                                                          
  215     continue                                                      
          if(igolr.eq.jt) go to 216                                     
          if(jbp.le.ibpfor.or.                                          
     a       jetzhd(1,jbp).eq.-1.or.                                    
     b                 jetzhd(2,jbp).eq.-1) then                        
            ifld=iquark(jt,jbp)                                         
            call luifld(ifle,0,ifld,ifldum,kf1)                         
          else                                                          
            ipbrk=ipiq(jbp)                                             
            iqbrk=mod(ipbrk,100)                                        
            ipbrk=ipbrk/100                                             
            if(ipbrk.gt.iprbr1) ipbrk=iprbr1                            
            if(iqbrk.gt.iprbr1) iqbrk=iprbr1                            
            iquabr=iquark(1,jbp)                                        
            if(iqbrk.gt.ipbrk) then                                     
              iquabr=-iquabr                                            
              ientry=iqbrk*(iqbrk+1)/2+ipbrk                            
            else                                                        
              ientry=ipbrk*(ipbrk+1)/2+iqbrk                            
            end if                                                      
            if(iquabr.lt.0) then                                        
              ike=1                                                     
            else                                                        
              ike=2                                                     
            end if                                                      
            if(prbrop(2*ike-1,ientry).eq.0.0) then                      
              call errex('strist215 abort,prbrop=0')                    
            end if                                                      
            par(2)=prbrop(ike*2,ientry)/prbrop(2*ike-1,ientry)          
            call luifld(ifle,0,0,ifld,kf1)                              
          end if                                                        
          if(ifld.eq.0.or.kf1.eq.0) then                                
             call errex('strist:luifld didnot generate quark')          
          end if                                                        
          p10sup=1.                                                     
          if(iabs(kfb).gt.60) p10sup=para11                             
          if(iabs(kf1).gt.60) p10sup=p10sup*para11                      
          if(ranf(0).gt.p10sup) go to 213                               
          if(jbp.gt.ibpfor) then                                        
            iquark(jt,jbp)=ifld                                         
            iquark(jr,jbp)=-ifld                                        
          end if                                                        
          bpli(igolr,ibp)=getmas(0,kf1,0,-1.0)                          
          jetzhd(igolr,ibp)=kf1                                         
          jetzhd(1,jbp)=-1                                              
          jetzhd(2,jbp)=-1                                              
  216     continue                                                      
          if(jbp.eq.jbpl) then                                          
            if(jbp.eq.jbpr) then                                        
              call errex('strist:ibp has identical partners,l and r')   
            end if                                                      
            ifle=-ifle                                                  
            jbp=jbpr                                                    
            jt=2                                                        
            jr=1                                                        
            go to 215                                                   
          end if                                                        
          par(2)=par2                                                   
          par(5)=par5                                                   
          par(11)=par11                                                 
        end if                                                          
        if(ibp.lt.ibptru) go to 220                                     
  222  continue                                                         
       pxsum=0.0                                                        
       pysum=0.0                                                        
       do 229  ibp=1,ibptru                                             
        pxsum=pxsum+ptr(1,1,ibp)+ptr(2,1,ibp)                           
        pysum=pysum+ptr(1,2,ibp)+ptr(2,2,ibp)                           
  229  continue                                                         
       if(abs(pxsum-ptsum(1)).gt.0.001.or.                              
     a           abs(pysum-ptsum(2)).gt.0.001) then                     
         write(8,*) 'px/ysum=',pxsum,pysum                              
         write(8,*) 'ptsum=',(ptsum(i),i=1,2)                           
         call errex('strist:no transv.mom. conservation, 229')          
       end if                                                           
       nsave=nlund                                                      
       iposi=0                                                          
       embum=0.0                                                        
       do 230  ibp=1,ibptru                                             
        if(lord(1,ibp).eq.0) go to 230                                  
        if(iquark(1,ibp).eq.0) go to 230                                
        ifla=iquark(1,ibp)                                              
        jbp=icomb(1,ibp)                                                
        iflb=iquark(2,jbp)                                              
        itra=itrip(ifla)                                                
        itrb=itrip(iflb)                                                
        if(itra*itrb.ne.10) then                                        
          call errex('strist 230:quarks not in color singlet')          
        end if                                                          
        if(iabs(ifla).gt.10) then                                       
          kf=jetzhd(1,ibp)                                              
          eml=bpli(1,ibp)                                               
        else if(iabs(iflb).gt.10) then                                  
          kf=jetzhd(2,jbp)                                              
          eml=bpli(2,jbp)                                               
        else                                                            
          call luifld(ifla,0,iflb,ifldum,kf)                            
          if(kf.ne.34.and.kf.ne.35)                                     
     a    eml=getmas(0,kf,1,-1.0)                                       
        end if                                                          
        if(kf.eq.0) call errex('strist:kf=0 230')                       
        if(kf.eq.25.and.ranf(0).gt.par(80)) then                        
          kf=124                                                        
          eml=getmas(0,kf,0,-1.0)                                       
        end if                                                          
        if(ida(51).ne.0) then                                           
          probch=0.6667*(sin(angmxv)-cos(angmxv)/sqrt(2.0))**2          
          if(kf.eq.34) then                                             
            if(ranf(0).lt.probch) kf=35                                 
          else if(kf.eq.35) then                                        
            if(ranf(0).lt.probch) kf=34                                 
          end if                                                        
        end if                                                          
        if(kf.eq.34.or.kf.eq.35)                                        
     a                 eml=getmas(0,kf,1,-1.0)                          
        nlund=nlund+1                                                   
        ivlc(1,nlund-nsave)=0                                           
        ipact(1,ibp)=nlund                                              
        if(iquark(2,ibp).eq.0) then                                     
          ipact(2,ibp)=0                                                
        end if                                                          
        ipact(2,jbp)=nlund                                              
        if(iquark(1,jbp).eq.0) then                                     
          ipact(1,jbp)=0                                                
        end if                                                          
        k(nlund,2)=kf                                                   
        plund(nlund,1)=ptr(1,1,ibp)+ptr(2,1,jbp)                        
        plund(nlund,2)=ptr(1,2,ibp)+ptr(2,2,jbp)                        
        plund(nlund,4)=0.5*(equark(1,ibp)+equark(2,jbp)+                
     a                      dquark(2,ibp)+dquark(1,jbp))                
        plund(nlund,3)=0.5*(equark(1,ibp)-equark(2,jbp)-                
     a                      dquark(2,ibp)+dquark(1,jbp))                
        plund(nlund,5)=eml                                              
        embum=embum+sqrt(eml*eml+plund(nlund,1)**2+plund(nlund,2)**2)   
        iposi=iposi+1                                                   
        xhad(4,iposi)=xlcmx(1,ibp)                                      
        xhad(3,iposi)=xlcmx(2,jbp)                                      
        if(mod(ida(16),2).eq.0) then                                    
          vf=0.5                                                        
        else                                                            
          vf=1.0                                                        
        end if                                                          
        if(lord(1,ibp).lt.0) then                                       
          vf=0.0                                                        
          if(lord(2,jbp).eq.-2) then                                    
            xhad(3,iposi)=xlcmx(2,ibp)                                  
          end if                                                        
        end if                                                          
        ppnl=plund(nlund,4)+plund(nlund,3)                              
        pmnl=plund(nlund,4)-plund(nlund,3)                              
        xhad(4,iposi)=xhad(4,iposi)-vf*ppnl                             
        xhad(3,iposi)=xhad(3,iposi)-vf*pmnl                             
        if(ibp.le.ibpfor) then                                          
          if(xhad(4,iposi).lt.xlcbp(1,jbp))                             
     a    xhad(4,iposi)=xlcbp(1,jbp)                                    
        else                                                            
          if(xhad(4,iposi).lt.                                          
     a        min(xlcbp(1,ibp),xlcbp(1,jbp))) then                      
            xhad(4,iposi)=min(xlcbp(1,ibp),xlcbp(1,jbp))                
          end if                                                        
        end if                                                          
        if(jbp.le.ibpfor) then                                          
          if(xhad(3,iposi).lt.xlcbp(2,ibp))                             
     a    xhad(3,iposi)=xlcbp(2,ibp)                                    
        else                                                            
          if(xhad(3,iposi).lt.                                          
     a        min(xlcbp(2,ibp),xlcbp(2,jbp))) then                      
            xhad(3,iposi)=min(xlcbp(2,ibp),xlcbp(2,jbp))                
          end if                                                        
        end if                                                          
        thad=0.5*(xhad(4,iposi)+xhad(3,iposi))*tprd                     
        zhad=0.5*(xhad(4,iposi)-xhad(3,iposi))*tprd                     
        xhad(4,iposi)=thad                                              
        xhad(3,iposi)=zhad                                              
        if(xhad(4,iposi).le.abs(xhad(3,iposi))) then                    
          write(8,*) 't,z=',thad,zhad                                   
          call errex(                                                   
     a          'strist502:produced hadron not in forward lc')          
        else                                                            
          dtinv2=sqrt(xhad(4,iposi)**2-xhad(3,iposi)**2)                
        end if                                                          
        ntri=0                                                          
  502   ntri=ntri+1                                                     
        if(ntri.gt.50) call errex('strist:ntri>50')                     
        fihad=pi2*ranf(0)                                               
        xdist=min(da(17),dtinv2)*sqrt(ranf(0))                          
        xhad(1,iposi)=xdist*cos(fihad)*tprd                             
        xhad(2,iposi)=xdist*sin(fihad)*tprd                             
        tinv2=xhad(4,iposi)**2                                          
        if(ntri.gt.30) then                                             
          write(8,*)'strist problem: xhad=',                            
     a                        (xhad(i,iposi),i=1,4)                     
        end if                                                          
        do 501 i=1,3                                                    
  501   tinv2=tinv2-xhad(i,iposi)**2                                    
        if(tinv2.lt.0.0) go to 502                                      
  230  continue                                                         
       if(embum.ge.emtrue) then                                         
         nlund=nsave                                                    
         go to 100                                                      
       end if                                                           
       ihad=kbreak+ibpfor/2                                             
       if(nlund-nsave.ne.ihad) then                                     
         write(8,*) 'ihad,kbreak,ibpfor',ihad,kbreak,ibpfor             
         call errex('strist:wrong no. of final hadrons')                
       end if                                                           
       nlmnsv=nlund-nsave                                               
       nlstrt=int(ranf(0)*(nlmnsv))                                     
       do 240  nll=nlstrt,nlstrt+nlmnsv-1                               
        nl=nsave+mod(nll,nlmnsv)+1                                      
        if(ivlc(1,nl-nsave).eq.1) go to 240                             
        eml=plund(nl,5)                                                 
        if(ranf(0).gt.0.5) then                                         
          jt1=1                                                         
          jt2=2                                                         
          ivzjt=1                                                       
        else                                                            
          jt1=2                                                         
          jt2=1                                                         
          ivzjt=-1                                                      
        end if                                                          
        do 239 jt=jt1,jt2,ivzjt                                         
         jr=3-jt                                                        
         ibp=0                                                          
         do 232 ibptst=1,ibptru                                         
          if(nl.eq.ipact(jt,ibptst)) then                               
            ibp=ibptst                                                  
            go to 233                                                   
          end if                                                        
  232    continue                                                       
  233    continue                                                       
         if(ibp.eq.0) then                                              
           call errex('strist 233:ibp=0')                               
         end if                                                         
         itry=0                                                         
  234    continue                                                       
         itry=itry+1                                                    
         nk=0                                                           
         jbpsto=0                                                       
         rmeasu=max(xlcrop(1)*xlcrop(2),xcutf*xcutf)                    
         jbstrt=int(ranf(0)*ibptru)                                     
         do 235 jbpp=jbstrt,jbstrt+ibptru-1                             
          jbp=mod(jbpp,ibptru)+1                                        
          if(lord(1,jbp).eq.0) go to 235                                
          if(lord(1,jbp).gt.lord(1,ibp).and.                            
     a        lord(2,jbp).gt.lord(2,ibp).and.                           
     b           ((ibp.gt.ibpfor.and.lord(1,ibp).gt.0).or.              
     c            (jbp.gt.ibpfor.and.lord(2,jbp).gt.0))                 
     d                                                 ) go to 235      
          do 236 ntr=1,itry-1                                           
  236     if(ivlc(2,ntr).eq.jbp) go to 235                              
          rmeas=(xlcbp(1,ibp)-xlcbp(1,jbp))*                            
     a                (xlcbp(2,jbp)-xlcbp(2,ibp))                       
          if(rmeas.lt.rmeasu) then                                      
            if(jbp.ne.icomb(jt,ibp).and.                                
     a                rmeas.gt.xcutf**2) go to 235                      
            if(iquark(jt,jbp).eq.0.or.                                  
     a                     lord(jt,jbp).lt.0) then                      
               nact=ipact(jr,jbp)                                       
            else if(iquark(jr,jbp).eq.0.or.                             
     a                     lord(jr,jbp).lt.0) then                      
               nact=ipact(jt,jbp)                                       
            elseif(jbp.eq.icomb(jt,ibp))then                            
               nact=ipact(jt,jbp)                                       
            else                                                        
              if(lord(jt,jbp).gt.lord(jt,ibp)) then                     
                nact=ipact(jr,jbp)                                      
              else                                                      
                nact=ipact(jt,jbp)                                      
              end if                                                    
            end if                                                      
            if(nact.eq.0) go to 235                                     
            if(nact.eq.nl) go to 235                                    
            rmeasu=rmeas                                                
            nk=nact                                                     
            jbpsto=jbp                                                  
          end if                                                        
  235    continue                                                       
         if(nk.eq.0) then                                               
           if(jt.eq.jt1) then                                           
             go to 239                                                  
           else                                                         
             nlund=nsave                                                
             go to 100                                                  
           end if                                                       
         end if                                                         
         ivlc(2,itry)=jbpsto                                            
         emk=plund(nk,5)                                                
         do 237 i=1,3                                                   
          p1(i)=plund(nl,i)                                             
          p2(i)=plund(nk,i)                                             
  237    continue                                                       
         p1(0)=plund(nl,4)                                              
         p2(0)=plund(nk,4)                                              
         pplav=p1(0)+p1(3)+p2(0)+p2(3)                                  
         pmiav=p1(0)-p1(3)+p2(0)-p2(3)                                  
         emb1=sqrt(eml**2+p1(1)**2+p1(2)**2)                            
         emb2=sqrt(emk**2+p2(1)**2+p2(2)**2)                            
         nkk=nk-nsave-1                                                 
         if(nkk.lt.nlstrt) nkk=nkk+nlmnsv                               
         if((emb1+emb2)**2.ge.pplav*pmiav) then                         
           plsq=p1(0)**2-p1(3)**2                                       
           pksq=p2(0)**2-p2(3)**2                                       
           if(emb1**2.gt.pplav*pmiav) then                              
             isccss=0                                                   
           else if(plsq.gt.emb1**2) then                                
             isccss=-1                                                  
             emb2=sqrt(pksq)                                            
           else if(nkk.gt.nll) then                                     
             ivlc(1,nk-nsave)=0                                         
             isccss=-1                                                  
             emb2=sqrt(pplav*pmiav)-emb1-1.e-3                          
           else                                                         
             isccss=0                                                   
           end if                                                       
           if(emb2.lt.1.e-3) isccss=0                                   
         else                                                           
           isccss=1                                                     
         end if                                                         
         if(iabs(isccss).eq.1) then                                     
           if(p1(0)+p1(3).gt.p2(0)+p2(3).and.                           
     a             p1(0)-p1(3).lt.p2(0)-p2(3))  then                    
             jdir=1                                                     
           else if(p1(0)+p1(3).lt.p2(0)+p2(3).and.                      
     a             p1(0)-p1(3).gt.p2(0)-p2(3))  then                    
             jdir=2                                                     
           else                                                         
             if(ranf(0).lt.0.5) then                                    
               jdir=1                                                   
             else                                                       
               jdir=2                                                   
             end if                                                     
           end if                                                       
           call newlcp(pplav,pmiav,emb1,emb2,                           
     a                          p1pl,p1mi,p2pl,p2mi,jdir)               
           p1(0)=0.5*(p1pl+p1mi)                                        
           p1(3)=0.5*(p1pl-p1mi)                                        
           p2(0)=0.5*(p2pl+p2mi)                                        
           p2(3)=0.5*(p2pl-p2mi)                                        
         end if                                                         
         if(iabs(isccss).ne.1) then                                     
           go to 234                                                    
         else                                                           
           ivlc(1,nl-nsave)=1                                           
           if(isccss.eq.1) ivlc(1,nk-nsave)=1                           
           do 238 i=1,3                                                 
            plund(nl,i)=p1(i)                                           
            plund(nk,i)=p2(i)                                           
  238      continue                                                     
           plund(nl,4)=p1(0)                                            
           plund(nk,4)=p2(0)                                            
           go to 240                                                    
         end if                                                         
  239   continue                                                        
  240  continue                                                         
       if(nsingl.gt.0)                                                  
     a   write(8,*) 'singlets involved in decay',nsingl                 
       jchrgs=0                                                         
       jbarys=0                                                         
       jstras=0                                                         
       do 241  nl=nsave+1,nlund                                         
        kf=isign(1,k(nl,2))*mod(iabs(k(nl,2)),100)                      
        call luiflv(kf,ifla1,iflb1,iflc1,ksp)                           
        if(iabs(ifla1).eq.3) jstras=jstras+isign(1,ifla1)               
        if(iabs(iflb1).eq.3) jstras=jstras+isign(1,iflb1)               
        if(iabs(iflc1).eq.3) jstras=jstras+isign(1,iflc1)               
        jchrgs=jchrgs+luchge(kf)                                        
        k2labs=iabs(kf)                                                 
        if(k2labs.le.90) then                                           
           if(kspin(k2labs).ge.2) then                                  
             jbarys=jbarys+3*isign(1,kf)                                
           endif                                                        
        else                                                            
           call errex('strist:unidentified hadron')                     
        endif                                                           
  241  continue                                                         
       if(jstras.ne.kstran.or.                                          
     a        jchrgs.ne.kcharg.or.                                      
     b            jbarys.ne.kbaryo) then                                
          write(8,*) 'charge:',jchrgs,kcharg                            
          write(8,*) 'baryno:',jbarys,kbaryo                            
          write(8,*) 'strange',jstras,kstran                            
          call errex('strist 240:no quantum no. conservation')          
       end if                                                           
       mst(1)=nsave+1                                                   
       mst(2)=nlund                                                     
       call lurobo(thetlu,philu,bex,bey,bez)                            
       do 248 i=0,3                                                     
  248  ptot(i)=0.0                                                      
       do 250  nl=nsave+1,nlund                                         
        do 249 i=1,3                                                    
  249   ptot(i)=ptot(i)+plund(nl,i)                                     
        ptot(0)=ptot(0)+plund(nl,4)                                     
  250  continue                                                         
       if((abs(ptot(0)-psum(0)).gt.0.005).or.                           
     a        (abs(ptot(1)-psum(1)).gt.0.005).or.                       
     b            (abs(ptot(2)-psum(2)).gt.0.005).or.                   
     c               (abs(ptot(3)-psum(3)).gt.0.005)) then              
        write(8,*) 'psum=',psum                                         
        write(8,*) 'ptot=',ptot                                         
        call errex('strist:250 no energy/mom. conservation')            
       end if                                                           
       do 251 iro=1,iroout                                              
        lstar(iro)=kstar(iro)                                           
        lende(iro)=kende(iro)                                           
  251  continue                                                         
       nbsrop=0                                                         
       nbqrop=0                                                         
       do 272 ibp=ibpfor+1,ibptru                                       
         if(lord(1,ibp).le.0) go to 272                                 
         if(icomb(1,ibp).eq.0) go to 272                                
         ifl=iquark(1,ibp)                                              
         if(iabs(ifl).le.2) then                                        
          nbqrop=nbqrop+1                                               
          ipa=ipiq(ibp)/100                                             
          iqa=mod(ipiq(ibp),100)                                        
          if(ipa.gt.iqa) then                                           
            ientry=min(iprbr2,ipa*(ipa+1)/2+iqa)                        
          else                                                          
            ientry=min(iprbr2,iqa*(iqa+1)/2+ipa)                        
          end if                                                        
          prbt                                                          
     a       =2.*prbrop(1,ientry)+prbrop(2,ientry)+2.*prbrop(3,ientry)  
     a         +prbrop(4,ientry)+2.*prbrop(5,ientry)+prbrop(6,ientry)+  
     b                        4.*prbrop(7,ientry)+2.*prbrop(8,ientry)+  
     c                        4.*prbrop(9,ientry)+2.*prbrop(10,ientry)  
          ssupp=ssupp+(prbrop(2,ientry)+prbrop(4,ientry))/prbt          
          qsupp=qsupp+(prbrop(1,ientry)+prbrop(3,ientry))/prbt          
          nsupp=nsupp+1                                                 
         else if(iabs(ifl).eq.3) then                                   
          nbsrop=nbsrop+1                                               
          ipa=ipiq(ibp)/100                                             
          iqa=mod(ipiq(ibp),100)                                        
          if(ipa.gt.iqa) then                                           
            ientry=min(iprbr2,ipa*(ipa+1)/2+iqa)                        
          else                                                          
            ientry=min(iprbr2,iqa*(iqa+1)/2+ipa)                        
          end if                                                        
          prbt                                                          
     a       =2.*prbrop(1,ientry)+prbrop(2,ientry)+2.*prbrop(3,ientry)  
     a         +prbrop(4,ientry)+2.*prbrop(5,ientry)+prbrop(6,ientry)+  
     b                        4.*prbrop(7,ientry)+2.*prbrop(8,ientry)+  
     c                        4.*prbrop(9,ientry)+2.*prbrop(10,ientry)  
          ssupp=ssupp+(prbrop(2,ientry)+prbrop(4,ientry))/prbt          
          qsupp=qsupp+(prbrop(1,ientry)+prbrop(3,ientry))/prbt          
          nsupp=nsupp+1                                                 
        end if                                                          
  272  continue                                                         
       if(nlund-nsave.ne.iposi) then                                    
          write(8,*) nlund-nsave,iposi                                  
          call errex('strist:nlund-nsave.ne.iposi')                     
       end if                                                           
       if(nlund+iposi.gt.2000) then                                     
          write(8,*) nlund,nsave,iposi                                  
          call errex('strist:nlund+iposi>2000')                         
       end if                                                           
       do 280 nlu=nlund+1,nlund+iposi                                   
        nl=nlu-nlund                                                    
        do 271 i=1,4                                                    
  271   plund(nlu,i)=xhad(i,nl)                                         
  280  continue                                                         
       mst(1)=nlund+1                                                   
       nlund=nlund+iposi                                                
       mst(2)=nlund                                                     
       call lurobo(thetlu,philu,bex,bey,bez)                            
       px=0.0                                                           
       py=0.0                                                           
       pz=0.0                                                           
       kaprod=0                                                         
       do 290 nlu=nlund-iposi+1,nlund                                   
        nl=nlu-iposi                                                    
        kf=k(nl,2)                                                      
        if(iabs(kf).eq.18.or.iabs(kf).eq.28.or.                         
     a        iabs(kf).eq.19.or.iabs(kf).eq.29) then                    
             kaprod=kaprod+1                                            
        end if                                                          
        eml=plund(nl,5)                                                 
        twait=0.0                                                       
        do 281 i=1,3                                                    
  281   plund(nlu,i)=plund(nlu,i)+xbang(i)+twait*plund(nl,i)/eml        
        plund(nlu,4)=plund(nlu,4)+xbang(0)+twait*plund(nl,4)/eml        
  290  continue                                                         
       nhfina=iposi                                                     
       write(8,*) 'nhstri,nhfina=',nhstri,nhfina                        
       write(8,*) 'kadel,kaprod=',kadel,kaprod                          
       write(8,412) nhstri,kadel,nbqstr,nbsstr,                         
     a                  nhfina,kaprod,nbqrop,nbsrop                     
       if(nsupp.gt.0)                                                   
     a write(8,*) 'ssupp=',ssupp/qsupp,nsupp                            
  412  format(4x,9(i4,1x))                                              
       write(8,*) 'strist run successfully finished'                    
      return                                                            
      end                                                               
