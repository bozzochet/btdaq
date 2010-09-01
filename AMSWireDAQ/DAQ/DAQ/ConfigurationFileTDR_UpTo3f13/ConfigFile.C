#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

void PrepareFCSTable();
unsigned short DoFCS(unsigned short data, unsigned short fcs);
int CreateScript();

unsigned short FCS_Table[256];

int oldconfname=0x6001;//0x because the names of the files on flash are hexadecimal
int newconfname=0x6001;//0x because the names of the files on flash are hexadecimal

int main(int argc, char ** argv) {
  int pri=1;

  if(argc<=7){
    printf("Usage: %s  <input_txt_file> <S1HighThresh> <S1LowThresh> <S2HighThresh> <S2LowThresh> <KHighThresh> <KLowThresh> [OldConfName] [NewConfName]\n",argv[0]);
    printf("Thresholds have to be passed in 'standard' way (1, 2, 3.5, ...)\n");
    printf(" It output 5 files configfile_X.dat and 5 files configfile_extpl_X.dat in ./dotdats/, x=[0-5]\n");
    return -1;
  }

  float S1H=0;
  float S1L=0;
  float S2H=0;
  float S2L=0;
  float KH=0;
  float KL=0;

  sscanf(argv[2],"%f", &S1H);
  sscanf(argv[3],"%f", &S1L);
  sscanf(argv[4],"%f", &S2H);
  sscanf(argv[5],"%f", &S2L);
  sscanf(argv[6],"%f", &KH);
  sscanf(argv[7],"%f", &KL);
  if(argc>8) sscanf(argv[8],"%hx", &oldconfname);
  if(argc>9) sscanf(argv[9],"%hx", &newconfname);

  int S1HighThresh=lrintf(8*S1H);
  int S1LowThresh =lrintf(8*S1L);
  int S2HighThresh=lrintf(8*S2H);
  int S2LowThresh =lrintf(8*S2L);
  int KHighThresh =lrintf(8*KH);
  int KLowThresh  =lrintf(8*KL);

  int InnerS1HighThresh=0;
  int InnerS1LowThresh =0;
  int InnerS2HighThresh=0;
  int InnerS2LowThresh =0;
  int InnerKHighThresh =0;
  int InnerKLowThresh  =0;
  
  for (int ext=0; ext<2; ext++){
    for (int cf=0; cf<5; cf++){
      
      PrepareFCSTable();
      unsigned short fcs=0xffff;
      unsigned short data=0;
      int bufsize = 1024;
      char buffer[bufsize];
      char *pch;
      char *nome;
      char comment[4] = "#|;";
      
      unsigned short datain[1000];
      unsigned short GSI[3];    
      
      FILE *infile=0, *outfile=0;
      infile=fopen(argv[1],"rt");
      if (!infile) {
	printf("input file error\n");
	return 1;
      }
      
      unsigned short val1, val2;
      int cnt=0;
      while (fgets (buffer, bufsize, infile)){ //loop sulle righe di file.conf
	if (feof(infile)) break;
	nome = strtok (buffer, " ");
	if (nome[0]==comment[0])  continue; // skip the comments
	
	//Read Group, SubG and ID
	
	sscanf(nome,"%hd", &GSI[0]);
	nome = strtok (NULL, " ");
	//	printf("%hd ", GSI[0]);
	
	sscanf(nome,"%hd", &GSI[1]);
	nome = strtok (NULL, " ");
	//	printf("%hd ", GSI[1]);
	
	sscanf(nome,"%hd", &GSI[2]);
	nome = strtok (NULL, " ");
	//	printf("%hd ", GSI[2]);
	
	datain[cnt]=GSI[0]<<12|GSI[1]<<8|GSI[2];
	//	printf("%hx ", datain[cnt]);
	
	//Read the value
	
	sscanf(nome,"%hx", &datain[cnt+1]);
	//	printf("%hx\n", datain[cnt+1]);
	if (datain[cnt]==1) InnerS1HighThresh = datain[cnt+1];
	if (datain[cnt]==2) InnerS1LowThresh = datain[cnt+1];
	if (datain[cnt]==3) InnerS2HighThresh = datain[cnt+1];
	if (datain[cnt]==4) InnerS2LowThresh = datain[cnt+1];
	if (datain[cnt]==5) InnerKHighThresh = datain[cnt+1];
	if (datain[cnt]==6) InnerKLowThresh = datain[cnt+1];
	
	//	printf("Old: %hx %hx\n", datain[cnt-2], datain[cnt-1]);
	//	printf("Actual: %hx %hx\n", datain[cnt], datain[cnt+1]);
	
	cnt+=2;
      }
      
      unsigned short npar=cnt/2;
      
      //  printf("cnt=%d\n",cnt);
      
      fclose(infile);
      
      char filename[256];
      if (ext==0) sprintf(filename, "./dotdats/configfile_%d.dat", cf);
      else if (ext==1) sprintf(filename, "./dotdats/configfile_extpl_%d.dat", cf);

      outfile=fopen(filename,"wt");
      if (!outfile) {
	printf("output file error\n");
	return 1;
      }
      
      printf("New Config File (%s):The TAS Ladder type parameter set to %d!\n", filename,cf);
      
      if(pri) printf("%04hx\n",newconfname);
      fprintf(outfile, "%04hx\n", newconfname);
      if(pri) printf("%04hx %04hx\n",0x4000+cnt+2, 0x9000);
      fprintf(outfile, "%04hx %04hx\n", 0x4000+cnt+2, 0x9000);
      
      npar+=0x1000; // 0x1000: for the tracker
      if(pri) printf("%04hx\n", npar);
      fprintf(outfile, "%04hx\n", npar);
      fcs=DoFCS(npar,fcs);
      
      for (int i=0; i<cnt; i++) {
	if (i==15) {
	  fprintf(outfile, "%04hx\n", cf);
	  if(pri) printf("%04hx\n",cf);
	  fcs=DoFCS(cf,fcs);
	}
	else if (ext==1 && i==1){
	  fprintf(outfile, "%04hx\n", S1HighThresh);
	  if(pri) printf("%04hx\n", S1HighThresh);
	  fcs=DoFCS(S1HighThresh,fcs);
	}
	else if (ext==1 && i==3){
	  fprintf(outfile, "%04hx\n", S1LowThresh);
	  if(pri) printf("%04hx\n", S1LowThresh);
	  fcs=DoFCS(S1LowThresh,fcs);
	}
	else if (ext==1 && i==5){
	  fprintf(outfile, "%04hx\n", S2HighThresh);
	  if(pri) printf("%04hx\n", S2HighThresh);
	  fcs=DoFCS(S2HighThresh,fcs);
	}
	else if (ext==1 && i==7){
	  fprintf(outfile, "%04hx\n", S2LowThresh);
	  if(pri) printf("%04hx\n", S2LowThresh);
	  fcs=DoFCS(S2LowThresh,fcs);
	}
	else if (ext==1 && i==9){
	  fprintf(outfile, "%04hx\n", KHighThresh);
	  if(pri) printf("%04hx\n", KHighThresh);
	  fcs=DoFCS(KHighThresh,fcs);
	}
	else if (ext==1 && i==11){
	  fprintf(outfile, "%04hx\n", KLowThresh);
	  if(pri) printf("%04hx\n", KLowThresh);
	  fcs=DoFCS(KLowThresh,fcs);
	}
	else {
	  fprintf(outfile, "%04hx\n",datain[i]);
	  if(pri) printf("%04hx\n",datain[i]);
	  fcs=DoFCS(datain[i],fcs);
	}
      }
      
      if(pri)    printf("%hx\n", fcs);
      fprintf(outfile, "%hx\n", fcs);
      
      /*
	for (int i=0; i<1000; i++) {
	printf("type number in hex: ");
	fflush(stdout);
	scanf("%hx", &data);
	fcs=DoFCS(data,fcs);
	printf("fcs= %x\n", fcs);
	}
      */
      fclose(outfile);
      
    }
  }

  if(CreateScript()) return -1;

  printf("You set TRESHOLD to 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx for inner tracker!\n", InnerS1HighThresh, InnerS1LowThresh, InnerS2HighThresh, InnerS2LowThresh, InnerKHighThresh, InnerKLowThresh);
  printf("You set TRESHOLD to 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx, 0x%hx external planes!\n", S1HighThresh, S1LowThresh, S2HighThresh, S2LowThresh, KHighThresh, KLowThresh);

  return 0;  
}

unsigned short DoFCS(unsigned short data, unsigned short fcs) {

  // from Kounine + Koutsenko documentation, p30

  //printf("DoFCS: data[0]=%x\n",data[0]);

  unsigned char val;

  val=data>>8;
  fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);
  val=data & 0xff;
  fcs=FCS_Table[(fcs>>8)^val]^(fcs<<8);

  //  printf("DoFCS = %04x\n",fcs);

  return fcs;
}

void PrepareFCSTable() {

  // from Kounine + Koutsenko documentation, p30

  unsigned short const g=0x1021;
  unsigned short d,i,k,fcs;

  // prepare lookup table

  for(i=0; i<256; i++) {
    fcs=0;
    d=i<<8;
    for(k=0; k<8; k++) {
      if ((fcs^d) & 0x8000) fcs=(fcs<<1)^g;
      else fcs<<=1;
      d<<=1;
    }
    FCS_Table[i]=fcs;
  }
}

int CreateScript(){

  FILE *outfile=0;
  int JinjPort[8];
  char sfirst[256];
  char ssecond[256];
  char sthird[256];
  
  outfile=fopen("./LoadConfs.sh","wt");
  if (!outfile) {
    printf("output file ./LoadConfs.sh error\n");
    return -1;
  }
  
  for (int CrateNum=0; CrateNum<8; CrateNum++){
    if (CrateNum==3) JinjPort[CrateNum]=1;
    else if (CrateNum==2) JinjPort[CrateNum]=0;
    else if (CrateNum==7) JinjPort[CrateNum]=23;
    else if (CrateNum==6) JinjPort[CrateNum]=22;
    else if (CrateNum==0) JinjPort[CrateNum]=3;
    else if (CrateNum==1) JinjPort[CrateNum]=9;
    else if (CrateNum==4) JinjPort[CrateNum]=16;
    else if (CrateNum==5) JinjPort[CrateNum]=17;
  }
  
  int CrateMin=0;
  int CrateMax=7;
  
  int tastype=0;
  int tastypecheck=99;
  int hwid=999;
  
  sprintf(sfirst, "rm -f ./file.dat\ncp -fv ./dotdats/configfile_");
  sprintf(ssecond, ".dat ./file.dat\nOUTPUT=`./TESTjmdc $port ");
  sprintf(sthird, " 45 | grep rxdone`\n");
  
  fprintf(outfile, "#!/bin/bash\n");
  
  fprintf(outfile, "if [ $# -lt 1 ]\n then\n");
  fprintf(outfile, "  echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]\n");
  fprintf(outfile, "  exit\n");
  fprintf(outfile, "fi\n");
  fprintf(outfile, "port=$1\n");
  
  fprintf(outfile, "CMDERRORS=0\n");
  fprintf(outfile, "WRTERRORS=0\n");
  fprintf(outfile, "ERSERRORS=0\n");
  
  //Erasing cycle...
  for (int CrateNum=CrateMin; CrateNum<=CrateMax; CrateNum++){
    for (int TdrNum=0; TdrNum<24; TdrNum++){
      //      printf("Processing Crate %d and TDR %d\n", CrateNum, TdrNum);
      fprintf(outfile, "OUTPUT=`./TESTjmdc $port %02x3f%02x3f 7 | grep %hx`\n", TdrNum, JinjPort[CrateNum], oldconfname);
      fprintf(outfile, "if [ -n \"$OUTPUT\" ]\n then\n");
      fprintf(outfile, "  OUTPUT=`./TESTjmdc $port %02x3f%02x3f 47 %hx | grep rxdone`\n", TdrNum, JinjPort[CrateNum], oldconfname);
      fprintf(outfile, "  usleep 100000\n");
      fprintf(outfile, "  echo Erase Flash on node %02x3f%02x3f: $OUTPUT\n", TdrNum, JinjPort[CrateNum]);
      fprintf(outfile, "  if [ \"$OUTPUT\" != \"rxdone = 0000\" ]\n   then\n    let \"CMDERRORS=$CMDERRORS+1\"\n  fi\n");
      fprintf(outfile, "  OUTPUT=`./TESTjmdc $port %02x3f%02x3f 7 | grep %hx`\n", TdrNum, JinjPort[CrateNum], oldconfname);
      fprintf(outfile, "  if [ -n \"$OUTPUT\" ]\n   then\n    echo DSP Code %hx Not Erased on node %02x3f%02x3f!!!\n    let \"ERSERRORS=$ERSERRORS+1\"\n  fi\n", oldconfname, TdrNum, JinjPort[CrateNum]);
      fprintf(outfile, " else\n  echo DSP Code %hx not found on node %02x3f%02x3f, so no erasing is needed\nfi\n", oldconfname, TdrNum, JinjPort[CrateNum]);
    }
  }
  //Writing cycle...
  for (int CrateNum=CrateMin; CrateNum<=CrateMax; CrateNum++){
    for (int TdrNum=0; TdrNum<24; TdrNum++){
      if (CrateNum==0){
	if (TdrNum==12) tastype=1;
	else if (TdrNum==14) tastype=2;
	else if (TdrNum==15) tastype=2;
	else if (TdrNum==19) tastype=3;
	else if (TdrNum==2) tastype=3;
	else if (TdrNum==3) tastype=3;
	else if (TdrNum==6) tastype=4;
	else if (TdrNum==7) tastype=2;
	else tastype=0;
      }
      else if (CrateNum==1){
	if (TdrNum==8) tastype=2;
	else if (TdrNum==5) tastype=3;
	else if (TdrNum==9) tastype=2;
	else tastype=0;
      }
      else if (CrateNum==2){
	if (TdrNum==10) tastype=1;
	else if (TdrNum==2) tastype=2;
	else if (TdrNum==3) tastype=2;
	else if (TdrNum==11) tastype=1;
	else if (TdrNum==7) tastype=3;
	else tastype=0;
      }
      else if (CrateNum==3){
	if (TdrNum==5) tastype=2;
	else if (TdrNum==8) tastype=3;
	else if (TdrNum==9) tastype=3;
	else if (TdrNum==12) tastype=4;
	else tastype=0;
      }
      else if (CrateNum==4){
	//	if (TdrNum==4) tastype=1;
	if (TdrNum==4) tastype=0;
	else if (TdrNum==5) tastype=3;
	else if (TdrNum==9) tastype=2;
	else if (TdrNum==8) tastype=2;
	else if (TdrNum==17) tastype=2;
	else if (TdrNum==20) tastype=3;
	else if (TdrNum==21) tastype=3;
	//	else if (TdrNum==22) tastype=4;
	else if (TdrNum==22) tastype=0;
	else tastype=0;
      }
      else if (CrateNum==5){
	if (TdrNum==3) tastype=3;
	else if (TdrNum==7) tastype=2;
	else if (TdrNum==2) tastype=3;
	else tastype=0;
      }
      else if (CrateNum==6){
	//	if (TdrNum==1) tastype=4;
	if (TdrNum==1) tastype=0;
	//	else if (TdrNum==0) tastype=4;
	else if (TdrNum==0) tastype=0;
	else if (TdrNum==5) tastype=2;
	else if (TdrNum==8) tastype=3;
	else if (TdrNum==9) tastype=3;
	else tastype=0;
      }
      else if (CrateNum==7){
	if (TdrNum==2) tastype=2;
	else if (TdrNum==3) tastype=2;
	else if (TdrNum==7) tastype=3;
	//	else if (TdrNum==22) tastype=1;
	else if (TdrNum==22) tastype=0;
	else tastype=0;
      }
      hwid=CrateNum*100+TdrNum;
      //below check of the windows setted before...
      if (hwid==210) tastypecheck=1;
      else if (hwid==108) tastypecheck=2;
      else if (hwid==105) tastypecheck=3;
      else if (hwid==109) tastypecheck=2;
      else if (hwid==503) tastypecheck=3;
      else if (hwid==507) tastypecheck=2;
      else if (hwid==502) tastypecheck=3;
      //      else if (hwid==600) tastypecheck=4;
      else if (hwid==600) tastypecheck=0;
      //---------------------------------      
      else if (hwid==211) tastypecheck=1;
      else if (hwid==202) tastypecheck=2;
      else if (hwid==207) tastypecheck=3;
      else if (hwid==203) tastypecheck=2;
      else if (hwid==609) tastypecheck=3;
      else if (hwid==605) tastypecheck=2;
      else if (hwid==608) tastypecheck=3;
      //      else if (hwid==601) tastypecheck=4;
      else if (hwid==601) tastypecheck=0;
      //---------------------------------      
      else if (hwid== 12) tastypecheck=1;
      else if (hwid== 14) tastypecheck=2;
      else if (hwid== 19) tastypecheck=3;
      else if (hwid== 15) tastypecheck=2;
      else if (hwid==421) tastypecheck=3;
      else if (hwid==417) tastypecheck=2;
      else if (hwid==420) tastypecheck=3;
      //      else if (hwid==422) tastypecheck=4;
      else if (hwid==422) tastypecheck=0;
      //---------------------------------      
      else if (hwid==  6) tastypecheck=4;
      else if (hwid==  2) tastypecheck=3;
      else if (hwid==  7) tastypecheck=2;
      else if (hwid==  3) tastypecheck=3;
      else if (hwid==409) tastypecheck=2;
      else if (hwid==405) tastypecheck=3;
      else if (hwid==408) tastypecheck=2;
      //      else if (hwid==404) tastypecheck=1;
      else if (hwid==404) tastypecheck=0;
      //---------------------------------      
      else if (hwid==312) tastypecheck=4;
      else if (hwid==308) tastypecheck=3;
      else if (hwid==305) tastypecheck=2;
      else if (hwid==309) tastypecheck=3;
      else if (hwid==703) tastypecheck=2;
      else if (hwid==707) tastypecheck=3;
      else if (hwid==702) tastypecheck=2;
      //      else if (hwid==722) tastypecheck=1;
      else if (hwid==722) tastypecheck=0;
      //---------------------------------      
      else tastypecheck=0;

      if (tastype!=tastypecheck) printf("Ladder %d has not the correct window!!!\n", hwid);

      int ext=0;
      if (hwid==000) ext=1;
      else if (hwid==001) ext=1;
      else if (hwid==004) ext=1;
      else if (hwid==006) ext=1;
      else if (hwid==010) ext=1;
      else if (hwid==011) ext=1;
      else if (hwid==012) ext=1;
      else if (hwid==013) ext=1;
      else if (hwid==016) ext=1;
      else if (hwid==102) ext=1;
      else if (hwid==110) ext=1;
      else if (hwid==111) ext=1;
      else if (hwid==112) ext=1;
      else if (hwid==113) ext=1;
      else if (hwid==116) ext=1;
      else if (hwid==200) ext=1;
      else if (hwid==201) ext=1;
      else if (hwid==204) ext=1;
      else if (hwid==206) ext=1;
      else if (hwid==210) ext=1;
      else if (hwid==211) ext=1;
      else if (hwid==212) ext=1;
      else if (hwid==213) ext=1;
      else if (hwid==216) ext=1;
      else if (hwid==302) ext=1;
      else if (hwid==310) ext=1;
      else if (hwid==311) ext=1;
      else if (hwid==312) ext=1;
      else if (hwid==313) ext=1;
      else if (hwid==316) ext=1;
      else if (hwid==400) ext=1;
      else if (hwid==401) ext=1;
      else if (hwid==404) ext=1;
      else if (hwid==406) ext=1;
      else if (hwid==410) ext=1;
      else if (hwid==411) ext=1;
      else if (hwid==418) ext=1;
      else if (hwid==422) ext=1;
      else if (hwid==423) ext=1;
      else if (hwid==500) ext=1;
      else if (hwid==501) ext=1;
      else if (hwid==508) ext=1;
      else if (hwid==518) ext=1;
      else if (hwid==522) ext=1;
      else if (hwid==523) ext=1;
      else if (hwid==600) ext=1;
      else if (hwid==601) ext=1;
      else if (hwid==604) ext=1;
      else if (hwid==606) ext=1;
      else if (hwid==610) ext=1;
      else if (hwid==611) ext=1;
      else if (hwid==618) ext=1;
      else if (hwid==622) ext=1;
      else if (hwid==623) ext=1;
      else if (hwid==700) ext=1;
      else if (hwid==701) ext=1;
      else if (hwid==708) ext=1;
      else if (hwid==718) ext=1;
      else if (hwid==722) ext=1;
      else if (hwid==723) ext=1;
      
      if (ext==0) fprintf(outfile, "%s%d%s%02x3f%02x3f%s", sfirst, tastype, ssecond, TdrNum, JinjPort[CrateNum], sthird);
      if (ext==1) fprintf(outfile, "%sextpl_%d%s%02x3f%02x3f%s", sfirst, tastype, ssecond, TdrNum, JinjPort[CrateNum], sthird);
      fprintf(outfile, "usleep 100000\n");
      fprintf(outfile, "echo Write Flash on node %02x3f%02x3f: $OUTPUT\n", TdrNum, JinjPort[CrateNum]);
      fprintf(outfile, "if [ \"$OUTPUT\" != \"rxdone = 0000\" ]; then let \"CMDERRORS=$CMDERRORS+1\"; fi\n");
    }
  }
  //Checking cycle...
  for (int CrateNum=CrateMin; CrateNum<=CrateMax; CrateNum++){
    for (int TdrNum=0; TdrNum<24; TdrNum++){
      fprintf(outfile, "usleep 100000\n");
      fprintf(outfile, "OUTPUT=`./TESTjmdc $port %02x3f%02x3f 7 | grep %hx`\n", TdrNum, JinjPort[CrateNum], newconfname);
      fprintf(outfile, "if [ -z \"$OUTPUT\" ]; then echo DSP Code %hx Not Loaded on node %02x3f%02x3f!!!; let \"WRTERRORS=$WRTERRORS+1\"; fi\n", newconfname, TdrNum, JinjPort[CrateNum]);
    }
  }

  fprintf(outfile, "echo There were $CMDERRORS erros in sending command!!!!\n");
  fprintf(outfile, "echo There were $ERSERRORS erros in erasing DSP files!!!!\n");
  fprintf(outfile, "echo There were $WRTERRORS erros in writing DSP files!!!!\n");

  printf("The script LoadConfs was created ");
  printf("using this scheme:\n");
  printf("-----------------------------------------------------------------------\n");
  printf("Ladder type	Channel ranges\n");
  printf("0      		None\n");
  printf("1		80-230, 640-680, 970-1020\n");
  printf("2		80-230, 720-830\n");
  printf("3		400-550, 720-830\n");
  printf("4		400-550, 640-680, 970-1020\n");
  printf("\n");
  printf("N.B.: Sonia dice 400-560!\n");
  printf("\n");
  printf("-------------------------------------------------------------  Philipp:\n");
  printf("hdw = 210   Swin=  80-110, 150-220    Kwin= 640-700, 960-1023     1\n");
  printf("hdw = 108   Swin=  80-225             Kwin= 720-830               2\n");
  printf("hdw = 105   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 109   Swin=  80-230             Kwin= 720-774, 776-830      2\n");
  printf("hdw = 503   Swin= 400-560             Kwin= 720-774, 776-830      3\n");
  printf("hdw = 507   Swin=  80-230             Kwin= 720-774, 776-830      2\n");
  printf("hdw = 502   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 600   Swin= 400-560             Kwin= ladder problems?      4 BUT SET TO 0 (AMS02P)\n");
  printf("-------------------------------------------------------------\n");
  printf("hdw = 211   Swin=  80-220             Kwin= 640-690, 970-1015     1\n");
  printf("hdw = 202   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 207   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 203   Swin=  80-230             Kwin= 720-774, 776-830      2\n");
  printf("hdw = 609   Swin= 400-560             Kwin= 720-774, 776-830      3\n");
  printf("hdw = 605   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 608   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 601   Swin= 400-560             Kwin= 640-700, 960-1023     4 BUT SET TO 0 (AMS02P)\n");
  printf("-------------------------------------------------------------\n");
  printf("hdw = 012   Swin= 100-150, 160-220    Kwin= 640-680, 970-1000     1\n");
  printf("hdw = 014   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 019   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 015   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 421   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 417   Swin=  80-230             Kwin= 720-772, 774-830      2\n");
  printf("hdw = 420   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 422   Swin= 400-560             Kwin= 640-700, 970-1023     4 BUT SET TO 0 (AMS02P)\n");
  printf("-------------------------------------------------------------\n");
  printf("hdw = 006   Swin= 400-560             Kwin= 640-700, 2nd missing  4\n");
  printf("hdw = 002   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 007   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 003   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 409   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 405   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 408   Swin=  80-230             Kwin= 720-774, 776-830      2\n");
  printf("hdw = 404   Swin=  80-200             Kwin= 640-690, 970-1023     1 BUT SET TO 0 (AMS02P)\n");
  printf("-------------------------------------------------------------\n");
  printf("hdw = 312   Swin= 400-560             Kwin= 640-700, 970-1023     4\n");
  printf("hdw = 308   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 305   Swin=  80-240             Kwin= 720-830               2\n");
  printf("hdw = 309   Swin= ladder problems?    Kwin= ladder problems?      3\n");
  printf("hdw = 703   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 707   Swin= 400-560             Kwin= 720-830               3\n");
  printf("hdw = 702   Swin=  80-230             Kwin= 720-830               2\n");
  printf("hdw = 722   Swin=  80-230             Kwin= 640-700, 960-1023     1 BUT SET TO 0 (AMS02P)\n");
  printf("-------------------------------------------------------------\n");
  printf("\n");


  printf("The script LoadConfs was created...\n");

  return 0;
}
