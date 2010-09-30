#include "MainFrame.h"
#include "Messages.h"
#include <iostream>

////////////////////////////////////////////////////////////////////////////
// CreateMenuBar
// Create Menu Bar
////////////////////////////////////////////////////////////////////////////
void MainFrame::MenuBar(){

  // Create menubar and popup menus.

  // layout hint items
  fItemOpt = kLHintsLeft | kLHintsTop;
  fItemLay = new TGLayoutHints(fItemOpt, 0, 4, 0, 0);
  fHelpOpt = kLHintsTop | kLHintsRight;
  fHelpLay = new TGLayoutHints(fHelpOpt,0,0,0,0);

  // ...

  // file popup menu
  fFile = new TGPopupMenu(gClient->GetRoot());  
  fFile->AddEntry("&New Ladder", kM_FILE_NEW_LADDER);
  fFile->AddSeparator();
  fStation = new TGPopupMenu(gClient->GetRoot());
  fStation->AddEntry("Geneva",kM_STATION_GENEVA);
  fStation->AddEntry("Perugia",kM_STATION_PERUGIA);
  fStation->AddEntry("GyA",kM_STATION_GYA);
  fStation->AddEntry("ETH/Prevessin",kM_STATION_PREVESSIN);
  fStation->AddEntry("Geneva-Eduardo",kM_STATION_EDUARDO);
  fStation->AddEntry("Perugia-Giovanni",kM_STATION_GIOVANNI);
  fFile->AddPopup("Set Test Station",fStation);
  fFile->AddEntry("&Directories", kM_FILE_DIRECTORIES);
  fFile->AddEntry("&Show Run.Summary", kM_FILE_SHOWRUNSUMMARY);
  fFile->AddSeparator();
  fFile->AddEntry("Set Shaping Time",kM_FILE_SETSHAPINGTIME);
  fFile->AddEntry("&Reset Hardware",kM_FILE_RESETHW);
  fFile->AddSeparator();
  fFile->AddEntry("Preferences",kM_FILE_PREFERENCES);
  fFile->AddEntry("&Print",kM_FILE_PRINT);
  fFile->AddEntry("&Exit", kM_FILE_EXIT);
  fFile->Associate(this);

  // Calibration popup menu
  fCal = new TGPopupMenu(gClient->GetRoot());
  fCal->AddEntry("Set Parameters",kM_CAL_SET_PARAMETERS);

  fCal->AddSeparator();
  fCal->AddEntry("Set Default Calibration",kM_CAL_SET_DEFAULT);
  fCal->AddEntry("&Dynamic", kM_CAL_DYNAMIC);
  fCal->AddSeparator();
  fCal->AddEntry("Full Calibration",kM_CAL_PASS_ALL);
  fCal->AddEntry("Pass &0 (Skip)",kM_CAL_PASS_0);
  fCal->AddEntry("Pass &1 (Pedestal)",kM_CAL_PASS_1);
  fCal->AddEntry("Pass &2 (Sigma Raw)",kM_CAL_PASS_2);
  fCal->AddEntry("Pass &3 (Sigma)",kM_CAL_PASS_3);
  fCal->AddEntry("Pass &4 (Gaussian)",kM_CAL_PASS_4);
  fCal->AddSeparator();
  fCal->AddEntry("Summary",kM_CAL_SUMMARY);
  fCal->Associate(this);

  //Gain Calibration popup menu
  fGain = new TGPopupMenu(gClient->GetRoot());
  fGain->AddEntry("Set Parameters",kM_GAIN_SET_PARAMETERS);
  fGain->AddSeparator();
  fGain->AddEntry("Start Gain cal",kM_GAIN_START);
  fGain->Associate(this);

  // Qualification popup menu
  fQualification = new TGPopupMenu(gClient->GetRoot());
  fQualification->AddEntry("Test A - Hybrid Calibration",     kM_QUAL_TEST_A);
  fQualification->AddEntry("Test B - Ladder Assembly",        kM_QUAL_TEST_B);
  fQualification->AddEntry("Test C - Reception at Perugia",   kM_QUAL_TEST_C);
  fQualification->AddEntry("Test D - Reception at Geneva",    kM_QUAL_TEST_D);
  fQualification->AddEntry("Test E - Hybrid Box Installation",kM_QUAL_TEST_E);
  fQualification->AddEntry("Test F - Leg Gluing",             kM_QUAL_TEST_F);
  fQualification->AddEntry("Test G - Spacer Gluing",          kM_QUAL_TEST_G);
  fQualification->AddEntry("Test H - Shield Wrapping",        kM_QUAL_TEST_H);
  fQualification->AddEntry("Test I - Preinstallation",        kM_QUAL_TEST_I);
  fQualification->AddEntry("Test M - Metrology",              kM_QUAL_TEST_M);
  fQualification->AddEntry("Test P - Plane Installation",     kM_QUAL_TEST_P);
  fQualification->AddSeparator();
  fQualification->AddEntry("Source Test V - Reception at GVA",kM_QUAL_TEST_V); 
  fQualification->AddEntry("Source Test W - Spacer Gluing",   kM_QUAL_TEST_W); 
  fQualification->Associate(this);

  // AMSW menu
  fAMSWireMenu = new TGPopupMenu(gClient->GetRoot());
  fAMSWireMenu->AddEntry("View Status", kM_AMSWIRE_STATUS);
  fAMSWireMenu->Associate(this);


  // help popup menu
  fHelp = new TGPopupMenu(gClient->GetRoot());
  fHelp->AddEntry("&AMS", kM_HELP_AMS);
  fHelp->AddEntry("&System Test", kM_HELP_SYSTEM);
  fHelp->AddSeparator();
  fHelp->AddEntry("&About", kM_HELP_ABOUT);
  fHelp->Associate(this);

  fMenuBarOpt = kLHintsLeft | kLHintsTop | kLHintsExpandX;
  fMenuBar = new TGMenuBar(this, 1, 1, fMenuBarOpt);
  fMenuBar->AddPopup("&File", fFile, fItemLay);
  fMenuBar->AddPopup("&Calibration", fCal, fItemLay);
  fMenuBar->AddPopup("&Gain", fGain, fItemLay);
  fMenuBar->AddPopup("&Qualification", fQualification, fItemLay);
  fMenuBar->AddPopup("&AMSwire", fAMSWireMenu, fItemLay);
  fMenuBar->AddPopup("&Help", fHelp, fHelpLay);
}
