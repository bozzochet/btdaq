      subroutine prodcorr
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
      INTEGER eventno,run,runtype,time(2),RawWords,TrTracks,TrRecHits
     + ,TrClusters,TrRawClusters,TrMCClusters,TOFClusters,TOFMCClusters
     + ,AntiMCClusters,TRDMCClusters,AntiClusters,EcalClusters,EcalHits
     + ,RICMCClusters,RICHits,TRDRawHits,TRDClusters,TRDSegments
     + ,TRDTracks,EventStatus(2),nbeta,betastatus(50),betapattern(50)
     + ,betantof(50),betaptof(4,50),betaptr(50),ncharge,chargestatus(30)
     + ,chargebetap(30),chargetof(30),chargetracker(30),chintof(4,30)
     + ,chintracker(4,30),ntrdmccl,trdlayermc(200),trdladdermc(200)
     + ,trdtubemc(200),trdtrackmc(200),ntrdht,trdhtlayer(200)
     + ,trdhtladder(200),trdhttube(200),ntrdseg,trdsegstatus(100)
     + ,trdsegor(100),trdsegpat(100),trdsegnh(100),trdsegpcl(12,100)
     + ,ntrdtrk,trdtrkstatus(40),trdtrkns(40),trdtrkpat(40)
     + ,trdtrkps(5,40),ntrdcl,trdclstatus(100),trdlayer(100),trdmul(100)
     + ,trdhmul(100),ptrdrht(100),npart,pbetap(20),pchargep(20)
     + ,ptrackp(20),ptrdp(20),prichp(20),pecalp(20),pid(20),pidvice(20)
     + ,ntof,TOFStatus(20),plane(20),bar(20),nmemb(20),neccl
     + ,EcclStatus(60),EcclProj(60),EcclPlane(60),EcclLeft(60)
     + ,EcclCenter(60),EcclRight(60),ecclpleft(60),ecclnhits(60),nec2d
     + ,Ec2dProj(10)
      REAL RadS,ThetaS,PhiS,YawS,PitchS,RollS,VelocityS,VelTheta,VelPhi
     + ,ThetaM,PhiM,beta(50),betac(50),betaerror(50),betaerrorc(50)
     + ,betachi2(50),betachi2s(50),probtof(4,30),probtracker(4,30)
     + ,proballtracker(30),truntof(30),truntofd(30),truntracker(30)
     + ,trdedepmc(200),trdekinmc(200),trdxglmc(3,200),trdstepmc(200)
     + ,trdampht(200),trdsegfpar(2,100),trdsegchi2(100),trdtrkcoo(3,40)
     + ,trdtrkercoo(3,40),trdtrkphi(40),trdtrktheta(40),trdtrkchi2(40)
     + ,trdclcoo(3,100),trddir(3,100),trdedep(100),probpid(2,20)
     + ,fitmom(20),pmass(20),perrmass(20),pmom(20),perrmom(20),pbeta(20)
     + ,perrbeta(20),pcharge(20),ptheta(20),pphi(20),thetagl(20)
     + ,phigl(20),pcoo(3,20),cutoff(20),cootof(3,4,20),cooanti(3,2,20)
     + ,cooecal(3,3,20),cootr(3,8,20),cootrd(3,20),TOFEdep(20)
     + ,TOFEdepd(20),TOFTime(20),TOFETime(20),TOFCoo(3,20)
     + ,TOFErCoo(3,20),EcclEdep(60),ecclsleak(60),eccldleak(60)
     + ,EcclCoo(3,60),Ec2dEdep(10),Ec2dCoo(10),ec2dtan(10),ec2dchi2(10)
     + ,Ecshdir(3,5),ecshemdir(3,5),Ecshentry(3,5),Ecshexit(3,5)
     + ,Ecshcofg(3,5),EcshErdir(5),ecshchi2dir(5),ecshfrontedep(5)
     + ,EcshEn(5),ecsherat(3,5),Ecshener(5),ecshdifosum(5),EcshsLeak(5)
     + ,EcshrLeak(5)
      REAL EcshdLeak(5),EcshoLeak(5),ecshchi2P(5),ecshparP(4,5)
     + ,ecshchi2T(5),ecshparT(3,5),EchtEdep(500),EchtCoo(3,500)
     + ,TOFMCXcoo(3,200),TOFMCtof(200),TOFMCedep(200),Sumt(200)
     + ,Sigmat(200),Meant(200),RMSt(200),ErrorMeant(200)
     + ,Amplitude(5,200),ss(5,2,200),xca(3,200),xcb(3,200),xgl(3,200)
     + ,summc(200),hitr(3,666),ehitr(3,666),sumr(666),difosum(666)
     + ,cofgx(666),cofgy(666),LocDBAver(100),Chi2StrLine(100)
     + ,Chi2Circle(100),CircleRidgidity(100),Chi2FastFit(100)
     + ,Ridgidity(100),ErrRidgidity(100),Theta(100),phi(100),p0(3,100)
     + ,gchi2(100),gridgidity(100),gerrridgidity(100),hchi2(2,100)
     + ,HRidgidity(2,100),HErrRidgidity(2,100),htheta(2,100),hphi(2,100)
     + ,hp0(3,2,100),fchi2ms(100),pirigerr(100),ridgidityms(100)
     + ,pirigidity(100),coo(3,100),dir(3,100),momentum(100),mass(100)
     + ,charge(100),radl(150),absl(150),pos(3,150),AntiEdep(16)
     + ,AntiCoo(3,16),AntiErCoo(3,16),AntiMCXcoo(3,200),AntiMCtof(200)
     + ,AntiMCedep(200),LVL3Residual(2,2),LVL3Time(2),LVL3ELoss(2)
     + ,s2n(300),antirawsignal(32),tofrtovta(2,20),tofrtovtd(2,20)
     + ,tofrsdtm(2,20),tofreda(20),tofredd(20),tofrtm(20),tofrcoo(20)
     + ,origin(3,300)
      REAL direction(3,300),Rchtx(100),Rchty(100),rcribeta(100)
     + ,rcriebeta(100),rcrichi2(100)
      INTEGER Ec2dNmemb(10),ec2dpcl(18,10),necsh,ecshstatus(5)
     + ,ecshpcl(2,5),necht,EchtStatus(500),EchtIdsoft(500),EchtProj(500)
     + ,EchtPlane(500),EchtCell(500),ntofmc,TOFMCIdsoft(200),Ntrcl
     + ,Idsoft(200),Statust(200),NelemL(200),NelemR(200),ntrclmc
     + ,IdsoftMC(200),Itra(200),Left(2,200),Center(2,200),Right(2,200)
     + ,ntrrh,px(666),py(666),statusr(666),Layer(666),ntrtr
     + ,trstatus(100),pattern(100),address(100),nhits(100),phits(8,100)
     + ,GeaneFitDone(100),AdvancedFitDone(100),nmcg,nskip(100)
     + ,Particle(100),nmct,vname(150),nanti,AntiStatus(16)
     + ,AntiSector(16),nantimc,AntiMCIdsoft(200),nlvl3,LVL3TOFTr(2)
     + ,LVL3AntiTr(2),LVL3TrackerTr(2),LVL3NTrHits(2),LVL3NPat(2)
     + ,LVL3Pattern(2,2),nlvl1,LVL1LifeTime(2),LVL1Flag(2)
     + ,LVL1TOFPatt(4,2),LVL1TOFPatt1(4,2),LVL1AntiPatt(2)
     + ,LVL1ECALflag(2),ntrraw,rawaddress(300),rawlength(300),nantiraw
     + ,antirawstatus(32),antirawsector(32),antirawupdown(32),ntofraw
     + ,tofrstatus(20),tofrplane(20),tofrbar(20),nsignals,sid(300)
     + ,RICstatus(300),nphgen,phit(300),Rhits,Rchtchannel(100)
     + ,Rchtadc(100),nrings,rcritrackn(100),rcrihused(100)
*
      COMMON /PAWCR4/ eventno,run,runtype,time,RawWords,RadS,ThetaS,PhiS
     + ,YawS,PitchS,RollS,VelocityS,VelTheta,VelPhi,ThetaM,PhiM,TrTracks
     + ,TrRecHits,TrClusters,TrRawClusters,TrMCClusters,TOFClusters
     + ,TOFMCClusters,AntiMCClusters,TRDMCClusters,AntiClusters
     + ,EcalClusters,EcalHits,RICMCClusters,RICHits,TRDRawHits
     + ,TRDClusters,TRDSegments,TRDTracks,EventStatus,nbeta,betastatus
     + ,betapattern,beta,betac,betaerror,betaerrorc,betachi2,betachi2s
     + ,betantof,betaptof,betaptr,ncharge,chargestatus,chargebetap
     + ,chargetof,chargetracker,probtof,chintof,probtracker,chintracker
     + ,proballtracker,truntof,truntofd,truntracker,ntrdmccl,trdlayermc
     + ,trdladdermc,trdtubemc,trdtrackmc,trdedepmc,trdekinmc,trdxglmc
     + ,trdstepmc,ntrdht,trdhtlayer,trdhtladder,trdhttube,trdampht
     + ,ntrdseg,trdsegstatus,trdsegor,trdsegfpar,trdsegchi2,trdsegpat
     + ,trdsegnh,trdsegpcl,ntrdtrk,trdtrkstatus,trdtrkcoo,trdtrkercoo
     + ,trdtrkphi,trdtrktheta,trdtrkchi2,trdtrkns,trdtrkpat,trdtrkps
     + ,ntrdcl,trdclstatus,trdclcoo,trdlayer,trddir,trdmul,trdhmul
     + ,trdedep,ptrdrht,npart,pbetap,pchargep,ptrackp,ptrdp,prichp
     + ,pecalp,pid,pidvice,probpid,fitmom,pmass,perrmass,pmom,perrmom
     + ,pbeta,perrbeta,pcharge,ptheta,pphi,thetagl,phigl,pcoo,cutoff
     + ,cootof,cooanti,cooecal,cootr,cootrd,ntof,TOFStatus,plane,bar
     + ,nmemb,TOFEdep,TOFEdepd,TOFTime,TOFETime,TOFCoo,TOFErCoo,neccl
     + ,EcclStatus,EcclProj,EcclPlane,EcclLeft,EcclCenter,EcclRight
     + ,EcclEdep,ecclsleak,eccldleak,EcclCoo,ecclpleft,ecclnhits,nec2d
     + ,Ec2dProj,Ec2dNmemb,Ec2dEdep,Ec2dCoo,ec2dtan,ec2dchi2,ec2dpcl
     + ,necsh,ecshstatus,Ecshdir,ecshemdir,Ecshentry,Ecshexit,Ecshcofg
     + ,EcshErdir,ecshchi2dir,ecshfrontedep,EcshEn,ecsherat,Ecshener
     + ,ecshdifosum,EcshsLeak,EcshrLeak,EcshdLeak,EcshoLeak,ecshchi2P
     + ,ecshparP,ecshchi2T,ecshparT,ecshpcl,necht,EchtStatus,EchtIdsoft
     + ,EchtProj,EchtPlane,EchtCell,EchtEdep,EchtCoo,ntofmc,TOFMCIdsoft
     + ,TOFMCXcoo,TOFMCtof,TOFMCedep,Ntrcl,Idsoft,Statust,NelemL,NelemR
     + ,Sumt,Sigmat,Meant,RMSt,ErrorMeant,Amplitude,ntrclmc,IdsoftMC
     + ,Itra,Left,Center,Right,ss,xca,xcb,xgl,summc,ntrrh,px,py,statusr
     + ,Layer,hitr,ehitr,sumr,difosum,cofgx,cofgy,ntrtr,trstatus,pattern
     + ,address,nhits,phits,LocDBAver,GeaneFitDone,AdvancedFitDone
     + ,Chi2StrLine,Chi2Circle,CircleRidgidity,Chi2FastFit,Ridgidity
     + ,ErrRidgidity,Theta,phi,p0,gchi2,gridgidity,gerrridgidity,hchi2
     + ,HRidgidity,HErrRidgidity,htheta,hphi,hp0,fchi2ms,pirigerr
     + ,ridgidityms,pirigidity,nmcg,nskip,Particle,coo,dir,momentum,mass
     + ,charge,nmct,radl,absl,pos,vname,nanti,AntiStatus,AntiSector
     + ,AntiEdep,AntiCoo,AntiErCoo,nantimc,AntiMCIdsoft,AntiMCXcoo
     + ,AntiMCtof,AntiMCedep,nlvl3,LVL3TOFTr,LVL3AntiTr,LVL3TrackerTr
     + ,LVL3NTrHits,LVL3NPat,LVL3Pattern,LVL3Residual,LVL3Time,LVL3ELoss
     + ,nlvl1,LVL1LifeTime,LVL1Flag,LVL1TOFPatt,LVL1TOFPatt1
     + ,LVL1AntiPatt,LVL1ECALflag,ntrraw,rawaddress,rawlength,s2n
     + ,nantiraw,antirawstatus,antirawsector,antirawupdown,antirawsignal
     + ,ntofraw,tofrstatus,tofrplane,tofrbar,tofrtovta,tofrtovtd
     + ,tofrsdtm,tofreda,tofredd,tofrtm,tofrcoo,nsignals,sid,origin
     + ,direction,RICstatus,nphgen,phit,Rhits,Rchtchannel,Rchtadc,Rchtx
     + ,Rchty,nrings,rcritrackn,rcrihused,rcribeta,rcriebeta,rcrichi2
      real brtime(12,4)
      real bramp(12,4),brlco(12,4),brtco,brzco
      real scamp(16),sclco(16)
      real xtofcoo(4),xtofecoo(4),xtofchi2(4)
      integer usedtofcoo(20)
      integer itofcoo(4)
      iver=rawwords/2**20
      if(iver.le.995)then
      do k=1,necsh
       xen=ecshen(k)
       if(xen.lt.2)xen=2
       xc=1.0067-6.6e-2/xen**0.5
        r1=(ecshen(k)-ecshrleak(k)*ecshen(k))
        r2=ecshrleak(k)*ecshen(k)/r1
        if(r2.gt.0.035)then
         r=r1/(1-r2*0.8)/momentum(1)
        else
         r=r1*(1+r2*0.8*r2/0.035)/momentum(1)
        endif
         rxc=r/xc
         if(rxc*momentum(1).gt.2500)then
          rxc=rxc*(1-0.165)/
     +    (1-0.165*rxc*momentum(1)/2500.)
         endif
         ecshen(k)=rxc*momentum(1)
      enddo 
      endif
      if(iver.le.996)then
c <--------- TOF
      if(nlvl1.eq.0.or.lvl1flag(1).gt.0)goto 999
      ntof=0
      nanti=0
      do il=1,4
      do ib=1,12
        brtime(ib,il)=100000.
        bramp(ib,il)=0.
	brlco(ib,il)=0.
      enddo
      enddo
      do mch=1,ntofmc
        id=TOFMCIdsoft(mch)
	il=id/100
	ib=mod(id,100)
	de=1000.*TOFMCedep(mch) ! in mev
	x=TOFMCXcoo(1,mch)
	y=TOFMCXcoo(2,mch)
        if(tofmctof(mch).lt.brtime(ib,il))brtime(ib,il)=tofmctof(mch)
	bramp(ib,il)=bramp(ib,il)+de
	if(il.eq.1.or.il.eq.4)then ! along X
	  brlco(ib,il)=brlco(ib,il)+x*de
	else                       ! along Y
	  brlco(ib,il)=brlco(ib,il)+y*de
	endif
      enddo
c
      trleft=-11*(12.-0.5)/2. ! transv.coo of the 1st counter center
      trstep=12.-0.5          ! transv. dist. of neigb. counters 
      do il=1,4
      do ib=1,12
        brtco=trleft+(ib-1)*trstep ! counter center transv.coo
        if(bramp(ib,il).gt.0.1)then
	  brlco(ib,il)=brlco(ib,il)/bramp(ib,il)
	  call rannor(r,rdummy)
	  brlco(ib,il)=brlco(ib,il)+2.6*r  ! use accur.in long.coo 2.6cm
	  call rannor(r,rdummy)
          bramp(ib,il)=bramp(ib,il)+0.1*bramp(ib,il)*r ! use de/dx accur. 10%
	  if(bramp(ib,il).gt.0.2)then  ! Bar_energy > thresh. -> write it
	    ntof=ntof+1
            if(ntof.gt.20)ntof=20
	    plane(ntof)=il
	    bar(ntof)=ib
	    nmemb(ntof)=1
	    TOFEdep(ntof)=bramp(ib,il)
            toftime(ntof)=brtime(ib,il)+rdummy*0.15e-9 
	    if(il.eq.1.or.il.eq.4)then ! along X
	      TOFCoo(1,ntof)=brlco(ib,il)
	      TOFCoo(2,ntof)=brtco
	      TOFErCoo(1,ntof)=2.6
	      TOFErCoo(2,ntof)=sqrt(12.)
	      if(il.eq.1)then
	        brzco= 66.4-0.6-1.45*(1-mod(ib,2))
	      else
	        brzco=-66.4+0.6+1.45*(1-mod(ib,2))
	      endif
	    else                       ! along Y
	      TOFCoo(1,ntof)=brtco
	      TOFCoo(2,ntof)=brlco(ib,il)
	      TOFErCoo(1,ntof)=sqrt(12.)
	      TOFErCoo(2,ntof)=2.6
	      if(il.eq.2)then
	        brzco= 66.4-0.6-2.7-1.45*(1-mod(ib,2))
	      else
	        brzco=-66.4+0.6+2.7+1.45*(1-mod(ib,2))
	      endif
	    endif
	    TOFCoo(3,ntof)=brzco
	    TOFETime(ntof)=1.e-8
	  endif ! <--- end of check on bar min.energy
        endif
      enddo
      enddo
c <--------- ANTI
      if(nantimc.eq.0)goto 999
       do is=1,16
        scamp(is)=0.
        sclco(is)=0.
      enddo
      do mch=1,nantimc
        de=1000.*AntiMCedep(mch)
	is=AntiMCIdsoft(mch)
	z=AntiMCXcoo(3,mch)
	scamp(is)=scamp(is)+de
	sclco(is)=sclco(is)+de*z
      enddo
c
      dphi=3.141593*360./16./180. ! sector phi-size(rad)
      ra=54.4+0.5 ! sector radial pos.
      do is=1,16
        if(scamp(is).gt.0.)then
	  sclco(is)=sclco(is)/scamp(is)
	  call rannor(r,rdummy)
	  sclco(is)=sclco(is)+20.*r  ! use accur.in long.coo 20cm
	  if(sclco(is).lt.-43.)sclco(is)=-43.
	  if(sclco(is).gt.43.)sclco(is)=43.
	  call rannor(r,rdummy)
          scamp(is)=scamp(is)+0.2*scamp(is)*r ! use de/dx accur. 20%
	  if(scamp(is).gt.0.3)then  ! sector_energy > thresh. -> write it
	    nanti=nanti+1
            if(nanti.gt.16)nanti=16
	    AntiSector(nanti)=is
	    AntiEdep(nanti)=scamp(is)
	    anphi=0.5*dphi+(is-1)*dphi ! sector phi-position
	    AntiCoo(3,nanti)=sclco(is)
	    AntiCoo(1,nanti)=ra*cos(anphi)
	    AntiCoo(2,nanti)=ra*sin(anphi)
	    AntiErCoo(3,nanti)=20.
	    AntiErCoo(1,nanti)=ra*dphi*sin(anphi)/sqrt(12.)
	    AntiErCoo(2,nanti)=ra*dphi*cos(anphi)/sqrt(12.)
	  endif ! <--- end of check on bar min.energy
	endif
      enddo
c 
      endif
 999  continue
      if(iver.lt.999)then
*
*     Rebuild Beta
*       
      if(nlvl1.eq.0.or.lvl1flag(1).eq.0)return
       if(ntof.gt.5.and.necsh.gt.0.and.npart.gt.0.and.
     +  ptrackp(1).gt.0)then
        do i=1,20
         usedtofcoo(i)=0
        enddo
 20     continue
        do i=1,4
         xtofcoo(i)=100000000
         itofcoo(i)=0
        enddo
        do i=1,ntof
         if(usedtofcoo(i).eq.0)then
         ip=plane(i)
         xmax=3
         xp1=abs(cootof(1,ip,1)-tofcoo(1,i))/tofercoo(1,i)
         xp2=abs(cootof(2,ip,1)-tofcoo(2,i))/tofercoo(2,i)
         xp3=abs(cootof(3,ip,1)-tofcoo(3,i))/tofercoo(3,i)
c         write(*,*)xp1,xp2,xp3
         if(xp1.lt.xmax.and.xp2.lt.xmax.and.xp3.lt.xmax)then
         xp=sqrt(xp1**2+xp2**2+xp3**2)
         if(xp.lt.xtofcoo(ip))then
          xtofcoo(ip)=xp
          itofcoo(ip)=i
         endif
         endif
         endif
        enddo
        ifound=0
        do i=1,4
         if(itofcoo(i).gt.0)ifound=ifound+1
        enddo
c        write(*,*)'ifound ',ifound
        if(ifound.lt.3)then
         return
        else
         xx=0
         yy=0
         xy=0
         x2=0
         y2=0
         iff=0
         do i=1,4
          if(itofcoo(i).gt.0)then
           ip=itofcoo(i)
           if(iff.eq.0)then
            iff=ip
           else
            dl=(tofcoo(3,ip)-tofcoo(3,iff))/abs(cos(ptheta(1)))
            xx=xx+dl/tofetime(ip)/tofetime(ip)
            x2=x2+(dl/tofetime(ip))**2
            xy=xy+toftime(ip)*dl/tofetime(ip)/tofetime(ip)
          endif
           yy=yy+toftime(ip)/tofetime(ip)/tofetime(ip)
           y2=y2+1/tofetime(ip)/tofetime(ip)
          endif
         enddo                 
         xx=xx/y2
         yy=yy/y2
         x2=x2/y2
         xy=xy/y2
         a=(xy-xx*yy)/(x2-xx*xx)
         b=yy-a*xx
         xchi2=0
         iff=0
         do i=1,4
          if(itofcoo(i).gt.0)then
           ip=itofcoo(i)
           if(iff.eq.0)then
            iff=ip
            dl=0
           else
            dl=(tofcoo(3,ip)-tofcoo(3,iff))/abs(cos(ptheta(1)))
           endif
           xchi2=xchi2+(a*dl+b-toftime(ip))**2/
     +                 (tofetime(ip))**2
          endif
         enddo
         xchi2=xchi2/(ifound-2)
         if(xchi2.lt.15)then
           ipb=pbetap(1)
           beta(ipb)=-1/a/2.99792e10
           betaerror(ipb)=sqrt(1/y2)/sqrt(x2-xx*xx)*2.99792e10
           betachi2(ipb)=xchi2
           betachi2s(ipb)=0
           imiss=0
           ikk=0
         do i=1,4
          if(itofcoo(i).gt.0)then
           betachi2s(ipb)=betachi2s(ipb)+xtofcoo(i)
           ip=itofcoo(i)
           ikk=ikk+1
           betaptof(ikk,ipb)=ip
           usedtofcoo(ip)=1
          else
           imiss=i
          endif
         enddo
          betapattern(ipb)=imiss 
          betantof(ipb)=ifound
c         write(*,*)'beta found ',beta(ipb),betaerror(ipb),
c     +   betantof(ipb),betapattern(ipb),betachi2(ipb),betachi2s(ipb)

*
* Update beta also
*                 

              pbetaold=pbeta(1)
         if(prichp(1).gt.0)then
            xbeta=1/abs(beta(pbetap(1)))
            xebeta=betaerror(pbetap(1))
            xebeta=xebeta**2
            b1=1/rcribeta(prichp(1))
            b2=b1**2*rcriebeta(prichp(1))
            b2=b2*b2
            if(abs(b1-xbeta).lt.3*sqrt(b2+xebeta) .or.
     +       xbeta.lt.1.05)then
              xbeta=(xbeta/xebeta+b1/b2)/(1/xebeta+1/b2)
              xebeta=1/(1/xebeta+1/b2)
              pbeta(1)=xbeta
              if(beta(pbetap(1)).lt.0)pbeta(1)=-xbeta
              perrbeta(1)=sqrt(xebeta)/xbeta/xbeta
            else
             write(*,*)'tof /rich disagree ',b1,xbeta
             pbeta(1)=beta(pbetap(1))
             perrbeta(1)=pbeta(1)**2*betaerror(pbetap(1))
            endif
         else
          pbeta(1)=beta(pbetap(1))
          perrbeta(1)=pbeta(1)**2*betaerror(pbetap(1))
         endif
              if(pbetaold*pbeta(1).lt.0)pmom(1)=-pmom(1)
*
* Update mass 
*
          pbb=0
          sg=1
          if(abs(pbeta(1)).gt.1)then
            pbb=pbeta(1)
            sg=-1
          else if(abs(pbeta(1)).gt.0)then
            pbb=1/pbeta(1)
          endif
          pmass(1)=sg*abs(pmom(1))*sqrt(pbb**2-1)
          
         else
         write(*,*)' bad beta ',xchi2,ifound
         xchi2max=0
         ichi2max=0
         do i=1,4
          if(itofcoo(i).gt.0)then
           ip=itofcoo(i)
           if(iff.eq.0)then
            iff=ip
            dl=0
           else
            dl=(tofcoo(3,ip)-tofcoo(3,iff))/abs(cos(ptheta(1)))
           endif
           xchi2=(a*dl+b-toftime(ip))**2/
     +                 (tofetime(ip))**2
           if(xchi2max.lt.xchi2)then
             xchi2max=xchi2
             ichi2max=ip
            endif
          endif
         enddo
         usedtofcoo(ichi2max)=1
 
         endif
         endif
        goto 20
       endif
      else if(iver.lt.1000)then
         if(prichp(1).gt.0)then
            xbeta=1/abs(beta(pbetap(1)))
            xebeta=betaerror(pbetap(1))
            xebeta=xebeta**2
            b1=1/rcribeta(prichp(1))
            b2=b1**2*rcriebeta(prichp(1))
            b2=b2*b2
            if(abs(b1-xbeta).lt.4*sqrt(b2+xebeta) .or.
     +       xbeta.lt.1.05)then
              xbeta=(xbeta/xebeta+b1/b2)/(1/xebeta+1/b2)
              xebeta=1/(1/xebeta+1/b2)
              pbeta(1)=xbeta
              if(beta(pbetap(1)).lt.0)pbeta(1)=-xbeta
              perrbeta(1)=sqrt(xebeta)/xbeta/xbeta
            else
             write(*,*)'tof /rich disagree ',b1,xbeta
             pbeta(1)=beta(pbetap(1))
             perrbeta(1)=pbeta(1)**2*betaerror(pbetap(1))
            endif
         else
          pbeta(1)=beta(pbetap(1))
          perrbeta(1)=pbeta(1)**2*betaerror(pbetap(1))
         endif
*  rebuiold mass
          pbb=0
          sg=1
          if(abs(pbeta(1)).gt.1)then
            pbb=pbeta(1)
            sg=-1
          else if(abs(pbeta(1)).gt.0)then
            pbb=1/pbeta(1)
          endif
          pmass(1)=sg*abs(pmom(1))*sqrt(pbb**2-1)
      endif
      return
      end
