*  $Id: convert.f,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
      character *128 fnam
      character *128 fnamo
      common /pawc/hl(10000000)
      call hlimit(10000000)
      write(*,*)'old Ntuple file name '
      read(*,fmt='(a)')fnam
      write(*,*)'new Ntuple file name '
      read(*,fmt='(a)')fnamo
      call prepare(fnam)
      write(*,*)'Starting converting'
      call convert(fnamo)
      end
      subroutine convert(fnam)
*********************************************************
*                                                       *
* Prepare to convert stupid ntuple into more clever one
*                                                       *
*********************************************************
*
*     Ntuple Id:      1    
*     Ntuple Title:   Simulation
*     Creation:       24/07/96 10.09.21
*
***********************
************************
      character *(*)  fnam
      LOGICAL         CHAIN
      CHARACTER*128   CFILE
      INTEGER         IDNEVT,NCHEVT,ICHEVT
      REAL            OBS(13)
*
      COMMON /PAWIDN/ IDNEVT,OBS
      COMMON /PAWCHN/ CHAIN, NCHEVT, ICHEVT
      COMMON /PAWCHC/ CFILE
*
*--   Ntuple Variable Declarations
*

*
      REAL Beta,BetaError,BetaChi2,ProbTOF,ProbTracker,PMass
     + ,PErrMass,PMom,PErrMom,PCharge,PTheta,PPhi,PCoo,PAnti
     + ,SignalCTC,BetaCTC,ErrorBetaCTC,CooCTC,TOFEdep,TOFTime
     + ,TOFETime,TOFCoo,TOFErCoo,TOFMCXcoo,TOFMCtof,TOFMCedep
     + ,Sumt,Sigmat,Meant,RMSt,ErrorMeant,SS,Xca,Xcb,Xgl,SumMC
     + ,HitR,EhitR,SumR,DifoSum,Chi2StrLine,Chi2Circle
     + ,CircleRidgidity,Chi2FastFit,Ridgidity,ErrRidgidity,Theta,Phi
     + ,P0,GChi2,GRidgidity,GErrRidgidity,GTheta,GPhi,GP0,HChi2 
     + ,HRidgidity ,HErrRidgidity ,HTheta ,HPhi ,HP0
     + ,FChi2MS,GChi2MS,RidgidityMS,GRidgidityMS,Coo,Dir,Momentum
     + ,Mass,Charge,CTCCoo,CTCErCoo,CTCRawSignal,CTCSignal
     + ,CTCESignal,CTCMCXcoo,CTCMCXdir,CTCstep,CTCcharge,CTCbeta
     + ,Amplitude,CTCedep
      INTEGER EventNo,Run,RunType,Time ,BetaEvent,BetaPattern
     + ,ChargeEvent,ChargeBetaP,ChargeTOF,ChargeTracker,ParticleEvent
     + ,PBetaPointer,PChargePointer,PtrackPointer,ParticleId,TOFCluster
     + ,TOFStatus,Ntof,Plane,TOFMCEvent,TOFMCIdsoft,TrCluster,Idsoft
     + ,Status,NelemL,NelemR,TrMCCluster,IdsoftMC,Itra,Left ,Center 
     + ,Right ,TrRecHit,StatusR,Layer,TrTrack,TrStatus,Pattern,NHits
     + ,phits,PCTCPointer,FastFitdone
     + ,GeaneFitDone,AdvancedFitDone,EventNoMCEventG,Particle,CTCCluster
     + ,CTCStatus,CTCLayer,CTCMCEvent,CTCMCIdsoft,bar,px,py,
     +  Particles, Tracks, Betas, Charges ,TrRecHits, TrClusters,
     +  TrMCClusters, TOFClusters, TOFMCClusters, CTCClusters,
     +  CTCMCClusters
*
*
*
*
*    idet=1
*
 
      parameter (maxbeta=10)
      parameter (maxch=10)
      parameter (maxpa=10)
      parameter(maxtof=20)
      parameter(maxtofmc=200)
      parameter(maxtrcl=50)
      parameter(maxtrclmc=200)
      parameter(maxtrrh=200)
      parameter(maxtrtr=20)
      parameter(maxmcg=20)
      parameter(maxctccl=20)
      parameter(maxctcclmc=200)

      common /eventhC/eventno,run,runtype,time,
     +    Particles, Tracks, Betas, Charges ,TrRecHits,
     +    TrClusters, TrMCClusters, TOFClusters, TOFMCClusters,
     +    CTCClusters, CTCMCClusters

      common /betaC/nbeta,
     + betapattern(maxbeta),beta(maxbeta),
     +              betaerror(maxbeta),betachi2(maxbeta)

      common /chargeC/ ncharge,
     + chargebetap(maxch),chargetof(maxch),
     + chargetracker(maxch),probtof(7,maxch),probtracker(7,maxch)

      common /particleC/npart,pctcpointer(2,maxpa),
     + pbetapointer(maxpa),pchargepointer(maxpa),
     + ptrackpointer(maxpa),particleid(maxpa),pmass(maxpa),
     + perrmass(maxpa),
     + pmom(maxpa),perrmom(maxpa),pcharge(maxpa),
     + ptheta(maxpa),pphi(maxpa),pcoo(3,maxpa),panti(maxpa),
     + signalctc(2,maxpa),
     + betactc(2,maxpa),errorbetactc(2,maxpa),cooctc(3,2,maxpa)

      common /TOFClusterC/ntof,
     + TOFStatus(maxtof),plane(maxtof),
     + bar(maxtof),
     + TOFEdep(maxtof),
     + TOFTime(maxtof),TOFETime(maxtof),TOFCoo(3,maxtof),
     + TOFErCoo(3,maxtof)

      common /TOFMCClusterC/ntofmc,
     + TOFMCIdsoft(maxtofmc),
     + TOFMCXcoo(3,maxtofmc),TOFMCtof(maxtofmc),
     + TOFMCedep(maxtofmc)
      
      common /TrClusterC/ntrcl,
     + Idsoft(maxtrcl),Statust(maxtrcl),NelemL(maxtrcl),
     + NelemR(maxtrcl),Sumt(maxtrcl),Sigmat(maxtrcl),
     + Meant(maxtrcl),RMSt(maxtrcl),ErrorMeant(maxtrcl),
     + Amplitude(5,maxtrcl)
    
      common/TrMCClusterC/ntrclmc,
     + IdsoftMC(maxtrclmc),Itra(maxtrclmc),
     + Left(2,maxtrclmc),
     + Center(2,maxtrclmc),
     + Right(2,maxtrclmc),ss(5,2,maxtrclmc),xca(3,maxtrclmc),
     + xcb(3,maxtrclmc),
     + xgl(3,maxtrclmc),summc(maxtrclmc)

      common/TrRecHitC/ntrrh,px(maxtrrh),py(maxtrrh),
     + statusr(maxtrrh),Layer(maxtrrh),
     + hitr(3,maxtrrh),ehitr(3,maxtrrh),sumr(maxtrrh),difosum(maxtrrh)

      common/TrTrackC/ntrtr,
     + trstatus(maxtrtr),pattern(maxtrtr),nhits(maxtrtr),
     + phits(6,maxtrtr),FastFitdone(maxtrtr),
     + GeaneFitDone(maxtrtr),
     + AdvancedFitDone(maxtrtr),Chi2StrLine(maxtrtr),
     + Chi2Circle(maxtrtr),CircleRidgidity(maxtrtr),
     + Chi2FastFit(maxtrtr),Ridgidity(maxtrtr),ErrRidgidity(maxtrtr),
     + Theta(maxtrtr),phi(maxtrtr),p0(3,maxtrtr),gchi2(maxtrtr),
     + gridgidity(maxtrtr),gerrridgidity(maxtrtr),gtheta(maxtrtr), 
     + gphi(maxtrtr),gp0(3,maxtrtr),hchi2(2,maxtrtr),
     + HRidgidity(2,maxtrtr),HErrRidgidity(2,maxtrtr),htheta(2,maxtrtr),
     + hphi(2,maxtrtr),hp0(3,2,maxtrtr),fchi2ms(maxtrtr),
     + gchi2ms(maxtrtr),ridgidityms(maxtrtr),gridgidityms(maxtrtr)

      common/McEventGC/nmcg,
     + Particle(maxmcg),coo(3,maxmcg),dir(3,maxmcg),
     + momentum(maxmcg),mass(maxmcg),charge(maxmcg)

      common/CTCClusterC/nctccl,
     + CTCStatus(maxctccl),CTCLayer(maxctccl),ctccoo(3,maxctccl),
     + ctcercoo(3,maxctccl),
     + ctcrawsignal(maxctccl),ctcsignal(maxctccl),ctcesignal(maxctccl)

      common/CTCMCClusterC/nctcclmc,
     + CTCMCIdsoft(maxctcclmc),CTCMCXcoo(3,maxctcclmc),
     + CTCMCXdir(3,maxctcclmc),CTCstep(maxctcclmc),
     + ctccharge(maxctcclmc),
     + ctcbeta(maxctcclmc),ctcedep(maxctcclmc)
      character *64 cdet
      parameter (ndet=13)
      character *64 cblock(ndet)
      integer events(ndet),eof(ndet)
      common /quest/iquest(100)
      integer runold
      logical newrun,truth/.true./
*
*--   Enter user code here
*
c       iquest(10)=128000
       call HROPEN(4,'output',fnam,'N',1024,iostat)
       if(iostat.ne.0)then
        write(*,*)'Unable to open output file ',fnam
        stop
       endif
       cblock(1)='EVENTH'
       cblock(2)='BETA'
       cblock(3)='CHARGE'
       cblock(4)='PARTICLE'
       cblock(5)='TOFCLUST'
       cblock(6)='TOFMCCLU'
       cblock(7)='TRCLUSTE'
       cblock(8)='TRMCCLUS'
       cblock(9)='TRRECHIT'
       cblock(10)='TRTRACK'
       cblock(11)='MCEVENTG'
       cblock(12)='CTCCLUST'
       cblock(13)='CTCMCCLU'
       do idet=1,ndet
        open(40+idet,file=cblock(idet),status='old',
     +  form='unformatted',err=998)
        eof(idet)=0
        events(idet)=0
       enddo
        call hbnt(1,'Converted Ntuple',' ')
       call hbname(1,cblock(1),Eventno,
     + 'eventno:I,run:I,runtype:I,time(2):I, Particles:I,'//
     + 'Tracks:I, Betas:I, Charges:I ,TrRecHits:I, TrClusters:I,'//
     + 'TrMCClusters:I, TOFClusters:I, TOFMCClusters:I,'//
     + 'CTCClusters:I, CTCMCClusters:I')

      call hbname(1,cblock(2),nbeta,
     + 'nbeta[0,10],betapattern(nbeta):I,beta(nbeta),'//
     + 'betaerror(nbeta),betachi2(nbeta)')

      call hbname(1,cblock(3), ncharge,
     + 'ncharge[0,10],chargebetap(ncharge),chargetof(ncharge):I,'//
     + 'chargetracker(ncharge):I,probtof(7,ncharge),'//
     + 'probtracker(7,ncharge)')

      call hbname(1,cblock(4), npart,
     + 'npart[0,10],pctcp(2,npart):I,'//
     + 'pbetap(npart):I,pchargep(npart):I,'//
     + 'ptrackp(npart):I,pid(npart):I,pmass(npart),'//
     + 'perrmass(npart),'//
     + 'pmom(npart),perrmom(npart),pcharge(npart),'//
     + 'ptheta(npart),pphi(npart),pcoo(3,npart),panti(npart),'//
     + 'signalctc(2,npart),'//
     + 'betactc(2,npart),errorbetactc(2,npart),cooctc(3,2,npart)')

      call hbname(1,cblock(5), ntof,
     + 'ntof[0,20],TOFStatus(ntof):I,plane(ntof):I,'//
     + 'bar(ntof):I,'//
     + 'TOFEdep(ntof),'//
     + 'TOFTime(ntof),TOFETime(ntof),TOFCoo(3,ntof),'//
     + 'TOFErCoo(3,ntof)')

      call hbname(1,cblock(6), ntofmc,
     + 'ntofmc[0,200],TOFMCIdsoft(ntofmc):I,'//
     + 'TOFMCXcoo(3,ntofmc),TOFMCtof(ntofmc),'//
     + 'TOFMCedep(ntofmc)')
      
      call hbname(1,cblock(7), ntrcl,
     + 'ntrcl[0,50],Idsoft(ntrcl):I,Statust(ntrcl):I,'//
     + 'NelemL(ntrcl):I,NelemR(ntrcl):I,'//
     + 'Sumt(ntrcl),Sigmat(ntrcl),'//
     + 'Meant(ntrcl):R,RMSt(ntrcl),ErrorMeant(ntrcl),'//
     + 'Amplitude(5,ntrcl):R')
     
      call hbname(1,cblock(8),ntrclmc,
     +'ntrclmc[0,200],IdsoftMC(ntrclmc),Itra(ntrclmc),'//
     +' Left(2,ntrclmc):I,'//
     + 'Center(2,ntrclmc):I,'//
     + 'Right(2,ntrclmc):I,ss(5,2,ntrclmc),xca(3,ntrclmc),'//
     + 'xcb(3,ntrclmc),'//
     + 'xgl(3,ntrclmc),summc(ntrclmc)')

      call hbname(1,cblock(9),ntrrh,
     + 'ntrrh[0,200],px(ntrrh):I,py(ntrrh):I,'//
     + 'statusr(ntrrh):I,Layer(ntrrh):I,'//
     + 'hitr(3,ntrrh),ehitr(3,ntrrh),sumr(ntrrh),difosum(ntrrh)')

      call hbname(1,cblock(10),ntrtr,
     + 'ntrtr[0,20],trstatus(ntrtr):I,pattern(ntrtr):I,'//
     + 'nhits(ntrtr),phits(6,ntrtr):I,'//
     + 'FastFitDone(ntrtr):I,'//
     + 'GeaneFitDone(ntrtr):I,'//
     + 'AdvancedFitDone(ntrtr):I,Chi2StrLine(ntrtr),'//
     + 'Chi2Circle(ntrtr),CircleRidgidity(ntrtr),'//
     + 'Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),'//
     + 'Theta(ntrtr),phi(ntrtr),p0(3,ntrtr),gchi2(ntrtr),'//
     + 'gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr), '//
     + 'gphi(ntrtr),gp0(3,ntrtr),hchi2(2,ntrtr),'//
     + 'HRidgidity(2,ntrtr),HErrRidgidity(2,ntrtr),htheta(2,ntrtr),'//
     + 'hphi(2,ntrtr),hp0(3,2,ntrtr),fchi2ms(ntrtr),'//
     + 'gchi2ms(ntrtr),ridgidityms(ntrtr),gridgidityms(ntrtr)')

      call hbname(1,cblock(11),nmcg,
     + 'nmcg[0,20],Particle(nmcg):I,coo(3,nmcg),dir(3,nmcg),'//
     + 'momentum(nmcg):R,mass(nmcg):R,charge(nmcg):R')

      call hbname(1,cblock(12),nctccl,
     + 'nctccl[0,20],CTCStatus(nctccl):I,CTCLayer(nctccl):I,'//
     + 'ctccoo(3,nctccl),ctcercoo(3,nctccl),'//
     + 'ctcrawsignal(nctccl),'//
     + 'ctcsignal(nctccl),ctcesignal(nctccl)')

      call hbname(1,cblock(13),nctcclmc,
     + 'nctcclmc[0,200],CTCMCIdsoft(nctcclmc):I,'//
     + 'CTCMCXcoo(3,nctcclmc),'//
     + 'CTCMCXdir(3,nctcclmc),CTCstep(nctcclmc),'//
     + 'ctccharge(nctcclmc),ctcbeta(nctcclmc),ctcedep(nctcclmc)')

        runold=-1
        do i=1,ndet
          events(idet)=0
        enddo
       do ll=1,100000000
        idet=1
        lun=40+idet
        read(lun,end=15)eventno,run,runtype,time, Particles ,
     +   Tracks , Betas , Charges  ,TrRecHits , TrClusters , 
     +   TrMCClusters , TOFClusters , TOFMCClusters , 
     +   CTCClusters , CTCMCClusters 
        if(run.ne.runold)then
          write(*,*)' New run # ',run
          runold=run
        endif
        goto 16
 15     eof(idet)=1
 16     continue
        events(idet)=eventno
        if(eof(1).ne.0)goto 999
c        if(mod(ll,1000).eq.1)write(*,*)'eventh ',eventno,ll
        idet=2
        lun=40+idet
         nbeta=1
        do while(truth)
          if(eof(idet).eq.0)read(lun,end=25)
     +    ne,betapattern(nbeta),
     +    beta(nbeta),betaerror(nbeta),betachi2(nbeta)
          goto 26
 25       eof(idet)=1
 26       continue
          if(eof(idet).ne.0)then
             events(idet)=0
          else
            if(events(idet).gt.ne)then
              newrun=.true.
            endif
            events(idet)=ne
          endif              
          if(eof(idet).ne.0 .or. 
     +    events(idet).gt.events(1).or.newrun)then
           if(eof(idet).eq.0)backspace lun
           nbeta=nbeta-1
           newrun=.false.
           goto 10
          else if (events(idet).lt.events(1))then
           nbeta=nbeta-1
          endif
          nbeta=min(nbeta+1,maxbeta)
        enddo   
 10     continue
        idet=3
        lun=40+idet
        ncharge=1
        do while (truth)
           if(eof(idet).eq.0)read(lun,end=35)
     +     ne,chargebetap(ncharge),chargetof(ncharge),
     +     chargetracker(ncharge),(probtof(k,ncharge),k=1,7),
     +     (probtracker(k,ncharge),k=1,7)
          goto 36
 35       eof(idet)=1
 36       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ncharge=ncharge-1
           newrun=.false.
          goto 20
         else if (events(idet).lt.events(1))then
          ncharge=ncharge-1
         endif
          ncharge=min(ncharge+1,maxch)
        enddo   
 20     continue
        idet=4
        lun=40+idet
        npart=1
        do while(truth)
           if(eof(idet).eq.0)read(lun,end=45)
     +     ne,(pctcpointer(kk,npart),kk=1,2),
     +     pbetapointer(npart),pchargepointer(npart),
     +     ptrackpointer(npart),particleid(npart),pmass(npart),
     +     perrmass(npart),pmom(npart),perrmom(npart),pcharge(npart),
     +     ptheta(npart),pphi(npart),(pcoo(k,npart),k=1,3),
     +     panti(npart),
     +     (signalctc(kk,npart),kk=1,2),
     +     (betactc(kk,npart),kk=1,2),(errorbetactc(kk,npart),kk=1,2),
     +     ((cooctc(k,kk,npart),k=1,3),kk=1,2)
          goto 46
 45       eof(idet)=1
 46       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           npart=npart-1
           newrun=.false.
          goto 30
          else if (events(idet).lt.events(1))then
           npart=npart-1
         endif
           npart=min(npart+1,maxpa)
        enddo   
 30     continue
        idet=5
        lun=40+idet
        ntof=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=55)
     +     ne,TOFStatus(ntof),Plane(ntof),bar(ntof),TOFEdep(ntof),
     +     TOFTime(ntof),TOFETime(ntof),(TOFCoo(k,ntof),k=1,3),
     +    (TOFErCoo(k,ntof),k=1,3)
          goto 56
 55       eof(idet)=1
 56       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntof=ntof-1
           newrun=.false.
          goto 40
          else if (events(idet).lt.events(1))then
           ntof=ntof-1
         endif
           ntof=min(ntof+1,maxtof)
        enddo   
 40     continue
        idet=6
        lun=40+idet
        ntofmc=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=65)
     +     ne,TOFMCIdsoft(ntofmc),
     +     (TOFMCXcoo(k,ntofmc),k=1,3),TOFMCtof(ntofmc),
     +     TOFMCedep(ntofmc)
           goto 66
 65       eof(idet)=1
 66       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntofmc=ntofmc-1
           newrun=.false.
          goto 50
          else if (events(idet).lt.events(1))then
            ntofmc=ntofmc-1
         endif
           ntofmc=min(ntofmc+1,maxtofmc)
        enddo   
 50     continue
        idet=7
        lun=40+idet
        ntrcl=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=75)
     +    ne,Idsoft(ntrcl),Statust(ntrcl),NelemL(ntrcl),
     +    NelemR(ntrcl),Sumt(ntrcl),Sigmat(ntrcl),
     +    Meant(ntrcl),RMSt(ntrcl),ErrorMeant(ntrcl),
     +    (Amplitude(kk,ntrcl),kk=1,5)
          goto 76
 75       eof(idet)=1
 76       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntrcl=ntrcl-1
           newrun=.false.
          goto 60
          else if (events(idet).lt.events(1))then
           ntrcl=ntrcl-1
         endif
           ntrcl=min(ntrcl+1,maxtrcl)
        enddo   
 60     continue
        idet=8
        lun=40+idet
        ntrclmc=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=85)
     +     ne,IdsoftMC(ntrclmc),Itra(ntrclmc),
     +     (Left(k,ntrclmc),k=1,2),
     +     (Center(k,ntrclmc),k=1,2),
     +     (Right(k,ntrclmc),k=1,2),
     +     ((ss(k,l,ntrclmc),k=1,5),l=1,2),
     +     (xca(k,ntrclmc),k=1,3),
     +     (xcb(k,ntrclmc),k=1,3),
     +     (xgl(k,ntrclmc),k=1,3),summc(ntrclmc) 
           goto 86
 85       eof(idet)=1
 86       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntrclmc=ntrclmc-1
           newrun=.false.
          goto 70
          else if (events(idet).lt.events(1))then
            ntrclmc=ntrclmc-1
         endif
           ntrclmc=min(ntrclmc+1,maxtrclmc)
        enddo   
 70     continue
        idet=9
        lun=40+idet
        ntrrh=1
        do while(truth) 
           iostat=1
           if(eof(idet).eq.0)read(lun,end=95)
     +     ne,px(ntrrh),py(ntrrh),statusr(ntrrh),Layer(ntrrh),
     +     (hitr(k,ntrrh),k=1,3),
     +     (ehitr(k,ntrrh),k=1,3),sumr(ntrrh),difosum(ntrrh)
           goto 96
 95       eof(idet)=1
 96       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntrrh=ntrrh-1
           newrun=.false.
          goto 80
          else if (events(idet).lt.events(1))then
            ntrrh=ntrrh-1
         endif
           ntrrh=min(ntrrh+1,maxtrrh)
        enddo   
 80     continue
          if(ntrrh.eq.0)then
c            write(*,*)' ntrrh eq 0 ', events
          endif

        idet=10
        lun=40+idet
        ntrtr=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=105)
     +     ne,trstatus(ntrtr),pattern(ntrtr),nhits(ntrtr),
     +     (phits(kk,ntrtr),kk=1,6),
     +     FastFitDone(ntrtr),GeaneFitDone(ntrtr),
     +     AdvancedFitDone(ntrtr),Chi2StrLine(ntrtr),Chi2Circle(ntrtr),
     +     CircleRidgidity(ntrtr),
     +     Chi2FastFit(ntrtr),Ridgidity(ntrtr),ErrRidgidity(ntrtr),
     +     Theta(ntrtr),phi(ntrtr),(p0(k,ntrtr),k=1,3),gchi2(ntrtr),
     +     gridgidity(ntrtr),gerrridgidity(ntrtr),gtheta(ntrtr), 
     +     gphi(ntrtr),(gp0(k,ntrtr),k=1,3),
     +     (hchi2(k,ntrtr),k=1,2),
     +     (HRidgidity(k,ntrtr),k=1,2),(HErrRidgidity(k,ntrtr),k=1,2),
     +     (htheta(k,ntrtr),k=1,2),
     +     (hphi(k,ntrtr),k=1,2),
     +     ((hp0(k,l,ntrtr),k=1,3),l=1,2),fchi2ms(ntrtr),gchi2ms(ntrtr),
     +     ridgidityms(ntrtr),gridgidityms(ntrtr) 
           goto 106
 105       eof(idet)=1
 106       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           ntrtr=ntrtr-1
           newrun=.false.
          goto 90
          else if (events(idet).lt.events(1))then
           ntrtr=ntrtr-1
         endif
           ntrtr=min(ntrtr+1,maxtrtr)
        enddo   
 90     continue
        idet=11
        lun=40+idet
        nmcg=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=115)
     +     ne,Particle(nmcg),(coo(k,nmcg),k=1,3),
     +     (dir(k,nmcg),k=1,3),
     +     momentum(nmcg),mass(nmcg),charge(nmcg)
           goto 116
 115       eof(idet)=1
 116       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           nmcg=nmcg-1
           newrun=.false.
          goto 100
          else if (events(idet).lt.events(1))then
          nmcg=nmcg-1
         endif
           nmcg=min(nmcg+1,maxmcg)
        enddo   
 100     continue
        idet=12
        lun=40+idet
        nctccl=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=125)
     +     ne,CTCStatus(nctccl),ctclayer(nctccl),
     +     (ctccoo(k,nctccl),k=1,3),
     +     (ctcercoo(k,nctccl),k=1,3),
     +     ctcrawsignal(nctccl),ctcsignal(nctccl),ctcesignal(nctccl) 
           goto 126
 125       eof(idet)=1
 126       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           nctccl=nctccl-1
           newrun=.false.
          goto 110
          else if (events(idet).lt.events(1))then
              nctccl=nctccl-1
         endif
           nctccl=min(nctccl+1,maxctccl)
        enddo   
 110     continue
        idet=13
        lun=40+idet
        nctcclmc=1
        do while(truth)
           iostat=1
           if(eof(idet).eq.0)read(lun,end=135)
     +     ne, CTCMCIdsoft(nctcclmc),(CTCMCXcoo(k,nctcclmc),k=1,3),
     +     (CTCMCXdir(k,nctcclmc),k=1,3),
     +     CTCstep(nctcclmc),ctccharge(nctcclmc),
     +     ctcbeta(nctcclmc),ctcedep(nctcclmc)
           goto 136
 135       eof(idet)=1
 136       continue
          if(eof(idet).ne.0)then
             events(idet)=0
           else
            if(events(idet).gt.ne)newrun=.true.
            events(idet)=ne
           endif              
         if(eof(idet).ne.0 .or. events(idet).gt.events(1).or.newrun)then
          if(eof(idet).eq.0)backspace lun
           nctcclmc=nctcclmc-1
           newrun=.false.
          goto 120
          else if (events(idet).lt.events(1))then
              nctcclmc=nctcclmc-1
         endif
           nctcclmc=min(nctcclmc+1,maxctcclmc)
        enddo   
 120     continue
c       if(mod(ll,1000).eq.1)write(*,*)' hfnt ',npart,nmcg
       call hfnt(1)
       enddo 
 999   continue
       do i=1,ndet
        close(40+i,status='delete')
       enddo
       ic=0
       call hrout(0,ic,' ')
       call hrend('output')
       close(4)
       return
 998   continue
       write(*,*)'Unable to open file ',cblock(idet)
       do i=1,idet-1
        close(40+i,status='delete')
       enddo
*
      end
      subroutine prepare(fnam)
*********************************************************
*                                                       *
* Prepare to convert stupid ntuple into more clever one
*                                                       *
*********************************************************
*
*     Ntuple Id:      1    
*     Ntuple Title:   Simulation
*     Creation:       24/07/96 10.09.21
*
*********************************************************
*
      character *(*)  fnam
      LOGICAL         CHAIN
      CHARACTER*128   CFILE
      INTEGER         IDNEVT,NCHEVT,ICHEVT
      REAL            OBS(13)
*
      COMMON /PAWIDN/ IDNEVT,OBS
      COMMON /PAWCHN/ CHAIN, NCHEVT, ICHEVT
      COMMON /PAWCHC/ CFILE
*
*--   Ntuple Variable Declarations
*

      REAL Beta,BetaError,BetaChi2,ProbTOF(7),ProbTracker(7),PMass
     + ,PErrMass,PMom,PErrMom,PCharge,PTheta,PPhi,PCoo(3),PAnti
     + ,SignalCTC(2),BetaCTC(2),ErrorBetaCTC(2),
     +  CooCTC(3,2),TOFEdep,TOFTime
     + ,TOFETime,TOFCoo(3),TOFErCoo(3),TOFMCXcoo(3),TOFMCtof,TOFMCedep
     + ,Sumt,Sigmat,Meant,RMSt,ErrorMeant,
     +  SS(5,2),Xca(3),Xcb(3),Xgl(3),SumMC,Amplitude(5)
     + ,HitR(3),EhitR(3),SumR,DifoSum,Chi2StrLine,Chi2Circle
     + ,CircleRidgidity,Chi2FastFit,Ridgidity,ErrRidgidity,Theta,Phi
     + ,P0(3),GChi2,GRidgidity,GErrRidgidity,GTheta,GPhi,GP0(3),HChi2(2)
     + ,HRidgidity(2),HErrRidgidity(2),HTheta(2),HPhi(2),HP0(3,2)
     + ,FChi2MS,GChi2MS,RidgidityMS,GRidgidityMS,Coo(3),Dir(3),Momentum
     + ,Mass,Charge,CTCCoo(3),CTCErCoo(3),CTCRawSignal,CTCSignal
     + ,CTCESignal,CTCMCXcoo(3),CTCMCXdir(3),CTCstep,CTCedep,CTCbeta
      INTEGER EventNo,Run,RunType,Time(2),BetaEvent,BetaPattern
     + ,ChargeEvent,ChargeBetaP,ChargeTOF,ChargeTracker,ParticleEvent
     + ,PBetaPointer,PChargePointer,PtrackPointer,ParticleId,TOFCluster
     + ,TOFStatus,Ntof,Plane,TOFMCEvent,TOFMCIdsoft,TrCluster,Idsoft
     + ,Status,Nelem,TrMCCluster,IdsoftMC,Itra,Left(2),Center(2)
     + ,Right(2),TrRecHit,StatusR,Layer,TrTrack,TrStatus,Pattern,NHits,
     +  phits(6),pctcpointer(2),FastFitdone
     + ,GeaneFitDone,AdvancedFitDone,EventNoMCEventG,Particle,CTCCluster
     + ,CTCStatus,CTCLayer,CTCMCEvent,CTCMCIdsoft,px,py,Particles ,
     +   Tracks , Betas , Charges  ,TrRecHits , TrClusters , 
     +   TrMCClusters , TOFClusters , TOFMCClusters , 
     +   CTCClusters , CTCMCClusters 
*

*
*
*
*
*    idet=1
*
      common /eventhC/eventno,run,runtype,time,Particles ,
     +   Tracks , Betas , Charges  ,TrRecHits , TrClusters , 
     +   TrMCClusters , TOFClusters , TOFMCClusters , 
     +   CTCClusters , CTCMCClusters 

*
*    idet=2
*
      common /betaC/betaevent,betapattern,beta,betaerror,betachi2
*
*    idet=3
*

      common /chargeC/chargeevent,chargebetap,chargetof,
     + chargetracker,probtof,probtracker

*
*    idet=4
*

      common/particleC/particleevent,pctcpointer,
     + pbetapointer,pchargepointer,
     + ptrackpointer,particleid,pmass,perrmass,pmom,perrmom,pcharge,
     + ptheta,pphi,pcoo,panti,signalctc,betactc,errorbetactc,cooctc
*
*    idet=5
*

      common /tofclustC/ TOFCluster,TOFStatus,Ntof,Plane,TOFEdep,
     + TOFTime,TOFETime,TOFCoo,TOFErCoo

*
*    idet=6
*
       common /tofmcccluC/TOFMCEvent,TOFMCIdsoft,TOFMCXcoo,TOFMCtof,
     +  TOFMCedep

*
*    idet=7
*
       common /trclusteC/TrCluster,Idsoft,Status,NelemL,NelemR,
     + Sumt,Sigmat,Meant,RMSt,ErrorMeant,Amplitude
*
*    idet=8
*
       common /trmcclusC/TrMCCluster,IdsoftMC,Itra,Left,Center,
     +  Right,ss,xca,xcb,xgl,summc
*
*    idet=9
*

      common /trrechitC/TrRecHit,px,py,
     +statusr,Layer,hitr,ehitr,sumr,difosum

*
*    idet=10
*
      common /trtrackC/trtrack,trstatus,pattern,nhits,phits,
     + FastFitDone,GeaneFitDone,
     + AdvancedFitDone,Chi2StrLine,Chi2Circle,CircleRidgidity,
     + Chi2FastFit,Ridgidity,ErrRidgidity,Theta,phi,p0,gchi2,
     + gridgidity,gerrridgidity,gtheta,gphi,gp0,hchi2,
     + HRidgidity,HErrRidgidity,htheta,hphi,hp0,fchi2ms,gchi2ms,
     + ridgidityms,gridgidityms
*
*    idet=11
*

      common /mceventgC/EventNoMCEventG,Particle,coo,dir,
     + momentum,mass,charge

*
*    idet=12
*
     
      common /ctcclustC/CTCCluster,CTCStatus,ctclayer,
     + ctccoo,ctcercoo,
     + ctcrawsignal,ctcsignal,ctcesignal

*
*    idet=13
*

      common /ctcmccluC/CTCMCEvent, CTCMCIdsoft,CTCMCXcoo,
     + CTCMCXdir,CTCstep,ctccharge,ctcbeta,ctcedep
      character *64 cdet
      parameter (ndet=13)
      character *64 cblock(ndet)
      integer events(10000),nevent
*
*--   Enter user code here
*
       call HROPEN(4,'input',fnam,' ',1024,iostat)
       if(iostat.ne.0)then
        write(*,*)'Unable to open input file ',fnam
        stop
       endif
       ic=0
       call hcdir('//input',' ')
       call hrin(1,9999,0)
       ic=0
       cblock(1)='EVENTH'
       cblock(2)='BETA'
       cblock(3)='CHARGE'
       cblock(4)='PARTICLE'
       cblock(5)='TOFCLUST'
       cblock(6)='TOFMCCLU'
       cblock(7)='TRCLUSTE'
       cblock(8)='TRMCCLUS'
       cblock(9)='TRRECHIT'
       cblock(10)='TRTRACK'
       cblock(11)='MCEVENTG'
       cblock(12)='CTCCLUST'
       cblock(13)='CTCMCCLU'
       do idet=1,ndet
        open(21,file=cblock(idet),status='unknown',
     +  form='unformatted')
        nmax=1000000000
        ierr=0
        call hbname(1,' ',0,'$CLEAR')
        if(cblock(idet).eq.'EVENTH')
     +  call hbname(1,cblock(idet),eventno,'$SET') 
        if(cblock(idet).eq.'BETA')
     +  call hbname(1,cblock(idet),betaevent,'$SET') 
        if(cblock(idet).eq.'CHARGE')
     +  call hbname(1,cblock(idet),chargeevent,'$SET') 
        if(cblock(idet).eq.'PARTICLE')
     +  call hbname(1,cblock(idet),particleevent,'$SET') 
        if(cblock(idet).eq.'TOFCLUST')
     +  call hbname(1,cblock(idet),tofcluster,'$SET') 
        if(cblock(idet).eq.'TOFMCCLU')
     +  call hbname(1,cblock(idet),tofmcevent,'$SET') 
        if(cblock(idet).eq.'TRCLUSTE')
     +  call hbname(1,cblock(idet),trcluster,'$SET') 
        if(cblock(idet).eq.'TRMCCLUS')
     +  call hbname(1,cblock(idet),trmccluster,'$SET') 
        if(cblock(idet).eq.'TRRECHIT')
     +  call hbname(1,cblock(idet),trrechit,'$SET') 
        if(cblock(idet).eq.'TRTRACK')
     +  call hbname(1,cblock(idet),trtrack,'$SET') 
        if(cblock(idet).eq.'MCEVENTG')
     +  call hbname(1,cblock(idet),EventNoMCEventG,'$SET') 
        if(cblock(idet).eq.'CTCCLUST')
     +  call hbname(1,cblock(idet),ctccluster,'$SET') 
        if(cblock(idet).eq.'CTCMCCLU')
     +  call hbname(1,cblock(idet),ctcmcevent,'$SET') 
        do i=1,nmax
         if(i.eq.1)write(*,*)cblock(idet)(1:8),' Started'
         call hgntb(1,cblock(idet),i,ierr)
         if(ierr.ne.0)then
          if( cblock(idet).eq.'EVENTH')then
            write(*,*)' Last Run&Event # ',run,eventno
          endif
          write(*,*)cblock(idet)(1:8),' finished with ',i
          goto 999
         endif
                if( cblock(idet).eq.'EVENTH')then
            write(21)eventno,run,runtype,time,Particles ,
     +   Tracks , Betas , Charges  ,TrRecHits , TrClusters , 
     +   TrMCClusters , TOFClusters , TOFMCClusters , 
     +   CTCClusters , CTCMCClusters 
           else if( cblock(idet).eq.'BETA')then
            write(21)betaevent,betapattern,beta,betaerror,betachi2
           else if( cblock(idet).eq.'CHARGE')then
            write(21)chargeevent,chargebetap,chargetof,
     +      chargetracker,probtof,probtracker
           else if( cblock(idet).eq.'PARTICLE')then
            write(21) particleevent,pctcpointer,
     + pbetapointer,pchargepointer,
     + ptrackpointer,particleid,pmass,perrmass,pmom,perrmom,pcharge,
     + ptheta,pphi,pcoo,panti,signalctc,betactc,errorbetactc,cooctc   
           else if( cblock(idet).eq.'TOFCLUST')then
            write(21) TOFCluster,TOFStatus,Ntof,Plane,TOFEdep,
     + TOFTime,TOFETime,TOFCoo,TOFErCoo
           else if( cblock(idet).eq.'TOFMCCLU')then
            write(21)TOFMCEvent,TOFMCIdsoft,TOFMCXcoo,TOFMCtof,
     +  TOFMCedep
           else if( cblock(idet).eq.'TRCLUSTE')then
            write(21)TrCluster,Idsoft,Status,NelemL,NelemR,
     +      Sumt,Sigmat,Meant,RMSt,ErrorMeant,Amplitude
           else if( cblock(idet).eq.'TRMCCLUS')then
            write(21)TrMCCluster,IdsoftMC,Itra,Left,Center,
     +  Right,ss,xca,xcb,xgl,summc
           else if( cblock(idet).eq.'TRRECHIT')then
            write(21)TrRecHit,px,py,statusr,Layer,
     +      hitr,ehitr,sumr,difosum
           else if( cblock(idet).eq.'TRTRACK')then
            write(21)trtrack,trstatus,pattern,nhits,phits,
     + FastFitdone,GeaneFitDone,
     + AdvancedFitDone,Chi2StrLine,Chi2Circle,CircleRidgidity,
     + Chi2FastFit,Ridgidity,ErrRidgidity,Theta,phi,p0,gchi2,
     + gridgidity,gerrridgidity,gtheta,gphi,gp0,hchi2,
     + HRidgidity,HErrRidgidity,htheta,hphi,hp0,fchi2ms,gchi2ms,
     + ridgidityms,gridgidityms 
           else if( cblock(idet).eq.'MCEVENTG')then
            write(21)EventNoMCEventG,Particle,coo,dir,
     + momentum,mass,charge
           else if( cblock(idet).eq.'CTCCLUST')then
            write(21)CTCCluster,CTCStatus,ctclayer,
     + ctccoo,ctcercoo,
     + ctcrawsignal,ctcsignal,ctcesignal
           else if( cblock(idet).eq.'CTCMCCLU')then
            write(21)CTCMCEvent, CTCMCIdsoft,CTCMCXcoo,
     + CTCMCXdir,CTCstep,ctccharge,ctcbeta,ctcedep
           else
            write(*,*)' Invalid cblock ',idet,' ',cblock(idet)
           endif
        enddo 
 999    continue
        close(21)
       enddo 
       call hrend('input')
       close(4)

*
      END
