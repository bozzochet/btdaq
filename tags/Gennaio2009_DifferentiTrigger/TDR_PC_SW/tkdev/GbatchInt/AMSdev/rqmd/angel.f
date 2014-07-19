CPAT,ANGEL    .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine angel(sig,emm,s,pr,icltyp,t1,c1)                       
        include 'amsrqmd.h'
        include 'scatter7'                                              
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'ver rqmd 1.07 ---- 29-apr-91'/                       
      data pronam/'module: a n g e l'/                                  
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
      if(icltyp.le.66) then                                             
        goto (                                                          
     >   1,  3,  1,  2,  2,                                             
     >   1,  2,  2,  2,  1,                                             
     >   2,  2,  2,  2,  1,                                             
     >   2,  2,  2,  2,  2,                                             
     >   1,  2,  2,  2,  2,                                             
     >   2,  2,  1,  2,  2,                                             
     >   2,  2,  2,  2,  2,                                             
     >   1,  2,  2,  2,  2,                                             
     >   2,  2,  2,  2,  1,                                             
     >   2,  2,  2,  2,  2,                                             
     >   2,  2,  2,  2,  2,                                             
     >   2,  2,  2,  2,  2,                                             
     >   2,  2,  2,  2,  2,                                             
     >   1                                                              
     & ) , icltyp                                                       
       end if                                                           
       if((icltyp.ge.67).and.(icltyp.le.78)) then                       
            if(icltyp.eq.78) go to 1                                    
            go to 2                                                     
       end if                                                           
       if((icltyp.ge.79).and.(icltyp.le.120)) then                      
            go to 2                                                     
       end if                                                           
       if((icltyp.ge.4754).and.(icltyp.le.4950)) then                   
            go to 2                                                     
       end if                                                           
      write(lun8,*)' icltyp invalid in  angel   ',icltyp                   
      call errex( 'fatal error')                                        
    1 factor=1.                                                         
      goto 101                                                          
    3 continue                                                          
      call angnp(iang,s,c1,t1)                                          
      if(iang.eq.1) return                                              
    2 factor=2.                                                         
  101 continue                                                          
      a=slopet(icltyp,pr,s,emm,sig)                                     
      ta=2.*pr*pr                                                       
      ata=ta*a                                                          
      if(ata.gt.35.) then                                               
           iloop=0                                                      
   13      zrand=ranf(0)                                                
           c1=1.+ 2./(ata) * log(max(cvalu2,zrand))                     
           if(c1.lt.0.) then                                            
              iloop=iloop+1                                             
              if(iloop.lt.100) then                                     
                 go to 13                                               
              else                                                      
                 call errex('no cosine theta found in angel')           
              end if                                                    
           else                                                         
              c1= sqrt(c1)                                              
           end if                                                       
      else if(ata.lt.1.e-7) then                                        
           c1=1.                                                        
      else                                                              
           y3=ranf(0)                                                   
           pt=log((1.-y3)*exp(-factor*ata)+y3)                          
           c1=1.+pt/ata                                                 
      end if                                                            
      if (abs(c1) .gt. 1.0) c1=sign(cvalu1,c1)                          
      t1=pi2*ranf(0)                                                    
      return                                                            
   10 c1=1.-2.*ranf(0)                                                  
      t1=pi2*ranf(0)                                                    
      return                                                            
      end                                                               
      subroutine angnp(iang,srt,c1,t1)                                  
        include 'amsrqmd.h'
      include 'scatter7'                                                
      data  npotnp,paranp,parcnp,tkmin / 4, 0.02, 0.04, 1.0/            
       iang=0                                                           
       tkin=(srt**2-4.*emnuc**2)/(2.*emnuc)                             
       slimit=(tkmin/max(tkin,tkmin))**2                                
       if(ranf(0).gt.slimit) return                                     
       iang=1                                                           
       if(tkin.le.paranp) then                                          
         c1=1.-2.*ranf(0)                                               
         t1=pi2*ranf(0)                                                 
         return                                                         
       end if                                                           
       vf=1./((parcnp/max(tkin,parcnp)+                                 
     a                   (1.-paranp/max(tkin,paranp))/(npotnp+1.)))     
       apr=(1.-paranp/max(tkin,paranp))/(npotnp+1.)                     
       bpr=parcnp/max(tkin,parcnp)/apr                                  
       zrand=1.-2.*ranf(0)                                              
       vrand=sign(cvalu1,zrand)                                         
       zrand=abs(zrand)                                                 
       sl=zrand/vf/apr                                                  
       xstor=0.                                                         
       sg=1.                                                            
       do 20 ilop=1,25                                                  
         dekr=1./(2)**ilop                                              
         xstor=xstor+sg*dekr                                            
         sg=sign(cvalu1,sl-(xstor**5+bpr*xstor))                        
   20  continue                                                         
       slpr=xstor**5+bpr*xstor                                          
       dlt=(sl-slpr)/(bpr+(npotnp+1.)*xstor**npotnp)                    
       xstor=xstor+dlt                                                  
       if(xstor.lt.0.or.xstor.gt.1.) then                               
          write(lun8,*) 'warning: wrong xstor in angnp', xstor,zrand       
          xstor=max(cvalu0,min(cvalu1,xstor))                           
       end if                                                           
       c1=xstor*vrand                                                   
       t1=pi2*ranf(0)                                                   
      return                                                            
      end                                                               
      function slopet(icltyp,pr,s,emm,sig)                              
        include 'amsrqmd.h'
      include 'scatter7'                                                
      if(icltyp.le.3) then                                              
         rshhpp=1.                                                      
      else                                                              
         rshhpp=sig/40.                                                 
      end if                                                            
      if(pr.lt.0.95) then                                               
        as=(3.65*(s- emm))**6                                           
        a=6.*as/(1.+as)                                                 
      else                                                              
        ssq=4.*(emnuc**2+pr**2)                                         
        plab=sqrt(ssq*(ssq-4.*emnuc**2))/(2.*emnuc)                     
        a=9.2+0.27*log(plab)-5.7/sqrt(plab)                             
      end if                                                            
      slopet=a*rshhpp                                                   
      return                                                            
      end                                                               
