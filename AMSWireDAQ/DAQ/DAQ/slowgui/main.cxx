#include "slowgui.h"



int main(int argv, char** argc) {

  if (argv!=3) { 
    printf("usage: %s crate port\n",argc[0]);
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

  int port=atoi(argc[2]);
  if (port<0) port=0;
  else if (port>3) port=3;

  theApp=new TApplication("App", &a, argc);

  //tcrate: 0, 1, 2, 3, 4, 5, 6, 7
    //octant: 1, 2, 4, 3, 5, 6, 8, 7
  unsigned int jinfaddr[8]={0x033F, 0x093F, 0x003F, 0x013F, 0x103F, 0x113F, 0x163F, 0x173F };

  //printf("%d %x\n", conversion_table[tcrate], jinfaddr[tcrate]);
  new SlowGui(gClient->GetRoot(),400,400, tcrate, jinfaddr[tcrate],port);
  theApp->Run();
  return 0;
}
