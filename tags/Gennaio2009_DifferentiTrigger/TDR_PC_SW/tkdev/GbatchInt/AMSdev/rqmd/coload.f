CPAT,COLOAD   .                                                         
      subroutine coload                                                 
        include 'amsrqmd.h'
        include 'scatter7'                                              
        include 'partblk7'                                              
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      logical hit                                                       
      data vernum/'version   rqmd 2.2 -- 28-oct-95'/                    
      data pronam /'coload -- collision array rebuild'/                 
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(lun8,*)'---------------- version number report -------------'
        write(lun8,*)pronam                                                
        write(lun8,*)vernum                                                
        write(lun8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      iorig=0                                                           
      if(.not.switch(23).and.ida(57).eq.1) iorig=1                      
      ntotal=nrbar(kclst)+nrmes(kclst)                                  
      if(ida(11).eq.1) ntotal=nrbar(kclst)                              
      nument=0                                                          
      if(ida(34).eq.2) then                                             
        do 20 i1=iofset+1,iofset+ntotal                                 
         prop(i1)=.false.                                               
   20   continue                                                        
      end if                                                            
      do 21 i1e=1,ntotal                                                
        i1=i1e+iofset                                                   
        if(ida(34).eq.2) then                                           
            ident1=issn(i1)                                             
            ncoli1=nclcnt(ident1)                                       
            if(prop(i1).or.                                             
     a         (ident1.le.nt.and.ident1.gt.n1.and.ncoli1.eq.0)) then    
            else                                                        
               iclyes=0                                                 
               call hit34(iorig,i1,i2,sig,thit,iclyes)                  
               if(iclyes.eq.1) then                                     
                   prop(i1)=.true.                                      
                   prop(i2)=.true.                                      
                   nument=nument+1                                      
                   if (nument.gt.numema)                                
     a                call errex( 'coload, collision storage exceeded') 
                   i1c(nument)=i1                                       
                   i2c(nument)=i2                                       
                   ctime(nument)=thit                                   
                   csig(nument)=sig                                     
               else                                                     
                 do 12 i2e=i1e+1,ntotal                                 
                   i2=i2e+iofset                                        
                   ident2=issn(i2)                                      
                   ncoli2=nclcnt(ident2)                                
                   if(prop(i2).or.                                      
     a                 (ident2.le.nt.and.ident2.gt.n1.and.ncoli2.eq.0)) 
     b                                                         go to 12 
                   if(hit(iorig,i1,i2,srt,sig,sigel,thit)) then         
                       nument=nument+1                                  
                       if (nument.gt.numema)                            
     a                call errex( 'coload, collision storage exceeded') 
                       i1c(nument)=i1                                   
                       i2c(nument)=i2                                   
                       ctime(nument)=thit                               
                       csig(nument)=sig                                 
                       prop(i1)=.true.                                  
                       prop(i2)=.true.                                  
                       go to 13                                         
                   end if                                               
   12            continue                                               
               end if                                                   
   13          continue                                                 
            end if                                                      
        else                                                            
           do 22 i2e=i1e+1,ntotal                                       
            i2=i2e+iofset                                               
              if(hit(iorig,i1,i2,srt,sig,sigel,thit)) then              
                nument=nument+1                                         
                if (nument.gt.numema)                                   
     a                call errex('coload, collision storage exceeded')  
                i1c(nument)=i1                                          
                i2c(nument)=i2                                          
                idud(1,issn(i1))=idud(1,issn(i1))+1                     
                idud(1,issn(i2))=idud(1,issn(i2))+1                     
                ctime(nument)=thit                                      
                csig(nument)=sig                                        
              end if                                                    
   22      continue                                                     
        end if                                                          
   21 continue                                                          
      return                                                            
      end                                                               
