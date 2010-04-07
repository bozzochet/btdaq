//  $Id: cern.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "typedefs.h"
#include "cern.h"
using namespace amsprotected;
void amsprotected::mm3(  number m1[3][3],  number m2[3][3], int choise){
  const int s=3;
  static number tmp[s][s];
  for (int i=0;i<s;i++){
    for (int j=0;j<s;j++){
    tmp[i][j]=0; 
     for (int l=0;l<s;l++){
      if(choise==0)tmp[i][j]=tmp[i][j]+m1[i][l]*m2[l][j];
      else tmp[i][j]=tmp[i][j]+m2[i][l]*m1[l][j];
    }
  }
} 
    UCOPY(tmp,m2,s*s*sizeof(tmp[0][0])/4);
}

void amsprotected::transpose( number m1[3][3], number m2[3][3]){
 m2[0][0]=m1[0][0];
 m2[0][1]=m1[1][0];
 m2[0][2]=m1[2][0];
 m2[1][0]=m1[0][1];
 m2[1][1]=m1[1][1];
 m2[1][2]=m1[2][1];
 m2[2][0]=m1[0][2];
 m2[2][1]=m1[1][2];
 m2[2][2]=m1[2][2];
}
