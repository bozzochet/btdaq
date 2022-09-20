#ifndef LadderConf_hh
#define LadderConf_hh

#include <iostream>
#include <map>

#include "TObject.h"
#include "TString.h"
#include "math.h"

class LadderParams: public TObject {
public:
  
  // HwId is the combination of 2 elements in ladder conf
  // Ladderconf version 0 has  9 _nelements
  // Ladderconf version 1 has 10 _nelements
  // Ladderconf version 2 has 14 _nelements
  static const int _nelements = 15;
  int _HwId;//two elements
  double _spitch;
  double _kpitch;
  double _sreso;
  double _kreso;
  bool _kmultiflip;
  bool _smirror;
  bool _kmirror;
  int _bondtype;
  double _shithresh;
  double _khithresh;
  double _slothresh;
  double _klothresh;
  bool _sideswap;

  void Dump() {
    printf("-----------------------------------------\n");
    std::cout << "HwID       = " << _HwId << std::endl;
    std::cout << "spitch     = " << _spitch << std::endl;
    std::cout << "kpitch     = " << _kpitch << std::endl;
    std::cout << "sreso      = " << _sreso << std::endl;
    std::cout << "kreso      = " << _kreso << std::endl;
    std::cout << "kmultiflip = " << _kmultiflip << std::endl;
    std::cout << "smirror    = " << _smirror << std::endl;
    std::cout << "kmirror    = " << _kmirror << std::endl;
    std::cout << "bondtype   = " << _bondtype << std::endl;
    std::cout << "shithresh  = " << _shithresh << std::endl;
    std::cout << "khithresh  = " << _shithresh << std::endl;
    std::cout << "slothresh  = " << _slothresh << std::endl;
    std::cout << "klothresh  = " << _slothresh << std::endl;
    std::cout << "sideswap   = " << _sideswap << std::endl;
    printf("-----------------------------------------\n");
  }

  ClassDef(LadderParams, 2)
};

class LadderParamsMap: public TObject {
public:
  std::map< int, LadderParams* > & GetMap() { return _ladders; };
  
private:
  std::map< int, LadderParams* > _ladders;

  ClassDef(LadderParamsMap, 2)
};

class LadderConf: public TObject {
public:
  static LadderConf *Instance();
  ~LadderConf();

  static void Init(TString conffilename = "ladderconf.dat", bool DEBUG = false);
  static void InitSize(size_t nJinf, size_t nTdrs) {
    NJINF = nJinf;
    NTDRS = nTdrs;
  }

  static bool GetMultiplicityFlip(int jinfnum, int tdrnum);
  static bool GetStripMirroring(int jinfnum, int tdrnum, int side);
  static double GetPitch(int jinfnum, int tdrnum, int side);
  static double GetResolution(int jinfnum, int tdrnum, int side);
  static int GetBondingType(int jinfnum, int tdrnum);
  static double GetSHiThreshold(int jinfnum, int tdrnum);
  static double GetKHiThreshold(int jinfnum, int tdrnum);
  static double GetSLoThreshold(int jinfnum, int tdrnum);
  static double GetKLoThreshold(int jinfnum, int tdrnum);
  static bool GetSideSwap(int jinfnum, int tdrnum);

  static bool IsTDRConfigured(int jinfnum, int tdrnum);
  static bool IsTDRConfigured(int HwId);

  static void PrintLadderParams(int jinfnum, int tdrnum);

  void Dump();

  LadderParamsMap* GetLadderParamsMap() const { return _ladders; };

  LadderConf(LadderParamsMap* lpm);
  
private:
  static LadderConf* _head;
  LadderConf();
  static LadderParamsMap* _ladders;//!

  static size_t NJINF;
  static size_t NTDRS;
};

#endif
