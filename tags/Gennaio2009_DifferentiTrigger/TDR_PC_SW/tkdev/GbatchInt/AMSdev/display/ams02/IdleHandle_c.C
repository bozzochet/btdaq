//  $Id: IdleHandle_c.C,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#include <iostream.h>
#include "AMSR_Types.h"

void IdleHandle(Int_t option)
{
  static TTree *t = 0;
  static AMSR_Ntuple *ntp = 0;
  static char *file_old = 0;
  static char file_holder[100];
  static char file_new[100];
  static ifstream iftxt;

  //
  // CUT comes here
  //
  static char *cut = "npart==1";

  static TTreeFormula *f_cut=0;
  static Int_t totEvt = 0;
  static char *data_dir;
  static Int_t first = -1;

  if (option < 0) {
     gAMSR_Display->IdleSwitch(1);   //Turn on automatic IdleHanding
  } else if (option == 0) {
jopa33:
    if (first == -1) {
       first = 0;
       data_dir = getenv("AMSEDDataDir");
       if (!data_dir) {
          cerr << "amsedc-F-AMSEDDataDir Not defined" << endl;
          gSystem->ExitLoop();
          return;
       }
       strcpy(file_holder,data_dir);
       strcat(file_holder,"/newroot");
       iftxt.open(file_holder,ios::in);
       if (!iftxt) {
          cerr <<"amsedc::init-F-ErrorOpenFile "<< file_holder << endl;
          gSystem->ExitLoop();
          return;
       }
       iftxt >> file_new;
       iftxt.close();
       char fullname[256];
       strcpy(fullname,data_dir);
       strcat(fullname,file_new);
       Int_t iret = gAMSR_Root->OpenDataFile(fullname,kUnknown);
       if (iret !=0 ) {
          cerr << "Failed to open file =" << fullname;
          gSystem->ExitLoop();
          return;
       }
       ntp=gAMSR_Root->GetNtuple();
       t        = ntp->GetTree();
       if ( t==0 || totEvt<0 ) {
          cout << "Empty file ?! No event in " << fullname << endl;
          gSystem->ExitLoop();
          return;
      }
       totEvt   = ntp->GetEntries();
       printf("data file %s has %d events\n", fullname, totEvt);
       totEvt = totEvt-1;
       if (f_cut != 0) f_cut->SetTree(t);
       else f_cut=new TTreeFormula("cut",cut,t);
//       printf("tree =%lx, formula =%lx\n", t, f_cut);
    }
    ntp->SetBranchStatus("*", 0);         //disable all branches
    ntp->SetBranchStatus("npart", 1);

    Int_t current = gAMSR_Root->Event();
    if (current < 0) current=-1;
    for (Int_t next=current+1; next<=totEvt; next++) {
      ntp->GetEvent(next);
      if (f_cut->EvalInstance(0) != 0 || rand()%10 == 3) {
        ntp->SetBranchStatus("*", 1);    //recover to enable all branches
        gAMSR_Root->Clear();
        gAMSR_Root->GetEvent(next);
        gAMSR_Display->Pad()->cd();
        gAMSR_Display->Draw();
        break;
      }
    }
    if ( next >= totEvt ) {
      ntp->SetBranchStatus("*", 1);       //recover to enable all branches
      printf("reach to the end of events, next %d !!\n",next);
      gAMSR_Display->IdleTime(30);
      gAMSR_Root->SetEvent(-1);
      printf("change IdleTimer %d seconds\n",gAMSR_Display->IdleTime());
      return;
    }
    printf("event changed to %d\n",next);

    char *file_old = gAMSR_Root->GetDataFileName();
    iftxt.open(file_holder,ios::in);
    iftxt >> file_new;
    iftxt.close();
       char fullname[256];
       strcpy(fullname,data_dir);
       strcat(fullname,file_new);
    if (strcmp(file_old,fullname)) {
       cout << "filename changed from " << file_old << " to " << fullname << endl;
       first=-1;
       goto jopa33;
       gSystem->ExitLoop();
       return;
    }

  } else if (option > 0) {
    if (f_cut) delete f_cut;
  }

  return;
}
