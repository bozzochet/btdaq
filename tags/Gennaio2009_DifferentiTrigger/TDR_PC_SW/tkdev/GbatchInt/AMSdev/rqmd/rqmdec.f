CPAT,RQMDEC   .                                                         
      subroutine rqmdec(ind)                                            
       include 'scatter7'                                               
       include 'partblk7'                                               
       include 'storbig'                                                
       real*4 prtgam                                                    
      common /aludat/ prtgam(120),isosp(120),isosp3(120),mgpar(120)     
       parameter (mxs=200)                                              
       common /rop1bk/ lstar(mxs),lende(mxs),iroin,iroout               
       common /ioutbk/i1,i2,i3,i4,nop,ichanl,iexpn1,iexpn2              
     a                 ,idcid1,idcid2,iqsq                              
       common /routbk/xold1(0:3),xold2(0:3),srt,bxtot,bytot,bztot       
       logical jets                                                     
            if(decay(ind).eq.x(0,ind).and.qdot(0,ind).eq.0.) then       
                delts=tlstev-time                                       
            else if(qdot(0,ind).gt.1.e-20) then                         
                delts=max(cvalu0,                                       
     a            (decay(ind)-x(0,ind)+qdot(0,ind)*dt)/qdot(0,ind) )    
            else                                                        
                write(8,*) 'is this particle decay reasonable?'         
                write(8,*) 'qdot,decay,dt,x0',issn(ind)                 
                write(8,*) qdot(0,ind),decay(ind),dt,x(0,ind)           
                delts=dt                                                
            end if                                                      
            dtprop=delts-dt                                             
            if(-dtprop.lt.-0.001*dt) then                               
               write(8,*) 'dtprop>0 in rqmdec ???',delts,dt,dtprop,kdt  
            end if                                                      
            tnexev=time+delts                                           
            if(tnexev+1.e-2.lt.tlstev.and.ida(23).eq.0) then            
               write(8,*) 'tlstev.ge.tnexev in rqmdec???',tlstev,tnexev 
               write(8,*) irun,itripl,kdt,kclst                         
               write(8,*) 'x=',(x(i,ind),i=0,3)                         
               write(8,*)'ident=',issn(ind),ityp(ind),npidec(ind)       
               write(8,*) 'qdot=',(qdot(i,ind),i=0,3)                   
               write(8,*) 'decay=',decay(ind),'delts=',delts            
               tnexev=tlstev                                            
            end if                                                      
            tlstev=tnexev                                               
            ntag=0                                                      
            iqcnvt=0                                                    
            tact=decay(ind)                                             
            ident=issn(ind)                                             
            ityp1=ityp(ind)                                             
            npid1=npidec(ind)                                           
            em1=em(ind)                                                 
           if(itdth(4,ident).ne.0) then                                 
             itdti=itdth(4,ident)                                       
             itdtia=iabs(itdti)                                         
             itdtj=itdth(4,itdtia)                                      
             itdtja=iabs(itdtj)                                         
             if(itdtja.eq.ident) then                                   
               identj=itdtia                                            
               identi=ident                                             
               ili=ind                                                  
               ilj=0                                                    
               do 301 il=iofset+1,iofset+nrbar(kclst)+nrmes(kclst)      
                identl=issn(il)                                         
                if(identl.eq.identj) then                               
                  ilj=il                                                
                  go to 302                                             
                end if                                                  
  301          continue                                                 
  302          continue                                                 
               if(ilj.eq.0)                                             
     a         call errex('rqmdec:no ilj found, abort')                 
               ityp2=ityp(ilj)                                          
               npid2=npidec(ilj)                                        
               em2=ityp(ilj)                                            
               i1=ili                                                   
               i2=ilj                                                   
               call sveifo(i1,1)                                        
               call sveifo(i2,2)                                        
               call oldnew(1,ityp1,npid1,em1,kmult1,ltyp1)              
               call oldnew(1,ityp2,npid2,em2,kmult2,ltyp2)              
               if(ltyp1*ltyp2.ge.0) then                                
                 call errex('rqmdec: bbar-problem with ingoing types')  
               else if(ltyp1.gt.0) then                                 
                 ibry1=1                                                
               else                                                     
                 ibry1=-1                                               
               end if                                                   
               if(itdti.lt.0) then                                      
                 ind=ilj                                                
                 ident=identj                                           
               end if                                                   
               idento=identi+identj-ident                               
               ilo=ili+ilj-ind                                          
               if(ida(7).eq.1) then                                     
                 if(itdth(2,ident).ne.0) then                           
                   call ofshel(3,ind,ident)                             
                 end if                                                 
                 if(itdth(2,idento).ne.0) then                          
                   call ofshel(3,ilo,idento)                            
                 end if                                                 
               end if                                                   
               srt=0.0                                                  
               do 304 i=0,3                                             
                xold1(i)=x(i,ili)+qdot(i,ili)*dtprop                    
                xold2(i)=x(i,ilj)+qdot(i,ilj)*dtprop                    
  304          continue                                                 
               do 303 i=0,3                                             
                p(i,ind)=p(i,ind)+p(i,ilo)                              
                srt=srt+isign(1,3-2*(i+1))*p(i,ind)**2                  
                p(i,ilo)=0.0                                            
  303          continue                                                 
               nop=1                                                    
               iqsq=0                                                   
               ichanl=2                                                 
               ievntn=ievntn+1                                          
               srt=sqrt(srt)                                            
               bxtot=p(1,ind)/p(0,ind)                                  
               bytot=p(2,ind)/p(0,ind)                                  
               bztot=p(3,ind)/p(0,ind)                                  
               iexpn1=0                                                 
               iexpn2=0                                                 
               call xtupdt                                              
               ida19=ida(19)                                            
               ic=-3                                                    
               if(ibry1.lt.0) then                                      
                 call antibb(ic,ityp1,ityp2,npid1,npid2,em1,em2,srt)    
                 ityp(ind)=ityp1                                        
                 npidec(ind)=npid1                                      
               else                                                     
                 call antibb(ic,ityp2,ityp1,npid2,npid1,em2,em1,srt)    
                 ityp(ind)=ityp2                                        
                 npidec(ind)=npid2                                      
               end if                                                   
               if(ic.eq.0) call errex('rqmdec: abort after antibb call')
               ida(19)=ida19                                            
               em(ind)=srt                                              
               tlast(ind)=decay(ind)                                    
               em(ilo)=0.0                                              
               ityp(ilo)=-9999                                          
               npidec(ilo)=0                                            
               decay(ilo)=1.e+35                                        
               call output(2)                                           
               call propag(ilo,-2,0)                                    
               ikenz=5                                                  
             else                                                       
               ikenz=6                                                  
             end if                                                     
           else if(itdth(2,ident).eq.0.and.                             
     a                                itdth(1,ident).ne.0) then         
             nmin=iofset+1                                              
             nmax=iofset+nrbar(kclst)+nrmes(kclst)                      
             ident1=min(ntotma,max(1,issn(max(nmin,ind-1))))            
             ident2=min(ntotma,max(1,issn(min(nmax,ind+1))))            
             ltdt1=itdth(1,ident)                                       
             if(ltdt1.ne.itdth(1,ident1).and.                           
     a                   (ltdt1.ne.itdth(1,ident2).or.                  
     a                    ident2.eq.ident)) then                        
                if(itdth(1,ident).ge.0) then                            
                  write(8,*) ident,ind,(itdth(i,ident),i=1,2)           
                  write(8,*) ident1,(itdth(i,ident1),i=1,2)             
                  write(8,*) ident2,(itdth(i,ident2),i=1,2)             
                  call errex(                                           
     a                 'rqmdec: string particle without partner?')      
                end if                                                  
                lstart=ind                                              
                lend=ind                                                
             else                                                       
                lstart=0                                                
                do 401 l=ind,1,-1                                       
                  identl=issn(l)                                        
                  if(ltdt1.ne.itdth(1,identl)) go to 402                
                lstart=l                                                
  401           continue                                                
  402           continue                                                
                lend=0                                                  
                do 403 l=lstart,iofset+nrbar(kclst)+nrmes(kclst)        
                 identl=issn(l)                                         
                 if(identl.le.0.or.identl.gt.nprtcl) go to 404          
                 if(ltdt1.ne.itdth(1,identl)) go to 404                 
                 lend=l                                                 
  403           continue                                                
  404           continue                                                
                if(lend.le.lstart) call errex('rqmdec:lend.le.lstart')  
                if(lstart.le.iofset+nrbar(kclst))                       
     a                         call errex('rqmdec:lstart too small')    
                if(ltdt1.ne.issn(lstart))                               
     a                    call errex('rqmdec:ltdt1.ne.ident(lstart)')   
              end if                                                    
              lstar(1)=lstart                                           
              lende(1)=lend                                             
              iroin=1                                                   
              iroout=1                                                  
              call tube                                                 
              do 407 iro=1,iroout                                       
              if(lstar(iro).eq.0) go to 407                             
              do 405 l=lstar(iro),lende(iro)                            
               identl=issn(l)                                           
               nclcnt(identl)=0                                         
               if(itdth(2,identl).ne.0  )                               
     a          call errex('rqmdec:itdth(2).ne.0 for string hadron' )   
               itdth(1,identl)=0                                        
               itdth(3,identl)=0                                        
               itdth(4,identl)=0                                        
               itdth(5,identl)=0                                        
               do 406 i=0,3                                             
  406          frfp(i,identl)=0.0                                       
               if(decay(l).eq.1.e+35) then                              
                 write(8,*) issn(l),ityp(l),npidec(l),                  
     a                      iro,(x(i,l),i=0,3)                          
                 call errex('rqmdec: decay(l)=infinity')                
               end if                                                   
               delttr=decay(l)-x(0,l)                                   
               if(qdot(0,l).gt.0.) then                                 
                 do 400 i=0,3                                           
                  x(i,l)=x(i,l)+qdot(i,l)/qdot(0,l)*delttr              
  400            continue                                               
               else                                                     
                 if(abs(delttr).gt.1.e-4) then                          
                   write(8,*) issn(l),decay(l),delttr                   
                   call errex('rqmdec:delttr.ne.0,qdot(0)=0')           
                 end if                                                 
               end if                                                   
               tlast(l)=decay(l)                                        
               startt=x(0,l)                                            
               call dupdat(l,startt)                                    
               tform=tlast(l)                                           
               zform=x(3,l)                                             
               zlast(identl)=                                           
     a              zform-(pnorm(0)*tform-pnorm(3)*zform)*pnorm(3)      
               if(ida(15).ne.0) then                                    
                   ichbig(l)=3                                          
                   do 408 i=0,3                                         
  408              qdot(i,l)=0.0                                        
               end if                                                   
               call propag(l,-2,0)                                      
  405         continue                                                  
  407         continue                                                  
              return                                                    
            else if(itdth(2,ident).ne.0.and.                            
     a           tdtg(ident).eq.decay(ind)) then                        
              ikenz=6                                                   
            else if(jets(ityp1,npid1,em1,0)) then                       
              ikenz=5                                                   
            else if(ityp1.ge.12.and.ityp1.le.15) then                   
              ikenz=4                                                   
            else if(ityp1.ge.3.and.ityp1.le.6) then                     
              ikenz=1                                                   
            else if(ityp1.ge.10.and.ityp1.le.11) then                   
              ikenz=2                                                   
            else if(ityp1.ge.7.and.ityp1.le.9.and.                      
     a            npid1.gt.0) then                                      
              ikenz=3                                                   
            else if(ityp1.ge.85.and.ityp1.le.97)then                    
              ikenz=7                                                   
            else if(ityp1.eq.98)then                                    
              ikenz=5                                                   
            else if(ityp1.eq.99)then                                    
              if(-npid1.ge.61.and.-npid1.le.64) then                    
                 ikenz=8                                                
              else                                                      
                 ikenz=4                                                
              endif                                                     
            else                                                        
              write(8,*) 'wrong ityp for decay in rqmdec'               
              write(8,*)'iwhat=d  ind=',ind,'decay=',decay(ind)         
              write(8,*) 'ident,itdth'                                  
              write(8,*)  ident,(itdth(j,ident),j=1,4)                  
              write(8,*) 'ind,ityp,npidec',ind,ityp(ind),npidec(ind),   
     a                                              issn(ind)           
              decay(ind)=1.e+35                                         
              return                                                    
            end if                                                      
            if(ida(7).eq.1.and.ikenz.ne.5) then                         
              if(itdth(2,ident).ne.0  ) then                            
                 call ofshel(3,ind,ident)                               
              end if                                                    
            end if                                                      
            if(ida(15).eq.2) then                                       
              if((ikenz.ge.1.and.ikenz.le.4).or.                        
     a            (ikenz.ge.7.and.ikenz.le.8)) then                     
                if(ident.le.n1old+n2old) then                           
                  if(qdot(0,ind).gt.0.) then                            
                    delttr=decay(ind)-x(0,ind)                          
                    zdeca=x(3,ind)+qdot(3,ind)/qdot(0,ind)*delttr       
                  else                                                  
                    zdeca=x(3,ind)                                      
                  end if                                                
                  if(zdeca.lt.zmnta.or.zdeca.gt.zmxpr) ikenz=9          
                else                                                    
                  zdeca=0.0                                             
                  ikenz=9                                               
                end if                                                  
              end if                                                    
              if(itdth(2,ident).ne.0.and.                               
     a            tdtg(ident).lt.decay(ind)) tlast(ind)=tdtg(ident)     
            end if                                                      
            idm=1                                                       
            call sveifo(ind,idm)                                        
            lmes=0                                                      
            if(ikenz.eq.0) then                                         
            else if(ida(1).eq.1.and.((ikenz.eq.3.and.npidec(ind).eq.2)  
     a              .or.(ikenz.eq.1.and.                                
     b                       npidec(ind).eq.1))) then                   
              decay(ind)=1.e+35                                         
              return                                                    
            else if(ikenz.eq.1) then                                    
              call resdec(ind,lmes,ntag)                                
            else if(ikenz.eq.2) then                                    
              call resdec(ind,lmes,ntag)                                
            else if(ikenz.eq.3) then                                    
              call lddec(ind,ntag)                                      
            else if(ikenz.eq.4) then                                    
              call lddec(ind,ntag)                                      
            else if(ikenz.eq.5) then                                    
              call jetdec(ind,ntag)                                     
            else if(ikenz.eq.6) then                                    
              itdth(1,ident)=0                                          
              itdth(2,ident)=0                                          
              itdth(3,ident)=0                                          
              itdth(4,ident)=0                                          
              tlast(ind)=decay(ind)                                     
              if(qdot(0,ind).gt.0.) then                                
                delttr=decay(ind)-x(0,ind)                              
                do 421 i=0,3                                            
  421           x(i,ind)=x(i,ind)+qdot(i,ind)/qdot(0,ind)*delttr        
              end if                                                    
              startt=x(0,ind)                                           
              call dupdat(ind,startt)                                   
              if(ida(15).ne.0.and.ident.gt.n1old+n2old) then            
                if(decay(ind).lt.1.e+30) then                           
                  delttr=decay(ind)-x(0,ind)                            
                  do 422 i=0,3                                          
                   x(i,ind)=x(i,ind)+p(i,ind)/p(0,ind)*delttr           
  422             continue                                              
                end if                                                  
                ichbig(ind)=3                                           
                do 423 i=0,3                                            
  423           qdot(i,ind)=0.0                                         
              end if                                                    
              call propag(ind,-1,0)                                     
            else if(ikenz.eq.7) then                                    
              call resdec(ind,lmes,ntag)                                
            else if(ikenz.eq.8) then                                    
              call resdec(ind,lmes,ntag)                                
            else if(ikenz.eq.9) then                                    
              itdth(1,ident)=0                                          
              itdth(2,ident)=0                                          
              itdth(3,ident)=0                                          
              itdth(4,ident)=0                                          
              ichbig(ind)=3                                             
              if(qdot(0,ind).gt.0.) then                                
                delttr=tlast(ind)-x(0,ind)                              
                do 523 i=0,3                                            
  523           x(i,ind)=x(i,ind)+qdot(i,ind)/qdot(0,ind)*delttr        
              end if                                                    
              do 524 i=0,3                                              
  524         qdot(i,ind)=0.0                                           
              call propag(ind,-1,0)                                     
            end if                                                      
            if(ierror.eq.1) return                                      
            if(ntag.lt.0)then                                           
             xbldel=xbldel+1.                                           
             return                                                     
            else                                                        
              if(ikenz.eq.1) then                                       
                idcntd=idcntd+1                                         
              else if(ikenz.eq.2) then                                  
                idcntn=idcntn+1                                         
              else if(ikenz.eq.3) then                                  
                idcntr=idcntr+1                                         
              end if                                                    
            end if                                                      
            if(ikenz.eq.5) then                                         
            else if                                                     
     a      (itdth(2,ident).ne.0.and.tdtg(ident).gt.tact) then          
              iexpn=itdth(2,ident)                                      
              call newcqe(ityp(ind),npidec(ind),iexpn,0)                
              itdth(2,ident)=iexpn                                      
            else                                                        
              itdth(1,ident)=0                                          
              itdth(2,ident)=0                                          
              itdth(3,ident)=0                                          
              itdth(4,ident)=0                                          
            end if                                                      
      return                                                            
      end                                                               
