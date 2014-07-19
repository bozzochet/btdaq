// $Id: daqevt.h,v 1.2 2007/11/14 11:33:45 zuccon Exp $
//   V. Choutko 15/6/97
//   //
//   A.Klimentov June 21, 1997.                   ! add functions
//   sdetlength(id)    !
//   data(buff)        !
//   sdet(id)          !
//   dump(id)          !
//   //
#ifndef __AMSDAQEVT__
#define __AMSDAQEVT__
#include "id.h"
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include <time.h>
#include <dirent.h>
typedef integer (*pid)(int16u id);
typedef void  (*pputdata)(integer n, int16u* data);

typedef integer (*pgetmaxblocks)();
typedef integer (*pgetl)(integer i);
typedef void  (*pgetdata)(integer i, integer n, int16u* data);

class DAQSubDet  {
protected:
  pid _pgetid;
  pputdata _pputdata;
  DAQSubDet * _next;
public:
  DAQSubDet():_next(0),_pgetid(0),_pputdata(0){}
  DAQSubDet(pid pgetid,  pputdata pput):
    _next(0),_pgetid(pgetid), _pputdata(pput){}

  friend class DAQEvent;
};

class DAQBlockType  {
protected:
  integer _maxbl;
  integer *  _plength;
  pgetmaxblocks _pgmb;
  pgetdata _pgetdata;
  pgetl   _pgetlength;
  DAQBlockType * _next;
public:
  DAQBlockType():_next(0),_pgetdata(0),
		 _pgetlength(0),_pgmb(0),_maxbl(0),_plength(0){}
  DAQBlockType(pgetmaxblocks pgmb, pgetl pgl, pgetdata pget):
    _next(0),_pgetlength(pgl),_pgetdata(pget), _pgmb(pgmb),
    _maxbl(0),_plength(0){}

  friend class DAQEvent;
};
const integer nbtps=8;    // blocks num 
class DAQEvent : public AMSlink{
protected:
  integer _BufferOwner;
  integer _Checked;
  uinteger _Length;
  uinteger _Event;
  uinteger _Run;
  uinteger _RunType;
  time_t _Time;
  uinteger _usec;
  int16u *  _pcur;
  int16u * _pData;
  static DAQSubDet * _pSD[nbtps];
  static DAQBlockType * _pBT[nbtps];
  static const integer _OffsetL;
  integer _EventOK();
  integer _HeaderOK();
  uinteger _GetBlType(){return _pData[1]>>13;}
  void _convert();
  void _convertl(int16u & l16);
  integer _create(uinteger btype=0);
  void _copyEl();
  void _writeEl(){}
  void _printEl(ostream& o){}
  static integer _Buffer[50000];
  static integer _BufferLock;
#if defined(__MACOSX__)
  static integer _select(dirent * entry=0);
  static int _sort(const void *de1, const void * de2){
    dirent **e1= (dirent**) de1;
    dirent **e2= (dirent**) de2;
    return strcmp((*e1)->d_name,(*e2)->d_name);}
#elif defined(__LINUXGNU__)
  static integer _select(const dirent * entry=0);
  static int _sort(const void *de1, const void * de2){
    dirent **e1= (dirent**) de1;
    dirent **e2= (dirent**) de2;
    return strcmp((*e1)->d_name,(*e2)->d_name);}
#else
   static integer _select(dirent * entry=0);
   static int _sort(dirent ** e1,  dirent ** e2){return strcmp((*e1)->d_name,(*e2)->d_name);}
#endif
public:
  uinteger GetBlType(){return _GetBlType();}
  ~DAQEvent();
  DAQEvent(): AMSlink(),_Length(0),_Event(0),_Run(0),_pcur(0),_pData(0),_Checked(0),
	      _Time(0),_RunType(0),_usec(0),_BufferOwner(0){}
  uinteger & eventno(){return _Event;}
  uinteger & runno(){return _Run;}
  time_t   & time(){return _Time;}
  uinteger & runtype(){return _RunType;}
  uinteger & usec(){return _usec;}
  void buildDAQ(uinteger btype=0);
  void buildRawStructures();
  void write();
  integer read();
  integer getoffset();
  void setoffset(uinteger offset);
  void shrink();
  integer getlength() const {return _Length*sizeof(_pData[0]);}
  void close(){ fbin.close();fbout.close();}
  static char ** ifnam;
  static integer InputFiles;
  static integer KIFiles;
  static fstream fbin;
  static char * ofnam;
  static fstream fbout;
  void SetEOFIn();
  enum InitResult{Interrupt,NoInputFile,UnableToOpenFile,UnableToFindRunEvent,OK};
  static InitResult init();
  static char * _getNextFile(integer & run, integer & event);
  static void initO(integer run);
  static void addsubdetector(pid pgetid, pputdata pput, uinteger btype=0);
  static void addblocktype(pgetmaxblocks pgmb, pgetl pgl,pgetdata pget, uinteger btype=0);

  //+
  void    data(uint16 *buff) {memcpy(buff,_pData,getlength());}
  uint16  sdetlength(uint16 sdetid);
  integer sdet(uint16 sdetid);
  void    dump(uint16 sdetid); 
  //-  
  static void setfiles(char *ifile, char *ofile);
  static void setfile(const char *ifile);
  static const char * getfile();
  static integer parser(char *ifile, char ** & ifnam); 
};

class DAQEventI{
public:
  DAQEventI();
  ~DAQEventI();
private:
  static integer _Count;
};
static DAQEventI DAQEventI;




#endif
