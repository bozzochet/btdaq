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
  float GetCSignal(int aa);

  int GetReadChannelK() { return KREADCHANN; };
  float GetSensPitchK() { return KSENSPITCH; };

  LadderConf *m_ladderConf; //!

  int SCHANN;//MD: e' una merda, ma non ho trovato di meglio....
  int KCHANN;//MD: e' una merda, ma non ho trovato di meglio...

public:
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

  double GetPitch(int side);
  double GetNominalResolution(int side);
  static double GetPitch(int jinfnum, int tdrnum, int side);
  static double GetNominalResolution(int jinfnum, int tdrnum, int side);
  int GetNChannels(int side) { return (side == 0) ? SCHANN : KCHANN; };

  //! Get the VA a single strip belongs to from its address
  static int GetVA(int strip_address);

  //! std constructor (create an empty cluster)
  Cluster();
  //! copy constructor
  Cluster(Cluster &orig);
  //! std destructor
  virtual ~Cluster(){};
  //! Fill the hit object with the right infos
  void Build(int lad, int sid, int add, int len, float *sig, float *noi, int *stat, int Sig2NoiRatio, int CNStatus,
             int PowBits, int badin = 0);
  //! reset the cluster object
  void Clear();
  //! Returns the address of the first strip in the cluster
  int GetAddress();
  //! Returns the lenght of the cluster
  int GetLength();
  int GetLength(float val);
  //! Returns the position of the seed in the Signal vector
  int GetSeed();
  //! Returns the strip number of the seed
  int GetSeedAdd();
  //! Returns the signal of the seed strip
  float GetSeedVal();
  //! Returns the SN of the seed strip
  float GetSeedSN();
  //! Returns the total CLe noise (sq. mean)
  float GetTotNoise();
  //! Calculate the CoG of the cluster using 2 strips
  float GetCoG();
  //! Returns the Total Signal of the cluster using 2 strips
  float GetSig();
  //! Returns the Total Signal of the cluster
  float GetTotSig();
  //! Return the Cluster SN
  float GetTotSN();
  //! Caclulate the eta (0.,1.)
  float GetEta();
  //! Calculate Etaraw (-0.5,0.5)
  float GetEtaRaw();
  //! printout the cluster infos
  void Print();
  //! Apply VA Equalization from the file loaded by the function Event::ReadGainCorrection
  void ApplyVAEqualization();

  //! Returns the position of the cluster (Cog), in mm units
  double GetPosition(int mult = 0);
  //! Returns the position of the cluster (Cog), in mm units and after alignment
  double GetAlignedPosition(int mult = 0);
  //! Viviana: Returns the position of the cluster (Cog), in mm units and after alignment for MC
  // MD: cannot be like this. Must be general
  double GetAlignedPositionMC();

  //! Returns the Z position
  double GetZPosition();

  double GetSeedCharge() { return sqrt(GetSeedVal()) / sqrt(MIPSIG[side]); };
  double GetCharge() { return sqrt(GetTotSig()) / sqrt(MIPSIG[side]); };

  int GoldRegion();
  bool BorderRegion();

  void SetLadderConf(LadderConf *lConf) { m_ladderConf = lConf; }

  int GetTDR() { return ladder % 100; };
  int GetJinf() { return (int)(ladder / 100); };

  ClassDef(Cluster, 2)
};

#endif
