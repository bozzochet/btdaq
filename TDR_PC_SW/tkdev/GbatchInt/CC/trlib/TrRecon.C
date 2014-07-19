/// $Id: TrRecon.C,v 1.28 2009/01/12 15:24:25 oliva Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TrRecon.C
///\brief Source file of TrRecon class
///
///\date  2008/02/16 AO  First version
///\date  2008/02/21 AO  BuildTrClusters(), BuildTrRecHits()
///\date  2008/02/24 AO  BuildPatterns(), some bug fixed
///\date  2008/02/28 AO  Bug fixing
///\date  2008/03/11 AO  Some change in clustering methods 
///\date  2008/06/19 AO  Updating TrCluster building 
///
/// $Date: 2009/01/12 15:24:25 $
///
/// $Revision: 1.28 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecon.h"
#include "TrMap.h"
#include "tkdcards.h"
#include "TrField.h"

#include "TrTrack.h"
#include "TrFit.h"
#include "TkSens.h"
#include "TkCoo.h"
#include "TrMCCluster.h"

VCon* TrRecon::TRCon=0;


TrRecon* TrRecon::Head=0;
TrCalDB* TrRecon::_trcaldb=0;

TrRecon* TrRecon::Create(int recreate){
  if (Head && !recreate){
    printf("TrRecon::Create Warning a Trecon instance already exist!\n");
    return Head;
  }
  if( (Head && recreate)||(!Head))
    Head= new TrRecon();
  for (int i = 0; i < BUFSIZE; i++) {
    _adcbuf2[i] =  0;
    _sigbuf2[i] =  -1;
    _stabuf2[i] =  -1;
  } 

  return Head;
}

// clustering - parameters
float       TrRecon::ThrSeed[2]  = {3.50,3.50};
float       TrRecon::ThrNeig[2]  = {0.10,0.10};
int         TrRecon::SeedDist[2] = {   3,   3};

// clustering - analysis structure
float       TrRecon::_adcbuf[TrRecon::BUFSIZE];
float       TrRecon::_sigbuf[TrRecon::BUFSIZE];
int         TrRecon::_stabuf[TrRecon::BUFSIZE];

float       TrRecon::_adcbuf2[TrRecon::BUFSIZE];
float       TrRecon::_sigbuf2[TrRecon::BUFSIZE];
int         TrRecon::_stabuf2[TrRecon::BUFSIZE];
vector<int> TrRecon::_seedaddresses;

// hit signal correlation (only muons/protons)
float  TrRecon::GGpars[6]  = {1428.,0.0000,0.1444,1645.,0.0109,0.0972};
float  TrRecon::GGintegral = 91765.;
float  TrRecon::ThrProb    = 0;//0.00001;


void TrRecon::SetParFromDataCards(){
  ThrSeed[0]  = TRCLFFKEY.ThrSeed[0] ;
  ThrSeed[1]  =	TRCLFFKEY.ThrSeed[1]  ;
  ThrNeig[0]  = TRCLFFKEY.ThrNeig[0]  ;
  ThrNeig[1]  = TRCLFFKEY.ThrNeig[1]  ;
  
  SeedDist[0] = TRCLFFKEY.SeedDist[0] ;
  SeedDist[1] = TRCLFFKEY.SeedDist[1] ;
  
  GGpars[0]  = TRCLFFKEY.GGpars[0]    ;
  GGpars[1]  = TRCLFFKEY.GGpars[1]    ;
  GGpars[2]  = TRCLFFKEY.GGpars[2]    ;
  GGpars[3]  = TRCLFFKEY.GGpars[3]    ;
  GGpars[4]  = TRCLFFKEY.GGpars[4]    ;
  GGpars[5]  = TRCLFFKEY.GGpars[5]    ;
  GGintegral = TRCLFFKEY.GGintegral   ;
  ThrProb    = TRCLFFKEY.ThrProb      ;
  

}
TrRecon::~TrRecon() {
  Clear();
}

void TrRecon::Clear(Option_t *option) {
  /*
  for(int ii=0;ii<2;ii++){
    ThrSeed[ii]=0;
    ThrNeig[ii]=0;
    SeedDist[ii]=0;
  }
  //
  memset(GGpars,0,6*sizeof(GGpars[0]));
  GGintegral=0;
  ThrProb=0;
  */
  _ClusterTkIdMap.clear();
  _RecHitLayerMap.clear();
  // _Patterns.clear();
}

int TrRecon::MaxNrawCls = 200;
int TrRecon::MaxNtrCls  = 100;
int TrRecon::MaxNtrHit  =  50;

// option is temporary, 0:No_reco 1:Up_to_TrCluster 2:Full
int TrRecon::Build(int option)
{
  VCon* cont = TRCon->GetCont("AMSTrRawCluster");
  if(!cont) {
    cerr << "TrRecon::Build  Cant Find AMSTrRawCluster" << endl;
    return -1;
  }

  // Check number of TrRawClusters
  int nraw = cont->getnelem();
  delete cont;

  if (option == 0) return nraw;
  if (nraw <= 0 || MaxNrawCls <= nraw) return 0;

  // temporary
  int rebuild = 0;

  // Build TrClusters
  int ncls = BuildTrClusters(rebuild);
  if (option == 1) return ncls;

  // Check number of TrClusters
  if (ncls <= 0 || MaxNtrCls <= ncls) return 0;

  // Build TrRecHits
  int nhit = BuildTrRecHits(rebuild);
  if (option == 2) return nhit;

  // Check number of TrRecHits
  int ret = 0;
  if (0 < nhit && nhit < MaxNtrHit) {
    // Build TrTracks and TrRecHits associated with the track
    ret = BuildTrTracks(rebuild);
  }

  return ret;
}

// void TrRecon::PrintEvent() {
//   // Summary
//   printf("Summary:\n");
//   printf("nRawClusters: %4d\n",GetEvent()->GetnTrRawClusters());
//   printf("nTrClusters:  %4d\n",GetEvent()->GetnTrClusters());
//   printf("nTrRecHits:   %4d\n",GetEvent()->GetnTrRecHits());
//   printf("nTrTracks:    %4d\n",GetEvent()->GetnTrTracks());
//   printf("\n");
//   // Summary by ladder
//   BuildClusterTkIdMap();
//   printf("List of the %3d active ladders: \n",GetnActiveLadders());
//   for (int tt=0; tt<GetnActiveLadders(); tt++) {
//     int tkid = GetTkIdActiveLadder(tt);
//     printf("TkId: %4d  nXCluster: %3d  nYCluster: %3d\n",
//            tkid,GetnTrClusters(tkid,0),GetnTrClusters(tkid,1));
//   }
//   printf("\n");
//   // Summary by layer
//   BuildRecHitLayerMap();
//   printf("Numbers of hits per layer: \n");
//   for (int ll=1; ll<9; ll++) {
//     printf("Layer: %1d  nHits: %3d\n",ll,GetnTrRecHits(ll));
//   }
//   printf("\n");
//   // Hit list
//   printf("Hit list (without multiplicity on x coordinate): \n");
//   for (int hh=0; hh<GetEvent()->GetnTrRecHits(); hh++) {
//     AMSTrRecHit* hit   = GetEvent()->GetTrRecHit(hh);
//     AMSPoint  point = hit->GetGlobalCoordinate();
//     printf("Hit: %3d  (x,y,z)=(%10.4f,%10.4f,%10.4f)  mult: %1d  corr: %8.4f  prob: %7.5f  stat: %2d\n",
//            hh,point.x(),point.y(),point.z(),hit->GetMultiplicity(),hit->GetCorrelation(),hit->GetProb(),hit->GetStatus());
//   }
//   printf("\n");
//  // Track list
//   printf("Track list: \n");
//   for (int tt=0; tt<GetEvent()->GetnTrTracks(); tt++) {
//     TrTrack* track  = GetEvent()->GetTrTrack(tt);
//     printf("nTrack: %2d  (x0,y0,z0)=(%8.4f,%8.4f,%8.4f)  (Theta,Phi)=(%9.6f,%9.6f)  nPoints: %1d  Chi2: %10.4f\n",
//            tt,track->GetP0x(),track->GetP0y(),track->GetP0z(),
//            track->GetTheta(),track->GetPhi(),track->GetNhits(),track->GetChisq());
//   }
//   printf("\n");
// }



//////////////////////
// --- CLUSTERS --- //
//////////////////////

void TrRecon::ClearBuffer() {
 


  memcpy(_adcbuf, _adcbuf2,BUFSIZE*sizeof(float));
  memcpy(_sigbuf, _sigbuf2,BUFSIZE*sizeof(float));
  memcpy(_stabuf, _stabuf2,BUFSIZE*sizeof(int));

  _seedaddresses.clear();
}

int TrRecon::ExpandClusterInBuffer(AMSTrRawCluster* cluster) {
  int entries = 0;
  int nelements = cluster->GetNelem();
  int firststripaddress = cluster->GetAddress();
  cal = GetTrCalDB()->FindCal_TkId(cluster->GetTkId());
  for (int jj=0; jj<nelements; jj++) {
    int address = firststripaddress + jj;
    if ( (address<0)||(address>=1024) ) continue;
    _adcbuf[address] = cluster->GetSignal(jj);
    _sigbuf[address] = cal->GetSigma(address);
    _stabuf[address] = cal->GetStatus(address);
    entries++;
  }
  return entries;
}


int TrRecon::GetAddressInSubBuffer(int address, int first, int last, int ciclicity) {
  // by default returns the address
  if ( (address<last)&&(address>=first) ) return address;
  // if not cyclic returns the first strip in case of underflow and the last-1 in case of overflow  
  if (ciclicity==0) return TMath::Max(first,TMath::Min(address,last-1));
  // if cyclic adds the interval (last-first)
  return (address>=last) ? (address-(last-first)) : (address+(last-first));
}


int TrRecon::BuildTrClusters(int rebuild) { 
  //Get the pointer to the TrRawCluster container
  //  AMSContainer* cont=GetCont(AMSID("AMSTrRawCluster"));
  VCon* cont=TRCon->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrRawCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    printf("TrRecon::BuildTrClusters  AMSTrRawCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }


  VCon* cont2=TRCon->GetCont("AMSTrCluster");
  if(!cont2){
    printf("TrRecon::BuildTrClusters  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container if requested
  if(rebuild) cont2->eraseC();


  //Build The Map of TrRawClusters
  TrMap<AMSTrRawCluster> RawMap;


  for (int ii=0;ii< cont->getnelem();ii++){
    AMSTrRawCluster* clu=(AMSTrRawCluster*)cont->getelem(ii);
    if (clu)  RawMap.Add(clu);
  }



  //  RawMap.info();
  // loop on ladders
  TrMap<AMSTrRawCluster>::TrMapIT lad=RawMap.LadMap.begin();
  int ncls = 0;
  while (lad!=RawMap.LadMap.end()){
    ClearBuffer();
    //expand clusters in the buffer
    for (int iclus=0; iclus<(int)lad->second.size(); iclus++) 
      ExpandClusterInBuffer( lad->second.at(iclus) );
    
    int TkId=lad->first;
    int layer = abs(TkId/100);
    ncls += BuildTrClustersInSubBuffer(TkId,0,640,0);      // S
    //printf("TkId: %+03d Found %d cluster on S\n",TkId,ncls);
    //printf("TrCluster container has: %d elements\n",cont2->getnelem());
    if ( (layer==1)||(layer==8) ) { 
      ncls += BuildTrClustersInSubBuffer(TkId,640,1024,1); // K7
    }
    else { 
      ncls += BuildTrClustersInSubBuffer(TkId,640,832,0);  // K5 - sensor1
      ncls += BuildTrClustersInSubBuffer(TkId,832,1024,0); // K5 - sensor2 
    }
    lad++;
  }

  if(cont) delete cont;
  if(cont2) delete cont2;
  return ncls;
}


int TrRecon::BuildTrClustersInSubBuffer(int tkid, int first, int last, int cyclicity) {
  int   ntrclusters = 0;
  int   side = (first>639)?0:1;
  if (_trcaldb==0) {
    printf("TrRecon::BuildTrClustersInSubBuffer Error, no _trcaldb specified.\n");
    return -9999; 
  }
  cal = GetTrCalDB()->FindCal_TkId(tkid);
  if (!cal) {printf ("TrRecon::BuildTrClustersInSubBuffer, WARNING calibration not found!!\n"); return -9999;}
  // create a list of seeds 
  int nseeds = BuildSeedListInSubBuffer(first, last, cyclicity);
//   printf("Found %d seeds\n",nseeds);
//   for (int jj=0;jj<nseeds;jj++) printf("seed %d  %d\n",jj,_seedaddresses.at(jj));

  VCon* cont=TRCon->GetCont("AMSTrCluster");

  // loop on seeds
  for (int ss=0; ss<nseeds; ss++) {
    int seedaddress  = _seedaddresses.at(ss);
    int boundary     = GetBoundariesInSubBuffer(ss,first,last,cyclicity);
    int leftaddress  = boundary/10000;             
    int rightaddress = boundary-leftaddress*10000; 
    int length       = (leftaddress<=rightaddress) ? rightaddress-leftaddress+1 : last-leftaddress+rightaddress-first+1;
    int seedind      = (seedaddress>=leftaddress)  ? seedaddress-leftaddress    : last-leftaddress+seedaddress-first;  
    float sigg[1024];
    for (int jj=0; jj<length; jj++) { 
      int address = GetAddressInSubBuffer(leftaddress+jj,first,last,cyclicity);
      sigg[jj]=_adcbuf[address];
    }

    AMSTrCluster* cluster = new AMSTrCluster(tkid,side,leftaddress,length,seedind,sigg,0);
//  cluster->BuildCoordinates();
    if(cont) cont->addnext(cluster);
    ntrclusters++;   
  }
  if(cont) delete cont;
  return ntrclusters;
}

int TrRecon::BuildSeedListInSubBuffer(int first, int last, int cyclicity) {
  _seedaddresses.clear();
  int side    = (first>639) ? 0 : 1;
  int address = 0;
  for (int ii=first; ii<last; ii++) {
    // 1. the strip has to be well defined
    // 2. a noisy strip can't be a seed strip
    // 3. the seed must exceed the ThrSeed S/N threshold
    if (  (_sigbuf[ii]<=0.)||(_stabuf[ii]!=0)||( (_adcbuf[ii]/_sigbuf[ii])<ThrSeed[side] )  ) continue;
    // 4. if the next strip is good, above threeshold and with signal larger than this one continue
    if( ii<(last-1)&&(_adcbuf[ii+1]>_adcbuf[ii]))
      if ((_sigbuf[ii+1]>0.)&&(_stabuf[ii+1]==0)&&( (_adcbuf[ii+1]/_sigbuf[ii+1])>=ThrSeed[side]))
	continue;

    // 5. the seed has to be the local maximum among 2*SeedDist+1 good (with status==0) strips
    bool localmax = true;
    for (int jj=1; jj<=SeedDist[side]; jj++) {
      // right
      address = GetAddressInSubBuffer(ii+jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
      // left
      address = GetAddressInSubBuffer(ii-jj,first,last,cyclicity);
      if ( (_adcbuf[ii]<=_adcbuf[address])&&(_stabuf[address]==0) ) { localmax = false; break; }
    }
    // is not a local maximum
    if (!localmax) continue;
    // A SEED IS CATCHED
    int seedaddress = ii;
    _seedaddresses.push_back(seedaddress);
  }
  return _seedaddresses.size();
}

int TrRecon::GetBoundariesInSubBuffer(int index, int first, int last, int cyclicity) {

  // the seeds vector
  int side         = (first>639)?0:1;
  int nseeds       = _seedaddresses.size();
  int seedaddress  = _seedaddresses.at(index);

  /// default boundary
  int leftaddress  = seedaddress;
  int rightaddress = seedaddress;

  /// the strip perimeter for the searching algorithm (from left to right)
  int left  = (index<=0) ? first : _seedaddresses.at(index-1);
  if ( (cyclicity!=0)&&(index<=0) ) left = _seedaddresses.at(nseeds-1);
  int right = (index>=(nseeds-1)) ? last-1 : _seedaddresses.at(index+1);
  if ((cyclicity!=0)&&(index>=(nseeds-1))) right = _seedaddresses.at(0);

  // search for the left boundary
  int address = GetAddressInSubBuffer(leftaddress-1,first,last,cyclicity); 
  while (address!=left) { 
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=ThrNeig[side]) break;
    // the neighboring strip signal has to be smaller than the seed one
    if (_adcbuf[address]>_adcbuf[seedaddress]) break;   
    // the strip is good
    leftaddress--;
    // new strip under analysis
    address = GetAddressInSubBuffer(leftaddress-1,first,last,cyclicity);
  } 
  leftaddress = GetAddressInSubBuffer(leftaddress,first,last,cyclicity);

  // search for the right boundary
  address = GetAddressInSubBuffer(rightaddress+1,first,last,cyclicity); 
  while (address!=right) { 
    // the neighboring strip must exceed the ThrNeig S/N threshold
    if (_adcbuf[address]/_sigbuf[address]<=ThrNeig[side]) break;
    // the neighboring strip signal has to be smaller than the seed one
    if (_adcbuf[address]>_adcbuf[seedaddress]) break;   
    // the strip is good
    rightaddress++;
    // new strip under analysis
    address = GetAddressInSubBuffer(rightaddress+1,first,last,cyclicity);
  } 
  rightaddress = GetAddressInSubBuffer(rightaddress,first,last,cyclicity);

  return 10000*leftaddress + rightaddress; 
}


//////////////////
// --- HITS --- //
//////////////////

float TrRecon::GetCorrelation(AMSTrCluster* cln, AMSTrCluster* clp) { 
  if (!cln) return -1.;
  if (!clp) return  1.;
  float n = cln->GetTotSignal();
  float p = clp->GetTotSignal();
  return (p - n)/(p + n); 
}

float  TrRecon::GetProbToBeCorrelated(float n, float p) {
  float correlation = GetCorrelation(n,p);
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
	   GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}

float  TrRecon::GetProbToBeCorrelated(AMSTrCluster* cln, AMSTrCluster* clp) {
  float correlation = GetCorrelation(cln,clp);
  return ( GGpars[0]*TMath::Gaus(correlation,GGpars[1],GGpars[2],kFALSE) +
	   GGpars[3]*TMath::Gaus(correlation,GGpars[4],GGpars[5],kFALSE) ) / GGintegral;
}

void TrRecon::BuildClusterTkIdMap() {
  _ClusterTkIdMap.clear();
  // loop on TrClusters
  VCon* cont=TRCon->GetCont("AMSTrCluster");
  for (int ii=0;ii<cont->getnelem();ii++){
    AMSTrCluster* cluster = (AMSTrCluster*)cont->getelem(ii);
    int tkid = cluster->GetTkId();
    int side = cluster->GetSide();
    // add the cluster in the vector of the map (if the tkid doesn't exists create the entry)
    if(side==0) 
      _ClusterTkIdMap[tkid].first.push_back(cluster);
    else 
      _ClusterTkIdMap[tkid].second.push_back(cluster);
  }
  if(cont) delete cont;
}


int  TrRecon::GetnTrClusters(int tkid, int side) { 
  return (side==0) ? _ClusterTkIdMap[tkid].first.size() : _ClusterTkIdMap[tkid].second.size(); 
}

AMSTrCluster* TrRecon::GetTrCluster(int tkid, int side, int iclus) { 
  if (iclus < 0 || GetnTrClusters(tkid, side) <= iclus) return 0;
  return (side==0) ? _ClusterTkIdMap[tkid].first.at(iclus) : _ClusterTkIdMap[tkid].second.at(iclus); 
}

int TrRecon::BuildTrRecHits(int rebuild) 
{
  //Get the pointer to the TrCluster container
  VCon* cont=TRCon->GetCont("AMSTrCluster");
  if (!cont) {
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrCluster Container "
	   "Reconstruction is Impossible !!!\n");
    return -1;
  }
  if (cont->getnelem() == 0) {
    delete cont;
    return 0;
  }

  VCon* cont2=TRCon->GetCont("AMSTrRecHit");
  if (!cont2){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrRecHit Container "
	   "Reconstruction is Impossible !!!\n");
    return -1;
  }
  //empty the destination container
  if (rebuild)
    cont2->eraseC();

  BuildClusterTkIdMap();

  int ntrrechits = 0;

  for (ITClusterMap lad = _ClusterTkIdMap.begin();
       lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int nx   = lad->second.first.size();
    int ny   = lad->second.second.size();

    // all combinations
    for (int iy=0; iy<ny; iy++) {
      for (int ix=0; ix<nx; ix++) {
	AMSTrCluster* clX = lad->second.first.at(ix);
	AMSTrCluster* clY = lad->second.second.at(iy);
	float     corr = GetCorrelation(clX,clY);
	float     prob = GetProbToBeCorrelated(clX,clY);
	if (prob<ThrProb) continue;  
	AMSTrRecHit* hit = new AMSTrRecHit(tkid,clX,clY,corr,prob,0);
	cont2->addnext(hit);
	ntrrechits++;
      }

      // "ghost" hits with only y-cluster
      AMSTrCluster *clY = GetTrCluster(tkid, 1, iy);
      AMSTrRecHit  *hit = new AMSTrRecHit(tkid, 0, clY, 1, 0, 0);
      cont2->addnext(hit);
      ntrrechits++;
    }    
  }
  if(cont) delete cont;
  if(cont2) delete cont2;

  return ntrrechits;
} 

AMSTrRecHit* TrRecon::FindHit(AMSTrCluster* xcls,AMSTrCluster* ycls){
  VCon* cont1 = TRCon->GetCont("AMSTrRecHit");
  AMSTrRecHit* goodhit=0;
  AMSTrRecHit* hit=0;
  for (int ii = 0; ii < cont1->getnelem(); ii++) {
    hit=(AMSTrRecHit*)cont1->getelem(ii);
    if(hit->GetXCluster()==xcls && hit->GetYCluster()==ycls){
      goodhit=hit;
      break;
    }
  }
  delete cont1;
  return goodhit;

}

////////////////////////////////////////////////////////////////
// ------------------------- TRACKS ------------------------- //
////////////////////////////////////////////////////////////////

#define TR_DEBUG_CODE_00 \
if (TrDEBUG >= 6) {\
  int nhit = cont->getnelem();\
  int tkid = 0, intv = 0;\
  for (int i = 0; i < nhit; i++) {\
    AMSTrRecHit *hit = (AMSTrRecHit *)cont->getelem(i);\
    if (tkid != 0 && hit->GetTkId() != tkid) intv = 0;\
    if ((intv++)%3 == 0) cout << endl;\
    cout << Form("%4d[%02d](%2d,%2d,%5.3f) ", hit->GetTkId(), i,\
		 hit->GetXClusterIndex(), hit->GetYClusterIndex(),\
		 hit->GetProb());\
    tkid = hit->GetTkId();\
  }\
  cout << endl;\
}

#define TR_DEBUG_CODE_01 \
if (TrDEBUG >= 5) {\
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();\
       it != _HitsTkIdMap.end(); it++) {\
    int tkid = it->first;\
    int nclx = GetnTrRecHits(tkid, 0);\
    int ncly = GetnTrRecHits(tkid, 1);\
    cout << Form("TkId %4d Nclx= %2d Ncly= %2d", tkid, nclx, ncly) << endl;\
    for (int i = 0; i < nclx; i++) {\
      cout << " ";\
      for (int j = 0; j < ncly; j++) {\
	AMSTrRecHit *hit = GetTrRecHit(tkid, i, j);\
	if (!hit) { if (j < ncly-1) cout << "-------------- "; }\
	else cout << Form("%2d[%2d][%2d](%2.0f) ",\
			  cont->getindex(hit), hit->GetXClusterIndex(),\
			  hit->GetYClusterIndex(), hit->GetProb()*100);\
      }\
      cout << endl;\
    }\
  }\
}

#define TR_DEBUG_CODE_02 \
if (TrDEBUG >= 1) {\
  for (int ly = 1; ly <= MAXLAY; ly++) {\
    int nlad = _LadderHitMap[ly-1].size();\
    cout << Form("L:%d (%2d,%2d) ", ly, nlad, _NladderXY[ly-1]);\
    for (int i = 0; i < nlad; i++) {\
      int tkid  = _LadderHitMap[ly-1].at(i);\
      int nhity =  GetnTrRecHits(tkid, 1);\
      int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;\
      cout << Form("%4d(%d,%d) ", tkid, nhitx, nhity);\
    }\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_03 \
if (TrDEBUG >= 1) {\
  for (int i = 0; i < MAXLAY; i++) {\
    int nlad = _LadderHitMap[i].size();\
    cout << Form("L:%d (%2d,%2d) ", i+1, nlad, _NladderXY[i]);\
    for (int j = 0; j < nlad; j++) {\
      int tkid  = _LadderHitMap[i].at(j);\
      int nhitx = GetnTrClusters(tkid, 0);\
      int nhity = GetnTrClusters(tkid, 1);\
      cout << Form("%4d(%d,%d) ", tkid, nhitx, nhity);\
    }\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_04 \
if (TrDEBUG >= 11 || (TrDEBUG >= 10 && abs(pc-intp) < it.psrange)) {\
  cout << "PreScan ";\
  for (int i = 0; i <= nlay; i++) {\
    cout << Form("%4d", it.tkid[it.ilay[i]]);\
    if (it.side == 0) cout << Form("-%d", it.imult[it.ilay[i]]);\
    cout << " ";\
  }\
  for (int i = nlay+1; i < MAXLAY; i++) {\
    cout << "     ";\
    if (it.side == 0) cout << "  ";\
  }\
  cout << Form(": %5.1f %3.1f", pc-intp, it.psrange) << endl;\
}

#define TR_DEBUG_CODE_05 \
if (TrDEBUG >= 6) {\
  int nhit  = GetnTrRecHits(tkid);\
  int nhitx = GetnTrRecHits(tkid, 0);\
  int nhity = GetnTrRecHits(tkid, 1);\
  if ((nhit-nhity)%nhity != 0 || (nhit-nhity)/nhity != nhitx-1) {\
    cout << Form("Warning in TrRecon::SetLayerOrder "\
		 "Consistency check failed: %d %d %d:%2d %d %d %d %d", \
		 i, nlay, layer, nhit, nhity, nhitx,\
		 (nhit-nhity)%nhity, (nhit-nhity)/nhity) << endl;\
  }\
}

#define TR_DEBUG_CODE_11 \
if (TrDEBUG >= 2) {\
  cout << "ScanLadders " << pattern << " "\
       << GetHitPatternStr(pattern, '_', 'O') << endl;\
}

#define TR_DEBUG_CODE_21 \
if (TrDEBUG >= 1) {\
  cout << "Lcand: ";\
  for (int i = 0; i < it.nlayer; i++){\
    int nhity = GetnTrRecHits(it.tkid[i], 1);\
    int nhitx = (GetnTrRecHits(it.tkid[i])-nhity)/nhity;\
    cout << Form("%4d(%d%d) ", it.tkid[i], nhitx, nhity);\
  }\
  cout << endl;\
}

#define TR_DEBUG_CODE_22 \
if (TrDEBUG >= 1) {\
  cout << "Ccand: ";\
  for (int i = 0; i < it.nlayer; i++) {\
    if (itchit.iscan[i][0] >= 0)\
      cout<< Form(" (%d,%d,",itchit.iscan[i][0], itchit.imult[i]);\
    else cout << " (-,-,";\
    if (itchit.iscan[i][1] >= 0) cout<< itchit.iscan[i][1];\
    else cout << "-";\
    cout << ") ";\
  }\
  cout << endl << "Chisq= "\
       << itchit.chisq[0] << " " << itchit.chisq[1] << endl;\
}

#define TR_DEBUG_CODE_30 \
if (TrDEBUG >= 4) {\
  cout << Form("ScanHits %d %3d %s", it.side, it.pattern,\
   	       GetHitPatternStr(it.pattern,'_','O')) << endl;\
}

#define TR_DEBUG_CODE_31 \
if (TrDEBUG >= 3) {\
  cout << "EvalHit side= " << it.side << " nlay= " << it.nlayer \
       << " csq= " << csq << " " << itcand.chisq[it.side] << endl;\
  if (TrDEBUG >= 4) {\
    for (int i = 0; i < it.nlayer && it.ilay[i] >= 0;i++)\
      cout << Form(" %4d-%d,%d", it.tkid[it.ilay[i]],\
                   it.iscan[it.ilay[i]][it.side], it.imult[it.ilay[i]]);\
    cout << endl;\
  }\
}

#define TR_DEBUG_CODE_32 \
if (TrDEBUG >= 4) {\
  cout << "Replaced: " << endl;\
  for (int i = 0; i < itcand.nlayer; i++)\
    cout << Form(" %4d-%d%d%d", itcand.tkid[i],\
		 itcand.iscan[i][0], itcand.iscan[i][1], itcand.imult[i]);\
  cout << endl;\
}

#define TR_DEBUG_CODE_40 \
if (TrDEBUG >= 6) {\
  float locx = TkCoo::GetLocalCoo(tkid, tks.GetStripX()+640, it.imult[il]);\
  AMSPoint loc(locx, 3.5, 0);\
  AMSPoint glo = TkCoo::GetGlobalA(tkid, loc);\
  if (std::abs(px-glo[0]) > 0.5)\
    cout << Form("Diff %5.2f TkId %4d Strp %3d "\
		 "sx %5.2f im %d sen %d dx %6.2f %5.2f", px-glo[0],\
		 tkid, tks.GetStripX(), tks.GetSensCoo().x(), \
		 it.imult[il], tks.GetSensor(), px-lx, ln/2) << endl;\
}

#define TR_DEBUG_CODE_41 \
if (TrDEBUG >= 5) {\
  VCon* cont2 = TRCon->GetCont("AMSTrRecHit");\
  int ihit = cont2->getindex(hit);\
  cout << "Removed hit " << itcand.tkid[i] << " " << ihit << endl;\
  int tkid = itcand.tkid[i];\
  int nclx = GetnTrRecHits(tkid, 0);\
  int ncly = GetnTrRecHits(tkid, 1);\
  cout << Form("TkId %4d Nclx= %2d Ncly= %2d", tkid, nclx, ncly) << endl;\
  for (int i = 0; i < nclx; i++) {\
    cout << " ";\
    for (int j = 0; j < ncly; j++) {\
      AMSTrRecHit *hit = GetTrRecHit(tkid, i, j);\
      if (!hit) { if (j < ncly-1) cout << "-------------- "; }\
      else cout << Form("%2d[%2d][%2d](%2.0f) ",\
			cont2->getindex(hit), hit->GetXClusterIndex(),\
			hit->GetYClusterIndex(), hit->GetProb()*100);\
    }\
    cout << endl;\
  }\
  delete cont2;\
}

#define TR_DEBUG_CODE_42 \
if (TrDEBUG >= 1) {\
  cout << "New Track: Pattern: "\
       << GetHitPatternStr(track->GetPattern(),'_','O');\
  cout << " (" << track->GetPattern() << ") "\
       << "Nhits= " << track->GetNhitsX() << " "\
                    << track->GetNhitsY() << " "\
                    << track->GetNhitsXY()<< endl;\
  for (int i = 0; i < track->GetNhits(); i++) {\
    AMSTrRecHit *hit = track->GetHit(i);\
    if (!hit) continue;\
    AMSTrCluster *xcls = hit->GetXCluster();\
    AMSTrCluster *ycls = hit->GetYCluster();\
    if (!xcls && !ycls) continue;\
    int tkid = hit->GetTkId();\
    char cx = (xcls) ? 'X' : ' ';\
    char cy = (ycls) ? 'Y' : ' ';\
    int  im = (xcls) ? hit->GetResolvedMultiplicity() : 0;\
    cout << Form("%4d(%c%c%d) ", tkid, cx, cy, im);\
  }\
  cout << endl;\
  int id;\
  if (MAGFIELDFFKEY.magstat) {\
    id = AMSTrTrack::kSimple;\
    track->Fit(id);\
    cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
	         id, track->GetChisq(id), \
	         track->GetChisqX(id), track->GetChisqY(id), \
	         track->GetRigidity(id), \
    	         track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
	         track->GetDir(id)[0]/track->GetDir(id)[2], \
	         track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
    id = AMSTrTrack::kAlcaraz;\
    track->Fit(id);\
    cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
	         id, track->GetChisq(id), \
	         track->GetChisqX(id), track->GetChisqY(id), \
	         track->GetRigidity(id), \
	         track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
	         track->GetDir(id)[0]/track->GetDir(id)[2], \
	         track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
    id = AMSTrTrack::kChoutko;\
    track->Fit(id);\
  }\
  else\
    id = AMSTrTrack::kLinear;\
  cout << Form("%2d: %8.2e %8.2e %8.2e %8.2e %6.2f %6.2f %6.2f %6.3f %6.3f",\
	       id, track->GetChisq(id),\
	       track->GetChisqX(id), track->GetChisqY(id),\
	       track->GetRigidity(id), \
	       track->GetP0x(id), track->GetP0y(id), track->GetP0z(id), \
	       track->GetDir(id)[0]/track->GetDir(id)[2], \
	       track->GetDir(id)[1]/track->GetDir(id)[2]) << endl;\
  if (TrDEBUG >= 3) {\
    for (int i = 0; i < track->GetNhits(); i++) {\
      AMSTrRecHit *hit = track->GetHit(i);\
      int il = std::abs(hit->GetTkId())/100-1;\
      cout << Form("%4d %03X %6.2f %6.2f %6.2f res %8.1f %8.1f %8.1f %8.1f", \
	  	   hit->GetTkId(), hit->GetStatus(), hit->GetCoord()[0],\
		   hit->GetCoord()[1], hit->GetCoord()[2],\
		   1e4*track->GetResidual(il, id)[0],\
		   1e4*track->GetResidual(il, AMSTrTrack::kChoutko)[0],\
		   1e4*track->GetResidual(il, id)[1],\
		   1e4*track->GetResidual(il, AMSTrTrack::kChoutko)[1])\
	   << endl;\
    }\
  }\
}


//========================================================
// Performance tuning parameters for track reconstruction
//========================================================
int    TrRecon::MaxNtrack = 2;
int    TrRecon::MinNhitX  = 4;
int    TrRecon::MinNhitY  = 6;
int    TrRecon::MinNhitXY = 4;
int    TrRecon::PatAllowOption = 0;
double TrRecon::MaxChisqAllowed   = 300;
double TrRecon::LadderScanRange   = 7.3; //= TkDBc::Head->_ladder_Ypitch
double TrRecon::ClusterScanRange  = 0.5;
double TrRecon::MaxChisqForLScan  = 2.2;
double TrRecon::ErrXForScan       = 300e-4;
double TrRecon::ErrYForScan       = 300e-4;

int TrRecon::TrDEBUG = 0;

//========================================================
// HitPatterns
//========================================================
int  TrRecon::NHitPatterns     = 0;
int *TrRecon::HitPatternMask   = 0;
int *TrRecon::HitPatternAttrib = 0;
int *TrRecon::HitPatternIndex  = 0;
int *TrRecon::HitPatternFirst  = 0;

void TrRecon::BuildHitPatterns(int nmask, int ilyr, int mask)
{
  static int PatternID = 0;

//====================================
// General managing = starting point
//====================================
  if (nmask < 0) {
    NHitPatterns = 256 -1 -8 -8*7/2; // = 219 (Nhit >= 3)
    if (!HitPatternMask)   HitPatternMask   = new int[NHitPatterns];
    if (!HitPatternAttrib) HitPatternAttrib = new int[NHitPatterns];
    if (!HitPatternIndex)  HitPatternIndex  = new int[256];
    if (!HitPatternFirst)  HitPatternFirst  = new int[6];
    for (int j = 0; j < 256; j++) HitPatternIndex[j] = -1;
    for (int j = 0; j <   6; j++) HitPatternFirst[j] = NHitPatterns;

    // Start building HitPattern
    PatternID = 0;
    for (int j = 0; j < 6; j++) {
      HitPatternFirst[j] = PatternID;
      BuildHitPatterns(j);
    }
    return;
  }

//=================================================================
// Build pattern mask bits with n hits masked, recursively called
//=================================================================
  if (nmask > 0) {
    const int MASK_ORDER[8] = { 1, 3, 5, 2, 4, 6, 0, 7 };
    for (int j = ilyr; j < MAXLAY; j++)
      BuildHitPatterns(nmask-1, j+1, mask | (1<<(MAXLAY-MASK_ORDER[j]-1)));
    return;
  }

//=================================================================
// Check pattern attribute
// ( = Nmask*100000 +Np1*10000 +Np2*1000 +Np3*100 +Np4*10 +Np5)
//=================================================================
  int atrb = 12221, iatrb = O_NP1;
  for (int i = 0; i < MAXLAY; i++) {
    if (i%2 == 1) iatrb /= 10;
    if (mask & (1<<(MAXLAY-i-1))) atrb += -iatrb+O_NMASK;
  }
  HitPatternMask[PatternID] = mask; HitPatternAttrib[PatternID] = atrb;
  HitPatternIndex[mask] = PatternID;

  if (!TRFITFFKEY.patternp02[PatternID]) {
    // Not allowed (1): Both external layers are masked
    if ( (!(PatAllowOption & NO_EXTP) && 
       (atrb/O_NP1)%10 == 0 && (atrb/O_NP5)%10 == 0)
    // Not allowed (2): Both layers on any of internal planes are masked
      || (!(PatAllowOption & NO_INTP) && 
       ((atrb/O_NP2)%10 == 0 || (atrb/O_NP3)%10 == 0 || (atrb/O_NP4)%10 == 0))
    // Not allowed (3): Any external layers are masked and Nmask >= 3
      || (!(PatAllowOption & ALL_5L) && 
	  (atrb/O_NMASK >= 3 && (atrb/10000 == 0 || atrb%10 == 0))) )
      HitPatternAttrib[PatternID] = -atrb;
  }
  PatternID++;
}

const char *TrRecon::GetHitPatternStr(int pat, char con, char coff)
{
  static char sbuf[MAXLAY+1];
  for (int ly = 1; ly <= MAXLAY; ly++)
    sbuf[ly-1] = (TestHitPatternMask(pat, ly)) ? con : coff;
  sbuf[MAXLAY] = '\0';
  return sbuf;
}

//========================================================
// Maps for fast hit scanning
//========================================================

void TrRecon::RemoveHits(int tkid, int icls, int side)
{
  HitsTkIdMap::iterator it = _HitsTkIdMap.find(tkid);
  if (it == _HitsTkIdMap.end()) return;

  Hits2DArray &ar = it->second;
  if ((side != 0 && side != 1) || ar.Ncls[side] <= 0) return;

  int ncls = GetnTrRecHits(tkid, 1-side);
  for (int i = 0; i < ncls; i++) {
    Hits2DArray::iterator it = ar.begin();
    it += (side == 0) ? ar.index(icls, i)-i
                      : ar.index(i, icls)-i;
    ar.erase(it);
  }

  if (--ar.Ncls[side] == 0) ar.Ncls[1-side] = 0;
}

void TrRecon::BuildHitsTkIdMap() 
{
  _HitsTkIdMap.clear();

  VCon* cont = TRCon->GetCont("AMSTrRecHit");
  if(!cont) {
    cerr << "TrRecon::BuildRecHitLayerMap:  "
            "Cant Find AMSTrRecHit Container" << endl;
    return;
  }
  TR_DEBUG_CODE_00;

  // Fill TrRecHits at _HitsTkIdMap[tkid]
  int nhit = cont->getnelem();
  for (int i = 0; i < nhit; i++) {
    AMSTrRecHit *hit = (AMSTrRecHit*)cont->getelem(i);
    _HitsTkIdMap[hit->GetTkId()].push_back(hit);
  }

  // Loop on each element of _HitsTkIdMap
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();
       it != _HitsTkIdMap.end(); it++) {
    // Copy original and sort array in descending order of prob.
    Hits2DArray ar0 = it->second;
    std::sort(ar0.begin(), ar0.end(), CompProb());

    // Clear and re-allocate original array
    int tkid = it->first;
    Hits2DArray &arr = it->second;
    arr.clear();
    arr.Ncls[0] = GetnTrClusters(tkid, 0)+1;
    arr.Ncls[1] = GetnTrClusters(tkid, 1);
    arr.insert(arr.begin(), arr.Ncls[0]*arr.Ncls[1], (AMSTrRecHit*)0);

    // Define cluster index vectors
    vector<int> vidcl[2];

    // Loop on the hits
    for (Hits2DArray::iterator ith = ar0.begin(); ith != ar0.end(); ith++) {
      AMSTrRecHit *hit = *ith;

      // Get cluster ID (as index in the container)
      int idcl[2], icls[2];
      idcl[0] = hit->GetXClusterIndex();
      idcl[1] = hit->GetYClusterIndex();

      // Loop on X and Y, find cluster ID, push back it if not found
      for (int ss = 0; ss <= 1; ss++) {
	vector<int>::iterator 
	  it = std::find(vidcl[ss].begin(), vidcl[ss].end(), idcl[ss]);
	icls[ss] = it-vidcl[ss].begin();
	if (it == vidcl[ss].end()) vidcl[ss].push_back(idcl[ss]);
      }

      // Add the hit at [icls[0]][icls[1]]
      int idx = arr.index(icls[0], icls[1]);
      if (idx < 0) std::cerr << "Warning in TrRecon::BuildHitsTkIdMap "
			     << "consistency check failed" << std::endl;
      else arr[idx] = hit;
    }
  }

  TR_DEBUG_CODE_01;
  delete cont;
}

void TrRecon::BuildLadderHitMap()
{
  // Clear LadderHitMap
  for (int i = 0; i < MAXLAY; i++) {
    _NladderXY[i] = 0;
    _LadderHitMap[i].clear();
  }

  // Build LadderHitMap
  for (ITHitsTkIdMap it = _HitsTkIdMap.begin();
       it != _HitsTkIdMap.end(); it++) {
    int tkid  = it->first;
    int layer = std::abs(tkid)/100;
    int nhity = GetnTrRecHits(tkid, 1);
    if (nhity <= 0) continue;

    int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;
    if (nhitx > 0) _NladderXY[layer-1]++;
    _LadderHitMap[layer-1].push_back(tkid);
  }
  TR_DEBUG_CODE_02;
}

void TrRecon::BuildLadderClusterMap()
{
  // Clear LadderHitMap
  for (int i = 0; i < MAXLAY; i++) {
    _NladderXY[i] = 0;
    _LadderHitMap[i].clear();
  }

  // Build LadderHitMap
  for (ITClusterMap lad = _ClusterTkIdMap.begin();
       lad != _ClusterTkIdMap.end(); lad++) {
    int tkid = lad->first;
    int il   = std::abs(tkid)/100-1;
    if (GetnTrClusters(tkid, 1) > 0 &&
	GetnTrClusters(tkid, 0) > 0) _NladderXY[il]++;
    _LadderHitMap[il].push_back(tkid);
  }
  for (int i = 0; i < MAXLAY; i++)
    std::sort(_LadderHitMap[i].begin(), _LadderHitMap[i].end());

  TR_DEBUG_CODE_03;
}

//========================================================
// Track reconstruction methods
//========================================================

int TrRecon::BuildTrTracks(int)
{
  // Build hit patterns if not yet built
  if (!HitPatternMask) BuildHitPatterns();

  // Build _HitTkIdMap
  BuildHitsTkIdMap();

  Ntrials1 = Ntrials2 = 0;

  // Main loop
  int ntrack = 0, found = 0;
  do {
    // Build LadderHitMap
    BuildLadderHitMap();

    found = 0;
    // Scan Ladders for each pattern until a track found
    for (int pat = 0; !found && pat < NHitPatterns; pat++) {
      TrHitIter itcand;
      if (HitPatternAttrib[pat] > 0 && (found = ScanLadders(pat, itcand)))
	ntrack += BuildATrTrack(itcand);
    }
  } while (found && ntrack < MaxNtrack);

  // Purge "ghost" hits and assign hit index to tracks
  PurgeGhostHits();

  return ntrack;
}

void TrRecon::PurgeGhostHits()
{
  VCon* cont = TRCon->GetCont("AMSTrRecHit");
  if(!cont) return;

  int nhit = cont->getnelem();
  for (int i = 0; i < nhit; i++) {
    AMSTrRecHit *hit = (AMSTrRecHit*)cont->getelem(i);
    if (!hit) continue;

    Hits2DArray &ar = _HitsTkIdMap[hit->GetTkId()];
    if (!(hit->GetStatus() & USED) &&
	((hit->GetStatus() & YONLY) ||
	 std::find(ar.begin(), ar.end(), hit) == ar.end())) {
      delete hit;
      cont->removeEl(cont->getelem(i-1));
      nhit--;
      i--;
    }
  }
  delete cont;

  // create hits indexes
  cont = TRCon->GetCont("AMSTrTrack");
  if(!cont) return;
  int ntrack = cont->getnelem();
  for (int i = 0; i < ntrack; i++) {
    AMSTrTrack *track = (AMSTrTrack*)cont->getelem(i);
    track->BuildHitsIndex();
  }
  delete cont;

}

bool TrRecon::PreScan(int nlay, TrHitIter &it) const
{
  if (nlay == 0) return true;

  // Obtain linear interpolation parameters
  if (MAGFIELDFFKEY.magstat == 0 || it.side == 0) {
    if (nlay == 1) { 
      AMSPoint pos0 = it.coo[it.ilay[0]];
      AMSPoint pos1 = it.coo[it.ilay[1]];
      it.param[2] = it.param[3] = 0;
      it.param[1] = (pos1[it.side]-pos0[it.side])/(pos1[2]-pos0[2]);
      it.param[0] =  pos0[it.side]-it.param[1]*pos0[2];
      return true;
    }
  }

  // Obtain circular (Pol2) interpolation parameters
  else {
    if (nlay == 1) return true;
    if (nlay == 2) {
      AMSPoint pos[3] 
	= { it.coo[it.ilay[0]], it.coo[it.ilay[1]], it.coo[it.ilay[2]] };
      double mtx[3][3], vec[3];
      for (int i = 0; i < 3; i++) {
	mtx[i][0] = 1;
	mtx[i][1] = pos[i][2];
	mtx[i][2] = pos[i][2]*pos[i][2];
	vec[i]    = pos[i][it.side];
      }
      TrFit::Inv33(mtx);
      for (int i = 0; i < 3; i++) {
	it.param[i] = 0;
	for (int j = 0; j < 3; j++) it.param[i] += mtx[i][j]*vec[j];
      }
      it.param[3] = 0;
      return true;
    }
  }

  // Check interpolation
  double pz   = it.coo[it.ilay[nlay]][2];
  double pc   = it.coo[it.ilay[nlay]][it.side];
  double intp = it.param[0]+it.param[1]*pz+it.param[2]*pz*pz;
  TR_DEBUG_CODE_04;

  return (abs(pc-intp) < it.psrange);
}

int TrRecon::SetLayerOrder(TrHitIter &it) const
{
  // Fill layer index to be scanned according to hit pattern
  int nlay = 0;
  for (int i = 0; i < it.nlayer; i++) {
    int tkid  = it.tkid[i];
    int layer = std::abs(tkid)/100;
    if (!TestHitPatternMask(it.pattern, layer)) {
      it.ilay[nlay++] = i;

      // Check number of hits if tkid is specified
      if (tkid%100 != 0) {
	TR_DEBUG_CODE_05;
	int nhity = GetnTrRecHits(tkid, 1);
	if (nhity <= 0) return 0;

	int nhitx = (GetnTrRecHits(tkid)-nhity)/nhity;
	if (it.side == 0 && nhitx <= 0) return 0;
      }
    }
  }
  // Disable the other layers
  for (int i = nlay; i < it.nlayer; i++) it.ilay[i] = -1;

  // Swap 2nd and last layer
  int swp = it.ilay[1]; 
            it.ilay[1] = it.ilay[nlay-1]; 
	                 it.ilay[nlay-1] = swp;

  // swap 3rd and middle layer for the bending side
  if (it.side == 1 && MAGFIELDFFKEY.magstat) {
    swp = it.ilay[2]; 
          it.ilay[2] = it.ilay[nlay/2]; 
	               it.ilay[nlay/2] = swp;
  }

  return 1;
}

int TrRecon::ScanRecursive(int idx, TrHitIter &it, TrHitIter &itcand, 
			   int (*Coord)(int, TrHitIter&, int), 
			   int (*Eval)(TrHitIter&, TrHitIter&)) const
{
  // Evaluate current candidates if idx has reached to the end
  if (idx == it.nlayer) return Eval(it, itcand);

  // Skip if current layer is disabled
  int il = it.ilay[idx];
  if (il < 0) return ScanRecursive(idx+1, it, itcand, Coord, Eval);

  // Loop on hit candidates in the current layer
  int nscan = Coord(idx, it, 1);
  for (it.Iscan(il) = 0; it.Iscan(il) < nscan; it.Iscan(il)++) {

    // Loop on multiplicities
    int mlast = Coord(idx, it, 2);
    for (; it.imult[il] <= mlast; it.imult[il]++) {
      // Set coordinates
      if (Coord(idx, it, 3) < 0) continue;

      // Pre selection
      if (!PreScan(idx, it)) continue;

      // Go to the next layer
      ScanRecursive(idx+1, it, itcand, Coord, Eval);
    }
    if (it.imult[il] > 0) it.imult[il]--;
  }
  return 0;
}


static int LadderCoordMgr(int idx, TrRecon::TrHitIter &it, int mode)
{ return TrRecon::Head->LadderCoordMgr(idx, it, mode); }

static int LadderScanEval(TrRecon::TrHitIter &it, TrRecon::TrHitIter &cand)
{ return TrRecon::Head->LadderScanEval(it, cand); }

int TrRecon::ScanLadders(int pattern, TrHitIter &itcand) const
{
  // Clear the-best-candidate parameters
  itcand.nlayer = 0; itcand.nhitc = MinNhitXY; 
  itcand.chisq[0] = itcand.chisq[1] = MaxChisqAllowed;

  // Define and fill iterator
  TrHitIter it;
  it.pattern = pattern; it.side = 1; it.psrange = LadderScanRange;

  // Loop on layers to check for an empty layer and fill iterator
  int nhitc = it.nlayer = 0;
  for (int i = 0; i < MAXLAY; i++) {
    if (!TestHitPatternMask(pattern, i+1)) {
      if (_LadderHitMap[i].empty()) return 0;
      if (_NladderXY[i]) nhitc++;
      it.tkid[it.nlayer++] = (i+1)*100;
    }
  }
  if (it.nlayer < MinNhitY || nhitc < MinNhitXY) return 0;
  TR_DEBUG_CODE_11;

  // Set the order of scanning layers
  SetLayerOrder(it);

  // Scan ladder combination recursively
  ScanRecursive(0, it, itcand, ::LadderCoordMgr, ::LadderScanEval);

  // Return 1 if track has been found
  return (itcand.nlayer > 0);
}

int TrRecon::LadderCoordMgr(int idx, TrHitIter &it, int mode) const
{
  // Check layer index
  int il = it.ilay[idx];
  if (il < 0) return 0;

  // Layer number
  int lyr = std::abs(it.tkid[il])/100;

  // Mode 1: Return number of ladder candidates to be scanned
  if (mode == 1) return _LadderHitMap[lyr-1].size();

  // Mode 2: Set multiplicity range
  if (mode == 2) {
    it.imult[il] = 0;
    return 0;
  }

  // Mode 3: Fill coordinates to be evaluated
  it.tkid[il] = _LadderHitMap[lyr-1].at(it.Iscan(il));
  int layer = std::abs(it.tkid[il])/100;
  it.coo[il] = AMSPoint(TkCoo::GetLadderCenterX(it.tkid[il]),
			TkCoo::GetLadderCenterY(it.tkid[il]),
			TkDBc::Head->GetZlayer(layer));
  return 1;
}

int TrRecon::LadderScanEval(TrHitIter &it, TrHitIter &itcand) const
{
  // Check number of hits with both-side clusters
  int nhitc = 0;
  for (int i = 0; i < it.nlayer; i++) {
    int nhity = GetnTrRecHits(it.tkid[i], 1);
    if (nhity <= 0) continue;
    int nhitx = (GetnTrRecHits(it.tkid[i])-nhity)/nhity;
    if (nhitx > 0) nhitc++;
  }
  if (nhitc < itcand.nhitc) return 0; 

  // Obtain a track with linear/circle fitting
  TrFit fit;
  for (int i = 0; i < it.nlayer; i++)
    fit.Add(it.coo[i], 0, LadderScanRange*0.75, 1);
  if (MAGFIELDFFKEY.magstat == 1)
    fit.CircleFit(2);
  else
    fit.LinearFit(2);

  // Check chisquare
  if (fit.GetChisqY() < 0 ||
      fit.GetChisqY() > MaxChisqForLScan) return 0;

  // Check if all the ladder positions are within the range
  for (int i = 0; i < fit.GetNhit(); i++)
    if (std::abs(fit.GetYr(i)) > LadderScanRange*0.75) return 0;
  TR_DEBUG_CODE_21;

  // Scan hits among the current ladder combination
  TrHitIter itchit = it;
  itchit.nhitc = itcand.nhitc;
  if (!ScanHits(it, itchit)) return 0;
  TR_DEBUG_CODE_22;

  // Replace the best candidate if better one found
  if (itchit.nhitc > itcand.nhitc ||
      itchit.chisq[0]+itchit.chisq[1] < itcand.chisq[0]+itcand.chisq[1])
    itcand = itchit;

  return 1;
}


static int HitScanEval(TrRecon::TrHitIter &it, TrRecon::TrHitIter &cand)
{ return TrRecon::Head->HitScanEval(it, cand); }

static int HitCoordMgr(int idx, TrRecon::TrHitIter &it, int mode)
{ return TrRecon::Head->HitCoordMgr(idx, it, mode); }

int TrRecon::ScanHits(TrHitIter &itlad, TrHitIter &itcand) const
{
  // Reset the-best-candidate parameters
  itcand.chisq[0] = itcand.chisq[1] = MaxChisqAllowed;
  for (int i = 0; i < MAXLAY; i++)
    itcand.iscan[i][0] = itcand.iscan[i][1] = itcand.imult[i] = 0;

  // Define and fill iterator
  TrHitIter it = itlad;
  it.chisq[0] = it.chisq[1] = MaxChisqAllowed;
  it.psrange = ClusterScanRange;
  for (int i = 0; i < it.nlayer; i++) 
    it.iscan[i][0] = it.iscan[i][1] = it.imult[i] = 0;

  // Loop on X and Y
  for (it.side = 0; it.side <= 1; it.side++) {
    int maxpat = (it.side == 0) ? GetHitPatternFirst(1+MAXLAY-MinNhitX) 
                                : itlad.pattern+1;
    itcand.side = it.side;

    int lmask = GetHitPatternMask(itlad.pattern);

    // Loop on each pattern
    int found = 0;
    for (it.pattern = itlad.pattern; it.pattern < maxpat; it.pattern++) {
      if ((GetHitPatternMask(it.pattern)&lmask) != lmask) continue;

      // Set the order of scanning layers
      if (!SetLayerOrder(it)) continue;

      // Hit scan
      TR_DEBUG_CODE_30;
      ScanRecursive(0, it, itcand, ::HitCoordMgr, ::HitScanEval);

      // Check chisquare
      if (found = (itcand.chisq[it.side] < MaxChisqAllowed)) break;
    }
    if (!found) return 0;

    if (it.side == 0) itcand.nhitc = MAXLAY-GetHitPatternNmask(it.pattern);
    it = itcand;
  }
  return 1;
}

int TrRecon::HitCoordMgr(int idx, TrHitIter &it, int mode) const
{
  // Check layer index
  int il = it.ilay[idx];
  if (il < 0) return 0;

  // Get TkId
  int tkid = it.tkid[il];

  // Mode 1: Return number of candidates to be scanned
  if (mode == 1) {
    int nhity = GetnTrRecHits(tkid, 1);
    if (it.side == 1 || nhity <= 0) return nhity;
    return (GetnTrRecHits(tkid)-nhity)/nhity;
  }

  // Mode 2: Set multiplicity range
  if (mode == 2) {
    if (it.side == 1) return it.imult[il];

    AMSTrRecHit *hit = GetTrRecHit(tkid, it.iscan[il][0], it.iscan[il][1]);
    if (!hit) return it.imult[il];

    if (it.side == 0 && (hit->GetStatus() & YONLY)) return -1;

    // Return multiplicity range
    it.imult[il] = 0;
    return hit->GetMultiplicity()-1;
  }

  // Mode 3: Set coordinates to be evaluated
  AMSTrRecHit *hit = GetTrRecHit(tkid, it.iscan[il][0], it.iscan[il][1]);
  if (!hit) return -1;

  if (hit->GetStatus() & YONLY)
    it.coo[il][1] = hit->GetCoord(it.imult[il])[1];
  else
    it.coo[il] = hit->GetCoord(it.imult[il]);
  return 1;
}

int TrRecon::HitScanEval(const TrHitIter &it, TrHitIter &itcand) const
{
  // Evaluate the track candidate
  TrFit fit;
  for (int i = 0; i < it.nlayer && it.ilay[i] >= 0; i++) {
    int j = (it.side == 0) ? it.ilay[i] : i;
    fit.Add(it.coo[j], ErrXForScan, ErrYForScan, 1);
  }

  // Select fitting method
  if (it.side == 0) {
    if (MAGFIELDFFKEY.magstat == 1)
      fit.PolynomialFit(1, 2);
    else
      fit.LinearFit(1);
  }
  else {
    if (MAGFIELDFFKEY.magstat == 1)
      fit.SimpleFit();
    else
      fit.LinearFit(2);
  }

  // Check chisquare
  double csq = (it.side == 0) ? fit.GetChisqX() : fit.GetChisqY();
  TR_DEBUG_CODE_31;
  if (csq < 0 || csq > itcand.chisq[it.side]) return 0;

  TrHitIter ittmp = it;

  // Fill dummy multiplicity and coordinates for YONLY hits
  if (ittmp.side == 0) {
    int ndim = 1;
    if (MAGFIELDFFKEY.magstat == 1) {
      if      (fit.GetNhit() >= 4) ndim = 3;
      else if (fit.GetNhit() >= 3) ndim = 2;
      if (ndim > 2) fit.PolynomialFit(1, ndim);
    }
    for (int i = 0; i < 4; i++)
      ittmp.param[i] = (i < ndim+1) ? fit.GetParam(i) : 0;

    for (int i = 0; i < ittmp.nlayer; i++) {
      int layer = std::abs(ittmp.tkid[i])/100;
      if (TestHitPatternMask(ittmp.pattern, layer)) {
	if (EstimateXCoord(i, ittmp) < 0) return 0;
	double zl = TkDBc::Head->GetZlayer(layer);
	ittmp.coo[i][2] = zl;
	ittmp.coo[i][0] = ittmp.param[0]+ittmp.param[1]*zl
	                 +ittmp.param[2]*zl*zl+ittmp.param[3]*zl*zl*zl;
	ittmp.iscan[i][0] = GetnTrRecHits(ittmp.tkid[i], 0)-1;
      }
    }
  }

  // Keep XZ fitting parameters
  if (ittmp.side == 1)
    for (int i = 0; i < 4; i++) ittmp.param[i] = itcand.param[i];

  // Replace candidate iterator
  itcand = ittmp;
  itcand.chisq[ittmp.side] = csq;
  TR_DEBUG_CODE_32;

  return 1;
}

int TrRecon::EstimateXCoord(int il, TrHitIter &it) const
{
  int tkid  = it.tkid[il];
  int layer = std::abs(tkid)/100;
  float ln  = TkCoo::GetLadderLength (tkid);
  float lx  = TkCoo::GetLadderCenterX(tkid);
  float ly  = TkCoo::GetLadderCenterY(tkid);
  float lz  = TkDBc::Head->GetZlayer(layer);

  // Interpolated position
  float px = it.param[0]+it.param[1]*lz
            +it.param[2]*lz*lz+it.param[3]*lz*lz*lz;

  // Check if track is inside the ladder
  if (std::abs(px-lx) > ln/2) return -1;

  // Estimate sensor number and strip position
  AMSPoint gcoo(px, ly, lz);
  TkSens tks(tkid, gcoo);
  if (tks.GetStripX() < 0) {
    float sx = tks.GetSensCoo().x();
    float sg = TkDBc::Head->FindTkId(tkid)->rot.GetEl(0, 0);
    if (sx < 0) gcoo[0] -= sg*(sx-1e-3);
    if (sx > TkDBc::Head->_ssize_active[0])
      gcoo[0] -= sg*(sx-TkDBc::Head->_ssize_active[0]+1e-3);
    tks.SetGlobalCoo(gcoo);
  }
  it.imult[il] = tks.GetMultIndex();
  TR_DEBUG_CODE_40;

  return tks.GetStripX();
}

int TrRecon::BuildATrTrack(TrHitIter &itcand)
{
  if (itcand.nlayer == 0) return 0;

  VCon* cont = TRCon->GetCont("AMSTrTrack");
  if (!cont) return 0;

  if (!MAGFIELDFFKEY.magstat)
    AMSTrTrack::DefaultFitID = AMSTrTrack::kLinear;

  // Create a new track
  AMSTrTrack *track = new AMSTrTrack(itcand.pattern);

  // Loop for each layer
  int maskc = GetHitPatternMask(itcand.pattern);
  for (int i = 0; i < itcand.nlayer; i++) {
    // Get clusters from candidate map
    AMSTrRecHit *hit = GetTrRecHit(itcand.tkid[i], itcand.iscan[i][0], 
				                   itcand.iscan[i][1]);
    if (!hit) continue;

    // Mark the hit as USED
    hit->SetStatus(USED);

    // Remove hit pointer from _HitsTkIdMap
    RemoveHits(itcand.tkid[i], itcand.iscan[i][0], 0);
    RemoveHits(itcand.tkid[i], itcand.iscan[i][1], 1);
    TR_DEBUG_CODE_41;

    if (hit->GetStatus() & YONLY) {
      maskc |= (1 << (MAXLAY-hit->GetLayer()));
      hit->SetDummyX(EstimateXCoord(i, itcand));
      hit->BuildCoordinates();
    }
    hit->SetResolvedMultiplicity(itcand.imult[i]);
    track->AddHit(hit, itcand.imult[i]);
  }

  // Fill patterns
  track->SetPatterns(TrRecon::GetHitPatternIndex(maskc), itcand.pattern,
		     TrRecon::GetHitPatternIndex(maskc));
  track->Fit();
  cont->addnext(track);
  TR_DEBUG_CODE_42;

  // fill cluster angles
  for (int i = 0; i<track->GetNhits(); i++) {
    AMSTrRecHit *hit = track->GetHit(i);
    hit->GetCoord().z();
    track->GetTrFit()->Propagate(hit->GetCoord().z());
    if (hit->GetXCluster()!=0) hit->GetXCluster()->SetDxDz(track->GetTrFit()->GetDxDz()); 
    if (hit->GetXCluster()!=0) hit->GetXCluster()->SetDyDz(track->GetTrFit()->GetDyDz());
    if (hit->GetYCluster()!=0) hit->GetYCluster()->SetDxDz(track->GetTrFit()->GetDxDz());
    if (hit->GetYCluster()!=0) hit->GetYCluster()->SetDyDz(track->GetTrFit()->GetDyDz());
  }
  delete cont;
  return 1;
}

void TrRecon::InitFFKEYs(int magstat)
{
  //init the datacards
  TKGEOMFFKEY.init();
  TRMCFFKEY.init();
  TRCALIB.init();
  TRALIG.init();
  TRCLFFKEY.init();
  TRFITFFKEY.init();

  // Some settings in TrRecon
  TRCLFFKEY.ThrProb = 0;

  // Some settings in TrTrack
  TRFITFFKEY.MaxChisq[0] = 100;
  TRFITFFKEY.MaxChisq[1] = 10000;
  TRFITFFKEY.SearchRegStrLine = 100;
  TRFITFFKEY.SearchRegCircle  = 100;
  TRFITFFKEY.MaxNtrack = 1;
  TRFITFFKEY.MinNhit   = 6;
  TRFITFFKEY.ErrX = 300e-4;
  TRFITFFKEY.ErrY = 300e-4;

  TrRecon::TrDEBUG   = 0;
  TrRecon::MaxNtrack = 2;            // Default
  TrRecon::MinNhitX  = 4;            // Default
  TrRecon::MinNhitY  = 6;            // Default
  TrRecon::MinNhitXY = 4;            // Default
  TrRecon::MaxChisqAllowed  = 300;   // Default
  TrRecon::LadderScanRange  = 7.3;   // Default
  TrRecon::ClusterScanRange = 0.5;   // Default
  TrRecon::MaxChisqForLScan = 2.2;   // Default

  MAGFIELDFFKEY.BZCorr  = 1;
  MAGFIELDFFKEY.magstat = magstat;
  if (!magstat) AMSTrTrack::DefaultFitID = AMSTrTrack::kLinear;
}

int TrRecon::ReadTrField(const char *fname, int magstat)
{
  MAGFIELDFFKEY.BZCorr  = 1;
  MAGFIELDFFKEY.magstat = magstat;
  if (!magstat) AMSTrTrack::DefaultFitID = AMSTrTrack::kLinear;
  return TrField::GetPtr()->Read(fname);
}

void TrRecon::GetTrField(float *pos, float *bf)
{
  TrField::GetPtr()->GuFld(pos, bf);
}

void TrRecon::GetTkFld(float *pos, float *hxy)
{
  TrField::GetPtr()->TkFld(pos, (float(*)[3])hxy);
}

///// For debug --------------------------------------


//=========================================================================
// #####################################
// #             MC HITS               #
// #####################################

extern "C" double rnormx();


void TrRecon::sitkhits(int idsoft, float vect[],
			   float edep, float step, int itra)
{
  if (edep <= 0) edep = 1e-4;

  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
		TkDBc::Head->_ssize_active[1]/2,
		TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
//int sensor = abs(idsoft)/10000;
  int tkid   = abs(idsoft)%1000;
  int ss     = abs(idsoft)%10000-tkid;
  if(!ss) tkid*=-1;
  // Convert global coo into sensor local coo
  // The origin is the first strip of the sensor
  TkSens tksa(tkid, ppa);
  TkSens tksb(tkid, ppb);
  AMSPoint pa = tksa.GetSensCoo(); pa[2] += size[2];
  AMSPoint pb = tksb.GetSensCoo(); pb[2] += size[2];

  // Range check
  for (int i = 0; i < 3; i++) {
    if (pa[i] < 0) pa[i] = 0;
    if (pb[i] < 0) pb[i] = 0;
    if (pa[i] > 2*size[i]) pa[i] = 2*size[i];
    if (pb[i] > 2*size[i]) pb[i] = 2*size[i];
  }
  
  // Create a new object
  VCon* aa=TRCon->GetCont("AMSTrMCCluster");
  if(aa)
    aa->addnext(  new AMSTrMCCluster(idsoft, pa, pb, pgl,pmom,edep , itra));
  if(aa) delete aa;
}



void TrRecon::sitkdigi()
{
// temporary: make sure to TrCalDB saved
  static int savecal = 0;
  if (!savecal) {
    savecal = 1;
    TrCalDB::Head->Write();
  }
// temporary: make sure to TrCalDB saved

  //Get the pointer to the TrMCCluster container
  VCon* cont=TRCon->GetCont("AMSTrMCCluster");
  if(!cont){
    printf("TrSim::sitkdigi()  Cant Find AMSMCCluster Container Digitization is Impossible !!!\n");
    return ;
  }
  if(cont->getnelem()==0){
//    printf("TrSim::sitkdigi()  AMSTrMCCluster Container is Empty  Digitzation is Impossible !!!\n");
    return ;
  }else
;//    printf("TrSim::sitkdigi()  AMSTrMCCluster Container has %d elements \n",cont->getnelem());
  //Create the map of TrMCClusters
  TrMap<AMSTrMCCluster> MCClMap;
  
  for (int ii=0;ii<cont->getnelem();ii++){
    AMSTrMCCluster* clu=(AMSTrMCCluster*)cont->getelem(ii);
    if (clu)MCClMap.Add(clu);
    //clu->_printEl(cerr); 
  }

  //LOOP ON ALL THE LADDERS
  for (int lay=1;lay<=trconst::nlays;lay++){
    for (int slot=1;slot<=trconst::maxlad;slot++){
      for(int side=-1;side<2;side=side+2){
	int tkid=(lay*100+slot)*side;
	if(!TkDBc::Head->FindTkId(tkid)) continue  ;
	
	TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
	if (!tcal) {
	  std::cerr << "ERROR(1) tcal not found: " << tkid << std::endl;
	  continue;
	}
	//	printf("Ladder %+03d Has been selected\n",tkid);
	geant* buf=new geant[1024];
	//Expand the noise
	for(int ii=0;ii<1024;ii++){
	  if(tcal->Status(ii)&TrLadCal::dead)
	    buf[ii]=-1000;
	  else
	    buf[ii]= tcal->Sigma(ii)*rnormx();
	  // printf("%4d: %6.2f ",ii, buf[ii]);
	}
	//expand the TrMCClusters
	//	printf("\n");

	for( int icl=0;icl<MCClMap.GetNelem(tkid);icl++){
	  AMSTrMCCluster* cl=MCClMap.GetElem(tkid,icl);

	  int addK=cl->GetAdd(0);
	  // printf("ClusterK: ");
	  for (int jj=0;jj<cl->GetSize(0);jj++){
	    buf[addK+jj] += cl->GetSignal(jj,0)*10.;
	    //  printf(" %4d: %6.2f",addK+jj,cl->GetSignal(jj,0));
	  }
	  //	   printf("\n");
	  int addS=cl->GetAdd(1);
	  //printf("ClusterS: ");
	  for (int jj=0;jj<cl->GetSize(1);jj++){
	    buf[addS+jj] += cl->GetSignal(jj,1)*10.;
	    // printf(" %4d: %6.2f",addS+jj,cl->GetSignal(jj,1));
	  }
	  //printf("\n");
	  //	  printf("Ladder %+03d Has been selected\n",tkid);
	}
	
	// Do clusterization the DSP way

	DSP_Clusterize(tkid,buf);

	delete [] buf;
      }

    }
  }
  if(cont) delete cont;
  return;
}


void TrRecon::DSP_Clusterize(int tkid,float *buf){

  //Get the pointer to the TrMCCluster container
  VCon* cont=TRCon->GetCont("AMSTrRawCluster");
  if(!cont){
    printf("TrRecon::DSP_Clusterize  Cant Find AMSTrRawCluster Container Digitization is Impossible !!!\n");
    return ;
  }


  TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
  if (!tcal) {
    std::cerr << "TrRecon::DSP_Clusterize ERROR(1) tcal not found: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  
  if(!buf){
    std::cerr << "TrRecon::DSP_Clusterize Buffer not available: " << tkid << std::endl;
    std::cerr << "Clusterization aborted for ladder " << tkid << std::endl;
    return;
  }
  int used[1024];
  memset(used,0,1024*sizeof(used[0]));

  for(int ii=0;ii<639;ii++){
//     printf("%4d: used: %d st: %4d S: %6.2f N: %6.2f  SN: %6.2f \n",
// 	   ii,used[ii],tcal->Status(ii),buf[ii],tcal->Sigma(ii),buf[ii]/tcal->Sigma(ii)
// 	   );
    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/tcal->Sigma(ii)<=TRMCFFKEY.th1sim[0]) continue;
    if(buf[ii+1]/tcal->Sigma(ii+1)>buf[ii]/tcal->Sigma(ii) ) continue;
    //SEED found
    //    printf("SEED %d \n",tkid);
    int seed=ii;
    used[ii]=1;
    int left=0;
    for (int jj=seed-1;jj>=0;jj--)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0])&& used[jj]==0){
	used[jj]=1;
	left=jj;
      }
    
    int right=639;
    for (int jj=seed+1;jj<640;jj++)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[0]) && used[jj]==0){
	used[jj]=1;
	right=jj;
      }
    if(left>0) left--;
    if(right<639) right++;
    //printf("LEFT:  %3d  RIGHT:  %3d \n",left,right);
    //    printf("Adding Raw Clus: %d\n",tkid);
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left+1, &(buf[left])));
  }


  for(int ii=640;ii<1024;ii++){

    if(used[ii]!=0) continue;
    if(tcal->Status(ii)!=0) continue;
    if(buf[ii]/tcal->Sigma(ii)<=TRMCFFKEY.th1sim[1]) continue;
    if(buf[ii+1]/tcal->Sigma(ii+1)>buf[ii]/tcal->Sigma(ii) ) continue;
    //SEED found
    int seed=ii;
    used[ii]=1;
    int left=640;
    for (int jj=seed-1;jj>=640;jj--)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	left=jj;
      }    
    int right=1023;
    for (int jj=seed+1;jj<1024;jj++)
      if(((buf[jj]/tcal->Sigma(jj))>TRMCFFKEY.th2sim[1])&& used[jj]==0){
	used[jj]=1;
	right=jj;
      }
    
    cont->addnext(new AMSTrRawCluster(tkid, left, right-left+1, &(buf[left])));
  }
  if(cont) delete cont;

}


/*
Tracker hit pattern list

PatID Nhits   Pattern  Sw Attrib
    0     8  11111111   1  12221

    1     7  10111111   1  11221
    2     7  11101111   1  12121
    3     7  11111011   1  12211
    4     7  11011111   1  11221
    5     7  11110111   1  12121
    6     7  11111101   1  12211
    7     7  01111111   1  02221
    8     7  11111110   1  12220

    9     6  10101111   1  11121
   10     6  10111011   1  11211
   11     6  10011111   0  10221
   12     6  10110111   1  11121
   13     6  10111101   1  11211
   14     6  00111111   1  01221
   15     6  10111110   1  11220
   16     6  11101011   1  12111
   17     6  11001111   1  11121
   18     6  11100111   0  12021
   19     6  11101101   1  12111
   20     6  01101111   1  02121
   21     6  11101110   1  12120
   22     6  11011011   1  11211
   23     6  11110011   1  12111
   24     6  11111001   0  12201
   25     6  01111011   1  02211
   26     6  11111010   1  12210
   27     6  11010111   1  11121
   28     6  11011101   1  11211
   29     6  01011111   1  01221
   30     6  11011110   1  11220
   31     6  11110101   1  12111
   32     6  01110111   1  02121
   33     6  11110110   1  12120
   34     6  01111101   1  02211
   35     6  11111100   1  12210
   36     6  01111110   0  02220

   37     5  10101011   1  11111
   38     5  10001111   0  10121
   39     5  10100111   0  11021
   40     5  10101101   1  11111
   41     5  00101111   0  01121
   42     5  10101110   0  11120
   43     5  10011011   0  10211
   44     5  10110011   1  11111
   45     5  10111001   0  11201
   46     5  00111011   0  01211
   47     5  10111010   0  11210
   48     5  10010111   0  10121
   49     5  10011101   0  10211
   50     5  00011111   0  00221
   51     5  10011110   0  10220
   52     5  10110101   1  11111
   53     5  00110111   0  01121
   54     5  10110110   0  11120
   55     5  00111101   0  01211
   56     5  10111100   0  11210
   57     5  00111110   0  01220
   58     5  11001011   1  11111
   59     5  11100011   0  12011
   60     5  11101001   0  12101
   61     5  01101011   0  02111
   62     5  11101010   0  12110
   63     5  11000111   0  11021
   64     5  11001101   1  11111
   65     5  01001111   0  01121
   66     5  11001110   0  11120
   67     5  11100101   0  12011
   68     5  01100111   0  02021
   69     5  11100110   0  12020
   70     5  01101101   0  02111
   71     5  11101100   0  12110
   72     5  01101110   0  02120
   73     5  11010011   1  11111
   74     5  11011001   0  11201
   75     5  01011011   0  01211
   76     5  11011010   0  11210
   77     5  11110001   0  12101
   78     5  01110011   0  02111
   79     5  11110010   0  12110
   80     5  01111001   0  02201
   81     5  11111000   0  12200
   82     5  01111010   0  02210
   83     5  11010101   1  11111
   84     5  01010111   0  01121
   85     5  11010110   0  11120
   86     5  01011101   0  01211
   87     5  11011100   0  11210
   88     5  01011110   0  01220
   89     5  01110101   0  02111
   90     5  11110100   0  12110
   91     5  01110110   0  02120
   92     5  01111100   0  02210

   93     4  10001011   0  10111
   94     4  10100011   0  11011
   95     4  10101001   0  11101
   96     4  00101011   0  01111
   97     4  10101010   0  11110
   98     4  10000111   0  10021
   99     4  10001101   0  10111
  100     4  00001111   0  00121
  101     4  10001110   0  10120
  102     4  10100101   0  11011
  103     4  00100111   0  01021
  104     4  10100110   0  11020
  105     4  00101101   0  01111
  106     4  10101100   0  11110
  107     4  00101110   0  01120
  108     4  10010011   0  10111
  109     4  10011001   0  10201
  110     4  00011011   0  00211
  111     4  10011010   0  10210
  112     4  10110001   0  11101
  113     4  00110011   0  01111
  114     4  10110010   0  11110
  115     4  00111001   0  01201
  116     4  10111000   0  11200
  117     4  00111010   0  01210
  118     4  10010101   0  10111
  119     4  00010111   0  00121
  120     4  10010110   0  10120
  121     4  00011101   0  00211
  122     4  10011100   0  10210
  123     4  00011110   0  00220
  124     4  00110101   0  01111
  125     4  10110100   0  11110
  126     4  00110110   0  01120
  127     4  00111100   0  01210
  128     4  11000011   0  11011
  129     4  11001001   0  11101
  130     4  01001011   0  01111
  131     4  11001010   0  11110
  132     4  11100001   0  12001
  133     4  01100011   0  02011
  134     4  11100010   0  12010
  135     4  01101001   0  02101
  136     4  11101000   0  12100
  137     4  01101010   0  02110
  138     4  11000101   0  11011
  139     4  01000111   0  01021
  140     4  11000110   0  11020
  141     4  01001101   0  01111
  142     4  11001100   0  11110
  143     4  01001110   0  01120
  144     4  01100101   0  02011
  145     4  11100100   0  12010
  146     4  01100110   0  02020
  147     4  01101100   0  02110
  148     4  11010001   0  11101
  149     4  01010011   0  01111
  150     4  11010010   0  11110
  151     4  01011001   0  01201
  152     4  11011000   0  11200
  153     4  01011010   0  01210
  154     4  01110001   0  02101
  155     4  11110000   0  12100
  156     4  01110010   0  02110
  157     4  01111000   0  02200
  158     4  01010101   0  01111
  159     4  11010100   0  11110
  160     4  01010110   0  01120
  161     4  01011100   0  01210
  162     4  01110100   0  02110

  163     3  10000011   0  10011
  164     3  10001001   0  10101
  165     3  00001011   0  00111
  166     3  10001010   0  10110
  167     3  10100001   0  11001
  168     3  00100011   0  01011
  169     3  10100010   0  11010
  170     3  00101001   0  01101
  171     3  10101000   0  11100
  172     3  00101010   0  01110
  173     3  10000101   0  10011
  174     3  00000111   0  00021
  175     3  10000110   0  10020
  176     3  00001101   0  00111
  177     3  10001100   0  10110
  178     3  00001110   0  00120
  179     3  00100101   0  01011
  180     3  10100100   0  11010
  181     3  00100110   0  01020
  182     3  00101100   0  01110
  183     3  10010001   0  10101
  184     3  00010011   0  00111
  185     3  10010010   0  10110
  186     3  00011001   0  00201
  187     3  10011000   0  10200
  188     3  00011010   0  00210
  189     3  00110001   0  01101
  190     3  10110000   0  11100
  191     3  00110010   0  01110
  192     3  00111000   0  01200
  193     3  00010101   0  00111
  194     3  10010100   0  10110
  195     3  00010110   0  00120
  196     3  00011100   0  00210
  197     3  00110100   0  01110
  198     3  11000001   0  11001
  199     3  01000011   0  01011
  200     3  11000010   0  11010
  201     3  01001001   0  01101
  202     3  11001000   0  11100
  203     3  01001010   0  01110
  204     3  01100001   0  02001
  205     3  11100000   0  12000
  206     3  01100010   0  02010
  207     3  01101000   0  02100
  208     3  01000101   0  01011
  209     3  11000100   0  11010
  210     3  01000110   0  01020
  211     3  01001100   0  01110
  212     3  01100100   0  02010
  213     3  01010001   0  01101
  214     3  11010000   0  11100
  215     3  01010010   0  01110
  216     3  01011000   0  01200
  217     3  01110000   0  02100
  218     3  01010100   0  01110
*/
