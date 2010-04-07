CPAT,ROPE     .                                                         
      subroutine tube                                                   
      include 'scatter7'                                                
      real*4 plund,par                                                  
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      real*4 bex,bey,bez                                                
      parameter (mxs=200)                                               
      common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout                
      common /rop2bk/  kparnt(3,mxs),                                   
     a    xori(0:3,mxs),pqu(0:3,mxs),pqb(0:3,mxs)                       
       logical manip                                                    
       common /rop3bk/manip(mxs)                                        
      real*4  atrmax,tnsmax,dndx4m,emqcns,emq,ems,prbrop                
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      common /dropbk/ distnk,xsprta,kprta                               
      include 'partblk7'                                                
      dimension xactl(0:3),veloc(0:3)                                   
      dimension pql(0:3),pqbarl(0:3)                                    
      dimension ptotl(0:3),ptot(0:3)                                    
      common /prinbk/ iprin                                             
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      logical nghbor                                                    
      data vernum/'version  rqmd 2.4 -- 30-jan-98'/                     
      data pronam/'module: t u b e '/                                   
      data ttvers/.true./                                               
      if(ttvers) then                                                   
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
        istrpr=0                                                        
        if(iz1old.ge.0) then                                            
          imespr=0                                                      
        else                                                            
          kfl=ityppr                                                    
          call luiflv(kfl,ifla,iflb,iflc,ksp)                           
          if(iabs(ifla).eq.3) istrpr=istrpr+1                           
          if(iabs(iflb).eq.3) istrpr=istrpr+1                           
          if(ksp.ge.2) then                                             
            imespr=0                                                    
            if(iabs(iflc).eq.3) istrpr=istrpr+1                         
          else                                                          
            imespr=1                                                    
          end if                                                        
        end if                                                          
        xsprta=sigasy(1,imespr,istrpr,0,0)-                             
     a                 sigasy(0,imespr,istrpr,0,0)                      
      end if                                                            
      if(iroin.eq.0) return                                             
      iroin=1                                                           
      l1b=lstar(1)                                                      
      l1e=lende(1)                                                      
        kansig=0                                                        
        mb=l1b                                                          
        me=l1e                                                          
        assign 50 to label                                              
        go to 901                                                       
   50   continue                                                        
        if(ignore.eq.1)  go to 69                                       
        do 51 i=0,3                                                     
         xori(i,1)=xactl(i)                                             
         pqb(i,1)=pqbarl(i)                                             
         pqu(i,1)=pql(i)                                                
   51   continue                                                        
        jdente=issn(l1e)                                                
        npar1=itdth(3,jdente)                                           
        npar2=npar1/100000                                              
        npar1=mod(npar1,100000)                                         
        if(npar2.eq.0) then                                             
          write(8,*)  jdente,itdth(1,jdente),npar1,npar2                
          call errex('tube:npar2=0')                                    
        end if                                                          
        kparnt(1,1)=npar1                                               
        kparnt(2,1)=npar2                                               
        if(zlast(issn(l1b)).ne.0.0) then                                
          kansig=1                                                      
          if(ida(15).ne.0) kansig=0                                     
          if(l1e-l1b.ne.1) kansig=0                                     
          kparnt(3,1)=40                                                
          xtrans=zlast(issn(l1b))/da(17)                                
          if(xtrans.lt.0.0) xtrans=0.0                                  
        else                                                            
          kparnt(3,1)=0                                                 
          xtrans=1.0                                                    
        end if                                                          
        if(ida(47).eq.0) go to 69                                       
        nmin=iofset+nrbar(kclst)+1                                      
        nmax=iofset+nrbar(kclst)+nrmes(kclst)                           
        kmin=0                                                          
        m=nmin-1                                                        
   56   continue                                                        
        m=m+1                                                           
         if(m.gt.nmax) go to 69                                         
         if(m.ge.l1b.and.m.le.l1e) go to 56                             
         if(m.le.kmin) go to 56                                         
         kmin=m                                                         
         identm=issn(m)                                                 
         if(identm.le.0.or.identm.gt.ntotma) then                       
           write(8,*) m,                                                
     a      'identm,issn(m),iofset,nrbar(kclst),nrmes(kclst)'           
           write(8,*) identm,issn(m),iofset,nrbar(kclst),nrmes(kclst)   
           call errex('tube:no particle at position m????')             
         end if                                                         
         if(itdth(2,identm).eq.0  .and.                                 
     a                        itdth(1,identm).ne.0  ) then              
           ktdt1=iabs(itdth(1,identm))                                  
           if(nprtcl.ge.100000) then                                    
             call errex('tube:nprtcl>100000,itdth-storage overflow')    
           end if                                                       
           mpar1=itdth(3,ktdt1)                                         
           mpar2=mpar1/100000                                           
           if(mpar2.eq.0) then                                          
             write(8,*) m, identm,itdth(1,identm),mpar1,mpar2           
             call errex('tube:mpar2=0')                                 
           end if                                                       
           mpar1=mod(mpar1,100000)                                      
           if(itdth(1,identm).gt.0) then                                
             ident2=min(ntotma,max(1,issn(min(nmax,m+1))))              
             if(ktdt1.ne.itdth(1,ident2)) then                          
                write(8,*) 'kmin=',kmin                                 
                write(8,*) identm,m,(itdth(i,identm),i=1,2)             
                write(8,*) issn(m+1),m+1,                               
     a                             (itdth(i,issn(m+1)),i=1,2)           
                write(8,*) ident2,(itdth(i,ident2),i=1,2)               
                write(8,*) 'tube56: string particle without partner?'   
                go to 56                                                
             end if                                                     
             kstart=m                                                   
             kend=m+1                                                   
             do 403 ik=kstart+2,nmax                                    
              identk=issn(ik)                                           
              if(ktdt1.ne.itdth(1,identk)) go to 404                    
              if(itdth(2,identk).ne.0) go to 404                        
              kend=ik                                                   
  403        continue                                                   
  404        continue                                                   
           else                                                         
             kstart=m                                                   
             kend=m                                                     
           end if                                                       
           kmin=kend                                                    
           iron=1                                                       
           mb=kstart                                                    
           me=kend                                                      
           assign 52 to label                                           
           go to 901                                                    
   52      continue                                                     
           if(ignore.eq.1) go to 56                                     
           irok=iroin+1                                                 
           if(irok.gt.mxs) then                                         
               write(8,*) 'tube56:irok>mxs'                             
               go to 56                                                 
           end if                                                       
           do 53 i=0,3                                                  
            xori(i,irok)=xactl(i)                                       
            pqb(i,irok)=pqbarl(i)                                       
            pqu(i,irok)=pql(i)                                          
   53      continue                                                     
           if(kend.gt.kstart) then                                      
             if(l1b.eq.l1e) then                                        
               jsingl=1                                                 
             else                                                       
               jsingl=0                                                 
             end if                                                     
           else                                                         
             if(kstart.eq.kend) then                                    
               jsingl=0                                                 
             else                                                       
               jsingl=2                                                 
             end if                                                     
           end if                                                       
           kprta=0                                                      
           if(max(npar1,npar2).le.n1old+n2old) then                     
             if(max(mpar1,mpar2).le.n1old+n2old) then                   
               if(npar1.eq.mpar1.or.npar1.eq.mpar2.or.                  
     a            npar2.eq.mpar1.or.npar2.eq.mpar2) then                
                 kprta=1                                                
               end if                                                   
             end if                                                     
           end if                                                       
           if(nghbor(iron,irok,jsingl)) then                            
             if(zlast(issn(kstart)).ne.0.0) then                        
               xtrtst=zlast(issn(kstart))/da(17)                        
               if(xtrtst.lt.0.0) xtrtst=0.0                             
               xtrtst=min(xtrans,xtrtst)                                
             else                                                       
               xtrtst=xtrans                                            
             end if                                                     
             if(xtrtst.le.0.0) then                                     
               write(8,*) "xtrtst,xtrans,zlast(issn(l1b))",             
     a                          "zlast(issn(kstart))"                   
               write(8,*) xtrtst,xtrans,zlast(issn(l1b)),               
     a                          zlast(issn(kstart))                     
               call errex("tube:neg. xtrtst")                           
             end if                                                     
             if(xtrtst.eq.1.0.or.ranf(0).lt.xtrtst*xtrtst) then         
                                                                        
               iroin=iroin+1                                            
               lstar(iroin)=kstart                                      
               lende(iroin)=kend                                        
               kparnt(1,iroin)=mpar1                                    
               kparnt(2,iroin)=mpar2                                    
               if(zlast(issn(kstart)).ne.0.0) then                      
                 kparnt(3,iroin)=40                                     
               else                                                     
                 kparnt(3,iroin)=0                                      
               end if                                                   
             end if                                                     
           end if                                                       
         end if                                                         
         if(m.lt.nmax) go to 56                                         
   69 continue                                                          
       iroout=iroin                                                     
       nsave=nlund                                                      
       call strist                                                      
       if(kansig.eq.1.and.nlund.eq.nsave) call sigkan                   
       call intfac(nsave)                                               
       nlund=nsave                                                      
      return                                                            
  901 continue                                                          
          identb=issn(mb)                                               
          idente=issn(me)                                               
          if(qdot(0,mb).gt.1.e-20) then                                 
           do 529 i=0,3                                                 
  529      veloc(i)=qdot(i,mb)/qdot(0,mb)                               
          else                                                          
           do 530 i=0,3                                                 
  530      veloc(i)=0.0                                                 
          end if                                                        
          ignore=0                                                      
          if(me.gt.mb) then                                             
            isingl=0                                                    
          else                                                          
            isingl=1                                                    
          end if                                                        
          do 424 i=0,3                                                  
  424     xactl(i)=frfp(i,identb)                                       
          if(isingl.eq.0) then                                          
            do 427 i=0,3                                                
             pql(i)=frfp(i,idente)                                      
  427       continue                                                    
            if(pql(0).lt.0.0) pql(0)=-pql(0)                            
          else                                                          
            nsave=nlund                                                 
            nlund=nlund+1                                               
            if(nlund.gt.2000)                                           
     a              call errex('tube:nlund>2000')                       
            mst(1)=nlund                                                
            mst(2)=nlund                                                
            bex=-p(1,mb)/p(0,mb)                                        
            bey=-p(2,mb)/p(0,mb)                                        
            bez=-p(3,mb)/p(0,mb)                                        
            emmb=p(0,mb)**2                                             
            do 422 i=1,3                                                
             emmb=emmb-p(i,mb)**2                                       
             plund(nlund,i)=x(i,mb)+veloc(i)*                           
     a                       (tlast(mb)-x(0,mb))-                       
     b                                    frfp(i,identb)                
  422       continue                                                    
            plund(nlund,4)=x(0,mb)+veloc(0)*                            
     a                       (tlast(mb)-x(0,mb))-                       
     b                                    frfp(0,identb)                
            call lurobo(0.,0.,bex,bey,bez)                              
            pabs=sqrt(plund(nlund,1)*plund(nlund,1)+                    
     a                  plund(nlund,2)*plund(nlund,2)+                  
     b                          plund(nlund,3)*plund(nlund,3))          
            thet=acos(max(-cvalu1,min(cvalu1,plund(nlund,3)/pabs)))     
            if(plund(nlund,2).eq.cvalu0.and.                            
     a                              plund(nlund,1).eq.cvalu0) then      
               phi=cvalu0                                               
            else                                                        
               phi=atan2(plund(nlund,2),plund(nlund,1))                 
            end if                                                      
            if(emmb.le.0.0) then                                        
              call errex('tube:emmb**2<0')                              
            else                                                        
              emmb=sqrt(emmb)                                           
            end if                                                      
            plund(nlund,4)=0.5*emmb                                     
            plund(nlund,3)=plund(nlund,4)*cos(thet)                     
            plund(nlund,1)=plund(nlund,4)*sin(thet)*cos(phi)            
            plund(nlund,2)=plund(nlund,4)*sin(thet)*sin(phi)            
            call lurobo(0.,0.,-bex,-bey,-bez)                           
            do 426 i=1,3                                                
  426       pql(i)=plund(nlund,i)                                       
            pql(0)=plund(nlund,4)                                       
            nlund=nsave                                                 
          end if                                                        
          do 429 i=0,3                                                  
           ptotl(i)=0.                                                  
  429     continue                                                      
          do 430 il=mb,me                                               
           do 431 i=0,3                                                 
  431      ptotl(i)=ptotl(i)+p(i,il)                                    
  430     continue                                                      
          do 432 i=0,3                                                  
  432     pqbarl(i)=ptotl(i)-pql(i)                                     
          nsave=nlund                                                   
          nlund=nlund+2                                                 
          if(nlund.gt.2000)                                             
     a              call errex('tube:nlund>2000')                       
          mst(1)=nsave+1                                                
          mst(2)=nlund                                                  
          bex=-ptotl(1)/ptotl(0)                                        
          bey=-ptotl(2)/ptotl(0)                                        
          bez=-ptotl(3)/ptotl(0)                                        
          do 440 i=1,3                                                  
           plund(nsave+1,i)=pql(i)                                      
           plund(nlund,i)=pqbarl(i)                                     
  440     continue                                                      
          plund(nsave+1,4)=pql(0)                                       
          plund(nlund,4)=pqbarl(0)                                      
          call lurobo(0.,0.,bex,bey,bez)                                
          pqpqb=0.                                                      
          pqsq=0.                                                       
          do 433 i=1,3                                                  
           pqpqb=pqpqb+plund(nlund,i)*plund(nsave+1,i)                  
           pqsq=pqsq+plund(nsave+1,i)*plund(nsave+1,i)                  
  433     continue                                                      
          plund(nlund,4)=abs(pqpqb)/sqrt(pqsq)                          
          do 434 i=1,3                                                  
  434     plund(nlund,i)=pqpqb/pqsq*plund(nsave+1,i)                    
          call lurobo(0.,0.,-bex,-bey,-bez)                             
          do 446 i=1,3                                                  
           pqbarl(i)=plund(nlund,i)                                     
  446     continue                                                      
          pqbarl(0)=plund(nlund,4)                                      
          nlund=nsave                                                   
          if(pqpqb.ge.0.0) then                                         
            ignore=1                                                    
            go to label                                                 
          end if                                                        
          emqsq=pql(0)**2                                               
          emqbsq=pqbarl(0)**2                                           
          do 435 i=1,3                                                  
           emqsq=emqsq-pql(i)**2                                        
           emqbsq=emqbsq-pqbarl(i)**2                                   
  435     continue                                                      
          if(abs(emqsq).gt.0.004.or.                                    
     a                  abs(emqbsq).gt.0.004) then                      
            write(8,*)                                                  
     a            'tube:wrong quark momenta'                            
            write(8,*) 'tube mb/me=',mb,me,issn(mb),issn(me)            
            write(8,*) 'pqbarl=',(pqbarl(i),i=0,3)                      
            write(8,*) 'pql=',(pql(i),i=0,3)                            
            write(8,*) emqsq,emqbsq                                     
            ignore=1                                                    
          end if                                                        
          go to label                                                   
      end                                                               
      logical function nghbor(iron,irok,jsingl)                         
       include 'scatter7'                                               
       real*4 plund,par                                                 
       common /lujets/ nlund,k(2000,2),plund(2000,5)                    
       common/ludat1/mst(40),par(80)                                    
       real*4 bex,bey,bez,thetlu,philu                                  
       parameter (mxs=200)                                              
       common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout               
       common /rop2bk/ kparnt(3,mxs),                                   
     a    xori(0:3,mxs),pqu(0:3,mxs),pqb(0:3,mxs)                       
       common /dropbk/ distnk,xsprta,kprta                              
      common /prinbk/ iprin                                             
       dimension ptot(0:3),ptotn(0:3)                                   
       dimension x1(3),x2(3)                                            
       nghbor=.false.                                                   
       if(max(iron,irok).gt.iroin+1) then                               
         call errex(                                                    
     a    'nghbor:max(iron,irok).gt.iroin+1')                           
       else if(min(iron,irok).le.0) then                                
         call errex(                                                    
     a    'nghbor:min(iron,irok).le.0')                                 
       end if                                                           
       do 401 i=0,3                                                     
  401  ptot(i)=pqu(i,irok)+pqb(i,irok)                                  
       do 402 i=0,3                                                     
  402  ptotn(i)=pqu(i,iron)+pqb(i,iron)                                 
       emksq=ptot(0)**2                                                 
       do 403 i=1,3                                                     
  403  emksq=emksq-ptot(i)**2                                           
       emnsq=ptotn(0)**2                                                
       do 404 i=1,3                                                     
  404  emnsq=emnsq-ptotn(i)**2                                          
       if(jsingl.eq.0) then                                             
         if(emksq.gt.emnsq) then                                        
           jrok=irok                                                    
           jron=iron                                                    
         else                                                           
           jrok=iron                                                    
           jron=irok                                                    
           do 405 i=0,3                                                 
  405      ptot(i)=ptotn(i)                                             
         end if                                                         
       elseif(jsingl.eq.1) then                                         
           jrok=irok                                                    
           jron=iron                                                    
       else                                                             
           jrok=iron                                                    
           jron=irok                                                    
           do 406 i=0,3                                                 
  406      ptot(i)=ptotn(i)                                             
       end if                                                           
       p0tot=ptot(0)                                                    
       bxnew=ptot(1)/p0tot                                              
       bynew=ptot(2)/p0tot                                              
       bznew=ptot(3)/p0tot                                              
       b2=bxnew**2+bynew**2+bznew**2                                    
       if(b2.ge.1.0) then                                               
         return                                                         
       end if                                                           
       nsave=nlund                                                      
       nlund=nsave+4                                                    
       plund(nsave+1,4)=pqu(0,jrok)                                     
       plund(nsave+2,4)=pqu(0,jron)*tprd                                
       plund(nsave+3,4)=pqb(0,jron)*tprd                                
       plund(nsave+4,4)=xori(0,jron)-xori(0,jrok)                       
       do 410 i=1,3                                                     
        plund(nsave+1,i)=pqu(i,jrok)                                    
        plund(nsave+2,i)=pqu(i,jron)*tprd                               
        plund(nsave+3,i)=pqb(i,jron)*tprd                               
        plund(nsave+4,i)=xori(i,jron)-xori(i,jrok)                      
  410  continue                                                         
       mst(1)=nsave+1                                                   
       mst(2)=nsave+4                                                   
       bex=-bxnew                                                       
       bey=-bynew                                                       
       bez=-bznew                                                       
       call lurobo(0.,0.,bex,bey,bez)                                   
       pabs=sqrt(plund(nsave+1,1)*plund(nsave+1,1)+                     
     a                  plund(nsave+1,2)*plund(nsave+1,2)+              
     b                          plund(nsave+1,3)*plund(nsave+1,3))      
       thet=acos(max(-cvalu1,min(cvalu1,plund(nsave+1,3)/pabs)))        
       if(plund(nsave+1,2).eq.0.0.and.                                  
     a                              plund(nsave+1,1).eq.0.0) then       
            phi=cvalu0                                                  
       else                                                             
            phi=atan2(plund(nsave+1,2),plund(nsave+1,1))                
       end if                                                           
       thetlu=-thet                                                     
       philu=-phi                                                       
       mst(1)=nsave+2                                                   
       mst(2)=nsave+4                                                   
       call lurobo(0.,philu,0.,0.,0.)                                   
       call lurobo(thetlu,0.,0.,0.,0.)                                  
       e2sq=0.                                                          
       e1e2=0.                                                          
       do 907 i=1,2                                                     
        x1(i)=plund(nsave+4,i)+plund(nsave+3,i)                         
        x2(i)=plund(nsave+2,i)-plund(nsave+3,i)                         
        e2sq=e2sq+x2(i)*x2(i)                                           
        e1e2=e1e2+x1(i)*x2(i)                                           
  907  continue                                                         
       if(e2sq.gt.1.e-10) then                                          
         smin=max(cvalu0,min(cvalu1,-e1e2/e2sq))                        
       else                                                             
         smin=0.                                                        
       end if                                                           
       e2sq=0.                                                          
       do 908 i=1,2                                                     
  908  e2sq=e2sq+(x1(i)+x2(i)*smin)**2                                  
       distnk=sqrt(e2sq)                                                
       radtr=da(17)                                                     
       if(kprta.eq.1) then                                              
         if(ranf(0).gt.10.*pi*radtr**2/xsprta) then                     
           go to 920                                                    
         end if                                                         
       else                                                             
         if(e2sq.gt.radtr**2) go to 920                                 
       end if                                                           
       x0=plund(nsave+4,4)                                              
       x3=plund(nsave+4,3)                                              
       xplu0=x0+x3                                                      
       xminu0=x0-x3                                                     
       x0=plund(nsave+4,4)+plund(nsave+3,4)+plund(nsave+2,4)            
       x3=plund(nsave+4,3)+plund(nsave+3,3)+plund(nsave+2,3)            
       xplus=x0+x3                                                      
       xminus=x0-x3                                                     
       if(xplu0.gt.0.) then                                             
         if(xminu0.gt.0.) then                                          
           gamk=1./sqrt(1.-bxnew**2-bynew**2-bznew**2)                  
           emk=p0tot/gamk*tprd                                          
           if(sqrt(2.)*max(xplu0,xminu0).gt.emk) go to 920              
         else                                                           
           if(xplus.gt.0..and.xminus.lt.0.) go to 920                   
         end if                                                         
       else                                                             
         if(xminu0.gt.0.) then                                          
           if(xplus.lt.0..and.xminus.gt.0.) go to 920                   
         else                                                           
           if(xplus.lt.0..or.xminus.lt.0.)go to 920                     
         end if                                                         
       end if                                                           
       nghbor=.true.                                                    
  920  continue                                                         
       nlund=nsave                                                      
      return                                                            
      end                                                               
      subroutine bplist(ipmx,iqmx,emtube,emtrue,atrel)                  
      parameter (mxs=200)                                               
      parameter (kbpmax=750)                                            
      common /bplibk/ libpmx,bpli(2,kbpmax)                             
      common /ropblk/                                                   
     a                ivlc(2,kbpmax),lord(2,kbpmax),                    
     b                ipact(2,kbpmax),iqact(2,kbpmax),ipiq(kbpmax),     
     c                iquark(2,kbpmax),icomb(2,kbpmax),                 
     d                xlcbp(2,kbpmax),xlcmx(2,kbpmax),                  
     e                equark(2,kbpmax),ptr(2,2,kbpmax)                  
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      data  atbas / 1.84/                                               
       ipm=ipmx                                                         
       iqm=iqmx                                                         
       libmx=min(nint((ipm+iqm)*.25*emtrue/sqrt(emqcns**2)+5.),         
     a                                            kbpmax-mxs*2)         
       if(ipm.gt.16) then                                               
          write(8,*) ipm,'too large field strength in bplist'           
          ipm=16                                                        
       end if                                                           
       if(iqm.gt.16) then                                               
          write(8,*) iqm,'too large field strength in bplist'           
          iqm=16                                                        
       end if                                                           
       if(ipm.lt.iqm) then                                              
          ipmax=iqm                                                     
          iqmax=ipm                                                     
       else                                                             
          ipmax=ipm                                                     
          iqmax=iqm                                                     
       end if                                                           
       libpmx=0                                                         
       ientry=ipmax*(ipmax+1)/2+iqmax                                   
       dndx4m=2.*prbrop(1,ientry)+prbrop(2,ientry)+2.*prbrop(3,ientry)  
     a         +prbrop(4,ientry)+2.*prbrop(5,ientry)+prbrop(6,ientry)+  
     b                        4.*prbrop(7,ientry)+2.*prbrop(8,ientry)+  
     c                        4.*prbrop(9,ientry)+2.*prbrop(10,ientry)  
       wid=dndx4m*atrel*atbas*emtube/tnsmax                             
       atrmax=atrel                                                     
       time=0.0                                                         
   10  continue                                                         
       tdel=-log(max(ranf(0),1.e-10))/wid                               
       time=time+tdel                                                   
       period=emtube/tnsmax                                             
       if(time.gt.period) then                                          
         go to 50                                                       
       else if(time.le.0.5*period) then                                 
         extens=2.*time                                                 
       else                                                             
         extens=2.*(period-time)                                        
       end if                                                           
       zrand=ranf(0)                                                    
       if(zrand.lt.extens/period) then                                  
         z=period*zrand-0.5*extens                                      
       else                                                             
         go to 10                                                       
       end if                                                           
       if(libpmx+1.gt.kbpmax) then                                      
          write(8,*) 'libpmx=',libpmx,kbpmax                            
          call errex('kbpmax array too small in bplist,abort')          
          go to 50                                                      
       end if                                                           
       libpmx=libpmx+1                                                  
       equark(1,libpmx)=time+z                                          
       equark(2,libpmx)=(time-z)*equark(1,libpmx)                       
       if(libpmx.eq.libmx) go to 50                                     
       go to 10                                                         
   50  continue                                                         
       if(libpmx.eq.0) return                                           
       do 60 ibp=1,libpmx                                               
        lord(1,ibp)=1                                                   
        do 59 jbp=1,ibp-1                                               
         if(equark(2,ibp).gt.equark(2,jbp))then                         
          lord(1,ibp)=lord(1,ibp)+1                                     
         else                                                           
          lord(1,jbp)=lord(1,jbp)+1                                     
         end if                                                         
   59   continue                                                        
   60  continue                                                         
       do 81 ibp=1,libpmx                                               
        lord(2,ibp)=lord(1,ibp)                                         
   81  continue                                                         
       isum=0                                                           
       do 79 ibp=1,libpmx                                               
        i=lord(2,ibp)                                                   
        isum=isum+i                                                     
        if(i.lt.1.or.i.gt.libpmx) then                                  
           call errex('bplist:ord problem')                             
        end if                                                          
        lord(1,i)=ibp                                                   
   79  continue                                                         
       if(isum.ne.libpmx*(libpmx+1)/2) then                             
           call errex('bplist 82:ord problem')                          
       end if                                                           
       do 90 ibp=1,libpmx                                               
        bpli(1,ibp)=equark(1,lord(1,ibp))                               
        bpli(2,ibp)=equark(2,lord(1,ibp))/bpli(1,ibp)                   
   90  continue                                                         
       taumax=sqrt(bpli(2,libpmx)*bpli(1,libpmx))                       
      return                                                            
      end                                                               
      function qmass(ifl)                                               
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
       iabifl=iabs(ifl)                                                 
       modifl=mod(iabifl,10)                                            
       iabifl=iabifl/10                                                 
       if(iabifl.eq.0) then                                             
         qmas1=0.0                                                      
       else if(iabifl.eq.1.or.iabifl.eq.2) then                         
         qmas1=emq+emqcns                                               
       else if(iabifl.eq.3) then                                        
         qmas1=ems+emqcns                                               
       else if(iabifl.eq.4) then                                        
         qmas1=1.6+emqcns                                               
       else                                                             
         call errex('wrong input ifl into qmass')                       
         qmas1=0.                                                       
       end if                                                           
       if(modifl.eq.1.or.modifl.eq.2) then                              
         qmass=qmas1+emq+emqcns                                         
       else if(modifl.eq.3) then                                        
         qmass=qmas1+ems+emqcns                                         
       else if(modifl.eq.4) then                                        
         qmass=qmas1+1.6+emqcns                                         
       else                                                             
         call errex('wrong input ifl into qmass')                       
         qmass=0.                                                       
       end if                                                           
      return                                                            
      end                                                               
      logical function reject(ips,iqs,atrel)                            
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
       if(ips.lt.iqs) then                                              
          ip=iqs                                                        
          iq=ips                                                        
       else                                                             
          ip=ips                                                        
          iq=iqs                                                        
       end if                                                           
       if(ip.gt.16) ip=16                                               
       if(iq.gt.16) iq=16                                               
       ientry=min(152,ip*(ip+1)/2+iq)                                   
       dndx4=2.*prbrop(1,ientry)+prbrop(2,ientry)+2.*prbrop(3,ientry)+  
     a          prbrop(4,ientry)+2.*prbrop(5,ientry)+prbrop(6,ientry)+  
     b                        4.*prbrop(7,ientry)+2.*prbrop(8,ientry)+  
     c                        4.*prbrop(9,ientry)+2.*prbrop(10,ientry)  
       if(ranf(0).gt.atrel*dndx4/(atrmax*dndx4m)) then                  
         reject=.true.                                                  
       else                                                             
         reject=.false.                                                 
       end if                                                           
      return                                                            
      end                                                               
      subroutine flavor(ips,iqs,ifl)                                    
      implicit real*8(a-h,o-z)                                          
      real*4 ranf,par                                                   
      real*4  atrmax,tnsmax,dndx4m,emqcns,emq,ems,prbrop                
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      common/ludat1/mst(40),par(80)                                     
       if(ips.lt.iqs) then                                              
          ip=iqs                                                        
          iq=ips                                                        
          jvz=-1                                                        
       else                                                             
          ip=ips                                                        
          iq=iqs                                                        
          jvz=1                                                         
       end if                                                           
       if(ip.gt.16) ip=16                                               
       if(iq.gt.16) iq=16                                               
       ientry=min(152,ip*(ip+1)/2+iq)                                   
       dndx4=2.*prbrop(1,ientry)+prbrop(2,ientry)+2.*prbrop(3,ientry)+  
     a          prbrop(4,ientry)+2.*prbrop(5,ientry)+prbrop(6,ientry)+  
     b                        4.*prbrop(7,ientry)+2.*prbrop(8,ientry)+  
     c                        4.*prbrop(9,ientry)+2.*prbrop(10,ientry)  
       pq1=2.*prbrop(1,ientry)/dndx4                                    
       ps1=pq1+prbrop(2,ientry)/dndx4                                   
       pq2=ps1+2.*prbrop(3,ientry)/dndx4                                
       ps2=pq2+prbrop(4,ientry)/dndx4                                   
       pqq=ps2+2.*prbrop(5,ientry)/dndx4                                
       pqs=pqq+prbrop(6,ientry)/dndx4                                   
       pdiq1=pqs+4.*prbrop(7,ientry)/dndx4                              
       pdiqs1=pdiq1+2.*prbrop(8,ientry)/dndx4                           
       pdiq2=pdiqs1+4.*prbrop(9,ientry)/dndx4                           
       pdiqs2=pdiq2+2.*prbrop(10,ientry)/dndx4                          
   10  continue                                                         
       zrand=ranf(0)                                                    
       if(zrand.lt.pq1) then                                            
         ifla=-1                                                        
         if(zrand/pq1.ge.0.5) then                                      
           ifla=-2                                                      
         end if                                                         
       else if(zrand.lt.ps1) then                                       
         ifla=-3                                                        
       else if(zrand.lt.pq2) then                                       
         ifla=1                                                         
         if((zrand-ps1)/(pq2-ps1).ge.0.5) then                          
           ifla=2                                                       
         end if                                                         
       else if(zrand.lt.ps2) then                                       
         ifla=3                                                         
       else if(zrand.lt.pqq) then                                       
         ifla=10                                                        
         if((zrand-ps2)/(pqq-ps2).ge.0.5) then                          
           ifla=20                                                      
         end if                                                         
         yrand=ranf(0)*ps1                                              
         if(yrand.lt.0.5*pq1) then                                      
           ifla=ifla+1                                                  
         else if(yrand.lt.pq1) then                                     
           ifla=ifla+2                                                  
         else                                                           
           ifla=ifla+3                                                  
         end if                                                         
       else if(zrand.lt.pqs) then                                       
         ifla=30                                                        
         yrand=ranf(0)*ps1                                              
         if(yrand.lt.0.5*pq1) then                                      
           ifla=ifla+1                                                  
         else if(yrand.lt.pq1) then                                     
           ifla=ifla+2                                                  
         else                                                           
           ifla=ifla+3                                                  
         end if                                                         
       else if(zrand.lt.pdiq1) then                                     
         if(ranf(0).ge.0.5) then                                        
           ifla=10                                                      
         else                                                           
           ifla=20                                                      
         end if                                                         
         yrand=ranf(0)                                                  
         if(yrand.lt.0.5) then                                          
           ifla=ifla+1                                                  
         else                                                           
           ifla=ifla+2                                                  
         end if                                                         
       else if(zrand.lt.pdiqs1) then                                    
         ifla=30                                                        
         yrand=ranf(0)                                                  
         if(yrand.lt.0.5) then                                          
           ifla=ifla+1                                                  
         else                                                           
           ifla=ifla+2                                                  
         end if                                                         
       else if(zrand.lt.pdiq2) then                                     
         if(ranf(0).ge.0.5) then                                        
           ifla=-10                                                     
         else                                                           
           ifla=-20                                                     
         end if                                                         
         yrand=ranf(0)                                                  
         if(yrand.lt.0.5) then                                          
           ifla=ifla-1                                                  
         else                                                           
           ifla=ifla-2                                                  
         end if                                                         
       else if(zrand.lt.pdiqs2) then                                    
         ifla=-30                                                       
         yrand=ranf(0)                                                  
         if(yrand.lt.0.5) then                                          
           ifla=ifla-1                                                  
         else                                                           
           ifla=ifla-2                                                  
         end if                                                         
       else                                                             
         write(8,*) 'flavor:numerical error',zrand                      
         go to 10                                                       
       end if                                                           
       ifla=jvz*ifla                                                    
       iflabs=iabs(ifla)                                                
       if(iflabs.lt.10) then                                            
         ifl=ifla                                                       
       else                                                             
         iflb=mod(iflabs,10)                                            
         iflc=iflabs/10                                                 
         if(iflb.eq.iflc) then                                          
           ifl=ifla                                                     
         else if(iflc.gt.iflb) then                                     
           if(ranf(0).gt.3.*par(4)/(1.+3.*par(4))) then                 
             ifl=isign(1,ifla)*(10*iflb+iflc)                           
           else                                                         
             ifl=ifla                                                   
           end if                                                       
         else                                                           
           if(ranf(0).lt.3.*par(4)/(1.+3.*par(4))) then                 
             ifl=isign(1,ifla)*(10*iflb+iflc)                           
           else                                                         
             ifl=ifla                                                   
           end if                                                       
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      subroutine newlcp(ppavl,pmavl,em1t,em2t,                          
     a             p1pl,p1mi,p2pl,p2mi,jt)                              
       include 'scatter7'                                               
       real*8 dacof,dccof,dp1mi,dp1pl,dp2mi,dp2pl,dem1sq,dem2sq,        
     a        dpmavl,dppavl                                             
           dem1sq=em1t*em1t                                             
           dem2sq=em2t*em2t                                             
           dppavl=ppavl                                                 
           dpmavl=pmavl                                                 
           dacof=0.5*(dpmavl*dppavl-dem2sq+dem1sq)/dppavl               
           dccof=-dpmavl*dem1sq/dppavl                                  
           if(jt.eq.1) then                                             
              dp1mi=dacof-dsqrt(dacof**2+dccof)                         
           else                                                         
              dp1mi=dacof+dsqrt(dacof**2+dccof)                         
           end if                                                       
           dp1pl=dem1sq/dp1mi                                           
           dp2mi=dpmavl-dp1mi                                           
           dp2pl=dem2sq/dp2mi                                           
           p1pl=dp1pl                                                   
           p1mi=dp1mi                                                   
           p2pl=dp2pl                                                   
           p2mi=dp2mi                                                   
           if(abs((p1pl+p2pl)/ppavl-1.).gt.0.001.or.                    
     a        abs((p1mi+p2mi)/pmavl-1.).gt.0.001) then                  
             write(8,*) 'newlcp no p conservation'                      
             write(8,*) 'p1/2+/-=',p1pl,p1mi,p2pl,p2mi                  
             write(8,*) ppavl,p1pl+p2pl,pmavl,p1mi+p2mi                 
           end if                                                       
           if(p1pl.lt.0.0.or.p1mi.lt.0.0.or.                            
     a                     p2pl.lt.0.0.or.p2mi.lt.0.0) then             
             write(8,*) 'p1/2+/-=',p1pl,p1mi,p2pl,p2mi                  
             call errex('newlcp:wrong lc momenta ????')                 
           end if                                                       
       return                                                           
       end                                                              
       function lchar(ifl)                                              
        iflabs=iabs(ifl)                                                
        ifl1=mod(iflabs,10)                                             
        ifl2=iflabs/10                                                  
        if(ifl1.eq.1) then                                              
          lchar=isign(2,ifl)                                            
        else if(ifl1.eq.2) then                                         
          lchar=-isign(1,ifl)                                           
        else if(ifl1.eq.3) then                                         
          lchar=-isign(1,ifl)                                           
        else if(ifl1.eq.4) then                                         
          lchar=isign(2,ifl)                                            
        else                                                            
          call errex('lchar:unknown ifl')                               
        end if                                                          
        if(ifl2.gt.0) then                                              
          if(ifl2.eq.1) then                                            
            lchar=lchar+isign(2,ifl)                                    
          else if(ifl2.eq.2) then                                       
            lchar=lchar-isign(1,ifl)                                    
          else if(ifl2.eq.3) then                                       
            lchar=lchar-isign(1,ifl)                                    
          else if(ifl2.eq.4) then                                       
            lchar=lchar-isign(2,ifl)                                    
          else                                                          
            call errex('lchar:unknown ifl')                             
          end if                                                        
        end if                                                          
       return                                                           
       end                                                              
       function itrip(ifl)                                              
        if(ifl.gt.0.and.ifl.lt.10) then                                 
          itrip=10                                                      
        else if(-ifl.gt.0.and.-ifl.lt.10) then                          
          itrip=1                                                       
        else if(ifl.gt.10) then                                         
          itrip=1                                                       
        else if(-ifl.gt.10) then                                        
          itrip=10                                                      
        else                                                            
          write(8,*) 'ifl=',ifl                                         
          itrip=1/ifl                                                   
          call errex('itrip:unknown ifl as input')                      
          itrip=0                                                       
        end if                                                          
       return                                                           
       end                                                              
      function sigptr(ips,iqs,k)                                        
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      common/ludat1/mst(40),par(80)                                     
       if(ips.lt.iqs) then                                              
          ip=iqs                                                        
          iq=ips                                                        
       else                                                             
          ip=ips                                                        
          iq=iqs                                                        
       end if                                                           
       if(ip.gt.16) ip=16                                               
       if(iq.gt.16) iq=16                                               
       if(ip.eq.1.and.iq.eq.0) then                                     
         sigptr=par(12)                                                 
         return                                                         
       end if                                                           
       ientry=min(152,ip*(ip+1)/2+iq)                                   
       if(k.ne.1.and.k.ne.2) then                                       
         call errex('sigptr:wrong k input')                             
         sigptr=0.0                                                     
       else                                                             
         k=k+10                                                         
         sosrc2=max(0.0,par(12)**2-4./par(71)*prbrop(11,1)**2)          
         stunn2=4./par(71)*prbrop(k,ientry)**2                          
         sigptr=sqrt(sosrc2+stunn2)                                     
       end if                                                           
      return                                                            
      end                                                               
      block data tubdat                                                 
      common /ptubeb/ atrmax,                                           
     a               tnsmax,dndx4m,emqcns,emq,ems,prbrop(14,152)        
      data emqcns,emq,ems /                                             
     *               0.35, 0.010, 0.160                                 
     * /                                                                
       data (prbrop( 1,im),im=1,152)   /                                
     *   .139,   .256,   .406,   .591,   .809,   .809,  1.060,  1.345,  
     *  1.664,  1.345,  1.664,  2.016,  2.402,  2.822,  2.016,  2.402,  
     *  2.822,  3.275,  3.761,  4.281,  2.822,  3.275,  3.761,  4.281,  
     *  4.835,  5.422,  6.043,  3.761,  4.281,  4.835,  5.422,  6.043,  
     *  6.697,  7.385,  8.106,  4.835,  5.422,  6.043,  6.697,  7.385,  
     *  8.106,  8.860,  9.647, 10.466,  6.043,  6.697,  7.385,  8.106,  
     *  8.860,  9.647, 10.466, 11.319, 12.204, 13.122,  7.385,  8.106,  
     *  8.860,  9.647, 10.466, 11.319, 12.204, 13.122, 14.071, 15.053,  
     * 16.066,  8.860,  9.647, 10.466, 11.319, 12.204, 13.122, 14.071,  
     * 15.053, 16.066, 17.111, 18.186, 19.293, 10.466, 11.319, 12.204,  
     * 13.122, 14.071, 15.053, 16.066, 17.111, 18.186, 19.293, 20.430,  
     * 21.598, 22.795, 12.204, 13.122, 14.071, 15.053, 16.066, 17.111,  
     * 18.186, 19.293, 20.430, 21.598, 22.795, 24.022, 25.278, 26.564,  
     * 14.071, 15.053, 16.066, 17.111, 18.186, 19.293, 20.430, 21.598,  
     * 22.795, 24.022, 25.278, 26.564, 27.878, 29.220, 30.591, 16.066,  
     * 17.111, 18.186, 19.293, 20.430, 21.598, 22.795, 24.022, 25.278,  
     * 26.564, 27.878, 29.220, 30.591, 31.989, 33.415, 34.868, 18.186,  
     * 19.293, 20.430, 21.598, 22.795, 24.022, 25.278, 26.564, 27.878,  
     * 29.220, 30.591, 31.989, 33.415, 34.868, 36.347, 37.853, 39.385   
     * /                                                                
       data (prbrop( 2,im),im=1,152)   /                                
     *   .041,   .105,   .199,   .325,   .482,   .482,   .672,   .893,  
     *  1.147,   .893,  1.147,  1.434,  1.753,  2.105,  1.434,  1.753,  
     *  2.105,  2.490,  2.908,  3.359,  2.105,  2.490,  2.908,  3.359,  
     *  3.843,  4.360,  4.911,  2.908,  3.359,  3.843,  4.360,  4.911,  
     *  5.494,  6.110,  6.760,  3.843,  4.360,  4.911,  5.494,  6.110,  
     *  6.760,  7.442,  8.157,  8.905,  4.911,  5.494,  6.110,  6.760,  
     *  7.442,  8.157,  8.905,  9.686, 10.499, 11.344,  6.110,  6.760,  
     *  7.442,  8.157,  8.905,  9.686, 10.499, 11.344, 12.221, 13.130,  
     * 14.071,  7.442,  8.157,  8.905,  9.686, 10.499, 11.344, 12.221,  
     * 13.130, 14.071, 15.043, 16.046, 17.080,  8.905,  9.686, 10.499,  
     * 11.344, 12.221, 13.130, 14.071, 15.043, 16.046, 17.080, 18.144,  
     * 19.239, 20.364, 10.499, 11.344, 12.221, 13.130, 14.071, 15.043,  
     * 16.046, 17.080, 18.144, 19.239, 20.364, 21.519, 22.702, 23.916,  
     * 12.221, 13.130, 14.071, 15.043, 16.046, 17.080, 18.144, 19.239,  
     * 20.364, 21.519, 22.702, 23.916, 25.157, 26.427, 27.726, 14.071,  
     * 15.043, 16.046, 17.080, 18.144, 19.239, 20.364, 21.519, 22.702,  
     * 23.915, 25.157, 26.427, 27.726, 29.052, 30.406, 31.787, 16.046,  
     * 17.080, 18.144, 19.239, 20.364, 21.519, 22.702, 23.915, 25.157,  
     * 26.427, 27.726, 29.052, 30.406, 31.787, 33.195, 34.629, 36.090   
     * /                                                                
       data (prbrop( 3,im),im=1,152)   /                                
     *   .000,   .256,   .000,   .406,   .809,   .000,   .591,  1.060,  
     *  1.664,   .000,   .809,  1.345,  2.016,  2.822,   .000,  1.060,  
     *  1.664,  2.402,  3.275,  4.281,   .000,  1.345,  2.016,  2.822,  
     *  3.761,  4.835,  6.043,   .000,  1.664,  2.402,  3.275,  4.281,  
     *  5.422,  6.697,  8.106,   .000,  2.016,  2.822,  3.761,  4.835,  
     *  6.043,  7.385,  8.860, 10.466,   .000,  2.402,  3.275,  4.281,  
     *  5.422,  6.697,  8.106,  9.647, 11.319, 13.122,   .000,  2.822,  
     *  3.761,  4.835,  6.043,  7.385,  8.860, 10.466, 12.204, 14.071,  
     * 16.066,   .000,  3.275,  4.281,  5.422,  6.697,  8.106,  9.647,  
     * 11.319, 13.122, 15.053, 17.111, 19.293,   .000,  3.761,  4.835,  
     *  6.043,  7.385,  8.860, 10.466, 12.204, 14.071, 16.066, 18.186,  
     * 20.430, 22.795,   .000,  4.281,  5.422,  6.697,  8.106,  9.647,  
     * 11.319, 13.122, 15.053, 17.111, 19.293, 21.598, 24.022, 26.564,  
     *   .000,  4.835,  6.043,  7.385,  8.860, 10.466, 12.204, 14.071,  
     * 16.066, 18.186, 20.430, 22.795, 25.278, 27.878, 30.591,   .000,  
     *  5.422,  6.697,  8.106,  9.647, 11.319, 13.122, 15.053, 17.111,  
     * 19.293, 21.598, 24.022, 26.564, 29.220, 31.989, 34.868,   .000,  
     *  6.043,  7.385,  8.860, 10.466, 12.204, 14.071, 16.066, 18.186,  
     * 20.430, 22.795, 25.278, 27.878, 30.591, 33.415, 36.347, 39.385   
     * /                                                                
       data (prbrop( 4,im),im=1,152)   /                                
     *   .000,   .105,   .000,   .199,   .482,   .000,   .325,   .672,  
     *  1.147,   .000,   .482,   .893,  1.434,  2.105,   .000,   .672,  
     *  1.147,  1.753,  2.490,  3.359,   .000,   .893,  1.434,  2.105,  
     *  2.908,  3.843,  4.911,   .000,  1.147,  1.753,  2.490,  3.359,  
     *  4.360,  5.494,  6.760,   .000,  1.434,  2.105,  2.908,  3.843,  
     *  4.911,  6.110,  7.442,  8.905,   .000,  1.753,  2.490,  3.359,  
     *  4.360,  5.494,  6.760,  8.157,  9.686, 11.344,   .000,  2.105,  
     *  2.908,  3.843,  4.911,  6.110,  7.442,  8.905, 10.499, 12.221,  
     * 14.071,   .000,  2.490,  3.359,  4.360,  5.494,  6.760,  8.157,  
     *  9.686, 11.344, 13.130, 15.043, 17.080,   .000,  2.908,  3.843,  
     *  4.911,  6.110,  7.442,  8.905, 10.499, 12.221, 14.071, 16.046,  
     * 18.144, 20.364,   .000,  3.359,  4.360,  5.494,  6.760,  8.157,  
     *  9.686, 11.344, 13.130, 15.043, 17.080, 19.239, 21.519, 23.916,  
     *   .000,  3.843,  4.911,  6.110,  7.442,  8.905, 10.499, 12.221,  
     * 14.071, 16.046, 18.144, 20.364, 22.702, 25.157, 27.726,   .000,  
     *  4.360,  5.494,  6.760,  8.157,  9.686, 11.344, 13.130, 15.043,  
     * 17.080, 19.239, 21.519, 23.916, 26.427, 29.052, 31.787,   .000,  
     *  4.911,  6.110,  7.442,  8.905, 10.499, 12.221, 14.071, 16.046,  
     * 18.144, 20.364, 22.703, 25.157, 27.726, 30.406, 33.195, 36.090   
     * /                                                                
       data (prbrop( 5,im),im=1,152)   /                                
     *   .016,   .000,   .112,   .008,   .000,   .278,   .056,   .008,  
     *   .000,   .512,   .139,   .056,   .008,   .000,   .813,   .256,  
     *   .139,   .056,   .008,   .000,  1.181,   .406,   .256,   .139,  
     *   .056,   .008,   .000,  1.617,   .591,   .406,   .256,   .139,  
     *   .056,   .008,   .000,  2.120,   .809,   .591,   .406,   .256,  
     *   .139,   .056,   .008,   .000,  2.690,  1.060,   .809,   .591,  
     *   .406,   .256,   .139,   .056,   .008,   .000,  3.328,  1.345,  
     *  1.060,   .809,   .591,   .406,   .256,   .139,   .056,   .008,  
     *   .000,  4.032,  1.664,  1.345,  1.060,   .809,   .591,   .406,  
     *   .256,   .139,   .056,   .008,   .000,  4.804,  2.016,  1.664,  
     *  1.345,  1.060,   .809,   .591,   .406,   .256,   .139,   .056,  
     *   .008,   .000,  5.643,  2.402,  2.016,  1.664,  1.345,  1.060,  
     *   .809,   .591,   .406,   .256,   .139,   .056,   .008,   .000,  
     *  6.549,  2.822,  2.402,  2.016,  1.664,  1.345,  1.060,   .809,  
     *   .591,   .406,   .256,   .139,   .056,   .008,   .000,  7.523,  
     *  3.275,  2.822,  2.402,  2.016,  1.664,  1.345,  1.060,   .809,  
     *   .591,   .406,   .256,   .139,   .056,   .008,   .000,  8.563,  
     *  3.761,  3.275,  2.822,  2.402,  2.016,  1.664,  1.345,  1.060,  
     *   .809,   .591,   .406,   .256,   .139,   .056,   .008,   .000   
     * /                                                                
       data (prbrop( 6,im),im=1,152)   /                                
     * .00014, .00000, .01476, .00007, .00000, .08190, .00738, .00007,  
     * .00000, .20912, .04095, .00738, .00007, .00000, .39798, .10456,  
     * .04095, .00738, .00007, .00000, .64947, .19899, .10456, .04095,  
     * .00738, .00007, .00000, .96434, .32474, .19899, .10456, .04095,  
     * .00738, .00007, .00000,1.34316, .48217, .32474, .19899, .10456,  
     * .04095, .00738, .00007, .00000,1.78642, .67158, .48217, .32474,  
     * .19899, .10456, .04095, .00738, .00007, .00000,2.29448, .89321,  
     * .67158, .48217, .32474, .19899, .10456, .04095, .00738, .00007,  
     * .00000,2.86769,1.14724, .89321, .67158, .48217, .32474, .19899,  
     * .10456, .04095, .00738, .00007, .00000,3.50629,1.43384,1.14724,  
     * .89321, .67158, .48217, .32474, .19899, .10456, .04095, .00738,  
     * .00007, .00000,4.21050,1.75314,1.43384,1.14724, .89321, .67158,  
     * .48217, .32474, .19899, .10456, .04095, .00738, .00007, .00000,  
     *4.98052,2.10525,1.75314,1.43384,1.14724, .89321, .67158, .48217,  
     * .32474, .19899, .10456, .04095, .00738, .00007, .00000,5.81643,  
     *2.49026,2.10525,1.75314,1.43384,1.14724, .89321, .67158, .48217,  
     * .32474, .19899, .10456, .04095, .00738, .00007, .00000,6.71842,  
     *2.90822,2.49026,2.10525,1.75315,1.43385,1.14724, .89321, .67158,  
     * .48217, .32474, .19899, .10456, .04095, .00738, .00007, .00000   
     * /                                                                
       data (prbrop( 7,im),im=1,152)   /                                
     *  152*0.0                                                         
     * /                                                                
       data (prbrop( 8,im),im=1,152)   /                                
     *  152*0.0                                                         
     * /                                                                
       data (prbrop( 9,im),im=1,152)   /                                
     *  152*0.0                                                         
     * /                                                                
       data (prbrop(10,im),im=1,152)   /                                
     *  152*0.0                                                         
     * /                                                                
       data (prbrop(11,im),im=1,152)   /                                
     *   .126,   .153,   .176,   .196,   .215,   .215,   .232,   .247,  
     *   .262,   .247,   .262,   .276,   .290,   .303,   .276,   .290,  
     *   .303,   .315,   .327,   .338,   .303,   .315,   .327,   .338,  
     *   .349,   .360,   .370,   .327,   .338,   .349,   .360,   .370,  
     *   .380,   .390,   .400,   .349,   .360,   .370,   .380,   .390,  
     *   .400,   .409,   .418,   .427,   .370,   .380,   .390,   .400,  
     *   .409,   .418,   .427,   .435,   .443,   .452,   .390,   .400,  
     *   .409,   .418,   .427,   .435,   .443,   .452,   .459,   .467,  
     *   .475,   .409,   .418,   .427,   .435,   .443,   .452,   .459,  
     *   .467,   .475,   .482,   .489,   .496,   .427,   .435,   .443,  
     *   .452,   .459,   .467,   .475,   .482,   .489,   .496,   .502,  
     *   .509,   .515,   .443,   .452,   .459,   .467,   .475,   .482,  
     *   .489,   .496,   .502,   .509,   .515,   .521,   .527,   .533,  
     *   .459,   .467,   .475,   .482,   .489,   .496,   .502,   .509,  
     *   .515,   .521,   .527,   .533,   .539,   .544,   .550,   .475,  
     *   .482,   .489,   .496,   .502,   .509,   .515,   .521,   .527,  
     *   .533,   .539,   .544,   .550,   .555,   .560,   .565,   .489,  
     *   .496,   .502,   .509,   .515,   .521,   .527,   .533,   .539,  
     *   .544,   .550,   .555,   .560,   .565,   .570,   .575,   .579   
     * /                                                                
       data (prbrop(12,im),im=1,152)   /                                
     * -1.000,   .153, -1.000,   .176,   .215, -1.000,   .196,   .232,  
     *   .262, -1.000,   .215,   .247,   .276,   .303, -1.000,   .232,  
     *   .262,   .290,   .315,   .338, -1.000,   .247,   .276,   .303,  
     *   .327,   .349,   .370, -1.000,   .262,   .290,   .315,   .338,  
     *   .360,   .380,   .400, -1.000,   .276,   .303,   .327,   .349,  
     *   .370,   .390,   .409,   .427, -1.000,   .290,   .315,   .338,  
     *   .360,   .380,   .400,   .418,   .435,   .452, -1.000,   .303,  
     *   .327,   .349,   .370,   .390,   .409,   .427,   .443,   .459,  
     *   .475, -1.000,   .315,   .338,   .360,   .380,   .400,   .418,  
     *   .435,   .452,   .467,   .482,   .496, -1.000,   .327,   .349,  
     *   .370,   .390,   .409,   .427,   .443,   .459,   .475,   .489,  
     *   .502,   .515, -1.000,   .338,   .360,   .380,   .400,   .418,  
     *   .435,   .452,   .467,   .482,   .496,   .509,   .521,   .533,  
     * -1.000,   .349,   .370,   .390,   .409,   .427,   .443,   .459,  
     *   .475,   .489,   .502,   .515,   .527,   .539,   .550, -1.000,  
     *   .360,   .380,   .400,   .418,   .435,   .452,   .467,   .482,  
     *   .496,   .509,   .521,   .533,   .544,   .555,   .565, -1.000,  
     *   .370,   .390,   .409,   .427,   .443,   .459,   .475,   .489,  
     *   .502,   .515,   .527,   .539,   .550,   .560,   .570,   .579   
     * /                                                                
       data (prbrop(13,im),im=1,152)   /                                
     *   .269,   .839,   .604,  1.152,  2.149,  1.075,  1.793,  2.625,  
     *  4.063,  1.679,  2.587,  3.570,  4.697,  6.581,  2.418,  3.534,  
     *  4.674,  5.946,  7.371,  9.704,  3.291,  4.632,  5.936,  7.356,  
     *  8.923, 10.647, 13.429,  4.298,  5.882,  7.355,  8.926, 10.637,  
     * 12.501, 14.524, 17.752,  5.439,  7.282,  8.930, 10.655, 12.511,  
     * 14.516, 16.677, 18.996, 22.669,  6.714,  8.833, 10.662, 12.543,  
     * 14.546, 16.692, 18.990, 21.445, 24.057, 28.168,  8.122, 10.534,  
     * 12.548, 14.588, 16.739, 19.026, 21.462, 24.052, 26.797, 29.697,  
     * 34.239,  9.663, 12.383, 14.588, 16.789, 19.088, 21.517, 24.091,  
     * 26.815, 29.691, 32.720, 35.901, 40.868, 11.334, 14.380, 16.780,  
     * 19.144, 21.592, 24.163, 26.874, 29.732, 32.739, 35.896, 39.202,  
     * 42.657, 48.037, 13.136, 16.522, 19.123, 21.651, 24.249, 26.962,  
     * 29.809, 32.799, 35.936, 39.220, 42.650, 46.227, 49.947, 55.732,  
     * 15.065, 18.808, 21.613, 24.308, 27.056, 29.911, 32.894, 36.016,  
     * 39.280, 42.689, 46.243, 49.940, 53.778, 57.756, 63.934, 17.120,  
     * 21.235, 24.250, 27.113, 30.011, 33.007, 36.125, 39.378, 42.769,  
     * 46.302, 49.977, 53.792, 57.747, 61.839, 66.066, 72.628, 19.299,  
     * 23.802, 27.031, 30.063, 33.112, 36.248, 39.500, 42.882, 46.400,  
     * 50.055, 53.850, 57.782, 61.851, 66.056, 70.393, 74.861, 81.794   
     * /                                                                
      data (prbrop(14,im),im=1,152)   /                                 
     *   152*0.0                                                        
     *            /                                                     
      end                                                               
      subroutine intfac(nsave)                                          
       include 'scatter7'                                               
       include 'partblk7'                                               
       real*4 plund                                                     
       common /lujets/ nlund,k(2000,2),plund(2000,5)                    
       parameter (mxs=200)                                              
       common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout               
       logical manip                                                    
       logical issers                                                   
       common /rop3bk/manip(mxs)                                        
       dimension xvold(0:3),ptot(0:3)                                   
       if(nlund.eq.nsave) go to 92                                      
       if(mod(nlund-nsave,2).eq.1) then                                 
         call errex('intfac:wrong nlund/nsave')                         
       end if                                                           
       iposi=(nlund-nsave)/2                                            
       ievntn=ievntn+1                                                  
       do 18 iroa=1,iroin                                               
        if(manip(iroa)) then                                            
          do 17  ik=lstar(iroa),lende(iroa)                             
           deltx0=decay(ik)-x(0,ik)                                     
           if(qdot(0,ik).eq.0) then                                     
           else                                                         
             do 6 i=0,3                                                 
    6        x(i,ik)=x(i,ik)+qdot(i,ik)/qdot(0,ik)*deltx0               
           end if                                                       
   17     continue                                                      
        end if                                                          
   18  continue                                                         
       if(ida(25).eq.1) then                                            
         if(switch(23)) then                                            
           ken=itripl                                                   
         else                                                           
           ken=kdt                                                      
         end if                                                         
         jkill=0                                                        
         do 5 iroa=1,iroin                                              
          if(manip(iroa)) then                                          
            jkill=jkill+lende(iroa)-lstar(iroa)+1                       
          end if                                                        
    5    continue                                                       
         write(15,661) irun,ken,4,0,jkill,0,0,0                         
       end if                                                           
  661  format(1x,i3,1x,i3,1x,i1,1x,i5,1x,i3,1x,i2,1x,i8,1x,i8)          
       do 8 iroa=1,iroin                                                
        if(manip(iroa)) then                                            
          do 7  ik=lstar(iroa),lende(iroa)                              
           idntk=issn(ik)                                               
           call transl(-1,ityp(ik),npidec(ik),kf)                       
           if(ida(25).eq.1) then                                        
             write(15,662)  9999,kf,(p(i,ik),i=0,3),                    
     a             em(ik),0,idntk,x(0,ik),                              
     b                               x(1,ik),x(2,ik),x(3,ik)            
           end if                                                       
           if(ida(42).eq.1) then                                        
             write(7)                                                   
     &              -9999,issn(ik),kf,                                  
     &              x(0,ik),x(1,ik),x(2,ik),x(3,ik),                    
     &              p(0,ik),p(1,ik),p(2,ik),p(3,ik),em(ik),             
     &              0.0,0,0                                             
           end if                                                       
           if(ida(38).eq.1.or.ida(38).eq.2.or.                          
     a                 ida(38).eq.5.or.ida(38).eq.7) then               
             if(ida(38).eq.1.or.ida(38).eq.2) then                      
               jeng=1                                                   
             else                                                       
               jeng=0                                                   
             end if                                                     
             emik=em(ik)                                                
             if(issers(jeng,ida(38),ityp(ik),npidec(ik),emik))then      
               write(13,888) idntk, 0,ityp(ik),npidec(ik),              
     a                          p(0,ik),(-p(i,ik),i=1,3)                
               write(13,889) (stork(idntk,i),i=1,5)                     
               write(13,890) (kstor(idntk,i),i=1,7)                     
               write(13,888) idntk,-3,ityp(ik),npidec(ik),              
     a                          p(0,ik),(-p(i,ik),i=1,3)                
               write(13,889) (stork(idntk,i),i=1,5)                     
               write(13,890) (kstor(idntk,i),i=1,7)                     
             end if                                                     
           end if                                                       
  888      format(1x,4(i7,1x),1x,4(f7.3,1x))                            
  889      format(1x,5(f7.3,1x))                                        
  890      format(1x,5(i7,1x),2(i4,1x))                                 
    7     continue                                                      
        end if                                                          
    8  continue                                                         
       if(ida(25).eq.1)                                                 
     a write(15,661) irun,ken,5,0,iposi,0,0,0                           
       iro=0                                                            
       irosto=0                                                         
       do 10 iroa=1,iroin                                               
        if(manip(iroa)) then                                            
          iro=iroa                                                      
          go to 12                                                      
        end if                                                          
   10  continue                                                         
   12  continue                                                         
       if(iro.eq.0) then                                                
         call errex('intfac:iro=0')                                     
       end if                                                           
       ntotm=ntotma+1                                                   
       lnew=0                                                           
       ksta=lstar(iro)                                                  
       ke=lende(iro)                                                    
       il=ksta-1                                                        
       do 80 nl=nsave+1,nsave+iposi                                     
        il=il+1                                                         
        if(min(ntotm,il).gt.ke) then                                    
          irosto=iro                                                    
          do 20 iroa=iro+1,iroin                                        
           if(manip(iroa)) then                                         
             iro=iroa                                                   
             go to 22                                                   
           end if                                                       
   20     continue                                                      
   22     continue                                                      
          if(irosto.eq.iro) then                                        
            ke=ntotm                                                    
            if(nrmes(kclst)+nrbar(kclst).lt.iclusz(kclst)) then         
              il=iofset+nrmes(kclst)+nrbar(kclst)+1                     
            else                                                        
              call errex('intfac:no free space in arrays left')         
            end if                                                      
            if(lnew.eq.0) then                                          
              lnew=1                                                    
              lstan=il                                                  
            end if                                                      
          else                                                          
            ksta=lstar(iro)                                             
            ke=lende(iro)                                               
            il=ksta                                                     
          end if                                                        
        end if                                                          
        if(ke.eq.ntotm) then                                            
          npion=npion+1                                                 
          nrmes(kclst)=nrmes(kclst)+1                                   
          if(nrmes(kclst)+nrbar(kclst).gt.iclusz(kclst)) then           
            call errex('intfac:rqmd array overflow')                    
          end if                                                        
          nprtcl=nprtcl+1                                               
          if(nprtcl.gt.ntotma) then                                     
            call errex('intfac:nprtcl>ntotma, abort')                   
          end if                                                        
          issn(il)=nprtcl                                               
          lendn=il                                                      
        end if                                                          
        do 71 i=1,3                                                     
   71   p(i,il)=plund(nl,i)                                             
        p(0,il)=plund(nl,4)                                             
        em(il)=plund(nl,5)                                              
        kf=k(nl,2)                                                      
        kmulth=iabs(kf)/100                                             
        kf=isign(1,kf)*mod(iabs(kf),100)                                
        eml=em(il)                                                      
        call oldnew(0,ityp(il),npidec(il),eml,kmulth,kf)                
        nlu=nl+iposi                                                    
        do 72 i=1,3                                                     
   72   x(i,il)=plund(nlu,i)                                            
        x(0,il)=plund(nlu,4)                                            
        decay(il)=x(0,il)                                               
        do 73 i=0,3                                                     
   73   qdot(i,il)=0.0                                                  
        lastcl(il)=0                                                    
        identl=issn(il)                                                 
        ylast(identl)=0.5*log((p(0,il)+p(3,il))/(p(0,il)-p(3,il)))      
        zlast(identl)=x(3,il)                                           
        if(ida(25).eq.1) then                                           
  662     format(1x,i4,1x,i4,1x,f8.3,1x,2(f7.3,1x),f8.3,1x,             
     a               f7.3,i3,1x,i8,1x,f8.3,2(1x,f7.3),1x,f8.3)          
             write(15,662) ityp(il),npidec(il),(p(i,il),i=0,3),         
     a              em(il),0,identl,x(0,il),                            
     b                               x(1,il),x(2,il),x(3,il)            
        end if                                                          
        if(ida(42).eq.1) then                                           
          write(7)                                                      
     &         ityp(il),issn(il),npidec(il),                            
     &         x(0,il),x(1,il),x(2,il),x(3,il),                         
     &         p(0,il),p(1,il),p(2,il),p(3,il),em(il),                  
     &         0.0,0,0                                                  
        end if                                                          
        if(ida(38).eq.3.or.ida(38).eq.4) then                           
          if(iposi.eq.1) then                                           
            kstor(identl,1)=1                                           
          else                                                          
            kstor(identl,1)=4                                           
          end if                                                        
          do 203 i=1,4                                                  
  203     stork(identl,i)=x(i-1,il)                                     
          do 303 i=6,8                                                  
  303     stork(identl,i)=p(i-5,il)                                     
        else if(ida(38).gt.0.and.ida(38).le.2) then                     
            if((ida(38).eq.1.and.(ityp(il).eq.14                        
     a                          .or.iabs(npidec(il)).eq.28.or.          
     b                                iabs(npidec(il)).eq.29)).or.      
     c       (ida(38).eq.2.and.                                         
     d        (ityp(il).eq.13.or.ityp(il).eq.15                         
     e         .or.(iabs(npidec(il)).ge.43.and.iabs(npidec(il)).le.47)  
     f                 .or.iabs(npidec(il)).eq.57                       
     g                         .or.iabs(npidec(il)).ge.65))             
     h                                                        ) then    
                 idntl=issn(il)                                         
                 do 201 i=1,7                                           
  201            stork(idntl,i)=0.0                                     
                 do 202 i=3,7                                           
  202            kstor(idntl,i)=0                                       
                 kstor(idntl,1)=0                                       
                 kstor(idntl,2)=9999                                    
            end if                                                      
        else if(ida(38).eq.1.or.ida(38).eq.2.or.                        
     a                 ida(38).eq.5.or.ida(38).eq.7) then               
          if(ida(38).eq.1.or.ida(38).eq.2) then                         
            jeng=1                                                      
          else                                                          
            jeng=0                                                      
          end if                                                        
          emil=em(il)                                                   
          if(issers(jeng,ida(38),ityp(il),npidec(il),emil))then         
            idntl=issn(il)                                              
            do 204 i=1,7                                                
  204       stork(idntl,i)=0.0                                          
            do 205 i=3,7                                                
  205       kstor(idntl,i)=0                                            
            kstor(idntl,1)=0                                            
            kstor(idntl,2)=9999                                         
          end if                                                        
        end if                                                          
   80  continue                                                         
       if(ke.ne.ntotm) then                                             
         ilstar=il                                                      
         do 81 il=ilstar+1,lende(iro)                                   
          ityp(il)=-9999                                                
          do 82 i=0,3                                                   
   82     qdot(i,il)=0.0                                                
   81    continue                                                       
         do 90 iroa=iro+1,iroin                                         
          if(manip(iroa)) then                                          
            do 85 il=lstar(iroa),lende(iroa)                            
             ityp(il)=-9999                                             
             do 94 i=0,3                                                
   94        qdot(i,il)=0.0                                             
   85       continue                                                    
          end if                                                        
   90    continue                                                       
       end if                                                           
       if(lnew.eq.1) then                                               
         if(iroout.eq.mxs) then                                         
           call errex('intfac:mxs-arrays too small')                    
         else                                                           
           iroout=iroout+1                                              
           lstar(iroout)=lstan                                          
           lende(iroout)=lendn                                          
           manip(iroout)=.true.                                         
         end if                                                         
       end if                                                           
   92  continue                                                         
       do 91 iro=2,iroin                                                
        if(.not.manip(iro)) lstar(iro)=0                                
   91  continue                                                         
      return                                                            
      end                                                               
      subroutine fndq(kl1,ifle1,iqtun,istat)                            
              istat=0                                                   
              call luiflv(kl1,ifla1,iflb1,iflc1,ksp)                    
              if(iflc1.eq.0) then                                       
                if(ifla1+iflb1.eq.0) then                               
                  ifle1=-iqtun                                          
                else                                                    
                  if(ifla1.eq.iqtun) then                               
                    ifle1=iflb1                                         
                  else if(iflb1.eq.iqtun) then                          
                    ifle1=ifla1                                         
                  else                                                  
                    istat=-1                                            
                  end if                                                
                end if                                                  
              else                                                      
                itry=0                                                  
  441           continue                                                
                call bele(ifla1,iflb1,kl1)                              
                if(iabs(iqtun).lt.10) then                              
                  iflt1=ifla1                                           
                  iflt2=iflb1                                           
                else                                                    
                  iflt2=ifla1                                           
                  iflt1=iflb1                                           
                end if                                                  
                if(iqtun.eq.iflt1) then                                 
                  ifle1=iflt2                                           
                else if(itry.le.50) then                                
                  itry=itry+1                                           
                  go to 441                                             
                else                                                    
                  istat=-1                                              
                end if                                                  
              end if                                                    
      return                                                            
      end                                                               
      function barwgh(para11,ifla,iflb)                                 
       if(para11.ge.0.25) then                                          
         call errex('rewrite barwgh source code')                       
       end if                                                           
       barwgh=1.0                                                       
       ifl1=iabs(ifla)                                                  
       ifl2=iabs(iflb)                                                  
       if(ifl1.gt.10) then                                              
         ifl3=ifl1/10                                                   
         ifl1=mod(ifl1,10)                                              
       else if(ifl2.gt.10) then                                         
         ifl3=ifl2/10                                                   
         ifl1=mod(ifl2,10)                                              
       else                                                             
         return                                                         
       end if                                                           
       iequal=0                                                         
       if(ifl1.eq.ifl2) iequal=1                                        
       if(ifl1.eq.ifl3) iequal=iequal+1                                 
       if(ifl2.eq.ifl3) iequal=iequal+1                                 
       if(iequal.eq.0) then                                             
         prbwgh=1.0                                                     
       else if(iequal.eq.1) then                                        
         prbwgh=0.83333*(1.+2.*para11)/(1.+para11)                      
       else if(iequal.eq.3) then                                        
         prbwgh=2.*para11                                               
         if(ifl1.eq.3) prbwgh=2.0*prbwgh                                
       end if                                                           
       barwgh=barwgh*prbwgh                                             
      return                                                            
      end                                                               
      function kqqret(ifl1,ifl2)                                        
       ifla=iabs(ifl1)                                                  
       iflb=iabs(ifl2)                                                  
       if(int(ranf(0)*4.).ne.0) then                                    
         kqqret=10*max(iflb,ifla)+min(iflb,ifla)                        
       else                                                             
         kqqret=10*min(iflb,ifla)+max(iflb,ifla)                        
       end if                                                           
       kqqret=isign(1,ifl1)*kqqret                                      
      return                                                            
      end                                                               
      function getmas(mltp,kf,igfix,emmax)                              
       include 'scatter7'                                               
       real*4 getms4,emmx4                                              
       emmx4=emmax                                                      
       getmas=getms4(mltp,kf,igfix,emmx4)                               
      return                                                            
      end                                                               
      function getms4(mltp,kf,igfix,emmax)                              
       mtry=0                                                           
       kfa=iabs(kf)                                                     
       if(kfa.gt.100) then                                              
         minw=kfa/100                                                   
         kfa=mod(kfa,100)                                               
       else                                                             
         minw=mltp                                                      
       end if                                                           
   10  mtry=mtry+1                                                      
       if(mtry.gt.50) then                                              
         getms4=-1000.0                                                 
         return                                                         
       end if                                                           
       if(minw.eq.0) then                                               
         em=ulmass(1,kfa)                                               
       else                                                             
         maxw=minw                                                      
         if(iabs(kfa).lt.40) then                                       
           idn=idnmes(kfa)                                              
           kmltdm=kresm(1,igfix,idn,minw,maxw,kfa,em)                   
         else                                                           
           idn=idnbar(kfa)                                              
           kmltdm=kresb(igfix,idn,minw,maxw,kfa,em)                     
         end if                                                         
       end if                                                           
       if(emmax.gt.0.0) then                                            
         if(em.ge.emmax) go to 10                                       
       end if                                                           
       getms4=em                                                        
      return                                                            
      end                                                               
      subroutine sigkan                                                 
       include 'scatter7'                                               
       include 'partblk7'                                               
       real*4 plund                                                     
       common /lujets/ nlund,k(2000,2),plund(2000,5)                    
       parameter (mxs=200)                                              
       common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout               
       logical manip                                                    
       common /rop3bk/manip(mxs)                                        
       if(lstar(1).eq.0) call errex('sigkan abort, input')              
       if(lende(1)-lstar(1).gt.1) return                                
       if(nlund+2.gt.2000) call errex('sigkan:array problem')           
       lsta=lstar(1)                                                    
       le=lende(1)                                                      
       k(nlund+1,2)=124                                                 
       eminv=0.0                                                        
       do 22 im=1,4                                                     
        i=mod(im,4)                                                     
        plund(nlund+1,im)=p(i,lsta)+p(i,le)                             
        eminv=eminv+isign(1,1-2*i)*plund(nlund+1,im)**2                 
        if(qdot(0,lsta).eq.0.0) then                                    
          if(abs(decay(lsta)-x(0,lsta)).gt.1.e-4) then                  
            call errex('sigkan:qdot=0,decay.ne.x(0)')                   
          end if                                                        
          qd0ls=1.0                                                     
        else                                                            
          qd0ls=qdot(0,lsta)                                            
        end if                                                          
        if(qdot(0,le).eq.0.0) then                                      
          if(abs(decay(le)-x(0,le)).gt.1.e-4) then                      
            call errex('sigkan:qdot=0,decay.ne.x(0)')                   
          end if                                                        
          qd0le=1.0                                                     
        else                                                            
          qd0le=qdot(0,le)                                              
        end if                                                          
        plund(nlund+2,im)=0.5*(x(i,lsta)+                               
     a                         qdot(i,lsta)/qd0ls*                      
     b                            (decay(lsta)-x(0,lsta))+              
     c                         x(i,le)+                                 
     d                         qdot(i,le)/qd0le*                        
     e                            (decay(le)-x(0,le)))                  
   22  continue                                                         
       plund(nlund+1,5)=sqrt(eminv)                                     
       if(plund(nlund+1,5).gt.1.0) return                               
       manip(1)=.true.                                                  
       nlund=nlund+2                                                    
      return                                                            
      end                                                               
