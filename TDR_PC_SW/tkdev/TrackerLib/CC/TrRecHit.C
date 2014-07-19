//////////////////////////////////////////////////////////////////////////
///
///\file  TrRecHit.C
///\brief Source file of AMSTrRecHit class
///
///\date  2007/12/03 SH  First import (just a copy of trrec.C in Gbatch)
///\date  2007/12/28 SH  First refinement (for TkTrack)
///\date  2008/01/14 SH  First stable vertion after a refinement
///\date  2008/01/20 SH  Imported to tkdev (test version)
///\date  2008/02/19 AO  New data format 
///\date  2008/02/22 AO  Temporary clusters reference  
///\date  2008/02/26 AO  Local and global coordinate (TkCoo.h)
///\data  2008/03/06 AO  Changing some data members and methods
///\data  2008/04/12 AO  From XEta to XCofG(3) (better for inclination)
///\data  2008/11/29 SH  _dummyX added, _residual moved to TrTrack
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecHit.h"
ClassImp(AMSTrRecHit);
string AMSTrRecHit::sout;

#ifdef __WRITEROOT_
#include "VCon_gbatch.h"
VCon* AMSTrRecHit::vcon= new VCon_gb();
#else  
#include "VCon_root.h"
VCon* AMSTrRecHit::vcon= new VCon_root();
#endif

AMSTrRecHit::AMSTrRecHit(void) {
  Clear();
}


AMSTrRecHit::AMSTrRecHit(const AMSTrRecHit& orig) {
  _tkid     = orig._tkid;     
  _clusterX = orig._clusterX;   
  _clusterY = orig._clusterY;  

  _iclusterX = orig._iclusterX;   
  _iclusterY = orig._iclusterY;  
  _corr     = orig._corr;
  _prob     = orig._prob;     
  _status   = orig._status;
  _mult     = orig._mult;
  _imult    = orig._imult;
  _coord    = orig._coord;
  _dummyX   = orig._dummyX;
}


AMSTrRecHit::AMSTrRecHit(int tkid, AMSTrCluster* clX, AMSTrCluster* clY, float corr, float prob, int imult) {
  _tkid     = tkid;     
  _clusterX = clX;   
  _clusterY = clY;   
  VCon* cont2=vcon->GetCont("AMSTrCluster");
  _iclusterX = (_clusterX) ? cont2->getindex(_clusterX) : -1;
  _iclusterY = (_clusterY) ? cont2->getindex(_clusterY) : -1;
  delete cont2;

  _status   = 0;
  if (!clX) _status |= YONLY;
  if (!clY) _status |= XONLY;

  _corr     = corr;
  _prob     = prob;     
  _dummyX   = 0;

  // coordinate construction
  BuildCoordinates();
  _imult    = imult; 
}

AMSTrCluster* AMSTrRecHit::GetXCluster() { 
  if(_clusterX==NULL&& !(_status&YONLY)){
    VCon* cont2=vcon->GetCont("AMSTrCluster");
    _clusterX = (AMSTrCluster*)cont2->getelem(_iclusterX);
    delete cont2;
  }
  return _clusterX;
}


AMSTrCluster* AMSTrRecHit::GetYCluster() { 
  if(_clusterY==NULL&& !(_status&XONLY)){
    VCon* cont2=vcon->GetCont("AMSTrCluster");
    _clusterY = (AMSTrCluster*)cont2->getelem(_iclusterY);
    delete cont2;
  }
  return _clusterY;
}



void AMSTrRecHit::BuildCoordinates() {
  // coordinate construction
  AMSTrCluster* clX= GetXCluster();
  int xaddr = (clX!=0) ? clX->GetAddress() : 640;
  _mult     = TkCoo::GetMaxMult(GetTkId(), xaddr)+1;
  _coord.clear();
  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetGlobalCoordinate(imult));
}

AMSTrRecHit::~AMSTrRecHit() {
  Clear();
}





void AMSTrRecHit::Clear() {
  _tkid     = 0; 
  _clusterX = 0;
  _clusterY = 0;
  _iclusterX = -1;
  _iclusterY = -1;
  _prob     = 0;
  _status   = 0;
  _mult     = 0;
  _imult    = -1; 
  _dummyX   = 0;
  _coord.clear();
}


void AMSTrRecHit::Print() { 
  Info();
  cerr <<sout;
}
void AMSTrRecHit::Info() { 
  char msg[1000];
  sout.clear();
  sprintf(msg,"tkid: %+03d (x,y,z)=(%10.4f,%10.4f,%10.4f)  corr: %8.4f  prob: %7.5f  stat: %2d\n",
	  _tkid,GetCoord(0).x(),GetCoord(0).y(),GetCoord(0).z(),GetCorrelation(),GetProb(),GetStatus());
  sout.append(msg);
  cout <<sout;
}

std::ostream &AMSTrRecHit::putout(std::ostream &ostr) const {
  //AMSPoint aa=this->GetGlobalCoordinate();
  return ostr << "TkId: " << GetTkId() <<"  Prob: "<<GetProb()<<endl;
    
}


AMSPoint AMSTrRecHit::GetGlobalCoordinate(int imult, char* options) {
  // parsing options
  bool ApplyAlignement = false;
  char character = ' ';
  int  cc = 0;
  while (character!='\0') {
    character = *(options+cc);
    if ( (character=='A')||(character=='a') ) ApplyAlignement = true;
    cc++;
  }
  // calculation
  AMSPoint loc = GetLocalCoordinate(imult);
  
  AMSPoint glo;
  if (!ApplyAlignement) {
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
  }
  return glo;
}

