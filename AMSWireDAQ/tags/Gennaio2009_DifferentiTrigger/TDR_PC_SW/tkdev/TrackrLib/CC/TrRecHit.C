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
///
//////////////////////////////////////////////////////////////////////////

#include "TrRecHit.h"

ClassImp(AMSTrRecHit);
string AMSTrRecHit::sout;
  
AMSTrRecHit::AMSTrRecHit(void) {
  Clear();
}


AMSTrRecHit::AMSTrRecHit(const AMSTrRecHit& orig) {
  _tkid     = orig._tkid;     
  _layer    = orig._layer;    
  _clusterX = orig._clusterX;   
  _clusterY = orig._clusterY;  
  _corr     = orig._corr;
  _prob     = orig._prob;     
  _status   = orig._status;
  _mult     = orig._mult;
  _imult    = orig._imult;
  _coord    = orig._coord;
  _residual = orig._residual;
}


AMSTrRecHit::AMSTrRecHit(int tkid, AMSTrCluster* clX, AMSTrCluster* clY, float corr, float prob, int status) {
  _tkid     = tkid;     
  _layer    = abs(tkid/100);    
  _clusterX = clX;   
  _clusterY = clY;   
  _corr     = corr;
  _prob     = prob;     
  _status   = status;
  // coordinate construction
  _mult     = (clX!=0) ? clX->GetMultiplicity() : 1;
  _imult    = -1; 
  for (int imult=0; imult<_mult; imult++) _coord.push_back(GetGlobalCoordinate(imult));
  _residual[0] = 0.;
  _residual[1] = 0.;
  _residual[2] = 0.;
}


AMSTrRecHit::~AMSTrRecHit() {
  Clear();
}


void AMSTrRecHit::Clear() {
  _tkid     = 0; 
  _layer    = 0;
  _clusterX = 0;
  _clusterY = 0;
  _prob     = 0;
  _status   = 0;
  _mult     = 0;
  _imult    = -1; 
  _coord.clear();
  _residual[0] = 0.;
  _residual[1] = 0.;
  _residual[2] = 0.;
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
    if (!GetXCluster()) { glo = TkCoo::GetGlobalN(_tkid,0.,loc[1]); glo[0] = -1000.; return glo; }
    if (!GetYCluster()) { glo = TkCoo::GetGlobalN(_tkid,loc[0],0.); glo[1] = -1000.; return glo; }
    glo = TkCoo::GetGlobalN(GetTkId(),loc);
  }
  else {
    if (!GetXCluster()) { glo = TkCoo::GetGlobalA(_tkid,0.,loc[1]); glo[0] = -1000.; return glo; }
    if (!GetYCluster()) { glo = TkCoo::GetGlobalA(_tkid,loc[0],0.); glo[1] = -1000.; return glo; }
    glo = TkCoo::GetGlobalA(GetTkId(),loc);
  }
  return glo;
}

