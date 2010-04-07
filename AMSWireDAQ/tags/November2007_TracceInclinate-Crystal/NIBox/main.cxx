#include "SmallNetServer.hh"
#include <unistd.h>
#include <string.h>


int main(int argc,char**argv){
  

  NIDaqClass* pp=new NIDaqClass();
  SmallNetServer *serv= new SmallNetServer(pp);

  serv->Start();
  
  return 0;
}
