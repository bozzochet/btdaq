#include "MainFrame.h"
#include "Messages.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// ProcessMessage
// Process any message from the Panel
////////////////////////////////////////////////////////////////////////////   
Bool_t MainFrame::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
  FILE *ifp;
  char aFile[100];
  char aMessage[200];
  char mydate[100]; 
  time_t now;
  int msgval=0;
  int valsubmsg;

  msgval=GET_MSG(msg);

  //printf("msgval=%d\n", msgval);

  switch (msgval) {
  
  case kC_TDRSIGNAL:
    ProcessGUI();
    break;

  case kC_SETSHAPINGTIME:
    switch (GET_SUBMSG(msg)) {
    case kSSTTB_OK:
    case kSSTTB_SAVE:
      sprintf(aMessage,"Shaping time = %3.1f us",control->Shaping);
      toLOG->Send(kGUI,aMessage,1);
      break;
    }
    break;

  case kC_SETLEAKTEMP:
    switch (GET_SUBMSG(msg)) {
      case kSLTTB_OK:
      case kSLTTB_SAVE:
        now = time(NULL);
        strftime(mydate,26,"%d/%m/%y %H:%M:%S",localtime(&now));
        sprintf(aMessage,"%4.1f C",control->RoomTemp);    	
	fIdf82->SetText(aMessage);
	sprintf(aMessage,"%4.1f uA",control->Ileak80);    	
	fIdf92->SetText(aMessage);
	sprintf(aMessage,"%4.1f uA",control->Ileak90);    	
	fIdfA2->SetText(aMessage);


	sprintf(aFile,"%s/%s.des",control->fDirLadder,control->fLadderName); 
	sprintf(aMessage,"rm -f %s",aFile); system(aMessage);
	toLOG->Send(kGUI,aMessage,1);

	ifp=fopen(aFile,"w");

	fprintf(ifp,"%s\n",mydate);
	fprintf(ifp,"%s\n",control->fLadderName);
	fprintf(ifp,"%s\n",control->TempId);
	fprintf(ifp,"%-10.1f\n",control->HybridTemp);      
	fprintf(ifp,"%-10.1f\n",control->RoomTemp);
	fprintf(ifp,"%-10.1f\n",control->Ileak80);
	fprintf(ifp,"%-10.1f\n",control->Ileak90);
	//fprintf(ifp,"%s\n",control->Comment);
	fclose(ifp);
 
	if (control->AMSTest>='a') {
	  if (control->AMSTest=='v' || control->AMSTest=='w'){
	    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_SOURCE);       
	    toCTL->Send(kGUI,kSourceTest,0);
	  } else 
	    toCTL->Send(kGUI,kAMSTest,0);
	}
	gSystem->ProcessEvents();
	break;
      case kSLTTB_CANCEL:
	control->AMSTest='@';
        break;
    }
    break;

  case kC_SETGAINPAR:
    break;

  case kC_NEWLADDER:
    switch (GET_SUBMSG(msg)) {
    case kNLTB_OK:
      toCTL->Send(kGUI,kCreateLadder,0);
      break;
    case kNLTB_HYB_OK:
      toCTL->Send(kGUI,kCreateLadder,0);
      break;	
    }
    break;
  
  case kC_TEXTENTRY:

    valsubmsg=GET_SUBMSG(msg);
    //printf("we are in textentry, submsg=%d\n",valsubmsg);

    switch (valsubmsg) {
      //case kTE_TEXTCHANGED:
    case kTE_ENTER:
      //printf("calling processtextentry\n");
      ProcessTextEntry(parm1,1);
      break;
    }
    break;

  case kC_COMMAND:
    switch (GET_SUBMSG(msg)) {
    case kCM_MENU:
      ProcessMenu(parm1,parm2);
      break;
    case kCM_COMBOBOX:
      ProcessComboBox(parm1,parm2);
      break;
    case kCM_BUTTON:
      ProcessTextButton(parm1,parm2);
      break;
    case kCM_CHECKBUTTON:
      ProcessCheckButton(parm1,parm2);
      break;
    }
  }
  return kTRUE;
}


void MainFrame::ProcessMenu(Long_t parm1, Long_t parm2){
  int retval;
  int aStation;
  char MsgMessage[200];
  char filename[200];
  char TestDesc[80];
  switch (parm1) {
  case kM_FILE_NEW_LADDER:
    if (control->RunStatus == kRUNSTATUS_STOP)
      toCTL->Send(kGUI,kNewLadder,0);
    else {
      strcpy  (MsgMessage,"If you want to change ladder\n");
      strcat(MsgMessage,"Please STOP the Run\n");
      new TGMsgBox(fClient->GetRoot(),this,"New Ladder",MsgMessage,
		   kMBIconExclamation,kMBDismiss,&retval);
    }
    break;        
  case kM_FILE_DIRECTORIES:
    break;
  case kM_FILE_SHOWRUNSUMMARY:
    sprintf(filename,"%s/Run.Summary",control->fDirLadder);
    new ShowFile(gClient->GetRoot(),this,850,400,
		 filename,filename);
    break;
  case kM_FILE_SETSHAPINGTIME:
    mySetShapingTime = new SetShapingTime(gClient->GetRoot(),this,200,100);
    break;
  case kM_FILE_RESETHW:
    toCTL->Send(kGUI,kResetHW,0);    
    break;
  case kM_FILE_PRINT:
    //printf("let us try the printing function\n");
    //cout << fClient->GetRoot() << " " << gClient->GetRoot() << " " << this << " " << fCanvas <<  endl;
    //cout << fh[fShowHisto]->GetTitle() << endl;
    //cout << "ok ??\n";
    if (control->RunStatus == kRUNSTATUS_STOP)
      //      myPrint=new SetPrinter(fClient->GetRoot(),this,300,180,fCanvas,fh[fShowHisto]->GetTitle());
      myPrint=new SetPrinter(fClient->GetRoot(),this,300,180,fCanvas,"out");
    else{ 
      strcpy(MsgMessage,"If you want to print any plot\n");
      strcat(MsgMessage,"    Please STOP the Run\n");
      new TGMsgBox(fClient->GetRoot(),this,"Print Message",MsgMessage,
		   kMBIconExclamation,kMBDismiss,&retval);
    }     
    break;
  case kM_FILE_PREFERENCES:
    printf("not implemented yet\n");
    break;
  case kM_FILE_EXIT:
    CloseWindow();    
    break;

  case kM_STATION_GENEVA:
    aStation=kSTATION_GENEVA;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;
  case kM_STATION_PERUGIA:
    aStation=kSTATION_PERUGIA;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;
  case kM_STATION_GYA:
    aStation=kSTATION_GYA;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;
  case kM_STATION_PREVESSIN:
    aStation=kSTATION_PREVESSIN;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;
  case kM_STATION_EDUARDO:
    aStation=kSTATION_EDUARDO;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;
  case kM_STATION_GIOVANNI:
    aStation=kSTATION_GIOVANNI;
    toCTL->Send(kGUI,kSetStation,aStation);
    break;

  case kM_CAL_SET_PARAMETERS:
    mySetCalPar = new SetCalPar(gClient->GetRoot(),this,250,560);
    break;

  case kM_CAL_SET_DEFAULT:
    //SetDefaultCalibration();
    SetLastCalibrationAsDefault();
    break;
  case kM_CAL_DYNAMIC:
    break;
  case kM_CAL_PASS_ALL:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_ALL);
    toCTL->Send(kGUI,kStartRun,0);
    break;
  case kM_CAL_PASS_0:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_0);
    toCTL->Send(kGUI,kStartRun,0);
    break;
  case kM_CAL_PASS_1:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_1);
    toCTL->Send(kGUI,kStartRun,0);
    break;
  case kM_CAL_PASS_2:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_2);
    toCTL->Send(kGUI,kStartRun,0);
    break;
  case kM_CAL_PASS_3:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_3);
    toCTL->Send(kGUI,kStartRun,0);
    break;
  case kM_CAL_PASS_4:
    toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
    toCTL->Send(kGUI,kSetupCalibration,cPASS_4);
    toCTL->Send(kGUI,kStartRun,0);
    break;

  case kM_CAL_SUMMARY:
    toCTL->Send(kGUI,kWriteCalSummary,0);
    break;

  case kM_GAIN_SET_PARAMETERS:
    mySetGainPar = new SetGainPar(gClient->GetRoot(),this,210,490);
    break;
  case kM_GAIN_START:
    break;

  case kM_QUAL_TEST_A:
    control->AMSTest='a';
    sprintf(TestDesc,"Hybrid Calibration"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_B:
    control->AMSTest='b';
    sprintf(TestDesc,"Ladder Assembly"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_C:
    control->AMSTest='c';
    sprintf(TestDesc,"Reception at Perugia"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_D:
    control->AMSTest='d';
    sprintf(TestDesc,"Reception at Geneva"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_E:
    control->AMSTest='e';
    sprintf(TestDesc,"Hybrid Box Installation"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_F:
    control->AMSTest='f';
    sprintf(TestDesc,"Leg Gluing"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_G:
    control->AMSTest='g';
    sprintf(TestDesc,"Spacer Gluing"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_H:
    control->AMSTest='h';
    sprintf(TestDesc,"Shield Wrapping"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_I:
    control->AMSTest='i';
    sprintf(TestDesc,"Preinstallation"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_M:
    control->AMSTest='m';
    sprintf(TestDesc,"Metrology"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_P:
    control->AMSTest='p';
    sprintf(TestDesc,"Plane Installation"); 
    AMSTest(TestDesc);
    break;
  case kM_QUAL_TEST_V:
    control->AMSTest='v';
    sprintf(TestDesc,"Source - Reception at Geneva ");
    SourceTest(TestDesc);
    break;
  case kM_QUAL_TEST_W:
    control->AMSTest='w';
    sprintf(TestDesc,"Source - After Shielding Wrapping");
    SourceTest(TestDesc);
    break;

  case kM_AMSWIRE_STATUS:
    myAMSWireWindow = new AMSWireWindow(gClient->GetRoot(),this,250,560);
    break;


  case kM_HELP_AMS:
    strcpy(MsgMessage,"http://www.ams02.org\n");
    new TGMsgBox(fClient->GetRoot(),this,"AMS web site",MsgMessage,
		 kMBIconAsterisk,kMBDismiss,&retval);
    break;
  case kM_HELP_SYSTEM:
    strcpy(MsgMessage,"Well, well...\n");
    strcat(MsgMessage,"How to use this program should be\n");
    strcat(MsgMessage,"self-explaining ;-)...\n");
    new TGMsgBox(fClient->GetRoot(),this,"Help ???",MsgMessage,
		 kMBIconAsterisk,kMBDismiss,&retval);
    break;
  case kM_HELP_ABOUT:
    strcpy(MsgMessage,Form("AMS Tracker Test System version %s\n",control->Version));
    strcat(MsgMessage,"2000-2004, Eduardo Cortina - University of Geneva\n");
    strcat(MsgMessage,"2005-2010, Philipp Azzarello - INFN Perugia\n");
    strcat(MsgMessage,"Support: Philipp.Azzarello@cern.ch\n");
    new TGMsgBox(fClient->GetRoot(),this,"About",MsgMessage,
		 kMBIconAsterisk,kMBDismiss,&retval);
    break;
  }
}


void MainFrame::GetVAFromChannel(float channel, int &va, int &vach) {

  // unhappy choise taken more than a decade ago, first channel is n. 1, first VA channel is also n. 1
  // thus channel = 1 + (VA-1)*64 + (VACH-1) = (VA-1)*64 + VACH

  int ichannel = (int)(channel);
  va=(ichannel-1)/channel_per_va + 1;
  vach=ichannel-(va-1)*channel_per_va;
}


void MainFrame::GetChannelFromVA(int va, int vach, float &channel) {

  channel = (va-1)*channel_per_va + vach;

}


void MainFrame::ProcessTextEntry(Long_t parm1, Long_t parm2){

  switch (parm1) {
  case kTE_HISTOS_TDR:
    control->TDRref = atoi(fSHTb72->GetString());
    if (control->TDRref<0) control->TDRref=0;
    if (control->TDRref>23) control->TDRref=23;
    //printf("tdr=%d\n",control->TDRref);
    break;
  case kTE_HISTOS_DACCHAN:
    //if (parm2 != 0) control->DACChannel = atoi(fSHTb42->GetString());  
    //if (control->DACChannel>384) control->DACChannel = 384;
    //if (control->DACChannel<1)   control->DACChannel = 1;
    break;
  case kTE_HISTOS_DACVAL:
    if (parm2 != 0) control->DACValue = atoi(fSHTb52->GetString());  
    if (control->DACValue>255) control->DACValue = 255;
    if (control->DACValue<0)   control->DACValue = 0;
    break;
  case kTE_HISTOS_MINX:
    if (parm2 != 0) fMinimumX = atof(fHOTb32->GetString());  
    //ffirstva = ((int) fMinimumX)/channel_per_va + 1;
    //ffirstch = 1 + ((int) fMinimumX) - (ffirstva-1)*channel_per_va;
    GetVAFromChannel(fMinimumX, ffirstva, ffirstch);
    printf("histo_minx: first_va=%d, first_ch=%d\n",ffirstva, ffirstch);
    UpdateMinimumX(fMinimumX);  
    fHOf12->Select(ffirstva);
    fHOf14->Select(ffirstch);
    break;
  case kTE_HISTOS_MAXX:
    printf("fMaximumX=%f\n",fMaximumX);
    if (parm2 != 0) fMaximumX = atof(fHOTb42->GetString());  
    printf("fMaximumX=%f\n",fMaximumX);
    //flastva  = ((int)(fMaximumX))/channel_per_va ;
    //flastch  = 1 + ((int)(fMaximumX)) - (flastva-1)*channel_per_va;
    GetVAFromChannel(fMaximumX, flastva, flastch);
    printf("histo_maxx: fmaximumx=%f, last_va=%d, last_ch=%d\n",fMaximumX,flastva, flastch);
    UpdateMaximumX(fMaximumX);  
    fHOf22->Select(flastva);
    fHOf24->Select(flastch);
    break;
  case kTE_HISTOS_MINY:
    if (parm2 != 0) fMinimumY = atof(fHOTb52->GetString()); 
    UpdateMinimumY(fMinimumY);  
    break;
  case kTE_HISTOS_MAXY:
    if (parm2 != 0) fMaximumY = atof(fHOTb62->GetString()); 
    UpdateMaximumY(fMaximumY); 
    break;
  case kTE_TDRDSP_CODE:
    control->TDRprog=(int)fNEtdrdsp->GetHexNumber();
    //printf("tdr dsp code is now %x\n",control->TDRprog);
    break;
  case kTE_AMSWSL:
    control->JINFaddr=(int)fNEamswsl->GetHexNumber();
    //printf("tdr dsp code is now %x\n",control->TDRprog);
    break;
  }
  UpdateMainFrame();
  gSystem->ProcessEvents();
  UpdateMainFrame();
}


void MainFrame::ProcessComboBox(Long_t parm1, Long_t parm2){
  char amessage[200];

  switch (parm1) {
  case kCB_XTANT:
    control->TrackerXtant=parm2-1;
    sprintf(amessage,"TCrate %d is selected",parm2);
    toLOG->Send(kGUI,amessage,1);
    toCTL->Send(kGUI,kSetOctant,0);
    break;
  case kCB_LOG_STATUS:
    if (control->RunStatus == kRUNSTATUS_STOP)
      toCTL->Send(kGUI,kSetLogStatus,parm2);
    else 
      fIdf22->Select(control->LogStatus);
    break;
  case kCB_DATA_MODE:
    if (control->RunStatus == kRUNSTATUS_STOP)
      switch (parm2) {
        case cPASS_0:
        case cPASS_1:
        case cPASS_2:
        case cPASS_3:
        case cPASS_4:
        case cPASS_5:
        case cPASS_ALL:
	  toCTL->Send(kGUI,kSetDataMode,kDATAMODE_CALIBRATION);
	  toCTL->Send(kGUI,kSetupCalibration,parm2);
	  break;
	case kFULLCALIBRATION:
  	  toCTL->Send(kGUI,kSetDataMode,parm2);       
	  toCTL->Send(kGUI,kFullCalibration,0);
	  break;      
      case kOCTANTCALIBRATION:
	toCTL->Send(kGUI,kSetDataMode,parm2);
	//toCLT->Send(kGUI,kOctantCalibration,0);
	break;
      case kDSPCALIBRATION:
	toCTL->Send(kGUI,kSetDataMode,parm2);       
	//toCTL->Send(kGUI,kDSPCalibration,0);
	break;
	case kDATAMODE_GAIN:
  	  toCTL->Send(kGUI,kSetDataMode,parm2);
	  toCTL->Send(kGUI,kSetupGain,0);
	  break;
	case kDATAMODE_SCOPE:
  	  toCTL->Send(kGUI,kSetDataMode,parm2);
	  fShowHisto=kHISTO_SHAPING;
	  ShowHisto();
	  UpdateMainFrame();
	  break;
        case kDATAMODE_COSMIC:
	  toCTL->Send(kGUI,kSetDataMode,parm2);
	  break;
	default:
  	  toCTL->Send(kGUI,kSetDataMode,parm2);       
	  break;  
      }
    else 
      fIdf32->Select(control->DataMode);
    break;
  case kCB_TRIGGER:
    if (control->RunStatus == kRUNSTATUS_STOP) 
      toCTL->Send(kGUI,kSetTrigger,parm2);
    else
      fIdf42->Select(control->Trigger);      
    break;
  case kCB_SOURCE:
    if (control->RunStatus == kRUNSTATUS_STOP) { 
      toCTL->Send(kGUI,kSetSource,parm2);
      switch (parm2) {
      case kSOURCE_BINARYFILE:
	SetFile(parm2);
	break;
      case kSOURCE_ASCIIFILE:
	SetFile(parm2);
	break;
      case kSOURCE_BEAMTESTX5:
	SetFile(parm2);
	break;
      case kSOURCE_TDRCDP:
	SetFile(parm2);
	break;
      }
    } else
      fIdf52->Select(control->Source);
    break;
  case kCB_HISTOS_HISTOGRAM:
    fShowHisto=parm2;
    ShowHisto();
    UpdateMainFrame();
    break;
  case kCB_HISTOS_VA:
    fva=parm2;
    control->DACva = fva;
    ShowHisto();
    UpdateMainFrame();
    break;
  case kCB_HISTOS_CHANNEL:
    fch=parm2;
    control->DACch = fch;
    ShowHisto();
    UpdateMainFrame();
    break;
  case kCB_HISTOS_FIRST_VA:
    ffirstva=parm2;
    //fMinimumX=(ffirstva-1)*channel_per_va+(ffirstch-1)+0.5;
    GetChannelFromVA(ffirstva, ffirstch, fMinimumX);
    printf("HISTOS_FIRST_VA: fminimumx=%f\n",fMinimumX);
    UpdateMinimumX(fMinimumX);  
    UpdateMainFrame();
    break;
  case kCB_HISTOS_LAST_VA:
    flastva=parm2;
    //fMaximumX=(flastva-1)*channel_per_va+(flastch)+0.5;    
    GetChannelFromVA(flastva, flastch, fMaximumX);
    printf("HISTOS_LAST_VA: lastvanew=%d, fmaximumx=%f\n",flastva,fMaximumX);  
    UpdateMaximumX(fMaximumX);  
    UpdateMainFrame();
    break;
  case kCB_HISTOS_FIRST_CHANNEL:
    ffirstch=parm2;
    //fMinimumX=(ffirstva-1)*channel_per_va+(ffirstch-1)+0.5;
    GetChannelFromVA(ffirstva, ffirstch, fMinimumX);
    printf("HISTOS_FIRST_CH: fminimumx=%f\n",fMinimumX);
    UpdateMinimumX(fMinimumX);  
    UpdateMainFrame();
    break;
  case kCB_HISTOS_LAST_CHANNEL:
    flastch=parm2;
    //fMaximumX=(flastva-1)*channel_per_va+flastch+0.5;  
    GetChannelFromVA(flastva, flastch, fMaximumX);    
    printf("HISTOS_LAST_CH: fmaximumx=%f\n",fMaximumX);  
    UpdateMaximumX(fMaximumX);  
    UpdateMainFrame();
    break;
  case kCB_HISTOS_REMAIN:
    fRemain=parm2;
    UpdateMainFrame();
    break;
  default:
   break;
  }
  if (control->Source == kSOURCE_SIMULATION) {
    toCTL->Send(kGUI,kSetTrigger,kTRIGGER_RANDOM);
    fIdf42->Select(kTRIGGER_RANDOM);
    gSystem->ProcessEvents();
  }
  if (control->Source == kSOURCE_BINARYFILE ||
      control->Source == kSOURCE_ASCIIFILE  ||
      control->Source == kSOURCE_BEAMTESTX5 ||
      control->Source == kSOURCE_TDRCDP){
    toCTL->Send(kGUI,kSetLogStatus,kLOGSTATUS_NOTLOG);
    fIdf22->Select(kLOGSTATUS_NOTLOG);
    gSystem->ProcessEvents();
  }
}


void MainFrame::ProcessCheckButton(Long_t parm1, Long_t parm2){

  switch (parm1) {
  case kCHB_SHOW_BAD:
    //printf("isdown=%d, ison=%d\n",fChk1->IsDown(),fChk1->IsOn());
    control->ShowBadChannels=fChk1->IsOn();
    break;
  case kCHB_PWG_MODE:
    control->ReadoutPWG=fChk2->IsOn();
    toCTL->Send(kGUI,kSetReferenceTDR,0);
    break;
  case kCHB_RECORD_GIF:
    control->RecordGifMovie=fChk3->IsOn();
    break;
  case kCHB_DYN_PED:
    control->DynPed=fChk4->IsOn();
    break;
  }

}

void MainFrame::ProcessTextButton(Long_t parm1, Long_t parm2){
  int retval;
  char MsgMessage[200];

  switch (parm1) {
  case kTB_START_RUN:
    if (control->RunStatus == kRUNSTATUS_STOP) {
	//if (fIdf61->GetState() == kButtonUp) control->OnlyHybrid = 0;
	//if (fIdf61->GetState() == kButtonDown) control->OnlyHybrid = 1; 
      toCTL->Send(kGUI,kStartRun,0);}
    else {
      fRCf11->ChangeBackground(fRed);
      fRCf11->SetState(kButtonDown); 
      fRCf12->ChangeBackground(fGreen);
      fRCf12->SetState(kButtonUp); 
      gClient->NeedRedraw(fRCf11);
      gClient->NeedRedraw(fRCf12);
      gSystem->ProcessEvents();
    }
    break;
  case kTB_STOP_RUN:
    if (control->RunStatus == kRUNSTATUS_RUN) 
      toCTL->Send(kGUI,kStopRun,0);
    else {
      fRCf11->ChangeBackground(fGreen);
      fRCf11->SetState(kButtonUp); 
      fRCf12->ChangeBackground(fRed);
      fRCf12->SetState(kButtonDown); 
      gClient->NeedRedraw(fRCf11);
      gClient->NeedRedraw(fRCf12);
      gSystem->ProcessEvents();
    }
    break;

  case kTB_READ_TEMP:
    if (control->RunStatus == kRUNSTATUS_STOP){
      toCTL->Send(kGUI,kReadTemp,0);
	//strcpy(MsgMessage,"   This option has not been \n");
	//strcat(MsgMessage,"      still implemented.\n\n");
	//strcat(MsgMessage,"Support: Eduardo.Cortina@physics.unige.ch\n");
	//new TGMsgBox(fClient->GetRoot(),this,"Read Hybrid Temperature",
	//	   MsgMessage,kMBIconExclamation,kMBDismiss,&retval);
    }
    break;

  case kTB_ENTER_TEMP_LEAK:
    if (control->RunStatus == kRUNSTATUS_STOP)
      mySetLeakTemp = new SetLeakTemp(gClient->GetRoot(),this,300,290); //140
    break;

  case kTB_ANALOG_S1:
    if (control->RunStatus == kRUNSTATUS_STOP){
      if (control->ActiveS1 == 1){
	fIdbutS1->ChangeBackground(fRed);
	fIdbutS1->SetState(kButtonDown);
	control->ActiveS1 = 0;
      } else {
	fIdbutS1->ChangeBackground(fGreen);
	fIdbutS1->SetState(kButtonUp);
	control->ActiveS1 = 1;
      }
      gClient->NeedRedraw(fIdbutS1);
      gSystem->ProcessEvents();
    }
    break;

  case kTB_ANALOG_S2:
    printf("runstatusS2: %i %i\n",control->RunStatus,kRUNSTATUS_STOP);
    if (control->RunStatus == kRUNSTATUS_STOP){
      if (control->ActiveS2 == 1){
	fIdbutS2->ChangeBackground(fRed);
	fIdbutS2->SetState(kButtonDown);
	control->ActiveS2 = 0;
      } else {
	fIdbutS2->ChangeBackground(fGreen);
	fIdbutS2->SetState(kButtonUp);
	control->ActiveS2 = 1;
      }
      gClient->NeedRedraw(fIdbutS2);
      gSystem->ProcessEvents();
    }
    break;


  case kTB_ANALOG_K:
    if (control->RunStatus == kRUNSTATUS_STOP){
      if (control->ActiveK == 1){
	fIdbutK->ChangeBackground(fRed);
	fIdbutK->SetState(kButtonDown);
	control->ActiveK = 0;
      } else {
	fIdbutK->ChangeBackground(fGreen);
	fIdbutK->SetState(kButtonUp);
	control->ActiveK = 1;
      }
      gClient->NeedRedraw(fIdbutK);
      gSystem->ProcessEvents();
    }
    break;

  case kTB_CHECK_ALL:
    if (control->RunStatus == kRUNSTATUS_STOP){    
      CheckProcess(kCTL);
      CheckProcess(kDSP);
      CheckProcess(kRUN);
      CheckProcess(kLOG);
      gSystem->ProcessEvents();
    }
    break;
  case kTB_CHECK_CONTROL:
    if (control->RunStatus == kRUNSTATUS_STOP){
      CheckProcess(kCTL);
      gSystem->ProcessEvents();
    }
    break;
  case kTB_CHECK_DSP:
    if (control->RunStatus == kRUNSTATUS_STOP){
      CheckProcess(kDSP);
      gSystem->ProcessEvents();
    }
    break;
  case kTB_CHECK_RUN:
    if (control->RunStatus == kRUNSTATUS_STOP){
      CheckProcess(kRUN);
      gSystem->ProcessEvents();
    }
    break;
  case kTB_CHECK_LOG:
    if (control->RunStatus == kRUNSTATUS_STOP){
      CheckProcess(kLOG);
      gSystem->ProcessEvents();
    }
    break;

  case kTB_HISTOS_MINX_MUL2:    
    fMinimumX *= 2.;
    ProcessTextEntry(kTE_HISTOS_MINX,0);
    break;
  case kTB_HISTOS_MINX_DIV2:    
    fMinimumX /= 2.;
    ProcessTextEntry(kTE_HISTOS_MINX,0);
    break;

  case kTB_HISTOS_MAXX_MUL2:    
    fMaximumX *= 2.;
    ProcessTextEntry(kTE_HISTOS_MAXX,0);
    break;
  case kTB_HISTOS_MAXX_DIV2:    
    fMaximumX /= 2.;
    ProcessTextEntry(kTE_HISTOS_MAXX,0);
    break;

  case kTB_HISTOS_MINY_MUL2:    
    fMinimumY *= 2.;
    ProcessTextEntry(kTE_HISTOS_MINY,0);
    break;
  case kTB_HISTOS_MINY_DIV2:    
    fMinimumY /= 2.;
    ProcessTextEntry(kTE_HISTOS_MINY,0);
    break;

  case kTB_HISTOS_MAXY_MUL2:    
    fMaximumY *= 2.;
    ProcessTextEntry(kTE_HISTOS_MAXY,0);
    break;
  case kTB_HISTOS_MAXY_DIV2:    
    fMaximumY /= 2.;
    ProcessTextEntry(kTE_HISTOS_MAXY,0);
    break;
  }

  for (int tdr=0; tdr<24; tdr++) {
    if (parm1==kTB_TDRSEL_0+tdr) {
      Pixel_t color=fTDRButton[control->TDRref]->GetDefaultFrameBackground();
      fTDRButton[control->TDRref]->SetBackgroundColor(color);
      gClient->NeedRedraw(fTDRButton[control->TDRref]);
      control->TDRref=tdr;
      printf("sending to ctl ref TDR\n");
      toCTL->Send(kGUI,kSetReferenceTDR,0);
      fTDRButton[control->TDRref]->ChangeBackground(fGreen);
      gClient->NeedRedraw(fTDRButton[control->TDRref]);
      break;
    }
  }


}
