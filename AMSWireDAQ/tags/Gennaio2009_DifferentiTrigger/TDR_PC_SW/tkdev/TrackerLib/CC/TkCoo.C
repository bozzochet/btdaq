/// $Id: TkCoo.C,v 1.16 2008/11/13 20:16:22 haino Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TkCoo.C
///\brief Source file of TkCoo class
///
///\date  2008/02/21 PZ  First version
///\date  2008/03/19 PZ  Add some features to TkSens
///\date  2008/04/10 AO  GetLocalCoo(float) of interstrip position 
///\date  2008/04/22 AO  Swiching back some methods  
///$Date: 2008/11/13 20:16:22 $
///
/// $Revision: 1.16 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TkCoo.h"

//--------------------------------------------------
AMSPoint TkCoo::GetGlobalN(int tkid,float X, float Y){
  // if(X<0||X > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",X);
  //   return AMSPoint();
  // }
  if(Y<0 || Y>TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalN Error Y is outside the ladder size %f\n",Y);
    return AMSPoint();
  }
  AMSPoint loc(X,Y,0.);
  return TkCoo::GetGlobalN(tkid, loc);
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalN(int tkid, AMSPoint& loc){
  // if(loc[0]<0||loc[0] > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",loc[0]);
  //   return AMSPoint();
  // }
  if(loc[1]<0 || loc[1] > TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalN Error Y is outside the ladder size %f\n",loc[1]);
    return AMSPoint();
  }
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetMaxMult: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint();
  }
  AMSPoint loc2(loc);
  // Set ladder local coo Z to zero
  loc2[2]=0;
  
  // Get the Global coo on the plane
  AMSPoint  oo = ll->GetRotMat()*loc2+ll->GetPos();
  
  // Get The Plane Pointer
  TkPlane*  pp = ll->_plane;

  // Convolute with the Plane pos in the space and Get the global Coo
  AMSPoint oo2 = pp->GetRotMat()*oo + pp->GetPos();

  return oo2;
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalA(int tkid,float X, float Y){
  // if(X<0||X > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",X);
  //   return AMSPoint();
  // }
  if(Y<0 || Y>TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalA Error Y is outside the ladder size %f\n",Y);
    return AMSPoint();
  }
  AMSPoint loc(X,Y,0.);
  return TkCoo::GetGlobalA(tkid, loc);
}


//--------------------------------------------------
AMSPoint TkCoo::GetGlobalA(int tkid, AMSPoint& loc){
  // if(loc[0]<0||loc[0] > TkDBc::Head->_ssize_active[0]){
  //   printf("TkCoo::GetGlobalN Error X is outside the ladder size %f\n",loc[0]);
  //   return AMSPoint();
  // }
  if(loc[1]<0 || loc[1]>TkDBc::Head->_ssize_active[1]){
    printf("TkCoo::GetGlobalA Error Y is outside the ladder size %f\n",loc[1]);
    return AMSPoint();
  }
  TkLadder* ll=TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetMaxMult: ERROR cant find ladder %d into the database\n",tkid);
    return AMSPoint();
  }
  AMSPoint loc2(loc);
  // Set ladder local coo Z to zero
  loc2[2]=0;

  // Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG  = ll->GetRotMatA()*ll->GetRotMat();

  // Alignment corrected Ladder postion
  AMSPoint  PosG  = ll->GetPosA()+ll->GetPos();

  // Get the Global coo on the plane
  AMSPoint  oo    = RotG*loc2+PosG;

  // Get The Plane Pointer
  TkPlane*  pp    = ll->_plane;

  // Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG = pp->GetRotMatA()*pp->GetRotMat();

  // Alignment corrected Plane postion
  AMSPoint  PPosG = pp->GetPosA()+pp->GetPos();

  // Covolute with the Plane pos in the space and Get the global Coo
  AMSPoint  oo2   = PRotG*oo + PPosG;

  return oo2;
}


//--------------------------------------------------
int TkCoo::GetMaxMult(int tkid, float readchann){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if(!ll){
    printf("GetMaxMult: ERROR cant find ladder %d into the database\n",tkid);
    return -1;
  }
  int max = 1;
  if(readchann<=639.) return 0;
  if( (ll->GetLayer()==8) || (ll->GetLayer()==1) ) 
    max=(int) ceil((ll->_nsensors*TkDBc::Head->_NReadStripK7-readchann+640)/384.);
  else
    max=(int) ceil((ll->_nsensors*TkDBc::Head->_NReadStripK5-readchann+640)/384.);
  
  return (max-1);
}


//--------------------------------------------------
float TkCoo::GetLocalCoo(int tkid, int readchann,int mult){
  int mmult=GetMaxMult(tkid,readchann);
  int plane=abs(tkid)/100;
  if(mult>mmult||mult<0) mult=mmult;
  // float out=0.;
  if(readchann<640) return GetLocalCooS(readchann);
  else{
    if(plane==1||plane==8) return GetLocalCooK7(readchann,mult);
    else return GetLocalCooK5(readchann,mult);
  }
}

//--------------------------------------------------
float TkCoo::GetLocalCoo(int tkid, float readchann,int mult){
  int mmult=GetMaxMult(tkid,readchann);
  int plane=abs(tkid)/100;
  if(mult>mmult||mult<0) mult=mmult;
  // float out=0.;
  int channel = (int) round(readchann);
  if(readchann<640) {
    number cooY1,cooY2;  
    cooY1=TkCoo::GetLocalCooS(channel);
    if((readchann-channel)>=0)
      cooY2=TkCoo::GetLocalCooS(channel+1);
    else
      cooY2=TkCoo::GetLocalCooS(channel-1);
    return cooY1 + (readchann-channel)*abs(cooY1-cooY2);
  }
  else{
    if(plane==1||plane==8) {
      number cooX1,cooX2;  
      cooX1=TkCoo::GetLocalCooK7(channel,mult);
      if((readchann-channel)>=0)
	cooX2=TkCoo::GetLocalCooK7(channel+1,mult);
      else
	cooX2=TkCoo::GetLocalCooK7(channel-1,mult);
      return cooX1 + (readchann-channel)*abs(cooX1-cooX2);
    }
    else {
      number cooX1,cooX2;  
      cooX1=TkCoo::GetLocalCooK5(channel,mult);
      if((readchann-channel)>=0)
	cooX2=TkCoo::GetLocalCooK5(channel+1,mult);
      else
	cooX2=TkCoo::GetLocalCooK5(channel-1,mult);
      return cooX1 + (readchann-channel)*abs(cooX1-cooX2);
    }
  }
}

//--------------------------------------------------
// The gaps between 0 and 1 and 638 to 639 are doubled
float TkCoo::GetLocalCooS(int readchann){
  if(readchann>639||readchann<0) return -1;
  float out= readchann*TkDBc::Head->_PitchS;
  if(readchann!=0)    out+=TkDBc::Head->_PitchS;
  if(readchann==639)  out+=TkDBc::Head->_PitchS;
  return out;
}


//--------------------------------------------------
// The gap between the 190 and 191 is one and half
float TkCoo::GetLocalCooK5(int readchann,int mult){
  if(readchann>1023||readchann<640) return -1;  
  int chann=readchann-640;
  int Sensor=((chann+mult*TkDBc::Head->_NReadoutChanK)/TkDBc::Head->_NReadStripK5);
  int chann2=chann;
  //  printf("Sensor %d  %d\n",Sensor,mult);
  if(chann2>191) chann2-=192;
  return TkDBc::Head->_SensorPitchK*Sensor+ chann2 * TkDBc::Head->_PitchK5
    + ((chann2 == 191) ? TkDBc::Head->_PitchK5*0.5 : 0);
}


//--------------------------------------------------
// The implantation of the strips in K7 is not uniform
// ImplantedFromReadK7 evaluate the implanted strips in terms of _ImplantPitchK unit
float TkCoo::GetLocalCooK7(int readchann,int mult){
  if(readchann>1023||readchann<640) return -1;  
  int chann=readchann-640;
  int Sensor=((chann+mult*TkDBc::Head->_NReadoutChanK)/TkDBc::Head->_NReadStripK7);
  int readstrip= (chann+mult*TkDBc::Head->_NReadoutChanK) - Sensor * TkDBc::Head->_NReadStripK7;
  return TkDBc::Head->_SensorPitchK*Sensor+ ImplantedFromReadK7(readstrip) * TkDBc::Head->_ImplantPitchK;    
}

//--------------------------------------------------
// - from   0 to  64: readout pitch is 2 1 2 1 2 1, ...
// - from  64 to 160: readout pitch is 2
// - from 160 to 224: readout pitch is 2 1 2 1 2 1, ... 
int TkCoo::ImplantedFromReadK7(int channel){
  // this is the conversion for sensor 1
  //NOTA: channel = 0 to 383..
  int pitch=0;
  int start=0;
  int strip=0;
  int diff=0;
  int startch=0;
  if (channel<64) { // subgroups 1 and 2, 64 channels
    pitch=3;
    startch=channel%2;
    start=2*startch;

    diff=(channel-startch)/2; // the difference is always even
    strip=start+diff*pitch;         
  } else if (channel<160) { // subgroups 3 to 5, 96 channels
    pitch=2;
    start=96;
    startch=64;
    diff=(channel-startch);
    strip=start+diff*pitch;
  } else if (channel<224) {
    pitch=3;
    startch=channel%2;
    start=48+2*startch;
    //    startch+=160;
    diff=(channel-startch)/2; // the difference is always even
    strip=start+diff*pitch; 
  }
  return strip;
}


//--------------------------------------------------
double TkCoo::GetLadderLength(int tkid) {
  // Get ladder length along the X-coordinate in cm
  if(!TkDBc::Head) return -9999.;
  TkLadder* pp=TkDBc::Head->FindTkId(tkid);
  if(!pp) return -9999.;
  
  return TkDBc::Head->_SensorPitchK*pp->_nsensors;
}


//--------------------------------------------------
double TkCoo::GetLadderCenterX(int tkid) { 
// Get central position of the ladder in the X-coordinate in cm
  if(!TkDBc::Head) return -9999.;
  TkLadder* pp=TkDBc::Head->FindTkId(tkid);
  if(!pp) return -9999.;

  double hlen = GetLadderLength(tkid)/2-TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  return TkCoo::GetGlobalA(tkid, hlen, hwid).x();
}


//--------------------------------------------------
double TkCoo::GetLadderCenterY(int tkid) {
  // Get central position of the ladder in the Y-coordinate in cm
  if(!TkDBc::Head) return -9999.;
  TkLadder* pp=TkDBc::Head->FindTkId(tkid);
  if(!pp) return -9999.;

  double hlen = GetLadderLength(tkid)/2-TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  return TkCoo::GetGlobalA(tkid, hlen, hwid).y();
}



AMSPoint TkCoo::GetGlobalNC(int tkid,float readchanK, float readchanS,int mult){
  int layer=abs(tkid)/100;
  if(layer==1||layer==8)
    return GetGlobalN(tkid,GetLocalCooK7((int)readchanK, mult),  GetLocalCooS((int)readchanS));
  else 
    return GetGlobalN(tkid,GetLocalCooK5((int)readchanK, mult),  GetLocalCooS((int)readchanS));
}

AMSPoint TkCoo::GetGlobalAC(int tkid,float readchanK, float readchanS,int mult){
  int layer=abs(tkid)/100;
  if(layer==1||layer==8)
    return GetGlobalA(tkid,GetLocalCooK7((int)readchanK, mult),  GetLocalCooS((int)readchanS));
  else 
    return GetGlobalA(tkid,GetLocalCooK5((int)readchanK, mult),  GetLocalCooS((int)readchanS));
}
