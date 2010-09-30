#include "slowgui2.h"



int main(int argv, char** argc) {

  if (argv!=4) {
    printf("usage: %s crate portA portB\n",argc[0]);
    return 1;
  }

  TApplication *theApp;
  //theApp=new TApplication("App", &argv, argc);
  int a=1;

  //char *paramc[1];
  //sprintf(paramc[0],"%s", argc[0]);

  int tcrate=atoi(argc[1]);
  if (tcrate<0) tcrate=0;
  else if (tcrate>7) tcrate=7;

  int portA=atoi(argc[2]);
  if (portA<0) portA=0;
  else if (portA>3) portA=3;

  int portB=atoi(argc[3]);
  if (portB<0) portB=0;
  else if (portB>3) portB=3;

  theApp=new TApplication("App", &a, argc);

  //tcrate: 0, 1, 2, 3, 4, 5, 6, 7
    //octant: 1, 2, 4, 3, 5, 6, 8, 7
  unsigned int jinfaddr[8]={0x033F, 0x093F, 0x003F, 0x013F, 0x103F, 0x113F, 0x163F, 0x173F };

  new SlowGui(gClient->GetRoot(),400,400, tcrate, jinfaddr[tcrate],jinfaddr[tcrate] ,portA, portB);
  theApp->Run();
  return 0;
}
