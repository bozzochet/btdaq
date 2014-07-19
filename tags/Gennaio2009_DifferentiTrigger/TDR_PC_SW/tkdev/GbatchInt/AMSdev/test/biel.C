//  $Id: biel.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <typedefs.h>
#include <iostream.h>
#include <amsstl.h>
#include <cern.h>
main(){
integer f[20];
integer g[20];
integer **ap=new integer*[20];
geant d;
for (int i=0;i<20;i++){
ap[i]=f+i;
f[i]=1000*RNDM(d);
}
  AMSsortNAG(ap,20);
  for ( i=0;i<20;i++){
   g[i]=*(ap[i]);
   cout <<i<<" "<<g[i]<<endl;
  }
integer tr;
for(;;){
cin >>tr;
integer iad=AMSbiel(g,tr,20);
cout<<iad<<" "<<tr<<endl;
}
return 0;
}
