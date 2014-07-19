CPAT,MESMES   .                                                         
      subroutine mesmes(jesant,                                         
     a    srt,prel,xsect,em1,em2,ic,                                    
     b     kmulo1,ltypo1,kmulo2,ltypo2,kmuln1,ltypn1,kmuln2,ltypn2,     
     c                                                   iswave,iswi)   
       include 'scatter7'                                               
       real*4 s,pr,xsppv                                                
       if(kmulo1.ne.0.or.kmulo2.ne.0) then                              
         if(iswi.eq.0) then                                             
           xsect=0.0                                                    
         else                                                           
           call errex('mesmes should not be called')                    
         end if                                                         
         return                                                         
       end if                                                           
       s=srt                                                            
       pr=prel                                                          
       kf1=ltypo1                                                       
       kf2=ltypo2                                                       
       call luiflv(kf1,ifla1,iflb1,iflc1,ksp)                           
       call luiflv(kf2,ifla2,iflb2,iflc2,ksp)                           
       istr1=0                                                          
       if(iabs(ifla1).eq.3) istr1=isign(1,ifla1)                        
       if(iabs(iflb1).eq.3) istr1=istr1+isign(1,iflb1)                  
       istr2=0                                                          
       if(iabs(ifla2).eq.3) istr2=isign(1,ifla2)                        
       if(iabs(iflb2).eq.3) istr2=istr2+isign(1,iflb2)                  
       istr=istr1+istr2                                                 
       if(istr.eq.1) then                                               
         iqcn=1                                                         
         kf1=kfcrev(kf1)                                                
         kf2=kfcrev(kf2)                                                
         istr=-1                                                        
       else                                                             
         iqcn=0                                                         
       end if                                                           
       call pspsv(s,pr,xsppv,istr,kf1,kf2,iswave,0)                     
       xsect=xsppv                                                      
       if(iswi.eq.1) then                                               
         itry=0                                                         
   10    continue                                                       
         if(xsect.eq.0.0) then                                          
           ic=0                                                         
           return                                                       
         else                                                           
           call pspsv(s,pr,xsppv,istr,kf1,kf2,iswave,iswi)              
         end if                                                         
         if(kf2.eq.0) then                                              
           em1=srt                                                      
           em2=0.0                                                      
           ic=-2                                                        
         else                                                           
           em1=getmas(0,kf1,1,-1.0)                                     
           em2=getmas(0,kf2,1,-1.0)                                     
         end if                                                         
         if(kf2.ne.0.and.srt-ekinmi.lt.em1+em2) then                    
           itry=itry+1                                                  
           if(itry.lt.10) then                                          
             go to 10                                                   
           else                                                         
             ic=0                                                       
             return                                                     
           end if                                                       
         end if                                                         
         if(ranf(0).gt.0.5) then                                        
           call swap(em1,em2)                                           
           call iswap(kf1,kf2)                                          
         end if                                                         
         kf1a=iabs(kf1)                                                 
         kf2a=iabs(kf2)                                                 
         if(kf1a/100.eq.60) then                                        
           iyess1=1                                                     
           jesant=1                                                     
           kmuln1=-1                                                    
         else                                                           
           iyess1=0                                                     
           kmuln1=kf1a/100                                              
         end if                                                         
         if(kf2a/100.eq.60) then                                        
           iyess2=1                                                     
           jesant=1                                                     
           kmuln2=-1                                                    
         else                                                           
           iyess2=0                                                     
           kmuln2=kf2a/100                                              
         end if                                                         
         kf1=isign(1,kf1)*mod(kf1a,100)                                 
         kf2=isign(1,kf2)*mod(kf2a,100)                                 
         if(iqcn.eq.1) then                                             
           ltypn1=kfcrev(kf1)                                           
           ltypn2=kfcrev(kf2)                                           
         else                                                           
           ltypn1=kf1                                                   
           ltypn2=kf2                                                   
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      subroutine pspsv(srt,pin,xsect,istr,itp1,itp2,iswave,iswi)        
      common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)      
       parameter (maxcha=20,iclass=10)                                  
       dimension sigcha(maxcha),ktpout(2,maxcha)                        
       logical isotro(maxcha)                                           
       logical fndwq                                                    
       logical empiri                                                   
       dimension pjin(4),sigiso(0:2)                                    
       common /pjbk/ pjin                                               
       dimension fndwq(0:iclass)                                        
       data  thetav,thetas  / 35.3 , -10.1 /                            
       dimension xsmmcl(9)                                              
       data xsmmcl/                                                     
     a  9.150,15.632,16.386,14.408,12.476,20.837, 0.0,4.342,1.696       
     b           /                                                      
       data  icall /1/                                                  
       data  empiri  / .false. /                                        
       logical new                                                      
       data  new  / .false. /                                           
       do 10 i=0,iclass                                                 
   10  fndwq(i)=.false.                                                 
       xsect=0.0                                                        
       ktp1=0                                                           
       ktp2=0                                                           
       if(itp1.eq.17) then                                              
         ktp1=1                                                         
         i31=2                                                          
         i1=2                                                           
         igp1=-1                                                        
       else if(itp1.eq.-17) then                                        
         ktp1=1                                                         
         i31=-2                                                         
         i1=2                                                           
         igp1=-1                                                        
       else if(itp1.eq.23) then                                         
         ktp1=1                                                         
         i31=0                                                          
         i1=2                                                           
         igp1=-1                                                        
       else if(itp1.eq.18) then                                         
         ktp1=2                                                         
         i31=1                                                          
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.-18) then                                        
         ktp1=3                                                         
         i31=-1                                                         
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.19) then                                         
         ktp1=2                                                         
         i31=-1                                                         
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.-19) then                                        
         ktp1=3                                                         
         i31=1                                                          
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.24) then                                         
         ktp1=4                                                         
         i31=0                                                          
         i1=0                                                           
         igp1=1                                                         
       else if(itp1.eq.27) then                                         
         ktp1=5                                                         
         i31=2                                                          
         i1=2                                                           
         igp1=1                                                         
       else if(itp1.eq.-27) then                                        
         ktp1=5                                                         
         i31=-2                                                         
         i1=2                                                           
         igp1=1                                                         
       else if(itp1.eq.33) then                                         
         ktp1=5                                                         
         i31=0                                                          
         i1=2                                                           
         igp1=1                                                         
       else if(itp1.eq.28) then                                         
         ktp1=6                                                         
         i31=1                                                          
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.-28) then                                        
         ktp1=7                                                         
         i31=-1                                                         
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.29) then                                         
         ktp1=6                                                         
         i31=-1                                                         
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.-29) then                                        
         ktp1=7                                                         
         i31=1                                                          
         i1=1                                                           
         igp1=0                                                         
       else if(itp1.eq.34) then                                         
         ktp1=8                                                         
         i31=0                                                          
         i1=0                                                           
         igp1=-1                                                        
       else if(itp1.eq.35) then                                         
         ktp1=9                                                         
         i31=0                                                          
         i1=0                                                           
         igp1=-1                                                        
       end if                                                           
       if(itp2.eq.17) then                                              
         ktp2=1                                                         
         i32=2                                                          
         i2=2                                                           
         igp2=-1                                                        
       else if(itp2.eq.-17) then                                        
         ktp2=1                                                         
         i32=-2                                                         
         i2=2                                                           
         igp2=-1                                                        
       else if(itp2.eq.23) then                                         
         ktp2=1                                                         
         i32=0                                                          
         i2=2                                                           
         igp2=-1                                                        
       else if(itp2.eq.18) then                                         
         ktp2=2                                                         
         i32=1                                                          
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.-18) then                                        
         ktp2=3                                                         
         i32=-1                                                         
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.19) then                                         
         ktp2=2                                                         
         i32=-1                                                         
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.-19) then                                        
         ktp2=3                                                         
         i32=1                                                          
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.24) then                                         
         ktp2=4                                                         
         i32=0                                                          
         i2=0                                                           
         igp2=1                                                         
       else if(itp2.eq.27) then                                         
         ktp2=5                                                         
         i32=2                                                          
         i2=2                                                           
         igp2=1                                                         
       else if(itp2.eq.-27) then                                        
         ktp2=5                                                         
         i32=-2                                                         
         i2=2                                                           
         igp2=1                                                         
       else if(itp2.eq.33) then                                         
         ktp2=5                                                         
         i32=0                                                          
         i2=2                                                           
         igp2=1                                                         
       else if(itp2.eq.28) then                                         
         ktp2=6                                                         
         i32=1                                                          
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.-28) then                                        
         ktp2=7                                                         
         i32=-1                                                         
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.29) then                                         
         ktp2=6                                                         
         i32=-1                                                         
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.-29) then                                        
         ktp2=7                                                         
         i32=1                                                          
         i2=1                                                           
         igp2=0                                                         
       else if(itp2.eq.34) then                                         
         ktp2=8                                                         
         i32=0                                                          
         i2=0                                                           
         igp2=-1                                                        
       else if(itp2.eq.35) then                                         
         ktp2=9                                                         
         i32=0                                                          
         i2=0                                                           
         igp2=-1                                                        
       end if                                                           
       if(iswi.eq.0) then                                               
         if(ktp1.eq.0.or.ktp2.eq.0) then                                
           return                                                       
         end if                                                         
       else                                                             
         if(ktp1.eq.0.or.ktp2.eq.0) then                                
           return                                                       
         end if                                                         
       end if                                                           
       iii=max(ktp1,ktp2)                                               
       jjj=min(ktp1,ktp2)                                               
       iclcod=iii*(iii-1)/2+jjj                                         
       if(iclcod.eq.1) then                                             
         fndwq(0)=.true.                                                
         fndwq(1)=.true.                                                
         fndwq(4)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.2) then                                        
         fndwq(2)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.3) then                                        
       else if(iclcod.eq.4) then                                        
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.5) then                                        
         fndwq(1)=.true.                                                
         fndwq(3)=.true.                                                
         fndwq(6)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.6) then                                        
       else if(iclcod.eq.7) then                                        
         fndwq(3)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.8) then                                        
         fndwq(5)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.9) then                                        
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.10) then                                       
         fndwq(9)=.true.                                                
       else if(iclcod.eq.11) then                                       
         fndwq(1)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.12) then                                       
         fndwq(2)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.13) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.14) then                                       
         fndwq(3)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.15) then                                       
         fndwq(1)=.true.                                                
         fndwq(4)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.16) then                                       
         fndwq(9)=.true.                                                
       else if(iclcod.eq.17) then                                       
       else if(iclcod.eq.18) then                                       
         fndwq(1)=.true.                                                
         fndwq(3)=.true.                                                
         fndwq(6)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.19) then                                       
         fndwq(9)=.true.                                                
       else if(iclcod.eq.20) then                                       
         fndwq(10)=.true.                                               
       else if(iclcod.eq.21) then                                       
       else if(iclcod.eq.22) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.23) then                                       
         fndwq(1)=.true.                                                
         fndwq(3)=.true.                                                
         fndwq(6)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.24) then                                       
       else if(iclcod.eq.25) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.26) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.27) then                                       
         fndwq(1)=.true.                                                
         fndwq(3)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.28) then                                       
       else if(iclcod.eq.29) then                                       
         fndwq(3)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.30) then                                       
         fndwq(2)=.true.                                                
         fndwq(5)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.31) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.32) then                                       
         fndwq(6)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.33) then                                       
         fndwq(3)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.34) then                                       
         fndwq(2)=.true.                                                
         fndwq(5)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.35) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.36) then                                       
         fndwq(6)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.37) then                                       
         fndwq(9)=.true.                                                
       else if(iclcod.eq.38) then                                       
         fndwq(2)=.true.                                                
         fndwq(5)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.39) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.40) then                                       
         fndwq(6)=.true.                                                
         fndwq(9)=.true.                                                
       else if(iclcod.eq.41) then                                       
         fndwq(3)=.true.                                                
       else if(iclcod.eq.42) then                                       
         fndwq(2)=.true.                                                
         fndwq(5)=.true.                                                
         fndwq(10)=.true.                                               
       else if(iclcod.eq.43) then                                       
         call errex('pspsv:s=-1 ingoing')                               
       else if(iclcod.eq.44) then                                       
         fndwq(6)=.true.                                                
       else if(iclcod.eq.45) then                                       
         fndwq(6)=.true.                                                
         fndwq(10)=.true.                                               
       end if                                                           
       kchact=0                                                         
       if(fndwq(0)) then                                                
         if(srt.lt.0.99) then                                           
           srtact=srt                                                   
         else                                                           
           srtact=0.99                                                  
         end if                                                         
           zmltp=80.*3.1415927/(5.0674**2)                              
           sigiso(0)=0.0                                                
           sigiso(1)=0.0                                                
           delt02=-0.12*pin/pmas(23)                                    
           sigiso(2)=zmltp/(pin*pin)*(sin(delt02))**2                   
           if(srt.gt.0.99) then                                         
             sigiso(2)=sigiso(2)*(0.99/srt)                             
           end if                                                       
         itag=-100                                                      
         call isocgk(i31,i1,i32,i2,2,2,i31new,i32new,itag)              
         m=i31+i32                                                      
         iabm=iabs(m)                                                   
         iff=0                                                          
         pjsum=0.                                                       
         do 95 loop=iabm,4,2                                            
          iff=iff+1                                                     
          jtrue=loop/2                                                  
          pjin(iff)=pjin(iff)* sigiso(jtrue)                            
          sigiso(jtrue)=pjin(iff)                                       
          pjsum=pjsum+pjin(iff)                                         
          sig=sigiso(jtrue)                                             
          xsect=xsect+sig                                               
          if(iswi.eq.1) then                                            
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               itag=-50                                                 
               jtot=loop                                                
               call isonew(jtot,m,2,2,ia,ib,itag)                       
               ktpout(1,kchact)=ia/2*17+23*(2-iabs(ia))/2               
               ktpout(2,kchact)=ib/2*17+23*(2-iabs(ib))/2               
          end if                                                        
   95    continue                                                       
       end if                                                           
       if(fndwq(1)) then                                                
         if(mod(i31,2).eq.0.and.mod(i32,2).eq.0) then                   
           if(iclcod.eq.1) then                                         
             if(i31+i32.eq.0.and.empiri) then                           
               jfine=0                                                  
             else if(i31.eq.i32.or.iabs(i31+i32).eq.2) then             
               jfine=2                                                  
             else                                                       
               jfine=1                                                  
             end if                                                     
           else if(iclcod.eq.11) then                                   
             jfine=3                                                    
           else if(iclcod.eq.15) then                                   
             if(i31.eq.i32.or.iabs(i31+i32).eq.2) then                  
               jfine=5                                                  
             else                                                       
               jfine=4                                                  
             end if                                                     
           else                                                         
             call errex('pspsv:illegal iclcod  in block1')              
           end if                                                       
  100      continue                                                     
           sigred=4.*spspsv(1,jfine,srt)                                
           if(sigred.eq.0.0) go to 105                                  
           if(iclcod.eq.15) then                                        
             em1new=pmas(19)                                            
             em2new=em1new                                              
             if(srt.le.em1new+em2new) then                              
                pout=0.0                                                
             else                                                       
                pout=prcms(srt,em1new,em2new)                           
             end if                                                     
             sigred=0.11111*sigred*(pout/pin)**2                        
           end if                                                       
           do 101 l=-1,1,2                                              
            fiso=cgksq1(-l,i32+l)*cgksq1(-(i32+l+i31),(i32+l))          
            if(fiso.ne.0.) then                                         
              if(jfine.eq.0) fiso=0.25                                  
              sig=sigred*fiso                                           
              xsect=xsect+sig                                           
              if(iswi.eq.1) then                                        
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                j=(i32+l+i31)                                           
                ktpout(1,kchact)=17+(3-j)/2                             
                ktpout(2,kchact)=-17-(3-l)/2                            
                if(jfine.eq.3) then                                     
                  if(ktp1.eq.5) then                                    
                    ktpout(2,kchact)=ktpout(2,kchact)-10                
                  else                                                  
                    ktpout(1,kchact)=ktpout(1,kchact)+10                
                  end if                                                
                else if(jfine.eq.6.or.jfine.eq.7) then                  
                    ktpout(1,kchact)=ktpout(1,kchact)+10                
                    ktpout(2,kchact)=ktpout(2,kchact)-10                
                end if                                                  
              end if                                                    
            end if                                                      
  101      continue                                                     
           if((ktp1.ne.ktp2.or.i31.ne.i32).and.                         
     a         jfine.ne.2.and.jfine.ne.5.and.jfine.ne.7) then           
             do 102 j=-1,1,2                                            
              fiso=cgksq1(j,i32-j)*cgksq1(-(i32-j+i31),(i32-j))         
              if(fiso.ne.0.) then                                       
                if(jfine.eq.0) fiso=0.25                                
                sig=sigred*fiso                                         
                xsect=xsect+sig                                         
                if(iswi.eq.1) then                                      
                  kchact=min(kchact+1,maxcha)                           
                  sigcha(kchact)=sig                                    
                  l=-(i32-j+i31)                                        
                  ktpout(1,kchact)=17+(3-j)/2                           
                  ktpout(2,kchact)=-17-(3-l)/2                          
                  if(jfine.eq.3) then                                   
                    if(ktp1.eq.5) then                                  
                      ktpout(1,kchact)=ktpout(1,kchact)+10              
                    else                                                
                      ktpout(2,kchact)=ktpout(2,kchact)-10              
                    end if                                              
                  else if(jfine.eq.6.or.jfine.eq.7) then                
                    ktpout(1,kchact)=ktpout(1,kchact)+10                
                    ktpout(2,kchact)=ktpout(2,kchact)-10                
                  end if                                                
                end if                                                  
              end if                                                    
  102        continue                                                   
           end if                                                       
  105      continue                                                     
           if(jfine.le.2) then                                          
             if(jfine.eq.0) then                                        
               if(i31.eq.i32) then                                      
                 jfine=2                                                
               else                                                     
                 jfine=1                                                
               end if                                                   
             end if                                                     
             jfine=jfine+5                                              
             go to 100                                                  
           end if                                                       
         else if(mod(iabs(i31),2).eq.1.and.mod(iabs(i32),2).eq.1) then  
           do 103 i=-1,1,2                                              
            fiso=cgksq1(i31,i)*cgksq1(i32,-i)                           
            if(fiso.ne.0.) then                                         
              ib=i31+i                                                  
              ia=i32-i                                                  
              ktpo1=ib/2*17+23*(2-iabs(ib))/2                           
              ktpo2=ia/2*17+23*(2-iabs(ia))/2                           
              if(iclcod.eq.5) then                                      
                if(i31+i32.eq.0.and.empiri) then                        
                  fiso=0.25                                             
                  jfine=0                                               
                  if(ktpo1.eq.ktpo2) then                               
                    symm=0.5                                            
                  else                                                  
                    symm=1.0                                            
                  end if                                                
                else if(ktpo1.eq.ktpo2.or.iabs(i31+i32).eq.2) then      
                  jfine=2                                               
                  if(ktpo1.eq.ktpo2) then                               
                    symm=0.5                                            
                  else                                                  
                    symm=1.0                                            
                  end if                                                
                else                                                    
                  jfine=1                                               
                  symm=1.0                                              
                end if                                                  
                em1new=pmas(23)                                         
                em2new=em1new                                           
              else if(iclcod.eq.27) then                                
                if(ktpo1.eq.ktpo2.or.iabs(i31+i32).eq.2) then           
                  jfine=7                                               
                  symm=0.5*0.11111                                      
                else                                                    
                  jfine=6                                               
                  symm=0.11111                                          
                end if                                                  
                em1new=pmas(23)                                         
                em2new=em1new                                           
              else if(iclcod.eq.18.or.iclcod.eq.23) then                
                jfine=3                                                 
                symm=1.0                                                
                em1new=pmas(33)                                         
                em2new=pmas(23)                                         
              else                                                      
                call errex('pspsv103:illegal iclcod  in block1')        
              end if                                                    
              if(srt.le.em1new+em2new) then                             
                pout=0.0                                                
              else                                                      
                pout=prcms(srt,em1new,em2new)                           
              end if                                                    
              sigred=4.*spspsv(1,jfine,srt)*(pout/pin)**2*symm          
              sig=sigred*fiso                                           
              xsect=xsect+sig                                           
              if(iswi.eq.1) then                                        
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                ktpout(1,kchact)=ktpo1                                  
                ktpout(2,kchact)=ktpo2                                  
                if(jfine.eq.3) then                                     
                  if(ktp1.eq.6.or.ktp1.eq.7) then                       
                    ktpout(2,kchact)=isign(1,ktpo2)*(iabs(ktpo2)+10)    
                  else                                                  
                    ktpout(1,kchact)=isign(1,ktpo1)*(iabs(ktpo1)+10)    
                  end if                                                
                end if                                                  
              end if                                                    
              if(iclcod.eq.5) then                                      
                jfine=jfine+2                                           
                sig=4.*spspsv(1,jfine,srt)*symm                         
                xsect=xsect+sig                                         
                if(iswi.eq.1) then                                      
                  kchact=min(kchact+1,maxcha)                           
                  sigcha(kchact)=sig                                    
                  ktpout(1,kchact)=isign(1,ktpo1)*(iabs(ktpo1)+10)      
                  ktpout(2,kchact)=isign(1,ktpo2)*(iabs(ktpo2)+10)      
                end if                                                  
              end if                                                    
              if(iabs(i31+i32).eq.2.and.                                
     a            (iclcod.eq.5.or.iclcod.eq.27)) go to 104              
            end if                                                      
  103      continue                                                     
  104      continue                                                     
         else                                                           
           call errex('pspsv:illegal isospin entrance in block1')       
         end if                                                         
       end if                                                           
       if(fndwq(2)) then                                                
         if(iclcod.eq.2.or.iclcod.eq.4.or.                              
     a                 iclcod.eq.12.or.iclcod.eq.13) then               
            if(mod(iclcod,10).eq.2) then                                
              iqrev=0                                                   
            else                                                        
              iqrev=1                                                   
            end if                                                      
            if(ktp1.eq.1.or.ktp1.eq.5) then                             
              ia=i31*(-1)**iqrev                                        
              j=i32*(-1)**iqrev                                         
            else                                                        
              ia=i32*(-1)**iqrev                                        
              j=i31*(-1)**iqrev                                         
            end if                                                      
            fiso=cgksq1(ia+j,-j)                                        
            if(fiso.ne.0.) then                                         
              if(iclcod.ge.12) then                                     
                jfine=3                                                 
              else                                                      
                jfine=1                                                 
              end if                                                    
              sig=2.*fiso*                                              
     a               spspsv(2,jfine,srt)*sin(thetav/360.0*6.283154)**2  
              xsect=xsect+sig                                           
              if(iswi.eq.1) then                                        
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                i3out=ia+j                                              
                ktpo1=17+(3-i3out)/2                                    
                if(iclcod.lt.12) then                                   
                  ktpo1=ktpo1+10                                        
                end if                                                  
                ktpout(1,kchact)=(-1)**iqrev*ktpo1                      
                ktpout(2,kchact)=34                                     
              end if                                                    
              jfine=jfine+1                                             
              sig=2.*fiso*                                              
     a               spspsv(2,jfine,srt)*cos(thetav/360.0*6.283154)**2  
              xsect=xsect+sig                                           
              if(iswi.eq.1) then                                        
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                ktpout(1,kchact)=ktpout(1,kchact-1)                     
                ktpout(2,kchact)=35                                     
              end if                                                    
            end if                                                      
         else                                                           
            if(ktp1.eq.8.or.ktp1.eq.9) then                             
              ikaon=ktp2                                                
              iw8=itp1                                                  
              if(ktp2.eq.2.or.ktp2.eq.6) then                           
                iqrev=0                                                 
              else if(ktp2.eq.3.or.ktp2.eq.7) then                      
                iqrev=1                                                 
              else                                                      
                call errex('pspsv:missing k in omega/k channel')        
              end if                                                    
            else if(ktp2.eq.8.or.ktp2.eq.9) then                        
              iw8=itp2                                                  
              ikaon=ktp1                                                
              if(ktp1.eq.2.or.ktp1.eq.6) then                           
                iqrev=0                                                 
              else if(ktp1.eq.3.or.ktp1.eq.7) then                      
                iqrev=1                                                 
              else                                                      
                call errex('pspsv:missing k in omega/k channel')        
              end if                                                    
            else                                                        
              call errex('pspsv:missing omega/phi in omega/k channel')  
            end if                                                      
            l=(-1)**iqrev*(i31+i32)                                     
            do 201 j=-1,1,2                                             
             fiso=cgksq1(j,l)                                           
             if(fiso.ne.0.) then                                        
               if(ikaon.ge.6) then                                      
                jfine=1                                                 
                em2new=pmas(23)                                         
                spf=0.1111                                              
               else                                                     
                jfine=3                                                 
                em2new=pmas(33)                                         
                spf=1.0                                                 
               end if                                                   
               if(iw8.eq.35) then                                       
                fthet=cos(thetav/360.0*6.283154)**2                     
                jfine=jfine+1                                           
               else                                                     
                fthet=sin(thetav/360.0*6.283154)**2                     
               end if                                                   
               em1new=pmas(19)                                          
               if(srt.le.em1new+em2new) then                            
                 pout=0.0                                               
               else                                                     
                 pout=prcms(srt,em1new,em2new)                          
               end if                                                   
               sig=2.*fiso*spspsv(2,jfine,srt)*fthet*(pout/pin)**2*spf  
               xsect=xsect+sig                                          
               if(iswi.eq.1) then                                       
                 kchact=min(kchact+1,maxcha)                            
                 sigcha(kchact)=sig                                     
                 i3out=l+j                                              
                 ktpo1=17+(3+j)/2                                       
                 ktpo2=(-1)**iqrev*(i3out/2*17)+23*(2-iabs(i3out))/2    
                 if(ikaon.le.3) then                                    
                   ktpo2=isign(1,ktpo2)*(iabs(ktpo2)+10)                
                 end if                                                 
                 ktpout(1,kchact)=(-1)**iqrev*ktpo1                     
                 ktpout(2,kchact)=ktpo2                                 
               end if                                                   
             end if                                                     
  201       continue                                                    
         end if                                                         
       end if                                                           
       if(fndwq(3)) then                                                
         if(i31.eq.0.or.i32.eq.0) then                                  
           if(iclcod.eq.7) then                                         
             proj8=cos(thetas/360.0*6.283154)**2                        
             jfine=1                                                    
           else if(iclcod.eq.14) then                                   
             proj8=cos(thetas/360.0*6.283154)**2                        
             jfine=3                                                    
           else if(iclcod.eq.29) then                                   
             proj8=sin(thetav/360.0*6.283154)**2                        
             jfine=4                                                    
           else if(iclcod.eq.33) then                                   
             proj8=sin(thetav/360.0*6.283154)**2                        
             jfine=6                                                    
           else if(iclcod.eq.41) then                                   
             proj8=cos(thetav/360.0*6.283154)**2                        
             jfine=7                                                    
           else                                                         
             call errex('pspsv:illegal iclcod  in block1')              
           end if                                                       
           ia=i31+i32                                                   
           sigrpi=2.*spspsv(3,2,srt)*proj8                              
           sigred=2.*spspsv(3,jfine,srt)*proj8                          
           if(iclcod.eq.33.or.iclcod.eq.41) then                        
             em1new=pmas(19)                                            
             em2new=em1new                                              
             if(srt.le.em1new+em2new) then                              
                pout=0.0                                                
             else                                                       
                pout=prcms(srt,em1new,em2new)                           
             end if                                                     
             sigred=0.11111*sigred*(pout/pin)**2                        
           end if                                                       
           do 301 l=-1,1,2                                              
            fiso=cgksq1(l,ia-l)                                         
            if(fiso.ne.0.) then                                         
              sig=sigred*fiso                                           
              xsect=xsect+sig                                           
              if(iclcod.eq.7) then                                      
                sigp=sigrpi*fiso                                        
                xsect=xsect+sigp                                        
              end if                                                    
              if(iswi.eq.1) then                                        
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                ktpout(1,kchact)=17+(3-l)/2                             
                ktpout(2,kchact)=-17-(3+(ia-l))/2                       
                if(iclcod.eq.7) then                                    
                  kchact=min(kchact+1,maxcha)                           
                  sigcha(kchact)=sigp                                   
                  ktpout(1,kchact)=27+(3-l)/2                           
                  ktpout(2,kchact)=-27-(3+(ia-l))/2                     
                else if(iclcod.eq.33.or.iclcod.eq.41) then              
                else                                                    
                  if(jfine.eq.3) then                                   
                    ktpout(1,kchact)=ktpout(1,kchact)+10                
                  else if(jfine.eq.4) then                              
                    ktpout(2,kchact)=ktpout(2,kchact)-10                
                  end if                                                
                  xsect=xsect+sig                                       
                  kchact=min(kchact+1,maxcha)                           
                  sigcha(kchact)=sig                                    
                  ktpout(1,kchact)=-17-(3+l)/2                          
                  ktpout(2,kchact)=17+(3-(ia-l))/2                      
                  if(jfine.eq.3) then                                   
                    ktpout(1,kchact)=ktpout(1,kchact)-10                
                  else if(jfine.eq.4) then                              
                    ktpout(2,kchact)=ktpout(2,kchact)+10                
                  end if                                                
                end if                                                  
              end if                                                    
            end if                                                      
  301      continue                                                     
         else if(mod(iabs(i31),2).eq.1.and.mod(iabs(i32),2).eq.1) then  
           if(iclcod.eq.5) then                                         
             janf=5                                                     
             jend=7                                                     
           else if(iclcod.eq.18.or.iclcod.eq.23) then                   
             janf=2                                                     
             jend=3                                                     
           else if(iclcod.eq.27) then                                   
             janf=1                                                     
             jend=1                                                     
           else                                                         
             call errex('spspv:wrong iclcod in block3,detailed balance')
           end if                                                       
           do 303 j=janf,jend                                           
            jfine=1+mod(j,7)                                            
            sigred=2.*spspsv(3,jfine,srt)                               
            fiso=cgksq1(i31,i32)                                        
            if(fiso.ne.0.) then                                         
               i3out=i31+i32                                            
               if(jfine.eq.1) then                                      
                 em1new=pmas(24)                                        
                 em2new=pmas(23)                                        
                 spf=cos(thetas/360.0*6.283154)**2                      
                 ktpo1=24                                               
                 ktpo2=(i3out/2*17+23*(2-iabs(i3out))/2)                
               else if(jfine.eq.2) then                                 
                 em1new=pmas(24)                                        
                 em2new=pmas(23)                                        
                 spf=0.1111*cos(thetas/360.0*6.283154)**2               
                 ktpo1=24                                               
                 ktpo2=(i3out/2*17+23*(2-iabs(i3out))/2)                
               else if(jfine.eq.3) then                                 
                 em1new=pmas(24)                                        
                 em2new=pmas(33)                                        
                 spf=cos(thetas/360.0*6.283154)**2                      
                 ktpo1=24                                               
                 ktpo2=(i3out/2*27+33*(2-iabs(i3out))/2)                
               else if(jfine.eq.4) then                                 
                 em1new=pmas(34)                                        
                 em2new=pmas(23)                                        
                 spf=sin(thetav/360.0*6.283154)**2                      
                 ktpo1=34                                               
                 ktpo2=(i3out/2*17+23*(2-iabs(i3out))/2)                
               else if(jfine.eq.6) then                                 
                 em1new=pmas(34)                                        
                 em2new=pmas(33)                                        
                 spf=sin(thetav/360.0*6.283154)**2                      
                 ktpo1=34                                               
                 ktpo2=(i3out/2*27+33*(2-iabs(i3out))/2)                
               else if(jfine.eq.7) then                                 
                 em1new=pmas(35)                                        
                 em2new=pmas(33)                                        
                 spf=cos(thetav/360.0*6.283154)**2                      
                 ktpo1=35                                               
                 ktpo2=(i3out/2*27+33*(2-iabs(i3out))/2)                
               else                                                     
                 call errex('pspsv:wrong jfine in block3,det.balance')  
               end if                                                   
               if(srt.le.em1new+em2new) then                            
                 pout=0.0                                               
               else                                                     
                 pout=prcms(srt,em1new,em2new)                          
               end if                                                   
               if(jfine.eq.6.or.jfine.eq.7) then                        
                 sig=fiso*spf*sigred                                    
               else                                                     
                 sig=fiso*spf*sigred*(pout/pin)**2                      
               end if                                                   
               xsect=xsect+sig                                          
               if(iswi.eq.1) then                                       
                 kchact=min(kchact+1,maxcha)                            
                 sigcha(kchact)=sig                                     
                 ktpout(1,kchact)=ktpo1                                 
                 ktpout(2,kchact)=ktpo2                                 
               end if                                                   
            end if                                                      
  303      continue                                                     
         else                                                           
           call errex('pspsv:wrong input in block3')                    
         end if                                                         
       end if                                                           
       if(fndwq(4)) then                                                
       end if                                                           
       if(fndwq(5)) then                                                
         if(iclcod.eq.8.or.iclcod.eq.9.or.                              
     a                 iclcod.eq.30.or.iclcod.eq.31) then               
            if(iclcod.ge.30) then                                       
              jfine=1                                                   
              fthet=sin(thetav)**4                                      
            else                                                        
              jfine=3                                                   
              fthet=sin(thetav)**2*cos(thetas)**2                       
            end if                                                      
            sig=spspsv(5,jfine,srt)*fthet                               
            xsect=xsect+sig                                             
            if(iswi.eq.1) then                                          
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               if(ktp1.eq.2.or.ktp1.eq.3) then                          
                 ktpo1=itp1                                             
               else if(ktp2.eq.2.or.ktp2.eq.3) then                     
                 ktpo1=itp2                                             
               else                                                     
                 call errex('pspsv:missing k/kbar in fndwq5 channel')   
               end if                                                   
               if(iclcod.le.9) then                                     
                 ktpo1=isign(1,ktpo1)*(iabs(ktpo1)+10)                  
               end if                                                   
               ktpout(1,kchact)=ktpo1                                   
               ktpout(2,kchact)=34                                      
            end if                                                      
            jfine=jfine+1                                               
            if(iclcod.ge.30) then                                       
              fthet=cos(thetav)**2*sin(thetav)**2                       
            else                                                        
              fthet=cos(thetav)**2*cos(thetas)**2                       
            end if                                                      
            sig=spspsv(5,jfine,srt)*fthet                               
            xsect=xsect+sig                                             
            if(iswi.eq.1) then                                          
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               ktpout(1,kchact)=ktpout(1,kchact-1)                      
               ktpout(2,kchact)=35                                      
            end if                                                      
         else                                                           
            if(iclcod.le.35) then                                       
              jfine=3                                                   
              fthet=sin(thetav)**2*cos(thetas)**2                       
              spf=0.111111                                              
              em2new=pmas(24)                                           
            else if(iclcod.le.39) then                                  
              jfine=2                                                   
              fthet=cos(thetav)**2*sin(thetav)**2                       
              spf=1.                                                    
              em2new=pmas(34)                                           
            else                                                        
              jfine=4                                                   
              fthet=cos(thetav)**2*cos(thetas)**2                       
              spf=0.111111                                              
              em2new=pmas(24)                                           
            end if                                                      
            em1new=pmas(19)                                             
            if(srt.le.em1new+em2new) then                               
              pout=0.0                                                  
            else                                                        
              pout=prcms(srt,em1new,em2new)                             
            end if                                                      
            sig=spspsv(5,jfine,srt)*fthet*(pout/pin)**2*spf             
            xsect=xsect+sig                                             
            if(iswi.eq.1) then                                          
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               if(ktp1.eq.9.or.ktp1.eq.8) then                          
                 ktpo1=itp2                                             
               else if(ktp2.eq.9.or.ktp2.eq.8) then                     
                 ktpo1=itp1                                             
               else                                                     
                 call errex('pspsv:missing phi/ome in fndwq5 channel')  
               end if                                                   
               if(iclcod.ge.42.or.iclcod.le.35) then                    
                 ktpo1=isign(1,ktpo1)*(iabs(ktpo1)-10)                  
                 ktpo2=24                                               
               else                                                     
                 ktpo2=34                                               
               end if                                                   
               ktpout(1,kchact)=ktpo1                                   
               ktpout(2,kchact)=ktpo2                                   
            end if                                                      
         end if                                                         
       end if                                                           
       if(fndwq(6)) then                                                
         if(iclcod.eq.5.or.iclcod.eq.18.or.                             
     a                 iclcod.eq.23) then                               
           if(mod(itp1,10)+mod(itp2,10).eq.0) then                      
            if(iclcod.eq.5) then                                        
              jfine=1                                                   
              fthet=sin(thetav)**4                                      
            else                                                        
              jfine=4                                                   
              fthet=sin(thetav)**2*cos(thetas)**2                       
            end if                                                      
            sig=spspsv(6,jfine,srt)*fthet                               
            xsect=xsect+sig                                             
            if(iswi.eq.1.and.sig.ne.0.0) then                           
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               if(iclcod.eq.5) then                                     
                ktpo2=34                                                
               else                                                     
                ktpo2=24                                                
               end if                                                   
               ktpout(1,kchact)=34                                      
               ktpout(2,kchact)=ktpo2                                   
            end if                                                      
            if(iclcod.eq.5) then                                        
              fthet=sin(thetav)**2*cos(thetav)**2                       
            else                                                        
              fthet=cos(thetav)**2*cos(thetas)**2                       
            end if                                                      
            jfine=jfine+1                                               
            sig=spspsv(6,jfine,srt)*fthet                               
            if(iswi.eq.1.and.sig.ne.0.0) then                           
               kchact=min(kchact+1,maxcha)                              
               sigcha(kchact)=sig                                       
               ktpout(1,kchact)=35                                      
               ktpout(2,kchact)=ktpout(2,kchact-1)                      
            end if                                                      
            if(iclcod.eq.5) then                                        
              jfine=jfine+1                                             
              fthet=cos(thetav)**4                                      
              sig=spspsv(6,jfine,srt)*fthet                             
              if(iswi.eq.1.and.sig.ne.0.0) then                         
                kchact=min(kchact+1,maxcha)                             
                sigcha(kchact)=sig                                      
                ktpout(1,kchact)=35                                     
                ktpout(2,kchact)=35                                     
              end if                                                    
            end if                                                      
           end if                                                       
         else                                                           
            if(iclcod.eq.36) then                                       
              jfine=1                                                   
              fthet=sin(thetav)**4                                      
              spf=0.111111                                              
            else if(iclcod.eq.44) then                                  
              jfine=2                                                   
              fthet=sin(thetav)**2*cos(thetav)**2                       
              spf=0.111111                                              
            else if(iclcod.eq.45) then                                  
              jfine=3                                                   
              fthet=cos(thetav)**4                                      
              spf=0.111111                                              
            else if(iclcod.eq.32) then                                  
              jfine=4                                                   
              fthet=sin(thetav)**2*cos(thetas)**2                       
              spf=1.0                                                   
            else if(iclcod.eq.40) then                                  
              jfine=5                                                   
              fthet=cos(thetav)**2*cos(thetas)**2                       
              spf=1.0                                                   
            else                                                        
              call errex('pspsv:wrong iclcod in block6,det.balance')    
            end if                                                      
            em1new=pmas(19)                                             
            if(iclcod.eq.32.or.iclcod.eq.40) then                       
             em2new=pmas(29)                                            
            else                                                        
             em2new=pmas(19)                                            
            end if                                                      
            if(srt.le.em1new+em2new) then                               
              pout=0.0                                                  
            else                                                        
              pout=prcms(srt,em1new,em2new)                             
            end if                                                      
            sig=spspsv(6,jfine,srt)*fthet*(pout/pin)**2*spf             
            xsect=xsect+sig                                             
            if(iswi.eq.1) then                                          
             if(iclcod.eq.32.or.iclcod.eq.40) then                      
               ktpo1=28                                                 
             else                                                       
               ktpo1=18                                                 
             end if                                                     
             kchact=min(kchact+1,maxcha)                                
             sigcha(kchact)=sig                                         
             ktpout(1,kchact)=ktpo1                                     
             ktpout(2,kchact)=-18                                       
             kchact=min(kchact+1,maxcha)                                
             sigcha(kchact)=sig                                         
             ktpout(1,kchact)=ktpout(1,kchact-1)+1                      
             ktpout(2,kchact)=-19                                       
             if(iclcod.eq.32.or.iclcod.eq.40) then                      
              kchact=min(kchact+1,maxcha)                               
              sigcha(kchact)=sig                                        
              ktpout(1,kchact)=-28                                      
              ktpout(2,kchact)=18                                       
              kchact=min(kchact+1,maxcha)                               
              sigcha(kchact)=sig                                        
              ktpout(1,kchact)=-29                                      
              ktpout(2,kchact)=19                                       
             end if                                                     
            end if                                                      
         end if                                                         
       end if                                                           
       if(fndwq(7)) then                                                
       end if                                                           
       if(fndwq(8)) then                                                
       end if                                                           
       do  80 kch=1,kchact                                              
   80  isotro(kch)=.false.                                              
       if(fndwq(9)) then                                                
         if(iclcod.eq.5) then                                           
           if(iabs(itp1).eq.18) isobre=1                                
           if(iabs(itp1).eq.19) isobre=2                                
         else                                                           
           isobre=0                                                     
         end if                                                         
         call mesres(iswi,                                              
     a      identr,i1,i31,i2,i32,k1n,k2n,iclcod,isobre,istr,srt,resxs,  
     b                                                         reswav)  
         xsect=xsect+resxs                                              
         if(iswi.eq.1.and.k1n.ne.0) then                                
           kchact=min(kchact+1,maxcha)                                  
           sigcha(kchact)=resxs                                         
           if(ranf(0).lt.reswav/resxs) then                             
             isotro(kchact)=.true.                                      
           else                                                         
             isotro(kchact)=.false.                                     
           end if                                                       
           ktpout(1,kchact)=k1n                                         
           ktpout(2,kchact)=k2n                                         
         end if                                                         
       end if                                                           
       if((fndwq(9).or.fndwq(10)).and.srt.gt.1.8) then                  
         do 81 jt=1,9                                                   
          if(jt.eq.2.or.jt.eq.3.or.jt.eq.7) go to 81                    
          if(icall.eq.0) then                                           
            call fndxsm                                                 
            write(8,87)                                                 
            write(8,88) (xsmmcl(is),is=1,9)                             
            write(8,89)                                                 
   87       format(7x,'data xsmmcl/')                                   
   88       format(5x,'a ',8(f5.2,',',1x),f5.2)                         
   89       format(5x,'b           /')                                  
            icall=1                                                     
          end if                                                        
          call mmann(itp1,itp2,i1,i31,igp1,i2,i32,igp2,iswi,            
     a                                        jt,punish,kfres)          
          if(kfres.ne.0) then                                           
            jtt=jt                                                      
            if(jt.eq.1) then                                            
              prdef=prcms(srt,pmas(33),pmas(23))                        
              iclcot=11                                                 
            else if(jt.eq.4) then                                       
              if(i1.eq.0.and.i2.eq.0) then                              
                prdef=prcms(srt,pmas(24),pmas(24))                      
                iclcot=10                                               
              else if(i1.eq.2.and.i2.eq.2) then                         
                prdef=prcms(srt,pmas(23),pmas(23))                      
                jtt=jt-1                                                
                iclcot=1                                                
              else if(i1.eq.1.and.i2.eq.1) then                         
                prdef=prcms(srt,pmas(18),pmas(18))                      
                jtt=jt-2                                                
                iclcot=5                                                
              end if                                                    
            else if(jt.eq.5) then                                       
              prdef=prcms(srt,pmas(23),pmas(23))                        
              iclcot=1                                                  
            else if(jt.eq.6) then                                       
              prdef=prcms(srt,pmas(18),pmas(23))                        
              iclcot=2                                                  
            else if(jt.eq.8) then                                       
              prdef=prcms(srt,pmas(33),pmas(23))                        
              iclcot=11                                                 
            else if(jt.eq.9) then                                       
              prdef=prcms(srt,pmas(18),pmas(18))                        
              iclcot=5                                                  
            end if                                                      
            call mesres(-1,jt,i1,i31,i2,i32,                            
     a                  k1n,k2n,iclcot,isobre,istr,srt,xsmmac,          
     b                                                         reswav)  
            xsmman=punish*prdef**2/pin**2*                              
     a                           (xsmmcl(jtt)*1.8/srt-xsmmac)           
            if(xsmman.lt.0.0) xsmman=0.0                                
            xsect=xsect+xsmman                                          
            if(iswi.eq.1) then                                          
              kchact=min(kchact+1,maxcha)                               
              sigcha(kchact)=xsmman                                     
              ktpout(1,kchact)=kfres                                    
              ktpout(2,kchact)=0                                        
              isotro(kchact)=.false.                                    
            end if                                                      
          end if                                                        
   81    continue                                                       
       end if                                                           
       if(iswi.eq.0) return                                             
       if(kchact.ge.maxcha) then                                        
         call errex('pspsv:kchact.ge.maxcha')                           
       end if                                                           
       if(xsect.eq.0.0) then                                            
         return                                                         
       end if                                                           
       zrand=ranf(0)                                                    
       iswave=0                                                         
       zcomp=0.0                                                        
       do 800 k=1,kchact                                                
        zcomp=zcomp+sigcha(k)/xsect                                     
        if(zrand.lt.zcomp) then                                         
          itp1=ktpout(1,k)                                              
          itp2=ktpout(2,k)                                              
          if(isotro(k)) iswave=1                                        
          return                                                        
        end if                                                          
  800  continue                                                         
      return                                                            
      end                                                               
      function spspsv(iblock,ifine,srt)                                 
       data ckkbar,spipir/ 1.8,1.5/                                     
       if(iblock.eq.1) then                                             
          if(ifine.eq.0) then                                           
            if(srt.gt.0.98) then                                        
              spspsv=ckkbar*(spipir/max(spipir,srt))**2                 
            else                                                        
              spspsv=0.0                                                
            end if                                                      
            return                                                      
          else if(ifine.eq.1) then                                      
           iunit=1                                                      
          else if(ifine.eq.2) then                                      
           iunit=2                                                      
          else if(ifine.eq.3) then                                      
           iunit=3                                                      
          else if(ifine.eq.4) then                                      
           iunit=4                                                      
          else if(ifine.eq.5) then                                      
           iunit=5                                                      
          else if(ifine.eq.6) then                                      
           iunit=6                                                      
          else if(ifine.eq.7) then                                      
           iunit=7                                                      
          else                                                          
           call errex('sppsv:iblock=1,wrong ifine in input')            
          end if                                                        
       else if(iblock.eq.2) then                                        
          if(ifine.eq.1) then                                           
           iunit=9                                                      
          else if(ifine.eq.2) then                                      
           iunit=8                                                      
          else if(ifine.eq.3) then                                      
           iunit=11                                                     
          else if(ifine.eq.4) then                                      
           iunit=10                                                     
          else                                                          
           call errex('sppsv:iblock=2,wrong ifine in input')            
          end if                                                        
       else if(iblock.eq.3) then                                        
          if(ifine.eq.1) then                                           
           iunit=12                                                     
          else if(ifine.eq.2) then                                      
           iunit=18                                                     
          else if(ifine.eq.3) then                                      
           iunit=13                                                     
          else if(ifine.eq.4) then                                      
           iunit=15                                                     
          else if(ifine.eq.5) then                                      
           iunit=14                                                     
          else if(ifine.eq.6) then                                      
           iunit=17                                                     
          else if(ifine.eq.7) then                                      
           iunit=16                                                     
          else                                                          
           call errex('sppsv:iblock=3,wrong ifine in input')            
          end if                                                        
       else if(iblock.eq.4) then                                        
          if(ifine.eq.1) then                                           
           iunit=19                                                     
          else if(ifine.eq.2) then                                      
           iunit=20                                                     
          else if(ifine.eq.3) then                                      
           iunit=21                                                     
          else if(ifine.eq.4) then                                      
           iunit=22                                                     
          else                                                          
           call errex('sppsv:iblock=4,wrong ifine in input')            
          end if                                                        
       else if(iblock.eq.5) then                                        
          if(ifine.eq.1) then                                           
           iunit=31                                                     
          else if(ifine.eq.2) then                                      
           iunit=23                                                     
          else if(ifine.eq.3) then                                      
           iunit=28                                                     
          else if(ifine.eq.4) then                                      
           iunit=26                                                     
          else                                                          
           call errex('sppsv:iblock=5,wrong ifine in input')            
          end if                                                        
       else if(iblock.eq.6) then                                        
          if(ifine.eq.1) then                                           
           iunit=30                                                     
          else if(ifine.eq.2) then                                      
           iunit=24                                                     
          else if(ifine.eq.3) then                                      
           iunit=27                                                     
          else if(ifine.eq.4) then                                      
           iunit=29                                                     
          else if(ifine.eq.5) then                                      
           iunit=25                                                     
          else                                                          
           call errex('sppsv:iblock=6,wrong ifine in input')            
          end if                                                        
       else                                                             
           call errex('sppsv:wrong iblock in input')                    
       end if                                                           
       spspsv=fctact(srt,iunit)                                         
      return                                                            
      end                                                               
      function fctact(x,iunit)                                          
       real*4 par(31,5),xmin(31)                                        
       data (par( 1,i),i=1,5) /                                         
     @     0.0496,   0.9664,   0.9717,   1.5285,  -0.3368/              
       data (par( 2,i),i=1,5) /                                         
     @     0.0688,   0.9673,   0.9724,   1.5303,  -0.3296/              
       data (par( 3,i),i=1,5) /                                         
     @     0.3717,   1.2782,   1.1125,   1.4592,  -1.3680/              
       data (par( 4,i),i=1,5) /                                         
     @     1.3562,   1.5325,   1.1570,   1.6747,  -1.8971/              
       data (par( 5,i),i=1,5) /                                         
     @     1.3808,   1.4883,   1.2045,   1.6506,  -1.6213/              
       data (par( 6,i),i=1,5) /                                         
     @     1.1329,   1.3409,   1.0993,   1.4629,  -1.8432/              
       data (par( 7,i),i=1,5) /                                         
     @     1.0561,   1.0760,   1.0083,   1.3433,  -1.4162/              
       data (par( 8,i),i=1,5) /                                         
     @     1.6333,   1.6699,   1.3007,   1.9609,  -1.7665/              
       data (par( 9,i),i=1,5) /                                         
     @     2.4496,   2.1528,   1.2514,   1.8029,  -1.7880/              
       data (par(10,i),i=1,5) /                                         
     @     1.1051,   1.5724,   1.2141,   1.6655,  -2.0837/              
       data (par(11,i),i=1,5) /                                         
     @     1.4993,   1.4053,   1.1072,   1.4924,  -1.6585/              
       data (par(12,i),i=1,5) /                                         
     @     1.6303,   2.4770,   1.3499,   1.1591,  -2.4693/              
       data (par(13,i),i=1,5) /                                         
     @     1.2547,   1.4840,   1.1716,   1.5817,  -1.7916/              
       data (par(14,i),i=1,5) /                                         
     @     1.2013,   1.5627,   1.1629,   1.5581,  -1.8396/              
       data (par(15,i),i=1,5) /                                         
     @     1.2013,   1.5627,   1.1629,   1.5581,  -1.8396/              
       data (par(16,i),i=1,5) /                                         
     @     2.3714,   2.3333,   1.3555,   1.9017,  -1.8627/              
       data (par(17,i),i=1,5) /                                         
     @     3.2197,   2.3056,   1.1654,   1.7033,  -1.4781/              
       data (par(18,i),i=1,5) /                                         
     @     2.5925,   2.6074,   1.3392,   1.6376,  -1.7021/              
       data (par(19,i),i=1,5) /                                         
     @     6.4184,   6.4412,   2.3909,   1.7606,  -2.6324/              
       data (par(20,i),i=1,5) /                                         
     @     7.8194,   6.9883,   2.4196,   1.7288,  -2.7481/              
       data (par(21,i),i=1,5) /                                         
     @     3.5109,   3.3602,   2.3839,   1.0591,  -1.9918/              
       data (par(22,i),i=1,5) /                                         
     @     5.7171,   3.8840,   2.1747,   1.0108,  -2.3779/              
       data (par(23,i),i=1,5) /                                         
     @     2.6782,   2.2225,   1.2666,   1.6661,  -1.7847/              
       data (par(24,i),i=1,5) /                                         
     @     5.0686,   4.2596,   1.7766,   1.9624,  -1.5748/              
       data (par(25,i),i=1,5) /                                         
     @     2.8077,   2.6044,   1.3822,   1.5931,  -1.9576/              
       data (par(26,i),i=1,5) /                                         
     @     3.8932,   3.3462,   1.6500,   2.1747,  -1.9046/              
       data (par(27,i),i=1,5) /                                         
     @     4.3529,   3.9049,   1.7330,   2.0365,  -1.9882/              
       data (par(28,i),i=1,5) /                                         
     @     3.8845,   7.3616,   2.3917,   2.9604,  -2.2731/              
       data (par(29,i),i=1,5) /                                         
     @     3.4868,   2.7246,   1.4156,   1.5358,  -1.6490/              
       data (par(30,i),i=1,5) /                                         
     @     5.1783,   6.3326,   2.4310,   2.8886,  -1.8918/              
       data (par(31,i),i=1,5) /                                         
     @     2.5771,   2.4656,   1.3140,   2.0519,  -1.6447/              
       data (xmin(i),i=1,31) /                                          
     @     0.9900,   0.9900,   1.3900,   1.5460,   1.5460,              
     @     1.7900,   1.7900,   1.9140,   1.6777,   1.5140,              
     @     1.2777,   0.9900,   1.3900,   1.3900,   1.3900,              
     @     1.7920,   1.5557,   1.7900,   1.5460,   1.5460,              
     @     0.2760,   0.2760,   1.5140,   1.8017,   1.5676,              
     @     1.9140,   2.0380,   1.6777,   1.3900,   1.5654,              
     @     1.2777/                                                      
       dx=x-xmin(iunit)                                                 
       if(dx.le.0.0) then                                               
         fctact=0.0                                                     
       else                                                             
         fctact=par(iunit,1)*                                           
     @      (1.-exp(-par(iunit,2)*dx-par(iunit,3)*dx**2))               
         if(x.gt.par(iunit,4)) then                                     
           fctact=fctact*(x/par(iunit,4))**par(iunit,5)                 
         end if                                                         
         if(fctact.lt.0.0) then                                         
           call errex('fctact gives negative x section, abort')         
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function cgksq1(im1,im2)                                          
       if(iabs(im1).gt.1.or.iabs(im2).gt.1) then                        
         cgksq1=0.0                                                     
       else                                                             
         if(iabs(im1+im2).eq.2) then                                    
           cgksq1=1.0                                                   
         else                                                           
           cgksq1=0.5                                                   
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      subroutine mesres(iswi,                                           
     a    identr,i1,i31,i2,i32,k1n,k2n,iclcod,isobre,istrin,s,resxs,    
     b                                                         reswav)  
      parameter (nres=28)                                               
      parameter (maxkan=10)                                             
      dimension lchan(0:nres),nchan(maxkan,0:nres),                     
     a          gchan(0:nres),em1cha(maxkan,0:nres),                    
     b          spin(maxkan,0:nres),em2cha(maxkan,0:nres),              
     c          emr(0:nres),brchan(maxkan,0:nres),                      
     d          kchan(2,0:nres),prbcha(nres),                           
     e          jtotal(0:nres),iorbam(maxkan,0:nres),isores(0:nres)     
      logical coher(0:nres)                                             
      dimension jclas(0:nres)                                           
      data lchan / 1,                                                   
     a             4,2,3,4,7,3,1,                                       
     b             2,3,1,1,3,1,1,2,2,5,                                 
     c             2,5,4,3,4,1,6,3,6,4,                                 
     d             3                                                    
     e           /                                                      
      data (nchan(i,0),i=1,maxkan) / 1, 9*0/                            
      data (nchan(i,1),i=1,maxkan) /11, 7, 5,33, 6*0/                   
      data (nchan(i,2),i=1,maxkan) / 5,10, 8*0/                         
      data (nchan(i,3),i=1,maxkan) /11,18,23, 7*0/                      
      data (nchan(i,4),i=1,maxkan) / 5,10, 1,15, 6*0/                   
      data (nchan(i,5),i=1,maxkan) / 29,7,1,14,18,23,37, 3*0/           
      data (nchan(i,6),i=1,maxkan) / 5,5,11,137, 6*0/                   
      data (nchan(i,7),i=1,maxkan) / 2, 9*0/                            
      data (nchan(i,8),i=1,maxkan) / 1,5, 8*0/                          
      data (nchan(i,9),i=1,maxkan) / 7,5,11, 7*0/                       
      data (nchan(i,10),i=1,maxkan) /11, 9*0/                           
      data (nchan(i,11),i=1,maxkan) /11, 9*0/                           
      data (nchan(i,12),i=1,maxkan) /1,5,15, 7*0/                       
      data (nchan(i,13),i=1,maxkan) /11,137, 8*0/                       
      data (nchan(i,14),i=1,maxkan) /11,144, 8*0/                       
      data (nchan(i,15),i=1,maxkan) /1,5, 8*0/                          
      data (nchan(i,16),i=1,maxkan) /23,18,141, 7*0/                    
      data (nchan(i,17),i=1,maxkan) /1,15,14,29,37, 5*0/                
      data (nchan(i,18),i=1,maxkan) /23,18, 8*0/                        
      data (nchan(i,19),i=1,maxkan) /1,15,29,37,5,436, 4*0/             
      data (nchan(i,20),i=1,maxkan) /12,16,30,38,301,138, 4*0/          
      data (nchan(i,21),i=1,maxkan) /16,2,12, 7*0/                      
      data (nchan(i,22),i=1,maxkan) /16,12,30,38,138, 5*0/              
      data (nchan(i,23),i=1,maxkan) /2, 9*0/                            
      data (nchan(i,24),i=1,maxkan) /2,16,20,12,30,38, 4*0/             
      data (nchan(i,25),i=1,maxkan) /2,12,16, 7*0/                      
      data (nchan(i,26),i=1,maxkan)/12,30,16,19,38,20,437, 3*0/         
      data (nchan(i,27),i=1,maxkan)/12,16, 2, 8,576, 5*0/               
      data (nchan(i,28),i=1,maxkan) /5,23,18,40, 6*0/                   
      data (spin(i,0),i=1,maxkan) /1.,  9*0./                           
      data (spin(i,1),i=1,maxkan) /3.,1.,1.,9.,  6*0./                  
      data (spin(i,2),i=1,maxkan) /1.,1.,  8*0./                        
      data (spin(i,3),i=1,maxkan) /3.,3.,3.,5.,1.,  5*0./               
      data (spin(i,4),i=1,maxkan) /1.,1.,1.,9.,  6*0./                  
      data (spin(i,5),i=1,maxkan) /1.,1.,1.,3.,3.,3.,3., 3*0./          
      data (spin(i,6),i=1,maxkan) /1.,1.,3.,1.,  6*0./                  
      data (spin(i,7),i=1,maxkan) /1.,  9*0./                           
      data (spin(i,8),i=1,maxkan) /1.,1.,  8*0./                        
      data (spin(i,9),i=1,maxkan) /1.,1.,3.,  7*0./                     
      data (spin(i,10),i=1,maxkan) /3.,  9*0./                          
      data (spin(i,11),i=1,maxkan) /3.,  9*0./                          
      data (spin(i,12),i=1,maxkan) /1.,1.,9.,  7*0./                    
      data (spin(i,13),i=1,maxkan) /3.,1.,  8*0./                       
      data (spin(i,14),i=1,maxkan) /3.,3.,  8*0./                       
      data (spin(i,15),i=1,maxkan) /1.,1.,  8*0./                       
      data (spin(i,16),i=1,maxkan) /3.,3.,3.,  7*0./                    
      data (spin(i,17),i=1,maxkan) /1.,9.,3.,3.,3.,  5*0./              
      data (spin(i,18),i=1,maxkan) /3.,3.,  8*0./                       
      data (spin(i,19),i=1,maxkan) /1.,9.,3.,3.,1.,5., 4*0./            
      data (spin(i,20),i=1,maxkan) /3.,3.,3.,3.,1.,1.,  4*0./           
      data (spin(i,21),i=1,maxkan) /3.,1.,3.,  7*0./                    
      data (spin(i,22),i=1,maxkan) /3.,3.,1.,3.,1.,  5*0./              
      data (spin(i,23),i=1,maxkan) /1.,  9*0./                          
      data (spin(i,24),i=1,maxkan) /1.,3.,9.,3.,3.,3., 4*0./            
      data (spin(i,25),i=1,maxkan) /1.,3.,3.,  7*0./                    
      data (spin(i,26),i=1,maxkan)/3.,3.,3.,3.,3.,9.,5., 3*0./          
      data (spin(i,27),i=1,maxkan)/3.,3.,1.,1.,5.,  5*0./               
      data (spin(i,28),i=1,maxkan)/1.,3.,3.,3.,  6*0./                  
      data (em1cha(i,0),i=1,maxkan)/0.138,  9*0./                       
      data (em2cha(i,0),i=1,maxkan)/0.138,  9*0./                       
      data (em1cha(i,1),i=1,maxkan)/0.770,0.550,0.498,0.783,6*0./       
      data (em2cha(i,1),i=1,maxkan)/0.138,0.138,0.498,0.460,6*0./       
      data (em1cha(i,2),i=1,maxkan)/0.498,0.550,  8*0./                 
      data (em2cha(i,2),i=1,maxkan)/0.498,0.550,  8*0./                 
      data (em1cha(i,3),i=1,maxkan)                                     
     a                  /0.770,0.496,0.496,1.270,1.400, 5*0./           
      data (em2cha(i,3),i=1,maxkan)                                     
     a                  /0.138,0.892,0.892,0.138,0.138, 5*0./           
      data (em1cha(i,4),i=1,maxkan)/0.498,0.550,0.138,0.770,6*0./       
      data (em2cha(i,4),i=1,maxkan)/0.498,0.550,0.138,0.770,6*0./       
      data (em1cha(i,5),i=1,maxkan)                                     
     a          /0.783,0.550,0.138,0.550,0.496,0.496,1.020, 3*0./       
      data (em2cha(i,5),i=1,maxkan)                                     
     a          /0.138,0.138,0.138,0.570,0.633,0.633,0.138, 3*0./       
      data (em1cha(i,6),i=1,maxkan)                                     
     a                  /0.494,0.498,0.138,0.138,6*0./                  
      data (em2cha(i,6),i=1,maxkan)                                     
     a                  /0.494,0.498,0.770,0.780,6*0./                  
      data (em1cha(i,7),i=1,maxkan) /0.496,  9*0./                      
      data (em2cha(i,7),i=1,maxkan) /0.138,  9*0./                      
      data (em1cha(i,8),i=1,maxkan) /0.138,0.498,  8*0./                
      data (em2cha(i,8),i=1,maxkan) /0.138,0.498,  8*0./                
      data (em1cha(i,9),i=1,maxkan)                                     
     a                  /0.550,0.498,0.770,  7*0./                      
      data (em2cha(i,9),i=1,maxkan)                                     
     a                  /0.138,0.498,0.138,  7*0./                      
      data (em1cha(i,10),i=1,maxkan) /0.770,  9*0./                     
      data (em2cha(i,10),i=1,maxkan) /0.138,  9*0./                     
      data (em1cha(i,11),i=1,maxkan) /0.770,  9*0./                     
      data (em2cha(i,11),i=1,maxkan) /0.138,  9*0./                     
      data (em1cha(i,12),i=1,maxkan)                                    
     a                   /0.138,0.498,0.570,  7*0./                     
      data (em2cha(i,12),i=1,maxkan)                                    
     a                   /0.138,0.498,0.570,  7*0./                     
      data (em1cha(i,13),i=1,maxkan) /0.770,0.804,  8*0./               
      data (em2cha(i,13),i=1,maxkan) /0.138,0.138,  8*0./               
      data (em1cha(i,14),i=1,maxkan) /0.770,0.783,  8*0./               
      data (em2cha(i,14),i=1,maxkan) /0.138,0.500,  8*0./               
      data (em1cha(i,15),i=1,maxkan) /0.138,0.498,  8*0./               
      data (em2cha(i,15),i=1,maxkan) /0.138,0.498,  8*0./               
      data (em1cha(i,16),i=1,maxkan)                                    
     a                  /0.496,0.496,0.770, 7*0./                       
      data (em2cha(i,16),i=1,maxkan)                                    
     a                  /0.800,0.800,0.555, 7*0./                       
      data (em1cha(i,17),i=1,maxkan)                                    
     a                  /0.138,0.770,0.550,0.783,1.02, 5*0./            
      data (em2cha(i,17),i=1,maxkan)                                    
     a                  /0.138,0.580,0.770,0.138,0.138, 5*0./           
      data (em1cha(i,18),i=1,maxkan) /0.496,0.496, 8*0./                
      data (em2cha(i,18),i=1,maxkan) /0.892,0.892, 8*0./                
      data (em1cha(i,19),i=1,maxkan)                                    
     a                  /0.138,0.770,0.783,1.02,0.498,1.320, 4*0./      
      data (em2cha(i,19),i=1,maxkan)                                    
     a                  /0.138,0.770,0.138,0.138,0.498,0.138, 4*0./     
      data (em1cha(i,20),i=1,maxkan)                                    
     a                  /0.496,0.892,0.783,1.020,1.030,0.670, 4*0./     
      data (em2cha(i,20),i=1,maxkan)                                    
     a                  /0.570,0.138,0.496,0.496,0.138,0.496, 4*0./     
      data (em1cha(i,21),i=1,maxkan) /0.892,0.496,0.496, 7*0./          
      data (em2cha(i,21),i=1,maxkan) /0.138,0.138,0.770, 7*0./          
      data (em1cha(i,22),i=1,maxkan)                                    
     a                  /0.892,0.496,0.496,0.496,0.496, 5*0./           
      data (em2cha(i,22),i=1,maxkan)                                    
     a                  /0.138,0.770,0.783,1.020,0.806, 5*0./           
      data (em1cha(i,23),i=1,maxkan) /0.496,  9*0./                     
      data (em2cha(i,23),i=1,maxkan) /0.138,  9*0./                     
      data (em1cha(i,24),i=1,maxkan)                                    
     a                  /0.498,0.892,0.780,0.496,0.496,0.496, 4*0./     
      data (em2cha(i,24),i=1,maxkan)                                    
     a                  /0.138,0.138,0.550,0.770,0.783,1.020, 4*0./     
      data (em1cha(i,25),i=1,maxkan)                                    
     a                  /0.498,0.496,0.892, 7*0./                       
      data (em2cha(i,25),i=1,maxkan)                                    
     a                  /0.138,0.770,0.138, 7*0./                       
      data (em1cha(i,26),i=1,maxkan)                                    
     a             /0.496,0.496,0.892,0.892,0.496,0.892,0.496, 3*0./    
      data (em2cha(i,26),i=1,maxkan)                                    
     a             /0.770,0.783,0.138,0.550,1.020,0.770,1.170, 3*0./    
      data (em1cha(i,27),i=1,maxkan)                                    
     a                   /0.496,0.892,0.498,0.498,1.430, 5*0./          
      data (em2cha(i,27),i=1,maxkan)                                    
     a                   /0.770,0.138,0.138,0.550,0.138, 5*0./          
      data (em1cha(i,28),i=1,maxkan)                                    
     a             /0.498,0.496,0.892,1.020, 6*0./                      
      data (em2cha(i,28),i=1,maxkan)                                    
     a             /0.498,0.892,0.496,0.550, 6*0./                      
      data emr /                                                        
     a    0.770,                                                        
     b    1.318,1.525,1.665,1.713,1.233,1.020,0.892,                    
     c    0.975,0.980,1.170,1.260,1.270,1.300,1.390,1.400,1.420,1.450,  
     d    1.510,1.690,1.270,1.370,1.400,1.430,1.430,1.680,1.770,1.780,  
     e    1.690                                                         
     f         /                                                        
      data (brchan(i,0),i=1,maxkan)                                     
     a               /1.0,  9*0.  /                                     
      data (brchan(i,1),i=1,maxkan)                                     
     a               /0.701,0.145,0.049,0.105,6*0.  /                   
      data (brchan(i,2),i=1,maxkan)                                     
     a               /0.713,0.287, 8*0./                                
      data (brchan(i,3),i=1,maxkan)                                     
     a               /0.31,0.021,0.021,0.562,0.087, 5*0./               
      data (brchan(i,4),i=1,maxkan)                                     
     a               /0.38,0.18,0.039,0.401, 6*0./                      
      data (brchan(i,5),i=1,maxkan)                                     
     a               /0.44,0.23,0.12,0.08,0.06,0.06,0.01, 3*0./         
      data (brchan(i,6),i=1,maxkan)                                     
     a               /0.495,0.344,0.129,0.032, 6*0./                    
      data (brchan(i,7),i=1,maxkan)                                     
     a               /1.0, 9*0./                                        
      data (brchan(i,8),i=1,maxkan)                                     
     a               /1.0,-1.0, 8*0./                                   
      data (brchan(i,9),i=1,maxkan)                                     
     a               /0.6,-1.0,0.1, 7*0./                               
      data (brchan(i,10),i=1,maxkan)                                    
     a               /1.0, 9*0./                                        
      data (brchan(i,11),i=1,maxkan)                                    
     a               /1.0, 9*0./                                        
      data (brchan(i,12),i=1,maxkan)                                    
     a               /0.851,0.047,0.102, 7*0./                          
      data (brchan(i,13),i=1,maxkan)                                    
     a               /0.320,0.680, 8*0./                                
      data (brchan(i,14),i=1,maxkan)                                    
     a               /0.850,0.015, 8*0./                                
      data (brchan(i,15),i=1,maxkan)                                    
     a               /0.930,0.070, 8*0./                                
      data (brchan(i,16),i=1,maxkan)                                    
     a               /0.35,0.35,0.3, 7*0./                              
      data (brchan(i,17),i=1,maxkan)                                    
     a               /0.14,0.82,0.02,0.01,0.01, 5*0./                   
      data (brchan(i,18),i=1,maxkan)                                    
     a               /0.5,0.5, 8*0./                                    
      data (brchan(i,19),i=1,maxkan)                                    
     a               /0.236,0.351,0.17,0.038,0.016,0.19, 4*0./          
      data (brchan(i,20),i=1,maxkan)                                    
     a               /0.42,0.16,-1.0,-1.0,0.28,0.03, 4*0./              
      data (brchan(i,21),i=1,maxkan)                                    
     a               /0.899,0.066,0.035, 7*0./                          
      data (brchan(i,22),i=1,maxkan)                                    
     a               /0.94,0.03,0.01,-1.0,0.02, 5*0./                   
      data (brchan(i,23),i=1,maxkan)                                    
     a               /1.0, 9*0./                                        
      data (brchan(i,24),i=1,maxkan)                                    
     a               /0.497,0.253,0.13,0.088,0.032,-1.0, 4*0./          
      data (brchan(i,25),i=1,maxkan)                                    
     a               /0.387,0.314,0.299, 7*0./                          
      data (brchan(i,26),i=1,maxkan)                                    
     a               /0.32,0.15,0.02,0.16,0.17,0.02,0.16, 3*0./         
      data (brchan(i,27),i=1,maxkan)                                    
     a               /0.43,0.27,0.19,0.08,0.03, 5*0./                   
      data (brchan(i,28),i=1,maxkan)                                    
     a               /0.201,0.268,0.268,0.263, 6*0./                    
      data (iorbam(i,0),i=1,maxkan) /1,  9*0/                           
      data (iorbam(i,1),i=1,maxkan) /2,2,2,1,  6*0/                     
      data (iorbam(i,2),i=1,maxkan) /2,2,  8*0/                         
      data (iorbam(i,3),i=1,maxkan) /1,1,1,0,2,  5*0/                   
      data (iorbam(i,4),i=1,maxkan) /2,2,2,0,  6*0/                     
      data (iorbam(i,5),i=1,maxkan) /0,2,2,0,0,0,0, 3*0/                
      data (iorbam(i,6),i=1,maxkan) /1,1,1,1,  6*0/                     
      data (iorbam(i,7),i=1,maxkan) /1,  9*0/                           
      data (iorbam(i,8),i=1,maxkan) /0,0,  8*0/                         
      data (iorbam(i,9),i=1,maxkan) /0,0,2,  7*0/                       
      data (iorbam(i,10),i=1,maxkan) /0,  9*0/                          
      data (iorbam(i,11),i=1,maxkan) /0,  9*0/                          
      data (iorbam(i,12),i=1,maxkan) /2,2,0,  7*0/                      
      data (iorbam(i,13),i=1,maxkan) /1,0,  8*0/                        
      data (iorbam(i,14),i=1,maxkan) /1,0,  8*0/                        
      data (iorbam(i,15),i=1,maxkan) /0,0,  8*0/                        
      data (iorbam(i,16),i=1,maxkan) /0,0,1,  7*0/                      
      data (iorbam(i,17),i=1,maxkan) /1,1,1,1,1,  5*0/                  
      data (iorbam(i,18),i=1,maxkan) /0,0,  8*0/                        
      data (iorbam(i,19),i=1,maxkan) /3,1,3,3,3,2, 4*0/                 
      data (iorbam(i,20),i=1,maxkan) /0,0,0,0,1,1,  4*0/                
      data (iorbam(i,21),i=1,maxkan) /1,1,1,  7*0/                      
      data (iorbam(i,22),i=1,maxkan) /0,0,0,0,1,  5*0/                  
      data (iorbam(i,23),i=1,maxkan) /0,  9*0/                          
      data (iorbam(i,24),i=1,maxkan) /2,2,0,2,2,2, 4*0/                 
      data (iorbam(i,25),i=1,maxkan) /1,1,1,  7*0/                      
      data (iorbam(i,26),i=1,maxkan)/1,1,1,1,1,1,0, 3*0/                
      data (iorbam(i,27),i=1,maxkan)/3,3,3,3,2,  5*0/                   
      data (iorbam(i,28),i=1,maxkan)/1,1,1,1,  6*0/                     
      data isores/                                                      
     a            2,                                                    
     b            2,0,2,0,2,0,1,                                        
     c            0,2,0,2,0,2,0,0,0,2,                                  
     d            0,2,1,1,1,1,1,1,1,1,                                  
     e            0                                                     
     f            /                                                     
      data jtotal/                                                      
     a            3,                                                    
     b            4*5,3*3,                                              
     c            2*1,2*3,5,1,3,1,2*3,                                  
     d            3,7,3*3,1,5,3,5,7,                                    
     e            3                                                     
     f            /                                                     
      data gchan /                                                      
     a     0.154,                                                       
     b     0.110,0.076,0.250,0.138,0.150,0.00441,0.0498,                
     c     0.053,0.057,0.311,0.400,0.185,0.400,0.224,0.600,0.055,0.237, 
     d     0.035,0.215,0.090,0.114,0.174,0.287,0.098,0.454,0.222,0.164, 
     e     0.220                                                        
     f     /                                                            
      data  coher /                                                     
     a     8*.false.,.true.,6*.false.,.true.,13*.false.                 
     e     /                                                            
      data jclas/                                                       
     a            5,                                                    
     b            1,4,1,4,5,9,6,                                        
     c            0,0,8,1,4,1,8,4,4,5,                                  
     d            4,5,6,6,6,6,6,6,6,6,                                  
     e            9                                                     
     f            /                                                     
      data f0kkb,d0kkb,gampl,gammi,thetaq,thetav/                       
     a      2.5, 2.5, 0.82, 0.59,  33.0,  35.2                          
     b                                   /                              
      parameter ( pi=3.1415927)                                         
      parameter (zmltp= 10.*pi/(5.0674**2))                             
      dimension pjin(4)                                                 
      common /pjbk/ pjin                                                
      common /gamrbk/ gamres                                            
      common /cohebk/ pchan(maxkan),ncha(maxkan)                        
       srt=s                                                            
       j2=0                                                             
       m2=0                                                             
       m1=i31+i32                                                       
       resxs=0.0                                                        
       reswav=0.0                                                       
       kentry=0                                                         
       do 50 ires=0,nres                                                
        j1=isores(ires)                                                 
        if(iswi.le.1) then                                              
          if(iswi.eq.-1) then                                           
            if(identr.ne.jclas(ires)) go to 50                          
          end if                                                        
          iopen=0                                                       
          do 51 ic=1,lchan(ires)                                        
   51      if(iclcod.eq.nchan(ic,ires)) iopen=1                         
          if(iopen.eq.0) go to 50                                       
          if(iswi.eq.-1) then                                           
            cgksq=1.0                                                   
          else if(iabs(m1).gt.j1) then                                  
            go to 50                                                    
          else if(mod(i1+i2+j1,2).ne.0) then                            
            go to 50                                                    
          else                                                          
            cgksq=cgks(i1,i2,j1,i31,i32)                                
            if(cgksq.eq.0.0) go to 50                                   
          end if                                                        
        else                                                            
          if(identr.ne.idres(ires,1)) go to 50                          
        end if                                                          
        jtot=jtotal(ires)                                               
        emres=emr(ires)                                                 
        gammar=gchan(ires)                                              
        gamt=0.0                                                        
        do 11 i=1,maxkan                                                
          pchan(i)=0.0                                                  
          br=brchan(i,ires)                                             
          if(br.eq.0.0) go to 11                                        
          iorb=iorbam(i,ires)                                           
          em1=em1cha(i,ires)                                            
          em2=em2cha(i,ires)                                            
          if(srt.gt.em1+em2) then                                       
            pr=prcms(srt,em1,em2)                                       
            gctact=1./(1.+(pr/emres)**2)**(iorb+1)                      
          else                                                          
            gctact=0.0                                                  
            pr=0.0                                                      
          end if                                                        
          if(br.lt.0.0) then                                            
            if(srt.gt.em1+em2) then                                     
              if(ires.eq.8.and.i.eq.2) then                             
                asol=f0kkb                                              
              else if(ires.eq.9.and.i.eq.2) then                        
                asol=d0kkb                                              
              else if(ires.eq.20.and.i.eq.3) then                       
                gqkom1=                                                 
     a            0.5*gampl*sin(thetav*2.*pi/360.)*                     
     b                      cos(thetaq*2.*pi/360.)-                     
     c            gammi*(sqrt(0.05)*sin(thetav*2.*pi/360.)-             
     d                   sqrt(0.40)*cos(thetav*2.*pi/360.))*            
     e                                     sin(thetaq*2.*pi/360.)       
                asol=gqkom1**2/emres**2                                 
              else if(ires.eq.20.and.i.eq.4) then                       
                gqkfi1=                                                 
     a            0.5*gampl*cos(thetav*2.*pi/360.)*                     
     b                      cos(thetaq*2.*pi/360.)-                     
     c            gammi*(sqrt(0.05)*cos(thetav*2.*pi/360.)-             
     d                   sqrt(0.40)*sin(thetav*2.*pi/360.))*            
     e                                     sin(thetaq*2.*pi/360.)       
                asol=gqkfi1**2/emres**2                                 
              else if(ires.eq.22.and.i.eq.4) then                       
                gqkfi2=                                                 
     a            0.5*gampl*cos(thetav*2.*pi/360.)*                     
     b                      sin(thetaq*2.*pi/360.)+                     
     c            gammi*(sqrt(0.05)*cos(thetav*2.*pi/360.)-             
     d                   sqrt(0.40)*sin(thetav*2.*pi/360.))*            
     e                                     cos(thetaq*2.*pi/360.)       
                asol=gqkfi2**2/emres**2/gctact                          
              else if(ires.eq.24.and.i.eq.6) then                       
                ema=0.783                                               
                emb=0.496                                               
                promeg=prcms(emres,ema,emb)                             
                asol=(cos(thetav*2.*pi/360)                             
     a                 /sin(thetav*2.*pi/360.))**2*                     
     b                   brchan(5,ires)*gammar/promeg**(2*iorb+1)       
              else                                                      
                call errex(                                             
     a           'mesres: unknown closed channel/resonance combi')      
              end if                                                    
              asol=asol*gctact                                          
            else                                                        
              asol=0.0                                                  
            end if                                                      
          else                                                          
            pres=prcms(emres,em1,em2)                                   
            gct=1./(1.+(pres/emres)**2)**(iorb+1)                       
            asol=br*gammar*gctact/(gct*pres**(2*iorb+1))                
          end if                                                        
          pchan(i)=asol*pr**(2*iorb+1)                                  
          gamt=gamt+pchan(i)                                            
   11    continue                                                       
         if(iswi.eq.3) then                                             
           gamres=gamt                                                  
           return                                                       
         end if                                                         
         do 49 ic=1,lchan(ires)                                         
          if(iswi.eq.2) then                                            
            if(ic.ne.1) go to 50                                        
            sig=1.0                                                     
          else                                                          
            if(iclcod.ne.nchan(ic,ires)) go to 49                       
            if(ires.eq.6.and.ic.le.2.and.isobre.ne.ic) go to 49         
            em1=em1cha(ic,ires)                                         
            em2=em2cha(ic,ires)                                         
            if(srt.le.em1+em2) go to 50                                 
            pr=prcms(srt,em1,em2)                                       
            if(iclcod.eq.1.or.iclcod.eq.10.or.                          
     a             iclcod.eq.15.or.                                     
     b               iclcod.eq.36.or.iclcod.eq.45) then                 
              symm=2.0                                                  
            else                                                        
              symm=1.0                                                  
            end if                                                      
            if(coher(ires)) then                                        
              idtres=idres(ires,1)                                      
              lcha=lchan(ires)                                          
              do 61 lch=1,lcha                                          
   61         ncha(lch)=nchan(lch,ires)                                 
              call cohere(                                              
     a           iclcod,lcha,idtres,srt,pr,emres,gamt,sumtsq)           
              sig=symm*cgksq*jtot*                                      
     a                  zmltp/pr**2/spin(ic,ires)*sumtsq                
            else                                                        
              sig=symm*cgksq*jtot*                                      
     a           zmltp/pr**2/spin(ic,ires)*pchan(ic)/                   
     a                       ((srt-emres)**2+0.25*gamt*gamt)            
              gamo=gamt                                                 
              do 13 i=lchan(ires)+1,maxkan                              
   13         gamo=gamo-pchan(i)                                        
              if(gamo.lt.0.0)                                           
     a          call errex('mesres: open gamma width < 0')              
              sig=sig*gamo                                              
            end if                                                      
            if(iorb.eq.0) reswav=reswav+sig                             
            resxs=resxs+sig                                             
          end if                                                        
          probs=0.0                                                     
          do 12 i=1,lchan(ires)                                         
   12     probs=probs+pchan(i)                                          
          if(probs.eq.0.0) go to 50                                     
          if(iswi.le.0) go to 50                                        
          call mescon(ires,m1,kmult,k1)                                 
          if(kmult.ge.0.and.iswi.ne.2) then                             
            k1=isign(100,k1)*kmult+k1                                   
            k2=0                                                        
          else if(ires.le.nres) then                                    
           zrand=ranf(0)                                                
           inew=0                                                       
           do 15 i=1,lchan(ires)                                        
            if(zrand.lt.pchan(i)/probs) then                            
             inew=i                                                     
             goto 14                                                    
            else                                                        
             zrand=zrand-pchan(i)/probs                                 
            end if                                                      
   15      continue                                                     
   14      continue                                                     
           if(inew.eq.0) then                                           
            call errex('mesres:inew=0??????')                           
           end if                                                       
           iclnew=nchan(inew,ires)                                      
           if(iclnew.eq.1.or.iclnew.eq.11.or.iclnew.eq.15) then         
             j1new=2                                                    
             j2new=2                                                    
           else if(iclnew.eq.2.or.iclnew.eq.12.or.iclnew.eq.16          
     a                                        .or.iclnew.eq.20) then    
             j1new=1                                                    
             j2new=2                                                    
           else if(iclnew.eq.5.or.iclnew.eq.18.or.iclnew.eq.23) then    
             j1new=1                                                    
             j2new=1                                                    
           else if(iclnew.eq.7.or.iclnew.eq.14) then                    
             j1new=0                                                    
             j2new=2                                                    
           else if(iclnew.eq.8.or.iclnew.eq.19.or.iclnew.eq.30          
     a                                        .or.iclnew.eq.38) then    
             j1new=1                                                    
             j2new=0                                                    
           else if(iclnew.eq.10) then                                   
             j1new=0                                                    
             j2new=0                                                    
           else if(iclnew.eq.29.or.iclnew.eq.33.or.iclnew.eq.37) then   
             j1new=0                                                    
             j2new=2                                                    
           else                                                         
             call errex('mesres: this decay channel not allowed')       
           end if                                                       
           itag=0                                                       
           call isocgk(m1,j1,m2,j2,j1new,j2new,m1new,m2new,itag)        
           if(itag.eq.-1)                                               
     a        call errex('mesres does not allow for decay')             
           if(iclnew.eq.1) then                                         
             k1=m1new/2*17+23*(2-iabs(m1new))/2                         
             k2=m2new/2*17+23*(2-iabs(m2new))/2                         
           else if(iclnew.eq.11) then                                   
             k1=m1new/2*27+33*(2-iabs(m1new))/2                         
             k2=m2new/2*17+23*(2-iabs(m2new))/2                         
           else if(iclnew.eq.15) then                                   
             k1=m1new/2*27+33*(2-iabs(m1new))/2                         
             k2=m2new/2*27+33*(2-iabs(m2new))/2                         
           else if(iclnew.eq.2.or.iclnew.eq.12.or.iclnew.eq.16          
     a                                        .or.iclnew.eq.20) then    
             if(iclnew.eq.2.or.iclnew.eq.12) then                       
               k1=17+(3-m1new)/2                                        
             else                                                       
               k1=27+(3-m1new)/2                                        
             end if                                                     
             if(iclnew.eq.2.or.iclnew.eq.16) then                       
               k2=m2new/2*17+23*(2-iabs(m2new))/2                       
             else                                                       
               k2=m2new/2*27+33*(2-iabs(m2new))/2                       
             end if                                                     
           else if(iclnew.eq.5) then                                    
             k1=17+(3-m1new)/2                                          
             k2=-17-(3+m2new)/2                                         
           else if(iclnew.eq.18) then                                   
             k1=27+(3-m1new)/2                                          
             k2=-17-(3+m2new)/2                                         
           else if(iclnew.eq.23) then                                   
             k1=17+(3-m1new)/2                                          
             k2=-27-(3+m2new)/2                                         
           else if(iclnew.eq.7) then                                    
             k1=24                                                      
             k2=m2new/2*17+23*(2-iabs(m2new))/2                         
           else if(iclnew.eq.8.or.iclnew.eq.19.or.iclnew.eq.30          
     a                                        .or.iclnew.eq.38) then    
             if(iclnew.eq.8.or.iclnew.eq.38.or.iclnew.eq.30) then       
               k1=17+(3-m1new)/2                                        
             else                                                       
               k1=27+(3-m1new)/2                                        
             end if                                                     
             if(iclnew.eq.8.or.iclnew.eq.19) then                       
               k2=24                                                    
             else if(iclnew.eq.30) then                                 
               k2=34                                                    
             else if(iclnew.eq.38) then                                 
               k2=35                                                    
             end if                                                     
           else if(iclnew.eq.10) then                                   
             k1=24                                                      
             k2=24                                                      
           else if(iclnew.eq.14) then                                   
             k1=24                                                      
             k2=m2new/2*27+33*(2-iabs(m2new))/2                         
           else if(iclnew.eq.29.or.iclnew.eq.33.or.iclnew.eq.37) then   
             if(iclnew.eq.29.or.iclnew.eq.33) then                      
               k1=34                                                    
             else                                                       
               k1=35                                                    
             end if                                                     
             if(iclnew.eq.29.or.iclnew.eq.37) then                      
               k2=m2new/2*17+23*(2-iabs(m2new))/2                       
             else                                                       
               k2=m2new/2*27+33*(2-iabs(m2new))/2                       
             end if                                                     
           else                                                         
             call errex('mesres: decay channel not allowed')            
           end if                                                       
          else                                                          
            call errex('mesres:ires wrong')                             
          end if                                                        
          kentry=kentry+1                                               
          kchan(1,kentry)=k1                                            
          kchan(2,kentry)=k2                                            
          prbcha(kentry)=sig                                            
          go to 50                                                      
   49    continue                                                       
   50  continue                                                         
       if(iswi.gt.0) then                                               
         icha=0                                                         
         probs=0.0                                                      
         do 52 i=1,kentry                                               
          probs=probs+prbcha(i)                                         
   52    continue                                                       
         zrand=ranf(0)                                                  
         do 53 i=1,kentry                                               
          if(zrand.lt.prbcha(i)/probs) then                             
            icha=i                                                      
            goto 54                                                     
          else                                                          
            zrand=zrand-prbcha(i)/probs                                 
          end if                                                        
   53    continue                                                       
   54    continue                                                       
         if(icha.eq.0) then                                             
           if(kentry.ne.0)                                              
     a        call errex('mesres:icha=0??????')                         
           k1n=0                                                        
           k2n=0                                                        
         else                                                           
           k1n=kchan(1,icha)                                            
           k2n=kchan(2,icha)                                            
         end if                                                         
       end if                                                           
      return                                                            
      end                                                               
      function idres(i,jswi)                                            
       idres=-1                                                         
       if(jswi.eq.1) then                                               
       if(i.ge.0.and.i.le.7) then                                       
         if(i.eq.0) then                                                
           idres=5                                                      
         else if(i.le.5) then                                           
           idres=29+i                                                   
         else if(i.eq.6) then                                           
           idres=9                                                      
         else if(i.eq.7) then                                           
           idres=6                                                      
         else                                                           
           call errex('idres:wrong i')                                  
         end if                                                         
       else if(i.le.27) then                                            
         idres=2+i                                                      
       else if(i.le.28) then                                            
         idres=7+i                                                      
       else                                                             
         call errex('idres:wrong jswi/i combination')                   
       end if                                                           
       else if(jswi.eq.-1) then                                         
       i=iabs(i)                                                        
       if(i.ge.1.and.i.le.7) then                                       
         if(i.eq.5) then                                                
           idres=0                                                      
         else if(i.eq.6) then                                           
           idres=7                                                      
         else if(i.eq.9) then                                           
           idres=6                                                      
         else                                                           
           call errex('idres:wrong i')                                  
         end if                                                         
       else if(i.le.29) then                                            
         idres=i-2                                                      
       else if(i.le.34) then                                            
         idres=i-29                                                     
       else if(i.le.35) then                                            
         idres=i-7                                                      
       else                                                             
         call errex('idres:wrong jswi/i combination')                   
       end if                                                           
       else                                                             
         call errex('idres:wrong jswi')                                 
       end if                                                           
      return                                                            
      end                                                               
      subroutine cohere(iclcod,lcha,idtres,srt,pr,emres,gamt,sumtsq)    
      parameter (maxkan=10)                                             
      dimension gami(maxkan)                                            
      common /cohebk/ pchan(maxkan),ncha(maxkan)                        
      complex saa                                                       
      save delta,sumtot                                                 
      gamtot=gamt                                                       
      pchsum=0.0                                                        
      sumtsq=gamtot**2/                                                 
     a                 ((srt-emres)**2+0.25*gamtot*gamtot)              
      gamsum=0.0                                                        
      do 10 i=1,lcha                                                    
       gami(i)=pchan(i)                                                 
       gamsum=gamsum+gami(i)                                            
       pchsum=pchsum+pchan(i)                                           
   10 continue                                                          
      if(abs(gamsum-gamtot).gt.1.e-4) then                              
        call errex(                                                     
     a       'cohere:gamma of channels adds up incorrectly')            
      end if                                                            
      if(pchsum.gt.0.0) then                                            
        do 11 i=1,lcha                                                  
   11   pchan(i)=pchan(i)/pchsum                                        
      end if                                                            
      iblock=0                                                          
      if(idtres.eq.10.or.idtres.eq.17) then                             
        iblock=1                                                        
        if(idtres.eq.10) then                                           
          delta=0.0                                                     
          sumtot=0.0                                                    
        end if                                                          
      else                                                              
        call errex('cohere should not be called')                       
      end if                                                            
      iel=0                                                             
      do 14 i=1,lcha                                                    
       if(ncha(i).eq.iclcod) then                                       
         iel=i                                                          
         delta=delta+atan2(0.5*gami(i),emres-srt)                       
         sumtsq=pchan(i)*sumtsq                                         
         pchael=pchan(i)                                                
         pchan(i)=0.0                                                   
       end if                                                           
   14 continue                                                          
      if(iel.eq.0) call errex('cohere abort:iel=0')                     
      sumts=sumtsq                                                      
      sumtsq=0.0                                                        
      do 13 i=1,lcha                                                    
       if(iel.ne.i) then                                                
         sumtsq=sumtsq+pchan(i)*sumts                                   
         if(pchan(i).gt.0.0)                                            
     a   pchan(i)=pchan(i)/(1.-pchael)                                  
       end if                                                           
   13 continue                                                          
      sumtot=sumtot+sumtsq                                              
      delbac=0.0                                                        
      ilast=0                                                           
      if(iblock.eq.1) then                                              
        if(idtres.eq.17) then                                           
          ilast=1                                                       
          if(iclcod.eq.1) then                                          
            if(srt.gt.0.7) then                                         
              delbac=60.0/360.0*2.*3.141592                             
              if(srt.gt.1.3) delbac=delbac*1.3/srt                      
            else                                                        
              delta=0.0                                                 
              gamsig=2.06*pr                                            
              delbac=atan2(0.5*gamsig,0.801-srt)                        
            end if                                                      
          else if(iclcod.eq.5) then                                     
            delbac=0.0                                                  
          end if                                                        
        end if                                                          
      end if                                                            
      if(ilast.eq.1) then                                               
        delta=delta+delbac                                              
        eta=1.-sumtot                                                   
        if(sumtot.lt.0.0) then                                          
          call errex('cohere abort:eta**2<0')                           
        end if                                                          
        eta=sqrt(eta)                                                   
        saa=eta*cmplx(cos(2.*delta),sin(2.*delta))                      
        taasq=abs(saa-cmplx(1.0,0.0))**2                                
        do 15 i=1,lcha                                                  
   15   if(i.ne.iel) pchan(i)=pchan(i)*sumtsq/(sumtsq+taasq)            
        sumtsq=sumtsq+taasq                                             
        pchan(iel)=taasq/sumtsq                                         
      end if                                                            
      return                                                            
      end                                                               
      subroutine mmann(kl1,kl2,i1,i31,igp1,i2,i32,igp2,iswi,            
     a                                       jtype,punish,kfres)        
      dimension isor(9),igpar(9)                                        
      data isor / 2,-1,-1,0,2,1,-1,0,0  /                               
      data igpar / -1,0,0,1,1,0,0,-1,-1  /                              
         if(jtype.gt.9.or.jtype.lt.1) then                              
           call errex('mmann: jtype out of range')                      
         end if                                                         
         kfres=0                                                        
         punish=0.0                                                     
         ig=igpar(jtype)                                                
         ig12=igp1*igp2                                                 
         if(ig.eq.0.or.ig12.eq.0) then                                  
         else if(ig.ne.ig12) then                                       
           return                                                       
         end if                                                         
         j1=isor(jtype)                                                 
         m1=i31+i32                                                     
         if(iabs(m1).gt.j1) then                                        
           return                                                       
         else if(mod(i1+i2+j1,2).ne.0) then                             
           return                                                       
         else                                                           
           punis=cgks(i1,i2,j1,i31,i32)                                 
           if(punis.eq.0.0) return                                      
         end if                                                         
         kl1a=iabs(kl1)                                                 
         kl2a=iabs(kl2)                                                 
         if(jtype.eq.9.or.kl1a.eq.35.or.kl2a.eq.35) then                
           if(kl1a.eq.17.or.kl1a.eq.27.or.kl1a.eq.23                    
     a                   .or.kl1a.eq.33.or.kl1a.eq.34) then             
             return                                                     
           else if(kl2a.eq.17.or.kl2a.eq.27.or.kl2a.eq.23               
     a                   .or.kl2a.eq.33.or.kl2a.eq.34) then             
             return                                                     
           else if(jtype.eq.8) then                                     
             return                                                     
           end if                                                       
         end if                                                         
         itry=0                                                         
   10    itry=itry+1                                                    
         punish=punis                                                   
         if(itry.gt.50) then                                            
           write(8,*)  'jtype=',jtype                                   
           write(8,*)  'kl1=',kl1,ifla1,iflb1,ksp                       
           write(8,*)  'kl2=',kl2,ifla2,iflb2,ksp                       
           call errex('mmann:no common q/qbar-pair')                    
         end if                                                         
         call bele(ifla1,iflb1,kl1)                                     
         call bele(ifla2,iflb2,kl2)                                     
         if(mod(itry,2).eq.0) then                                      
           call iswap(ifla1,ifla2)                                      
           call iswap(iflb1,iflb2)                                      
         end if                                                         
         if(ifla1*iflb1.ge.0.or.ifla2*iflb2.ge.0) then                  
           call errex('mmann:mm annihilation, but wrong types')         
         end if                                                         
         if(ifla1*ifla2.gt.0) then                                      
           call iswap(ifla1,iflb1)                                      
         end if                                                         
         if(ifla1.eq.-ifla2) then                                       
           if(iabs(ifla1).eq.3) punish=punish*0.2                       
           ifla3=iflb1                                                  
           iflb3=iflb2                                                  
         else if(iflb1.eq.-iflb2) then                                  
           if(iabs(iflb1).eq.3) punish=punish*0.2                       
           ifla3=ifla1                                                  
           iflb3=ifla2                                                  
         else                                                           
           if(ifla1+iflb1.eq.0) then                                    
           else if(ifla2+iflb2.eq.0) then                               
           else                                                         
             call errex('mmann:dead end')                               
           end if                                                       
           go to 10                                                     
         end if                                                         
         if(jtype.eq.9) then                                            
           if(iabs(ifla3).ne.3) go to 10                                
         else if(jtype.eq.8) then                                       
           if(iabs(ifla3).eq.3) go to 10                                
         end if                                                         
         if(iswi.eq.1) then                                             
           call luifld(ifla3,0,iflb3,ifldum,kfres)                      
           if(kfres.eq.0) then                                          
             call errex('mmann abort: kfres=0')                         
           end if                                                       
           kfresa=iabs(kfres)                                           
           if(kfresa.eq.17.or.kfresa.eq.23) then                        
             if(jtype.eq.4) kfresa=24                                   
             if(jtype.eq.5) kfresa=kfresa+10                            
             if(jtype.eq.8.or.jtype.eq.9)                               
     a                         kfresa=34+jtype-8                        
           else if(kfresa.eq.27.or.kfresa.eq.33) then                   
             if(jtype.eq.1) kfresa=kfresa-10                            
             if(jtype.eq.4) kfresa=24                                   
             if(jtype.eq.8.or.jtype.eq.9)                               
     a                         kfresa=34+jtype-8                        
           else if(kfresa.eq.24.or.kfresa.eq.25) then                   
             if(jtype.eq.1) kfresa=23                                   
             if(jtype.eq.5) kfresa=33                                   
             if(jtype.eq.8.or.jtype.eq.9)                               
     a                         kfresa=34+jtype-8                        
           else if(kfresa.eq.34) then                                   
             if(jtype.eq.1) kfresa=23                                   
             if(jtype.eq.4) kfresa=24                                   
             if(jtype.eq.5) kfresa=33                                   
           else if(jtype.eq.6) then                                     
             if(kfresa.eq.18.or.kfresa.eq.19)kfresa=kfresa+10           
           end if                                                       
           kfres=isign(1,kfres)*(6000+kfresa)                           
         end if                                                         
      return                                                            
      end                                                               
      subroutine fndxsm                                                 
      common /mmclbk/xsmmcl(9)                                          
       do 10   jt=1,9                                                   
        xsmmcl(jt)=0.0                                                  
        if(jt.eq.2.or.jt.eq.3.or.jt.eq.7) go to 10                      
        iloop=1                                                         
        srtt=1.8                                                        
        if(jt.eq.6) srtt=1.8                                            
        if(jt.eq.9) srtt=1.8                                            
        istrt=0                                                         
        i1t=0                                                           
        i31t=0                                                          
        i2t=0                                                           
        i32t=0                                                          
   20   if(jt.eq.4) iloop=iloop-1                                       
        if(jt.eq.1) then                                                
          iclcot=11                                                     
        else if(jt.eq.4) then                                           
          if(iloop.eq.0) then                                           
            iclcot=10                                                   
          else if(iloop.eq.-1) then                                     
            iclcot=1                                                    
          else if(iloop.eq.-2) then                                     
            iclcot=5                                                    
          end if                                                        
        else if(jt.eq.5) then                                           
          iclcot=1                                                      
        else if(jt.eq.6) then                                           
          iclcot=2                                                      
        else if(jt.eq.8) then                                           
          iclcot=11                                                     
        else if(jt.eq.9) then                                           
          iclcot=5                                                      
        end if                                                          
        call mesres(-1,jt,i1t,i31t,i2t,                                 
     a                i32t,k1n,k2n,iclcot,isobre,istrt,srtt,rescl)      
        if(iloop.le.0) then                                             
          xsmmcl(jt+iloop)=rescl                                        
        else                                                            
          xsmmcl(jt)=rescl                                              
        end if                                                          
        if(iloop.le.0.and.iloop.ge.-1) go to 20                         
   10  continue                                                         
      return                                                            
      end                                                               
      subroutine mescon(ires,m,kmult,kf)                                
      parameter (nres=28)                                               
      dimension kmcon(0:nres),idncon(0:nres)                            
      data kmcon /                                                      
     a            0,                                                    
     b            3, 3,-1,-1, 2, 0, 0,                                  
     c           -1,-1, 2, 2, 3,-1,-1,-1, 2,-1,                         
     d            2,-1, 2,-1, 2,-1, 3,-1,-1,-1,                         
     e           -1                                                     
     &           /                                                      
      data idncon /                                                     
     a            5,                                                    
     b            1, 3,-1,-1, 5, 7, 8,                                  
     c           -1,-1, 6, 1, 2,-1,-1,-1, 2,-1,                         
     d            7,-1, 4,-1, 8,-1, 4,-1,-1,-1,                         
     e           -1                                                     
     &           /                                                      
       if(ires.lt.0.or.ires.gt.nres) then                               
         call errex('mescon: ires input out of range')                  
       end if                                                           
       iabm=iabs(m)                                                     
       kmult=kmcon(ires)                                                
       if(kmult.ge.0) then                                              
         idn=idncon(ires)                                               
         if(idn.eq.1.or.idn.eq.5) then                                  
           if(iabm.gt.2.or.mod(iabm,2).ne.0)                            
     a              call errex('mescon abort')                          
           kf=m/2*17+23*(2-iabm)/2                                      
           if(idn.eq.5) kf=kf+isign(1,kf)*10                            
         else if(idn.eq.2.or.idn.eq.6) then                             
           if(iabm.ne.0) call errex('mescon abort')                     
           kf=24                                                        
           if(idn.eq.6) kf=34                                           
         else if(idn.eq.3.or.idn.eq.7) then                             
           if(iabm.ne.0) call errex('mescon abort')                     
           kf=25                                                        
           if(idn.eq.7) kf=35                                           
         else if(idn.eq.4.or.idn.eq.8) then                             
           if(iabm.gt.1.or.mod(iabm,2).ne.1)                            
     a              call errex('mescon abort')                          
           kf=(37-m)/2                                                  
           if(idn.eq.8) kf=kf+10                                        
         else                                                           
           call errex('mescon abort:wrong idn')                         
         end if                                                         
       else                                                             
         kf=0                                                           
       end if                                                           
      return                                                            
      end                                                               
