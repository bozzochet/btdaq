CPAT,FRAGM    .                                                         
      subroutine lusysj(ip)                                             
      dimension pall(4),ifstor(2)                                       
      dimension zpm(2)                                                  
      common /luspbk/ pall,iqqsp,ifirst,ibegin,istart,istfl             
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
      common /zdisbk/ paraqq,paraq,paraqs                               
      common /su3mix/ angmxv,angeff                                     
      common /luopt1/                                                   
     a         iopt(20),kfstr,icqq,mbfix,ideltq,ibbcol,                 
     b         mmult(100),                                              
     a         iprbcq,ilead,iback,ibrtag,icqbar,idlj,klmold,            
     b         klbold,isot,misot,lbreak(100),emst,emda,emdb             
      common /prinbk/ iprin                                             
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /ludat1/ mst(40),par(80)                                   
      dimension ps(5),ifl(3),px(4),py(4),gam(3),pr(2),in(9),hm(4),hg(4),
     &  lrk(2),ie(2),iflf(2),iflj(2),pxj(2),pyj(2),zj(2),               
     &  zpos(2),pmq(3)                                                  
      double precision dp(5,5),dfour,hkc,hks,hk1,hk2,hc12,hcx1,hcx2,    
     &  hcxx,hcy1,hcy2,hcyx,hcyy                                        
      four(i,j)=p(i,4)*p(j,4)-p(i,1)*p(j,1)-p(i,2)*p(j,2)-p(i,3)*p(j,3) 
      dfour(i,j)=dp(i,4)*dp(j,4)-dp(i,1)*dp(j,1)-dp(i,2)*dp(j,2)-       
     &  dp(i,3)*dp(j,3)                                                 
      if(iopt(7).ne.0) jdeltq=ideltq                                    
      kopt1=0                                                           
      icqstr=icqbar                                                     
      igpar=mgpar(iabs(kfstr))                                          
      par1=par(1)                                                       
      par2=par(2)                                                       
      par11=par(11)                                                     
      par51=par(51)                                                     
      par52=par(52)                                                     
      if(icqq.eq.2) lflav=int(rlu(0)*(2.+par(3)))+1                     
      if(kopt1.eq.1)then                                                
         iantop=1                                                       
         iantpr=1                                                       
         par1=par(1)                                                    
         par(1)=10.                                                     
      else                                                              
         iantop=0                                                       
         iantpr=0                                                       
      end if                                                            
      ifirst=0                                                          
      ibegin=0                                                          
      istart=0                                                          
      ntry=0                                                            
      np=0                                                              
      do 100 j=1,5                                                      
  100 ps(j)=0.                                                          
      i=ip-1                                                            
  110 i=i+1                                                             
      if(i.gt.min(n,mst(30)-np-5-mst(31))) then                         
        mst(24)=mst(24)+1                                               
        mst(25)=2                                                       
        if(i.le.n) mst(25)=1                                            
        if(mst(23).ge.1) return                                         
      endif                                                             
      if(k(i,1).ge.20000.or.iabs(k(i,2)).lt.500) goto 110               
      np=np+1                                                           
      k(n+np,1)=i                                                       
      k(n+np,2)=k(i,2)                                                  
      do 120 j=1,5                                                      
      p(n+np,j)=p(i,j)                                                  
  120 ps(j)=ps(j)+p(i,j)                                                
      if(p(n+np,4)**2.lt.p(n+np,1)**2+p(n+np,2)**2+p(n+np,3)**2) then   
        p(n+np,4)=sqrt(p(n+np,1)**2+p(n+np,2)**2+p(n+np,3)**2+          
     &  p(n+np,5)**2)                                                   
        ps(4)=ps(4)+max(0.,p(n+np,4)-p(i,4))                            
      endif                                                             
      if(k(i,1).ge.10000) goto 110                                      
      mbst=0                                                            
      if(ps(1)**2+ps(2)**2+ps(3)**2.gt.0.5*ps(4)**2) then               
        mbst=1                                                          
        pebst=max(ps(4),1.0001*sqrt(ps(1)**2+ps(2)**2+ps(3)**2))        
        mst(1)=n+1                                                      
        mst(2)=n+np                                                     
        call lurobo(0.,0.,-ps(1)/pebst,-ps(2)/pebst,-ps(3)/pebst)       
      endif                                                             
      do  4 i=1,4                                                       
    4 pall(i)=ps(i)                                                     
      emstr=pall(4)**2-(pall(1)**2+pall(2)**2+pall(3)**2)               
      emstr=sqrt(emstr)                                                 
      iflab=-1                                                          
      nr=np                                                             
  130 if(nr.le.2) goto 180                                              
      drmin=2.*par(59)                                                  
      do 140 i=n+1,n+nr                                                 
      if(i.eq.n+nr.and.iabs(k(n+1,2)).ne.500) goto 140                  
      i1=i+1-nr*(i/(n+nr))                                              
      pap=sqrt((p(i,1)**2+p(i,2)**2+p(i,3)**2)*(p(i1,1)**2+             
     &p(i1,2)**2+p(i1,3)**2))                                           
      pvp=p(i,1)*p(i1,1)+p(i,2)*p(i1,2)+p(i,3)*p(i1,3)                  
      dr=4.*(pap-pvp)**2/(par(60)**2*pap+2.*(pap-pvp))                  
      if(dr.lt.drmin) then                                              
        ir=i                                                            
        drmin=dr                                                        
      endif                                                             
  140 continue                                                          
      if(drmin.lt.par(59).and.ir.eq.n+nr) then                          
        do 150 j=1,4                                                    
  150   p(n+1,j)=p(n+1,j)+p(n+nr,j)                                     
        p(n+1,5)=sqrt(max(0.,p(n+1,4)**2-p(n+1,1)**2-p(n+1,2)**2-       
     &  p(n+1,3)**2))                                                   
        nr=nr-1                                                         
        goto 130                                                        
      elseif(drmin.lt.par(59)) then                                     
        do 160 j=1,4                                                    
  160   p(ir,j)=p(ir,j)+p(ir+1,j)                                       
        p(ir,5)=sqrt(max(0.,p(ir,4)**2-p(ir,1)**2-p(ir,2)**2-           
     &  p(ir,3)**2))                                                    
        do 170 i=ir+1,n+nr-1                                            
        k(i,2)=k(i+1,2)                                                 
        do 170 j=1,5                                                    
  170   p(i,j)=p(i+1,j)                                                 
        if(ir.eq.n+nr-1) k(ir,2)=k(n+nr,2)                              
        nr=nr-1                                                         
        goto 130                                                        
      endif                                                             
  180 if(n+5*nr+11.ge.mst(30)-5-mst(31)) then                           
        mst(24)=mst(24)+1                                               
        mst(25)=1                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
                                                                        
      if(iabs(k(n+1,2)).ne.500) then                                    
        ns=nr-1                                                         
        nb=1                                                            
      else                                                              
        ns=nr+1                                                         
        w2sum=0.                                                        
        do 190 is=1,nr                                                  
        p(n+nr+is,1)=0.5*four(n+is,n+is+1-nr*(is/nr))                   
  190   w2sum=w2sum+p(n+nr+is,1)                                        
        w2ran=ranf(0)*w2sum                                             
        nb=0                                                            
  200   nb=nb+1                                                         
        w2sum=w2sum-p(n+nr+nb,1)                                        
        if(w2sum.gt.w2ran.and.nb.lt.nr) goto 200                        
      endif                                                             
                                                                        
      do 220 is=1,ns                                                    
      is1=n+is+nb-1-nr*((is+nb-2)/nr)                                   
      is2=n+is+nb-nr*((is+nb-1)/nr)                                     
      do 210 j=1,5                                                      
      dp(1,j)=p(is1,j)                                                  
      if(iabs(k(is1,2)).eq.500) dp(1,j)=0.5*dp(1,j)                     
      dp(2,j)=p(is2,j)                                                  
  210 if(iabs(k(is2,2)).eq.500) dp(2,j)=0.5*dp(2,j)                     
      dp(3,5)=dfour(1,1)                                                
      dp(4,5)=dfour(2,2)                                                
      hkc=dfour(1,2)                                                    
      if(dp(3,5)+2.*hkc+dp(4,5).le.0.) then                             
        dp(3,5)=dp(1,5)**2                                              
        dp(4,5)=dp(2,5)**2                                              
        dp(1,4)=dsqrt(dp(1,1)**2+dp(1,2)**2+dp(1,3)**2+dp(1,5)**2)      
        dp(2,4)=dsqrt(dp(2,1)**2+dp(2,2)**2+dp(2,3)**2+dp(2,5)**2)      
        hkc=dfour(1,2)                                                  
      endif                                                             
      hks=dsqrt(hkc**2-dp(3,5)*dp(4,5))                                 
      hk1=0.5*((dp(4,5)+hkc)/hks-1.)                                    
      hk2=0.5*((dp(3,5)+hkc)/hks-1.)                                    
      in1=n+nr+4*is-3                                                   
      p(in1,5)=sqrt(dp(3,5)+2.*hkc+dp(4,5))                             
      do 220 j=1,4                                                      
      p(in1,j)=(1.+hk1)*dp(1,j)-hk2*dp(2,j)                             
  220 p(in1+1,j)=(1.+hk2)*dp(2,j)-hk1*dp(1,j)                           
      nrs=nr+4*ns+7                                                     
  230 ntry=ntry+1                                                       
      if(ntry.gt.200) then                                              
        mst(24)=mst(24)+1                                               
        mst(25)=3                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      i=n+nrs                                                           
      do 240 j=1,4                                                      
      p(i,j)=0.                                                         
      do 240 is=1,nr                                                    
  240 p(i,j)=p(i,j)+p(n+is,j)                                           
      do 250 jt=1,2                                                     
      lrk(jt)=0                                                         
      ie(jt)=k(n+1+(jt/2)*(np-1),1)                                     
      iflj(jt)=0                                                        
      pxj(jt)=0.                                                        
      pyj(jt)=0.                                                        
      zj(jt)=0.                                                         
      zpos(jt)=1.                                                       
      zpm(jt)=1.                                                        
      in(3*jt+1)=n+nr+1+4*(jt/2)*(ns-1)                                 
      in(3*jt+2)=in(3*jt+1)+1                                           
      in(3*jt+3)=n+nr+4*ns+2*jt-1                                       
      do 250 in1=n+nr+2+jt,n+nr+4*ns-2+jt,4                             
      p(in1,1)=2-jt                                                     
      p(in1,2)=jt-1                                                     
  250 p(in1,3)=1.                                                       
      igparb=igpar                                                      
      igpara=1                                                          
      ideltq=0                                                          
      zqq=0.0                                                           
      iqqsp=0                                                           
      istfl=0                                                           
      iprtno=0                                                          
      iflstr=0                                                          
      ifjsid=0                                                          
      iplus=0                                                           
      icqqop=0                                                          
      jfl1=-1                                                           
      jfl2=-1                                                           
      mresex=0                                                          
      ipun=1                                                            
      if(ns.eq.nr-1) then                                               
        px(1)=0.                                                        
        py(1)=0.                                                        
        if(ns.eq.1) call luptdi(93,px(1),py(1))                         
        px(2)=-px(1)                                                    
        py(2)=-py(1)                                                    
        kfsum=0                                                         
        do 260 jt=1,2                                                   
        iflf(jt)=mod(k(ie(jt),2),500)                                   
        kfsum=kfsum+isign(1,iflf(jt)*(10-iabs(iflf(jt))))               
        ifl(jt)=iflf(jt)                                                
        gam(jt)=0.                                                      
        if(iabs(iflf(jt)).ge.10) then                                   
          ifjsid=jt                                                     
          if(icqq.ne.0) then                                            
            iflqq=ifl(jt)                                               
            ifl(jt)=iqqrnd(iflqq)                                       
          end if                                                        
          if(icqq.eq.2.and.ntry.le.100) then                            
            if(iflf(jt)/10.ne.3.and.mod(iflf(jt),10).ne.3) then         
              if(lflav.eq.3) then                                       
                ihisbm=3                                                
              else                                                      
                ihisbm=4                                                
              end if                                                    
            else                                                        
              if(lflav.eq.3) then                                       
                ihisbm=1                                                
              else                                                      
                ihisbm=2                                                
              end if                                                    
            end if                                                      
          else                                                          
            ihisbm=0                                                    
          end if                                                        
        end if                                                          
        if(mod(mst(10),2).eq.1.and.iabs(iflf(jt)).ge.10) then           
          ifla=iflf(jt)/10                                              
          iflb=iflf(jt)-10*ifla                                         
          if(iflab.eq.-1) then                                          
            iflab=int(ranf(0)+0.5)                                      
            if(ibbcol.eq.0) then                                        
            else if(iabs(iflb).eq.3.and.iflab.eq.0) then                
              if(ranf(0).le.0.25) iflab=1                               
            else if(iabs(ifla).eq.3.and.iflab.eq.1) then                
              if(ranf(0).le.0.25) iflab=0                               
            end if                                                      
          end if                                                        
          iflj(jt)=ifla+iflab*(iflb-ifla)                               
          if(n.gt.ie(jt).and.k(ie(jt)+1,1)/10000.eq.6.and.iabs(k(ie(jt)+
     &    1,2)).ge.610) iflj(jt)=mod(k(ie(jt)+1,2)/10,10)               
          ifl(jt)=ifla+iflb-iflj(jt)                                    
          ifstor(jt)=ifl(jt)                                            
          ibrtag=0                                                      
          icqbar=icqstr                                                 
          idlj=-1                                                       
          inol=0                                                        
          if(iflj(jt).ne.0) then                                        
           iqqsp=1                                                      
           if(icqstr.ne.0) then                                         
              if(iflj(jt).ne.icqstr) then                               
                 if(icqstr.ne.ifla.and.icqstr.ne.iflb) then             
                     write(8,*) 'icqstr,ifla,iflb',icqstr,ifla,iflb     
                     call errex('wrong icqstr in lusysj input')         
                 end if                                                 
                 iflj(jt)=icqstr                                        
                 ifl(jt)=ifla+iflb-icqstr                               
              end if                                                    
           end if                                                       
          end if                                                        
          call luzdis(0,1,0.,zj(jt))                                    
          if(iopt(4).ne.0) zj(jt)=0.0                                   
          if(par(12).gt.0.) then                                        
            call luptdi(94,pxr,pyr)                                     
          else                                                          
            pxr=0.                                                      
            pyr=0.                                                      
          end if                                                        
          px(jt)=0.5*px(jt)+pxr                                         
          py(jt)=0.5*py(jt)+pyr                                         
          pxj(jt)=px(jt)-2.*pxr                                         
          pyj(jt)=py(jt)-2.*pyr                                         
          if(n.gt.ie(jt).and.k(ie(jt)+1,1)/10000.eq.6) p(ie(jt)+1,1)=   
     &    zj(jt)                                                        
        endif                                                           
        pmq(jt)=ulmass(2,ifl(jt))                                       
  260   continue                                                        
        if(kfsum.ne.0) then                                             
          mst(24)=mst(24)+1                                             
          mst(25)=2                                                     
          if(mst(23).ge.1) return                                       
        endif                                                           
      else                                                              
        ifl(3)=int(1.+(2.+par(2))*ranf(0))*(-1)**int(ranf(0)+0.5)       
        call luifld(ifl(3),0,0,ifl(1),kdump)                            
        call luifld(-ifl(3),0,0,ifl(2),kdump)                           
        if(iabs(ifl(2)).lt.100) ifl(2)=-mod(ifl(1),100)                 
        if(iabs(ifl(2)).gt.100) ifl(1)=-mod(ifl(2),100)                 
        iflstr=(iabs(ifl(1))+90)/100                                    
        call luptdi(ifl(1),px(1),py(1))                                 
        px(2)=-px(1)                                                    
        py(2)=-py(1)                                                    
        pr3=min(25.,0.1*p(n+nr+1,5)**2)                                 
  270   call luzdis(ifl(1),0,pr3,z)                                     
        zr=pr3/(z*p(n+nr+1,5)**2)                                       
        if(zr.ge.1.) goto 270                                           
        do 280 jt=1,2                                                   
        pmq(jt)=ulmass(2,ifl(jt))                                       
        gam(jt)=pr3*(1.-z)/z                                            
        in1=n+nr+3+4*(jt/2)*(ns-1)                                      
        p(in1,jt)=1.-z                                                  
        p(in1,3-jt)=jt-1                                                
        p(in1,3)=(2-jt)*(1.-z)+(jt-1)*z                                 
        p(in1+1,jt)=zr                                                  
        p(in1+1,3-jt)=2-jt                                              
  280   p(in1+1,3)=(2-jt)*(1.-zr)+(jt-1)*zr                             
      endif                                                             
                                                                        
      do 320 jt=1,2                                                     
      if(jt.eq.1.or.ns.eq.nr-1) then                                    
        in1=in(3*jt+1)                                                  
        in3=in(3*jt+3)                                                  
        do 290 j=1,4                                                    
        dp(1,j)=p(in1,j)                                                
        dp(2,j)=p(in1+1,j)                                              
        dp(3,j)=0.                                                      
  290   dp(4,j)=0.                                                      
        dp(1,4)=dsqrt(dp(1,1)**2+dp(1,2)**2+dp(1,3)**2)                 
        dp(2,4)=dsqrt(dp(2,1)**2+dp(2,2)**2+dp(2,3)**2)                 
        dp(5,1)=dp(1,1)/dp(1,4)-dp(2,1)/dp(2,4)                         
        dp(5,2)=dp(1,2)/dp(1,4)-dp(2,2)/dp(2,4)                         
        dp(5,3)=dp(1,3)/dp(1,4)-dp(2,3)/dp(2,4)                         
        if(dp(5,1)**2.le.dp(5,2)**2+dp(5,3)**2) dp(3,1)=1.              
        if(dp(5,1)**2.gt.dp(5,2)**2+dp(5,3)**2) dp(3,3)=1.              
        if(dp(5,2)**2.le.dp(5,1)**2+dp(5,3)**2) dp(4,2)=1.              
        if(dp(5,2)**2.gt.dp(5,1)**2+dp(5,3)**2) dp(4,3)=1.              
        hc12=dfour(1,2)                                                 
        hcx1=dfour(3,1)/hc12                                            
        hcx2=dfour(3,2)/hc12                                            
        hcxx=1./dsqrt(1.+2.*hcx1*hcx2*hc12)                             
        hcy1=dfour(4,1)/hc12                                            
        hcy2=dfour(4,2)/hc12                                            
        hcyx=hcxx*(hcx1*hcy2+hcx2*hcy1)*hc12                            
        hcyy=1./dsqrt(1.+2.*hcy1*hcy2*hc12-hcyx**2)                     
        do 300 j=1,4                                                    
        dp(3,j)=hcxx*(dp(3,j)-hcx2*dp(1,j)-hcx1*dp(2,j))                
        p(in3,j)=dp(3,j)                                                
  300   p(in3+1,j)=hcyy*(dp(4,j)-hcy2*dp(1,j)-hcy1*dp(2,j)-             
     &  hcyx*dp(3,j))                                                   
      else                                                              
        do 310 j=1,4                                                    
        p(in3+2,j)=p(in3,j)                                             
  310   p(in3+3,j)=p(in3+1,j)                                           
      endif                                                             
  320 continue                                                          
      io4qq=0                                                           
      istore=i+1                                                        
  330 i=i+1                                                             
      if(i.ge.mst(30)-5-mst(31)) then                                   
        mst(24)=mst(24)+1                                               
        mst(25)=1                                                       
        if(mst(23).ge.1) return                                         
      endif                                                             
      ipun=max(0,ipun-1)                                                
      iprtno=iprtno+1                                                   
      mmult(iprtno)=0                                                   
      if(iprtno.eq.1) then                                              
        if(iopt(3).gt.0) then                                           
          iopt(3)=1                                                     
          if(ifjsid.ne.0) then                                          
            jtopt3=ifjsid                                               
          else                                                          
            jtopt3=1                                                    
          end if                                                        
          if(iopt(4).ne.0) call errex('lusysj:iopt(3)/iopt(4).ne.0')    
        else if(iopt(4).gt.0) then                                      
          iopt(4)=1                                                     
          if(ifjsid.ne.0) then                                          
            jtopp=3-ifjsid                                              
            call luifld(ifl(jtopp),0,0,ifl(3),k(i,2))                   
          end if                                                        
          if(iabs(ifl(3)).gt.10.and.ranf(0).gt.0.5) io4qq=1             
        end if                                                          
      end if                                                            
      if(iopt(7).eq.1.and.iprtno.eq.1) then                             
        iop7=1                                                          
      else                                                              
        iop7=0                                                          
      end if                                                            
      if(iop7.eq.1) then                                                
        if(iabs(ifl(1)).gt.10) then                                     
          jt=2                                                          
        else if(iabs(ifl(2)).gt.10) then                                
          jt=1                                                          
        else                                                            
          call errex('lusysj: missing qq at end for iop7=1')            
        end if                                                          
        if(px(jt).ne.0.0.or.py(jt).ne.0.0)                              
     a      call errex('lusysj:iop7=1  with init pt')                   
        iflfb=iflf(3-jt)                                                
        iflfa=iflf(jt)                                                  
        if(iprbcq.eq.0.and.jdeltq.eq.0.and.iopt(11).eq.0) then          
          kswi=1                                                        
        else                                                            
          kswi=2                                                        
        end if                                                          
        ichobr=iopt(10)                                                 
        ida53=iopt(12)                                                  
        ida55=iopt(13)                                                  
        call bresdc(imany,                                              
     a              iflfb,iflfa,ichobr,mbfix,                           
     b              kswi,ida53,ida55,klbold,klmold,emstr,emda,emdb,     
     c              kfbar,kfmes,kmltba,kmltme,                          
     d              iflb,ifla,ifl3,kdfr,embar,emmes,ptx,pty             
     e                 )                                                
        if(imany.eq.1) then                                             
          iopt(7)=2                                                     
          go to 230                                                     
        end if                                                          
        ifl(3-jt)=iflb                                                  
        ifl(jt)=ifla                                                    
        ifl(3)=ifl3                                                     
        px(3)=ptx                                                       
        py(3)=pty                                                       
        if(kswi.eq.1) then                                              
          iflf(3-jt)=iflb                                               
          iflf(jt)=ifla                                                 
          ideltq=100*iflf(jt)+iflf(3-jt)                                
        end if                                                          
        if(kdfr.eq.1) then                                              
          iopt(5)=2                                                     
          iprbcq=2                                                      
        end if                                                          
        k(i,2)=kfmes                                                    
      end if                                                            
      if(iprtno.ge.2) then                                              
        lrkjr=lrk(3-jt)                                                 
      else                                                              
        lrkjr=0                                                         
      end if                                                            
      lrkjt=0                                                           
      kreflc=0                                                          
      if(icqq.eq.2.and.iprtno.eq.2) kreflc=1                            
      if(iop7.eq.1) then                                                
      else if(io4qq.eq.1.and.iprtno.eq.1) then                          
        jt=3-ifjsid                                                     
      else if(iopt(4).ne.0.and.(iflj(1).ne.0.or.iflj(2).ne.0)) then     
        jt=ifjsid                                                       
      else if(iopt(5).eq.1.and.iprtno.eq.1) then                        
        jt=1                                                            
      else if(icqq.eq.1.and.iprtno.le.2) then                           
        jt=1                                                            
        if(iprtno.eq.1) kreflc=1                                        
      else if(iopt(3).eq.1) then                                        
        jt=jtopt3                                                       
      else if(icqq.eq.0.and.iprbcq.ne.0.and.iprtno.eq.1) then           
        jt=3-iprbcq                                                     
        if(iopt(7).eq.0) kreflc=1                                       
      else if(iprbcq.eq.0.and.iprtno.ge.2.and.                          
     a                   (iopt(3).ne.0.or.iopt(4).ne.0).and.            
     b                                           lrkjr.eq.0) then       
        lrkjt=lrk(jt)                                                   
        jt=3-jt                                                         
      else                                                              
        jt=1.5+ranf(0)                                                  
      end if                                                            
      if(iop7.eq.1) then                                                
      else                                                              
        call luptdi(ifl(jt),px(3),py(3))                                
      end if                                                            
      px(4)=px(3)                                                       
      py(4)=py(3)                                                       
      mqj=0                                                             
      if(iflj(jt).ne.0) then                                            
        prji=par(37)**2+(px(jt)+px(3))**2+(py(jt)+py(3))**2             
        call luzdis(ifl(jt),iflj(jt),prji,z)                            
        if((1.-z)*zpos(jt).le.zj(jt)) mqj=1                             
      end if                                                            
      lrk(jt)=lrk(jt)+1                                                 
      jr=3-jt                                                           
      js=3-2*jt                                                         
      k(i,1)=ie(jt)                                                     
      if(mqj.eq.1) then                                                 
           if(iabs(ifl(jt)).gt.10) goto 230                             
           if(lrk(jt).eq.1) ifl(jt)=iflf(jt)                            
           if(lrk(jt).eq.1) pmq(jt)=ulmass(2,ifl(jt))                   
           px(jt)=px(jt)+pxj(jt)                                        
           py(jt)=py(jt)+pyj(jt)                                        
           if(n.gt.ie(jt).and.k(ie(jt)+1,1)/10000.eq.6)                 
     &                                              p(ie(jt)+1,2)=i-nrs 
      end if                                                            
      prbdcp=par(11)                                                    
      prboct=par(77)                                                    
      if(iopt(14).eq.1.and.ifjsid.eq.jt.and.mresex.eq.0                 
     a           .and.iopt(5).eq.0.and.icqq.eq.0                        
     b           .and.(iabs(ifl(jt)).gt.10.or.mqj.eq.1)) then           
        iresex=1                                                        
        par(11)=prbdcp/(1.+prboct)                                      
      else                                                              
        iresex=0                                                        
      end if                                                            
      if(iop7.eq.1) then                                                
        go to 103                                                       
      else if(io4qq.eq.1.and.iprtno.eq.1) then                          
        go to 103                                                       
      end if                                                            
      mtry=0                                                            
  101 continue                                                          
      mtry=mtry+1                                                       
      if(mtry.gt.50) then                                               
            call errex('lusysj101:infinite loop??')                     
      end if                                                            
      call luifld(ifl(jt),mqj*iflj(jt),0,ifl(3),k(i,2))                 
      if(k(i,2).eq.25.and.ranf(0).gt.par(80)) then                      
        mmult(iprtno)=1                                                 
        k(i,2)=24                                                       
      end if                                                            
      par(2)=par2                                                       
      if(iopt(9).ne.0) then                                             
        if(k(i,2).eq.34.or.k(i,2).eq.35) then                           
          probch=0.6667*(sin(angmxv)-cos(angmxv)/sqrt(2.0))**2          
          if(k(i,2).eq.34) then                                         
            if(ranf(0).lt.probch) k(i,2)=35                             
          else if(k(i,2).eq.35) then                                    
            if(ranf(0).lt.probch) k(i,2)=34                             
          end if                                                        
        end if                                                          
      end if                                                            
  103 continue                                                          
      par(2)=par2                                                       
      par(1)=par1                                                       
      par(11)=par11                                                     
      if(k(i,2).eq.0)  then                                             
        goto 230                                                        
      end if                                                            
      if(iresex.eq.1) then                                              
        if(iopt(7).eq.1) call errex('lusysj-abort,iopt(7)=1')           
        mresex=1                                                        
        kfbar=k(i,2)                                                    
        kmltba=mlt810(k(i,2),embar)                                     
        if(kmltba.lt.0) go to 101                                       
        mmult(iprtno)=kmltba                                            
      end if                                                            
      if(jfl1.eq.-1.and.jt.eq.1) then                                   
        jfl1=0                                                          
      else                                                              
        jfl1=ifl(1)                                                     
      end if                                                            
      if(jfl2.eq.-1.and.jt.eq.2) then                                   
        jfl2=0                                                          
      else                                                              
        jfl2=ifl(2)                                                     
      end if                                                            
      if(iopt(5).eq.1) then                                             
        if(iprtno.eq.1) then                                            
          if(iabs(ifl(3)).eq.3) then                                    
            if(ranf(0).gt.0.2) go to 230                                
          end if                                                        
          k(i,2)=kfstr                                                  
          mmult(iprtno)=0                                               
          if(ranf(0).gt.0.5) ifl(3)=-ifl(3)                             
          ifl(2)=ifl(3)                                                 
        end if                                                          
      end if                                                            
      if(jt.eq.1) then                                                  
        lbreak(iprtno)=ifl(3)                                           
        lbreak(iprtno+1)=ifl(2)                                         
        if(iprtno.eq.2.and.icqq.eq.2) lbreak(iprtno+1)=-ifl(3)          
      else                                                              
        lbreak(iprtno)=ifl(2)                                           
        lbreak(iprtno+1)=-ifl(3)                                        
      end if                                                            
      if(jt.ne.ifjsid) istore=istore+1                                  
      if(jt.eq.ifjsid.and.inol.eq.0) then                               
          idlj=idlj+1                                                   
      end if                                                            
      if(mqj.eq.1) inol=1                                               
      if(mqj.eq.1.and.i.gt.istore) then                                 
         if(jt.eq.1) then                                               
            ibrtag=2                                                    
         else                                                           
            ibrtag=1                                                    
         end if                                                         
         icqbar=ifstor(jt)                                              
      end if                                                            
      if(igpar.ne.0) then                                               
        igparh=mgpar(iabs(k(i,2)))                                      
        igpara=igpara*igparh                                            
        if(igparh.eq.0) igparb=0                                        
      end if                                                            
      if(iantop.eq.1.and.iantpr.eq.0)then                               
         if(i.eq.n+nrs+1) then                                          
            iantpr=1                                                    
            par(1)=10.                                                  
         end if                                                         
      end if                                                            
      if(iantpr.eq.1)then                                               
         kl2=iabs(k(i,2))                                               
       if(kl2.eq.41.or.kl2.eq.42.or.(kl2.ge.61.and.kl2.le.64)) then     
         ifyes=1                                                        
       else                                                             
         ifyes=0                                                        
       end if                                                           
       if(kopt1.eq.1.and.ifyes.eq.0) then                               
         if(ntry.lt.20) then                                            
            go to 230                                                   
         else                                                           
            kopt1=0                                                     
            par(1)=par1                                                 
         end if                                                         
       else if(kopt1.eq.1.and.ifyes.ne.0) then                          
         iantpr=0                                                       
         par(1)=par1                                                    
       end if                                                           
      end if                                                            
      lfl3=iabs(ifl(3))                                                 
      if(ihisbm.eq.2.and.lfl3.eq.3) then                                
        go to 230                                                       
      else if(ihisbm.eq.3.and.lfl3.ne.3) then                           
        go to 230                                                       
      end if                                                            
      pmq(3)=ulmass(2,ifl(3))                                           
      wmin=par(22+mst(4))+pmq(1)+pmq(2)+par(26)*pmq(3)                  
      if(iflj(jt).ne.0.and.mqj.eq.0) wmin=wmin+ulmass(2,iflj(jt))       
      if(iflj(jr).ne.0) wmin=wmin+ulmass(2,iflj(jr))                    
      if(iabs(ifl(jt)).gt.100) wmin=wmin+par(26)*(ulmass(2,mst(33))-    
     &   pmq(3))                                                        
      wrem2=four(n+nrs,n+nrs)                                           
      if(iop7.eq.1.or.ipun.eq.1) go to 460                              
      if(wrem2.lt.0.10) goto 230                                        
      if(iopt(7).eq.2.and.iprtno.eq.1.and.wrem2.gt.4.0) then            
      else                                                              
     &  if(wrem2.lt.max(wmin*(1.+(2.*ranf(0)-1.)*par(27)),              
     &                          par(22)+pmq(1)+pmq(2))**2) then         
        goto 460                                                        
      end if                                                            
      if(mst(4).eq.1.and.max(mod(iabs(ifl(1)),10),mod(iabs(ifl(2)),     
     &10)).ge.4) then                                                   
        wm1=max(par(22),pmq(1))**2                                      
        wm2=max(par(22),pmq(2))**2                                      
        gamh=0.5*(wrem2-wm1-wm2-sqrt((wrem2-wm1-wm2)**2-4.*wm1*wm2))*   
     &  par(32)/(1.+par(31))                                            
        if(gamh.lt.1..and.0.5*gamh**(1.+par(31)).gt.ranf(0)) goto 460   
        if(gamh.ge.1..and.0.4+0.1*gamh.gt.ranf(0)) goto 460             
      endif                                                             
      if(iresex.ne.0) then                                              
        p(i,5)=embar                                                    
      else if(mmult(iprtno).ne.0) then                                  
        minw=mmult(iprtno)                                              
        p(i,5)=getms4(minw,k(i,2),0,-1.0)                               
      else                                                              
        p(i,5)=ulmass(1,k(i,2))                                         
      end if                                                            
      pr(jt)=p(i,5)**2+(px(jt)+px(3))**2+(py(jt)+py(3))**2              
      if(io4qq.eq.1.and.iprtno.eq.2) then                               
        go to 230                                                       
      end if                                                            
      if(iopt(4).eq.1.and.mqj.eq.0.and.io4qq.eq.0) then                 
        if(iabs(ifl(3)).eq.3) then                                      
          if(ranf(0).gt.par(79)) go to 230                              
        end if                                                          
        call luzdis(0,1,0.,z)                                           
        zqq=1.0-z                                                       
        par(51)=paraqs                                                  
        par(52)=paraqs                                                  
        call luzdis(0,1,0.,z)                                           
        z=zqq*z                                                         
        zqq=zqq-z                                                       
        par(51)=par51                                                   
        par(52)=par52                                                   
        ifljcq=iflj(jt)                                                 
        if(ifljcq.eq.0) then                                            
           call errex('lusysj:ifljcq=0')                                
        end if                                                          
      else if(iopt(4).eq.1.and.mqj.eq.1) then                           
        iopt(4)=2                                                       
        if(zqq.eq.0.0) then                                             
          call errex('lusysj:uninitialized value of zqq')               
        end if                                                          
        z=zqq                                                           
        z=z/zpm(jt)                                                     
        if(z.ge.1.0) go to 230                                          
      else if(iopt(3).eq.1.and.jt.eq.jtopt3) then                       
        call luzdis(0,1,0.,z)                                           
        z=1.0-z                                                         
        iopt(3)=2                                                       
      else if(iopt(5).eq.1.and.iprtno.eq.1) then                        
        call luzdis(0,1,0.,z)                                           
        if(iabs(ifl(jt)).gt.10) then                                    
          par(51)=paraqq                                                
          par(52)=paraq                                                 
        else                                                            
          par(51)=paraqs                                                
          par(52)=paraqs                                                
        end if                                                          
        call luzdis(0,1,0.,zint)                                        
        par(51)=par51                                                   
        par(52)=par52                                                   
        call luzdis(ifl(jt),0,pr(jt),z)                                 
        z=1.0-(1.0-z)*zint                                              
      else if(mqj.eq.0) then                                            
        call luzdis(ifl(jt),0,pr(jt),z)                                 
      elseif(mqj.eq.1) then                                             
        gamji=(1.+par(35))/par(36)                                      
        zbc=(pr(jt)-prji-z*gamji+prji/z)/(2.*gamji)                     
        z=sqrt(zbc**2+pr(jt)/gamji)-zbc                                 
        if(iopt(3).ne.0.or.iopt(4).ne.0.or.                             
     a              iopt(5).ne.0) then                                  
          call errex('lusysj: this z branch should not be called')      
        end if                                                          
      endif                                                             
      if(iopt(4).ne.0) then                                             
        istfl=1                                                         
        if(iflj(jt).ne.0.and.mqj.eq.0) then                             
          if(ifjsid.gt.0) zj(ifjsid)=1.0                                
        end if                                                          
      end if                                                            
      if(icqq.eq.2.and.jt.eq.ifjsid.and.icqqop.eq.0) then               
        icqqop=1                                                        
        z=pr(jt)**2/(emstr*emstr*z)                                     
        if(z.ge.1.0) go to 230                                          
      end if                                                            
      if(lrkjt.ne.0) then                                               
        if(icqq.ne.2) then                                              
          z=pr(jt)**2/(emstr*emstr*zpm(3-jt)*z)                         
          if(z.ge.1.0) go to 230                                        
        end if                                                          
      end if                                                            
      gam(3)=(1.-z)*(gam(jt)+pr(jt)/z)                                  
      do 340 j=1,3                                                      
  340 in(j)=in(3*jt+j)                                                  
      if(in(1)+1.eq.in(2).and.z*p(in(1)+2,3)*p(in(2)+2,3)*              
     &p(in(1),5)**2.ge.pr(jt)) then                                     
        p(in(jt)+2,4)=z*p(in(jt)+2,3)                                   
        p(in(jr)+2,4)=pr(jt)/(p(in(jt)+2,4)*p(in(1),5)**2)              
        do 350 j=1,4                                                    
  350   p(i,j)=(px(jt)+px(3))*p(in(3),j)+(py(jt)+py(3))*p(in(3)+1,j)    
        goto 420                                                        
      elseif(in(1)+1.eq.in(2)) then                                     
        p(in(jr)+2,4)=p(in(jr)+2,3)                                     
        p(in(jr)+2,jt)=1.                                               
        in(jr)=in(jr)+4*js                                              
        if(js*in(jr).gt.js*in(4*jr)) goto 230                           
        if(four(in(1),in(2)).le.1e-2) then                              
          p(in(jt)+2,4)=p(in(jt)+2,3)                                   
          p(in(jt)+2,jt)=0.                                             
          in(jt)=in(jt)+4*js                                            
        endif                                                           
      endif                                                             
  360 if(js*in(1).gt.js*in(3*jr+1).or.js*in(2).gt.js*in(3*jr+2).or.     
     &in(1).gt.in(2)) goto 230                                          
      if(in(1).ne.in(3*jt+1).or.in(2).ne.in(3*jt+2)) then               
        do 370 j=1,4                                                    
        dp(1,j)=p(in(1),j)                                              
        dp(2,j)=p(in(2),j)                                              
        dp(3,j)=0.                                                      
  370   dp(4,j)=0.                                                      
        dp(1,4)=dsqrt(dp(1,1)**2+dp(1,2)**2+dp(1,3)**2)                 
        dp(2,4)=dsqrt(dp(2,1)**2+dp(2,2)**2+dp(2,3)**2)                 
        hc12=dfour(1,2)                                                 
        if(hc12.le.1e-2) then                                           
          p(in(jt)+2,4)=p(in(jt)+2,3)                                   
          p(in(jt)+2,jt)=0.                                             
          in(jt)=in(jt)+4*js                                            
          goto 360                                                      
        endif                                                           
        in(3)=n+nr+4*ns+5                                               
        dp(5,1)=dp(1,1)/dp(1,4)-dp(2,1)/dp(2,4)                         
        dp(5,2)=dp(1,2)/dp(1,4)-dp(2,2)/dp(2,4)                         
        dp(5,3)=dp(1,3)/dp(1,4)-dp(2,3)/dp(2,4)                         
        if(dp(5,1)**2.le.dp(5,2)**2+dp(5,3)**2) dp(3,1)=1.              
        if(dp(5,1)**2.gt.dp(5,2)**2+dp(5,3)**2) dp(3,3)=1.              
        if(dp(5,2)**2.le.dp(5,1)**2+dp(5,3)**2) dp(4,2)=1.              
        if(dp(5,2)**2.gt.dp(5,1)**2+dp(5,3)**2) dp(4,3)=1.              
        hcx1=dfour(3,1)/hc12                                            
        hcx2=dfour(3,2)/hc12                                            
        hcxx=1./dsqrt(1.+2.*hcx1*hcx2*hc12)                             
        hcy1=dfour(4,1)/hc12                                            
        hcy2=dfour(4,2)/hc12                                            
        hcyx=hcxx*(hcx1*hcy2+hcx2*hcy1)*hc12                            
        hcyy=1./dsqrt(1.+2.*hcy1*hcy2*hc12-hcyx**2)                     
        do 380 j=1,4                                                    
        dp(3,j)=hcxx*(dp(3,j)-hcx2*dp(1,j)-hcx1*dp(2,j))                
        p(in(3),j)=dp(3,j)                                              
  380   p(in(3)+1,j)=hcyy*(dp(4,j)-hcy2*dp(1,j)-hcy1*dp(2,j)-           
     &  hcyx*dp(3,j))                                                   
        px(3)=-(px(4)*four(in(3*jt+3),in(3))+py(4)*                     
     &  four(in(3*jt+3)+1,in(3)))                                       
        py(3)=-(px(4)*four(in(3*jt+3),in(3)+1)+py(4)*                   
     &  four(in(3*jt+3)+1,in(3)+1))                                     
        if(abs(px(3)**2+py(3)**2-px(4)**2-py(4)**2).gt.0.01) then       
          px(3)=px(4)                                                   
          py(3)=py(4)                                                   
        endif                                                           
      endif                                                             
      do 400 j=1,4                                                      
      hg(j)=0.                                                          
      p(i,j)=px(jt)*p(in(3*jt+3),j)+py(jt)*p(in(3*jt+3)+1,j)+           
     &px(3)*p(in(3),j)+py(3)*p(in(3)+1,j)                               
      do 390 in1=in(3*jt+1),in(1)-4*js,4*js                             
  390 p(i,j)=p(i,j)+p(in1+2,3)*p(in1,j)                                 
      do 400 in2=in(3*jt+2),in(2)-4*js,4*js                             
  400 p(i,j)=p(i,j)+p(in2+2,3)*p(in2,j)                                 
      hm(1)=four(i,i)                                                   
      hm(2)=2.*four(i,in(1))                                            
      hm(3)=2.*four(i,in(2))                                            
      hm(4)=2.*four(in(1),in(2))                                        
                                                                        
      do 410 in2=in(1)+1,in(2),4                                        
      do 410 in1=in(1),in2-1,4                                          
      hc=2.*four(in1,in2)                                               
      hg(1)=hg(1)+p(in1+2,jt)*p(in2+2,jt)*hc                            
      if(in1.eq.in(1)) hg(2)=hg(2)-js*p(in2+2,jt)*hc                    
      if(in2.eq.in(2)) hg(3)=hg(3)+js*p(in1+2,jt)*hc                    
  410 if(in1.eq.in(1).and.in2.eq.in(2)) hg(4)=hg(4)-hc                  
                                                                        
      hs1=hm(jr+1)*hg(4)-hm(4)*hg(jr+1)                                 
      if(abs(hs1).lt.1e-4) goto 230                                     
      hs2=hm(4)*(gam(3)-hg(1))-hm(jt+1)*hg(jr+1)-hg(4)*                 
     &(p(i,5)**2-hm(1))+hg(jt+1)*hm(jr+1)                               
      hs3=hm(jt+1)*(gam(3)-hg(1))-hg(jt+1)*(p(i,5)**2-hm(1))            
      p(in(jr)+2,4)=0.5*(sqrt(max(0.,hs2**2-4.*hs1*hs3))/abs(hs1)-      
     &hs2/hs1)                                                          
      if(hm(jt+1)+hm(4)*p(in(jr)+2,4).le.0.) goto 230                   
      p(in(jt)+2,4)=(p(i,5)**2-hm(1)-hm(jr+1)*p(in(jr)+2,4))/           
     &(hm(jt+1)+hm(4)*p(in(jr)+2,4))                                    
                                                                        
      if(p(in(jr)+2,4).gt.p(in(jr)+2,3)) then                           
        p(in(jr)+2,4)=p(in(jr)+2,3)                                     
        p(in(jr)+2,jt)=1.                                               
        in(jr)=in(jr)+4*js                                              
        if(js*in(jr).gt.js*in(4*jr)) goto 230                           
        if(four(in(1),in(2)).le.1e-2) then                              
          p(in(jt)+2,4)=p(in(jt)+2,3)                                   
          p(in(jt)+2,jt)=0.                                             
          in(jt)=in(jt)+4*js                                            
        endif                                                           
        goto 360                                                        
      elseif(p(in(jt)+2,4).gt.p(in(jt)+2,3)) then                       
        p(in(jt)+2,4)=p(in(jt)+2,3)                                     
        p(in(jt)+2,jt)=0.                                               
        in(jt)=in(jt)+4*js                                              
        goto 360                                                        
      endif                                                             
  420 do 430 j=1,4                                                      
      p(i,j)=p(i,j)+p(in(1)+2,4)*p(in(1),j)+p(in(2)+2,4)*p(in(2),j)     
  430 p(n+nrs,j)=p(n+nrs,j)-p(i,j)                                      
      jfl3=ifl(3)                                                       
      if(jt.eq.1) then                                                  
        jfljt=jfl1                                                      
      else                                                              
        jfljt=jfl2                                                      
      end if                                                            
      call lupos(z,zact,emht,i,jt,mqj,jfl3,jfljt)                       
      zr=emht*emht/(emstr*emstr*zact*zpm(jt))                           
      zpm(jt)=(1.-zact)*zpm(jt)                                         
      zpm(3-jt)=zpm(3-jt)-zr                                            
      if(p(i,4).le.0.) goto 230                                         
      if(kreflc.eq.1) then                                              
        ifl(jt)=ifl(3-jt)                                               
        ifl(3-jt)=-ifl(3)                                               
        ifl(3)=-ifl(jt)                                                 
        if(lrk(3-jt).eq.0.and.ifjsid.eq.3-jt) ifjsid=jt                 
      end if                                                            
      ifl(jt)=-ifl(3)                                                   
      pmq(jt)=pmq(3)                                                    
      if(mqj.eq.1) iflj(jt)=0                                           
      zpos(jt)=(1.-z)*zpos(jt)                                          
      px(jt)=-px(3)                                                     
      py(jt)=-py(3)                                                     
      gam(jt)=gam(3)                                                    
      if(in(3).ne.in(3*jt+3)) then                                      
        do 440 j=1,4                                                    
        p(in(3*jt+3),j)=p(in(3),j)                                      
  440   p(in(3*jt+3)+1,j)=p(in(3)+1,j)                                  
      endif                                                             
      do 450 jq=1,2                                                     
      in(3*jt+jq)=in(jq)                                                
      p(in(jq)+2,3)=p(in(jq)+2,3)-p(in(jq)+2,4)                         
  450 p(in(jq)+2,jt)=p(in(jq)+2,jt)-js*(3-2*jq)*p(in(jq)+2,4)           
      goto 330                                                          
  460 continue                                                          
      if(max(iabs(ifl(jr)),iabs(ifl(3))).gt.100) goto 230               
      if(iopt(7).eq.1) then                                             
      else if(iopt(6).gt.0.and.iprtno.eq.1) then                        
        if(iabs(ifl(3)).eq.3.and.                                       
     a                 ranf(0).gt.0.6666667) goto 230                   
      end if                                                            
      jfl3=ifl(3)                                                       
      if(jt.eq.1) then                                                  
        jfla=jfl1                                                       
        jflb=jfl2                                                       
      else                                                              
        jfla=jfl2                                                       
        jflb=jfl1                                                       
      end if                                                            
      mmult(iprtno+1)=0                                                 
      do 470 jf=jt,jr,js                                                
      if(jf.eq.jr) i=i+1                                                
      if(jf.eq.jr) lrk(jf)=lrk(jf)+1                                    
      if(jf.eq.jr) k(i,1)=ie(jf)                                        
      if(jf.eq.jr) iprtno=iprtno+1                                      
      if(jf.eq.jt.and.(1-mqj)*iflj(jf).eq.0) then                       
      elseif(jf.eq.jr.and.iflj(jf).eq.0) then                           
        if(min(iabs(ifl(jf)),iabs(ifl(3))).gt.10) goto 230              
        if(iop7.eq.1) then                                              
          k(i,2)=kfbar                                                  
        else                                                            
          call luifld(ifl(jf),0,-ifl(3),ifldmp,k(i,2))                  
        end if                                                          
        if(iopt(9).ne.0) then                                           
          if(iopt(9).eq.2.and.(iopt(3).eq.1.or.iopt(4).eq.1)            
     a                                     .and.ifjsid.ne.0) then       
            angmix=angeff                                               
          else                                                          
            angmix=angmxv                                               
          end if                                                        
          if(k(i,2).eq.34.or.k(i,2).eq.35) then                         
            probch=0.6667*(sin(angmix)-cos(angmix)/sqrt(2.0))**2        
            if(k(i,2).eq.34) then                                       
              if(ranf(0).lt.probch) k(i,2)=35                           
            else if(k(i,2).eq.35) then                                  
              if(ranf(0).lt.probch) k(i,2)=34                           
            end if                                                      
          end if                                                        
        end if                                                          
      else                                                              
        if(iabs(ifl(jf)).gt.100) goto 230                               
        if(iflj(jf).ne.0.and.iabs(ifl(jf)).gt.10) goto 230              
        ifl3=0                                                          
        if(jf.eq.jr) ifl3=-ifl(3)                                       
        if((iabs(ifl(jf)).gt.10.or.iflj(jf).ne.0).and.iabs(ifl3).gt.10) 
     &  goto 230                                                        
        call luifld(ifl(jf),iflj(jf),ifl3,ifl(3),k(i,2))                
        if(k(i,2).eq.0) goto 230                                        
        mmult(iprtno)=0                                                 
        if(jf.eq.ifjsid.and.inol.eq.0.and.jf.eq.jr) then                
            idlj=idlj+1                                                 
        end if                                                          
        if(jf.eq.jr.and.jf.ne.ifjsid) istore=istore+1                   
        if(i.gt.istore) then                                            
           if(jf.eq.1) then                                             
              ibrtag=2                                                  
           else                                                         
              ibrtag=1                                                  
           end if                                                       
           icqbar=ifstor(jf)                                            
        end if                                                          
        if(iflj(jf).ne.0.and.mqj*jf.ne.jt) px(jf)=px(jf)+pxj(jf)        
        if(iflj(jf).ne.0.and.mqj*jf.ne.jt) py(jf)=py(jf)+pyj(jf)        
        if(iflj(jf).ne.0.and.n.gt.ie(jf).and.k(ie(jf)+1,1)/10000.eq.6)  
     &  p(ie(jf)+1,2)=i-nrs                                             
      endif                                                             
      if(jf.eq.jr.and.iflj(jf).ne.0) then                               
          mqjj=1                                                        
      else if(iflj(jf).ne.0.and.mqj.eq.0) then                          
          mqj=1                                                         
      else                                                              
          mqjj=0                                                        
      end if                                                            
      if(iop7.eq.1) then                                                
        if(jf.eq.jt) then                                               
          p(i,5)=emmes                                                  
          mmult(1)=kmltme                                               
        else                                                            
          p(i,5)=embar                                                  
          mmult(2)=kmltba                                               
        end if                                                          
      else if(iresex.ne.0.and.jf.eq.jt) then                            
        k(i,2)=kfbar                                                    
        p(i,5)=embar                                                    
        mmult(iprtno)=kmltba                                            
      else if(mmult(iprtno).ne.0) then                                  
        minw=mmult(iprtno)                                              
        p(i,5)=getms4(minw,k(i,2),0,-1.0)                               
      else                                                              
        p(i,5)=ulmass(1,k(i,2))                                         
      end if                                                            
      if(jf.eq.jt) pr(jf)=p(i,5)**2+(px(jf)+px(3))**2+(py(jf)+py(3))**2 
      if(jf.eq.jr) pr(jf)=p(i,5)**2+(px(jf)-px(3))**2+(py(jf)-py(3))**2 
  470 continue                                                          
      jq=1                                                              
      if(p(in(4)+2,3)*p(in(5)+2,3)*four(in(4),in(5)).lt.p(in(7),3)*     
     &p(in(8),3)*four(in(7),in(8))) jq=2                                
      hc12=four(in(3*jq+1),in(3*jq+2))                                  
      hr1=four(n+nrs,in(3*jq+2))/hc12                                   
      hr2=four(n+nrs,in(3*jq+1))/hc12                                   
      if(in(4).ne.in(7).or.in(5).ne.in(8)) then                         
        px(3-jq)=-four(n+nrs,in(3*jq+3))-px(jq)                         
        py(3-jq)=-four(n+nrs,in(3*jq+3)+1)-py(jq)                       
        pr(3-jq)=p(i+(jt+jq-3)**2-1,5)**2+(px(3-jq)+(2*jq-3)*js*        
     &  px(3))**2+(py(3-jq)+(2*jq-3)*js*py(3))**2                       
      endif                                                             
      wrem2=wrem2+(px(1)+px(2))**2+(py(1)+py(2))**2                     
      hd=(sqrt(pr(1))+sqrt(pr(2)))/sqrt(wrem2)                          
      if(iop7.eq.1.and.hd.ge.1.) then                                   
        write(8,*) 'wrem2,pr(1),pr(2)=',wrem2,pr(1),pr(2)               
        write(8,*) sqrt(wrem2),sqrt(pr(1)),sqrt(pr(2))                  
        call errex('lusysj:iop7=1 energy too small for final two')      
      end if                                                            
      if(hd.ge.1.) goto 230                                             
      if(isot.ge.0.and.iprtno.eq.2) then                                
        if(iop7.eq.1) call errex('lusysj:isot/iop7 conflict')           
        iso1=isosp(iabs(k(i-1,2)))                                      
        i31=isign(1,k(i-1,2))*isosp3(iabs(k(i-1,2)))                    
        iso2=isosp(iabs(k(i,2)))                                        
        i32=isign(1,k(i,2))*isosp3(iabs(k(i,2)))                        
        punis=cgks(iso1,iso2,isot,i31,i32)                              
        ipun=2                                                          
        if(ranf(0).gt.punis) go to 230                                  
      end if                                                            
      if(iopt(7).ne.1.and.                                              
     a         (iopt(3).eq.1.or.iopt(4).eq.1)) then                     
        call luzdis(0,1,0.,z)                                           
        z=1.0-z                                                         
        if(z.lt.0.5) then                                               
          prvrs=1.                                                      
        else                                                            
          prvrs=0.                                                      
        end if                                                          
      else if(iopt(5).ne.0.and.iprtno.le.3) then                        
        prvrs=0.                                                        
      else if(icqq.eq.1.and.iprtno.le.3) then                           
        prvrs=0.0                                                       
      else if(iopt(7).ne.1.and.                                         
     a        lrk(1).ne.lrk(2).and.                                     
     a        ((lrk(1).eq.1.and.iprbcq.ne.1).or.                        
     b         (lrk(2).eq.1.and.iprbcq.ne.2))) then                     
        if(lrk(1).eq.1) then                                            
          prvrs=0.                                                      
        else                                                            
          prvrs=1.                                                      
        end if                                                          
      else                                                              
         prvrs=0.5*hd**par(27+mst(4))                                   
      end if                                                            
      ha=wrem2+pr(jt)-pr(jr)                                            
      vzdir=js*(ranf(0)-prvrs)                                          
      hb=sign(sqrt(max(0.,ha**2-4.*wrem2*pr(jt))),                      
     &                                         vzdir)                   
      if(max(mod(iabs(ifl(1)),10),mod(iabs(ifl(2)),10)).ge.6)           
     &  hb=sign(sqrt(max(0.,ha**2-4.*wrem2*pr(jt))),float(js))          
      do 480 j=1,4                                                      
       p(i-1,j)=(px(jt)+px(3))*p(in(3*jq+3),j)+(py(jt)+py(3))*          
     &  p(in(3*jq+3)+1,j)+0.5*(hr1*(ha+hb)*p(in(3*jq+1),j)+             
     &  hr2*(ha-hb)*p(in(3*jq+2),j))/wrem2                              
       p(i,j)=p(n+nrs,j)-p(i-1,j)                                       
  480 continue                                                          
      if(mbst.eq.1) then                                                
        mst(1)=n+nrs+1                                                  
        mst(2)=i                                                        
        call lurobo(0.,0.,ps(1)/pebst,ps(2)/pebst,ps(3)/pebst)          
        mst(1)=0                                                        
        mst(2)=0                                                        
      endif                                                             
      if(igpar.ne.0) then                                               
        igparh=mgpar(iabs(k(i,2)))                                      
        igpara=igpara*igparh                                            
        if(igparh.eq.0) igparb=0                                        
        if(igpara.ne.igparb) then                                       
          go to 230                                                     
        end if                                                          
      end if                                                            
      if(jt.eq.1) then                                                  
        jtact=3                                                         
      else                                                              
        jtact=-3                                                        
      end if                                                            
      call lupos(0.,zact,emht,i-1,jtact,mqj,jfl3,jfla)                  
      call lupos(0.,zact,emht,i,4,mqjj,-jfl3,jflb)                      
      ni=i-nrs                                                          
      do 490 i=n+1,n+np                                                 
  490 k(k(i,1),1)=k(k(i,1),1)+20000                                     
      do 500 i=n+1,ni                                                   
      k(i,1)=k(i+nrs,1)                                                 
      k(i,2)=k(i+nrs,2)                                                 
      do 500 j=1,5                                                      
  500 p(i,j)=p(i+nrs,j)                                                 
      if(mst(22).ge.1) then                                             
        if(2*ni-n.ge.mst(30)-5-mst(31)) then                            
          mst(24)=mst(24)+1                                             
          mst(25)=1                                                     
          if(mst(23).ge.1) return                                       
        endif                                                           
        do 510 i=n+1,ni                                                 
        k(i-n+ni,1)=k(i,1)                                              
        k(i-n+ni,2)=k(i,2)                                              
        do 510 j=1,5                                                    
  510   p(i-n+ni,j)=p(i,j)                                              
        i1=n                                                            
        do 530 i=ni+1,2*ni-n                                            
        if(k(i,1).ne.ie(1)) goto 530                                    
        i1=i1+1                                                         
        k(i1,1)=k(i,1)                                                  
        k(i1,2)=k(i,2)                                                  
        do 520 j=1,5                                                    
  520   p(i1,j)=p(i,j)                                                  
  530   continue                                                        
        do 550 i=2*ni-n,ni+1,-1                                         
        if(k(i,1).eq.ie(1)) goto 550                                    
        i1=i1+1                                                         
        k(i1,1)=k(i,1)                                                  
        k(i1,2)=k(i,2)                                                  
        do 540 j=1,5                                                    
  540   p(i1,j)=p(i,j)                                                  
  550   continue                                                        
      endif                                                             
      if(mst(22).ge.2.and.iflstr.ne.0) then                             
        do 560 i=n+1,n+iflstr                                           
        k(i-n+ni,1)=k(i,1)                                              
        k(i-n+ni,2)=k(i,2)                                              
        do 560 j=1,5                                                    
  560   p(i-n+ni,j)=p(i,j)                                              
        do 570 i=n+1,ni                                                 
        k(i,1)=k(i+iflstr,1)                                            
        k(i,2)=k(i+iflstr,2)                                            
        do 570 j=1,5                                                    
  570   p(i,j)=p(i+iflstr,j)                                            
      endif                                                             
      n=ni                                                              
      return                                                            
      end                                                               
       subroutine lupos(z,zact,emht,iact,jt,mqj,jfl3,jfljt)             
      dimension xpos(0:5,100),xprev(2,2),phd(5),pall(4)                 
      dimension jtstor(100)                                             
      common /luspbk/ pall,iqqsp,ifirst,ibegin,istart,istfl             
      common /lujets/ n,k(2000,2),p(2000,5)                             
      common /lupobk/ xpos,ilund                                        
      common /luopt1/                                                   
     a         iopt(20),kfstr,icqq,mbfix,ideltq,ibbcol,                 
     b         mmult(100),                                              
     a         iprbcq,ilead,iback,ibrtag,icqbar,idlj,klmold,            
     b         klbold,isot,misot,lbreak(100),emst,emda,emdb             
      common /prinbk/ iprin                                             
      dimension ltagqq(100)                                             
      dimension plchd(2,100)                                            
      save xprev,jerror,pplus,pminus,pp,pm,ifnl2,icqend,                
     a     jt1f,ibar,xpold,xmold,xplus,xminus,deltf,deltb,              
     b     jlead,jback,jtstor,ltagqq,plchd,plc1f,jtdif                  
      data isignl/0/                                                    
      ifljt=iabs(jfljt)                                                 
      ifl3=iabs(jfl3)                                                   
      phd(1)=p(iact,1)                                                  
      phd(2)=p(iact,2)                                                  
      phd(3)=p(iact,3)                                                  
      phd(4)=p(iact,4)                                                  
      phd(5)=p(iact,5)                                                  
      if(iabs(jt).eq.3) then                                            
         if(jt.eq.3) then                                               
           jtdif=0                                                      
         else                                                           
           jtdif=1                                                      
         end if                                                         
         jt=3                                                           
      else if(jt.eq.4) then                                             
      else                                                              
         jtdif=0                                                        
      end if                                                            
      if(iact.eq.istart) ibegin=0                                       
      if(ifirst.eq.0) then                                              
         ifirst=1                                                       
         istart=iact                                                    
      end if                                                            
      if(ibegin.eq.0) then                                              
         jerror=0                                                       
         ibegin=1                                                       
         pplus=pall(4)+pall(3)                                          
         pminus=pall(4)-pall(3)                                         
         pm=pminus                                                      
         pp=pplus                                                       
         ilund=0                                                        
         xprev(1,1)=pplus                                               
         xprev(2,1)=0.                                                  
         xprev(1,2)=0.                                                  
         xprev(2,2)=pminus                                              
         xpold=0.                                                       
         xmold=0.                                                       
         if(jt.eq.3) then                                               
           ifnl2=1                                                      
         else                                                           
           ifnl2=0                                                      
         end if                                                         
         icqend=mod(3-iprbcq,3)                                         
         if(iopt(2).gt.1.and.icqend.ne.1.and.icqend.ne.0) then          
            call errex('wrong icqend in lupos')                         
         end if                                                         
         deltf=0.                                                       
         deltb=0.                                                       
         jlead=0                                                        
         ilead=0                                                        
         jback=0                                                        
         iback=0                                                        
         jt1f=0                                                         
         ibar=0                                                         
      end if                                                            
      ilund=ilund+1                                                     
      if(jt.eq.4) then                                                  
          jtact=2-jtdif                                                 
          isignl=1                                                      
      else if(jt.eq.3) then                                             
          jtact=1+jtdif                                                 
      else                                                              
          jtact=jt                                                      
      end if                                                            
      if(mqj.eq.1) then                                                 
        if(jt.eq.4) jtact=1                                             
        ibar=ibar+1                                                     
      else if(iqqsp.eq.0.and.jtact.eq.1.and.ibar.eq.0) then             
        ibar=ibar+1                                                     
      end if                                                            
      jtstor(ilund)=jtact                                               
      emhtsq=phd(5)*phd(5)+phd(1)*phd(1)+phd(2)*phd(2)                  
      emht=sqrt(emhtsq)                                                 
      pphd=phd(4)+phd(3)                                                
      pmhd=phd(4)-phd(3)                                                
      if(jtact.eq.1) then                                               
         if(pphd.lt.0.0.or.pplus.lt.0.0) then                           
           if(jerror.eq.1) then                                         
             write(8,*) 'pphd or pplus <0 in lupos  ??????'             
             write(8,*) pmhd,pphd,(phd(i),i=1,5)                        
             write(8,*) 'pplus,pminus,xmold,xpold'                      
             write(8,*)  pplus,pminus,xmold,xpold                       
           end if                                                       
           pplus=max(pphd,pplus)                                        
           jerror=1                                                     
         end if                                                         
         if(pphd.gt.pplus+0.001) then                                   
           if(jerror.eq.1) then                                         
             write(8,*) 'pphd>pplus in lupos  ??????'                   
             write(8,*) pmhd,pphd,(phd(i),i=1,5)                        
             write(8,*) 'pplus,pminus,xmold,xpold'                      
             write(8,*)  pplus,pminus,xmold,xpold                       
           end if                                                       
           zact=1.                                                      
           jerror=1                                                     
         else                                                           
             zact=pphd/pplus                                            
         end if                                                         
         deltp=zact*pplus                                               
         xplus=xpold+(1.-zact)*pplus                                    
         pplus=(1.-zact)*pplus                                          
         deltm=emhtsq/deltp                                             
         pminus=pminus-deltm                                            
         xminus=xmold+deltm                                             
         xmold=xmold+deltm                                              
         if(xmold.gt.1.001*pm) then                                     
             write(8,*) 'xmold>pm in lupos??????'                       
             write(8,*) 'pp,pm,xmold,xpold'                             
             write(8,*)  pp,pm,xmold,xpold                              
         end if                                                         
      else if(jtact.eq.2) then                                          
         if(pmhd.lt.0.0.or.pminus.lt.0.0) then                          
           if(jerror.eq.1) then                                         
             write(8,*) 'pmhd or pminus <0 in lupos  ??????'            
             write(8,*) pmhd,pphd,(phd(i),i=1,5)                        
             write(8,*) 'pplus,pminus,xmold,xpold'                      
             write(8,*)  pplus,pminus,xmold,xpold                       
           end if                                                       
           pminus=max(pmhd,pminus)                                      
           jerror=1                                                     
         end if                                                         
         if(pmhd.gt.pminus+0.001) then                                  
           if(jerror.eq.1) then                                         
             write(8,*) 'pmhd>pminus in lupos  ??????'                  
             write(8,*) pmhd,pphd,(phd(i),i=1,5)                        
             write(8,*) 'pplus,pminus,xmold,xpold'                      
             write(8,*)  pplus,pminus,xmold,xpold                       
           end if                                                       
           zact=1.                                                      
           jerror=1                                                     
         else                                                           
             zact=pmhd/pminus                                           
         end if                                                         
         deltm=zact*pminus                                              
         xminus=xmold+(1.-zact)*pminus                                  
         pminus=(1.-zact)*pminus                                        
         deltp=emhtsq/deltm                                             
         pplus=pplus-deltp                                              
         xplus=xpold+deltp                                              
         xpold=xpold+deltp                                              
         if(xpold.gt.1.001*pp) then                                     
             write(8,*) 'xpold>pp in lupos??????'                       
             write(8,*) 'pp,pm,xmold,xpold'                             
             write(8,*)  pp,pm,xmold,xpold                              
         end if                                                         
      else                                                              
            write(8,*) '??????????? in lupos  jtact=',jtact             
      end if                                                            
      if(ilund.gt.100) then                                             
          write(8,*) 'ilund too large in lupos'                         
          call errex('array overflow in lupos')                         
      end if                                                            
      plchd(1,ilund)=deltp                                              
      plchd(2,ilund)=deltm                                              
      xpos(4,ilund)=emht                                                
      xpos(1,ilund)=0.                                                  
      xpos(2,ilund)=0.                                                  
      if((ifl3.gt.10.or.ifljt.gt.10).and.                               
     a   (ifl3.lt.10.or.ifljt.lt.10)) then                              
        ltagqq(ilund)=1                                                 
      else                                                              
        ltagqq(ilund)=0                                                 
      end if                                                            
      if(iopt(2).le.1.or.ifnl2.eq.1) then                               
        if(iopt(2).eq.0.or.ifnl2.eq.1) then                             
          xphad=0.5*(xprev(1,jtact)+ xplus)                             
          xmhad=0.5*(xprev(2,jtact)+ xminus)                            
        else                                                            
          if(jtact.eq.1) then                                           
             xmhad=xprev(2,1)                                           
             xphad=xplus                                                
          else                                                          
             xphad=xprev(1,2)                                           
             xmhad=xminus                                               
          endif                                                         
        endif                                                           
        if(xphad.gt.pp.or.xphad.lt.0.) then                             
          write(8,*) 'xphad is nonsense  xphad,pp=',xphad,pp            
        end if                                                          
        if(xmhad.gt.pm.or.xmhad.lt. 0.) then                            
            write(8,*) 'xmhad is nonsense  xmhad,pm=',xmhad,pm          
        end if                                                          
        xprev(1,jtact)=xplus                                            
        xprev(2,jtact)=xminus                                           
        xpos(0,ilund)=0.5*(xphad+xmhad)                                 
        xpos(3,ilund)=0.5*(xphad-xmhad)                                 
        if(iopt(8).eq.1) then                                           
          if(ltagqq(ilund).eq.1) then                                   
            xpos(0,ilund)=xpos(0,ilund)+0.5*(deltp+deltm)               
            xpos(3,ilund)=xpos(3,ilund)+0.5*(deltp-deltm)               
          end if                                                        
        end if                                                          
      else                                                              
        xp=xplus-deltb                                                  
        xm=xminus-deltf                                                 
        if(icqend.ne.1.and.jtact.eq.2.and.jback.eq.0) then              
          jback=1                                                       
          iback=ilund                                                   
          deltb=deltp                                                   
          if(iopt(2).eq.2) then                                         
            xphad=0.5*deltp                                             
            xmhad=0.5*deltm                                             
          else                                                          
            xphad=0.                                                    
            xmhad=0.                                                    
          end if                                                        
          do 49 il=1,ilund-1                                            
           if(il.eq.ilead) go to 49                                     
           xpos(0,il)=xpos(0,il)-deltb                                  
   49     continue                                                      
        else if(icqend.ne.2.and.jtact.eq.1.and.jlead.eq.0               
     a            .and.(mqj.eq.1.or.istfl.eq.0.or.iqqsp.eq.0))then      
          jlead=1                                                       
          ilead=ilund                                                   
          deltf=deltm                                                   
          plc1f=0.0                                                     
          do 50 il=1,ilund-1                                            
           if(il.eq.iback) go to 50                                     
           if(jt1f.gt.0.and.jtstor(il).eq.1) then                       
             xpos(0,il)=xpos(0,il)-deltp                                
             plc1f=plc1f+plchd(1,il)                                    
           else if(jtstor(il).eq.1) then                                
             write(8,*) 'lupos-warning:jt1f=0,jtstor=1 before'          
           else                                                         
             xpos(3,il)=xpos(3,il)-deltm                                
           end if                                                       
   50     continue                                                      
          if(icqend.eq.0.and.icqq.ne.2) then                            
            xphad=pp-(deltp+plc1f)                                      
          else                                                          
            if(iopt(2).eq.2) then                                       
              xphad=0.5*deltp                                           
            else                                                        
              xphad=0.                                                  
            end if                                                      
          end if                                                        
          if(iopt(2).eq.2) then                                         
            xmhad=0.5*deltm                                             
          else                                                          
            xmhad=0.                                                    
          end if                                                        
        else                                                            
          if(jtact.eq.1.and.jlead.eq.0                                  
     a         .and.mqj.eq.0.and.istfl.eq.1) jt1f=jt1f+1                
          xphad=xp                                                      
          xmhad=xm                                                      
        end if                                                          
        xpos(0,ilund)=xphad                                             
        xpos(3,ilund)=xmhad                                             
      end if                                                            
      if(isignl.eq.1 ) then                                             
         if(istfl.eq.1.and.ibar.ne.1) then                              
            write(8,*) 'wrong ibar for baryon string in lupos',ibar     
            call errex('inconsistent baryon fragmentation')             
         end if                                                         
         if(iopt(2).ge.2.and.ifnl2.eq.0) then                           
           if(iopt(5).gt.0) then                                        
             if(icqend.ne.1)                                            
     a                call errex('lupos:icqend.ne.1,iopt(5)>0')         
             icqend=0                                                   
             xprev(1,2)=0.                                              
             xprev(2,2)=pm-deltf                                        
           end if                                                       
            if(jlead.gt.0) then                                         
              if(icqend.eq.0.and.icqq.ne.2) then                        
                if(ilund.gt.jback+1+jt1f) then                          
                  xpos(0,ilead)=xpos(0,ilead)-deltb                     
                else                                                    
                  if(iopt(2).eq.2) then                                 
                    xpos(0,ilead)=0.5*plchd(1,ilead)                    
                  else                                                  
                    xpos(0,ilead)=0.                                    
                  end if                                                
                end if                                                  
              end if                                                    
              xprev(1,1)=pp-(plchd(1,ilead)+deltb)                      
              xprev(2,1)=0.                                             
            else                                                        
              xprev(1,1)=pp-deltb                                       
              xprev(2,1)=0.                                             
            end if                                                      
            if(iback.gt.0) then                                         
              xprev(1,2)=0.                                             
              xprev(2,2)=pm-(plchd(2,iback)+deltf)                      
            else                                                        
              xprev(2,2)=pm-deltf                                       
              xprev(1,2)=0.                                             
            end if                                                      
           do 70 il=1,ilund                                             
            jtact=jtstor(il)                                            
            if(jtact.eq.1) then                                         
              deltxp=0.5*(xprev(1,1)-xpos(0,il))                        
              deltxm=0.5*(xpos(3,il)-xprev(2,1))                        
            else                                                        
              deltxp=0.5*(xpos(0,il)-xprev(1,2))                        
              deltxm=0.5*(xprev(2,2)-xpos(3,il))                        
            end if                                                      
            if(il.eq.ilead.or.il.eq.iback) then                         
              xphad=xpos(0,il)                                          
              xmhad=xpos(3,il)                                          
              if(il.eq.ilead) then                                      
                xprev(1,1)=pp-(plchd(1,il)+deltb+plc1f)                 
                xprev(2,1)=0.                                           
              end if                                                    
            else if(jt1f.gt.0.and.jtact.eq.1) then                      
              jt1f=jt1f-1                                               
              if(iopt(2).eq.2) then                                     
                xphad=0.5*plchd(1,il)                                   
                xmhad=0.5*plchd(2,il)                                   
              else                                                      
                xphad=0.                                                
                xmhad=0.                                                
              end if                                                    
            else                                                        
              if(iopt(2).eq.2) then                                     
                xphad=0.5*(xprev(1,jtact)+ xpos(0,il))                  
                xmhad=0.5*(xprev(2,jtact)+ xpos(3,il))                  
              else                                                      
                if(jtact.eq.1) then                                     
                  xmhad=xprev(2,1)                                      
                  xphad=xpos(0,il)                                      
                else                                                    
                  xphad=xprev(1,2)                                      
                  xmhad=xpos(3,il)                                      
                end if                                                  
              end if                                                    
              xprev(1,jtact)=xpos(0,il)                                 
              xprev(2,jtact)=xpos(3,il)                                 
              if(deltxp.le.0.0.or.deltxm.le.0.0) then                   
                write(8,*)                                              
     a            'lupos:deltxp.or.deltxm.le.0.0',deltxp,deltxm         
                jerror=2                                                
              end if                                                    
              inkrem=0                                                  
              if(ilead.eq.0.or.iback.eq.0) inkrem=1                     
              if(ilund.eq.3-inkrem) then                                
                xmhad=xmhad+deltf                                       
              end if                                                    
            end if                                                      
            if(iopt(8).eq.1.and.ilund.gt.3-inkrem) then                 
              if(ltagqq(il).eq.1) then                                  
                xphad=xphad+deltxp                                      
                xmhad=xmhad+deltxm                                      
              end if                                                    
            end if                                                      
            xpos(0,il)=0.5*(xphad+xmhad)                                
            xpos(3,il)=0.5*(xphad-xmhad)                                
   70      continue                                                     
         end if                                                         
         isignl=0                                                       
         ifirst=0                                                       
         ibegin=0                                                       
         istart=0                                                       
         if(jerror.eq.1) then                                           
            write(8,*) 'jerror=1 in lupos energy and/or momentum'       
            write(8,*) 'is violated. ???????????????????'               
         else if(jerror.eq.2) then                                      
            write(8,*) 'jerror=2 in lupos'                              
         end if                                                         
      end if                                                            
      return                                                            
      end                                                               
      function  kresm(jqqb,igfix,idn,minv,maxv,kfmes,emmes)             
       common /ludat1/ mst(40),par(80)                                  
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       if(idn.lt.0.or.idn.gt.8)                                         
     a      call errex('kresm: wrong input idn')                        
       if(maxv.lt.0.or.maxv.gt.mxmsta(idn))                             
     a      call errex('kresm: wrong input maxv')                       
       if(minv.gt.maxv)                                                 
     a      call errex('kresm: minv>maxv')                              
       vmin=float(minv)                                                 
       vmax=float(maxv+1)                                               
       itry=0                                                           
   33  continue                                                         
       itry=itry+1                                                      
       if(itry.gt.100)  then                                            
         write(8,*) jqqb,igfix,idn,minv,maxv,kfmes                      
         call errex('kresm abort: itry>100')                            
       end if                                                           
       if(maxv.eq.minv) then                                            
         kmult=maxv                                                     
       else                                                             
         kmult=minv+int((vmax-vmin)*ranf(0))                            
         if(kmult.gt.maxv) kmult=maxv                                   
       end if                                                           
       if(idn.le.4) then                                                
         strrat=0.2*(jspmes(kmult,idn)+1.)                              
       else                                                             
         strrat=0.333333*(jspmes(kmult,idn)+1.)                         
       end if                                                           
       if(ranf(0).gt.strrat)  go to 33                                  
       if(jqqb.eq.0) then                                               
         if(idn.le.4) then                                              
           angmx=anglmx(kmult,1)/180.0*par(71)                          
         else                                                           
           angmx=anglmx(kmult,2)/180.0*par(71)                          
         end if                                                         
         kfmold=kfmes                                                   
         if(kfmes.eq.24.or.kfmes.eq.25) then                            
           probch=0.66667*(cos(angmx)+sin(angmx)/sqrt(2.0))**2          
           if(ranf(0).lt.probch) kfmes=24+25-kfmes                      
         else if(kfmes.eq.34.or.kfmes.eq.35) then                       
           probch=0.66667*(sin(angmx)-cos(angmx)/sqrt(2.0))**2          
           if(ranf(0).lt.probch) kfmes=34+35-kfmes                      
         end if                                                         
         if(kfmold.ne.kfmes) then                                       
           if(kfmes.eq.24) then                                         
             idn=2                                                      
           else if(kfmes.eq.25) then                                    
             idn=3                                                      
           else if(kfmes.eq.34) then                                    
             idn=6                                                      
           else if(kfmes.eq.35) then                                    
             idn=7                                                      
           end if                                                       
         end if                                                         
       end if                                                           
       if(kmult.eq.0) then                                              
         emmes=ulmass(1,kfmes)                                          
       else                                                             
         emres=emmesr(kmult,idn)                                        
         widres=gammes(kmult,idn)                                       
         emmin=emmmin(idn)                                              
         cr=2.*(emmin-emres)/widres                                     
         cr1=atan(cr)                                                   
         cr2=par(71)/2.-cr1                                             
         emmx=2.8                                                       
         if(emmx.lt.emmin)                                              
     a           call errex('too low maximum m mass in kresm')          
         xmax=(atan(2.*(emmx-emres)/widres)-cr1)/ cr2                   
         xmax=min(1.0,xmax)                                             
         mtry=0                                                         
   10    continue                                                       
         xx=xmax*ranf(0)                                                
         t=tan(xx*cr2)                                                  
         emmes=emres+.5*widres*(cr+t)/(1.-cr*t)                         
         if(igfix.eq.1) then                                            
           prbmxa=3.*widres/((emmes-emres)**2+0.25*widres**2)           
           gamr=dcrm(kmult,kfmes,emmes)                                 
           if(gamr.le.0.0) go to 10                                     
           prbact=gamr/((emmes-emres)**2+0.25*gamr**2)                  
           mtry=mtry+1                                                  
           if(mtry.lt.30.and.ranf(0).gt.prbact/prbmxa) go to 10         
           if(mtry.eq.30)                                               
     a         write(8,*) 'warning:mtry=30,kresm'                       
         end if                                                         
       end if                                                           
       kresm=kmult                                                      
      return                                                            
      end                                                               
      function kresb(igfix,idn,minv,maxv,kfbar,embar)                   
       common /ludat1/ mst(40),par(80)                                  
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       if(idn.le.0.or.idn.gt.8)                                         
     a      call errex('kresb: wrong input idn')                        
       if(minv.gt.maxv)                                                 
     a      call errex('kresb: minv>maxv')                              
       vmax=float(maxv+1)                                               
       vmin=float(minv)                                                 
       itry=0                                                           
   33  continue                                                         
       itry=itry+1                                                      
       if(itry.gt.100)  call errex('kresb abort: itry>100')             
       if(maxv.eq.minv) then                                            
         kmult=maxv                                                     
       else                                                             
         kmult=minv+int((vmax-vmin)*ranf(0))                            
         if(kmult.gt.maxv) kmult=maxv                                   
       end if                                                           
       if(kmult.lt.minv.or.kmult.gt.min(maxv,mxbsta(idn)))              
     a      call errex('kresb: wrong kmult')                            
       strrat=0.125*(jspbar(kmult,idn)+1.)                              
       if(ranf(0).gt.strrat)  go to 33                                  
       if(kmult.eq.0) then                                              
         embar=ulmass(1,kfbar)                                          
       else                                                             
         emres=embarr(kmult,idn)                                        
         widres=gambar(kmult,idn)                                       
         emmin=embmin(idn)                                              
         emmin=max(emmin,emres-2.*widres)                               
         emmx=2.8                                                       
         if(emmx.lt.emmin)                                              
     a           call errex('too low maximum b mass in kresb')          
         cr=2.*(emmin-emres)/widres                                     
         cr1=atan(cr)                                                   
         cr2=par(71)/2.-cr1                                             
         xmax=(atan(2.*(emmx-emres)/widres)-cr1)/ cr2                   
         xmax=min(1.0,xmax)                                             
         mtry=0                                                         
   10    continue                                                       
         xx=xmax*ranf(0)                                                
         t=tan(xx*cr2)                                                  
         embar=emres+.5*widres*(cr+t)/(1.-cr*t)                         
         if(igfix.eq.1) then                                            
           prbmxa=3.*widres/((embar-emres)**2+0.25*widres**2)           
           gamr=dcr(kmult,kfbar,embar)                                  
           if(gamr.le.0.0) then                                         
             call errex('kresb:dcr abort')                              
           end if                                                       
           prbact=gamr/((embar-emres)**2+0.25*gamr**2)                  
           mtry=mtry+1                                                  
           if(mtry.lt.30.and.ranf(0).gt.prbact/prbmxa) go to 10         
           if(mtry.eq.30)                                               
     a         write(8,*) 'warning:mtry=30,kresb'                       
         end if                                                         
       end if                                                           
       kresb=kmult                                                      
      return                                                            
      end                                                               
      subroutine kfindb(kswi,ifla,iflb,iflc,ifld,kfbar)                 
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       common /wghblk/ flvwgh(8)                                        
       parameter (kbart=18)                                             
       dimension icon(kbart),iwgh(kbart)                                
       dimension ip(4),iq(3)                                            
       data icon/                                                       
     a           41,42,43,44,45,46,47,57,61,                            
     b           62,63,64,65,66,67,68,69,70                             
     c           /                                                      
       data iwgh/                                                       
     a           2,2,4,4,4,5,5,3,1,1,1,1,6,6,6,7,7,8/                   
       dimension  iba(4)                                                
       mfla=ifla                                                        
       mflb=iflb                                                        
       mflc=iflc                                                        
       if(mfla.lt.0) then                                               
          mfla=-mfla                                                    
          mflb=-mflb                                                    
          mflc=-mflc                                                    
          ivz=-1                                                        
       else                                                             
          ivz=1                                                         
       end if                                                           
       kfbar=0                                                          
       mtry=0                                                           
  402  continue                                                         
       mtry=mtry+1                                                      
       if(mtry.gt.600) then                                             
         call errex('kfindb abort: mtry.gt.300')                        
       end if                                                           
       lb=min(kbart,int(ranf(0)*kbart)+1)                               
       ilb=icon(lb)                                                     
       call luiflv(ilb,ilbfla,ilbflb,ilbflc,ksp)                        
       istart=int(ranf(0)*3.0)                                          
       isucc=0                                                          
       iqnew=0                                                          
       probqp=0.0                                                       
       do 401 ieff=istart,istart+2                                      
        ifl=mod(ieff,3)+1                                               
        iq(1)=ilbfla                                                    
        iq(2)=ilbflb                                                    
        iq(3)=ilbflc                                                    
        ip(1)=ifl                                                       
        ip(2)=mfla                                                      
        ip(3)=mflb                                                      
        ip(4)=mflc                                                      
        do 397 k=1,4                                                    
        do 397 j=1,3                                                    
         if(ip(k).eq.iq(j)) then                                        
           ip(k)=0                                                      
           iq(j)=0                                                      
         end if                                                         
  397   continue                                                        
        if(iq(1)+iq(2)+iq(3).eq.0) then                                 
          if(kswi.ne.1.and.(ip(2)+ip(3).ne.0                            
     a                      .or.ip(1).ne.0))go to 401                   
          kfbar=ilb                                                     
          if(ip(1).ne.0) then                                           
            if(ifl.eq.3.and.ranf(0).gt.ssbdif) go to 401                
            nflb=10*mfla+mflb                                           
            nfla=ifl                                                    
            iqnew=ifl                                                   
            probqp=probqp+1.0                                           
            iflc=1                                                      
          else if(ip(2).ne.0) then                                      
            nfla=mfla                                                   
            nflb=10*mflb+mflc                                           
            iqnew=ifl                                                   
            probqp=probqp+1.0                                           
            iflc=0                                                      
          else if(ip(3).ne.0) then                                      
            nfla=mflb                                                   
            nflb=10*mfla+mflc                                           
            iqnew=ifl                                                   
            probqp=probqp+1.0                                           
            iflc=0                                                      
          else if(ip(4).ne.0) then                                      
            nflb=10*mfla+mflb                                           
            nfla=mflc                                                   
            iqnew=ifl                                                   
            probqp=probqp+1.0                                           
            iflc=0                                                      
          else                                                          
            call errex('garbage in kfindb')                             
          end if                                                        
          isucc=1                                                       
        end if                                                          
  401  continue                                                         
       if(isucc.eq.0) go to 402                                         
       if(ranf(0).gt.probqp*flvwgh(iwgh(lb))) go to 402                 
       if(kfbar.eq.0)  call errex('kfindb:kfbar=0')                     
       kfbar=ivz*kfbar                                                  
       ifla=ivz*nfla                                                    
       iflb=ivz*nflb                                                    
       ifld=ivz*iqnew                                                   
      return                                                            
      end                                                               
      function idnbar(kf)                                               
        kfbabs=iabs(kf)                                                 
        if(kfbabs.ge.41.and.kfbabs.le.42) then                          
          idnbar=2                                                      
        else if(kfbabs.ge.61.and.kfbabs.le.64) then                     
          idnbar=1                                                      
        else if(kfbabs.eq.57) then                                      
          idnbar=3                                                      
        else if(kfbabs.ge.43.and.kfbabs.le.45) then                     
          idnbar=4                                                      
        else if(kfbabs.ge.46.and.kfbabs.le.47) then                     
          idnbar=5                                                      
        else if(kfbabs.ge.65.and.kfbabs.le.67) then                     
          idnbar=6                                                      
        else if(kfbabs.ge.68.and.kfbabs.le.69) then                     
          idnbar=7                                                      
        else if(kfbabs.eq.70) then                                      
          idnbar=8                                                      
        else                                                            
          idnbar=0                                                      
        end if                                                          
      return                                                            
      end                                                               
      function idnmes(kf)                                               
       kfa=iabs(kf)                                                     
       if(kfa.eq.17.or.kfa.eq.23) then                                  
         idnmes=1                                                       
       else if(kf.eq.24) then                                           
         idnmes=2                                                       
       else if(kf.eq.25) then                                           
         idnmes=3                                                       
       else if(kfa.eq.18.or.kfa.eq.19) then                             
         idnmes=4                                                       
       else if(kfa.eq.27.or.kfa.eq.33) then                             
         idnmes=5                                                       
       else if(kf.eq.34) then                                           
         idnmes=6                                                       
       else if(kf.eq.35) then                                           
         idnmes=7                                                       
       else if(kfa.eq.28.or.kfa.eq.29) then                             
         idnmes=8                                                       
       else                                                             
         call errex('wrong input in idnmes')                            
       end if                                                           
      return                                                            
      end                                                               
      subroutine notsto(iswi,                                           
     a            kfbar,kfmes,kmltba,kmltme,                            
     b            iflb,ifla,ifl3,kdfr,embar,emmes)                      
        save                                                            
     a            lfbar,lfmes,lmltba,lmltme,                            
     b            jflb,jfla,jfl3,ldfr,fmbar,fmmes                       
       data lfbar /0/                                                   
       if(iswi.ne.0) then                                               
         lfbar=kfbar                                                    
         lfmes=kfmes                                                    
         lmltba=kmltba                                                  
         lmltme=kmltme                                                  
         jflb=iflb                                                      
         jfla=ifla                                                      
         jfl3=ifl3                                                      
         ldfr=kdfr                                                      
         fmbar=embar                                                    
         fmmes=emmes                                                    
       else                                                             
         if(lfbar.eq.0) call errex('notsto-abort')                      
         kfbar=lfbar                                                    
         kfmes=lfmes                                                    
         kmltba=lmltba                                                  
         kmltme=lmltme                                                  
         iflb=jflb                                                      
         ifla=jfla                                                      
         ifl3=jfl3                                                      
         kdfr=ldfr                                                      
         embar=fmbar                                                    
         emmes=fmmes                                                    
         lfbar=0                                                        
       end if                                                           
      return                                                            
      end                                                               
      block data resdat                                                 
       common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),         
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       data embarr /                                                    
     a  1.23,1.60,1.62,1.70,1.90,1.905,1.910,1.92,1.93,1.95,2*0.0,      
     b  0.938,1.44,1.52,1.535,1.65,1.675,1.680,1.7,1.71,1.72,1.99,0.0,  
     c  1.116,1.407,1.52,1.60,1.675,1.69,1.80,1.81,1.82,1.83,1.89,1.90, 
     d  1.192,1.66,1.67,1.75,1.775,1.80,1.85,1.86,1.87,1.915,2*0.0,     
     e  1.318,1.74,1.82,1.835,1.945,1.95,2.00,2.01,2.02,2.040,2*0.0,    
     f  1.385,1.75,1.77,1.94,2.03,2.05,2.055,2.06,2.07,2.08,2*0.0,      
     g  1.533,1.90,1.92,2.00,2.13,2.20,2.205,2.21,2.22,2.23,2*0.0,      
     h  1.672,2.05,2.07,2.15,2.35,2.355,2.36,2.37,2.38,2.40,2*0.0       
     i             /                                                    
       data gambar /                                                    
     a   0.12,0.25,0.16,0.280,0.15,0.300,0.220,0.22,0.25,0.24,2*0.0,    
     b   -1.0,0.20,0.125,0.15,0.15,0.155,0.125,0.1,0.11,0.20,0.29,0.0,  
     c   -1.0,.050,.0155,.15,.045,.046,.30,.15,.062,.095,.10,.085,      
     d   -1.0,0.10,0.06,0.09,0.12,0.127,0.085,0.093,0.170,0.120,2*0.0,  
     e   -1.0,0.124,.025,.076,.084,.093,.062,.068,.124,.059,2*0.0,      
     f   0.036,0.212,0.136,0.22,0.18,0127,0.255,0.187,0.212,0.212,2*0.0,
     g   0.0095,0.155,.099,.174,.046,.093,.186,.136,.155,.155,2*0.0,    
     h   .054,.112,.072,.126,.067,.135,.099,.112,.112,.108,2*0.0        
     i             /                                                    
       data embmin /1.079,1.079,1.340,1.340,1.460,1.340,1.460,1.820/    
       data nstatb /                                                    
     a               4,8,10,14,16,22,24,28,34,42,2*0,                   
     b               2,4, 8,10,12,18,24,28,30,34,42,0,                  
     c               2,4,8,10,12,16,18,20,26,32,36,40,                  
     d               2,4,8,10,16,18,22,24,28,34,2*0,                    
     e               2,4,8,10,16,18,22,24,28,34,2*0,                    
     f               4,8,10,14,22,24,30,32,36,42,2*0,                   
     g               4,8,10,14,22,24,30,32,36,42,2*0,                   
     h               4,8,10,14,16,22,24,28,34,42,2*0                    
     i             /                                                    
       data mxbsta /9,9,11,9,9,9,9,9/                                   
       data jspbar /                                                    
     a               3,3,1,3,1,5,1,3,5,7,2*0,                           
     b               1,1,3,1,1,5,5,3,1,3,7,0,                           
     c               1,1,3,1,1,3,1,1,5,5,3,3,                           
     d               1,1,3,1,5,1,3,1,3,5,2*0,                           
     e               1,1,3,1,5,1,3,1,3,5,2*0,                           
     f               3,3,1,3,7,1,5,1,3,5,2*0,                           
     g               3,3,1,3,7,1,5,1,3,5,2*0,                           
     h               3,3,1,3,1,5,1,3,5,7,2*0                            
     i             /                                                    
       data emmesr /                                                    
     a   0.138,0.980,1.260,1.318,8*0.0,                                 
     b   0.550,1.400,1.420,1.270,8*0.0,                                 
     c   0.958,0.975,1.510,1.525,8*0.0,                                 
     d   0.495,1.430,1.270,1.430,8*0.0,                                 
     e   0.770,-1.0, 1.233, -1.0,8*0.0,                                 
     f   0.783,-1.0, 1.170, -1.0,8*0.0,                                 
     g   1.020,-1.0, 1.510, -1.0,8*0.0,                                 
     h   0.895,-1.0, 1.400, -1.0,8*0.0                                  
     i             /                                                    
       data gammes /                                                    
     a   -1.0,   0.057,0.138,0.110,8*0.0,                               
     b   -1.0,   0.600,0.055,0.185,8*0.0,                               
     c   2.8e-4, 0.053,0.035,0.076,8*0.0,                               
     d   -1.0,   0.287,0.090,0.098,8*0.0,                               
     e   0.154,  -1.0, 0.150, -1.0,8*0.0,                               
     f   0.0099, -1.0, 0.311, -1.0,8*0.0,                               
     g   0.00421,-1.0, 0.035, -1.0,8*0.0,                               
     h   0.0508, -1.0, 0.174, -1.0,8*0.0                                
     i             /                                                    
       data emmmin /0.69,0.4,1.0,0.64,0.69,0.4,1.0,1.1/                 
       data nstatm /                                                    
     a               1,2,5,10,8*0,                                      
     b               1,2,5,10,8*0,                                      
     c               1,2,5,10,8*0,                                      
     d               1,2,5,10,8*0,                                      
     e               3,3,6,6,8*0,                                       
     f               3,3,6,6,8*0,                                       
     g               3,3,6,6,8*0,                                       
     h               3,3,6,6,8*0                                        
     i             /                                                    
       data mxmsta /3,3,3,3,2,2,2,2/                                    
       data jspmes /                                                    
     a               0, 0, 2, 4,8*0,                                    
     b               0, 0, 2, 4,8*0,                                    
     c               0, 0, 2, 4,8*0,                                    
     d               0, 0, 2, 4,8*0,                                    
     e               2,-1, 2,-1,8*0,                                    
     f               2,-1, 2,-1,8*0,                                    
     g               2,-1, 2,-1,8*0,                                    
     h               2,-1, 2,-1,8*0                                     
     i             /                                                    
       data anglmx /                                                    
     a          -18.0,-54.74,-54.74,-54.74,8*0.0,                       
     b           41.0, 0.0 ,35.26,  0.0,8*0.0                           
     c             /                                                    
       data qqbdif,ssbdif /0.0,0.3/                                     
      end                                                               
      function prcms(a,b,c)                                             
       if(b+c.gt.a)                                                     
     a      call errex('prcms-abort')                                   
       prcms=sqrt((a**2-(b+c)**2)*(a**2-(b-c)**2))/(2.*a)               
      return                                                            
      end                                                               
      function pssbar(ida55,pr)                                         
       if(ida55.eq.0) then                                              
         pssbar=0.2                                                     
       else                                                             
         pssbar=0.3                                                     
       end if                                                           
      return                                                            
      end                                                               
      subroutine bresdc(imany,                                          
     a                   iflfb,iflfa,ichobr,nbfix,                      
     b                   kswi,ida53,ida55,klbol,klmol,emstr,emda,emdb,  
     c                   kfbar,kfmes,kmltba,kmltme,                     
     d                   iflb,ifla,ifl3,kdfr,embar,emmes,ptx,pty        
     e                 )                                                
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
        common /bresbk/ embarr(0:11,8),gambar(0:11,8),embmin(8),        
     a                 emmesr(0:11,8),gammes(0:11,8),emmmin(8),         
     b                 nstatb(0:11,8), mxbsta(8),jspbar(0:11,8),        
     c                 nstatm(0:11,8), mxmsta(8),jspmes(0:11,8),        
     d                 anglmx(0:11,2),                                  
     e                 qqbdif,ssbdif                                    
       common /ludat1/ mst(40),par(80)                                  
       dimension  jentbx(8),jentmx(8),flwwgh(8)                         
       common /wghblk/ flvwgh(8)                                        
       dimension imbout(5,8)                                            
       data  (imbout(1,i),i=1,8) /                                      
     a         1,1,2,2,0,2,0,0/                                         
       data  (imbout(2,i),i=1,8) /                                      
     b         2,2,3,1,2,1,2,0/                                         
       data  (imbout(3,i),i=1,8) /                                      
     b         2,2,1,1,2,1,2,0/                                         
       data  (imbout(4,i),i=1,8) /                                      
     c         0,0,2,2,1,2,1,2/                                         
       data  (imbout(5,i),i=1,8) /                                      
     d         0,0,0,0,2,0,2,1/                                         
       dimension icho(6)                                                
       data icho / 1,1,2,3,4,5 /                                        
        mbfix=nbfix                                                     
        klmold=klmol                                                    
        klbold=klbol                                                    
        if(ida55.gt.0) then                                             
          if(emda+emdb.gt.emstr) then                                   
            write(8,*) 'bresdc-warning:imag.prold',emstr,emda,emdb      
            ida55=0                                                     
            mbfix=0                                                     
            klbold=0                                                    
            klmold=0                                                    
            write(8,*) 'ida55,mbfix,klbold,klmold',                     
     a                  ida55,mbfix,klbold,klmold,'set to 0'            
            prold=0.0                                                   
          else                                                          
            prold=prcms(emstr,emda,emdb)                                
          end if                                                        
        else                                                            
          prold=-1.0                                                    
        end if                                                          
        imany=0                                                         
        em2nur=2.8                                                      
        jgfix=1                                                         
        if(ichobr.le.2) then                                            
          mflt=0                                                        
        else if(ichobr.eq.3.or.ichobr.eq.4) then                        
          mflt=1                                                        
        else if(ichobr.eq.5) then                                       
          mflt=2                                                        
        else if(ichobr.eq.6) then                                       
          mflt=3                                                        
        end if                                                          
        em2nur=em2nur+mflt*0.15                                         
        if(ichobr.eq.2.or.ichobr.eq.5) then                             
          isotot=1                                                      
        else if(ichobr.eq.1) then                                       
          isotot=3                                                      
        else if(ichobr.eq.3) then                                       
          isotot=0                                                      
        else if(ichobr.eq.4) then                                       
          isotot=2                                                      
        else if(ichobr.eq.6) then                                       
          isotot=0                                                      
        end if                                                          
        nscod=0                                                         
        if(mbfix.gt.0) then                                             
          nscod=mbfix/100                                               
          mbfix=mod(mbfix,100)                                          
          idnfix=mbfix/10                                               
          jdnfix=mod(mbfix,10)                                          
        else if(-mbfix.ge.5.and.-mbfix.le.9.and.                        
     a                           emstr.lt.1.55) then                    
          if(-mbfix.eq.5) then                                          
            mbfix=61                                                    
            idnfix=6                                                    
            jdnfix=1                                                    
          else                                                          
            idnfix=9                                                    
            jdnfix=9                                                    
          end if                                                        
        else                                                            
          idnfix=0                                                      
          jdnfix=0                                                      
        end if                                                          
        do 105 idn=1,8                                                  
         jentbx(idn)=-1                                                 
         if(idnfix.eq.9) then                                           
           jentbx(idn)=0                                                
           go to 105                                                    
         end if                                                         
         if(mbfix.gt.0) then                                            
           embam=emstr                                                  
         else if(imbout(icho(ichobr),idn).eq.0) then                    
           embam=0.0                                                    
         else if(imbout(icho(ichobr),idn).eq.1) then                    
           embam=emstr-0.14                                             
         else if(imbout(icho(ichobr),idn).eq.2) then                    
           embam=emstr-0.496                                            
         else if(imbout(icho(ichobr),idn).eq.3) then                    
           embam=emstr-0.55                                             
         end if                                                         
         do 104 ms=0,mxbsta(idn)                                        
          if(embarr(ms,idn).gt.embam) go to 105                         
          jentbx(idn)=ms                                                
  104    continue                                                       
  105   continue                                                        
        stamax=0.0                                                      
        do 106 idn=1,8                                                  
         if(jentbx(idn).eq.-1) go to 106                                
         staidn=float(nstatb(jentbx(idn),idn))                          
         if(staidn.gt.stamax) then                                      
           stamax=staidn                                                
         end if                                                         
  106   continue                                                        
        if(stamax.eq.0.0)  then                                         
          call errex('bresdc:stamax=0, abort')                          
        end if                                                          
        stamax=3.*stamax                                                
        do 107 idn=1,8                                                  
         if(jentbx(idn).eq.-1) then                                     
           flvwgh(idn)=0.0                                              
           go to 107                                                    
         end if                                                         
         staidn=float(nstatb(jentbx(idn),idn))                          
         flvwgh(idn)=staidn/stamax                                      
         if(idnfix.gt.0.and.idnfix.le.8) then                           
           if(idn.ne.idnfix) then                                       
             flvwgh(idn)=0.0                                            
           else                                                         
             flvwgh(idn)=0.333333                                       
             jentbx(idn)=0                                              
           end if                                                       
         end if                                                         
  107   continue                                                        
        if(ichobr.eq.5) then                                            
          emmam=emstr-1.16                                              
        else if(ichobr.eq.6) then                                       
          emmam=emstr-1.32                                              
        else                                                            
          emmam=emstr-0.94                                              
        end if                                                          
        do 203 jdn=1,8                                                  
         jentmx(jdn)=0                                                  
         if(jdnfix.eq.9) go to 203                                      
         do 204 ms=0,mxmsta(jdn)                                        
          if(emmesr(ms,jdn)                                             
     a                                    .gt.emmam) go to 203          
          jentmx(jdn)=ms                                                
  204   continue                                                        
  203   continue                                                        
        jentmx(2)=max(jentmx(2),jentmx(3))                              
        jentmx(3)=jentmx(2)                                             
        jentmx(6)=max(jentmx(6),jentmx(7))                              
        jentmx(7)=jentmx(6)                                             
        do 205 jdn=1,8                                                  
         flwwgh(jdn)=nstatm(jentmx(jdn),jdn)                            
         if(jdnfix.gt.0.and.jdnfix.le.8) then                           
           if(jdn.ne.jdnfix) then                                       
             flwwgh(jdn)=0.0                                            
           else                                                         
             flwwgh(jdn)=1.0                                            
             jentmx(jdn)=0                                              
           end if                                                       
         end if                                                         
  205   continue                                                        
        if(jentmx(2).ne.jentmx(3).or.                                   
     a           jentmx(6).ne.jentmx(7)) then                           
          call errex('bresdc:different no. of meson states')            
        end if                                                          
        do 208 jdn=1,4                                                  
  208   flwwgh(jdn)=flwwgh(jdn)+flwwgh(4+jdn)                           
        stamax=flwwgh(1)                                                
        do 206 jdn=2,4                                                  
         stajdn=flwwgh(jdn)                                             
         if(stajdn.gt.stamax) then                                      
           stamax=stajdn                                                
         end if                                                         
  206   continue                                                        
        do 207 jdn=1,4                                                  
         flwwgh(jdn)=flwwgh(jdn)/stamax                                 
         flwwgh(jdn+4)=flwwgh(jdn+4)/stamax                             
  207   continue                                                        
        para8=par(8)                                                    
        para9=par(9)                                                    
        par(8)=0.                                                       
        par(9)=0.                                                       
        notst=0                                                         
        mtry=0                                                          
  102   continue                                                        
        mtry=mtry+1                                                     
        if(mtry.gt.800) then                                            
          if(notst.eq.0) then                                           
            if(klbold.eq.0.and.kswi.eq.1) then                          
              if(ichobr.eq.3.and.emstr.gt.1.34) then                    
                newchr=int(3.*ranf(0))-1                                
                klbold=44+newchr                                        
                klbold=isign(1,iflfa)*klbold                            
                klmold=isign(1,iflfa)*newchr*17+(1-iabs(newchr))*23     
                write(8,*) 'not:',klbold, klmold,newchr                 
              end if                                                    
            end if                                                      
            if(klbold.ne.0) then                                        
              write(8,*)'bresdc102:infinite loop?????',notst            
              kfbar=isign(1,iflfa)*mod(iabs(klbold),100)                
              kfmes=isign(1,klmold)*mod(iabs(klmold),100)               
              if(kfmes.ne.23.and.kfmes.ne.24.and.kfmes.ne.25            
     a                    .and.kfmes.ne.34.and.kfmes.ne.35) then        
                kfmes=isign(1,iflfa)*kfmes                              
              end if                                                    
              mmtry=0                                                   
 1102         continue                                                  
              mmtry=mmtry+1                                             
              if(mmtry.gt.200) then                                     
                write(8,*)                                              
     a                   iflfb,iflfa,ichobr,nbfix,                      
     b                   kswi,ida53,ida55,klbol,klmol,emstr,emda,emdb   
                call errex('bresdc:mmtry abort')                        
              end if                                                    
              emmes=getms4(0,kfmes,jgfix,emstr)                         
              embar=getms4(0,kfbar,jgfix,emstr)                         
              if(abs(embar)+abs(emmes).ge.emstr) go to 1102             
              kmltba=0                                                  
              kmltme=0                                                  
              kdfr=0                                                    
              call luiflv(kfbar,mfla,mflb,mflc,ksp)                     
              iflb=10*mfla+mflb                                         
              call luiflv(kfmes,mfla,mflb,mflc,ksp)                     
              if(mfla*iflb.gt.0) then                                   
                ifla=mfla                                               
                ifl3=mflb                                               
              else                                                      
                ifla=mflb                                               
                ifl3=mfla                                               
              end if                                                    
            else                                                        
              if(kswi.eq.2) then                                        
                write(8,*)'bresdc102:infinite loop?????',notst          
                write(8,*) 'kswi=2'                                     
                kswi=1                                                  
                mtry=0                                                  
                go to 102                                               
              end if                                                    
              write(8,*)                                                
     a          iflfb,iflfa,nbfix,emstr,                                
     b          kswi,ichobr,ida53,ida55,klbol ,klmol ,prol              
              call errex('bresdc102:infinite loop??')                   
            end if                                                      
          else                                                          
            write(8,*)'bresdc102:infinite loop?????',notst              
            call notsto(0,                                              
     a                kfbar,kfmes,kmltba,kmltme,                        
     b                iflb,ifla,ifl3,kdfr,embar,emmes)                  
          end if                                                        
          par(8)=para8                                                  
          par(9)=para9                                                  
          pr4=prcms(emstr,emmes,embar)                                  
          pt=pr4*sqrt(1.-ranf(0)**2)                                    
          phi=par(72)*ranf(0)                                           
          ptx=pt*cos(phi)                                               
          pty=pt*sin(phi)                                               
          return                                                        
        end if                                                          
        mfla=iflfb/10                                                   
        mflb=iflfb-10*mfla                                              
        mflc=iflfa                                                      
        call kfindb(kswi,mfla,mflb,mflc,mfld,kfbar)                     
        kfbabs=iabs(kfbar)                                              
        mfld=-mfld                                                      
        iflb=mflb                                                       
        ifla=mfla                                                       
        ifl3=mfld                                                       
        kdfr=mflc                                                       
        if(iabs(mfld).eq.3.and.ranf(0).gt.0.3) then                     
          go to 102                                                     
        end if                                                          
        call luifld(mfla,0,mfld,ifldmp,kfmes)                           
        if(kfmes.eq.26)  go to 102                                      
        if(iabs(mfla).eq.3) then                                        
          if(kfmes.eq.24) kfmes=25                                      
        else                                                            
          if(kfmes.eq.25) kfmes=24                                      
        end if                                                          
        kfmesa=iabs(kfmes)                                              
        iso1=isosp(kfbabs)                                              
        i31=isign(1,kfbar)*isosp3(kfbabs)                               
        iso2=isosp(kfmesa)                                              
        i32=isign(1,kfmes)*isosp3(kfmesa)                               
        punis=cgks(iso1,iso2,isotot,i31,i32)                            
        if(ranf(0).gt.punis) go to 102                                  
        jdn=idnmes(kfmes)                                               
        if(jdn.gt.4)  then                                              
          write(8,*) 'bresdc,iop7=1, unknown meson code',jdn,mfla,mfld  
          if(jdn.le.8)  then                                            
            jdn=jdn-4                                                   
            kfmes=isign(1,kfmes)*(kfmesa-10)                            
            kfmesa=iabs(kfmes)                                          
          end if                                                        
        end if                                                          
        if(ranf(0).gt.flwwgh(jdn)) go to 102                            
        if(ranf(0).lt.flwwgh(jdn+4)/flwwgh(jdn)) then                   
          jdn=jdn+4                                                     
          kfmes=isign(1,kfmes)*(kfmesa+10)                              
        end if                                                          
        maxw=jentmx(jdn)                                                
        minw=0                                                          
        igfix=0                                                         
        kmltme=kresm(0,igfix,jdn,minw,maxw,kfmes,emmes)                 
        idn=idnbar(kfbabs)                                              
        if(nscod.ne.0) then                                             
          minv=nscod                                                    
          maxv=nscod                                                    
        else if(idn.gt.0) then                                          
          minv=0                                                        
          maxv=jentbx(idn)                                              
        else                                                            
          minv=0                                                        
          maxv=0                                                        
        end if                                                          
        kmltba=kresb(igfix,idn,minv,maxv,kfbar,embar)                   
        if(notst.ne.1) then                                             
          emme0=getms4(0,kfmes,jgfix,emstr)                             
          emba0=getms4(0,kfbar,jgfix,emstr)                             
          if(abs(emba0)+abs(emme0).lt.emstr) then                       
            notst=2                                                     
            call notsto(1,                                              
     a           kfbar,kfmes,0,0,                                       
     b           iflb,ifla,ifl3,kdfr,emba0,emme0)                       
          end if                                                        
        end if                                                          
        if(emstr.lt.emmes+embar+0.001) go to 102                        
        pr4=prcms(emstr,emmes,embar)                                    
        prmb=pr4                                                        
        irgs23=0                                                        
        if(mbfix.le.0.and.ida55.gt.0) then                              
          kfms=100*isign(kmltme,kfmes)+kfmes                            
          kfbr=100*isign(kmltba,kfbar)+kfbar                            
          rgs=regsup(ida55,kfms,kfbr,klmold,klbold,mfld,kex,            
     a                       prmb,prold,emstr,pr0reg,spower,iscons)     
          if(rgs.lt.1.0) then                                           
            if(ranf(0).gt.rgs) then                                     
              if(emstr.le.em2nur) then                                  
                go to 102                                               
              else                                                      
                if(em2nur.gt.emmes+embar                                
     a                 .and.em2nur.gt.emda+emdb) then                   
                  prol1=prcms(em2nur,emda,emdb)                         
                  prmb1=prcms(em2nur,emmes,embar)                       
                  ida55d=-1000                                          
                  rgs1=regsup(ida55d,kfms,kfbr,klmold,klbold,mfld,kex,  
     a                        prmb1,prol1,em2nur,pr0reg,spower,iscons)  
                else                                                    
                  rgs1=rgs                                              
                end if                                                  
                prb2n3=rgs1-rgs                                         
                if(prb2n3.lt.-1.e-4) then                               
                  write(8,*) 'rgs,rgs1=',rgs,rgs1,emstr,prmb,prold      
                  write(8,*)                                            
     a               kfms,kfbr,klmold,klbold,mfld,kex,                  
     b                      prmb1,prol1,em2nur,spower                   
                  call errex('rgs increasing with energy')              
                end if                                                  
                if(ranf(0).lt.prb2n3) then                              
                  irgs23=1                                              
                else                                                    
                  go to 102                                             
                end if                                                  
              end if                                                    
            end if                                                      
          end if                                                        
        end if                                                          
        if(mbfix.lt.0) then                                             
          mbfi=-mbfix                                                   
          idnjdn=10*idn+jdn                                             
          if(mbfi.eq.5) then                                            
            if(kmltme.eq.0.and.kmltba.eq.0.and.                         
     a         (idnjdn.eq.24.or.((idnjdn.eq.31.or.                      
     b                              idnjdn.eq.41)                       
     c                            .and.emstr.lt.2.50                    
     d                                              )                   
     e                                             )) go to 102         
          else if(mbfi.eq.6) then                                       
            if(kmltme.eq.0.and.kmltba.eq.0.and.                         
     a                               idnjdn.eq.24) go to 102            
          else if(mbfi.eq.11) then                                      
            if(kmltme.eq.0.and.kmltba.eq.0.and.                         
     a                               idnjdn.eq.21) go to 102            
          else if(mbfi.eq.13) then                                      
            if(kmltme.eq.0.and.idnjdn.eq.21                             
     b                                      ) go to 102                 
          else if(mbfi.eq.15) then                                      
            if(kmltme.eq.0.and.                                         
     a         (idnjdn.eq.21                                            
     a           .or.(idnjdn.eq.44.and.kmltba.eq.0))) go to 102         
          else                                                          
            call errex('uninitialized mbfi in bresdc')                  
          end if                                                        
        end if                                                          
        if(ida55.eq.0.and.iabs(mfld).eq.3) then                         
          ratz=pssbar(ida55,pr4)/0.3                                    
          if(ranf(0).gt.ratz) go to 102                                 
        end if                                                          
        notst=1                                                         
        call notsto(1,                                                  
     a         kfbar,kfmes,kmltba,kmltme,                               
     b         iflb,ifla,ifl3,kdfr,embar,emmes)                         
        if(ida53.eq.0.or.mbfix.gt.0) then                               
          mttry=0                                                       
  302     continue                                                      
          par12=par(12)                                                 
          if(mbfix.gt.0) then                                           
            par(12)=min(par(12),                                        
     a          sqrt(0.66667*prcms(emstr,emmes,embar)**2))              
          end if                                                        
          call luptdi(ifla,ptx,pty)                                     
          par(12)=par12                                                 
          ptsq=ptx**2+pty**2                                            
          ksuff=0                                                       
          if(emstr.lt.sqrt(emmes**2+ptsq)+                              
     a                sqrt(embar**2+ptsq)+0.001) ksuff=1                
          if(mtry.le.400) then                                          
            if(ksuff.eq.1) go to 102                                    
          else                                                          
            mttry=mttry+1                                               
            if(mttry.lt.50) then                                        
              if(ksuff.eq.1)  go to 302                                 
            else                                                        
              if(ksuff.eq.1)  go to 102                                 
            end if                                                      
          end if                                                        
        else if(ida53.eq.1) then                                        
          avan=1.2                                                      
          kvan=2                                                        
          emtmx=2.5                                                     
          probmx=(0.5*kvan/avan)**kvan/exp(float(kvan))                 
          if(emstr.gt.2.*emtmx) then                                    
            emtm1=emtmx                                                 
            emtm2=emtmx                                                 
            probmx=min(probmx,prcms(emstr,emtm1,emtm2)**kvan*           
     a             exp(-avan*sqrt(emstr**2-4.*emtmx**2)))               
          end if                                                        
          probmx=max((0.5*emstr)**kvan*exp(-avan*emstr),probmx)         
          probac=prcms(emstr,emmes,embar)**kvan*                        
     a             exp(-avan*sqrt(emstr**2-(emmes+embar)**2))           
          if(probac.gt.probmx)                                          
     a        call errex('bresdc: probac>probmx')                       
          if(ranf(0).gt.probac/probmx) go to 102                        
        end if                                                          
        if(mbfix.le.0) then                                             
          ltry=0                                                        
  202     ltry=ltry+1                                                   
          if(ltry.gt.200) call errex('bresdc202: infinite loop?')       
          par12=par(12)                                                 
          par(12)=min(par(12),                                          
     a          sqrt(0.66667*prcms(emstr,emmes,embar)**2))              
          call luptdi(ifla,ptx,pty)                                     
          par(12)=par12                                                 
          ptsq=ptx**2+pty**2                                            
          if(emstr.lt.sqrt(emmes**2+ptsq)+                              
     a                sqrt(embar**2+ptsq)+0.001) go to 202              
        end if                                                          
        if(kmltme.gt.0) then                                            
          emmax=sqrt((emstr-sqrt(embar**2+ptsq))**2-ptsq)               
          emmes=getms4(kmltme,kfmes,jgfix,emmax)                        
          if(emmes.lt.0.0) go to 102                                    
        end if                                                          
        if(kmltba.gt.0) then                                            
          emmax=sqrt((emstr-sqrt(emmes**2+ptsq))**2-ptsq)               
          embar=getms4(kmltba,kfbar,jgfix,emmax)                        
          if(embar.lt.0.0) go to 102                                    
        end if                                                          
        par(8)=para8                                                    
        par(9)=para9                                                    
        if(mbfix.le.0.and.ida55.gt.0) then                              
          if(irgs23.eq.1) then                                          
            imany=1                                                     
            kfbar=0                                                     
            kfmes=0                                                     
            return                                                      
          end if                                                        
        end if                                                          
      return                                                            
      end                                                               
      function regsup(ida55,kfms,kfbr,klmold,klbold,mfld,kex,           
     a                    prnew,prold,emstr,pr0reg,spower,iscons)       
       common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)    
       data p0rgm,p0rgk,p0rgb,p0rgy                                     
     a      /0.80, 0.80, 1.40, 1.40/                                    
       data scons /0.5/                                                 
        if(ida55.eq.-1000) go to 44                                     
        regsup=1.0                                                      
        spower=0.0                                                      
        iscons=0                                                        
        if(ida55.eq.0) return                                           
        if(ida55.eq.2) then                                             
        else                                                            
          call errex('regsup: no valid ida55 parametrization')          
        end if                                                          
        if(mod(iabs(klmold),100).ge.40)                                 
     a  call errex('regsup: wrong klmold')                              
          kgmes=isign(1,kfms)*mod(iabs(kfms),100)                       
          kgbar=isign(1,kfbr)*mod(iabs(kfbr),100)                       
          if(kgmes.eq.23.or.kgmes.eq.24.or.kgmes.eq.25.or.              
     a                      kgmes.eq.33.or.kgmes.eq.34) then            
            iskfb=1                                                     
          else                                                          
            iskfb=isign(1,kgbar)                                        
          end if                                                        
          kfmeso=iskfb*kfms                                             
          inepsv=imspin(kfmeso)                                         
          imepsv=imspin(klmold)                                         
          klqa=iskfb*kgmes                                              
          klqb=isign(1,klmold)*mod(iabs(klmold),100)                    
          iqmm=luchge(klqa)-luchge(klqb)                                
          s0srat=1.0                                                    
          ime10=imepsv/10                                               
          ine10=inepsv/10                                               
          imcode=mod(imepsv,10)                                         
          incode=mod(inepsv,10)                                         
          spower=-1.0                                                   
          if((ime10+ine10.eq.3.and.                                     
     a                 min(ime10,ine10).gt.0)                           
     b       .or.(ime10+ine10.eq.2.and.                                 
     c                 min(ime10,ine10).eq.0                            
     d                            .and.iabs(iqmm).ge.6)                 
     e                                              ) then              
            kex=1                                                       
            pr0reg=p0rgy                                                
            spower=spower+3.8                                           
          else if(iabs(iqmm).ge.6) then                                 
            kex=2                                                       
            pr0reg=p0rgb                                                
            spower=spower+2.8                                           
            iscons=1                                                    
          else                                                          
            if(iabs(mfld).eq.3.and.ine10.eq.0) ine10=3                  
            if(iabs(mfld).ne.3.and.ine10.eq.3) ine10=0                  
            if(ime10.eq.ine10) then                                     
              pr0reg=p0rgm                                              
              if(ime10.eq.0) then                                       
                if(isosp(iabs(klqa)).eq.0.and.                          
     a                         isosp(iabs(klqb)).eq.0) then             
                  ig=0                                                  
                else                                                    
                  ig=10                                                 
                end if                                                  
                if(mgpar(iabs(klqa))*                                   
     a                       mgpar(iabs(klqb)).eq.-1) then              
                  ig=ig+1                                               
                end if                                                  
                if(ig.eq.10.or.ig.eq.1) then                            
                  spower=spower+1.0                                     
                else                                                    
                  spower=spower+2.0                                     
                end if                                                  
              else if(ime10.eq.1.or.ime10.eq.2) then                    
                if(imcode+incode.eq.0) then                             
                  spower=spower+1.0                                     
                else if(imcode+incode.eq.1) then                        
                  spower=spower+2.0                                     
                else                                                    
                  spower=spower+1.5                                     
                end if                                                  
              else                                                      
                pr0reg=p0rgb                                            
                spower=spower+3.8                                       
              end if                                                    
              kex=3                                                     
            else if(ime10+ine10.eq.2.or.                                
     a              ime10+ine10.eq.4) then                              
              pr0reg=p0rgm                                              
              if(imcode+incode.eq.0) then                               
                spower=spower+1.6                                       
              else if(imcode+incode.eq.1) then                          
                spower=spower+2.4                                       
              else                                                      
                spower=spower+2.0                                       
              end if                                                    
              kex=5                                                     
            else if(ime10+ine10.eq.3.and.                               
     a                 min(ime10,ine10).eq.0) then                      
              pr0reg=p0rgb                                              
              spower=spower+3.8                                         
              kex=7                                                     
            else                                                        
              pr0reg=p0rgk                                              
              kgb1=mod(iabs(kgbar),100)                                 
              kgb2=mod(iabs(klbold),100)                                
              kgbsum=kgb1+kgb2                                          
              kgb1=max(kgb1,kgb2)                                       
              kgb2=kgbsum-kgb1                                          
              ihyp=0                                                    
              if(kgb2.eq.41.or.kgb2.eq.42) then                         
                if(kgb1.eq.57) then                                     
                  ihyp=1                                                
                else if((kgb1.ge.43.and.kgb1.le.45).or.                 
     a                  (kgb1.ge.65.and.kgb1.le.67)) then               
                  ihyp=2                                                
                end if                                                  
              end if                                                    
              if(ihyp.eq.0) then                                        
                spower=spower+2.0                                       
              else if(ihyp.eq.1) then                                   
                spower=spower+2.4                                       
              else if(ihyp.eq.2) then                                   
                spower=spower+1.6                                       
              end if                                                    
              kex=8                                                     
              iscons=1                                                  
            end if                                                      
          end if                                                        
          if(prold.lt.0.0.or.prnew.le.0.0)                              
     a           call errex('regsup: abort,wrong prold/new')            
   44     continue                                                      
          s0srat=1./(1.+prold*prnew/pr0reg**2)                          
          regsup=s0srat**spower                                         
          if(iscons.eq.1)  regsup=scons*regsup                          
      return                                                            
      end                                                               
      function  cqpfrc(icho)                                            
       common /zdisbk/ paraqq,paraq,paraqs                              
       common /ludat1/ mst(40),par(80)                                  
        par51=par(51)                                                   
        par52=par(52)                                                   
        if(icho.gt.0) then                                              
          if(icho.eq.1) then                                            
            par(51)=paraqs                                              
            par(52)=paraqs                                              
          else if(icho.eq.2) then                                       
            par(51)=paraqq                                              
            par(52)=paraq                                               
          end if                                                        
          call luzdis(0,1,0.,z)                                         
          par(51)=par51                                                 
          par(52)=par52                                                 
          cqpfrc=z                                                      
        else                                                            
          cqpfrc=1.0                                                    
        end if                                                          
      return                                                            
      end                                                               
      function iqqrnd(iflqq)                                            
       iqqrnd=iflqq                                                     
       isig=isign(1,iflqq)                                              
       jflqq=iabs(iflqq)                                                
       if(jflqq.lt.10) return                                           
       ifl1=mod(jflqq,10)                                               
       ifl2=jflqq/10                                                    
       if(ifl1.eq.ifl2) return                                          
       zrand=ranf(0)                                                    
       if((ifl1.gt.ifl2.and.zrand.gt.0.75)                              
     a          .or.(ifl1.lt.ifl2.and.zrand.le.0.75) ) then             
         iqqrnd=isig*(10*ifl2+ifl1)                                     
       end if                                                           
      return                                                            
      end                                                               
      function mlt810(kfbar,embar)                                      
       common /ludat1/ mst(40),par(80)                                  
       mlt810=-1                                                        
       prboct=par(77)                                                   
       w1385=par(78)                                                    
       kfbabs=iabs(kfbar)                                               
       idn=idnbar(kfbabs)                                               
       if(kfbabs.gt.60) then                                            
         maxv=0                                                         
       else                                                             
         maxv=int((1.+prboct)*ranf(0))                                  
         if(idn.eq.3.and.maxv.eq.1) maxv=2                              
       end if                                                           
       if(idn.eq.6.and.maxv.eq.0.and.ranf(0).gt.w1385) then             
         if(kfbar.eq.66) then                                           
           kfbar=isign(1,kfbar)*57                                      
           maxv=1                                                       
         else                                                           
           return                                                       
         end if                                                         
       end if                                                           
       minv=maxv                                                        
       embar=getms4(maxv,kfbabs,1,-1.0)                                 
       mlt810=maxv                                                      
      return                                                            
      end                                                               
