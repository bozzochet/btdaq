//  $Id: uzstat.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// Author V. Choutko 24-may-1996
 
// The "uztime L3" like package
// to check the CPU time consuming by different code parts.
// To use it:
// AMSgObj:BookTimer.book("string");   // books the timer
// AMSgObj:BookTimer.start("string");   // starts the timer
// AMSgObj:BookTimer.stop("string");   // stops the timer & update the statistics
// AMSgObj::BookTimer.print();        // prints the timers statistics, i.e.
//                                    // min,max,average,sum

#ifndef __AMSUZSTAT__
#define __AMSUZSTAT__
#include "cern.h"
#include "typedefs.h"
#include <limits.h>
#include <math.h>
#include "node.h"
#include "snode.h"
#include <float.h>

// High granularity time

extern "C" number HighResTime();


class AMSStatNode: public AMSNode{

friend class AMSStat;

private:
integer _startstop;
number _time;
number _entry;
number _sum;
number _max;
number _min;
integer _freq;
public:
AMSStatNode():AMSNode(0),_startstop(0),_entry(0),_sum(0),_max(-FLT_MAX),_min(FLT_MAX),_freq(1){};
AMSStatNode(char * name, int freq):AMSNode(name),_startstop(0),_entry(0),_sum(0),_max(-FLT_MAX),_min(FLT_MAX),_freq(freq){};
void _init(){};
ostream & print(ostream & stream ) const;
};


class AMSStat: public AMSNodeMap{
private:
AMSStatNode Timer;
public: 
  AMSStat();
  ~AMSStat();
  void book(char * name, int freq=1);
  void start(char * name);
  number check(char *name);
  number stop(char * name, integer force=0);
  void print();
};



class AMSStatErrNode: public AMSNode{

friend class AMSStatErr;

private:
uinteger _entry;
char _severity;
public:
AMSStatErrNode():AMSNode(0),_severity('I'),_entry(0){};
AMSStatErrNode(char * name, uinteger severity ):AMSNode(name),_entry(0){
  switch (severity){
  case 0:
   _severity='I';
   break;
  case 1:
   _severity='W';
   break;
  case 2:
   _severity='E';
   break;
  case 3:
   _severity='S';
   break;
  case 4:
   _severity='F';
   break;
  default:
   _severity='I';
  }
}

void _init(){};
ostream & print(ostream & stream ) const;
};



class AMSStatErr: public AMSNodeMap{
private:
AMSStatErrNode Timer;
public:
AMSStatErr();
~AMSStatErr();
void book(char * name, char severity);
void print(char * name, char *message=0, uinteger maxprint=10);
void print();
};
#endif
