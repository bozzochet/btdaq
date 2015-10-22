#ifndef Cluster_hh
#define Cluster_hh
#include "TObject.h"
#define MAXLENGHT 128
#define NTDRS  24

//!  Tracker Cluster class. 
/*!  Tracker Cluser class contains all the information about a Cluster
 */

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
  //! Ladder (0-1 upstream, 2-5 downstream)
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
  int GoldRegion();



  int GetLenght(float val=1.);

  ClassDef(Cluster,2)
};

#endif
