*  $Id: ntuple.f,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
         include 'sc.inc'
                 common /maxevC/fdir,maxev
                 character *256 fdir
        character *256 fname,flist
         common /pawc/hm(4000000 )
        call hlimit(4000000) 
         i=0
         write(*,*)'ntuple list file name'
         read(*,*)flist
         write(*,*)'new ntuples dir'
         read(*,fmt='(a)')fdir
         write(*,*)fdir
         write(*,*)'max out'
         read(*,*)maxev
         open(20,file=flist,status='old')
10         read(20,fmt='(a)',end=100,err=100)fname
         i=i+1
           call openold(fname)
           call readold(1)
         goto 10           
100     continue
        if(i.gt.0)then
            call hcdir( '//output',' ')
            call hrout(0,ic,' ')
            call hrend('output')
            close(2)
        endif
        end

        subroutine readold(fast)
         include 'sc.inc'
          integer ipos,iposw
          data iposw /0/
          integer fast,userok
          call hcdir( '//input',' ')
          ipos=1
          newwrite=1        
10        continue         
           if(newwrite.eq.1)then
            newwrite=0
            if(fast.eq.1)then
             call hgntv(1,'eventstatus',1,ipos,ierr)
            else 
             call hgnt(1,ipos,ierr)
            endif
           else
            call hgntf(1,ipos,ierr) 
           endif
           if(ierr.ne.0)then
             call hrend('input')
             close(1)
             write(*,*)'input file closed ',ipos
             
           else
            if(userok(dummy).eq.1)then
             if(fast.eq.1)call hgnt(1,ipos,ierr)
             iposw=iposw+1
             if(mod(iposw,1000).eq.1)write(*,*)ipos,iposw
             call writenew(iposw,run,eventno)
             call hcdir( '//input',' ')
             newwrite=1
            endif
            ipos=ipos+1
            goto 10
           endif
        end
        subroutine writenew(iposw,runl,eventl)
                 common /maxevC/fdir,maxev
                 character *256 fdir
          integer runl,eventl
         include 'sc.inc'
         if(iposw.eq.1)then
          call opennew(runl,eventl)
         endif
         call hcdir( '//output',' ')
         call hfnt(2)
         if(iposw.gt.maxev)then
            call hrout(0,ic,' ')
            call hrend('output')
            close(2)
            iposw=0
         endif
        end
        subroutine openold(fname)
         include 'sc.inc'
         character *(*)fname
         call hropen(1,'input',fname,' ',8000,iostat)
         if(iostat.ne.0)then
          write(*,*)'Unable to open  input file',fname
          stop
         else
          write(*,*)'Opened  input file',fname(1:70)
         
         endif   
         call hcdir('//input',' ')
         call hrin(1,999,0)
         call hbname(1,' ',0,'$CLEAR')
      CALL HBNAME(1,'EVENTH',eventno,'$SET')
*
      CALL HBNAME(1,'BETA',nbeta,'$SET')
*
      CALL HBNAME(1,'CHARGE',ncharge,'$SET')
*
      CALL HBNAME(1,'PARTICLE',npart,'$SET')
*
      CALL HBNAME(1,'TOFCLUST',ntof,'$SET')
*
      CALL HBNAME(1,'TOFMCCLU',ntofmc,'$SET')
*
      CALL HBNAME(1,'TRCLUSTE',Ntrcl,'$SET')
*
      CALL HBNAME(1,'TRMCCLUS',ntrclmc,'$SET')
*
      CALL HBNAME(1,'TRRECHIT',ntrrh,'$SET')
*
      CALL HBNAME(1,'TRTRACK',ntrtr,'$SET')
*
      CALL HBNAME(1,'MCEVENTG',nmcg,'$SET')
*
      CALL HBNAME(1,'CTCCLUST',nctccl,'$SET')
*
      CALL HBNAME(1,'CTCMCCLU',nctcclmc,'$SET')
*
      CALL HBNAME(1,'ANTICLUS',nanti,'$SET')
*
      CALL HBNAME(1,'ANTIMCCL',nantimc,'$SET')
*
      CALL HBNAME(1,'LVL3',nlvl3,'$SET')
*
      CALL HBNAME(1,'LVL1',nlvl1,'$SET')
*
      CALL HBNAME(1,'CTCHIT',nctcht,'$SET')
*
      CALL HBNAME(1,'TRRAWCL',ntrraw,'$SET')
*
      CALL HBNAME(1,'ANTIRAWC',nantiraw,'$SET')
*
      CALL HBNAME(1,'TOFRAWCL',ntofraw,'$SET')
*


        end

        subroutine opennew(runl,eventl)
                 common /maxevC/fdir,maxev
                 character *256 fdir
        include 'sc.inc'    
        integer runl,eventl
        character *256 fname
        length=1
         write(*,*)fdir,len(fdir)
        do i=len(fdir),1,-1
          if(fdir(i:i).ne.' ')then
            length=i
            goto 10
          endif
        enddo
 10     continue
        write(fname,fmt='(a,a1,i9,a1,i7.7)')fdir(1:length)
     +   ,'/',runl,'.',eventl
        lun=2
        CALL HROPEN(2,'output',fname,'N',8000,iostat)
        if(iostat.ne.0)then
          write(*,*)'Unable to open  output file',fname
          stop
        else
          write(*,*)'Opened  output file',fname(1:80)
        endif
         call hcdir( '//output',' ')
        CALL HBNT(lun, 'AMS User Ntuple',' ')
        CALL HBNAME(lun,'EventH',Eventno,
     +  'eventno[0,10000000]:I,run:I,runtype:I,time(2):I,'//
     +  'RawWords[0,32000]:I,'//
     +  'RadS:R,ThetaS:R,PhiS:R,YawS:R,PitchS:R,RollS:R,VelocityS:R,'//
     +  'VelTheta:R,VelPhi:R,Particles[0,100]:I,Tracks[0,100]:I,'//
     +  'Betas[0,100]:I,Charges[0,100]:I,TrRecHits[0,10000]:I,'//
     +  'TrClusters[0,10000]:I,TrRawClusters[0,10000]:I,'//
     +  'TrMCClusters[0,10000]:I,TOFClusters[0,1000]:I,'//
     +  'TOFMCClusters[0,10000]:I,CTCClusters[0,100]:I,'//
     +  'CTCMCClusters[0,100000]:I,AntiMCClusters[0,10000]:I,'//
     +  'AntiClusters[0,100]:I,EventStatus:I')

         CALL HBNAME(lun,'Beta',Nbeta,
     +   'nbeta[0,100],betastatus(nbeta):I,'//
     +   'betapattern(nbeta)[0,100]:I,'//
     +   'beta(nbeta),betaerror(nbeta),betachi2(nbeta),'//
     +   'betachi2s(nbeta),'//
     +   'betantof(nbeta)[0,4]:I,betaptof(4,nbeta)[-1,1000]:I,'//
     +   'betaptr(nbeta)[-1,1000]:I')

         CALL HBNAME(lun,'Charge',Ncharge,
     +   'ncharge[0,10],chargestatus(ncharge):I,'//
     +   'chargebetap(ncharge)[-1,30000]:I,'//
     +   'chargetof(ncharge)[0,100]:I,'//
     +   'chargetracker(ncharge)[0,100]:I,probtof(10,ncharge),'//
     +   'probtracker(10,ncharge),truntof(ncharge),'//
     +   'truntracker(ncharge)')

         CALL HBNAME(lun,'Particle',Npart,
     +   'npart[0,10],pctcp(2,npart)[-1,30000]:I,'//
     +   'pbetap(npart)[0,30000]:I,pchargep(npart)[-1,30000]:I,'//
     +   'ptrackp(npart)[-1,30000]:I,pid(npart)[0,1000]:I,'//
     +   'pmass(npart),perrmass(npart),pmom(npart),perrmom(npart),'//
     +   'pcharge(npart),ptheta(npart),pphi(npart),thetagl(npart),'//
     +   'phigl(npart),pcoo(3,npart),signalctc(2,npart),'//
     +   'betactc(2,npart),errorbetactc(2,npart),cooctc(3,2,npart),'//
     +   'cootof(3,4,npart),cooanti(3,2,npart),cootr(3,6,npart)')

         CALL HBNAME(lun,'TOFClust',Ntof,
     +   'ntof[0,20],TOFStatus(ntof):I,plane(ntof)[0,10]:I,'//
     +   'bar(ntof)[0,100]:I,TOFEdep(ntof),TOFTime(ntof),'//
     +   'TOFETime(ntof),TOFCoo(3,ntof),TOFErCoo(3,ntof)')

         CALL HBNAME(lun,'TOFMCClu',Ntofmc,
     +  'ntofmc[0,200],TOFMCIdsoft(ntofmc):I,TOFMCXcoo(3,ntofmc),'//
     +  'TOFMCtof(ntofmc),TOFMCedep(ntofmc)')

         CALL HBNAME(lun,'TrCluste',Ntrcl, 
     +  'Ntrcl[0,200],Idsoft(ntrcl):I,Statust(ntrcl):I,'//
     +  'NelemL(ntrcl)[-100,100]:I,NelemR(ntrcl)[0,100]:I,'//
     +  'Sumt(ntrcl),'//
     +  'Sigmat(ntrcl),Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),'//
     +  'Amplitude(5,ntrcl):R')

         CALL HBNAME(lun,'TrMCClus',Ntrclmc,
     +  'ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),'//
     +  'Left(2,ntrclmc):I,Center(2,ntrclmc):I,Right(2,ntrclmc):I,'//
     +  'ss(5,2,ntrclmc),xca(3,ntrclmc),xcb(3,ntrclmc),'//
     +  'xgl(3,ntrclmc),summc(ntrclmc)')

         CALL HBNAME(lun,'TrRecHit',Ntrrh,
     +  'ntrrh[0,500],px(ntrrh)[-1,30000]:I,py(ntrrh)[-1,30000]:I,'//
     +  'statusr(ntrrh):I,Layer(ntrrh)[0,10]:I,hitr(3,ntrrh),'//
     +  'ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh)')

         CALL HBNAME(lun,'TrTrack',Ntrtr,
     +  'ntrtr[0,20],trstatus(ntrtr):I,pattern(ntrtr)[0,100]:I,'//
     +  'nhits(ntrtr)[0,6],phits(6,ntrtr)[-1,30000]:I,'//
     +  'FastFitDone(ntrtr)[0,1000]:I,GeaneFitDone(ntrtr)[0,1000]:I,'//
     +  'AdvancedFitDone(ntrtr)[0,1000]:I,Chi2StrLine(ntrtr),'//
     +  'Chi2Circle(ntrtr),CircleRidgidity(ntrtr),'//
     +  'Chi2FastFit(ntrtr),'//
     +  'Ridgidity(ntrtr),ErrRidgidity(ntrtr),Theta(ntrtr),'//
     +  'phi(ntrtr),'//
     +  'p0(3,ntrtr),gchi2(ntrtr),gridgidity(ntrtr),'//
     +  'gerrridgidity(ntrtr),'//
     +  'gtheta(ntrtr),gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),'//
     +  'HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),'//
     +  'hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),gchi2ms(ntrtr),'//
     +  'ridgidityms(ntrtr),gridgidityms(ntrtr)')

         CALL HBNAME(lun,'MCEventG',Nmcg,
     +  'nmcg[0,20],nskip(nmcg):I,Particle(nmcg)[-1,30000]:I,'//
     +  'coo(3,nmcg),dir(3,nmcg),momentum(nmcg):R,mass(nmcg):R,'//
     +  'charge(nmcg):R')

         CALL HBNAME(lun,'CTCClust',Nctccl,
     +  'nctccl[0,20],CTCStatus(nctccl):I,CTCLayer(nctccl)[0,10]:I,'//
     +  'ctccoo(3,nctccl),ctcercoo(3,nctccl),ctcrawsignal(nctccl),'//
     +  'ctcsignal(nctccl),ctcesignal(nctccl)')

         CALL HBNAME(lun,'CTCMCClu',Nctcclmc,
     +  'nctcclmc[0,200],CTCMCIdsoft(nctcclmc):I,'//
     +  'CTCMCXcoo(3,nctcclmc),'//
     +  'CTCMCXdir(3,nctcclmc),CTCstep(nctcclmc),'//
     + 'ctccharge(nctcclmc),'//
     +  'ctcbeta(nctcclmc),ctcedep(nctcclmc)')

         CALL HBNAME(lun,'AntiClus',Nanti,
     +  'nanti[0,16],AntiStatus(nanti):I,'//
     +  'AntiSector(nanti)[0,100]:I,'//
     +  'AntiEdep(nanti),AntiCoo(3,nanti),AntiErCoo(3,nanti)')

          CALL HBNAME(lun,'AntiMCCl',Nantimc,
     +  'nantimc[0,200],AntiMCIdsoft(nantimc):I,'//
     +  'AntiMCXcoo(3,nantimc),'//
     +  'AntiMCtof(nantimc),AntiMCedep(nantimc)')

          CALL HBNAME(lun,'LVL3',Nlvl3,
     +  'nlvl3[0,2],LVL3TOFTr(nlvl3)[0,10],LVL3AntiTr(nlvl3)[0,10],'//
     +  'LVL3TrackerTr(nlvl3),LVL3NTrHits(nlvl3)[0,1000],'//
     +  'LVL3NPat(nlvl3)[0,10],LVL3Pattern(2,nlvl3)[-1,100],'//
     +  'LVL3Residual(2,nlvl3):R,LVL3Time(nlvl3):R,LVL3ELoss(nlvl3):R')

         CALL HBNAME(lun,'LVL1',Nlvl1,
     +  'nlvl1[0,2],LVL1LifeTime(nlvl1),'//
     +  'LVL1Flag(nlvl1)[-10,10],LVL1TOFPatt(4,nlvl1),'//
     +  'LVL1TOFPatt1(4,nlvl1),LVL1AntiPatt(nlvl1)')

         CALL HBNAME(lun,'CTCHit',Nctcht,
     +  'nctcht[0,50],CTChitStatus(nctcht):I,'//
     +  'CTChitLayer(nctcht)[0,10]:I,ctchitcolumn(nctcht)[0,100]:I,'//
     +  'ctchitrow(nctcht)[0,100]:I,ctchitsignal(nctcht)')

         CALL HBNAME(lun,'TrRawCl',Ntrraw,
     +  'ntrraw[0,500],rawaddress(ntrraw):I,'//
     +  'rawlength(ntrraw)[-1,30000]:I,s2n(ntrraw):R')

         CALL HBNAME(lun,'AntiRawC',Nantiraw,
     +  'nantiraw[0,32],antirawstatus(nantiraw):I,'//
     +  'antirawsector(nantiraw)[0,100]:I,'//
     +  'antirawupdown(nantiraw)[0,7]:I,antirawsignal(nantiraw)')

         CALL HBNAME(lun,'TOFRawCl',Ntofraw,
     +  'ntofraw[0,20],tofrstatus(ntofraw):I,'//
     +  'tofrplane(ntofraw)[0,7]:I,'//
     +  'tofrbar(ntofraw)[0,31]:I,tofrtovta(2,ntofraw),'//
     +  'tofrtovtd(2,ntofraw),tofrsdtm(2,ntofraw)')

         end
         integer function userok(dummy)
         include 'sc.inc'
*
*  write only heavy ions or negative momentum 
*       
         userok=0
         if(mod(eventstatus/2097152,4).gt.0)then
          ic=mod(eventstatus/32,8)
          im=mod(eventstatus/256,2)
          if(ic.gt.0.or.im.eq.0)userok=1
         endif 
         end
