#include <ViewerGUI.h>

#include <TGClient.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TRandom.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>
#include <TGTab.h>
#include <RQ_OBJECT.h>

#include "ViewerCore.h"

class TGWindow;
class TGMainFrame;
class TRootEmbeddedCanvas;

using namespace std;

#define BUTTOMHEIGHT 40

void ViewerGUI::DoNextEvent() {
	cout << "NextEvent: " << ViewerCore::GetIstance().GetCurrentEvent()+1 << endl;
	fEventNumbEntry->SetIntNumber(ViewerCore::GetIstance().GetCurrentEvent()+1);
	if ( ViewerCore::GetIstance().NeedToDelete()) ViewerCore::GetIstance().DeleteEvent();
	int status = ViewerCore::GetIstance().DrawNextEvent();
	if (status == 0) {
		exit(EXIT_FAILURE);
	} else if(status==2) {
		DoNextEvent();
	}
}

void ViewerGUI::DoEvent() {
	cout << "Event: " << fEventNumbEntry->GetIntNumber() << endl;
	cout << "Need to delete: " << ViewerCore::GetIstance().NeedToDelete() << endl;
	if (ViewerCore::GetIstance().NeedToDelete()) ViewerCore::GetIstance().DeleteEvent();
	int status = ViewerCore::GetIstance().DrawEvent(fEventNumbEntry->GetIntNumber());
	if (status == 0) {
		exit(EXIT_FAILURE);
	} else if(status==2) {
		DoNextEvent();
	}
}

void ViewerGUI::DoPrintEvent() {
	cout << "Print event: " << fEventNumbEntry->GetIntNumber() << endl;
	ViewerCore::GetIstance().PrintEvent();
}

void ViewerGUI::DoDrawAMSOccupancy() {
	cout << "Draw AMS occupancy" << endl;
	ViewerCore::GetIstance().DrawAMSOccupancy();
}

void ViewerGUI::DoPrintAMSOccupancy() {
	cout << "Print AMS occupancy" << endl;
}

void ViewerGUI::DoDrawDAMPEOccupancy() {
	cout << "Draw DAMPE occupancy" << endl;
	ViewerCore::GetIstance().DrawDAMPEOccupancy();
}

void ViewerGUI::DoPrintDAMPEOccupancy() {
	cout << "Print DAMPE occupancy" << endl;
}

ViewerGUI::ViewerGUI(const TGWindow *p,UInt_t w,UInt_t h, int VCONF) {
  // Create a main frame
  fMain = new TGMainFrame(p,w,h);
  fMain->SetWindowName("TIC viewer");

  // Creating tab for event viewer and cumulative plots
  //--------- create the Tab widget
  TGTab *Tab = new TGTab(fMain, 300, 300);

  //--------- create a tab element
  TGCompositeFrame *TabEvent = Tab->AddTab("Event");

  // Create sub frames
  TGHorizontalFrame *canvasFrame = new TGHorizontalFrame(TabEvent,w,h-BUTTOMHEIGHT);

  // Create canvas widget and adding to horizontal frame
  double canWidth = w;
  if (VCONF==1) canWidth = w/2.;
	// Set TCanvas,
	if (VCONF==1 || VCONF==3) {
		fEc3D	= new TRootEmbeddedCanvas("Ec3D",canvasFrame,canWidth,h-BUTTOMHEIGHT);
		canvasFrame->AddFrame(fEc3D, new TGLayoutHints(kLHintsExpandX |
        kLHintsExpandY, 10,10,10,1));
	}
	// 2D viewer
	if (VCONF==1 || VCONF==2){
		fEc2D	= new TRootEmbeddedCanvas("Ec2D",canvasFrame,canWidth,h-BUTTOMHEIGHT);
		canvasFrame->AddFrame(fEc2D, new TGLayoutHints(kLHintsExpandX |
        kLHintsExpandY, 10,10,10,1));
	}

  // adding horizontal frame to the main window
  TabEvent->AddFrame(canvasFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY,
                                            2,2,2,2));

  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *buttomFrame = new TGHorizontalFrame(TabEvent,200,BUTTOMHEIGHT);
  TGTextButton *nextButtom = new TGTextButton(buttomFrame,"&Next");
  nextButtom->Connect("Clicked()","ViewerGUI",this,"DoNextEvent()");
  buttomFrame->AddFrame(nextButtom, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  TGTextButton *exit = new TGTextButton(buttomFrame,"&Exit",
                               "gApplication->Terminate(0)");
  buttomFrame->AddFrame(exit, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  TGTextButton *printButtom = new TGTextButton(buttomFrame,"&Print");
  printButtom->Connect("Clicked()","ViewerGUI",this,"DoPrintEvent()");
  buttomFrame->AddFrame(printButtom, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  //create n. of event conuter
  fEventNumbEntry = new TGNumberEntry(buttomFrame);
  fEventNumbEntry->Connect("ValueSet(Long_t)","ViewerGUI",this,"DoEvent()");
  buttomFrame->AddFrame(fEventNumbEntry, new TGLayoutHints(kLHintsCenterX,
      5,5,3,4	));

  TabEvent->AddFrame(buttomFrame, new TGLayoutHints(kLHintsCenterX,
                                            2,2,2,2));

  //--------- create AMS tab element
  TGCompositeFrame *TabAMS = Tab->AddTab("AMS");

  // Create sub frame and canvas
  TGHorizontalFrame *AMScanvasFrame = new TGHorizontalFrame(TabAMS,w,h-BUTTOMHEIGHT);
  fEcAMS	= new TRootEmbeddedCanvas("EcAMS",AMScanvasFrame,w,h-BUTTOMHEIGHT);
  		AMScanvasFrame->AddFrame(fEcAMS, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,1));

  // adding canvas frame to this tab
  TabAMS->AddFrame(AMScanvasFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,2,2,2));

  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *AMSbuttomFrame = new TGHorizontalFrame(TabAMS,200,BUTTOMHEIGHT);
  TGTextButton *AMSDrawButtom = new TGTextButton(AMSbuttomFrame,"&Draw");
  AMSDrawButtom->Connect("Clicked()","ViewerGUI",this,"DoDrawAMSOccupancy()");
  AMSbuttomFrame->AddFrame(AMSDrawButtom, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *AMSexit = new TGTextButton(AMSbuttomFrame,"&Exit","gApplication->Terminate(0)");
  AMSbuttomFrame->AddFrame(AMSexit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *AMSprintButtom = new TGTextButton(AMSbuttomFrame,"&Print");
  AMSprintButtom->Connect("Clicked()","ViewerGUI",this,"DoPrintAMSOccupancy()");
  AMSbuttomFrame->AddFrame(AMSprintButtom, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TabAMS->AddFrame(AMSbuttomFrame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  //--------- create DAMPE tab element
  TGCompositeFrame *TabDAMPE = Tab->AddTab("DAMPE");

  // Create sub frame and canvas
  TGHorizontalFrame *DAMPEcanvasFrame = new TGHorizontalFrame(TabDAMPE,w,h-BUTTOMHEIGHT);
  fEcDAMPE	= new TRootEmbeddedCanvas("EcDAMPE",DAMPEcanvasFrame,w,h-BUTTOMHEIGHT);
  		DAMPEcanvasFrame->AddFrame(fEcDAMPE, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,1));

  // adding canvas frame to this tab
  TabDAMPE->AddFrame(DAMPEcanvasFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,2,2,2));

  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *DAMPEbuttomFrame = new TGHorizontalFrame(TabDAMPE,200,BUTTOMHEIGHT);
  TGTextButton *DAMPEDrawButtom = new TGTextButton(DAMPEbuttomFrame,"&Draw");
  DAMPEDrawButtom->Connect("Clicked()","ViewerGUI",this,"DoDrawDAMPEOccupancy()");
  DAMPEbuttomFrame->AddFrame(DAMPEDrawButtom, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *DAMPEexit = new TGTextButton(DAMPEbuttomFrame,"&Exit","gApplication->Terminate(0)");
  DAMPEbuttomFrame->AddFrame(DAMPEexit, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  TGTextButton *DAMPEprintButtom = new TGTextButton(DAMPEbuttomFrame,"&Print");
  DAMPEprintButtom->Connect("Clicked()","ViewerGUI",this,"DoPrintDAMPEOccupancy()");
  DAMPEbuttomFrame->AddFrame(DAMPEprintButtom, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TabDAMPE->AddFrame(DAMPEbuttomFrame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));

  //--------- create ev-by-ev tracker 1D histos tab element
  TGCompositeFrame *TabTracker = Tab->AddTab("Ev-by-ev tracker");

  // Create sub frame and canvas
  TGHorizontalFrame *TrackerCanvasFrame = new TGHorizontalFrame(TabTracker,w,h-BUTTOMHEIGHT);
  fEcTracker	= new TRootEmbeddedCanvas("EcTracker",TrackerCanvasFrame,w,h-BUTTOMHEIGHT);
  		TrackerCanvasFrame->AddFrame(fEcTracker, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 10,10,10,1));

  // adding canvas frame to this tab
  TabTracker->AddFrame(TrackerCanvasFrame, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 2,2,2,2));

  // Create a horizontal frame widget with buttons
  TGHorizontalFrame *TrackerButtomFrame = new TGHorizontalFrame(TabTracker,200,BUTTOMHEIGHT);
  TGTextButton *TrackerNextButtom = new TGTextButton(TrackerButtomFrame,"&Next");
  TrackerNextButtom->Connect("Clicked()","ViewerGUI",this,"DoNextEvent()");
  TrackerButtomFrame->AddFrame(TrackerNextButtom, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));
  TGTextButton *TrackerExit = new TGTextButton(TrackerButtomFrame,"&Exit",
                               "gApplication->Terminate(0)");
  TrackerButtomFrame->AddFrame(TrackerExit, new TGLayoutHints(kLHintsCenterX,
                                           5,5,3,4));

  TabTracker->AddFrame(TrackerButtomFrame, new TGLayoutHints(kLHintsCenterX,2,2,2,2));


  // adding tab to main windows
  fMain->AddFrame(Tab, new TGLayoutHints(kLHintsExpandX |
                                         kLHintsExpandY, 2, 2, 2, 2));

  // Map all subwindows of main frame
  fMain->MapSubwindows();

  // Initialize the layout algorithm
  fMain->Resize(fMain->GetDefaultSize());

  // Map main frame
  fMain->MapWindow();
}

ViewerGUI::~ViewerGUI() {
  // Clean up used widgets: frames, buttons, layout hints
  fMain->Cleanup();
  delete fMain;
}
