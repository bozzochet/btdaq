#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/times.h>
#include <limits.h>
#include <time.h>
#include <fstream>
#include <string>
#include <iostream>
#include <unistd.h>
#include "QuickUsb.h"

#define BUFFER_LENGTH 128

using namespace std;

int main(int argc, char **argv)
{



  struct tms tbuff;
  clock_t start, end;
  
  HANDLE hDevice;
  char dlist[BUFFER_LENGTH];
  unsigned char *buf;
  long int result;
  //memset((void*) buf, 0, BUFFER_LENGTH);
  //int val =1;
  //for(int i=0; i<BUFFER_LENGTH; i++) buf[i]= val & 0xff;

  if(QuickUsbFindModules(dlist,128)>0)
    {
      cout<<"Device Found"<<endl;
      cout<<dlist<<endl;
    }

  if(QuickUsbOpen(&hDevice,"QUSB-0")==0)
    {
      cout<<"Open Device Error"<<endl;
      return -1;
    }
  else cout<<"Open Device OK"<<endl;

  int val =65535;
  int addr= 511 | 0x8000;
  int length=102400000;
  cout<<"pippo"<<endl;
  //buf = new unsigned char[2*length];
  buf = (unsigned char *) malloc(2*length);
  cout<<"pippo"<<endl;
  for (int i = 0; i <length; i++)
    {
      buf[i*2] = val & 0xFF;
      // buf[ (i * 2) + 1] = ((val & 0xFF00) / 0x100);
      buf[ (i * 2) + 1] = ((val & 0xFF00) >> 8);
    }
  cout<<"pippo"<<endl;
  unsigned short int vvalue=0 | 0x8000;
  result = QuickUsbReadSetting(hDevice,SETTING_DATAADDRESS, &vvalue);
  if(result>0) cout<<vvalue<<endl;
  result = QuickUsbWriteSetting(hDevice,SETTING_DATAADDRESS, addr);
  result = QuickUsbReadSetting(hDevice,SETTING_DATAADDRESS, &vvalue);
  if(result>0) cout<<vvalue<<endl;
  if(result==0)
    {
      cout<<"Setting Error"<<endl;
      return -1;
    }

  start = times(&tbuff);
  //start = clock();
  //cout<<start<<endl;
  //result = QuickUsbWriteCommand(hDevice, addr, buf,(2*length));
  //int ii, jj;
  //for(ii=0;ii<20000000;ii++) jj = ii*7; /* waste some time */
  result = QuickUsbWriteData(hDevice, buf,(2*length));
  //end = clock();
  end = times(&tbuff);
  //cout<<end<<endl;
  if(result==0)
    {
      cout<<"Write Error"<<endl;
      return -1;
    }
  else  cout<<"Write Data OK"<<endl;

  if (QuickUsbClose(hDevice)==0)
    {
      cout<<"Close Device Error"<<endl;
      return -1;
    }
  else cout<<"Close Device OK"<<endl;
  
  // printf("r = %lf u = %lf s = %lf\n",
  //	 ((double)(end-start))/CLOCKS_PER_SEC,
  //	 ((double)tbuff.tms_utime)/CLOCKS_PER_SEC,
  //	 ((double)tbuff.tms_stime)/CLOCKS_PER_SEC);
  //end = times(&tbuff);
  double cps =  (double) sysconf(_SC_CLK_TCK);
  cout<<"clock="<<cps<<endl;
  double dt = ((double)(end-start))/cps;
  printf("dt = %lf \n",dt);
  printf("rate = %lf \n",(2*length*8)/(1024000*dt));
  
  return 0;
}
