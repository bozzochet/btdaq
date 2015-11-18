#ifndef Cluster_hh
#define Cluster_hh

#include "TObject.h"
#include "math.h"

#define MAXLENGHT 128
#define NTDRS  24

static double MIPSIG[2] = {35.0, 35.0};

//!  Tracker Cluster class. 
/*!  Tracker Cluser class contains all the information about a Cluster
 */

//class Event;//forward declaration

class Cluster :public TObject{

private:
  float GetCSignal(int aa);

public:
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
  //! Ladder (trdnum + 100*jinfnum)
  int ladder;
  //! side (0=S, 1=K)
  int side; //0 S / 1 K
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

  static double GetPitch(int side){ return (side==0)?0.104:0.208; };//104mum and 208mum
  static double GetNominalResolution(int side){ return (side==0)?0.01:0.03; };//10mum and 30mum
  
  //! std constructor (create an empty cluster)
  Cluster();
  //! copy constructor 
  Cluster(Cluster & orig);
  //! std destructor
  virtual  ~Cluster() {};
  //! Fill the hit object with the right infos
  void Build(int lad, int sid, int add, int len, float* sig, float* noi, int* stat, int Sig2NoiRatio, int CNStatus, int PowBits, int badin=0);
  //! reset the cluster object
  void Clear();
  //! Returns the position of the seed in the Signal vector 
  int   GetSeed();
  //! Returns the strip number of the seed
  int   GetSeedAdd();
  //! Returns the signal of the seed strip
  float GetSeedVal();
  //! Returns the SN of the seed strip
  float GetSeedSN();
  //! Returns the total CLe noise (sq. mean)
  float GetTotNoise();
  //! Returns the lenght of the cluster
  int   GetLength();
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

  //! Returns the position of the cluster (Cog), in mm units and after alignment
  double GetAlignedPosition();
  //! Returns the Z position
  double GetZPosition();

  double GetCharge() { return sqrt(GetTotSig())/sqrt(MIPSIG[side]); };
  
  int GoldRegion();

  int GetLenght(float val=1.);

  int GetTDR() { return ladder%100;};
  int GetJinf() { return (int)(ladder/100);};
  
  ClassDef(Cluster,2)
};

#endif
