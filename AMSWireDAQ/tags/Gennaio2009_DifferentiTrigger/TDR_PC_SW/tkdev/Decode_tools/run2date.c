#include <time.h>
#include <stdio.h>

int main(int argc,char**argv){
if(argc==2){
  time_t pippo=atoi(argv[1]);
printf("%s",ctime(&pippo));
}
  
else if(argc==3){
  time_t pippo=atoi(argv[1])+315964800;
printf("%s",ctime(&pippo));
} 
  else
  printf("Convert unixtime into a date string\n");
return 0;

}
