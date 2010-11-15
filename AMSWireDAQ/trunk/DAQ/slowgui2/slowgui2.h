#ifndef SLOWGUI_H
#define SLOWGUI_H


#include <stdio.h>
#include <stdlib.h>
#include "TApplication.h"
#include "TGFrame.h"
#include "TGWindow.h"
#include "TROOT.h"
#include "TGFrame.h"
#include "TCanvas.h"
#include "TGTab.h"
#include "TColor.h"
#include "TStyle.h"
#include "TGMsgBox.h"
#include "TTimer.h"
#include "TRandom.h"
#include "TGComboBox.h"
#include "TGLabel.h"
#include "TGDockableFrame.h"
#include "TGLayout.h"
#include "TGNumberEntry.h"
#include "TGToolTip.h"
#include "TGTextEdit.h"
#include "TGraph.h"
#include "TTree.h"
#include "TLegend.h"
#include "TGProgressBar.h"
#include "TGStatusBar.h"
#include "TRootEmbeddedCanvas.h"
#include "TLatex.h"
#include "TFile.h"
#include "TH2F.h"
#include "TMath.h"

#include "../SlowControl/SlowControlTypes.h"
#include "../TrackerLadders/TrackerLadders.h"

#define NTPSFE 4
#define NTBS 2
#define NTBSHOTCOLD 2
#define NTdrTPSFE 6
#define NTdrTBS 12
#define NACTEL 2
#define NTBSGR 2 /* hot and cold LR */
#define N9051 4
#define NSIDES 2
#define N9053 2
#define N9053HOTCOLD 2
#define N9055 2
#define N9055HOTCOLD 2
#define NTDR 24

//#define HELPICON "sm_help.xpm"
#define REFRESHICON "refresh2.xpm"

//#define INITLECROYICON "interrupt.xpm"
#define REINITDCDC "./slowgui2/icons/smaller/system-reboot2.xpm"
#define HELPICON "./slowgui2/icons/smaller/help-about.xpm"
//#define REFRESHICON "./slowgui2/icons/smaller/view-refresh.xpm"
#define INITLECROYICON "./slowgui2/icons/smaller/quickopen.xpm"
#define S9053HTOCICON "./slowgui2/icons/smaller/hottocold.xpm"
#define ANALOGOFF "./slowgui2/icons/smaller/system-shutdown.xpm"
#define SWITCHACTEL "./slowgui2/icons/smaller/system-switch-user.xpm"


enum mycolors {
  kMyWhite, kMyBlack, kMyRed, kMyGreen, kMyBlue, kMyYellow, kMyMagenta, kMyCyan
};


//class TPSFE; // defined elsewhere...
//class TBS; // defined elsewhere...
class AMSWcom; // defined elsewhere...
//class SlowControl; // defined elsewhere...
//class S9011AT; // guess what...
class Editor;
class TrackerLadders;

class SlowGui {

  //RQ_OBJECT("ScopeDis");

 private:
  TGMainFrame *fMain, *fTrackerMonitorMainFrame;
  TGVerticalFrame *fFrame0;
  TGHorizontalFrame *fFrame1, *fFrame2;
  TGStatusBar *fAMSWireStatus;
  TGTab *fTab, *fTBSTab, *fTPSFETab, *fPlotTab;
  TGLayoutHints *fL1, *fL3, *fL31, *fL32, *fL33, *fL34, *fL35, *fL5, *fL51, *fL7, *fL71, *fL72, *fL73, *fL74, *fL75, *fL76, *fL77, *fL78, *fL79;
  TGCompositeFrame *tf, *fF1, *fF2;
  TGCompositeFrame *tS9011main, *tTPSFEmain, *tTBSmain, *tSettingsTab, *tPlotTab;
  TGCompositeFrame *tTPSFEpage[NTPSFE], *tTBSpage[NTBS];

  TGLabel *tTPSFETdrLabel[NTPSFE][NTdrTPSFE+1], *TripLabelSSF[NTPSFE][NTdrTPSFE], *TripLabelLRS[NTPSFE][NTdrTPSFE], *TripLabelLRK[NTPSFE][NTdrTPSFE];
  TGLabel *tS9051TripLabel[N9051][NSIDES], *tS9053TripLabel[N9053][N9053HOTCOLD], *tS9055TripLabel[N9055][N9055HOTCOLD];;
  TGLabel *tS9051OvercurrentLabel[N9051][NSIDES], *tS9053OvercurrentLabel[N9053][N9053HOTCOLD], *tS9055OvercurrentLabel[N9055][N9055HOTCOLD];;
  TGTextButton *tTPSFEActelHotButton[NTPSFE], *tTPSFEActelColdButton[NTPSFE];
  TGTextButton *tTBSActelHotButton[NTPSFE], *tTBSActelColdButton[NTPSFE];
  TGTextButton *tS9011ATActelHot0Button, *tS9011ATActelCold0Button,*tS9011ATActelHot1Button, *tS9011ATActelCold1Button;
  TGComboBox *tTPSFEActelCombo[NTPSFE], *tTBSActelCombo[NTPSFE], *tS9011ATActelCombo;
  TGTextButton *tTDRSSF[NTPSFE][NTdrTPSFE+1], *tTDRLRS[NTPSFE][NTdrTPSFE+1], *tTDRLRK[NTPSFE][NTdrTPSFE+1];
  TGTextButton *tTDRALL[NTPSFE][NTdrTPSFE+1];
  TGDockableFrame *TripFrame[NTPSFE], *ADCcurrentFrame[NTBS];

  TGTextButton *tTBSLRhot[NTBS][NTBSGR], *tTBSLRcold[NTBS][NTBSGR], *tTBSVOLT80[NTBS][NTBSGR], *tTBSVOLT60[NTBS][NTBSGR], *tTBSGetVOLT[NTBS][NTBSGR];
  TGLabel *tTBSADChot[NTBS][NTBSGR],*tTBSADCcold[NTBS][NTBSGR];
  TGTextButton *tButTBSADCcurrent[NTBS][NTdrTBS], *tButTBSADCcurrentsALL[NTBS];
  TGLabel *tTBSADCcurrent[NTBS][NTdrTBS];
  TGLabel *tTBSTripIndicator[NTBS], *tTBSPowerOnIndicator[NTBS];
  TGLabel *tTPSFETripIndicator[NTPSFE], *tTPSFEPowerOnIndicator[NTPSFE];
  TGLabel *tS9011ATTripIndicator, *tS9011ATPowerOnIndicator;
  TGTextButton *tS9051DCDC[N9051][NSIDES], *tS9053DCDC[N9053][N9053HOTCOLD], *tS9055DCDC[N9055][N9055HOTCOLD];
  TGCheckButton *tAutoSSFButton[NTPSFE], *tAutoLRSButton[NTPSFE], *tAutoLRKButton[NTPSFE];
  TGCheckButton *tS9053Enable;

  TGNumberEntry *tAMSWaddr,*tAMSWaddrB, *tTimerDel;
  TGComboBox *aAMSWmodeCombo, *aAMSWportCombo, *aAMSWportBCombo;

  TTimer *TimerCheck;
  ushort TbsVolt[NTBS][NTBSGR][2];
  ushort TbsCurrent[NTBS][NTdrTBS];

  TCanvas *PlotCanvas;
  TGraph *LadderCurrent[NTDR];

  TGCheckButton *PlotTDR[NTDR];

  AMSWcom *Node, *NodeB;
  unsigned int AMSWaddr, AMSWaddrB;
  ushort  JinftAver, JinftAsubver,JinftBver, JinftBsubver;

  //TPSFE *MyTpsfe[NTPSFE][NACTEL];
  //TBS *MyTbs[NTBS][NACTEL];
  //  S9011AT *MyS9011AT[NACTEL];
  //sACTEL ActelTpsfe[NTPSFE]; // active actel mode 
  //sACTEL ActelTbs[NTBS];
  //sACTEL ActelS9011AT;
  //sActelMode ActelTbsState[NTBS][NACTEL], ActelTpsfeState[NTPSFE][NACTEL]; // ActelS9011ATState[NACTEL];
  sVOLT Voltages[NTBS][NTBS*NTBSHOTCOLD];
  sDCDC TBSlrState[NTBS][NTBS*NTBSHOTCOLD]; // k0A, k0B, k1A, k1B
  sDCDC TDRSSFstate[NTPSFE][NTdrTPSFE], TDRLRSstate[NTPSFE][NTdrTPSFE], TDRLRKstate[NTPSFE][NTdrTPSFE];
  sDCDC S9051state[N9051][NSIDES], S9053state[N9053][N9053HOTCOLD], S9055state[N9055][N9055HOTCOLD];
  sDCDC S9051overc[N9051][NSIDES], S9053overc[N9053][N9053HOTCOLD], S9055overc[N9055][N9055HOTCOLD];
  sDCDC AutoSSF[NTPSFE], AutoLRS[NTPSFE], AutoLRK[NTPSFE];

  ushort ActelMask;
  //  int S9011ATline;

  Float_t CorrADC[NTBS][NTdrTBS];

  Pixel_t PixelColor[20][2];  
 
  Int_t TPSFETabColor[NTPSFE], TBSTabColor[NTBS];
  Int_t ADCmode;
  Int_t CanvasCount;

  FILE *DataFile;
  TFile *RootFile;



  //trackergeo ladder[133];
  TCanvas *LadCan[4][NTdrTPSFE], *LadCanZoom[4][NTdrTPSFE];
  TTree *LadGraph[4][NTdrTPSFE];

  TGHProgressBar *fHProgressLeak[4][NTdrTPSFE], *testpb[4];
  TGStatusBar *fStatusPwg[4];
  TGLabel *fLabelTripS[4][NTdrTPSFE], *fLabelTripScnt[4][NTdrTPSFE];
  TGLabel *fLabelTripK[4][NTdrTPSFE], *fLabelTripKcnt[4][NTdrTPSFE];
  TGLabel *fLabelTripT[4][NTdrTPSFE], *fLabelTripTcnt[4][NTdrTPSFE];
  TGLabel *fSlabel[4][NTdrTPSFE], *fKlabel[4][NTdrTPSFE], *fTlabel[4][NTdrTPSFE];
  TGGroupFrame *fGroupFrame[4][NTdrTPSFE];

  ULong_t PwgCol[4];
  ULong_t PwgColDark[4];

  int ladmax;
 
  int zoomcounter, zoommin;
  
  TrackerLadders InnerTracker;
  int Tim[4][NTdrTPSFE];
  double Current[4][NTdrTPSFE];


  void InitStyle();
  void SetTabColor(TGTab *aTab, Char_t *name,  Pixel_t color);
  void SetTabColor(TGTab *aTab, Int_t index,  Pixel_t color);
  void UpdateTabColor(const Char_t *type);
  void UpdateSettings();
  void SetLabelColor(TGLabel *label, Pixel_t fore, Pixel_t back);
  void BuildActelDisplay(TGCompositeFrame *hframe, TGTextButton **button1, TGTextButton **button2, TGComboBox **combo, TGLabel **label1, TGLabel **label2, Int_t page, const Char_t *type);
  void BuildActelDisplay2(TGCompositeFrame *hframe, TGTextButton **button1, TGTextButton **button2, TGTextButton **button3, TGTextButton **button4, TGLabel **label1, TGLabel **label2, Int_t page, const Char_t *type);

  int AMSWmode, AMSWportA, AMSWportB;
  int TCrate;

 public:
  SlowGui(const TGWindow *p, UInt_t w, UInt_t h, int tcrate, unsigned int jinfaddra, unsigned int jinfaddrb, int porta, int portb );
  virtual ~SlowGui();
  void SetPosition(int crate, int type);
  void CreateTpsfe();
  void CreateS9011ATFrame(TGCompositeFrame *frame); 
  void CreateTBSFrame(TGCompositeFrame *frame); 
  void CreateTpsfeFrame(TGCompositeFrame *frame); 
  void CheckTab(Int_t val, Char_t *FrameName);
  void Exit();
  void CheckSlowControl();
  void FillTpsfePage(Int_t page);
  Int_t TPSFEtoTDR(Int_t tpsfe, Int_t tdr);
  void ControlTpsfe(Int_t Mode, Int_t Tpsfe, Int_t Tdr);
  int GetTpsfeStatus(Int_t tpsfe);
  //  void GetTpsfeTrips(Int_t Tpsfe);
  void SwitchTpsfeTdr(Int_t Tpsfe, Int_t Tdr);
  void SwitchTpsfeLrs(Int_t Tpsfe, Int_t Tdr);
  void SwitchTpsfeLrk(Int_t Tpsfe, Int_t Tdr);
  void SwitchTpsfeAll(Int_t Tpsfe, Int_t Tdr);
  void SwitchTpsfeTdrAll(Int_t Tpsfe);
  void SwitchTpsfeLrsAll(Int_t Tpsfe);
  void SwitchTpsfeLrkAll(Int_t Tpsfe);
  void SwitchTpsfeAllAll(Int_t Tpsfe);
  //  void SelectTPSFEActel(sACTEL Actel, Int_t Tpsfe);
  void CheckMainTab(Int_t val);
  void CheckTpsfeClickDockable(Event_t *event);
  //  ClassDef(SlowGui,0);
  //void SwitchActel(sACTEL Brother, Int_t ID, Char_t *type);
  void SwitchActel2(int Actel, Int_t ID, Char_t *type);
  void CreateSettingsFrame(TGCompositeFrame *frame);

  void SelectCommunicationMode(Int_t mode);
  void SelectCommunicationPort(Int_t port);
  void SelectCommunicationPortB(Int_t port);
  void ApplyAMSWire();

  void FillTbsPage(Int_t page);
  void CreateTbs();
  //  void SelectTBSActel(sACTEL Actel, Int_t Tbs);
  int GetTbsStatus(Int_t Tbs);
  void CheckTbsClickDockable(Event_t *event);
  void CheckTbsUndock();
  void CheckTbsDock();
  void ControlTbsVoltage(Int_t tbs, Int_t group, Int_t voltage);
  //  void SwitchTBSActel(sACTEL Actel, Int_t Tbs);
  void DisplayTbsStatus(Int_t Tbs);
  //  void ShowBrotherTrip(SlowControl *slowctl);
  void ShowBrotherTrip(int group, int trip);
  //void ShowPowerOn(SlowControl *slowctl);
  void ShowPowerOn(int group, int trip);
  //void SelectActel(sACTEL Actel, Int_t ID, Char_t *type);
  //  void SwitchActel(sACTEL Actel, Int_t ID, Char_t *type);

  void CreateS9011AT();
  void CreateS9051Page(TGCompositeFrame *frame);
  void CreateS9053Page(TGCompositeFrame *frame);
  void CreateS9055Page(TGCompositeFrame *frame);
  void CreateTripIndicator(TGCompositeFrame *frame, TGLabel **triplabel);
  void CreatePowerOnIndicator(TGCompositeFrame *frame, TGLabel **pwonlabel);
  void ShowTripIndicator(TGLabel *alabel, Int_t show);
  void ShowPowerOnIndicator(TGLabel *alabel, Int_t show);
  void CreateTextButton(TGCompositeFrame *frame, TGTextButton **button, const Char_t *text);
  void DisplayHelp(Char_t *help);
  TGPictureButton* CreateHelpButton(TGCompositeFrame *hmain, const Char_t *item);
  TGPictureButton* CreateRefreshButton(TGCompositeFrame *hmain, const Char_t *item);
  TGPictureButton* CreateInitLecroyButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  TGPictureButton* CreateReInitDCDCButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  TGPictureButton* CreateS9053HotColdButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  TGPictureButton* CreateS9053HotCold2Button(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  TGPictureButton* CreateAnalogicalSwitchoffButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  TGPictureButton* CreateS9011ActelSwitchButton(TGCompositeFrame *hmain, const Char_t *item, Int_t val2);
  void GetTbsADCBiasVoltage(Int_t Tbs, Int_t update);
  void ControlTbsLr(Int_t Tbs, Int_t group, Int_t lr);
  void UpdateTbsBiasVoltage(Int_t Tbs, ushort group, Int_t empty=0);
  void UpdateTbsBiasCurrent(Int_t Tbs, ushort Tdr);
  void UpdateTbsBiasCurrents(Int_t Tbs, Int_t empty=0);
  //  void GetTbsADCBiasCurrent(Int_t Tbs, ushort Tdr, Int_t update);
  //  void GetTbsADCBiasCurrents(Int_t Tbs, Int_t update);
  Pixel_t LightColor(Pixel_t color);
  void ShowTpsfeTrips(Int_t Tpsfe, Int_t empty=0);
  Float_t ADCtoVolt(ushort adc, ushort group, ushort cold);
  Float_t ADCtoCurrent(ushort adc, ushort Tbs, ushort Tdr, Int_t correct);
  void ShowTbsEmpty(Int_t Tbs);
  void DisplayTpsfeStatus(Int_t Tpsfe);
  int GetS9011atStatus();
  void DisplayS9011atStatus();
  void ShowTpsfeEmpty(Int_t Tpsfe);
  void ControlS9051(Int_t group, Int_t side);
  void ControlS9053(Int_t group, Int_t side);
  void ControlS9055(Int_t group, Int_t side);
  void SelectADCmode(Int_t mode);
  void ShowS9011atEmpty();
  void CreateOvercurrentIndicator(TGCompositeFrame *frame, TGLabel **overlabel);
  void ShowOvercurrentIndicator(TGLabel *alabel, Int_t show);
  void CalibrateADCCurrent(Int_t Tbs);
  void CreatePlotFrame(TGCompositeFrame *frame);
  void ReadADCCurrentCalib(Int_t);
  void CreatePlot();
  void NewCanvas(Int_t x, Int_t y, const Char_t *name="");
  void CreateTDRPlotPage(TGCompositeFrame *frame);
  void UpdateCanvases();
  void RefreshCanvas(TCanvas *aCanvas);
  void InitGraphs();
  void FillGraphs();
  void OpenDataFile(int crate);
  void CloseDataFile();
  void SetButtonColor(TGTextButton *button, Pixel_t fore, Pixel_t back);
  void SetS9011DCDCButton(Int_t over, Int_t mode, TGTextButton *button);
  void SwitchAutoMode(Int_t Tpsfe, Char_t *type);
  void InitLecroy(Int_t val1, Int_t val2);
  void ReInitDCDC(Int_t val1, Int_t val2);
  void SwitchOffAnalogical(Int_t val1, Int_t val2);
  void S9011SwitchActel(Int_t val1, Int_t val2);
  void S9053HotToCold(Int_t val1, Int_t val2);
  void S9053HotToCold2(Int_t val1, Int_t val2);
  void PrintRXDONE(AMSWcom *node);
  void PrintLecroyError(unsigned int err);
  void PrintEvent(AMSWcom *node, int length=-1);
  void TrackerMonitor();
  void ProcessCanvas(TVirtualPad *pad,TObject *obj,Int_t event);
  void UpdateZoomCanvases();
  void InitGraph(int pwg, int pwgpos);
  void FillTKGraph(int pwg, int pwgpos, double val);
  void UpdateCurrent2(int pwg, int pwgpos, double current);
  void RefreshCanvas(int pwg, int pwgpos);
  void UpdateTKTrips(int pwg, int pwgpos, int typ, int cnt);
  void DisplayTKVoltages(int pwg, double voltage);
  void UpdateTKBiasVoltage(Int_t Tbs, ushort group, int cold);
  void RefreshAll();
  void InitCorrCurrent();
  void UpdateTKTPSFE(int pwg, int pwgpos, int typ, int on);
  void EnableS9053();
  void TestAMSW();
  int TestJinf(AMSWcom *node, unsigned int addr, ushort &ver, ushort &subver);
  int TestAMSWError(AMSWcom *node);
  void SetTimerDelay(Int_t val);
  ushort FindParameter(int param);
  void UpdateAMSWireStatusBar();
};
    
    


#endif /* SLOWGUI_H */
