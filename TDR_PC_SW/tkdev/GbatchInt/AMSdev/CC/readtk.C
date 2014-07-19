//  $Id: readtk.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <iostream.h>
#include "amsdbc.h"
#include "commons.h"
AMSDATADIR_DEF amsdatadir_;
integer getnum(int,int,int);
int main(){
  int i,j,k;
  cout << " Please give lay lad sen ";  
  cin >> i >>j >>k;
  cout << " Number is " <<getnum(i-1,j-1,k-1)<<endl;
return 0;
}


integer getnum(integer layer, integer ladder, integer sensor){
  // calculate sensor #
  // layer, ladder , sensor from 0
       int num=0;
       int i,j,k;
       for ( i=0;i<layer;i++){
         for ( j=0;j<AMSDBc::nlad(i+1);j++){
           for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++)num++;
         }
       }

        i=layer;
        for ( j=0;j<ladder;j++){
          for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++)num++;
        }
        j=ladder;
        for ( k=0;k<sensor;k++)num++;
        return num;
}


