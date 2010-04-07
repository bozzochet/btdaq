#include "TrigClass.hh"
#include <cstdio>
#include <string.h>

int main(int argc,char** argv){

 TrigClass* tt= new TrigClass("localhost",1700);
 if(argc<2) return -3;
 if(strstr(argv[1],"start")) tt->TriggerOn();
 else if(strstr(argv[1],"stop")) tt->TriggerOff();
 else if(strstr(argv[1],"calib")) tt->BeamTrig();
 else if(strstr(argv[1],"beam")) tt->CalibTrig();
 else printf("count %d\n",tt->ReadCounter());
return 0;
}


