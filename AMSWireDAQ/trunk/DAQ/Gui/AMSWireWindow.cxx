#include "AMSWireWindow.h"
#include "Messages.h"
#include <WidgetMessageTypes.h>
#include "amswcom.h"
#include <TG3DLine.h>

AMSWireWindow::AMSWireWindow(const TGWindow *p, const TGWindow *main, 
		     UInt_t w, UInt_t h) 
  : TGTransientFrame(p, main, w, h) 
{
  SetCleanup(kDeepCleanup);
  char value[200];

  // Creating share memory segments
  char file[200];
#include "shm_definition.code"

  AmswHardware=control->Hardware;
  AmswEppPort=control->EPPport;
  AmswPciPort=control->PCIport;

  TGLayoutHints *layLeft=new TGLayoutHints(kLHintsLeft,1,1,1,1);
  TGLayoutHints *layRight=new TGLayoutHints(kLHintsRight,1,1,1,1);
  TGLayoutHints *layExpX=new TGLayoutHints(kLHintsExpandX,1,1,1,1);
  TGLayoutHints *lay0ExpX=new TGLayoutHints(kLHintsExpandX,0,0,0,0);
  TGLayoutHints *lay0ExpY=new TGLayoutHints(kLHintsExpandY,0,0,0,0);


  fMain=main;

  TGVerticalFrame *vframe=new TGVerticalFrame(this,200,40);
  TGHorizontalFrame *hwframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *portframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *subaddframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *closeframe=new TGHorizontalFrame(vframe,40,200);
  TGHorizontalFrame *sumframe=new TGHorizontalFrame(vframe,40,200);

  //  TGHorizontalFrame *portframe=new TGHorizontalFrame(vframe,40,200);

  commlabel=new TGLabel(hwframe,"Communication mode:");
  hwframe->AddFrame(commlabel,layLeft);

  HWCombo=new TGComboBox(hwframe, kSAMSWCB_HWTYPE);
  HWCombo->Associate(this);
  HWCombo->AddEntry("PCI",kAMSW_PCI);
  HWCombo->AddEntry("EPP",kAMSW_EPP);
  HWCombo->Select((AmswHardware==kHARDWARE_JINF_PCI)?kAMSW_PCI:kAMSW_EPP);
  HWCombo->Resize(70,20);
  //aCombo->Connect("Selected(Int_t)","SlowGui",this,"SelectCommunicationMode(Int_t)");
  hwframe->AddFrame(HWCombo, layRight);
  vframe->AddFrame(hwframe, layExpX);

  portlabel=new TGLabel(portframe,"Communication port:");
  portframe->AddFrame(portlabel,layLeft);

  PortCombo=new TGComboBox(portframe, kSAMSWCB_HWPORT);
  PortCombo->Associate(this);
  PortCombo->AddEntry("0",0);
  PortCombo->AddEntry("1",1);
  PortCombo->AddEntry("2",2);
  PortCombo->AddEntry("3",3);
  PortCombo->AddEntry("4",4);
  PortCombo->AddEntry("5",5);
  PortCombo->AddEntry("6",6);
  PortCombo->AddEntry("7",7);
  PortCombo->Select((AmswHardware==kAMSW_PCI)?AmswPciPort:AmswEppPort);
  PortCombo->Resize(70,20);

  portframe->AddFrame(PortCombo,layRight);
  vframe->AddFrame(portframe,layExpX);

  subaddlabel=new TGLabel(subaddframe,"Node address:");
  subaddframe->AddFrame(subaddlabel, layLeft);
  
  NodeNumEnt=new TGNumberEntry(subaddframe,0xffff,5,kSAMSWTE_SUBADD);
  NodeNumEnt->Associate(this);
  NodeNumEnt->SetFormat(TGNumberFormat::kNESHex, TGNumberFormat::kNEANonNegative);
  NodeNumEnt->SetLimits(TGNumberFormat::kNELLimitMinMax,0,0xffff);
  NodeNumEnt->Resize(70,20);
  subaddframe->AddFrame(NodeNumEnt, layRight);
  vframe->AddFrame(subaddframe,layExpX);

  TGVerticalFrame *columnframe;
  columnframe=new TGVerticalFrame(sumframe,200,40);
  for (int i=0; i<9; i++) {
    if (i==0) sectlabel[i]=new TGLabel(columnframe,"Sector");
    else {
      sectlabel[i]=new TGLabel(columnframe,Form("0x0000",i));
      sectlabel[i]->SetTextColor(0x00ff00);
      sectlabel[i]->SetBackgroundColor(0x000000);
    }
    columnframe->AddFrame(sectlabel[i],lay0ExpX);
    TGHorizontal3DLine *horline=new TGHorizontal3DLine(columnframe);
    columnframe->AddFrame(horline,lay0ExpX);
  }
  sumframe->AddFrame(columnframe,layExpX);  

  TGVertical3DLine *vertline=new TGVertical3DLine(sumframe);
  sumframe->AddFrame(vertline, lay0ExpY);

  columnframe=new TGVerticalFrame(sumframe,200,40);
  for (int i=0; i<9; i++) {
    if (i==0) idlabel[i]=new TGLabel(columnframe,"File id");
    else {
      idlabel[i]=new TGLabel(columnframe,Form("0x0000",i));
      idlabel[i]->SetTextColor(0x00ff00);
      idlabel[i]->SetBackgroundColor(0x000000);
    }
    columnframe->AddFrame(idlabel[i],lay0ExpX);
    TGHorizontal3DLine *horline=new TGHorizontal3DLine(columnframe);
    columnframe->AddFrame(horline,lay0ExpX);
  }
  sumframe->AddFrame(columnframe,layExpX);  

  vertline=new TGVertical3DLine(sumframe);
  sumframe->AddFrame(vertline, lay0ExpY);

  columnframe=new TGVerticalFrame(sumframe,200,40);
  for (int i=0; i<9; i++) {
    if (i==0) adhlabel[i]=new TGLabel(columnframe,"AddrH");
    else {
      adhlabel[i]=new TGLabel(columnframe,Form("0x0000",i));
      adhlabel[i]->SetTextColor(0x00ff00);
      adhlabel[i]->SetBackgroundColor(0x000000);
    }
    columnframe->AddFrame(adhlabel[i],lay0ExpX);
    TGHorizontal3DLine *horline=new TGHorizontal3DLine(columnframe);
    columnframe->AddFrame(horline,lay0ExpX);
  }
  sumframe->AddFrame(columnframe,layExpX);  

  vertline=new TGVertical3DLine(sumframe);
  sumframe->AddFrame(vertline, lay0ExpY);

  columnframe=new TGVerticalFrame(sumframe,200,40);
  for (int i=0; i<9; i++) {
    if (i==0) erslabel[i]=new TGLabel(columnframe,"EraseS");
    else {
      erslabel[i]=new TGLabel(columnframe,Form("0x0000",i));
      erslabel[i]->SetTextColor(0x00ff00);
      erslabel[i]->SetBackgroundColor(0x000000);
    }
    columnframe->AddFrame(erslabel[i],lay0ExpX);
    TGHorizontal3DLine *horline=new TGHorizontal3DLine(columnframe);
    columnframe->AddFrame(horline,lay0ExpX);
  }
  sumframe->AddFrame(columnframe,layExpX);  

  vertline=new TGVertical3DLine(sumframe);
  sumframe->AddFrame(vertline, lay0ExpY);

  columnframe=new TGVerticalFrame(sumframe,200,40);
  for (int i=0; i<9; i++) {
    if (i==0) stalabel[i]=new TGLabel(columnframe,"Stat");
    else {
      stalabel[i]=new TGLabel(columnframe,Form("0x0000",i));
      stalabel[i]->SetTextColor(0x00ff00);
      stalabel[i]->SetBackgroundColor(0x000000);
    }
    columnframe->AddFrame(stalabel[i],lay0ExpX);
    TGHorizontal3DLine *horline=new TGHorizontal3DLine(columnframe);
    columnframe->AddFrame(horline,lay0ExpX);
  }
  sumframe->AddFrame(columnframe,layExpX);  

  //  vertline=new TGVertical3DLine(sumframe);
  //  sumframe->AddFrame(vertline, lay0ExpY);

  vframe->AddFrame(sumframe,layExpX);

  CancelButton=new TGTextButton(closeframe,"Cancel",kSAMSWTB_CANCEL);
  CancelButton->Associate(this);
  OkButton=new TGTextButton(closeframe,"Ok",kSAMSWTB_OK);
  OkButton->Associate(this);

  closeframe->AddFrame(CancelButton,layLeft);
  closeframe->AddFrame(OkButton,layRight);
  vframe->AddFrame(closeframe,layLeft);
  AddFrame(vframe,layLeft);

  cout << "before map subwindows"<<endl;
  MapSubwindows();
  SetWindowName("AMSWire");
  SetIconName("AMSWire");

  Layout();
  Resize();
  CenterOnParent();
  MapWindow();
  gClient->WaitFor(this);
  cout<< "end of creation"<<endl;
}


Bool_t AMSWireWindow::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  switch (GET_MSG(msg)) {
  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_BUTTON:
      ProcessTextButton(parm1,parm2);
      break;
    case kCM_COMBOBOX:
      ProcessComboBox(parm1,parm2);
      break;
    }
    break;
  case kC_TEXTENTRY:
    ProcessTextEntry(parm1,parm2);
    break;
  default:
    printf("msg is %d\n", GET_MSG(msg));
    break;
  }
  return kTRUE;
}



void AMSWireWindow::ProcessTextEntry(Long_t parm1, Long_t parm2){
  cout << parm1 << " " << parm2 << endl;
  switch(parm1) {
  case kSAMSWTE_SUBADD:
    printf("nodeaddress is 0x%04x\n",NodeNumEnt->GetHexNumber());
    break;
  }
}



void AMSWireWindow::ProcessComboBox(Long_t parm1, Long_t parm2){
  cout << parm1 << " " << parm2 << endl;
  switch (parm1) {
  case kSAMSWCB_HWTYPE:
    if (parm2==kAMSW_PCI) AmswHardware=kHARDWARE_JINF_PCI;
    if (parm2==kAMSW_EPP) AmswHardware=kHARDWARE_JINF_EPP;
    break;
  case kSAMSWCB_HWPORT:
    printf("port %d is chosen\n", parm2);
    if (AmswHardware==kHARDWARE_JINF_PCI) AmswPciPort=parm2;
    if (AmswHardware==kHARDWARE_JINF_EPP) AmswEppPort=parm2;
    break;
  }
}


void AMSWireWindow::ProcessTextButton(Long_t parm1, Long_t parm2) {
  switch (parm1) {
//   case kSCPTB_OK:
//   case kSCPTB_SAVE:
//     Int_t aMessage;

//     control->EventsStep[0] = atoi(fSCP0Tb12->GetString());
//     control->EventsStep[1] = atoi(fSCP1Tb12->GetString());
//     control->EventsStep[2] = atoi(fSCP2Tb12->GetString());
//     control->SRSigmaCut          = atof(fSCP2Tb22->GetString());
//     control->SRBin               = atoi(fSCP2Tb32->GetString());
//     control->SRLow               = atof(fSCP2Tb42->GetString());
//     control->SRHigh              = atof(fSCP2Tb52->GetString());
//     control->EventsStep[3] = atoi(fSCP3Tb12->GetString());
//     control->SigmaLow            = atof(fSCP3Tb22->GetString());
//     control->SigmaHigh           = atof(fSCP3Tb32->GetString());
//     control->EventsStep[4] = atoi(fSCP4Tb12->GetString());
//     control->GASigmaCut          = atof(fSCP4Tb22->GetString());
//     control->GAOut               = atof(fSCP4Tb32->GetString()); 

//     //    if (parm1 == kSCPTB_SAVE) { } Save Calibration numbers on file
//     aMessage=MK_MSG((EWidgetMessageTypes)kC_SETCALPAR,
//                   (EWidgetMessageTypes)parm1);
//     SendMessage(fMain,aMessage,0,0);
//     CloseWindow();
//     break;

   case kSAMSWTB_CANCEL:
     CloseWindow();
     break;

   case kSAMSWTB_OK:
     control->Hardware=AmswHardware;
     control->EPPport=AmswEppPort;
     control->PCIport=AmswPciPort;
     CloseWindow();
     break;
  }	
}

void AMSWireWindow::CloseWindow(){
  //TGTransientFrame::CloseWindow();
  DeleteWindow();
}

AMSWireWindow::~AMSWireWindow()
{
#include "shm_detach.code"

}

