//  $Id: tkgeom.C,v 1.5 2008/10/14 16:51:56 zuccon Exp $



#include "gvolume.h"  // for AMSgvolume

#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"

#include <strstream>
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

// Ladder support thickness in cm
static double sup_foam_w   = 0.5;
// Ladder support z-offset in cm
static double sup_foam_tol = 0.05;

int _nrot = 0;



void        BuildHoneycomb    (AMSgvolume *mvol, int plane);
AMSgvolume *BuildPlaneEnvelop (AMSgvolume *mvol, int plane);
AMSgvolume *BuildLadder       (AMSgvolume *mvol, int tkid);
void        BuildSensor       (AMSgvolume *mvol, int tkid, int s);
void        BuildHybrid       (AMSgvolume *mvol, int tkid);
void        BuildSupport      (AMSgvolume *mvol, int tkid);


void amsgeom::tkgeom02(AMSgvolume &mother)
{
// Build tracker geometry

  // Temporary arbitary assignment
  _nrot = 501;

  int nladder = 0;

  // Loop on planes
  for (int plane=1; plane<=trconst::nplanes;plane++){

    // Build a plane geometry (made of VACUUM)
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, plane);
    // Build plane support
    BuildHoneycomb(vplane, plane);
    
    // Loop on Si layer attched to the plane
    int maxp_lay=1;
    int minp_lay=0;
    if(plane==1)      {minp_lay=1;maxp_lay=2;}
    else if(plane==8) {minp_lay=0;maxp_lay=1;}
    else              {minp_lay=0;maxp_lay=2;}

    for (int p_lay = minp_lay; p_lay<maxp_lay; p_lay++) {
      int layer=2*(plane-1)+p_lay;
         
      // Loop for each slot
      for (int slot = 1; slot <= trconst::maxlad; slot++) {
	
	// Loop for each side
	for (int side = -1; side <= 1; side += 2) {
	  
	  // Check TkId is valid or not
	  int tkid = side*(layer*100+slot);
	  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	  if (!lad) continue;
	  
	  // Build ladder
	  AMSgvolume *vlad = BuildLadder(vplane, tkid);
	  nladder++;
	  
	  // Build electronics
	  BuildHybrid(vplane, tkid);
	  
	  // Build ladder support
	  BuildSupport(vplane, tkid);

	} //side
      } //slot
      
    }//lay

  }//plane

  std::cout << "<---- TKGeom-I-" << nladder
	    << " Active halfladders initialized" 
	    << std::endl << std::endl;
}
//============================================================================
AMSgvolume *BuildPlaneEnvelop(AMSgvolume *mvol, int plane)
{
// Build Plane container geometry (plane number begins from 1)
  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "STK" << plane << std::ends;

  TkPlane *pl = TkDBc::Head->GetPlane(plane);

  geant coo[3];
  coo[0] = pl->pos[0]+pl->GetPosA()[0];
  coo[1] = pl->pos[1]+pl->GetPosA()[1];
  coo[2] = pl->pos[2]+pl->GetPosA()[2]+TkDBc::Head->_dz[plane-1];

  geant par[3];
  par[0] = 0;
  par[1] = TkDBc::Head->_plane_d1[plane-1]; //container radius
  par[2] = TkDBc::Head->_plane_d2[plane-1]; //container half thickness

  AMSRotMat lrm = pl->GetRotMatA();
  lrm * pl->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  return (AMSgvolume*)mvol
    ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
			 par, 3, coo, nrm, "ONLY", 0, plane, 1));
}
//============================================================================

AMSgvolume *BuildLadder(AMSgvolume *mvol, int tkid)
{
// Build ladder geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return 0;

  int layer = std::abs(tkid)/100;
  int islot = (tkid > 0) ? tkid%100 : -tkid%100+20;
  int iname = layer*100+islot;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "L" << iname << std::ends;

  geant par[3];
  par[0] = TkCoo::GetLadderLength(tkid)/2;
  par[1] = TkDBc::Head->_ssize_inactive[1]/2;
  par[2] = TkDBc::Head->_silicon_z/2;

  double hlen = TkCoo::GetLadderLength(tkid)/2
        -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = oo.z();
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];

  AMSRotMat lrm = lad->GetRotMatA();
  lrm * lad->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  int gid =abs(tkid)/tkid* (abs(tkid)+1000);
  AMSgvolume* ladd=(AMSgvolume*)mvol
    ->add(new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
			 par, 3, coo, nrm, "ONLY", 1, gid, 1));

  //  printf("Ladder name %s\n",name);
  
  // Build sensors
  for (int sensor = 0; sensor < lad->_nsensors; sensor++){
    char nameS[5];
    std::ostrstream ost2(nameS,sizeof(nameS));
    ost2 << "S" << iname << std::ends;

    geant par[3];
    par[0] = TkDBc::Head->_ssize_active[0]/2;
    par[1] = TkDBc::Head->_ssize_active[1]/2;
    par[2] = TkDBc::Head->_silicon_z/2;
    
    // Get coordinate w.r.t. the mother ladder
    geant coo[3];
    coo[0] = -TkCoo::GetLadderLength(tkid)/2+ TkDBc::Head->_ssize_inactive[0]/2.+ sensor * TkDBc::Head->_SensorPitchK;
    coo[1] = coo[2] = 0;

    number nrm[3][3];
    VZERO(nrm, 9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    int lside= (tkid>0)? 1:0;
    int gid = abs(tkid)+1000*(lside)+10000*(sensor+1);
    //printf("Sensor name %s  %+03d   %+9d \n",nameS,tkid,gid);
    ladd->add(new AMSgvolume("ACTIVE_SILICON", _nrot++, nameS, "BOX",
			     par, 3, coo, nrm, "ONLY", 1, gid, 1));
  }
  return ladd;
}
//============================================================================

void BuildHybrid(AMSgvolume *mvol, int tkid)
{
// Build hybrid geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return;

  int layer = std::abs(tkid)/100;
  int sign  = (tkid > 0) ? 1 : -1;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << ((tkid < 0) ? "ELL" : "ELR") << layer << std::ends;

  geant par[3];
  par[0] = TkDBc::Head->_zelec[0]/2;
  par[1] = TkDBc::Head->_zelec[1]/2;
  par[2] = TkDBc::Head->_zelec[2]/2;

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x() +sign*(TkCoo::GetLadderLength(tkid)/2+par[0]);
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(abs(oo.z())+par[2]); 
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  
  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("ELECTRONICS", _nrot++, name,
			   "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));
}
//============================================================================

void BuildSupport(AMSgvolume *mvol, int tkid)
{
// Build ladder support geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);

  if (!lad) return;


  int layer = std::abs(tkid)/100;
  
  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "FOA" << layer << std::ends;
  
  int nsen = lad->_nsensors;
  double signp = (layer%2 == 1) ? -1 : 1;

  geant par[3];
  par[0] = TkDBc::Head->_ssize_inactive[0]*nsen/2;
  par[1] = 7.3/2;//TkDBc::Head->_ladder_Ypitch/2;
  par[2] = sup_foam_w/2;    
  

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  
  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

// Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(abs(oo.z())-TkDBc::Head->_silicon_z/2-par[2]-sup_foam_tol);
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
	     
	     
  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("Tr_Foam", _nrot++, name, "BOX",
			   par, 3, coo, nrm, "ONLY", 1, gid, 1));
}


//============================================================================
void BuildHoneycomb(AMSgvolume *mvol, int plane)
{
// Build plane honeycomb support geometry (plane number begins from 1)

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "PLA" << plane << std::ends;

  geant par[3];
  par[0] = 0;
  par[1] = TkDBc::Head->_sup_hc_r[plane-1];
  par[2] = TkDBc::Head->_sup_hc_w[plane-1]/2.;

  geant coo[3];
  coo[0] = coo[1] = 0;
  coo[2] = -1*TkDBc::Head->_dz[plane-1];


  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  mvol->add(new AMSgvolume("Tr_Honeycomb", _nrot++, name,
			   "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));
}
