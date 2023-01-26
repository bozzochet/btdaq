#ifndef Cluster_hh
#define Cluster_hh

#include "math.h"

#include "TObject.h"

#include "LadderConf.hh"

#define MAXLENGHT 128

/* defined by GenericEvent::AddCluster. Numbers by Viviana sostantially ignored...
// Viviana: originally was 640 and 384 channels
//#define SCHANN 640
#define SCHANN 2048 // 4096 // 50cmx50cm sensors
//#define KCHANN 384
#define KCHANN 2048 // fake K same 50x50cm sensors
*/

// https://twiki.cern.ch/twiki/pub/Sandbox/HerdBT/HERDBT_Silicon_detector_details.pdf

#define KREADCHANN 192
#define KSENSPITCH 41.40

//-------------------------------------------------------------------
//unused:
// 715 + 40 + 715 = 1470μm - this is the gap between two sensor on S
// (the strip conceptually is just one but is the particle passes in this gap is not detected, both on K but also on S)
#define SGAP 1470
// 676 + 40 + 676 = 1392μm
#define KGAP 1392
//-------------------------------------------------------------------

//unused:
// 40007 + 1392 (KGAP)
#define KACTIVE 41399

static double MIPSIG[2] = {40.0, 40.0};

struct ClusterSummary {
  unsigned int ladder;
  unsigned int side;
  double signal;
  double charge;
  double son;
};

//!  Tracker Cluster class.
/*!  Tracker Cluser class contains all the information about a Cluster
 */

// class Event;//forward declaration

class Cluster : public TObject {

private:
  float GetCSignal(int aa) const;

  int GetReadChannelK() const { return KREADCHANN; };
  float GetSensPitchK() const { return KSENSPITCH; };

  LadderConf *m_ladderConf; //!

  int SCHANN;//MD: e' una merda, ma non ho trovato di meglio....
  int KCHANN;//MD: e' una merda, ma non ho trovato di meglio...

public:
  static void SetMip(double mips, double mipk) { MIPSIG[0] = mips; MIPSIG[1] = mipk; }

  void SetNChannels(int side, int _channels) {
    if (side==0)
      SCHANN=_channels;
    else
      KCHANN=_channels;
  }

  //! Adress of the first strip of the cluster
  int address;
  //! Nstrips of the clusters
  int length;
  //! Vector of the cluster signals
  float Signal[MAXLENGHT];
  //! Vector of the noise of the cluster strips
  float Noise[MAXLENGHT];
  //! Vector of the status of the cluster strips
  int Status[MAXLENGHT];
  //! Vector of the cluster signals after the VA equalization
  float SignalVAEqualized[MAXLENGHT];
  //! Vector of the noise of the cluster strips after the VA equalization
  float NoiseVAEqualized[MAXLENGHT];
  //! Ladder (trdnum + 100*jinfnum)
  int ladder;
  //! side (0=S, 1=K)
  int side; // 0 S / 1 K
  //! cluster > MAXLENGHT strips
  int bad;
  //! Golden Flag
  int golden;
  // Signal/Noise ratio
  int snratio;
  // CN status
  int CNstatus;
  // Power bits
  int powbits;

  double GetPitch(int side) const;
  double GetNominalResolution(int side) const;
  static double GetPitch(int jinfnum, int tdrnum, int side);
  static double GetNominalResolution(int jinfnum, int tdrnum, int side);
  int GetNChannels(int side) const { return (side == 0) ? SCHANN : KCHANN; };

  //! Get the VA a single strip belongs to from its address
  static int GetVA(int strip_address);

  //! std constructor (create an empty cluster)
  Cluster();
  //! copy constructor
  Cluster(const Cluster &orig);
  //! std destructor
  virtual ~Cluster() = default;
  //! Fill the hit object with the right infos
  void Build(int lad, int sid, int add, int len, float *sig, float *noi, int *stat, int Sig2NoiRatio, int CNStatus,
             int PowBits, int badin = 0);
  //! reset the cluster object
  void Clear();
  //! Returns the address of the first strip in the cluster
  int GetAddress() const;
  //! Returns the lenght of the cluster
  int GetLength() const;
  int GetLength(float val) const;
  //! Returns the position of the seed in the Signal vector
  int GetSeed() const ;
  //! Returns the strip number of the seed
  int GetSeedAdd() const;
  //! Returns the signal of the seed strip
  float GetSeedVal() const;
  //| Returns position of the secondary strip in the Signal vector
  int GetSec() const;
  //! Returns the signal of the secondary strip
  float GetSecVal() const;
  //! Returns the SN of the seed strip
  float GetSeedSN() const;
  //! Returns the total CLe noise (sq. mean)
  float GetTotNoise() const;
  //! Calculates the CoG of the cluster using 2 strips
  float GetCoG() const;
  //! Returns the Total Signal of the cluster using 2 strips
  float GetSig() const;
  //! Returns the Total Signal of the cluster
  float GetTotSig() const;
  //! Returns the Cluster SN
  float GetTotSN() const;
  //! Caclulate sthe eta (0.,1.)
  float GetEta() const;
  //! Calculates Etaraw (-0.5,0.5)
  float GetEtaRaw() const;
  //! printout the cluster infos
  void Print();
  //! Returns a new Signal vector in descending order
  std::vector<float> Sort();
  //! Apply VA Equalization from the file loaded by the function Event::ReadGainCorrection
  void ApplyVAEqualization();

  //! Returns the position of the cluster (Cog), in mm units
  double GetPosition(int mult = 0) const;
  //! Returns the position of the cluster (Cog), in mm units and after alignment
  double GetAlignedPosition(int mult = 0) const;
  //! Viviana: Returns the position of the cluster (Cog), in mm units and after alignment for MC
  // MD: cannot be like this. Must be general
  double GetAlignedPositionMC() const;

  //! Returns the Z position
  double GetZPosition() const;

  double GetSeedCharge() const { return sqrt(GetSeedVal()) / sqrt(MIPSIG[side]); };
  double GetCharge() const { return sqrt(GetTotSig()) / sqrt(MIPSIG[side]); };

  int GoldRegion();
  bool BorderRegion();

  void SetLadderConf(LadderConf *lConf) { m_ladderConf = lConf; }

  int GetTDR() const { return ladder % 100; };
  int GetJinf() const { return (int)(ladder / 100); };

  ClassDef(Cluster, 2)
};

#endif
