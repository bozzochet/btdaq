#ifndef EventUtils_hh
#define EventUtils_hh

#include <array>
#include <vector>

#include "TClonesArray.h"
#include "TH2F.h"
#include "TObject.h"

#include "Cluster.hh"
#include "LadderConf.hh"
#include "RHClass.hh"

// #define NJINF 1
// #define NTDRS 24 // this a "max number of". For FOOT/OCA remember that we have 2 sensors for board (i.e. 6 DE-10 nano
// means 12 "TDR") #define NCHAVA 64 //for POX MC it was 256 or 128. To be understood...

// #ifdef OCA
// #define NADCS 5
// #define NVAS 10
// #elif defined FOOT
// #define NADCS 5
// #define NVAS 10
// #else
// #define NVAS  16
// #endif

//// for MC events
#define MAXNHITS 500

class GainCorrectionPars {
public:
  static GainCorrectionPars *Instance() {
    static GainCorrectionPars instance;
    return &instance;
  }
  ~GainCorrectionPars() = default;

  void InitSize(size_t nJinf, size_t nTdr, size_t nVA_S, size_t nVA_K);
  void Init(const std::string &filename);
  float GetPar(size_t jinfnum, size_t tdrnum, size_t vanum, size_t parnum) {
    try {
      return m_pars.at(jinfnum).at(tdrnum).at(vanum)[parnum];
    } catch (const std::out_of_range &e) {
      printf("*** GainCorrectionPars for {Jinf=%lu, TDRnum=%lu, VAnum=%lu} not defined. Using default values...\n",
             jinfnum, tdrnum, vanum);
      m_pars[jinfnum][tdrnum][vanum][0] = 0.0;
      m_pars[jinfnum][tdrnum][vanum][1] = 1.0;
      return m_pars[jinfnum][tdrnum][vanum][parnum];
    }
  };

private:
  GainCorrectionPars() = default;

  std::map<size_t, std::map<size_t, std::map<size_t, std::array<float, 2>>>> m_pars;
};

class AlignmentPars {
public:
  static AlignmentPars *Instance() {
    static AlignmentPars instance;
    return &instance;
  }
  ~AlignmentPars() = default;

  void InitSize(size_t nJinf, size_t nTdr);
  void Init(const std::string &filename);
  float GetPar(size_t jinfnum, size_t tdrnum, size_t parnum) {
    try {
      return m_pars.at(jinfnum).at(tdrnum)[parnum];
    } catch (const std::out_of_range &e) {
      printf("*** AlignmentPars for {Jinf=%lu, TDRnum=%lu} not defined. Using default values...\n", jinfnum, tdrnum);
      m_pars[jinfnum][tdrnum][0] = 0.0;
      m_pars[jinfnum][tdrnum][1] = 0.0;
      m_pars[jinfnum][tdrnum][2] = 0.0;
      return m_pars[jinfnum][tdrnum][parnum];
    }
  };
  bool GetMultiplicityFlip(size_t jinfnum, size_t tdrnum) {
    try {
      return m_multflip.at(jinfnum).at(tdrnum);
    } catch (const std::out_of_range &e) {
      printf("*** AlignmentPars (MultiplicityFlip) for {Jinf=%lu, TDRnum=%lu} not defined. Using default values...\n",
             jinfnum, tdrnum);
      m_multflip[jinfnum][tdrnum] = false;
      return m_multflip[jinfnum][tdrnum];
    }
  };

  private:
    AlignmentPars() = default;

    std::map<size_t, std::map<size_t, std::array<float, 3>>> m_pars;
    std::map<size_t, std::map<size_t, bool>> m_multflip;
  };

using laddernumtype = std::pair<int, int>;
using Hit = std::pair<int, std::pair<double, double>>;

/*!  Tracker Event class contains all the information about a Event
 */
struct track {
  track();
  std::vector<Hit> hits;
  void update();
  // private:
  double prod_angle;
  double prod_dist;

  double exit_angle;
  double exit_dist;
};

using trackColl = std::vector<track>;

std::pair<double, double> vertex(track &tr1, track &tr2);

class HoughSpace {
  double risth, risr;
  std::map<std::pair<double, double>, int> votes;
  std::pair<double, double> maxpos;
  int maxfreq;

public:
  HoughSpace(double, double);
  void Add(double, double);
  std::pair<double, double> GetMax() { return maxpos; };
};

#endif
