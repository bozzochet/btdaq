//  $Id: IdleHandle.C,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
void IdleHandle(Int_t option)
{
  static TTree *t = 0;
  static AMSR_Ntuple *ntp = 0;
  static char *filename = 0;

  //
  // CUT comes here
  //
//  static char *cut   
//  static char *cut = "Chi2strline[0]<1 && Chi2circle[0]<1 && Chi2fastfit[0]<10 && beta[0]<0.";
//    static char *cut = "npart==1 && pcharge[0]==2 && pmom[0]>0 && ptrackp[0]==1 && gchi2[0]>0";
//  static char *cut = "npart==1";


  static TTreeFormula *f_cut=0;
  static Int_t totEvt = 0;

  printf("IdleHandle option %d\n",option);

  if (option <= 0) {
    if (option < 0) {
       ntp=gAMSR_Root->GetNtuple();
       gAMSR_Display->IdleSwitch(1);   //Turn on automatic IdleHanding
    }

    char *newfile = gAMSR_Root->GetDataFileName();
    if ( newfile==0 || strlen(newfile)==0 ) {       //null data file
       printf("No data file opened ?!\n");
       gAMSR_Display->IdleSwitch(-1);   //Stop IdleHandle
       return;
    }

    if ( option<0 || filename==0 || strcmp(filename, newfile) ) {

      if (filename != 0) delete filename;
      if (strlen(newfile) > 0) filename = new char[strlen(newfile)+1];
      strcpy(filename,newfile);

      t        = ntp->GetTree();
      totEvt   = ntp->GetEntries();
      printf("data file %s has %d events\n", filename, totEvt);
      totEvt = totEvt-1;

      if ( totEvt<0 ) {
         printf("Empty file ?!\n");
         gAMSR_Display->IdleSwitch(-1);   //Stop IdleHandle
         return;
      }

//      if (f_cut != 0) f_cut->SetTree(t);
//      else f_cut=new TTreeFormula("cut",cut,t);
//      printf("tree =%lx, formula =%lx\n", t, f_cut);

    }
  }

  if (option == 0) {

    ntp->SetBranchStatus("*", 0);         //disable all branches
//    ntp->SetBranchStatus("Chi2strline", 1);
//    ntp->SetBranchStatus("Chi2circle", 1);
//    ntp->SetBranchStatus("Chi2fastfit",1);
//    ntp->SetBranchStatus("beta",1);      //end of enabling branches
    ntp->SetBranchStatus("npart", 1);
    ntp->SetBranchStatus("ptrackp", 1);
    ntp->SetBranchStatus("pmom", 1);
    ntp->SetBranchStatus("pcharge", 1);
    ntp->SetBranchStatus("ntrtr", 1);
    ntp->SetBranchStatus("gchi2", 1);

//    ntp->SetBranchStatus("pmass", 1);

    Int_t current = gAMSR_Root->Event();
    if (current < 0) current=-1;
    for (Int_t next=current+1; next<=totEvt; next++) {
      ntp->GetEvent(next);
      if (f_cut->EvalInstance(0) != 0) {
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
      gAMSR_Display->IdleSwitch(-1);   //Stop IdleHandle
//      gSystem->ExitLoop();  
      return;
    }
    printf("event changed to %d\n",next);

  } else if (option > 0) {

    if (f_cut) delete f_cut;

  }

  return;
}
