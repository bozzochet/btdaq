#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include <arpa/inet.h>


#include "DeBlock.h"
#include "JMDCcodes.h"
#include "TkDBc.h"
#include "TrRawCluster.h"
#include "POutF.h"
//#include "AMSEvent.h"


typedef unsigned long long int64;

int IsJinJ(int slave);
char label[25][10]={
  //0    1   2    3    4    5   
  "T2","T3","U1","T0","S1A","S1B",
  //6    7   8    9    10    11         
  "S0A","S0B","U0","T1","R0", "R1",
  //12  13   14     15   16    17       
  "E0","E1","J1A","J1B","T4","T5",
  //18   19   20   21   22  23          
  "S2A", "S2B","S3A","S3B","T6","T7","All"
};

extern int verbose;

char errmess[16][80]={"","","","","","replying node is CDP",
		      " RAW events",
		      " COMPRESSED events",
		      " cumulative status",
		      "build errors (sequencer errors for event building",
		      "build conditions (FE power fault for event building",
		      "NEXT bit / timeout (if DATA=1), set by master"	      ,
		      "ERROR bit / AMSW error (if DATA=1), set by master",
		      "ABORT bit / assembly error (if DATA=1), set by master",
		      "END bit / CRC error (if DATA=1), set by master",
		      "DATA bit, set by master when assembling group reply"};


void SubBlock::Print(){
  PRINTF("------> New SubBlock \n");
  Block_ph::Print();
  PRINTF("ev: %d  NodeStatus: %d FirstDword: %d LastDword: %d\n ",
	 eventnum,nodestatus,FirstDword,LastDword);
  
  return;
  
}

void JMDCBlock::Print(){    
  PRINTF("------> New JMDC SubBlock \n");
  Block_ph::Print();
  PRINTF("Run: %d  Tag: %d Event: %d FineTime: %d Time: %s ",
	 runnum,runtag,eventnum,event_ftime,ctime(&event_time));
  if(JinJMask){
    PRINTF("JinJ Mask: %X \n",JinJMask);
    for(int ii=0;ii<24;ii++)
      if(JinJSlave[ii]>=0)
	PRINTF("JinJ Slave %02d %4s  Mask: %X \n",JinJSlave[ii],label[JinJSlave[ii]],JinJSlaveMask[ii]); }
}


void JMDCBlock::Clear(){ 
  Block_ph::Clear();toff=0; 
  runnum=0;runtag=0; eventnum=0;event_time=0; event_ftime=0;event_fftime=0; 
  JinJMask=0; 
  TofFlag1=-1;
  for (int ii=0;ii<24;ii++) {JinJSlave[ii]=-1; JinJSlaveMask[ii]=0;
  }
}
 
 



//=============================================================================================

char Block_ph::rrtypestring[2][10]={"Request","Reply"};
char Block_ph::rwtypestring[2][20]={"Write","Write and Read"};

DeBlock::DeBlock(){
  Clear();
  CNWords=16;
  OnlyTestErrors=0;
  TDROff=1;
  caldb=0;
  rawfile=0;
  hh1=new TH2F("SizevsErrCode","Size Vs Error Code",55,0,55,5000,0, 50000);
  //  fopen(fullfname,"r");
  //   if(!rawfile){ PRINTF("Fatal: Cannot Open input file  %s!!!\n",fullfname); exit(2);}
  
  //  outfile= fopen(outname,"w");
  //   if(!outfile){ PRINTF("Fatal: Cannot Open Output file!!!\n"); exit(2);}
}
void DeBlock::Clear(){ 
  CRCwords=0;
  HeaderWords=0;
  block_size=0;
  block_size2=0;
  written=0;
  debugIO=0;
  memset(bblock,0,MAXSIZE*sizeof(bblock[0]));
  for(int ii=0;ii<24;ii++)  frag_size[ii]=-1;
  for(int ii=0;ii<192;ii++) tdr_size[ii]=-1;
  JMDC.Clear();
  bh1.Clear();
}

DeBlock::~DeBlock(){
  //  fclose(rawfile);
  // fclose(outfile);
  //  if(bblock) delete[] bblock;
}







int DeBlock::WriteTheBlock(int start, int stop){
  int aa;
  int end=stop;
  //  if (stop>=(fh1.packet_len-CRCwords)) end=(fh1.packet_len-CRCwords);
  for (int ii=start;ii<end;ii++){
    if((aa=writeOneWord(Block(ii)))<0) return aa;
    written++;
  }
  PRINTF(" Written  %d words out of  %d... \n",written,block_size2);
  
  return 1;
}


size_t DeBlock::writeOneWord(ushort wsize){
  size_t aa=fwrite(&wsize,sizeof(wsize),1,outfile);
  if(debugIO) PRINTF("Writing  %d %hX\n",wsize,wsize);
  return aa;
}

int DeBlock::readOneWord(){
  int aa=readOneWord(&size,rawfile);
  return aa;
}

int DeBlock::readOneWord(ushort* sizeout,FILE* rawfile,int byteswap){
  ushort rsize;
  size_t aa=fread(&rsize,sizeof(rsize),1,rawfile); 
  if(ferror(rawfile)) {PRINTF("Error reading \n"); return -1;}
  if(feof(rawfile)) {PRINTF("End of file \n"); return -2;}
  ushort tmp=rsize;
  
  if(byteswap){
    rsize=ntohs(tmp);
    if (debugIO)PRINTF("------->pos: %4ld  read: %7d  %4hX  swapped: %7d  %4hX\n",
		       ftell(rawfile)/2,tmp,tmp,rsize,rsize);
  }
  else
    if (debugIO)PRINTF("------->pos: %4ld  read: %7d  %4hX  \n",
		       ftell(rawfile)/2,rsize,rsize);
  *sizeout=rsize;
  
  return aa;
}



int DeBlock::ReadOneBlock(){
  int aa=0;
  ushort s2;
  //PRINTF("Starting postion is %d \n",ftell(rawfile));
  if((aa=readOneWord())<0) return aa;
  bh1.bsize=size;
  if(block_size>>15&1){
    if((aa=readOneWord())<0) return aa;
    s2=size;
    bh1.bsize = bh1.bsize << 16 + s2;
  }

  //  PRINTF("Size in byte is %d \n",bh1.bsize);
  int oddsize=bh1.bsize%2;

  size_t cc=fread(bblock,1,bh1.bsize,rawfile);

  if(ferror(rawfile)) {PRINTF("Error reading \n"); return -1;}
  if(feof(rawfile)) {PRINTF("End of file \n"); return -2;}

  if(oddsize) {
    fread(&size,1,1,rawfile);
    if(ferror(rawfile)) {PRINTF("Error reading \n"); return -1;}
    if(feof(rawfile)) {PRINTF("End of file \n"); return -2;}
  }

  bh1.bsize++;
  bh1.bsize/=2;
  block_size2=  bh1.bsize;
//   //  bblock = new ushort[bh1.bsize];  
//   for (int ii=0;ii< bh1.bsize;ii++){
//     if ((aa=readOneWord())<0) return aa;
//     bblock[ii]=size;
//     //    PRINTF("  %4d  %6d\n",ii,bblock[ii]);
//   }
//   if(oddsize) {
//     char odd;
//     size_t cc=fread(&odd,1,1,rawfile);
//     PRINTF("Fucking odd size block!!!!!!!!!!! reading one more byte  %d !!!!!!!!! \n",cc);
//   }
  HeaderWords=0;
  CRCwords=0;
  return 0;
}


void  DeBlock::DecodeBlockHeader(){
  int hw=0;
  int offset=0;
  bh1.bsize  = block_size2;
  bh1.node   = Block(offset+hw) >>5  & 0x01ff;
  bh1.type   = Block(offset+hw)      & 0x001f;
  bh1.rrflag = Block(offset+hw) >>15 & 0x0001;
  bh1.rwflag = Block(offset+hw) >>14 & 0x0001;
  if(bh1.type==0x1f){
    hw++;
    bh1.type=Block(offset+hw);
  }
  hw++;
  bh1.status = Block(offset+hw) >> 12 & 0x000f;
  bh1.tag    = Block(offset+hw)       & 0x0fff;
  hw++;
  bh1.time   = Block(offset+hw);
  bh1.time   = bh1.time<<16;
  hw++;
  bh1.time  += Block(offset+hw);

  bh1.first_word=offset;
  bh1.last_word=offset+hw;
  HeaderWords=++hw;
  return;
 
}

void DeBlock::DecodeSubBlockHeader(SubBlock *bb,int offset){

  int hw=1;
  int sw=1;
  bb->bsize  = Block(offset+hw);
  if(bb->bsize>>15&1){
    sw++;
    bb->bsize =  bb->bsize << 16;
    hw++;
    bb->bsize  += Block(offset+hw);
  }
  bb->bsize  /= 2;
  bb->bsize  += sw;
  hw++;
  bb->node   = Block(offset+hw) >>5  & 0x01ff;
  bb->type   = Block(offset+hw)      & 0x001f;
  bb->rrflag = Block(offset+hw) >>15 & 0x0001;
  bb->rwflag = Block(offset+hw) >>14 & 0x0001;
  if(bb->type==0x1f){
    hw++;
    bb->type=Block(offset+hw);
  }
  hw++;
  bb->eventnum=Block(offset+hw);
  hw++;
  bb->FirstDword=offset+hw;

  bb->first_word=offset+1;
  bb->last_word=offset+bb->bsize;

  bb->LastDword= bb->last_word-2;

  bb->nodestatus=Block(bb->last_word-1);
  bb->CRC=Block(bb->last_word);
  
  
  return;
 
}

int DeBlock::DecodeJMDCSubBlock(){
  int offset=HeaderWords;
 
  JMDC.bsize  = Block(offset)/2;
  JMDC.node   = Block(offset+1) >>5  & 0x01ff;
  JMDC.type   = Block(offset+1)      & 0x001f;
  JMDC.rrflag = Block(offset+1) >>15 & 0x0001;
  JMDC.rwflag = Block(offset+1) >>14 & 0x0001;
  
  JMDC.runnum   = Block(offset+4);
  JMDC.runnum   = JMDC.runnum<<16;
  JMDC.runnum  += Block(offset+5);

  JMDC.runtag   = Block(offset+2);
  JMDC.runtag   = JMDC.runtag<<16;
  JMDC.runtag  += Block(offset+3);

  JMDC.eventnum   = Block(offset+6);
  JMDC.eventnum   = JMDC.eventnum<<16;
  JMDC.eventnum  += Block(offset+7);
  
  JMDC.event_time   = Block(offset+8);
  JMDC.event_time   = JMDC.event_time<<16;
  JMDC.event_time  += Block(offset+9);
  
  
  JMDC.event_ftime   = Block(offset+10);
  JMDC.event_ftime   = JMDC.event_ftime<<16;
  JMDC.event_ftime  += Block(offset+11);
  int gpspresent= Block(offset+12)>>15  &   1;
  int MaskNumber= Block(offset+12)>>6   & 0xf;
  int goff=0;
  //  PRINTF(" %X  %d \n",Block(offset+12),MaskNumber);
  if(gpspresent) goff=5;
  if(MaskNumber){
    JMDC.JinJMask=(Block(offset+goff+13)&0xff);
    JMDC.JinJMask=  JMDC.JinJMask <<16;
    JMDC.JinJMask= JMDC.JinJMask | Block(offset+goff+14);
    for (int ii=0;ii<MaskNumber-1;ii++){
      int soffset=offset+goff+15+2*ii;
      JMDC.JinJSlave[ii]=Block(soffset)>>8 &0xff;
      JMDC.JinJSlaveMask[ii]=Block(soffset) &0xff;
      JMDC.JinJSlaveMask[ii]= JMDC.JinJSlaveMask[ii] <<16;
      JMDC.JinJSlaveMask[ii]= JMDC.JinJSlaveMask[ii] | Block(soffset+1);
    }
  }
  JMDC.toff=JMDC.bsize;
  JMDC.first_word = offset;
  JMDC.last_word = offset+JMDC.bsize;
  
  return 0;
}




int DeBlock::DecodeJLV1(int p, int len, int pri){


  // p=p-1;//to follow VC-convention(here points to length-word)
  int64 event_time3, event_time1, event_time2; 
  static int64 event_oldtime=0;
  int64 event_time=0;
  static int oldevnum=0; 
  ushort jleng=ushort(len&(0xFFFFL));
  ushort Status=Block(p+jleng);// JLV1 fragment's last word: Status+slaveID(its id)

  event_time = 0;
  event_time1 = Block(p+1+6)&0x00FF;
  event_time1 = event_time1<<32;
  event_time2 = Block(p+1+5);
  event_time2 = event_time2<<16;
  event_time3 = Block(p+1+4);
  event_time = (event_time1 +event_time2 +event_time3);

  if(JMDC.eventnum==(oldevnum+1)){
    JMDC.event_fftime=(event_time-event_oldtime)*0.64;
    if(pri) PRINTF("---------------> event time  %lld  %lld %lld  %f %f  %d %d \n",event_time,event_oldtime,
		   event_time-event_oldtime,
		   ( event_time-event_oldtime)*0.64,JMDC.event_fftime,
		   JMDC.eventnum,oldevnum);

  
  }else {
    JMDC.event_fftime=0;
    
  }
//    if(JMDC.event_fftime<250)
//      printf("=========================================> event time  %lld  %lld %lld  %f %f  %d %d \n",event_time,event_oldtime,
//	  event_time-event_oldtime,
//	  ( event_time-event_oldtime)*0.64,JMDC.event_fftime,
//	  JMDC.eventnum,oldevnum);
  JMDC.TofFlag1=12;
  event_oldtime=event_time;
  oldevnum=JMDC.eventnum;
  //   PRINTF("event-------------------> evtold   %30.4f\n",event_oldtime);
  //   PRINTF("event-------------------> evtcurr  %30.4f\n",event_time);
  //   PRINTF("event-------------------> evtcurr  %30.4f\n",JMDC.event_fftime);
  ushort word=Block(p+1+3);//Tof CP,CT,BZ layers pattern
  if((word&0x000F)==0x000F)     JMDC.TofFlag1=0;//1111 //all4
  else if((word&0x000F)==0x000E)JMDC.TofFlag1=1;//1110 //2,3,4(miss1)
  else if((word&0x000F)==0x000D)JMDC.TofFlag1=2;//1101 //1,3,4(miss2)
  else if((word&0x000F)==0x000B)JMDC.TofFlag1=3;//1011 //1,2,4(miss3)
  else if((word&0x000F)==0x0007)JMDC.TofFlag1=4;//0111 //1,2,3(miss4)
  else if((word&0x000F)==0x0005)JMDC.TofFlag1=5;//0101 //1,3
  else if((word&0x000F)==0x0009)JMDC.TofFlag1=6;//1001 //1,4
  else if((word&0x000F)==0x0006)JMDC.TofFlag1=7;//0110 //2,3
  else if((word&0x000F)==0x000A)JMDC.TofFlag1=8;//1010 //2,4
  else if((word&0x000F)==0x0003)JMDC.TofFlag1=9;//0011 //1,2(miss3,4)
  else if((word&0x000F)==0x000C)JMDC.TofFlag1=10;//1100 //3,4(miss1,2)
  else
    JMDC.TofFlag1=12;
  //  printf("%04X\n",Block(p+1+1));
  //if(Block(p+1+1)==1) JMDC.TofFlag1*=-1;
  if(pri)PRINTF("   <-- Trigger2LVL1 TofFlag = %d\n",JMDC.TofFlag1);
  return 0;
}



int DeBlock::ScanBlocks(int pri){
  SubBlock aa;
  int offset=JMDC.last_word;
  int bb=0;
  int cc=0;
  while(aa.last_word<(bh1.bsize-1)){
    DecodeSubBlockHeader(&aa,offset);
    if(pri)aa.Print();
    if(pri)PRINTF("===============================================\n");
    if(pri)PRINTF("%s size: %d address %02d  -------------------------------> status %hX \n",aa.JMDCcodes[aa.node],aa.bsize,aa.nodestatus&0x1f,aa.nodestatus); 

    cc=TestBoardErrors(aa.JMDCcodes[aa.node],aa.nodestatus,0);
    if(cc<0) hh1->Fill(50,block_size2);

    if(IsJinJ(aa.node)){  
      int joffset=aa.FirstDword;
      int jsize=Block(joffset);
      int frag=Block(joffset+jsize)&0x1F;
      bb=0;
      while ( joffset<(aa.last_word-2) )  {
	//    frag_size[frag]=jsize;
	int nn=ReadOneJINF(joffset,jsize,pri);
	if(nn<0) bb=-1;
	joffset = joffset + jsize +1 ;
	frag=Block(joffset+jsize)&0x1F;
	jsize = Block(joffset);
      }
    }else if(aa.node==158 ||aa.node==159 ){ //JINR0
      frag_size[10]+=aa.bsize;
    }else if(aa.node==162 ||aa.node==163 ){ //JINR1
      frag_size[11]+=aa.bsize; 
    }
    offset=aa.last_word;
    if(bb<0||cc<0) {return -1;}


  }
  return 0;
}


void DeBlock::ScanBlocksCal(int pri){
  SubBlock aa;
  int offset=JMDC.last_word;
  PRINTF("===============================================\n");
  DecodeSubBlockHeader(&aa,offset);
  aa.Print();
  int joffset=aa.FirstDword-1;
  int jsize=aa.LastDword-aa.FirstDword+2;
  //  if(aa.node> 281&&aa.node<474) 
DecodeOneCal(&aa,joffset,jsize);
     
   
}

void DeBlock::ScanBlocksSlow(int pri){
  int count;
  if(bh1.node==0x82&&Block(5)==0x23C3){
    PRINTF("\n");
    count=0;
    for(int ii=0;ii<5;ii++)
      {
        PRINTF(" %04X ",Block(ii));
	if((count++)%8==7) PRINTF("\n");
      }
    PRINTF("\n");
    PRINTF("\n");
    count=0;
    for(int ii=5;ii<15;ii++)
      {
        PRINTF(" %04X ",Block(ii));
	if((count++)%8==7) PRINTF("\n");
      }
    PRINTF("\n");
    int start=8;
    int off=start+Block(8);
    int msize=Block(8);
    int status=Block(off);

    while(off<bh1.bsize){
      PRINTF("Size: %5d Status: %04X  JINF: %02d\n",msize,status,status&0x1f);
      if(verbose>2){
	count=0;
	//	PRINTF(" %04X %04X %04X \n",Block(start+1),Block(start+2),Block(start+3));
	for(int ii=start+1;ii<off;ii++)
	  {
	    PRINTF(" %04X ",Block(ii));
	    if((count++)%9==8) PRINTF("\n");
	  }
	PRINTF("\n\n");
      }

      start=off+1;
      msize=Block(off+1);
      off+= (msize+1);
      status=Block(off);


    
    }
    count=0;
    int kk=695*8+3+4-10;
    for (int ii=kk;ii<bh1.bsize+1;ii++){
      PRINTF(" %04X ",Block(ii));
      if((count++)%8==7) PRINTF("\n");
    }
    PRINTF("\n\n");
  }
  
  
}





int DeBlock::ReadOneJINF(const int offset, const int jsize,int pri){ 

  ushort status=Block(offset+jsize);
  int JinfNum=((unsigned int)status)&0x1f;
  char nname[400];
   frag_size[JinfNum]=jsize;
  sprintf(nname,"JINF Board: %2d %5s status: %hX Size: %5d offset: %5d last word: %5d \n",
	  status&0x1f,
	  label[status&0x1f],
	  status,jsize,offset,offset+jsize); 
  int ret=TestBoardErrors(nname,status,0);
  if(ret==0)PRINTF("%s",nname);	
  //           0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 
  int JT[24]={ 2, 3,11, 0,11,11,11,11,11, 1,11,11,11,11,11,11, 4, 5,11,11,11,11, 6, 7};
  if(ret ) {hh1->Fill(JinfNum,block_size2);}
  if(ret<0 && JinfNum!=12 && JinfNum!=13) {return -1;}
  //if(ret ) {return -1;}
  //if(ret) {hh1->Fill(JinfNum,block_size2);}
  //  if(!out_flag)JinfMap[nJinf++]=status&0x1f;
  //      JinfNum==3||JinfNum==9||JinfNum==16||JinfNum==17||  //WAKE
  if(JinfNum==3||JinfNum==9||JinfNum==16||JinfNum==17||  //WAKE
     JinfNum==1||JinfNum==0||JinfNum==23||JinfNum==22){  //RAM
    int toffset=offset+1;
    int tsize=Block(toffset);
    int frag=Block(toffset+tsize)&0x1F;
    int bb=0;
    int tdrcount=0;
    while ( toffset<(offset+jsize-1) )  {
      tdr_size[frag+JT[JinfNum]*24]=tsize;
      int nn=ReadOneTDR(toffset,tsize,JinfNum,pri);
      if(nn<0) bb=-1;
      toffset = toffset + tsize +1 ;
      tsize = Block(toffset);
      frag=Block(toffset+tsize)&0x1F; 
      tdrcount++;
    }
    if(pri>2) PRINTF(" On JINF %2d Found %2d TDRs\n",JinfNum,tdrcount);
    if(bb<0) return -1;
  }
  else if( JinfNum==4){
    ev->TofData.Decode(offset, jsize, bblock);
    ev->T1 = ev->TofData.HitPattern[3];
    ev->T2 = ev->TofData.HitPattern[0];
  }

  else if (JinfNum == 14){
    DecodeJLV1(offset,jsize,pri);
  }

  else if (JinfNum == 6 || JinfNum == 18 || JinfNum == 20)
    ev->TofData.Decode(offset, jsize, bblock);

  return 0;
}


int DeBlock::PrintToFFiring(ushort w0,ushort w1){

  if(w1&1)    PRINTF("TOF  101 Fired\n");
  if(w1>>1&1) PRINTF("TOF  103 Fired\n");
  if(w1>>2&1) PRINTF("TOF  105 Fired\n");
  if(w1>>3&1) PRINTF("TOF  107 Fired\n");
  if(w1>>5&1) PRINTF("TOF  102 Fired\n");
  if(w1>>6&1) PRINTF("TOF  104 Fired\n");
  if(w1>>7&1) PRINTF("TOF  106 Fired\n");
  if(w1>>8&1) PRINTF("TOF  108 Fired\n");


  if(w0&1)    PRINTF("TOF  201 Fired\n");
  if(w0>>1&1) PRINTF("TOF  203 Fired\n");
  if(w0>>2&1) PRINTF("TOF  205 Fired\n");
  if(w0>>3&1) PRINTF("TOF  207 Fired\n");
  if(w0>>5&1) PRINTF("TOF  202 Fired\n");
  if(w0>>6&1) PRINTF("TOF  204 Fired\n");
  if(w0>>7&1) PRINTF("TOF  206 Fired\n");
  if(w0>>8&1) PRINTF("TOF  208 Fired\n");
  return 0;

}

int DeBlock::ReadOneTDR(int offset,int tsize,int Jinfnum,int pri){
int clcount=0;
  int rwords=1+CNWords;
  
  char ttname[500];
  ushort status=Block(offset+tsize);
  int TDRnum=((unsigned int)status)&0x1f;
  sprintf(ttname,"=======>JINF %d %s TDR Board: %2d status: %hX offset: %d last word: %d  Size: %d \n",Jinfnum,label[Jinfnum],status&0x1f,status,offset,offset+tsize,tsize); 
  int ret=TestBoardErrors(ttname,status,pri);
  if(OnlyTestErrors) return ret;  
  int RawOffset=0;
  if (status>>6&1) {
    RawOffset=1024;   // RAW data present
    if(pri>4)PRINTF("  RAW data present  ignoring it\n ");
    
  }  
    
  int cluster_size=tsize-rwords;
  
  if ((status>>7&1)&&(cluster_size>0)){    // Compressed data present
    if(pri>3)PRINTF(" Dumping cluster  Compressed data present \n");
    //dump clusters
    int  count=offset+RawOffset+TDROff;
    while (count<(offset+tsize-rwords)){
      int bad=0;
      int cluslen=Block(count++)+1;
      int clusadd=Block(count++);
      short int  signal[1024];
      float      sigma[1024];
      short int  status[1024];
      if(pri>3)PRINTF("Cluster: add: %d  lenght: %d\n",clusadd,cluslen);
      //count+=(cluslen-1);
      if(cluslen>1024) cluslen=1024;
      int hwid=TkDBc::Head->GetCrateFromJinJ(Jinfnum)*100+TDRnum;
      TkLadder *lad= TkDBc::Head->FindHwId(hwid);
      int tkid=0;
      if(lad) tkid=lad->GetTkId();

      for(int hh=0;hh<cluslen;hh++){
	if(pri>3)PRINTF("signal: %d %d\n",hh,Block(count));
	signal[hh]  = (short int)Block(count);
	//if(pri)PRINTF("        %d %f %f %d\n",hh,signal[hh]/8.,sigma[hh],status[hh]);
	count++;
      }
      
      int sid=0;
      if(clusadd>640) sid=1;
      AMSTrRawCluster* pp= new AMSTrRawCluster(tkid,clusadd,cluslen,signal);
//       AMSContainer* con= AMSEvent::gethead()->getC(AMSID("AMSTrRawCluster"));
//       if(con){
// 	con->addnext(pp);
//       }
      if(ev){	
 	ev->AddTrRawCluster(pp);
	clcount++;
      }
    
    }
  }    
  int cc=1;
  //Read the  cnoise values
  if(pri>4&&rwords==17){
    PRINTF("Cnoise \n" );
    for (int ii=offset+tsize-rwords;ii<offset+tsize-1;ii++){
      PRINTF(" %4d:  %6.3f ",cc++,((short int)Block(ii))/8.);
      if(cc%4==1) PRINTF("\n");
    }
  }

  if(pri>2) PRINTF(" On TDR %2d Found %2d Clus\n",TDRnum,clcount);
  if(pri>3)PRINTF("End of the TDR %2d  \n",TDRnum);
  
 
  return 0;
    
}




int DeBlock::TestBoardErrors(char *name,ushort status,int pri){

  int count;
  if(pri>4){
  for (count=5;count<16;count++)
    if(status>>count&1)
      PRINTF("%02d: %s\n",count,errmess[count]);
  }

  
  //if(out_flag)ev->JINFStatus[Jinfnum]=status;

  if((status>>9&0x3f)>0)    // (status>>9&1) ||  (status>>10&1)||  (status>>11&1)||  (status>>12&1)|| (status>>13&1)|| (status>>14&1))      
    {
      
      //PRINTF("\n  =======================> Error!!!  Skip this event:%s Board %d %s %hX\n",name,status&0x1f,label[status&0x1f],status); 
      PRINTF("%s",name);
      count=5;
      for (count=5;count<16;count++)
	if(status>>count&1)
	  if(verbose)		PRINTF("%02d: %s\n",count,errmess[count]);
      if(verbose)		PRINTF("\n");
      return -1;
    }
  return 0;
}
 
 
int DeBlock::DecodeOneCal(SubBlock *bb,int offset,int csize,int pri){
  CaloutDSP cal;
  ushort size;
  int rr=offset;
  int cpar=Block(rr++);
  PRINTF("Command parameter is: %X \n",cpar); 

  if(cpar&1){ //pedestals
    PRINTF("Reading Pedestals\n");
    for (int ii=0;ii<1024;ii++){
      cal.ped[ii]=Block(rr++);
    }
  }

  if(cpar&2){ //flags
    PRINTF("Reading flags\n");
    for (int ii=0;ii<1024;ii++){
      cal.status[ii]=Block(rr++);
    }
  }

  if(cpar&4){ //sigma
    PRINTF("Reading Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      cal.sig[ii]=Block(rr++);
    }
  }

  if(cpar&8){ //rawsigma
    PRINTF("Reading Raw Sigmas\n");
    for (int ii=0;ii<1024;ii++){
      cal.rsig[ii]=Block(rr++);
    }
  }
 

  PRINTF("Detector DSP version %hX\n",Block(rr++));
  
  cal.S_lowthres=Block(rr++);
  PRINTF("Lowsigma factor S %f\n",cal.S_lowthres);
  
  cal.S_highthres=Block(rr++);
  PRINTF("Highsigma factor S %f\n",cal.S_highthres);

  cal.K_lowthres=Block(rr++);
  PRINTF("Lowsigma factor K %f\n",cal.K_lowthres);
  
  cal.K_highthres=Block(rr++);
  PRINTF("Highsigma factor K %f\n",cal.K_highthres);
  
  cal.sigrawthres=Block(rr++);
  PRINTF("sigmaraw factor %f\n",cal.sigrawthres);
  
  cal.calnum=Block(rr++);
  PRINTF("Calibration Events %d\n",cal.calnum);
  
  
  cal.calstatus=Block(rr++);
  PRINTF("Calibration status 0x%04X\n",cal.calstatus);

  int TDRStat=Block(rr++);
  PRINTF("TDR status 0x%04X\n",TDRStat);
  
  PRINTF("---------> READ   %d  word for this cal\n",rr-offset);
  //Fill the Calibration object
  TrLadCal* ladcal=caldb->FindCal_JMDC(bb->node);
  if(ladcal){
    ladcal->Fill(&cal);
    ladcal->PrintInfo();
  }else
    PRINTF("-------------------> WARNING I CANT FIND The calibration object to be filled\n");

 
  
  return 1;
}



int IsJinJ(int slave){
  if(slave>=128 && slave <= 135 ) return 1;
  else return 0;

}
