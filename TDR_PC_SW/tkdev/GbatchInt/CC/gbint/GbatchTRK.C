// $Id: GbatchTRK.C,v 1.4 2008/04/02 11:34:41 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  GbatchTRK.cc
///\brief Source file of GbatchTRK
///
///\date  2007/11/24 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/04/02 11:34:41 $
///
///$Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////////

#include "GbatchTRK.h"
#include "TrField.h"
#include "TkDBc.h"
#include "tkinits.h"
#include "tkgeom.h"
#include "tkcalls.h"

#include "job.h"      // for AMSJob

#include <iostream>
#include <fstream>

ClassImp(GbatchTRK)

GbatchTRK::GbatchTRK(void) : _nevent(0)
{
  // Initialize simulation
  InitFirst();
}

GbatchTRK::~GbatchTRK()
{
}

Int_t GbatchTRK::Main(void)
{
//-------------------------------------
// Main routine of gbatch
//-------------------------------------

  // Initialize simulation
  InitSecond();

  // Pre-run action
  PreRun();

  // Run the Simulation 
  StartRun();

  // Post-run action
  PostRun();

  // Terminate simulation
  Terminate();

  return 0;
}

Int_t GbatchTRK::GetRecCodes(void)
{
  // Get reconstruction codes
  return (Int_t)gbtrk::reccode;
}

void GbatchTRK::SetRecCodes(Int_t c)
{
  // Set reconstruction codes
  if (c == gbtrk::GBATCH || c == gbtrk::TKREC || c == gbtrk::TKRAW)
    gbtrk::reccode = (gbtrk::ERecCodes)c;
}

Int_t GbatchTRK::GetRecDebug(void)
{
  // Get debug message level of tkrec
  return gbtrk::debug;
}

void GbatchTRK::SetRecDebug(Int_t d)
{
  // Set debug message level of tkrec
  gbtrk::debug = d;
}

Int_t GbatchTRK::InitBfield(const char *fname)
{
  // Initialize magnetic field map (in AMS format)
  TrField *fld = TrField::GetPtr();
  return fld->Read(fname);
}

Int_t GbatchTRK::DisableLadders(const char *fname)
{
  std::ifstream fin(fname);
  if (!fin) return -1;

  int tkid, n = 0;;
  for (;;) {
    fin >> tkid;
    if (fin.eof()) break;
    TkDBc::Head->KillTkId(tkid);
    n++;
  }

  return n;
}

Event *GbatchTRK::GetEvent(void)
{
  return gbtrk::event;
}

void GbatchTRK::SetEvent(Event *evt)
{
  gbtrk::event = evt;
}

void GbatchTRK::NewJob(void)
{
  // Create a JOB object
  if (!AMSJob::gethead()) new AMSJob();

  // Setup the datacards (default values and add to FFKEY queue)
  gbtrk::cardinit();

  // Set some values according to the datacards readings
  GCTLIT.ITCKOV=1;
  GCPHYS.IRAYL=1;
}

void GbatchTRK::Dcards(void)
{
  // Set some values according to the datacards readings
  gbtrk::datainit();

  // Initialize the global AMS rotation matrix
  AMSDBc::init();  

  // Pre-init action
  PreInit();
}

void GbatchTRK::InitJob(void)
{
  // Map AMS nodes
  AMSJob::map();

  // Initialize jobs
  gbtrk::jobinit();
}

void GbatchTRK::Remap(void)
{
  // Remap AMS nodes
  AMSJob::map(1);
}

void GbatchTRK::DelJob(void)
{
  // Delete JOB object
  delete AMSJob::gethead();
}

void GbatchTRK::Materials(void)
{
  // Define materials
  gbtrk::amsmat();

  // Define mediums
  gbtrk::amstmed();
}

void GbatchTRK::Geometry(void)
{
  // Build tracker geometry
  gbtrk::amsgeom();
}

void GbatchTRK::UserKine(void)
{
  // Event initialization for tracker
  gbtrk::tkkine();

  // Pre-event action
  PreEvent();

  // Set primary kinematics
  GbatchInterface::UserKine();
}

void GbatchTRK::UserStep(void)
{
  // Call default method
  GbatchInterface::UserStep();

  // Cluster generation for tracker
  gbtrk::tkstep();
}

void GbatchTRK::UserOut(void)
{
  // Call default method
  GbatchInterface::UserOut();

  // Digitization and reconstruction for tracker
  gbtrk::tkout();

  // Post-event action
  PostEvent();

  // Clear event
  gbtrk::tkclr();

  // Show status
  if (++_nevent%1000 == 0)
    std::cout << _nevent << " events processed." << std::endl;
}
