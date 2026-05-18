#ifndef RHClass_hh
#define RHClass_hh

#include "TClonesArray.h"
#include "TH2F.h"
#include "TObject.h"

// < Id, raw_or_cmp_or_mix>
using laddernumtype = std::pair<int, int>;

struct DataVersion {
  unsigned int major{0};
  unsigned int minor{0};
  unsigned int patch{0};
};

// class JINJ;
class JINF;
class TDR;

class GenericRHClass : public TObject {
public:
  virtual int GetNJINF() = 0;
  virtual int GetNTDRS() = 0;

  static inline int ComputeTdrId(int tdrnum, int jinfnum) { return 100 * jinfnum + tdrnum; }

  virtual TDR GetTDR_bynums(int tdrnum, int jinfnum) = 0;
  virtual TDR GetTDR_byglobindex(int tdrglobindex) = 0;
  virtual int GetTdrNum_byID(int tdrid) = 0;
  virtual int GetJinfNum_byID(int tdrid) = 0;
  virtual int GetTdrNum_byglobindex(int tdrglobindex) = 0;
  virtual int GetJinfNum_byglobindex(int tdrglobindex) = 0;
  virtual int GetTdrGlobIndex_bynums(int tdrnum, int jinfnum) = 0;
  virtual int GetJinfIndex_bynum(int jinfnum) = 0;
  virtual int GetJinfNum_byindex(int jinfindex) = 0;

  virtual void Print() = 0;

  ClassDef(GenericRHClass, 7)
};

//! Run Header Class
template <size_t NJINF, size_t NTDRS> class RHClass : public GenericRHClass {
public:
  enum class RunType { Unknown = 0, SC = 1, HK = 2, CAL = 4, TC = 8 };
  static std::string to_string(RunType);

  unsigned int unixTime{0};
  RunType runType{RunType::Unknown};
  unsigned int runTag{0};

  inline int GetNJINF() final { return NJINF; }
  inline int GetNTDRS() final { return NTDRS; }

private:
  //! Run number
  int Run{0};
  std::string date{""};
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
  void PrintHeader();
  void PrintFooter();

  inline void SetRun(int _run) {
    Run = _run;
    return;
  }
  inline int GetRun() { return Run; }

  inline void SetDate(const char *_date) {
    date = _date;
    return;
  }
  inline const std::string GetDate() { return date; }

  void SetUnixTime(unsigned int utime) { unixTime = utime; }
  unsigned int GetUnixTime() { return unixTime; }

  void SetRunType(RunType type) { runType = type; }
  RunType GetRunType() { return runType; }

  void SetRunTag(unsigned int tag) { runTag = tag; }
  unsigned int GetRunTag() { return runTag; }

  inline void SetNJinf(int _nJinf) {
    nJinf = _nJinf;
    return;
  }
  constexpr inline int GetNJinfs() { return nJinf; }

  inline void SetNTdrsCmp(int _nTdrCmp) {
    ntdrCmp = _nTdrCmp;
    return;
  }
  inline void SetNTdrsRaw(int _nTdrRaw) {
    ntdrRaw = _nTdrRaw;
    return;
  }
  inline int GetNTdrsCmp() { return ntdrCmp; }
  inline int GetNTdrsRaw() { return ntdrRaw; }
  inline int GetNTdrs() { return ntdrRaw + ntdrCmp; }

  void SetJinfMap(int *_JinfMap);
  void SetTdrMap(laddernumtype *_TdrMap);

  TDR GetTDR_bynums(int tdrnum, int jinfnum) final;
  TDR GetTDR_byglobindex(int tdrglobindex) final;
  int GetTdrNum_byID(int tdrid) final;
  int GetJinfNum_byID(int tdrid) final;
  int GetTdrNum_byglobindex(int tdrglobindex) final;
  int GetJinfNum_byglobindex(int tdrglobindex) final;
  int GetTdrGlobIndex_bynums(int tdrnum, int jinfnum) final;
  int GetJinfIndex_bynum(int jinfnum) final;
  int GetJinfNum_byindex(int jinfindex) final;

  ClassDef(RHClass, 7)
};

class RHClassOCA : public RHClass<1, 24> {
private:
  unsigned int numBoards{0};
  DataVersion dataVersion{};

  std::string gitSHA{};
  std::vector<unsigned int> boardIDs{};

public:
  //! default constructor
  RHClassOCA() = default;

  //! default destructor
  ~RHClassOCA() = default;

  //! Prints the Header infos
  inline void Print();

  void SetNumBoards(unsigned int nboards) { numBoards = nboards; }
  unsigned int GetNumBoards() { return numBoards; }

  void SetDataVersion(unsigned int maj, unsigned int min, unsigned int pat) {
    dataVersion.major = maj;
    dataVersion.minor = min;
    dataVersion.patch = pat;
  }
  DataVersion GetDataVersion() { return dataVersion; }

  void SetGitSHA(const std::string &sha) { gitSHA = sha; }
  std::string GetGitSHA() { return gitSHA; }

  void AddBoardID(unsigned int id) { boardIDs.push_back(id); }
  std::vector<unsigned int> GetBoardIDs() { return boardIDs; }

  ClassDef(RHClassOCA, 7);
};

class RHClassHEF : public RHClass<1, 24> {
private:
  unsigned int numBoards{0};
  DataVersion dataVersion{};

  std::string gitSHA{};
  std::vector<unsigned int> boardIDs{};

public:
  //! default constructor
  RHClassHEF() = default;

  //! default destructor
  ~RHClassHEF() = default;

  //! Prints the Header infos
  inline void Print();

  void SetNumBoards(unsigned int nboards) { numBoards = nboards; }
  unsigned int GetNumBoards() { return numBoards; }

  void SetDataVersion(unsigned int maj, unsigned int min, unsigned int pat) {
    dataVersion.major = maj;
    dataVersion.minor = min;
    dataVersion.patch = pat;
  }
  DataVersion GetDataVersion() { return dataVersion; }

  void SetGitSHA(const std::string &sha) { gitSHA = sha; }
  std::string GetGitSHA() { return gitSHA; }

  void AddBoardID(unsigned int id) { boardIDs.push_back(id); }
  std::vector<unsigned int> GetBoardIDs() { return boardIDs; }

  ClassDef(RHClassHEF, 7);
};

/*
class JINJ {

};
*/

class JINF {
private:
  int JINFnum{0}; // JINF number, also jinf pos in 2D arrays;

public:
  inline void setJINFnum(int _JINFnum) { JINFnum = _JINFnum; }
  inline int getJINFnum() { return JINFnum; }
};

class TDR {
private:
  int globindex{-9999}; // position in the TdrMap
  int JINFnum{0};       // JINF number, also jinf pos in 2D arrays;
  int TDRnum{-9999};    // TDR number, also tdr pos in 2D arrays;
  int ID{-9999};        // unique ID for the TDR, essentially 100*JINFnum + TDRnum

public:
  inline int getglobindex() { return globindex; }
  inline int getJINFnum() { return JINFnum; }
  inline int getTDRnum() { return TDRnum; }
  inline int getID() { return ID; }
  inline void setglobindex(int _globindex) { globindex = _globindex; }
  inline void setJINFnum(int _JINFnum) { JINFnum = _JINFnum; }
  inline void setTDRnum(int _TDRnum) { TDRnum = _TDRnum; }
  inline void setID(int _ID) { ID = _ID; }
  inline JINF getJINF() {
    JINF jinf;
    jinf.setJINFnum(JINFnum);
    return jinf;
  };
};

#endif
