CPAT,SVEIFO   .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine sveifo(ip,is)                                          
        include 'scatter7'                                              
        dimension pkeep(0:3,2),ekeep(2),itkeep(2),deceep(2),npkeep(2)   
        save  pkeep,ekeep,itkeep,deceep,npkeep                          
        include 'partblk7'                                              
      if ((is .le. 0) .or. (is .gt. 2)) call errex('is -- save')        
      pkeep(0,is)=p(0,ip)                                               
      pkeep(1,is)=p(1,ip)                                               
      pkeep(2,is)=p(2,ip)                                               
      pkeep(3,is)=p(3,ip)                                               
      ekeep(is)=em(ip)                                                  
      itkeep(is)=ityp(ip)                                               
      deceep(is)=decay(ip)                                              
      npkeep(is)=npidec(ip)                                             
      return                                                            
      entry rstifo(ip,is)                                               
      if ((is .le. 0) .or. (is .gt. 2)) call errex('is -- restore')     
      p(0,ip)=pkeep(0,is)                                               
      p(1,ip)=pkeep(1,is)                                               
      p(2,ip)=pkeep(2,is)                                               
      p(3,ip)=pkeep(3,is)                                               
      em(ip)=ekeep(is)                                                  
      ityp(ip)=itkeep(is)                                               
      decay(ip)=deceep(is)                                              
      npidec(ip)=npkeep(is)                                             
      return                                                            
      entry oldat(is,itold,npold,p0old,pxold,pyold,pzold,emold)         
      if ((is .le. 0) .or. (is .gt. 2)) call errex('is -- oldat  ')     
      itold=itkeep(is)                                                  
      p0old=pkeep(0,is)                                                 
      pxold=pkeep(1,is)                                                 
      pyold=pkeep(2,is)                                                 
      pzold=pkeep(3,is)                                                 
      emold=ekeep(is)                                                   
      npold=npkeep(is)                                                  
      return                                                            
      end                                                               
