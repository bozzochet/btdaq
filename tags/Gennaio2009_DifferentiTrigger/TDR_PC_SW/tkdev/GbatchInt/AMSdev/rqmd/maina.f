cCPAT,MAINA    .                                                         
c        program rqmd                                                    
cc program: relativistic quantum molecular dynamics                      
cc                                                                       
cc                                                                       
cc author:  Heinz Sorge                                                  
cc          Nuclear Theory Group                                         
cc          S.U.N.Y.                                                     
cc          Stony Brook, NY 11794-3800                                   
cc          e-mail:sorge@nuclear.physics.sunysb.edu                      
cc                                                                       
c        include 'scatter7'                                              
c        include 'partblk7'                                              
c        ibas=0                                                          
c        ievco=0                                                         
c        call start                                                      
c        idifr=0                                                         
c        if(iz1.le.-5) then                                              
c           inondf=0                                                     
c        end if                                                          
c    1   continue                                                        
c        do 10 irun=ibas*nruns+1,(ibas+1)*nruns                          
c        if(ievco.ge.nruns) then                                         
c            call final                                                  
c            stop                                                        
c        end if                                                          
c        call event                                                      
c        if(nruns.gt.1) rewind 8                                         
c        idifr=idifr+1                                                   
c        if(iz1.le.-5) then                                              
c            if(idcdif.eq.1) inondf=inondf+1                             
c        end if                                                          
c        if(ierror.eq.0) then                                            
c            ievco=ievco+1                                               
c            call endev                                                  
c            if(ierror.eq.1) ievco=ievco-1                               
c        end if                                                          
c        ierror=0                                                        
c   10   continue                                                        
c        if(ievco.lt.nruns) then                                         
c            ibas=ibas+1                                                 
c            go to 1                                                     
c16
        end if                                                          
c        call final                                                      
c        stop                                                            
c        end                                                             
        subroutine rqmd_start                                                
        include 'amsrqmd.h'
c       using the code you can set switches in your input data          
c       meaning of switch(i):                                           
c       i=1  on                                                         
c       i=2  off                                                        
c       i=3  on   fix the random number initialization via iy           
c       i=4  off  pauli-blocking of collisions possible                 
c       i=5  off                                                        
c       i=6  off  potential included (on=cascade)                       
c       i=7  on   emulsion target (specified in emuls)                  
c       i=8  on   1.collision only between proj. and targ. particle     
c       i=9  on    delta's,rho's,eta's                                  
c                   and strange hadrons are kept stable.                
c       i=10 on   pauli-blocking after resonance decay possible         
c       i=11 on  initialization includes possible pauli blocking        
c       i=12 on  potential only, no collisions                          
c       i=13 on  string fusion to ropes                                 
c       i=14 on  print phase space info on unit 7                       
c       i=16 off  all instable particles are forced to                  
c                 decay at the end of the event (see fndnxt)            
c                                                                       
c       i=17 off  participant/spectator approximation: the system is    
c                 clustered from the beginning into fireball and        
c                 projectile/target spectators. the spectator           
c                 parts are heated up by impinging hadrons.             
c       i=18 on   no particle/jet system decay                          
c                 (see fndnxt)                                          
c       i=19 on   no transverse motion                                  
c       i=20 on   print only inelastic events on unit 9                 
c       i=21 on   only the fireball dynamics is computed, not the       
c                 spectator part.                                       
c       i=22 on   restart option. the content of the common-block       
c                 partblk7 is read in from unit 11.                     
c                 (if cpu time is not enough phase space info is        
c                  written on unit 12.)                                 
c       i=23 on   fast cascade                                          
c       i=24 on   one projectile in nucleus with potential              
      include 'scatter7'                                                
      external rqmdat                                                   
c      namelist/input/ header, n1,iz1,n2,iz2,ndt,nprint,nruns,ref,elgev, 
c     a  inttyp,ieos,b,dt,sepfp,sepft,switch,cutfct,iseed,               
c     b  idtcl                                                           
c      namelist/output/  n1,iz1,n2,iz2,ref,elgev,                        
c     a                   b,iyy                                          
        common /prinbk/ iprin                                           
        character*2 elem                                                
        dimension elem(108)                                             
        real*8 s2,avirt,pproj                                           
        real*4 pmas,pwid,cfr,dpar,cbr,x0l,x3l                           
        common /ludat2/ ktyp(120),pmas(120),pwid(60),kfr(80),cfr(40)    
        common /ludat3/ dpar(20),idb(120),cbr(400),kdp(1600)            
        common /ranfbk/iy                                               
        common /ierdum/ierr                                             
        common/dnsblk/jokall,timpri,bdens,hdens,edens,t11,t22,t33       
     a                   ,tb11,tb22,tb33                                
     b                   ,tp11,tp22,tp33,hdcent                         
     c                   ,rtsq(2,-5:5),drzsq(2,-5:5)                    
     d                   ,rvel(2,-5:5),yldet(2,-5:5)                    
        data  iruact,itprn,dtprn /0,1,1.0/                              
        logical hit                                                     
        include 'partblk7'                                              
        include 'sigmasx'                                               
        include 'storbig'                                               
        logical issers                                                  
        save  dtold,pproj,xcoll, itimdt,icld,itimx,iflpr,ikdtsw,iswi17  
     a        ,bmax17                                                   
        data  iclrst,irest/0,0/                                         
        save gam1,gam2,beta1,beta2                                      
        data elem/' p','he','li','be',' b',' c',' n',' o',              
     *          ' f','ne','na','mg','al','si',' p',' s',                
     *          'cl','ar',' k','ca','sc','ti',' v','cr',                
     *          'mn','fe','co','ni','cu','zn','ga','ge',                
     *          'as','se','br','kr','rb','sr',' y','zr',                
     *          'nb','mo','te','ru','rh','pd','ag','cd',                
     *          'in','sn','sb','te',' j','xe','cs','ba',                
     *          'la','ce','pr','nd','pm','sm','eu','gd',                
     *          'tb','dy','ho','er','tm','yb','lu','hf',                
     *          'ta',' w','re','os','ir','pt','au','hg',                
     *          'tl','pb','bi','po','at','rn','fr','ra',                
     *          'ac','th','pa',' u','np','pu','am','cm',                
     *          'bk','cf','es','fm','md','no','lw','ku',                
     *          '  ','  ','  ','  '                                     
     *         /                                                        
      character*33 vernum                                               
      character*46 pronam                                               
      logical*1 ttvers                                                  
      data vernum/'version rqmd 2.4  -- 30-jan-98'/                     
      data pronam /'relativistic quantum molecular dynamics'/           
      data ttvers/.true./                                               
      iprin=0                                                           
      xcnt(1)=0.                                                        
      xcnt(2)=0.                                                        
      xcnt(3)=0.                                                        
      const=2.*(emnuc+empion-emdelt)/ widdlt                            
      const1=atan(const)                                                
      const2=pi/2.-const1                                               
      dcanon=sqrt(4.2/pi)                                               
        seacut(1)=dcanon*1.2                                            
        seacut(2)=dcanon*1.2                                            
        seacut(3)=dcanon*1.2                                            
      do 102 i=4,66                                                     
        seacut(i)=sqrt(25./pi)                                          
102   continue                                                          
      do 103 l=3,6                                                      
        iclt=l*(l-1)/2+1                                                
        seacut(iclt)=sqrt(40./pi)                                       
        seacut(iclt+1)=sqrt(40./pi)                                     
103   continue                                                          
      if(ida(4).gt.0) then                                              
         fmltp=2.0                                                      
      else                                                              
         fmltp=1.                                                       
      end if                                                            
      seacut(22)=fmltp*sqrt(21./pi)                                     
      seacut(23)=fmltp*sqrt(7./pi)                                      
      seacut(29)=fmltp*sqrt(16.3333/pi)                                 
      seacut(30)=fmltp*sqrt(16.3333/pi)                                 
      seacut(37)=fmltp*sqrt(7./pi)                                      
      seacut(38)=fmltp*sqrt(21./pi)                                     
      seacut(24)=sqrt(21./pi)                                           
      seacut(25)=sqrt(21./pi)                                           
      seacut(26)=sqrt(21./pi)                                           
      seacut(27)=sqrt(21./pi)                                           
      seacut(31)=sqrt(21./pi)                                           
      seacut(32)=sqrt(21./pi)                                           
      seacut(33)=sqrt(21./pi)                                           
      seacut(34)=sqrt(21./pi)                                           
      seacut(39)=sqrt(21./pi)                                           
      seacut(40)=sqrt(21./pi)                                           
      seacut(41)=sqrt(21./pi)                                           
      seacut(42)=sqrt(21./pi)                                           
      seacut(52)=sqrt(21./pi)                                           
      seacut(53)=sqrt(21./pi)                                           
      seacut(54)=sqrt(21./pi)                                           
      seacut(62)=sqrt(21./pi)                                           
      seacut(63)=sqrt(21./pi)                                           
      seacut(64)=sqrt(21./pi)                                           
      seacut(28)=sqrt(2.5/pi)                                           
      seacut(35)=sqrt(12./pi)                                           
      seacut(36)=sqrt(3.6/pi)                                           
      seacut(43)=sqrt(15./pi)                                           
      seacut(44)=sqrt(12./pi)                                           
      seacut(45)=sqrt(2.5/pi)                                           
      do 10 i=67,maxtyp +1                                              
        seacut(i)=dcanon                                                
 10   continue                                                          
      seacut(67)=sqrt(41./pi)                                           
      seacut(68)=sqrt(41./pi)                                           
      seacut(69)=sqrt(41./pi)                                           
      seacut(70)=sqrt(41./pi)                                           
      seacut(71)=sqrt(41./pi)                                           
      seacut(72)=sqrt(41./pi)                                           
      seacut(76)=sqrt(41./pi)                                           
      seacut(77)=sqrt(41./pi)                                           
      seacut(90)=sqrt(10./pi)                                           
      seacut(117)=sqrt(10./pi)                                          
      seacut(79)=sqrt(5.0/pi)                                           
      seacut(80)=sqrt(5.0/pi)                                           
      seacut(106)=sqrt(5.0/pi)                                          
      seacut(107)=sqrt(5.0/pi)                                          
      seacut(92)=sqrt(41./pi)                                           
      seacut(93)=sqrt(41./pi)                                           
      seacut(85)=sqrt(41./pi)                                           
      seacut(86)=sqrt(41./pi)                                           
      seacut(87)=sqrt(41./pi)                                           
      seacut(112)=sqrt(41./pi)                                          
      seacut(113)=sqrt(41./pi)                                          
      seacut(114)=sqrt(41./pi)                                          
      seacut(94)=sqrt(41./pi)                                           
      seacut(95)=sqrt(41./pi)                                           
      seacut(96)=sqrt(41./pi)                                           
      seacut(97)=sqrt(41./pi)                                           
      seacut(98)=sqrt(13./pi)                                           
      seacut(99)=sqrt(13./pi)                                           
      seacut(100)=sqrt(13./pi)                                          
      seacut(101)=sqrt(41./pi)                                          
      seacut(102)=sqrt(41./pi)                                          
      seacut(104)=sqrt(10./pi)                                          
      seacut(119)=sqrt(10./pi)                                          
      seacut(105)=sqrt(41./pi)                                          
      seacut(maxtyp+1)=sqrt(41./pi)                                     
      do 30 index=1,itblsz                                              
        snntot(index)=spptot(index)                                     
        snnel(index)=sppel(index)                                       
 30   continue                                                          
      ierror=0                                                          
      b=-100.                                                           
      dt=0.2                                                            
      ref='lab'                                                         
      sepfp=1.                                                          
      sepft=1.                                                          
      cutfct=1.0                                                        
      ieos=2                                                            
      ndt=500                                                           
      idtcl=250                                                         
      nprint=0                                                          
      header='rqmd'                                                     
      iseed=-100                                                        
c      n1=1                                                              
c      iz1=1                                                             
c      n2=1                                                              
c      iz2=1                                                             
c      elgev=1.                                                          
      nruns=1                                                           
      do 1027 i=1,numswi                                                
        switch(i)=.false.                                               
 1027 continue                                                          
      ida(1)=0
      ida(2)=0
      ida(5)=0
      ida(11)=0
      ida(13)=3
      ida(27)=1
      ida(37)=0
      ida(38)=0
      ida(41)=1
      ida(42)=0
      ida(43)=0

      switch(8)=.true.                                                  
      switch(9)=.true.
      switch(13)=.true.                                                 
      switch(16)=.false.                                                 
      switch(17)=.true.                                                 
      switch(23)=.true.                                                 
      call getinp                                                       
      if (ttvers) then                                                  
        write(lun8,*)'---------------- version number report -------------'
        write(lun8,*)pronam                                                
        write(lun8,*)vernum                                                
        write(lun8,*)'----------------------------------------------------'
        ttvers=.false.                                                  
      end if                                                            
      if(switch(7)) then                                                
        switch(17)=.true.                                               
        switch(21)=.false.                                              
        b=-100.0                                                        
        n2=300                                                          
        iz2=300                                                         
      end if                                                            
      if(iz1.ge.0) then                                                 
        if(n1.le.0 .or. n2.lt. 0 ) then                                 
         write(lun14,*) 'wrong mass numbers',n1,n2                          
         ierror=1
          return                                                           
        end if                                                          
        if(n1.lt.iz1.or. n2.lt.iz2) then                                
         write(lun14,*) 'wrong charge numbers',iz1,iz2                      
         ierror=1
         return                                                           
        end if                                                          
        emproj=emnuc                                                    
      else                                                              
        ityppr=n1                                                       
        if(iabs(ityppr).lt.70.and.iabs(ityppr).gt.0) then               
           emproj=pmas(iabs(ityppr))                                    
        else                                                            
            write(lun14,*) 'iz1<0  wrong n1 n1=',n1                         
         ierror=1
          return                                                           
        end if                                                          
        n1=1                                                            
        call luiflv(ityppr,ifla,iflb,iflpr,ksp)                         
      end if                                                            
      if(n1.ge.n2+20.and.n2.ne.0) then                                  
        write(lun14,*)                                                      
     a        'n1,n2=',n1,n2                                            
        write(lun14,*)                                                      
     a        'rqmd abort: choose larger particle number as target'     
         ierror=1
          return                                                           
      end if                                                            
      if(switch(24)) then                                               
        if(n1.ne.1) then                                                
          write(lun14,*) 'switch(24)=t only for single projectile allowed'  
         ierror=1
          return                                                           
        end if                                                          
        switch(6)=.false.                                               
        switch(8)=.true.                                                
        switch(17)=.true.                                               
        switch(19)=.false.                                              
        switch(21)=.false.                                              
        switch(23)=.false.                                              
        idtcl=ndt+10                                                    
        ida(23)=1                                                       
        ida(27)=1                                                       
        ida(28)=0                                                       
        ida(33)=1                                                       
        ida(34)=2                                                       
      end if                                                            
      if(n1.eq.1.and.n2.eq.1) then                                      
        ida(4)=0                                                        
        ida(7)=0                                                        
        switch(23)=.true.                                               
      end if                                                            
      if(elgev. lt.0.1) then                                            
         write(lun14,*) 'rqmd should not be used for such a kinetic energy' 
         ierror=1
          return                                                           
         
      end if                                                            
      if(ref.eq.'cld') then                                             
        ref=' nn'                                                       
        elab=2.*(elgev*elgev-emnuc**2)/emnuc                            
        icld=1                                                          
        if(n1.eq.1.and.n2.eq.1) then                                    
          write(lun8,*) 'attention: only non single diffractive events'    
          ida(46)=1                                                     
        end if                                                          
      else                                                              
        elab=elgev                                                      
        icld=0                                                          
      end if                                                            
        pproj=sqrt(elab*(2.*emproj+elab))                               
        s2=(elab+emnuc+emproj)**2-pproj**2                              
        avirt=0.5*(s2-emnuc*emnuc-emproj*emproj)/emnuc/emproj           
        if(avirt.lt.1.) then                                            
          write(lun14,*) 'numerical error avirt.lt.1.   avirt=',avirt       
         ierror=1
          return                                                           
        end if                                                          
        betann=sqrt((avirt-1.)/(avirt+1.))                              
      gamnn=1./sqrt(1.-betann*betann)                                   
      if(ref.eq.'lab') then                                             
        beta1=-pproj/sqrt(emproj**2+pproj**2)                           
        beta2=0.                                                        
      else if((ref.eq.'nn').or.(ref.eq.' nn')) then                     
        beta1=-betann                                                   
        beta2=betann                                                    
      else                                                              
        write(lun14,*)' frame not implemented'                              
c        call errex('run aborted')                                       
         ierror=1
          return                                                           
      end if                                                            
      if(sepfp.ne.1.) then                                              
          da(4)=sepfp                                                   
      end if                                                            
      if(sepft.ne.1.) then                                              
          da(9)=sepft                                                   
      end if                                                            
      da(11)=b                                                          
      if(idtcl.lt.0) then                                               
         idtcl=ndt+10                                                   
      else                                                              
         idtcl=idtcl-mod(idtcl,5)+1                                     
         if(ida(8).le.2) then                                           
           write(lun14,*) 'conflicting idtcl and ida(8) specification'      
c           call errex('change input data')                              
         ierror=1
          return                                                           
         end if                                                         
      end if                                                            
      write(lun8,8101)                                                     
 8101 format(1x,72('-'))                                                
      nt=n1+n2                                                          
      if(nt.gt.nbarym) then                                             
        call errex('particle number too large:nt.gt.nbarym')            
         ierror=1
          return                                                           
      end if                                                            
      if(iz1.ge.0) then                                                 
         write(lun8,8100) n1,iz1,n2,iz2,nt                                 
      else                                                              
         write(lun8,8102 )ityppr,n2,iz2,nt                                 
      end if                                                            
 8102 format(/,' collision of hadron (type',i4,') on a,z=(',2i4,        
     *       ');total=', i4)                                            
 8100 format(/,' collision of a,z=(',2i4,') on a,z=(',2i4,');total=',   
     *       i4)                                                        
      if(nruns.lt.0) then                                               
         write(lun8,*) 'event no. should be positive. ????? nruns=',nruns  
      else                                                              
         write(lun8,*) 'event no.:',nruns                                  
      end if                                                            
      gam1=1./sqrt(1.-beta1*beta1)                                      
      p01=emproj*gam1*beta1                                             
      gam2=1./sqrt(1.-beta2*beta2)                                      
      p02=emnuc*gam2*beta2                                              
      write(lun8,8110)p01,gam1,p02,gam2                                    
 8110 format(' proj. p,gamma: ',2f11.4,'; targ. p,gamma: ',2f11.4)      
      if(nprint.eq.0) nprint=ndt+1                                      
      if(ida(17).eq.0) then                                             
         rr1=da(3)*n1**0.333333333                                      
         rr2=da(3)*n2**0.333333333                                      
      else                                                              
         rr1=1.19* n1**0.3333-1.61* n1**(-0.3333)                       
         rr2=1.19* n2**0.3333-1.61* n2**(-0.3333)                       
      end if                                                            
      if(n1.eq.1) rr1=0.                                                
      if(n2.eq.1) rr2=0.                                                
      write(lun8,9902)  rr1,rr2                                            
 9902 format('radius proj, targ:',2f8.4)                                
      if(nt.ne.2.and.ida(46).eq.1) ida(46)=0                            
      if(.not.switch(23).or.dt.ne.0.2) write(lun8,9901)dt                  
     a                                                                  
 9901 format('timestep ',1f8.4)                                         
      if(idtcl.le.ndt)then                                              
        if(idtcl.lt.5) then                                             
            write(lun8,*) 'idtcl<5 does not seem to be appropriate'        
            idtcl=ndt+1                                                 
        end if                                                          
        if(.not.switch(23)) then                                        
           write(lun8,*)                                                   
     a      'after ',idtcl,'timesteps separate evolution of clusters'   
        end if                                                          
      else                                                              
         write(lun8,*) 'no separate evolution of clusters'                 
      end if                                                            
      inocol=0                                                          
      if(switch(7)) then                                                
        write(lun8,*) 'emulsion target chosen:'                            
        write(lun8,*) '   events are  generated with targets randomly'     
        write(lun8,*) '   selected according probabilities in subr. emuls' 
        write(lun8,*) '   impact parameter stochastically chosen,'         
        write(lun8,*) '   events without collisions are discarded.'        
      else if(b.ge.0.) then                                             
         write(lun8,9911) b                                                
 9911    format('fixed impact parameter=',1f8.4)                        
      else                                                              
         write(lun8,*)'impact parameter stochastically chosen between 0.', 
     a             ' and ',min(-b,rr1+rr2+3.) ,' fm '                   
         if(b.lt.-99.) then                                             
                write(lun8,*) 'events without collisions not  recorded'    
         end if                                                         
      end if                                                            
      do 101 i=1,maxtyp+1                                               
        seacut(i)=seacut(i)*cutfct                                      
 101  continue                                                          
      if(cutfct.ne.1.0) then                                            
      write(lun8,8120)(seacut(i),i=1,maxtyp+1)                             
 8120 format(' collision search cutoffs',                               
     * /,(5(1x,f7.1)))                                                  
      end if                                                            
      if(switch(3)) then                                                
        iy=iseed                                                        
      else                                                              
        call seed(iseed)                                                
      end if                                                            
      if(ida(15).ne.0.or.sepfp.gt.999.) then                            
        write(lun8,*) 'option: suppression of secondary rescattering'      
        write(lun8,*) '      no collisions with hadrons from resonance'    
        write(lun8,*) '      decay and string sea'                         
        if(ida(15).eq.0) ida(15)=1                                      
        if(sepfp.gt.999.) da(4)=1.                                      
      end if                                                            
      if(switch(9))then                                                 
        write(lun8,*)                                                      
     a       'delta,rho,omega and strange hadrons are kept stable.'     
        ida(1)=1                                                        
        ida(41)=1                                                       
        ida(52)=0                                                       
        switch(16)=.true.                                               
      end if                                                            
      if(switch(13)) then                                               
        write(lun8,*) 'option: string fusion to ropes'                     
        ida(47)=1                                                       
      else                                                              
        ida(47)=0                                                       
      end if                                                            
      if (switch(6).or.switch(23).or.switch(19)) then                   
       write(lun8,*) 'cascade mode'                                        
       ida(28)=1                                                        
      end if                                                            
      if(switch(8))then                                                 
        write(lun8,*)'first n collision with nucleon from other nucleus or'
        write(lun8,*)'a secondary, resp. a n which had a collision before.'
      end if                                                            
      if(switch(10))then                                                
        if(switch(4))then                                               
          write(lun14,*) 'switches 4 and 10 conflict'                       
         ierror=1
          return                                                           
        else                                                            
          write(lun8,*) 'pauli blocking for delta decay'                   
        end if                                                          
      end if                                                            
      if(switch(11))then                                                
        if(switch(4))then                                               
          write(lun14,*) 'switches 4 and 11 conflict'                       
         ierror=1
          return                                                           
        else                                                            
          write(lun8,*) 'pauli blocking for initialisation'                
        end if                                                          
      end if                                                            
      if(switch(12))then                                                
        write(lun8,*) 'no collisions'                                      
      end if                                                            
      if(switch(14))then                                                
         ida(42)=1                                                      
      end if                                                            
      call ldinit                                                       
      if(.not.switch(16))then                                           
          write(lun8,*)                                                    
     a       'unstable particles are forced to decay at the end'        
      end if                                                            
      if(.not.switch(17))then                                           
        iswi17=0                                                        
        if(elab.lt. 10.)then                                            
          write(lun8,*)'switch(17)=f does not seem to be appropriate'      
          write(lun8,*) 'elab=',elab                                       
          write(lun8,*)'switch(17) is set to true'                         
          switch(17)=.true.                                             
          if(elab.lt.1.9) then                                          
             ida(23)=1                                                  
             write(lun8,*) 'non-relativistic propagation chosen'           
          end if                                                        
          iswi17=1                                                      
        end if                                                          
        if(iswi17.eq.0)then                                             
          bmax17=0.5*(rr1+rr2)                                          
          if((n2.lt.n1+20.and.(n2.eq.0.or.abs(b).lt.bmax17))            
     a                                            .or.nt.le.20) then    
            write(lun8,*)'switch(17)=f does not seem to be appropriate'    
            write(lun8,*) 'n2,n1=',n2,n1,'b=',b,                           
     a                    '0.5*(rr1+rr2)=',bmax17                       
            write(lun8,*)'switch(17) is set to true'                       
            switch(17)=.true.                                           
            iswi17=1                                                    
          else if(.not.switch(21).and.b.lt.0.0) then                    
            iswi17=2                                                    
          end if                                                        
        end if                                                          
      else                                                              
        iswi17=1                                                        
      end if                                                            
      if(iswi17.ne.1) write(lun8,*)                                        
     a       'separated evolution of the participants and spectators'   
      if(switch(18))then                                                
          write(lun8,*)                                                    
     a       'no particle/jet sytem decay during dynamical evolution'   
      end if                                                            
      if(switch(19))then                                                
        switch(6)=.true.                                                
        if(ida(34).eq.0)  switch(23)=.true.                             
          write(lun8,*)                                                    
     a       '1+1-dimensional motion in events'                         
          ida(23)=1                                                     
          ida(7)=0                                                      
      end if                                                            
      if(switch(20))then                                                
          write(lun8,*)                                                    
     a       'no elastic events are written on unit 9'                  
      end if                                                            
      if(switch(21))then                                                
          write(lun8,*)                                                    
     a       'only the fireball dynamics is calculated. spectators ',   
     b       'are left out.'                                            
          if(switch(17)) then                                           
             call errex('switch(17),switch(21) conflict')               
         ierror=1
          return                                                           
          end if                                                        
        switch(11)=.false.                                              
        if(iz1.ge.0) then                                               
           iarg=min(iz1,108)                                            
           header(1:2)=elem(iarg)                                       
        else                                                            
          if(ityppr.eq.41) then                                         
           header(1:2)='p+'                                             
          else if(ityppr.eq.42) then                                    
           header(1:2)='n '                                             
          else if(ityppr.eq.-41) then                                   
           header(1:2)='p-'                                             
          else if(iabs(ityppr).eq.17.or.ityppr.eq.23) then              
           header(1:2)='pi'                                             
          else if(ityppr.eq.18) then                                    
           header(1:2)='k+'                                             
          else if(ityppr.eq.-18) then                                   
           header(1:2)='k-'                                             
          else                                                          
           header(1:2)='hd'                                             
          end if                                                        
        end if                                                          
        iarg=min(iz2,108)                                               
        header(3:4)=elem(iarg)                                          
      end if                                                            
      if(switch(22))then                                                
          write(lun8,*)                                                    
     a       'restart option is switched on '                           
      end if                                                            
      if(switch(23)) then                                               
         switch(6)=.true.                                               
         switch(8)=.true.                                               
         if(ida(23).eq.0)  ida(23)=1                                    
         ida(27)=1                                                      
         ndt=1                                                          
         if(dt.eq.0.2)  dt=max(da(5)*15.*(rr1+rr2),da(5)*100.0)         
         write(lun8,*) 'modified (sw(23)=t)  dt=',dt                       
         if(nt.eq.2) switch(21)=.true.                                  
      end if                                                            
      if(ida(23).eq.1) then                                             
         ida(27)=1                                                      
      end if                                                            
      if(ida(36).gt.1) then                                             
       if(ida(38).ne.3.and.ida(38).ne.4) ida(36)=0                      
      end if                                                            
      write(lun8,*)'input complete'                                        
      write(lun8,*)'switches :',switch                                     
      write(lun8,*)'+++++++++++++++++++++++++++++++++++++++++++++++++'     
      if (nt .le. 0) then                                               
        call errex('no particles')                                      
         ierror=1
          return                                                           
      end if                                                            
      call seteos                                                       
      xblock=0.                                                         
      xbldel=0.                                                         
      xcoll=0.                                                          
      tmcoll=0.                                                         
      tmsctr=0.                                                         
      tmpali=0.                                                         
      tmprop=0.                                                         
      tmabsr=0.                                                         
      xfirst=0.0                                                        
      xininw=0.0                                                        
      if(switch(21).or.switch(23)) then                                 
         da(22)=2.                                                      
         if(n1.eq.1.and.elab.lt.2.) then                                
           da(22)=3.                                                    
         end if                                                         
      end if                                                            
      rprsq=(rr1+1.5*da(22))**2                                         
      betapr=beta1                                                      
      rtasq=(rr2+1.5*da(22))**2                                         
      betata=beta2                                                      
      if(n2.eq.0) rtasq=0.                                              
      write(lun8,*)'reaction variables'                                    
      write(lun8,*)'rprsq,rtasq,betapr,betata'                             
      write(lun8,*) rprsq,rtasq,betapr,betata                              
      yproj=0.5* log((1.+betapr)/(1.-betapr) )                          
      ytarg=0.5* log((1.+betata)/(1.-betata) )                          
      betapl=-0.9* betann                                               
      betatl=0.9* betann                                                
      if(ref.eq.'lab') then                                             
         betaol=betapl                                                  
         betapl=(-betann+betapl)                                        
         betatl=(-betann+betatl)                                        
         gammal=(1.-betann*betaol)                                      
         betapl=betapl/gammal                                           
         gammal=(1.+betann*betaol)                                      
         betatl=betatl/gammal                                           
         if(abs(betapl).gt.1. .or. abs(betatl).gt.1.) then              
            write(lun14,*)' wrong betapl, betatl=',betapl,betatl            
         ierror=1
          return                                                           
         end if                                                         
      endif                                                             
      n1old=n1                                                          
      iz1old=iz1                                                        
      n2old=n2                                                          
      iz2old=iz2                                                        
      dtold=dt                                                          
      ikdtsw=0                                                          
      if(gamnn.gt.4..and.ida(30).eq.1.and.ida(23).eq.0) then            
          if(ida(27).ne.1) then                                         
              write(lun8,*) 'warning:ida(27)=0,ida(30)=1 do not fit'       
          end if                                                        
          ikdtsw=int(24.*(rr1+rr2)/(dt*gamnn))+1                        
          write(lun8,*) 'ikdtsw=',ikdtsw                                   
      end if                                                            
      return                                                            
      entry rqmd_event                                                       
      call rstall                                                       
      iyy=iy                                                            
      write(lun8,*) iy,'value of iy=input integer random number:',         
     a               ' irun=',irun                                      
      if(switch(7)) then                                                
        if(irun.eq.1.or.ncollt-npaubl.gt.0) then                        
          call emuls(iz2old,n2old)                                      
        else                                                            
          nruns=nruns+1                                                 
        end if                                                          
      end if                                                            
      idcntd=0                                                          
      idcntn=0                                                          
      idcntr=0                                                          
      iabspi=0                                                          
      ncollt=0                                                          
      npaubl=0                                                          
      npion=0                                                           
      ievntn=0                                                          
      ibrhig=0                                                          
      if(iz1.le.-5)  then                                               
         idcdif=0                                                       
         inn2df=0                                                       
         ibrhig=1                                                       
      end if                                                            
        nrclst=1                                                        
        kclst=1                                                         
        iofset=0                                                        
        dtprop=0.0                                                      
         if(irun.gt.iruact) itprn=1                                     
         if(itprn.eq.1) then                                            
            iruact=irun                                                 
            timpri=dtprn                                                
            jokall=0                                                    
            hdcent=0.0                                                  
            tp11=0.0                                                    
            tp22=0.0                                                    
            tp33=0.0                                                    
         end if                                                         
      if(.not.switch(22)) then                                          
        write(lun8,*) 'irun=',irun                                         
        call initev                                                     
        if(iswi17.eq.2) then                                            
          if(b.gt.bmax17) then                                          
            switch(17)=.false.                                          
          else                                                          
            switch(17)=.true.                                           
          end if                                                        
        end if                                                          
        if(irun.eq.1.and.ref.ne.'lab'.and.icld.eq.0) then               
           betprl=pproj/sqrt(emproj**2+pproj**2)                        
           gampl=1./sqrt(1.- betprl*betprl)                             
           zmnp=zmnpr*gamnn/gampl                                       
           zmxp=zmxpr*gamnn/gampl                                       
           zmnt=zmnta*gamnn                                             
           zmxt=zmxta*gamnn                                             
           write(lun8,*) 'zmxpr,zmnpr,zmxta,zmnta in the ref.system'       
           write(lun8,*)  zmxpr,zmnpr,zmxta,zmnta                          
           write(lun8,*)'betapl=',betapl,'betatl=',betatl                  
           write(lun8,*) 'first event in the lab.system: '                 
           write(lun8,*) 'proj/target has z coordinates',                  
     b               ' between  zmxp>pr.n.>zmnp,zmxt>t.n.>zmnt'         
           write(lun8,*) 'zmxp,zmnp,zmxt,zmnt'                             
           write(lun8,*)  zmxp,zmnp,zmxt,zmnt                              
        end if                                                          
        if(ierror.eq.1) return                                          
        if(iz1.ge.0) then                                               
            iqtot=iz1+iz2                                               
        else                                                            
            iqtot=luchge(ityppr)/3+iz2                                  
        end if                                                          
        if(iz1.ge.0.or.iflpr.gt.0) then                                 
           nrmes(1)=0                                                   
           nrbar(1)=nt                                                  
           ntbar=nt                                                     
        else                                                            
           nrmes(1)=1                                                   
           if(iflpr.eq.0) then                                          
               ntbar=nt-1                                               
           else                                                         
               ntbar=nt-2                                               
           end if                                                       
           nrbar(1)=nt-1                                                
        end if                                                          
        nspect=0                                                        
        nsppr=0                                                         
        nspta=0                                                         
        if(.not.switch(17)) then                                        
          call spctat(0,1,0)                                            
          betdif=betapr- betata                                         
          do 411 l=ntotma+1-nspect,ntotma                               
              if(lastcl(l)/100000.eq.-1) then                           
                 tnew=-(x(3,l)- betapr*x(0,l)- zmxta)/betdif            
                 if(tnew.lt. x(0,l))then                                
                    write(lun8,*) issn(l),l,tnew,(x(i,l),i=0,3)            
                    call errex('1 wrong tnew in event???????')          
                 else                                                   
                    x0old=x(0,l)                                        
                    x(0,l)=tnew                                         
                    x(3,l)=x(3,l)+betapr* (tnew-x0old)                  
                 end if                                                 
              else if(lastcl(l)/100000.eq.-2) then                      
                 tnew=(x(3,l)- betata*x(0,l)- zmnpr)/betdif             
                 if(tnew.lt. x(0,l))then                                
                    write(lun8,*) tnew,(x(i,l),i=0,3)                      
                    call errex('2 wrong tnew in event???????')          
                 else                                                   
                    x0old=x(0,l)                                        
                    x(0,l)=tnew                                         
                    x(3,l)=x(3,l)+betata* (tnew-x0old)                  
                 end if                                                 
              else                                                      
                write(lun8,*) l,'lastcl=',lastcl(l)                        
                call errex('lastcl not correct for spectator in event') 
              endif                                                     
              tlast(l)=x(0,l)                                           
              do 410 i=0,3                                              
  410         qdot(i,l)=0.0                                             
  411     continue                                                      
            itrmx=3                                                     
        else                                                            
            itrmx=1                                                     
        endif                                                           
        nspnuc=nspect                                                   
         itrmxr=0                                                       
         kdtr=0                                                         
      else                                                              
         if(.not.switch(17)) then                                       
             itrmx=3                                                    
         else                                                           
             itrmx=1                                                    
         endif                                                          
         read(11) itrmxr,kdtr                                           
      end if                                                            
      if(nt.lt.30) then                                                 
          irestl=2                                                      
      else if(nt.gt.300) then                                           
          irestl=12                                                     
      else                                                              
          irestl=5                                                      
      end if                                                            
      do 1005 itripl=1,itrmx                                            
      if(itripl.lt.itrmxr) then                                         
          go to 1005                                                    
      else if(itripl.eq.itrmxr) then                                    
          go to 126                                                     
      end if                                                            
      time=0.                                                           
      tlstev=0.                                                         
      if(itripl.eq.1) then                                              
         iaddit=                                                        
     a    max(int(4.*(max(n1,25)*max(n2,25))**0.6666                    
     b                                      *0.95* log(elab +9.0)),90)  
     c                                       *(1.0+0.3*log(elab+1.0))   
                                                                        
         if(nrbar(1)+nrmes(1)+iaddit.ge.ntotma-nspect) then             
             iadpho=max(10,nint(float(iaddit)/10.))                     
             iaddit=max(0,ntotma-nspect-nrbar(1)-nrmes(1)-iadpho)       
         end if                                                         
      else                                                              
         iaddit=max(int(3.2*nrbar(itripl)*log(elab)),90)                
      end if                                                            
      iclusz(itripl)=nrbar(itripl)+nrmes(itripl)+iaddit                 
      if(itripl.gt.1) then                                              
  401    continue                                                       
        igo=0                                                           
        if(iclusz(1)+iclusz(itripl).ge.ntotma+1-nspect) then            
            iclz=nrbar(itripl)+nrmes(itripl)                            
            idif=max(0,nint(0.4* float(ntotma-nspect-iclusz(1)-iclz)))  
            iclusz(itripl)=iclz+idif                                    
            igo=igo+1                                                   
            if(igo.eq.1) then                                           
               go to 401                                                
            else                                                        
               call errex(                                              
     a                'storage garbage with nrbar,nrmes(itripl)')       
            end if                                                      
        end if                                                          
      end if                                                            
      lmax=0                                                            
      do 45 lm=1,itripl                                                 
   45 lmax=lmax+iclusz(lm)                                              
      nonrel(itripl)=.false.                                            
      kclst=itripl                                                      
      if(ida(23).ne.0) then                                             
         tobs=1.e+30                                                    
         tmxobs=-1.e+30                                                 
         iofset=0                                                       
         do 203 icl=1,itripl-1                                          
 203     iofset=iofset+iclusz(icl)                                      
         do 133 il=iofset+1,iofset+nrbar(itripl)+nrmes(itripl)          
             if(qdot(0,il).eq.0.0.or.itripl.eq.1                        
     a                                .or..not.switch(23)) then         
               tl=x(0,il)                                               
               zl=x(3,il)                                               
             else                                                       
               tl=tlast(il)                                             
               dtl=tl-x(0,il)                                           
               zl=x(3,il)+qdot(3,il)/qdot(0,il)*dtl                     
             end if                                                     
             tilobs=tl*pnorm(0)-zl*pnorm(3)                             
             tobs=min(tobs,tilobs)                                      
             tmxobs=max(tmxobs,tilobs)                                  
  133    continue                                                       
         tbegin=tobs                                                    
         tmxobs=max(max(tmxobs,8.*rr1),8.*rr2)                          
         if(switch(23).and.                                             
     a                  itripl.gt.1) then                               
           do 134 il=iofset+1,iofset+nrbar(itripl)+nrmes(itripl)        
            if(qdot(0,il).gt.0.0) then                                  
              dtl=(tlast(il)-x(0,il))/qdot(0,il)                        
              do 136 i=0,3                                              
  136         x(i,il)=x(i,il)+qdot(i,il)*dtl                            
            end if                                                      
            tilobs=x(0,il)*pnorm(0)-                                    
     a                            x(3,il)*pnorm(3)                      
            dtl=(tbegin-tilobs)                                         
            identl=issn(il)                                             
            ncolil=nclcnt(identl)                                       
            if(ncolil.eq.0.and.identl.le.nt) then                       
              if(identl.le.n1) then                                     
                pilobs=p(0,il)*pnorm(0)-p(0,il)*betapr*pnorm(3)         
              else                                                      
                pilobs=p(0,il)*pnorm(0)-p(0,il)*betata*pnorm(3)         
              end if                                                    
              if(identl.le.n1) then                                     
                x(3,il)=x(3,il)+betapr*p(0,il)/pilobs*dtl               
              else                                                      
                x(3,il)=x(3,il)+betata*p(0,il)/pilobs*dtl               
              end if                                                    
              x(0,il)=x(0,il)+p(0,il)/pilobs*dtl                        
            else                                                        
              pilobs=p(0,il)*pnorm(0)-                                  
     a                            p(3,il)*pnorm(3)                      
              do 135 i=0,3                                              
  135         x(i,il)=x(i,il)+p(i,il)/pilobs*dtl                        
            end if                                                      
  134      continue                                                     
         end if                                                         
         if(switch(23).and.(tbegin+ndt*dtold).lt.tmxobs)then            
             dt=tmxobs+10.-tbegin                                       
             write(lun8,*)'dt has been modified dt,tmxobs,tbegin'          
             write(lun8,*)  dt,tmxobs,tbegin,'irun,itripl=',irun,itripl    
         else                                                           
             dt=dtold                                                   
         end if                                                         
         nonrel(itripl)=.true.                                          
      end if                                                            
      if(nprint.le.ndt.or.ida(42).eq.1) then                            
         iofs=0                                                         
         do 201 icl=1,itripl-1                                          
 201     iofs=iofs+iclusz(icl)                                          
            do 3058 l=iofs+1,iofs+nrbar(itripl)                         
             identl=issn(l)                                             
             write(7)                                                   
     &           ityp(l),issn(l),npidec(l),                             
     &           x(0,l),x(1,l),x(2,l),x(3,l),                           
     &           p(0,l),p(1,l),p(2,l),p(3,l),em(l),                     
     &         tdtg(identl),itdth(2,identl),itdth(3,identl)             
 3058       continue                                                    
      end if                                                            
      ierr=systim(timdt,dummy)                                          
  126 do 26 kdt=1,ndt                                                   
        if(switch(22)) then                                             
           if(kdt.lt.kdtr) go to 26                                     
           if(iclrst.eq.0) then                                         
              iclrst=1                                                  
              ndtold=ndt                                                
              call rstart                                               
              ndt=ndtold                                                
           end if                                                       
        end if                                                          
        istop=0                                                         
        if(ida(24).eq.1) then                                           
            iresto=irest                                                
            irest=jrtime(idummy)                                        
            if(.not.switch(23).or.itripl.eq.1) itimdt=iresto-irest      
            if(irun.eq.0) itimx=0                                       
            itimx=max(itimx,itimdt)                                     
            if(irest.lt.2*itimdt.or.irest.le.irestl) istop=1            
            if(switch(23).and.irest.lt.2*itimx) istop=1                 
        else if(ida(24).eq.2) then                                      
            if(.not.switch(23).and.ndt.lt.500.and.kdt.eq.ndt            
     a            .and.itripl.eq.1                                      
     a                                           ) istop=2              
        end if                                                          
        if(istop.ne.0) call rend(istop)                                 
        if(kdt.le.ikdtsw.and.itripl.eq.1) then                          
           dt=0.5*dtold                                                 
        else                                                            
           dt=dtold                                                     
        end if                                                          
       if(itripl.gt.1 .and.switch(21))goto 259                          
      if(itripl.gt.1 .and. .not.switch(17).and.ncollt.eq.0)goto 259     
      if(kdt.eq.idtcl) call clustr                                      
      if(ierror.eq.1) return                                            
      timmax=time+dt                                                    
      if(mod(kdt-1,5).eq.0) then                                        
         itrig=0                                                        
      else                                                              
         itrig=1                                                        
      end if                                                            
      ncoll=0                                                           
      iofset=0                                                          
      do 202 icl=1,itripl-1                                             
 202  iofset=iofset+iclusz(icl)                                         
      do 108 kclst=itripl,nrclst                                        
       if(iofset+iclusz(kclst).ge.ntotma+1-nspect) then                 
          write(lun8,*) iofset,iclusz(kclst),ntotma,nspect                 
          call errex('ntotma is too low, error in event')               
          if(ierror.eq.1) return                                        
       end if                                                           
       ierr=systim(strt,dummy)                                          
       nrpion=nrmes(kclst)                                              
       ntt=nrbar(kclst)                                                 
       ntclst=ntt+nrpion                                                
       if(ida(11).eq.1) ntclst=ntt                                      
       if(ntclst.eq.1) then                                             
       else if(ntclst.eq.0) then                                        
           go to 107                                                    
       else if((mod(kdt,10).ne.0).and.(nonrel(kclst))) then             
          itrigg=-1                                                     
          call relham(itrigg,ntt,nrpion)                                
       else if(ntclst.gt.1) then                                        
          call relham(itrig,ntt,nrpion)                                 
       end if                                                           
       ierr=systim(tend,dummy)                                          
       tmprop=tmprop+tend-strt                                          
       if(ierror.eq.1) return                                           
       if(kdt.eq.idtcl-1) go to 107                                     
         if(switch(12))goto 1999                                        
         tlstev=time                                                    
         ierr=systim(strt,dummy)                                        
          nttot=nrbar(kclst)+nrmes(kclst)                               
          if(nttot.gt.1) then                                           
            call coload                                                 
            if(ierror.eq.1) return                                      
          else                                                          
            nument=0                                                    
          end if                                                        
          ierr=systim(tend,dummy)                                       
          tmcoll=tmcoll+tend-strt                                       
 1000     continue                                                      
         if(ida(36).gt.1.and.kclst.eq.itripl) then                      
           if(switch(23)) then                                          
             if(itprn.le.40.and.tlstev.ge.timpri) then                  
               jokall=1                                                 
             end if                                                     
           else                                                         
             if(itprn.gt.40) jokall=2                                   
           end if                                                       
           if(jokall.eq.1) then                                         
             dtpr=tlstev-(time+dt)                                      
             call pribd(dtpr)                                           
             itprn=itprn+1                                              
             jokall=0                                                   
             timpri=itprn*dtprn                                         
           end if                                                       
         end if                                                         
          call fndnxt(ind,iwhat)                                        
          if(ida(24).eq.1) then                                         
            istop=0                                                     
            irest=jrtime(idummy)                                        
            if(irest.lt.2) call errex('no cpu time left')               
          end if                                                        
          if(ierror.eq.1) return                                        
          if (iwhat.eq.0) goto 1999                                     
          if(iwhat.eq.1) then                                           
             i1cind=i1c(ind)                                            
             i2cind=i2c(ind)                                            
             ident1=issn(i1cind)                                        
             ident2=issn(i2cind)                                        
            call coll(ind)                                              
          else if(iwhat.eq.2) then                                      
            call rqmdec(ind)                                            
          end if                                                        
          if(ierror.eq.1) return                                        
          ntclst=nrbar(kclst)+nrmes(kclst)                              
          if(ida(11).eq.1) ntclst=nrbar(kclst)                          
          goto 1000                                                     
 1999    continue                                                       
         call  comprs(0)                                                
         if(ierror.eq.1) return                                         
         if(.not.switch(21).and..not.switch(17)                         
     a                                       .and.itripl.eq.1) then     
           ianf=iofset                                                  
           kclu=kclst                                                   
           if(switch(23)) then                                          
             ispc=1                                                     
           else                                                         
             ispc=0                                                     
           end if                                                       
           call spctat(ianf,kclu,ispc)                                  
        end if                                                          
 107  iofset=iofset+iclusz(kclst)                                       
 108  continue                                                          
      ncollt=ncollt+ncoll                                               
      xcoll=xcoll+ncoll                                                 
  259 continue                                                          
      nall=nspect                                                       
      do 5057 kcl=1,nrclst                                              
         nall=nall+nrbar(kcl)+nrmes(kcl)                                
 5057 continue                                                          
      if(nall.ne. nt+npion) then                                        
        call errex('event:wrong particle number in main')               
        if(ierror.eq.1) return                                          
      end if                                                            
      kp=nprint*(kdt/nprint)                                            
      if(kdt.eq.kp) then                                                
            if(itripl.eq.1) then                                        
               iofset=0                                                 
            else                                                        
               iofset=iclusz(1)                                         
            end if                                                      
            do 4057 kcl=itripl,nrclst                                   
               if(nrbar(kcl)+nrmes(kcl).eq.1) then                      
                    if(ichbig(iofset+1).eq.5) go to 4056                
                    ichbig(iofset+1)=5                                  
               end if                                                   
            do 4058 l=iofset+1,iofset+nrbar(kcl)+nrmes(kcl)             
             identl=issn(l)                                             
             write(lun7)                                                   
     &         ityp(l),issn(l),npidec(l),                               
     &         x(0,l),x(1,l),x(2,l),x(3,l),                             
     &         p(0,l),p(1,l),p(2,l),p(3,l),em(l),                       
     &         tdtg(identl),itdth(2,identl),itdth(3,identl)             
 4058       continue                                                    
 4056       iofset=iofset+iclusz(kcl)                                   
 4057     continue                                                      
      end if                                                            
      time=time+dt                                                      
      ierr=systim(ttotal,dummy)                                         
      timdt=ttotal                                                      
  26  continue                                                          
      iof=iclusz(1)                                                     
      nrold=nrbar(1)+nrmes(1)                                           
      lloop=nrold                                                       
      do 1014 icl=2,nrclst                                              
       do 1013 l=1,nrbar(icl)+nrmes(icl)                                
          il=iof+l                                                      
          lloop=lloop+1                                                 
          do 1012 i=0,3                                                 
             x(i,lloop)=x(i,il)                                         
             p(i,lloop)=p(i,il)                                         
 1012     continue                                                      
          em(lloop)=em(il)                                              
          ityp(lloop)=ityp(il)                                          
          decay(lloop)=decay(il)                                        
          lastcl(lloop)=lastcl(il)                                      
          issn(lloop)=issn(il)                                          
          npidec(lloop)=npidec(il)                                      
          tlast(lloop)=tlast(il)                                        
 1013  continue                                                         
       nrbar(1)=nrbar(1)+nrbar(icl)                                     
       nrmes(1)=nrmes(1)+nrmes(icl)                                     
       iof=iof+iclusz(icl)                                              
 1014 continue                                                          
      iclusz(1)=nrbar(1)+nrmes(1)                                       
      nrclst=1+mod(itripl,itrmx)                                        
      if(lloop.ne. iclusz(1)) call errex('wrong lloop in event')        
      if(lloop +nspect.ne.nt+npion)                                     
     a                call errex('wrong iclusz(1)+nspect in event')     
      ntt=0                                                             
      nrpion=0                                                          
      idelte=0                                                          
      do 1015 il=ntotma,ntotma-nspect+1,-1                              
         ispmod=-lastcl(il)/100000                                      
         if(ispmod.lt.0.or.ispmod.gt.2) then                            
             write(lun8,*) 'ispmod=',ispmod                                
             call errex('unclear situation for spectat lastcl in event')
         end if                                                         
         if(ispmod.ne.itripl) go to 1015                                
           if(ityp(il) .le.6 .or.                                       
     a            (ityp(il) .ge.10 .and.                                
     b                  ityp(il) .le.11 )                               
     c            .or. ityp(il) .eq.13                                  
     d            .or. ityp(il) .eq.15  )then                           
            lloop=lloop+1                                               
            ntt=ntt+1                                                   
            do 1016 i=0,3                                               
              x(i,lloop)=x(i,il)                                        
              qdot(i,lloop)=qdot(i,il)                                  
              p(i,lloop)=p(i,il)                                        
 1016       continue                                                    
              em(lloop)=em(il)                                          
              ityp(lloop)=ityp(il)                                      
              decay(lloop)=decay(il)                                    
              lastcl(lloop)=mod(-lastcl(il),100000)                     
              issn(lloop)=issn(il)                                      
              npidec(lloop)=npidec(il)                                  
              tlast(lloop)=tlast(il)                                    
              if(ichbig(il).ne.3) then                                  
                 ichbig(lloop)=0                                        
              else                                                      
                 ichbig(lloop)=3                                        
              end if                                                    
        end if                                                          
 1015 continue                                                          
      do 2015 il=ntotma,ntotma-nspect+1,-1                              
         if(-lastcl(il)/100000.ne.itripl) then                          
           ill=il+idelte                                                
            do 2017 i=0,3                                               
              x(i,ill)=x(i,il)                                          
              p(i,ill)=p(i,il)                                          
              qdot(i,ill)=qdot(i,il)                                    
 2017       continue                                                    
              em(ill)=em(il)                                            
              ityp(ill)=ityp(il)                                        
              decay(ill)=decay(il)                                      
              lastcl(ill)=lastcl(il)                                    
              issn(ill)=issn(il)                                        
              npidec(ill)=npidec(il)                                    
              tlast(ill)=tlast(il)                                      
              ichbig(ill)=ichbig(il)                                    
           go to 2015                                                   
         endif                                                          
         idelte=idelte+1                                                
      if((ityp(il) .ge.7                                                
     a                .and. ityp(il) .le.9 ).or.                        
     b          ityp(il) .eq.12  .or.                                   
     c          ityp(il) .eq.14  .or.                                   
     d          (ityp(il).ge.85.and.                                    
     d           ityp(il).le.98).or.                                    
     e          ityp(il) .eq.99                 )then                   
            lloop=lloop+1                                               
            nrpion=nrpion+1                                             
            do 2016 i=0,3                                               
              x(i,lloop)=x(i,il)                                        
              qdot(i,lloop)=qdot(i,il)                                  
              p(i,lloop)=p(i,il)                                        
 2016       continue                                                    
              em(lloop)=em(il)                                          
              ityp(lloop)=ityp(il)                                      
              decay(lloop)=decay(il)                                    
              lastcl(lloop)=mod(-lastcl(il),100000)                     
              issn(lloop)=issn(il)                                      
              npidec(lloop)=npidec(il)                                  
              tlast(lloop)=tlast(il)                                    
              if(ichbig(il).ne.3) then                                  
                 ichbig(lloop)=0                                        
              else                                                      
                 ichbig(lloop)=3                                        
              end if                                                    
        end if                                                          
 2015 continue                                                          
      if(idelte.ne. ntt+nrpion) then                                    
        write(lun8,*)'ntt=',ntt,'nrpion=',nrpion,'idelte=',idelte          
        call errex('unidentified particle in spect.group')              
      end if                                                            
      nspect=nspect-ntt-nrpion                                          
      if(itripl.eq.itrmx.and.                                           
     a                   ida(7).eq.1) then                              
               do 1509 il=1,nrbar(1)+nrmes(1)                           
                identl=issn(il)                                         
                if(ida(7).eq.1) then                                    
                  if(itdth(2,identl).ne.0) then                         
                      call ofshel(8,il,identl)                          
                  end if                                                
                endif                                                   
 1509          continue                                                 
      end if                                                            
      if(itripl.eq.1) then                                              
             pnorm(0)=gam1                                              
             pnorm(1)=0.                                                
             pnorm(2)=0.                                                
             pnorm(3)=gam1*beta1                                        
             nsppr=0                                                    
             nspta=nspect                                               
      else if(itripl.eq.2) then                                         
             pnorm(0)=gam2                                              
             pnorm(1)=0.                                                
             pnorm(2)=0.                                                
             pnorm(3)=gam2*beta2                                        
             nspta=0                                                    
      end if                                                            
      if(itripl.gt.1) then                                              
            iclusz(itripl)=0                                            
            nrbar(itripl)=0                                             
            nrmes(itripl)=0                                             
      end if                                                            
      nrbar(itripl+1)=ntt                                               
      nrmes(itripl+1)=nrpion                                            
      if(lloop+nspect.ne. nt+npion)                                     
     a        call errex('error in sorting particl./event')             
      if(itripl.eq.2.and.nspect.ne.0) then                              
              write(lun8,*) 'error at end of itripl-loop'                  
              do 1009 l=ntotma,ntotma+1-nspect,-1                       
              write(lun8,*) l,'type=',ityp(l),npidec(l),'ident=',issn(l),  
     a                                       'lastcl=',lastcl(l)        
 1009         continue                                                  
              call errex('error:itripl=2 spectators.ne.0/event')        
      end if                                                            
 1005 continue                                                          
         call cnstst(2)                                                 
         if(iz1.ne.-7.and.(iz1.le.-6.and.iz1.ge.-9))then                
           if(ncollt.eq.0) inn2df=1                                     
           do 191 l=1,nrbar(1)+nrmes(1)                                 
              if(issn(l).eq.1) then                                     
                if(nclcnt(issn(l)).ne.1) then                           
                   ierror=1                                             
                else                                                    
                   idcdif=1                                             
                end if                                                  
              end if                                                    
  191      continue                                                     
         end if                                                         
      return                                                            
      entry rqmd_endev                                                       
      ntweak=nrbar(1)+nrmes(1)                                          
      if(.not.switch(16).and.ida(1).eq.0)then                           
        iofset=0                                                        
        kclst=1                                                         
        kdt=ndt+1                                                       
        iclusz(1)=ntotma                                                
        ifndnx=1                                                        
        if(ida(41).eq.0) then                                           
            idb(19)=-idb(19)                                            
            idb(43)=-idb(43)                                            
            idb(44)=-idb(44)                                            
            idb(45)=-idb(45)                                            
            idb(57)=-idb(57)                                            
            idb(46)=-idb(46)                                            
            idb(47)=-idb(47)                                            
            idb(70)=-idb(70)                                            
        end if                                                          
        do 91 l=1,nrbar(1)+nrmes(1)                                     
         ntag=0                                                         
         call lddec(l,ntag)                                             
   91   continue                                                        
        if(ida(41).eq.0) then                                           
            idb(19)=-idb(19)                                            
            idb(43)=-idb(43)                                            
            idb(44)=-idb(44)                                            
            idb(45)=-idb(45)                                            
            idb(57)=-idb(57)                                            
            idb(46)=-idb(46)                                            
            idb(47)=-idb(47)                                            
            idb(70)=-idb(70)                                            
        end if                                                          
        if(nrbar(1)+nrmes(1).ne.nt+npion) then                          
          write(lun8,*)'particle number not conserved in endev'            
         ierror=1
          return                                                           
        end if                                                          
      else                                                              
        if(ida(5).ne.0) then                                            
          do 92 l=1,nrbar(1)+nrmes(1)                                   
           em1=em(l)                                                    
           call oldnew(1,ityp(l),npidec(l),em1,kmult1,kl2)              
           if(kmult1.eq.0) then                                         
             if(kl2.eq.34.or.kl2.eq.25.or.                              
     a                   kl2.eq.33.or.kl2.eq.35.or.                     
     b                     (ida(5).gt.1.and.kl2.eq.24)) then            
               write(lun10,1663) kl2,p(0,l),(-p(i,l),i=1,3),em(l)          
             end if                                                     
           end if                                                       
   92     continue                                                      
        end if                                                          
 1663   format(1x,i4,1x,5(f7.3,1x))                                     
      end if                                                            
      if(ida(25).eq.1) then                                             
        write(lun15,661) 0,0,0,0,0,0,0,0                                   
  661   format(1x,i3,1x,i3,1x,i1,1x,i4,1x,i3,1x,i2,1x,i7,1x,i7)         
      end if                                                            
      if(nprint.le.ndt.or.ida(42).eq.1) then                            
         if(nprint.le.ndt) then                                         
            do 452 l=1,nrbar(1)+nrmes(1)                                
            identl=issn(l)                                              
             write(lun7)                                                   
     &         ityp(l),issn(l),npidec(l),                               
     &         x(0,l),x(1,l),x(2,l),x(3,l),                             
     &         p(0,l),p(1,l),p(2,l),p(3,l),em(l),                       
     &         tdtg(identl),itdth(2,identl),itdth(3,identl)             
  452       continue                                                    
         end if                                                         
         write(lun7)                                                       
     &            0,0,0,                                                
     &            10*0.0,0,0                                            
      end if                                                            
      if(iz1.lt.0) then                                                 
        n1=ityppr                                                       
      end if                                                            
      if(icld.eq.1) then                                                
        ref='cld'                                                       
      end if                                                            
      if(iz1.eq.-5) then                                                
        if(idcdif.eq.0) then                                            
          ierror=1                                                      
          return                                                        
        end if                                                          
      else if(iz1.le.-6.and.iz1.ge.-9) then                             
        return                                                          
      end if                                                            
      if(da(11).lt.-99.) then                                           
           if(ncollt-npaubl.eq.0) then                                  
             inocol=inocol+1                                            
             return                                                     
           end if                                                       
      end if                                                            
      if(switch(20)) then                                               
        call transl(-1,ityp(nt),npidec(nt),kf)                          
        if(npion.eq.0.and.                                              
     a       (iz1.ge.0.or.(kf.eq.ityppr                                 
     b                     .and.iz1.lt.0)))then                         
          inocol=inocol+1                                               
          return                                                        
        end if                                                          
      end if                                                            
      if(((ida(38).gt.0.and.ida(38).le.2)                               
     a              .or.ida(38).eq.7                                    
     b                   .or.ida(38).eq.5).and.                         
     c                        ncollt.ne.0.and.npion.ne.0) then          
        jeng=0                                                          
        do 1558 l=1,nrbar(1)+nrmes(1)                                   
         eml=em(l)                                                      
         if(issers(jeng,ida(38),ityp(l),npidec(l),eml)) then            
             ident=issn(l)                                              
             write(lun13,890)                                              
     a        ident,0,ityp(l),npidec(l),p(0,l),(-p(i,l),i=1,3)          
             write(lun13,888) (stork(ident,i),i=1,5)                       
             write(lun13,889) (kstor(ident,i),i=1,7)                       
         end if                                                         
 1558   continue                                                        
        write(lun13,889) 0                                                 
  888   format(1x,5(f7.3,1x))                                           
  889   format(1x,5(i7,1x),2(i4,1x))                                    
  890   format(1x,4(i7,1x),1x,4(f7.3,1x))                               
      end if                                                            
      if(ncollt.ne.0.and.npion.ne.0) then                               
        if(ida(5).ne.0)  write(lun10,885) 0                                
      end if                                                            
  885 format(1x,i4)                                                     
      nonin1=0                                                          
      ionin1=0                                                          
      nonin2=0                                                          
      ionin2=0                                                          
      nonint=0                                                          
      nextra=0                                                          
      ngamma=0                                                          
      do 335 l=1,nt+npion                                               
       if(ityp(l).eq.99.and.npidec(l).eq.1) ngamma=ngamma+1             
       if(ida(52).eq.1.and.n1.eq.1) then                                
         if(nclcnt(issn(l)).eq.0.and.issn(l).gt.n1.and.                 
     a                         issn(l).le.n1old+n2old) then             
           ityp(l)=-9999                                                
           if(ityp(l).eq.2) ionin2=ionin2+1                             
           nonin2=nonin2+1                                              
           nonint=nonint+1                                              
         end if                                                         
       else if(ida(52).eq.2) then                                       
         if((nclcnt(issn(l)).eq.0.and.                                  
     b       issn(l).le.n1old+n2old).or.                                
     c       ityp(l).eq.1.or.                                           
     d       (ityp(l).eq.8.and.npidec(l).eq.0)) then                    
           if(issn(l).le.n1old+n2old) then                              
             if(issn(l).gt.n1old) then                                  
               if(ityp(l).eq.2) ionin2=ionin2+1                         
               nonin2=nonin2+1                                          
             else                                                       
               if(ityp(l).eq.2) ionin1=ionin1+1                         
               nonin1=nonin1+1                                          
             end if                                                     
           else                                                         
             nextra=nextra+1                                            
           end if                                                       
           ityp(l)=-9999                                                
           nonint=nonint+1                                              
         end if                                                         
       end if                                                           
  335 continue                                                          
         if(nonint.ne.0) then                                           
           n1ol=n1                                                      
           iz1ol=iz1                                                    
           n1=n1-nonin1                                                 
           iz1=iz1-ionin1                                               
           n2ol=n2                                                      
           iz2ol=iz2                                                    
           n2=n2-nonin2                                                 
           iz2=iz2-ionin2                                               
         end if                                                         
c      write(lun9,output)                                                   
         if(nonint.ne.0) then                                           
           n1=n1ol                                                      
           iz1=iz1ol                                                    
           n2=n2ol                                                      
           iz2=iz2ol                                                    
         end if                                                         
       npitru=npion                                                     
       npion=npion-nextra                                               
       if(iz1.lt.0) then                                                
          n1=1                                                          
       end if                                                           
       if(icld.eq.1) then                                               
         ref=' nn'                                                      
       end if                                                           
       write(lun9,905) npion,npitru,nspnuc,ngamma                          
       write(lun9,906)  ncollt-npaubl,idcntd,idcntn,idcntr,iabspi          
 906   format(    5i8)                                                  
 905   format(6i6 )                                                     
       do 458 l=1,nrbar(1)+nrmes(1)                                     
        if(ida(38).eq.3.or.ida(38).eq.4) then                           
          identl=issn(l)                                                
          if(identl.le.n1old+n2old.and.                                 
     a                     nclcnt(identl).eq.0.and.                     
     b                           kstor(identl,1).eq.0) then             
            lastcl(l)=-1                                                
          else if(l.gt.ntweak) then                                     
          else                                                          
            lastcl(l)=kstor(identl,1)                                   
          end if                                                        
        else                                                            
          lastcl(l)=0                                                   
        end if                                                          
               if((ida(38).eq.3.or.ida(38).eq.4).and.                   
     a                               lastcl(l).ne.0) then               
                 if(lastcl(l).eq.-1) then                               
                   do 464 i=0,3                                         
  464              x(i,l)=0.0                                           
                 else                                                   
                   do 463 i=1,4                                         
  463              x(i-1,l)=stork(identl,i)                             
                   pvec2=0.0                                            
                   do 563 i=1,3                                         
                    p(i,l)=stork(identl,i+5)                            
                    pvec2=pvec2+p(i,l)**2                               
  563              continue                                             
                   p(0,l)=sqrt(em(l)**2+pvec2)                          
                 end if                                                 
               else                                                     
                 if(switch(23)) then                                    
                   tl=tlast(l)-x(0,l)                                   
                 else                                                   
                   tl=x(0,l)-qdot(0,l)*dt                               
                   tlastl=tlast(l)                                      
                   tl=max(tlastl,tl)                                    
                 end if                                                 
                 if(qdot(0,l).gt.0.0) then                              
                   do 462 i=0,3                                         
  462              x(i,l)=x(i,l)+qdot(i,l)/qdot(0,l)*tl                 
                 end if                                                 
               end if                                                   
               if(npidec(l).lt.-99) then                                
                 npidec(l)=-mod(-npidec(l),100)                         
               end if                                                   
               if(nonint.ne.0) then                                     
                 if(ityp(l).eq.-9999)  then                             
                   nonint=nonint-1                                      
                   go to 458                                            
                 end if                                                 
               end if                                                   
               write(lun9,1907)  ityp(l),npidec(l),                        
     &                   (x(i,l),i=0,3),                                
     &                   (p(i,l),i=0,3),em(l),decay(l),                 
     &                    nclcnt(issn(l)),lastcl(l)                     
 1907  format(i2,i4,1x,f11.3,2f10.3,f11.3,                              
     a                 f10.4,2f9.4,f11.4,f7.4,1x,1pe12.4,1x,i3,1x,i3)   
  458  continue                                                         
       if(nonint.ne.0) call errex('subr. event: nonint counter wrong')  
      return                                                            
      entry rqmd_final                                                       
      write(lun8,*)'final information:'                                    
      write(lun8,*)'collisions in region: fb/fb  fb/sp    sp/sp'           
      write(lun8,*)'xcnt(1)=',xcnt(1)/float(nruns),                        
     a          'xcnt(2)=',xcnt(2)/float(nruns),                        
     b                                'xcnt(3)=',xcnt(3)/float(nruns)   
      if(iz1.le.-5) then                                                
          xnondf=float(inondf)/float(idifr)                             
          write(lun8,*) 'percentage of nondiffractive events',xnondf*100.0 
      end if                                                            
      if(da(11).lt.-99.) then                                           
          xnocol=float(inocol)/float(nruns)                             
          write(lun8,*) 'percentage of events without particle production:'
     a                ,xnocol*100.                                      
      end if                                                            
      if(nfrmcq.gt.0.or.nfrmhd.gt.0)                                    
     a   write(lun8,*) 'time delay for hadron production in jet decay'     
      if(nfrmcq.gt.0)                                                   
     a   write(lun8,*)  float(nfrmcq)/float(nruns),                        
     a            'hadrons containing constituent '                     
     b         , 'quarks with mean formation time',tfrmcq/nfrmcq,'fm/c' 
      if(nfrmhd.gt.0)                                                   
     a   write(lun8,*)  float(nfrmhd)/float(nruns),                        
     a            'new hadrons ',                                       
     b                  'with mean formation time',tfrmhd/nfrmhd,'fm/c' 
      write(lun8,*) 'information about the usage of the array nghbor and'  
      write(lun8,*)'the arrays dimensioned on nclmx'                       
      write(lun8,*) 'maximum value of nttot in relham=',nttomx             
      write(lun8,*) 'compared to the array size parameter nclmx=',nclmx    
      write(lun8,*) 'maximum value of used argument in the array nghbor=', 
     a            nghbmx                                                
      write(lun8,*) 'compared to the size of the array nghbor=',           
     a            ngbrmx*nclmx                                          
 99   write(lun8,8093) xcoll/float(nruns),                                 
     * xblock/float(nruns),                                             
     * xbldel/float(nruns),                                             
     * xininw/float(nruns)                                              
 8093 format(1x,/,'                      per run ',/,                   
     * '-----------------------------------',/,                         
     * ' collisions     =',f10.4,/,                                     
     * ' fermi blocks   =',f10.4,/,                                     
     * ' bardec. p.block =',f10.4,/,                                    
     * ' initial.p.block =',f10.4,/)                                    
      ierr=systim(ttotal,dummy)                                         
      write(lun8,8094)tmprop/float(nruns), tmcoll/float(nruns) ,           
     * tmsctr/float(nruns),tmpali/float(nruns),                         
     * ttotal/float(nruns)                                              
 8094 format('    timer report              sec/ensemble',//,           
     * ' hamilton - propag. =',f13.6,/,                                 
     * ' collisions   =',f13.6,/,                                       
     * ' scattering         =',f13.6,/,                                 
     * ' pauli blocking     =',f13.6,///,                               
     * ' total batch time =  ',f15.6,/)                                 
      write(lun8,*) 'cpu-time in absorb:',tmabsr/nruns                     
      return                                                            
      end                                                               
