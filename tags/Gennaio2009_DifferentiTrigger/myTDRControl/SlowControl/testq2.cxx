
#include <stdio.h>
#include <iostream>
#include "QList.h"
#include "amswcom.h"

using namespace std;


int main() {
  QCommand *qcmd=0, *first=0;
  QList *qlist=new QList(1<<15,0); // 0 is lecroy
  
  qlist->AddCommand(0x10,0x10, 0xe7a4,0);

  printf("The list contains %d command%s", qlist->GetCmdcnt(), (qlist->GetCmdcnt()>1)?"s\n":"\n");


  for (int i=0; i<qlist->GetCmdcnt(); i++) {
    if (i==0) qcmd=qlist->GetQfirst();
    for (int i=0; i<5; i++) printf("%04x ",qcmd->GetCommand(i));
    printf("\n");
    qcmd=qcmd->GetNext();
  }

  //  AMSWcom *Jinf=new AMSWcom();
  //  Jinf->SetHW(kAMSW_EPP);
  //  Jinf->SetPort(0);

  //  Jinf->WriteQList(0xffff,qlist);


  delete qlist;
}
