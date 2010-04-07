//  $Id: updtdv.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
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
#include "cern.h"
#include <stdio.h>
#include <iomanip.h>
void _InitTable();
uinteger * _Table=0;
uinteger _CalcCRC(uinteger *arr, uinteger size);
void convert(int, int, char[], char[]);
const uinteger CRC32=0x04c11db7;

int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime);
int main(int argc, char* argv[]){
  switch (argc) {
  case 1:
    cerr <<"cdaq-F-Please give a tdv name as first parameter "<<endl;
    return 1;
  case 2:
    cerr <<"cdaq-F-Please give a correction as eg. begin+34000"<<endl;
    return 1;  
  default:
   break;
  }
  
  time_t begin,end,insert;
  int dbeg(0),dend(0),dins(0);
  
  int i,k,ok;
  for( i=2;i<argc;i++){
    if(strstr(argv[i],"begin")){
     sscanf(argv[i]+5,"%d",&dbeg);
    }
    else if( strstr(argv[i],"end")){
     sscanf(argv[i]+3,"%d",&dend);
    }
    else if( strstr(argv[i],"insert")){
     sscanf(argv[i]+6,"%d",&dins);
    }
  }
  cout <<dbeg<<" "<<dend<<" "<<dins<<endl;
     cout <<"cdaq-I-Open I file "<<argv[1]<<endl;
      ifstream fbin;
      long id,size,flags;
      int suc=UnixFilestat(argv[1],&id,&size,&flags,&insert);
      cout << "size "<<size<<endl;
      size=size/4;
     fbin.open(argv[1],ios::in);
     unsigned int * arr  = new unsigned int[size];
     fbin.read((char*)arr,size*4);
     end=arr[size-1];
     begin=arr[size-2];
     insert=arr[size-3];
     arr[size-4]=_CalcCRC(arr,size-4);
     
     cout <<"Old ***"<<endl;
     cout <<"b "<<ctime(&begin)<<endl;
     cout <<"e "<<ctime(&end)<<endl;
     cout <<"i "<<ctime(&insert)<<endl;
     begin+=dbeg;
     end+=dend;
     insert+=dins;
     arr[size-1]=end;
     arr[size-2]=begin;
     arr[size-3]=insert;
     cout <<"New ***"<<endl;
     cout <<"b "<< ctime(&begin)<<endl;
     cout <<"e "<<ctime(&end)<<endl;
     cout <<"i "<<ctime(&insert)<<endl;
     fbin.close();
      ofstream fbout;
     fbout.open(argv[1],ios::out|ios::trunc);
     if(!fbout)cerr <<" cannot open file"<<endl;
     fbout.write((char*)arr,size*4);
     fbout.close();
   return 0;
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



void _InitTable(){
  if(!_Table){
    _Table=new uinteger[256];
    assert(_Table!=NULL);
    integer i,j;
    uinteger crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
      //cout << i<<" "<<_Table[i]<<endl;
    }  
  }
}

uinteger _CalcCRC(uinteger _pData[],uinteger n){
 _InitTable();
  int i,j,k;
  uinteger crc;
  if( n < 1) return 0;
  uinteger *pu=(uinteger *)_pData; 
    crc=~pu[0];
    for(i=1;i<n;i++){
      for(j=0;j<3;j++)crc=_Table[crc>>24]^(crc<<8);
     crc=crc^pu[i];  
    }
  return ~crc;
}