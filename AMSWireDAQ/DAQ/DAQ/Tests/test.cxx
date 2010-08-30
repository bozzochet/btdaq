#include "amswcomtest.h"
#include "QList.h"
#include <cstring> // needed to add this for gcc 4.3
#include <stdio.h>
#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include <math.h>
#include "color.h"
#include <sys/time.h>

#define    SUCCESS        0
#define    ERROR          1

//secondo me non servono qui ma devo stare in amswcom...
// #define MAX_STRN 0x100

// #define kPEDS 1
// #define kSIGL 2
// #define kSIGH 4
// #define kSIGR 8
// #define kSTAT 16
// #define kALL  0x1f

using namespace std;

void Reorganize(unsigned short *array);

//CalibMemory Prog_3123, Prog_3ed3, Prog_3f83, Prog_3710_07;//dovrebbe stare in amswcom e non servire più qui...

int main(int argv, char **argc) {

  unsigned short param[50];

  if (argv<4) {
    printf("usage: %s port amsw_node cmd arg1 arg2 ... \n",argc[0]);
    printf("numbers must be written in hex\n");
    printf("call it with TESTepp for epp communication\n");
    printf("call it with TESTpci for pci communication\n");
    printf("call it with TESTjmdc for jmdc (enveloped) communication\n");
    return 1;
  }

  node->InitCalParMem();

  memset(param,0,sizeof(param));

  for (int i=0; i<argv; i++) printf("%s ",argc[i]);
  printf("\n");

  unsigned int addr=0;
  unsigned short cmd=0, port=0;
  int  mode=0;


  mode=(strstr(argc[0],"epp"))?kAMSW_EPP:kAMSW_PCI;
  mode=(strstr(argc[0],"jmdc"))?kAMSW_JMDC:mode;

  printf("mode = %d\n",mode);

  sscanf(argc[1],"%d",&port);
  sscanf(argc[2],"%x",&addr); // in case you wonder, 'h' is for 'short'...
  sscanf(argc[3],"%hx",&cmd);

  int start=4, nparam=0;
  //if (cmd==cAMSW_COMMAND) start++;
  for (int i=start; i<argv; i++) sscanf(argc[i],"%hx", &param[nparam++]);

  printf("addr=%08x   cmd=%04x\n",addr,cmd);
  for (int i=0; i<nparam; i++) printf("0x%04x ",param[i]);
  printf("\n"); 

  //return 0;

  AMSWcom *Jinf=new AMSWcom();
  Jinf->SetHW(mode);
  Jinf->SetPort(port);
  Jinf->SetTimeOut(100000); // 100 secs of timeout (for dac cal)
  Jinf->SetInteractive();


  if (Jinf->Init()==ERROR) {
    printf("init error\n");
    return 1;  
  } 

  //  Jinf->SetCalParMem(&Prog_3f83);//Philipp removed this so it's usefull?! Secondo me sta dentro amswcom quando calibra...

  Jinf->ProcessHexCommand(addr,cmd,nparam,param);

  //  PrintSummary(Jinf,0x003f);
  // PrintNodeStatus(Jinf,0xffff);
  //PrintNodeStatus(Jinf,0x003f);

  Jinf->Shut();

  return 0;
}

// void ReadQList(AMSWcom *node, unsigned int addr) {

//   char typech[4][20]={"LeCroy\0", "AMSWire\0", "Memory op\0", "IO op\0"};

//   unsigned int freq=0, time=0;
//   int executed=0, attribute=0, first=0, last=0;
//   unsigned short w0=0, w1=0, r0=0, r1=0, offset=0, type=0;


//   node->ReadQList(addr);
//   int cnt=0;
//   unsigned length=node->GetEventSize();
//   while (cnt<length-1) {

//     type=node->Event[cnt] & 3;
//     last=node->Event[cnt] & (1<<2);
//     first=node->Event[cnt] & (1<<3);
//     attribute=(node->Event[cnt] & 0x70);
//     executed=node->Event[cnt] & (1<<7);
//     freq=(node->Event[cnt] & 0xff00)<<8;
//     offset=node->Event[++cnt];
//     freq+=node->Event[++cnt];

//     w0=node->Event[++cnt];
//     w1=node->Event[++cnt];
//     time=node->Event[++cnt];
//     time+=(node->Event[++cnt]<<16);
//     r0=node->Event[++cnt];
//     r1=node->Event[++cnt];

//     char timech[20];
//     time/=100;
//     int hours=time/3600;
//     int minutes=(time-(hours*3600))/60;
//     int seconds=(time-hours*3600-minutes*60);
//     sprintf(timech,"%02d:%02d:%02d",hours,minutes,seconds);
//     if (first) printf("first ");
//     if (last) printf("last ");
//     printf("command - offset 0x%04x - exec: %s\n",offset,(executed)?"yes":"no");
//     printf("%s  freq=%6d  attr=0x%2x type=%s\n",timech, freq, attribute,typech[type]);
//     printf("W0=%04x W1=%04x R0=%04x R1=%04x\n",w0,w1,r0,r1);
//     cnt++;

//     if (r0==0) {
//       printf("r0=%d... probably the actel is not powered\n",r0);
//     } else if ((w0^r0)!=0x8000) {
//       printf("LeCroy error: w0 xor r0 = 0x%x\n",w0^r0);
//     } else {

//       unsigned short address=((r0>>7) & 0x1f);
//       unsigned short bus=attribute;
//       int half=0, actel=0;
      
//       int test9011=(bus & 0x40);
//       if (test9011) {
// 	if (address!=3) {
// 	  printf("strange, address=%d, while bus seems to say it is an S9011\n", address);
	  
// 	}
// 	half=1 & (bus>>5);
// 	actel=1 & (bus>>4);
//       } else {
// 	if (address>10) if (!(bus & 0x10)) {
// 	  printf("strange, address=%d>10, while bus seems to say that is should be <11\n", address);
// 	}
// 	actel=1 & (bus>>5);
//       }
      
//       char boardname[10];
//       if (address==3) sprintf(boardname,"S9011AT half %d", half);
//       else if ((address==5) || (address==15)) sprintf(boardname,"TBS_%02d", address);
//       else sprintf(boardname,"TPSFE_%02d", address);

//       int r;
//       r=r0 & 7;
//       r+=((r0 & 0x30)>>1);      


//       printf("%s: Actel %s, register %d\n", boardname, (actel==kActelHot)?"Hot":"Cold", r);
//     }

//     printf("\n\n");
//   }

// }
