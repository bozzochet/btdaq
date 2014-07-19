*//+LIP commons 
*---------------------------------------------------------------
*  constants
*---------------------------------------------------------------
      real pi,twopi,piby2,degrad,raddeg,
     &     clight,big,emass,emmu,pmass,avo

      PARAMETER (PI=3.14159265358979324)      
      PARAMETER (TWOPI=6.28318530717958648)
      PARAMETER (PIBY2=1.57079632679489662)
      PARAMETER (DEGRAD=0.0174532925199432958)
      PARAMETER (RADDEG=57.2957795130823209)
      PARAMETER (CLIGHT=29979245800.)
      PARAMETER (BIG=10000000000.)
      PARAMETER (EMASS=0.00051099906)
      PARAMETER (EMMU=0.105658389)
      PARAMETER (PMASS=0.93827231)
      PARAMETER (AVO=0.60221367)

*---------------------------------------------------------------
* INTERFACE WITH C++
*---------------------------------------------------------------

       real ztoprad_ams,zpmtdet,
     + hmir,rtmir,rbmir,emcxlim,emcylim,
     + ztmirgap,zbmirgap,                 ! dimensions
     + reflec, 
     + lg_length,lg_height,               ! LG
     + pmt_suptk,pmt_shdtk,pmt_sidel


      common /lipgeo/ztoprad_ams,       ! top of radiator (AMS frame) 
     +               zpmtdet,           ! z of PMT matrix (RICH frame)
     +               hmir,              ! mirror height (cm)
     +               rtmir,             ! mirror top radius (cm)
     +               rbmir,             !   "   bottom "    (cm)
     +               emcxlim,           ! hole lim. x    
     +               emcylim,           ! hole lim. y    
     +               ztmirgap,          ! zgap betw. mirror and rad
     +               zbmirgap,          ! zgap betw. mirror and lg
     +               reflec,            ! mirror reflectivity
     +               lg_length,         ! LG length (cm) 
     +               lg_height,         ! LG height(cm) 
     +               pmt_suptk,
     +               pmt_shdtk,
     +               pmt_sidel

       integer radtype,nabsrad
       real hrad,refindex,clarity,rrad,ltile,labsrad,     ! radiator
     + hpgl,pglix                                         ! foil

       common/liprad/ 
     +               hrad,              ! radiator thickness          
     +               refindex,          !   "      refractive index
     +               clarity,           !   "      clarity
     +               radtype,           ! 0(empty),1(agl),2(naf)
     +               rrad,              ! radiator radius
     +               ltile,             ! tile dimensions
     +               nabsrad,           ! Nelm( absorption length)
     +               labsrad(44),       ! absorption length                       
     +               hpgl,              ! foil thickness(cm)
     +               pglix              !  "   refractive index 



      integer nbhitsmax,nbhits,hitsadc,hitshid
      real hitsnpe,hitscoo

      common/lipdat/nbhitsmax,nbhits,
     +              hitsadc(16000), 
     +              hitsnpe(16000), 
     +              hitscoo(3,16000), 
     +              hitshid(16000)       


      real pimp,pmom,pthe,pphi,pcoopmt,cerang,pbeta,pchg

      common /liptrk/pimp(3),   ! particle ip in radiator (AMS frame)
     +               pmom,
     +               pthe,
     +               pphi,
     +               pcoopmt(3),
     +               cerang,
     +               pbeta,
     +               pchg 

      integer nbhits_used
      real thcrec,betarec,ebetarec,likep,chi2beta,rprob

      common/lipvar/nbhits_used,  ! number of hits used 
     +              thcrec,       ! reconstructed Ck. angle
     +              betarec,      ! reconstructed velocity 
     +              ebetarec,     ! error on "      "
     +              likep,        ! likelihood f. probability
     +              chi2beta,     ! chi2 
     +              rprob         ! kolmogorov test prob.

*--------------------------------------------------------------------------
* PMT matrix geometry and other geometry variables ( filled once per run)
*--------------------------------------------------------------------------

      real pitchx,pitchy,xlimv,xlimh,ylimv,ylimh,jump,dx0,
     +     xcpmm,ycpmm

      common/pmtmatrix/pitchx,pitchy,
     +                 xlimv,xlimh,
     +                 ylimv,ylimh,
     +                 jump,dx0,xcpmm,ycpmm

      real vtmir, tgmir
      common/miraux/vtmir(3), 
     +              tgmir    

*---------------------------------------------------------------
* rotation matrix
*---------------------------------------------------------------

       integer irotflg
       real figrotm(3,3)
       common /matrixf/ irotflg        
       common /matrixc/ figrotm  

* -------------------------------------------------------------------------
* --- REC working variables
* -------------------------------------------------------------------------

        integer ievnumb,itknumb
        common/genevent/ievnumb,itknumb

        integer nbhits_nass
        common /richauxc/ nbhits_nass   ! nb hits non-associated


       integer ihit,nmirefhit
       real curhit,hypthc

        common /cerwrkc/  ihit,       ! hit number
     +                   curhit(3),   ! coord of current hit
     +                   hypthc,      ! cerenkov angle being tested
     +                   nmirefhit    ! nb of reflections of current hit

        real pcervtx
        common /richvtxc/ pcervtx(3)  ! cerenkov vertex

        character chradid*3
        common /radiatc1/ chradid     ! radiator name           

        real cangrecup,cangreclow

        common /cerlimc/ cangrecup,   ! thetac search upper limit
     +                   cangreclow  ! thetac search lower limit

* --------------------------------------------------------------------------
* --- variables for cerenkov angle reconstruction (per event)             
* --------------------------------------------------------------------------

      integer ntherecmax,nhitmax

**      PARAMETER (NTHERECMAX=10, NHITMAX=16000)
      PARAMETER (NTHERECMAX=1, NHITMAX=16000)

      real chi2hit_fit,chi2hit_last,phihit_fit,chi2rec_fit
      integer nbushits_fit,ipushits_fit,nbminshit_fit,ireflechit_fit

      common /richfitc/ 
     +                    chi2hit_fit(nhitmax),
     +                    phihit_fit(nhitmax),
     +                    chi2rec_fit,
     +                    nbushits_fit,
     +                    ipushits_fit(nhitmax),
     +                    nbminshit_fit(nhitmax),
     +                    ireflechit_fit(nhitmax),
     +                    chi2hit_last(nhitmax)

      integer ipthetac,ntherec,nbushits, 
     &        ipushits,nbminshit,iflghit,ireflechit

      real cangrec, chi2rec, likerec
      real chi2hit,phihit,cangrecdif

      common /richbetc/ 
     +         ipthetac,                     ! pointer to best fit
     +         ntherec,                      ! nb of rec cerenkov theta
     +         nbushits(ntherecmax),         ! nb hits close to pattern
     +         ipushits(ntherecmax,nhitmax), ! pointer to used hits
     +         nbminshit(ntherecmax,nhitmax),! nb of phi minimums/hits
     +         iflghit(nhitmax),             ! hit flag 
*                                                =1 assoc to track
*                                                =2 used on 1st rec
     +         ireflechit(ntherecmax,nhitmax), ! (=0 nonref,=1 reflect)
     +         cangrec(ntherecmax),            ! cerenkov angle rec.
     +         chi2rec(ntherecmax),            ! reduced chi2 
     +         likerec(ntherecmax),            ! lorentzian event prob
     +         chi2hit(ntherecmax,nhitmax),    ! chi2 of all hits
     +         phihit(ntherecmax,nhitmax),     ! hit phi from minimum
     +         cangrecdif(ntherecmax)          ! quality of minimum


************************************************************
* RICH VELOCITY RECONSTRUCTION PARAMETERS (LIP)
************************************************************

      real phistep,chi2htcut,chi2hthint,fsigma2,fbackgr,fdistan,thcstep
      common /liprecc/ 
     +  PHISTEP,  !phi step for hit chi2 scanning  and min. finding  
     +  CHI2HTCUT,!chi2 max for a correlated hit with ph. pattern
     +  CHI2HTHINT,!chi2 max for a correlated hit with ph. pattern
     +  FSIGMA2,  !lorentz function width 
     +  FBACKGR,  !photon background fraction
     +  FDISTAN,  !effective detector distance
     +  THCSTEP   !thetac step for lorentz scan and min. finding  





