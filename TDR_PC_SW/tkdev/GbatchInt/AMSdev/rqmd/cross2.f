CPAT,CROSS2   .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine cross2(srt,pr,em1,em2,npid1,npid2,icltyp,              
     &               istra1,istra2,imeso1,imeso2,ibry1,ibry2,           
     &                  ityp1,ityp2,sig,sigel,sigin,icnum,nscop,nonstr) 
        include 'scatter7'                                              
        include 'sigmasx'                                               
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
        real*4 empar(99,4),itpar(99,4),nppar(99,4)                      
        common/sigsum/vkp,vlam,vtot,vsgp,sgt,vk0,vksp,vknul,vkm,        
     &                vk0kq,vkpkq,vk0km,vkpkm,empar,itpar,nppar         
      dimension sigin(mxchan)                                           
      real*4 cgks                                                       
      real*4 pjin(4)                                                    
      common /pjbk/ pjin                                                
      data vernum/'version rqmd 1.09 -- 14-jul-93'/                     
      data pronam/'module: c r o s s 2'/                                
      data ttvers/.true./                                               
      data j1d,j2d,j1dnew,j2n,j2dnew,ibasd,ibasn                        
     a       /3,  3,   3,    1,   3,    9,    3    /                    
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      tblsft=0.                                                         
      nscop=0                                                           
      if(icltyp.le.66) then                                             
          s=sqrt(em1**2+pr*pr)+sqrt(em2**2+pr*pr)                       
      else                                                              
          s=srt                                                         
      end if                                                            
      nonstr=2                                                          
      ibn1=0                                                            
      ibn2=0                                                            
      if((icltyp.ge.4754).and.(icltyp.le.4950)) then                    
        if(imeso1.eq.1.and.imeso2.eq.1) then                            
        else                                                            
            go to 720                                                   
        end if                                                          
      else                                                              
        if(ityp1.le.15) then                                            
           call  retcgk(ityp1,npid1,j1,m1,-1,cgks)                      
           if(ibry1.gt.0) ibn1=1                                        
        else                                                            
            call errex('wrong ityp in cross2')                          
        end if                                                          
        if(ityp2.le.15) then                                            
           call  retcgk(ityp2,npid2,j2,m2,-1,cgks)                      
           if(ibry2.gt.0) ibn2=1                                        
        else                                                            
           call errex('wrong ityp in cross2')                           
        end if                                                          
      end if                                                            
      if (ibn1.eq.1.and.ibn2.eq.1) then                                 
      goto (                                                            
     >  10, 20, 10, 50, 50,                                             
     >   65, 50, 50,  65,65,                                            
     >  50, 50, 65, 65, 65,                                             
     >  50, 50, 65, 65,  65,                                            
     >   65, 40,  40, 40, 40,                                           
     >  40, 40, 40, 40, 40,                                             
     >  40, 40, 40, 40, 40,                                             
     >  40, 40, 40, 40, 40,                                             
     >  40, 40, 40, 40, 40,                                             
     > 110,110,110,110,110,                                             
     > 110, 40, 40, 40,110,                                             
     > 110,110,110,110,110,110,40 ,40 ,40 ,110 ,                        
     > 110, 40, 40, 40, 40, 40, 40, 40, 40, 40 ,                        
     >  40, 40, 40,150,150,150,150,150,150, 40 ,                        
     >  40, 40,150,150, 40,150, 40, 40, 40, 40 ,                        
     >  40, 40, 40, 40, 40, 40, 40, 40, 40, 40 ,                        
     > 150,150,150,150,150,150, 40, 40, 40,150 ,                        
     > 150, 40,150, 40,150                                              
     & ) , icltyp                                                       
   40 continue                                                          
      write(8,*)' icltyp invalid in  cross2(bb)   ',icltyp,ibn1,ibn2    
      call errex( 'fatal error')                                        
      return                                                            
 10   continue                                                          
      if (s.lt.2.*emnuc+empion+ekinmi ) then                            
        icnum=1                                                         
        return                                                          
      end if                                                            
      if(s .ge. tblhig ) then                                           
         sigin(1)=sig-sigel                                             
         icnum=2                                                        
         return                                                         
      end if                                                            
      iofs=itblsz * 6                                                   
      assign 100 to label                                               
      goto 1000                                                         
  100 sigin(1)=.25*sintot                                               
      sigin(2)=.75*sintot                                               
      if(s.lt.snnlim.and.ida(4).gt.0) then                              
         sigin(1)=enhcnd*sigin(1)                                       
         sigin(2)=enhcnd*sigin(2)                                       
      end if                                                            
      iofs=itblsz * 8                                                   
      assign 101 to label                                               
      goto 1000                                                         
  101 sigin(3)= sintot                                                  
      iofs=itblsz * 10                                                  
      assign 102 to label                                               
      goto 1000                                                         
  102 sigin(4)= 0.6 * sintot                                            
      sigin(5)= 0.4 * sintot                                            
      iofs=itblsz * 12                                                  
      assign 103 to label                                               
      goto 1000                                                         
  103 sigin(6)= 0.25 * sintot                                           
      sigin(7)= 0.75 * sintot                                           
      iofs=itblsz * 13                                                  
      assign 104 to label                                               
      goto 1000                                                         
  104 sigin(8)= sintot                                                  
      k=8                                                               
      nonstr=8                                                          
      icnum=k+1                                                         
      if(s.gt.snnlim.and.ida(26).gt.0) then                             
         return                                                         
      else                                                              
         goto 6000                                                      
      end if                                                            
 20   continue                                                          
      if (s.lt.2.*emnuc+empion+ekinmi ) then                            
        icnum=1                                                         
        return                                                          
      end if                                                            
      if(s .ge. tblhig ) then                                           
         sigin(1)=sig-sigel                                             
         icnum=2                                                        
         return                                                         
      end if                                                            
      iofs=itblsz * 6                                                   
      assign 200 to label                                               
      goto 1000                                                         
  200 sigin(1)=.25*sintot                                               
      sigin(2)=sigin(1)                                                 
      if(s.lt.snnlim.and.ida(4).gt.0) then                              
         sigin(1)=enhcnd*sigin(1)                                       
         sigin(2)=enhcnd*sigin(2)                                       
      end if                                                            
      iofs=itblsz * 9                                                   
      assign 201 to label                                               
      goto 1000                                                         
  201 sigin(3)= 0.5 * sintot                                            
      sigin(4)= sigin(3)                                                
      iofs=itblsz * 10                                                  
      assign 202 to label                                               
      goto 1000                                                         
  202 sigin(5)= 0.05* sintot                                            
      sigin(6)= 0.45* sintot                                            
      iofs=itblsz * 11                                                  
      assign 203 to label                                               
      goto 1000                                                         
  203 sigin(5)= sigin(5) + 0.25 * sintot                                
      sigin(6)= sigin(6) + 0.25 * sintot                                
      iofs=itblsz * 12                                                  
      assign 204 to label                                               
      goto 1000                                                         
  204 sigin(7)= 0.25 * sintot                                           
      sigin(8)= sigin(7)                                                
      iofs=itblsz * 14                                                  
      assign 205 to label                                               
      goto 1000                                                         
  205 sigin(9)= sintot                                                  
      sigin(10)=0.                                                      
      sigin(11)=0.                                                      
      k=11                                                              
      nonstr=11                                                         
      icnum=k+1                                                         
      if(s.gt.snnlim.and.ida(26).gt.0) then                             
         return                                                         
      else                                                              
         goto 6000                                                      
      end if                                                            
   50 continue                                                          
      k=2                                                               
      if(s.lt.tblhig.and.iabs(npid1).le.1.and.iabs(npid2).le.2) then    
        k=6                                                             
        call wqdnch(icltyp,ityp1,ityp2,npid1,npid2,s,pr,em1,em2,        
     a                       sigeno,sigel1,sigel2,sigdd1,sigdd2,sigdb)  
        sigin(1)=sigdb                                                  
        sigin(3)=sigel1                                                 
        sigin(4)=sigel2                                                 
        sigin(5)=sigdd1                                                 
        sigin(6)=sigdd2                                                 
        siginl=sigin(1)                                                 
        do 501 jch=1,4                                                  
  501   siginl=siginl+sigin(jch+2)                                      
      else                                                              
        call detbal(s,pr,icltyp,ityp1,npid1,ityp2,npid2,em1,em2,sigdb)  
        sigin(1)=sigdb                                                  
        siginl=sigin(1)                                                 
      end if                                                            
      if(siginl.gt.sig-sigel+1.) then                                   
         write(8,*) 'cross2-warning: sum of channels> sigtot for d n'   
         write(8,*) sigin(1),(sigin(ij),ij=3,6), sig, sigel,icltyp,s    
      end if                                                            
      nonstr=k                                                          
      sigin(2)= max(cvalu0,sig-sigel-siginl)                            
      icnum=k+1                                                         
      if(s.gt.snnlim.and.ida(26).gt.0) then                             
         return                                                         
      else                                                              
         goto 6000                                                      
      end if                                                            
   65 continue                                                          
      k=2                                                               
      sigin(3)=0.                                                       
      sigin(4)=0.                                                       
      if(s.lt.tblhig)then                                               
        call detbal(s,pr,icltyp,ityp1,npid1,ityp2,npid2, em1,em2,sigdb) 
        sigin(1)= min(sig-sigel, sigdb)                                 
        if(sigdb.gt.sig-sigel+1.) then                                  
           write(8,*) 'delta + delta'                                   
           write(8,*) 'warning from cross2: sigdb>sig-sigel'            
           write(8,*) s,pr,icltyp,ityp1,npid1,ityp2,npid2               
           write(8,*) em1,em2,sigdb,sig,sigel                           
        end if                                                          
        if(iabs(npid1).le.1.and.iabs(npid2).le.1) then                  
          k=4                                                           
          iofs=itblsz*23                                                
          assign 301 to label                                           
          goto 1000                                                     
  301     continue                                                      
          emmxd=s-max(em1,em2)                                          
          vfd=pi/(atan(2.*(emmxd-emdelt)/widdlt)-                       
     a                 atan(2.*(emnuc+empion-emdelt)/widdlt))           
          sintot=vfd*da(12)*sintot                                      
          if(s.gt.emnuc+emdelt) then                                    
            pout=pcmsr(s,emnuc,emdelt)                                  
          else                                                          
            pout=0.                                                     
          end if                                                        
          m1= 2 * ityp1 - ibasd                                         
          m2= 2 * ityp2 - ibasd                                         
          itag=-100                                                     
          call isocgk(                                                  
     a               m1,j1d,m2,j2d,j1dnew,j2dnew,m1new,m2new,itag)      
          m=m1+m2                                                       
          ino=0                                                         
          do 305 imt=iabs(m),6,2                                        
           if(iabs(m)+2*ino.eq.2)then                                   
             sigin(3)=0.5*pout**2/pr/pr*sintot*0.44444*pjin(ino+1)      
             sigin(4)=0.5*pout**2/pr/pr*sintot*0.8*pjin(ino+2)          
             go to 306                                                  
           else if(iabs(m)+2*ino.eq.4)then                              
             sigin(3)=0.                                                
             sigin(4)=0.5*pout**2/pr/pr*sintot*0.8*pjin(ino+1)          
             go to 306                                                  
           end if                                                       
           ino=ino+1                                                    
  305    continue                                                       
  306    continue                                                       
        end if                                                          
      else                                                              
        sigin(1)=0.                                                     
      end if                                                            
      sigin(2)=sig-sigel-sigin(1)-sigin(3)-sigin(4)                     
      if (sigin(2).lt.0.) then                                          
       if (sigin(2).lt.-2.) then                                        
         write(8,*)                                                     
     a       'warning from cross2: sum(inelastic channels) > siginel'   
         write(8,*) icltyp,s,sigin(2),sig,sigel                         
       end if                                                           
       sigin(2)=0.                                                      
      end if                                                            
      nonstr=k                                                          
      icnum=k+1                                                         
      if(s.gt.snnlim.and.ida(26).gt.0) then                             
         return                                                         
      else                                                              
         goto 6000                                                      
      end if                                                            
  110 continue                                                          
      k=2                                                               
      if(s.lt.tblhig)then                                               
        call detbal(s,pr,icltyp,ityp1,npid1,ityp2,npid2, em1,em2,sigdb) 
        sigin(1)= min(sig-sigel, sigdb)                                 
        if(sigdb.gt.sig-sigel+1.) then                                  
           write(8,*) 'warning from cross2: sigdb>sig-sigel'            
           write(8,*) s,pr,icltyp,ityp1,npid1,ityp2,npid2               
           write(8,*) em1,em2,sigdb,sig,sigel                           
        end if                                                          
      else                                                              
          sigin(1)=0.                                                   
      end if                                                            
      sigin(2)=sig-sigel-sigin(1)                                       
      if (sigin(2).lt.0.) then                                          
       if (sigin(2).lt.-2.) then                                        
         write(8,*)                                                     
     a       'warning from cross2: sum(inelastic channels) > siginel'   
         write(8,*) icltyp,s,sigin,sig,sigel                            
       end if                                                           
       sigin(2)=0.                                                      
      end if                                                            
      nonstr=k                                                          
      icnum=k+1                                                         
      if(s.gt.snnlim.and.ida(26).gt.0) then                             
         return                                                         
      else                                                              
         goto 6000                                                      
      end if                                                            
  150 continue                                                          
      sigin(1)=0.                                                       
      sigin(2)=sig-sigel                                                
      k=2                                                               
      nonstr=2                                                          
      icnum=k+1                                                         
      return                                                            
 6000 continue                                                          
      iflag=0                                                           
      ikinp=0                                                           
      nscop=k                                                           
         parstr=0.                                                      
         call getbb(s,sigin,k,ityp1,ityp2,npid1,npid2,em1,em2,          
     &   itp1,itp2,itp3,npd1,npd2,npd3,emb,emy,emk,iflag,ikinp,         
     &   parstr,arat)                                                   
      if(ida(26).gt.0.and.icltyp.gt.3) then                             
         do 6001 i=nonstr+1,k                                           
          sigin(i)=0.                                                   
 6001    continue                                                       
      end if                                                            
         iflg=0                                                         
         call gpair(icltyp,ityp1,npid1,em1,ityp2,npid2,em2,             
     &               itp1,npd1,e1,                                      
     &               itp2,npd2,e2,                                      
     &               itp3,npd3,e3,                                      
     &               itp4,npd4,e4,                                      
     &               ikinp,iflg,                                        
     &               sigin,k,nscop,s,parstr,arat)                       
      if(icltyp.gt.3) then                                              
          sumin=0.                                                      
          if (k.gt.nonstr) then                                         
                     do 6002 i=nonstr+1,k                               
                     sumin=sumin+sigin(i)                               
 6002                continue                                           
              if(sigin(2).lt.sumin) then                                
                 sigin(2)=0.                                            
              else                                                      
                 sigin(2)=sigin(2)-sumin                                
              end if                                                    
          end if                                                        
      end if                                                            
      icnum=k+1                                                         
      return                                                            
      else if (ibn1.eq.0.and.ibn2.eq.0) then                            
      nscop=2                                                           
      sigin(1)=0.                                                       
      nonstr=2                                                          
      sigadd=sigasy(1,imeso1,istra1,imeso2,istra2)-                     
     a           sigasy(0,imeso1,istra1,imeso2,istra2)                  
      rlimit=min(cvalu1,1.6/s)                                          
      sigin(2)=(1.-rlimit)*sigadd                                       
      sigin(3)=sig-sigin(2)                                             
      if(sigin(3).lt.0.0) then                                          
        write(8,*) icltyp,ityp1,npid1,ityp2,npid2,s                     
        write(8,*) 'sigadd,rlimit,istra1,imeso1,istra2,imeso2'          
        write(8,*) sigadd,rlimit,istra1,imeso1,istra2,imeso2            
        write(8,*) 'sigel,sig,sigin(2),sigin(3)'                        
        write(8,*) sigel,sig,sigin(2),sigin(3)                          
        call errex('cross2:mm-sigin(3)<0')                              
      end if                                                            
      icnum=4                                                           
      return                                                            
      else if(ibn1.eq.1.or.ibn2.eq.1) then                              
   60 continue                                                          
      sigin(1)=0.                                                       
      sigin(2)=sig-sigel                                                
      k=2                                                               
      nonstr=k                                                          
      if(icltyp.eq.67.or.icltyp.eq.68) then                             
        go to 601                                                       
      else if(icltyp.le.75) then                                        
        go to 44                                                        
      else                                                              
          goto(                                                         
     >    601,601,41, 41, 41, 41, 41, 41, 41, 44,                       
     >     44, 44, 41, 41, 44, 41,622,622, 44, 44,                      
     >    44, 44, 41, 41, 41, 44, 44, 41,601, 41,                       
     >    41, 41, 41, 41, 41, 41, 44, 44, 44, 41,                       
     >    41, 44, 41, 44, 41                                            
     &                     ) , icltyp-75                                
      end if                                                            
   41 continue                                                          
      write(8,*)' icltyp invalid in  cross2(mb) ',icltyp,ibn1,ibn2      
      call errex( 'fatal error')                                        
  601 continue                                                          
      ictest=0                                                          
      call  enlk(icltyp,ityp1,npid1,ityp2,npid2,                        
     a                          em1,em2,s,pr,ictest)                    
      if(ictest.ne.0) then                                              
        k=k+1                                                           
        sigin(k)=10.0*(1.5/s)**2                                        
      end if                                                            
      go to 44                                                          
  622 continue                                                          
      sigin(1)=0                                                        
      sigin(2)=sig-sigel                                                
      k=2                                                               
      nonstr=k                                                          
      if(s.le.tbllow-0.46) s=1.44                                       
      if((ityp1.eq.14.and.npid1.lt.0.and.ityp2.le.2)                    
     &     .or.(ityp2.eq.14.and.npid2.lt.0.and.ityp1.le.2)) then        
      elseif((ityp1.eq.14.and.npid1.gt.0.and.ityp2.le.2)                
     &          .or.(ityp2.eq.14.and.npid2.gt.0.and.ityp1.le.2)) then   
           if(m1+m2.eq.0) then                                          
             tblsft=0.46                                                
             iofs=(kaofs+14)*itblsz                                     
             assign 702 to label                                        
             goto 1000                                                  
  702        continue                                                   
             sigin(k+1)=sintot                                          
             k=k+1                                                      
           else                                                         
             tblsft=0.46                                                
             iofs=(kaofs+16)*itblsz                                     
             assign 703 to label                                        
             goto 1000                                                  
  703        continue                                                   
             sigin(k+1)=sintot                                          
             k=k+1                                                      
             iofs=(kaofs+17)*itblsz                                     
             assign 704 to label                                        
             goto 1000                                                  
  704        continue                                                   
             sigin(k+1)=sintot                                          
             k=k+1                                                      
             iofs=(kaofs+18)*itblsz                                     
             assign 705 to label                                        
             goto 1000                                                  
  705        continue                                                   
             sigin(k+1)=sintot                                          
             k=k+1                                                      
           end if                                                       
      else                                                              
           call errex('wrong  type in cross2')                          
      end if                                                            
   44 continue                                                          
      if ((k.gt.nonstr)) then                                           
                     sinto=0.                                           
                     do 53 i=nonstr+1,k                                 
                     sinto=sinto+sigin(i)                               
   53                continue                                           
                     if (sinto.gt.sigin(2)) then                        
                       do 623 i=nonstr+1,k                              
                       sigin(i)=sigin(i)*sigin(2)/sinto                 
  623                  continue                                         
                       sigin(2)=0.                                      
                     else                                               
                       do 624 i=nonstr+1,k                              
                       sigin(2)=sigin(2)-sigin(i)                       
  624                  continue                                         
                     end if                                             
      end if                                                            
      icnum=k+1                                                         
      return                                                            
      end if                                                            
  720  continue                                                         
      sigin(1)=0.                                                       
      sigin(2)=sig-sigel                                                
      nonstr=2                                                          
      icnum=3                                                           
      return                                                            
 1000 xpt=log(s+tblsft)                                                 
      tlow=log(tbllow)                                                  
      index=int((xpt-tlow)/tblstp)+1                                    
      if(index.le.0)then                                                
            sintot=0.                                                   
            goto label                                                  
      else if(index.ge.itblsz) then                                     
            sintot=sigcom(itblsz+iofs)                                  
            goto label                                                  
      end if                                                            
      x1=(index-1)*tblstp + tlow                                        
      index=index+iofs                                                  
      if(index .gt.indmax)                                              
     a call errex(' index too big in cross2' )                          
      y1in=sigcom(index)                                                
      y2in=sigcom(index+1)                                              
      slin=(y2in-y1in)/tblstp                                           
      sintot=slin*(xpt-x1) + y1in                                       
      goto label                                                        
      end                                                               
      subroutine                                                        
     a     enlk(icltyp,ityp1,npid1,ityp2,npid2,em1,em2,s,pr,ic)         
        include 'scatter7'                                              
        include 'sigmasx'                                               
        real*4 cgks                                                     
       icstor=ic                                                        
       if(icltyp.eq.67.or.icltyp.eq.68) then                            
         iforw=1                                                        
         ibares=0                                                       
         if(ic.eq.0) ic=1                                               
       else if(icltyp.eq.76.or.icltyp.eq.77) then                       
         iforw=1                                                        
         ibares=1                                                       
         if(ic.eq.0) ic=1                                               
       else if(icltyp.eq.104) then                                      
         iforw=2                                                        
         if((ityp1.eq.14.and.npid1.gt.0).or.                            
     a      (ityp2.eq.14.and.npid2.gt.0)) then                          
           if(ic.eq.0) ic=1                                             
         end if                                                         
         if((ityp1.eq.13.and.em1.lt.1.2).or.                            
     a             (ityp2.eq.13.and.em2.lt.1.2)) then                   
           ibares=1                                                     
         else                                                           
           ibares=0                                                     
         end if                                                         
       else                                                             
         call errex('enlk:wrong icltyp')                                
       end if                                                           
       if(icstor.eq.0) return                                           
       sigmax=10.0*(1.5/s)**2                                           
       tblsft=0.46                                                      
       iofs=(kaofs+14)*itblsz                                           
       xpt=log(s+tblsft)                                                
       tlow=log(tbllow)                                                 
       index=int((xpt-tlow)/tblstp)+1                                   
       if(index.ge.itblsz) then                                         
         sintot=0.                                                      
       else if(s.gt.emlamb+emka0) then                                  
         x1=(index-1)*tblstp + tlow                                     
         index=index+iofs                                               
         if(index.gt.indmax)                                            
     a            call errex(' index too big in enlk')                  
         y1in=sigcom(index)                                             
         y2in=sigcom(index+1)                                           
         slin=(y2in-y1in)/tblstp                                        
         sintot=slin*(xpt-x1) + y1in                                    
       else                                                             
         sintot=0.                                                      
       end if                                                           
       call  retcgk(ityp1,npid1,j1,m1,-1,cgks)                          
       call  retcgk(ityp2,npid2,j2,m2,-1,cgks)                          
       m=m1+m2                                                          
       if(ityp1.eq.12.or.ityp1.eq.14) then                              
         isw=1                                                          
       else                                                             
         isw=0                                                          
       end if                                                           
       if(iforw.eq.1) then                                              
         if(ibares.eq.1) then                                           
           if(isw.eq.0) embar=em1+0.16                                  
           if(isw.eq.1) embar=em2+0.16                                  
           if(embar.lt.1.4) embar=emlamb                                
         else                                                           
           embar=emlamb                                                 
         end if                                                         
         if(m.eq.1) then                                                
           emmes=emkap                                                  
         else                                                           
           emmes=emka0                                                  
         end if                                                         
       else                                                             
         if(ibares.eq.1) then                                           
           if(isw.eq.0) embar=em1-0.16                                  
           if(isw.eq.1) embar=em2-0.16                                  
           if(embar.lt.1.3) then                                        
             embar=emnuc                                                
             ibares=0                                                   
           end if                                                       
         else                                                           
           embar=emnuc                                                  
         end if                                                         
         emmes=emeta                                                    
       end if                                                           
       if(s.gt.embar+emmes) then                                        
         pout=pcmsr(s,embar,emmes)                                      
       else                                                             
         pout=0.                                                        
       end if                                                           
       senlk=pout**2/pr**2*sintot                                       
       if(senlk.gt.sigmax) then                                         
         write(8,*) 'warning:  eta n(*) <==> lambda(*)+kaon'            
         write(8,*) 'sigma>max',ityp1,npid1,ityp2,npid2,em1,em2,senlk   
       end if                                                           
       if(ranf(0).lt.senlk/sigmax) then                                 
          if(iforw.eq.1) then                                           
            ityp1=13                                                    
            npid1=0                                                     
            if(ibares.eq.1.and.embar.ge.1.4) then                       
              ityp1=85                                                  
              npid1=57                                                  
            end if                                                      
            ityp2=14                                                    
            npid2=(m+3)/2                                               
          else                                                          
            ityp1=(m+3)/2                                               
            if(ibares.eq.0) then                                        
              npid1=0                                                   
            else                                                        
              npid1=-mbafnd(2,em1)                                      
            end if                                                      
            ityp2=12                                                    
            npid2=2                                                     
          end if                                                        
          em1=embar                                                     
          em2=emmes                                                     
          if(isw.eq.1) then                                             
            call swap(em1,em2)                                          
            call iswap(npid1,npid2)                                     
            call iswap(ityp1,ityp2)                                     
          end if                                                        
       else                                                             
          ic=0                                                          
       end if                                                           
      return                                                            
      end                                                               
      function piynkb(i1,m1,i2,m2,itot,jnum,srt,pin)                    
       include 'scatter7'                                               
       include 'sigmasx'                                                
       data srtofs /0.46/                                               
       real*4 cgks                                                      
       piynkb=0.0                                                       
       inum=jnum                                                        
       ivz=isign(1,inum)                                                
       inum=iabs(inum)                                                  
       if(inum.le.0.or.inum.gt.4) then                                  
         call errex('piynkb: inum out of range')                        
       end if                                                           
       if(inum.eq.1) then                                               
         if(itot.ne.2) return                                           
         if(i2.eq.2) return                                             
       else if(inum.eq.2) then                                          
         if(itot.ne.0) return                                           
         if(i2.eq.0) return                                             
       else if(inum.eq.3) then                                          
         if(itot.ne.2) return                                           
         if(i2.eq.0) return                                             
       else if(inum.eq.4) then                                          
         if(itot.ne.0) return                                           
         if(i2.eq.0.or.i2.eq.2) return                                  
       end if                                                           
       s=srt                                                            
       if(s.le.1.441) s=1.441                                           
       cgk2=cgks(i1,i2,itot,m1,m2)                                      
       if(inum.le.3) then                                               
         iofs=itblsz*(kaofs+inum-1)                                     
       else                                                             
         iofs=itblsz*(kaofs+15)                                         
       end if                                                           
       xpt=log(s+srtofs)                                                
       tlow=log(tbllow)                                                 
       maxtbl=itblsz                                                    
       index=int((xpt-tlow)/tblstp)+1                                   
       if(index.ge.maxtbl)then                                          
         sintot=0.                                                      
       else                                                             
         x1=(index-1)*tblstp+tlow                                       
         index=index+iofs                                               
         y1in=sigcom(index)                                             
         y2in=sigcom(index+1)                                           
         slin=(y2in-y1in)/tblstp                                        
         sintot=slin*(xpt-x1)+y1in                                      
       end if                                                           
       if(inum.eq.4) sintot=2.*sintot                                   
       if(ivz.eq.1.and.inum.le.3) then                                  
         if(inum.eq.1) then                                             
           emy=emlamb                                                   
         else                                                           
           emy=emsig0                                                   
         end if                                                         
         pout=pcmsr(s,emy,empion)                                       
         sintot=sintot*(pout/pin)**2                                    
       end if                                                           
       piynkb=cgk2*sintot                                               
      return                                                            
      end                                                               
