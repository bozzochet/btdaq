#ifndef MAINFRAME_H
#define MAINFRAME_H


#include <TROOT.h>
#include <TApplication.h> 
#include <TVirtualX.h> 
#include <TGClient.h>
#include <TGButton.h>
#include <TGLabel.h>
#include <TGMenu.h>
#include <TGTab.h>
#include <TGComboBox.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TGFrame.h>
#include <TGString.h>
#include <TGMsgBox.h>
#include <TSystem.h>
#include <TH1.h>
#include <TTimer.h>
#include <TGTextView.h>
#include <TGView.h>
#include <TLine.h>
#include <TStyle.h>
#include <TGFileDialog.h>
#include <TPostScript.h>
#include <TRandom.h>
#include <TColor.h>
#include <TTF.h>
#include <TBox.h>
#include <TGFont.h>

#include <stdio.h>
#include <iostream>
#include <iomanip>
//#include <strstream.h>
#include <fstream>
#include <sys/stat.h>
#include <time.h>

#include "NewLadder.h"
#include "SetCalPar.h"
#include "SetGainPar.h"
#include "SetShapingTime.h"
#include "SetLeakTemp.h"
#include "SelectLadder.h"
#include "ShowFile.h"
#include "SetPrinter.h"
#include "DefineFonts.h"
#include "AMSWireWindow.h"
#include "Queue.h"
#include "Share.h"


class MainFrame : public TGMainFrame {

private:

  // Queues
#define Q_CTL
#define Q_LOG
#define Q_GUI
#undef  Q_RUN
#undef  Q_DSP
#include "msg_define.code"
  long mtype;
  int par1;
  int par2;


  // Share memory
#define S_CONTROL
#undef  S_DATA
#define S_HISTO
#include "shm_define.code"

#define HISMAX  40
  TH1F *fh[HISMAX]; Axis_t fAxis[HISMAX][1026];

#define SCOPECOPYMAX 500
#define MAXTYP 4

  TH1F *fhh[MAXTYP][SCOPECOPYMAX];
  int fHistCnt;
//  TH1F *fhScope;        Axis_t faScope[HISTO_SCOPE_BIN+2];
//  TH1F *fhSignalMean;   Axis_t faSignalMean[HISTO_SIGNAL_MEAN_BIN+2]; 
//  TH1F *fhSignalHits;   Axis_t faSignalHits[HISTO_SIGNAL_HITS_BIN+2]; 
//  TH1F *fhRawCal;       Axis_t faRawCal[HISTO_RAWCAL_BIN+2];
//  TH1F *fhPedestal;     Axis_t faPedestal[HISTO_PEDESTAL_BIN+2];
//  TH1F *fhPedSum;       Axis_t faPedSum[HISTO_PEDSUM_BIN+2];
//  TH1F *fhSigmaRaw;     Axis_t faSigmaRaw[HISTO_SIGMARAW_BIN+2];
//  TH1F *fhSigRawSum;    Axis_t faSigRawSum[HISTO_SIGRAWSUM_BIN+2];
//  TH1F *fhCN;           Axis_t faCN[HISTO_CN_BIN+2];
//  TH1F *fhSigma;        Axis_t faSigma[HISTO_SIGMA_BIN+2];
//  TH1F *fhSigSum;       Axis_t faSigSum[HISTO_SIGSUM_BIN+2];
//  TH1F *fhGain;         Axis_t faGain[HISTO_GAIN_BIN+2];
//  TH1F *fhGainPlot;     Axis_t faGainPlot[HISTO_GAINPLOT_BIN+2];
//  TH1F *fhShaping;      Axis_t faShaping[HISTO_SHAPING_BIN+2]; 
//  TH1F *fhClusLength_S; Axis_t faClusLength_S[HISTO_CLUSTER_LENGTH_S_BIN+2];
//  TH1F *fhClusLength_K; Axis_t faClusLength_K[HISTO_CLUSTER_LENGTH_K_BIN+2];
//  TH1F *fhClusNumber_S; Axis_t faClusNumber_S[HISTO_CLUSTER_NUMBER_S_BIN+2];
//  TH1F *fhClusNumber_K; Axis_t faClusNumber_K[HISTO_CLUSTER_NUMBER_K_BIN+2];
//  TH1F *fhClusCog;      Axis_t faClusCog[HISTO_CLUSTER_COG_BIN+2];
//  TH1F *fhClusMaxloc;   Axis_t faClusMaxloc[HISTO_CLUSTER_MAXLOC_BIN+2];
//  TH1F *fhClusIntegral_S; 
//           Axis_t faClusIntegral_S[HISTO_CLUSTER_INTEGRAL_S_BIN+2];
//  TH1F *fhClusIntegral_K; 
//           Axis_t faClusIntegral_K[HISTO_CLUSTER_INTEGRAL_K_BIN+2];
//  TH1F *fhClusSovern_S; Axis_t faClusSovern_S[HISTO_CLUSTER_SOVERN_S_BIN+2];
//  TH1F *fhClusSovern_K; Axis_t faClusSovern_K[HISTO_CLUSTER_SOVERN_K_BIN+2];

  

  //Message Box
  // new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
  //              fMsgIconType,fMsgButtons,&retval);
  // icontype = kMBIconQuestion;  // see TGMsgBox.h
  // buttons = kMBYes | kMBNo;

  char fMsgTitle[100];
  char fMsgMessage[200];
  EMsgBoxIcon fMsgIconType;
  int fMsgButtons;

  // Colours
  ULong_t fGreen;
  ULong_t fRed;
  ULong_t fYellow;


  // Layouts  
  TGLayoutHints   *fL1;
  TGLayoutHints   *fL2;

  // TDR selection buttons
  TGTextButton *fTDRButton[24];
  TGGroupFrame     *fSelectTDR; 

  // Menu Bar Definition
  TGMenuBar   *fMenuBar; UInt_t fMenuBarOpt; 
  TGPopupMenu *fFile; UInt_t fItemOpt; TGLayoutHints *fItemLay;  
  TGPopupMenu *fStation;
  TGPopupMenu *fCal;
  TGPopupMenu *fGain;
  TGPopupMenu *fQualification;
  TGPopupMenu *fAMSWireMenu;
  TGPopupMenu *fHelp; UInt_t fHelpOpt; TGLayoutHints *fHelpLay;  

  // Title Definition
  TGCompositeFrame *fTitle;        UInt_t        fTitleOpt;
  TGPictureButton  *fLogoUG;       TGLayoutHints *fLogoUGLay;
  TGCompositeFrame *fTextFrame;    TGLayoutHints *fTextFrameLay;
  TGLabel          *fTitleText[3]; TGLayoutHints *fTitleTextLay[3];

  // Work Area
  TGCompositeFrame *fWork;

  // Control Area Definition
  TGCompositeFrame *fControl;
  //   +- Id SubFrame
  TGGroupFrame     *fId;  
  TGLabel *fIdf11; TGLabel    *fIdf12;   
  TGLabel *fIdf21; TGComboBox *fIdf22;
  TGLabel *fIdf31; TGComboBox *fIdf32;
  TGLabel *fIdf41; TGComboBox *fIdf42;
  TGLabel *fIdf51; TGComboBox *fIdf52;
  TGCheckButton *fIdf61; TGLabel *fIdf62;
  TGLabel *fIdf71; TGLabel*fIdf72; TGTextButton *fIdf73;
  TGLabel *fIdf81; TGLabel*fIdf82; TGTextButton *fIdf83;
  TGLabel *fIdf91; TGLabel*fIdf92; 
  TGLabel *fIdfA1; TGLabel*fIdfA2; 

  TGTextButton *fIdbutS1; TGTextButton *fIdbutS2; TGTextButton *fIdbutK;
  //   +- Run Info SubFrame
  TGGroupFrame     *fRunInfo; 
  TGLabel *fRIf11; TGLabel *fRIf12;
  TGLabel *fRIf21; TGLabel *fRIf22;
  TGLabel *fRIf31; TGLabel *fRIf32;
  TGLabel *fRIf41; TGLabel *fRIf42;
  TGLabel *fRIf51; TGLabel *fRIf52;
  TGLabel *fRIf61; TGLabel *fRIf62;
  TGLabel *fRIf71; TGLabel *fRIf72;
  //   +- Run Control SubFrame
  TGGroupFrame     *fRunControl; 
  TGTextButton *fRCf11; TGTextButton *fRCf12;
  //   +- Process Control SubFrame
  TGGroupFrame     *fProcessControl;
  TGTextButton *fPCf11; TGLabel      *fPCf12;
  TGTextButton *fPCf21; TGTextButton *fPCf22;
  TGTextButton *fPCf31; TGTextButton *fPCf32;


  // Display Frame
  TGCompositeFrame *fDisplay; 
  //        +- Tab Selection SubFrame
  TGCompositeFrame *fSel;
  TGCompositeFrame *fTDRSel[8];
  //              +- Select Histogram
  TGGroupFrame     *fSelectHisto;  
  TGLabel *fSHf11; TGComboBox *fSHf12;
  TGLabel *fSHf21; TGComboBox *fSHf22;
  TGLabel *fSHf31; TGComboBox *fSHf32;
  TGLabel *fSHf61; TGComboBox *fSHf62;
  TGLabel *fSHf41; TGTextEntry *fSHf42; TGTextBuffer *fSHTb42;
  TGLabel *fSHf51; TGTextEntry *fSHf52; TGTextBuffer *fSHTb52;
  TGLabel *fSHf71; TGTextEntry *fSHf72; TGTextBuffer *fSHTb72;
  TGComboBox *fComboXtant;

  TGNumberEntry *fNEtdrdsp, *fNEamswsl;

  //              +- Histogram Options
  TGGroupFrame *fHistoOptions;
  TGLabel *fHOf11; TGComboBox  *fHOf12; TGLabel *fHOf13; TGComboBox *fHOf14;
  TGLabel *fHOf21; TGComboBox  *fHOf22; TGLabel *fHOf23; TGComboBox *fHOf24;
  TGLabel *fHOf31; TGTextEntry *fHOf32; TGTextBuffer *fHOTb32; 
  TGTextButton *fHOf33; TGTextButton *fHOf34;
  TGLabel *fHOf41; TGTextEntry *fHOf42; TGTextBuffer *fHOTb42;
  TGTextButton *fHOf43; TGTextButton *fHOf44;
  TGLabel *fHOf45;
  TGLabel *fHOf51; TGTextEntry *fHOf52; TGTextBuffer *fHOTb52;
  TGTextButton *fHOf53; TGTextButton *fHOf54;
  TGLabel *fHOf55;
  TGLabel *fHOf61; TGTextEntry *fHOf62; TGTextBuffer *fHOTb62;
  TGTextButton *fHOf63; TGTextButton *fHOf64;
  TGLabel *fHOf65;  TGCheckButton *fChk1;

  TGCheckButton *fChk2, *fChk3, *fChk4;


  //        +- Canvas SubFrame
  TRootEmbeddedCanvas *fCanvas;


  //Text Frame
  TGTextView *fText;

  // Calibration Parameters frame
  SetCalPar *mySetCalPar;

  // Gain Calibration Parameters frame
  SetGainPar *mySetGainPar;

  // Set Shaping Time Frame
  SetShapingTime *mySetShapingTime;

  // Set Leakage and Temperature Frame
  SetLeakTemp *mySetLeakTemp;

  // Print Dialog Box
  SetPrinter *myPrint;

  // AMSwire dialog box
  AMSWireWindow *myAMSWireWindow;

  //TTF *myTTF;

  // Some control variables
  int fShowHisto; 
  int fva; int fch;
  int fRemain, fOldRemain;
  int ffirstva; int flastva;
  int ffirstch; int flastch;
  float fMinimumX; float fMaximumX;
  float fMinimumY; float fMaximumY;
  int fOldHisto;
  int fOldva; int fOldch;
  int fOldfirstva; int fOldlastva;
  int fOldfirstch; int fOldlastch;
  float fOldMinimumX; float fOldMaximumX;
  float fOldMinimumY; float fOldMaximumY;
  char fOldLadderName[15];
  int fOldDACChannel; int fOldDACVA; int fOldDACValue;
  int fOldTDRref;
public:
  MainFrame(const TGWindow *p, UInt_t w, UInt_t h);
  virtual ~MainFrame();
  void CreateStyles();
  void DefineHistos();
  void MenuBar();
  void Title();
  void Work();
  void Control();
  void LadderInfo();
  void RunInfo();  
  void RunControl();
  void ProcessControl();
  void ProcessOk(int aProc);
  void ProcessNotOk(int aProc);
  void Display();
  void Selection();
  void TDRSelection();
  void SelectHisto();
  void HistoOptions();
  void Canvas(); 
  void Text();
  virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
  void ProcessMenu(Long_t parm1, Long_t parm2);
  void ProcessTextEntry(Long_t parm1, Long_t parm2);
  void ProcessComboBox(Long_t parm1, Long_t parm2);
  void ProcessTextButton(Long_t parm1, Long_t parm2);
  void ProcessCheckButton(Long_t parm1, Long_t parm2);
  void CheckLadder();
  void ProcessGUI();
  void CloseWindow();
  void CheckProcess(int aProc);
  void UpdateMinimumX(float aMinimumX);
  void UpdateMaximumX(float aMaximumX);
  void UpdateMinimumY(float aMinimumY);
  void UpdateMaximumY(float aMaximumY);
  void UpdateMainFrame();
  void RunOnCurse();
  void RefreshRunInfo();
  void RefreshId();
  void RefreshLog();
  void RefreshLog2(char *message);
  int SetBounds(int ava, int min, int max);
  float SetBounds(float ava, float min, float max);
  void UpdateHisto(double *aLink);
  void UpdateHisto2(double *aLink);
  void ShowHisto();
  void GridVA();
  void ShowBad();
  void FileDialog(int aType, EFileDialogMode aOp);
  void SetFile(int aSource); 
  void CreateDirectory(char *aDirectory); 
  void SetDefaultCalibration();
  void SetLastCalibrationAsDefault();
  void AMSTest(char *TestDesc);
  void SourceTest(char *TestDesc);
  void CreateHisto(int aHisto,int aNbin,float aMinX, 
		   float aMaxX,float aMinY, float aMaxY, const char *aName);
  void CreateHisto2(int aHisto,int aNbin,float aMinX, 
		   float aMaxX,float aMinY, float aMaxY, const char *aName);
  void ShowTemp();
  void GetVAFromChannel(float channel, int &va, int &vach);
  void GetChannelFromVA(int va, int vach, float &channel);

  int HHConvert(int aHisto);

  void Test(){
    cerr << "Test: " <<  fIdf11->GetText() << endl;;
    fRIf32->SetText(new TGString("EDUARDO"));
    gClient->NeedRedraw(fRIf32);
    gSystem->ProcessEvents();
  }
};

#endif  /*MAINFRAME_H*/
