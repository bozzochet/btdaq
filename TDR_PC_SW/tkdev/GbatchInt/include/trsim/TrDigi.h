//  $Id: TrDigi.h,v 1.2 2008/04/02 11:36:43 haino Exp $
#ifndef __TrDigi__
#define __TrDigi__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrDigi.h
///\brief Header file of TrDigi class
///
///\class TrDigi
///\brief A class implemnting the Tracker digitization
///\ingroup tksim
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc
///$Date: 2008/04/02 11:36:43 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include <vector>
#include <iostream>

class TrDigi {

public:
  // TRMCFFKEY
  static int   adcoverflow;
  static float thr1R[2];
  static float thr2R[2];
  static int   neib [2];
  static int   CalcCmnNoise[2];

  static int VAchannels;

  static const int MATCHED;

  static float CmnNoise[10][4000];

protected:
  unsigned int _status;

  int   _address;   // as for idsoft
  int   _strip;
  int   _nelem;
  float _s2n;       // signal to noise for triggered strip
  int * _array;

  void _printEl(std::ostream & stream);

  static std::vector<TrDigi *> _cont;

public:
  TrDigi(int id, int nleft, int nright, float *p, float s2n);
 ~TrDigi();

  unsigned int checkstatus(int c) const { return _status&c; }
  unsigned int getstatus  (void)  const { return _status; }
  void setstatus  (unsigned int s) { _status |=  s; }
  void clearstatus(unsigned int s) { _status &= ~s; }

  int  getid   () const { return _address; }
  int  getstrip() const { return _strip; }
  int  getnelem() const { return _nelem; }
  int *getarray()       { return _array; }

  static void sitkdigi();
  static void matchKS();

  static int ReadCmnNoise(const char *fname);

  static int GetN(void) { return _cont.size(); }
  static TrDigi *At(int i) { return _cont.at(i); }
  static void clear(void) { 
    for (int i = 0; i < _cont.size(); i++) delete _cont.at(i);
    _cont.clear();
  }
};

#endif
