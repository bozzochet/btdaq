#ifndef RHClass_hh
#define RHClass_hh

#include "TClonesArray.h"
#include "TH2F.h"
#include "TObject.h"

using laddernumtype = std::pair<int, int>;

struct DataVersion {
    unsigned int major{0};
    unsigned int minor{0};
    unsigned int patch{0};
};

//! Run Header Class
template<size_t NJINF, size_t NTDRS>
class RHClass : public TObject {
public:
    enum class RunType {
        Unknown = 0, SC = 1, HK = 2, CAL = 4, TC = 8
    };
    static std::string to_string(RunType);

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

    // OCA only
    unsigned int unixTime{0};
    RunType runType{RunType::Unknown};
    unsigned int numBoards{0};
    DataVersion dataVersion{};

    std::string gitSHA{};
    std::vector<unsigned int> boardIDs{};

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

    void SetUnixTime(unsigned int utime) { unixTime = utime; }

    void SetRunType(RunType type) { runType = type; }

    void SetNumBoards(unsigned int nboards) { numBoards = nboards; }

    void SetDataVersion(unsigned int maj, unsigned int min, unsigned int pat) {
        dataVersion.major = maj;
        dataVersion.minor = min;
        dataVersion.patch = pat;
    }

    void SetGitSHA(const std::string &sha) { gitSHA = sha; }

    void AddBoardID(unsigned int id) { boardIDs.push_back(id); }

    unsigned int GetUnixTime() { return unixTime; }

    RunType GetRunType() { return runType; }

    unsigned int GetNumBoards() { return numBoards; }

    DataVersion GetDataVersion() { return dataVersion; }

    std::string GetGitSHA() { return gitSHA; }

    std::vector<unsigned int> GetBoardIDs() { return boardIDs; }

ClassDef(RHClass, 6)
};

#endif
