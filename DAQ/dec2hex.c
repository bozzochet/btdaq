#include <stdio.h>
#include <stdlib.h>
int main(int argc,char** argv){
if(argc!=2){
printf("Usage: %s <num> \n It returns the corresponding num in hexadecimal\n",argv[0]);
return -1;

}
printf(" %X \n",atoi(argv[1]));
return 0;
}
