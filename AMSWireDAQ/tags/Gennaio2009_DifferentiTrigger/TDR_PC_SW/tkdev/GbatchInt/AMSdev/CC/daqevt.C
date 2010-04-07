//  $Id: daqevt.C,v 1.4 2007/11/14 13:59:51 zuccon Exp $
#include <stdio.h>
#include "daqevt.h"
#include "event.h"
#include "dcards.h"
#include <sys/stat.h>
#include <sys/file.h>
#include <ctype.h>
#include "astring.h"
#include <fstream.h>
#ifndef __ALPHA__
using std::ostrstream;
using std::istrstream;
#endif




integer DAQEvent::_Buffer[50000];
integer DAQEvent::_BufferLock=0;
const integer lover=2;
uinteger      Time_1;

DAQEvent::~DAQEvent(){
  shrink();
}

void DAQEvent::shrink(){

  if(_pData && !_BufferOwner)UPool.udelete(_pData);
  if(_BufferOwner)_BufferLock=0;
  _pData=0;
  //_Length=0;
  _pcur=0;
  _Event=0;
  _Run=0;
  _RunType=0;
  _Time=0;
  _Checked=0;
  _BufferOwner=0;
}

DAQSubDet * DAQEvent::_pSD[nbtps]={0,0,0,0,0,0,0,0};
DAQBlockType * DAQEvent::_pBT[nbtps]={0,0,0,0,0,0,0,0};

void DAQEvent::addsubdetector(pid pgetid, pputdata pput, uinteger btype){
  DAQSubDet * p= new DAQSubDet(pgetid, pput);
  if(_pSD[btype]){
    DAQSubDet *paux=_pSD[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pSD[btype]=p;
}
void DAQEvent::addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget,uinteger btype){
  DAQBlockType * p= new DAQBlockType(pgmb,pgl,pget);
  if(_pBT[btype]){
    DAQBlockType *paux=_pBT[btype];
    while(paux->_next)paux=paux->_next;
    paux->_next=p; 
  }
  else _pBT[btype]=p;
  p->_maxbl=(p->_pgmb)();
  p->_plength=new integer[p->_maxbl];
  if(p->_plength == NULL){
    cerr<<"DAQEvent::addblock-F-Memory exhausted "<<p->_maxbl<<endl;  
    p->_maxbl=0;
    throw amsglobalerror();     
  }
}

#ifdef __AMS02DAQ__
const integer DAQEvent::_OffsetL=0;
#else
const integer DAQEvent::_OffsetL=1;
#endif
char ** DAQEvent::ifnam=0;
integer DAQEvent::InputFiles=0;
integer DAQEvent::KIFiles=0;
char * DAQEvent::ofnam=0;
fstream DAQEvent::fbin;
fstream DAQEvent::fbout;


void DAQEvent::setfile(const char *ifile){
  if(ifnam[0])delete [] ifnam[0];
  ifnam[0]=new char[strlen(ifile)+1];
  strcpy(ifnam[0],ifile);
  InputFiles=1;
  KIFiles=0;
}

const char * DAQEvent::getfile(){
  return ifnam[0];
}

void DAQEvent::setfiles(char *ifile, char *ofile){
  if(ifile){
    InputFiles=parser(ifile,ifnam);
    if(InputFiles)
      cout <<"DAQEvent::setfiles-I-"<<InputFiles<<" input files parsed"<<endl;
    else if(DAQCFFKEY.mode%10){
      ifnam=new char*[1];
      ifnam[0]=0;
      InputFiles=0;
      KIFiles=0;
      //cerr <<"DAQEvent::setfiles-F-No input files parsed"<<endl; 
    }
  }
  if(ofile){
    ofnam=new char[strlen(ofile)+1];
    strcpy(ofnam,ofile);
  }
}


void DAQEvent::buildDAQ(uinteger btype){
  DAQBlockType *fpl=_pBT[btype];
  if(fpl == NULL && btype){
    static int init=0;
    if(init++==0)cerr << "DAQEvent::build-S-NoSubdetectors in DAQ"<<endl;
    return;
  }
  integer ntot=0;
  while(fpl){
    for(int i=0;i<fpl->_maxbl;i++){
      *(fpl->_plength+i)=(fpl->_pgetlength)(i)+1;
      if(*(fpl->_plength+i)>1)ntot+=*(fpl->_plength+i);
    }
    fpl=fpl->_next;
  }


  // Make array


  if(ntot)_Length=lover+ntot;
  if(_Length > 65535*4){
    cerr<<"DAQEvent::buildDAQ-F-lengthToobig "<<_Length<<endl;
    throw amsglobalerror("The Subject Says It All"); 
  }
#ifdef __AMSDEBUG__
  assert(sizeof(time_t) == sizeof(integer));
#endif

  if(_create(btype) ){
    fpl=_pBT[btype];
    while(fpl){
      for(int i=0;i<fpl->_maxbl;i++){
	if(*(fpl->_plength+i)>1){
	  *_pcur=*(fpl->_plength+i)-_OffsetL;
	  int16u *psafe=_pcur+1;
	  fpl->_pgetdata(i,*(fpl->_plength+i)-1,psafe);
	  _pcur=_pcur+*_pcur+_OffsetL;
	}
      }
      fpl=fpl->_next;
    }
  }
}

 

integer DAQEvent::_EventOK(){
#ifdef __AMS02DAQ__
  if(_Length >1 && _pData ){
    integer ntot=0;
    _pcur=_pData+2;
    for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL) {
      ntot+=*(_pcur)+_OffsetL;
    }
    if(ntot != _Length-2){             //  ?  I don't know
      cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
	_Length<<" Blocks say length is "<<ntot+2<<endl;
      cerr <<" SubBlock dump follows"<<endl;
      _pcur=_pData+2;
      for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL)
	cerr <<" ID " <<*(_pcur+1)<<" Length "<< *(_pcur)+_OffsetL<<endl;
      if(!fbin.good()){
        cerr<<" DAQEvent::_Eventok-S-HardError,CouldNotReadFile"<<endl;
      }
      if(fbin.eof()){
        cerr<<"DAQEvent::_Eventok-W-UnexpectedEOF"<<endl;
      }
      return 0;
    }
    else return 1;    
  }
  return 0;

#else  
  const int tofidL = 0x1400;
  const int tofidR = 0x15C1;

  const int trkidL = 0x1680;
  const int trkidR = 0x174C;

  integer tofL = 0;
  integer trkL = 0;

  if(_Length >1 && _pData ){
    if( (_pData[1] & ~63)<<3 ==0){
      // envelop header
      integer ntot=0;
      _pcur=_pData+2;
      for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL) {
	ntot+=*(_pcur)+_OffsetL;
	if (AMSJob::gethead() -> isMonitoring()) {
	  int l   = *(_pcur) + _OffsetL;
	  int bid = *(_pcur+1);
	  int aid = bid;
	  int tofid = bid & 0x1400;
	  if (tofid == 0x1400) aid = aid &~60;
	  int hid    = 300000 + aid; 
	  if (aid == 0x200 ||  aid == 0x0440) HF1(hid,l,1.);
	  if (aid >= tofidL && aid<=tofidR) {
	    HF1(hid,l,1.);
	    tofL = tofL + l;
	  }
	  if (aid == 0x1680 || aid == 0x1740 ||
	      aid == 0x1681 || aid == 0x1741 ||
	      aid == 0x168C || aid == 0x174C) {
	    trkL = trkL + l;
	    HF1(hid,l,1.);
	  }
	}
      }
      if (AMSJob::gethead()->isMonitoring()) {
	HF1(300000,_Length,1.);
	if (tofL) HF1(300001,tofL,1.);
	if (trkL) HF1(300002,trkL,1.);
      }
      if(ntot != _Length-2){
	cerr <<"DAQEvent::_Eventok-E-length mismatch: Header says length is "<<
	  _Length<<" Blocks say length is "<<ntot+2<<endl;
	cerr <<" SubBlock dump follows"<<endl;
	_pcur=_pData+2;
	for(_pcur=_pData+2;_pcur<_pData+_Length;_pcur+=*(_pcur)+_OffsetL)
	  cerr <<" ID " <<*(_pcur+1)<<" Length "<< *(_pcur)+_OffsetL<<endl;
	//     int ic=0;
	//     for(_pcur=_pData;_pcur<_pData+_Length;_pcur++)
	//       cout <<ic++ <<" "<<*(_pcur)<<endl;
	if(!fbin.good()){
	  cerr<<" DAQEvent::_Eventok-S-HardError,CouldNotReadFile"<<endl;
	}
	if(fbin.eof()){
	  cerr<<"DAQEvent::_Eventok-W-UnexpectedEOF"<<endl;
	}
	return 0;
      }
      else return 1;    
    }
    else {
      cout <<"DAQEvent::_EventOK-I-NonEventBlockFound-ID "<<_pData[1]<<" Length "<<
        _Length<<endl;
      return -1;
    }
  }
  else return 0;
#endif
}

integer DAQEvent::_HeaderOK(){
  for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
    if(AMSEvent::checkdaqid(*(_pcur+1))){
      AMSEvent::buildraw(*(_pcur)+_OffsetL-1,_pcur+1, _Run,_Event,_RunType,_Time,_usec);
      _Checked=1;
      if (AMSJob::gethead()->isMonitoring()) {
	if (Time_1 != 0 && _usec > Time_1) {
	  geant d = _usec - Time_1;
	  d = d/1000.;
	  HF1(300003,d,1.);
	}
	Time_1 = _usec;
      }      
#ifdef __AMSDEBUG__
      //cout << "Run "<<_Run<<" Event "<<_Event<<" RunType "<<_RunType<<endl;
      //cout <<ctime(&_Time)<<" usec "<<_usec<<endl;
#endif
      // fix against event 0
      if(_Event==0)return 0;
            
      return 1;
    }
  }
  cerr<<"DAQEvent::_HeaderOK-E-NoHeaderinEvent Type "<<_pData[1]<<endl;
  return 0;
}


  


void DAQEvent::buildRawStructures(){
#ifdef __AMS02DAQ__
  if(_Checked ||(_EventOK()==1 && (_HeaderOK()))){
    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
	for(int16u * pdown=_pcur+1;pdown<_pcur+*_pcur+_OffsetL;pdown=pdown+*pdown){
	  if(fpl->_pgetid(*(pdown+*(pdown+*pdown-1)))){
	    int16u *psafe=pdown+1;
	    fpl->_pputdata(*pdown,psafe);
	  }
	}
      }
      fpl=fpl->_next; 
    }
  }
#else
  if(_Checked ||(_EventOK()==1 && (_HeaderOK()))){
    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      for(_pcur=_pData+lover;_pcur < _pData+_Length;_pcur=_pcur+*_pcur+_OffsetL){
	if(fpl->_pgetid(*(_pcur+1))){
	  int16u *psafe=_pcur+1;
	  fpl->_pputdata(*_pcur+_OffsetL-1,psafe);
	}
      }
      fpl=fpl->_next; 
    }
  }
  else if(_EventOK()==-1){
    // without envelop only one block
    DAQSubDet * fpl=_pSD[_GetBlType()];
    while(fpl){
      _pcur=_pData;
      if(fpl->_pgetid(*(_pcur+1))){
	fpl->_pputdata(*_pcur+_OffsetL-1,_pcur+1);
      }
      fpl=fpl->_next; 
    }
  }
#endif
}


void DAQEvent::write(){
  if(_Length){
    _convert();
    fbout.write((char*)_pData,sizeof(_pData[0])*_Length);
    // Unfortunately we shoulf flush output for each event
    //
    fbout.flush();
  }

}

integer DAQEvent::getoffset(){
  if(fbin)return integer(fbin.tellg())-sizeof(_pData[0])*(_Length);
  else {
    cerr<<"DAQEvent::getoffset-E-fbinNotOPened"<<endl;
    return -1;
  }
}

void DAQEvent::setoffset(uinteger offset){
  if(fbin)fbin.seekg(offset);
}

integer DAQEvent::read(){
  enum open_mode{binary=0x80};
  do{
    if(fbin.eof()){
#ifdef __ALPHA__ 
      fbin.close();
#else
      if(fbin.is_open())fbin.close();
#endif
      integer Run,Event;
      char * fnam=_getNextFile(Run, Event);
      if(fnam){
	for(;;){
	  fbin.open(fnam,ios::in);
	  if(fbin){ 
	    cout <<"DAQEvent::read-I-opened file "<<fnam<<endl;
	    break;
	  }    
	  else{
	    cerr<<"DAQEvent::read-F-cannot open file "<<fnam<<endl;
	    if((fnam=_getNextFile(Run, Event))==0)return 0;
	  }
	}
      }
    }
    if(fbin.good() && !fbin.eof()){
      int16u l16[2];
      fbin.read(( char*)(l16),sizeof(l16));
      _convertl(l16[0]);
      _Length=l16[0]+_OffsetL;
      // get more length (if any)
      _convertl(l16[1]);
      _Length= _Length | ((l16[1] & 63)<<16);
      //cout <<" Length "<<_Length<<endl;

      if(fbin.eof()){
	integer Run,Event;
	char * fnam=_getNextFile(Run, Event);
	if(fnam){
#ifdef __ALPHA__ 
	  fbin.close();
#else
	  if(fbin.is_open())fbin.close();
#endif
	  for(;;){
	    fbin.open(fnam,ios::in);
	    if(fbin){ 
	      cout <<"DAQEvent::read-I-opened file "<<fnam<<endl;
	      fbin.read(( char*)(l16),sizeof(l16));
	      _convertl(l16[0]);
	      _Length=l16[0]+_OffsetL;
	      _convertl(l16[1]);
	      _Length= _Length | ((l16[1] & 63)<<16);
	      //cout <<" Length "<<_Length<<endl;
	      break;
	    }    
	    else{
	      cerr<<"DAQEvent::read-F-cannot open file "<<fnam<<endl;
	      if((fnam=_getNextFile(Run, Event))==0)return 0;
     
	    }
	  }
	}
      }
      if(fbin.good() && !fbin.eof()){
	if(_create()){
	  fbin.seekg(integer(fbin.tellg())-2*sizeof(_pData[0]));
	  fbin.read(( char*)(_pData),sizeof(_pData[0])*(_Length));
	  _convert();
	}
	else{
	  fbin.seekg(integer(fbin.tellg())+sizeof(_pData[0])*(_Length-1));
	  _Length=0;
	}
      }
      else break;
    }
    else break;  
  }while(_EventOK()==0 || (_HeaderOK()==0 ));
  return fbin.good() && !fbin.eof();
}





DAQEvent::InitResult DAQEvent::init(){
  enum open_mode{binary=0x80};
  integer Run,Event;
  char * fnam=_getNextFile(Run, Event);
  if(fnam){
#ifdef __ALPHA__ 
    fbin.close();
#else
    if(fbin.is_open())fbin.close();
#endif
    fbin.open(fnam,ios::in);
    if(fbin){ 
      if(Run){
	DAQEvent daq;
	integer run=-1;
	integer ok=1;
	integer iposr=0;
	while(ok){
	  if(GCFLAG.IEOTRI){
	    cerr<<"DAQEvent::init-F-Interrupt"<<endl;
	    return Interrupt;
	  }
	  fbin.clear();
	  ok=daq.read();
	  if(ok){
	    iposr++;
	    if (daq.runno() != run){
	      cout <<" DAQEvent::init-I-New Run "<<daq.runno()<<endl;
	      run=daq.runno();
	      Time_1 = 0;
	    } 
	    if(Event >=0 && daq.runno() == Run &&
	       daq.eventno() >= Event)break;
	    if(daq.runno() == Run && iposr ==-Event)break;
	    run=daq.runno();
	    daq.shrink();
	  }
	}
	// pos back if fbin.good
	if(ok){
	  fbin.seekg(integer(fbin.tellg())-daq.getlength());
	  cout<<"DAQEvent::init-I-Selected Run = "<<Run<<
	    " Event = "<<daq.eventno()<< " Position = "<<iposr<<" Time "<<ctime(&daq.time())<<endl;

	}
	else {
	  cerr <<"DAQEvent::init-F-Failed to select Run = "<<
	    Run<<" Event >= "<<Event<<endl;
	  return UnableToFindRunEvent;
	}

      }
    }
    else{
      cerr<<"DAQEvent::init-F-cannot open file "<<fnam<<" in mode input"<<endl;
      return UnableToOpenFile;
    }
  }
  else {
    cout <<"DAQEvent::init-I-NoMoreInputFiles"<<endl;
    return NoInputFile;
  }
  return OK;
}


void DAQEvent::initO(integer run){
  enum open_mode{binary=0x80};
  integer mode=DAQCFFKEY.mode;
  if(mode/10 ){
    if(ofnam){
      char name[255];
      ostrstream ost(name,sizeof(name));
      if(ofnam[strlen(ofnam)-1]=='/')ost << ofnam<<run<<ends;
      //next line gives compiler error on gcc
      // else ost << ofnam<<ends;
      if(fbout)fbout.close();
      if(ofnam[strlen(ofnam)-1]!='/')ost << ofnam<<ends;
#if !defined(__USE_STD_IOSTREAM) && !defined(__STDC_HOSTED__) 
      if((mode/10)%10 ==1)fbout.open(name,ios::out|ios::noreplace);
#else
      if((mode/10)%10 ==1)fbout.open(name,ios::out);
#endif
      if((mode/10)%10 ==2)fbout.open(name,ios::out|ios::app);
      if(fbout){ 
	static char buffer[2048+1];
	// Associate buffer
#if defined(__USE_STD_IOSTREAM) || defined(__STDC_HOSTED__)  || defined(sun) || defined(__ICC__)
	(fbout.rdbuf())->pubsetbuf(buffer,2048);
#else
	//      (fbout.rdbuf())->pubsetbuf(buffer,2048);
	(fbout.rdbuf())->setbuf(buffer,2048);
#endif
	cout<<"DAQEvent::initO-I- opened file "<<name<<" in mode "<<mode<<endl;

      }
      else{
	cerr<<"DAQEvent::initO-F-cannot open file "<<name<<" in mode "<<mode<<endl;
	exit(1);
      }
    }
    else {
      cerr <<"DAQEvent::init-F-no output file to init "<<endl;
      exit(1);
    }
  }




}



void DAQEvent::_convert(){
  if(AMSDBc::BigEndian){
    unsigned char tmp;
    unsigned char *pc = (unsigned char*)(_pData);
    int i;
    for(i=0;i<_Length;i++){
      tmp=*pc;
      *pc=*(pc+1);
      *(pc+1)=tmp;
      pc=pc+sizeof(int16u);
    }
  }
}




void DAQEvent::_convertl(int16u & l16){
  if(AMSDBc::BigEndian){
    unsigned char tmp;
    unsigned char *pc = (unsigned char*)(&l16);
    tmp=*pc;
    *pc=*(pc+1);
    *(pc+1)=tmp;
  }
}


void DAQEvent::_copyEl(){
  if(DAQCFFKEY.mode/10)write();


}




integer DAQEventI::_Count=0;
DAQEventI::DAQEventI(){
  _Count++;
}
DAQEventI::~DAQEventI(){
  if(--_Count==0){
#ifdef __ALPHA__ 
    DAQEvent::fbin.close();
#else
    if(DAQEvent::fbin.is_open())DAQEvent::fbin.close();
#endif
#ifdef __ALPHA__ 
    DAQEvent::fbout.close();
#else
    if(DAQEvent::fbout.is_open())DAQEvent::fbout.close();
#endif
  }
}

integer DAQEvent::_create(uinteger btype){
  if(_pData)shrink();
  if(_Length <=0 )return 0;
  if(_Length*sizeof(_pData[0])> sizeof(_Buffer) || _BufferLock)
    _pData= (int16u*)UPool.insert(sizeof(_pData[0])*_Length);
  else {
    _pData=(int16u*)_Buffer;
    _BufferOwner=1;
    _BufferLock=1;
  }
  if(_pData){
    _pData[0]=(_Length-_OffsetL)%65536;
    _pData[1]=(btype<<13) | (_Length-_OffsetL)/65536;  // Event ID
    _pcur=_pData+2;
  }
  return _pData != NULL ;
}


//+

uint16  DAQEvent::sdetlength(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
    id = _pData[offset];
    l  = _pData[offset - 1];
    if (id == sdetid) return l;
    offset = offset  + l + 1;
    if (offset > getlength()/sizeof(_pData[0])) break;
  }
  return 1;
}  

integer DAQEvent::sdet(uint16 sdetid) {
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16 id;
  uint16 l;
  for (;;) {
    id = _pData[offset];
    l  = _pData[offset-1];
    if (id == sdetid) return offset;
    offset = offset  + l + 1;
    if (offset > getlength()/sizeof(_pData[0])) break;
  }
  return -1;
}  

void DAQEvent::dump(uint16 sdetid) {
  // dump event
  // if sdetid == -1 dump whole event
  //
  cout<<"run, event, length "<<runno()<<", "<<eventno()<<", "<<getlength()
      <<endl;
  enum {block_offset = 3};
  integer offset = block_offset;
  uint16  id;
  integer l;
  for (;;) {
    id = _pData[offset];
    l  = _pData[offset - 1];
    if (id == sdetid || sdetid == 0) {
      printf("sub.detector id... %#x, length... %d\n ",id,l);
      if (l > 0) {
	for (int i = 1; i < l-1; i++) {
	  cout<<*((uint16*)(_pData + offset + i))<<" ";
	}
	cout<<endl;
      } else {
	cout<<"Error : invalid length. Quit"<<endl;
	return;
      }
      offset = offset  + l + 1;
      if (offset > getlength()/sizeof(_pData[0])) break;
    }
  }
}  
//-  




int DAQEvent::parser(char a[], char **& fname){


  int ntot=0;
  int kl;
  for(kl=strlen(a);kl>=0;kl--){
    if(a[kl]=='/'){
      kl++;
      break;
    }
  }
  if(kl<0)kl=0;
  // cout << " kl "<<kl<<endl;

  if(kl==0 || kl==strlen(a)){
    // Whole directory  wanted
    dirent ** namelist;
    AString fdir(a);

    ntot=scandir((const char *)fdir,&namelist,_select,_sort);


    if(ntot>0){
      fname =new char*[ntot];
      for(int i=0;i<ntot;i++){
	fname[i]=new char[strlen(a)+strlen(namelist[i]->d_name)+1];
	strcpy(fname[i],a);
	strcat(fname[i],namelist[i]->d_name);
	//             cout << i<< "  "<< fname[i]<<endl;
	free(namelist[i]);
      }
      free(namelist);
    }
    return ntot>0?ntot:0;      
  }


  {
    int coma=kl-1;
    for(int i=kl;i<strlen(a)+1;i++){
      if(a[i]==',' || i==strlen(a)){
	if(i-coma > 1){
	  // find -
	  int tire=0;
	  for(int j=coma+1;j<i;j++){
	    if(a[j]=='-' && j != coma+1 && j != i-1){
	      istrstream osta(a+coma+1,j-coma-1);
	      int ia= 1000000000;
	      int ib=-1000000000;
	      osta >>ia;   
	      istrstream ostb(a+j+1,i-j-1);
	      ostb >>ib;   
	      if(ib >=ia)ntot+=ib-ia+1;
	      tire=1;  
	    }
	  }
	  if(tire==0)ntot++; 
	  coma=i;
	}
      }
    }
  }
  //   cout << " ntot "<<ntot<<endl;
  fname =new char*[ntot];
  ntot=0;
  {
    int coma=kl-1;
    for(int i=kl;i<strlen(a)+1;i++){
      if(a[i]==',' || i==strlen(a)){
	if(i-coma > 1){
	  // find -
	  int tire=0;
	  for(int j=coma+1;j<i;j++){
	    if(a[j]=='-' && j!=coma+1 && j!=i-1){
	      istrstream osta(a+coma+1,j-coma-1);
	      int ia= 1000000000;
	      int ib=-1000000000;
	      osta >>ia;   
	      istrstream ostb(a+j+1,i-j-1);
	      ostb >>ib;   
	      if(ib >=ia){
		//add leading zero(s)
		int lz=0;
		int l;
		for(l=coma+1;l<j;l++){
		  if(a[l]=='0')lz++;
		  else break;
		}
		for(int k=ia;k<=ib;k++){
		  fname[ntot++]=new char[255];
		  for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
		  for(l=kl;l<kl+lz;l++)fname[ntot-1][l]='0';
		  for(l=kl+lz;l<255;l++)fname[ntot-1][l]='\0';
		  ostrstream ost(fname[ntot-1]+kl+lz,255-kl-lz);
		  ost <<k;
		}
	      }
	      tire=1;  
	    }
	  }
	  if(tire==0){
	    fname[ntot++]=new char[255];
	    int l;
	    for(l=0;l<255;l++)fname[ntot-1][l]='\0';
	    for(l=0;l<kl;l++)fname[ntot-1][l]=a[l];
	    for(l=kl;l<i-coma-1+kl;l++)fname[ntot-1][l]=a[l-kl+coma+1];
	  }
	  coma=i;
	}
      }
    }
  }
  return ntot;  


}


#if defined( __ALPHA__) || defined(sun) || defined(__MACOSX__) 
integer DAQEvent::_select( dirent *entry)
#elif defined(__LINUXGNU__) 
integer DAQEvent::_select(const dirent *entry)
#endif
{
  for(int i=0;i<strlen(entry->d_name);i++){
    if(!isdigit((entry->d_name)[i]))return 0;
  }

  int e1;
  sscanf(entry->d_name,"%d",&e1);
  // if(e1<SELECTFFKEY.Run)return 0;
  if(e1<SELECTFFKEY.Run || (SELECTFFKEY.RunE>0 && e1>SELECTFFKEY.RunE))return 0;

  else {
    if(AMSEvent::getselrun(0)>0){
      for(int i=0;;i++){
	if(e1==AMSEvent::getselrun(i))return 1;
	else if(!AMSEvent::getselrun(i))break;
      }
      return 0;
    }
    else return 1;
  }
}

void DAQEvent::SetEOFIn(){
  if(fbin){
    fbin.clear(fbin.rdstate() | ios::eofbit);
    if(fbin.eof())cout <<"DAQEvent::SetEOFIn-I-EOFSetForRun "<<_Run<<" "<<endl;
    else cerr <<"DAQEvent::SetEOFIn-E-EOFSetFailedForRun "<<_Run<<" "<<endl;
    GCFLAG.IEORUN=0;
  }
}

char * DAQEvent::_getNextFile(integer & run, integer &event){
  if(KIFiles==0){
    run=SELECTFFKEY.Run;
    event=SELECTFFKEY.Event;
  }
  else{
    run=0;
    event=0;
  }
  if(KIFiles<InputFiles)return ifnam[KIFiles++];
  else return 0;


}

