// $Id: GbatchInterface.C,v 1.1 2008/01/21 21:58:26 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchInterface.cc
///\brief Source file of GbatchInterface
///
///\date  2007/11/21 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:26 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchInterface.h"
#include "GbatchDefs.h"

#include "TMath.h"
#include "cern.h"     // for GEANT3 calls
#include "gvolume.h"  // for AMSgvolume

#include <iostream>

GbatchInterface* GbatchInterface::_ptr = 0;

ClassImp(GbatchInterface)

GbatchInterface::GbatchInterface(void)
{
  // Check duplication
  if (_ptr) {
    std::cerr << "Only one GbatchInterface object is allowed." 
	      << std::endl;
    return;
  }

  _status = NOT_INITIALIZED;
  _ptr = this;

  _mother = 0;

  _verbose  = 0;
  _particle = 14;  // Proton
  _momentum = 10;  // in GeV/c

  _vertex[0] =   0;
  _vertex[1] =   0;
  _vertex[2] = 200;  // in cm

  _direction[0] =  0;
  _direction[1] =  0;
  _direction[2] = -1;

  cout.sync_with_stdio();   
}

GbatchInterface::~GbatchInterface()
{
  _ptr = 0;
  delete _mother;
}

void GbatchInterface::SetVertex(Float_t x, Float_t y, Float_t z)
{
  // Set vertex positon in cm
  _vertex[0] = x;
  _vertex[1] = y;
  _vertex[2] = z;
}

void GbatchInterface::SetDirection(Float_t x, Float_t y, Float_t z)
{
  // Set momentum direction, automatically converted to unit vector

  Float_t abs = TMath::Sqrt(x*x+y*y+z*z);
  if (abs == 0) abs = 1;

  _direction[0] = x/abs;
  _direction[1] = y/abs;
  _direction[2] = z/abs;
}

Int_t GbatchInterface::DataCard(const char *str)
{
//-------------------------------------
// Add one datacard
//-------------------------------------

  // Check status
  if (!CheckStatus(GET_DATACARD, "DataCard")) return -1;

  // Call FFONE
  Int_t ret = gbint::ffone(str);

  // Set new status
  if (ret) ChangeStatus(READY_TO_INIT);

  return ret;
}

void GbatchInterface::InitFirst(void)
{
//-------------------------------------
// Initialize process before FFGO
//-------------------------------------

  // Check status
  if (!CheckStatus(NOT_INITIALIZED, "InitFirst")) return;

  // Initialzie zebra
  GZEBRA(NWGEAN);
  HLIMIT(-NWPAW);

  // Initialzie geant
  GINIT();

  // Create a JOB object
  NewJob();

  // Define GEANT/FFREAD data cards
  GFFGOI();

  // Set new status
  ChangeStatus(GET_DATACARD);
}

void GbatchInterface::InitSecond(void)
{
//-------------------------------------
// Initialize process after FFGO
//-------------------------------------

  // Check status
  if (!CheckStatus(READY_TO_INIT, "InitSecond")) return;

  // Read GEANT/FFREAD data cards
  GFFGOF();

  // Set values according to the datacards readings
  Dcards();

  // Initialize GEANT/ZEBRA data structures
  GZINIT();

  // Define particles
  Particles();

  // Define materials and mediums
  Materials();

  // Define geometry
  Geometry();

  // Initialize Job
  InitJob();

  // Initialize physics interfaces
  Physics();

  // Remap nodes
  Remap();

  // Set new status
  ChangeStatus(READY_TO_RUN);
}

void GbatchInterface::StartRun(void)
{
//-------------------------------------
// Run the Geant3 simulation
//-------------------------------------

  // Check status
  if (!CheckStatus(READY_TO_RUN, "StartRun")) return;

  // Run the Simulation 
  GRUN();

  // Set new status
  ChangeStatus(RUN_FINISHED);
} 

void GbatchInterface::Terminate(void)
{
//-------------------------------------
// Terminate the Geant3 simulation
//-------------------------------------

  // Check status
  if (!CheckStatus(RUN_FINISHED, "Terminate")) return;

  // Terminate geant
  GLAST();

  // Delete JOB object
  DelJob();

  // Set new status
  ChangeStatus(TERMINATED);
}

/*
void GbatchInterface::Gvolume(const char *name,   const char *mother, 
			      const char *matter, const char *shape,
			      float *par, int npar, float *pos, float *rot, 
			      int copy, int option)
{
  number nrm[3][3];
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++) nrm[i][j] = rot[i*3+j];

  const char *only = (option&GVOL_ONLY) ? "ONLY" : "MANY";
  integer     posp = (option&GVOL_POSP) ? 1 : 0;
  integer     rel  = (option&GVOL_REL)  ? 1 : 0;
  integer     nrot =  option/GVOL_NROT;

  AMSgvolume *gvol = new AMSgvolume(matter, nrot, name, shape, par, npar,
				    pos, nrm, only, posp, copy, rel);
}
*/

Bool_t GbatchInterface::CheckStatus(EStatus status, const char *name)
{
  if (_status != status) {
    cerr << name << ": Wrong status: " << _status 
	 << " != " << status << endl;
    return kFALSE;
  }

  return kTRUE;
}

void GbatchInterface::Particles(void)
{
  // Define standard GEANT particles
  GPART();
}

void GbatchInterface::Physics(void)
{
  // Initialise material constants for all the physics mechanisms
  GPHYSI();
}

void GbatchInterface::UserKine(void)
{
//-------------------------------------
// Set primary kinematics
//-------------------------------------

  // Set primary vertex
  geant vrtx[3];
  vrtx[0] = _vertex[0];
  vrtx[1] = _vertex[1];
  vrtx[2] = _vertex[2];

  integer nv = 0;
  GSVERT(vrtx, 0, 0, 0, 0, nv);

  // Set primary particle and momentum
  geant plab[3];
  plab[0] = _direction[0]*_momentum;
  plab[1] = _direction[1]*_momentum;
  plab[2] = _direction[2]*_momentum;

  integer nt = 0, ipart = _particle;
  GSKINE(plab, ipart, nv, 0, 0, nt);
}

void GbatchInterface::UserStep(void)
{
//-------------------------------------
// Called at each step
//-------------------------------------

  // Print step
  if (_verbose >= 2) GPCXYZ();
}

void GbatchInterface::UserOut(void)
{
//-------------------------------------
// Called at the end of event
//-------------------------------------
}

void GbatchInterface::Gukine(void)
{
//-------------------------------------
// Geant3 user call GUKINE
//-------------------------------------

  if (_ptr) _ptr->UserKine();
}

void GbatchInterface::Gustep(void)
{
//-------------------------------------
// Geant3 user call GUSTEP
//-------------------------------------

  if (_ptr) _ptr->UserStep();
}

void GbatchInterface::Guout(void)
{
//-------------------------------------
// Geant3 user call GUOUT
//-------------------------------------

  if (_ptr) _ptr->UserOut();
}
