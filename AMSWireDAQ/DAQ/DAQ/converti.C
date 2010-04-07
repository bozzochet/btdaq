
#include <string.h>
#include <stdio.h>


int main(int argc,char**argv){
 
   char name[13];
   memset(name, 0, sizeof(name));
 int left_adjust=0;
   int i = strlen(argv[1]);
   if ( left_adjust || i >= 12 ) 
	   strncpy(name, argv[1], 12);
   else {
	   memset(name, 0x20, 12);
	   strncpy(&name[12 - i], argv[1], i);
   }
   for(int ii=0;ii<12;ii++){
	printf("%02hhx",name[ii]);
	if(ii%2) printf(" "); 
}
	 printf("\n"); 
return 0;
}
