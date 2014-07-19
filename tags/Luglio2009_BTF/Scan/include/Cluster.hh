#ifndef Cluster_hh
#define Cluster_hh
#include "TObject.h"
#include "Align.hh"
#define MAXLENGTH 50
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
  //! the first strip in readout order for position determination with eta function
  int piste_eta;
  int piste_eta3;
  //! Nstrips of the clusters 
  int length;
  //! Nombre des pistes utilise pour la position
  int npiste;
  //! Vector of the cluster signals
  float Signal[MAXLENGTH];
  //! Vector of the noise of the cluster strips
  float Noise[MAXLENGTH];
  //! Vector of the status of the cluster strips
  int Status[MAXLENGTH];
  //! Ladder (0-1 upstream, 2-5 downstream)
  int ladder;
  //! side (0=S, 1=K)
  int side; //0 S / 1 K
  //! cluster > MAXLENGHT strips
  int bad;
  //! Golden Flag
  int golden;

  //! std constructor (create an empty cluster)
  Cluster();
  //! copy constructor 
  Cluster(Cluster & orig);
  //! std destructor
  virtual  ~Cluster() {};
  //! Fill the hit object with the right infos
  void Build(int lad, int sid,int add,int len,float* sig,float* noi,int* stat,int badin=0);
  //! reset the cluster object
  void Clear();
  //! Returns the position of the seed in the Signal vector 
  int   GetSeed();
  //! Returns the status of the seed strip 
  int   GetSeedStatus();
  //! Returns the strip number of the seed
  int   GetSeedAdd();
  //! Returns the signal of the seed strip
  float GetSeedVal();
  //! Returns the SN of the seed strip
  float GetSeedSN();
  //! Returns the total CLe noise (sq. mean) for strips with s/n >= seuil*noise
  float GetTotNoise(float seuil);
  //! Returns the length of the cluster
  int GetLength();
  //! Calculate the CoG of the cluster using 2 strips
  float GetCoG();
  //! Calculate the CoG of the cluster using n strips
  float GetCoGN(int n);
  //! Calculate position in gap via eta function
  float GetPositionEtaP(Align* alg, int det);
  float GetPositionEtaN(Align* alg, int det);
  float GetPositionEta3N(Align* alg, int det);
  //! Returns the Total Signal of the cluster for strips with s/n >= seuil*noise
  float GetTotSig(float seuil);
  //! Return the Cluster SN using the strips with s/n >= seuil*noise
  float GetTotSN(float seuil);  
  //! Calculate the eta (0.,1.) selon s1/(s1+s2)
  float GetEtaTrue();
  //! Calculate the eta (0.,2.) selon (s2+2*s3/(s1+s2+s3)
  float GetEta3();
  //! Calculate the eta (0.,1.)
  float GetEta();
  //! Calculate Etaraw (-0.5,0.5)
  float GetEtaRaw();
  //! conversion echelle a TDR (0-23)
  //  int GetTDR();
  //! printout the cluster infos
  void Print();
  int GoldRegion();



  int GetLargeur(float val=1.);

  ClassDef(Cluster,2)
};

#endif
