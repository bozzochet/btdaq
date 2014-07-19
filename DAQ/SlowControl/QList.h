#ifndef QLIST_H 
#define QLIST_H

#include "SlowControlTypes.h"
#include <stdio.h>

class QCommand {

 private:
  unsigned short fCmd[5];
  QCommand *fNext;

 public:
  QCommand();
  QCommand(unsigned short header, unsigned short rep, unsigned int freq, unsigned short w0, unsigned short w1);
  void SetNext(QCommand *nextq) { fNext=nextq; }
  QCommand* GetNext() { return fNext; }
  unsigned short GetCommand(int i) { return fCmd[i]; }
  unsigned short GetLRBus() { return fCmd[0]&0x70; }
  unsigned short GetFreq() { return fCmd[2]; }
  unsigned short GetW0() { return fCmd[3]; }
  unsigned short GetW1() { return fCmd[4]; }
  void SetLast(int i);
};



class QList {

 private:
  //  unsigned int fFreq; // freq. in 10ms units
  unsigned short fRep; // number of repetition. 1<<15 is forever
  unsigned short fType; // 0=lecroy, 1=amsw, 2=memory operation, 3= io operation
  //unsigned short fHeader;
  int fCmdcnt; // command counter
  QCommand *fQfirst, *fQcmd;


 public:

  QList(unsigned short rep, unsigned short type);
  ~QList();
  int AddCommand(unsigned short bus, unsigned int freq, unsigned short command, unsigned short data);

  //unsigned int GetFrequency() { return fFreq; }
  unsigned short GetRepetition() { return fRep; }
  int IsForever() { return (fRep & (1<<15)) ; }

  int GetCmdcnt() { return fCmdcnt; }
  QCommand* GetQfirst() { return fQfirst; }

};



class QData {

 private:
  unsigned short fType, fOffset, fW0, fW1, fR0, fR1;
  int fLast, fFirst, fAttribute, fExecuted;
  unsigned int fFreq, fTime;

  QData* fNext;

 public:
  QData();
 
  void SetType(unsigned short type) { fType=type; }
  unsigned short GetType() { return fType; }
  void SetOffset(unsigned short offset) { fOffset=offset; }
  unsigned short GetOffset() { return fOffset; }
  void SetW0(unsigned short w0) { fW0=w0; }
  unsigned short GetW0() { return fW0; }
  void SetW1(unsigned short w1) { fW1=w1; }
  unsigned short GetW1() { return fW1; }
  void SetR0(unsigned short r0) { fR0=r0; }
  unsigned short GetR0() { return fR0; }
  void SetR1(unsigned short r1) { fR1=r1; }
  unsigned short GetR1() { return fR1; }

  void SetLast(int last) { fLast=last; }
  int IsLast() { return (fLast!=0); }
  void SetFirst(int first) { fFirst=first; }
  int IsFirst() { return (fFirst!=0); }
  void SetExecuted(int executed) { fExecuted=executed; }
  int IsExecuted() { return (fExecuted!=0); }
  void SetAttribute(int attribute) { fAttribute=attribute; }
  int GetAttribute() { return fAttribute; }

  void SetFreq(unsigned int freq) { fFreq=freq; }
  unsigned int GetFreq() { return fFreq; }
  void SetTime(unsigned int time) { fTime=time; }
  unsigned int GetTime() { return fTime; }

  void SetNext(QData *next) { fNext=next; }
  QData* GetNext() { return fNext; }

  int TestLecroy();
  unsigned short GetLrAddr();
  int IsS9011AT();
  int GetHalf();
  int GetActel();
  int IsTBS();
  int IsTPSFE();

  int GetRegister();
  int CompareLrData(QData *qdata);

  int Read(FILE *file);
  int Write(FILE *file);
};


class QEvent {

 private:
  int fNdata;
  QData *fQStart;
  
 public:
  QEvent();
  ~QEvent();
  int GetNdata() { return fNdata; }
  void GetQListData(unsigned short *data, int len);
  void GetLrData(unsigned short *data, int len);
  void Print(int pos=-1);
  QData* GetQStart() { return fQStart; }
  //  int CompareEventData(QEvent *qevent);
  QData* GetQData(int pos);
  void Clear();

  int Read(char *cfile);
  int Write(char *cfile);
  int Read(FILE *file);
  int Write(FILE *file);
  int TestLecroy();
};






#endif /* QLIST_H */
