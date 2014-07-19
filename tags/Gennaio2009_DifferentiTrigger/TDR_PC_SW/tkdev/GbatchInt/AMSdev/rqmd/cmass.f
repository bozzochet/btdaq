CPAT,CMASS    .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine cmass(srt,pr,em1,em2,                                  
     a                 ityp1,ityp2,iqcnvt,icltyp,npid1,npid2,ic)        
        include 'scatter7'                                              
        include 'sigmasx'                                               
        include 'amsrqmd.h'
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      real*4  branch(500),embr(4),zbr(4),beta(100)                      
      save embr                                                         
      data (branch(i),i=1,250)/                                         
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.800,0.200,0.000,0.800,0.800,0.800,0.200,0.000,0.800,0.800,     
     @ 0.752,0.248,0.000,0.800,0.800,0.708,0.292,0.000,0.800,0.800,     
     @ 0.670,0.330,0.000,0.800,0.800,0.624,0.376,0.000,0.800,0.780,     
     @ 0.579,0.421,0.000,0.800,0.754,0.541,0.459,0.000,0.800,0.740,     
     @ 0.499,0.372,0.000,0.800,0.727,0.462,0.393,0.000,0.800,0.706,     
     @ 0.430,0.412,0.000,0.800,0.692,0.402,0.427,0.000,0.800,0.678,     
     @ 0.376,0.442,0.000,0.800,0.661,0.353,0.456,0.000,0.800,0.650,     
     @ 0.333,0.467,0.000,0.800,0.632,0.315,0.477,0.000,0.800,0.619,     
     @ 0.299,0.486,0.000,0.800,0.605,0.284,0.200,0.295,0.800,0.592,     
     @ 0.272,0.200,0.302,0.800,0.578,0.261,0.200,0.308,0.800,0.567,     
     @ 0.252,0.200,0.313,0.800,0.555,0.245,0.200,0.317,0.800,0.545/     
      data (branch(i),i=251,500)/                                       
     @ 0.240,0.200,0.320,0.800,0.531,0.235,0.200,0.323,0.800,0.522,     
     @ 0.232,0.200,0.324,0.800,0.510,0.230,0.200,0.326,0.800,0.500,     
     @ 0.227,0.200,0.327,0.800,0.493,0.225,0.200,0.329,0.800,0.483,     
     @ 0.224,0.200,0.329,0.800,0.472,0.222,0.200,0.330,0.800,0.465,     
     @ 0.221,0.200,0.331,0.800,0.457,0.221,0.200,0.331,0.800,0.449,     
     @ 0.220,0.200,0.331,0.800,0.445,0.219,0.200,0.332,0.800,0.439,     
     @ 0.219,0.200,0.332,0.800,0.431,0.218,0.200,0.332,0.800,0.428,     
     @ 0.217,0.200,0.333,0.800,0.423,0.217,0.200,0.333,0.800,0.419,     
     @ 0.216,0.200,0.334,0.800,0.417,0.216,0.200,0.334,0.800,0.414,     
     @ 0.216,0.200,0.334,0.799,0.412,0.215,0.200,0.334,0.799,0.410,     
     @ 0.215,0.200,0.334,0.797,0.408,0.215,0.200,0.334,0.794,0.407,     
     @ 0.215,0.200,0.335,0.793,0.405,0.214,0.200,0.335,0.790,0.406,     
     @ 0.214,0.200,0.335,0.791,0.406,0.214,0.200,0.335,0.789,0.405,     
     @ 0.214,0.200,0.335,0.788,0.404,0.214,0.200,0.335,0.786,0.402,     
     @ 0.213,0.200,0.335,0.781,0.402,0.213,0.200,0.335,0.782,0.402,     
     @ 0.213,0.200,0.335,0.783,0.402,0.213,0.200,0.335,0.779,0.401,     
     @ 0.213,0.200,0.336,0.778,0.401,0.212,0.200,0.336,0.785,0.401,     
     @ 0.212,0.200,0.336,0.782,0.401,0.212,0.200,0.336,0.780,0.400,     
     @ 0.212,0.200,0.336,0.778,0.400,0.212,0.200,0.336,0.781,0.400,     
     @ 0.212,0.200,0.336,0.779,0.399,0.212,0.200,0.336,0.777,0.399,     
     @ 0.212,0.200,0.336,0.777,0.399,0.212,0.200,0.336,0.780,0.398,     
     @ 0.212,0.200,0.336,0.779,0.398,0.211,0.200,0.336,0.776,0.398,     
     @ 0.211,0.200,0.336,0.778,0.398,0.211,0.200,0.336,0.781,0.398,     
     @ 0.211,0.200,0.336,0.779,0.399,0.211,0.200,0.336,0.783,0.399,     
     @ 0.211,0.200,0.337,0.783,0.399,0.211,0.200,0.337,0.780,0.399      
     @ /                                                                
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
      data vernum/'version   rqmd 1.09--- 25-jan-94'/                   
      data pronam/'module: c m a s s '/                                 
      data ttvers/.true./                                               
      real*4 pjin(4)                                                    
      common /pjbk/ pjin                                                
      if (ttvers) then                                                  
        write(lun8,*)'---------------- version number report -------------'
        write(lun8,*)pronam                                                
        write(lun8,*)vernum                                                
        write(lun8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
        embr(1)=emmnn2                                                  
        embr(2)=emnuc+2.*(empion+ekinmi)                                
        embr(3)=emnuc+emrho+ekinmi                                      
        embr(4)=emnuc+emeta+ekinmi                                      
      end if                                                            
      s=srt                                                             
      if(s.gt.tblhig) then                                              
         ic=0                                                           
         return                                                         
      end if                                                            
      xpt= log(srt)                                                     
      tlow = log(tbllow)                                                
      index= int((xpt- tlow)/tblstp)                                    
      if( index .le. 0) index =1                                        
      if( index .gt. itblsz) index =itblsz                              
      bet= beta(index)                                                  
      if(icltyp.le.3) then                                              
        pdn=min(cvalu1,exp(-bet*(srt- emdsmn-emnuc)))                   
        pdd=min(cvalu1,exp(-bet*(srt- 2*emdsmn)))                       
        pdnp=min(cvalu1,exp(-bet*(srt- emdsmn-emmnn2)))                 
        p1plus=0.5*(srt-ekinmi) + pr                                    
        p1mins=0.5*(srt-ekinmi) - pr                                    
        p2plus=p1mins                                                   
        p2mins=p1plus                                                   
        pplus=srt-ekinmi                                                
        pminus=srt-ekinmi                                               
      else if(ic.ge.3.and.                                              
     a       icltyp.ge.4.and.icltyp.le.17.and.                          
     b                                     min(ityp1,ityp2).le.2) then  
        go to 8                                                         
      else if(ic.ge.3.and.                                              
     a       icltyp.ge.4.and.icltyp.le.21.and.                          
     b                                     min(ityp1,ityp2).ge.3) then  
        go to 9                                                         
      end if                                                            
      goto (                                                            
     >   1,  2,  3,  4,  5,                                             
     >   4,  5,  6,  4,  5,                                             
     >   6,  7,  5,  6,  7,                                             
     >   7,  4,  6,  7,  4,                                             
     >   4,  4,  4,  4,  4,                                             
     >   4,  4,  4,  4,  4,                                             
     >   4,  4,  4,  4,  4,                                             
     >   4,  4,  4,  4,  4,                                             
     >   4,  4,  4,  4,  4,                                             
     >   5,  6,  4,  5,  6,                                             
     >   7,  4,  4,  4,  5,                                             
     >   6,  7,  5,  6,  7,                                             
     >   4,  4,  4,  4,  6,                                             
     >   7                                                              
     & ) , icltyp                                                       
      write(lun8,*)' icltyp invalid in  cmass   ',icltyp                   
      call errex( 'cmass:fatal error')                                  
    1 assign 100 to label                                               
      if (ic .eq. 1 .or. ic .eq. 2 ) then                               
          goto 1000                                                     
      else if (ic .eq. 3 ) then                                         
          goto 2000                                                     
      else if (ic .eq. 4 .or. ic .eq. 5 ) then                          
          goto 3000                                                     
      else if (ic .eq. 6 .or. ic .eq. 7 ) then                          
          goto 4000                                                     
      else if (ic .eq. 8 ) then                                         
          goto 5000                                                     
      else                                                              
          call errex( 'error in cmass 1 invalid channel' )              
      end if                                                            
  100 if (ic .eq. 1) then                                               
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=4                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp2=4                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 2) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=3                                                       
          ityp2=2                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp1=2                                                       
          ityp2=3                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 3) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=10                                                      
          npid1=npid                                                    
        else                                                            
          em2=em                                                        
          ityp2=10                                                      
          npid2=npid                                                    
        end if                                                          
      else if (ic .eq. 4) then                                          
        em1=ema                                                         
        em2=emb                                                         
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
        if (ranf(0) .gt. 0.5) then                                      
          ityp1=5                                                       
          ityp2=3                                                       
        else                                                            
          ityp1=3                                                       
          ityp2=5                                                       
        end if                                                          
      else if (ic .eq. 5) then                                          
        em1=ema                                                         
        em2=emb                                                         
          ityp1=4                                                       
          ityp2=4                                                       
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
      else if (ic .eq. 6) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=ema                                                       
          em2=emb                                                       
          ityp1=4                                                       
          ityp2=10                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          em2=ema                                                       
          ityp1=10                                                      
          ityp2=4                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 7) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=ema                                                       
          em2=emb                                                       
          ityp1=3                                                       
          ityp2=11                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          em2=ema                                                       
          ityp1=11                                                      
          ityp2=3                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 8) then                                          
          em1=ema                                                       
          em2=emb                                                       
          ityp1=10                                                      
          ityp2=10                                                      
          npid1=npida                                                   
          npid2=npidb                                                   
      else                                                              
        call errex( 'ic-error cmass-100' )                              
      end if                                                            
      return                                                            
    2 assign 200 to label                                               
      if (ic .eq. 1 .or. ic .eq. 2 ) then                               
          goto 1000                                                     
      else if (ic .eq. 3 .or. ic .eq. 4 ) then                          
          goto 2000                                                     
      else if (ic .eq. 5 .or. ic .eq. 6 ) then                          
          goto 3000                                                     
      else if (ic .eq. 7 .or. ic .eq. 8 ) then                          
          goto 4000                                                     
      else if (ic .eq. 9 ) then                                         
          goto 5000                                                     
      else                                                              
          call errex( 'error in cmass 1 invalid channel' )              
      end if                                                            
  200 if (ic .eq. 1) then                                               
        if (ranf(0).gt. 0.5) then                                       
          em1=em                                                        
          ityp1=5                                                       
          ityp2=1                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp1=1                                                       
          ityp2=5                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 2) then                                          
        if (ranf(0).gt. 0.5) then                                       
          em1=em                                                        
          ityp1=4                                                       
          ityp2=2                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp1=2                                                       
          ityp2=4                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 3) then                                          
        if (ranf(0).gt. 0.5) then                                       
          em1=em                                                        
          ityp1=11                                                      
          ityp2=1                                                       
          npid1=npid                                                    
        else                                                            
          em2=em                                                        
          ityp1=1                                                       
          ityp2=11                                                      
          npid2=npid                                                    
        end if                                                          
      else if (ic .eq. 4) then                                          
        if (ranf(0).gt. 0.5) then                                       
          em1=em                                                        
          ityp1=10                                                      
          ityp2=2                                                       
          npid1=npid                                                    
        else                                                            
          em2=em                                                        
          ityp1=2                                                       
          ityp2=10                                                      
          npid2=npid                                                    
        end if                                                          
      else if (ic .eq. 5) then                                          
        em1=ema                                                         
        em2=emb                                                         
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
        if (ranf(0).gt. 0.5) then                                       
          ityp1=4                                                       
          ityp2=5                                                       
        else                                                            
          ityp1=5                                                       
          ityp2=4                                                       
        end if                                                          
      else if (ic .eq. 6) then                                          
        em1=ema                                                         
        em2=emb                                                         
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
        if (ranf(0).gt. 0.5) then                                       
          ityp1=6                                                       
          ityp2=3                                                       
        else                                                            
          ityp1=3                                                       
          ityp2=6                                                       
        end if                                                          
      else if (ic .eq. 7) then                                          
        if (ranf(0).gt. 0.5) then                                       
          em1=ema                                                       
          ityp1=5                                                       
          em2=emb                                                       
          ityp2=10                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          ityp1=10                                                      
          em2=ema                                                       
          ityp2=5                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 8) then                                          
        if (ranf(0).gt. 0.5) then                                       
          em1=ema                                                       
          ityp1=4                                                       
          em2=emb                                                       
          ityp2=11                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          ityp1=11                                                      
          em2=ema                                                       
          ityp2=4                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 9) then                                          
        em1=ema                                                         
        em2=emb                                                         
        npid1=npida                                                     
        npid2=npidb                                                     
        if (ranf(0).gt. 0.5) then                                       
          ityp1=10                                                      
          ityp2=11                                                      
        else                                                            
          ityp1=11                                                      
          ityp2=10                                                      
        end if                                                          
      else                                                              
        call errex( 'ic-error cmass-200' )                              
      end if                                                            
      return                                                            
    3 assign 300 to label                                               
      if (ic .eq. 1 .or. ic .eq. 2 ) then                               
          goto 1000                                                     
      else if (ic .eq. 3 ) then                                         
          goto 2000                                                     
      else if (ic .eq. 4 .or. ic .eq. 5 ) then                          
          goto 3000                                                     
      else if (ic .eq. 6 .or. ic .eq. 7 ) then                          
          goto 4000                                                     
      else if (ic .eq. 8 ) then                                         
          goto 5000                                                     
      else                                                              
          call errex( 'error in cmass 2 invalid channel' )              
      end if                                                            
  300 if (ic .eq. 1) then                                               
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=5                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp2=5                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 2) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=6                                                       
          ityp2=1                                                       
          if(ipdn.eq.0) then                                            
             npid1=1                                                    
          else                                                          
             npid1=npid                                                 
          end if                                                        
        else                                                            
          em2=em                                                        
          ityp1=1                                                       
          ityp2=6                                                       
          if(ipdn.eq.0) then                                            
             npid2=1                                                    
          else                                                          
             npid2=npid                                                 
          end if                                                        
        end if                                                          
      else if (ic .eq. 3) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=em                                                        
          ityp1=11                                                      
          npid1=npid                                                    
        else                                                            
          em2=em                                                        
          ityp2=11                                                      
          npid2=npid                                                    
        end if                                                          
      else if (ic .eq. 4) then                                          
        em1=ema                                                         
        em2=emb                                                         
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
        if (ranf(0) .gt. 0.5) then                                      
          ityp1=6                                                       
          ityp2=4                                                       
        else                                                            
          ityp1=4                                                       
          ityp2=6                                                       
        end if                                                          
      else if (ic .eq. 5) then                                          
        em1=ema                                                         
        em2=emb                                                         
          ityp1=5                                                       
          ityp2=5                                                       
        if(ipdd.eq.0) then                                              
           npid1=1                                                      
           npid2=1                                                      
        else                                                            
           npid1=npida                                                  
           npid2=npidb                                                  
        end if                                                          
      else if (ic .eq. 6) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=ema                                                       
          em2=emb                                                       
          ityp1=5                                                       
          ityp2=11                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          em2=ema                                                       
          ityp1=11                                                      
          ityp2=5                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 7) then                                          
        if (ranf(0) .gt. 0.5) then                                      
          em1=ema                                                       
          em2=emb                                                       
          ityp1=6                                                       
          ityp2=10                                                      
          if(ipdnp.eq.0) then                                           
             npid1=1                                                    
          else                                                          
             npid1=npida                                                
          end if                                                        
          npid2=npidb                                                   
        else                                                            
          em1=emb                                                       
          em2=ema                                                       
          ityp1=10                                                      
          ityp2=6                                                       
          if(ipdnp.eq.0) then                                           
             npid2=1                                                    
          else                                                          
             npid2=npida                                                
          end if                                                        
          npid1=npidb                                                   
        end if                                                          
      else if (ic .eq. 8) then                                          
          em1=ema                                                       
          em2=emb                                                       
          ityp1=11                                                      
          ityp2=11                                                      
          npid1=npida                                                   
          npid2=npidb                                                   
      else                                                              
        call errex( 'ic-error cmass-300' )                              
      end if                                                            
      return                                                            
    4 call errex( ' cmass -- 4')                                        
    5 em1=emnuc                                                         
      em2=emnuc                                                         
      ityp1=1                                                           
      ityp2=1                                                           
      npid1=0                                                           
      npid2=0                                                           
      return                                                            
    6 em1=emnuc                                                         
      em2=emnuc                                                         
      npid1=0                                                           
      npid2=0                                                           
      if( ranf(0) .gt. 0.5 ) then                                       
         ityp1=1                                                        
         ityp2=2                                                        
      else                                                              
         ityp1=2                                                        
         ityp2=1                                                        
      end if                                                            
      return                                                            
    7 em1=emnuc                                                         
      em2=emnuc                                                         
      ityp1=2                                                           
      ityp2=2                                                           
      npid1=0                                                           
      npid2=0                                                           
      return                                                            
    8 continue                                                          
      ibasd= 9                                                          
      ibasn= 3                                                          
      if(ityp1.le.2) then                                               
          m1= 2 * ityp2 - ibasd                                         
          m2= 2 * ityp1 - ibasn                                         
      else                                                              
          m1= 2 * ityp1 - ibasd                                         
          m2= 2 * ityp2 - ibasn                                         
      end if                                                            
      m=m1+m2                                                           
      if(mod(ic,2).eq.0) then                                           
            jnew=4                                                      
      else                                                              
            jnew=2                                                      
      end if                                                            
      if(ic.le.4) then                                                  
        npid1=0                                                         
        npid2=0                                                         
        pdn=1.                                                          
        assign 801 to label                                             
        go to 1000                                                      
  801   continue                                                        
        itag=-50                                                        
        j1new=1                                                         
        j2new=3                                                         
        call isonew(jnew,m,j1new,j2new,m1new,m2new,itag)                
        if(itag.eq.-1) then                                             
         write(lun8,*) 'dn==> closed in cmass ????? itag=-1'               
         ic=0                                                           
         return                                                         
        end if                                                          
        if(ityp2.le.2) then                                             
          em1=emnuc                                                     
          em2=em                                                        
          ityp1=(m1new+ibasn)/2                                         
          ityp2=(m2new+ibasd)/2                                         
          npid2=1                                                       
        else                                                            
          em2=emnuc                                                     
          em1=em                                                        
          ityp2=(m1new+ibasn)/2                                         
          ityp1=(m2new+ibasd)/2                                         
          npid1=1                                                       
        end if                                                          
      else                                                              
        if(da(12).lt.1.) then                                           
          if(ranf(0).gt.da(12)) then                                    
             ic=0                                                       
             return                                                     
          end if                                                        
        end if                                                          
        npid1=1                                                         
        npid2=1                                                         
        pdd=1.                                                          
        assign 800 to label                                             
        go to 3000                                                      
  800   continue                                                        
        em1=ema                                                         
        em2=emb                                                         
        j1new=3                                                         
        j2new=3                                                         
        itag=-50                                                        
        call isonew(jnew,m,j1new,j2new,m1new,m2new,itag)                
        if(itag.eq.-1) then                                             
         write(lun8,*) 'dn==> closed in cmass ????? itag=-1'               
         ic=0                                                           
         return                                                         
        end if                                                          
        ityp1=(m1new+ibasd)/2                                           
        ityp2=(m2new+ibasd)/2                                           
      end if                                                            
      return                                                            
    9 continue                                                          
      ibasd= 9                                                          
      ibasn= 3                                                          
      m1= 2 * ityp1 - ibasd                                             
      m2= 2 * ityp2 - ibasd                                             
      m=m1+m2                                                           
      if(mod(ic,2).eq.0) then                                           
            jnew=4                                                      
      else                                                              
            jnew=2                                                      
      end if                                                            
      itag=-50                                                          
      j1new=1                                                           
      j2new=3                                                           
      call isonew(jnew,m,j1new,j2new,m1new,m2new,itag)                  
      if(itag.eq.-1) then                                               
       write(lun8,*) 'dd==>dn closed in cmass ????? itag=-1'               
         ic=0                                                           
         return                                                         
      end if                                                            
      if(ranf(0).gt.0.5) then                                           
          em1=emnuc                                                     
          ityp1=(m1new+ibasn)/2                                         
          ityp2=(m2new+ibasd)/2                                         
          npid1=0                                                       
          npid2=1                                                       
      else                                                              
          em2=emnuc                                                     
          ityp2=(m1new+ibasn)/2                                         
          ityp1=(m2new+ibasd)/2                                         
          npid2=0                                                       
          npid1=1                                                       
      end if                                                            
      if(em1+em2+ekinmi.ge.s) then                                      
        assign 900 to label                                             
        go to 1000                                                      
  900   continue                                                        
        if(em1.eq.emnuc) then                                           
          em2=em                                                        
        else                                                            
          em1=em                                                        
        end if                                                          
      end if                                                            
      return                                                            
 1000 emmax=srt-emnuc-ekinmi                                            
      ipdn=0                                                            
      if(ranf(0).gt.pdn) then                                           
        if(emmax.gt.emdsmn) then                                        
          ipdn=1                                                        
          go to 1001                                                    
        else                                                            
          write(lun8,*)'cmass:not enough cms energy in n+delta* channel'   
        end if                                                          
      end if                                                            
      if(emmax.le.emnuc+empion) then                                    
          ic=0                                                          
          return                                                        
      end if                                                            
      xmax=(atan(2.*(emmax-emdelt)/widdlt)-const1)/const2               
      itry=0                                                            
  998 continue                                                          
      xmax=min(cvalu1,xmax)                                             
      if (xmax.le.0.) then                                              
        ic=0                                                            
        write(lun8,*) ' lower delta threshold violated in cmass 1000'      
        return                                                          
      end if                                                            
      x=xmax*ranf(0)                                                    
      t=tan(x*const2)                                                   
      em=emdelt+.5*widdlt*(const+t)/(1.-const*t)                        
      prel=pcmsr(em,emnuc,empion)                                       
      gamd=29.*prel**3/(1.+40.*prel*prel)                               
      prbact=gamd/((em-emdelt)**2+0.25*gamd*gamd)                       
      index=int((log(s)-log(tbllow))/tblstp) + 1                        
      if(index.gt.7) then                                               
        if(index.gt.60) index=60                                        
        indeff=index-7                                                  
        emmin=emnuc+empion                                              
        emdmx=2.0                                                       
        mbin=                                                           
     a    int(float(20)*(em-emmin)/(min(emdmx,s-emnuc)-emmin))+1        
        mbin=min(20,max(1,mbin))                                        
        prbact=prbact*dsdm(indeff,mbin)                                 
      end if                                                            
      prbmxa=3.0*widdlt/((em-emdelt)**2+0.25*widdlt*widdlt)             
      prbact=prbact/prbmxa                                              
      if(prbact.gt.1.3) then                                            
         write(lun8,*) 'cmass-warning: 1.3< prbact=',prbact                
      end if                                                            
      if(itry.lt.30.and.ranf(0).gt.prbact) then                         
           itry=itry+1                                                  
           go to 998                                                    
      else if(itry.eq.30) then                                          
           write(lun8,*) itry,s,'cmass 1000:itry=30 ????   em=',em         
      end if                                                            
      if(em.gt.emmax) then                                              
        write(lun8,*)'warning: em.gt.emmax in cmass1000 ',em,emmax         
        em=emmax                                                        
      end if                                                            
      goto label                                                        
 1001 continue                                                          
      zbr(1)=0.2                                                        
      zbr(2)=0.65                                                       
      zbr(3)=1.0                                                        
      zbr(4)=1.0                                                        
      zchnce=ranf(0)                                                    
      do 1002 i=1,4                                                     
        if(zchnce.lt.zbr(i)) then                                       
          ist=i                                                         
          go to 1003                                                    
        end if                                                          
 1002 continue                                                          
 1003 continue                                                          
      embrd=embr(ist)                                                   
      if(srt-ekinmi.lt. emnuc+max(emmnd2,embrd))then                    
         write(lun8,*) 'cmass:1003 lower cmsenergy threshold violated'     
         write(lun8,*) srt, ist                                            
         if(ist.eq.1) then                                              
             ic=0                                                       
             return                                                     
         end if                                                         
         ist=1                                                          
         go to 1003                                                     
      end if                                                            
      epdmin=srt                                                        
      epmin=emnuc                                                       
      etdmin=emmnd2                                                     
      etmin=emmnd2                                                      
      assign 1004 to label2                                             
      ltry=0                                                            
      go to 60                                                          
 1004 continue                                                          
      em=em2new                                                         
      ltry=ltry+1                                                       
      npid=-mbafnd(1,em)                                                
      if(npid.eq.3) then                                                
        if(ltry.gt.50) then                                             
          npid=-9                                                       
        else                                                            
          go to 1004                                                    
        end if                                                          
      end if                                                            
      if(iqcnvt.eq.0) npid=-1000*ist+npid                               
      go to label                                                       
 2000 continue                                                          
      zbr(1)=0.7                                                        
      zbr(2)=0.86                                                       
      zbr(3)=0.86                                                       
      zbr(4)=1.0                                                        
      zchnce=ranf(0)                                                    
      do 2002 i=1,4                                                     
        if(zchnce.lt.zbr(i)) then                                       
          ist=i                                                         
          go to 2003                                                    
        end if                                                          
 2002 continue                                                          
 2003 continue                                                          
      if(srt-ekinmi.lt. emnuc+embr(ist))then                            
         write(lun8,*) 'cmass:2003 lower cmsenergy threshold violated'     
         write(lun8,*) srt, ist                                            
         if(ist.eq.1) then                                              
             ic=0                                                       
             return                                                     
         end if                                                         
         ist=1                                                          
         go to 2003                                                     
      end if                                                            
      epdmin=srt                                                        
      epmin=emnuc                                                       
      etdmin=emmnn2                                                     
      etmin=emmnn2                                                      
      assign 2004 to label2                                             
      ltry=0                                                            
      go to 60                                                          
 2004 continue                                                          
      em=em2new                                                         
      ltry=ltry+1                                                       
      npid=-mbafnd(2,em)                                                
      if(npid.eq.3) then                                                
        if(ltry.gt.50) then                                             
          npid=-10                                                      
        else                                                            
          go to 2004                                                    
        end if                                                          
      end if                                                            
      if(iqcnvt.eq.0) npid=-1000*ist+npid                               
      goto label                                                        
 3000 continue                                                          
      ipdd=0                                                            
      if(ranf(0).gt.pdd) then                                           
         ipdd=1                                                         
         zbr(1)=0.2                                                     
         zbr(2)=0.65                                                    
         zbr(3)=1.0                                                     
         zbr(4)=1.0                                                     
         zchnce=ranf(0)                                                 
         do 3002 i=1,4                                                  
          if(zchnce.lt.zbr(i)) then                                     
            ista=i                                                      
            go to 3003                                                  
          end if                                                        
 3002    continue                                                       
 3003    continue                                                       
         zchnce=ranf(0)                                                 
         do 3004 i=1,4                                                  
          if(zchnce.lt.zbr(i)) then                                     
            istb=i                                                      
            go to 3005                                                  
          end if                                                        
 3004    continue                                                       
 3005    continue                                                       
         if(srt-ekinmi.lt.embr(ista)+embr(istb)) then                   
            write(lun8,*) 'cmass:3005 lower cms energy threshold violated' 
            write(lun8,*) srt, ist                                         
            if(ista.eq.1.and.istb.eq.1) then                            
               ic=0                                                     
               return                                                   
            end if                                                      
            if(ista.eq.1.and.istb.ne.1) istb=1                          
            ista=1                                                      
            if(ista+istb.gt.2 ) go to 3003                              
            go to 3005                                                  
         end if                                                         
         epdmin=emmnd2                                                  
         epmin=emmnd2                                                   
         etdmin=emmnd2                                                  
         etmin=emmnd2                                                   
         assign 3008 to label2                                          
         ltry=0                                                         
         npida=3                                                        
         npidb=3                                                        
         go to 60                                                       
 3008    continue                                                       
         ema=em1new                                                     
         emb=em2new                                                     
         if(npida.eq.3) npida=-mbafnd(1,ema)                            
         if(npidb.eq.3) npidb=-mbafnd(1,emb)                            
         ltry=ltry+1                                                    
         if(npida.eq.3.or.npidb.eq.3) then                              
            if(ltry.gt.50) then                                         
              if(npida.eq.3) npida=-9                                   
              if(npidb.eq.3) npidb=-9                                   
            else                                                        
              go to 3008                                                
            end if                                                      
         end if                                                         
         if(iqcnvt.eq.0) npida=-1000*ista+npida                         
         if(iqcnvt.eq.0) npidb=-1000*istb+npidb                         
         if (ema+emb.ge.s-ekinmi ) then                                 
            call errex('cmass:too large masses in d* d* branch')        
         end if                                                         
         go to label                                                    
      else                                                              
        emmx1=srt-(emnuc+empion+ekinmi)                                 
        if(emmx1.le.emnuc+empion) then                                  
           ic=0                                                         
           return                                                       
        end if                                                          
        itry=0                                                          
 3009   continue                                                        
        xmax=(atan(2.*(emmx1-emdelt)/widdlt)-const1)/const2             
        xmax=min(cvalu1,xmax)                                           
        if (xmax .le. 0.) then                                          
           ic=0                                                         
           write(lun8,*) ' lower threshold violated in cmass 3000'         
           return                                                       
        end if                                                          
        x=xmax*ranf(0)                                                  
        t=tan(x*const2)                                                 
        ema = emdelt+.5*widdlt*(const+t)/(1.-const*t)                   
        prel=pcmsr(ema,emnuc,empion)                                    
        gamd=29.*prel**3/(1.+40.*prel*prel)                             
        prbact=gamd/((ema-emdelt)**2+0.25*gamd*gamd)                    
        prbmxa=1.9*widdlt/((ema-emdelt)**2+0.25*widdlt*widdlt)          
        prbact=prbact/prbmxa                                            
        if(itry.lt.30.and.ranf(0).gt.prbact) then                       
           itry=itry+1                                                  
           go to 3009                                                   
        end if                                                          
        emmx2=srt-(ema+ekinmi)                                          
        xmax=(atan(2.*(emmx2-emdelt)/widdlt)-const1)/const2             
        xmax=min(cvalu1,xmax)                                           
        if (xmax.le.0.) then                                            
           ic=0                                                         
           write(lun8,*) 'lower threshold violated in cmass 3000'          
           return                                                       
        end if                                                          
        x=xmax*ranf(0)                                                  
        t=tan(x*const2)                                                 
        emb = emdelt+.5*widdlt*(const+t)/(1.-const*t)                   
        prel=pcmsr(emb,emnuc,empion)                                    
        gamd=29.*prel**3/(1.+40.*prel*prel)                             
        prbact=gamd/((emb-emdelt)**2+0.25*gamd*gamd)                    
        prbmxb=1.9*widdlt/((emb-emdelt)**2+0.25*widdlt*widdlt)          
        prbact=prbact/prbmxb                                            
        if(itry.lt.30.and.ranf(0).gt.prbact) then                       
           itry=itry+1                                                  
           go to 3009                                                   
        end if                                                          
        if(ranf(0).gt.0.5) then                                         
          em=ema                                                        
          ema=emb                                                       
          emb=em                                                        
        end if                                                          
        goto label                                                      
      end if                                                            
 4000 continue                                                          
      ipdnp=0                                                           
      if(ranf(0).gt.pdnp) then                                          
         ipdnp=1                                                        
         xpt= log(s-ekinmi)                                             
         tlow = log(tbllow)                                             
         index= int((xpt- tlow)/tblstp)+1                               
         if( index.le.0) index =1                                       
         indf=5*(index-1)+4                                             
         zbr(1)=branch(indf)                                            
         zbr(2)=1.                                                      
         zbr(3)=1.                                                      
         zbr(4)=1.                                                      
         zchnce=ranf(0)                                                 
         do 4004 i=1,4                                                  
          if(zchnce.lt.zbr(i)) then                                     
            istb=i                                                      
            go to 4005                                                  
          end if                                                        
 4004    continue                                                       
 4005    continue                                                       
         zbr(1)=0.2                                                     
         zbr(2)=0.65                                                    
         zbr(3)=1.0                                                     
         zbr(4)=1.0                                                     
         zchnce=ranf(0)                                                 
         do 4002 i=1,4                                                  
          if(zchnce.lt.zbr(i)) then                                     
            ista=i                                                      
            go to 4003                                                  
          end if                                                        
 4002    continue                                                       
 4003    continue                                                       
         if(srt-ekinmi.lt.embr(ista)+embr(istb)) then                   
            write(lun8,*) 'cmass:4005 lower cms energy threshold violated' 
            write(lun8,*) srt, ist                                         
            if(ista.eq.1.and.istb.eq.1) then                            
               ic=0                                                     
               return                                                   
            end if                                                      
            if(istb.eq.1) ista=1                                        
            istb=1                                                      
            if(ista+istb.gt.2 ) go to 4005                              
            go to 4003                                                  
         end if                                                         
         epdmin=emmnd2                                                  
         epmin=emmnd2                                                   
         etdmin=emmnn2                                                  
         etmin=emmnn2                                                   
         assign 4009 to label2                                          
         ltry=0                                                         
         npida=3                                                        
         npidb=3                                                        
         go to 60                                                       
 4009    continue                                                       
         ema=em1new                                                     
         emb=em2new                                                     
         if(npida.eq.3) npida=-mbafnd(1,ema)                            
         if(npidb.eq.3) npidb=-mbafnd(2,emb)                            
         ltry=ltry+1                                                    
         if(npida.eq.3.or.npidb.eq.3) then                              
            if(ltry.gt.50) then                                         
              if(npida.eq.3) npida=-9                                   
              if(npidb.eq.3) npidb=-10                                  
            else                                                        
              go to 4009                                                
            end if                                                      
         end if                                                         
         if(iqcnvt.eq.0) npida=-1000*ista+npida                         
         if(iqcnvt.eq.0) npidb=-1000*istb+npidb                         
         if(ema+emb.gt.srt) then                                        
             write(lun8,*) 'ema,emb,s',ema,emb,s                           
             call errex('wrong masses in cmass 4000/first branch')      
         end if                                                         
         go to label                                                    
      else                                                              
         xpt= log(srt)                                                  
         tlow = log(tbllow)                                             
         index= int((xpt- tlow)/tblstp)+1                               
         if( index .le. 0) index =1                                     
         indf=5*(index-1)+1                                             
         zbr(1)=branch(indf)                                            
         zbr(2)=zbr(1)+branch(indf+1)                                   
         zbr(3)=zbr(2)+branch(indf+2)                                   
         zbr(4)=1.0                                                     
         zchnce=ranf(0)                                                 
         do 4006 i=1,4                                                  
          if(zchnce.lt.zbr(i)) then                                     
            ist=i                                                       
            go to 4007                                                  
          end if                                                        
 4006    continue                                                       
 4007    continue                                                       
         if(srt-ekinmi.lt. emdelt+embr(ist))then                        
           write(lun8,*) 'cmass:4007 lower cmsenergy threshold violated'   
           write(lun8,*) srt, ist                                          
           if(ist.eq.1) then                                            
               ic=0                                                     
               return                                                   
           end if                                                       
           ist=1                                                        
           go to 4007                                                   
         end if                                                         
         epdmin=srt                                                     
         epmin=emdelt                                                   
         etdmin=emmnn2                                                  
         etmin=emmnn2                                                   
         assign 4008 to label2                                          
         ltry=0                                                         
         go to 60                                                       
 4008    continue                                                       
         ema=emdelt                                                     
         emb=em2new                                                     
         npidb=-mbafnd(2,emb)                                           
         ltry=ltry+1                                                    
         if(npidb.eq.3) then                                            
            if(ltry.gt.50) then                                         
              if(npidb.eq.3) npidb=-10                                  
            else                                                        
              go to 4008                                                
            end if                                                      
         end if                                                         
         if(iqcnvt.eq.0) npidb=-1000*ist+npidb                          
         if( ema+emb.gt.srt) then                                       
             write(lun8,*) 'ema,emb,s',ema,emb,s                           
             call errex('too large masses in cmass4000')                
         end if                                                         
         go to label                                                    
      end if                                                            
 5000 continue                                                          
      xpt= log(s-ekinmi)                                                
      tlow = log(tbllow)                                                
      index= int((xpt- tlow)/tblstp)+1                                  
      if( index .le. 0) index =1                                        
      indf=5*(index-1)+5                                                
      zbr(1)=branch(indf)                                               
      zbr(2)=1.0                                                        
      zbr(3)=1.0                                                        
      zbr(4)=1.0                                                        
      zchnce=ranf(0)                                                    
      do 5002 i=1,4                                                     
       if(zchnce.lt.zbr(i)) then                                        
          ista=i                                                        
          go to 5003                                                    
       end if                                                           
 5002 continue                                                          
 5003 continue                                                          
      zchnce=ranf(0)                                                    
      do 5004 i=1,4                                                     
       if(zchnce.lt.zbr(i)) then                                        
         istb=i                                                         
         go to 5005                                                     
       end if                                                           
 5004 continue                                                          
 5005 continue                                                          
      if(srt-ekinmi.lt.embr(ista)+embr(istb)) then                      
          write(lun8,*) 'cmass:3005 lower cms energy threshold violated'   
          write(lun8,*) srt, ist                                           
          if(ista.eq.1.and.istb.eq.1) then                              
               ic=0                                                     
               return                                                   
          end if                                                        
          if(ista.eq.1.and.istb.ne.1) istb=1                            
          ista=1                                                        
          if(ista+istb.gt.2 ) go to 5003                                
          go to 5005                                                    
      end if                                                            
      epdmin=emmnn2                                                     
      epmin=emmnn2                                                      
      etdmin=emmnn2                                                     
      etmin=emmnn2                                                      
      assign 5009 to label2                                             
      ltry=0                                                            
      npida=3                                                           
      npidb=3                                                           
      go to 60                                                          
 5009 continue                                                          
      ema=em1new                                                        
      emb=em2new                                                        
      if(npida.eq.3) npida=-mbafnd(2,ema)                               
      if(npidb.eq.3) npidb=-mbafnd(2,emb)                               
      ltry=ltry+1                                                       
      if(npida.eq.3.or.npidb.eq.3) then                                 
        if(ltry.gt.50) then                                             
          if(npida.eq.3) npida=-10                                      
          if(npidb.eq.3) npidb=-10                                      
        else                                                            
          go to 5009                                                    
        end if                                                          
      end if                                                            
      if(iqcnvt.eq.0) npida=-1000*ista+npida                            
      if(iqcnvt.eq.0) npidb=-1000*istb+npidb                            
      if (ema+emb.ge.srt) then                                          
          write(lun8,*) 'ema,emb,srt',ema,emb,srt                          
          call errex('not allowed n* masses in cmass 5000' )            
      end if                                                            
      go to label                                                       
   60 continue                                                          
      if(srt.lt.(etmin+epmin)) then                                     
         write(lun8,*) 'cmass 60: elastic scattering, too low cms energy'  
         write(lun8,*) 'srt,icltyp,ic,etmin,epmin'                         
         write(lun8,*)  srt,icltyp,ic,etmin,epmin                          
         ic=0                                                           
         return                                                         
      end if                                                            
      nyrak=0                                                           
      p1mi= p1mins                                                      
      p1pl=p1plus                                                       
      p2mi=p2mins                                                       
      p2pl=p2plus                                                       
  201 nyrak=nyrak+1                                                     
      if(nyrak.gt.20) then                                              
          if(epdmin.gt.epmin) then                                      
             em1max=epmin                                               
          else                                                          
             em1max=epmin+0.5*(srt-ekinmi-epmin-etmin)                  
          end if                                                        
          em2max=etmin+0.5*(srt-ekinmi-epmin-etmin)                     
          z1rand=ranf(0)                                                
          z2rand=ranf(0)                                                
          em1new=epmin*(em1max/epmin)**z1rand                           
          em2new=etmin*(em2max/etmin)**z2rand                           
          go to label2                                                  
      end if                                                            
      p1mins= p1mi                                                      
      p1plus=p1pl                                                       
      p2mins=p2mi                                                       
      p2plus=p2pl                                                       
      zprand = ranf(0)                                                  
      zmrand = ranf(0)                                                  
      p1mins= p1mins*(p2mins/p1mins)**zmrand                            
      p2plus= p2plus*(p1plus/p2plus)**zprand                            
      p1plus= pplus - p2plus                                            
      p2mins= pminus- p1mins                                            
      if(p1plus.le.p1mins) go to 201                                    
      if(p2plus.gt.p2mins) go to 201                                    
      arg=p1plus*p1mins                                                 
      if(arg.gt.0.) then                                                
          em1new=sqrt(arg)                                              
      else                                                              
         go to 201                                                      
      end if                                                            
      arg=p2plus*p2mins                                                 
      if(arg.gt.0.) then                                                
          em2new=sqrt(arg)                                              
      else                                                              
         go to 201                                                      
      end if                                                            
      if(em1new.lt. epmin) go to 201                                    
      if(em2new.lt. etmin) go to 201                                    
      if( em1new.lt. epdmin) then                                       
           pminny=(epmin*epmin)/p1plus                                  
           delpm= p1mins- pminny                                        
           p1mins= pminny                                               
           p2mins= p2mins+delpm                                         
           em1new= epmin                                                
           em2new=sqrt(p2plus*p2mins)                                   
           if(em2new.lt.etmin) go to 201                                
      end if                                                            
      if( em2new.lt. etdmin) then                                       
           pptany= (etmin*etmin)/p2mins                                 
           delpp= p2plus- pptany                                        
           p1plus= p1plus+delpp                                         
           em2new= etmin                                                
           em1new=sqrt(p1plus*p1mins)                                   
           if(em1new.lt.epmin) go to 201                                
      end if                                                            
      go to label2                                                      
      end                                                               
