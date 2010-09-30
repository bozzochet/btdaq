#include "MainFrame.h"
#include "Messages.h"


const char *ftcal[]       = { "Cal files"   , "*.cal",
			      "All files"   , "*",
			     0,0};
const char *ftfilebin[]   = {"Input binary", "*.binary",
			     "All files"   , "*",
			     0,0};
const char *ftfileascii[] = {"Input ascii", "*.ascii",
			     "All files"   , "*",
			     0,0};
const char *ftfileX5[]    = {"Test Beam X5", "*.bin",
			     "Raw Data" , "*.raw",
			     "All files"   , "*",
			     0,0};
const char *ftfileCDP[]    = {"TDR CDP", "*.dat",
			     "Raw Data" , "*.raw",
			     "All files"   , "*",
			     0,0};

void MainFrame::FileDialog(int aType, EFileDialogMode aOp){
  TGFileInfo fi;
  char oldPath[200];
  char aMessage[200];
  
  sprintf(oldPath,"%s",gSystem->WorkingDirectory());
  switch (aType) {
  case kSOURCE_BINARYFILE:
    fi.fFileTypes = ftfilebin;
    //fi.fFileTypes = (char **)ftfilebin;
    break;
  case kSOURCE_ASCIIFILE:
    fi.fFileTypes = ftfileascii;
    //fi.fFileTypes = (char **)ftfileascii;
    break;

  case kSOURCE_BEAMTESTX5:
    fi.fFileTypes = ftfileX5;
    //fi.fFileTypes = (char **)ftfileX5;
    break;
  case kSOURCE_TDRCDP:
    fi.fFileTypes = ftfileCDP;
    //fi.fFileTypes = (char **)ftfileCDP;
    break;
  }
  gSystem->ChangeDirectory(control->fDirLadder);
  new TGFileDialog(gClient->GetRoot(),this,aOp,&fi);
  sprintf(control->fFileName,"%s",fi.fFilename);
  sprintf(aMessage,"Selected input file: %s",fi.fFilename);
  toLOG->Send(kGUI,aMessage,1);
  gSystem->ChangeDirectory(oldPath);      
}			    



