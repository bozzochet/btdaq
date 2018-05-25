#ifndef LadderConf_hh
#define LadderConf_hh

#include <iostream>
#include <map>

#include "TObject.h"
#include "TString.h"
#include "math.h"

struct LadderParams {
  // HwId is the combination of 2 elements in ladder conf
  // Ladderconf version 0 has  9 _nelements
  // Ladderconf version 1 has 10 _nelements
  static const int _nelements = 10;
  int    _HwId;
  double _spitch;
  double _kpitch;
  double _sreso;
  double _kreso;
  bool   _kmultiflip;
  bool   _smirror;
  bool   _kmirror;
  int    _bondtype;

  void Dump(){
    std::cout << "HwID       = " << _HwId << std::endl;
    std::cout << "spitch     = " << _spitch << std::endl;
    std::cout << "kpitch     = " << _kpitch << std::endl;
    std::cout << "sreso      = " << _sreso << std::endl;
    std::cout << "kreso      = " << _kreso << std::endl;
    std::cout << "kmultiflip = " << _kmultiflip << std::endl;
    std::cout << "smirror    = " << _smirror << std::endl;
    std::cout << "kmirror    = " << _kmirror << std::endl;
    std::cout << "bondtype   = " << _bondtype << std::endl;
  }
};

class LadderConf {
public:
  LadderConf();
  ~LadderConf();

  void Init(TString conffilename="ladderconf.dat", bool DEBUG=false);

  bool GetMultiplicityFlip(int jinfnum, int tdrnum);
  bool GetStripMirroring(int jinfnum, int tdrnum, int side);
  double GetPitch(int jinfnum, int tdrnum, int side);
  double GetResolution(int jinfnum, int tdrnum, int side);
  int GetBondingType(int jinfnum, int tdrnum);

  bool IsTDRConfigured(int jinfnum, int tdrnum);
  bool IsTDRConfigured(int HwId);

  void PrintLadderParams(int jinfnum, int tdrnum);

private:
  std::map<int, LadderParams*> _ladders;

};




#endif
