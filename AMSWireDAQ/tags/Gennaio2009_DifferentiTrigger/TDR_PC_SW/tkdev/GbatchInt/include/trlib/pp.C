


void AMSTrTrack::interpolate(AMSPoint  pntplane, AMSDir dirplane,AMSPoint & P1,
                             number & theta, number & phi, number & length, int icase){
  
  // interpolates track to plane (pntplane, dirplane)
  // and calculates the track parameters(P1,theta,phi) and total track length
  
  geant out[7];
  static number m55[5][5];
  geant init[7];
  geant point[6];
  geant charge=1;
  AMSDir dir;
  if(icase==0){
    AMSDir  dir(_Theta,_Phi);
    init[0]=_P0[0];
    init[1]=_P0[1];
    init[2]=_P0[2];
    init[3]=dir[0];
    init[4]=dir[1];
    init[5]=dir[2];
    init[6]=_Ridgidity;
  }
  else if (icase==1){
    AMSDir  dir(_GTheta,_GPhi);
    init[0]=_GP0[0];
    init[1]=_GP0[1];
    init[2]=_GP0[2];
    init[3]=dir[0];
    init[4]=dir[1];
    init[5]=dir[2];
    init[6]=_GRidgidity;
  }
  else {
    AMSDir  dir(_PITheta,_PIPhi);
    init[0]=_PIP0[0];
    init[1]=_PIP0[1];
    init[2]=_PIP0[2];
    init[3]=dir[0];
    init[4]=dir[1];
    init[5]=dir[2];
    init[6]=_PIRigidity;
  }
  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=dirplane[0];
  point[4]=dirplane[1];
  point[5]=dirplane[2];
  geant slength;
  TKFITPAR(init, charge,  point,  out,  m55, slength);
  P1[0]=out[0];
  P1[1]=out[1];
  P1[2]=out[2];
  theta=acos(out[5]);
  phi=atan2(out[4],out[3]);
  length=slength;
}
