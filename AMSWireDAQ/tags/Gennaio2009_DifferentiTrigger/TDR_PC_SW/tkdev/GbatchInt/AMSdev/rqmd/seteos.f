CPAT,SETEOS   .                                                         
      subroutine seteos                                                 
        include 'scatter7'                                              
        include 'partblk7'                                              
      intrep=inttyp/1000                                                
      inttp=mod(inttyp,1000)                                            
      intdel=inttp/100                                                  
      inttp=mod(inttyp,100)                                             
      intbar=inttp/10                                                   
      ictoff=mod(inttp,10)                                              
      if(intdel.lt.0.or.intdel.gt.2) then                               
        call errex('wrong interaction type as input')                   
      else if(intdel.gt.0) then                                         
        ida(35)=intdel                                                  
      end if                                                            
      if(intbar.lt.0.or.intbar.gt.3) then                               
            call errex('wrong interaction type as input')               
      else if(intbar.gt.0) then                                         
        ida(13)=intbar                                                  
      end if                                                            
      if(ictoff.lt.0.or.ictoff.gt.2) then                               
            call errex('wrong interaction type as input')               
      else if(ictoff.gt.0) then                                         
        ida(12)=ictoff                                                  
      end if                                                            
      if(switch(6)) then                                                
      else if(ida(13).eq.1) then                                        
           write(8,*)'potential between nucleons and deltas only'       
      else if(ida(13).eq.2) then                                        
           write(8,*)'potential between non-strange baryons only'       
      else if(ida(13).eq.3) then                                        
           write(8,*)'potential between all baryons'                    
      end if                                                            
      if(switch(6)) then                                                
      else if(ida(12).eq.1) then                                        
           write(8,*)                                                   
     a      'relative momentum dependence in 2body potential'           
      else if(ida(12).eq.2) then                                        
           write(8,*)                                                   
     a      'no relative momentum dependence in 2body potential'        
      end if                                                            
      al =        8.66                                                  
      sal =      21.0                                                   
      if(ida(32).eq.0) then                                             
          alpot =     4.33                                              
          salpot =    7.525                                             
      else if(ida(32).eq.1) then                                        
          alpot =     2.20                                              
          salpot =    (pi*alpot)**1.5*0.15                              
      else if(ida(32).eq.2) then                                        
          alpot =     2.20                                              
          salpot=(pi*alpot)**1.5*0.17                                   
      end if                                                            
      if(ida(32).eq.0.or.ida(32).eq.1) then                             
        if (ieos.eq. 1) then                                            
             alpha = -0.124                                             
             beta  =  0.0705                                            
             gamma =  2.                                                
        else if (ieos.eq. 2)then                                        
             alpha = -0.356                                             
             beta  =  0.303                                             
             gamma =  7./6.                                             
        else                                                            
             write(8,*)'error in seteos  select of eos  run aborted'    
             stop                                                       
        end if                                                          
      else                                                              
        if (ieos.eq. 1) then                                            
             alpha = -0.1466                                            
             beta  =  0.0988                                            
             gamma =  2.                                                
        else if (ieos.eq. 2) then                                       
             alpha = -0.4365                                            
             beta  =  0.385                                             
             gamma =  7./6.                                             
        else                                                            
             write(8,*)'error in seteos  select of eos  run aborted'    
             stop                                                       
        end if                                                          
      end if                                                            
      if(ida(12).eq.1) then                                             
        pr2max=0.5*emnuc*0.27                                           
        alprep=alpha+0.08                                               
      else                                                              
        pr2max=0.0                                                      
      end if                                                            
      if(.not.switch(6)) then                                           
      write(8,116)  alpha,beta,gamma                                    
  116 format(1x,'qmd skyrme   alpha=',f7.3,'  beta=',f7.3,              
     &                                         ' gamma=',f7.3)          
      if(ieos.eq.2) write(8,115)                                        
      if(ieos.eq.1)  write(8,1151)                                      
115   format(1x,'soft e.o.s')                                           
1151  format(1x,'stiff e.o.s')                                          
      end if                                                            
      dstint=25.                                                        
      glk=exp(-dstint/al)*al/dstint                                     
      gm1=gamma-1.                                                      
      eosc1=2.*alpha/(alpot*salpot)                                     
      eosc2=4./(gamma+1.)*beta*gamma/(alpot*salpot)                     
      coef1=alpha*emnuc/salpot                                          
      coef2=2./(gamma+1.)*beta*emnuc                                    
      if(ida(12).eq.1) then                                             
        coef3=alprep*emnuc/salpot                                       
        eosc3=2.*alprep/(alpot*salpot)                                  
      else                                                              
        coef3=coef1                                                     
        eosc3=eosc1                                                     
      end if                                                            
      return                                                            
      end                                                               
      function velo12(srt,pr,eml,emk)                                   
        include 'scatter7'                                              
        denom=srt**2-eml**2-emk**2                                      
        if(denom.le.0.0) then                                           
          velo12=0.0                                                    
        else                                                            
          velo12=2.*pr*srt/denom                                        
        end if                                                          
      return                                                            
      end                                                               
