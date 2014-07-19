#include "RTDisplay3D.h"
#include "RTGLViewer.h"

#include "Event.h"
#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"
#include "TrFit.h"
#include "TrField.h"

#include "TCanvas.h"
#include "TObjArray.h"
#include "TMath.h"
#include "TString.h"
#include "TSystem.h"

#include "TGeoManager.h"
#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TGLViewer.h"

#include "TPluginManager.h"

#include <iostream>
#include <fstream>

int RTrackerDisplay3D::Option 
  = RTrackerDisplay3D::TRKHIT | RTrackerDisplay3D::ANYTOF | 
    RTrackerDisplay3D::TOFHIT;

RTrackerDisplay3D *RTrackerDisplay3D::fSelfPtr = 0;

void RTrackerDisplay3D::RunDisplay3D(const char *option)
{
  if (RTGLViewer::GetPtr()) {
    std::cout << "RTGLViewer is already runnning" << std::endl;
    return;
  }

  if (!TkDBc::Head) {
    TkDBc::CreateTkDBc();
    TkDBc::Head->init();
  }

  if (!fSelfPtr) new RTrackerDisplay3D;
  if (!gGeoManager || !gGeoManager->IsClosed()) {
    fSelfPtr->BuildGeom();
    gPluginMgr->AddHandler("TVirtualViewer3D", "rtogl", "RTGLViewer",
			   "RTGL", "RTGLViewer(TVirtualPad*)");
  }

  fSelfPtr->fCanvas = new TCanvas("");

  TGLViewer*glv = (TGLViewer*)fSelfPtr->fCanvas->GetViewer3D(option);
  glv->SetResetCamerasOnUpdate(kFALSE);
  glv->SetCurrentCamera(TGLViewer::kCameraPerspXOY);

  Double_t center[3] = { 0, 0, 0 };
  glv->ResetCameras();
  glv->SetPerspectiveCamera(TGLViewer::kCameraPerspXOY, 10, 20, center,
			    -TMath::DegToRad()*20, TMath::DegToRad()*150);

  if (glv->ClassName() == TString("RTGLViewer"))
    ((RTGLViewer *)glv)->Entry();
}

void RTrackerDisplay3D::DrawOneEvent(Event *event)
{
  if (!event) event = Event::currev;
  if (fSelfPtr && event) 
    fSelfPtr->DrawEvent(event);
}

void RTrackerDisplay3D::DrawOneTrack(TrProp *trp)
{
  if (fSelfPtr) fSelfPtr->DrawTrack(trp);
}

void RTrackerDisplay3D::PrintEvent(const char *pname)
{
  if (fSelfPtr && fSelfPtr->fCanvas)
    fSelfPtr->fCanvas->SaveAs(pname);
}


RTrackerDisplay3D::RTrackerDisplay3D(void) : LineArray(0), fCanvas(0)
{
  fSelfPtr = this;
}

RTrackerDisplay3D::~RTrackerDisplay3D()
{
}

void RTrackerDisplay3D::BuildGeom(void)
{
  if (!gGeoManager) new TGeoManager();

  TGeoVolume *mere = gGeoManager->MakeBox("MERE", 0, 600, 600, 600);
  mere->SetVisibility(kTRUE);
  gGeoManager->SetTopVolume(mere);

  TGeoVolume *tmere = gGeoManager->MakeBox("TMERE", 0, 600, 600, 600);
  tmere->SetVisibility(kTRUE);

  mere->AddNode(tmere, 1, new TGeoTranslation(0, 0, 0));

  BuildTracker(tmere);
  BuildTof(tmere);

  gGeoManager->CloseGeometry();
}

void RTrackerDisplay3D::BuildTracker(TGeoVolume *tmere)
{
  for (int i = 0; i < 512; i++)
    PxLadLine[i] = PyLadLine[i] = PzLadLine[i] = 0;

  int ent = TkDBc::Head->GetEntries();
  for (int i = 0; i < ent; i++) {
    TkLadder *ladder = TkDBc::Head->GetEntry(i);
    int tkid  = ladder->GetTkId();
    int slot  = ladder->GetSlot();
    int layer = ladder->GetLayer();
    double dx = TkCoo::GetLadderLength(ladder->GetTkId())/2;
    double dy = TkDBc::Head->_ssize_inactive[1]/2;
    double dz = 0.2;

    TGeoVolume *gvol 
      = gGeoManager->MakeBox(Form("lad%04d", tkid), 0, dx, dy, dz);
    gvol->SetLineColor(3);
    gvol->SetVisibility(kTRUE);

    double x = TkCoo::GetLadderCenterX(tkid);
    double y = TkCoo::GetLadderCenterY(tkid);
    double z = TkDBc::TkDBc::Head->GetZlayer(layer);
    tmere->AddNode(gvol, 1, new TGeoTranslation(x, y, z));

    int j = (layer-1)*64+(slot+16)*2;
    PxLadLine[j] = x+TMath::Sign(dx, x);
    PyLadLine[j] = y+TMath::Sign(dy, x); PzLadLine[j] = z; j++;
    PxLadLine[j] = x+TMath::Sign(dx, x); 
    PyLadLine[j] = y-TMath::Sign(dy, x); PzLadLine[j] = z;

    if (!TkDBc::Head->FindTkId(-tkid)) {
      j = (layer-1)*64+(-slot+16)*2;
      PxLadLine[j] = x-TMath::Sign(dx, x);
      PyLadLine[j] = y-TMath::Sign(dy, x); PzLadLine[j] = z; j++;
      PxLadLine[j] = x-TMath::Sign(dx, x); 
      PyLadLine[j] = y+TMath::Sign(dy, x); PzLadLine[j] = z;
    }
  }

  for (int i = 0; i < 8; i++) {
    int n = 0;
    for (int j = 0; j < 64; j++)
      if (PzLadLine[i*64+j] != 0) {
	PxLadLine[i*64+n] = PxLadLine[i*64+j];
	PyLadLine[i*64+n] = PyLadLine[i*64+j];
	PzLadLine[i*64+n] = PzLadLine[i*64+j]; n++;
      }
    NpLadLine[i] = n+1;
    PxLadLine[i*64+n] = PxLadLine[i*64]; PyLadLine[i*64+n] = PyLadLine[i*64];
    PzLadLine[i*64+n] = PzLadLine[i*64];
  }

  for (int i = 0; i < 5; i++) {
    TkPlane* plan = TkDBc::Head->GetPlane(i+1);
    double rmax = TkDBc::Head->_sup_hc_r[i];
    double dz   = TkDBc::Head->_sup_hc_w[i]/2.;
    TGeoVolume *gvol 
      = gGeoManager->MakeTube(Form("plane%d", i+1), 0, 0, rmax, dz);
    gvol->SetVisibility(Option & TRKSUP);
    gvol->SetLineColor(16);

    double x = plan->pos[0]+plan->GetPosA()[0];
    double y = plan->pos[1]+plan->GetPosA()[1];
    double z = plan->pos[2]+plan->GetPosA()[2];
    tmere->AddNode(gvol, 1, new TGeoTranslation(x, y, z));
  }
}

void RTrackerDisplay3D::BuildTof(TGeoVolume *tmere)
{
  // Check Tof2DBc
  if (TrTof::_planes == 0 && TrTof::tofdbc() < 0) return;
  TrTof::setdbpars();

  // Check TofCal
  if (!TrTof::tofped && TrTof::tofcal() < 0) return;

  for (int ip = 0; ip < TrTof::_planes; ip++){
    int nbm = TrTof::_bperpl[ip];

    for (int ib = 0; ib < nbm; ib++) {
      AMSPoint pp = TrTof::getpos (ip, ib);
      AMSPoint ss = TrTof::getsize(ip, ib);
      TString vname = Form("TF%03d", 100*(ip+1)+ib+1);

      TGeoVolume *gvol = (ib == 0 || ib == nbm-1) ? BuildExtTof(ip, vname)
	: gGeoManager->MakeBox(vname, 0, ss[0], ss[1], ss[2]);
      TGeoRotation *grot = new TGeoRotation;
      if (!TrTof::_plrotm[ip]) {
	if (ib == 0) grot->RotateY(180);
      }
      else {
	if (ib == nbm-1) grot->RotateZ( 90);
	else             grot->RotateZ(-90);
      }
      gvol->SetLineColor(5);
      gvol->SetVisibility(kTRUE);
      tmere->AddNode(gvol, 1, new TGeoCombiTrans(pp[0], pp[1], pp[2], grot));
    }
  }
}

#include "TGeoArb8.h"
#include "TGeoBBox.h"
#include "TGeoCompositeShape.h"

TGeoVolume *RTrackerDisplay3D::BuildExtTof(int ip, const char *vname)
{
  double len = TrTof::_brlen[TrTof::brtype(ip, 0)-1];
  double wid = TrTof::_outcp[ip][0];
  double xc  = TrTof::_outcp[ip][1];
  double bx  = (wid-xc)/2;
  double by  = len/2;
  double bz  = TrTof::_plnstr[6-1]/2;

  new TGeoArb8(Form("TF%dARB8", ip+1), bz, &TrTof::exvert[16*ip]);
  new TGeoBBox(Form("TF%dBBOX", ip+1), bx, by, bz);

  (new TGeoTranslation(Form("TF%dt1", ip+1), 0, 0, 0))
    ->RegisterYourself();
  (new TGeoTranslation(Form("TF%dt2", ip+1), -xc/2, 0, 0))
    ->RegisterYourself();

  TString str = Form("TF%dARB8:TF%dt1+TF%dBBOX:TF%dt2", 
		     ip+1, ip+1, ip+1, ip+1);
  return new TGeoVolume(vname, 
			new TGeoCompositeShape(vname, str.Data()));
}

void RTrackerDisplay3D::DrawEvent(Event *event)
{
  DrawTrackerHits(event);
  DrawTofHits(event);

  for (int i = 0; event && i < event->NTrTracks; i++)
    DrawTrack(event->GetTrTrack(i), 2);

  fCanvas->cd();
  fCanvas->Clear();

  TGeoNode* node = gGeoManager->GetTopVolume()->FindNode("TMERE_1");
  if (node) node->Draw();
  DrawLines();

  fCanvas->Update();
}

void RTrackerDisplay3D::DrawTrack(TrTrackR *trk, int col, int wid, int sty)
{
  int fid = TrTrackR::DefaultFitID;//kChoutko;
  if (!trk->ParExists(fid)) fid = TrTrackR::kLinear;

  if (fid == TrTrackR::kLinear) {
    double z[2] = { -70, 70 };
    double p0z  =   trk->GetP0z(fid);
    double x[2] = { trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[0]-p0z), 
		    trk->GetP0x(fid)+trk->GetThetaXZ(fid)*(z[1]-p0z) };
    double y[2] = { trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[0]-p0z), 
		    trk->GetP0y(fid)+trk->GetThetaYZ(fid)*(z[1]-p0z) };
    DrawLine(2, x, y, z, col, wid, sty);
  }
  else {
    TrProp trp(trk->GetP0(fid), trk->GetDir(fid), trk->GetRigidity(fid));
    DrawTrack(&trp, col, wid, sty);
  }
}

void RTrackerDisplay3D::DrawTrack(TrProp *trp, int col, int wid, int sty)
{
  if (trp->GetRigidity() != 0 && !MAGFIELDFFKEY.magstat) {
    TString mfile = "$AMSDataDir/DataBase/MagneticFieldMap07/"
                    "1104793200/MagneticFieldMap07.0.1191837815";
    gSystem->ExpandPathName(mfile);
    if (TrField::GetPtr()->Read(mfile) > 0) MAGFIELDFFKEY.magstat = 1;
  }

  double zlist[14];
  for (int i = 0; i < 14; i++) zlist[i] = 70-10*(i+0.5);

  AMSPoint plist[14];
  trp->Interpolate(14, zlist, plist);
  DrawLine(14, plist, col, wid, sty);
}

void RTrackerDisplay3D::DrawTrackerHits(Event *event)
{
  if (!(Option & ALLTRK)) {
    for (int i = 0; i < 8; i++)
      DrawLine(NpLadLine[i], &PxLadLine[i*64], 
	       &PyLadLine[i*64], &PzLadLine[i*64], 3);
  }

  for (int i = 1; i <= 5; i++) {
    TGeoVolume *gvol = gGeoManager->FindVolumeFast(Form("plane%d", i)); 
    if (gvol) gvol->SetVisibility(Option & TRKSUP);
  }

  int ladmap1[8*35];
  int ladmap2[8*35];
  for (int i = 0; i < 8*35; i++) ladmap1[i] = ladmap2[i] = 0;

  for (int i = 0; event && i < event->NTrClusters; i++) {
    TrClusterR *cls = event->GetTrCluster(i);
    if (!cls) continue;
    int tkid = cls->GetTkId();
    int side = cls->GetSide();
    int lyr  = cls->GetLayer();
    int slot = tkid%100;
    int id   = (lyr-1)*35+slot+16;
    if (side == 0) ladmap1[id]++;
    else ladmap2[id]++;
    if (!(Option & TRKHIT)) continue;

    double x  = TkCoo::GetLadderCenterX(tkid);
    double y  = TkCoo::GetLadderCenterY(tkid);
    double z  = TkDBc::TkDBc::Head->GetZlayer(lyr);
    double dx = TkCoo::GetLadderLength(tkid)/2;
    double dy = TkDBc::Head->_ssize_inactive[1]/2;

    cls->BuildCoordinates();
    double px[2], py[2], pz[4];
    for (int j = 0; j < cls->GetMultiplicity(); j++) {
      double coo = cls->GetGCoord(j);
      px[0] = (side == 0) ? coo : x-dx;
      px[1] = (side == 0) ? coo : x+dx;
      py[0] = (side == 1) ? coo : y-dy;
      py[1] = (side == 1) ? coo : y+dy;
      pz[0] = pz[1] = z+0.3; pz[2] = pz[3] = z-0.3;
      DrawLine(2, px, py, pz,   7, 2);
      DrawLine(2, px, py, pz+2, 7, 2);
    }
  }

  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 35; j++) {
      int lyr  = i+1;
      int slot = j-16;
      int tkid = TMath::Sign(lyr*100+TMath::Abs(slot), slot);
      if (!TkDBc::Head->FindTkId(tkid)) continue;

      int col = 3;
      if (ladmap1[i*35+j]) col = 2;
      if (ladmap2[i*35+j]) col = 4;
      if (ladmap1[i*35+j] && ladmap2[i*35+j]) col = 6;

      if (col != 3 || ((Option & ALLTRK) && (Option & NUMTRK))) {
	double x  = TkCoo::GetLadderCenterX(tkid);
	double y  = TkCoo::GetLadderCenterY(tkid);
	double z  = TkDBc::TkDBc::Head->GetZlayer(lyr);
	double dx = TkCoo::GetLadderLength(tkid)/2-2;
	double dy = TkDBc::Head->_ssize_inactive[1]/2;
	DrawDigit(x+TMath::Sign(dx, x), y+dy-1.5, z-0.3, tkid, -1, 1.5, 10);
	DrawDigit(x+TMath::Sign(dx, x), y+dy-1.5, z+0.3, tkid,  1, 1.5, 10);
      }

      TGeoVolume *vlad = gGeoManager->FindVolumeFast(Form("lad%04d", tkid));
      if (vlad) {
	vlad->SetLineColor(col);
	if (col == 3) vlad->SetVisibility(Option & ALLTRK);
	else          vlad->SetVisibility(kTRUE);
      }
    }
  }
}

void RTrackerDisplay3D::DrawTofHits(Event *event)
{
  if (!(Option & ALLTOF) && (Option & ANYTOF)) {
    DrawLine(9, TrTof::tofpxu, TrTof::tofpyu, TrTof::tofpzu, 5);
    DrawLine(9, TrTof::tofpxl, TrTof::tofpyl, TrTof::tofpzl, 5);
  }

  TrTof *tof = (event) ? &event->TofData : 0;
  if (tof) tof->Reconstruct();

  for (int ip = 0; ip < TrTof::_planes; ip++) {
    for (int ib = 0; ib < TrTof::_bperpl[ip]; ib++) {
      TGeoVolume *gvol = gGeoManager
	->FindVolumeFast(Form("TF%03d", 100*(ip+1)+ib+1));

      int hit = (tof && (tof->AdcHit[ip] & (1<<ib))) ? 1 : 0;
      if (((Option & ANYTOF) && hit) || 
	  ((Option & ALLTOF) && (Option & NUMTOF))) {
	double xo = 0;
	if (ib == 0) xo = 5;
	if (ib == TrTof::_bperpl[ip]-1) xo = -5;

	AMSPoint pp = TrTof::getpos (ip, ib);
	AMSPoint ss = TrTof::getsize(ip, ib);
	double x1 = (TrTof::_plrotm[ip] == 0) ? pp[0]-5+xo : pp[0]-ss[1]+3;
	double x2 = (TrTof::_plrotm[ip] == 0) ? pp[0]-5+xo : pp[0]+ss[1]-3;
	double y1 = (TrTof::_plrotm[ip] == 1) ? pp[1]+5+xo : pp[1]-ss[1]+3;
	double y2 = (TrTof::_plrotm[ip] == 1) ? pp[1]+5+xo : pp[1]+ss[1]-3;
	double z  = pp[2];
	int md = (TrTof::_plrotm[ip]) ? 1 : 2;
	DrawDigit(x1, y1, z-0.7, (ip+1)*100+ib+1, -md, 2.5, 4);
	DrawDigit(x1, y1, z+0.7, (ip+1)*100+ib+1,  md, 2.5, 4);
	DrawDigit(x2, y2, z-0.7, (ip+1)*100+ib+1, -md, 2.5, 4);
	DrawDigit(x2, y2, z+0.7, (ip+1)*100+ib+1,  md, 2.5, 4);
      }
      if (gvol) {
	if ((hit && (Option & ANYTOF)) || (Option & ALLTOF)) 
	  gvol->SetVisibility(kTRUE);
	else gvol->SetVisibility(kFALSE);
      }

      if ((Option & ANYTOF) && (Option & TOFHIT) && hit && tof) {
	double vpos = tof->Vpos[ip*10+ib];
	double vrng = TrTof::_brlen[TrTof::brtype(ip, ib)-1]/2;
	if (-vrng < vpos && vpos < vrng) {
	  int rot = TrTof::_plrotm[ip];
	  double xx[5], yy[5], zz[10];
	  double dv = 2.5;
	  AMSPoint pp = TrTof::getpos(ip, ib);
	  double v1 = vpos-dv, v2 = vpos+dv;
	  xx[0] =  (rot) ? v1 : pp[0]-TrTof::getwid(ip, ib, v1, -1)+1;
	  yy[0] = (!rot) ? v1 : pp[1]-TrTof::getwid(ip, ib, v1, -1)+1;
	  xx[1] =  (rot) ? v2 : pp[0]+TrTof::getwid(ip, ib, v1,  1)-1;
	  yy[1] = (!rot) ? v1 : pp[1]-TrTof::getwid(ip, ib, v2, -1)+1;
	  xx[2] =  (rot) ? v2 : pp[0]+TrTof::getwid(ip, ib, v2,  1)-1;
	  yy[2] = (!rot) ? v2 : pp[1]+TrTof::getwid(ip, ib, v2,  1)-1;
	  xx[3] =  (rot) ? v1 : pp[0]-TrTof::getwid(ip, ib, v2, -1)+1;
	  yy[3] = (!rot) ? v2 : pp[1]+TrTof::getwid(ip, ib, v1,  1)-1;
	  xx[4] = xx[0]; yy[4] = yy[0];
	  for (int kk = 0; kk < 5; kk++) {
	    zz[kk] = pp[2]-0.7; zz[kk+5] = pp[2]+0.7;
	  }
	  DrawLine(5, xx, yy, zz,   2, 2);
	  DrawLine(5, xx, yy, zz+5, 2, 2);
	}
      }
    }
  }
}

#include "TPolyLine3D.h"

void RTrackerDisplay3D::DrawLine(int n, double *x, double *y, double *z,
				 int col, int wid, int sty)
{
  if (!LineArray) LineArray = new TObjArray;

  TPolyLine3D *line = new TPolyLine3D(n, x, y, z);
  line->SetLineColor(col);
  line->SetLineWidth(wid);
  line->SetLineStyle(sty);
  LineArray->Add(line);
}

void RTrackerDisplay3D::DrawLine(int n, AMSPoint *plist, 
				 int col, int wid, int sty)
{
  double *x = new double[n];
  double *y = new double[n];
  double *z = new double[n];

  for (int i = 0; i < n; i++) {
    x[i] = plist[i].x();
    y[i] = plist[i].y();
    z[i] = plist[i].z();
  }
  DrawLine(n, x, y, z, col, wid, sty);

  delete [] x;
  delete [] y;
  delete [] z;
}

void RTrackerDisplay3D::DrawLines(void)
{
  if (!LineArray) LineArray = new TObjArray;

  LineArray->Draw();
  LineArray->Clear();
}

void RTrackerDisplay3D::DrawDigit(double x0, double y0, double z0, int digit, 
				  int mode, double size, int col, int wid)
{
  if (digit < -999 || 999 < digit) {
    std::cout << "Sorry, digit out of range: " << digit << std::endl;
    return;
  }

  double step = -size;
  if (digit < 0) {
    DrawNum(x0, y0, z0, -1, mode, size, col, wid);
    digit = -digit;
  }
  double sx = (TMath::Abs(mode) == 2) ? -step : 0;
  double sy = (TMath::Abs(mode) == 1) ?  step : 0;
  x0+= sx; y0+= sy;
  DrawNum(x0, y0, z0, (digit/100)%10, mode, size, col, wid); x0+= sx; y0+= sy;
  DrawNum(x0, y0, z0, (digit/ 10)%10, mode, size, col, wid); x0+= sx; y0+= sy;
  DrawNum(x0, y0, z0, (digit    )%10, mode, size, col, wid);
}

void RTrackerDisplay3D::DrawNum(double x0, double y0, double z0, 
				int num, int mode, 
				double size, int col, int wid)
{
  if (num < -1 || 9 < num) return;

  double scl = size*0.01;

  double x[30], y[30], z[30];
  for (int j = 0; j < NumDataN[num+1]; j++) {
    int jj = NumDataI[num+1]+j;
    x[j] = (TMath::Abs(mode) == 2) ? x0+NumDataX[jj]*scl 
                                   : x0+NumDataY[jj]*scl*mode;
    y[j] = (TMath::Abs(mode) == 2) ? y0+NumDataY[jj]*scl*mode/2
                                   : y0-NumDataX[jj]*scl;
    z[j] = z0;
  }
  DrawLine(NumDataN[num+1], x, y, z, col, wid);
}


int RTrackerDisplay3D::NumDataN[11] 
 = { 2, 17, 4, 14, 15, 4, 17, 23, 4, 29, 23 };
int RTrackerDisplay3D::NumDataI[11] 
 = { 0, 2, 19, 23, 37, 52, 56, 73, 96, 100, 129 };
int RTrackerDisplay3D::NumDataX[152] 
= { -50,  50,  -5, -21, -31, -37, -37, -31, -21,  -5,   6,  21,  32,  37,  37,
     32,  21,   6,  -5, -13,  -2,  13,  13, -32, -32, -26, -21, -11,  11,  21,
     26,  32,  32,  26,  16, -37,  37, -27,  32,   0,  16,  26,  32,  37,  37,
     32,  21,   5, -11, -27, -32, -37,  14,  14, -40,  40,  26, -26, -32, -26,
    -10,   6,  22,  32,  38,  38,  32,  22,   6, -10, -26, -32, -38,  29,  24,
      8,  -3, -19, -29, -34, -34, -29, -19,  -3,   3,  19,  29,  34,  34,  29,
     19,   3,  -3, -19, -29, -34, -16,  37, -37,  37, -11, -26, -32, -32, -26,
    -16,   5,  21,  32,  37,  37,  32,  27,  11, -11, -26, -32, -37, -37, -32,
    -21,  -5,  16,  27,  32,  32,  27,  11, -11,  34,  28,  18,   2,  -2, -18,
    -30, -34, -34, -30, -18,  -2,   2,  18,  28,  34,  34,  28,  18,   2,  -8,
    -24, -30 };
int RTrackerDisplay3D::NumDataY[152] 
= {   0,   0,  56,  50,  34,   8,  -8, -34, -50, -56, -56, -50, -34,  -8,   8,
     34,  50,  56,  56,  34,  40,  56, -56,  29,  34,  45,  50,  56,  56,  50,
     45,  34,  24,  13,  -3, -56, -56,  56,  56,  13,  13,   8,   3, -13, -24,
    -40, -50, -56, -56, -50, -45, -34, -56,  56, -19, -19,  56,  56,   8,  13,
     19,  19,  13,   3, -13, -24, -40, -50, -56, -56, -50, -45, -34,  40,  50,
     56,  56,  50,  34,   8, -19, -40, -50, -56, -56, -50, -40, -24, -19,  -3,
      8,  13,  13,   8,  -3, -19, -56,  56,  56,  56,  56,  50,  40,  29,  19,
     13,   8,   3,  -8, -19, -34, -45, -50, -56, -56, -50, -45, -34, -19,  -8,
      3,   8,  13,  19,  29,  40,  50,  56,  56,  19,   3,  -8, -13, -13,  -8,
      3,  19,  24,  40,  50,  56,  56,  50,  40,  19,  -8, -34, -50, -56, -56,
    -50, -40 };
