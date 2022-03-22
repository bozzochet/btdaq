#include <stdio.h>
#include "QList.h"


QCommand::QCommand() {

  fCmd[0]=0;
  fCmd[1]=0;
  fCmd[2]=0;
  fCmd[3]=0;
  fCmd[4]=0;
  
  fNext=0;

}

QCommand::QCommand(unsigned short header, unsigned short rep, unsigned int freq, unsigned short w0, unsigned short w1) {

  //printf("%d %d %d %d %d\n", header, rep, freq, w0, w1);

  fCmd[0]=header;
  fCmd[1]=rep;
  fCmd[2]=freq;
  fCmd[3]=w0;
  fCmd[4]=w1;
  
  fNext=0;

  //printf("Qcommand object created\n");
  //for (int i=0; i<5; i++) printf ("%d ", (int)fCmd[i]);
  //printf("\n");
  //cout << fNext << endl;

}

void QCommand::SetLast(int i) {

  if (i) fCmd[0]|=(1<<2); // set last
  else fCmd[0]&=(~(1<<2));// unset last
}



QData::QData() {

  fType=0;
  fLast=0;
  fFirst=0;
  fAttribute=0;
  fExecuted=0;
  fFreq=0;
  fOffset=0;
  fW0=0;
  fW1=0;
  fTime=0;
  fR0=0;
  fR1=0;
  
  fNext=0;

}


int QData::Write(FILE *file) {

  if (file==0) return 1; // file pointer does not exist

  // data a written to a file previously opened by another process

  int ushsy=sizeof(unsigned short);
  int intsy=sizeof(int);
  int uinsy=sizeof(unsigned int);
  int a=0;

  a=fwrite(&fType,ushsy,1,file);
  a+=fwrite(&fOffset,ushsy,1,file);
  a+=fwrite(&fW0,ushsy,1,file);
  a+=fwrite(&fW1,ushsy,1,file);
  a+=fwrite(&fR0,ushsy,1,file);
  a+=fwrite(&fR1,ushsy,1,file);

  a+=fwrite(&fLast,intsy,1,file);
  a+=fwrite(&fFirst,intsy,1,file);
  a+=fwrite(&fAttribute,intsy,1,file);
  a+=fwrite(&fExecuted,intsy,1,file);

  a+=fwrite(&fFreq,uinsy,1,file);
  a+=fwrite(&fTime,uinsy,1,file);

  //  if write was successful, a==12, else write error

  return (a!=12);
}



int QData::Read(FILE *file) {

  if (file==0) return 1; // file pointer problem

    int ushsy=sizeof(unsigned short);
  int intsy=sizeof(int);
  int uinsy=sizeof(unsigned int);
  int a=0;

  a=fread(&fType,ushsy,1,file);
  a+=fread(&fOffset,ushsy,1,file);
  a+=fread(&fW0,ushsy,1,file);
  a+=fread(&fW1,ushsy,1,file);
  a+=fread(&fR0,ushsy,1,file);
  a+=fread(&fR1,ushsy,1,file);

  a+=fread(&fLast,intsy,1,file);
  a+=fread(&fFirst,intsy,1,file);
  a+=fread(&fAttribute,intsy,1,file);
  a+=fread(&fExecuted,intsy,1,file);

  a+=fread(&fFreq,uinsy,1,file);
  a+=fread(&fTime,uinsy,1,file);

  //  if write was successful, a==12, else write error

  return (a!=12);
}





int QData::TestLecroy() {

  if (fType) return -1; // not Lecroy
  if (fR0==0) return 2; // actel did not reply, probably
  unsigned short res=(fW0^fR0);
  if (res!=0x8000) return 3; // actel replied, but error in the format

  return 0;
}


unsigned short QData::GetLrAddr() {

  return ((fR0>>7) & 0x1f);
}



int QData::IsS9011AT() {

  int test=(fAttribute & 0x40) && (GetLrAddr()==3);

  return test;
}


int QData::GetHalf() { // only for S9011
  if (!IsS9011AT()) return -1;

  return (1 & (fAttribute>>5));
}


int QData::GetActel() {
  if (IsS9011AT()) return (1 & (fAttribute>>4));
  else return (1 & (fAttribute>>5));
}


int QData::IsTBS() {

  if (GetLrAddr()==5 && !(fAttribute & 0x10)) return 1; 
  if (GetLrAddr()==15 && (fAttribute & 0x10)) return 1; 

  return 0;
}


int QData::IsTPSFE() {

  if ((GetLrAddr()==4 || GetLrAddr()==6) && !(fAttribute & 0x10)) return 1; 
  if ((GetLrAddr()==14 || GetLrAddr()==16) && (fAttribute & 0x10)) return 1; 

  return 0;
}



int QData::GetRegister() {
  int r;
  r=fR0 & 7;
  r+=((fR0 & 0x30)>>1);      
  
  return r;  
}


int QData::CompareLrData(QData *qdata) {

  if (qdata==0) return -1;

  int test1=(fW0==qdata->GetW0());     // 1
  int test2=(fW1==qdata->GetW1())<<1;  // 2 
  int test3=(fR0==qdata->GetR0())<<2;  // 4
  int test4=(fR1==qdata->GetR1())<<3;  // 8

  return (test1+test2+test3+test4); // all ok should be 0xf, a data change should be 0x7

}



QEvent::QEvent() {
  fNdata=0;
  fQStart=0;
}



QEvent::~QEvent() {

  Clear();
 
}


void QEvent::Clear() {
  
  QData *data=fQStart, *next=0;

  if (data==0) return;

  do {
    next=data->GetNext();
    delete data;
    data=next;
  } while (data);
  
  fNdata=0;
  fQStart=0;
  
}




void QEvent::GetQListData(unsigned short *data, int length) {

  Clear();

  QData *qdata=0;

  int cnt=0;
  //  fNdata=0;

  while (cnt<length-1) {
    unsigned int freq=0, time=0;

    if (qdata==0) {
      qdata=new QData();
      fQStart=qdata;
      //cout << qdata << endl;
    }
    else {
      QData *next=new QData();
      qdata->SetNext(next);
      //      cout << qdata->GetNext() << endl;
      qdata=next;
    }
    
    qdata->SetType(data[cnt] & 3);
    qdata->SetLast(data[cnt] & (1<<2));
    qdata->SetFirst(data[cnt] & (1<<3));
    qdata->SetAttribute(data[cnt] & 0x70);
    qdata->SetExecuted(data[cnt] & (1<<7));
    freq=(data[cnt] & 0xff00)<<8;
    qdata->SetOffset(data[++cnt]);
    qdata->SetFreq(data[++cnt]+freq);
    
    qdata->SetW0(data[++cnt]);
    qdata->SetW1(data[++cnt]);
    time=data[++cnt];
    qdata->SetTime(time+(data[++cnt]<<16));
    qdata->SetR0(data[++cnt]);
    qdata->SetR1(data[++cnt]);
    //    printf("print offset=%d\n",qdata->GetOffset());
    cnt++;
    fNdata++;
    //cout << qdata<< endl;
  }
}


void QEvent::GetLrData(unsigned short *data, int length) {

  Clear();

  QData *qdata=0;

  int cnt=0;
  //  fNdata=0;

  unsigned int lrlen=data[cnt++];

  while (cnt<length-1) {
    unsigned int freq=0, time=0;

    if (qdata==0) {
      qdata=new QData();
      fQStart=qdata;
      qdata->SetFirst(1);
      //cout << qdata << endl;
    }
    else {
      QData *next=new QData();
      qdata->SetNext(next);
      //      cout << qdata->GetNext() << endl;
      qdata=next;
    }
    
    qdata->SetType(0);
    qdata->SetFreq(data[cnt++]);
    qdata->SetAttribute(data[cnt++]);

    qdata->SetOffset(fNdata);    
    qdata->SetExecuted(1);
    //freq=(data[cnt] & 0xff00)<<8;
    //qdata->SetOffset(data[++cnt]);

    
    qdata->SetW0(data[cnt++]);
    qdata->SetW1(data[cnt++]);
    //    time=data[++cnt];
    //qdata->SetTime(time+(data[++cnt]<<16));
    qdata->SetR0(data[cnt++]);
    qdata->SetR1(data[cnt++]);
    //    printf("print offset=%d\n",qdata->GetOffset());
    fNdata++;
    //cout << qdata<< endl;
  }
  qdata->SetLast(1);

  printf("fndata=%d, lrlen=%d\n",fNdata,lrlen);

}




int QEvent::Read(char *cfile) {

  FILE *file=fopen(cfile,"rb");
  if (file==0) return 1;

  Clear(); // just in case there was some old data inside
  int err=Read(file);

  fclose(file);
  
  return err;

}


int QEvent::Write(char *cfile) {

  FILE *file=fopen(cfile,"wb");
  if (file==0) return 1;

  int err=Write(file);

  fclose(file);
  
  return err;

}


int QEvent::Read(FILE *file) {
  // write to a file previously opened
  if (file==0) return 1;
  
  QData *qdata=0, *qprevious=0;
  
  int cnt=0;
  fNdata=0;

  while (!feof(file)) {

    if (qdata==0) {
      qdata=new QData();
      fQStart=qdata;
      //cout << qdata << endl;
    }
    else {
      QData *next=new QData();
      qdata->SetNext(next);
      //      cout << qdata->GetNext() << endl;
      qprevious=qdata;
      qdata=next;
    }
    
    int err=qdata->Read(file);

    if (err) {
      if (qprevious) delete qdata;
      else if (fQStart) delete (fQStart); 
      if (feof(file)) return 0;
      else return 2;

    } else fNdata++;


    //cout << qdata<< endl;
  } 

  return 0;
}


int QEvent::Write(FILE *file) {

  if (file==0) return 1;

  QData *qdata=fQStart;

  for(int i=0; i<fNdata; i++) {
    if (i>0) qdata=(qdata->GetNext());
    if (qdata==0) break;

    int err=qdata->Write(file);
    if (err) return 2;
  }

  return 0;
}


int QEvent::TestLecroy() {

  QData *qdata=fQStart;

  int nerr=0;

  for(int i=0; i<fNdata; i++) {
    if (i>0) qdata=(qdata->GetNext());
    if (qdata==0) break;

    if (qdata->TestLecroy()) {
      printf("Data position %d: Lecroy error %d\n", i, qdata->TestLecroy());
      nerr++;
    }

  }

  return nerr;
}


void QEvent::Print(int pos) {

  QData *qdata=fQStart;
  char typech[4][20]={"LeCroy\0", "AMSWire\0", "Memory op\0", "IO op\0"};

  //cout << qdata << endl;

  for(int i=0; i<fNdata; i++) {
    if (i>0) qdata=(qdata->GetNext());
    if (qdata==0) break;

    if (pos>-1 && pos!=i) continue; 

    unsigned int time=qdata->GetTime();
    char timech[20];
    time/=100;
    int hours=time/3600;
    int minutes=(time-(hours*3600))/60;
    int seconds=(time-hours*3600-minutes*60);
    sprintf(timech,"%02d:%02d:%02d",hours,minutes,seconds);

    if (qdata->IsFirst()) printf("first ");
    if (qdata->IsLast()) printf("last ");

    printf("command - offset 0x%04x - exec: %s\n",qdata->GetOffset(),(qdata->IsExecuted())?"yes":"no");
    printf("%s  freq=%6d  attr=0x%2x type=%s\n",timech, qdata->GetFreq(), qdata->GetAttribute(),typech[qdata->GetType()]);
    printf("W0=%04x W1=%04x R0=%04x R1=%04x\n",qdata->GetW0(),qdata->GetW1(),qdata->GetR0(),qdata->GetR1());

    if (qdata->TestLecroy()) {
      printf("Lecroy error %d\n", qdata->TestLecroy());
      continue;
    }

    char boardname[64];
    if (qdata->IsS9011AT()) sprintf(boardname,"S9011AT half %d", qdata->GetHalf());
    else if (qdata->IsTBS()) sprintf(boardname,"TBS_%02d", qdata->GetLrAddr());
    else if (qdata->IsTPSFE()) sprintf(boardname,"TPSFE_%02d", qdata->GetLrAddr());
    else sprintf(boardname,"unknown");

    printf("%s: Actel %s, register %d\n", boardname, (qdata->GetActel()==kActelHot)?"Hot":"Cold", qdata->GetRegister());

    printf("\n\n");

  }


}



// int QEvent::CompareEventData(QEvent *qref) {

//   QData *qdata=0, *qdataref=0;

//   for (int i=0; i<fNdata; i++) {
//     if (i==0) {
//       qdata=fQStart; 
//       qdataref=qref->GetQStart();
//     } else {
//       qdata=qdata->GetNext();
//       qdataref=qdataref->GetNext();
//     }

//     if (qdata==0 || qdataref==0) return -1; // problem with getting data

//     int compar=qdata->CompareLrData(qdataref);

//     if (compar!=0xf) {
//       if (compar!=7) {
// 	printf("compar=%d, commands are different\n",compar);
// 	return i;
//       } else {
// 	//printf("measurements are different, register %d\n", qdata->GetRegister());
// 	//if (qdata->IsTBS()) printf("TBS\n");
// 	//if (qdata->IsTPSFE()) printf("TPSFE\n");
// 	//if (qdata->IsS9011AT()) printf("S9011AT\n");
// 	// let's try to understand who is the guilty guy
// 	if (qdata->IsTBS()) {
// 	  int reg=qdata->GetRegister();
// 	  int addr=qdata->GetLrAddr();
// 	  if (reg>=16 && reg<20) {
// 	    // voltages
// 	    int vdiff=qdata->GetR1()-qdataref->GetR1();
// 	    if (abs(vdiff)>2) printf("TBS %d, register %d: voltage fluctation: diff=%d\n", addr,reg, vdiff);
// 	  } else if (reg>19) {
// 	    // currents
// 	    int idiff=qdata->GetR1()-qdataref->GetR1();
// 	    if (abs(idiff)>2) printf("TBS %d, register %d: current fluctation: diff=%d\n", addr, reg, idiff);
// 	  } else { 
// 	    printf("TBS %d, register %d ahs changed\n", addr, reg);
// 	    //return i;
// 	  }
// 	} else return i;
//       }
//     }
//   }
  
//   return 0;
 
// }



QData* QEvent::GetQData(int pos) {

  QData *qdata=fQStart;

  for (int i=0; i<fNdata; i++) {
    if (i==pos) return qdata;
    qdata=qdata->GetNext();
  }

  return 0;
}




QList::QList(unsigned short rep, unsigned short type) {

  //  fFreq=(0xffffff & freq);
  fRep=rep;
  fType=(3 & type);
  fCmdcnt=0;

  //  fHeader=((fFreq & 0xff0000)>>8);
  //  fHeader=fType;

  fQfirst=0;
  fQcmd=0;
}


QList::~QList() {
  //printf("deleting qlist\n");
  QCommand *qcmd=fQfirst, *qnext=0;

  if (qcmd==0) return;
  do {
    qnext=qcmd->GetNext();
    delete qcmd;
    qcmd=qnext;
  } while (qnext);

}


int QList::AddCommand(unsigned short bus, unsigned int freq, unsigned short command, unsigned short data) {

  unsigned short header=fType;
  header|=bus;
  header|=((freq & 0xff0000)>>8);
 
  if (!fCmdcnt) {
    header|=(1<<3); // first
    fQcmd=new QCommand(header, fRep, (freq & 0xffff), command, data);
    fQfirst=fQcmd;
  }
  else {
    fQcmd->SetLast(0);
    QCommand *next=new QCommand(header, fCmdcnt, (freq & 0xffff), command, data); // all the commands but the first, fFreq is the time delay after which the command is executed in 10ms units
    fQcmd->SetNext(next);
    fQcmd=next;
  }

  fCmdcnt++;

  fQcmd->SetLast(1);

  return fCmdcnt;
}


