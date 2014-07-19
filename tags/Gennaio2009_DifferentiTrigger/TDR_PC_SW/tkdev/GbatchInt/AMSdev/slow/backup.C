//  $Id: backup.C,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
/* Directory backup from Dir1 Dir2 */
#include <time.h>
#include <iostream.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <string.h>
int DELETE,DELCOPY,COPY,MKDIR,TOUCHED;
int AMSbi(char *a[], char * obj, int i);
int _sort(const dirent ** d1, const dirent ** d2);
int _select(const dirent *d1);
void check(char *buf, int del);
int backup(char * dir1 , char * dir2, int del);
int main (int argc, char *argv[]) {
int delay = 360*2; /* dlay in minutes */
int erase = 7*24*60;

char *dir1, *dir2;
  if(argc > 2){
    delay = atoi(argv[1]);
    erase = atoi(argv[2]);
  }
  else if (argc > 1) {
    delay = atoi(argv[1]);
    cout <<"backup-W-DeleteTimeIsNotSet. Will delete  files after "<<erase<<" minutes"<<endl;
  }
  else {
    cout <<"backup-W-DeleteTimeIsNotSet. Will delete  files after "<<erase<<" minutes"<<endl;
    cout <<"backup-W-SleepTimeIsNotSet. Will use  sleep time "<<delay<<" minutes"<<endl;
  }
  delay *= 60;
  erase *= 60;
  
  while (1) {/* main loop */

    
    DELETE=0;
    DELCOPY=0;
    MKDIR=0;
    COPY=0;
    TOUCHED=0;
    dir1=getenv("DirFrom");
    if (dir1==NULL) {
      puts("backup-F-setenv DirFrom and try again.Stop");
      exit(1);
    }
    dir2=getenv("DirTo");
    if (dir2==NULL) {
      puts("backup-F-setenv DirTo and try again.Stop");
      exit(1);
    }
    int succ=backup(dir1,dir2,erase);

       time_t Time;
       time(&Time);
    if(!succ){
       cerr<<"backup-E-backupErrorAt  "<<ctime(&Time)<<endl; 
    }
    else {
     cout<<"backup-I-SuccAt  "<<ctime(&Time)<<endl; 
     cout <<"COPIED: "<<COPY<<endl;
     cout <<"MKDIRED: "<<MKDIR<<endl;
     cout <<"DELCOPIED: "<<DELCOPY<<endl;
     cout <<"DELETED: "<<DELETE<<endl;
     cout <<"TOUCHED: "<<TOUCHED<<endl;

    }
    sleep(delay);

  }


}
int _sort(const dirent ** d1, const dirent **d2){
  return strcmp( (*d1)->d_name, (*d2)->d_name);
}

int _select(const dirent *d1){
 if(!strcmp(d1->d_name,"."))return 0;
 if(!strcmp(d1->d_name,".."))return 0;
 return 1;
}
int backup(char * dir1, char * dir2, int erase){
    cout <<"Backup-I-Started for "<<dir1<<" "<<dir2<<endl;
    struct stat statbuf1,statbuf2;
    int i;
   char buf[1024];
   char buf1[256];
   char buf2[256];
   dirent ** namelist1;
   dirent ** namelist2;
   char ** ar1=0;
   char ** ar2=0;
   int l1 = scandir(dir1, &namelist1,&_select,&_sort);
    if (l1<0) {
      cout <<"Backup-F-CanNotScanDir "<<dir1<<endl;
      exit(1);
    }
    if(l1>0)ar1= new char*[l1];
    for(i=0;i< l1;i++)ar1[i]=namelist1[i]->d_name;
    int l2 = scandir(dir2, &namelist2,&_select,&_sort);
    if (l2<0) {
      strcpy(buf,"mkdir ");
      strcat(buf,dir2);
      system(buf);
      MKDIR++;
      l2 = scandir(dir2, &namelist2,NULL,&_sort);
      if (l2<0){
        cout <<"Backup-F-CanNotCreateDir "<<dir2<<endl; 
        exit(1);
      }
    }
    if(l2>0)ar2= new char*[l2];
    for(i=0;i< l2;i++)ar2[i]=namelist2[i]->d_name;
    // first scand dir2 and remove unnecessary files
    for(i=0;i<l2;i++){
        if(AMSbi(ar1,namelist2[i]->d_name,l1)!=-1){
           // delete if necessary
            strcpy(buf,dir2);
            strcat(buf,"/");
            strcat(buf,namelist2[i]->d_name);
            check(buf,erase);
        }
    }        
    // scan now dir1
     for (i=0; i<l1; i++) {
        if(AMSbi(ar2,namelist1[i]->d_name,l2)==-1){
            //entry found lets look more closely
           strcpy(buf1,dir1);
           strcat(buf1,"/");
           strcat(buf1,namelist1[i]->d_name);
           strcpy(buf2,dir2);
           strcat(buf2,"/");
           strcat(buf2,namelist1[i]->d_name);
            int ok1=stat(buf1,&statbuf1);
            int ok2=stat(buf2,&statbuf2);
            if(ok1 || ok2){
              if(ok1)cerr<<"backup-E-StatRejected for "<<buf1<<endl;
              if(ok2)cerr<<"backup-E-StatRejected for "<<buf2<<endl;
            }
            else{
               // check first dir status
               int d1=(statbuf1.st_mode & S_IFMT)==S_IFDIR?1:0;
               int d2=(statbuf2.st_mode & S_IFMT)==S_IFDIR?1:0;
               if(statbuf1.st_mtime>statbuf2.st_mtime || d1){
                  //do smth
                   if(d1){
                     if(d2)backup(buf1,buf2,erase);
                     else{
                       strcpy(buf,"rm -rf ");
                       strcat(buf,buf2);
                       system(buf);
                       DELCOPY++;
                       strcpy(buf,"cp -R ");
                       strcat(buf,buf1);
                       strcat(buf, " ");
                       strcat(buf,buf2);  
                       system(buf);
                       COPY++;
                     }
                   }
                   else{
                    if(d2){
                       strcpy(buf,"rm -rf ");
                       strcat(buf,buf2);
                       system(buf);
                       DELCOPY++;
                    }
                    strcpy(buf,"cp -R ");
                    strcat(buf,buf1);
                    strcat(buf, " ");
                    strcat(buf,buf2);  
                    system(buf);
                    COPY++;
                    
                   }
               }
               else {
                   // touch copies
                   strcpy(buf,"touch ");
                   strcat(buf,buf2);
                   system(buf);
                   TOUCHED++;
               } 
            }
        }
        else {
           // just copy
           strcpy(buf,"cp -R ");
           strcat(buf,dir1);
           strcat(buf,"/");
           strcat(buf,namelist1[i]->d_name);
           strcat(buf," ");
           strcat(buf,dir2);
           strcat(buf,"/");
           strcat(buf,namelist1[i]->d_name);
           system(buf);
           COPY++;
        }   

      }
    delete[] ar1;
    delete[] ar2;
    for(i=0;i<l1;i++){
      free(namelist1[i]);
    }
    for(i=0;i<l2;i++){
      free(namelist2[i]);
    }
    free(namelist1);
    free(namelist2);
    return 1;
}



 int AMSbi(char* a[], char * obj, int i){
//
// binary search
// input T *a[]   // adresses pf array to search
// T& obj object
// i array size
// output :
//  -1              obj found
//  any other       min {*a[]} with obj<*a[output]
//
int ia=0;
int il=1;
int ir=1;
int ib=i-1;
int j=0;
if(i<1)return 0;
while(ia<ib-1 || il*ir){
  int k=(ia+ib)/2;
  int i=strcmp(obj,a[k]);
  if(!i)return -1;
  else if(i>0){
   ia=k;
   il=0;
  }  
  else{
    ib=k;
    ir=0;
  }
}
if(ir){
 j=strcmp(obj,a[ib]);
 if(!j)return -1;
 else if(j>0)return ib+1;
}
else if(il){
 j=strcmp(obj,a[ia]);
 if(!j)return -1;
 else if(j<0)return ia;
}
return ib;
}


void check(char *fname, int erase){
  //  cout <<"Check-I-Started for "<<fname<<" "<<endl;
  char buf[1024];
  struct stat statbuf;
  if(erase<=0){
    strcpy(buf,"rm -rf ");
    strcat(buf,fname);
    system(buf);
    DELETE++;
    return;
  }
  int ok=stat(fname,&statbuf);
  if(ok){
     cerr<<"check-E-StatRejected for "<<fname<<endl;
     return;
  }
  time_t Time;
  time(&Time);
  if(statbuf.st_mtime+erase<Time){
     // closer look
     int d1=(statbuf.st_mode & S_IFMT)==S_IFDIR?1:0;
     if(d1){
       // it's directory
       // check if it the empty one
       dirent** namelist;
       int nptr=scandir(fname,&namelist,&_select,&_sort);
       if(nptr==0){
         strcpy(buf,"rm -rf ");
         strcat(buf,fname);
         system(buf);
         DELETE++;
       }
       else {
         for(int i=0;i<nptr;i++){
          strcpy(buf,fname);
          strcat(buf,"/");
          strcat(buf,namelist[i]->d_name);
          check(buf,erase);
         }
       }
       for(int i=0;i<nptr;i++){
        free(namelist[i]);
       }
       free (namelist);
     }
     else {
         strcpy(buf,"rm -rf ");
         strcat(buf,fname);
         system(buf);
         DELETE++;
     }
  }
}
