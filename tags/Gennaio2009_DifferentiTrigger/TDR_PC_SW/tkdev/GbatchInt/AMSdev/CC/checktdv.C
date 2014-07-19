//  $Id: checktdv.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include "typedefs.h"
#include <ctype.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <stdio.h>
#include <iomanip.h>
#include <dirent.h>                   
#include "astring.h"


   static integer total=0;
   char *TDV_dir;
   char * TDV_name;
   int _select(dirent *entry);
   int _sort(dirent ** e1, dirent ** e2);
   void convert(int,int, char*);
   int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime);
int main (int argc, char *argv[]) {

        switch (argc) {
        default:
         break;
        }
         TDV_dir=getenv("TDVDataDir");
         if(!TDV_dir){
          cerr<<"Please setenv  TDVDataDir "<<endl;
          exit(1);
         }
         TDV_name=getenv("TDVName");
         if(!TDV_name){
          cerr<<"Please setenv  TDVName "<<endl;
          exit(1);
         }

         cout <<"checktdv-I-Scanning directory "<<TDV_dir<<endl;
         dirent ** namelist;                            
         int nptr=scandir(TDV_dir,&namelist,&_select,&_sort);    
         if(nptr>0){
          cout <<"checkTDV-I-Found "<<nptr <<" entries"<< endl;
           int i;
           for(i=0;i<nptr;i++){
             convert(i,nptr-i-1,namelist[i]->d_name);
           }
         }
         else {
          cerr<<" checktdv-E-NoEntriesFound "<<endl;
         }
        cout <<" total "<<total<<" sec uncorrected"<<endl;
        return 0;
}



         void convert (int begin, int end,char file[]){
           static char fname[2][256];
           static ifstream fbin[2];   // TDV
           static unsigned int * arr[2];
           static long id,size,flags;
           int k=begin%2;
           static time_t insert;
           if(begin==0){
              char path[512];
              strcpy(path,TDV_dir);
              strcat(path,"/");
              strcat(path,file); 
              int suc=UnixFilestat(path,&id,&size,&flags,&insert);
              size=size/4;
              strcpy(fname[0],TDV_dir);
              strcat(fname[0],"/");
              strcat(fname[0],file);
              arr[0]  = new unsigned int[size];
              arr[1]  = new unsigned int[size];
              fbin[0].open(fname[0],ios::in);
              fbin[0].read((char*)arr[0],size*4);
           }
           else {
              int kl=(begin-1)%2;
              strcpy(fname[k],TDV_dir);
              strcat(fname[k],"/");
              strcat(fname[k],file);
              fbin[k].open(fname[k],ios::in);
              fbin[k].read((char*)arr[k],size*4);
              fbin[k].close();
              time_t end=arr[kl][size-1];
              if(end<arr[k][size-2]){
                char ch;
                if(arr[k][size-2]-end>100){
                 cout<<"checktdv-S- Bigmismatch found "<<fname[kl] <<" "<<fname[k]<<" or "<< arr[k][size-2]-end <<" Will not be corrected "<<endl;
                 ch='n';
                }
                else if(arr[k][size-2]-end>200){
                 cout<<"checktdv-E- mismatch found "<<fname[kl] <<" "<<fname[k]<<" or "<< arr[k][size-2]-end <<" sec Correct? y/n ";
                 cin >> ch;
                }
                else ch='y';
                if(ch=='y' || ch=='Y'){
                  cout <<"checktdv-I-Correcting " <<arr[k][size-2]-end<<" sec started for " <<fname[kl]<<endl;
                  arr[kl][size-1]=arr[k][size-2];
                  ofstream fbout;
                  fbout.open(fname[kl],ios::out|ios::trunc);
                  if(!fbout)cerr <<" cannot open file "<<fbout<<endl;
                  else{
                   fbout.write((char*)arr[kl],size*4);
                   fbout.close();
                  }
                }
                else {
                 total+=arr[k][size-2]-end;
                }
              }
              else {
               // cout<<end<<" "<<arr[k][size-2]<<endl;
              }
           }
     }

       int _select(dirent *entry){
        for(int i=0;i<strlen(entry->d_name);i++){
         if(entry->d_name[i]=='.'){
          if(i<strlen(entry->d_name)-3){
           if(entry->d_name[i+1]=='1' && entry->d_name[i+2]=='.'){
            if(strstr(entry->d_name,TDV_name)){
             for(int k=i+3;k<strlen(entry->d_name);k++){
               if(!isdigit((entry->d_name)[k]))return 0;
             }
             return 1;
            }
           }
          }
         }
        }
        return 0;
      }


       

int _sort(dirent ** e1, dirent ** e2){

 return strcmp((*e1)->d_name,(*e2)->d_name);

}



int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime)
{
   // Get info about a file: id, size, flags, modification time.
   // Id      is (statbuf.st_dev << 24) + statbuf.st_ino
   // Size    is the file size
   // Flags   is file type: bit 0 set executable, bit 1 set directory,
   //                       bit 2 set regular file
   // Modtime is modification time
   // The function returns 0 in case of success and 1 if the file could
   // not be stat'ed.
   //
   struct stat statbuf;

   if (path != 0 && stat(path, &statbuf) >= 0) {
      if (id)
         *id = (statbuf.st_dev << 24) + statbuf.st_ino;
      if (size)
         *size = statbuf.st_size;
      if (modtime)
         *modtime = statbuf.st_mtime;
      if (flags) {
         *flags = 0;
         if (statbuf.st_mode & ((S_IEXEC)|(S_IEXEC>>3)|(S_IEXEC>>6)))
            *flags |= 1;
         if ((statbuf.st_mode & S_IFMT) == S_IFDIR)
            *flags |= 2;
         if ((statbuf.st_mode & S_IFMT) != S_IFREG &&
             (statbuf.st_mode & S_IFMT) != S_IFDIR)
            *flags |= 4;
      }
      return 0;
   }
   return 1;
}
