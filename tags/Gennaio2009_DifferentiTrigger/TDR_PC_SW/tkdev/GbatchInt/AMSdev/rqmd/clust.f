CPAT,CLUST    .                                                         
      subroutine clustr                                                 
      include 'amsrqmd.h'
      include 'scatter7'                                                
      include 'partblk7'                                                
      include 'storbig'                                                 
      real*4 force,dcoef                                                
      common/blocal/ ngbor(ntotma),force(0:3,ntotma),dcoef(ntotma)      
      dimension att(17,ntotma)                                          
      dimension qlk(0:3),pclust(0:3)                                    
      equivalence(att(1,1),at(1,1) )                                    
      if(nclmx*nclmx.lt.17*ntotma) then                                 
         call errex('att has not enough space ,abort in clustr')        
      end if                                                            
      io=0                                                              
      do 631 iclu=1,itripl-1                                            
        io=io+iclusz(iclu)                                              
  631 continue                                                          
      inold=io                                                          
      io=io+iclusz(itripl)                                              
      nttot= nrbar(itripl)+nrmes(itripl)                                
      do 4000 l=1,nttot                                                 
        il=inold+l                                                      
        do 3990 i=0,3                                                   
         att(1+i,l)=x(i,il)                                             
         att(5+i,l)=p(i,il)                                             
 3990 continue                                                          
         att(9,l)=em(il)                                                
         att(10,l)=decay(il)                                            
         att(15,l)=tlast(il)                                            
 4000 continue                                                          
      do 4110 l=1,nttot                                                 
         il=inold+l                                                     
         att(11,l)=float(ityp(il))                                      
         att(13,l)=float(npidec(il))                                    
         att(14,l)=float(lastcl(il))                                    
         att(16,l)=float(ichbig(il))                                    
         att(12,l)=float(issn(il))                                      
 4110 continue                                                          
      if(nttot.le.1) then                                               
       do 657 l=1,nttot                                                 
        ngbrnr(l)=1                                                     
  657  continue                                                         
      end if                                                            
      do 3790 l=1,nttot                                                 
      il=inold+l                                                        
      if(ngbrnr(l).eq.1) go to 3790                                     
      mnr=1                                                             
 3791 if(ngbrnr(l).lt.mnr) go to 3790                                   
      m=nghbor(ngbsum(l)+mnr)                                           
      im=inold+m                                                        
      if((m.eq.l-1).or.(m.eq.l+1)) then                                 
          do 3799 i=0,3                                                 
          qlk(i)=  x(i,il)-x(i,im)                                      
 3799     continue                                                      
          dqlksq = qlk(0) *qlk(0)                                       
          do  3800 i=1,3                                                
          dqlksq = dqlksq - qlk(i) *qlk(i)                              
 3800     continue                                                      
          if(-dqlksq.gt.dstint) then                                    
           do 3794 lnr=mnr+1,ngbrnr(l)                                  
 3794      nghbor(ngbsum(l)+lnr-1)=nghbor(ngbsum(l)+lnr)                
           ngbrnr(l)=ngbrnr(l)-1                                        
           go to 3791                                                   
          end if                                                        
      else if(((l.eq.1).and.(m.eq.nttot)).or.                           
     a       ((m.eq.1).and.(l.eq.nttot)))then                           
        do 3803 i=0,3                                                   
        qlk(i)=  x(i,il)-x(i,im)                                        
 3803   continue                                                        
        dqlksq = qlk(0) *qlk(0)                                         
        do  3802 i=1,3                                                  
        dqlksq = dqlksq - qlk(i) *qlk(i)                                
 3802   continue                                                        
        if(-dqlksq.gt.dstint) then                                      
           do 3793 lnr=mnr+1,ngbrnr(l)                                  
 3793      nghbor(ngbsum(l)+lnr-1)=nghbor(ngbsum(l)+lnr)                
           ngbrnr(l)=ngbrnr(l)-1                                        
           go to 3791                                                   
        end if                                                          
      end if                                                            
      mnr=mnr+1                                                         
      go to 3791                                                        
 3790 continue                                                          
      lclust=itripl-1                                                   
      iloop= inold                                                      
      if(nrclst.gt.itripl) then                                         
         do 655 kcl=itripl+1,nrclst                                     
           do 654 l=1,nrbar(kcl)+nrmes(kcl)                             
             lclust=lclust+1                                            
             il=io+l                                                    
             iloop=iloop+1                                              
              do 653 i=0,3                                              
                x(i,iloop)=x(i,il)                                      
                p(i,iloop)=p(i,il)                                      
  653         continue                                                  
              em(iloop)=em(il)                                          
              ityp(iloop)=ityp(il)                                      
              decay(iloop)=decay(il)                                    
              lastcl(iloop)=lastcl(il)                                  
              issn(iloop)=issn(il)                                      
              npidec(iloop)=npidec(il)                                  
              tlast(iloop)=tlast(il)                                    
              ichbig(iloop)=0                                           
  654      continue                                                     
           nonrel(lclust)=.false.                                       
           iclusz(lclust)= 1                                            
           if(nrbar(kcl).eq.1) then                                     
              nrbar(lclust)=1                                           
              nrmes(lclust)=0                                           
           else                                                         
              nrbar(lclust)=0                                           
              nrmes(lclust)=1                                           
           endif                                                        
           io=io+iclusz(kcl)                                            
  655    continue                                                       
      end if                                                            
      lclold=lclust                                                     
      nrclst=lclold                                                     
      ngrprv=0                                                          
      ngr=0                                                             
      do 3100 l=1,nttot                                                 
      if(ngbrnr(l).eq.0) go to 3100                                     
      mnr=0                                                             
      nrclst=nrclst+1                                                   
      if(ngbrnr(l).eq.1) then                                           
          ngr=ngr+1                                                     
          ngbor(ngr)=l                                                  
          go to 3099                                                    
      else                                                              
          do 3049 knr=1,ngbrnr(l)                                       
             ngr=ngr+1                                                  
             ngbor(ngr) =nghbor(ngbsum(l)+knr)                          
 3049     continue                                                      
      end if                                                            
 3050 mnr=mnr+1                                                         
      if(ngbrnr(l).lt.mnr) go to 3099                                   
      m=ngbor(ngrprv+mnr)                                               
      if(m.eq.l) go to 3050                                             
      do 3060 knr=1,ngbrnr(m)                                           
      k=nghbor(ngbsum(m)+knr)                                           
      ic=0                                                              
      do 3055 llnr=ngrprv+1,ngrprv+ngbrnr(l)                            
        ll=ngbor(llnr)                                                  
        if(ll.eq.k) ic=1                                                
 3055 continue                                                          
      if(ic.eq.0) then                                                  
          ngbrnr(l)=ngbrnr(l)+1                                         
          ngr=ngr+1                                                     
          ngbor(ngr)=k                                                  
      end if                                                            
 3060 continue                                                          
      ngbrnr(m)=0                                                       
      go to 3050                                                        
 3099 continue                                                          
      ngrprv=ngr                                                        
 3100 continue                                                          
      if(ngr.ne.nttot) then                                             
         write(lun8,*)'ngr=',ngr,'nttot=',nttot                            
         write(lun8,*) 'clustr: m,ngbor(m),ngbrnr(m)'                      
         do 7100 m=1,nttot                                              
          ic=0                                                          
          do 7101 k=1,ngr                                               
           if(ngbor(k).eq.m) ic=1                                       
 7101     continue                                                      
          if(ic.eq.0) write(lun8,*) m,' ist faul......?????????'           
          write(lun8,*) m,ngbor(m),ngbrnr(m)                               
 7100    continue                                                       
         call errex('ngr=nttot not fulfilled in clustr')                
      end if                                                            
      ilauf=iloop                                                       
      nl1=0                                                             
      nl2=0                                                             
      do 4120 l=1,nttot                                                 
      if(ngbrnr(l).eq.0) go to 4120                                     
      lclust= lclust+1                                                  
      nrbar(lclust)=0                                                   
      nrmes(lclust)=0                                                   
      do 4118 mnr=1,ngbrnr(l)                                           
      nl1=nl1+1                                                         
      m=ngbor(nl1)                                                      
      if((nint(att(11,m)).le.6).or.                                     
     a    ( (nint(att(11,m)).ge.10).and.                                
     b               (nint(att(11,m)).le.11))                           
     c          .or.(nint(att(11,m)).eq.13)                             
     d          .or.(nint(att(11,m)).eq.15) )then                       
         ilauf=ilauf+1                                                  
         if(ilauf.gt.ntotma)                                            
     a           call errex('storage overflow in entry clustr')         
         nrbar(lclust)=nrbar(lclust)+1                                  
         do 4115 i=0,3                                                  
         x(i,ilauf)=att(1+i,m)                                          
         p(i,ilauf)=att(5+i,m)                                          
 4115    continue                                                       
         em(ilauf)=att(9,m)                                             
         decay(ilauf)=att(10,m)                                         
         ityp(ilauf)=nint(att(11,m))                                    
         npidec(ilauf)=nint(att(13,m))                                  
         lastcl(ilauf)=nint(att(14,m))                                  
         tlast(ilauf)=att(15,m)                                         
         ichbig(ilauf)=nint(att(16,m))                                  
         issn(ilauf)=nint(att(12,m))                                    
      end if                                                            
 4118 continue                                                          
        do 4119 mnr=1,ngbrnr(l)                                         
         nl2=nl2+1                                                      
         m=ngbor(nl2)                                                   
         if(((nint(att(11,m)).ge.7)                                     
     a                .and.(nint(att(11,m)).le.9)).or.                  
     b         (nint(att(11,m)).eq.12) .or.                             
     c         (nint(att(11,m)).eq.14) .or.                             
     d         (nint(att(11,m)).ge.85.and.                              
     d          nint(att(11,m)).le.98).or.                              
     e         (nint(att(11,m)).eq.99)          )then                   
           ilauf=ilauf+1                                                
           if(ilauf.gt.ntotma)                                          
     a           call errex('storage overflow in entry clustr')         
           nrmes(lclust)=nrmes(lclust)+1                                
           do 4116 i=0,3                                                
            x(i,ilauf)=att(1+i,m)                                       
            p(i,ilauf)=att(5+i,m)                                       
 4116      continue                                                     
           em(ilauf)=att(9,m)                                           
           decay(ilauf)=att(10,m)                                       
           ityp(ilauf)=nint(att(11,m))                                  
           npidec(ilauf)=nint(att(13,m))                                
           lastcl(ilauf)=nint(att(14,m))                                
           tlast(ilauf)=att(15,m)                                       
           ichbig(ilauf)=nint(att(16,m))                                
           issn(ilauf)=nint(att(12,m))                                  
         end if                                                         
 4119 continue                                                          
      iwid= nint(float(nrbar(lclust))/5.)                               
      do 4117 ilf=ilauf+1,ilauf+iwid                                    
      do 4121 i=0,3                                                     
      x(i,ilf)=0.                                                       
      p(i,ilf)=0.                                                       
 4121 continue                                                          
      em(ilf)=0.                                                        
      decay(ilf)=0.                                                     
      ityp(ilf)=0                                                       
      lastcl(ilf)=0                                                     
 4117 continue                                                          
      ilauf=ilauf+iwid                                                  
      iclusz(lclust)= nrmes(lclust)+ nrbar(lclust)  +iwid               
 4120 continue                                                          
      lmax=ilauf                                                        
      if(lmax.ge.ntotma+1-nspect)                                       
     a   call errex('too high line number in clustr')                   
      nall=nspect                                                       
      do 503 l=1,nrclst                                                 
  503 nall= nall + nrbar(l)+nrmes(l)                                    
      if(nall.ne.nt+ npion) then                                        
        call errex('particle number not constant in clustr')            
      end if                                                            
      iof=0                                                             
      do 1809 kcl=1,itripl-1                                            
       iof=iof+iclusz(kcl)                                              
 1809 continue                                                          
      do 1810 kcl=itripl,nrclst                                         
          nonrel(kcl)=.false.                                           
          ntt=nrbar(kcl)                                                
          nrpion=nrmes(kcl)                                             
          if(ntt+nrpion.gt.1) then                                      
             do 1811 i=0,3                                              
                pclust(i)=0.                                            
 1811        continue                                                   
             do 1813 l=1,ntt+nrpion                                     
               do 1812 i=0,3                                            
                  pclust(i)=pclust(i)+ p(i,iof+l)                       
 1812          continue                                                 
 1813        continue                                                   
             xmass =pclust(0)*pclust(0)                                 
             do 1888 i=1,3                                              
 1888        xmass =xmass -pclust(i)*pclust(i)                          
             if(ntt.gt.0) then                                          
                 xmass = sqrt(xmass )/ntt                               
                 if(xmass.lt. 0.970) then                               
                       nonrel(kcl)=.true.                               
                       write(lun8,*) 'nonrelativistic cluster ntt=',ntt    
                 end if                                                 
                write(lun8,*)'irun=',irun,'kcl=',kcl,'xmass=',xmass,       
     a                              'ntt=',ntt                          
             end if                                                     
          end if                                                        
          iof=iof+iclusz(kcl)                                           
 1810 continue                                                          
      return                                                            
      end                                                               
