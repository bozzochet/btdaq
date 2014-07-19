CPAT,ANGIN    .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine angin(s,pr,prold,icltyp,t1,c1)                         
        include 'amsrqmd.h'
        include 'scatter7'                                              
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'ver rqmd 1.07 ---- 09-mar-91'/                       
      data pronam/'module: a n g i n'/                                  
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(lun8,*)'---------------- version number report -------------'
        write(lun8,*)pronam                                                
        write(lun8,*)vernum                                                
        write(lun8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      if(ida(37).eq.1) then                                             
          go to 10                                                      
      end if                                                            
      ta= 2.*pr*prold                                                   
      a1=6.                                                             
      a=a1+max(cvalu0,0.23*(log(s*s)-log(100.)))                        
      y3=ranf(0)                                                        
      ata=ta *a                                                         
      if(ata.gt.35.) then                                               
           iloop=0                                                      
   13      zrand=ranf(0)                                                
           c1=1.+ 2./(ata) * log(max(cvalu2,zrand))                     
           if(c1.lt.0.) then                                            
              iloop=iloop+1                                             
              if(iloop.lt.100) then                                     
                 go to 13                                               
              else                                                      
                 call errex('no cosine theta found in angin')           
              end if                                                    
           else                                                         
              c1= sqrt(c1)                                              
           end if                                                       
      else                                                              
         arg=   exp(-2.*ata)                                            
         pt=log( (1.-y3)*arg+y3 )                                       
         c1=1.+pt/ata                                                   
      end if                                                            
      if (abs(c1) .gt. 1.0) c1=sign(cvalu1,c1)                          
      t1=pi2*ranf(0)                                                    
      return                                                            
   10 c1=1.-2.*ranf(0)                                                  
      t1=pi2*ranf(0)                                                    
      return                                                            
      end                                                               
