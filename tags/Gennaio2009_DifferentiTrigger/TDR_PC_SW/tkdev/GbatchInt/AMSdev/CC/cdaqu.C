//  $Id: cdaqu.C,v 1.1.1.1 2007/11/13 09:56:10 zuccon Exp $
#include <iostream.h>
#include <fstream.h>
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
       //     convert(0,0,"/offdata/raw/run23001.dat","/dev/null");
   return 0;
}


void convert(int ibeg, int iend,char in[], char out[]){
integer BigEndian=0;
static ofstream fbout;
static int16u Record[100000];
int16u pData[24][4000];
int16u pntdr[12];
int16u pnblk[12];
int16u pheader[12];
  enum open_mode{binary=0x80};
  if(ibeg==0){
    fbout.open(out,ios::out|binary|ios::app);
      if(fbout){
       cout <<"cdaq-I-Open O file "<<out<<endl;
      }
      else {
        cerr <<"cdaq=F-CouldNotOPenFileO "<<out<<endl;
        exit(1);
      }




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
    
    FILE * fbin=fopen(in,"r");
    static int gevt=0;
    int tevt=0;
    int evt;
    char name[80];
    strncpy(name,in+strlen(in)-7,3);
    int run =atoi(name);    
    cout <<"run "<<run<<endl;
    if(in[strlen(in)-3]=='d'){
      strncpy(name,in+strlen(in)-8,1);
    }
    else if(in[strlen(in)-3]=='c'){
      strncpy(name,in+strlen(in)-9,1);
    }
    else {
      cerr <<"couldnt get crate number, stop"<<in[0]<<endl;
      exit(1);
    }
    int crtn= name[0]=='3'?2:5;    
    cout <<" crate" <<crtn<<endl;
    long id, size, flags;
    time_t time;
    int suc=UnixFilestat(in, &id, &size,&flags, &time);
    cout <<ctime(&time);
    int ie;
      if(fbin){
       cout <<"cdaq-I-Open I file "<<in<<endl;
      }
      else {
        cerr <<"cdaq-F-CouldNotOPenFileI "<<in<<endl;
        exit(1);
      }
       integer cdr[24];
       integer nrecords=0; 
       {
       for(int  ll=0;ll<24;ll++)cdr[ll]=-1;
       }
         nrecords=0;
         int nblk=0;
       do {
        int l16ptr=0;
        int tlength,ntdr,len,tdrn,ch1,ch2,elem;
        ie=fscanf(fbin,"%x",&tlength);
        l16ptr++;
        ie=fscanf(fbin,"%x",&ntdr);
        l16ptr++;
        int datatype=(ntdr>>11)&31;
        // cout <<"nblk "<<nblk<<endl; 
        pheader[nblk]=ntdr;
        pnblk[nblk]=tlength;
        ntdr=ntdr & 31;
        pntdr[nblk++]=ntdr;
        //cout << " tlength "<<tlength <<endl;
        //cout << " ntdr "<<ntdr <<" "<<pntdr[nblk-1]<<" "<<nblk-1<<endl;
        //cout <<" datatype "<<datatype<<endl;
        if(ie!=EOF && ntdr>0){
        }
       else{
         
         break;
       }
       do{
        ie=fscanf(fbin,"%x",&tdrn);
        ie=fscanf(fbin,"%x",&len);
        ie=fscanf(fbin,"%x",&evt);
        cdr[nrecords++]=tdrn;
        //cout << " ntdr "<<ntdr <<" "<<tdrn<<" "<<len<<" "<<evt<<endl;
         pData[tdrn][0]=len;
         for(int i=1;i<len-1;i++){
          ie=fscanf(fbin,"%x",&elem);
          pData[tdrn][i]=elem;
         }                
          l16ptr+=len+1;         
       }while (l16ptr<tlength-2);
        ie=fscanf(fbin,"%x",&ch1);
        ie=fscanf(fbin,"%x",&ch2);

       if(tdrn == 23){
         if(nrecords != 24 ){
           cerr <<" cdaq-E-EventError-"<<nrecords<<" "<<datatype<<endl;
         }
           for(int ll=0;ll<nrecords;ll++){
             if(cdr[ll]!=ll)cout <<"cdaq-OrderError"<<ll<<" "<<cdr[ll]<<endl;
           }
         int frp;
         for( ll=0;ll<24;ll++)cdr[ll]=-1;
         nrecords=0;
         // read whole event; try to write it
         Record[1]=0x0;
         Record[2]=11;
         Record[3]=0x200;
         Record[5]=run&65535;
         Record[4]=(run>>16)&65535;
         Record[6]=0;
         Record[7]=0;
         Record[9]=evt&65535;
         Record[8]=(evt>>16)&65535;
         Record[11]=time&65535;
         Record[10]=(time>>16)&65535;
         Record[12]=0;
         Record[13]=0;
         // Here DataType
         // was :
         // Raw 0 Status 1 Sigma 2 Ped 3 Cmn 4 Compressed 14
         // Will
         // Compressed 0 Raw 1 Status 2 Ped 3 sigma 4 cmn 5
         switch (datatype){
          case 0:
           datatype=1;
           break;
          case 1:
           datatype=2;
           break;
          case 2:
           datatype=4;
           break;
          case 3:
           datatype=3;
           break;
          case 4:
           datatype=5;
           break;
          case 14:
           datatype=0;
           break;
         case 12:
           datatype=12;
           break;
         default:
           cerr <<"unknown datatype "<<datatype<<endl;
         }
         if(datatype!=12)Record[15]= datatype | crtn<<6 | 11 <<9;    // Crate 32; 5 for 72
         else Record[15]= 1 | crtn<<6 | 11 <<9;    //rawfirst
         //
         //Find the event length
         //
        Record[14]=1;
        int mixed=-1;
        int cntdr=0;
        if (datatype==0){
          cntdr=0;
          // Compressed mode - get rid of junk
          for(int k=0;k<nblk;k++){
            for(int j=0;j<pntdr[k];j++){
              Record[14]+=pData[cntdr++][0]-2;
            }
          }
         frp=16;
         int cntdr=0;
         for(ll=0;ll<nblk;ll++){
          for(int l1=0;l1<pntdr[ll];l1++){
           int len=0;
           int tlen=0;
           for(int k=1;k<pData[cntdr][0]-2;k+=len){
            len=(pData[cntdr][k]&255 )+3;
            tlen+=len;
            for(int lc=0;lc<len;lc++){
              Record[frp]=pData[cntdr][k+lc];
              if(lc==len-1)Record[frp]= Record[frp] & (~(1<<15));
              //              if(evt==20)cout <<k<<" "<<len<<" "<<Record[frp]<<endl;   
              frp++;
            }
          
           }
           if(tlen%2){
             //if(evt==20)cout <<"subt"<<endl;
             Record[14]--;
           }
           cntdr++;
    
          }
         } 
        }
       


        

        else if(datatype !=12){
          cntdr=0;
          //cout <<" nblk "<<nblk<<endl;
          for(int k=0;k<nblk;k++){
            Record[14]+=2;
            for(int j=0;j<pntdr[k];j++){
              Record[14]+=1+pData[cntdr][0];
              //cout <<cntdr<<" "<<Record[14]<<" "<<pData[cntdr][0]<<endl;
              cntdr++;
            }
          Record[14]+=2;  //crc
         }
            cout << " Output record "<<Record[14]<<endl;
         frp=16;
         int cntdr=0;
         for(ll=0;ll<nblk;ll++){
          Record[frp]=pnblk[ll];
          Record[frp+1]=pheader[ll];
          frp+=2;
          for(int l1=0;l1<pntdr[ll];l1++){
           Record[frp]=cntdr;
           Record[frp+1]=pData[cntdr][0];
           Record[frp+2]=0;
           frp=frp+3;
           for(int k=1;k<pData[cntdr][0]-1;k++){
            Record[frp]=pData[cntdr][k];
            frp++;
           }
           cntdr++;
          }   
           Record[frp]=0;
           frp++;
           Record[frp]=0;
           frp++;
         }


        }
        else {
          //raw first
          for(int k=0;k<nblk;k++){
            Record[14]+=2;
            for(int j=0;j<pntdr[k];j++){
              Record[14]+=1+pData[cntdr++][1];
            }
           Record[14]+=2;  //crc
          }
          //cout << " Output record mixed"<<Record[14]<<endl;
         frp=16;
         int cntdr=0;
         int icntdr=0;         
         for(ll=0;ll<nblk;ll++){
           // find the correct raw length
           int rawl=2+2;
            for(int j=0;j<pntdr[ll];j++){
              rawl+=1+pData[icntdr++][1];
            }
          Record[frp]=rawl;
          Record[frp+1]=pheader[ll];
          frp+=2;
          for(int l1=0;l1<pntdr[ll];l1++){
           Record[frp]=cntdr;
           Record[frp+1]=pData[cntdr][1];
           Record[frp+2]=0;
           frp=frp+3;
           for(int k=3;k<pData[cntdr][1]+1;k++){
            Record[frp]=pData[cntdr][k];
            frp++;
           }
           cntdr++;
          }   
           Record[frp]=0;
           frp++;
           Record[frp]=0;
           frp++;
         }
         
         mixed=frp;
         Record[mixed]=1;
         Record[mixed+1]= 0 | crtn<<6 | 11 <<9;    //compressed
         //         cout <<" d "<< mixed-14 <<" "<< Record[14]<<endl;
         
         cntdr=0;
          // Compressed mode - get rid of junk
          for(k=0;k<nblk;k++){
            for(int j=0;j<pntdr[k];j++){
              Record[mixed]+=pData[cntdr][pData[cntdr][1]+1]-2;
              cntdr++;
            }
          }
         frp=mixed+2;
         cntdr=0;
         for(ll=0;ll<nblk;ll++){
          for(int l1=0;l1<pntdr[ll];l1++){
           int len=0;
           int tlen=0;
           for(int k=pData[cntdr][1]+3;k<pData[cntdr][1]+3+pData[cntdr][1+pData[cntdr][1]]-3;k+=len){
            len=(pData[cntdr][k]&255 )+3;
            tlen+=len;
            for(int lc=0;lc<len;lc++){
              Record[frp]=pData[cntdr][k+lc];
              if(lc==len-1)Record[frp]= Record[frp] & (~(1<<15));
              //              if(evt==20)cout <<k<<" "<<len<<" "<<Record[frp]<<endl;   
              frp++;
            }
          
           }
           if(tlen%2){
             //if(evt==20)cout <<"subt"<<endl;
             Record[mixed]--;
           }
           cntdr++;
          }   
        }
       


       }
        nblk=0;       
        Record[0]=Record[14]+1+Record[2]+1+1;
        if(mixed > 0)Record[0]+=Record[mixed]+1;
        int tl=Record[0]+1;
        if(evt<50){
           cout <<" Out "<<tl << " "<<evt<<" "<<datatype<<" "<<mixed<<endl;
           if(mixed > 0)cout <<" mixed "<<Record[mixed]<<endl;
        }
              //  Dump every 1000 event
         //                  if(evt%1000 == 0){
         //                    for(int itl=0;itl<tl;itl++){
         //                      cout <<itl <<" "<<Record[itl]<<endl;
         //                    }
         //                  }
         _convert(Record,tl,BigEndian);
         fbout.write((unsigned char*)Record,tl*sizeof(Record[0]));  
         tevt++;
       }
      }while (ie!=EOF);
      
  fclose(fbin);
  gevt+=tevt-1;
  if(iend==0){
      fbout.close();
      cout <<" Total of "<<gevt<<" events  converted"<<endl;
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
