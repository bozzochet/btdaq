#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

void PrepareFCSTable();
unsigned short DoFCS(unsigned short data, unsigned short fcs);
int CreateScript();

unsigned short FCS_Table[256];

int newconfname=0x6001;//0x because the names of the files on flash are hexadecimal

int main(int argc, char ** argv) {
  int pri=0;

  if(argc<=8){
    printf("Usage: %s  <input_txt_file> <S1HighThresh> <S1LowThresh> <S2HighThresh> <S2LowThresh> <KHighThresh> <KLowThresh> <ConfName>\n",argv[0]);
    printf("Thresholds have to be passed in 'standard' way (1, 2, 3.5, ...)\n");
    printf(" It output 21 files configfile_YX.dat and 21 files configfile_extpl_YX.dat in ./dotdats/, TasType X=[0-5], TasColumn Y=[0-5]\n");
    printf("ConfName is the name of the configuration file to be crated and then to be uploaded\n");
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
  if(argc>8) sscanf(argv[8],"%hx", &newconfname);
	       
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
    for (int column=0; column<6; column++){
      for (int type=0; type<5; type++){
	
	// In the case type = 0 only the case column = 0 is meaningful...
	//	printf("Preliminary: type = %d, column = %d\n", type, column);
	if (type==0 || column==0) {
	  //	  printf("Before: type = %d, column = %d\n", type, column);
	  if (!(type==0 && column==0)) continue;
	  //	  printf("After: type = %d, column = %d\n", type, column);
	}
	
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
	  //    printf("%hd ", GSI[0]);
	  
	  sscanf(nome,"%hd", &GSI[1]);
	  nome = strtok (NULL, " ");
	  //    printf("%hd ", GSI[1]);
	  
	  sscanf(nome,"%hd", &GSI[2]);
	  nome = strtok (NULL, " ");
	  //    printf("%hd ", GSI[2]);
	  
	  datain[cnt]=GSI[0]<<12|GSI[1]<<8|GSI[2];
	  //    printf("%hx ", datain[cnt]);
	  
	  //Read the value
	  
	  sscanf(nome,"%hx", &datain[cnt+1]);
	  //    printf("%hx\n", datain[cnt+1]);
	  if (datain[cnt]==1) InnerS1HighThresh = datain[cnt+1];
	  if (datain[cnt]==2) InnerS1LowThresh = datain[cnt+1];
	  if (datain[cnt]==3) InnerS2HighThresh = datain[cnt+1];
	  if (datain[cnt]==4) InnerS2LowThresh = datain[cnt+1];
	  if (datain[cnt]==5) InnerKHighThresh = datain[cnt+1];
	  if (datain[cnt]==6) InnerKLowThresh = datain[cnt+1];
	  
	  //    printf("Old: %hx %hx\n", datain[cnt-2], datain[cnt-1]);
	  //    printf("Actual: %hx %hx\n", datain[cnt], datain[cnt+1]);
	  
	  cnt+=2;
	}
	
	unsigned short npar=cnt/2;
	
	//  printf("cnt=%d\n",cnt);
	
	fclose(infile);
	
	char filename[256];
	if (ext==0) sprintf(filename, "./dotdats/configfile_%d%d.dat", column, type);
	else if (ext==1) sprintf(filename, "./dotdats/configfile_extpl_%d%d.dat", column, type);
	
	outfile=fopen(filename,"wt");
	if (!outfile) {
	  printf("output file error\n");
	  return 1;
	}
	
	printf("New Config File (%s): the TAS Column parameter setted to %d and the TAS Type to %d!\n", filename, column, type);
	
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
	    int coltyp=0;
	    if (column!=0) coltyp=1<<(7+column);
	    coltyp=coltyp+type;
	    fprintf(outfile, "%04hx\n", coltyp);
	    if(pri) printf("%04hx\n",coltyp);
	    fcs=DoFCS(coltyp,fcs);
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
	
	if(pri) printf("%hx\n", fcs);
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
  int tascolumn=0;
  int tastypecheck=99;
  int tascolumncheck=99;
  int hwid=999;
  
  sprintf(sfirst, "rm -f ./file.dat\ncp -fv ./dotdats/configfile_");
  sprintf(ssecond, ".dat ./file.dat\nOUTPUT=`./TESTjmdc $port ");
  sprintf(sthird, " 45 | grep rxdone`\n");
  
  fprintf(outfile, "#!/bin/bash\n");
  fprintf(outfile, "\n");
  fprintf(outfile, "if [ $# -lt 2 ]\n then\n");
  fprintf(outfile, "  echo You must indicate the port of JMDC to be used [i.e. which JINJ and which side of JINF-T]\n");
  fprintf(outfile, "  echo \"You must indicate the name of the old config name to be erased (for example 6001)\"\n");
  fprintf(outfile, "  exit\n");
  fprintf(outfile, "fi\n");
  fprintf(outfile, "port=$1\n");
  fprintf(outfile, "oldconfname=$2\n");
  fprintf(outfile, "\n");

  //Erasing...
  fprintf(outfile, "./DeleteFlashFileOnAllTDRs.sh $port $oldconfname\n");
  fprintf(outfile, "usleep 100000\n");
  fprintf(outfile, "\n");
  fprintf(outfile, "CMDERRORS=0\n");
  fprintf(outfile, "WRTERRORS=0\n");
  fprintf(outfile, "ERSERRORS=0\n");
  fprintf(outfile, "\n");
  //Writing cycle...
  for (int CrateNum=CrateMin; CrateNum<=CrateMax; CrateNum++){
    for (int TdrNum=0; TdrNum<24; TdrNum++){
      if (CrateNum==0){
	if (TdrNum==12) { tastype=1; tascolumn=3; }
	else if (TdrNum==14) { tastype=2; tascolumn=3; }
	else if (TdrNum==15) { tastype=2; tascolumn=3; }
	else if (TdrNum==19) { tastype=3; tascolumn=3; }
	else if (TdrNum==2) { tastype=3; tascolumn=2; }
	else if (TdrNum==3) { tastype=3; tascolumn=2; }
	else if (TdrNum==6) { tastype=4; tascolumn=2; }
	else if (TdrNum==7) { tastype=2; tascolumn=2; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==1){
	if (TdrNum==8) { tastype=2; tascolumn=4; }
	else if (TdrNum==5) { tastype=3; tascolumn=4; }
	else if (TdrNum==9) { tastype=2; tascolumn=4; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==2){
	if (TdrNum==10) { tastype=1; tascolumn=4; }
	else if (TdrNum==2) { tastype=2; tascolumn=5; }
	else if (TdrNum==3) { tastype=2; tascolumn=5; }
	else if (TdrNum==11) { tastype=1; tascolumn=5; }
	else if (TdrNum==7) { tastype=3; tascolumn=5; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==3){
	if (TdrNum==5) { tastype=2; tascolumn=1; }
	else if (TdrNum==8) { tastype=3; tascolumn=1; }
	else if (TdrNum==9) { tastype=3; tascolumn=1; }
	else if (TdrNum==12) { tastype=4; tascolumn=1; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==4){
	//	if (TdrNum==4) { tastype=1; tascolumn=2; }
	if (TdrNum==4) { tastype=0; tascolumn=0; }
	else if (TdrNum==5) { tastype=3; tascolumn=2; }
	else if (TdrNum==9) { tastype=2; tascolumn=2; }
	else if (TdrNum==8) { tastype=2; tascolumn=2; }
	else if (TdrNum==17) { tastype=2; tascolumn=3; }
	else if (TdrNum==20) { tastype=3; tascolumn=3; }
	else if (TdrNum==21) { tastype=3; tascolumn=3; }
	//	else if (TdrNum==22) { tastype=4; tascolumn=3; }
	else if (TdrNum==22) { tastype=0; tascolumn=0; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==5){
	if (TdrNum==3) { tastype=3; tascolumn=4; }
	else if (TdrNum==7) { tastype=2; tascolumn=4; }
	else if (TdrNum==2) { tastype=3; tascolumn=4; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==6){
	//	if (TdrNum==1) { tastype=4; tascolumn=5; }
	if (TdrNum==1) { tastype=0; tascolumn=0; }
	//	else if (TdrNum==0) { tastype=4; tascolumn=4; }
	else if (TdrNum==0) { tastype=0; tascolumn=0; }
	else if (TdrNum==5) { tastype=2; tascolumn=5; }
	else if (TdrNum==8) { tastype=3; tascolumn=5; }
	else if (TdrNum==9) { tastype=3; tascolumn=5; }
	else { tastype=0; tascolumn=0; }
      }
      else if (CrateNum==7){
	if (TdrNum==2) { tastype=2; tascolumn=1; }
	else if (TdrNum==3) { tastype=2; tascolumn=1; }
	else if (TdrNum==7) { tastype=3; tascolumn=1; }
	//	else if (TdrNum==22) { tastype=1; tascolumn=1; }
	else if (TdrNum==22) { tastype=0; tascolumn=0; }
	else { tastype=0; tascolumn=0; }
      }
      hwid=CrateNum*100+TdrNum;
      //below check of the windows and columns setted before...
      if      (hwid==210) { tastypecheck=1; tascolumncheck=4; }
      else if (hwid==108) { tastypecheck=2; tascolumncheck=4; }
      else if (hwid==105) { tastypecheck=3; tascolumncheck=4; }
      else if (hwid==109) { tastypecheck=2; tascolumncheck=4; }
      else if (hwid==503) { tastypecheck=3; tascolumncheck=4; }
      else if (hwid==507) { tastypecheck=2; tascolumncheck=4; }
      else if (hwid==502) { tastypecheck=3; tascolumncheck=4; }
      //      else if (hwid==600) { tastypecheck=4; tascolumncheck=4; }
      else if (hwid==600) { tastypecheck=0; tascolumncheck=0; }
      //---------------------------------      
      else if (hwid==211) { tastypecheck=1; tascolumncheck=5; }
      else if (hwid==202) { tastypecheck=2; tascolumncheck=5; }
      else if (hwid==207) { tastypecheck=3; tascolumncheck=5; }
      else if (hwid==203) { tastypecheck=2; tascolumncheck=5; }
      else if (hwid==609) { tastypecheck=3; tascolumncheck=5; }
      else if (hwid==605) { tastypecheck=2; tascolumncheck=5; }
      else if (hwid==608) { tastypecheck=3; tascolumncheck=5; }
      //      else if (hwid==601) { tastypecheck=4; tascolumncheck=5; }
      else if (hwid==601) { tastypecheck=0; tascolumncheck=0; }
      //---------------------------------      
      else if (hwid== 12) { tastypecheck=1; tascolumncheck=3; }
      else if (hwid== 14) { tastypecheck=2; tascolumncheck=3; }
      else if (hwid== 19) { tastypecheck=3; tascolumncheck=3; }
      else if (hwid== 15) { tastypecheck=2; tascolumncheck=3; }
      else if (hwid==421) { tastypecheck=3; tascolumncheck=3; }
      else if (hwid==417) { tastypecheck=2; tascolumncheck=3; }
      else if (hwid==420) { tastypecheck=3; tascolumncheck=3; }
      //      else if (hwid==422) { tastypecheck=4; tascolumncheck=3; }
      else if (hwid==422) { tastypecheck=0; tascolumncheck=0; }
      //---------------------------------      
      else if (hwid==  6) { tastypecheck=4; tascolumncheck=2; }
      else if (hwid==  2) { tastypecheck=3; tascolumncheck=2; }
      else if (hwid==  7) { tastypecheck=2; tascolumncheck=2; }
      else if (hwid==  3) { tastypecheck=3; tascolumncheck=2; }
      else if (hwid==409) { tastypecheck=2; tascolumncheck=2; }
      else if (hwid==405) { tastypecheck=3; tascolumncheck=2; }
      else if (hwid==408) { tastypecheck=2; tascolumncheck=2; }
      //      else if (hwid==404) { tastypecheck=1; tascolumncheck=2; }
      else if (hwid==404) { tastypecheck=0; tascolumncheck=0; }
      //---------------------------------      
      else if (hwid==312) { tastypecheck=4; tascolumncheck=1; }
      else if (hwid==308) { tastypecheck=3; tascolumncheck=1; }
      else if (hwid==305) { tastypecheck=2; tascolumncheck=1; }
      else if (hwid==309) { tastypecheck=3; tascolumncheck=1; }
      else if (hwid==703) { tastypecheck=2; tascolumncheck=1; }
      else if (hwid==707) { tastypecheck=3; tascolumncheck=1; }
      else if (hwid==702) { tastypecheck=2; tascolumncheck=1; }
      //      else if (hwid==722) { tastypecheck=1; tascolumncheck=1; }
      else if (hwid==722) { tastypecheck=0; tascolumncheck=0; }
      //---------------------------------      
      else { tastypecheck=0; tascolumncheck=0; }

      if (tastype!=tastypecheck) {
	printf("\n-------------------------------------------------------------------\n");
	printf("Ladder %d has not the correct window!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", hwid);
	printf("-------------------------------------------------------------------\n");
	sleep(3);
      }
      if (tascolumn!=tascolumncheck) {
	printf("\n-------------------------------------------------------------------\n");
	printf("Ladder %d has not the correct column!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n", hwid);
	printf("-------------------------------------------------------------------\n");
	sleep(3);
      }


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
      
      if (ext==0) fprintf(outfile, "%s%d%d%s%02x3f%02x3f%s", sfirst, tascolumn, tastype, ssecond, TdrNum, JinjPort[CrateNum], sthird);
      if (ext==1) fprintf(outfile, "%sextpl_%d%d%s%02x3f%02x3f%s", sfirst, tascolumn, tastype, ssecond, TdrNum, JinjPort[CrateNum], sthird);
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
  printf("-----------------------------------------------------------------------\n\n");
  printf("Hardware ID (HwId = iTDR + iCrate*100)\n\n");                
  printf("PAR8 (TYPE: bits 0 to 7)         Channel ranges\n");
  printf("-------------------------------------------------------\n");
  printf("           0                           None\n");
  printf("           1                 64-255, 640-703, 960-1023\n");
  printf("           2                 64-255, 704-831\n");
  printf("           3                384-575, 704-831\n");
  printf("           4                384-575, 640-703, 960-1023\n");
  printf("-------------------------------------------------------\n\n");
  printf("PAR8 (COL: bits 8 to 15)              Column\n");
  printf("-------------------------------------------------------\n");
  printf("         0                             None\n");
  printf("       0x100                             1\n");
  printf("       0x200                             2\n");
  printf("       0x400                             3\n");
  printf("       0x800                             4\n");
  printf("       0x1000                            5\n");
  printf("-------------------------------------------------------\n\n");
  printf("                                           L A D D E R S   M A P\n");
  printf("----------------------------------------------------------------------------------------------------------\n");
  printf("i =  0   Ladder  1 (L15AP099)    myTkId =  110     hdw = 312    TDR-3-02-B   TYPE 2  COL 1   PAR8 =  102\n");
  printf("i =  1   Ladder  2 (L12AJ051)    myTkId =  210     hdw = 308    TDR-3-04-A   TYPE 3  COL 1   PAR8 =  103\n");
  printf("i =  2   Ladder  3 (L12AH058)    myTkId =  310     hdw = 305    TDR-3-04-B   TYPE 3  COL 1   PAR8 =  103\n");
  printf("i =  3   Ladder  4 (L12AJ019)    myTkId =  410     hdw = 309    TDR-3-06-A   TYPE 4  COL 1   PAR8 =  104\n");
  printf("i =  4   Ladder  5 (L12AH036)    myTkId =  510     hdw = 703    TDR-7-01-A   TYPE 2  COL 1   PAR8 =  102\n");
  printf("i =  5   Ladder  6 (L12AJ052)    myTkId =  610     hdw = 707    TDR-7-01-B   TYPE 2  COL 1   PAR8 =  102\n");
  printf("i =  6   Ladder  7 (L12AH046)    myTkId =  710     hdw = 702    TDR-7-03-B   TYPE 3  COL 1   PAR8 =  103\n");
  printf("i =  7   Ladder  8 (L15AN118)    myTkId =  810     hdw = 722    TDR-7-11-A   TYPE 1  COL 1   PAR8 =  101 (BUT SET TO NO TAS)\n");
  printf("----------------------------------------------------------------------------------------------------------\n");
  printf("i =  8   Ladder  9 (L15AP109)    myTkId =  108     hdw = 6      TDR-0-01-A   TYPE 3  COL 2   PAR8 =  203\n");
  printf("i =  9   Ladder 10 (L12AJ021)    myTkId =  207     hdw = 2      TDR-0-01-B   TYPE 3  COL 2   PAR8 =  203\n");
  printf("i = 10   Ladder 11 (L12SH169)    myTkId =  307     hdw = 7      TDR-0-03-A   TYPE 4  COL 2   PAR8 =  204\n"); 
  printf("i = 11   Ladder 12 (L12AJ018)    myTkId =  407     hdw = 3      TDR-0-03-B   TYPE 2  COL 2   PAR8 =  202\n");
  printf("i = 12   Ladder 13 (L12AH035)    myTkId =  507     hdw = 409    TDR-4-02-A   TYPE 1  COL 2   PAR8 =  201\n"); 
  printf("i = 13   Ladder 14 (L12AJ023)    myTkId =  607     hdw = 405    TDR-4-02-B   TYPE 3  COL 2   PAR8 =  203\n");
  printf("i = 14   Ladder 15 (L12AH045)    myTkId =  707     hdw = 408    TDR-4-04-A   TYPE 2  COL 2   PAR8 =  202\n"); 
  printf("i = 15   Ladder 16 (L15AN117)    myTkId =  808     hdw = 404    TDR-4-04-B   TYPE 2  COL 2   PAR8 =  202 (BUT SET TO NO TAS)\n");
  printf("----------------------------------------------------------------------------------------------------------\n");
  printf("i = 16   Ladder 17 (L15AN113)    myTkId =  106     hdw = 12     TDR-0-06-A   TYPE 1  COL 3   PAR8 =  401\n");
  printf("i = 17   Ladder 18 (L12AH057)    myTkId =  206     hdw = 14     TDR-0-07-A   TYPE 2  COL 3   PAR8 =  402\n");     
  printf("i = 18   Ladder 19 (L12AJ033)    myTkId =  306     hdw = 19     TDR-0-07-B   TYPE 2  COL 3   PAR8 =  402\n");
  printf("i = 19   Ladder 20 (L12AH061)    myTkId =  406     hdw = 15     TDR-0-09-B   TYPE 3  COL 3   PAR8 =  403\n");
  printf("i = 20   Ladder 21 (L12AJ022)    myTkId =  506     hdw = 421    TDR-4-08-B   TYPE 2  COL 3   PAR8 =  402\n");
  printf("i = 21   Ladder 22 (L12AH037)    myTkId =  606     hdw = 417    TDR-4-10-A   TYPE 3  COL 3   PAR8 =  403\n");
  printf("i = 22   Ladder 23 (L12AJ047)    myTkId =  706     hdw = 420    TDR-4-10-B   TYPE 3  COL 3   PAR8 =  403\n");
  printf("i = 23   Ladder 24 (L15AP112)    myTkId =  806     hdw = 422    TDR-4-11-A   TYPE 4  COL 3   PAR8 =  404 (BUT SET TO NO TAS)\n");
  printf("----------------------------------------------------------------------------------------------------------\n");
  printf("i = 24   Ladder 25 (L15AN101)    myTkId = -107     hdw = 210    TDR-1-02-B   TYPE 3  COL 4   PAR8 =  803\n");
  printf("i = 25   Ladder 26 (L12AH024)    myTkId = -206     hdw = 108    TDR-1-04-A   TYPE 2  COL 4   PAR8 =  802\n");          
  printf("i = 26   Ladder 27 (L12AJ055)    myTkId = -306     hdw = 105    TDR-1-04-B   TYPE 2  COL 4   PAR8 =  802\n");
  printf("i = 27   Ladder 28 (L12AH060)    myTkId = -406     hdw = 109    TDR-2-05-A   TYPE 1  COL 4   PAR8 =  801\n");      
  printf("i = 28   Ladder 29 (L12AJ034)    myTkId = -506     hdw = 503    TDR-5-01-A   TYPE 3  COL 4   PAR8 =  803\n");
  printf("i = 29   Ladder 30 (L12AH030)    myTkId = -606     hdw = 507    TDR-5-01-B   TYPE 3  COL 4   PAR8 =  803\n");
  printf("i = 30   Ladder 31 (L12AJ020)    myTkId = -706     hdw = 502    TDR-5-03-B   TYPE 2  COL 4   PAR8 =  802\n");
  printf("i = 31   Ladder 32 (L15AP098)    myTkId = -807     hdw = 600    TDR-6-00-A   TYPE 4  COL 4   PAR8 =  804 (BUT SET TO NO TAS)\n");      
  printf("----------------------------------------------------------------------------------------------------------\n");
  printf("i = 32   Ladder 33 (L15AN100)    myTkId = -109     hdw = 211    TDR-2-01-A   TYPE 2  COL 5   PAR8 = 1002\n");
  printf("i = 33   Ladder 34 (L12AH029)    myTkId = -209     hdw = 202    TDR-2-01-B   TYPE 2  COL 5   PAR8 = 1002\n");       
  printf("i = 34   Ladder 35 (L12AJ054)    myTkId = -309     hdw = 207    TDR-2-03-B   TYPE 3  COL 5   PAR8 = 1003\n");
  printf("i = 35   Ladder 36 (L12AH059)    myTkId = -409     hdw = 203    TDR-2-05-B   TYPE 1  COL 5   PAR8 = 1001\n");
  printf("i = 36   Ladder 37 (L12AJ053)    myTkId = -509     hdw = 609    TDR-6-00-B   TYPE 4  COL 5   PAR8 = 1004\n");
  printf("i = 37   Ladder 38 (L12AH028)    myTkId = -609     hdw = 605    TDR-6-02-B   TYPE 2  COL 5   PAR8 = 1002\n");
  printf("i = 38   Ladder 39 (L12AJ056)    myTkId = -709     hdw = 608    TDR-6-04-A   TYPE 3  COL 5   PAR8 = 1003\n");
  printf("i = 39   Ladder 40 (L15AP110)    myTkId = -809     hdw = 601    TDR-6-04-B   TYPE 3  COL 5   PAR8 = 1003 (BUT SET TO NO TAS)\n");
  printf("----------------------------------------------------------------------------------------------------------\n\n");

  printf("The script LoadConfs was created...\n");

  return 0;
}
