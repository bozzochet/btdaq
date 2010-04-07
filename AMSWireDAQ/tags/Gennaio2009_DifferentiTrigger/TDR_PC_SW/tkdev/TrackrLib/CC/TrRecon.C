/// $Id: TrRecon.C,v 1.12 2008/10/23 10:12:39 zuccon Exp $ 

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
/// $Date: 2008/10/23 10:12:39 $
///
/// $Revision: 1.12 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecon.h"
#include "TrMap.h"
#include "tkdcards.h"
#include "TrField.h"

#include "TrTrack.h"
#include "TkSens.h"
#include "TrMCCluster.h"

double TrRecon::_csqmin;
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
float       TrRecon::ThrSeed[2]  = {4.00,4.00};
float       TrRecon::ThrNeig[2]  = {1.00,1.00};
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
float  TrRecon::ThrProb    = 0.00001;

/*
// tracks
float TrRecon::ErrX = 0.0100;
float TrRecon::ErrY = 0.0100;
float TrRecon::ErrZ = 0.0100;
int   TrRecon::MaxNhits = 5;
float TrRecon::MaxChisq[2];
// int   TrRecon::MaxNtrack;

vector<int> TrRecon::_Patterns;
vector<AMSTrRecHit*> TrRecon::_trhits;
TrFit* TrRecon::_trfit = 0;
*/


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
  while (lad!=RawMap.LadMap.end()){
    ClearBuffer();
    //expand clusters in the buffer
    for (int iclus=0; iclus<lad->second.size(); iclus++) 
      ExpandClusterInBuffer( lad->second.at(iclus) );
    
    int TkId=lad->first;
    int layer = abs(TkId/100);
    int ncls=BuildTrClustersInSubBuffer(TkId,0,640,0);      // S
    //printf("TkId: %+03d Found %d cluster on S\n",TkId,ncls);
    //printf("TrCluster container has: %d elements\n",cont2->getnelem());
    if ( (layer==1)||(layer==8) ) { 
      BuildTrClustersInSubBuffer(TkId,640,1024,1); // K7
    }
    else { 
      BuildTrClustersInSubBuffer(TkId,640,832,0);  // K5 - sensor1
      BuildTrClustersInSubBuffer(TkId,832,1024,0); // K5 - sensor2 
    }
    lad++;
  }

  if(cont) delete cont;
  if(cont2) delete cont2;
  return 0;
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
    cluster->BuildCoordinates();
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
  float n = cln->GetTotSignal("AGLN");
  float p = clp->GetTotSignal("AGLN");
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
  return (side==0) ? _ClusterTkIdMap[tkid].first.at(iclus) : _ClusterTkIdMap[tkid].second.at(iclus); 
}

//================================================================================================
//================================================================================================

int TrRecon::BuildTrRecHits(int rebuild) {

  
  //Get the pointer to the TrCluster container
  VCon* cont=TRCon->GetCont("AMSTrCluster");
  if(!cont){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrCluster Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    printf("TrRecon::BuildTrRecHit  AMSTrCluster Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }

  VCon* cont2=TRCon->GetCont("AMSTrRecHit");
  if(!cont2){
    printf("TrRecon::BuildTrRecHit  Cant Find AMSTrRecHit Container Reconstruction is Impossible !!!\n");
    return -1;
  }
 
  BuildClusterTkIdMap();

  int ntrrechits = 0;
  int nactiveladders = _ClusterTkIdMap.size();
  for ( ITClusterMap lad=_ClusterTkIdMap.begin();lad!=_ClusterTkIdMap.end();lad++){
    int tkid = lad->first;
    int nx   = lad->second.first.size();
    int ny   = lad->second.second.size();

    /*
    // "ghost" hits
    for (int ix=0; ix<nx; ix++) {
    if (ThrProb>0.) continue;  
    AMSTrRecHit* hit = new AMSTrRecHit(tkid,GetTrCluster(tkid,0,ix),0,-1.,0.,1);
    GetEvent()->AddTrRecHit(hit);     
    ntrrechits++;
    }
    for (int iy=0; iy<ny; iy++) {
    if (ThrProb>0.) continue;  
       AMSTrRecHit* hit = new AMSTrRecHit(tkid,0,GetTrCluster(tkid,1,iy),1.,0.,1);
       GetEvent()->AddTrRecHit(hit);     
       ntrrechits++;
       }
    */

    // all combinations
    for (int ix=0; ix<nx; ix++) {
      for (int iy=0; iy<ny; iy++) {
	AMSTrCluster* clX = lad->second.first.at(ix);
	AMSTrCluster* clY = lad->second.second.at(iy);
	float     corr = GetCorrelation(clX,clY);
	float     prob = GetProbToBeCorrelated(clX,clY);
	// the hit must have a minimum required probability (???)
	if (prob<ThrProb) continue;  
	AMSTrRecHit* hit = new AMSTrRecHit(tkid,clX,clY,corr,prob,0);
	cont2->addnext(hit);
	//	hit->Print();
	ntrrechits++;
      }
    }    
  }
  if(cont) delete cont;
  if(cont2) delete cont2;

  return ntrrechits;
} 


/*

////////////////////
// --- TRACKS --- //
////////////////////

int TrRecon::BuildPatterns() {
  AddLayersCombinationPatterns(8);
  AddLayersCombinationPatterns(7);
  AddLayersCombinationPatterns(6);
  AddLayersCombinationPatterns(5);
  int  ii = 0;
  while (!(ii==(int)_Patterns.size())) {
    if (!IsAllowed(_Patterns.at(ii))) _Patterns.erase(_Patterns.begin()+ii); 
    else ii++;
  }
  return _Patterns.size();
}

void TrRecon::AddLayersCombinationPatterns(int n, int i, int mask) {
  int imask[8] = { 6, 4, 2, 5, 3, 1, 7, 0 };
  if (n > 0) {
    for (int j=i; j<8; j++) AddLayersCombinationPatterns(n-1, j+1, mask | (1<<imask[j]));
    return;
  }
  _Patterns.push_back(mask); 
}

bool TrRecon::IsAllowed(int pattern) {
  // bit definition
  enum { B0 = 1<<0, B1 = 1<<1, B2 = 1<<2, B3 = 1<<3,
         B4 = 1<<4, B5 = 1<<5, B6 = 1<<6, B7 = 1<<7 };
  int nlayers=0;
  for (int j=0; j<8; j++) if (pattern&(1<<j)) nlayers++;
  // a minimum of 5 points per track
  if ( nlayers<5 )                             return false; 
  // at last 1 layer for each plane
  if ( ( !(pattern&B1) && !(pattern&B2) ) ||
       ( !(pattern&B3) && !(pattern&B4) ) ||
       ( !(pattern&B5) && !(pattern&B6) ) )    return false;
  // both the external planes 
  if ( (!(pattern&B0) || !(pattern&B7)) ) return false; 
  // both the external planes for the 5 point tracks 
  // if ( (!(pattern&B0) || !(pattern&B7)) && nlayers==5) return false; 
  return true;
}
*/

void TrRecon::BuildRecHitLayerMap() {
  _RecHitLayerMap.clear();
  // loop on TrRecHit
  VCon* cont4=TRCon->GetCont("AMSTrRecHit");
  for (int ii=0;ii<cont4->getnelem();ii++){
    AMSTrRecHit* hit = (AMSTrRecHit*)cont4->getelem(ii);
    int lay = hit->GetLayer();
    _RecHitLayerMap[lay].push_back(hit);
  }
  if(cont4) delete cont4;
  return;
}


/*
// COMBINATION ALGORITHM
// layer 1, 4 hits --> 0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3
// layer 2, 2 hits --> 0 0 0 0  1 1 1 1  0 0 0 0  1 1 1 1  0 0 0 0  1 1 1 1
// layer 3, 3 hits --> 0 0 0 0  0 0 0 0  1 1 1 1  1 1 1 1  2 2 2 2  2 2 2 2
// index           --> 0 1 2 3  4 5 6 7  8 91011 12131415 16171819 20212223
// index%4         --> 0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3  0 1 2 3
// index/4         --> 0 0 0 0  1 1 1 1  2 2 2 2  3 3 3 3  4 4 4 4  5 5 5 5 
// (index/4)%2     --> 0 0 0 0  1 1 1 1  0 0 0 0  1 1 1 1  0 0 0 0  1 1 1 1
// (index/(4*2))   --> 0 0 0 0  0 0 0 0  1 1 1 1  1 1 1 1  2 2 2 2  2 2 2 2
// combinations = 4*2*3 = 24 = ntracks
int TrRecon::BuildTrTracks() {  

  BuildRecHitLayerMap();

  int ncurrentlayers = 0;
  int ntracksfounded = 0;
  int ntracksgoodY   = 0;
  int ntracksfitted  = 0;
  if (_trfit==0) _trfit = new TrFit();
  vector<int> multiplicity;
  
  // A. loop on all the possible patterns
  int npatterns = _Patterns.size();
  for (int pp=0; pp<npatterns; pp++) {
    int pattern = _Patterns.at(pp);

    // calculation of the possible hits combination in the pattern
    int nlayers = 0;
    int ntracks = 1;
    for (int ll=0; ll<8; ll++) {
      if (!(pattern&(1<<ll))) continue;
      int layer = ll+1;
      int nhits = GetnTrRecHits(layer);
      // check number of hits per plane overflow
      if (nhits>MaxNhits) nhits = 0; 
      ntracks = ntracks*nhits;
      nlayers++;
    }
    // cout << "pattern: " << pattern << " nlayers: " << nlayers << " nhitcombinations: " << ntracks << endl;

    // if a saved track has a number of points greater than 
    // the used in the pattern the pattern is skipped
    if (nlayers<ncurrentlayers) continue;

    // B. loop on all the possible hits combination in the pattern
    for (int itrack=0; itrack<ntracks; itrack++) {
      // clear the hit combination list
      _trhits.clear();
      if (_trfit==0) _trfit = new TrFit();
      int previous = 1;
      int totmult = 1;
      for (int ll=0; ll<8; ll++) {
	if (!(pattern&(1<<ll))) continue;
	int layer = ll+1;
	// get the hit index for the current track
	int nhits = GetnTrRecHits(layer);  
	int ihit  = (itrack/previous)%nhits;
	AMSTrRecHit* hit = GetTrRecHit(layer,ihit);
	// set the current hit in the current combination
	_trhits.push_back(hit);
	previous = previous*nhits; 
	// calculate the total multiplicity of the current hit combination
	totmult = totmult*hit->GetMultiplicity();
      }

      // cout << " -------- totmultiplicity: " << totmult << endl; 
      YRoadCheck();

      // a fast check on the Y side road 
      if (!YRoadCheck()) continue;
      ntracksgoodY++;
      
      // C. loop on all the possible multiplicity for a given hit combination 
      for (int imult=0; imult<totmult; imult++) {
	_trfit->Clear();
	multiplicity.clear();
	int previousmult = 1;
	for (int ll=0; ll<nlayers; ll++) {
	  int hitmult  = _trhits.at(ll)->GetMultiplicity(); 
	  int ihitmult = (imult/previousmult)%hitmult;
	  multiplicity.push_back(ihitmult);
	  AMSPoint point = _trhits.at(ll)->GetCoord(ihitmult);
	  // SET THE TRACK
	  _trfit->Add(point.x(),point.y(),point.z(),ErrX,ErrY,ErrZ);
	  previousmult = previousmult*hitmult;
	}
        // Check on the track
	if (!ExternalPlanesRoadCheck()) continue;
	if (!InnerTrackerCheck()) continue;
	ntracksfitted++;
	
	// Add the track
	if (_trfit->Fit()<0. ) continue;
	if (_trfit->GetChisq()>1000.) continue;
	TrTrack* track = new TrTrack(pattern,nlayers,_trhits,multiplicity,_trfit);
	GetEvent()->AddTrTrack(track);
	ncurrentlayers = nlayers;
	// prenotazione degli hit????
	// ???
	ntracksfounded++;
      }
    }
  }
  // cout << "nTracks goodY:   " << ntracksgoodY   << endl;
  // cout << "nTracks fitted:  " << ntracksfitted  << endl;
  // cout << "nTracks founded: " << ntracksfounded << endl;
  return ntracksfounded;
}


bool TrRecon::YRoadCheck() {
  bool check = true;
  int  nhits = _trhits.size();
  AMSPoint first = _trhits.at(0)->GetCoord();
  AMSPoint last  = _trhits.at(nhits-1)->GetCoord();
  float z0 = first.z(); 
  float z1 = last.z();
  if (fabs(z0-z1)<100.) return check;
  // check of the hits in the road (5 cm)
  float y0 = first.y(); 
  float y1 = last.y(); 
  float by = (y0-y1)/(z0-z1); 
  float ay = y0 - by*z0; 
  // check on the interior planes
  for (int hh=1; hh<nhits-1; hh++) {
    AMSPoint coord = _trhits.at(hh)->GetCoord();
    float y  = coord.y();
    float z  = coord.z();
    float dy = fabs(ay + by*z - y);
    // cout << hh << " " << y0 << " " << y << " " << y1 << " --> " << dy << endl;
    if (dy>5.) { check = false; break; }
  }
  return check;
}


bool TrRecon::ExternalPlanesRoadCheck() {
  bool check = true;
  int  nhits = _trfit->GetNhit();
  // the two external planes
  float z0 = _trfit->GetZ(0); float z1 = _trfit->GetZ(nhits-1);
  if (fabs(z0-z1)<100.) return check;
  // check of the hits in the road (5 cm)
  float x0 = _trfit->GetX(0); float x1 = _trfit->GetX(nhits-1); 
  float y0 = _trfit->GetY(0); float y1 = _trfit->GetY(nhits-1); 
  float bx = (x0-x1)/(z0-z1); float ax = x0 - bx*z0;   
  float by = (y0-y1)/(z0-z1); float ay = y0 - by*z0; 
  // cout << x0 << " " << y0 << " " << z0 << " | " << x1 << " " << y1 << " " << z1 << endl;
  // check on the interior planes
  for (int hh=1; hh<nhits-1; hh++) {
    float x = _trfit->GetX(hh);
    float y = _trfit->GetY(hh);
    float z = _trfit->GetZ(hh);
    float dx = fabs(ax + bx*z - x);
    float dy = fabs(ay + by*z - y);
    if ( (dx>5.)||(dy>5.) ) { check = false; break; }
  }
  return check;
}


bool TrRecon::InnerTrackerCheck() {
  // two hit on the two layers of a single plane (2&3, 4&5, 6&7) 
  // must to be similar in 5 cm (more than 70 degrees)
  bool check = true;
  int  nhits = _trfit->GetNhit();
  for (int hh=0; hh<nhits-1; hh++) {
    // two hits on the two layers of a single plane (DeltaZ<5.cm)
    if (fabs(_trfit->GetZ(hh)-_trfit->GetZ(hh+1))>5.) continue;
    // probably wrong tracks (too inclinated) 
    if (fabs(_trfit->GetX(hh)-_trfit->GetX(hh+1))>5.) check = false;        
  }
  return check;
}


*/


int TrRecon::BuildTrTracks(int rebuild){

  int ntrack = 0;
  int maxtr  = TRFITFFKEY.MaxNtrack;

 //Get the pointer to the TrRecHit container
  VCon* cont=TRCon->GetCont("AMSTrRecHit");
  if(!cont){
    printf("TrRecon::BuildTrTrack  Cant Find AMSTrRecHit Container Reconstruction is Impossible !!!\n");
    return -1;
  }
  if(cont->getnelem()==0){
    printf("TrRecon::BuildTrTrack  AMSTrRecHit Container is Empty  Reconstruction is Impossible !!!\n");
    return -1;
  }

  VCon* cont2=TRCon->GetCont("AMSTrTrack");
  if(!cont2){
    printf("TrRecon::BuildTrTrack  Cant Find AMSTrTrack Container Reconstruction is Impossible !!!\n");
    return -1;
  }

  //build a map for the hits layer

  BuildRecHitLayerMap();




  _ptest = new AMSTrTrack();

  while (ntrack < maxtr) {
    _csqmin = TRFITFFKEY.MaxChisq[0];
    _ptrack = 0;

    // Scan pattern
    _ptest->_Pattern = -1;
    
    for (int i = 0; i < 3 && !_ptrack; i++) ScanPat(i); // Temporally

    // If a track is found
    if (_ptrack) {
      _ptrack->Fit(0);
      cont2->addnext(_ptrack);

      // Mark hits as USED
      int nhits = _ptrack->GetNhits();
      for (int i = 0; i < nhits; i++) {
	AMSTrRecHit *phit = _ptrack->GetHit(i);
	if (phit->_status&USED) phit->_status |= AMBIG;
	else phit->_status |= USED;

	if (phit->_imult < 0) phit->_imult = _ptrack->_iMult[i];
      }

      ntrack++;
      _ptrack = 0;
    }

    // Break if no track is found
    else break;
  }
  if(cont) delete cont;
  if(cont2) delete cont2;

  return ntrack;
}

void  TrRecon::ScanPat(int n, int i, int mask)
{
  enum { B0 = 1<<0, B1 = 1<<1, B2 = 1<<2, B3 = 1<<3,
	 B4 = 1<<4, B5 = 1<<5, B6 = 1<<6, B7 = 1<<7 };


  // Scan pattern recursively
  if (n > 0) {
    for (int j = i; j < trconst::maxlay; j++)
      ScanPat(n-1, j+1, mask | (1<<j));
    return;
  }

  // Make a pattern vector: layer[]
  int layer[trconst::maxlay], nlyr = 0;
  for (int j = 0; j < trconst::maxlay; j++)
    if (!(mask & (1<<j))) layer[nlyr++] = j;

  // Increment pattern ID
  _ptest->_Pattern++;

  // Return if the pattern is not allowed
  // Not allowed pattern
  // Nly=6:     (2&3) (4&5) (6&7)
  // Nly=5: 1 | (2&3) (4&5) (6&7) | 8

  if ( (mask & B0 && mask & B7) ) return;
  if ( (mask & B1 && mask & B2) || 
       (mask & B3 && mask & B4) ||
       (mask & B5 && mask & B6) ) return;
  if ( (mask & B0 || mask & B7) && nlyr == trconst::maxlay-3) return;

  // Scan hits
  int iscan[trconst::maxlay], imult[trconst::maxlay];
  _ptest->_Nhits = nlyr;
  ScanHit(layer, iscan, imult);
}




void TrRecon::ScanHit(int *layer, int *iscan, int *imult, int i)
{
  // Clear hits vector
  if (i == 0) for (int j = 0; j < trconst::maxlay; j++) _ptest->_Hits[j] = 0;

  int nhits = _ptest->_Nhits;

  // Evaluate a track
  if (i == nhits) {

    // Ambiguity check
    if (layer[0] != 0 && layer[nhits-1] != trconst::maxlay-1) return;

    // Fit and check chisquare
    if (_ptest->SimpleFit() < _csqmin && _ptest->Fit(0) < TRFITFFKEY.MaxChisq[1]) {

      // Replace the track candidate
      if (!_ptrack) _ptrack = new AMSTrTrack;
      *_ptrack = *_ptest;
      _csqmin = _ptest->GetChisqWoMS();
    }

    return;
  }

  // Loop for each hit in the layer
  
 //Get the pointer to the TrRecHit container
  VCon* cont=TRCon->GetCont("AMSTrRecHit");

  int nscan = cont->getnelem();
  for (iscan[i] = 0; iscan[i] < nscan; iscan[i]++) {

    // Get and check a hit
    AMSTrRecHit *hit = (AMSTrRecHit*)cont->getelem(iscan[i]);
    if (hit->GetLayer() != layer[i]+1) continue;
    if ((hit->_status&USED) && hit->GetLayer() > 2) continue;
    _ptest->_Hits[i] = hit;
    _ptest->_iMult[i] = 0;

    // Optimization only for non-B run: Check with only Y-side
    if (MAGFIELDFFKEY.magstat == 0 && i >= 2) {
      double err[3] = { 0, 0.03, 0.03 };

      _ptest->_Nhits = i+1;
      double csq = _ptest->SimpleFit(err);
      _ptest->_Nhits = nhits;
      if (csq > TRFITFFKEY.MaxChisq[0]) continue;
    }

    int nmult = hit->GetMultiplicity(), im0 = 0;

    // Fix multiplicity index if already determined
    if (hit->_imult >= 0) { 
      im0 = hit->_imult; 
      nmult = im0+1; 
    }

    // Tighten multiplicity range if the previous layer is on the same plane
    else if (i > 0 && 
	     (layer[i-1] == 1 || layer[i-1] == 3 || layer[i-1] == 5) &&
	     layer[i-1]+1 == layer[i]) {
      im0   = std::max(imult[i-1]-1, 0);
      nmult = std::min(imult[i-1]+2, hit->GetMultiplicity());
    }

    // Loop for each multiplicity
    for (imult[i] = im0; imult[i] < nmult; imult[i]++) {

      _ptest->_iMult[i] = imult[i];

      // Optimization only for non-B run: Check with only X-side
      if (MAGFIELDFFKEY.magstat == 0 && i >= 2) {
	double err[3] = { 0.03, 0, 0.03 };

	_ptest->_Nhits = i+1;
	double csq = _ptest->SimpleFit(err);
	_ptest->_Nhits = nhits;
	if (csq > TRFITFFKEY.MaxChisq[0]) continue;
      }

      else {

      // Make a quick check parameter
      if (i == 2) {
	AMSPoint x0 = _ptest->GetHit(0)->GetCoord(imult[0]);
	AMSPoint x2 = _ptest->GetHit(2)->GetCoord(imult[2]);

	_ptest->_Par[0][0] = (x2[0]-x0[0])/(x2[2]-x0[2]);
	_ptest->_Par[1][0] = (x2[1]-x0[1])/(x2[2]-x0[2]);
	_ptest->_Par[0][1] =  x0[0]-_ptest->_Par[0][0]*x0[2];
	_ptest->_Par[1][1] =  x0[1]-_ptest->_Par[1][0]*x0[2];
	_ptest->_Par[0][2] = sqrt(1+_ptest->_Par[0][0]*_ptest->_Par[0][0]);
	_ptest->_Par[1][2] = sqrt(1+_ptest->_Par[1][0]*_ptest->_Par[1][0]);
      }

      // Quick check
      if (i >= 2) {
	int l = (i == 2) ? 1 : i;
	AMSPoint x1 = _ptest->GetHit(l)->GetCoord(imult[l]);

	if ( !(fabs(_ptest->_Par[0][1]+_ptest->_Par[0][0]*x1[2]-x1[0]) 
	       < TRFITFFKEY.SearchRegStrLine*_ptest->_Par[0][2] &&
	       fabs(_ptest->_Par[1][1]+_ptest->_Par[1][0]*x1[2]-x1[1]) 
	       < TRFITFFKEY.SearchRegCircle *_ptest->_Par[1][2]) ) continue;

	_ptest->_Nhits = i+1;
	double csq = _ptest->SimpleFit();
	_ptest->_Nhits = nhits;
	if (csq > TRFITFFKEY.MaxChisq[0]) continue;
      }

      }

      // Go to the next layer
      ScanHit(layer, iscan, imult, i+1);
    }
  }

  if (cont) delete cont;
}

//=========================================================================
// #####################################
// #             MC HITS               #
// #####################################

extern "C" double rnormx();


void TrRecon::sitkhits(int idsoft, float vect[],
			   float edep, float step, int itra)
{
  AMSPoint ppa (vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pmom(vect[3]*vect[6], vect[4]*vect[6], vect[5]*vect[6]);
  AMSPoint ppb = ppa-dirg*step;
  AMSPoint pgl = ppa-dirg*step/2;
  
  AMSPoint size(TkDBc::Head->_ssize_active[0]/2,
		TkDBc::Head->_ssize_active[1]/2,
		TkDBc::Head->_silicon_z/2);
  //  XXPLSS  |XX=sensor|P 0 neg 1 pos |L=layer |SS=slot|
  int sensor = abs(idsoft)/10000;
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
  //Get the pointer to the TrMCCluster container
  VCon* cont=TRCon->GetCont("AMSTrMCCluster");
  if(!cont){
    printf("TrSim::sitkdigi()  Cant Find AMSMCCluster Container Digitization is Impossible !!!\n");
    return ;
  }
  if(cont->getnelem()==0){
    printf("TrSim::sitkdigi()  AMSTrMCCluster Container is Empty  Digitzation is Impossible !!!\n");
    return ;
  }else
    printf("TrSim::sitkdigi()  AMSTrMCCluster Container has %d elements \n",cont->getnelem());
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
