CPAT,KMASS    .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine kmass(s,pr,em1,em2,em3,em4,ityp1,ityp2,ityp3,ityp4,    
     a      ichank,npid1,npid2,npid3,npid4,ic,icltyp,nop,nscop,nonstr)  
        include 'scatter7'                                              
        include 'sigmasx'                                               
      logical jets                                                      
      dimension sigstr(mxchan)                                          
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      real*4 cgks                                                       
      data vernum/'version  rqmd 1.09 -- 25-jan-94'/                    
      data pronam/'k m a s s treats flavor changing channels '/         
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      if(s.lt. em1+em2 +ekinmi) then                                    
          ic=0                                                          
          return                                                        
      end if                                                            
      nop=2                                                             
      em1old=em1                                                        
      em2old=em2                                                        
      ityp1o=ityp1                                                      
      ityp2o=ityp2                                                      
      npid1o=npid1                                                      
      npid2o=npid2                                                      
      call  retcgk(ityp1,npid1,j1,m1,-1,cgks)                           
      call  retcgk(ityp2,npid2,j2,m2,-1,cgks)                           
      goto (                                                            
     >  20, 20, 20, 20, 20,                                             
     >  20, 20, 20, 20, 20,                                             
     >  20, 20, 20, 20, 20,                                             
     >  20, 20, 20, 20, 20,                                             
     >  20, 30, 30, 30, 30,                                             
     >  30, 30, 10, 30, 30,                                             
     >  30, 30, 30, 30, 10,                                             
     >  10, 30, 30, 30, 30,                                             
     >  30, 30, 10, 10, 10,                                             
     >  20, 20, 20, 20, 20,                                             
     >  20, 30, 30, 30, 20,                                             
     >  20, 20, 20, 20, 20, 20, 30, 30, 30, 20,                         
     >  20, 30, 30, 30, 30,30 ,30 ,10 ,10 ,10 ,                         
     >  30, 30, 10, 10, 10,10 ,10 ,10 ,10 ,30 ,                         
     >  30, 30, 10, 10, 10,10,30 ,30 ,10 ,10 ,                          
     >  10, 10, 10, 10, 10,10 ,10 ,10 ,30 ,10 ,                         
     >  10, 10, 10, 10, 10,10 ,30 ,30 ,30 ,10 ,                         
     >  10, 10, 10, 10, 10                                              
     & ) , icltyp                                                       
   10 continue                                                          
      call errex('wrong icltyp in kmass')                               
      return                                                            
   20 continue                                                          
      if(ic.le.nscop) then                                              
        icn=ic-nonstr                                                   
         ktot=0                                                         
         iflag=1                                                        
         ikinp=icn                                                      
         call getbb(s,sigstr,ktot,ityp1,ityp2,npid1,npid2,em1,em2,      
     &                 itp1,itp2,itp3,npd1,npd2,npd3,emb,emy,emk,       
     &                 iflag,ikinp,                                     
     &                 vkkb,arat)                                       
         em1=emb                                                        
         em2=emy                                                        
         em3=emk                                                        
         em4=0.                                                         
         ityp1=itp1                                                     
         ityp2=itp2                                                     
         ityp3=itp3                                                     
         ityp4=0                                                        
         npid1=npd1                                                     
         npid2=npd2                                                     
         npid3=npd3                                                     
         npid4=0                                                        
         nop=3                                                          
      else                                                              
     & if (ic.gt.nscop.and.ic.le.ichank-1) then                         
        nop=4                                                           
        ikinp=ic                                                        
        k=nscop                                                         
        iflg=1                                                          
         arat=0.0                                                       
         par=0.0                                                        
         call gpair(icltyp,ityp1,npid1,em1,ityp2,npid2,em2,             
     &               itp1,npd1,e1,                                      
     &               itp2,npd2,e2,                                      
     &               itp3,npd3,e3,                                      
     &               itp4,npd4,e4,                                      
     &               ikinp,iflg,                                        
     &               sigstr,k,nscop,s,par,arat)                         
         em1=e1                                                         
         em2=e2                                                         
         em3=e3                                                         
         em4=e4                                                         
         ityp1=itp1                                                     
         ityp2=itp2                                                     
         ityp3=itp3                                                     
         ityp4=itp4                                                     
         npid1=npd1                                                     
         npid2=npd2                                                     
         npid3=npd3                                                     
         npid4=npd4                                                     
      else                                                              
        call errex('wrong ic in kmass bb')                              
      end if                                                            
      if(s.le.em1+em2+em3+em4+ekinmi) then                              
        ic=0                                                            
        nop=2                                                           
        goto 5000                                                       
      end if                                                            
      if (ranf(0).lt.0.5) then                                          
         call swap(em1,em2)                                             
         call iswap(ityp1,ityp2)                                        
         call iswap(npid1,npid2)                                        
      end if                                                            
      if (ranf(0).lt.0.5.and.nop.eq.4) then                             
         call swap(em3,em4)                                             
         call iswap(ityp3,ityp4)                                        
         call iswap(npid3,npid4)                                        
      end if                                                            
       if (nop.eq.3) then                                               
          if (ityp1.ge.3.and.ityp1.le.6) npid1=1                        
          if (ityp2.ge.3.and.ityp2.le.6) npid2=1                        
       end if                                                           
        if(ityp1.eq.1.or.ityp1.eq.2) then                               
        else                                                            
          if((ityp1.ge.3.and.ityp1.le.6).and.em1.gt.emmnd2) then        
            npid1=-mbafnd(1,em1)                                        
          else if((ityp1.ge.10.and.ityp1.le.11).and.em1.gt.emmnn2) then 
            npid1=-mbafnd(2,em1)                                        
          end if                                                        
          if(npid1.lt.0) then                                           
            if(jets(ityp1,npid1,em1,0)) then                            
              npid1=npid1-1000                                          
            end if                                                      
          end if                                                        
        end if                                                          
        if(ityp2.eq.1.or.ityp2.eq.2) then                               
        else                                                            
          if((ityp2.ge.3.and.ityp2.le.6).and.em2.gt.emmnd2) then        
            npid2=-mbafnd(1,em2)                                        
          else if((ityp2.ge.10.and.ityp2.le.11).and.em2.gt.emmnn2) then 
            npid2=-mbafnd(2,em2)                                        
          end if                                                        
          if(npid2.lt.0) then                                           
            if(jets(ityp2,npid2,em2,0)) then                            
              npid2=npid2-1000                                          
            end if                                                      
          end if                                                        
        end if                                                          
      go to 50                                                          
   30 continue                                                          
      icn=ic-nonstr                                                     
      goto (                                                            
     >  12 , 12 , 12 , 12 , 12 ,                                        
     >  12 , 12 , 12 , 12 , 12 ,                                        
     >  12 , 12 , 12 , 12 , 12 ,                                        
     >  12 , 12 , 12 , 12 , 12 ,                                        
     >  12 , 12 , 12 ,12  , 12 ,                                        
     >   12, 12 ,12  , 12 , 12 ,                                        
     >   12, 12 , 12 , 12 ,12  ,                                        
     >  12 , 12 , 12 , 12 , 12 ,                                        
     >   12,12  ,12  ,12  ,12  ,                                        
     >  12 ,12  ,12  ,12  ,12  ,                                        
     >  12 , 12 , 12 , 12 ,12  ,                                        
     >  12 ,12  ,12  ,12  ,12  ,12 , 12, 12, 12,12 ,                    
     >  12, 311 ,311 , 12 , 12 , 12, 12,12 ,12 ,12 ,                    
     >  311,311, 12,  12  ,12  ,12 ,12 ,12 ,12 ,315,                    
     >  315,315 ,12  ,12  ,12  ,12 ,316,316,12 ,12 ,                    
     >  12 ,12  ,12  ,12  ,12  ,12 ,12 ,12 ,311,12 ,                    
     >  12 ,12  ,12  ,12  ,12  ,12 ,315,315,315,12 ,                    
     >  12 ,12  ,12  ,12  ,12                                           
     & ) , icltyp                                                       
   12 continue                                                          
      call errex('wrong icltyp in kmass')                               
      return                                                            
  311 continue                                                          
      call                                                              
     a     enlk(icltyp,ityp1,npid1,ityp2,npid2,em1,em2,s,pr,ic)         
      if(ic.eq.0) then                                                  
        goto 5000                                                       
      else                                                              
        goto 50                                                         
      end if                                                            
  315 continue                                                          
      goto 5000                                                         
  316 continue                                                          
      isw=0                                                             
      if ((ityp2.ge.1).and.(ityp2.le.2)) then                           
                    call swap(em1,em2)                                  
                    call iswap(npid1,npid2)                             
                    call iswap(ityp1,ityp2)                             
                    call iswap(j1,j2)                                   
                    call iswap(m1,m2)                                   
                    isw=1                                               
      end if                                                            
      if(((ityp1.eq.14).and.(npid1.lt.0).and.(ityp2.le.2))              
     &  .or.((ityp2.eq.14).and.(npid2.lt.0).and.(ityp1.le.2)))          
     &                                                    then          
        goto 5000                                                       
      else                                                              
     &   if (((ityp1.eq.14).and.(npid1.gt.0).and.(ityp2.le.2))          
     &    .or.((ityp2.eq.14).and.(npid2.gt.0).and.(ityp1.le.2)))        
     &                                                        then      
        if(m1+m2.eq.0) then                                             
          if(icn.eq.1) then                                             
            if (npid2.eq.1) then                                        
                               em1=emnuc                                
                               em2=emkap                                
                               ityp1=1                                  
                               ityp2=14                                 
                               npid1=0                                  
                               npid2=2                                  
            else if (npid2.eq.2) then                                   
                               em1=emnuc                                
                               em2=emka0                                
                               ityp1=2                                  
                               ityp2=14                                 
                               npid1=0                                  
                               npid2=1                                  
            else                                                        
                   call errex('wrong npid in kmass -')                  
            end if                                                      
          end if                                                        
        else                                                            
          if(icn.eq.1.and.m1+m2.ne.0) then                              
                         if (ranf(0).ge.0.5) then                       
                               em1=emdelt                               
                               ityp1=3+npid2                            
                               npid1=1                                  
                               em2=em2                                  
                               ityp2=14                                 
                               npid2=npid2                              
                         else                                           
                               ityp1=3*npid2                            
                               npid1=1                                  
                               em1=emdelt                               
                               ityp2=14                                 
                               npid2=(3-npid2)                          
                                if (npid2.eq.1) then                    
                                  em2=emka0                             
                                else                                    
                                  em2=emkap                             
                                end if                                  
                         end if                                         
          else if(icn.eq.2) then                                        
                             if (npid2.eq.1) then                       
                               ityp2=99                                 
                               npid2=29                                 
                               em2=getmas(0,npid2,1,-1.0)               
                             else                                       
                               ityp2=99                                 
                               npid2=28                                 
                               em2=getmas(0,npid2,1,-1.0)               
                             end if                                     
          else if(icn.eq.3) then                                        
                         if (ranf(0).ge.0.5) then                       
                            em1=emdelt                                  
                            ityp1=3+npid2                               
                            npid1=1                                     
                            ityp2=99                                    
                            npid2=30-npid2                              
                            em2=getmas(0,npid2,1,-1.0)                  
                         else                                           
                            ityp1=3*npid2                               
                            npid1=1                                     
                            em1=emdelt                                  
                            ityp2=99                                    
                            npid2=27+npid2                              
                            em2=getmas(0,npid2,1,-1.0)                  
                         end if                                         
                                                                        
          else                                                          
                write (8,*) 'wrong icn in kmass'                        
                ic=0                                                    
                goto 5000                                               
          end if                                                        
        end if                                                          
      end if                                                            
      if(s.lt. em1 + em2+ekinmi) then                                   
           ic=0                                                         
           goto 5000                                                    
      end if                                                            
  310 continue                                                          
      if(isw.eq.1) then                                                 
        call swap(em1,em2)                                              
        call iswap(npid1,npid2)                                         
        call iswap(ityp1,ityp2)                                         
      end if                                                            
      if(em1+em2.gt.s) call errex('wrong masses selected in kmass30')   
   50 continue                                                          
      return                                                            
 5000 continue                                                          
      ic=0                                                              
      em1=em1old                                                        
      em2=em2old                                                        
      ityp1=ityp1o                                                      
      ityp2=ityp2o                                                      
      npid1=npid1o                                                      
      npid2=npid2o                                                      
      return                                                            
      end                                                               
