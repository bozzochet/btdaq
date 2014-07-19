// $Id: GbatchAMS.C,v 1.1 2008/01/21 21:58:26 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchAMS.cc
///\brief Source file of GbatchAMS
///
///\date  2007/11/22 SH  First test version
///\date  2007/11/26 SH  First stable version
///$Date: 2008/01/21 21:58:26 $
///
///$Revision: 1.1 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchAMS.h"
#include "GbatchDefs.h"

#include "amsdbc.h"   // for amsglobalerror
#include "job.h"      // for AMSJob
#include "gmat.h"     // for AMSgmat and AMSgtmed
#include "gvolume.h"  // for AMSgvolume

ClassImp(GbatchAMS)

GbatchAMS::GbatchAMS(void)
{
  try {
    // Initialize process before FFGO
    InitFirst();
  }

  // Handle exceptions
  catch (amsglobalerror &a){
    cerr << a.getmessage() << endl;
  }
}

GbatchAMS::~GbatchAMS()
{
}

Int_t GbatchAMS::Main(void)
{
//-------------------------------------
// Main routine of gbatch
//-------------------------------------

  Int_t ret = 0;

  try{
    // Initialize simulation
    InitSecond();

    // Run the Simulation 
    StartRun();
  } 

  // Handle exceptions
  catch (amsglobalerror &a){
    cerr << a.getmessage() << endl;
    ret = 1;
  }
  catch (std::bad_alloc aba){
    cerr << "catch-F-NoMemoryAvailable " << endl;
    ret = 1;
  }

  try{
    // Terminate simulation
    Terminate();
  }

  // Handle exceptions
  catch (amsglobalerror &a){
    cerr << a.getmessage() << " in UGLAST" << endl;
    ret = 1;
  }    

  return ret;
}

void GbatchAMS::NewJob(void)
{
  // Create a JOB object
  if (!AMSJob::gethead()) new AMSJob();

  // Setup the datacards (default values and add to FFKEY queue)
  AMSJob::gethead()->data();

  // Set some values according to the datacards readings
  GCTLIT.ITCKOV=1;
  GCPHYS.IRAYL=1;
}

void GbatchAMS::Dcards(void)
{
  // Set some values according to the datacards readings
  AMSJob::gethead()->udata();

  // Initialize the global AMS rotation matrix
  AMSDBc::init();  
}

void GbatchAMS::InitJob(void)
{
  // Initialize AMSJob
  AMSJob::map();

  AMSJob::gethead()->init();
  if (!AMSJob::gethead()->isProduction()) AMSJob::gethead()->uhinit();
}

void GbatchAMS::Remap(void)
{
  // Remap AMS nodes
  AMSJob::map(1);
}

void GbatchAMS::DelJob(void)
{
  // Delete JOB object
  delete AMSJob::gethead();
}


void GbatchAMS::Particles(void)
{
  // Define standard GEANT particles
  GPART();

  // Define a subset of the 'stable' most common elements in the Nature
  GPIONS(4);

  // Define the stranglet particle
  gbint::gstran(4);
}

void GbatchAMS::Physics(void)
{
  // Initialise material constants for all the physics mechanisms
  GPHYSI();
}

void GbatchAMS::Materials(void)
{
  // Define materials for AMS detector
  AMSgmat::amsmat();

  // Define mediums for AMS detector
  AMSgtmed::amstmed();
}

void GbatchAMS::Geometry(void)
{
  // Define geometry AMS detector
  AMSgvolume::amsgeom();
}

extern "C" {
  void gbkine_(void);
  void gbstep_(void);
  void gbout_ (void);
}

void GbatchAMS::UserKine(void)
{
  // Call Gbatch version of GUKINE
  gbkine_();
}

void GbatchAMS::UserStep(void)
{
  // Call Gbatch version of GUSTEP
  gbstep_();
}

void GbatchAMS::UserOut(void)
{
  // Call Gbatch version of GUOUT
  gbout_();
}
