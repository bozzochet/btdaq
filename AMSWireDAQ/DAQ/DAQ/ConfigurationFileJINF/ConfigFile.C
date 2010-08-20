#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void PrepareFCSTable();
unsigned short DoFCS(unsigned short data, unsigned short fcs);
int CreateScript(int crate, int jinj1, int jinj2, int oldconfname, int newconfname);
void ScriptCore(FILE *outfile, int OldConfName, int NewConfName, int JinjPort[], int Crate, int Jinj); 

unsigned short FCS_Table[256];

int main(int argc, char ** argv) {
  int pri=0;

  if(argc!=7){
    printf("Usage: %s <input_txt_file> <cratenum [0-7]> <jinj1> <jinj2> <oldconfname> <newconfname>\n",argv[0]);
    printf("cratenum is the number of the for which to create the conf file\n");
    printf("jinj1 is the jinj by which load the conf file into the JINF A (B)\n");
    printf("jinj2 is the jinj by which load the conf file into the JINF B (A)\n");
    printf("oldconfname is the name of the configuration file to erase\n");
    printf("newconfname is the name of the configuration file to upload");
    printf(" It output the file configfile_X.dat in ./dotdats/, x=[0-7]\n");
    return -1;
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
  
  int jinj1=atoi(argv[3]);
  int jinj2=atoi(argv[4]);

  int oldconfname=0;
  sscanf(argv[5],"%hx", &oldconfname);//hexdecimal!!!
  int newconfname=0;
  sscanf(argv[6],"%hx", &newconfname);//hexdecimal!!!

  int cf=atoi(argv[2]);
  
  char outfilename[256];
  sprintf(outfilename, "./dotdats/configfile_%d.dat", cf);
  
  outfile=fopen(outfilename,"wt");
  if (!outfile) {
    printf("output file error\n");
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
    
    //    printf("Old: %hx %hx\n", datain[cnt-2], datain[cnt-1]);
    //    printf("Actual: %hx %hx\n", datain[cnt], datain[cnt+1]);
    
    cnt+=2;
  } 
  
  unsigned short npar=cnt/2;
  
  //  printf("cnt=%d\n",cnt);
  
  fclose(infile);
  
  if (pri) printf("%04hx\n",newconfname);
  fprintf(outfile, "%04hx\n",newconfname);
  if (pri) printf("%04hx\n",0x4000+cnt+2);
  fprintf(outfile, "%04hx\n",0x4000+cnt+2);
  if (pri) printf("%04hx\n",0x9000);
  fprintf(outfile, "%04hx\n",0x9000);
    
  npar+=0x1000; //0x1000: for the tracker
  if (pri) printf("%04hx\n", npar);
  fprintf(outfile, "%04hx\n", npar);
  fcs=DoFCS(npar,fcs);
  
  for (int i=0; i<cnt; i++) {
    if (pri) printf("%04hx\n",datain[i]);
    fprintf(outfile, "%04hx\n",datain[i]);
    fcs=DoFCS(datain[i],fcs);
  }
  
  if (pri) printf("%hx\n",fcs);
  fprintf(outfile, "%hx\n",fcs);
  
  fclose(outfile);
  
  /*
    for (int i=0; i<1000; i++) {
    printf("type number in hex: ");
    fflush(stdout);
    scanf("%hx", &data);
    fcs=DoFCS(data,fcs);
    printf("fcs= %x\n", fcs);
    }
  */
  
  if (CreateScript(cf, jinj1, jinj2, oldconfname, newconfname)) return -1;

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

int CreateScript(int crate, int jinj1, int jinj2, int oldconfname, int newconfname){

  FILE *outfile=0;
  int JinjPort[8];
  
  char outname[256];
  sprintf(outname,"./LoadConfs_%d.sh", crate);
  outfile=fopen(outname,"wt");
  if (!outfile) {
    printf("output file %s error\n", outname);
    return -1;
  }
  
  for (int CrateNum=0; CrateNum<8; CrateNum++){
    if (CrateNum==3)
      JinjPort[CrateNum]=1;
    else if (CrateNum==2)
      JinjPort[CrateNum]=0;
    else if (CrateNum==7)
      JinjPort[CrateNum]=23;
    else if (CrateNum==6)
      JinjPort[CrateNum]=22;
    else if (CrateNum==0)
      JinjPort[CrateNum]=3;
    else if (CrateNum==1)
      JinjPort[CrateNum]=9;
    else if (CrateNum==4)
      JinjPort[CrateNum]=16;
    else if (CrateNum==5)
      JinjPort[CrateNum]=17;
  }
  
  fprintf(outfile, "#!/bin/bash\n");
  fprintf(outfile, "echo You will use JINJ %d and JINJ %d to load on JINFs A and on JINFs B\n", jinj1, jinj2);
  fprintf(outfile, "sleep 5\n");
  fprintf(outfile, "CMDERRORS=0\n");
  fprintf(outfile, "WRTERRORS=0\n");
  fprintf(outfile, "ERSERRORS=0\n");

  ScriptCore(outfile, oldconfname, newconfname, JinjPort, crate, jinj1);
  ScriptCore(outfile, oldconfname, newconfname, JinjPort, crate, jinj2);
  
  fprintf(outfile, "echo There were $CMDERRORS erros in sending command!!!!\n");
  fprintf(outfile, "echo There were $ERSERRORS erros in erasing DSP files!!!!\n");
  fprintf(outfile, "echo There were $WRTERRORS erros in writing DSP files!!!!\n");
  
  printf("The script %s was created...\n", outname);
  
  return 0;
}

void ScriptCore(FILE *outfile, int OldConfName, int NewConfName, int JinjPort[], int Crate, int Jinj){

  int CrateNum=Crate;
  int JmdcPort=Jinj;
  int newconfname=NewConfName;
  int oldconfname=OldConfName;
  
  fprintf(outfile, "OUTPUT=`./TESTjmdc %d %02x3f 7 | grep %hx`\n", JmdcPort, JinjPort[CrateNum], oldconfname);
  fprintf(outfile, "if [ -n \"$OUTPUT\" ]\n then\n");
  fprintf(outfile, "  OUTPUT=`./TESTjmdc %d %02x3f 47 %hx | grep rxdone`\n", JmdcPort, JinjPort[CrateNum], oldconfname);
  fprintf(outfile, "  usleep 200000\n"); 
  fprintf(outfile, "  echo Erase Flash on node %d %02x3f: $OUTPUT\n", JmdcPort, JinjPort[CrateNum]);
  fprintf(outfile, "  if [ \"$OUTPUT\" != \"rxdone = 0000\" ]\n   then\n    let \"CMDERRORS=$CMDERRORS+1\"\n  fi\n");
  fprintf(outfile, "  OUTPUT=`./TESTjmdc %d %02x3f 7 | grep %hx`\n", JmdcPort, JinjPort[CrateNum], oldconfname);
  fprintf(outfile, "  if [ -n \"$OUTPUT\" ]\n    then\n    echo DSP Code %hx Not Erased on node %d %02x3f!!!\n    let \"ERSERRORS=$ERSERRORS+1\"\n  fi\n", oldconfname, JmdcPort, JinjPort[CrateNum]);
  fprintf(outfile, " else\n  echo DSP Code %hx not found on node %d %02x3f, so no erasing is needed\nfi\n", oldconfname, JmdcPort, JinjPort[CrateNum]);
  
  fprintf(outfile, "rm -f ./file.dat\ncp -fv ./dotdats/configfile_%d.dat ./file.dat\n", CrateNum);
  fprintf(outfile, "OUTPUT=`./TESTjmdc %d %02x3f 45 | grep rxdone`\n", JmdcPort, JinjPort[CrateNum]);
  fprintf(outfile, "usleep 100000\n");
  fprintf(outfile, "echo Write Flash on node %d %02x3f: $OUTPUT\n", JmdcPort, JinjPort[CrateNum]);
  fprintf(outfile, "if [ \"$OUTPUT\" != \"rxdone = 0000\" ]\n then\n  let \"CMDERRORS=$CMDERRORS+1\"\nfi\n");
  fprintf(outfile, "OUTPUT=`./TESTjmdc %d %02x3f 7 | grep %hx`\n", JmdcPort, JinjPort[CrateNum], newconfname);
  fprintf(outfile, "if [ -z \"$OUTPUT\" ]\n then\n  echo DSP Code %hx Not Loaded on node %d %02x3f!!!\n  let \"WRTERRORS=$WRTERRORS+1\"\nfi\n", newconfname, JmdcPort, JinjPort[CrateNum]);
  
  return;
}
