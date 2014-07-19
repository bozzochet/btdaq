//  $Id: iotest.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <iostream>
#include <fstream.h>
#include <stdlib.h>
#include <streambuf.h>
#include <vector>
void main(){
float a=10;
double arr[10]={a,a,a,a,a,a,a,a,a,a};
char fnam[128]="datafile";
  ifstream iftxt(fnam,ios::in);
   if(!iftxt){
     cerr <<"Error open"<<endl;
     exit(1);
   }
   for(int i=0;i<10;i++){
    iftxt >> arr[i];
    if(iftxt.eof() && i<9){
      cerr<< "Unexpected eof"<<endl;
      exit(1);
    }
      }
 for (int i=0;i<10;i++)cout<<i<<" "<<arr[i]<<endl;
iftxt.close();
 
   }
