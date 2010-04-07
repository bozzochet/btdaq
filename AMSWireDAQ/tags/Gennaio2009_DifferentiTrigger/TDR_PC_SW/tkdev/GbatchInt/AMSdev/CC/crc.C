#include <iostream.h>
#include <stdlib.h>
#include <fstream.h>
#include <dirent.h>
#include "astring.h"
#include <sys/stat.h>
#include <stdio.h>
unsigned int * _Table=0;
const unsigned int CRC32=0x04c11db7;
void _inittable();
extern "C" int _select(const dirent *entry);
extern "C" int _selectsdir(const dirent *entry);
extern "C" int _sort(const dirent ** e1, const dirent ** e2);
extern "C" int _sorta(const dirent ** e1, const dirent ** e2);

// return codes 
//       1         everything ok
//       0         wrong crc or crc was not compared
//       128+1     too few parameters
//       128+2     file is not a plain file
//       128+3     unable to open file 

  int main(int argc, char * argv[]){
    unsigned int crc_to_compare=0;
    if(argc<2){
      cerr<< "  Please provide Directory Or File Name"<<endl;
      return 128+1;
    }
    else if(argc>2){
      //   crc mode
      sscanf(argv[2],"%u",&crc_to_compare);
    }
      _inittable();
      AString fdir(argv[1]);
      AString fout(fdir);
          struct stat statdir_map;
          stat((const char*)fdir,&statdir_map);
         ifstream fbin;
          if((statdir_map.st_mode & 16384) ){
            if( crc_to_compare!=0){
                return 128+2;
            }
      fout+=".crc";
      ofstream fbout;
      fbout.open((const char*)fout);
      dirent ** namelist;
      dirent ** namelistsubdir;
      int nptrdir=scandir((const char *)fdir,&namelistsubdir,&_selectsdir,&_sort);
       for(int is=nptrdir-1;is<nptrdir;is++){
        AString fsdir(fdir);
//        fsdir+="/";     
//        fsdir+=namelistsubdir[is]->d_name;
       int nptr=scandir((const char *)fsdir,&namelist,_select,_sorta);
        for(int ii=0;ii<nptr;ii++){
         AString fnam(fsdir);
         fnam+="/";
         fnam+=namelist[ii]->d_name;    
         fbin.open((const char*)fnam);
         unsigned int data;
         unsigned int crc;
          struct stat statbuf_map;
          stat((const char*)fnam,&statbuf_map);
          int fsize=statbuf_map.st_size;
          unsigned int chunk[32000]; 
         int i=0;
         for(;;){
           if(!fsize) break;
           int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
           fbin.read((char*)chunk,myread);
           fsize-=myread;
           if(fbin.good() && !fbin.eof()){
           int beg;
           if(i==0){
            crc=~chunk[0];
            beg=1;
           }
           else{
            beg=0;
           }
           for(int m=beg;m<myread/sizeof(chunk[0]);m++){
            for(int j=0;j<3;j++)crc=_Table[crc>>24]^(crc<<8);
            crc=crc^chunk[m];  
           }
           i++;
          }
          else break;
         }
         fbin.close();
         crc=~crc;
         cout <<fnam<<" "<<crc<<endl;
         fbout <<fnam<<" "<<crc<<endl;
         }
        }     
       fbout.close();
  }
  else{
         fbin.open((const char*)fdir);
         if(!fbin){
            return 128+3;
         }
         unsigned int data;
         unsigned int crc;
          int fsize=statdir_map.st_size;
          unsigned int chunk[32000]; 
         int i=0;
         for(;;){
           if(!fsize) break;
           int myread=fsize>sizeof(chunk)?sizeof(chunk):fsize;
           fbin.read((char*)chunk,myread);
           fsize-=myread;
           if(fbin.good() && !fbin.eof()){
           int beg;
           if(i==0){
            crc=~chunk[0];
            beg=1;
           }
           else{
            beg=0;
           }
           for(int m=beg;m<myread/sizeof(chunk[0]);m++){
            for(int j=0;j<3;j++)crc=_Table[crc>>24]^(crc<<8);
            crc=crc^chunk[m];  
           }
           i++;
          }
          else break;
         }
         fbin.close();
         crc=~crc;
         if(crc_to_compare==crc)return 1;  
         cout <<fdir<<" "<<crc<<" "<<crc_to_compare<<endl;
  }
   return 0;
  }

int _select(const dirent *entry){
 return strstr(entry->d_name,".hbk")!=NULL;    
}

void _inittable(){
  if(!_Table){
    _Table=new unsigned int[256];
    int i,j;
    unsigned int crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
      //cout << i<<" "<<_Table[i]<<endl;
    }  
  }
}

int _selectsdir(const dirent *entry){
return (entry->d_name)[0] != '.';   
}

int _sort(const dirent ** e1, const dirent ** e2){

 return strcmp((*e1)->d_name,(*e2)->d_name);

}
int _sorta(const dirent ** e1, const dirent ** e2){
char tmp[255];
char buf[255];
      unsigned int run1(0),run2(0),ev1(0),ev2(0);
strcpy(tmp,(*e1)->d_name);
for (int i=strlen(tmp)-1;i>=0;i--){
   if(tmp[i]=='/'){
    for (int j=i+1;j<strlen(tmp);j++){
     if(tmp[j]='.'){
      tmp[j]='\0';
      sscanf(tmp+i+1,"%d",&run1);
      for (int k=j+1;k<strlen(tmp);k++){
      if(tmp[k]='.'){
       tmp[k]='\0';
       sscanf(tmp+j+1,"%d",&ev1);
      }
      }
      break;
     }
    }
    break; 
   }
  }  

strcpy(tmp,(*e2)->d_name);
for (int i=strlen(tmp)-1;i>=0;i--){
   if(tmp[i]=='/'){
    for (int j=i+1;j<strlen(tmp);j++){
     if(tmp[j]='.'){
      tmp[j]='\0';
      sscanf(tmp+i+1,"%d",&run2);
      for (int k=j+1;k<strlen(tmp);k++){
      if(tmp[k]='.'){
       tmp[k]='\0';
       sscanf(tmp+j+1,"%d",&ev2);
      }
      }
      break;
     }
    }
    break; 
   }
  }  

 if(run1<run2){
  return -1;
 }
 else if(run1==run2){
  if(ev1<ev2){
   return -1;
  }
  else if (ev1==ev2)return 0;
  else return 1;
 }
 else return 1;
}
