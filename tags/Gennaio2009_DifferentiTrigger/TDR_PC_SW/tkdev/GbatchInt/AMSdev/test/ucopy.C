//  $Id: ucopy.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include <typedefs.h>
#include <cern.h>
#include <point.h>
main(){
AMSPoint pig(-11.,13.5,66.6);
AMSPoint apig;
number jopa;
number after[3];
cin >>jopa;
integer * p=new integer[100];
UCOPY(&pig,p+2,3*sizeof(number)/4);
UCOPY (p+2,&apig,3*sizeof(number)/4);
cout <<pig<<" " <<apig<<endl;
UCOPY (p+2,after,3*sizeof(number)/4);

cout <<after[0]<<endl;
cout <<after[1]<<endl;
cout <<after[2]<<endl;
return 0;
}
