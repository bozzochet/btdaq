#include <TFile.h>
#include <TrCalDB.h>
#include <TrLadCal.h>
#include <TGClient.h>
#include <TApplication.h>
#include <TGMenu.h>
#include <TGButton.h>
#include <TGFileDialog.h>
#include "CalSlider.h"
#include "CalPainterUI.h"

#define M_FILE_OPEN_CAL 0
#define M_FILE_OPEN_HIS 1
#define M_FILE_EXIT     2
#define M_PRINT_THIS    3
#define M_PRINT_ALL     4

ClassImp(CalPainterUI);

CalPainterUI::CalPainterUI(const TGWindow *p,UInt_t w,UInt_t h,char *filename):
		TGMainFrame(p,w,h){

	// add menu bar
	TGMenuBar *menubar = new TGMenuBar(this,100,20,kHorizontalFrame);
	TGPopupMenu *menufile = new TGPopupMenu();
	menufile->AddEntry("Open &Calibration File",M_FILE_OPEN_CAL);
	menufile->AddEntry("Open &Histograms File",M_FILE_OPEN_HIS);
	menufile->AddSeparator();
	menufile->AddEntry("&Exit",M_FILE_EXIT);
	menufile->Connect("Activated(Int_t)","CalPainterUI",this,
		"HandleMenu(Int_t)");
	menubar->AddPopup("&File",menufile,
		new TGLayoutHints(kLHintsTop | kLHintsLeft));
	TGPopupMenu *menuprint = new TGPopupMenu();
	menuprint->AddEntry("Print &This",M_PRINT_THIS);
	menuprint->AddEntry("Print &All",M_PRINT_ALL);
	menuprint->Connect("Activated(Int_t)","CalPainterUI",this,
		"HandleMenu(Int_t)");
	menubar->AddPopup("&Print",menuprint,
		new TGLayoutHints(kLHintsTop | kLHintsLeft));
	
	AddFrame(menubar,new TGLayoutHints(kLHintsTop | kLHintsExpandX));

	// build vertical panel
	TGVerticalFrame *vp = new TGVerticalFrame(this);
	// build mainapp to pass pointer to components
	gs = new GenericSlider(vp);
	// add mainapp (canvas,buttons)
	vp->AddFrame(gs,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
	// interaction (HwId)
	intpan = new InteractionPanel(vp,"HwId",gs);
	vp->AddFrame(intpan,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
	// tracker schematics (TkId)
	track = new TrackerPanel(vp,"TkId",gs);
	vp->AddFrame(track,new TGLayoutHints(kLHintsCenterX | kLHintsTop));
	// search
	search = new SearchPanel(vp,"Search",gs);
	vp->AddFrame(search,new TGLayoutHints(kLHintsCenterX | kLHintsTop));

	AddFrame(vp,new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));

	// connect signals
	gs->Connect("Updated()","InteractionPanel",intpan,"Update()");
	gs->Connect("Updated()","TrackerPanel",track,"Update()");

	// add filename if provided in commandline
	if(filename[0])
		gs->setRootFile(filename);
	// finish up
	//SetMWMHints(kMWMDecorAll | kMWMDecorMaximize | kMWMDecorMinimize |
	//		kMWMDecorMenu,
	//	kMWMFuncAll | kMWMFuncMaximize | kMWMFuncMinimize | 
	//		kMWMFuncClose,
	//	kMWMInputFullApplicationModal);

	SetWindowName("CalPainterUI");
	//Connect("CloseWindow()","CalPainterUI",this,"CloseWindow()");
	MapSubwindows();
	MoveResize(1200,700);
	MapWindow();
}

void CalPainterUI::HandleMenu(Int_t id){
	switch(id){
		case M_FILE_OPEN_CAL:
		case M_FILE_OPEN_HIS:
			{
				const char *ft[] = {"ROOT Files","*.root",0,0};
				TGFileInfo fi;
				fi.fFileTypes = ft;
				fi.fIniDir = NULL;
				new TGFileDialog(gClient->GetRoot(),this,
					kFDOpen,&fi);
				if(fi.fFilename != NULL){
					if(id == M_FILE_OPEN_CAL)
						gs->setRootFile(fi.fFilename);
					else if(id == M_FILE_OPEN_HIS)
						gs->setHistFile(fi.fFilename);
				}
			}
			break;
		case M_FILE_EXIT:
			 {
				 gApplication->Terminate(0);
			 }
			 break;
		case M_PRINT_THIS:
			 {
				 gs->PrintThis();
			 }
			 break;
		case M_PRINT_ALL:
			 {
				 gs->PrintAll();
			 }
			 break;
	}
}

CalPainterUI::~CalPainterUI(){}

void CalPainterUI::CloseWindow(){
	gApplication->Terminate(0);
}

int main(int argc, char **argv){
	char filename[30];
	if(argc == 2){
		strcpy(filename,argv[1]);
	}else
		filename[0] = '\0';
	TApplication app("App",&argc,argv);
	new CalPainterUI(gClient->GetRoot(),200,200,filename);
	app.Run();
	return 0;
}

