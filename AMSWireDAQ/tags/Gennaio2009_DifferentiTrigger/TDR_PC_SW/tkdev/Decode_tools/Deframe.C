#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <arpa/inet.h>

#include "Deframe.h"
#include "JMDCcodes.h"
char Block_ph::rrtypestring[2][10]={"Request","Reply"};
char Block_ph::rwtypestring[2][20]={"Write","Write and Read"};

Deframe::Deframe(FILE *fin,char* dout){
  CRCwords=0;
  HeaderWords=0;
  block_size=0;
  block_size2=0;
  written=0;
  debugIO=0;
  goodnode=0;
  goodtype=0;
  rawfile=fin;
  outfile=0;
  memset(frame,0,MAXSIZEF*sizeof(frame[0]));
  memset(bblock,0,MAXSIZE*sizeof(bblock[0]));
  sprintf(outdir,"%s",dout);
}
 
Deframe::~Deframe(){
    //  if(bblock) delete[] bblock;
  //  if(frame) delete[] frame;
}


 
void Deframe::DecodeFrameSecondaryHeader(){
  int debug=0;
  if(debug)printf("CCSD Secondary Header Present\n");

  time_t time=frame[HeaderWords]; 
  time=time<<16;

  HeaderWords++;
  time+=frame[HeaderWords]; 
  time+=315964800;
   fh2.time=time;

  HeaderWords++;
  fh2.finetime=frame[HeaderWords]>>8&0xff;
  fh2.timeid=frame[HeaderWords]>>6&0x3;
  fh2.CHK=frame[HeaderWords]>>5&1;
  fh2.ZOE=frame[HeaderWords]>>4&1;
  fh2.packet_type=frame[HeaderWords]&0xf;

  HeaderWords++;
  fh2.elemID=frame[HeaderWords]>>11&0xf;
  fh2.packetID1=frame[HeaderWords]&0x3ff;

  HeaderWords++;
  fh2.packetID2=frame[HeaderWords];
  if(fh2.CHK) CRCwords++;
  return; 
}
void Deframe::DecodeAMSHeader(){
  //Read AMS Header
  //! AMS Sequence word is missing!!!! 
  HeaderWords++;
  amsh.block_offset=frame[HeaderWords]&0xfff;
  amsh.fstatus=frame[HeaderWords]>>12&0xf;
  //skip the AMS secondary header
  if(amsh.fstatus&0x1)    HeaderWords+=5;
  if(amsh.fstatus&0x4)    CRCwords++;
  //  printf(" Header Words: %d CRCwords %d \n",HeaderWords, CRCwords);
  return;
}


int Deframe::WriteTheFrame(int start, int stop){
  int aa;
  int end=stop;
  if (stop>=(fh1.packet_len-CRCwords)) end=(fh1.packet_len-CRCwords);
  for (int ii=start;ii<end;ii++){
    //    if((aa=writeOneWord(frame[ii],outfile))<0) return aa;
    if (written<MAXSIZE){
      bblock[written]=frame[ii];
      written++;
    }
  }
  printf(" Written  %d words out of  %d... \n",written,block_size2);
  
  if(written==block_size2)
    if(GoodBlock()){
      for (int ii=0;ii<written;ii++)
	if((aa=writeOneWord(bblock[ii],outfile))<0) return aa;
      
    }
  //  delete[] bblock;
  return 1;
}


size_t Deframe::writeOneWord(ushort wsize,FILE * filout){
  size_t aa=fwrite(&wsize,sizeof(wsize),1,filout);
  if(debugIO) printf("Writing  %d %hX\n",wsize,wsize);
  return aa;
}

int Deframe::readOneWord(){
  int aa=readOneWord(&size,rawfile);
  return aa;
}

int Deframe::readOneWord(ushort* sizeout,FILE* rawfile,int byteswap){
  ushort rsize;
  size_t aa=fread(&rsize,sizeof(rsize),1,rawfile); 
  if(ferror(rawfile)) {printf("Error reading \n"); return -1;}
  if(feof(rawfile)) {printf("End of file \n"); return -2;}
  ushort tmp=rsize;
  
  if(byteswap){
    rsize=ntohs(tmp);
    if (debugIO)printf("------->pos: %4ld  read: %7d  %4hX  swapped: %7d  %4hX\n",
		     ftell(rawfile)/2,tmp,tmp,rsize,rsize);
  }
  else
    if (debugIO)printf("------->pos: %4ld  read: %7d  %4hX  \n",
		       ftell(rawfile)/2,rsize,rsize);
  *sizeout=rsize;
  
  return aa;
}

int Deframe::GetBlockSize(int boffset){
  int s2;
  int bhsiz=1;
  block_size=frame[boffset];
  int node=frame[boffset+1] >> 5 & 0x1ff;
  if(block_size>>15&1){
    s2=frame[boffset+1];
    block_size=block_size<<16+s2;
    node=frame[boffset+2] >> 5 & 0x1ff;
    bhsiz++;
 }
  block_size/=2;
  block_size2=  block_size+bhsiz;
  int hw=bhsiz;
  bhead.bsize  = block_size2;
  bhead.node   = frame[boffset+hw] >>5  & 0x01ff;
  bhead.type   = frame[boffset+hw]      & 0x001f;
  bhead.rrflag = frame[boffset+hw] >>15 & 0x0001;
  bhead.rwflag = frame[boffset+hw] >>14 & 0x0001;
  if(bhead.type==0x1f){
    hw++;
    bhead.type=frame[boffset+hw];
  }
  hw++;
  bhead.status = frame[boffset+hw] >> 12 & 0x000f;
  bhead.tag    = frame[boffset+hw]       & 0x0fff;
  hw++;
  bhead.time   = frame[boffset+hw];
  bhead.time   = bhead.time<<16;
  hw++;
  bhead.time  += frame[boffset+hw];

  bhead.first_word=boffset;
  bhead.last_word=boffset+hw;

  
  //printf("-------------> Block found size: %7d   node %3d\n",block_size,node);
  printf("Allocating a new block in memory siz=%d  bblock is %d\n",block_size2,bblock);
 //  if(bblock) free((void*) bblock);
//   bblock= (ushort*) malloc(sizeof(ushort)*block_size2);
//   if(bblock!=0) delete[] bblock;
//   bblock = new ushort [block_size2];
  written=0;
  return hw;
}

int Deframe::DecodeJMDCSubBlock(int offset){
 
  JMDC.bsize  = bblock[offset]/2;
  JMDC.node   = bblock[offset+1] >>5  & 0x01ff;
  JMDC.type   = bblock[offset+1]      & 0x001f;
  JMDC.rrflag = bblock[offset+1] >>15 & 0x0001;
  JMDC.rwflag = bblock[offset+1] >>14 & 0x0001;
  
  JMDC.runnum   = bblock[offset+4];
  JMDC.runnum   = JMDC.runnum<<16;
  JMDC.runnum  += bblock[offset+5];

  JMDC.runtag   = bblock[offset+2];
  JMDC.runtag   = JMDC.runtag<<16;
  JMDC.runtag  += bblock[offset+3];

  JMDC.eventnum   = bblock[offset+6];
  JMDC.eventnum   = JMDC.eventnum<<16;
  JMDC.eventnum  += bblock[offset+7];
  
  JMDC.event_time   = bblock[offset+8];
  JMDC.event_time   = JMDC.event_time<<16;
  JMDC.event_time  += bblock[offset+9];
  

  JMDC.event_ftime   = bblock[offset+10];
  JMDC.event_ftime   = JMDC.event_ftime<<16;
  JMDC.event_ftime  += bblock[offset+11];
  JMDC.toff=JMDC.bsize;
  JMDC.first_word = offset;
  JMDC.last_word = offset+JMDC.bsize;
  
  return 0;
}


int Deframe::BlockScan(int boffset){
  int aa;
  int bhsiz;
  int debug=0;
  for(;;){
    bhsiz=GetBlockSize(boffset);
    bhead.Print();

    if(block_size==0){ //it is the stupid fill block, dump everYthing
      if(debug)printf("Fill block skip the rest of frame \n");
      return 2;
    }else if(block_size<0){ //it is the stupid wrong block, skip
      return 3;
    }else {  //Block is regular
      if(debug)printf( "Writing the Block part of the frame to  ouput file ...\n"); fflush(stdout);
      if(block_size2>=(fh1.packet_len-CRCwords-boffset)){ 
	//block is longer than the frame
	aa=WriteTheFrame(boffset);
	if(debug)printf("The block continues in the next frame ... %d \n",block_size);
	return 2;
      }else{ //block is shorter than frame
	aa=WriteTheFrame(boffset,boffset+block_size2);
	if(debug)printf(" block ended ...  \n");
	boffset+=(block_size2);
      }
    }
  }
  return 0;
}


int Deframe::ReadOneFrame(){
  int aa=0;
  
  if((aa=readOneWord())<0) return aa;
  fh1.version = size >> 13 & 0x007;
  fh1.type    = size >> 12 & 0x001;
  fh1.SHF     = size >> 11 & 0x001;
  fh1.APID    = size       & 0x7ff;

  if((aa=readOneWord())<0) return aa;
  fh1.seqflag = size >> 14 & 0x0003;
  fh1.seqnum  = size       & 0x3fff;

  if((aa=readOneWord())<0) return aa;
  fh1.packet_len = (size+1)/2;
  //  if(frame) delete[] frame;
  //frame= new ushort[fh1.packet_len];
  for (int ii=0;ii<fh1.packet_len;ii++){
    if ((aa=readOneWord())<0) return aa;
    frame[ii]=size;
  }
  HeaderWords=0;
  CRCwords=0;
  return 0;
}


bool Deframe::GoodBlock(){
  char fullfname[120];
  bool good=(bhead.type==5||bhead.type==6 )&&
    bhead.node<goodnode && bhead.node>=0&& block_size2<MAXSIZE;

  if(good){
    
    int bhsiz=DecodeJMDCSubBlock(bhead.last_word-bhead.first_word+1);
    JMDC.Print();
    if(JMDC.runnum!=goodrun){
      goodrun=JMDC.runnum;
      if(outfile) fclose(outfile);
      if(bhead.type==6) //calibration
	sprintf(fullfname,"%s/%dC.bin",outdir,goodrun);
      else   //data
	sprintf(fullfname,"%s/%d.bin",outdir,goodrun);
      outfile=fopen(fullfname,"w");
      if(!outfile){ printf("Fatal: Cannot Open output file  %s!!!\n",fullfname); exit(2);}
      else
	printf("====================> New Run found %d creating the output file %s\n",goodrun,fullfname);
    }
  }

  return good;
      

}
