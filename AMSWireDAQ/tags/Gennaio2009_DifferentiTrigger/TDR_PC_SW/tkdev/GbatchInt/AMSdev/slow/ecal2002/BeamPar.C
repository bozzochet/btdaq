#include <typedefs.h>
#include <cfortran.h>
#include <hbook.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <strings.h>
#include <iostream.h>
#include <fstream.h>
const int NWPAW=700000;
struct PAWC_DEF{
float q[NWPAW];
};
#define PAWC COMMON_BLOCK(PAWC,pawc)
#define N_bad 500   // maximum number of bad CAS blocks

COMMON_BLOCK_DEF(PAWC_DEF,PAWC);
PAWC_DEF PAWC;
uint _CalcCRC(uint *pData, int length);
class BeamPar {
 public:
  uint RunTag;   //runTag
  time_t Time;   //UNIX sec, of the manipulator PC
  float X;       //cm, the beam cross in AMS coo system
  float Y;       //cm
  float Z;       //cm
  float Theta;   //rad [0-Pi], direction of the beam (Pi- from z-direction)
  float Phi;     //rad [0-2Pi]
  uint  Pid;     //Beam particle ID  == Geant Pid
  float Mom;     //Beam particle momentum (GeV/c) - 0-default
  float Intens;  //Beam intensity (part/sec), 0-default
  uint  Position;    // position number
  float Size;    //beam size in cm;
 };
   static BeamPar record[60];
      
   main(){
        BeamPar tmp;
        int iostat,rsize=1024;
        HLIMIT(NWPAW);
        HROPEN(3,"output","beam","N",rsize,iostat);
           if(iostat){
             cout << "Error opening Histo file beam"<<endl;
             exit(1);
           }
           else cout <<"Histo file "<<"beam"<<" opened."<<endl;

              HBOOK1(101,"td",400,-38.5,361.5,0.);
    char input[]="beampar.list";
    ifstream fin;
    fin.open(input);
    uinteger run;
    int nline=0;
    while(1){
     fin>>run;
     fin.ignore(INT_MAX,'\n');
     if(fin.eof())break;
     nline++;
    }     
    fin.close();
    BeamPar * mypar = new BeamPar[nline];  
            ofstream fbin;
            fbin.open("a");
            int qq=0;
            fbin.write((char*)&qq,sizeof(qq));
            fbin.close();
    fin.open(input);
    nline=0;
    while(!fin.eof()){
     fin >>run>>tmp.RunTag>>tmp.Time>>tmp.X>>tmp.Y>>tmp.Z>>tmp.Theta>>tmp.Phi>>tmp.Pid>>tmp.Mom>>tmp.Intens>>tmp.Position>>tmp.Size;
     tmp.Position=run;
     cout <<" "<<run<<" "<<tmp.RunTag<<" "<<tmp.Time<<" "<<tmp.X<<" "<<tmp.Y<<" "<<tmp.Z<<" "<<tmp.Theta<<" "<<tmp.Phi<<" "<<tmp.Pid<<" "<<tmp.Mom<<" "<<tmp.Intens<<" "<<tmp.Position<<" "<<tmp.Size<<endl;
     
      mypar[nline++]=tmp;
    }
    fin.close();

      int i;
      int good=0;
       BeamPar blockNt;
              HBNT(1,"Beam Data"," ");
              HBNAME(1,"EcalBeam",(int*)(&blockNt),"RunTag:I,Time:I,X,Y,Z,Theta,Phi,Pid:I,Mom:R,intens:R,Run:I,size:R");
       for(i=0;i<nline;i++){
        blockNt=mypar[i];
        HFNT(1);
       }
                int ntuple_entries;
                HNOENT(1, ntuple_entries);
                cout << " Closing CAS ntuple  with "<< ntuple_entries
                << " events" << endl;
                char hpawc[256]="//PAWC";
                HCDIR (hpawc, " ");
                char houtput[9]="//output";
                HCDIR (houtput, " ");
                integer ICYCL=0;
                HROUT (0, ICYCL, " ");
                HREND ("output");
//                CLOSEF(1);

        int curr=-1;
        time_t begin,end,insert;
      for(i=0;i<nline;i++){
//   write BeamPar
         BeamPar block=mypar[i];
        curr++;
         curr=curr%60;
         int iw=0;
         record[curr]=block;
         if(curr==0){
           //start record
           begin=mypar[i].Time;
         }
         else if(curr==59 || i==nline-1){
            time(&insert);
            sleep(2);
           //end record;
               iw=1;
            if(i<nline-1 ){
              end=mypar[i+1].Time;
            }
            else end=mypar[i].Time+1200;
            for(int k=curr+1;k<60;k++)record[k]=block;
            ofstream fbin;
            char fname[256];
            sprintf(fname,"BeamPar.1.%d",begin);
            fbin.open(fname);
            insert++;
            uint crc=_CalcCRC((uinteger*)record,sizeof(record));
            fbin.write((char*)record,sizeof(record));
            fbin.write(( char*)&crc,sizeof(crc));
            fbin.write(( char*)&insert,sizeof(insert));
            fbin.write((char*)&begin,sizeof(begin));
            fbin.write((char*)&end,sizeof(end));
            fbin.close();
         }
     }

}


/*  CRC calculation */
uint _Table[256];
const uint CRC32=0x04c11db7;
uint Table_init=0;



void _InitTable(){
  if(!Table_init){
    int i,j;
    uint crc;
    for(i=0;i<256;i++){
      crc=i<<24;
      for(j=0;j<8;j++)
        crc=crc&0x80000000 ? (crc<<1)^CRC32 : crc<<1;
      _Table[i]=crc;
    }
    Table_init=1;
  }
}

uint _CalcCRC(uint *pData, int nsize){
  int i,j;
  uint crc, *pu= (uint *)pData;
 _InitTable();
  if( nsize < 1) return 0;
  crc=~pu[0];
    for(i=1;i<(nsize/4);i++){
      for(j=0;j<3;j++)
        crc=_Table[crc>>24]^(crc<<8);
     crc=crc^pu[i];
    }
  return ~crc;
}
