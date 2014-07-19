#ifndef Deframe_h
#define Deframe_h

#define MAXSIZE 100000 
#define MAXSIZEF 50000 
typedef  unsigned short int ushort;

class Frame_head1{
public:
  Frame_head1(){Clear();}
  void Clear(){version=0;type=0;SHF=0;APID=0;seqflag=0;seqnum=0;packet_len=0;}
  ushort version;
  ushort type;
  ushort SHF;
  ushort APID;
  ushort seqflag;
  ushort seqnum;
  ushort packet_len;
  void Print(){printf("Size:  %5d SHF: %d SeqTyp: %d  SeqNo: %d APID: %4d ]\n",
		      packet_len,SHF,seqflag,seqnum,APID);}
};

class  Frame_head2{
public:
  Frame_head2(){Clear();}
  void Clear(){time=0;finetime=0;timeid=0;CHK=0;ZOE=0;packet_type=0;elemID=0;packetID1=0;packetID2=0;}
  time_t time;
  ushort finetime;
  ushort timeid;
  ushort CHK;
  ushort ZOE;
  ushort packet_type;
  ushort elemID;
  ushort packetID1;
  ushort packetID2;
  void Print(){ printf("[   CHK: %d PacketType: %d  %s",
		       CHK, packet_type,ctime(&time));}
};
class Frame_amsh{
public:
  Frame_amsh(){Clear();}
  void Clear(){fstatus=0;block_offset=0;}
  int fstatus;
  int block_offset;
  void Print(){printf("Fstatus: %d  offset: %d %hX\n",fstatus,block_offset,block_offset);}
} ;

class Block_ph{
protected:
  static char rrtypestring[2][10];
  static char rwtypestring[2][20];
  static char JMDCcodes[512][40];
public:
  Block_ph(){Clear();}
  void Clear(){bsize=0;node=0;type=0;rrflag=-1;rwflag=-1; nwords=0; status=0; tag=0;time=0;first_word=0;last_word=0;}
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
  void Print(int pri=0){
    printf("BLOCK Size: %5d Type:  %d| %s| %s|  FirstW: %d  LastW: %d  Node: %4d %s\n",
	   bsize,type,rrtypestring[rrflag],rwtypestring[rwflag],first_word,last_word,node,JMDCcodes[node]);
    if(pri&&time) printf("Status: %d Tag: %d Time: %s ",status,tag,ctime(&time));

  }
};
class JMDCBlock:public Block_ph{
public:
  JMDCBlock():Block_ph(){Clear();}
  void Clear(){ Block_ph::Clear();toff=0; 
    runnum=0;runtag=0; eventnum=0;event_time=0; event_ftime=0; }
  int toff;
  int runnum;
  int runtag;
  int eventnum;
  time_t event_time;
  int event_ftime;
  void Print(int ff=0){    //printf("------> New JMDC SubBlock \n");
    if(ff)Block_ph::Print();
    printf("JMDC Run: %d  Tag: %X Event: %d FineTime: %d Time: %s ",
	   runnum,runtag,eventnum,event_ftime,ctime(&event_time));
  }
};
class Deframe{
private:
  int readOneWord(ushort* size,FILE* rawfile,int byteswap=1 );  
  int debugIO;
  ushort size;
  int written;

public:
  ushort frame[MAXSIZEF];
  ushort bblock[MAXSIZE];
  Frame_head1 fh1;
  Frame_head2 fh2;
  Frame_amsh amsh;
  Block_ph bhead;
  JMDCBlock JMDC;
  FILE *rawfile;
  FILE *outfile;
  char outdir[255];

  int goodnode;
  int goodtype;
  int goodrun;
  int HeaderWords;
  int CRCwords;
  int block_size;
  int block_size2;
  void Clear(){if (frame) delete[] frame;}
  void SetInputFile(FILE *in){rawfile=in;}

  Deframe(FILE * fin,char *dout);
  ~Deframe();
  void DecodeFrameSecondaryHeader();
  void DecodeAMSHeader();  
  int WriteTheFrame(int start, int stop =9999999);


  int readOneWord();
  size_t writeOneWord(ushort hh,FILE* fil);

  
  int  GetBlockSize(int boffset);
  int DecodeJMDCSubBlock(int offset);

  int ReadBlockSize(int boffset);
  int BlockScan(int boffset);
  int ReadOneFrame();
  bool GoodBlock();

};


#endif

