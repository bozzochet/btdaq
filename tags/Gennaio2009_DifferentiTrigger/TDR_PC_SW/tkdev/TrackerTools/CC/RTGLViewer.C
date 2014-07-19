#include "RTGLViewer.h"
#include "RTDisplay3D.h"
#include "Event.h"
#include "TrTrack.h"
#include "TrFit.h"

#include "TApplication.h"
#include "TFile.h"
#include "TTree.h"

#include "TGLWidget.h"
#include "TGButton.h"
#include "TGNumberEntry.h"
#include "TGTextView.h"
#include "TGFileDialog.h"


ClassImp(RTGLViewer)
ClassImp(RTConnector)

RTGLViewer *RTConnector::fViewer = 0;

Bool_t RTGLViewer::fIsStandAlone = kTRUE;
RTGLViewer *RTGLViewer::fSelfPtr = 0;

RTGLViewer::RTGLViewer(TVirtualPad *pad) : TGLViewer(pad, 0, 0, 600, 600)
{
  fFile  = 0;
  fTree  = 0;
  fEvent = 0;
  fConn  = new RTConnector(this);
  fFrame = new TGMainFrame(gClient->GetRoot());
  fFrame->Connect("CloseWindow()", "RTGLViewer", this, "Exit()");

  CreateFrames();

  fFrame->SetWindowName("RTGLVierwer");
  fFrame->SetClassHints("GLViewer", "GLViewer");
  fFrame->SetMWMHints(kMWMDecorAll, kMWMFuncAll, kMWMInputModeless);
  fFrame->MapSubwindows();

  fFrame->MoveResize(0, 0, 720, 600);

  Show();

  fSelfPtr = this;
}

RTGLViewer::~RTGLViewer()
{
  delete fFrame;
}

Int_t RTGLViewer::GetEntry(void)
{
  return fEntry->GetIntNumber();
}

void RTGLViewer::SetEntry(Int_t entry)
{
  fEntry->SetIntNumber(entry);
}

void RTGLViewer::SetMax(Int_t max)
{
  fEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, max);
}

TGButton *RTGLViewer::GetButton(Int_t id)
{
  TGButton *btn;
  for (TIter next(&fBarray); (btn = (TGButton*)next());)
    if (btn->WidgetId() == id) return btn;
  return 0;
}

void RTGLViewer::CreateFrames()
{
  TGCompositeFrame *frame0 
    = new TGCompositeFrame(fFrame, 100, 100, kHorizontalFrame | kRaisedFrame);
  fFrame->AddFrame(frame0, new TGLayoutHints(kLHintsExpandX | 
					     kLHintsExpandY));
  TGCompositeFrame *frame1
    = new TGVerticalFrame(frame0, 120, 10, kFixedWidth);
  frame0->AddFrame(frame1, new TGLayoutHints(kLHintsLeft | 
					     kLHintsExpandY , 3, 3, 2, 2));

  TGGroupFrame *frame11 = new TGGroupFrame(frame1, "File control");
  frame11->AddFrame(new TGTextButton(frame11, "Open",  "RTGLViewer::Open()"),
		    new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  frame11->AddFrame(new TGTextButton(frame11, "Print", "RTGLViewer::Print()"),
		    new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  frame11->AddFrame(new TGTextButton(frame11, "Exit",  "RTGLViewer::Exit()"),
		    new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  frame1->AddFrame(frame11, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));

  TGGroupFrame *frame12 = new TGGroupFrame(frame1, "Event control");
  frame12->AddFrame(new TGTextButton(frame12, "Next", "RTGLViewer::Next()"),
		    new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  frame12->AddFrame(new TGTextButton(frame12, 
				     "Previous", "RTGLViewer::Prev()"),
		    new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  fEntry = new TGNumberEntry(frame12, 0, 8, 1,
			     TGNumberFormat::kNESInteger,
			     TGNumberFormat::kNEANonNegative);
  frame12->AddFrame(fEntry, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  frame1->AddFrame(frame12, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  fEntry->Connect("ValueChanged(Long_t)", "RTGLViewer", this, "Entry()");
  fEntry->Connect("ValueSet(Long_t)", "RTGLViewer", this, "Entry()");

  TGGroupFrame *frame13 = new TGGroupFrame(frame1, "Tracker display");
  fBarray.Add(new TGCheckButton(frame13, "Supports",    31));
  fBarray.Add(new TGCheckButton(frame13, "All Ladders", 32));
  fBarray.Add(new TGCheckButton(frame13, "All TkIDs",   33));
  fBarray.Add(new TGCheckButton(frame13, "Clusters",    34));
  for (Int_t i = 0; i < 4; i++) {
    TGButton *btn = GetButton(31+i);
    btn->Connect("Toggled(Bool_t)", "RTGLViewer", 
		 this, "HandleButtons(Bool_t)");
    frame13->AddFrame(btn);
  }
  frame1->AddFrame(frame13, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));
  
  TGGroupFrame  *frame14 = new TGGroupFrame(frame1, "TOF display");
  fBarray.Add(new TGCheckButton(frame14, "Show TOF", 41));
  fBarray.Add(new TGCheckButton(frame14, "All pads", 42));
  fBarray.Add(new TGCheckButton(frame14, "All IDs",  43));
  fBarray.Add(new TGCheckButton(frame14, "hits",     44));
  for (Int_t i = 0; i < 4; i++) {
    TGButton *btn = GetButton(41+i);
    btn->Connect("Toggled(Bool_t)", "RTGLViewer", 
		 this, "HandleButtons(Bool_t)");
    frame14->AddFrame(btn);
  }
  frame1->AddFrame(frame14, new TGLayoutHints(kLHintsExpandX, 0, 0, 2, 2));

  GetButton(31)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::TRKSUP));
  GetButton(32)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ALLTRK));
  GetButton(33)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::NUMTRK));
  GetButton(34)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::TRKHIT));
  GetButton(41)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ANYTOF));
  GetButton(42)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ALLTOF));
  GetButton(43)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::NUMTOF));
  GetButton(44)
    ->SetOn(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::TOFHIT));
  GetButton(33)
    ->SetEnabled(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ALLTRK));
  GetButton(42)
    ->SetEnabled(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ANYTOF));
  GetButton(43)
    ->SetEnabled(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ANYTOF)&
		 RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ALLTOF));
  GetButton(44)
    ->SetEnabled(RTrackerDisplay3D::TestOption(RTrackerDisplay3D::ANYTOF));
  
  frame1->MapSubwindows();

  for (Int_t i = 0; i < 8; i++)
    GetButton((3+i/4)*10+i%4+1)->Connect("Toggled(Bool_t", "RTGLViewer", 
					 this, "HandleButtons(Bool_t)");


  TGVerticalFrame *frame2 = new TGVerticalFrame(frame0, 10, 10, 
						kSunkenFrame);
  frame0->AddFrame(frame2, new TGLayoutHints(kLHintsRight | kLHintsExpandX | 
					     kLHintsExpandY, 0, 2, 2, 2));

  TGVerticalFrame *frame21 = new TGVerticalFrame(frame2, 10, 10, 
						 kSunkenFrame);
  frame2->AddFrame(frame21, new TGLayoutHints(kLHintsRight | kLHintsExpandX,
					      0, 2, 2, 2));

  fTview = new TGTextView(frame21, 500, 40, 1, 0);
  frame21->AddFrame(fTview, new TGLayoutHints(kLHintsRight | kLHintsExpandX,
					      10, 10, 2, 2));

  fTview->SetFont(
    fFrame->GetClient()->GetFontByName(
      "-*-helvetica-bold-r-*-*-14-*-*-*-*-*-iso8859-1"));

  TGVerticalFrame *frame22 = new TGVerticalFrame(frame2, 10, 10, 
						 kSunkenFrame);
  frame2->AddFrame(frame22, new TGLayoutHints(kLHintsRight | kLHintsExpandX |
					      kLHintsExpandY, 0, 2, 2, 2));

  fGLWindow = new TGLWidget(*frame22, kTRUE, 10, 10, 0);

  // Direct events from the TGWindow directly to the base viewer
  fGLWindow->Connect("HandleButton(Event_t*)", "TGLViewer", this, 
		     "HandleButton(Event_t*)");
  fGLWindow->Connect("HandleDoubleClick(Event_t*)", "TGLViewer", this, 
		     "HandleDoubleClick(Event_t*)");
  fGLWindow->Connect("HandleKey(Event_t*)", "TGLViewer", this, 
		     "HandleKey(Event_t*)");
  fGLWindow->Connect("HandleMotion(Event_t*)", "TGLViewer", this,
		     "HandleMotion(Event_t*)");
  fGLWindow->Connect("Repaint()", "TGLViewer", this, "Repaint()");
  fGLWindow->Connect("HandleConfigureNotify(Event_t*)", "TGLViewer", this, 
		     "HandleConfigureNotify(Event_t*)");
  
  frame22->AddFrame(fGLWindow, new TGLayoutHints(kLHintsExpandX | 
						 kLHintsExpandY));
  frame2->MapSubwindows();
}

void RTGLViewer::Show()
{
  fFrame->MapRaised();
  RequestDraw();
}

void RTGLViewer::OpenFile(const char *fname)
{
  if (!(fFile = TFile::Open(fname))) return;
  fFile->cd();
  if (GetTree()) Entry();
}

TTree *RTGLViewer::GetTree(void)
{
  if (!fFile && !(fFile = gFile)) return 0;
  if (!(fTree = (TTree *)gFile->Get("t4"))) return 0;
  fEvent = 0;
  if (Event::currev) fEvent = Event::currev;
  else fTree->SetBranchAddress("Tracker", &fEvent);
  fEntry->SetLimits(TGNumberFormat::kNELLimitMinMax, 0, fTree->GetEntries());
  fEntry->SetIntNumber(0);

  return fTree;
}

void RTGLViewer::DrawInfo(void)
{
  if (!fEvent) return;

  TString str = Form(" Run: %10d  Event: %7d  Nraw: %d  "
		     "Ncls: %d  Nhit: %d  Ntrk: %d", 
		     fEvent->runnum, fEvent->eventnum,
		     fEvent->NTrRawClusters, fEvent->NTrClusters,
		     fEvent->NTrRecHits, fEvent->NTrTracks);
  if (fEvent->NTrTracks > 0) {
    TrTrackR *track = fEvent->GetTrTrack(0);
    int fid = TrTrackR::kChoutko;
    if (track->ParExists(fid))
      str += Form("  Rgt: %.2f", track->GetRigidity(fid));
  }

  TGText *tx = new TGText;
  TGText tgx(str);
  tx->AddText(&tgx);

  fTview->SetText(tx);
  fTview->Update();
}

void RTGLViewer::HandleButtons(Bool_t sw)
{
  TGButton *btn = (TGButton *)gTQSender;
  Int_t id = btn->WidgetId();

  switch (id) {
  case 31:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::TRKSUP, sw);
    Draw();
    break;
  case 32:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::ALLTRK, sw);
    GetButton(33)->SetEnabled(sw);
    Draw();
    break;
  case 33:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::NUMTRK, sw);
    Draw();
    break;
  case 34:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::TRKHIT, sw);
    Draw();
    break;
  case 41:
    if (!sw) {
      RTrackerDisplay3D::SetOption(RTrackerDisplay3D::ALLTOF, sw);
      RTrackerDisplay3D::SetOption(RTrackerDisplay3D::NUMTOF, sw);
      GetButton(42)->SetOn(sw);
      GetButton(43)->SetOn(sw);
    }
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::ANYTOF, sw);
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::TOFHIT, sw);
    GetButton(42)->SetEnabled(sw);
    GetButton(43)->SetEnabled(sw);
    GetButton(44)->SetEnabled(sw);
    GetButton(44)->SetOn(sw);
    Draw();
    break;

  case 42:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::ALLTOF, sw);
    GetButton(43)->SetEnabled(sw);
    Draw();
    break;
  case 43:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::NUMTOF, sw);
    Draw();
    break;
  case 44:
    RTrackerDisplay3D::SetOption(RTrackerDisplay3D::TOFHIT, sw);
    Draw();
    break;

  default:
    break;
  };
}

void RTGLViewer::Open()
{
  if (!fIsStandAlone) {
    fSelfPtr->fileOpen();
    return;
  }

  const char *filetypes[] 
    = { "ROOT files", "*.root", "All files", "*", 0, 0 };
  static TString dir(".");
  TGFileInfo fi;

  fi.fFileTypes = filetypes;
  fi.fIniDir    = StrDup(dir);
  new TGFileDialog(gClient->GetRoot(), fSelfPtr->fFrame, kFDOpen, &fi);
  if (fi.fFilename && fi.fFilename[0])
    fSelfPtr->OpenFile(fi.fFilename);
  dir = fi.fIniDir;
}

void RTGLViewer::Print()
{
}

void RTGLViewer::Exit()
{
  gApplication->Terminate();
}

void RTGLViewer::Next()
{
  fSelfPtr->fEntry->IncreaseNumber(TGNumberFormat::kNSSSmall, 1);
  Entry();
}

void RTGLViewer::Prev()
{
  fSelfPtr->fEntry->IncreaseNumber(TGNumberFormat::kNSSSmall, -1);
  Entry();
}

void RTGLViewer::Draw()
{
  if (!fSelfPtr->fEvent) fSelfPtr->fEvent = Event::currev;
  fSelfPtr->DrawInfo();
  RTrackerDisplay3D::DrawOneEvent(fSelfPtr->fEvent);
}

void RTGLViewer::Entry()
{
  if (!fIsStandAlone) {
    fSelfPtr->eventEntry();
    return;
  }
  if (!fSelfPtr->fTree && !fSelfPtr->GetTree()) return;
  fSelfPtr->fTree->GetEntry(fSelfPtr->fEntry->GetIntNumber());
  Draw();
}
