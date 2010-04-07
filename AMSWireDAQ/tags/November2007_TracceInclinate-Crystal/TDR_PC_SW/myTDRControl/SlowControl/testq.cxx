
#include <stdio.h>
#include <iostream>
#include "QList.h"

using namespace std;


int main() {
  QCommand *qcmd=0, *first=0;

  for (unsigned short i=0; i<5; i++) {
    if (i==0) { 
      qcmd=new QCommand(0,1,2,3,i);
      cout << qcmd << endl;
      first=qcmd;
    }
    else {
      QCommand *dum=new QCommand(i,i,0,i,0);
      qcmd->SetNext(dum);
      qcmd=dum;
    }
    printf("i=%d\n",i);
  }


  qcmd=first;
  int cnt=0;
  do {
    printf("%d: %d %d %d %d %d\n",cnt, qcmd->GetCommand(0),qcmd->GetCommand(1),qcmd->GetCommand(2),qcmd->GetCommand(3),qcmd->GetCommand(4));
    qcmd=qcmd->GetNext();

    cnt++;
  } while (qcmd);



}
