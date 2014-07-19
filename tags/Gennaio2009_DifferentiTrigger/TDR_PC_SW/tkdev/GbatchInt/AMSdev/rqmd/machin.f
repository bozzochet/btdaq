CPAT,MACHIN   .                                                         
      function ranf(x)                                                  
      common /ranfbk/iy                                                 
c      ranf=ran0(iy)                                                     
       ranf=rndm(x)
      return                                                            
      end                                                               
      subroutine seed(iseed)                                            
      common /ranfbk/iy                                                 
      if(iseed.le.-99) then                                             
         mst=mstime(dum)                                                
         iy=mst+int(1000.*secnds(0.))+345672197                         
         write(8,*)'seed changed into ',iy                              
      else                                                              
         iy=iseed                                                       
         write(8,*)'starting seed is',iy                                
      end if                                                            
      return                                                            
      end                                                               
      function ran0(idum)                                               
      integer idum,ia,im,iq,ir                                          
      real ran0, am                                                     
      parameter (ia=16807,im=2147483647,am=1./im,                       
     &    iq=127773,ir=2836)                                            
      integer k                                                         
      k = idum / iq                                                     
      idum = ia * (idum - k * iq) - ir * k                              
      if (idum .lt. 0) idum = idum + im                                 
      ran0 = am * idum                                                  
      ran0=rndm(dd)
      return                                                            
      end                                                               
      function systim(start,dum)                                        
      call timex(start)                                                 
      systim=0.0                                                        
      return                                                            
      end                                                               
      subroutine getinp                                                 
        include 'scatter7'                                              
        include 'partblk7'                                              
        integet init/0/
        integer file
        character *256 filename 
        include 'amsrqmd.h'
c      namelist/input/ header, n1,iz1,n2,iz2,ndt,nprint,nruns,ref,elgev, 
c     a  inttyp,ieos,b,dt,sepfp,sepft,switch,cutfct,iseed,               
c     b  idtcl                                                           
       if(init.ne.0)return
       init=1
       if(file(lun7).gt.0)then
         filename='file7'
       else
         filename='/dev/null'
       endif
       if(file(lun7.ne.0)then
          open(unit=lun7,form='unformatted',file=filename,
     a    status='unknown',access='sequential')                               
       endif

       if(file(lun8).gt.0)then
         filename='file8'
       else
         filename='/dev/null'
       endif
       if(file(lun8.ne.0)then
          open(unit=lun8,form='formatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun9).gt.0)then
         filename='file9'
       else
         filename='/dev/null'
       endif
       if(file(lun9.ne.0)then
          open(unit=lun9,form='formatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun10).gt.0)then
         filename='file10'
       else
         filename='/dev/null'
       endif
       if(file(lun10.ne.0)then
          open(unit=lun10,form='formatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun11).gt.0)then
         filename='file11'
       else
         filename='/dev/null'
       endif
       if(file(lun11.ne.0)then
          open(unit=lun11,form='unformatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun12).gt.0)then
         filename='file12'
       else
         filename='/dev/null'
       endif
       if(file(lun12.ne.0)then
          open(unit=lun12,form='unformatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun13).gt.0)then
         filename='file13'
       else
         filename='/dev/null'
       endif
       if(file(lun13.ne.0)then
          open(unit=lun13,form='formatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun14).gt.0)then
         filename='file14'
       else
         filename='/dev/null'
       endif
       if(file(lun14.ne.0)then
          open(unit=lun14,form='formatted',file=filename,
     a    status='unknown')                               
       endif

       if(file(lun15).gt.0)then
         filename='file15'
       else
         filename='/dev/null'
       endif
       if(file(lun15.ne.0)then
          open(unit=lun15,form='formatted',file=filename,
     a    status='unknown')                               
       endif

      return                                                            
      end                                                               
      integer function file(lun)
       if(lun.eq.6)then
        file=0
       else if(lun.gt.0)then
        file=1
       else
        file=-1
        lun=-lun
       endif
      end
        include 'amsrqmd.h'
      end
       function jrtime(idummy)                                          
        jrtime=999999.                                                  
       return                                                           
       end                                                              
      function mstime(dummy)                                            
      character*8 id,it                                                 
      character*1 itb(8)                                                
      integer tb(8),th,tm,ts                                            
      equivalence (it,itb(1))                                           
      call datimh(id,it)                                                
      do 100 i=1,8                                                      
      tb(i)=ichar(itb(i))-48                                            
 100  continue                                                          
      th=tb(1)*36000+tb(2)*3600                                         
      tm=tb(4)*600+tb(5)*60                                             
      ts=tb(7)*10+tb(8)                                                 
      mstime=th+tm+ts                                                   
      return                                                            
      end                                                               
