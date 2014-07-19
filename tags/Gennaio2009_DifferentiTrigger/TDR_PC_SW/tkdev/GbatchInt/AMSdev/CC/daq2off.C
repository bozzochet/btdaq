//  $Id: daq2off.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <iostream.h>
#include <fstream.h>
#include "astring.h"
#include <stdlib.h>
#include "typedefs.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <iomanip.h>
void _convertl(int16u & w, int be);
void _convert(int16u * pdat, int length, int be);
void convert(int, int, char[], char[]);
int UnixFilestat(const char *path, long *id, long *size,
                              long *flags, time_t *modtime);
int main(int argc, char* argv[]){
  switch (argc) {
  case 1:
    cerr <<"cdaq-F-Please give an input file name as first parameter "<<endl;
    return 1;
  case 2:
    cerr <<"cdaq-F-Please give an output file name as last parameter "<<endl;
    return 1;
  default:
   break;
  }
  int i,k,ok;
   for(i=1;i<argc-1;i++){
     cout <<"cdaq-I-Open I file "<<argv[i]<<endl;
     convert(i-1,argc-i-2,argv[i],argv[argc-1]);
   }
   return 0;
}


void convert(int ibeg, int iend,char in[], char out[]){
uinteger _Length;
integer BigEndian=0;
static ofstream fbout;
static int16u Record[130000];
static int16u Input[130000];
int16u pData[2][24][1536];
int15u pDataH[100];    // Header
int16u pDataA[65535];   // scint etc....
static integer _Srun =-1;
  enum open_mode{binary=0x80};
  if(ibeg==0){




       integer b64=0;
       if(sizeof(ulong)>sizeof(uinteger))b64=1;
       uinteger test1,test2;
       test1=1;
       test1+=2<<8;
       test1+=4<<16;
       test1+=8<<24;
       unsigned char *pt= (unsigned char*)&test1;
       test2=pt[0];
       test2+=pt[1]<<8;
       test2+=pt[2]<<16;
       test2+=pt[3]<<24;
       integer lend = test1==test2;
       if(lend)cout <<"AMSTrIdSoftI-I-Identified as LittleEndian";
       else {
         cout <<"AMSTrIdSoftI-I-Identified as BigEndian";
         BigEndian=1;
       }
       if(b64)cout <<" 64 bit machine."<<endl;
       else cout <<" 32 bit machine."<<endl;
      






  }
    
    ifstream fbin;
    fbin.open(in,ios::in|binary);
    int fevt=1;
    static int gevt=0;
    static int terr=0;
    int tevt=0;
    long id, size, flags;
    uinteger run;
    time_t time;
    int suc=UnixFilestat(in, &id, &size,&flags, &time);
    int ie;
      if(fbin){
       cout <<"cdaq-I-Open I file "<<in<<endl;
      }
      else {
        cerr <<"cdaq=F-CouldNotOPenFileI "<<in<<endl;
        exit(1);
      }
       int ncnt=0;
       int eventok=0;
       
       do {
         // try to find something 
         uinteger _Length;
                             
       int16u tdrno=-1;
       int16u evto=-1;
       int l16ptr=0;
       int16u l16;
        fbin.read((unsigned char *)&l16,sizeof(l16));
        _convertl(l16,BigEndian);
        _Length=l16+1;
        fbin.read(unsigned char *)&l16,sizeof(l16));
        _Length=_Length | ((l16 & 63)<<16);
        fbin.seekg(fbin.tellg()-2*sizeof(l16));
        fbin.read((unsigned char *)Input,sizeof(Input[0])*_Length);
       int16u tlength,ntdr,len,tdrn,ch1,ch2,elem,ncrt,evt;
          // new event found
          evto=evt;
          if(ncnt == 24 && tdrn==23)eventok=1;
          else if (ncnt==0)eventok==0;
          else {
           eventok=0;
           if(!fbin.eof() && !fevt){
             terr++;
             cerr <<" Error event # "<<evt <<" "<<ncnt <<" "<<ntdr<<endl;
           }
          }
          ncnt=0;



       if(eventok){
         // read whole event; try to write it
         // if first event or run has been changed reopen the file
         if(_SRun !=run){
          if(fbout)fbout.close();
          fbout.open(out,ios::out|binary|ios::app);
          if(fbout){
           cout <<"cdaq-I-Open O file "<<out<<endl;
          }
          else {
           cerr <<"cdaq=F-CouldNotOPenFileO "<<out<<endl;
           exit(1);
          }
         }
         fevt=0;
         
         Record[1]=0x0;
         Record[2]=9;
         Record[3]=0x200;
         Record[4]=run&65535;
         Record[5]=(run>>16)&65535;
         Record[6]=0;
         Record[7]=evt&65535;
         Record[8]=(evt>>16)&65535;
         Record[9]=time&65535;
         Record[10]=(time>>16)&65535;
         Record[11]=0;
         _Length=Record[2]+1+1;
         int frp=12;
         // first crate
         for(int icrt=0;icrt<2;icrt++){
          Record[frp]=(32)*(640+1)+32*(384+1)+1;
          _Length+=Record[frp]+1;
          frp++;
          if(icrt==0)Record[frp]= 2<<6 | 11 <<9;    // 2 Crate 32; 5 for 72
          else Record[frp]= 5<<6 | 11 <<9; 
          frp++;
          for (int k=0;k<16;k++){
           for(int j=0;j<2;j++){
             // make address
             int16u conn, tdrs;
             if(k%2 ==0 ){
               if(j==0)conn=0;
               else conn=1; 
             }             
             else{
               if(j==0)conn=2;
               else conn=3; 
             }
             tdrs=k/2;
             //             if(icrt==0 && conn==2 && tdrs==7)continue;
             int16u addr=(conn<<10) | (tdrs <<12);
             Record[frp]=addr;
             frp++;
             int lj;
             if(j==0)lj=2;
             else lj=0;
             for(int l=0;l<320;l++){
              Record[frp]=pData[icrt][k][lj*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
             }
             if(j==0)lj=3;
             else lj=1;
             for( l=0;l<320;l++){
              Record[frp]=pData[icrt][k][lj*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
             }
           }
         }
         for(k=16;k<24;k++){
           for(int j=0;j<4;j++){
             // make address
             int16u conn, tdrk;
             if(k%2 ==0){
              if(j<2)conn=j+2;
              else conn=j-2;
             }
             else {
              if(j<2)conn=j+2;
              else conn=j-2;
               conn+= 4;
             }
             tdrk=(k-16)/2;
             //swap 3.2 <-> 3.0 for 32
             if(icrt==0 && tdrk == 2){
              if(conn == 0)conn=3;
              else if (conn==3)conn=0;  
             }
             int16u addr=(10<<6) |( conn<<10) | (tdrk <<13);
               
             Record[frp]=addr;
             frp++;
             for(int l=0;l<384;l++){
              Record[frp]=pData[icrt][k][j*384+l];
              Record[frp]=Record[frp] | (1<<15);
              frp++;     
              
             }
           }
         }
         }
         //make length
          Record[0]=_Length%65536;
          Record[1]=Record[1] | (_Length/65536);
         int tl=_Length+1;
         //         cout <<" tl "<<tl<<endl;
              //  Dump every 1000 event
         //                           if(evt%1000 == 0){
         //                             for(int itl=0;itl<tl;itl++){
         //                               cout <<itl <<" "<<Record[itl]<<endl;
         //                             }
         //                           }
         _convert(Record,tl,BigEndian);
         // skip first event for safety
         //cout <<" write " <<tevt << endl;
         if(tevt!=0)fbout.write((unsigned char*)Record,tl*sizeof(Record[0]));  
         tevt++;
       }



        }  
       if(fbin.good()&& !fbin.eof() ){
       }
       else{
         
         break;
       }
       fbin.read((unsigned char*)&tlength,sizeof(tlength));
       _convertl(tlength,BigEndian);
       //       cout << "tlength "<<tlength<<endl;
       tlength++;
       if(tlength != 3084 ){
         cerr<<"cdaqn-S-Bad Total Length "<<tlength<<endl;
         int nskp=0;
         while(tlength !=3084 || !fbin.eof()){
         fbin.read((unsigned char*)&tlength,sizeof(tlength));
         _convertl(tlength,BigEndian);
         tlength++;
         nskp++;
         }
         cerr<<"cdaqn-S-"<<nskp<<" words skipped"<<endl;
       }
       l16ptr++;
       fbin.read((unsigned char*)&ntdr,sizeof(ntdr));
       _convertl(ntdr,BigEndian);
       ncrt=0;
       if(ntdr & (1<<12))ncrt=1;
       ntdr=ntdr & ~(1<<12); 
       ncnt++;
       l16ptr++;
       //cout << " tlength "<<tlength <<endl;
       //cout << " ntdr "<<ntdr <<" "<<ncrt<<endl;
       if(fbin.good()&& !fbin.eof() ){
       }
       else{
         
         break;
       }
       do{
       tdrno=tdrn;       
       fbin.read((unsigned char*)&tdrn,sizeof(tdrn));
       fbin.read((unsigned char*)&len,sizeof(tdrn));
       fbin.read((unsigned char*)&evt,sizeof(tdrn));
       _convertl(tdrn,BigEndian);
       _convertl(len,BigEndian);
       _convertl(evt,BigEndian);
       //cout << " ntdr "<<ntdr <<" "<<tdrn<<" "<<len<<" "<<evt<<" "<<ncrt<<" "<<ncnt<<endl;
       if(len-4 != 1536){
         cerr <<" Cdaq-S-wrong length event # "<<evt<<" "<<len-4<<endl;
         // Try to skip to another event
       }
                assert(len-4 <= 1536);
          fbin.read((unsigned char*)pData[ncrt][tdrn],sizeof(pData[0][0][0])*(len-4));
          _convert(pData[ncrt][tdrn],len-4,BigEndian);
          fbin.read((unsigned char*)&ch1,sizeof(ch1));
          fbin.read((unsigned char*)&ch2,sizeof(ch2));
          l16ptr+=len+1;         
       }while (l16ptr<tlength);
      }while (fbin.good() && !fbin.eof());
      
  fbin.close();
  gevt+=tevt-1;
  if(iend==0){
      fbout.close();
      cout <<" Total of "<<gevt-terr<<" events  converted with "<<terr<<" errors"<<endl;
  }
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





void _convertl(int16u & l16, int BigEndian){
  if(BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(&l16);
     tmp=*pc;
     *pc=*(pc+1);
     *(pc+1)=tmp;
  }
}


void _convert(int16u * _pData,int _Length, int BigEndian){





  if(BigEndian){
   unsigned char tmp;
   unsigned char *pc = (unsigned char*)(_pData);
   int i;
   for(i=0;i<_Length;i++){
     tmp=*pc;
     *pc=*(pc+1);
     *(pc+1)=tmp;
     pc=pc+sizeof(int16u);
   }
  }
}
