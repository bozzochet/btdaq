/********************************************************************/
/* Routine to create a TTree from Binary data file                  */
/*	     Fabio,    May 2014				    */
/*  May be compiled within root	: gSystem->CompileMacro("BinDataToRootTree.C") */
/********************************************************************/
/*
Version V12 of labiew DAQ program MainWithSpill_V13
 */

/* 
gSystem->CompileMacro("BinDataToRootTree_V13.C")
BinDataToRootTree("Path_to_data_file")

*/


#include <TFile.h>
#include <TTree.h>
#include <TRegexp.h>
#include <TString.h>
#include "stdio.h"
#include <TTimeStamp.h>

#include <iostream>
#include <fstream>
#include <stdlib.h>

using namespace std;
/* Defintion oh headers */
#define HEADER_START_ID  0x01
#define HEADER_END_ID  0xFF
#define V785_ID 0x03
#define V792_ID 0x05
#define V560_ID 0x07
#define V775_ID 0x09

#define SPILL_ID    0xFE
#define SPILL_END_ID    0xFC
#define EVENT_START_ID    0x02
#define EVENT_END_ID    0xFD

void BinDataToRootTree(const char* filename, const Int_t NNV785=0, const Int_t NNV792=0, const Int_t NNV560=0, const Int_t NNV775=0)
/*
NNV785 = numero di moduli inseriti durante l'acquizione. E' fondamentale che questo numero sia fornito se si analizza solo uno spill. 
Se si inserisce 0 nessun modulo verrà decodificato
NNV792 = numero di moduli inseriti durante l'acquizione. E' fondamentale che questo numero sia fornito se si analizza solo uno spill. 
Se si inserisce 0 nessun modulo verrà decodificato
NNV560 = numero di moduli inseriti durante l'acquizione. E' fondamentale che questo numero sia fornito se si analizza solo uno spill. 
NNV775 = numero di moduli inseriti durante l'acquizione. E' fondamentale che questo numero sia fornito se si analizza solo uno spill.
Se si inserisce 0 nessun modulo verrà decodificato
*/


{
  /* Variables for Decoding Binary File*/
 
  //Int_t 
  Int_t  aword[1];
  Int_t ModID;
  Int_t header[10], data[1000];
  Int_t aheader;// evtsyze=0, spill=0, evtspill=0;

  //  Int_t ssdid=0, qch=0, qval=0, nbch=0;
  Int_t version, configuration;
  Int_t year, month, day, hh,mm,ss,ns;
  
  Int_t HeaderSizeInByte=0;
  Int_t TotEvt=0;
  
  Int_t SpillNumber=0,LenghtOfSpillInByte=0,NumberOfEvtInSpill=0,LenghtOfEvtInByte=0,SpillDuration=0;
  Int_t evtnum=0;
  Int_t evtinspill=0;
  Int_t NV785=NNV785;
  Int_t NV792=NNV792;
  Int_t NV560=NNV560;
  Int_t NV775=NNV775;
  Int_t V785_Number=0, V792_Number=0,V560_Number=0 ,V775_Number=0 ;
  Int_t V785_ChBit=0, V792_ChBit=0, V775_ChBit=0;
  Int_t V775_TimeRes=0, V775_PsTimeRes=0;
  Int_t V785_EvtCnt=0, V792_EvtCnt=0,V775_EvtCnt=0 ;  
  Int_t V785_ConvCh=0, V785_Val=0, V785_Ch=0;
  Int_t V792_ConvCh=0, V792_Val=0, V792_Ch=0;
  Int_t V775_ConvCh=0, V775_Val=0, V775_Ch=0;
  Int_t  V560_Ch=0, V560_StatusReg=0;
  Int_t V560_Ch_MSB[8]={6,4,2,0,14,12,10,8};
  ULong_t V560_Val=0;
  Int_t LenghtOfModDataInByte=0;
  Int_t V2718Counts=0;

  TTimeStamp *RunStart = new TTimeStamp();
  UInt_t RunStart_Date=0;
  UInt_t RunStart_Time=0;
  UInt_t RunStart_nsec=0;

  TTimeStamp *RunStop = new TTimeStamp();
  UInt_t RunStop_Date=0;
  UInt_t RunStop_Time=0;
  UInt_t RunStop_nsec=0;
 
  TTimeStamp *SpillTime = new TTimeStamp();
  UInt_t Spill_Date=0;
  UInt_t Spill_Time=0;
  UInt_t Spill_nsec=0;



  FILE *fp;
  TString sfilename(filename);
 


  /* Check Endian type */
int num = 1;
if(*(char *)&num == 1)
{
printf("\nLittle-Endian\n");
}
else
{
printf("Big-Endian\n");
}

/* Opening Binary File for reading the number of VME modules*/
 fp = fopen(filename, "r");
 printf("First file opening to readout the number of modules \n");
 /* READ ONLY HEADER */
 fread(aword,4,1,fp);
 aheader = (aword[0]>>24)&0xFF;
 if (aheader == HEADER_START_ID){
   printf("ALL THE RUN \n");
   // Event Header : get event header size from 1st line        
   HeaderSizeInByte=aword[0]&0x00FFFFFF;
   // read 4 more words to get more information about the event 
   fread(header,4,4,fp);
   
   // read all the last  header lines (module informations)
   fread(header,5,(HeaderSizeInByte-5*4)/4,fp);
   for (int i=0; i<(HeaderSizeInByte-5*4)/4;i++){		
     ModID=(header[i]>>16)&0xFF;
     switch(ModID){
     case V785_ID:		 
       NV785 = (header[i])&0xFFF;
       printf("Number of V785 modules %i \n",NV785 );
       break;
     case V792_ID:		  
       NV792 = (header[i])&0xFFF;
       printf("Number of V792 modules %i \n",NV792 );
       break;
     case V560_ID:		  
       NV560 = (header[i])&0xFFF;
       printf("Number of V560 modules %i \n",NV560 );
       break;
    case V775_ID:		  
       NV775 = (header[i])&0xFFF;
       printf("Number of V775 modules %i \n",NV775 );
       break;
     default:
       break;
     }
   }
 }
 else{
   printf("JUST LAST SPILL \n");
   printf("Number of V785 modules %i \n",NV785 );
   printf("Number of V792 modules %i \n",NV792 );
   printf("Number of V775 modules %i \n",NV775 );
 }
fclose(fp);

/* Open ROOT file and define ROOT Tree */
// create File
 char title[100]; 
 TRegexp dat(".dat"); // modificare in base alla estensione file di output 
 TRegexp cal(".cal"); // modificare in base alla estensione file di output
 if (sfilename.Contains(dat)){
   cout << "data file" << endl;
   sfilename(dat) = ".root";
 }
 else if (sfilename.Contains(cal)){
   cout << "calibration file" << endl;
   sfilename(cal) = ".root";
 }
 else{
   cout << "bad file extansion .... exit" << endl;
   exit(0);
 }

 
 sprintf(title,sfilename);
 TFile *Rootfile = new TFile(title,"RECREATE");  

 /*
   Define the tree
  */
 // create TTree AnaEvent 
 // TTree *Ancillary = new TTree("AncillaryEvent","AncillaryEvent");  
 TTree *AncillaryEvent = new TTree("AncillaryEvent","AncillaryEvent");  
 AncillaryEvent->Branch("evtnum",&evtnum,"evtnum/I");
 AncillaryEvent->Branch("LenghtOfEvtInByte",&LenghtOfEvtInByte,"LenghtOfEvtInByte/I");
 AncillaryEvent->Branch("SpillNumber",&SpillNumber,"SpillNumber/I");
 AncillaryEvent->Branch("evtinspill",&evtinspill,"evtinspill/I");

 Int_t V785[NV785][32];
 if(NV785>0){
   //Int_t V785[NV785+1][32];
   for(Int_t i=0; i<NV785; i++){
     for(Int_t j=0; j<32; j++)
       V785[i][j]=-1;
   }
   TString V785_var("V785[");
   V785_var+=NV785;
   V785_var+="][32]/I";
   AncillaryEvent->Branch("V785",&V785,V785_var);
 }
 Int_t V792[NV792][32];
 if(NV792>0){
   //Int_t V792[NV792+1][32];
   for(Int_t i=0; i<NV792; i++){
     for(Int_t j=0; j<32; j++)
       V792[i][j]=-1;
   }
   TString V792_var("V792[");
   V792_var+=NV792;
   V792_var+="][32]/I";
   AncillaryEvent->Branch("V792",&V792,V792_var);
 }
Int_t V775[NV775][32];
 if(NV775>0){
   //Int_t V775[NV775+1][32];
   for(Int_t i=0; i<NV775; i++){
     for(Int_t j=0; j<32; j++)
       V775[i][j]=-1;
   }
   TString V775_var("V775[");
   V775_var+=NV775;
   V775_var+="][32]/I";
   AncillaryEvent->Branch("V775",&V775,V775_var);
 }
 TTree *AncillarySpill = new TTree("AncillarySpill","AncillarySpill");  
 AncillarySpill->Branch("SpillNumber",&SpillNumber,"SpillNumber/I");
 AncillarySpill->Branch("NumberOfEvtInSpill",&NumberOfEvtInSpill,"NumberOfEvtInSpill/I");
 AncillarySpill->Branch("Spill_Date",&Spill_Date,"Spill_Date/i");
 AncillarySpill->Branch("Spill_Time",&Spill_Time,"Spill_Time/i");
 AncillarySpill->Branch("Spill_nsec",&Spill_nsec,"Spill_nsec/i");
 AncillarySpill->Branch("SpillDuration",&SpillDuration,"SpillDuration/i");
 AncillarySpill->Branch("V2718Counts",&V2718Counts,"V2718Counts/i");
 ULong_t V560[NV560][16];
 if(NV560>0){
   for(Int_t i=0; i<NV560; i++){
     for(Int_t j=0; j<16; j++)
       V560[i][j]=-1;
   }
   TString V560_var("V560[");
   V560_var+=NV560;
   V560_var+="][16]/I";
   AncillarySpill->Branch("V560",&V560,V560_var);
 }

 TTree *AncillaryRun = new TTree("AncillaryRun","AncillaryRun");  
 AncillaryRun->Branch("NV785",&NV785,"NV785/I");
 AncillaryRun->Branch("NV792",&NV792,"NV792/I");
 AncillaryRun->Branch("NV775",&NV775,"NV775/I");
 AncillaryRun->Branch("NV560",&NV560,"NV560/I");

 AncillaryRun->Branch("RunStart_Date",&RunStart_Date,"RunStart_Date/i");
 AncillaryRun->Branch("RunStart_Time",&RunStart_Time,"RunStart_Time/i");
 AncillaryRun->Branch("RunStart_nsec",&RunStart_nsec,"RunStart_nsec/i");
 
 AncillaryRun->Branch("RunStop_Date",&RunStop_Date,"RunStop_Date/i");
 AncillaryRun->Branch("RunStop_Time",&RunStop_Time,"RunStop_Time/i");
 AncillaryRun->Branch("RunStop_nsec",&RunStop_nsec,"RunStop_nsec/i");
 
 /* to read out use 
    class TTimeStamp
    void  Set(Int_t date, Int_t time, Int_t nsec, Bool_t isUTC, Int_t secOffset)
 */


/*Reopen to read evts*/
 fp = fopen(filename, "r");
 printf("Second file opening to readout data \n");
 
 
 while(!feof(fp)) 
   
   {
     /*read the first word and get its type by reading the header*/
     fread(aword,4,1,fp);
     aheader = (aword[0]>>24)&0xFF;
     //printf("aheader %x \n", aheader);
     switch(aheader)
       {
	 
       case HEADER_START_ID: 
	 // Event Header : get event header size from 1st line        
	 //printf("Event Header Start %x\n",aword[0]);
	 HeaderSizeInByte=aword[0]&0x00FFFFFF;
	 // read 4 more words to get more information about the event 
	 fread(header,4,4,fp);
	 
	 version = header[0]>>14&0x7;
	 configuration = header[0]>>7&0x3F;
	 
	 year=header[1]>>12&0xFFF;
	 day = header[1]>>4&0xFF;
	 month = header[1]&0xF;
	 hh=header[2]>>16&0xFF;
	 mm = header[2]>>8&0xFF;
	 ss = header[2]&0xFF;
	 ns =  (header[3]&0xFFFFFF)*1E6;
	 RunStart->Set(2000+year,month,day,hh,mm,ss,ns,kTRUE,0);
	 RunStart_Date = RunStart->GetDate();
	 RunStart_Time = RunStart->GetTime();
	 RunStart_nsec = RunStart->GetNanoSec();
	 // read all the last  header lines (module informations)
	 fread(header,4,(HeaderSizeInByte-5*4)/4,fp);
	 printf("RUN START \nDate %i Time %i NanoSec %i\n",RunStart_Date,RunStart_Time, RunStart_nsec    );
	 
	 //printf("HeaderSizeInByte %i \n Version %i \n Configuration %i \n Date %i/%i/%i\n Time %i:%i:%i \n",HeaderSizeInByte,version,configuration,day,month,year,hh,mm,ss );
	 //printf("Size %i EvtNb %i SpillNb %i SpillEvt %i\n", evtsize, evtnum, spill, evtspill);
	 break;
       case HEADER_END_ID:
	 //printf("Event Header Stop %x\n",aword[0]);	 
	 TotEvt=aword[0]&0x00FFFFFF;
	  // read 4 more words to get more information about the event 
	 // read 4 instead of 3 to avoid to read two times the  HEADER_END
	  fread(header,4,4,fp);
	  year=header[0]>>12&0xFFF;
	  day = header[0]>>4&0xFF;
	  month = header[0]&0xF;
	  hh=header[1]>>16&0xFF;
	  mm = header[1]>>8&0xFF;
	  ss = header[1]&0xFF; 
	  ns =  (header[2]&0xFFFFFF)*1E6;
	  RunStop->Set(2000+year,month,day,hh,mm,ss,ns,kTRUE,0);
	  RunStop_Date = RunStop->GetDate();
	  RunStop_Time = RunStop->GetTime();
	  RunStop_nsec = RunStop->GetNanoSec();
	 
	  printf("RUN STOP \nTotal Number of Events %i  \nDate %i/%i/%i\n Time %i:%i:%i  Date %i Time %i NanoSec %i \n",TotEvt,day,month,year,hh,mm,ss,RunStop_Date,RunStop_Time,RunStop_nsec  );
	   AncillaryRun->Fill();
	  break;
	case SPILL_ID: 
	  //printf("Spill Header %x\n",aword[0]);
	 
	  SpillNumber=aword[0]&0x00FFFFFF;
	  // read 7 more words to get more information about the spill
	  fread(header,4,7,fp);
	  LenghtOfSpillInByte = header[0]&0xFFFFFF;
	  NumberOfEvtInSpill =header[1]&0xFFFFFF;
	  SpillDuration = header[2]&0xFFFFFF;
	  year=header[3]>>12&0xFFF;
	  day = header[3]>>4&0xFF;
	  month = header[3]&0xF;
	  hh=header[4]>>16&0xFF;
	  mm = header[4]>>8&0xFF;
	  ss = header[4]&0xFF;
	  ns =  (header[5]&0xFFFFFF)*1E6;
	  V2718Counts = (header[6]&0xFFFFFF);
	  //printf("Spill Number %i,SpillSize %i \n EvtInSpill %i \n Date %i/%i/%i\n Time %i:%i:%i \n",SpillNumber,LenghtOfSpillInByte,NumberOfEvtInSpill,day,month,year,hh,mm,ss );

	  SpillTime->Set(2000+year,month,day,hh,mm,ss,ns,kTRUE,0);
	  Spill_Date = SpillTime->GetDate();
	  Spill_Time = SpillTime->GetTime();
	  Spill_nsec = SpillTime->GetNanoSec();
	  printf("SPILL\nSpill Number %i EvtInSpill %i SpillDuration %i \nDate %i Time %i NanoSec %i \n",SpillNumber, NumberOfEvtInSpill, SpillDuration, Spill_Date,Spill_Time,Spill_nsec  );
	    

	  
	  break;

       case SPILL_END_ID: 
	 AncillarySpill->Fill();
	 break;
	 
       case  EVENT_START_ID:
	 // read 1 more word to get information about the event
	 fread(header,4,1,fp);
	 evtinspill= aword[0]&0xFFFFFF;	 
	 LenghtOfEvtInByte = header[0]&0xFFFFFF;
	 //printf("Evt Number %i,Event in Spill %i, EventSize %i \n",evtnum,evtinspill,LenghtOfEvtInByte);

	 

	 break;
       case  EVENT_END_ID:
	 //printf("Event End \n");
	 AncillaryEvent->Fill();
	 //printf("Eevnt. %i\n", evtnum);

	 evtnum++;
	 break;
	 
	case V785_ID:
	  if(NV785>0){
	  // read 1 more word to get  information about V785
	  fread(header,4,1,fp);
	  V785_Number = aword[0]&0x7FFFFF;
	  V785_ChBit = (aword[0]>>23)&0x1;
	  LenghtOfModDataInByte = header[0]&0xFFFFFF;
	  //printf("V785 n.%i with ch bit %i ---LenghtOfModDataInByte %i   \n", V785_Number, V785_ChBit,LenghtOfModDataInByte );
	  fread(data,4,(LenghtOfModDataInByte-2*4)/4,fp);
	  V785_ConvCh=(data[0]>>8)&0x3F;
	  //printf("V785 n.%i Converted ch. %i \n",  V785_Number,V785_ConvCh);
	  for (int i=1; i<V785_ConvCh+1;i++){
	    V785_Val=data[i]&0xFFF ;
	    if(V785_ChBit==0){
	      V785_Ch=(data[i]>>17)&0xF;}
	    else{
	      V785_Ch=(data[i]>>16)&0x1F;}
	    //printf("V785 ch.%i value %i \n",V785_Ch,V785_Val );
	   
	      V785[V785_Number][V785_Ch]=V785_Val;
	  }
	  V785_EvtCnt = data[V785_ConvCh+1]&0XFFFFFF; 
	  //printf("V785 Event counter %i \n",V785_EvtCnt);
	  }
	  break;
	case V775_ID:
	  if(NV775>0){
	  // read 1 more word to get  information about V792
	  fread(header,4,1,fp);
	  V775_Number = aword[0]&0x3FF;
	  V775_ChBit = (aword[0]>>23)&0x1;
	  V775_TimeRes = (aword[0]>>14)&0xFF;
	  switch(V775_TimeRes)	
	    {
	    case 255:
	      V775_PsTimeRes = 35;
	      break;
	    case 200:
	      V775_PsTimeRes = 50;
	      break;
	    case 90:
	      V775_PsTimeRes = 100;
	      break;
	    case 60:
	      V775_PsTimeRes = 150;
	      break;
	    case 45:
	      V775_PsTimeRes = 200;
	      break;
	    case 35:
	      V775_PsTimeRes = 250;
	      break;
	    case 30:
	      V775_PsTimeRes = 300;
	      break;
	    }
	  //printf("V775_TimeRes %i %i   \n",V775_TimeRes,V775_PsTimeRes  );
	  LenghtOfModDataInByte = header[0]&0xFFFFFF;
	  
	  fread(data,4,(LenghtOfModDataInByte-2*4)/4,fp);
	  V775_ConvCh=(data[0]>>8)&0x3F;
	  //printf("V775_ConvCh %i \n", V775_ConvCh);
	  for (int i=1; i<V775_ConvCh+1;i++){
	    V775_Val=(data[i]&0xFFF);
	    // NOTA BENE La conversione diretta in psec basata sulla risoluzione del modulo non è affidabile. Va sempre fatta una calibrazione con ritardi noti
	    //*V775_PsTimeRes; //in pico seconds 
	    
	    //printf("V775_Val %i \n",V775_Val );
	    if(V775_ChBit==0){
	      V775_Ch=(data[i]>>17)&0xF;}
	    else{
	      V775_Ch=(data[i]>>16)&0x1F;}
	    if(V775_Ch==0){
	      printf("V775_Number %i, V775_ch %i, data %i, V775_Val %i, V775_PsTimeRes %i\n", V775_Number,V775_Ch, data[i]&0xFFF, V775_Val, V775_PsTimeRes);
	    }
	    V775[V775_Number][V775_Ch]=V775_Val;
	  }
	
	  V775_EvtCnt = data[V775_ConvCh+1]&0XFFFFFF; 
	  
	  }
	  break;
	case V792_ID:
	  if(NV792>0){
	  // read 1 more word to get  information about V792
	  fread(header,4,1,fp);
	  V792_Number = aword[0]&0x7FFFFF;
	  V792_ChBit = (aword[0]>>23)&0x1;
	  LenghtOfModDataInByte = header[0]&0xFFFFFF;
	  //printf("V792 n.%i with ch bit %i ---LenghtOfModDataInByte %i   \n", V792_Number, V792_ChBit,LenghtOfModDataInByte );
	  fread(data,4,(LenghtOfModDataInByte-2*4)/4,fp);
	  V792_ConvCh=(data[0]>>8)&0x3F;
	  //printf("V792 n.%i Converted ch. %i \n",  V792_Number,V792_ConvCh);
	  for (int i=1; i<V792_ConvCh+1;i++){
	    V792_Val=data[i]&0xFFF ;
	    if(V792_ChBit==0){
	      V792_Ch=(data[i]>>17)&0xF;}
	    else{
	      V792_Ch=(data[i]>>16)&0x1F;}
	    //printf("V792 ch.%i value %i \n",V792_Ch,V792_Val );
	    
	    V792[V792_Number][V792_Ch]=V792_Val;
	  }
	
	  V792_EvtCnt = data[V792_ConvCh+1]&0XFFFFFF; 
	  //printf("V792 Event counter %i \n",V792_EvtCnt);
	  }
	  break;
       case V560_ID:
	 if(NV560>0){
	 
	  V560_Number = aword[0]&0x7FFFFF;
	  
	  // read 2 more word to get  information about V560
	  fread(header,4,2,fp);
	  LenghtOfModDataInByte = header[0]&0xFFFFFF;
	  V560_StatusReg = header[1]&0xFF;
	  //printf("V560 n.%i LenghtOfModDataInByte %i Status register %i \n", V560_Number,LenghtOfModDataInByte,V560_StatusReg  );
	 }	
	 // read 16 more word to get V560 ch
	 fread(header,4,16,fp);
	 for(Int_t i=0; i<8; i++){
	   if(((V560_StatusReg>>i)&0x1) == 1 ){
	     V560_Ch=V560_Ch_MSB[i]+1;
	     V560_Val=(((long long)header[V560_Ch_MSB[i]])<<32)+header[V560_Ch_MSB[i]+1];
	     V560[V560_Number][V560_Ch] =  V560_Val;
	     //printf("V560[%i][%i] = %i \n",V560_Number,V560_Ch, V560_Val );

	    
	     V560_Ch=V560_Ch_MSB[i];
	     V560_Val = 0;
	     V560[V560_Number][V560_Ch] =  V560_Val;
	     //printf("V560[%i][%i] = %i \n",V560_Number,V560_Ch, V560_Val );
	   }
	   else{
	     V560_Ch=V560_Ch_MSB[i];
	     V560_Val=header[V560_Ch_MSB[i]];
	     V560[V560_Number][V560_Ch] =  V560_Val;
	     //printf("V560[%i][%i] = %i \n",V560_Number,V560_Ch, V560_Val ); 
	     
	     V560_Ch=V560_Ch_MSB[i]+1;
	     V560_Val=header[V560_Ch_MSB[i]+1];
	     V560[V560_Number][V560_Ch] =  V560_Val;
	     //printf("V560[%i][%i] = %i \n",V560_Number,V560_Ch, V560_Val );


	   }  
	 }


	 break;
 	 /* Unknown Header - shall not happen... */
         default:
	   // printf("Unknown Header... how did you do that !?\n");
	   break;
        }     

/*	 if ((evtnum+1)%1==0) cout<<evtnum+1<<endl;*/

   } 

/* Close the Binary data file when done */
fclose(fp);
/* close the root file */
Rootfile->cd();	  
Rootfile->Write();	  
Rootfile->Close();

  return;

}

int main (int argc, char *argv[]) {
	if (argc == 2)
		BinDataToRootTree(argv[1]);
	else
		printf("usage:\n%s <file>\n", argv[0]);
	return 0;
}

