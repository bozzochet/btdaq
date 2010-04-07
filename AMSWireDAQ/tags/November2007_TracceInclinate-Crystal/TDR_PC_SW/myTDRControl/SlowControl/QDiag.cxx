#include <stdio.h>
#include <iostream>
#include "QDiag.h"


//#define PRINTF(a...) { printf(a); if (fLogFile) fprintf(fLogFile,a); fflush(fLogFile); }

#define PRINTF(a...) { printf(a); }


QDiag::QDiag() {

  fQref=0;

}


QDiag::~QDiag() {

  Clear();

}


QDiag::QDiag(QEvent *qref) {

  fQref=qref;

}



void QDiag::Clear() {

  if (fQref) delete fQref;
  fQref=0;

}


void QDiag::SetLogFile(FILE *file) {

  fLogFile= file;

}


void QDiag::SetQRef(QEvent *qref) {

  Clear();
  fQref=qref;

}


int QDiag::Compare(QEvent *qevent) {
  
  QData *qdata=0, *qdataref=0;

  //  PRINTF("ref ndata=%d  ndata=%d\n",fQref->GetNdata(),qevent->GetNdata());

  if (fQref->GetNdata()!=qevent->GetNdata()) return -2; // not the same event size

  for (int i=0; i<fQref->GetNdata(); i++) {
    if (i==0) {
      qdata=qevent->GetQStart(); 
      qdataref=fQref->GetQStart();
    } else {
      qdata=qdata->GetNext();
      qdataref=qdataref->GetNext();
    }

    if (qdata==0 || qdataref==0) return -1; // problem with getting data

    int compar=qdata->CompareLrData(qdataref);
    
    if (compar!=0xf) {
      if (compar!=7) {
	PRINTF("compar=%d, commands are different\n",compar);
	return i+100;
      } else {
	if (qdata->IsTBS()) DiagTBS(qdataref,qdata);
	else if (qdata->IsTPSFE()) DiagTPSFE(qdataref,qdata);
	else if (qdata->IsS9011AT()) DiagS9011AT(qdataref,qdata);
	else return i;
      }
    }
  }
  
  return 0;
  
}


void QDiag::DiagTBS(QData *qdataref, QData *qdata) {

  int reg=qdata->GetRegister();
  int addr=qdata->GetLrAddr();

  unsigned short valref=qdataref->GetR1();
  unsigned short val=qdata->GetR1();
  unsigned short diff=val^valref;

  const char cstate[2][10]={"ON\0","OFF\0"};
  //  const char lrname[4][10]={"0_A\0","0_B\0","1_A\0","1_B\0"};
  const char lrname[4][10]={"0_A\0","1_A\0","0_B\0","1_B\0"};

  char cid[10];
  sprintf(cid,"TBS   %2d %s",addr,(qdata->GetActel()==kActelHot)?"Hot ":"Cold");

  if (reg==0) {

    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;


      if (bit==0)  PRINTF("%s, Brother control - expected %s, but is %s\n",cid,cstate[bref],cstate[bval]);
      if (bit==8)  PRINTF("%s,    LR 0 HOT     - expected %s, but is %s\n",cid,cstate[bref],cstate[bval]);
      if (bit==9)  PRINTF("%s,    LR 1 HOT     - expected %s, but is %s\n",cid,cstate[bref],cstate[bval]);
      if (bit==13) PRINTF("%s,  Brother TRIP   - expected %d, but is %d\n",cid,bref,bval);
      if (bit==14) PRINTF("%s,  Brother STATUS - expected %d, but is %d\n",cid,bref,bval);
      if (bit==15) PRINTF("%s,   Actel PWON    - expected %d, but is %d\n",cid,bref,bval);
    }
  } else 
    if (reg==1) {
      for (int bit=0; bit<16; bit++) {
	unsigned short test=(1<<bit) & diff;
	if (!test) continue; // everything is ok
	int bval=(val>>bit)&1;
	int bref=(valref>>bit)&1;
	
	if (bit<4)           PRINTF("%s, Local  control, LR %s - expected %s  , but is %s\n",cid,lrname[bit],cstate[bref],cstate[bval]);
	if (bit>3 && bit<8)  PRINTF("%s, Local  control, LR %s - expected %d V, but is %d V\n",cid, lrname[bit-4], (bref)?60:80, (bval)?60:80);
	if (bit>7 && bit<12) PRINTF("%s, Global control, LR %s - expected %s  , but is %s\n",cid,lrname[bit-8],cstate[bref],cstate[bval]);
	if (bit>11)          PRINTF("%s, Global control, LR %s - expected %d V, but is %d V\n",cid, lrname[bit-12], (bref)?60:80, (bval)?60:80);
      }
      
    } else 
      if (reg>=16 && reg<20) {
	// voltages
	int vdiff=qdata->GetR1()-qdataref->GetR1();
	if (abs(vdiff)>3) PRINTF("%s, LR %s - voltage fluctation: diff=%d\n",cid,lrname[reg-16], vdiff);
      } else 
	if (reg>19) {
	  // currents
	  int idiff=qdata->GetR1()-qdataref->GetR1();
	  if (abs(idiff)>3) PRINTF("%s, TDR %2d - current fluctation: diff=%d\n", cid, reg-20, idiff);
	} else
	  PRINTF("%s, register %d has changed\n", cid, reg);
  
}



void QDiag::DiagTPSFE(QData *qdataref, QData *qdata) {

  int reg=qdata->GetRegister();
  int addr=qdata->GetLrAddr();

  unsigned short valref=qdataref->GetR1();
  unsigned short val=qdata->GetR1();
  unsigned short diff=val^valref;

  const char cstate[2][10]={"ON \0","OFF\0"};
  const char cauto[2][10]={"UNSET\0"," SET \0"};
  char cid[10];
  sprintf(cid,"TPSFE %2d %s",addr,(qdata->GetActel()==kActelHot)?"Hot ":"Cold");

  if (reg==0) {

    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;

      if (bit<6)           PRINTF("%s, Loc ctrl 3.3V %2d - expected %s, but is %s\n",cid, bit, cstate[bref],cstate[bval]);
      if (bit==6)          PRINTF("%s, Auto mode 3.3V   - expected %s, but is %s\n",cid, cauto[bref],cauto[bval]);
      if (bit==7)          PRINTF("%s, Brother control  - expected %s, but is %s\n",cid, cstate[bref],cstate[bval]);
      if (bit>7 && bit<14) PRINTF("%s, Status 3.3V  %2d  - expected %s, but is %s\n",cid, bit-8, cstate[bref],cstate[bval]);
      if (bit==14)         PRINTF("%s,  Brother STATUS - expected %d, but is %d\n",cid,bref,bval);
      if (bit==15)         PRINTF("%s,   Actel PWON    - expected %d, but is %d\n",cid,bref,bval);
    }
  } else if (reg==1) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<6)           PRINTF("%s, Loc ctrl LRS %2d - expected %s, but is %s\n",cid, bit, cstate[bref],cstate[bval]);
      if (bit==6)          PRINTF("%s, Auto mode LRS   - expected %s, but is %s\n",cid, cauto[bref],cauto[bval]);
      if (bit>7 && bit<14) PRINTF("%s, Status LRS %2d   - expected %s, but is %s\n",cid, bit-8, cstate[bref],cstate[bval]);	
    }
  } else if (reg==2) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<6)           PRINTF("%s, Loc ctrl LRK %2d - expected %s, but is %s\n",cid, bit, cstate[bref],cstate[bval]);
      if (bit==6)          PRINTF("%s, Auto mode LRK   - expected %s, but is %s\n",cid, cauto[bref],cauto[bval]);
      if (bit>7 && bit<14) PRINTF("%s, Status LRK %2d   - expected %s, but is %s\n",cid, bit-8, cstate[bref],cstate[bval]);	
    }
  } else if (reg==3) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<6)           PRINTF("%s, Glob ctrl LRS %2d - expected %s, but is %s\n",cid, bit, cstate[bref],cstate[bval]);
      if (bit>7 && bit<14) PRINTF("%s, Glob ctrl LRK %2d - expected %s, but is %s\n",cid, bit-8, cstate[bref],cstate[bval]);	
    }
  } else if (reg==4) {
    // trip counters
    for (int j=0; j<4; j++) {
      int trip=(val>>(4*j)) & 0xf;
      if (trip==0) continue;
      PRINTF("%s, TDR %d - trip counter is %d\n",cid,j,trip);
    }
  } else if (reg==5) {
    // trip counters
    for (int j=0; j<4; j++) {
      int trip=(val>>(4*j)) & 0xf;
      if (trip==0) continue;
      if (j<2) { PRINTF("%s, TDR %d - trip counter is %d\n",cid,j+4,trip); }
      else { PRINTF("%s, LRS %d - trip counter is %d\n",cid,j-2,trip); }
    }
  } else if (reg==6) {
    // trip counters
    for (int j=0; j<4; j++) {
      int trip=(val>>(4*j)) & 0xf;
      if (trip==0) continue;
      PRINTF("%s, LRS %d - trip counter is %d\n",cid,j+2,trip);
    }
  } else if (reg==7) {
    // trip counters
    for (int j=0; j<4; j++) {
      int trip=(val>>(4*j)) & 0xf;
      if (trip==0) continue;
      PRINTF("%s, LRK %d - trip counter is %d\n",cid,j,trip);
    }
  } else if (reg==8) {
    // trip counters
    for (int j=0; j<3; j++) {
      int trip=(val>>(4*j)) & 0xf;
      if (trip==0) continue;
      if (j<2) { PRINTF("%s, LRK %d - trip counter is %d\n",cid,j+4,trip); }
      else { PRINTF("%s, Actel brother - trip counter is %d\n",cid,trip); }
    }
  } else { PRINTF("%s, register %d has changed\n", cid, reg); }
  
  
}


void QDiag::DiagS9011AT(QData *qdataref, QData *qdata) {

  int reg=qdata->GetRegister();
  int addr=qdata->GetLrAddr();

  unsigned short valref=qdataref->GetR1();
  unsigned short val=qdata->GetR1();
  unsigned short diff=val^valref;

  const char cstate[2][10]={"ON \0","OFF\0"};
  const char cauto[2][10]={"UNSET\0"," SET \0"};
  const char names[8][10]={"0_A\0","0_B\0","1_A\0","1_B\0","2_A\0","2_B\0","3_A\0","3_B\0"};
  char cid[10];
  sprintf(cid,"S9011AT  %s",(qdata->GetActel()==kActelHot)?"Hot ":"Cold");

  if (reg==0) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;

      if (bit==0)  PRINTF("%s, Brother control  - expected %s, but is %s\n",cid, cstate[bref],cstate[bval]);
      if (bit==13) PRINTF("%s,  Brother TRIP   - expected %d, but is %d\n",cid,bref,bval);
      if (bit==14) PRINTF("%s,  Brother STATUS - expected %d, but is %d\n",cid,bref,bval);
      if (bit==15) PRINTF("%s,   Actel PWON    - expected %d, but is %d\n",cid,bref,bval); 
    }
    
  } else if (reg==1) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<8) { PRINTF("%s, Loc ctrl S9051 %s - expected %s, but is %s\n",cid, names[bit], cstate[bref],cstate[bval]); }
      else      { PRINTF("%s, Status   S9051 %s - expected %s, but is %s\n",cid, names[bit-8], cstate[bref],cstate[bval]); }

    }
    
  } else if (reg==2) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<2)           PRINTF("%s, Local  ctrl S9053 %s - expected %s, but is %s\n",cid, names[bit], cstate[bref],cstate[bval]);
      if (bit>1 && bit<4)  PRINTF("%s, Local  ctrl S9053 %s - expected %s, but is %s\n",cid, names[bit], cstate[1-bref],cstate[1-bval]);
      if (bit>3 && bit<8)  PRINTF("%s, Local  ctrl S9055 %s - expected %s, but is %s\n",cid, names[bit-4], cstate[bref],cstate[bval]);
      if (bit>7 && bit<12) PRINTF("%s, Status   S9053 %s - expected %d, but is %d\n",cid, names[bit-8], bref,bval);
      if (bit>11)          PRINTF("%s, Status   S9055 %s - expected %d, but is %d\n",cid, names[bit-12], bref,bval);
    }
    
  } else if (reg==3) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<8)            PRINTF("%s, Global ctrl S9051 %s - expected %s, but is %s\n",cid, names[bit], cstate[bref],cstate[bval]);
      if (bit>7 && bit<10)  PRINTF("%s, Global ctrl S9053 %s - expected %s, but is %s\n",cid, names[bit-8], cstate[bref],cstate[bval]);
      if (bit>9 && bit<12)  PRINTF("%s, Global ctrl S9053 %s - expected %s, but is %s\n",cid, names[bit-8], cstate[1-bref],cstate[1-bval]);
      if (bit>11 && bit<16) PRINTF("%s, Global ctrl S9055 %s - expected %s, but is %s\n",cid, names[bit-12], cstate[bref],cstate[bval]);
    }
  } else if (reg==4) {
    for (int bit=0; bit<16; bit++) {
      unsigned short test=(1<<bit) & diff;
      if (!test) continue; // everything is ok
      int bval=(val>>bit)&1;
      int bref=(valref>>bit)&1;
      
      if (bit<8)            PRINTF("%s, trip bit of S9051 %s it set to %d\n",cid, names[bit], bval);
      if (bit>7 && bit<12)  PRINTF("%s, trip bit of S9053 %s it set to %d\n",cid, names[bit-8], bval);
      if (bit>11 && bit<16) PRINTF("%s, trip bit of S9055 %s it set to %d\n",cid, names[bit-12], bval);
      
    }
  } else PRINTF("%s, register %d has changed\n", cid, reg);

}
