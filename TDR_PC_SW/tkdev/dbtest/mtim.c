#include <cstdio>
#include "time.h"
#include <cstdlib>

int main(int argc,char** argv){


if(argc!=2){
  printf("%s Errore \n",argv[0]);
  return -2;\
}
time_t tt=atoi(argv[1]);
printf("Time   %s is %s \n",argv[1],asctime(localtime(&tt)));


return 0;
}
