CPAT,CUPDAT   .                                                         
      subroutine cupdat(i1,i2)                                          
        include 'scatter7'                                              
        include 'partblk7'                                              
      logical hit                                                       
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version   rqmd 2.2  -- 28-oct-95'/                   
      data pronam/'c u p d a t  -- collision update'/                   
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      idelte = 0                                                        
      do 100 jjj=1,nument                                               
        if((i1c(jjj).eq.i1).or.(i2c(jjj).eq.i1).or.                     
     *      (i1c(jjj).eq.i2).or.(i2c(jjj).eq.i2).or.                    
     *                        (i1c(jjj).le.0)) then                     
           idelte=idelte+1                                              
          if(i1c(jjj).le.0) then                                        
          else if(i1c(jjj).eq.i2.or.i2c(jjj).eq.i2) then                
            idud(1,issn(i2))=idud(1,issn(i2))-1                         
            if(i1c(jjj).eq.i2) then                                     
              idud(1,issn(i2c(jjj)))=idud(1,issn(i2c(jjj)))-1           
            else                                                        
              idud(1,issn(i1c(jjj)))=idud(1,issn(i1c(jjj)))-1           
            end if                                                      
          else                                                          
            idud(1,issn(i1))=idud(1,issn(i1))-1                         
            if(i1c(jjj).eq.i1) then                                     
              idud(1,issn(i2c(jjj)))=idud(1,issn(i2c(jjj)))-1           
            else                                                        
              idud(1,issn(i1c(jjj)))=idud(1,issn(i1c(jjj)))-1           
            end if                                                      
          end if                                                        
          go to 100                                                     
      end if                                                            
      jj=jjj-idelte                                                     
      if(idelte.gt.0) then                                              
        i1c(jj)=i1c(jjj)                                                
        i2c(jj)=i2c(jjj)                                                
        ctime(jj)=ctime(jjj)                                            
        csig(jj)=csig(jjj)                                              
      end if                                                            
  100 continue                                                          
      nument=nument-idelte                                              
      if(ifndnx.eq.1) then                                              
         return                                                         
      end if                                                            
      if(ida(34).eq.2)  return                                          
      iorig=0                                                           
      if(.not.switch(23).and.ida(57).eq.1) iorig=1                      
      if(i1.gt.0) then                                                  
        if(ida(11).eq.1.and.i1.gt.iofset+ntclst) go to 30               
        do 22 i3e=1,ntclst                                              
          i3=i3e+iofset                                                 
          if(i3.eq.i1) go to 22                                         
          if (hit(iorig,i1,i3,srt,sig,sigel,thit)) then                 
            nument=nument+1                                             
            if(nument.gt.numema)                                        
     *        call errex( 'cupdat, collision storage exceeded')         
            i1c(nument)=i1                                              
            i2c(nument)=i3                                              
            ctime(nument)=thit                                          
            csig(nument)=sig                                            
            idud(1,issn(i1))=idud(1,issn(i1))+1                         
            idud(1,issn(i3))=idud(1,issn(i3))+1                         
          end if                                                        
   22   continue                                                        
      end if                                                            
   30 continue                                                          
      if(i2.gt.0) then                                                  
        if(ida(11).eq.1.and.i2.gt.iofset+ntclst) return                 
        do 33 i3e=1,ntclst                                              
          i3=i3e+iofset                                                 
          if(i3.eq.i2) go to 33                                         
          if(i3.eq.i1) go to 33                                         
          if (hit(iorig,i2,i3,srt,sig,sigel,thit)) then                 
            nument=nument+1                                             
            if (nument .gt. numema)                                     
     *        call errex( 'cupdat, collision storage exceeded')         
            i1c(nument)=i2                                              
            i2c(nument)=i3                                              
            ctime(nument)=thit                                          
            csig(nument)=sig                                            
            idud(1,issn(i2))=idud(1,issn(i2))+1                         
            idud(1,issn(i3))=idud(1,issn(i3))+1                         
          end if                                                        
   33   continue                                                        
      end if                                                            
      return                                                            
      end                                                               
