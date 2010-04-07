// 
// File Is Auto Generated 
// All Manual Changes Will Be Lost 
// 
   unsigned int   ecalhit_status(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Status:0;} 
   int   ecalhit_idsoft(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Idsoft:0;} 
   int   ecalhit_proj(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Proj:0;} 
   int   ecalhit_plane(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Plane:0;} 
   int   ecalhit_cell(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Cell:0;} 
   float ecalhit_edep(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Edep:0;} 
   float ecalhit_edcorr(unsigned int i){return pEcalHit(i)?pEcalHit(i)->EdCorr:0;} 
   float ecalhit_attcor(unsigned int i){return pEcalHit(i)?pEcalHit(i)->AttCor:0;} 
   float ecalhit_coo(unsigned int j, unsigned int i){return (j<3 && pEcalHit(i))?pEcalHit(i)->Coo[j]:0;} 
   float ecalhit_adc(unsigned int j, unsigned int i){return (j<3 && pEcalHit(i))?pEcalHit(i)->ADC[j]:0;} 
   float ecalhit_ped(unsigned int j, unsigned int i){return (j<3 && pEcalHit(i))?pEcalHit(i)->Ped[j]:0;} 
   float ecalhit_gain(unsigned int i){return pEcalHit(i)?pEcalHit(i)->Gain:0;} 
   unsigned int ecalcluster_status(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Status:0;} 
   int ecalcluster_proj(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Proj:0;} 
   int ecalcluster_plane(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Plane:0;} 
   int ecalcluster_left(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Left:0;} 
   int ecalcluster_center(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Center:0;} 
   int ecalcluster_right(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Right:0;} 
   float ecalcluster_edep(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->Edep:0;} 
   float ecalcluster_sideleak(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->SideLeak:0;} 
   float ecalcluster_deadleak(unsigned int i){return pEcalCluster(i)?pEcalCluster(i)->DeadLeak:0;} 
   float ecalcluster_coo(unsigned int j, unsigned int i){return (j<3 && pEcalCluster(i))?pEcalCluster(i)->Coo[j]:0;} 
int ecalcluster_fecalhit(unsigned int j, unsigned int i){return ( pEcalCluster(i) && j<pEcalCluster(i)->fEcalHit.size() )?pEcalCluster(i)->fEcalHit[j]:0;} 
int ecalcluster_fecalhit_size(unsigned int i){return ( pEcalCluster(i)  )?pEcalCluster(i)->fEcalHit.size():0;} 
   unsigned int ecal2dcluster_status(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Status:0;} 
   int ecal2dcluster_proj(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Proj:0;} 
   float ecal2dcluster_edep(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Edep:0;} 
   float ecal2dcluster_coo(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Coo:0;} 
   float ecal2dcluster_tan(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Tan:0;} 
   float ecal2dcluster_chi2(unsigned int i){return pEcal2DCluster(i)?pEcal2DCluster(i)->Chi2:0;} 
int ecal2dcluster_fecalcluster(unsigned int j, unsigned int i){return ( pEcal2DCluster(i) && j<pEcal2DCluster(i)->fEcalCluster.size() )?pEcal2DCluster(i)->fEcalCluster[j]:0;} 
int ecal2dcluster_fecalcluster_size(unsigned int i){return ( pEcal2DCluster(i)  )?pEcal2DCluster(i)->fEcalCluster.size():0;} 
   unsigned int   ecalshower_status(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Status:0;} 
   float ecalshower_dir(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Dir[j]:0;} 
   float ecalshower_emdir(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->EMDir[j]:0;} 
   float ecalshower_entry(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Entry[j]:0;} 
   float ecalshower_exit(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Exit[j]:0;} 
   float ecalshower_cofg(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->CofG[j]:0;} 
   float ecalshower_erdir(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErDir:0;} 
   float ecalshower_chi2dir(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Dir:0;} 
   float ecalshower_firstlayeredep(unsigned int i){return pEcalShower(i)?pEcalShower(i)->FirstLayerEdep:0;} 
   float ecalshower_energyc(unsigned int i){return pEcalShower(i)?pEcalShower(i)->EnergyC:0;} 
   float ecalshower_energy3c(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->Energy3C[j]:0;} 
   float ecalshower_erenergyc(unsigned int i){return pEcalShower(i)?pEcalShower(i)->ErEnergyC:0;} 
   float ecalshower_difosum(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DifoSum:0;} 
   float ecalshower_sideleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->SideLeak:0;} 
   float ecalshower_rearleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->RearLeak:0;} 
   float ecalshower_deadleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->DeadLeak:0;} 
   float ecalshower_attleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->AttLeak:0;} 
   float ecalshower_nlinleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->NLinLeak:0;} 
   float ecalshower_orpleak(unsigned int i){return pEcalShower(i)?pEcalShower(i)->OrpLeak:0;} 
   float ecalshower_orp2denergy(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Orp2DEnergy:0;} 
   float ecalshower_chi2profile(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Profile:0;} 
   float ecalshower_parprofile(unsigned int j, unsigned int i){return (j<4 && pEcalShower(i))?pEcalShower(i)->ParProfile[j]:0;} 
   float ecalshower_chi2trans(unsigned int i){return pEcalShower(i)?pEcalShower(i)->Chi2Trans:0;} 
   float ecalshower_sphericityev(unsigned int j, unsigned int i){return (j<3 && pEcalShower(i))?pEcalShower(i)->SphericityEV[j]:0;} 
int ecalshower_fecal2dcluster(unsigned int j, unsigned int i){return ( pEcalShower(i) && j<pEcalShower(i)->fEcal2DCluster.size() )?pEcalShower(i)->fEcal2DCluster[j]:0;} 
int ecalshower_fecal2dcluster_size(unsigned int i){return ( pEcalShower(i)  )?pEcalShower(i)->fEcal2DCluster.size():0;} 
   unsigned int   tofrawcluster_status(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Status:0;} 
   int   tofrawcluster_layer(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Layer:0;} 
   int   tofrawcluster_bar(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->Bar:0;} 
   float tofrawcluster_adca(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->adca[j]:0;} 
   float tofrawcluster_adcd(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->adcd[j]:0;} 
   float tofrawcluster_adcdr(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pTofRawCluster(i))?pTofRawCluster(i)->adcdr[j][k]:0;} 
   float tofrawcluster_sdtm(unsigned int j, unsigned int i){return (j<2 && pTofRawCluster(i))?pTofRawCluster(i)->sdtm[j]:0;} 
   float tofrawcluster_edepa(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->edepa:0;} 
   float tofrawcluster_edepd(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->edepd:0;} 
   float tofrawcluster_time(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->time:0;} 
   float tofrawcluster_cool(unsigned int i){return pTofRawCluster(i)?pTofRawCluster(i)->cool:0;} 
   int tofrawside_swid(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->swid:0;} 
   int tofrawside_hwid(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->hwid:0;} 
   int tofrawside_stat(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->stat:0;} 
   int tofrawside_nftdc(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nftdc:0;} 
   int tofrawside_ftdc(unsigned int j, unsigned int i){return (j<8 && pTofRawSide(i))?pTofRawSide(i)->ftdc[j]:0;} 
   int tofrawside_nstdc(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nstdc:0;} 
   int tofrawside_stdc(unsigned int j, unsigned int i){return (j<16 && pTofRawSide(i))?pTofRawSide(i)->stdc[j]:0;} 
   int tofrawside_nsumh(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nsumh:0;} 
   int tofrawside_sumht(unsigned int j, unsigned int i){return (j<16 && pTofRawSide(i))?pTofRawSide(i)->sumht[j]:0;} 
   int tofrawside_nsumsh(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nsumsh:0;} 
   int tofrawside_sumsht(unsigned int j, unsigned int i){return (j<16 && pTofRawSide(i))?pTofRawSide(i)->sumsht[j]:0;} 
   float tofrawside_adca(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->adca:0;} 
   int tofrawside_nadcd(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->nadcd:0;} 
   float tofrawside_adcd(unsigned int j, unsigned int i){return (j<3 && pTofRawSide(i))?pTofRawSide(i)->adcd[j]:0;} 
   float tofrawside_temp(unsigned int i){return pTofRawSide(i)?pTofRawSide(i)->temp:0;} 
   unsigned int tofcluster_status(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Status:0;} 
   int tofcluster_layer(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Layer:0;} 
   int tofcluster_bar(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Bar:0;} 
   float tofcluster_edep(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Edep:0;} 
   float tofcluster_edepd(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Edepd:0;} 
   float tofcluster_time(unsigned int i){return pTofCluster(i)?pTofCluster(i)->Time:0;} 
   float tofcluster_errtime(unsigned int i){return pTofCluster(i)?pTofCluster(i)->ErrTime:0;} 
   float tofcluster_coo(unsigned int j, unsigned int i){return (j<3 && pTofCluster(i))?pTofCluster(i)->Coo[j]:0;} 
   float tofcluster_errorcoo(unsigned int j, unsigned int i){return (j<3 && pTofCluster(i))?pTofCluster(i)->ErrorCoo[j]:0;} 
int tofcluster_ftofrawcluster(unsigned int j, unsigned int i){return ( pTofCluster(i) && j<pTofCluster(i)->fTofRawCluster.size() )?pTofCluster(i)->fTofRawCluster[j]:0;} 
int tofcluster_ftofrawcluster_size(unsigned int i){return ( pTofCluster(i)  )?pTofCluster(i)->fTofRawCluster.size():0;} 
   unsigned int   anticluster_status(unsigned int i){return pAntiCluster(i)?pAntiCluster(i)->Status:0;} 
   int   anticluster_sector(unsigned int i){return pAntiCluster(i)?pAntiCluster(i)->Sector:0;} 
   int   anticluster_ntimes(unsigned int i){return pAntiCluster(i)?pAntiCluster(i)->Ntimes:0;} 
   int   anticluster_npairs(unsigned int i){return pAntiCluster(i)?pAntiCluster(i)->Npairs:0;} 
   float anticluster_times(unsigned int j, unsigned int i){return (j<16 && pAntiCluster(i))?pAntiCluster(i)->Times[j]:0;} 
   float anticluster_timese(unsigned int j, unsigned int i){return (j<16 && pAntiCluster(i))?pAntiCluster(i)->Timese[j]:0;} 
   float anticluster_edep(unsigned int i){return pAntiCluster(i)?pAntiCluster(i)->Edep:0;} 
   float anticluster_coo(unsigned int j, unsigned int i){return (j<3 && pAntiCluster(i))?pAntiCluster(i)->Coo[j]:0;} 
   float anticluster_errorcoo(unsigned int j, unsigned int i){return (j<3 && pAntiCluster(i))?pAntiCluster(i)->ErrorCoo[j]:0;} 
   int trrawcluster_address(unsigned int i){return pTrRawCluster(i)?pTrRawCluster(i)->address:0;} 
   int trrawcluster_nelem(unsigned int i){return pTrRawCluster(i)?pTrRawCluster(i)->nelem:0;} 
   float trrawcluster_s2n(unsigned int i){return pTrRawCluster(i)?pTrRawCluster(i)->s2n:0;} 
   int trcluster_idsoft(unsigned int i){return pTrCluster(i)?pTrCluster(i)->Idsoft:0;} 
   unsigned int trcluster_status(unsigned int i){return pTrCluster(i)?pTrCluster(i)->Status:0;} 
   int trcluster_neleml(unsigned int i){return pTrCluster(i)?pTrCluster(i)->NelemL:0;} 
   int trcluster_nelemr(unsigned int i){return pTrCluster(i)?pTrCluster(i)->NelemR:0;} 
   float trcluster_sum(unsigned int i){return pTrCluster(i)?pTrCluster(i)->Sum:0;} 
   float trcluster_sigma(unsigned int i){return pTrCluster(i)?pTrCluster(i)->Sigma:0;} 
   float trcluster_mean(unsigned int i){return pTrCluster(i)?pTrCluster(i)->Mean:0;} 
   float trcluster_rms(unsigned int i){return pTrCluster(i)?pTrCluster(i)->RMS:0;} 
   float trcluster_errormean(unsigned int i){return pTrCluster(i)?pTrCluster(i)->ErrorMean:0;} 
float trcluster_amplitude(unsigned int j, unsigned int i){return ( pTrCluster(i) && j<pTrCluster(i)->Amplitude.size() )?pTrCluster(i)->Amplitude[j]:0;} 
float trcluster_amplitude_size(unsigned int i){return ( pTrCluster(i)  )?pTrCluster(i)->Amplitude.size():0;} 
   unsigned int   trrechit_status(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->Status:0;} 
   int   trrechit_layer(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->Layer:0;} 
   float trrechit_hit(unsigned int j, unsigned int i){return (j<3 && pTrRecHit(i))?pTrRecHit(i)->Hit[j]:0;} 
   float trrechit_ehit(unsigned int j, unsigned int i){return (j<3 && pTrRecHit(i))?pTrRecHit(i)->EHit[j]:0;} 
   float trrechit_sum(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->Sum:0;} 
   float trrechit_difosum(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->DifoSum:0;} 
   float trrechit_cofgx(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->CofgX:0;} 
   float trrechit_cofgy(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->CofgY:0;} 
   float trrechit_bfield(unsigned int j, unsigned int i){return (j<3 && pTrRecHit(i))?pTrRecHit(i)->Bfield[j]:0;} 
   int  trrechit_ftrclusterx(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->fTrClusterX:0;} 
   int  trrechit_ftrclustery(unsigned int i){return pTrRecHit(i)?pTrRecHit(i)->fTrClusterY:0;} 
   unsigned int trtrack_status(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Status:0;} 
   int trtrack_pattern(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Pattern:0;} 
   int trtrack_address(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Address:0;} 
   float trtrack_locdbaver(unsigned int i){return pTrTrack(i)?pTrTrack(i)->LocDBAver:0;} 
   int   trtrack_geanefitdone(unsigned int i){return pTrTrack(i)?pTrTrack(i)->GeaneFitDone:0;} 
   int   trtrack_advancedfitdone(unsigned int i){return pTrTrack(i)?pTrTrack(i)->AdvancedFitDone:0;} 
   float trtrack_chi2strline(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Chi2StrLine:0;} 
   float trtrack_chi2withoutms(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Chi2WithoutMS:0;} 
   float trtrack_rigiditywithoutms(unsigned int i){return pTrTrack(i)?pTrTrack(i)->RigidityWithoutMS:0;} 
   float trtrack_chi2fastfit(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Chi2FastFit:0;} 
   float trtrack_rigidity(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Rigidity:0;} 
   float trtrack_errrigidity(unsigned int i){return pTrTrack(i)?pTrTrack(i)->ErrRigidity:0;} 
   float trtrack_theta(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Theta:0;} 
   float trtrack_phi(unsigned int i){return pTrTrack(i)?pTrTrack(i)->Phi:0;} 
   float trtrack_p0(unsigned int j, unsigned int i){return (j<3 && pTrTrack(i))?pTrTrack(i)->P0[j]:0;} 
   float trtrack_gchi2(unsigned int i){return pTrTrack(i)?pTrTrack(i)->GChi2:0;} 
   float trtrack_grigidity(unsigned int i){return pTrTrack(i)?pTrTrack(i)->GRigidity:0;} 
   float trtrack_gerrrigidity(unsigned int i){return pTrTrack(i)?pTrTrack(i)->GErrRigidity:0;} 
   float trtrack_hchi2(unsigned int j, unsigned int i){return (j<2 && pTrTrack(i))?pTrTrack(i)->HChi2[j]:0;} 
   float trtrack_hrigidity(unsigned int j, unsigned int i){return (j<2 && pTrTrack(i))?pTrTrack(i)->HRigidity[j]:0;} 
   float trtrack_herrrigidity(unsigned int j, unsigned int i){return (j<2 && pTrTrack(i))?pTrTrack(i)->HErrRigidity[j]:0;} 
   float trtrack_htheta(unsigned int j, unsigned int i){return (j<2 && pTrTrack(i))?pTrTrack(i)->HTheta[j]:0;} 
   float trtrack_hphi(unsigned int j, unsigned int i){return (j<2 && pTrTrack(i))?pTrTrack(i)->HPhi[j]:0;} 
   float trtrack_hp0(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pTrTrack(i))?pTrTrack(i)->HP0[j][k]:0;} 
   float trtrack_fchi2ms(unsigned int i){return pTrTrack(i)?pTrTrack(i)->FChi2MS:0;} 
   float trtrack_pierrrig(unsigned int i){return pTrTrack(i)?pTrTrack(i)->PiErrRig:0;} 
   float trtrack_rigidityms(unsigned int i){return pTrTrack(i)?pTrTrack(i)->RigidityMS:0;} 
   float trtrack_pirigidity(unsigned int i){return pTrTrack(i)?pTrTrack(i)->PiRigidity:0;} 
int trtrack_ftrrechit(unsigned int j, unsigned int i){return ( pTrTrack(i) && j<pTrTrack(i)->fTrRecHit.size() )?pTrTrack(i)->fTrRecHit[j]:0;} 
int trtrack_ftrrechit_size(unsigned int i){return ( pTrTrack(i)  )?pTrTrack(i)->fTrRecHit.size():0;} 
    unsigned int richhit_status(unsigned int i){return pRichHit(i)?pRichHit(i)->Status:0;} 
   int   richhit_channel(unsigned int i){return pRichHit(i)?pRichHit(i)->Channel:0;} 
   int   richhit_counts(unsigned int i){return pRichHit(i)?pRichHit(i)->Counts:0;} 
   float richhit_npe(unsigned int i){return pRichHit(i)?pRichHit(i)->Npe:0;} 
   float richhit_coo(unsigned int j, unsigned int i){return (j<3 && pRichHit(i))?pRichHit(i)->Coo[j]:0;} 
  unsigned int richring_status(unsigned int i){return pRichRing(i)?pRichRing(i)->Status:0;} 
   int   richring_used(unsigned int i){return pRichRing(i)?pRichRing(i)->Used:0;} 
   int   richring_usedm(unsigned int i){return pRichRing(i)?pRichRing(i)->UsedM:0;} 
   float richring_beta(unsigned int i){return pRichRing(i)?pRichRing(i)->Beta:0;} 
   float richring_errorbeta(unsigned int i){return pRichRing(i)?pRichRing(i)->ErrorBeta:0;} 
   float richring_chi2(unsigned int i){return pRichRing(i)?pRichRing(i)->Chi2:0;} 
   float richring_betarefit(unsigned int i){return pRichRing(i)?pRichRing(i)->BetaRefit:0;} 
   float richring_prob(unsigned int i){return pRichRing(i)?pRichRing(i)->Prob:0;} 
   float richring_kdist(unsigned int i){return pRichRing(i)?pRichRing(i)->KDist:0;} 
   float richring_phispread(unsigned int i){return pRichRing(i)?pRichRing(i)->PhiSpread:0;} 
   float richring_udist(unsigned int i){return pRichRing(i)?pRichRing(i)->UDist:0;} 
   float richring_npexp(unsigned int i){return pRichRing(i)?pRichRing(i)->NpExp:0;} 
   float richring_npcol(unsigned int i){return pRichRing(i)?pRichRing(i)->NpCol:0;} 
   float richring_theta(unsigned int i){return pRichRing(i)?pRichRing(i)->Theta:0;} 
   float richring_errortheta(unsigned int i){return pRichRing(i)?pRichRing(i)->ErrorTheta:0;} 
   float richring_trradpos(unsigned int j, unsigned int i){return (j<3 && pRichRing(i))?pRichRing(i)->TrRadPos[j]:0;} 
   float richring_trpmtpos(unsigned int j, unsigned int i){return (j<3 && pRichRing(i))?pRichRing(i)->TrPMTPos[j]:0;} 
   int   richring_liphitsused(unsigned int i){return pRichRing(i)?pRichRing(i)->lipHitsUsed:0;} 
   float richring_lipthetac(unsigned int i){return pRichRing(i)?pRichRing(i)->lipThetaC:0;} 
   float richring_lipbeta(unsigned int i){return pRichRing(i)?pRichRing(i)->lipBeta:0;} 
   float richring_liperrorbeta(unsigned int i){return pRichRing(i)?pRichRing(i)->lipErrorBeta:0;} 
   float richring_liplikelihoodprob(unsigned int i){return pRichRing(i)?pRichRing(i)->lipLikelihoodProb:0;} 
   float richring_lipchi2(unsigned int i){return pRichRing(i)?pRichRing(i)->lipChi2:0;} 
   float richring_liprecprob(unsigned int i){return pRichRing(i)?pRichRing(i)->lipRecProb:0;} 
   int richring_ftrtrack(unsigned int i){return pRichRing(i)?pRichRing(i)->fTrTrack:0;} 
int richring_frichhit(unsigned int j, unsigned int i){return ( pRichRing(i) && j<pRichRing(i)->fRichHit.size() )?pRichRing(i)->fRichHit[j]:0;} 
int richring_frichhit_size(unsigned int i){return ( pRichRing(i)  )?pRichRing(i)->fRichHit.size():0;} 
   int trdrawhit_layer(unsigned int i){return pTrdRawHit(i)?pTrdRawHit(i)->Layer:0;} 
   int trdrawhit_ladder(unsigned int i){return pTrdRawHit(i)?pTrdRawHit(i)->Ladder:0;} 
   int trdrawhit_tube(unsigned int i){return pTrdRawHit(i)?pTrdRawHit(i)->Tube:0;} 
   float trdrawhit_amp(unsigned int i){return pTrdRawHit(i)?pTrdRawHit(i)->Amp:0;} 
   unsigned int   trdcluster_status(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->Status:0;} 
   float trdcluster_coo(unsigned int j, unsigned int i){return (j<3 && pTrdCluster(i))?pTrdCluster(i)->Coo[j]:0;} 
   int   trdcluster_layer(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->Layer:0;} 
   int   trdcluster_direction(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->Direction:0;} 
   float trdcluster_clsizer(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->ClSizeR:0;} 
   float trdcluster_clsizez(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->ClSizeZ:0;} 
   int   trdcluster_multip(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->Multip:0;} 
   int   trdcluster_hmultip(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->HMultip:0;} 
   float trdcluster_edep(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->EDep:0;} 
   int trdcluster_ftrdrawhit(unsigned int i){return pTrdCluster(i)?pTrdCluster(i)->fTrdRawHit:0;} 
   unsigned int   trdsegment_status(unsigned int i){return pTrdSegment(i)?pTrdSegment(i)->Status:0;} 
   int   trdsegment_orientation(unsigned int i){return pTrdSegment(i)?pTrdSegment(i)->Orientation:0;} 
   float trdsegment_fitpar(unsigned int j, unsigned int i){return (j<2 && pTrdSegment(i))?pTrdSegment(i)->FitPar[j]:0;} 
   float trdsegment_chi2(unsigned int i){return pTrdSegment(i)?pTrdSegment(i)->Chi2:0;} 
   int   trdsegment_pattern(unsigned int i){return pTrdSegment(i)?pTrdSegment(i)->Pattern:0;} 
int trdsegment_ftrdcluster(unsigned int j, unsigned int i){return ( pTrdSegment(i) && j<pTrdSegment(i)->fTrdCluster.size() )?pTrdSegment(i)->fTrdCluster[j]:0;} 
int trdsegment_ftrdcluster_size(unsigned int i){return ( pTrdSegment(i)  )?pTrdSegment(i)->fTrdCluster.size():0;} 
   unsigned int   trdtrack_status(unsigned int i){return pTrdTrack(i)?pTrdTrack(i)->Status:0;} 
   float trdtrack_coo(unsigned int j, unsigned int i){return (j<3 && pTrdTrack(i))?pTrdTrack(i)->Coo[j]:0;} 
   float trdtrack_ercoo(unsigned int j, unsigned int i){return (j<3 && pTrdTrack(i))?pTrdTrack(i)->ErCoo[j]:0;} 
   float trdtrack_phi(unsigned int i){return pTrdTrack(i)?pTrdTrack(i)->Phi:0;} 
   float trdtrack_theta(unsigned int i){return pTrdTrack(i)?pTrdTrack(i)->Theta:0;} 
   float trdtrack_chi2(unsigned int i){return pTrdTrack(i)?pTrdTrack(i)->Chi2:0;} 
   int   trdtrack_pattern(unsigned int i){return pTrdTrack(i)?pTrdTrack(i)->Pattern:0;} 
int trdtrack_ftrdsegment(unsigned int j, unsigned int i){return ( pTrdTrack(i) && j<pTrdTrack(i)->fTrdSegment.size() )?pTrdTrack(i)->fTrdSegment[j]:0;} 
int trdtrack_ftrdsegment_size(unsigned int i){return ( pTrdTrack(i)  )?pTrdTrack(i)->fTrdSegment.size():0;} 
   int   level1_physbpatt(unsigned int i){return pLevel1(i)?pLevel1(i)->PhysBPatt:0;} 
   int   level1_jmembpatt(unsigned int i){return pLevel1(i)?pLevel1(i)->JMembPatt:0;} 
   int   level1_tofflag1(unsigned int i){return pLevel1(i)?pLevel1(i)->TofFlag1:0;} 
   int   level1_tofflag2(unsigned int i){return pLevel1(i)?pLevel1(i)->TofFlag2:0;} 
   int   level1_tofpatt1(unsigned int j, unsigned int i){return (j<4 && pLevel1(i))?pLevel1(i)->TofPatt1[j]:0;} 
   int   level1_tofpatt2(unsigned int j, unsigned int i){return (j<4 && pLevel1(i))?pLevel1(i)->TofPatt2[j]:0;} 
   int   level1_antipatt(unsigned int i){return pLevel1(i)?pLevel1(i)->AntiPatt:0;} 
   int   level1_ecalflag(unsigned int i){return pLevel1(i)?pLevel1(i)->EcalFlag:0;} 
   unsigned short int level1_ecalpatt(unsigned int j, unsigned int k, unsigned int i){return (j<6 && k<3 && pLevel1(i))?pLevel1(i)->EcalPatt[j][k]:0;} 
   float level1_ecaltrsum(unsigned int i){return pLevel1(i)?pLevel1(i)->EcalTrSum:0;} 
   float level1_livetime(unsigned int i){return pLevel1(i)?pLevel1(i)->LiveTime:0;} 
   float level1_trigrates(unsigned int j, unsigned int i){return (j<6 && pLevel1(i))?pLevel1(i)->TrigRates[j]:0;} 
   int   level3_toftr(unsigned int i){return pLevel3(i)?pLevel3(i)->TOFTr:0;} 
   int   level3_trdtr(unsigned int i){return pLevel3(i)?pLevel3(i)->TRDTr:0;} 
   int   level3_trackertr(unsigned int i){return pLevel3(i)?pLevel3(i)->TrackerTr:0;} 
   int   level3_maintr(unsigned int i){return pLevel3(i)?pLevel3(i)->MainTr:0;} 
   int   level3_direction(unsigned int i){return pLevel3(i)?pLevel3(i)->Direction:0;} 
   int   level3_ntrhits(unsigned int i){return pLevel3(i)?pLevel3(i)->NTrHits:0;} 
   int   level3_npatfound(unsigned int i){return pLevel3(i)?pLevel3(i)->NPatFound:0;} 
   int   level3_pattern(unsigned int j, unsigned int i){return (j<2 && pLevel3(i))?pLevel3(i)->Pattern[j]:0;} 
   float level3_residual(unsigned int j, unsigned int i){return (j<2 && pLevel3(i))?pLevel3(i)->Residual[j]:0;} 
   float level3_time(unsigned int i){return pLevel3(i)?pLevel3(i)->Time:0;} 
   float level3_eloss(unsigned int i){return pLevel3(i)?pLevel3(i)->ELoss:0;} 
   int   level3_trdhits(unsigned int i){return pLevel3(i)?pLevel3(i)->TRDHits:0;} 
   int   level3_hmult(unsigned int i){return pLevel3(i)?pLevel3(i)->HMult:0;} 
   float level3_trdpar(unsigned int j, unsigned int i){return (j<2 && pLevel3(i))?pLevel3(i)->TRDPar[j]:0;} 
   int   level3_ecemag(unsigned int i){return pLevel3(i)?pLevel3(i)->ECemag:0;} 
   int   level3_ecmatc(unsigned int i){return pLevel3(i)?pLevel3(i)->ECmatc:0;} 
   float level3_ectofcr(unsigned int j, unsigned int i){return (j<4 && pLevel3(i))?pLevel3(i)->ECTOFcr[j]:0;} 
   unsigned int       beta_status(unsigned int i){return pBeta(i)?pBeta(i)->Status:0;} 
   int       beta_pattern(unsigned int i){return pBeta(i)?pBeta(i)->Pattern:0;} 
   float     beta_beta(unsigned int i){return pBeta(i)?pBeta(i)->Beta:0;} 
   float     beta_betac(unsigned int i){return pBeta(i)?pBeta(i)->BetaC:0;} 
   float     beta_error(unsigned int i){return pBeta(i)?pBeta(i)->Error:0;} 
   float     beta_errorc(unsigned int i){return pBeta(i)?pBeta(i)->ErrorC:0;} 
   float     beta_chi2(unsigned int i){return pBeta(i)?pBeta(i)->Chi2:0;} 
   float     beta_chi2s(unsigned int i){return pBeta(i)?pBeta(i)->Chi2S:0;} 
   int   beta_ftrtrack(unsigned int i){return pBeta(i)?pBeta(i)->fTrTrack:0;} 
int beta_ftofcluster(unsigned int j, unsigned int i){return ( pBeta(i) && j<pBeta(i)->fTofCluster.size() )?pBeta(i)->fTofCluster[j]:0;} 
int beta_ftofcluster_size(unsigned int i){return ( pBeta(i)  )?pBeta(i)->fTofCluster.size():0;} 
   unsigned int charge_status(unsigned int i){return pCharge(i)?pCharge(i)->Status:0;} 
   int charge_chargetof(unsigned int i){return pCharge(i)?pCharge(i)->ChargeTOF:0;} 
   int charge_chargetracker(unsigned int i){return pCharge(i)?pCharge(i)->ChargeTracker:0;} 
   int charge_chargerich(unsigned int i){return pCharge(i)?pCharge(i)->ChargeRich:0;} 
   float charge_probtof(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ProbTOF[j]:0;} 
   int charge_chintof(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ChInTOF[j]:0;} 
   float charge_probtracker(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ProbTracker[j]:0;} 
   int charge_chintracker(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ChInTracker[j]:0;} 
   float charge_probrich(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ProbRich[j]:0;} 
   int charge_chinrich(unsigned int j, unsigned int i){return (j<4 && pCharge(i))?pCharge(i)->ChInRich[j]:0;} 
   float charge_proballtracker(unsigned int i){return pCharge(i)?pCharge(i)->ProbAllTracker:0;} 
   float charge_truntof(unsigned int i){return pCharge(i)?pCharge(i)->TrunTOF:0;} 
   float charge_truntofd(unsigned int i){return pCharge(i)?pCharge(i)->TrunTOFD:0;} 
   float charge_truntracker(unsigned int i){return pCharge(i)?pCharge(i)->TrunTracker:0;} 
   int  charge_fbeta(unsigned int i){return pCharge(i)?pCharge(i)->fBeta:0;} 
   int  charge_frichring(unsigned int i){return pCharge(i)?pCharge(i)->fRichRing:0;} 
   unsigned int   vertex_status(unsigned int i){return pVertex(i)?pVertex(i)->Status:0;} 
   float vertex_mass(unsigned int i){return pVertex(i)?pVertex(i)->Mass:0;} 
   float vertex_momentum(unsigned int i){return pVertex(i)?pVertex(i)->Momentum:0;} 
   float vertex_errmomentum(unsigned int i){return pVertex(i)?pVertex(i)->ErrMomentum:0;} 
   float vertex_theta(unsigned int i){return pVertex(i)?pVertex(i)->Theta:0;} 
   float vertex_phi(unsigned int i){return pVertex(i)?pVertex(i)->Phi:0;} 
   float vertex_vertex(unsigned int j, unsigned int i){return (j<3 && pVertex(i))?pVertex(i)->Vertex[j]:0;} 
   int   vertex_charge(unsigned int i){return pVertex(i)?pVertex(i)->Charge:0;} 
   float vertex_chi2(unsigned int i){return pVertex(i)?pVertex(i)->Chi2:0;} 
   int   vertex_ndof(unsigned int i){return pVertex(i)?pVertex(i)->Ndof:0;} 
int vertex_ftrtrack(unsigned int j, unsigned int i){return ( pVertex(i) && j<pVertex(i)->fTrTrack.size() )?pVertex(i)->fTrTrack[j]:0;} 
int vertex_ftrtrack_size(unsigned int i){return ( pVertex(i)  )?pVertex(i)->fTrTrack.size():0;} 
   unsigned int particle_status(unsigned int i){return pParticle(i)?pParticle(i)->Status:0;} 
   int   particle_particle(unsigned int i){return pParticle(i)?pParticle(i)->Particle:0;} 
   int   particle_particlevice(unsigned int i){return pParticle(i)?pParticle(i)->ParticleVice:0;} 
   float particle_prob(unsigned int j, unsigned int i){return (j<2 && pParticle(i))?pParticle(i)->Prob[j]:0;} 
   float particle_fitmom(unsigned int i){return pParticle(i)?pParticle(i)->FitMom:0;} 
   float particle_mass(unsigned int i){return pParticle(i)?pParticle(i)->Mass:0;} 
   float particle_errmass(unsigned int i){return pParticle(i)?pParticle(i)->ErrMass:0;} 
   float particle_momentum(unsigned int i){return pParticle(i)?pParticle(i)->Momentum:0;} 
   float particle_errmomentum(unsigned int i){return pParticle(i)?pParticle(i)->ErrMomentum:0;} 
   float particle_beta(unsigned int i){return pParticle(i)?pParticle(i)->Beta:0;} 
   float particle_errbeta(unsigned int i){return pParticle(i)?pParticle(i)->ErrBeta:0;} 
   float particle_charge(unsigned int i){return pParticle(i)?pParticle(i)->Charge:0;} 
   float particle_theta(unsigned int i){return pParticle(i)?pParticle(i)->Theta:0;} 
   float particle_phi(unsigned int i){return pParticle(i)?pParticle(i)->Phi:0;} 
   float particle_thetagl(unsigned int i){return pParticle(i)?pParticle(i)->ThetaGl:0;} 
   float particle_phigl(unsigned int i){return pParticle(i)?pParticle(i)->PhiGl:0;} 
   float particle_coo(unsigned int j, unsigned int i){return (j<3 && pParticle(i))?pParticle(i)->Coo[j]:0;} 
   float particle_cutoff(unsigned int i){return pParticle(i)?pParticle(i)->Cutoff:0;} 
   float particle_tofcoo(unsigned int j, unsigned int k, unsigned int i){return (j<4 && k<3 && pParticle(i))?pParticle(i)->TOFCoo[j][k]:0;} 
   float particle_anticoo(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pParticle(i))?pParticle(i)->AntiCoo[j][k]:0;} 
   float particle_ecalcoo(unsigned int j, unsigned int k, unsigned int i){return (j<3 && k<3 && pParticle(i))?pParticle(i)->EcalCoo[j][k]:0;} 
   float particle_trcoo(unsigned int j, unsigned int k, unsigned int i){return (j<8 && k<3 && pParticle(i))?pParticle(i)->TrCoo[j][k]:0;} 
   float particle_trdcoo(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pParticle(i))?pParticle(i)->TRDCoo[j][k]:0;} 
   float particle_richcoo(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<3 && pParticle(i))?pParticle(i)->RichCoo[j][k]:0;} 
   float particle_richpath(unsigned int j, unsigned int i){return (j<2 && pParticle(i))?pParticle(i)->RichPath[j]:0;} 
   float particle_richpathbeta(unsigned int j, unsigned int i){return (j<2 && pParticle(i))?pParticle(i)->RichPathBeta[j]:0;} 
   float particle_richlength(unsigned int i){return pParticle(i)?pParticle(i)->RichLength:0;} 
   int   particle_richparticles(unsigned int i){return pParticle(i)?pParticle(i)->RichParticles:0;} 
   float particle_local(unsigned int j, unsigned int i){return (j<8 && pParticle(i))?pParticle(i)->Local[j]:0;} 
   float particle_trdlikelihood(unsigned int i){return pParticle(i)?pParticle(i)->TRDLikelihood:0;} 
   int  particle_fbeta(unsigned int i){return pParticle(i)?pParticle(i)->fBeta:0;} 
   int  particle_fcharge(unsigned int i){return pParticle(i)?pParticle(i)->fCharge:0;} 
   int  particle_ftrtrack(unsigned int i){return pParticle(i)?pParticle(i)->fTrTrack:0;} 
   int  particle_ftrdtrack(unsigned int i){return pParticle(i)?pParticle(i)->fTrdTrack:0;} 
   int  particle_frichring(unsigned int i){return pParticle(i)?pParticle(i)->fRichRing:0;} 
   int  particle_fecalshower(unsigned int i){return pParticle(i)?pParticle(i)->fEcalShower:0;} 
   int  particle_fvertex(unsigned int i){return pParticle(i)?pParticle(i)->fVertex:0;} 
   int   antimccluster_idsoft(unsigned int i){return pAntiMCCluster(i)?pAntiMCCluster(i)->Idsoft:0;} 
   float antimccluster_coo(unsigned int j, unsigned int i){return (j<3 && pAntiMCCluster(i))?pAntiMCCluster(i)->Coo[j]:0;} 
   float antimccluster_tof(unsigned int i){return pAntiMCCluster(i)?pAntiMCCluster(i)->TOF:0;} 
   float antimccluster_edep(unsigned int i){return pAntiMCCluster(i)?pAntiMCCluster(i)->Edep:0;} 
   int trmccluster_idsoft(unsigned int i){return pTrMCCluster(i)?pTrMCCluster(i)->Idsoft:0;} 
   int trmccluster_trackno(unsigned int i){return pTrMCCluster(i)?pTrMCCluster(i)->TrackNo:0;} 
   int trmccluster_left(unsigned int j, unsigned int i){return (j<2 && pTrMCCluster(i))?pTrMCCluster(i)->Left[j]:0;} 
   int trmccluster_center(unsigned int j, unsigned int i){return (j<2 && pTrMCCluster(i))?pTrMCCluster(i)->Center[j]:0;} 
   int trmccluster_right(unsigned int j, unsigned int i){return (j<2 && pTrMCCluster(i))?pTrMCCluster(i)->Right[j]:0;} 
   float trmccluster_ss(unsigned int j, unsigned int k, unsigned int i){return (j<2 && k<5 && pTrMCCluster(i))?pTrMCCluster(i)->SS[j][k]:0;} 
   float trmccluster_xca(unsigned int j, unsigned int i){return (j<3 && pTrMCCluster(i))?pTrMCCluster(i)->Xca[j]:0;} 
   float trmccluster_xcb(unsigned int j, unsigned int i){return (j<3 && pTrMCCluster(i))?pTrMCCluster(i)->Xcb[j]:0;} 
   float trmccluster_xgl(unsigned int j, unsigned int i){return (j<3 && pTrMCCluster(i))?pTrMCCluster(i)->Xgl[j]:0;} 
   float trmccluster_sum(unsigned int i){return pTrMCCluster(i)?pTrMCCluster(i)->Sum:0;} 
   int   tofmccluster_idsoft(unsigned int i){return pTofMCCluster(i)?pTofMCCluster(i)->Idsoft:0;} 
   float tofmccluster_coo(unsigned int j, unsigned int i){return (j<3 && pTofMCCluster(i))?pTofMCCluster(i)->Coo[j]:0;} 
   float tofmccluster_tof(unsigned int i){return pTofMCCluster(i)?pTofMCCluster(i)->TOF:0;} 
   float tofmccluster_edep(unsigned int i){return pTofMCCluster(i)?pTofMCCluster(i)->Edep:0;} 
   int   trdmccluster_layer(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Layer:0;} 
   int   trdmccluster_ladder(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Ladder:0;} 
   int   trdmccluster_tube(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Tube:0;} 
   int   trdmccluster_particleno(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->ParticleNo:0;} 
   float trdmccluster_edep(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Edep:0;} 
   float trdmccluster_ekin(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Ekin:0;} 
   float trdmccluster_xgl(unsigned int j, unsigned int i){return (j<3 && pTrdMCCluster(i))?pTrdMCCluster(i)->Xgl[j]:0;} 
   float trdmccluster_step(unsigned int i){return pTrdMCCluster(i)?pTrdMCCluster(i)->Step:0;} 
   int   richmccluster_id(unsigned int i){return pRichMCCluster(i)?pRichMCCluster(i)->Id:0;} 
   float richmccluster_origin(unsigned int j, unsigned int i){return (j<3 && pRichMCCluster(i))?pRichMCCluster(i)->Origin[j]:0;} 
   float richmccluster_direction(unsigned int j, unsigned int i){return (j<3 && pRichMCCluster(i))?pRichMCCluster(i)->Direction[j]:0;} 
   int   richmccluster_status(unsigned int i){return pRichMCCluster(i)?pRichMCCluster(i)->Status:0;} 
   int   richmccluster_numgen(unsigned int i){return pRichMCCluster(i)?pRichMCCluster(i)->NumGen:0;} 
   int   richmccluster_frichhit(unsigned int i){return pRichMCCluster(i)?pRichMCCluster(i)->fRichHit:0;} 
 float mctrack_radl(unsigned int i){return pMCTrack(i)?pMCTrack(i)->RadL:0;} 
 float mctrack_absl(unsigned int i){return pMCTrack(i)?pMCTrack(i)->AbsL:0;} 
 float mctrack_pos(unsigned int j, unsigned int i){return (j<3 && pMCTrack(i))?pMCTrack(i)->Pos[j]:0;} 
 char  mctrack_volname(unsigned int j, unsigned int i){return (j<5 && pMCTrack(i))?pMCTrack(i)->VolName[j]:0;} 
   int mceventg_nskip(unsigned int i){return pMCEventg(i)?pMCEventg(i)->Nskip:0;} 
   int mceventg_particle(unsigned int i){return pMCEventg(i)?pMCEventg(i)->Particle:0;} 
   float mceventg_coo(unsigned int j, unsigned int i){return (j<3 && pMCEventg(i))?pMCEventg(i)->Coo[j]:0;} 
   float mceventg_dir(unsigned int j, unsigned int i){return (j<3 && pMCEventg(i))?pMCEventg(i)->Dir[j]:0;} 
   float mceventg_momentum(unsigned int i){return pMCEventg(i)?pMCEventg(i)->Momentum:0;} 
   float mceventg_mass(unsigned int i){return pMCEventg(i)?pMCEventg(i)->Mass:0;} 
   float mceventg_charge(unsigned int i){return pMCEventg(i)?pMCEventg(i)->Charge:0;} 
