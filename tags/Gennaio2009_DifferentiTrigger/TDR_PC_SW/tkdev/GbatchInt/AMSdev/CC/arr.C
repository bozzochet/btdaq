//  $Id: arr.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
// Author V. Choutko 24-may-1996
 
#include "arr.h"
#include "id.h"
const integer ncharge=7;
integer AMSArr<int>::debug=1;
integer AMSArr<double>::debug=1;
integer AMSArr<AMSID>::debug=1;
// Simple array example;
main(){
integer i,j,k,l;
AMSArr<double> ddou(3,4,5,6);
AMSArr<AMSID>  damsid(20,2);
AMSArr<int>   dint(ncharge);  
for( i=0;i<103;i++)dint(i)=i*i;
for( i=0;i<3*4*5*6+2;i++)ddou.pp()=i;

for( i=0;i<3*4*5*6+2;i++)damsid.pp()=AMSID(i,"my_id");
cout << " ****dint****"<<endl;
cout <<dint;
cout << " ****ddou****"<<endl;
cout <<ddou;
cout <<"******AMSID*****"<<endl;
cout <<damsid;
for(;;){
cin >> i>>j>>k>>l;
cout << ddou(i,j,k,l) <<endl;

}
return 0;
}
