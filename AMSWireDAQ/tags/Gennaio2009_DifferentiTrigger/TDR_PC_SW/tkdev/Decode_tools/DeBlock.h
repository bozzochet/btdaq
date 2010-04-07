#ifndef Deframe_h
#define Deframe_h
#include "Event.h"
#include "TrCalDB.h"
#include "TH2F.h"
#include "POutF.h"
#define MAXSIZE 100000
typedef  unsigned short int ushort;

class Block_ph{
protected:
  static char rrtypestring[2][10];
  static char rwtypestring[2][20];
public:
  static char JMDCcodes[512][40];
  Block_ph(){Clear();}
  void Clear(){bsize=0;node=0;type=0;rrflag=-1;rwflag=-1; nwords=0; status=0; tag=0;time=0;first_word=0;last_word=0;
  }
  int bsize;
  int node;
  int type;
  int rrflag;
  int rwflag;
  int nwords;
  int status;
  int tag;
  time_t time;
  int first_word;
  int last_word;
  void Print(){
    PRINTF("Size: %5d Type:  %d| %s| %s|  Node: %4d %s \n FirstW: %d  LastW: %d  ",
	   bsize,type,rrtypestring[rrflag],rwtypestring[rwflag],node,JMDCcodes[node],first_word,last_word);
    if(time) PRINTF("Status: %d Tag: %d Time: %s ",status,tag,ctime(&time));
    
  }
};


class JMDCBlock:public Block_ph{
public:
  JMDCBlock():Block_ph(){Clear();}
  void Clear();

  int toff;
  int runnum;
  int runtag;
  int eventnum;
  time_t event_time;
  int event_ftime;
  double event_fftime;
  int JinJMask;
  int JinJSlave[24];
  int JinJSlaveMask[24];
  int TofFlag1;
  void Print();
};


class SubBlock:public Block_ph{
  
public:
  SubBlock():Block_ph(){Clear();}
  void Clear(){ Block_ph::Clear();
  eventnum=0; FirstDword=0;  LastDword=0; nodestatus=0; CRC=0;}
  int eventnum;
  int FirstDword;
  int LastDword;
  int nodestatus;
  int CRC;
  void Print();
};




class DeBlock{
private:
  int readOneWord(ushort* size,FILE* rawfile,int byteswap=0 );  
  int debugIO;
  ushort bblock[MAXSIZE];
  ushort size;
  int written;
  int CNWords;
  int OnlyTestErrors;
public:
  Block_ph bh1;
  JMDCBlock JMDC;
  TH2F* hh1;
  ushort Block(int ii){ if(ii<bh1.bsize&&ii>=0) return bblock[ii]; else {/*printf("erroe\n");*/return 0;}}
  
  Event* ev;
  TrCalDB* caldb; 
  FILE *rawfile;
  FILE *outfile;
  int TDROff;
  int HeaderWords;
  int CRCwords;
  int block_size;
  int block_size2;
  int frag_size[24];
  int tdr_size[192];
  

  DeBlock();
  ~DeBlock();
  int GetCNWords(){return CNWords;}
  void SetCNWords(int cn) {CNWords=cn;}
  void Clear(); 
  void DecodeBlockHeader();
  void DecodeSubBlockHeader(SubBlock *bb,int offset=0);
  int  WriteTheBlock(int, int);
  int PrintToFFiring(ushort w0,ushort w1);
  void SetOnlyTestErrors(){OnlyTestErrors=1;}
  void UnSetOnlyTestErrors(){OnlyTestErrors=0;}
  int  readOneWord();
  size_t writeOneWord(ushort hh);

  int  ReadBlockSize();

  int  ReadOneBlock();
  int ScanBlocks(int pri=1);
  void ScanBlocksCal(int pri=1);
  void ScanBlocksSlow(int pri=1);
  void SetInputFile(FILE *fin){rawfile=fin;}
  int  DecodeJMDCSubBlock();
  int  DecodeDetSubBlock();
  int  DecodeOneCal(SubBlock *bb,int offset, int size, int pri=0);

  int DecodeJLV1(int offset, int size, int pri=0);

  int ReadOneJINF(int offset, int size, int pri=0);
  int ReadOneTDR(int offset,int size,int Jinfnum, int pri=0);
  static int TestBoardErrors(char * ll,ushort status,int pri=0);

};


#endif

