#include "NIDaqClass.hh"

#include <unistd.h>
#include <cstdio>

int main(){
  uInt32  wr;
  NIDaqClass* ni= new NIDaqClass();
  ni->Init();
  for (int ii=0;ii<10000000;ii++){
    if(ii%2) wr=0x0;
    else wr=0x21;
    printf("------------------------------------------------------- \n ");
    printf("Leggo %5d \n",ii);
    ni->Read();
    ni->Print();
    
    printf("Scrivo  %d  \n",wr);
    ni->Write(wr,wr+1);
    if(ii%200) ni->ResetCounter();
    printf("RiLeggo \n ");
    ni->Read();
    ni->Print();
    
    usleep(100000);
  }

  delete ni;
  return 0;
}
