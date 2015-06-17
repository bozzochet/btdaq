#include "MainFrame.h"

void MainFrame::ProcessGUI(){
  int retval;
  char aMessage[100];
  char filename[200];
  SelectLadder *mySelectLadder;
  NewLadder *myNewLadder;


  //static int ctt;
  //if (++ctt%5 == 0  && control->RunStatus == kRUNSTATUS_PAUSE) { 
  //  toCTL->Send(kGUI,kRefreshTime,0);
  //  RefreshRunInfo();
  //}

  Pixel_t color;

  int messages = toGUI->NumberOfMessages();
  if (messages > 0){
    for (int i=0;i<messages;i++){
      toGUI->Receive(&mtype,&par1,&par2);

      switch (par1) {
      case kTimeOut:
	break;
       
      case kWakeUp:
	toCTL->Send(kGUI,kReady,0);
	break;

      case kLadderExist:
	CheckLadder();
	break;
	
      case kLadderNotExist:
	//cout << gClient->GetRoot() << " " << fClient->GetRoot() << " " << this << endl;
	mySelectLadder = new SelectLadder(gClient->GetRoot(),this,300,120);
	switch (control->LadderType) {
	case kLadderLast:
	  sprintf(control->fLadderName,"%s",control->fLadderNameAtExit);
	  toCTL->Send(kGUI,kCreateLadder,0);
	  break;
	case kLadderTest:
	  //control->fVoltage='T';
	  //strcpy(control->fSensors,"ES");
	  //control->fAssembly='T';
	  //control->fLayer='L';
	  //strcpy(control->fSerial,"ADD");
	  strcpy(control->fLadderName,"testladder");  
	  toCTL->Send(kGUI,kCreateLadder,0);
	  break;
	case kLadderAMS:
	  myNewLadder =  new NewLadder(1,gClient->GetRoot(),this,230,200);
	  break;
	case kLadderHybrid:
	  myNewLadder =  new NewLadder(2,gClient->GetRoot(),this,230,200);
	  break;  
	}
	break;
	
      case kDefineLadderPlease:
	fMsgIconType = kMBIconExclamation; 
	strcpy(fMsgTitle,"Ladder not Defined");
	sprintf(fMsgMessage,"%s\n%s",
		"Ladder has not been defined.",
		"Do you want to define a new one?");
	fMsgButtons = kMBYes | kMBDismiss;
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     fMsgIconType,fMsgButtons,&retval);
	if (retval == kMBYes) 
	  toCTL->Send(kGUI,kNewLadder,0);    
	break;
	
      case kLadderCreated:
	//if (par1 == kLadderCreated){ 
	fIdf12->SetText(new TGString(control->fLadderName));
	RefreshId();
	//	  char file[200];
	//	  strcpy(file,getenv("PWD"));
	//	  strcat(file,"/Control/");
	//	  strcat(file,control->fName);
	//	  strcat(file,".map");
	//	  cout << "Process: " << file << endl;
	//}
	break;
	
      case kLadderSaved:
	break;
	
      case kLadderDeleted:
	fIdf12->SetText(new TGString(" "));
	break;
	
      case kCheckProcess:
	break;
	
      case kProcessOk:
	ProcessOk(par2);
	break;
	
      case kProcessNotOk:
	ProcessNotOk(par2);
	break;

      case kCalibrationFinished:
	//toCTL->Send(kGUI,kWriteCalSummary,0);
	//fMsgIconType = kMBIconQuestion; 
	//strcpy(fMsgTitle,"Calibration Finished");
	//sprintf(fMsgMessage,"%s\n%s",
	//	"Ladder Calibration has finished.",
	//	"Do you want to save it?");
	//fMsgButtons = kMBYes | kMBNo;
	//new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
	//	     fMsgIconType,fMsgButtons,&retval);
	//switch (retval) {
	//case kMBYes:
	//  toCTL->Send(kGUI,kSaveCalibration,0);
	//  break;
	//case kMBNo:
	//  break;
	//}
	//break;

      case kShowCalSummary:
	sprintf(filename,"%s/%s.sum",control->fDirLadder,
		control->fLadderName);

	struct stat finfo;
	if (stat(filename, &finfo) == 0){
	  new ShowFile(gClient->GetRoot(),this,600,400,
		       "Calibration Summary",filename);
	} else {
	  sprintf(filename,"%s/%s_%05d.sum",control->fDirLadder,
		  control->fLadderName,control->NRun);
	  new ShowFile(gClient->GetRoot(),this,600,400,
		       "Calibration Summary",filename);
	}
	break;


      case kGainFinished:
	//toCTL->Send(kGUI,kWriteGainSummary,0);
	//fMsgIconType = kMBIconQuestion; 
	//strcpy(fMsgTitle,"Gain Calibration Finished");
	//sprintf(fMsgMessage,"%s\n%s",
	//	"Gain Calibration has finished.",
	//	"Do you want to save it?");
	//fMsgButtons = kMBYes | kMBNo;
	//new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
	//	     fMsgIconType,fMsgButtons,&retval);
	//switch (retval) {
	//case kMBYes:
	//  toCTL->Send(kGUI,kSaveGain,0);
	//  break;
	//case kMBNo:
	//  break;
	//}
	//break;

      case kShowGainSummary:
	sprintf(filename,"data/%s.sum",control->fLadderName);
	new ShowFile(gClient->GetRoot(),this,600,400,
		     "Gain Calibration Summary",filename);
	break;


      case kRefresh:
	RefreshId();
	RefreshRunInfo();
	//toCTL->Send(kGUI,kRefreshDone,0);
	break;

      case kRefreshLog:
	RefreshLog2(control->LogMessage);
	sprintf(control->LogMessage,"");
	//printf("Message: %s\n", control->LogMessage);
	break;
	
      case kRunStarted:
	fRCf11->ChangeBackground(fRed);
	fRCf11->SetState(kButtonDown); 
	fRCf12->ChangeBackground(fGreen);
	fRCf12->SetState(kButtonUp); 
	gClient->NeedRedraw(fRCf11);
	gClient->NeedRedraw(fRCf12);
	while (control->RunStatus == kRUNSTATUS_RUN){
	  UpdateMainFrame();
	  usleep(100000);    
	}
	gSystem->ProcessEvents();
	break;

      case kRunStoped:
	fRCf11->ChangeBackground(fGreen);
	fRCf11->SetState(kButtonUp); 
	fRCf12->ChangeBackground(fRed);
	fRCf12->SetState(kButtonDown); 
	gClient->NeedRedraw(fRCf11);
	gClient->NeedRedraw(fRCf12);
	gSystem->ProcessEvents();
	break;

      case kShowTemp:
	ShowTemp();
	break;


      case kHardware_Problem:
	fMsgIconType = kMBIconExclamation; 
	strcpy(fMsgTitle,"Hardware Problem");
	sprintf(fMsgMessage,"%s\n%s",
		"Hardware does not respond.",
		"Please switch it on or reset it");
	fMsgButtons = kMBOk;
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     fMsgIconType,fMsgButtons,&retval);

	break;


      case kTemperature_Problem:
	fMsgIconType = kMBIconExclamation; 
	strcpy(fMsgTitle,"Hardware Problem");
	sprintf(fMsgMessage,"%s\n%s",
		"Temperature sensor does not respond.",
		"Please check K hybrid connection");
	fMsgButtons = kMBOk;
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     fMsgIconType,fMsgButtons,&retval);

	break;


      case kTemperature_Error:
	fMsgIconType = kMBIconExclamation; 
	strcpy(fMsgTitle,"Temperature Problem");
	sprintf(fMsgMessage,"%s\n%s\n%s",
		"Problem in temperature reading.",
		"CRC check does not match.",
	        "Do you want to read the temperature again?");
	fMsgButtons = kMBYes | kMBNo ;
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     fMsgIconType,fMsgButtons,&retval);

	if (retval == kMBYes) toCTL->Send(kGUI,kReadTemp,0);
	break;

      case kSourceAck2b:
	strcpy(fMsgTitle,"Source Test");
	sprintf(fMsgMessage,"Please put the source in position 2 B\n\n");
	strcat(fMsgMessage,"     Click Ok when ready\n\n");
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     kMBIconExclamation,kMBOk,&retval);
	toCTL->Send(kGUI,kSource2bReady,0);
	break;
      case kSourceAck2c:
	strcpy(fMsgTitle,"Source Test");
	sprintf(fMsgMessage,"Please put the source in position 2 C\n\n");
	strcat(fMsgMessage,"     Click Ok when ready\n\n");
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     kMBIconExclamation,kMBOk,&retval);
	toCTL->Send(kGUI,kSource2cReady,0);
	break;
      case kSourceAck2d:	
	strcpy(fMsgTitle,"Source Test");
	sprintf(fMsgMessage,"Please put the source in position 2 D\n\n");
	strcat(fMsgMessage,"     Click Ok when ready\n\n");
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     kMBIconExclamation,kMBOk,&retval);
	toCTL->Send(kGUI,kSource2dReady,0);
	break;


      case kEndSourceTest:	
	strcpy(fMsgTitle,"Source Test");
	sprintf(fMsgMessage,"Please remember to put back the source \n");
        strcat(fMsgMessage,"     in position GARAGE\n\n");
	strcat(fMsgMessage,"     Click Ok when ready\n\n");
	new TGMsgBox(fClient->GetRoot(),this,fMsgTitle,fMsgMessage,
		     kMBIconExclamation,kMBOk,&retval);
	break;


      case kShowConnectedTdrs:
	printf("gui is about to update mask info=%d\n",control->TDRmask);
	color=fTDRButton[0]->GetDefaultFrameBackground();
	for (int tdr=0; tdr<24; tdr++) { 
	  printf("tdr %2d is %d\n",tdr, ((control->TDRmask & (1<<tdr))!=0));
	  //fTDRButton[tdr]->ChangeBackground(((control->TDRmask & (1<<tdr))!=0)?fYellow:fRed);
	  fTDRButton[tdr]->SetEnabled(((control->TDRmask & (1<<tdr))!=0)?kTRUE:kFALSE);
	  if (control->TDRref==tdr) fTDRButton[tdr]->ChangeBackground(fGreen);
	  else fTDRButton[tdr]->SetBackgroundColor(color);
	  
	  //cout << fTDRButton[tdr] << endl;
	  //fTDRButton[tdr]->ChangeBackground(fRed);
	  gClient->NeedRedraw(fTDRButton[tdr]);
	}
	
	break;

      case kUpdateToolTipsTdrs:
	for (int tdr=0; tdr<24; tdr++) fTDRButton[tdr]->SetToolTipText(control->LadderNameOctant[tdr]);
	break;
      }
    }
  }
}













