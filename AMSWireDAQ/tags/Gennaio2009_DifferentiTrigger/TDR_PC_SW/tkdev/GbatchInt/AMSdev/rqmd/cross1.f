CPAT,CROSS1   .                                                         
      subroutine cross1(iswel,srt,pr,icltp,sig,em1,em2,                 
     a                                 ityp1,ityp2,npid1,npid2)         
        include 'scatter7'                                              
        include 'sigmasx'                                               
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      real*4 pjin                                                       
      dimension pjin(4)                                                 
      common /pjbk/ pjin                                                
      real*4 cgk212,cgk232                                              
      common /xspibk/ xspin1,xspin3,wnorm1,wnorm3,pow12,pow32,          
     a                xsbgl1,xsbgl3,xs1218,xs32pn,xs12pn                
      real*4 a,b,en,c,d                                                 
      data emkofs /0.483/                                               
      data fstrsq / 4.5238/                                             
      data   ibaspi,j1,j1new,j2,j2new,j1d,j2d,j1dnew,j1n,j2n,j2dnew,    
     @       ibasd,ibasn,zmltp,parama,paramb,b0im  /                    
     @           16,2,2,2,2,3,3,3,1,1,3,9,3,4*0.0 /                     
      data vernum/'version rqmd 2.1  -- 24-nov-94'/                     
      data pronam /'cross1 -- total and elastic crossections'/          
      data ttvers/.true./                                               
       tblfit(plab,a,b,en,c,d)=a+b*plab**en+c*(log(plab))**2+           
     a                              d*log(plab)                         
      if(ttvers) then                                                   
        zmltp=80.*pi/(5.0674**2)                                        
        parama=fstrsq/empion/empion/6./pi*emnuc                         
        paramb=20./3.*fstrsq/empion/empion*(hc/pi2)**2                  
        b0im=0.035/empion**4                                            
         xs32pn=36.11                                                   
         xs12pn=34.45                                                   
         pow32=4.6                                                      
         pow12=5.4                                                      
         xspin1=30.0                                                    
         xspin3=30.32                                                   
         xsbgl1=4.7                                                     
         xsbgl3=4.7                                                     
         stest=2.0                                                      
         pcms=pcmsr(stest,emnuc,empion)                                 
         bwigs=0.0                                                      
         do 166 jent=1,9                                                
          call bwdist(1,1,stest,jent,1,zdnp,bwig)                       
          bwigs=bwigs+bwig                                              
  166    continue                                                       
         gmdlt=29.*pcms**3/(1.+40.*pcms*pcms)                           
         bwigs=bwigs+                                                   
     a          4.*gmdlt/((stest-emdelt)**2+0.25*gmdlt**2)              
         sigbws=0.5*pi*10.0*(hc/pi2)**2/pcms**2*bwigs                   
         wnorm3=sigbws/xspin3                                           
         stest=1.8                                                      
         pcms=pcmsr(stest,emnuc,empion)                                 
         bwigs=0.0                                                      
         do 165 jent=1,9                                                
          call bwdist(2,1,stest,jent,1,zdnp,bwig)                       
          bwigs=bwigs+bwig                                              
  165    continue                                                       
         sigbws=0.5*pi*10.0*(hc/pi2)**2/pcms**2*bwigs                   
         xs1218=(37.64-9.78)                                            
         wnorm1=sigbws/xs1218                                           
         write(8,*) 'wnorm1=',wnorm1,'wnorm3=',wnorm3                   
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      icltyp=icltp                                                      
      if(icltyp.gt.maxtyp) icltyp=maxtyp+1                              
      if(icltyp.le.66) then                                             
        s=sqrt(em1**2+pr*pr)+sqrt(em2**2+pr*pr)                         
      else                                                              
        s=srt                                                           
      end if                                                            
      if(s.lt.em1+em2) then                                             
          sig=0.                                                        
          return                                                        
      end if                                                            
      iswold=iswel                                                      
      iswelt=mod(iswel,2)                                               
      itp1=ityp1                                                        
      npd1=npid1                                                        
      itp2=ityp2                                                        
      npd2=npid2                                                        
      igo=0                                                             
   11 continue                                                          
      igo=igo+1                                                         
      if(igo.gt.2) then                                                 
        write(8,*) 'wrong igo in cross1',igo                            
        call errex('cross1:wrong igo')                                  
      end if                                                            
      tblsft=0.0                                                        
      if(icltyp.eq.maxtyp+1) then                                       
        if(itp1.ge.85.and.itp1.le.99) then                              
          if(npd1.eq.34.or.npd1.eq.35) then                             
            if(itp2.le.11.and.mod(itp2,10).le.6)  go to 92              
          else if(npd1.eq.28.or.npd1.eq.29) then                        
            if(itp2.eq.13.or.itp2.eq.15) go to 92                       
          end if                                                        
        else if(itp2.ge.85.and.itp2.le.99) then                         
          if(npd2.eq.34.or.npd1.eq.35) then                             
            if(itp1.le.11.and.mod(itp1,10).le.6) go to 92               
          else if(npd2.eq.28.or.npd2.eq.29) then                        
            if(itp1.eq.13.or.itp1.eq.15) go to 92                       
          end if                                                        
        end if                                                          
      end if                                                            
      goto (                                                            
     >  10, 20, 10, 30, 30, 30, 30, 30, 30, 30,                         
     >  30, 30, 30, 30, 30, 30, 30, 30, 30, 30,                         
     >  30, 80, 80,110, 92, 92, 92,110, 80, 80,                         
     >  92, 92, 92, 92,110,110, 80, 80, 92, 92,                         
     >  92,110,110,110,110, 30, 30, 30, 30, 30,                         
     >  30, 92, 92, 92, 30,                                             
     >  30, 30, 30, 30, 30, 30, 92, 92, 92, 30,                         
     >  30, 92, 92, 92, 92, 92, 92,110,110,110,                         
     >  92, 92,110,110,110,110,110,110,110,110,                         
     > 110,110,110,110,110,110,400,400,110,110,                         
     > 110,110,110,110,110,110,110,110, 86,110,                         
     > 110,110,110,110,110,110,110,110,110,110,                         
     > 110,110,110, 86,110,                                             
     >  110                                                             
     & ) , icltyp                                                       
       write(8,*)' icltyp invalid in  cross1   ',icltyp                 
       write(8,*) itp1,itp2,npd1,npd2                                   
       call errex('cross1:fatal error')                                 
 10    continue                                                         
       if(s.gt.2.*emnuc) then                                           
         ppr=sqrt(((s*s-2.*emnuc*emnuc)/(2.*emnuc))**2-emnuc*emnuc)     
       else                                                             
         ppr=0.                                                         
       end if                                                           
       if(ppr.gt.3.)then                                                
         if(iswelt.eq.1) then                                           
           sig=tblfit(ppr,45.60,219.0,-4.23,0.410,-3.41)                
         else                                                           
           sig=tblfit(ppr,11.20,25.5,-1.12,0.151,-1.62)                 
         end if                                                         
       else                                                             
          if(iswelt.eq.1) then                                          
            ioftab=itblsz*4                                             
          else                                                          
            ioftab=itblsz*5                                             
          end if                                                        
          assign 100 to label                                           
          goto 1000                                                     
  100     sig=sigin                                                     
       end if                                                           
       if(iswelt.eq.1.and.s.lt.snnlim.and.ida(4).gt.0) then             
            ioftab=itblsz*6                                             
            assign 101 to label                                         
            goto 1000                                                   
  101       sig=sig+(enhcnd-1.)*sigin                                   
       end if                                                           
       return                                                           
 20    continue                                                         
       if(s.gt.2.*emnuc) then                                           
         ppr=sqrt(((s*s-2.*emnuc*emnuc)/(2.*emnuc))**2-emnuc*emnuc)     
       else                                                             
         ppr=0.                                                         
       end if                                                           
       if(iswelt.eq.1) then                                             
           if(ppr.gt.280.) then                                         
              sig=38.3+ 0.43*(log(s*s*0.01))**2                         
           else if(ppr.gt.2.)then                                       
              sig=tblfit(ppr,47.70,-100.0,-4.57,0.512,-4.29)            
           else                                                         
              ioftab=itblsz*2                                           
              assign 200 to label                                       
              goto 1000                                                 
  200         sig=sigin                                                 
           end if                                                       
           if(s.lt.snnlim.and.ida(4).gt.0) then                         
              ioftab=itblsz*6                                           
              assign 202 to label                                       
              goto 1000                                                 
  202         sig=sig+0.5*(enhcnd-1.)*sigin                             
           end if                                                       
       else                                                             
           if(s.ge.tblhig)then                                          
              sig=tblfit(ppr,11.20,25.5,-1.12,0.151,-1.62)              
           else                                                         
              ioftab=itblsz*3                                           
              assign 201 to label                                       
              goto 1000                                                 
  201         sig=sigin                                                 
           end if                                                       
       end if                                                           
       return                                                           
 30    continue                                                         
       if(s.lt.tblhig.and.iabs(npd1).le.1.and.iabs(npd2).le.1.and.      
     a     ((itp1.le.2.and.itp2.ge.3.and.itp2.le.6).or.                 
     b           (itp2.le.2.and.itp1.ge.3.and.itp1.le.6))) then         
         idnyes=1                                                       
       else                                                             
         idnyes=0                                                       
       end if                                                           
       if(s.lt.tblhig.and.iabs(npd1).le.1.and.iabs(npd2).le.1.and.      
     a       itp1.ge.3.and.itp1.le.6.and.                               
     b            itp2.ge.3.and.itp2.le.6) then                         
         iddyes=1                                                       
       else                                                             
         iddyes=0                                                       
       end if                                                           
       srtold=s                                                         
       prelsq=pr*pr                                                     
       srtact=s                                                         
       if(srtact.gt.2.*emnuc) then                                      
         ppr=sqrt(((srtact*srtact-2.*emnuc*emnuc)                       
     a                       /(2.*emnuc))**2-emnuc*emnuc)               
       else                                                             
         call errex('cross1:ppr=0.')                                    
       end if                                                           
       if(iswelt.eq.1) then                                             
         if(ppr.gt.280.) then                                           
           sig=38.3+0.43*(log(srtact*srtact*0.01))**2                   
         else if(ppr.gt.2.)then                                         
           sig=tblfit(ppr,47.70,-100.0,-4.57,0.512,-4.29)               
         else                                                           
           s=srtact                                                     
           ioftab=itblsz*2                                              
           assign 300 to label                                          
           goto 1000                                                    
  300      sig=sigin                                                    
           s=srtold                                                     
         end if                                                         
       else                                                             
         if(srtact.ge.tblhig)then                                       
           sig=tblfit(ppr,11.20,25.5,-1.12,0.151,-1.62)                 
         else                                                           
           s=srtact                                                     
           ioftab=itblsz*3                                              
           assign 301 to label                                          
           goto 1000                                                    
  301      sig=sigin                                                    
           s=srtold                                                     
         end if                                                         
       end if                                                           
       if(idnyes.eq.1) then                                             
         go to 35                                                       
       else                                                             
         if(iswelt.eq.1) then                                           
           call detbal(s,pr,icltyp,itp1,npd1,itp2,npd2,em1,em2,sigdb)   
           sig=sig+sigdb                                                
           if(iddyes.eq.1) then                                         
             ioftab=itblsz*23                                           
             assign 302 to label                                        
             goto 1000                                                  
  302        continue                                                   
             m1=2*itp1-ibasd                                            
             m2=2*itp2-ibasd                                            
             itag=-100                                                  
             call isocgk(                                               
     a                m1,j1d,m2,j2d,j1dnew,j2dnew,m1new,m2new,itag)     
             m=m1+m2                                                    
             ino=0                                                      
             sigdd=0.                                                   
             do 305 imt=iabs(m),6,2                                     
              if(iabs(m)+2*ino.eq.2)then                                
                sigdd=0.44444*pjin(ino+1)*sigin                         
                sigdd=sigdd+0.8*pjin(ino+2)*sigin                       
                go to 306                                               
              else if(iabs(m)+2*ino.eq.4)then                           
                sigdd=0.8*pjin(ino+1)*sigin                             
                go to 306                                               
              end if                                                    
              ino=ino+1                                                 
  305        continue                                                   
  306        continue                                                   
             emmxd=s-max(em1,em2)                                       
             vfd=pi/(atan(2.*(emmxd-emdelt)/widdlt)-                    
     a                 atan(2.*(emnuc+empion-emdelt)/widdlt))           
             sigdd=vfd*sigdd*da(12)                                     
             if(s.gt.emnuc+emdelt) then                                 
               pout=pcmsr(s,emnuc,emdelt)                               
             else                                                       
               pout=0.                                                  
             end if                                                     
             sig=sig+0.5*pout**2/prelsq*sigdd                           
           end if                                                       
         end if                                                         
         return                                                         
       end if                                                           
   35  continue                                                         
       if(iswelt.eq.0) then                                             
         if(s.le.emdelt+emnuc) then                                     
           s=sqrt(emnuc**2+pr**2)+sqrt(emdelt**2+pr**2)                 
         end if                                                         
         ioftab=itblsz*22                                               
         assign 351 to label                                            
         goto 1000                                                      
  351    sigeld=sigin                                                   
         s=srtold                                                       
         emmin=emnuc+empion                                             
         emmxd=max(s-emnuc,emmin)                                       
         vfd=1./pi*(atan(2.*(emmxd-emdelt)/widdlt)-                     
     a            atan(2.*(emmin-emdelt)/widdlt))                       
         sigeld=sigeld*vfd                                              
         sigeno=max(cvalu0,vfd*sig-sigeld)                              
         sig=sigeno                                                     
         return                                                         
       else  if(iswelt.eq.1) then                                       
         call wqdnch(icltyp,itp1,itp2,npd1,npd2,s,pr,em1,em2,           
     a                       sigeno,sigel1,sigel2,sigdd1,sigdd2,sigdb)  
         if(s.gt.2.5) then                                              
           sig=max(sig,sigdb+sigel1+sigel2+sigeno+sigdd1+sigdd2)        
         else                                                           
           sig=sigdb+sigel1+sigel2+sigeno+sigdd1+sigdd2                 
         end if                                                         
         return                                                         
       end if                                                           
   80 continue                                                          
      if(npd1.ne.0.or.npd2.ne.0) then                                   
        go to 92                                                        
      end if                                                            
      tblsft=emnuc                                                      
      if(s+tblsft.lt.tbllow ) then                                      
        sig=0.0                                                         
        return                                                          
      end if                                                            
      if((icltyp.eq.22).or.(icltyp.eq.38) )then                         
         imixt=1                                                        
         ioftab=itblsz*7                                                
         ioftb2=itblsz*16                                               
      else if((icltyp.eq.37).or.(icltyp.eq.23) )then                    
         imixt=2                                                        
         ioftab=itblsz*15                                               
         ioftb2=itblsz*17                                               
      else if((icltyp.eq.29).or.(icltyp.eq.30) )then                    
         imixt=3                                                        
         ioftab=itblsz*7                                                
         ioftb2=itblsz*16                                               
      end if                                                            
      ppr=sqrt(((s*s-emnuc*emnuc-empion*empion)/                        
     a                                 (2.*emnuc))**2-empion*empion)    
      if(ppr.le.4.) then                                                
         assign 800 to label                                            
         goto 1000                                                      
  800    if(iswelt.eq.1)then                                            
           if(imixt.eq.3) then                                          
             sig=0.5*sigin                                              
             ioftab=itblsz*15                                           
             assign 810 to label                                        
             goto 1000                                                  
  810        sig=sig+0.5*sigin                                          
           else                                                         
             sig=sigin                                                  
           end if                                                       
         else                                                           
           ioftab=ioftb2                                                
           assign 820 to label                                          
           goto 1000                                                    
  820      if(imixt.eq.3) then                                          
             sig=0.5*sigin                                              
             ioftab=itblsz*17                                           
             assign 830 to label                                        
             goto 1000                                                  
  830        sig=sig+0.5*sigin                                          
           else                                                         
             sig=sigin                                                  
           end if                                                       
         end if                                                         
         if(ida(4).gt.0.and.iswelt.eq.1) then                           
            if(s.le.emnuc+empion.or.s.gt.1.5) then                      
            else                                                        
              pcms=pcmsr(s,emnuc,empion)                                
              gamd=parama/s*pcms**3                                     
              sigin=paramb/(s/emnuc*pcms)*                              
     a             pcms**2*0.04/((s-emdelt)**2+0.25*gamd**2)            
              sig=sig+sigin                                             
              pilu=sqrt(empion*empion+ppr*ppr)                          
              tpion=pilu-empion                                         
              sigsab=1.5*(hc/pi2)**5*20.*pi2/ppr*                       
     a             (1.+0.5*pilu/empion)*b0im*rho0*                      
     b                            exp(-6.*max(cvalu0,tpion-.1))         
              sig=sig+sigsab                                            
            end if                                                      
         end if                                                         
      else                                                              
         pprmx=340.                                                     
         ppr=min(ppr,pprmx)                                             
         if(imixt.eq.1) then                                            
           if(iswelt.eq.1) then                                         
             sig=tblfit(ppr,32.10,48700.0,-7.85,0.540,-4.41)            
           else                                                         
             sig=tblfit(ppr,7.07,11.3,-1.60,0.160,-1.56)                
           end if                                                       
         elseif(imixt.eq.2) then                                        
           if(iswelt.eq.1) then                                         
             sig=tblfit(ppr,33.10,15.0,-1.41,0.458,-4.06)               
           else                                                         
             sig=tblfit(ppr,1.73,11.2,-0.63,0.0437,0.00)                
           end if                                                       
         elseif(imixt.eq.3) then                                        
           if(iswelt.eq.1) then                                         
             sig=0.5*tblfit(ppr,33.10,15.0,-1.41,0.458,-4.06)+          
     a             0.5* tblfit(ppr,32.10,48700.0,-7.85,0.540,-4.41)     
           else                                                         
             sig=0.5*tblfit(ppr,1.73,11.2,-0.63,0.0437,0.0)+            
     a             0.5*tblfit(ppr,7.07,11.3,-1.60,0.160,-1.56)          
           end if                                                       
         endif                                                          
      end if                                                            
      if(iswold.eq.1) then                                              
      else if(iswold.eq.3) then                                         
          if(s.le.2.0) then                                             
            rlimit=1.0                                                  
          else                                                          
            if(imixt.eq.3) then                                         
              xspin=0.3333*xs12pn+0.6667*xs32pn                         
            else if(imixt.eq.2) then                                    
              xspin=0.3333*xs32pn+0.6667*xs12pn                         
            else                                                        
              xspin=xs32pn                                              
            end if                                                      
            rlimit=min(cvalu1,xspin*(4.0/s**2)**da(7)/sig)              
          end if                                                        
          sig=max(cvalu0,sig*(1.-rlimit))                               
      else if(iswold.eq.0) then                                         
        if(iswelt.eq.1) then                                            
          if(s.le.2.0) then                                             
            rlimit=1.0                                                  
          else                                                          
            if(imixt.eq.3) then                                         
              xspin=0.3333*xs12pn+0.6667*xs32pn                         
            else if(imixt.eq.2) then                                    
              xspin=0.3333*xs32pn+0.6667*xs12pn                         
            else                                                        
              xspin=xs32pn                                              
            end if                                                      
            rlimit=min(cvalu1,xspin*(4.0/s**2)**da(7)/sig)              
          end if                                                        
          sig=max(cvalu0,sigto*(1.-rlimit))                             
        else                                                            
          sigto=sig                                                     
          iswelt=1                                                      
          go to 80                                                      
        end if                                                          
      end if                                                            
      return                                                            
   86  continue                                                         
       if(itp2.eq.13.or.itp2.eq.15) then                                
         if(npd1.lt.0) go to 110                                        
       else  if(itp1.eq.13.or.itp1.eq.15) then                          
         if(npd2.lt.0) go to 110                                        
       else                                                             
         go to 92                                                       
       end if                                                           
   92 continue                                                          
      ichd=-99                                                          
      ichn=-99                                                          
      if(itp1.le.6.or.itp1.eq.10.or.itp1.eq.11.or.                      
     a   itp1.eq.13.or.itp1.eq.15.or.                                   
     b   (itp1.ge.98.and.itp1.le.99.and.                                
     c    kspin(min(max(iabs(npd1),1),71)).ge.2)) then                  
        call ndstar(s,pr,em2,em1,icltyp,itp2,npd2,itp1,npd1,            
     a              icollc,mstr,ichd,ichn,ireco,nscod,strrat,vfspin)    
      else                                                              
        call ndstar(s,pr,em1,em2,icltyp,itp1,npd1,itp2,npd2,            
     a              icollc,mstr,ichd,ichn,ireco,nscod,strrat,vfspin)    
      end if                                                            
      qlimit=min(cvalu1,(4.0/s**2)**da(7))                              
      if(mstr.eq.0) then                                                
        sig=sigasy(iswelt,1,0,0,0)                                      
      else                                                              
        sig=sigasy(iswelt,1,1,0,1)                                      
      end if                                                            
      sig=(1.-qlimit)*sig                                               
      if(iswelt.eq.1) then                                              
        if(icltyp.eq.67.or.icltyp.eq.68) then                           
          sig=sig+10.0*(1.5/s)**2                                       
        else if(icltyp.eq.76.or.icltyp.eq.77) then                      
          sig=sig+10.0*(1.5/s)**2                                       
        else if(icltyp.eq.104) then                                     
          if((itp1.eq.14.and.npd1.gt.0).or.                             
     a              (itp2.eq.14.and.npd2.gt.0))  then                   
            sig=sig+10.0*(1.5/s)**2                                     
          end if                                                        
        end if                                                          
      end if                                                            
      if(iswold.eq.1) then                                              
        if(s.gt.1.3) then                                               
          xsann=xsesti(1,s,pr,strrat,ireco,mstr,icollc)                 
          sig=sig+xsann                                                 
        end if                                                          
      end if                                                            
      return                                                            
  400 continue                                                          
      tblsft=emkofs                                                     
      ppr=sqrt(((s*s-emnuc*emnuc-emkap*emkap)/                          
     a                                 (2.*emnuc))**2-emkap*emkap)      
      if(ppr.gt.3.0) then                                               
        ippr=1                                                          
      else                                                              
        ippr=0                                                          
      end if                                                            
      jann=0                                                            
      if(((itp1.eq.1).and.(npd2.eq.-2))                                 
     &        .or.((itp2.eq.1).and.(npd1.eq.-2))                        
     &        .or.((itp1.eq.2).and.(npd2.eq.-1))                        
     &        .or.((itp2.eq.2).and.(npd1.eq.-1))) then                  
               jann=1                                                   
               if(ippr.eq.1) then                                       
                 if(iswelt.eq.1) then                                   
                   sig=tblfit(ppr,25.2,0.,0.,0.38,-2.9)                 
                 else                                                   
                   sig=tblfit(ppr,7.24,46.0,-4.71,0.279,-2.35)          
                 end if                                                 
               else                                                     
                 if(iswelt.eq.1) then                                   
                   ioftab=(kaofs+6)*itblsz                              
                 else                                                   
                   ioftab=(kaofs+7)*itblsz                              
                 end if                                                 
               end if                                                   
      else if(((itp1.eq.2).and.(npd2.eq.-2))                            
     &        .or.((itp2.eq.2).and.(npd1.eq.-2))                        
     &        .or.((itp1.eq.1).and.(npd2.eq.-1))                        
     &        .or.((itp2.eq.1).and.(npd1.eq.-1))) then                  
               jann=2                                                   
               if(ippr.eq.1) then                                       
                 if(iswelt.eq.1) then                                   
                   if(ppr.gt.310.0) ppr=310.0                           
                   sig=tblfit(ppr,-21.1,56.2,-0.27,-0.155,6.24)         
                 else                                                   
                   sig=tblfit(ppr,7.24,46.0,-4.71,0.279,-2.35)          
                 end if                                                 
               else                                                     
                 if(iswelt.eq.1) then                                   
                   ioftab=(kaofs+8)*itblsz                              
                 else                                                   
                   ioftab=(kaofs+9)*itblsz                              
                 end if                                                 
               end if                                                   
               if(itp1.eq.14.and.npd1.eq.-1.or.                         
     b                    itp2.eq.14.and.npd2.eq.-1) tblsft=tblsft+.004 
      else if(((itp1.eq.2).and.(npd2.eq.2))                             
     &       .or.((itp2.eq.2).and.(npd1.eq.2))                          
     &       .or.((itp1.eq.1).and.(npd2.eq.1))                          
     &       .or.((itp2.eq.1).and.(npd1.eq.1))) then                    
               if(ippr.eq.1) then                                       
                 if(iswelt.eq.1) then                                   
                   sig=tblfit(ppr,17.1,5.54,-2.67,0.139,-0.270)         
                 else                                                   
                   sig=tblfit(ppr,5.84,17.2,-3.06,0.206,-1.71)          
                 end if                                                 
               else                                                     
                 if(iswelt.eq.1) then                                   
                  ioftab=(kaofs+10)*itblsz                              
                 else                                                   
                  ioftab=(kaofs+11)*itblsz                              
                 end if                                                 
               end if                                                   
      else if(((itp1.eq.1).and.(npd2.eq.2))                             
     &        .or.((itp2.eq.1).and.(npd1.eq.2))                         
     &        .or.((itp1.eq.2).and.(npd2.eq.1))                         
     &        .or.((itp2.eq.2).and.(npd1.eq.1))) then                   
               if(ippr.eq.1) then                                       
                 if(iswelt.eq.1) then                                   
                   sig=tblfit(ppr,18.4,175.,-7.85, 0.198,-0.753)        
                 else                                                   
                   sig=tblfit(ppr,5.84,17.2,-3.06,0.206,-1.71)          
                 end if                                                 
               else                                                     
                 if(iswelt.eq.1) then                                   
                  ioftab=(kaofs+12)*itblsz                              
                 else                                                   
                  ioftab=(kaofs+13)*itblsz                              
                 end if                                                 
               end if                                                   
      else                                                              
         write(8,*) itp1,npd1,itp2,npd2                                 
         call errex('undefined particle pair in cross1,400')            
      end if                                                            
      if(ippr.eq.0) then                                                
        if(s.ge.tblhig-tblsft) then                                     
          call errex('cross1: kaon nucleon cm energy out of range')     
        end if                                                          
        assign 106 to label                                             
        goto 1000                                                       
  106   continue                                                        
        sig=sigin                                                       
      end if                                                            
      if(iswold.eq.0.and.jann.eq.2) then                                
          inum=4                                                        
          itot=0                                                        
          call retcgk(itp1,npd1,i1,m1,-1,cgk212)                        
          call retcgk(itp2,npd2,i2,m2,-1,cgk212)                        
          if(i1.eq.1) then                                              
            if(i2.ne.1) call errex('cross1:i2.ne.1')                    
            call iswap(i1,i2)                                           
            call iswap(m1,m2)                                           
          end if                                                        
          sigch=piynkb(i1,m1,i2,m2,itot,inum,s,pr)                      
          sig=max(cvalu0,sig-sigch)                                     
      else if(iswold.eq.0.and.jann.eq.1) then                           
        if(s.le.2.0) then                                               
          sig=0.                                                        
        else                                                            
          sig=sig*(1.-(2./s)**5)                                        
        end if                                                          
      else if(iswold.eq.3.and.jann.gt.0) then                           
        qlimit=min(cvalu1,(4.0/s**2)**da(7))                            
        signon=sig*(1.-qlimit)                                          
        if(ippr.eq.1) then                                              
          sigin=tblfit(ppr,7.24,46.0,-4.71,0.279,-2.35)                 
        else                                                            
          ioftab=(ioftab/itblsz+1)*itblsz                               
          assign 107 to label                                           
          goto 1000                                                     
  107     continue                                                      
        end if                                                          
        sig=sigin*qlimit+signon                                         
        if(jann.eq.2) then                                              
          inum=4                                                        
          itot=0                                                        
          call retcgk(itp1,npd1,i1,m1,-1,cgk212)                        
          call retcgk(itp2,npd2,i2,m2,-1,cgk212)                        
          if(i1.eq.1) then                                              
            if(i2.ne.1) call errex('cross1:i2.ne.1')                    
            call iswap(i1,i2)                                           
            call iswap(m1,m2)                                           
          end if                                                        
          sigch=piynkb(i1,m1,i2,m2,itot,inum,s,pr)                      
        else if(jann.eq.1) then                                         
          sigch=sigin*min(cvalu1,(2./s)**5)                             
        end if                                                          
        sig=max(cvalu0,sig-sigch)                                       
      end if                                                            
      return                                                            
  110 continue                                                          
       call transl(-1,itp1,npd1,ltyp1)                                  
       call transl(-1,itp2,npd2,ltyp2)                                  
       call contnt(ltyp1,ltyp2,istra1,istra2,                           
     a                            imeso1,imeso2,ibry1,ibry2,iann)       
        if(ibry1*ibry2.ge.2) then                                       
          if(istra1+istra2.eq.1) then                                   
            snew=s                                                      
            ppr=sqrt(((snew*snew-emlamb*emlamb-emnuc*emnuc)/            
     a                                 (2.*emnuc))**2-emlamb*emlamb)    
            if(iswelt.eq.1) then                                        
              if(ppr.lt.3.0) then                                       
                ioftab=(kaofs+19)*itblsz                                
                sold=s                                                  
                s=snew                                                  
                assign 102 to label                                     
                go to 1000                                              
  102           continue                                                
                sig=sigin                                               
              else                                                      
                pprmax=21.                                              
                pprmin=3.0                                              
                ppr=max(pprmin,min(pprmax,ppr))                         
                sig=tblfit(ppr,30.4,0.0,1.0,0.0,1.60)                   
              end if                                                    
            else                                                        
              if(ppr.lt.3.) then                                        
                ioftab=(kaofs+20)*itblsz                                
                sold=s                                                  
                s=snew                                                  
                assign 103 to label                                     
                go to 1000                                              
  103           continue                                                
                sig=sigin                                               
              else                                                      
                pprmax=24.                                              
                pprmin=3.                                               
                ppr=max(pprmin,min(pprmax,ppr))                         
                sig=tblfit(ppr,12.3,0.0,1.0,0.0,-2.4)                   
              end if                                                    
            end if                                                      
          else                                                          
            sig=sigasy(iswelt,0,istra1,0,istra2)                        
          end if                                                        
        else if(ibry1*ibry2.lt.0) then                                  
          dsann=0.0                                                     
          if(ida(19).eq.0) then                                         
            snew=s                                                      
            if(iswelt.eq.1) then                                        
              sann=sppban(snew)                                         
              dsann=0.0                                                 
            end if                                                      
          else                                                          
            snew=2.*sqrt(emnuc**2+pr**2)                                
            if(iswelt.eq.1) then                                        
              sann=sppban(snew)                                         
              if(ibry1*ibry2.eq.-1)  dsann=sann-sppban(s)               
            end if                                                      
          end if                                                        
          if(ibry1*ibry2.eq.-1) then                                    
            ppr=sqrt((0.5*s*s/emnuc-emnuc)**2-emnuc*emnuc)              
            if(iswelt.eq.1) then                                        
              if(ppr.gt.5.) then                                        
                sig=tblfit(ppr,41.1,77.2,-0.68,0.293,-1.82)             
              else                                                      
                sold=s                                                  
                s=min(snew,tblhig-ekinmi)                               
                ioftab=18*itblsz                                        
                assign 111 to label                                     
                goto 1000                                               
  111           continue                                                
                sig=sigin                                               
              end if                                                    
              sig=sig+dsann                                             
            else                                                        
              if(ppr.gt.2.) then                                        
                sig=tblfit(ppr,10.6,53.1,-1.19,0.136,-1.41)             
              else                                                      
                sold=s                                                  
                s=min(snew,tblhig-ekinmi)                               
                ioftab=19*itblsz                                        
                assign 112 to label                                     
                goto 1000                                               
  112           continue                                                
                sig=sigin                                               
              end if                                                    
            end if                                                      
          else                                                          
            sig=sigasy(iswelt,0,istra1,0,istra2)                        
            if(iswelt.eq.1) then                                        
              sig=sig+sann                                              
            end if                                                      
          end if                                                        
        else if(ibry1.lt.0.or.ibry2.lt.0) then                          
          call qinvrt(itp1,npd1,em1)                                    
          call qinvrt(itp2,npd2,em2)                                    
          icltyp=ictype(itp1,itp2)                                      
          if(icltyp.gt.maxtyp) icltyp=maxtyp+1                          
          go to 11                                                      
        else if(ibry1.eq.0.and.ibry2.eq.0) then                         
          sig=sigasy(iswelt,1,istra1,1,istra2)                          
          qlimit=min(cvalu1,1.6/s)                                      
          if(iswelt.eq.0) then                                          
            sig=(1.-qlimit)*sig                                         
          else if(iswelt.eq.1) then                                     
            siglow=0.0                                                  
            if(s.lt.1.8.or.pr.lt.0.9) then                              
              if(icltyp.le.45.and.npd1.eq.0.and.                        
     a                                     npd2.eq.0) then              
                if(icltyp.eq.35.or.icltyp.eq.44) then                   
                  siglow=120.0                                          
                else if(icltyp.eq.43) then                              
                  siglow=150.0                                          
                else                                                    
                  siglow=40.0                                           
                end if                                                  
              else if(icltyp.eq.105.and.npd1*npd2.lt.0) then            
                if(s.lt.1.2) then                                       
                  siglow=6./pr**2                                       
                else                                                    
                  siglow=40.0                                           
                end if                                                  
              else if(icltyp.ge.98.and.icltyp.le.100) then              
                if(s.gt.0.822.and.s.lt.0.962)  then                     
                  siglow=130.                                           
                else                                                    
                  siglow=40.0                                           
                end if                                                  
              else                                                      
                siglow=25.0/pr                                          
              end if                                                    
            else if(s.lt.2.5) then                                      
              if(icltyp.eq.105.and.npd1*npd2.lt.0) then                 
                siglow=10.0                                             
              else if(icltyp.le.45.and.                                 
     a                (npd1.ne.0.or.npd2.ne.0)) then                    
                siglow=10.0                                             
              end if                                                    
            end if                                                      
            sig=sig+siglow                                              
          end if                                                        
        else if((ibry1.ne.0.and.ibry1*ibry2.eq.0).or.                   
     a           (ibry2.ne.0.and.ibry1*ibry2.eq.0)) then                
          if(iann.eq.4) then                                            
            if(s.gt.2.0) then                                           
              qlimit=(4.0/s**2)**da(7)                                  
              sig=sigasy(iswelt,imeso1,istra1,imeso2,istra2)            
              sigbas=max(cvalu0,sig*(1.-qlimit))                        
              sig=sigbas                                                
            else                                                        
              sig=0.0                                                   
            end if                                                      
          else if(iann.eq.3) then                                       
            if(s.gt.2.0) then                                           
              qlimit=(4.0/s**2)**da(7)                                  
              sig=sigasy(iswelt,imeso1,istra1,imeso2,istra2)            
              sigbas=max(cvalu0,sig*(1.-qlimit))                        
              sig=sigbas                                                
            else                                                        
              sig=0.0                                                   
            end if                                                      
            if(iswold.eq.1) then                                        
              xsann=xsesti(5,s,pr,strdum,iredu,mdum,icodum)             
              sig=sig+xsann                                             
            end if                                                      
          else if(iann.eq.2) then                                       
              if(s.le.1.50) then                                        
                sig=0.0                                                 
                return                                                  
              else                                                      
                if(s.gt.2.0) then                                       
                  qlimit=(4.0/s**2)**da(7)                              
                  sig=sigasy(iswelt,imeso1,istra1,imeso2,istra2)        
                  sigbas=max(cvalu0,sig*(1.-qlimit))                    
                  sig=sigbas                                            
                else                                                    
                  sig=0.0                                               
                end if                                                  
                if(iswold.eq.1) then                                    
                  xsann=xsesti(4,s,pr,strdum,iredu,mdum,icodum)         
                  sig=sig+xsann                                         
                end if                                                  
              end if                                                    
          else                                                          
            sig=sigasy(iswelt,imeso1,istra1,imeso2,istra2)              
          end if                                                        
        end if                                                          
      return                                                            
 1000 xpt=log(s+tblsft)                                                 
      tlow=log(tbllow)                                                  
       if(xpt.lt.tlow) then                                             
         xpt=tlow                                                       
       end if                                                           
       index=int((xpt-tlow)/tblstp)+1                                   
       if(index.ge.itblsz) then                                         
         write(8,*)'warning:index.ge.itblsz in cross1 index=',index     
         write(8,*)'icltyp,s',icltyp,s                                  
         index=itblsz-1                                                 
       end if                                                           
       indin=ioftab+index                                               
       x1=(index-1)*tblstp+tlow                                         
       y1in=sigcom(indin)                                               
       y2in=sigcom(indin+1)                                             
       slin=(y2in-y1in)/tblstp                                          
       sigin=slin*(xpt-x1)+y1in                                         
      goto label                                                        
      end                                                               
      subroutine contnt(kl1,kl2,istra1,istra2,                          
     a                            imeso1,imeso2,ibry1,ibry2,iann)       
                    call luiflv(kl1,ifla1,iflb1,iflc1,ksp)              
                    istra1=0                                            
                    imeso1=0                                            
                    istra2=0                                            
                    imeso2=0                                            
                    ibry1=0                                             
                    ibry2=0                                             
                    if(iabs(ifla1).eq.3) istra1=1                       
                    if(iabs(iflb1).eq.3) istra1=istra1+1                
                    if(iabs(iflc1).eq.3) istra1=istra1+1                
                    if(iflc1.eq.0) then                                 
                       imeso1=1                                         
                    else if(iflc1.lt.0) then                            
                       if((-kl1.ge.41.and.-kl1.le.42).or.               
     a                    (-kl1.ge.61.and.-kl1.le.64)) then             
                         ibry1=-1                                       
                       else                                             
                         ibry1=-2                                       
                       end if                                           
                    else if(iflc1.gt.0) then                            
                       if((kl1.ge.41.and.kl1.le.42).or.                 
     a                    (kl1.ge.61.and.kl1.le.64)) then               
                         ibry1=1                                        
                       else                                             
                         ibry1=2                                        
                       end if                                           
                    end if                                              
                    call luiflv(kl2,ifla2,iflb2,iflc2,ksp)              
                    if(iabs(ifla2).eq.3) istra2=1                       
                    if(iabs(iflb2).eq.3) istra2=istra2+1                
                    if(iabs(iflc2).eq.3) istra2=istra2+1                
                    if(iflc2.eq.0) then                                 
                       imeso2=1                                         
                    else if(iflc2.lt.0) then                            
                       if((-kl2.ge.41.and.-kl2.le.42).or.               
     a                    (-kl2.ge.61.and.-kl2.le.64)) then             
                         ibry2=-1                                       
                       else                                             
                         ibry2=-2                                       
                       end if                                           
                    else if(iflc2.gt.0) then                            
                       if((kl2.ge.41.and.kl2.le.42).or.                 
     a                    (kl2.ge.61.and.kl2.le.64)) then               
                         ibry2=1                                        
                       else                                             
                         ibry2=2                                        
                       end if                                           
                    end if                                              
        iann=0                                                          
        if((ibry1.ne.0.or.ibry2.ne.0).and.ibry1*ibry2.eq.0) then        
          if(iflc1.ne.0) then                                           
            iann=ianout(kl1,istra1,ifla1,iflb1,iflc1,                   
     a                              kl2,istra2,ifla2,iflb2)             
          else if(iflc2.ne.0) then                                      
            iann=ianout(kl2,istra2,ifla2,iflb2,iflc2,                   
     a                              kl1,istra1,ifla1,iflb1)             
          end if                                                        
        end if                                                          
       return                                                           
       end                                                              
       subroutine qinvrt(itp1,npd1,em1)                                 
         include 'scatter7'                                             
         ires=0                                                         
         if((itp1.ge.10.and.itp1.le.11).or.                             
     a    (itp1.ge.3.and.itp1.le.6.and.npd1.ne.0.and.npd1.ne.1)) then   
           call subst2(1,itp1,npd1)                                     
           if(itp1.le.11) then                                          
             ires=1                                                     
           else                                                         
             ires=-5                                                    
           end if                                                       
         else if((itp1.eq.98).and.                                      
     a          ( (-npd1.ge.41.and.-npd1.le.42).or.                     
     b            (-npd1.ge.61.and.-npd1.le.64) )) then                 
               ires=-1                                                  
         else if(itp1.ge.7.and.itp1.le.9)then                           
                ires=-2                                                 
         else if(itp1.eq.12) then                                       
                ires=-2                                                 
         else if((itp1.eq.98).and.                                      
     a          ((-npd1.ge.43.and.-npd1.le.45).or.                      
     b                                    (npd1.eq.-57))) then          
         else if(itp1.eq.13.or.itp1.eq.15) then                         
                if(em1.ge.emlamb+empion) ires=-4                        
         else if(itp1.ge.85.and.itp1.le.97)then                         
           ires=-5                                                      
         else if(itp1.eq.98)then                                        
           ires=-3                                                      
         end if                                                         
         itpold=itp1                                                    
         npdold=npd1                                                    
         call transl(-1,itp1,npd1,kl2)                                  
         if((kl2.eq.23).or.(kl2.eq.24).or.(kl2.eq.25).or.(kl2.eq.33)    
     a         .or.(kl2.eq.34).or.(kl2.eq.35).or.(kl2.eq.36) ) then     
           kl2=-kl2                                                     
         end if                                                         
         call transl(1,itp1,npd1,-kl2)                                  
         if(ires.eq.1) then                                             
             itp1=98                                                    
         else if(ires.eq.-1) then                                       
             if(itp1.ge.3.and.itp1.le.6) then                           
             else if(itp1.ge.1.and.itp1.le.2) then                      
                itp1=9+itp1                                             
             else                                                       
                write(8,*) 'ires,itp1,npd1,em1'                         
                write(8,*)  ires,itp1,npd1,em1                          
                call errex('what is this particle in qinvrt?????')      
             end if                                                     
             npd1=3                                                     
         else if(ires.eq.-2) then                                       
             npd1=npdold                                                
         else if(ires.eq.-3.or.ires.eq.-5) then                         
             itp1=itpold                                                
             if(ires.eq.-5) then                                        
               npd1=-kl2                                                
               if(npd1.gt.40) call subst2(2,itp1,npd1)                  
             end if                                                     
         else if(ires.eq.-4) then                                       
             itp1=98                                                    
         end if                                                         
       return                                                           
       end                                                              
       function kfcrev(kl2)                                             
         if((kl2.eq.23).or.(kl2.eq.24).or.(kl2.eq.25).or.(kl2.eq.33)    
     a         .or.(kl2.eq.34).or.(kl2.eq.35).or.(kl2.eq.36) ) then     
           kfcrev=kl2                                                   
         else                                                           
           kfcrev=-kl2                                                  
         end if                                                         
       return                                                           
       end                                                              
      subroutine detbal(s,pr,icltyp,ityp1,npid1,ityp2,npid2,            
     a                                                em1,em2,sigdb)    
        include 'scatter7'                                              
        include 'sigmasx'                                               
      dimension beta(100)                                               
      data beta/                                                        
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.03,  0.00,  0.02,  0.00,  0.02,          
     @  0.00,  0.10,  0.28,  0.20,  0.40,  0.68,  0.70,  0.78,          
     @  0.48,  0.48,  0.38,  0.08,  0.01,  0.04,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,  0.00,          
     @  1.67,  1.89,  1.93,  1.77,  1.64,  1.48,  1.32,  1.12,          
     @  1.10,  1.01,  1.02,  1.00,  1.20,  1.22,  1.26,  1.31,          
     @  1.31,  1.36,  1.34,  1.32,  1.31,  1.28,  1.27,  1.23,          
     @  1.21,  1.20,  1.19,  1.15/                                      
      data emdsmn/ 1.708/                                               
      sigdb=0.                                                          
      if(s.ge.tblhig) return                                            
      if(pr.lt.0) call errex(' detbal, pr invalid')                     
      sigin0=0.                                                         
      pr2new=.25*s*s-emnuc**2                                           
      if(icltyp.eq.4.or.icltyp.eq.5.or.                                 
     a              icltyp.eq.7.or.icltyp.eq.8.or.                      
     b                    icltyp.eq.11.or.icltyp.eq.12.or.              
     c                         icltyp.eq.16.or.icltyp.eq.17) then       
           emd=max(em1,em2)                                             
             if(icltyp.eq.4.or.icltyp.eq.17) then                       
               return                                                   
             else if(icltyp.eq.5) then                                  
               weight=0.25*.75                                          
             else if(icltyp.eq.7) then                                  
               weight=0.25*.25                                          
             else if((icltyp.eq.11).or.(icltyp.eq.8)) then              
               weight=0.25*.5                                           
             else if(icltyp.eq.16) then                                 
               weight=0.25*.75                                          
             else if(icltyp.eq.12) then                                 
               weight=0.25*.25                                          
             end if                                                     
             if(ida(4).gt.0.and.npid1.le.1.and.npid2.le.1) then         
               call wqdnnn(s,pr,emd,wqout)                              
               sigdb=weight*wqout                                       
               return                                                   
             else                                                       
               iofs=6                                                   
               assign 500 to label                                      
               go to 1000                                               
             end if                                                     
      end if                                                            
      if(icltyp.eq.10) then                                             
             weight=.1                                                  
             iofs=10                                                    
      else if(icltyp.eq.15) then                                        
             weight=.1                                                  
             iofs=10                                                    
      else if((icltyp.eq.46).or.(icltyp.eq.57)) then                    
             weight=.5                                                  
             iofs=8                                                     
      else if((icltyp.eq.47).or.(icltyp.eq.56)) then                    
             weight=.5                                                  
             iofs=9                                                     
      else if((icltyp.eq.49).or.(icltyp.eq.60)) then                    
            weight=1./ 16.                                              
            iofs=12                                                     
      else if((icltyp.eq.50).or.(icltyp.eq.59)) then                    
            weight=0.125                                                
            iofs=12                                                     
      else if((icltyp.eq.51).or.(icltyp.eq.58)) then                    
            weight=3./ 16.                                              
            iofs=12                                                     
      else if((icltyp.eq.55).or.(icltyp.eq.66)) then                    
            weight=1.                                                   
            iofs=13                                                     
      else if(icltyp.eq.65) then                                        
            weight=1.                                                   
            iofs=14                                                     
      else                                                              
         weight=0.                                                      
         if(icltyp.eq.13) then                                          
            weight=3./40.                                               
         else if(icltyp.eq.14) then                                     
            weigh0=1./16.                                               
            weigh1=0.0125                                               
         else if(icltyp.eq.18) then                                     
            weigh0=1./16.                                               
            weigh1=9./80.                                               
         else if(icltyp.eq.19) then                                     
            weight=3./40.                                               
         else                                                           
            sigdb=0.                                                    
            return                                                      
         end if                                                         
         if(weight.eq.0.) then                                          
           iofs=11                                                      
           assign 970 to label                                          
           goto 1000                                                    
  970      sigin0=weigh0*sintot                                         
           weight=weigh1                                                
         end if                                                         
         iofs=10                                                        
      end if                                                            
      assign 500 to label                                               
      go to 1000                                                        
  500 sintot=weight*sintot                                              
      emmax1=s-em2                                                      
      emmax2=s-em1                                                      
      index=index-iofst                                                 
      if(index.le.0.or.index.gt.itblsz) then                            
        write(8,*) 'detbal: s,pr=',s,pr,'icltyp=',icltyp                
        write(8,*) 'ityp1,npid1,ityp2,npid2,em1,em2'                    
        write(8,*)  ityp1,npid1,ityp2,npid2,em1,em2                     
        write(8,*)  'index=',index                                      
        write(8,*) 'index out of range in detbal'                       
        sigdb=0.0                                                       
        return                                                          
      end if                                                            
      if(iofs.eq.6) then                                                
        bet=beta(index)                                                 
        pdn=min(cvalu1,exp(-bet*(s- emdsmn-emnuc)))                     
        if(ityp1.le.2) then                                             
               if(npid2.eq.0.or.npid2.eq.1) then                        
               else                                                     
                  pdn=(1.-pdn)*probdl(emmax2,em2,npid2)                 
               end if                                                   
        else if(ityp2.le.2) then                                        
               if(npid1.eq.0.or.npid1.eq.1) then                        
               else                                                     
                  pdn=(1.-pdn)*probdl(emmax1,em1,npid1)                 
               end if                                                   
        else                                                            
              call errex('missing nucleon in ndelta channel,detbal')    
        end if                                                          
        sintot=pdn*sintot                                               
      else if(iofs.eq.10.or.iofs.eq.11) then                            
           bet=beta(index)                                              
           pdd=min(cvalu1,exp(-bet*(s- 2.*emdsmn)))                     
           if((npid1.eq.0.or.npid1.eq.1).and.                           
     a              (npid2.eq.0.or.npid2.eq.1)) then                    
           else if(npid1.ne.0.and.npid1.ne.1.and.                       
     a               npid2.ne.0.and.npid2.ne.1) then                    
               pdd=(1.-pdd)*probdl(emmax2,em2,npid2)*                   
     a                                        probdl(emmax1,em1,npid1)  
           else                                                         
               pdd=0.                                                   
           end if                                                       
           sintot=pdd*sintot                                            
           sigin0=pdd*sigin0                                            
      else if(iofs.eq.8.or.iofs.eq.9) then                              
           if(ityp1.le.2) then                                          
                  pnn=probn2(emmax2,em2,npid2)                          
           else if(ityp2.le.2) then                                     
                  pnn=probn2(emmax1,em1,npid1)                          
           else                                                         
              call errex('missing nucleon in nn* channel,detbal')       
           end if                                                       
           sintot=pnn*sintot                                            
      else if(iofs.eq.13.or.iofs.eq.14) then                            
            pnn=probn2(emmax1,em1,npid1)*probn2(emmax2,em2,npid2)       
            sintot=pnn*sintot                                           
      else if(iofs.eq.12) then                                          
        bet=beta(index)                                                 
        pdnp=min(cvalu1,exp(-bet*(s- emdsmn-emmnn2)))                   
           if(ityp1.ge.3.and.ityp1.le.6) then                           
                  if(npid1.eq.0.or.npid1.eq.1) then                     
                  else                                                  
                     pdnp=probdl(emmax1,em1,npid1)*(1.-pdnp)            
                  end if                                                
                  pdnp=probn2(emmax2,em2,npid2) *pdnp                   
           else if(ityp2.ge.3.and.ityp2.le.6) then                      
                  if(npid2.eq.0.or.npid2.eq.1) then                     
                  else                                                  
                     pdnp=probdl(emmax2,em2,npid2)*(1.-pdnp)            
                  end if                                                
                  pdnp=probn2(emmax1,em1,npid1) *pdnp                   
           else                                                         
              call errex('missing delta in dn* channel,detbal')         
           end if                                                       
           sintot=pdnp*sintot                                           
      end if                                                            
      sigdb=pr2new*(sigin0+sintot)/ (pr*pr)                             
      return                                                            
 1000 xpt=log(s)                                                        
      iofst=itblsz*iofs                                                 
      tlow=log(tbllow)                                                  
      index=int((xpt-tlow)/tblstp)+1                                    
      if(index.le.0)then                                                
        sintot=0.                                                       
        goto label                                                      
      else if(index.ge.itblsz) then                                     
        sintot=sigcom(itblsz+iofst)                                     
        goto label                                                      
      end if                                                            
      x1=(index-1)*tblstp + tlow                                        
      index=index+iofst                                                 
      if(index.gt.indmax)                                               
     a call errex('wrong index  in detbal' )                            
      y1in=sigcom(index)                                                
      y2in=sigcom(index+1)                                              
      slin=(y2in-y1in)/tblstp                                           
      sintot=slin*(xpt-x1) + y1in                                       
      goto label                                                        
      end                                                               
        subroutine wqdnch(icltyp,itp1,itp2,npd1,npd2,srt,pr,em1,em2,    
     a                       sigeno,sigel1,sigel2,sigdd1,sigdd2,sigdb)  
        include 'scatter7'                                              
        include 'sigmasx'                                               
        real*4 pjin                                                     
        dimension pjin(4)                                               
        common /pjbk/ pjin                                              
        data sold / -1./                                                
        ioften=itblsz*3                                                 
        iofte1=itblsz*20                                                
        iofte2=itblsz*21                                                
        ioftdd=itblsz*23                                                
        j1d=3                                                           
        j1dnew=3                                                        
        j2n=1                                                           
        j2dnew=3                                                        
        ibasd=9                                                         
        ibasn=3                                                         
        srtact=2.*sqrt(emnuc*emnuc+pr*pr)                               
        s=srtact                                                        
        sold=-1.0                                                       
        assign  10 to label                                             
        ioftab=ioften                                                   
        go to 1000                                                      
   10   continue                                                        
        sigeno=sigin                                                    
        s=srt                                                           
        if(itp1.le.2) then                                              
          m1=2*itp2-ibasd                                               
          m2=2*itp1-ibasn                                               
        else                                                            
          m1=2*itp1-ibasd                                               
          m2=2*itp2-ibasn                                               
        end if                                                          
        itag=-100                                                       
        call isocgk(m1,j1d,m2,j2n,j1dnew,j2dnew,m1new,m2new,itag)       
        m=m1+m2                                                         
        if(s.le.emdelt+emnuc) then                                      
          s=sqrt(emnuc**2+pr**2)+sqrt(emdelt**2+pr**2)                  
        end if                                                          
        sigel1=0.                                                       
        sigel2=0.                                                       
        assign  20 to label                                             
        ioftab=iofte1                                                   
        go to 1000                                                      
   20   continue                                                        
        sigel1=sigin                                                    
        assign  30 to label                                             
        ioftab=iofte2                                                   
        go to 1000                                                      
   30   continue                                                        
        sigel2=sigin                                                    
        assign  40 to label                                             
        ioftab=22*itblsz                                                
        go to 1000                                                      
   40   continue                                                        
        saver=sigin                                                     
        s=srt                                                           
        emmin=emnuc+empion                                              
        emmxd=max(s-emnuc,emmin)                                        
        vfd=1./pi*(atan(2.*(emmxd-emdelt)/widdlt)-                      
     a            atan(2.*(emmin-emdelt)/widdlt))                       
        sigeno=max(cvalu0,vfd*sigeno-vfd*saver)                         
        sigel1=max(cvalu0,sigel1-sigeno)                                
        sigel2=max(cvalu0,sigel2-sigeno)                                
        if(iabs(m).eq.4)then                                            
          sigel1=0.                                                     
          sigel2=vfd*sigel2*pjin(1)                                     
        else                                                            
          sigel1=vfd*sigel1*pjin(1)                                     
          sigel2=vfd*sigel2*pjin(2)                                     
        end if                                                          
        assign  50 to label                                             
        ioftab=ioftdd                                                   
        go to 1000                                                      
   50   continue                                                        
        sigred=sigin                                                    
        if(da(12).gt.1.) sigred=da(12)*sigred                           
        s=srt                                                           
        if(iabs(m).eq.4)then                                            
          sigdd1=0.                                                     
          sigdd2=sigred*0.8*pjin(1)                                     
        else                                                            
          sigdd1=sigred*4./9.*pjin(1)                                   
          sigdd2=sigred*0.8* pjin(2)                                    
        end if                                                          
        call detbal(s,pr,icltyp,itp1,npd1,itp2,npd2,em1,em2,sigdb)      
        return                                                          
 1000 continue                                                          
      if(s.ne.sold) then                                                
        xpt=log(s)                                                      
        tlow=log(tbllow)                                                
        if(xpt.lt.tlow) xpt=tlow                                        
        index=int((xpt-tlow)/tblstp)+1                                  
        if(index.ge.itblsz) then                                        
          write(8,*)'warning:index.ge.itblsz in wqdnch index=',index    
          write(8,*)'icltyp,srt',icltyp,s                               
          index=itblsz-1                                                
        end if                                                          
        x1=(index-1)*tblstp+tlow                                        
      end if                                                            
      sold=s                                                            
      indin=ioftab+index                                                
      if(indin.lt.0) then                                               
        write(8,*) indin,ioftab,index,s,xpt                             
        call errex('abort in wqdnch,1000')                              
      end if                                                            
      y1in=sigcom(indin)                                                
      y2in=sigcom(indin+1)                                              
      slin=(y2in-y1in)/tblstp                                           
      sigin=slin*(xpt-x1) + y1in                                        
      goto label                                                        
        end                                                             
      subroutine  wqdnnn(srt,prel,emdlt,wqout)                          
        include 'scatter7'                                              
        include 'sigmasx'                                               
      wqout=0.                                                          
      emmin=emnuc+empion                                                
      emmax=2.                                                          
      if(srt.le.emdlt+emnuc.or.srt.gt.tblhig) then                      
         return                                                         
      else                                                              
         assign 800 to label                                            
         goto 1000                                                      
      end if                                                            
  800 continue                                                          
      sigin=1./prel**2*(0.25*srt*srt-emnuc*emnuc)*sigin                 
      if(index.gt.7.and.index.le.60) then                               
        mbin=                                                           
     a   int(float(20)*(emdlt-emmin)/(min(emmax,srt-emnuc)-emmin))+1    
        mbin=min(20,max(1,mbin))                                        
        indeff=index-7                                                  
        xdown=max(cvalu0,min(cvalu1,1.-(xpt-x1)/tblstp))                
        fcact=xdown*dnnnfc(indeff)+(1-xdown)*dnnnfc(min(53,indeff+1))   
        sem=xdown*dsdm(indeff,mbin)+                                    
     a                            (1-xdown)*dsdm(min(53,indeff+1),mbin) 
        wqout=sigin*fcact*sem                                           
      else                                                              
        wqout=sigin                                                     
      endif                                                             
      return                                                            
 1000 xpt=log(srt)                                                      
      tlow=log(tbllow)                                                  
      index=int((xpt-tlow)/tblstp)+1                                    
      if(xpt.lt.tlow+7.*tblstp) then                                    
        xpt=tlow+7.*tblstp                                              
        index=8                                                         
      end if                                                            
      indin=index                                                       
      x1=(index-1)*tblstp+tlow                                          
      y1in=wqnnnd(indin)                                                
      y2in=wqnnnd(indin+1)                                              
      slin=(y2in-y1in)/tblstp                                           
      sigin=slin*(xpt-x1)+y1in                                          
      goto label                                                        
      end                                                               
      function mbafnd(ibar,em)                                          
       include 'scatter7'                                               
       common /xspibk/ xspin1,xspin3,wnorm1,wnorm3,pow12,pow32,         
     a                xsbgl1,xsbgl3,xs1218,xs32pn,xs12pn                
       real*4 gamres                                                    
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       common /gamrbk/ gamres                                           
       dimension prob(11),gamact(11)                                    
       dimension kflv(8)                                                
       data kflv / 61,41,57,43,46,65,68,70/                             
        if(ibar.le.0.or.ibar.gt.8)                                      
     a  call errex('mbafnd abort: wrong ibar input')                    
        iremx=mxbsta(ibar)                                              
        emadd=0.0                                                       
        if(ibar.eq.1) then                                              
          powr=pow32                                                    
          embas=2.0                                                     
        else if(ibar.eq.2) then                                         
          embas=1.80                                                    
          powr=pow12                                                    
          iremx=10                                                      
        else                                                            
          embas=2.00                                                    
          if(ibar.le.4) then                                            
            emadd=0.15                                                  
          else if(ibar.eq.5) then                                       
            emadd=0.3                                                   
          else                                                          
            emadd=0.15*(ibar-5)                                         
          end if                                                        
          embas=embas+emadd                                             
          powr=5.0                                                      
        end if                                                          
        if(em.le.2.8+emadd) then                                        
          zlim=(embas/em)**(powr-1.)                                    
          if(ranf(0).lt.zlim) then                                      
            probs=0.0                                                   
            do 20 i=1,iremx                                             
             prob(i)=0.0                                                
             jspb=jspbar(i,ibar)                                        
             kf=kflv(ibar)                                              
             gamb=widr(i,kf,em)                                         
             if(gamb.le.cvalu0) go to 20                                
             prob(i)=(jspb+1.)*gamb/                                    
     a          ((em-embarr(i,ibar))**2+0.25*gamb**2)                   
             probs=probs+prob(i)                                        
   20       continue                                                    
            zrand=ranf(0)                                               
            do 30 i=1,iremx                                             
             jentry=i                                                   
             prob(i)=prob(i)/probs                                      
             if(i.gt.1) prob(i)=prob(i-1)+prob(i)                       
             if(zrand.lt.prob(i)) go to 31                              
   30       continue                                                    
   31       continue                                                    
          else                                                          
            jentry=-3                                                   
          end if                                                        
        else                                                            
          jentry=-3                                                     
        end if                                                          
        mbafnd=jentry                                                   
      return                                                            
      end                                                               
      function mmefnd(imes,em)                                          
       include 'scatter7'                                               
       real*4 gamres                                                    
       real*4 embarr,gambar,embmin,emmesr,gammes,emmmin,anglmx,         
     a        qqbdif,ssbdif                                             
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       common /gamrbk/ gamres                                           
       dimension prob(11),gamact(11)                                    
       dimension kflv(8)                                                
       data kflv / 17,24,25,18,27,34,35,28/                             
        if(imes.le.0.or.imes.gt.8)                                      
     a  call errex('mmefnd abort: wrong imes input')                    
        iremx=mxmsta(imes)                                              
        dem=0.0                                                         
        if(imes.eq.4.or.imes.eq.8) then                                 
          ndem=1                                                        
        else if(imes.eq.3.or.imes.eq.7) then                            
          ndem=2                                                        
        else                                                            
          ndem=0                                                        
        end if                                                          
        dem=ndem*0.15                                                   
        if(em.le.1.6+dem) then                                          
          probs=0.0                                                     
          do 20 i=1,iremx                                               
           prob(i)=0.0                                                  
           jspm=jspmes(i,imes)                                          
           if(jspm.lt.0) go to 20                                       
           if(em.le.emmmin(imes)) go to 20                              
           kf=kflv(imes)                                                
           gamm=widr(i,kf,em)                                           
           if(gamm.le.cvalu0) go to 20                                  
           prob(i)=(jspm+1.)*gamm/                                      
     a          ((em-emmesr(i,imes))**2+0.25*gamm**2)                   
           probs=probs+prob(i)                                          
   20     continue                                                      
          if(probs.eq.cvalu0) then                                      
            mmefnd=9999                                                 
            return                                                      
          end if                                                        
          zrand=ranf(0)                                                 
          do 30 i=1,iremx                                               
           jentry=i                                                     
           prob(i)=prob(i)/probs                                        
           if(i.gt.1) prob(i)=prob(i-1)+prob(i)                         
           if(zrand.lt.prob(i)) go to 31                                
   30     continue                                                      
   31     continue                                                      
        else                                                            
          jentry=-1                                                     
        end if                                                          
        mmefnd=jentry                                                   
      return                                                            
      end                                                               
      function probdl(emmax,em1,npid1)                                  
      include 'scatter7'                                                
      dimension emdl( 9)                                                
      data emdl  /1.60, 1.62, 1.700, 1.90, 1.905, 1.910,                
     a              1.92,1.93,1.95/                                     
      if(npid1.ge.2.and.npid1.le.4) then                                
           probdl=0.                                                    
           return                                                       
      else                                                              
           npid=-npid1                                                  
           if(npid.gt.100) npid=mod(npid,100)                           
      end if                                                            
      bwsum=0.                                                          
      do 21 i=1,9                                                       
       gamr=0.1*emdl(i)                                                 
       gamsq=gamr**2                                                    
       bwterm=gamr/((em1-emdl(i))**2 + 0.25* gamsq)                     
       if(npid.eq.i) bwi=bwterm                                         
       bwsum=bwsum+bwterm                                               
   21 continue                                                          
      if(ida(4).gt.0) then                                              
        enhanc=pi/(atan(2.*(emmax-emdl(npid))/gamr)+0.5*pi)             
      else                                                              
        enhanc=1.                                                       
      end if                                                            
      probdl=bwi/bwsum*enhanc                                           
      return                                                            
      end                                                               
      function probn2(emmax,em1,npid1)                                  
      include 'scatter7'                                                
      dimension emn2(10)                                                
      data emn2  /1.44, 1.52, 1.535, 1.65, 1.675, 1.680,                
     a              1.7, 1.71, 1.72, 1.99/                              
      if(npid1.ge.0.and.npid1.le.4) then                                
         probn2=0.                                                      
         return                                                         
      else                                                              
         npid=-npid1                                                    
         if(npid.gt.100) npid=mod(npid,100)                             
      end if                                                            
      bwsum=0.                                                          
      do 26 i=1,10                                                      
       gamr=0.1*emn2(i)                                                 
       gamsq=gamr**2                                                    
       bwterm=gamr/((em1-emn2(i))**2 + 0.25* gamsq)                     
       if(npid.eq.i) bwi=bwterm                                         
       bwsum=bwsum+bwterm                                               
   26 continue                                                          
      if(ida(4).gt.0) then                                              
        enhanc=pi/(atan(2.*(emmax-emn2(npid))/gamr)+0.5*pi)             
      else                                                              
        enhanc=1.                                                       
      end if                                                            
      probn2=bwi/bwsum*enhanc                                           
      return                                                            
      end                                                               
      function sigasy(iswelt,imeso1,istra1,imeso2,istra2)               
        include 'scatter7'                                              
        imeso=imeso1+imeso2                                             
        sig=40.*0.6666**imeso*                                          
     a        (1.-0.4*istra1/(3.-imeso1))*(1.-0.4*istra2/(3.-imeso2))   
        if(iswelt.eq.0) then                                            
          sigasy=sig**1.5*(10./40.**1.5)                                
        else                                                            
          sigasy=sig                                                    
        end if                                                          
      return                                                            
      end                                                               
      function ianout(kl1,istra1,ifla1,iflb1,iflc1,                     
     a                              kl2,istra2,ifla2,iflb2)             
        ian=-1                                                          
        ivz3=isign(3,iflc1)                                             
        if(istra2.eq.2) then                                            
          if(istra1.gt.0) then                                          
            ian=istra1                                                  
          else                                                          
            if(kl2.eq.24.or.kl2.eq.25) ian=0                            
          end if                                                        
        else if(istra2.eq.1) then                                       
          if(istra1.eq.0) then                                          
            if(ifla2.eq.ivz3.or.iflb2.eq.ivz3) then                     
              isum=isign(1,ivz3)*(ifla1+iflb1+iflc1+ifla2+iflb2)        
              if(isum.ge.5.and.isum.le.7) ian=1                         
            end if                                                      
          else if(istra1.eq.1) then                                     
            if(ifla2.eq.-ivz3.or.iflb2.eq.-ivz3) then                   
              ian=0                                                     
            else                                                        
              isum=ifla1+iflb1+iflc1+ifla2+iflb2-ivz3*2                 
              if(isum.eq.isign(1,ivz3).or.                              
     a           isum.eq.isign(2,ivz3)) ian=2                           
            end if                                                      
          else if(istra1.eq.2) then                                     
            if(ifla2.eq.-ivz3.or.iflb2.eq.-ivz3) then                   
              ian=1                                                     
            else                                                        
              if(ifla1+iflb1+iflc1+ifla2+iflb2.eq.ivz3*3) ian=3         
            end if                                                      
          else if(istra1.eq.3) then                                     
            if(ifla2.eq.-ivz3.or.iflb2.eq.-ivz3) ian=2                  
          end if                                                        
        else if(istra2.eq.0) then                                       
          if(istra1.eq.0) then                                          
            isum=ifla1+iflb1+iflc1+ifla2+iflb2                          
            isum=iabs(isum)                                             
            if(isum.gt.2.and.isum.lt.7) ian=0                           
          else if(istra1.eq.1) then                                     
            isum=isign(1,ivz3)*(ifla1+iflb1+iflc1+ifla2+iflb2)          
            if(isum.ge.5.and.isum.le.7) ian=1                           
          else if(istra1.eq.2) then                                     
            isum=ifla1+iflb1+iflc1+ifla2+iflb2-ivz3*2                   
            if(isum.eq.isign(1,ivz3).or.                                
     a           isum.eq.isign(2,ivz3)) ian=2                           
          else if(istra1.eq.3) then                                     
            isum=ifla1+iflb1+iflc1+ifla2+iflb2-ivz3*3                   
            if(isum.eq.0) then                                          
              if(mod(iabs(kl2),10).ne.3) ian=3                          
            end if                                                      
          end if                                                        
        end if                                                          
       ianout=ian+1                                                     
      return                                                            
      end                                                               
