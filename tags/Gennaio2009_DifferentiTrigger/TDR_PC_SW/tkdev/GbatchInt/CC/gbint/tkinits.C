// $Id: tkinits.C,v 1.6 2008/03/17 22:03:34 haino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  tkinits.cc
///\brief Routines to initialize parameters for tracker
/// 
/// Tracker-relapted parts extracted from job.C
///
///\date  2007/11/27 SH  First test version
///\date  2007/11/29 SH  First stable version
///$Date: 2008/03/17 22:03:34 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////

#include "extC.h"      // for TKFINI
#include "dcards.h"    // for FFKEYs
#include "cern.h"      // for Geant3 parameters
#undef GUFLD

#include "Event.h"
#include "TrField.h"
#include "TkDBc.h"
#include "TrCalDB.h"
#include "TrDigi.h"

void sitkinitjob(void);
void signinitjob(void);
void retkinitjob(void);
void timeinitjob(void);

int  InitTrCalDB(void);
void SetFFKEY   (void) {}

void setsetup(char *);

namespace gbtrk {

void cardinit(void)
{
//-------------------------------------
// Tracker-related parts extracted from
// AMSJob::gethead()->data();
//-------------------------------------

  // Set setup
  setsetup("AMS02");

  // Initialize Geant3 parameters
  GCSTAK.NJTMIN=1;
  GCTRAK.upwght=0;
  GCTIME.TIMINT=10000000;
  GCTIME.TIMEND=10;
  GCTIME.ITIME=0;

  // Initialize FFKEYs
  AMSFFKEY   .init();
  CCFFKEY    .init();
  MISCFFKEY  .init();
  TKGEOMFFKEY.init();
  TRMCFFKEY  .init();
  TKFIELD    .init();
  TRCLFFKEY  .init();
  TRFITFFKEY .init();
  MAGSFFKEY  .init();
  TKFINI();
}

void datainit(void)
{
//-------------------------------------
// Tracker-related parts extracted from
// AMSJob::gethead()->udata();
//-------------------------------------

  // Check FFKEY
  if (TRMCFFKEY.gammaA[0] < 0) TRMCFFKEY.gammaA[0]=0.2;
  if (TRMCFFKEY.delta [0] < 0) TRMCFFKEY.delta [0]=0.9;
  if (TRMCFFKEY.thr1R [0] < 0) TRMCFFKEY.thr1R [0]=3;
  if (TRCLFFKEY.Thr1R [0] < 0) TRCLFFKEY.Thr1R [0]=3;

  // Copy FFKEY
  new Event;
  SetFFKEY();

  // Initialize TrField
  TrField *fld = TrField::GetPtr();
  fld->Read("dat/MagneticFieldMap.0.dat");

  // Initialize TKDBc
  TkDBc::CreateTkDBc();
  TkDBc::Head->init();

  // Initialize TrCalDB
  (new TrCalDB)->init();

  InitTrCalDB();
}

void jobinit(void)
{
//-------------------------------------
// Tracker-related parts extracted from
// AMSJob::gethead()->init();
//-------------------------------------

  // Initialize jobs
  sitkinitjob();
  signinitjob();
  retkinitjob();
  timeinitjob();
}

}  // End of namespace gbtrk


//-------------------------------------
// Followings are extracted from job.C
//-------------------------------------

#include "timeid.h"    // for AMSTimeID
#include "mceventg.h"  // for AMSmceventg
#include "job.h"       // for AMSJob

void setsetup(char *setup)
{
  AMSJob::gethead()->setsetup(setup);
}

void sitkinitjob(void)
{
  TRMCFFKEY.year[1]=TRMCFFKEY.year[0]-1;

  AMSgObj::BookTimer.book("GEANTTRACKING");
  AMSgObj::BookTimer.book("GUSTEP",10);
  AMSgObj::BookTimer.book("AMSGUSTEP",10);
  AMSgObj::BookTimer.book("SYSGUSTEP",10);
  AMSgObj::BookTimer.book("TrdRadiationGen",10);
  AMSgObj::BookTimer.book("SITKHITS");
  AMSgObj::BookTimer.book("SITKNOISE");
  AMSgObj::BookTimer.book("SITKDIGIa");
  AMSgObj::BookTimer.book("SITKDIGIb");
  AMSgObj::BookTimer.book("SITKDIGIc");
  AMSgObj::BookTimer.book("TrMCCluster");
  AMSgObj::BookTimer.book("TDV");
}

void signinitjob(void)
{
  AMSgObj::BookTimer.book("SetTimeCoo");

  AMSmceventg::setcuts(CCFFKEY.coo,CCFFKEY.dir,CCFFKEY.momr,
		       CCFFKEY.fixp,CCFFKEY.albedor,CCFFKEY.albedocz);
  //
  AMSmceventg::setspectra(CCFFKEY.begindate,CCFFKEY.begintime,
			  CCFFKEY.enddate, CCFFKEY.endtime, GCKINE.ikine,CCFFKEY.low);
  
}

void retkinitjob(void)
{
  AMSgObj::BookTimer.book("RETKEVENT");
  AMSgObj::BookTimer.book("TrCluster");
  AMSgObj::BookTimer.book("TrClusterRefit");
  AMSgObj::BookTimer.book("TrRecHit");
  AMSgObj::BookTimer.book("TrTrack");
  AMSgObj::BookTimer.book("TrFalseX");
}

void timeinitjob(void)
{
  static AMSTimeID TID(AMSID("TDV:",0));
  AMSJob::gethead()->addup( &TID);

  //
  // Magnetic Field Map
  //
  AMSTimeID::CType server=AMSTimeID::Standalone;
  {
    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec=TKFIELD.isec[0];
    begin.tm_min=TKFIELD.imin[0];
    begin.tm_hour=TKFIELD.ihour[0];
    begin.tm_mday=TKFIELD.iday[0];
    begin.tm_mon=TKFIELD.imon[0];
    begin.tm_year=TKFIELD.iyear[0];
    end.tm_sec=TKFIELD.isec[1];
    end.tm_min=TKFIELD.imin[1];
    end.tm_hour=TKFIELD.ihour[1];
    end.tm_mday=TKFIELD.iday[1];
    end.tm_mon=TKFIELD.imon[1];
    end.tm_year=TKFIELD.iyear[1];

    TID.add (new AMSTimeID(AMSID("MagneticFieldMap",
				 AMSJob::gethead()->isRealData()),
			   begin,end,sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile),
			   (void*)&TKFIELD.iniok,server,1));
  }
}

int readGb(const char *fname, int size, void *ptr)
{
  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "File open error: " << fname << std::endl;
    return -1;
  }
  fin.read((char*)ptr, size);
  std::cout <<"AMSTimeID::read-I-Open file " << fname << std::endl;

  return 0;
}

int InitTrCalDB(void)
{
  enum { NUMEL = 196608, NUMELL = 104448, MS = 4000, NLAY = 8, NLAD = 15};

  const int nstrip[2] = { 384, 640 };

  int   status[NUMEL];
  float ped   [NUMEL];
  float gain  [NUMEL];
  float sigmas[NUMEL];
  float sigmar[NUMEL];

  if (readGb("dat/TrackerPedestals.2l.dat",
	     sizeof(ped[0])*NUMELL, (void*)ped) < 0) return -1;

  if (readGb("dat/TrackerPedestals.2r.dat", 
	    sizeof(ped[0])*(NUMEL-NUMELL),
	     (void*)(ped+NUMELL)) < 0) return -1;

  if (readGb("dat/TrackerRawSigmas.2l.dat",
	    sizeof(sigmar[0])*NUMELL, (void*)sigmar) < 0) return -1;

  if (readGb("dat/TrackerRawSigmas.2r.dat",
	    sizeof(sigmar[0])*(NUMEL-NUMELL), 
	    (void*)(sigmar+NUMELL)) < 0) return -1;

  if (readGb("dat/TrackerGains.2l.dat",
	    sizeof(gain[0])*NUMELL, (void*)gain) < 0) return -1;

  if (readGb("dat/TrackerGains.2r.dat",
	    sizeof(gain[0])*(NUMEL-NUMELL),
	    (void*)(gain+NUMELL)) < 0) return -1;

  if (readGb("dat/TrackerSigmas.2l.dat",
	    sizeof(sigmas[0])*NUMELL, (void*)sigmas) < 0) return -1;

  if (readGb("dat/TrackerSigmas.2r.dat",
	    sizeof(sigmas[0])*(NUMEL-NUMELL),
	    (void*)(sigmas+NUMELL)) < 0) return -1;

  if (readGb("dat/TrackerStatus.2l.dat",
	    sizeof(status[0])*NUMELL, (void*)status) < 0) return -1;

  if (readGb("dat/TrackerStatus.2r.dat",
	    sizeof(status[0])*(NUMEL-NUMELL),
	    (void*)(status+NUMELL)) < 0) return -1;

  if (TrDigi::ReadCmnNoise("dat/TrackerCmnNoise2.0.dat") < 0) return -1;

  for(int oct=0;oct<8;oct++){
    int crate=TkDBc::Head-> _octant_crate[oct];
    for(int tdr=0;tdr<24;tdr++){
      int hwid=crate*100+tdr;
      TrLadCal *tcal = TrCalDB::Head->FindCal_HwId(hwid);
      if (tcal) tcal->SetFilled();
      else cout << "Error tcal not found: " << hwid << endl;
    }
  }

  int num = 0, num1 = 0;
  for (int k = 0; k < 2; k++) {
    for (int i = 0; i < NLAY; i++)
      for (int l = 0; l < 2; l++)
	for (int j = 0; j < NLAD; j++) {
	  int tkid = (k*2-1)*((i+1)*100+j+1);
	  if (!TkDBc::Head->FindTkId(tkid)) continue;

	  TrLadCal *tcal = TrCalDB::Head->FindCal_TkId(tkid);
	  if (tcal && l == 0 && tcal->Sigma(0) != 0) {
	    TkLadder *tlad = TkDBc::Head->FindTkId(tkid);
	    std::cerr << "ERROR OVERWRAP " 
		      << tkid << " " << tlad->GetHwId() << std::endl;
	  }

	  if (tcal) {
	    for (int n = 0; n < nstrip[l]; n++) {
	      int idx = (l == 0) ? nstrip[1]+n : n;
	      tcal->SetPedestal(idx, ped   [num+n]);
	      tcal->SetSigmaRaw(idx, sigmar[num+n]);
	      tcal->SetSigma   (idx, sigmas[num+n]);
	      tcal->SetStatus  (idx, status[num+n]);
	    }
	  }
	  else {
	    TkLadder *tlad = TkDBc::Head->FindTkId(tkid);
	    std::cerr << "ERROR TrLadCal not found: TkID=" << tkid << " "
		      << "HwID= " << tlad->GetHwId() << std::endl;
	  }

	  num += nstrip[l];
	}
    if (k == 0) num1 = num;
  }

  if (num != NUMEL)
    std::cerr << "WARNING: num differs " 
	      << num << " " << NUMEL << std::endl;
  if (num1 != NUMELL)
    std::cerr << "WARNING: num1 differs " 
	      << num1 << " " << NUMELL << std::endl;

  return 0;
}

/*
#include "TrCluster.h"

void SetFFKEY(void)
{
  TrCluster::Analysis = 1;
  TrCluster::BadBits  = TrCluster::BAD;

  TrCluster::NmaxCofG[0] = 0;
  TrCluster::NmaxCofG[1] = 0;

  TrCluster::ThrClA[0] = TRCLFFKEY.ThrClA[0];
  TrCluster::ThrClA[1] = TRCLFFKEY.ThrClA[1];
  TrCluster::ThrClS[0] = TRCLFFKEY.ThrClS[0];
  TrCluster::ThrClS[1] = TRCLFFKEY.ThrClS[1];
  TrCluster::ThrClR[0] = TRCLFFKEY.ThrClR[0];
  TrCluster::ThrClR[1] = TRCLFFKEY.ThrClR[1];
  TrCluster::Thr1A [0] = TRCLFFKEY.Thr1A [0];
  TrCluster::Thr1A [1] = TRCLFFKEY.Thr1A [1];
  TrCluster::Thr1S [0] = TRCLFFKEY.Thr1S [0];
  TrCluster::Thr1S [1] = TRCLFFKEY.Thr1S [1];
  TrCluster::Thr1R [0] = TRCLFFKEY.Thr1R [0];
  TrCluster::Thr1R [1] = TRCLFFKEY.Thr1R [1];
  TrCluster::Thr2A [0] = TRCLFFKEY.Thr2A [0];
  TrCluster::Thr2A [1] = TRCLFFKEY.Thr2A [1];
  TrCluster::Thr2S [0] = TRCLFFKEY.Thr2S [0];
  TrCluster::Thr2S [1] = TRCLFFKEY.Thr2S [1];
  TrCluster::Thr2R [0] = TRCLFFKEY.Thr2R [0];
  TrCluster::Thr2R [1] = TRCLFFKEY.Thr2R [1];
  TrCluster::Thr3R [0] = TRCLFFKEY.Thr3R [0];
  TrCluster::Thr3R [1] = TRCLFFKEY.Thr3R [1];

  TrCluster::ThrClNMin[0] = TRCLFFKEY.ThrClNMin[0];
  TrCluster::ThrClNMin[1] = TRCLFFKEY.ThrClNMin[1];
  TrCluster::ThrClNEl [0] = TRCLFFKEY.ThrClNEl [0];
  TrCluster::ThrClNEl [1] = TRCLFFKEY.ThrClNEl [1];

  TrCluster::ErrX = TRCLFFKEY.ErrX;
  TrCluster::ErrY = TRCLFFKEY.ErrY;
  TrCluster::ErrZ = TRCLFFKEY.ErrZ;

  for (int i = 0; i < 6; i++) {
    TrCluster::CorFunParA[0][i] = TRCLFFKEY.CorFunParA[0][i];
    TrCluster::CorFunParA[1][i] = TRCLFFKEY.CorFunParA[1][i];
    TrCluster::CorFunParB[0][i] = TRCLFFKEY.CorFunParB[0][i];
    TrCluster::CorFunParB[1][i] = TRCLFFKEY.CorFunParB[1][i];
  }

  TrCluster::CommonGain [0] = TRCLFFKEY.CommonGain [0];
  TrCluster::CommonGain [1] = TRCLFFKEY.CommonGain [1];
  TrCluster::CommonShift[0] = TRCLFFKEY.CommonShift[0];
  TrCluster::CommonShift[1] = TRCLFFKEY.CommonShift[1];
}
*/
