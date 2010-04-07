#include <cstdlib>
#include <cstring>
#include <cstdio>


#include "SelfClass.hh"



SelfClass::SelfClass(char* address,int port):CommClass(address,port){

}

int SelfClass::CalibPG(int run,float timeout){

  char command[255];
  
  sprintf(command,"CALIBPG1 %d",run);
  return SendCommandWithReply(command,"READY","ERROR","CalibPG",timeout);
  
}
