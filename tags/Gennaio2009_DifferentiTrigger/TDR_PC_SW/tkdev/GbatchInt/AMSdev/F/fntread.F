      INTEGER FUNCTION ifntread(fname,nev,iver)
c---------------------------------------------------------
c
      parameter (nwpawc=10000000)
      common/PAWC/ h(nwpawc)
c
      INTEGER eventno,run,runtype,time(2),RawWords,TrTracks,TrRecHits
     + ,TrClusters,TrRawClusters,TrMCClusters,TOFClusters,TOFMCClusters
     + ,AntiMCClusters,TRDMCClusters,AntiClusters,EcalClusters,EcalHits
     + ,RICMCClusters,RICHits,TRDRawHits,TRDClusters,TRDSegments
     + ,TRDTracks,EventStatus(2)
      REAL RadS,ThetaS,PhiS,YawS,PitchS,RollS,VelocityS,VelTheta,VelPhi
     + ,ThetaM,PhiM
*
      COMMON /EVENTH/eventno,run,runtype,time,RawWords,RadS,ThetaS,PhiS
     + ,YawS,PitchS,RollS,VelocityS,VelTheta,VelPhi,ThetaM,PhiM,TrTracks
     + ,TrRecHits,TrClusters,TrRawClusters,TrMCClusters,TOFClusters
     + ,TOFMCClusters,AntiMCClusters,TRDMCClusters,AntiClusters
     + ,EcalClusters,EcalHits,RICMCClusters,RICHits,TRDRawHits
     + ,TRDClusters,TRDSegments,TRDTracks,EventStatus
*
c
      character*(*) fname
      integer nev,nbadev,first,lrec
c
      data first/0/
      data lrec/8000/
c
      if(first.eq.0)then
        call hlimit(nwpawc)
	first=1
      endif
c
      nbadev=0
      nlong=0
      call hcdir('//PAWC',' ')
      call Hropen(20,'INPNTP',fname,'PQ',lrec,ista)
      if(ista.ne.0)then
        if(iver.gt.0)then
          write(*,*)'problem to open file ',fname
          write(*,*)'ista=',ista
        endif
	ifntread=-1
	return
      endif
      call hcdir('//INPNTP',' ')
      call Hrin(1,999999,0)
      call Hbname(1,' ',0,'$CLEAR')
      call Hbname(1,'EVENTH',eventno,'$SET')
      call Hnoent(1,nevents)
      if(iver.gt.0)write(*,*)'Events in Ntuple: ',nevents
      if(nevents.le.0)then
        ifntread=-2 
        goto 10
      endif
c---
      do i=1,nevents
        if(i.eq.1)then
	  call Hgnt(1,i,ierr)
	else
	  call Hgntf(1,i,ierr)
	endif
	if(ierr.ne.0)then
	  if(iver.gt.0)write(*,*)'Error reading event ',i
	  ifntread=-2 
	  goto 10
	endif
c -----------> event tests:
        if(mod(EventStatus(1)/1073741824,2).ne.0)nbadev=nbadev+1 ! "errored" events
ccc        write(*,*)'eventn/run=',eventno,run
      enddo
c---
c -----------> final tests:
c---
      if(nev.gt.nevents)then
        ifntread=-3
	goto 10
      elseif(nev.lt.nevents)then
        ifntread=-4
        goto 10
      else
        rrr=100*float(nbadev)/float(nevents)
ccc        write(*,*)'BadStatBitEvs:',rrr
        i=rrr
        ifntread=i ! OK
      endif
c
 10   continue
      call Hrend('INPNTP')
      close(20)
c
      return
      end
