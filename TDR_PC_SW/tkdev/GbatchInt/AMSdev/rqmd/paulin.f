CPAT,PAULIN   .  PDS = UF09.RQMD9.FORT  DATE = 951006  TIME = 215431562 
      subroutine paulin(ind,ntag)                                       
      include 'scatter7'                                                
      include 'partblk7'                                                
      dimension qlk(0:3),plk(0:3),xind(0:3),pind(0:3)                   
      real*4  con(ntotma),  dxvc(ntotma),dpvc(ntotma)                   
      dimension pik(0:3)                                                
      dimension ptngb(0:3),pdelt(0:3)                                   
      real*8 pindsq,qlk,plk,dqlksq,dplksq,xpind,dqpind,dppind,          
     a       pr2,r2,tcm,xcm,ycm,zcm,ecm,pxcm,pycm,pzcm,prcm,xcos,       
     b       pxcos,                                                     
     c       ptngb,pdelt,pik,pind,xind                                  
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data cos45 / 0.707/                                               
      common/paulbk/                                                    
     a     aux,const3,radn,padn,radn2,padn2,phs                         
      data vernum/'version rqmd 1.08 -- 22-apr-92'/                     
      data pronam /'p a u l i n-no collisions into occupied states'/    
      data ttvers/.true./                                               
      if (ttvers) then                                                  
        write(8,*)'---------------- version number report -------------'
        write(8,*)pronam                                                
        write(8,*)vernum                                                
        write(8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
        aux=(4.*pi/3.)**2                                               
        const3=4./9.*pi*pi*4/(hc)**3.                                   
        radn=3.                                                         
        padn=hc/radn/aux**.333/4**0.333                                 
        radn2=(2.*radn)**2                                              
        padn2=(2.*padn)**2                                              
        phs=16./9.*pi*pi*radn**3*padn**3/(2*pi *.197)**3                
233     format(1x,'pauli radii   r = ',f6.2,'   p = ',f6.2)             
      end if                                                            
      if(ntag.gt.0) then                                                
         linepi=ntag                                                    
      else                                                              
         linepi=0                                                       
      end if                                                            
      ntag=0                                                            
      if(iabs(ityp(ind)).gt.2) return                                   
      sum=0.                                                            
      psum=0.                                                           
      contrs=0.                                                         
      tcm=0.                                                            
      xcm=0.                                                            
      ycm=0.                                                            
      zcm=0.                                                            
      ecm=0.                                                            
      pxcm=0.                                                           
      pycm=0.                                                           
      pzcm=0.                                                           
      nn=0                                                              
      nn3=0                                                             
      nn4=0                                                             
      nn5=0                                                             
      nn6=0                                                             
      do 5 i=0,3                                                        
       xind(i)=x(i,ind)+qdot(i,ind)*dtprop                              
    5 continue                                                          
      do 15 i=0,3                                                       
      pind(i)=p(i,ind)                                                  
      ptngb(i)=0.                                                       
   15 continue                                                          
      pindsq=pind(0)*pind(0)                                            
      do 7 i=1,3                                                        
    7 pindsq=pindsq-pind(i)*pind(i)                                     
      if(pindsq.lt.0.) then                                             
          call errex('imaginary mass in paulin')                        
      end if                                                            
      do 1 k=1,nrbar(kclst)                                             
      con(k)=0.                                                         
      ik=iofset+k                                                       
      if(iabs(ityp(ik)).gt.2) go to 1                                   
      if(ik.eq.ind) go to 1                                             
      do 6 i=0,3                                                        
      qlk(i)=x(i,ik)+qdot(i,ik)*dtprop-xind(i)                          
      plk(i)=p(i,ik)                                                    
    6 continue                                                          
      if(switch(23)) then                                               
       if(qdot(0,ik).eq.0.) then                                        
         do  9 i=0,3                                                    
    9    pik(i)=p(i,ik)                                                 
       else                                                             
         do 3 i=0,3                                                     
    3    pik(i)=qdot(i,ik)                                              
       end if                                                           
       pkpkin =pik(0)*(pind(0)+plk(0))                                  
       dqpkin =qlk(0)*(pind(0)+plk(0))                                  
       do 4 i=1,3                                                       
        pkpkin=pkpkin-pik(i)*(pind(i)+plk(i))                           
        dqpkin=dqpkin-qlk(i)*(pind(i)+plk(i))                           
    4  continue                                                         
       do 27 i=0,3                                                      
       qlk(i)=qlk(i)- dqpkin/pkpkin * pik(i)                            
   27  continue                                                         
      end if                                                            
      dqlksq=qlk(0)*qlk(0)                                              
      dplksq=plk(0)*plk(0)                                              
      xpind =xind(0)*pind(0)                                            
      dqpind =qlk(0)*pind(0)                                            
      dppind =plk(0)*pind(0)                                            
      do 8 i=1,3                                                        
      xpind=xpind-xind(i)*pind(i)                                       
      dqpind=dqpind-qlk(i)*pind(i)                                      
      dppind=dppind-plk(i)*pind(i)                                      
      dqlksq=dqlksq-qlk(i)*qlk(i)                                       
      dplksq=dplksq-plk(i)*plk(i)                                       
    8 continue                                                          
      pr2= -dplksq+dppind*dppind/pindsq                                 
      r2= -dqlksq+dqpind*dqpind/pindsq                                  
      r=sqrt(r2)                                                        
      dxvc(k)=r                                                         
      if(r2.gt.radn2) goto 1                                            
      nn3=nn3+1                                                         
      if(issn(ik).le.nt.and.nclcnt(issn(ik)).eq.0) then                 
        if(issn(ik).le.n1) then                                         
          nn5=nn5+1                                                     
        else                                                            
          nn6=nn6+1                                                     
        end if                                                          
      end if                                                            
      do 34 i=0,3                                                       
       ptngb(i)=ptngb(i)+plk(i)                                         
   34 continue                                                          
      if(pr2.gt.padn2) goto 1                                           
      if(pr2.lt.0.) then                                                
        pr2=0.                                                          
      end if                                                            
      pr=sqrt(pr2)                                                      
      r=sqrt(r2)                                                        
      dxvc(k)=r                                                         
      dpvc(k)=pr                                                        
      ahr=radn-r/2.                                                     
      ahp=padn-pr/2.                                                    
      contr=const3*ahr*ahr*(3*radn-ahr)*ahp*ahp*(3*padn-ahp)            
      con(k)=contr                                                      
      tcm=tcm+qlk(0)-dqpind/pindsq*pind(0)                              
      xcm=xcm+qlk(1)-dqpind/pindsq*pind(1)                              
      ycm=ycm+qlk(2)-dqpind/pindsq*pind(2)                              
      zcm=zcm+qlk(3)-dqpind/pindsq*pind(3)                              
      ecm=ecm+plk(0)-dppind/pindsq*pind(0)                              
      pxcm=pxcm+plk(1)-dppind/pindsq*pind(1)                            
      pycm=pycm+plk(2)-dppind/pindsq*pind(2)                            
      pzcm=pzcm+plk(3)-dppind/pindsq*pind(3)                            
      nn=nn+1                                                           
      contrs=contrs+contr                                               
    1 continue                                                          
      if(nn.eq.0)return                                                 
      rhosph=nn3/(4./3.*pi*radn2**1.5)/rho0                             
      eexct=sqrt(ptngb(0)**2-ptngb(1)**2-ptngb(2)**2-ptngb(3)**2)/nn3   
      ekcms=(pind(0)*ptngb(0)-pind(1)*ptngb(1)-pind(2)*ptngb(2)         
     a                                     -pind(3)*ptngb(3))/ eexct/nn3
     b       -sqrt(pindsq)                                              
      if(ida(3).eq.1.and.ekcms.lt.0.037) then                           
       ekcms=ekcms-0.039                                                
      end if                                                            
      ectoff=-eexct+emnuc+0.3*0.2684**2/emnuc*rhosph**0.66667+          
     a                  0.5*alpha*rhosph+beta/(gamma+1.)*rhosph**gamma  
      if(linepi.gt.0) then                                              
        if(nn5.gt.nn6) then                                             
          inucl=1                                                       
        else                                                            
          inucl=0                                                       
        end if                                                          
        nn5=max(nn5,nn6)                                                
        if(nn5.gt.0) then                                               
           xfrac=float(nn5)/float(nn3)                                  
           if(xfrac.lt.0.9) xfrac=0.                                    
        else                                                            
           xfrac=0.                                                     
        end if                                                          
        if(eexct-emnuc.lt.0.01.and.xfrac.gt.0.) then                    
          if(linepi.le.0.or.linepi.gt.ntotma-nspect) then               
             call errex('linepi out of range in paulin')                
          end if                                                        
          pdelt(0)=p(0,linepi)+pind(0)                                  
          edelt=pdelt(0)*ptngb(0)                                       
          emdact=pdelt(0)*pdelt(0)                                      
          do 41 i=1,3                                                   
           pdelt(i)=p(i,linepi)+pind(i)                                 
           edelt=edelt-pdelt(i)*ptngb(i)                                
           emdact=emdact-pdelt(i)*pdelt(i)                              
   41     continue                                                      
          edelt=edelt/eexct/nn3                                         
          emdact=sqrt(emdact)                                           
          pdact=sqrt(edelt*edelt-emdact*emdact)                         
          prel=pcmsr(emdact,emnuc,empion)                               
        else                                                            
          linepi=0                                                      
          if(ekcms.gt.ectoff) then                                      
            sumf=contrs                                                 
            go to 33                                                    
          end if                                                        
        end if                                                          
      else                                                              
        if(ekcms.gt.ectoff) then                                        
          sumf=contrs                                                   
          go to 33                                                      
        end if                                                          
      end if                                                            
      tcm=tcm/nn-xind(0)+xpind/pindsq*pind(0)                           
      xcm=xcm/nn-xind(1)+xpind/pindsq*pind(1)                           
      ycm=ycm/nn-xind(2)+xpind/pindsq*pind(2)                           
      zcm=zcm/nn-xind(3)+xpind/pindsq*pind(3)                           
      rcm=sqrt(-tcm*tcm+xcm*xcm+ycm*ycm+zcm*zcm)                        
      ecm=ecm/nn                                                        
      pxcm=pxcm/nn                                                      
      pycm=pycm/nn                                                      
      pzcm=pzcm/nn                                                      
      arg = -ecm*ecm+pxcm*pxcm+pycm*pycm+pzcm*pzcm                      
      if(arg.lt.0.) then                                                
         prcm=0.                                                        
         if(-arg.gt. 1.e-3) then                                        
            write(8,*) 'ind=',ind,'nn=',nn                              
            write(8,*)'paulin: imaginary prcm arg=',arg                 
            call errex('num. error in paulin too large,not corrected')  
         end if                                                         
      else                                                              
         prcm=sqrt(arg)                                                 
      end if                                                            
      do 2 k=1,nrbar(kclst)                                             
      if(linepi.eq.0.and.con(k).eq.0.) go to 2                          
      ik=k+iofset                                                       
      do 16 i=0,3                                                       
      qlk(i)=x(i,ik)+qdot(i,ik)*dtprop-xind(i)                          
      plk(i)=p(i,ik)                                                    
   16 continue                                                          
      if(switch(23)) then                                               
        if(qdot(0,ik).eq.0.) then                                       
         do 31 i=0,3                                                    
   31    pik(i)=p(i,ik)                                                 
        else                                                            
         do 32 i=0,3                                                    
   32    pik(i)=qdot(i,ik)                                              
        end if                                                          
        pkpkin =pik(0)*(pind(0)+plk(0))                                 
        dqpkin =qlk(0)*(pind(0)+plk(0))                                 
        do 21 i=1,3                                                     
         pkpkin=pkpkin-pik(i)*(pind(i)+plk(i))                          
         dqpkin=dqpkin-qlk(i)*(pind(i)+plk(i))                          
   21   continue                                                        
        do 28 i=0,3                                                     
         qlk(i)=qlk(i)- dqpkin/pkpkin * pik(i)                          
   28   continue                                                        
      end if                                                            
      dr=dxvc(k)                                                        
      pdr=dpvc(k)                                                       
      if(dr*rcm.gt.1.e-4) then                                          
        xcos=(-qlk(0)*tcm+qlk(1)*xcm+qlk(2)*ycm+qlk(3)*zcm)/dr/rcm      
      else                                                              
        xcos=1.                                                         
      end if                                                            
      if(xcos.gt.cos45) then                                            
        sum=sum+con(k)                                                  
      end if                                                            
      if(linepi.gt.0) then                                              
        if(dr*dr.le.radn2.and.xcos.gt.cos45) then                       
           if(nclcnt(issn(ik)).eq.0.and.issn(ik).le.                    
     a           nt-inucl*n2.and.issn(ik).gt.(1-inucl)*n1) then         
             nn4=nn4+1                                                  
           end if                                                       
        end if                                                          
      end if                                                            
      if(pdr.eq.0.) then                                                
       pxcos=1.                                                         
      else if(prcm.eq.0.) then                                          
       pxcos=1.                                                         
      else                                                              
       pxcos=(-plk(0)*ecm+plk(1)*pxcm+plk(2)*pycm+plk(3)*pzcm)/pdr/prcm 
      end if                                                            
      if(pxcos.gt.cos45) then                                           
        psum=psum+con(k)                                                
      end if                                                            
   2  continue                                                          
      sum=sum*6.83                                                      
      psum=psum*6.83                                                    
      sumf=max(sum,psum)                                                
      if(sumf.lt.contrs)  sumf=contrs                                   
      if(linepi.gt.0) then                                              
        rhodlt=nn4/(2.*(1.-cos45)/3.*pi*radn2**1.5)/rho0                
        rhod=min(cvalu1,rhodlt)                                         
        fpact=0.27*rhod**0.3333                                         
        costmn=(prel*prel+(pdact/emdact*eexct)**2-fpact*fpact)*emdact/  
     a             (2.*eexct*pdact*prel)                                
        costmn=min(cvalu1,max(costmn,-cvalu1))                          
        pcoll=0.25*(2.+ costmn + costmn**3)                             
        pblock=1.-pcoll                                                 
        if(pblock.gt.sumf) sumf=pblock                                  
      end if                                                            
   33 continue                                                          
      if(sumf.gt.ranf(0))ntag=-1                                        
      return                                                            
      end                                                               
