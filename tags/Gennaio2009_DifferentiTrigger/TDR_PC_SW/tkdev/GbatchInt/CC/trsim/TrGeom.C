//  $Id: TrGeom.C,v 1.3 2008/04/18 19:41:31 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrGeom.C
///\brief Source file of TrGeom class
///
///\date  2008/02/19 SH  First import from Gbatch
///\date  2008/03/14 SH  First stable version after a refinement
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Update for alignment study
///$Date: 2008/04/18 19:41:31 $
///
///$Revision: 1.3 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrGeom.h"
#include "gvolume.h"  // for AMSgvolume

#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"

#include <sstream>

// Layer radius in cm
double TrGeom::layer_d1[TrGeom::maxlay] = { 72.0, 54.0, 54.0, 54.0, 54.0, 54.0, 54.0, 72.0 };
// Layer half thickness in cm
double TrGeom::layer_d2[TrGeom::maxlay] = {  5.3, 1.95, 1.95, 1.95, 1.95, 1.95, 1.95,  5.3 };
// Plane support thickness in cm
double TrGeom::sup_hc_w[TrGeom::maxlay] = {  4.0, 0.85, 0.85, 0.85, 0.85, 0.85, 0.85,  4.0 };
// Plane support radius in cm
double TrGeom::sup_hc_r[TrGeom::maxlay] = { 71.5, 53.6, 53.6, 53.6, 53.6, 53.6, 53.6, 71.5 };
// Plane support z-offset in cm
double TrGeom::sup_hc_z[TrGeom::maxlay] = { -3.375, 1.7,-2.2, 1.7,-2.2, 2.2,-1.7, 3.375};
// Electronics dimension parameters
double TrGeom::zelec[3] = { 5, 0.8, -0.015 };
// Ladder support whickness in cm
double TrGeom::sup_foam_w   = 0.5;
// Ladder support z-offset in cm
double TrGeom::sup_foam_tol = 0.05;

int TrGeom::_nrot = 0;

void TrGeom::Build(AMSgvolume *mother)
{
// Build tracker geometry

  // Temporary arbitary assignment
  _nrot = 501;

  int nladder = 0;

  // Loop for each layer
  for (int layer = 1; layer <= maxlay; layer++) {

    // Build a layer geometry (made of VACUUM)
    AMSgvolume *vlay = BuildLayer(mother, layer);

    // Loop for each slot
    for (int slot = 1; slot <= trconst::maxlad; slot++) {

      // Loop for each side
      for (int side = -1; side <= 1; side += 2) {

	// Check TkId is valid or not
	int tkid = side*(layer*100+slot);
	TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	if (!lad) continue;

	// Build ladder
	AMSgvolume *vlad = BuildLadder(vlay, tkid);
	nladder++;

        // Build sensors
	for (int sensor = 0; sensor < lad->_nsensors; sensor++)
	  BuildSensor(vlad, tkid, sensor);

	// Build electronics
	BuildHybrid(vlay, tkid);

	// Build ladder support
	if (side == -1) BuildSupport(vlay, tkid);
      }
    }

    // Build plane support
    BuildHoneycomb(vlay, layer);
  }

  std::cout << "<---- TKGeom-I-" << nladder
	    << " Active halfladders initialized" 
	    << std::endl << std::endl;
}

AMSgvolume *TrGeom::BuildLayer(AMSgvolume *mvol, int layer)
{
// Build layer geometry (layer number begins from 1)

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "STK" << layer << std::ends;

  int ipl = layer/2;
  TkPlane *pl = TkDBc::Head->GetPlane(ipl+1);

  geant coo[3];
  coo[0] = pl->pos[0]+pl->GetPosA()[0];
  coo[1] = pl->pos[1]+pl->GetPosA()[1];
  coo[2] = pl->pos[2]+pl->GetPosA()[2]+TkDBc::Head->_layer_deltaZ[layer-1];

  geant par[3];
  par[0] = 0;
  par[1] = layer_d1[layer-1];
  par[2] = layer_d2[layer-1];

  AMSRotMat lrm = pl->GetRotMatA();
  lrm * pl->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  return (AMSgvolume*)mvol
    ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
			 par, 3, coo, nrm, "ONLY", 0, layer, 1));
}

AMSgvolume *TrGeom::BuildLadder(AMSgvolume *mvol, int tkid)
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
  coo[2] = oo.z()-lad->GetPos()[2];

  AMSRotMat lrm = lad->GetRotMatA();
  lrm * lad->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  int gid = tkid+1000;
  return (AMSgvolume*)mvol
    ->add(new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
			 par, 3, coo, nrm, "ONLY", 1, gid, 1));
}

void TrGeom::BuildSensor(AMSgvolume *mvol, int tkid, int s)
{
// Build sensor geometry (sensor number begins from 0)

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return;

  int layer = std::abs(tkid)/100;
  int islot = (tkid > 0) ? tkid%100 : -tkid%100+20;
  int iname = layer*100+islot;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "S" << iname << std::ends;

  geant par[3];
  par[0] = TkDBc::Head->_ssize_active[0]/2;
  par[1] = TkDBc::Head->_ssize_active[1]/2;
  par[2] = TkDBc::Head->_silicon_z/2;

  // Get coordinate w.r.t. the mother ladder
  geant coo[3];
  coo[0] = TkDBc::Head->_SensorPitchK*(s-(lad->_nsensors-1)/2.);
  coo[1] = coo[2] = 0;

  number nrm[3][3];
  VZERO(nrm, 9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000+10000*(s+1);
  mvol->add(new AMSgvolume("ACTIVE_SILICON", _nrot++, name, "BOX",
			   par, 3, coo, nrm, "ONLY", 1, gid, 1));
}

void TrGeom::BuildHybrid(AMSgvolume *mvol, int tkid)
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
  par[0] = zelec[1]/2;
  par[1] = 7.3/2;//TkDBc::Head->_ladder_Ypitch/2;
  par[2] = (mvol->getpar(0)-zelec[2])/2;

  geant coo[3];
  coo[0] = TkCoo::GetLadderCenterX(tkid)
    +sign*(TkCoo::GetLadderLength(tkid)/2+par[0]);
  coo[1] = TkCoo::GetLadderCenterY(tkid)*mvol->getnrm(1, 1);
  coo[2] = lad->GetPosA()[2]+zelec[2]+par[2];

  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("ELECTRONICS", _nrot++, name,
			   "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));
}

void TrGeom::BuildSupport(AMSgvolume *mvol, int tkid)
{
// Build ladder support geometry
// To keep consistency with the original Gbatch,
// supports for positive and negative side of the ladders 
// are defined as one volume

  TkLadder *lad1 = TkDBc::Head->FindTkId(-tkid);
  TkLadder *lad2 = TkDBc::Head->FindTkId( tkid);
  TkLadder *lad  = (lad1) ? lad1 : lad2;
  if (!lad) return;

  tkid = lad->GetTkId();
  int layer = std::abs(tkid)/100;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "FOA" << layer << std::ends;

  int nsen = ((lad1) ? lad1->_nsensors : 0)
            +((lad2) ? lad2->_nsensors : 0);
  double signp = (layer%2 == 1) ? -1 : 1;

  geant par[3];
  par[0] = TkDBc::Head->_ssize_inactive[0]*nsen/2;
  par[1] = 7.3/2;//TkDBc::Head->_ladder_Ypitch/2;
  par[2] = sup_foam_w/2;    

  geant coo[3];
  coo[0] = 0;
  coo[1] = TkCoo::GetLadderCenterY(tkid)*signp;
  coo[2] = -TkDBc::Head->_silicon_z/2-par[2]-sup_foam_tol;

  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("Tr_Foam", _nrot++, name, "BOX",
			   par, 3, coo, nrm, "ONLY", 1, gid, 1));
}

void TrGeom::BuildHoneycomb(AMSgvolume *mvol, int layer)
{
// Build plane honeycomb support geometry (layer number begins from 1)

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "PLA" << layer << std::ends;

  geant par[3];
  par[0] = 0;
  par[1] = sup_hc_r[layer-1];
  par[2] = sup_hc_w[layer-1]/2;

  geant coo[3];
  coo[0] = coo[1] = 0;
  coo[2] = -TkDBc::Head->_silicon_z/2.+sup_hc_z[layer-1];

  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  mvol->add(new AMSgvolume("Tr_Honeycomb", _nrot++, name,
			   "TUBE", par, 3, coo, nrm, "ONLY", 1, layer, 1));
}
