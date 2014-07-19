CPAT,STRINGA  .                                                         
      subroutine string                                                 
      include 'scatter7'                                                
      external ludata                                                   
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version  rqmd 2.4 -- 30-jan-98'/                     
      data pronam/'module: s t r i n g'/                                
      data ttvers/.true./                                               
      dimension ptot(0:3),dr(0:3),qlk(0:3),xold(0:3),xnew(0:3)          
      dimension p1(0:3),p2(0:3),dxqb(3,0:3),xqj(0:3)                    
      real*4  xform(0:3),pform(0:3)                                     
      real*4 bex,bey,bez,x1lu,x3lu,psi,vink,thetlu,philu                
      real*4 xpos,plund,par,dpar,cbr,prtgam,emst,emda,emdb              
      real*4 angmxv,angeff                                              
      real*4 paraqq,paraq,paraqs                                        
      dimension pq(0:3),pqbar(0:3),ptst(0:3),veloc(0:3)                 
      dimension xpos(0:5,100),iflold(2)                                 
      include 'partblk7'                                                
      common /lujets/ nlund,k(2000,2),plund(2000,5)                     
      common/ludat1/mst(40),par(80)                                     
      common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)              
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
      common /lupobk/ xpos,ilund                                        
      common /luopt1/                                                   
     a         iopt(20),kfstr,icqq,mbfix,ideltq,ibbcol,                 
     b         mmult(100),                                              
     a         iprbcq,ilead,iback,ibrtag,icqbar,idlj,klmold,            
     b         klbold,isot,misot,lbreak(100),emst,emda,emdb             
      common /zdisbk/ paraqq,paraq,paraqs                               
      common /su3mix/ angmxv,angeff                                     
      common /prinbk/ iprin                                             
      save angsto,ssbsp1,ssbsp2                                         
      data drind/ 0.3/                                                  
      data ptkl40/ 0.6/                                                 
      data irk /1/                                                      
      data ishyp,ifhyp /0,0/                                            
      dimension nsco(9)                                                 
      data nsco / 1,1,0,1,2,1,1,5,6/                                    
      real*4 par2,par5,par8,par12,par14,par15,par51,par52               
      logical issers                                                    
      data  klchmx/17/                                                  
      save   nstp,mst10,mst11,                                          
     a        par1,par2,par5,par8,par12,par14,par15,par51,par52,gprd    
      entry ldinit                                                      
       nstp=0                                                           
       nlund=0                                                          
       tfrmhd=0.                                                        
       tfrmcq=0.                                                        
       nfrmhd=0                                                         
       nfrmcq=0                                                         
       write(8,*)'rqmd version including  string decay'                 
       if(.not.switch(15)) then                                         
         write(8,*) 'modifications of the jetset default parameters'    
         write(8,*) 'resonance masses are smeared out'                  
         mst(8)=1                                                       
         write(8,*) 'mst(8)=',mst(8)                                    
         mst(10)=0                                                      
         write(8,*) 'mst(10)=',mst(10)                                  
         write(8,*) 'string endpoints get transverse momenta'           
         mst(11)=1                                                      
         write(8,*) 'mst(11)=',mst(11)                                  
         par(5)=0.9                                                     
         write(8,*)                                                     
     a   'enhanced b m anti-b/ b anti-b configuration par(5)=',par(5)   
         par(1)=0.10                                                    
         par1=par(1)                                                    
         write(8,*)                                                     
     a  'diquark suppression parameter: par(1)=',par(1)                 
         par(77)=1.0                                                    
         if(par(77).gt.0.0)                                             
     a    write(8,*)                                                    
     b    'suppression of lowlying baryon resonance nonet',par(77)      
         par(78)=0.4                                                    
         write(8,*)                                                     
     a       'suppression of sigma*(1385)  from qq-q:',par(78)          
         par(79)=1.0                                                    
         if(par(79).lt.1.0) then                                        
           write(8,*)                                                   
     a       'ssbar suppression if diquark breaks:',par(79)             
         end if                                                         
         par(80)=0.25                                                   
         write(8,*)                                                     
     a       'eta(prime) suppression:',par(80)                          
         par(15)=par(12)                                                
         write(8,*)                                                     
     a       'opposite pt for quarks in splitting qq, par(15)=par(12)'  
       end if                                                           
       idb(19)=-idb(19)                                                 
       idb(43)=-idb(43)                                                 
       idb(44)=-idb(44)                                                 
       idb(45)=-idb(45)                                                 
       idb(57)=-idb(57)                                                 
       if(ida(41).eq.1) then                                            
         prtgam(35)=-1.0                                                
         idb(35)=-idb(35)                                               
       end if                                                           
       idb(46)=-idb(46)                                                 
       idb(47)=-idb(47)                                                 
       idb(70)=-idb(70)                                                 
       if(switch(9)) then                                               
         prtgam(28)=-1.0                                                
         idb(28)=-idb(28)                                               
         prtgam(29)=-1.0                                                
         idb(29)=-idb(29)                                               
         prtgam(34)=-1.0                                                
         idb(34)=-idb(34)                                               
         if(idb(35).gt.0) idb(35)=-idb(35)                              
         prtgam(65)=-1.0                                                
         idb(65)=-idb(65)                                               
         prtgam(66)=-1.0                                                
         idb(66)=-idb(66)                                               
         prtgam(67)=-1.0                                                
         idb(67)=-idb(67)                                               
       end if                                                           
       if(ida(49).eq.1.and.mod(ida(16),2).eq.0) then                    
         iopt(8)=1                                                      
       else                                                             
         iopt(8)=0                                                      
       end if                                                           
       mst10=mst(10)                                                    
       mst11=mst(11)                                                    
       par2=par(2)                                                      
       par5=par(5)                                                      
       par8=par(8)                                                      
       par12=par(12)                                                    
       par14=par(14)                                                    
       par15=par(15)                                                    
       par51=par(51)                                                    
       par52=par(52)                                                    
       if(da(4).ne.1.) then                                             
         write(8,*) 'warning: da(4).ne.1. do you know what you do?'     
         tprd=tprd*da(4)                                                
       end if                                                           
       if(ida(29).ne.3) then                                            
         ida(44)=0                                                      
         ida(45)=0                                                      
         ida(46)=0                                                      
         ida(47)=0                                                      
       else                                                             
         ida(21)=1                                                      
       end if                                                           
       if(ida(10).ne.0) ida(47)=0                                       
       if(ida(45).eq.0) ida(47)=0                                       
       if(ida(21).eq.1) then                                            
          gmprop=-abs(gmprop)                                           
       else                                                             
          ida(18)=0                                                     
       end if                                                           
       if(tprd.gt.1.e-30) then                                          
         gprd=1./tprd*hc/pi2                                            
       else                                                             
         gprd=1.e+29                                                    
       end if                                                           
       iopt(2)=ida(16)                                                  
       if(ida(16).ge.2) then                                            
         ida(29)=3                                                      
       end if                                                           
       if(mod(iopt(2),2).eq.0) then                                     
         if(tpar2.eq.cvalu0) then                                       
           write(8,*)                                                   
     a      'hadron formation=arithmetic mean of string break points'   
         else if(tpar2.eq.0.5) then                                     
           write(8,*)                                                   
     a        'hadron formation=yo-yo meeting point (in the average)'   
         end if                                                         
       end if                                                           
       if(ida(51).eq.2) then                                            
         angmxv=da(9)*pi2/360.0                                         
         angeff=da(10)*pi2/360.0                                        
       else if(ida(51).eq.1) then                                       
         angmxv=da(9)*pi2/360.0                                         
         angeff=angmxv                                                  
       else if(ida(51).eq.0) then                                       
         angmxv=35.3*pi2/360.0                                          
         angeff=angmxv                                                  
       else                                                             
         call  errex('input  value  for ida(51) not  allowed')          
       end if                                                           
       angsto=angeff                                                    
       paraqq=1.0                                                       
       paraq=3.0                                                        
       paraqs=1.0                                                       
       ssbsp2=1.0                                                       
       iopt(12)=ida(53)                                                 
      return                                                            
      entry jetdec(ind,ntag)                                            
        if(ttvers) then                                                 
          write(8,*)'-------------- version number report -------------'
          write(8,*)pronam                                              
          write(8,*)vernum                                              
          write(8,*)'--------------------------------------------------'
          ttvers=.false.                                                
        end if                                                          
       ievnt=lastcl(ind)                                                
       ident=issn(ind)                                                  
       itpind=ityp(ind)                                                 
       npind=npidec(ind)                                                
       emind=em(ind)                                                    
       call oldnew(1,itpind,npind,emind,kmult,ltyp)                     
       iprin=0                                                          
       kopt1=0                                                          
       ibar1=0                                                          
       gamind=p(0,ind)/emind                                            
       if(tlast(ind)-1.e-2.gt. decay(ind))  then                        
         write(8,*) 'ind,issn,decay,x(0,ind),tlast'                     
         write(8,*)  ind,issn(ind),decay(ind),x(0,ind),tlast(ind)       
         call errex('wrong tlast in jetdec')                            
       end if                                                           
       if(qdot(0,ind).gt.0.) then                                       
         do 107 i=0,3                                                   
  107    veloc(i)=qdot(i,ind)/qdot(0,ind)                               
       else                                                             
         if(ifndnx.eq.0.and.tlast(ind).ne.decay(ind)) then              
           write(8,*)                                                   
     a        'string ident=',ident ,'ind=',ind,'type=',ltyp,           
     a                    npind                                         
           call errex('jetdec: qdot=0 ????')                            
         end if                                                         
         do 108 i=0,3                                                   
  108    veloc(i)=0.0                                                   
       end if                                                           
       if(decay(ind)-tlast(ind).gt.1.e-3) then                          
         iftalt=1                                                       
       else                                                             
         iftalt=0                                                       
       end if                                                           
       delttr=decay(ind)-x(0,ind)                                       
       do 211 i=0,3                                                     
        x(i,ind)=x(i,ind)+veloc(i)*delttr                               
        xold(i)=x(i,ind)                                                
        dxqb(1,i)=0.0                                                   
 211   continue                                                         
       iprbcq=0                                                         
       icqend=0                                                         
       icqsto=0                                                         
       if(xold(0).lt.tdtg(ident).and.itdth(2,ident).ne.0) then          
         ilogo=itdth(2,ident)                                           
         iprbcq=iabs(ilogo)/100                                         
         icqend=isign(1,ilogo)*mod(iabs(ilogo),100)                     
         icqsto=icqend                                                  
         if(iprbcq.ne.1.and.iprbcq.ne.2) then                           
           call errex('iprbcq cannot be interpreted in jetdec')         
         end if                                                         
         do 199 i=0,3                                                   
  199    xqj(i)=xold(i)+veloc(i)*(tdtg(ident)-xold(0))                  
       else if(itdth(2,ident).ne.0) then                                
          call ofshel(10,ind,ident)                                     
       end if                                                           
       ltdt3=itdth(3,ident)                                             
       idfhad=iprbcq                                                    
       kpartr=0                                                         
       kpartm=0                                                         
       tdtg(ident)=0.                                                   
       itdth(1,ident)=0                                                 
       itdth(2,ident)=0                                                 
       itdth(3,ident)=0                                                 
       ijet=0                                                           
       nll=0                                                            
       do 14 nl=1,nlund                                                 
        kh=mod(k(nl,1),100000)                                          
        ks=k(nl,1)/100000                                               
        if(ks.eq.4) then                                                
          if(kh.eq.ident ) then                                         
            if(ijet.ne.0) call errex('not unique jetsystem for ind')    
            ijet=1                                                      
            klflag=k(nl,2)                                              
            klchar=k(nl+1,2)                                            
            p0h=plund(nl,4)+plund(nl+1,4)                               
            bx=(plund(nl,1)+plund(nl+1,1))/p0h                          
            by=(plund(nl,2)+plund(nl+1,2))/p0h                          
            bz=(plund(nl,3)+plund(nl+1,3))/p0h                          
            emda=plund(nl,5)                                            
            emdb=plund(nl+1,5)                                          
            do  12 ii=1,4                                               
             i=mod(ii,4)                                                
             pq(i)=plund(nl,ii)                                         
             pqbar(i)=plund(nl+1,ii)                                    
   12       continue                                                    
            emjtsm=(pq(0)+pqbar(0))**2                                  
            do 46 i=1,3                                                 
   46       emjtsm=emjtsm-(pq(i)+pqbar(i))**2                           
            emjtsm=sqrt(emjtsm)                                         
            mst(1)=nl                                                   
            mst(2)=nl+1                                                 
            bex=-bx                                                     
            bey=-by                                                     
            bez=-bz                                                     
            call lurobo(0.,0.,bex,bey,bez)                              
            mst(1)=0                                                    
            mst(2)=0                                                    
            pabs=sqrt(plund(nl,1)*plund(nl,1)+                          
     a                   plund(nl,2)*plund(nl,2)+                       
     b                   plund(nl,3)*plund(nl,3))                       
            thet=acos(max(-cvalu1,min(cvalu1,plund(nl,3)/pabs)))        
            if(plund(nl,2).eq.0.0.and.plund(nl,1).eq.0.) then           
              phi=pi2*ranf(0)                                           
            else                                                        
              phi=atan2(plund(nl,2),plund(nl,1))                        
            end if                                                      
          end if                                                        
        end if                                                          
        if(ijet.eq.0.or.ijet.gt.2) then                                 
          nll=nll+1                                                     
          do 11 i=1,5                                                   
  11      plund(nll,i)=plund(nl,i)                                      
          k(nll,1)=k(nl,1)                                              
          k(nll,2)=k(nl,2)                                              
        else                                                            
          ijet=ijet+1                                                   
        end if                                                          
  14   continue                                                         
       if(ijet.ne.3) then                                               
         write(8,*)'issn(ind),type,kmult,em'                            
         write(8,*) issn(ind),ltyp,kmult,emind                          
         write(8,*)'ijet=',ijet,nlund                                   
         call errex('wrong ijet in jetdec,14')                          
       end if                                                           
       nlund=nlund-2                                                    
       nsave=nlund                                                      
       if(nlund+6.ge.2000)                                              
     a       call errex('lund storage too small, abort in string')      
       pp=p(0,ind)*p(0,ind)                                             
       do 78 i=1,3                                                      
  78   pp=pp-p(i,ind)*p(i,ind)                                          
       emjs=sqrt(pp)                                                    
       iopt(7)=0                                                        
       iopt(4)=0                                                        
       iopt(10)=0                                                       
       iopt(11)=0                                                       
       irever=1                                                         
       mbfix=0                                                          
       mbtag=0                                                          
       ivzklc=isign(1,klchar)                                           
       klchar=iabs(klchar)                                              
       klmold=ivzklc*mod(klchar,100000)/100                             
       klbold=klchar/100000                                             
       klchar=mod(klchar,100)                                           
       lmult=0                                                          
       ltypa=iabs(ltyp)                                                 
       mbconr=0                                                         
       if(kmult.gt.0.and.klchar.le.3) then                              
         iopt(13)=0                                                     
         if(ltypa.gt.40) then                                           
           klchar=10                                                    
           klflag=0                                                     
         end if                                                         
       else                                                             
         iopt(13)=ida(55)                                               
         mlklmo=iabs(klmold)/100                                        
         mlklbo=iabs(klbold)/100                                        
         if(mlklmo.eq.9.or.mlklbo.eq.14) then                           
           mbconr=1                                                     
           iopt(13)=-1                                                  
         end if                                                         
         if(ida(55).le.0) iopt(11)=1                                    
       end if                                                           
       if(ltypa.eq.40.or.iprbcq.ne.0) then                              
         isot=-1                                                        
       else if(klchar.eq.3) then                                        
         isot=isosp(ltypa)                                              
         misot=isign(1,ltyp)*isosp3(ltypa)                              
       else                                                             
         isot=-1                                                        
       end if                                                           
       if(klchar.ge.0.and.klchar.le.3) then                             
       else if(klchar.ge.5.and.klchar.le.klchmx) then                   
         if(klchar.eq.5.or.klchar.eq.6.or.klchar.eq.11.or.              
     a      klchar.eq.13.or.klchar.eq.15) then                          
           mbfix=-klchar                                                
         else if(klchar.eq.7) then                                      
           mbfix=24                                                     
         else if(klchar.eq.8) then                                      
           mbfix=31                                                     
         else if(klchar.eq.9) then                                      
           mbfix=41                                                     
         else if(klchar.eq.12) then                                     
           mbfix=21                                                     
         else if(klchar.eq.14) then                                     
           idcod=kmult                                                  
           if(idcod.le.0.or.idcod.gt.9) then                            
             call errex('jetdec: idcod out of range')                   
           end if                                                       
           nsc=nsco(idcod)                                              
           mbfix=100*nsc+21                                             
         else if(klchar.eq.16) then                                     
           mbfix=44                                                     
         else if(klchar.eq.17) then                                     
           mbfix=61                                                     
         end if                                                         
         call luiflv(ltyp,mfl1,mfl2,mfl3,ksp)                           
         if(iopt(11).eq.1.and.mbfix.gt.0) iopt(11)=0                    
         mfl1=iabs(mfl1)                                                
         mfl2=iabs(mfl2)                                                
         mfl3=iabs(mfl3)                                                
         if(mfl1.ne.3)mfl1=0                                            
         if(mfl2.ne.3)mfl2=0                                            
         if(mfl3.ne.3)mfl3=0                                            
         mflt=mfl1+mfl2+mfl3                                            
         em2nur=2.8+(mflt/3)*0.15                                       
         if(mflt.eq.0) then                                             
           if(iabs(ltyp).ge.61.and.iabs(ltyp).le.64) then               
             iopt(10)=1                                                 
           else                                                         
             iopt(10)=2                                                 
           end if                                                       
         else if(mflt.eq.3) then                                        
           if(iabs(ltyp).eq.57) then                                    
             iopt(10)=3                                                 
           else                                                         
             iopt(10)=4                                                 
           end if                                                       
         else if(mflt.eq.6) then                                        
           iopt(10)=5                                                   
         else if(mflt.eq.9) then                                        
           iopt(10)=6                                                   
         else                                                           
           call errex('jetdec:unknown baryon resonance, mflt')          
         end if                                                         
         if(iopt(10).eq.3.or.iopt(10).eq.4) then                        
           if(mbfix.ne.0)then                                           
           else if(emjs.lt.1.60)then                                    
             mbfix=99                                                   
           end if                                                       
         else if(iopt(10).eq.5)then                                     
           if(emjs.lt.1.75.or.kmult.gt.0) then                          
             mbfix=99                                                   
           end if                                                       
         else if(iopt(10).eq.6)then                                     
           if(emjs.lt.1.90.or.kmult.gt.0) then                          
             mbfix=99                                                   
           end if                                                       
         end if                                                         
       else                                                             
         write(8,*) 'klchar=',klchar                                    
         call errex('jetdec:unknown klchar')                            
       end if                                                           
       if(ida(51).eq.2.and.                                             
     a           (klchar.ge.5.and.klchar.le.klchmx)) then               
         angeff=angmxv                                                  
       else                                                             
         angeff=angsto                                                  
       end if                                                           
       do 79 i=0,3                                                      
   79  ptot(i)=p(i,ind)                                                 
       bxnew=ptot(1)/ptot(0)                                            
       bynew=ptot(2)/ptot(0)                                            
       bznew=ptot(3)/ptot(0)                                            
       ntry=0                                                           
       iokay=0                                                          
       iopt(5)=0                                                        
       icqq=0                                                           
       iopt(6)=0                                                        
       kl40=0                                                           
       if(klchar.eq.1) then                                             
         if(ranf(0).lt.da(20)) then                                     
           iopt(5)=1                                                    
         end if                                                         
       else if(klchar.eq.2) then                                        
         iopt(2)=mod(ida(16),2)                                         
       else if(klchar.eq.3) then                                        
         if(iprbcq.ne.0) klchar=0                                       
       else if(klchar.ge.5.and.klchar.le.klchmx) then                   
         iopt(2)=mod(ida(16),2)                                         
         zlimit=(em2nur/emjs)                                           
         if(mbconr.eq.1) then                                           
           iopt(7)=2                                                    
         else if(ranf(0).gt.zlimit) then                                
           iopt(7)=2                                                    
         else                                                           
           iopt(7)=1                                                    
           if(icqsto.eq.0.and.iopt(11).eq.0) icqend=0                   
         end if                                                         
         if(icqend.eq.0) icqend=klflag                                  
       else if(ltypa.eq.40) then                                        
         iopt(2)=mod(ida(16),2)                                         
         kl40=1                                                         
         rtsea=denspp(ind)                                              
                                                                        
       end if                                                           
       if(iftalt.eq.1.and.iprbcq.eq.0) then                             
         iopt(2)=mod(iopt(2),2)                                         
       end if                                                           
       emst=emjs                                                        
       if(klchar.eq.2)  then                                            
         iopt(6)=2                                                      
       else if(kl40.eq.1) then                                          
         iopt(6)=2                                                      
       end if                                                           
       if(ida(51).eq.2) then                                            
         if(klchar.ge.5.and.klchar.le.klchmx) then                      
           iopt(9)=1                                                    
         else                                                           
           iopt(9)=2                                                    
         end  if                                                        
       else if(ida(51).eq.1) then                                       
         iopt(9)=1                                                      
       else if(ida(51).eq.0) then                                       
         iopt(9)=0                                                      
       end if                                                           
       if(kl40.eq.1) then                                               
         iopt(14)=0                                                     
       else                                                             
         iopt(14)=1                                                     
       end if                                                           
       fist=0.0                                                         
   17  ntry=ntry+1                                                      
       ideltq=0                                                         
       if(icqq.ne.2) mbcqno=0                                           
       nlund=nsave                                                      
       emst=emjs                                                        
       if((abs(bxnew-bx).gt.cvalu6).or.                                 
     b            (abs(bynew-by).gt.cvalu6).or.                         
     c               (abs(bznew-bz).gt.cvalu6).or.                      
     d                  (abs(emjs-emjtsm).gt.cvalu6)) then              
         nlcor=nlund                                                    
         mst(1)=nlcor+1                                                 
         mst(2)=nlcor+2                                                 
         plund(nlcor+1,4)=pq(0)*emjs/emjtsm                             
         plund(nlcor+2,4)=pqbar(0)*emjs/emjtsm                          
         do  8 i=1,3                                                    
          plund(nlcor+1,i)=pq(i)*emjs/emjtsm                            
          plund(nlcor+2,i)=pqbar(i)*emjs/emjtsm                         
    8    continue                                                       
         nlund=nlund+2                                                  
         bex=-bx                                                        
         bey=-by                                                        
         bez=-bz                                                        
         call lurobo(0.,0.,bex,bey,bez)                                 
         bex=bxnew                                                      
         bey=bynew                                                      
         bez=bznew                                                      
         call lurobo(0.,0.,bex,bey,bez)                                 
         mst(1)=0                                                       
         mst(2)=0                                                       
         nlund=nlcor                                                    
         do  9 ii=1,4                                                   
          i=mod(ii,4)                                                   
          pq(i)=plund(nlund+1,ii)                                       
          pqbar(i)=plund(nlund+2,ii)                                    
    9    continue                                                       
         bx=bxnew                                                       
         by=bynew                                                       
         bz=bznew                                                       
         emjtsm=emjs                                                    
       end if                                                           
       mst(7)=0                                                         
       mst(11)=mst11                                                    
       if(ntry.gt.30.and.icqsto.eq.0) then                              
         if(icqend.ne.0)  then                                          
           icqend=0                                                     
         end if                                                         
         iokay=0                                                        
       endif                                                            
       if(ntry.gt.40) then                                              
           write(8,*) 'iprbcq,icqend,ifla,iflb',ntry                    
           write(8,*)  iprbcq,icqend,ifla,iflb,kl40,iokay               
       endif                                                            
       if(ntry.gt.50) then                                              
             write(8,*) 'no fragmentation of jetsystem in string',      
     a             ' possible ltyp=',ltyp,'emjs=',emjs,issn(ind)        
             call errex('error occurred in string')                     
       endif                                                            
       ilead=0                                                          
       iback=0                                                          
       if(kl40.eq.1)  iokay=0                                           
       if(iokay.eq.0) call bele(ifla,iflb,ltyp)                         
       if(kl40.eq.1) then                                               
         if(iabs(ifla).gt.10) then                                      
           iflabs=iabs(ifla)                                            
           mfl1=mod(iflabs,10)                                          
           mfl2=iflabs/10                                               
           if(mfl1.ne.3)mfl1=0                                          
           if(mfl2.ne.3)mfl2=0                                          
           mflt=mfl1+mfl2                                               
           demjs=(mflt/3)*2.*0.15                                       
           if(emjs.lt.2.2+demjs) go to 17                               
           if(emjs.lt.2.8) then                                         
             prmx=pcmsr(emjs,emnuc,emnuc)                               
             if(ranf(0).gt.1.-exp(-(prmx/(1.41*ptkl40))**2)) go to 17   
           end if                                                       
         else if(iabs(ifla).eq.3) then                                  
           if(emjs.lt.1.0) go to 17                                     
           if(emjs.lt.2.0) then                                         
             prmx=pcmsr(emjs,emkap,emkap)                               
             if(ranf(0).gt.1.-exp(-(prmx/(1.41*ptkl40))**2)) go to 17   
           end if                                                       
         end if                                                         
       end if                                                           
       ibbcol=0                                                         
       if(klchar.ge.5.and.klchar.le.klchmx) then                        
       else                                                             
         if(kl40.eq.1.or.klchar.ge.1.or.icqsto.ne.0) then               
         else if(iabs(iflb).gt.10) then                                 
           ibbcol=1                                                     
           mfl1=iabs(iflb)/10                                           
           mfl2=mod(iabs(iflb),10)                                      
           mfl3=iabs(ifla)                                              
           if(mfl3.ne.3) then                                           
             if(mfl1.eq.3.and.ranf(0).gt.0.6) then                      
               iokay=0                                                  
               go to 17                                                 
             end if                                                     
             if(mfl2.eq.3.and.ranf(0).gt.0.6) then                      
               iokay=0                                                  
               go to 17                                                 
             end if                                                     
           end if                                                       
         end if                                                         
       end if                                                           
       if(iabs(iflb).gt.10) then                                        
         ibstri=1                                                       
       else                                                             
         ibstri=0                                                       
       end if                                                           
       if(iopt(5).eq.1) then                                            
         if(icqend.eq.0) then                                           
           icqend=iflb                                                  
           iprbcq=2                                                     
         end if                                                         
       end if                                                           
       if(iabs(ifla).gt.10) then                                        
         if(kl40.eq.1) then                                             
         else if(icqq.eq.1.or.icqq.eq.2) then                           
         else                                                           
           write(8,*)                                                   
     a   'some counters like idlj do not always work for iabs(ifla)>10' 
           call errex('problem in string decay')                        
         end if                                                         
       end if                                                           
       qqsupp=1.0                                                       
       par(1)=par1*qqsupp                                               
       par(14)=par14                                                    
       par(12)=par12                                                    
       ifl1=mod(iabs(iflb),10)                                          
       ifl2=iabs(iflb)/10                                               
       if(ida(20).ne.0) then                                            
         probqq=0.                                                      
       else if(iopt(5).eq.0.and.iprbcq.ne.0) then                       
         if(iabs(iflb).gt.10) then                                      
           probqq=1.                                                    
         else                                                           
           probqq=0.                                                    
         end if                                                         
       else if(iopt(5).eq.1) then                                       
         probqq=0.                                                      
       else if(iopt(4).eq.1) then                                       
         probqq=0.                                                      
       else if(klchar.eq.3.and.ranf(0).lt.0.66667) then                 
         probqq=0.                                                      
       else if(ifl2.ge.ifl1) then                                       
         probqq=1.-da(18)                                               
       else                                                             
         probqq=1.-da(19)                                               
       end if                                                           
       iopt(3)=0                                                        
       iopt(4)=0                                                        
       if(icqq.gt.0) then                                               
       else if(iopt(5).eq.1) then                                       
       else if(iopt(7).eq.1) then                                       
         iopt(3)=1                                                      
       else if(kl40.eq.1) then                                          
         par(12)=ptkl40                                                 
       else if(iabs(iflb).gt.10.and.ranf(0).lt.probqq) then             
         iopt(3)=1                                                      
         if(iprbcq.eq.0) then                                           
           par(51)=paraqq                                               
           par(52)=paraq                                                
         else                                                           
           par(51)=paraqs                                               
           par(52)=paraqs                                               
         end if                                                         
       else if(iprbcq.ne.0) then                                        
       else if(iabs(iflb).lt.10) then                                   
         iopt(3)=1                                                      
         par(51)=paraqs                                                 
         par(52)=paraqs                                                 
       else if(iabs(iflb).gt.10.and.ida(20).eq.0) then                  
         iopt(4)=1                                                      
         par(51)=paraqq                                                 
         par(52)=paraq                                                  
       end if                                                           
       if(iabs(iflb).gt.10.and.iopt(3).eq.1                             
     a                           .and.iopt(7).eq.0) then                
         iopt(6)=3                                                      
       end if                                                           
       if(ida(45).eq.0) then                                            
         iopt(2)=mod(iopt(2),2)                                         
         iopt(3)=0                                                      
         iopt(5)=0                                                      
         iopt(6)=0                                                      
         iopt(7)=0                                                      
         iopt(9)=0                                                      
         mst(10)=1                                                      
       end if                                                           
       if(iopt(5).eq.1) mst(11)=0                                       
       if(iopt(4).eq.1)  mst(10)=1                                      
       if(iopt(7).eq.1)  mst(11)=0                                      
       if((iopt(10).ge.3.and.iopt(10).le.6).and.                        
     a           emjs.gt.2.8.and.icqend.ne.0.and.                       
     a             icqsto.eq.0.and.iopt(4).eq.1.and.                    
     a             iopt(7).ne.1.and.                                    
     a                     ranf(0).lt.(4.0/emjs)**3)  then              
         iokay=0                                                        
         icqend=0                                                       
         go to 17                                                       
       end if                                                           
       ibrtag=0                                                         
       icqbar=0                                                         
       jctvrt=0                                                         
       jfla=0                                                           
       jflb=0                                                           
       if(iprbcq.ne.0.or.                                               
     a      (icqend.ne.0.and.klchar.ge.5.and.klchar.le.klchmx           
     b                                   .and.icqq.ne.2)) then          
         if(iprbcq.eq.1) then                                           
           if(ifla.ne.icqend) then                                      
             if(iabs(ifla).lt.10.and.iabs(iflb).lt.10) then             
               if(ifla+iflb.eq.0) then                                  
                 if(iabs(icqend).gt.10) then                            
                   write(8,*) 'icqend,ifla,iflb,type'                   
                   write(8,*)  icqend,ifla,iflb,ltyp                    
                   call errex(                                          
     a                     'jetdec:icqend should not be a diquark')     
                 end if                                                 
                    ifla=icqend                                         
                      iflb=-icqend                                      
               else                                                     
                 ifl1=ifla                                              
                 ifl2=iflb                                              
                 ifla=ifl2                                              
                 iflb=ifl1                                              
                 if(ifla.ne.icqend) then                                
                   write(8,*) ltyp,icqend,ifla, iflb                    
                   call errex('wrong icqend in jetdec')                 
                 end if                                                 
               end if                                                   
             else                                                       
               if(iabs(icqend).le.10.and.iabs(ifla).gt.10) then         
                    call errex('iabs(ifla).gt.10 notallowed in jetdec') 
               else if(iabs(icqend).le.10.and.iabs(ifla).le.10) then    
                 go to 17                                               
               else                                                     
                 call errex('iabs(ifla).gt.10 notallowed in jetdec')    
               end if                                                   
             end if                                                     
           end if                                                       
         else                                                           
           if(iflb.ne.icqend) then                                      
             if(iabs(ifla).lt.10.and.iabs(iflb).lt.10) then             
               if(ifla+iflb.eq.0) then                                  
                 iflb=icqend                                            
                 ifla=-icqend                                           
               else                                                     
                 ifl1=ifla                                              
                 ifl2=iflb                                              
                 ifla=ifl2                                              
                 iflb=ifl1                                              
                 if(iflb.ne.icqend) then                                
                   write(8,*) ltyp,icqend,ifla,iflb                     
                   call errex('wrong icqend in jetdec')                 
                 end if                                                 
               end if                                                   
             else                                                       
               if(iabs(icqend).le.10.and.iabs(iflb).gt.10) then         
                 if1=iflb/10                                            
                 if2=iflb-10*if1                                        
                 if(icqend.ne.if1.and.icqend.ne.if2) then               
                   go to 17                                             
                 else                                                   
                   jctvrt=1                                             
                   if(icqend.eq.if1) then                               
                     icqbar=if2                                         
                   else                                                 
                     icqbar=if1                                         
                   end if                                               
                 end if                                                 
               else if(iabs(icqend).le.10.and.iabs(iflb).le.10) then    
                 go to 17                                               
               else                                                     
                 if1=iflb/10                                            
                 if2=iflb-10*if1                                        
                 if3=icqend/10                                          
                 if4=icqend-10*if3                                      
                 if(if3.eq.if1) then                                    
                   if(if4.ne.if2) go to 17                              
                 else if(if3.eq.if2) then                               
                   if(if4.ne.if1) go to 17                              
                 else                                                   
                   go to 17                                             
                 end if                                                 
                 iflb=icqend                                            
               end if                                                   
             end if                                                     
           end if                                                       
         end if                                                         
         if(ida(20).eq.0.and.iopt(2).ge.2.and.iopt(5).eq.0.and.         
     a                             klchar.eq.0.and.icqq.eq.0) then      
           if(iabs(icqend).gt.10.and.jctvrt.eq.0) then                  
             if(icqend.ne.iflb)                                         
     a              call errex('iflb.ne.icqend in string')              
             if(mst(10).ne.0)                                           
     a              call errex('mst(10).ne.0,ida(20)=0 conflicting')    
             icqq=2                                                     
             iprbcq=0                                                   
             ifl1=isign(1,iflb)*mod(iabs(iflb),10)                      
             ifl2=isign(1,iflb)*iabs(iflb)/10                           
             if(ranf(0).gt.0.5) then                                    
               call iswap(ifl1,ifl2)                                    
             end if                                                     
             iflh=ifl1                                                  
             if(iabs(iflh).gt.iabs(ifla)) then                          
               call iswap(iflh,ifla)                                    
             end if                                                     
             if(ranf(0).lt.0.75) then                                   
               ifla=10*ifla+iflh                                        
             else                                                       
               ifla=10*iflh+ifla                                        
             end if                                                     
             iflb=ifl2                                                  
             icqend=ifl1                                                
           else if(iabs(iflb).gt.10.and.jctvrt.eq.1) then               
             if(iprbcq.ne.2)                                            
     a              call errex('iprbcq.ne.2,jctvrt=1 in string')        
             icqq=1                                                     
             iopt(3)=0                                                  
             ifl1=isign(1,iflb)*mod(iabs(iflb),10)                      
             ifl2=isign(1,iflb)*iabs(iflb)/10                           
             if(ifl1.eq.icqend) then                                    
               call iswap(ifl1,ifl2)                                    
             end if                                                     
             if(ifl2.ne.icqend)                                         
     a            call errex('jetdec,icqq=1:where is icqend')           
             zrand=ranf(0)                                              
             if((iabs(ifl1).gt.iabs(ifla).and.zrand.gt.0.75)            
     a          .or.(iabs(ifl1).lt.iabs(ifla).and.zrand.le.0.75)        
     b                                                      ) then      
               call iswap(ifl1,ifla)                                    
             end if                                                     
             ifla=10*ifl1+ifla                                          
             iflb=icqend                                                
           end if                                                       
         else if(iopt(7).ne.0.and.icqsto.ne.0) then                     
           if(iabs(icqend).gt.10) then                                  
             iprbcq=0                                                   
             ideltq=1                                                   
             ifl1=isign(1,iflb)*mod(iabs(iflb),10)                      
             ifl2=isign(1,iflb)*iabs(iflb)/10                           
             iflqb=isign(1,icqsto)*mod(iabs(icqsto),10)                 
             if(ifl1.ne.iflqb)                                          
     a           call iswap(ifl1,ifl2)                                  
             iflb=10*ifla+ifl1                                          
             ifla=ifl2                                                  
             jfla=ifl2                                                  
             jflb=ifl1                                                  
           else                                                         
             iopt(3)=0                                                  
             iopt(4)=0                                                  
             mst(10)=0                                                  
             mbcqno=1                                                   
             ifl1=isign(1,iflb)*mod(iabs(iflb),10)                      
             ifl2=isign(1,iflb)*iabs(iflb)/10                           
             if(ifl1.eq.icqsto) call iswap(ifl1,ifl2)                   
             if(ifl2.ne.icqend)                                         
     a            call errex('jetdec,where is icqsto')                  
             iflb=10*ifl1+ifla                                          
             ifla=icqsto                                                
             iprbcq=3-iprbcq                                            
             jfla=ifla                                                  
             jflb=iflb                                                  
             if(iprbcq.ne.1.and.iprbcq.ne.2)                            
     a       call errex('cq-qq resonance: unexpected iprbcq')           
           end if                                                       
         end if                                                         
       end if                                                           
       iflold(1)=ifla                                                   
       iflold(2)=iflb                                                   
       if(jfla.ne.0) iflold(1)=jfla                                     
       if(jflb.ne.0) iflold(2)=jflb                                     
       if(npind.eq.4.and.iabs(ltyp).gt.40) then                         
         par(2)=0.001                                                   
       else                                                             
         par(2)=par2                                                    
       end if                                                           
       kfstr=ltyp                                                       
       iokay=1                                                          
       ktori=2                                                          
       kt=0                                                             
       call torsts(emjs,ltyp,iflb,ifla,x1,x3,kt)                        
       if(kt.eq.-1) then                                                
         if(kl40.eq.1) then                                             
           par(8)=0.0                                                   
           kt=2                                                         
           ktori=-2                                                     
         else if(iopt(7).eq.0) then                                     
           npart1=nsave+2                                               
           npart2=nsave+1                                               
           go to 172                                                    
         else                                                           
           kt=2                                                         
         end if                                                         
       end if                                                           
       if(ida(45).eq.1) then                                            
         kt=2                                                           
       end if                                                           
       npart1=nsave+kt                                                  
       npart2=nsave+1                                                   
       if(kl40.eq.1.or.(iprbcq.eq.2.and.icqq.ne.1)) then                
       else                                                             
         if(fist.eq.0.0) then                                           
           do 197 ide=1,2                                               
            deltr=0.8*sqrt(ranf(0))                                     
            fist=pi2*ranf(0)                                            
            nlund=nlund+1                                               
            mst(1)=nlund                                                
            mst(2)=nlund                                                
            plund(nlund,1)=deltr*cos(fist)                              
            plund(nlund,2)=deltr*sin(fist)                              
            plund(nlund,3)=0.0                                          
            plund(nlund,4)=0.0                                          
            thetlu=thet                                                 
            philu=phi                                                   
            bex=bxnew                                                   
            bey=bynew                                                   
            bez=bznew                                                   
            call lurobo(thetlu,philu,bex,bey,bez)                       
            mst(1)=0                                                    
            do 198 i=1,4                                                
  198       dxqb(ide,mod(i,4))=plund(nlund,i)                           
            nlund=nlund-1                                               
  197      continue                                                     
         end if                                                         
       end if                                                           
       if(kt.eq.2) then                                                 
         if(ktori.eq.-2) then                                           
           call twodec(kay,ifla,iflb,nsave,kt,1,                        
     a                                emjs,bxnew,bynew,bznew)           
           if(kay.eq.0) go to 17                                        
         else                                                           
           call lu2jet(nsave+1,iflb,ifla,emst)                          
           call luexec                                                  
         end if                                                         
         mst(1)=nsave+1                                                 
         nlu=nlund                                                      
         nlund=2*nlu-nsave-kt                                           
         if(nlund.gt.2000) then                                         
           call errex(                                                  
     a        'jetdec: not enough free space in jetset arrays')         
         end if                                                         
         if(nlu-nsave.gt.100) then                                      
           write(8,*) 'too many particles produced in jetdec for',      
     a                 'array xpos  no=',(nlu-nsave)                    
           call errex('jetdec:stop')                                    
         end if                                                         
         nht=nlu-nsave-kt                                               
         do 101 l=1,nht                                                 
            il=nlu+l                                                    
            nlochd=nsave+l+kt                                           
            do 103 i=1,3                                                
             plund(il,i)=irever*xpos(i,l)                               
  103       continue                                                    
            plund(il,4)=xpos(0,l)                                       
  101    continue                                                       
         if(irever.eq.-1)then                                           
           do  75 i=0,3                                                 
             p1(i)=pqbar(i)                                             
             pqbar(i)=pq(i)                                             
             pq(i)=p1(i)                                                
   75      continue                                                     
           do 117 nlochd=nsave+kt+1,nlu                                 
            do 116 i=1,3                                                
  116       plund(nlochd,i)=-plund(nlochd,i)                            
  117      continue                                                     
         end if                                                         
       elseif(kt.eq.3) then                                             
         x1lu=x1                                                        
         x3lu=x3                                                        
         call lu3jet(nsave+1,iflb,ifla,emst,x1lu,x3lu)                  
         call luexec                                                    
         mst(1)=nsave+1                                                 
         th=atan2(plund(nsave+3,1),plund(nsave+3,3))                    
         thp=pi -th                                                     
         psi=0.5*atan((x3*x3*sin(2*thp))/(x1*x1+x3*x3*cos(2*thp)))      
         nlu=nlund                                                      
         nlund=2*nlu-nsave-kt                                           
         do 105   l=1,nlu-nsave-kt                                      
            il=nlu+l                                                    
            nlochd=nsave+l+kt                                           
            do 104   i=1,3                                              
             plund(il,i)=irever*xpos(i,l)                               
  104       continue                                                    
            plund(il,4)=xpos(0,l)                                       
  105    continue                                                       
         if(irever.eq.-1)then                                           
           do  76 i=0,3                                                 
            p1(i)=pqbar(i)                                              
            pqbar(i)=pq(i)                                              
            pq(i)=p1(i)                                                 
   76      continue                                                     
           do 115 nlochd=nsave+kt+1,nlu                                 
            do 113 i=1,3                                                
  113       plund(nlochd,i)=-plund(nlochd,i)                            
  115      continue                                                     
         end if                                                         
         call lurobo(psi,0.,0.,0.,0.)                                   
         vink=pi2*ranf(0)                                               
         call lurobo(0.,vink,0.,0.,0.)                                  
       else                                                             
         write(8,*) kt                                                  
         call errex('error in jetdec:wrong kt')                         
       endif                                                            
       if(switch(19))then                                               
         do 317 nlochd=nsave+kt+1,nlu                                   
          do 316 i=1,2                                                  
           psyst(i)=psyst(i)-plund(nlochd,i)                            
           plund(nlochd,i)=0.                                           
  316     continue                                                      
  317    continue                                                       
       endif                                                            
       thetlu=thet                                                      
       philu=phi                                                        
       bex=bxnew                                                        
       bey=bynew                                                        
       bez=bznew                                                        
       call lurobo(thetlu,philu,bex,bey,bez)                            
       mst(1)=0                                                         
       nlund=nlu                                                        
       ks1=k(nsave+1,1)/10000                                           
  172  continue                                                         
       mst(7)=1                                                         
       if(ks1.le.1.or.kt.eq.-1) then                                    
         mst(24)=0                                                      
         call twodec(kay,ifla,iflb,nsave,kt,2,                          
     a                                emjs,bxnew,bynew,bznew)           
         if(kay.eq.0) go to 17                                          
         ideltq=0                                                       
       end if                                                           
       if(kl40.eq.1) then                                               
         ifl3=0                                                         
       else                                                             
         call luiflv(ltyp,ifl1,ifl2,ifl3,ksp)                           
       end if                                                           
       if(ifl3.eq.0) then                                               
         ldbar=0                                                        
       else                                                             
         ldbar=isign(1,ifl3)                                            
         if( (ifl1*ifl3.lt. 0).or.(ifl1*ifl2.lt.0))                     
     a          call errex('wrong quark content in subr. string, 15')   
       end if                                                           
       if(ideltq.ne.0) then                                             
         if(iopt(7).eq.1) then                                          
           ivz=isign(1,ideltq)                                          
           ideltq=iabs(ideltq)                                          
           iflold(1)=ivz*ideltq/100                                     
           iflold(2)=ivz*mod(ideltq,100)                                
         end if                                                         
       end if                                                           
       if(ida(7).eq.1.and.idfhad.ne.0) then                             
         do 38 i=0,3                                                    
   38    ptst(i)=frfp(i,ident)                                          
         iop1=0                                                         
   35    continue                                                       
         do 34 lloop=nsave+1,nlund                                      
           if(iop1.gt.1) go to 34                                       
           ks=k(lloop,1)/10000                                          
           if(ks.gt.1) go to 34                                         
           kh=mod(k(lloop,1),10000)                                     
           if(iop1.eq.1.or.lloop.eq.nlund-1.or.                         
     a                  (iprbcq.eq.2.and.kh.eq.npart1).or.              
     b                              (iprbcq.eq.1.and.kh.eq.npart2))then 
               if(iop1.eq.0)iop2=0                                      
               if(iop1.eq.1)iop2=1                                      
   37          continue                                                 
               do 36 llo=lloop+1,nlund                                  
                if(iop2.gt.1) go to 36                                  
                if(iop2.eq.1.or.llo.eq.nlund.or.                        
     a                      (iprbcq.eq.2.and.kh.eq.npart1)              
     b                        .or.(iprbcq.eq.1.and.kh.eq.npart2))then   
                  emsq=(plund(lloop,4)+plund(llo,4)+ptst(0))**2-        
     a                ((plund(lloop,1)+plund(llo,1)+ptst(1))**2+        
     b                    (plund(lloop,2)+plund(llo,2)+ptst(2))**2+     
     c                      (plund(lloop,3)+plund(llo,3)+ptst(3))**2)   
                  if(emsq.le.(plund(lloop,5)+plund(llo,5))**2) then     
                      if(lloop.eq.nlund-1.and.llo.eq.nlund.and.         
     a                 iop1.eq.1) then                                  
                      end if                                            
                  else                                                  
                      if(iop1.gt.1.or.iop2.gt.1) call errex(            
     a                     'unexpected iop1,iop2 in string decay loop') 
                      iop1=5                                            
                      iop2=5                                            
                      isccss=0                                          
                      do 62 ii=1,4                                      
                        i=mod(ii,4)                                     
                        plund(lloop,ii)=plund(lloop,ii)+ptst(i)         
                        psyst(i)=psyst(i)+ptst(i)                       
                        frfp(i,ident)=0.                                
   62                 continue                                          
                      emllp=plund(lloop,5)                              
                      p1(0)=plund(lloop,4)                              
                      emll=plund(llo,5)                                 
                      p2(0)=plund(llo,4)                                
                      do 61 i=1,3                                       
                       p1(i)=plund(lloop,i)                             
                       p2(i)=plund(llo,i)                               
   61                 continue                                          
                      call onshel(2,isccss,emllp,emll,                  
     a                          p1(0),p1(1),p1(2),p1(3),                
     b                           p2(0),p2(1),p2(2),p2(3))               
                      plund(lloop,5)=emllp                              
                      plund(lloop,4)=p1(0)                              
                      plund(llo,5)=emll                                 
                      plund(llo,4)=p2(0)                                
                      do 60 i=1,3                                       
                       plund(lloop,i)=p1(i)                             
                       plund(llo,i)=p2(i)                               
   60                 continue                                          
                      if(isccss.eq.0) then                              
                          write(8,*) 'string onsh lloop=',lloop,        
     a                            (plund(lloop,i),i=1,5)                
                          write(8,*) 'string onsh llo=',llo,            
     a                                   (plund(llo,i),i=1,5)           
                          call errex('isccss=0 in string decay?????')   
                      end if                                            
                  end if                                                
                end if                                                  
   36          continue                                                 
               iop2=iop2+1                                              
               if(iop2.eq.1) go to 37                                   
           end if                                                       
   34    continue                                                       
         iop1=iop1+1                                                    
         if(iop1.eq.1) then                                             
            go to 35                                                    
         else if(iop1.eq.2.and.iop2.eq.2) then                          
            emjtsm=emjs                                                 
            pp=0.                                                       
            do 51 i=0,3                                                 
             p(i,ind)=p(i,ind)+ptst(i)                                  
             ptot(i)=p(i,ind)                                           
             psyst(i)=psyst(i)+ptst(i)                                  
             frfp(i,ident)=0.                                           
             ivz=isign(1,-i)                                            
             pp=pp+ivz*p(i,ind)*p(i,ind)                                
   51       continue                                                    
            emjs=sqrt(pp)                                               
            bxnew=p(1,ind)/p(0,ind)                                     
            bynew=p(2,ind)/p(0,ind)                                     
            bznew=p(3,ind)/p(0,ind)                                     
            if(iopt(7).ne.0) then                                       
              iokay=0                                                   
              if(iopt(7).ne.0.and.icqsto.ne.0                           
     a                             .and.iabs(icqend).lt.10) then        
                iprbcq=3-iprbcq                                         
              end if                                                    
              go to 17                                                  
            end if                                                      
            call twodec(kay,ifla,iflb,nsave,kt,2,                       
     a                                   emjs,bxnew,bynew,bznew)        
            if(kay.eq.0) go to 17                                       
            nlcor=nlund                                                 
            mst(1)=nlcor+1                                              
            mst(2)=nlcor+2                                              
            plund(nlcor+1,4)=pq(0)*emjs/emjtsm                          
            plund(nlcor+2,4)=pqbar(0)*emjs/emjtsm                       
            do  1008 i=1,3                                              
             plund(nlcor+1,i)=pq(i)*emjs/emjtsm                         
             plund(nlcor+2,i)=pqbar(i)*emjs/emjtsm                      
 1008       continue                                                    
            nlund=nlund+2                                               
            if(nlund.ge.2000)                                           
     a         call errex('lund storage too small, abort in string')    
             bex=-bx                                                    
             bey=-by                                                    
             bez=-bz                                                    
             call lurobo(0.,0.,bex,bey,bez)                             
             bex=bxnew                                                  
             bey=bynew                                                  
             bez=bznew                                                  
             call lurobo(0.,0.,bex,bey,bez)                             
             mst(1)=0                                                   
             nlund=nlcor                                                
             do  1009 ii=1,4                                            
              i=mod(ii,4)                                               
              pq(i)=plund(nlund+1,ii)                                   
              pqbar(i)=plund(nlund+2,ii)                                
 1009        continue                                                   
             bx=bxnew                                                   
             by=bynew                                                   
             bz=bznew                                                   
         else if(iop1.ne.6.or.iop2.ne.6) then                           
            write(8,*) 'iop1,iop2=',iop1,iop2                           
            call errex('unexpected iop1,iop2 in string decay')          
         end if                                                         
       end if                                                           
       if(ida(29).eq.3) then                                            
         do 110   l=1,nlund-nsave-kt                                    
            il=nlund+l                                                  
            nlochd=nsave+l+kt                                           
            if(k(nlochd,2).eq.1) go to 110                              
            do 112   i=1,4                                              
              plund(il,i)=tprd*plund(il,i)                              
  112       continue                                                    
               do 118   i=1,3                                           
  118          xpos(i,l)=xold(i)+plund(il,i)                            
               xpos(0,l)=xold(0)+plund(il,4)                            
  110    continue                                                       
       end if                                                           
       if(kl40.eq.1) then                                               
         izcq=0                                                         
       else                                                             
         if(iprbcq.eq.0) then                                           
           izcq=2                                                       
         else                                                           
           izcq=1                                                       
         end if                                                         
         if(ibrtag.ne.0) izcq=izcq+1                                    
       end if                                                           
       nprtno=nlund-(nsave+kt)                                          
       nstpur=nprtno-izcq                                               
       if(icqq.eq.1) nstpur=nstpur-1                                    
       if(kmult.gt.0) nstpur=0                                          
       if(nstpur.gt.0.and.                                              
     a      nrbar(kclst)+nrmes(kclst)+nprtno.gt.iclusz(kclst))          
     b                                                nstpur=0          
       iopt(2)=ida(16)                                                  
       mst(10)=mst10                                                    
       mst(11)=mst11                                                    
       par(1)=par1                                                      
       par(2)=par2                                                      
       par(5)=par5                                                      
       par(8)=par8                                                      
       par(12)=par12                                                    
       par(14)=par14                                                    
       par(15)=par15                                                    
       par(51)=par51                                                    
       par(52)=par52                                                    
       lindf=1                                                          
       kright=0                                                         
       kleft=0                                                          
       bx=ptot(1)/ptot(0)                                               
       by=ptot(2)/ptot(0)                                               
       bz=ptot(3)/ptot(0)                                               
       if(emind.ne.0.) then                                             
           gam=ptot(0)/emind                                            
       else                                                             
          write(8,*) ind,'p=',(p(i,ind),i=0,3),'em=',emind              
          write(8,*) 'type=',ltyp,kmult                                 
          call errex('unstable photon in string????')                   
       end if                                                           
       b2=max(cvalu7,bx**2+by**2+bz**2)                                 
       if(ida(25).eq.1.and.ntag.eq.0) then                              
         if(switch(23)) then                                            
           ken=itripl                                                   
         else                                                           
           ken=kdt                                                      
         end if                                                         
         ihatot=nlund-nsave-kt                                          
         write(15,661)                                                  
     a       irun,ken,3,0,ihatot,0,issn(ind)*1000+nclcnt(issn(ind)),0   
  661    format(1x,i3,1x,i3,1x,i1,1x,i5,1x,i3,1x,i2,1x,i8,1x,i8)        
  662    format(1x,i4,1x,i4,1x,f8.3,1x,2(f7.3,1x),f8.3,1x,              
     a               f7.3,i3,1x,i8,1x,f8.3,2(1x,f7.3),1x,f8.3)          
         call oldat(1,itold,npold,p0old,pxold,pyold,pzold,emold)        
         write(15,662)itold,npold,p0old,pxold,pyold,pzold,emold,        
     a                           icqsto,ident,xold(0),                  
     b                               xold(1),xold(2),xold(3)            
       end if                                                           
       iditag=0                                                         
       if(jctvrt.eq.1) ibrtag=0                                         
       lstart=0                                                         
       jleft=0                                                          
       jright=0                                                         
       nidbeg=0                                                         
       jlrel=0                                                          
       jrrel=0                                                          
       irrel=0                                                          
       if(kl40.eq.1) then                                               
         ityp(ind)=-9999                                                
         decay(ind)=1.e+35                                              
         lindf=0                                                        
       end if                                                           
       irank1=0                                                         
       irank2=0                                                         
       zplsum=cvalu0                                                    
       if(2*nlund-nsave-kt.gt.2000) then                                
          call errex('string:lund storage too small')                   
       end if                                                           
       ide=0                                                            
       do 20 lloop=nsave+1,nlund                                        
        iprtno=lloop-nsave-kt                                           
        ks=k(lloop,1)/10000                                             
        kfm=k(lloop,2)                                                  
        if(ks.gt.1) go to 20                                            
        kh=mod(k(lloop,1),10000)                                        
        if(kh-nsave.eq.1) then                                          
          irank1=irank1+1                                               
        else if(kh-nsave.eq.2) then                                     
          irank2=irank2+1                                               
        else                                                            
          write(8,*) nsave,kh                                           
          call errex('unclear parent quark in string decay')            
        end if                                                          
        call luiflv(kfm,ifla,iflb,iflc,ksp)                             
        if(iflc.eq.0) then                                              
          lnbar=0                                                       
        else                                                            
          lnbar=isign(1,iflc)                                           
        end if                                                          
        if(iditag.eq.1) then                                            
          if(kh.eq.npart1.and.ibrtag.eq.1) then                         
             idlj=idlj-1                                                
             if(idlj.eq.0) then                                         
               kleft=0                                                  
               iditag=2                                                 
             end if                                                     
          end if                                                        
          if(kh.eq.npart2.and.ibrtag.eq.2) then                         
             idlj=idlj-1                                                
             if(idlj.eq.0) then                                         
               kright=0                                                 
               iditag=2                                                 
             end if                                                     
          end if                                                        
        end if                                                          
        if(kleft.eq.0) then                                             
          if(kh.eq.npart1) then                                         
            kleft=1                                                     
            if(ibrtag.eq.1.and.iditag.eq.0) then                        
              iditag=1                                                  
            end if                                                      
          end if                                                        
        else if(kleft.eq.1) then                                        
          kleft=2                                                       
        endif                                                           
        if(kright.eq.0) then                                            
          if(kh.eq.npart2) then                                         
            kright=1                                                    
            if(ibrtag.eq.2.and.iditag.eq.0) then                        
              iditag=1                                                  
            end if                                                      
          end if                                                        
        else if(kright.eq.1) then                                       
          kright=2                                                      
        endif                                                           
        if(lloop.eq.nlund) then                                         
          if(kleft.eq.0) then                                           
            kleft=1                                                     
          else if(kright.eq.0) then                                     
            kright=1                                                    
          end if                                                        
        end if                                                          
        lfill=0                                                         
        if(lindf.eq.1) then                                             
          if(lnbar.eq.ldbar.or.(ind.gt.iofset+nrbar(kclst)))then        
            if(kright.gt.0.or.(icqq.eq.2.and.kleft.gt.0)                
     a                                                 ) then           
              lfill=1                                                   
            end if                                                      
          end if                                                        
        else if(iopt(5).eq.1) then                                      
          lfill=1                                                       
        end if                                                          
        if((iopt(5).eq.1).or.                                           
     a      (kl40.eq.0.and.((kleft.eq.1.and.iprbcq.ne.2).or.            
     b                       (kright.eq.1.and.iprbcq.ne.1)))) then      
          notdly=1                                                      
        else                                                            
          notdly=0                                                      
        end if                                                          
        if(mbcqno.eq.1) lfill=0                                         
        if(notdly.eq.0.and.izcq.ne.0.and.lfill.eq.1) then               
          lfill=0                                                       
        end if                                                          
        if(notdly.eq.1) then                                            
          if(kh.eq.npart2) irrel=irrel+1                                
        end if                                                          
        if(kh.eq.npart1) then                                           
          jlrel=jlrel+1                                                 
          inkre=2*nlund+1-nsave-kt-jlrel                                
        else                                                            
          jrrel=jrrel+1                                                 
          inkre=nlund+jrrel                                             
        end if                                                          
        k(inkre,1)=lbreak(lloop-nsave-kt)                               
        if(notdly.eq.0.and.lstart.ne.0.and.lfill.eq.1) then             
          lfill=0                                                       
        end if                                                          
        if(lfill.eq.1) then                                             
          l=ind                                                         
          lindf=0                                                       
        else                                                            
          npion=npion+1                                                 
          if(nrmes(kclst)+nrbar(kclst).eq.iclusz(kclst)) then           
            if(kclst.eq.itripl.and.kdt.lt.idtcl) then                   
              write(8,*)'warning string:iclusz(itripl) too low'         
            end if                                                      
            io=0                                                        
            do 45 kcl=1,nrclst                                          
  45        io=io+ iclusz(kcl)                                          
            nrclst=nrclst+1                                             
            nrbar(nrclst)=0                                             
            nrmes(nrclst)=1                                             
            iclusz(nrclst)=1                                            
            nonrel(nrclst)=.false.                                      
            l=io+1                                                      
            lmax=l                                                      
            if(lmax.ge.ntotma+1-nspect)                                 
     a      call errex('particle number too large,abend in string')     
          else if(nrmes(kclst)+nrbar(kclst).lt.iclusz(kclst))then       
            nrmes(kclst)=nrmes(kclst)+1                                 
            l=iofset+nrbar(kclst)+nrmes(kclst)                          
            if(notdly.eq.0.and.lstart.eq.0) then                        
              if(nstpur.ge.1) then                                      
                lstart=l                                                
                izcqst=max(0,izcq-lindf)                                
                nmax=nlund-lloop+1-izcq                                 
                nmax2=nlund-lloop+1-lindf                               
                if(mbcqno.eq.1) then                                    
                  nmax2=nmax2+1                                         
                  if(izcq.eq.1) izcqst=izcqst+1                         
                end if                                                  
                if(lstart+nmax2+lindf-1-iofset.gt.                      
     a                                          iclusz(kclst)) then     
                  call errex(                                           
     a                  'string: lstart.ne.0, iclusz problem')          
                else                                                    
                  do 22 lst=lstart,lstart+nmax2+lindf-1                 
   22             ityp(lst)=0                                           
                end if                                                  
              end if                                                    
            end if                                                      
            if(lstart.ne.0) then                                        
              if(notdly.eq.1) then                                      
                if(izcqst-izcq.ge.0) then                               
                  l=lstart+izcqst-izcq                                  
                else                                                    
                  l=lstart+nmax                                         
                end if                                                  
              else                                                      
                if(kh.eq.npart2) then                                   
                  l=lstart+izcqst+jright                                
                  jright=jright+1                                       
                else                                                    
                  jleft=jleft+1                                         
                  l=lstart+nmax2-jleft                                  
                end if                                                  
              end if                                                    
              if(ityp(l).ne.0) then                                     
                write(8,*)                                              
     a               'notdly,lstart,izcqst,izcq,nmax,nmax2,kh',         
     b                    ',npart2,jright,jleft'                        
                write(8,*)                                              
     a                notdly,lstart,izcqst,izcq,nmax,nmax2,kh,          
     b                     npart2,jright,jleft                          
                write(8,*)'ityp(l),l=',ityp(l),l                        
                call errex(                                             
     a                   'string: lstart.ne.0, ityp(l).ne.0 ')          
              end if                                                    
              if(l.lt.lstart.or.l.gt.lstart+nmax2+lindf-1) then         
                call errex(                                             
     a                  'string: lstart.ne.0, l out of range')          
              end if                                                    
            end if                                                      
          else                                                          
            call errex('string:nrbar+nrmes.gt.iclusz  ????')            
          end if                                                        
          nprtcl=nprtcl+1                                               
          if(nprtcl.gt.ntotma) then                                     
            call errex('string:nprtcl>ntotma, abort')                   
          end if                                                        
          issn(l)=nprtcl                                                
          lastcl(l)=ievnt                                               
        end if                                                          
        if(l.eq.ind) then                                               
          nidhad=ident                                                  
        else                                                            
          nidhad=nprtcl                                                 
        end if                                                          
        if(notdly.eq.1.and.iopt(5).eq.0) then                           
          if(kright.eq.1.and.ibstri.eq.1) then                          
            if(kpartm.eq.0.and.iopt(4).gt.0.and.ibrtag.ne.0) then       
              kpartm=nidhad                                             
            else                                                        
              kpartr=nidhad                                             
            end if                                                      
          end if                                                        
        end if                                                          
        eml=plund(lloop,5)                                              
        p1(0)=plund(lloop,4)                                            
        do 21 i=1,3                                                     
  21    p1(i)=plund(lloop,i)                                            
        iexpon=0                                                        
        if(notdly.eq.1) then                                            
          ierr=0                                                        
          if(kleft.eq.1) then                                           
            kact=1                                                      
          else                                                          
            kact=2                                                      
          end if                                                        
          if(ldbar.eq.0) then                                           
            iexpon=iflold(kact)                                         
            if(ifla.ne.iexpon.and.iflb.ne.iexpon.and.                   
     a                    iflc.ne.iexpon.and.                           
     b                                   ifla+iflb+iflc.ne.0) then      
              ierr=1                                                    
            else                                                        
              iflold(kact)=0                                            
            end if                                                      
          else                                                          
            if(iflc.eq.0) then                                          
              isg=isign(1,iflold(kact))                                 
              iabold=iabs(iflold(kact))                                 
              if(iabold.gt.10) then                                     
                if1=isg*(iabold/10)                                     
                if2=isg*(iabold-10*(iabold/10))                         
                if(if1.eq.icqbar)  then                                 
                  iexpon=if1                                            
                  iflold(kact)=if2                                      
                else if(if2.eq.icqbar)  then                            
                  iexpon=if2                                            
                  iflold(kact)=if1                                      
                else                                                    
                  ierr=2                                                
                end if                                                  
              else                                                      
                iexpon=iflold(kact)                                     
                if(ifla.ne.iexpon.and.iflb.ne.iexpon.and.               
     a                            iflc.ne.iexpon.and.                   
     b                                           ifla+iflb.ne.0) then   
                  ierr=2                                                
                else                                                    
                  iflold(kact)=0                                        
                end if                                                  
              end if                                                    
            else                                                        
              if(iditag.eq.1.and.kact.eq.ibrtag) then                   
                iexpon=icqbar                                           
                if(ifla.ne.iexpon.and.iflb.ne.iexpon.and.               
     a                                             iflc.ne.iexpon) then 
                  ierr=3                                                
                else                                                    
                  isg=isign(1,iflold(kact))                             
                  iabold=iabs(iflold(kact))                             
                  if(iabold.le.10) then                                 
                    ierr=4                                              
                  else                                                  
                    if1=isg*(iabold/10)                                 
                    if2=isg*(iabold-10*(iabold/10))                     
                    if(icqbar.eq.if1)then                               
                      iflold(kact)=if2                                  
                    else if(icqbar.eq.if2) then                         
                      iflold(kact)=if1                                  
                    else                                                
                      ierr=5                                            
                    end if                                              
                  end if                                                
                end if                                                  
              else                                                      
                iexpon=iflold(kact)                                     
                isg=isign(1,iflold(kact))                               
                iabold=iabs(iflold(kact))                               
                if1=isg*(iabold/10)                                     
                if2=isg*(iabold-10*(iabold/10))                         
                if((if1.ne.0.and.if1.ne.ifla.and.                       
     a             if1.ne.iflb.and.if1.ne.iflc).or.                     
     b                         (if2.ne.ifla.and.                        
     c                         if2.ne.iflb.and.if2.ne.iflc))then        
                  ierr=6                                                
                else                                                    
                  iflold(kact)=0                                        
                end if                                                  
              end if                                                    
            end if                                                      
          end if                                                        
          if(iditag.eq.2) then                                          
            iditag=0                                                    
          end if                                                        
          if(ierr.ne.0) then                                            
            write(8,*) 'const. quark cannot be identified:jetdec'       
            write(8,*) 'ierr=',ierr                                     
            write(8,*) 'kleft,kright,iditag,ibrtag,ldbar'               
            write(8,*)  kleft,kright,iditag,ibrtag,ldbar                
            write(8,*)'kact,ifla,iflb,iflc,kfm,ltyp,iflold,',           
     a                         'kleft,kright'                           
            write(8,*) kact,ifla,iflb,iflc,kfm,ltyp,iflold(1),          
     a                          iflold(2),kleft,kright                  
            write(8,*) 'if1/2',if1,if2                                  
            call errex('jetdec- garbage')                               
          end if                                                        
          if(iopt(5).gt.0) then                                         
            iopt(5)=-1                                                  
            kleft=1                                                     
            kright=1                                                    
            iflold(1)=0                                                 
            iflold(2)=0                                                 
          end if                                                        
          notdly=1                                                      
          izcq=izcq-1                                                   
          if(nprtcl.gt.ntotma) then                                     
            write(8,*) 'nprtcl=',nprtcl,'in jetdec'                     
            write(8,*) 'too large nprtcl for array itdth'               
            call errex('jetdec:ntotma arrays too small')                
          end if                                                        
          if(l.eq.ind) then                                             
            iposit=0                                                    
            dr(0)=0.                                                    
          else                                                          
                do 310 i=0,3                                            
 310             x(i,l)=xold(i)                                         
                if(ida(29).eq.3) then                                   
                  iposit=0                                              
                  dr(0)=0.                                              
                else                                                    
                  iposit=1                                              
                end if                                                  
          end if                                                        
          if(ida(29).ne.3) then                                         
            if(ida(29).eq.2) then                                       
              pbvec=p1(1)*bxnew+p1(2)*bynew+p1(3)*bznew                 
              p0rsjs=gamind*(p1(0)-pbvec)                               
              gamhrs=p0rsjs/eml                                         
              if(gamhrs.le.1.) then                                     
                write(8,*) 'string:gamhrs.le.1 ????  gamhrs=',gamhrs    
                gamhrs=1.                                               
              end if                                                    
              pminsm=gamhrs*(1.-sqrt(1.-1./(gamhrs*gamhrs)))            
              gwait=2.*gprd/(emjs*pminsm+                               
     a                              2.*tpar2*sqrt(eml*eml+0.16))        
              wait=gwait                                                
              tfrmcq=tfrmcq+hc/(wait*pi2)                               
              nfrmcq=nfrmcq+1                                           
              twait=timelf(wait,eml,p1(1),p1(2),p1(3))                  
            else if(ida(29).le.1) then                                  
              wait=gprd                                                 
              tfrmcq=tfrmcq+hc/(wait*pi2)                               
              nfrmcq=nfrmcq+1                                           
              twait=timelf(wait,eml,p1(1),p1(2),p1(3))                  
            end if                                                      
            tdtg(nidhad)=max(xold(0)+twait,xold(0))                     
          else if(ida(29).eq.3) then                                    
            lact=lloop-kt-nsave                                         
            emht=xpos(4,lact)                                           
            tpr=tpar3                                                   
            if(tpr.lt.1.e-30) then                                      
              twait=0.                                                  
            else                                                        
              wait=gprd/(tpr*emht)                                      
              twait=timelf(wait,eml,p1(1),p1(2),p1(3))                  
            end if                                                      
            if(icqq.eq.2.and.ident.eq.nidhad) then                      
              do 309 i=0,3                                              
 309          qdot(i,l)=0.5*(xpos(i,lact)+xqj(i))-xold(i)               
              tdtg(nidhad)=0.5*(xpos(0,lact)+xqj(0))+twait              
            else                                                        
              do 311 i=0,3                                              
 311          qdot(i,l)=xpos(i,lact)-xold(i)                            
              tdtg(nidhad)=xpos(0,lact)+twait                           
            end if                                                      
            if(ident.ne.nidhad) then                                    
              ide=ide+1                                                 
              ide=min(2,ide)                                            
              do 312 i=0,3                                              
               x(i,l)=xold(i)+dxqb(ide,i)                               
 312          continue                                                  
              tdtg(nidhad)=tdtg(nidhad)+ dxqb(ide,0)                    
            end if                                                      
            if(iopt(5).eq.0.or.idfhad.ne.0) then                        
              gaml=p1(0)/eml                                            
              taul=(tdtg(nidhad)-xold(0))/gaml                          
              tfrmcq=tfrmcq+taul*emht/eml                               
              nfrmcq=nfrmcq+1                                           
            end if                                                      
          end if                                                        
        else                                                            
          iposit=2                                                      
          if(ida(29).eq.0) then                                         
              wait=gprd                                                 
              tfrmhd=tfrmhd+hc/(wait*pi2)                               
              nfrmhd=nfrmhd+1                                           
          else if(ida(29).eq.1) then                                    
              wait=gprd*empion/eml                                      
              tfrmhd=tfrmhd+hc/(wait*pi2)                               
              nfrmhd=nfrmhd+1                                           
          else if(ida(29).eq.2) then                                    
                    emht=sqrt(eml*eml+0.16)                             
                    wait=gprd/(sqrt(tpar1+0.25*emht*emht)+tpar2*emht)   
                    tfrmhd=tfrmhd+hc/(wait*pi2)                         
                    nfrmhd=nfrmhd+1                                     
          else if(ida(29).eq.3) then                                    
                    emht=xpos(4,lloop-kt-nsave)                         
                    if(tpar2.lt.1.e-30) then                            
                       iposit=0                                         
                       wait=1.e+30                                      
                    else                                                
                       wait=gprd/(tpar2*emht)                           
                    end if                                              
                    do 114   i=0,3                                      
  114               x(i,l)=xpos(i,lloop-kt-nsave)                       
                    if(lstart.gt.0) then                                
                       if(nstpur.ge.2.and.l.eq.lstart+izcqst) then      
                         nidbeg=nidhad                                  
                       end if                                           
                       phpqb=p1(0)*pqbar(0)                             
                       do 102 i=1,3                                     
  102                  phpqb=phpqb-p1(i)*pqbar(i)                       
                       zplus=2.*phpqb/emjs/emjs                         
                       zplsum=zplsum+zplus                              
                       zminus=emht**2/emjs**2/zplus                     
                       if(mod(iopt(2),2).eq.1) then                     
                         ph0=zplus*pq(0)+zminus*pqbar(0)                
                         deltt=0.5*ph0*tprd                             
                       else                                             
                         deltt=0.                                       
                       end if                                           
                       tlastl=xpos(0,lloop-kt-nsave)+deltt              
                    end if                                              
                    gaml=p1(0)/eml                                      
                    taul=(x(0,l)-xold(0))/gaml+1./wait*hc/pi2           
                    nfrmhd=nfrmhd+1                                     
                    tfrmhd=tfrmhd+taul*emht/eml                         
          endif                                                         
          if(iopt(5).eq.2) then                                         
            iposit=1                                                    
            do 308 i=0,3                                                
 308        x(i,l)=xold(i)                                              
          end if                                                        
        endif                                                           
        if(iposit.eq.1) then                                            
            deltr=drind*sqrt(ranf(0))                                   
            phi=pi2*ranf(0)                                             
            costet=1.-2.*ranf(0)                                        
            sintet=sqrt(1.-costet*costet)                               
            xrel=deltr* sintet* sin(phi)                                
            yrel=deltr* sintet* cos(phi)                                
            zrel=deltr* costet                                          
            xb=bx*xrel+by*yrel+bz*zrel                                  
            gg=(gam-1.)*xb/b2                                           
            dr(0)=gam*xb                                                
            dr(1)=gg*bx+xrel                                            
            dr(2)=gg*by+yrel                                            
            dr(3)=gg*bz+zrel                                            
            do 31 i=0,3                                                 
  31        x(i,l)=xold(i)+dr(i)                                        
            wait=100.                                                   
            if(iabs(kfm).le.10) then                                    
               iposit=0                                                 
            end if                                                      
        end if                                                          
        if(iposit.eq.1.or.iposit.eq.2) then                             
          ixnew=0                                                       
          twait=timelf(wait,eml,p1(1),p1(2),p1(3))                      
          if(iposit.eq.1) dr(0)=dr(0)+twait                             
  27      continue                                                      
          ixnew=ixnew+1                                                 
          if(ixnew.gt.5) then                                           
              write(8,*) 'ixnew>5 in string'                            
              do 128 i=0,3                                              
               x(i,l)=xold(i)                                           
 128          continue                                                  
              twait=0.001*p1(0)                                         
          end if                                                        
          if(ixnew.ge.2.or.iposit.eq.1.or.                              
     a                                 ida(29).eq.3) then               
             do 228 i=0,3                                               
               xnew(i)=x(i,l)+p1(i)/p1(0)*twait                         
               qlk(i)=xnew(i)-xold(i)                                   
 228         continue                                                   
          else                                                          
             do 129 i=0,3                                               
               xnew(i)=xold(i)+p1(i)/p1(0)*twait                        
               qlk(i)=xnew(i)-xold(i)                                   
 129         continue                                                   
          end if                                                        
          irturn=0                                                      
          dqlksq=qlk(0)*qlk(0)                                          
          do 19 i=1,3                                                   
  19       dqlksq=dqlksq-qlk(i)*qlk(i)                                  
          if(ixnew.lt.10) then                                          
            if(ida(29).eq.3) then                                       
            else                                                        
              if(dqlksq.le.0..or.qlk(0).le.0.) then                     
                irturn=1                                                
              end if                                                    
            end if                                                      
          end if                                                        
          if(irturn.eq.1) then                                          
             delqp=qlk(0)*ptot(0)                                       
             pindsq=ptot(0)*ptot(0)                                     
             do 33 i=1,3                                                
              pindsq=pindsq-ptot(i)*ptot(i)                             
              delqp=delqp-qlk(i)*ptot(i)                                
  33         continue                                                   
             val=delqp/pindsq                                           
             arg=(0.01-dqlksq)/pindsq+val*val                           
             if(arg.lt.0.) then                                         
                if(-arg.gt.0.01) then                                   
                   write(8,*) 'arg.lt.0 in string,33  ????????',arg     
                   call errex('numerical problem in string')            
                end if                                                  
                arg=0.                                                  
             else                                                       
                arg=sqrt(arg)                                           
             end if                                                     
             delts=-val+arg                                             
             do 32 i=0,3                                                
               x(i,l)=xnew(i)+ptot(i)*delts                             
  32         continue                                                   
             twait=0.01                                                 
             if(iposit.eq.1) dr(0)=dr(0)+0.01+ptot(0)*delts             
             go to 27                                                   
          end if                                                        
          do 26 i=0,3                                                   
            x(i,l)=xnew(i)                                              
  26      continue                                                      
        end if                                                          
        if(notdly.eq.0.and.lstart.gt.0.and.                             
     a        l.ge.lstart+izcqst.and.l.le.lstart+izcqst+nstpur-1) then  
          tlast(l)=tlastl                                               
          ipurst=1                                                      
        else                                                            
          tlast(l)=x(0,l)                                               
          ipurst=0                                                      
        end if                                                          
        ylast(nidhad)=0.5*log((p1(0)+p1(3))/(p1(0)-p1(3)))              
        if(ipurst.eq.0) then                                            
          zlast(nidhad)=                                                
     a           x(3,l)-(pnorm(0)*x(0,l)-pnorm(3)*x(3,l))*pnorm(3)      
        else                                                            
          if(kl40.eq.1) then                                            
            if(rtsea.gt.0.0) then                                       
              zlast(nidhad)=rtsea                                       
            else                                                        
              zlast(nidhad)=-1.0                                        
            end if                                                      
            if(rtsea.le.0.0)then                                        
              call errex("0/neg. radius of sea string")                 
            end if                                                      
          else                                                          
            zlast(nidhad)=0.                                            
          end if                                                        
        end if                                                          
        ichbig(l)=0                                                     
        kmulth=mmult(iprtno)                                            
        call oldnew(0,ityp(l),npidec(l),eml,kmulth,kfm)                 
        em(l)=eml                                                       
        itdth(4,nidhad)=0                                               
        itdth(5,nidhad)=0                                               
        do 28 i=0,3                                                     
   28   p(i,l)=p1(i)                                                    
        kfa=iabs(kfm)                                                   
        if(notdly.eq.0) then                                            
          if(l.ne.ind) then                                             
            tdtg(nidhad)=0.                                             
            itdth(1,nidhad)=0                                           
            itdth(2,nidhad)=0                                           
            itdth(3,nidhad)=0                                           
          end if                                                        
          do 206 i=0,3                                                  
  206     xform(i)=x(i,l)                                               
        else                                                            
          tdtg(nidhad)=tdtg(nidhad)+dr(0)                               
          itdth(3,nidhad)=ltdt3                                         
          if(iprbcq.eq.0) then                                          
            if(icqq.eq.2) then                                          
              if(kact.eq.2) itdth(3,nidhad)=nidhad                      
            else                                                        
              if(kact.eq.1) itdth(3,nidhad)=nidhad                      
            end if                                                      
          else                                                          
            itdth(3,nidhad)=nidhad                                      
          end if                                                        
          if(icqq.eq.2.and.kact.eq.1) iexpon=icqend                     
          if(iopt(7).eq.1.and.iprbcq.eq.0) then                         
            iexpon=0                                                    
            kact=0                                                      
          else if(iopt(2).ge.2) then                                    
            kact=2                                                      
          end if                                                        
          itdth(2,nidhad)=iexpon+isign(1,iexpon)*kact*100               
          if(iopt(5).eq.-1.and.idfhad.eq.0) then                        
            tdtg(nidhad)=0.                                             
            itdth(1,nidhad)=0                                           
            itdth(2,nidhad)=0                                           
            itdth(3,nidhad)=0                                           
            iexpon=0                                                    
          end if                                                        
          if(ida(10).eq.1) then                                         
            if(l.ne.ind) then                                           
              do 321 i=0,3                                              
               x(i,l)=x(i,l)+p(i,l)/p(0,l)*                             
     a                        (tdtg(nidhad)-x(0,l))                     
  321         continue                                                  
              notdly=0                                                  
            end if                                                      
            tdtg(nidhad)=0.                                             
            itdth(1,nidhad)=0                                           
            itdth(2,nidhad)=0                                           
            itdth(3,nidhad)=0                                           
            iexpon=0                                                    
          end if                                                        
          if(notdly.eq.0) then                                          
          else if(iexpon.eq.0) then                                     
            deltf=0.0                                                   
          else                                                          
            deltf=tdtg(nidhad)-x(0,l)                                   
          end if                                                        
          if(notdly.eq.0) then                                          
          else if(ida(29).eq.3) then                                    
            do 202 i=0,3                                                
  202       xform(i)=x(i,l)+qdot(i,l)/qdot(0,l)*deltf                   
          else                                                          
            do 201 i=0,3                                                
  201       xform(i)=x(i,l)+p(i,l)/p(0,l)**deltf                        
          end if                                                        
        end if                                                          
        if(ida(25).eq.1)then                                            
          if(ntag.eq.0) then                                            
            write(15,662) ityp(l),npidec(l),(p(i,l),i=0,3),em(l),       
     a                    iexpon,issn(l),xform(0),                      
     b                    xform(1),xform(2),xform(3)                    
          end if                                                        
        end if                                                          
        if(ida(38).eq.3.or.ida(38).eq.4) then                           
          idntl=issn(l)                                                 
          kstor(idntl,1)=2                                              
          if(itdth(2,nidhad).eq.0) then                                 
            do 203 i=1,4                                                
  203       stork(idntl,i)=x(i-1,l)                                     
          else                                                          
            do 204 i=1,4                                                
  204       stork(idntl,i)=xform(i-1)                                   
          end if                                                        
          do 303 i=6,8                                                  
  303     stork(idntl,i)=p(i-5,l)                                       
        else if(ida(38).eq.1.or.ida(38).eq.2.or.                        
     a                   ida(38).eq.5.or.ida(38).eq.7) then             
          if(ida(38).eq.1.or.ida(38).eq.2) then                         
            jeng=1                                                      
          else                                                          
            jeng=0                                                      
          end if                                                        
          emil=em(l)                                                    
          if(issers(jeng,ida(38),ityp(l),npidec(l),emil))then           
                 idntl=issn(l)                                          
                 stork(idntl,1)=stork(ident,1)                          
                 stork(idntl,2)=stork(ident,2)                          
                 stork(idntl,3)=stork(ident,3)                          
                 stork(idntl,4)=stork(ident,4)                          
                 stork(idntl,5)=stork(ident,5)                          
                 kstor(idntl,1)=kstor(ident,1)                          
                 kstor(idntl,2)=kstor(ident,2)                          
                 kstor(idntl,3)=kstor(ident,3)                          
                 kstor(idntl,4)=kstor(ident,4)                          
                 kstor(idntl,5)=kstor(ident,5)                          
                 kstor(idntl,6)=kstor(ident,6)                          
                 kstor(idntl,7)=kstor(ident,7)                          
          end if                                                        
        end if                                                          
        if(notdly.eq.1.and.iexpon.ne.0) then                            
          if(ida(7).eq.1) then                                          
            emlef=p(0,l)*p(0,l)                                         
            phpq=p(0,l)*pq(0)                                           
            phpqb=p(0,l)*pqbar(0)                                       
            do 40 i=1,3                                                 
             phpq=phpq-p(i,l)*pq(i)                                     
             phpqb=phpqb-p(i,l)*pqbar(i)                                
             emlef=emlef-p(i,l)*p(i,l)                                  
   40       continue                                                    
            phpq=phpq/emjs/emjs                                         
            phpqb=phpqb/emjs/emjs                                       
            ptpart=1.                                                   
            if(emlef.gt.0.) then                                        
              emlef=sqrt(emlef)                                         
            else                                                        
              call errex('imaginary emlef in string decay')             
            end if                                                      
            if(abs(emlef-em(l)).gt.0.001) then                          
              write(8,*) 'difference between effective and free mass'   
              write(8,*)'noticed in string:  emlef,em=',emlef,em(l)     
              write(8,*) 'l,ident=',l,issn(l)                           
              if(kfa.ne.17.and.kfa.ne.18.and.kfa.ne.19.and.kfa.ne.23    
     a                               .and.kfa.ne.41.and.kfa.ne.42) then 
                em(l)=emlef                                             
                write(8,*) issn(l),' got its new effective mass'        
              end if                                                    
            end if                                                      
            do 41 i=0,3                                                 
             frfp(i,nidhad)=                                            
     a              ptpart*(p(i,l)-2.*(phpq*pqbar(i)+phpqb*pq(i)))      
             p(i,l)=p(i,l)-frfp(i,nidhad)                               
             psyst(i)=psyst(i)-frfp(i,nidhad)                           
   41       continue                                                    
            plor=frfp(0,nidhad)**2-frfp(1,nidhad)**2-                   
     a                          frfp(2,nidhad)**2-frfp(3,nidhad)**2     
            if(plor.gt.0.001) then                                      
              write(8,*) 'string:nidhad=',nidhad,phpq,phpqb             
              write(8,*) 'pq=',(pq(i),i=0,3)                            
              write(8,*) 'pqbar=',(pqbar(i),i=0,3)                      
              write(8,*) nidhad,'p=',(p(i,l),i=0,3)                     
              write(8,*) 'frfp=',(frfp(i,nidhad),i=0,3)                 
              write(8,*) 'plor=',plor,'should be nonpositively definite'
              call errex('numerical problem in string')                 
            end if                                                      
          end if                                                        
          decay(l)=tdtg(nidhad)                                         
        else                                                            
          if(lstart.gt.0.and.                                           
     a        l.ge.lstart+izcqst.and.l.le.lstart+izcqst+nstpur-1) then  
            decay(l)=x(0,l)                                             
          else                                                          
            startt=x(0,l)                                               
            call dupdat(l,startt)                                       
          end if                                                        
        end if                                                          
        if(notdly.eq.0.and.lstart.gt.0.and.                             
     a      l.ge.lstart+izcqst.and.l.le.lstart+izcqst+nstpur-1) then    
          iswprp=2                                                      
        else                                                            
     a  if(notdly.eq.1.or.ida(23).ne.0) then                            
          iswprp=0                                                      
        else                                                            
          iswprp=1                                                      
          do 345 i=0,3                                                  
 345      qdot(i,l)=0.                                                  
          ichbig(l)=1                                                   
        end if                                                          
        if(ida(15).gt.0) then                                           
          if(notdly.eq.1)then                                           
          else if(notdly.eq.0.and.lstart.gt.0.and.                      
     a      l.ge.lstart+izcqst.and.l.le.lstart+izcqst+nstpur-1) then    
              iswprp=2                                                  
          else                                                          
            do 346 i=0,3                                                
 346        qdot(i,l)=0.                                                
            ichbig(l)=3                                                 
          end if                                                        
        end if                                                          
        if(kdt.le.ndt.and.l.le.iofset+iclusz(kclst).and.ntag.eq.0) then 
          if(ida(42).eq.1) then                                         
            identl=issn(l)                                              
            write(7)                                                    
     &          ityp(l),issn(l),npidec(l),                              
     &          x(0,l),x(1,l),x(2,l),x(3,l),                            
     &          p(0,l),p(1,l),p(2,l),p(3,l),em(l),                      
     &          tdtg(identl),itdth(2,identl),itdth(3,identl)            
            if(notdly.eq.1) then                                        
              do 352 i=0,3                                              
 352          pform(i)=p(i,l)+frfp(i,identl)                            
              write(7)                                                  
     &          ityp(l),issn(l),npidec(l),                              
     &          (xform(i),i=0,3),                                       
     &          (pform(i),i=0,3),em(l),                                 
     &          0.0,0,0                                                 
            end if                                                      
          end if                                                        
          call propag(l,-2,iswprp)                                      
        else                                                            
          do 347 i=0,3                                                  
 347      qdot(i,l)=0.                                                  
        end if                                                          
        k(lloop,1)=l                                                    
  20   continue                                                         
       if(ida(25).eq.1) then                                            
         if(ntag.eq.0) then                                             
           if(ida(36).eq.1) then                                        
              if(ifndnx.eq.0) then                                      
                 call dnscal(1,ind,-1,dens1)                            
                 write(15,663) dens1                                    
              else                                                      
                 write(15,663)  0.                                      
              end if                                                    
           end if                                                       
         end if                                                         
  663    format(1x,1(f6.3,1x))                                          
       end if                                                           
       if(iditag.ne.0) write(8,*) 'iditag=',iditag,'??????',            
     a             kdt,ident,itripl                                     
       if(lindf.eq.1) then                                              
         iswprp=1                                                       
         ityp(ind)=-9999                                                
         decay(ind)=1.e+35                                              
         call propag(ind,-2,iswprp)                                     
       end if                                                           
       if(izcq.ne.0.and.ida(29).eq.3) then                              
         call errex('final izcq.ne.0 in string')                        
       end if                                                           
       if((kleft.eq.0.or.kright.eq.0)) then                             
         write(8,*) 'kleft,kright=',kleft,kright                        
         call errex('error  in subr. string')                           
       end if                                                           
       if(lstart.gt.0) then                                             
         nstp=nstp+nstpur                                               
         lstart=lstart+izcqst                                           
         identb=issn(lstart)                                            
         do 299 i=0,3                                                   
  299    frfp(i,identb)=xold(i)                                         
         lend=lstart+nstpur-1                                           
         if(nidbeg.eq.0) then                                           
           itdth(1,identb)=-identb                                      
           itdth(3,identb)=ltdt3                                        
           inkre=nlund+irrel+1                                          
           if(icqq.eq.1) inkre=inkre+1                                  
           lbrek=k(inkre,1)                                             
           if(lbrek.lt.0) lbrek=100-lbrek                               
           nclcnt(identb)=lbrek                                         
         else                                                           
           ldif=nlund+irrel                                             
           if(icqq.eq.1) ldif=ldif+1                                    
           do 301 l=lstart,lend                                         
            nidhad=issn(l)                                              
            itdth(3,nidhad)=ltdt3                                       
            if(itdth(1,nidhad).ne.0  .or.                               
     a                       itdth(2,nidhad).ne.0  ) then               
              call  errex(                                              
     a          'tdtag.ne.0 for newly formed hadrons in string')        
            end if                                                      
            ldif=ldif+1                                                 
            lbrek=k(ldif,1)                                             
            if(iabs(lbrek).gt.100) then                                 
              lbrek=isign(1,lbrek)*mod(iabs(lbrek),100)                 
            end if                                                      
            if(lbrek.lt.0) lbrek=100-lbrek                              
            nclcnt(nidhad)=lbrek                                        
            itdth(1,nidhad)=nidbeg                                      
  301      continue                                                     
           if(zplsum.lt.cvalu0.or.zplsum.gt.1.001) then                 
             write(8,*)                                                 
     a         'string: strange value for zplsum:',zplsum,ident         
             zplsum=min(cvalu1,max(cvalu0,zplsum))                      
           end if                                                       
           do 304 i=0,3                                                 
  304      frfp(i,nidhad)=pq(i)*zplsum                                  
         end if                                                         
       end if                                                           
       nlstor=nlund                                                     
       nlund=nsave                                                      
       do 30 lloop=nsave+kt+1,nlstor                                    
        l=k(lloop,1)                                                    
        if(l.le.0.or.l.gt.ntotma)                                       
     a           call errex('jetdec30, abort')                          
        eml=em(l)                                                       
        ident=issn(l)                                                   
        call oldnew(1,ityp(l),npidec(l),eml,kmulth,kfm)                 
        if(kmulth.ne.0.and.iabs(kfm).gt.40) call makjet(l)              
  30   continue                                                         
      return                                                            
      end                                                               
