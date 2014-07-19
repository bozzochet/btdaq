#include <stdio.h>

typedef struct calib{
  float ped[1024];
  float rsig[1024];
  float sig[1024];
  int status[1024];

} calib;

int main(){
  char filename[100]="tt.cal";
  FILE* fil= fopen(filename,"r");
    
  int a,b,c,ii;
  float d,e;
  calib cc;
  calib* cal=&cc;
  for ( ii=0;ii<16;ii++){
    fscanf(fil,"%d, %f, %f",&a,&d,&e);
    printf("%d, %f, %f\n",a,d,e);
  }
  for ( ii=0;ii<1024;ii++){
    fscanf(fil,"%d  %d  %d  %f %f %f %f %d  ",&a,&b,&c,
	   &(cal->ped[ii]),
	   &(cal->rsig[ii]),
	   &(cal->sig[ii]),
	   &d,
	   &(cal->status[ii]));
    printf("%d  %f %f %f %d\n",ii,cal->ped[ii],cal->rsig[ii],cal->sig[ii],cal->status[ii]);
  }

  fclose(fil);
  return 1;
}
