/// $Id: TkSens.C,v 1.13 2008/12/02 23:27:50 haino Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TkSens.C
///\brief Source file of TkSens class
///
///\date  2008/03/18 PZ  First version
///\date  2008/04/02 SH  Some bugs are fixed
///\date  2008/04/18 SH  Updated for alignment study
///\date  2008/04/21 AO  Ladder local coordinate and bug fixing
///$Date: 2008/12/02 23:27:50 $
///
/// $Revision: 1.13 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"

#include "TkSens.h"
#include "TkCoo.h"


//--------------------------------------------------
TkSens::TkSens() {
  Clear();
}


//--------------------------------------------------
TkSens::TkSens(AMSPoint& GCoo){
  Clear();
  GlobalCoo=GCoo;
  Recalc();
}


//--------------------------------------------------
TkSens::TkSens(int tkid, AMSPoint& GCoo){
  Clear();
  GlobalCoo=GCoo;
  lad_tkid=tkid; 
  Recalc();
}


//--------------------------------------------------
void TkSens::Clear(){

  lad_tkid =0;
  sens     =-1;
  mult     =-1;
  SensCoo[0]=-1000;SensCoo[1]=-1000;SensCoo[2]=-1000;
  LaddCoo=SensCoo;
  ReadChanX =-1;
  ReadChanY =-1;
  ImpactPointX =-1;
  ImpactPointY =-1;

}


//--------------------------------------------------
bool TkSens::IsInsideLadder(int tkid){
  TkLadder* lad=0;
  lad=TkDBc::Head->FindTkId(lad_tkid);
  if (!lad) {return  false;}
  return IsInsideLadder(lad);

}

//--------------------------------------------------
void TkSens::Recalc(){

  int lay = -1;
  if( lad_tkid==0) {
    lay=GetLayer();
    if(lay)
      lad_tkid=FindLadder();
    if(!IsInsideLadder(lad_tkid)) return;
  }
  
  sens=GetSens();
  ReadChanY=GetStripFromLocalCooS(SensCoo[1]);
  number cooY1,cooY2;  
  cooY1=TkCoo::GetLocalCooS(ReadChanY);
  if((LaddCoo[1]-cooY1)>=0)
    cooY2=TkCoo::GetLocalCooS(ReadChanY+1);
  else
    cooY2=TkCoo::GetLocalCooS(ReadChanY-1);
  ImpactPointY=(LaddCoo[1]-cooY1)/abs(cooY1-cooY2);
  
  mult = -1;
  int layer=abs(lad_tkid)/100;
  number cooX1,cooX2;  
  if(layer==1||layer==8){
    ReadChanX=GetStripFromLocalCooK7(SensCoo[0],sens);
    mult = sens*TkDBc::Head->_NReadStripK7/TkDBc::Head->_NReadoutChanK;
    int test = (mult*TkDBc::Head->_NReadoutChanK+ReadChanX)/TkDBc::Head->_NReadStripK7;
    if (test!=sens) mult = mult+1;
    cooX1=TkCoo::GetLocalCooK7(640+ReadChanX,mult);
    if((LaddCoo[0]-cooX1)>=0) 
      cooX2=TkCoo::GetLocalCooK7(640+ReadChanX+1,mult);
    else
      cooX2=TkCoo::GetLocalCooK7(640+ReadChanX-1,mult);
  }    
  else{
    ReadChanX=GetStripFromLocalCooK5(SensCoo[0],sens);
    mult = sens*TkDBc::Head->_NReadStripK5/TkDBc::Head->_NReadoutChanK;
    cooX1=TkCoo::GetLocalCooK5(640+ReadChanX,mult);
    if((LaddCoo[0]-cooX1)>=0)
      cooX2=TkCoo::GetLocalCooK5(640+ReadChanX+1,mult);
    else
      cooX2=TkCoo::GetLocalCooK5(640+ReadChanX-1,mult);
  }
  ImpactPointX=(LaddCoo[0]-cooX1)/abs(cooX1-cooX2);

  // - always defined (also in regions of no definition 640 and -0.2)
  // - sensor/multiplicity (880)

  /*
  // to be removed
  cerr << "Layer:             " << lay << endl;
  cerr << "TkId:              " << lad_tkid << endl;
  cerr << "GlobalCoordinate:  " << GlobalCoo << endl;
  cerr << "SensorCoordinate:  " << SensCoo << endl;
  cerr << "ReadChanX:         " << ReadChanX << endl;
  cerr << "ReadChanY:         " << ReadChanY << endl;
  cerr << "LadderCoordinate:  " << LaddCoo << endl;
  cerr << "LadderCoordinate1: " << cooX1 << "  " << cooY1 << endl;
  cerr << "LadderCoordinate2: " << cooX2 << "  " << cooY2 << endl;
  cerr << "ImpactPointX:      " << ImpactPointX << endl;
  cerr << "ImpactPointY:      " << ImpactPointY << endl;
  cerr << "Multiplicity:      " << mult << endl;
  cerr << "Sensor:            " << sens << endl;
  */
  return;

}


//--------------------------------------------------
int TkSens::GetSens(){
  if(lad_tkid==0) return -1;

  //Get the Ladder Pointer
  TkLadder* lad=TkDBc::Head->FindTkId(lad_tkid);
  if(!lad) return -1;

  //Get The Plane Pointer
  TkPlane * pp=lad->_plane;

  //Alignment corrected Plane postion
  AMSPoint PPosG=pp->GetPosA()+pp->GetPos();

  //Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG=pp->GetRotMat().Invert()*pp->GetRotMatA().Invert();

  //Convolute with the Plane pos in the space
  AMSPoint oo= PRotG*(GlobalCoo-PPosG);

  //Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG=lad->GetRotMat().Invert()*lad->GetRotMatA().Invert();

  //Alignment corrected Ladder postion
  AMSPoint PosG=lad->GetPosA()+lad->GetPos();

  //Get the local coo on the Ladder
  SensCoo = RotG*(oo-PosG);
  LaddCoo = SensCoo;

  //Offset of the first strip position
  double Ax= (TkDBc::Head->_ssize_inactive[0]-
	      TkDBc::Head->_ssize_active[0])/2;

  //Sensor number
  int nsens = (int)(abs(SensCoo[0]+Ax)/TkDBc::Head->_SensorPitchK);

  //Sensor coordinate
  SensCoo[0] -= nsens*TkDBc::Head->_SensorPitchK;

  return nsens;
}

//--------------------------------------------------
int TkSens::GetLayer() {

///\param z  Z coordinate of global position (cm)
///
/// For the Z-coordinate, +/- 1 cm of the layer position is accepted
/// For the X,Y-coordinates, inactive area is also accepted
  
  // Estimate layer
  double z=GlobalCoo[2];
  double toll=0.1; //1 mm
  int layer;
  if      (  z < TkDBc::Head->GetZlayer(1)+toll  &&   
             z > TkDBc::Head->GetZlayer(1)-toll ) layer = 1; 
  else if (  z < TkDBc::Head->GetZlayer(2)+toll  &&   
             z > TkDBc::Head->GetZlayer(2)-toll ) layer = 2; 
  else if (  z < TkDBc::Head->GetZlayer(3)+toll  &&   
             z > TkDBc::Head->GetZlayer(3)-toll ) layer = 3; 
  else if (  z < TkDBc::Head->GetZlayer(4)+toll  &&   
             z > TkDBc::Head->GetZlayer(4)-toll ) layer = 4; 
  else if (  z < TkDBc::Head->GetZlayer(5)+toll  &&   
             z > TkDBc::Head->GetZlayer(5)-toll ) layer = 5; 
  else if (  z < TkDBc::Head->GetZlayer(6)+toll  &&   
             z > TkDBc::Head->GetZlayer(6)-toll ) layer = 6; 
  else if (  z < TkDBc::Head->GetZlayer(7)+toll  &&   
             z > TkDBc::Head->GetZlayer(7)-toll ) layer = 7; 
  else if (  z < TkDBc::Head->GetZlayer(8)+toll  &&   
             z > TkDBc::Head->GetZlayer(8)-toll ) layer = 8; 
  else    
                                                  layer = 0;
 return layer;	
}



//--------------------------------------------------
//! It takes a point in global coo and chek if it is inside the ladder pointed by the tkid
int TkSens::FindLadder(){
  TkLadder* lad=0;
  bool found=0;
  for(int ii=0;ii<TkDBc::Head->GetEntries();ii++){
    lad=TkDBc::Head->GetEntry(ii);
    found=IsInsideLadder(lad);
    if(found) break;
  }
  if(!found) return 0;
  else return lad->GetTkId();
  
}


//--------------------------------------------------
bool TkSens::IsInsideLadder(TkLadder* lad){
  // Z check: layer check
  if (GetLayer()!=lad->GetLayer()) return false; // AO Bug fix: otherwise takes the first plane column
  // XY check
  double X=GlobalCoo[0];
  double Y=GlobalCoo[1];

  double vx=lad->rot.GetEl(0,0);
  double vy=lad->rot.GetEl(1,1);

  double Ax= lad->pos[0] - (TkDBc::Head->_ssize_inactive[0]-
		     TkDBc::Head->_ssize_active[0])/2. *vx;


  double Ay= lad->pos[1] - (TkDBc::Head->_ssize_inactive[1]-
		     TkDBc::Head->_ssize_active[1])/2. *vy;

  double Dx= Ax + (lad->_nsensors*
	    (TkDBc::Head->_ssize_inactive[0]+0.004)
	    -0.004)*vx;

  double Dy= Ay + TkDBc::Head->_ssize_inactive[1] * vy;

  int good=1;
  if( Ax >Dx){
    if( X > Ax || X < Dx )       good*= 0;
  }
  else{
    if( X < Ax || X > Dx )       good*= 0;
  }

  if( Ay >Dy){
    if( Y > Ay || Y < Dy )       good*= 0;
  }
  else{
    if( Y < Ay || Y > Dy )       good*= 0;
  }
  
  return (good>0);
}


//--------------------------------------------------
int TkSens::GetStripFromLocalCooS(number Y){
  if( Y<0 ||Y > TkDBc::Head->_ssize_active[1])
    return -1;

  else if(Y >= 1.5*TkDBc::Head->_PitchS && 
     Y < TkDBc::Head->_ssize_active[1] - 2.5*TkDBc::Head->_PitchS)
    return 1+(int)round((Y-  2*TkDBc::Head->_PitchS)/TkDBc::Head->_PitchS);
  
  else if( Y >= 0 && Y < 1.5*TkDBc::Head->_PitchS)
    return 0;
  else 
    return 639;
}

//--------------------------------------------------
int TkSens::GetStripFromLocalCooK5(number X,int sens){

  if( X<0 ||X > TkDBc::Head->_ssize_active[0])
    return -1;
  
  int chan=(int)round(X/TkDBc::Head->_PitchK5);
  if(sens%2==1)
    chan+=192;
  return chan;
}




//--------------------------------------------------
int TkSens::GetStripFromLocalCooK7(number X,int sens){


  if( X<0 ||X > TkDBc::Head->_ssize_active[0])
    return -1;
  
  int gstrip=-1;
  float strip=(X/TkDBc::Head->_ImplantPitchK);

  if(strip<95.5){
    int tstrip=(int)trunc(strip);
    int rstrip=(int)round(strip);
    int ff=tstrip%3;
    if(ff==2) gstrip=rstrip*2/3;
    else if(ff==0) gstrip=tstrip*2/3;
    else gstrip= (tstrip+1)*2/3;
    
  }else if (strip>=95.5 && strip<287.5){
    int tstrip=(int)trunc(strip)-96;
    int ff=tstrip%2;
    if(ff==0) gstrip=(tstrip)/2+64;
    else gstrip=(tstrip+1)/2+64;
  }
  else{
    int tstrip=(int)trunc(strip)-288;
    int rstrip=(int)round(strip)-288;
    int ff=tstrip%3;
    if(ff==2) gstrip=rstrip*2/3+160;
    else if(ff==0) gstrip=tstrip*2/3+160;
    else gstrip= (tstrip+1)*2/3+160;
  }

  if(gstrip<0) return -1;
  return (sens*224+gstrip)%384;
}

