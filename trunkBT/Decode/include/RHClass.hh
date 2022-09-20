#ifndef RHClass_hh
#define RHClass_hh

#include "TClonesArray.h"
#include "TH2F.h"
#include "TObject.h"

using laddernumtype = std::pair<int, int>;

//! Run Header Class
template <size_t NJINF, size_t NTDRS> class RHClass : public TObject {

private:
  //! Run number
  int Run{0};
  char date[50]{""};
  int nJinf{0};
  int JinfMap[NJINF]{0};
  int ntdrRaw{0};
  int ntdrCmp{0};
  //  double CNMean[NTDRS][NVAS];//added by Viviana? Do we really need?
  //  double CNSigma[NTDRS][NVAS];//added by Viviana? Do we really need?
  laddernumtype tdrMap[NJINF * NTDRS];

public:
  //! default constructor
  RHClass() = default;
  //! default destructor
  virtual ~RHClass() = default;
  //! Prints the Header infos
  void Print();

  inline void SetRun(int _run) {
    Run = _run;
    return;
  }
  inline int GetRun() { return Run; }

  inline void SetDate(char *_date) {
    snprintf(date, 50, "%s", _date);
    return;
  }
  inline const char *GetDate() { return date; }

  constexpr inline int GetNJinfs() { return NJINF; }

  inline int GetNTdrs() { return ntdrRaw + ntdrCmp; }
  inline int GetNTdrsCmp() { return ntdrCmp; }
  inline int GetNTdrsRaw() { return ntdrRaw; }
  inline void SetNTdrsCmp(int _nTdrCmp) {
    ntdrCmp = _nTdrCmp;
    return;
  }
  inline void SetNTdrsRaw(int _nTdrRaw) {
    ntdrRaw = _nTdrRaw;
    return;
  }

  void SetJinfMap(int *_JinfMap);
  void SetTdrMap(laddernumtype *_TdrMap);

  // FIXME: the methods below must include also the jinfnum
  // sometims infact I use tdrnum+100*jinfnum that CANNOT WORK (see above)!

  int GetTdrNum(int tdrpos);
  int GetTdrType(int tdrpos);

  int FindPos(int tdrnum);

  ClassDef(RHClass, 5)
};

#endif
