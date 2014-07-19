#include <iostream>
#include <fstream>

using namespace std;
int main(){

 char filename[190]="/amssw/AMSDataDir/v4.00/TKGeom_AMS02.0";
 ifstream iftx((const char*)filename,ios::in);
 if(iftx){
   char lay;
   int idum;
   iftx >>lay>>lay>>lay>>lay>>lay>>idum;
   
   return 0;
 }
 else return 4;
}
