CPAT,FNDNXT   .                                                         
      subroutine fndnxt(index,iwhat)                                    
        include 'scatter7'                                              
        include 'partblk7'                                              
        data iloop /0/                                                  
      save iruno,itripo,kclsto                                          
      data wrilim / 9999.99 /                                           
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version  rqmd 1.09 -- 07-jul-94'/                    
      data pronam/'fndnxt: find the next event(collision,decay)'/       
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      nttot=nrbar(kclst)+nrmes(kclst)                                   
   89 continue                                                          
      if(iloop.eq.1) then                                               
        if(irun.ne.iruno.or.itripl.ne.itripo.or.kclst.ne.kclsto) then   
          iloop=0                                                       
          ifndnx=0                                                      
        end if                                                          
      end if                                                            
      if(iloop.eq.1) then                                               
         do 91 je=1,nttot                                               
          j=je+iofset                                                   
          if(decay(j).lt.wrilim) then                                   
            decayj=decay(j)                                             
            identj=issn(j)                                              
            if(qdot(0,j).gt.0.0) then                                   
              do 88 i=0,3                                               
               x(i,j)=x(i,j)+qdot(i,j)/qdot(0,j)*                       
     a                              (decayj-x(0,j))                     
   88         continue                                                  
            else                                                        
              do 87 i=0,3                                               
               x(i,j)=x(i,j)+p(i,j)/p(0,j)*                             
     a                              (decayj-x(0,j))                     
   87         continue                                                  
            end if                                                      
            qdot(0,j)=0.                                                
            decay(j)=x(0,j)                                             
          end if                                                        
   91    continue                                                       
      end if                                                            
      imin=0                                                            
      tmin=timmax                                                       
      if((iloop.ne.0).or.(.not.switch(18)))then                         
        do 101 je=1,nttot                                               
         j=je+iofset                                                    
         if(ichbig(j).eq.3.and.(iloop.eq.0.or.ida(15).eq.2)) go to 101  
         if(decay(j).le.x(0,j)) then                                    
           if(qdot(0,j).gt.1.e-20) then                                 
             delts=(decay(j)-x(0,j)+qdot(0,j)*dt)/qdot(0,j)             
           else if(qdot(0,j).eq.0..and.x(0,j).eq.decay(j)) then         
             delts=tlstev-time                                          
           else                                                         
             go to 101                                                  
           end if                                                       
           if(time+delts.lt.tmin) then                                  
             imin=j                                                     
             tmin=time+delts                                            
             iwhat=2                                                    
           end if                                                       
         end if                                                         
  101   continue                                                        
      end if                                                            
      if(iloop.eq.0) then                                               
       do 100 i=1,nument                                                
       if(i1c(i).le.0) go to 100                                        
          if (ctime(i).le.tmin) then                                    
            tmin=ctime(i)                                               
            imin=i                                                      
            iwhat=1                                                     
          end if                                                        
  100  continue                                                         
      end if                                                            
      if(imin.eq.0) then                                                
        iwhat=0                                                         
      else                                                              
        index=imin                                                      
      end if                                                            
      if(iwhat.eq.0.and.iloop.eq.0.and.kdt.eq.ndt                       
     a        .and.(itripl.gt.1.or.switch(17).or.switch(23)))then       
         iloop=1                                                        
         ifndnx=1                                                       
         iruno=irun                                                     
         itripo=itripl                                                  
         kclsto=kclst                                                   
         goto 89                                                        
      end if                                                            
      return                                                            
      end                                                               
