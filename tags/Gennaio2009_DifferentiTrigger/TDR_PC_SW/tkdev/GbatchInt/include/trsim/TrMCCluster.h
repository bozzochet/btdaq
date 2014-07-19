//  $Id: TrMCCluster.h,v 1.2 2008/04/02 11:37:13 haino Exp $
#ifndef __TrMCCluster__
#define __TrMCCluster__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrMCCluster.h
///\brief Header file of TrMCCluster class
///
///\class TrMCCluster
///\brief A class implemnting the Tracker MC Clusters
///\ingroup tksim
///
///\date  2008/02/14 SH  First import from Gbatch
///\date  2008/03/17 SH  Compatible with new TkDBc and TkCoo
///\date  2008/04/02 SH  Compatible with new TkDBc and TkSens
///$Date: 2008/04/02 11:37:13 $
///
///$Revision: 1.2 $
///
//////////////////////////////////////////////////////////////////////////

#include "point.h"
#include <vector>

class TrMCCluster {

public:
  /// Status bits
  enum { AwayTOF = 4096 };

  // TRMCFFKEY
  static float alpha;
  static float beta;
  static float fastswitch;
  static float dedx2nprel;
  static float delta [2];
  static float gammaA[2];

protected:
  unsigned int _status;

  int _idsoft;
  int _itra;
  int _left  [2];
  int _center[2];
  int _right [2];

  float    _ss[2][5];
  AMSPoint _xca;
  AMSPoint _xcb;
  AMSPoint _xgl;
  double   _sum;

  void _shower();
  void _printEl(ostream & stream);

  static int _NoiseMarker;

  static std::vector<TrMCCluster*> _cont;

public:
  // Constructor for float track
  TrMCCluster(int idsoft, AMSPoint xca, 
	      AMSPoint xcb, AMSPoint xgl, float sum,int itra)
    : _idsoft(idsoft), _xca(xca), _xcb(xcb), 
      _xgl(xgl), _sum(sum), _itra(itra) {
    _shower();
  }
 ~TrMCCluster(){}

  unsigned int checkstatus(int c) const { return _status&c; }
  unsigned int getstatus  (void)  const { return _status; }
  void setstatus  (unsigned int s) { _status |=  s; }
  void clearstatus(unsigned int s) { _status &= ~s; }

  int IsNoise(){ return _itra == _NoiseMarker; }

  void addcontent(char xy, float ** p, int noise=0);

  static void sitkhits(int idsoft, float vect[],
		       float edep, float step, int itra );

  static int GetN(void) { return _cont.size(); }
  static TrMCCluster *At(int i) { return _cont.at(i); }
  static void clear(void) { 
    for (int i = 0; i < _cont.size(); i++) delete _cont.at(i);
    _cont.clear();
  }

  static float strip2x(int tkid, int side, int strip, int mult);

  // Functions imported from tkmccl.F
  static double fints(double, double);
  static double fint2(double, double, double, double, double);
  static double fintc(double, double, double, double);
  static double fintr(double, double, double, double);
  static double fintl(double, double, double, double);
  static double fdiff(double, int);
};

#endif
