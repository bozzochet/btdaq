#ifndef Event_hh
#define Event_hh
  
#include "TObject.h"
#include "TClonesArray.h" 
#include "Cluster.hh"
#include "TLinearFitter.h"

#define NTDRS 24
#define NJINF 2

//! Run Header Class
class RHClass:public TObject{

public:
  //! Run number
  int Run;
  char date[30];
  int nJinf;
  int JinfMap[NJINF];
  int ntdrRaw;
  int tdrRawMap[24];
  int ntdrCmp;
  int tdrCmpMap[24];
  double CNMean[NTDRS][16];
  double CNSigma[NTDRS][16];

  
  //! default constructor
  RHClass(){  
  Run=0;
  ntdrRaw=0;
  ntdrCmp=0;
  nJinf=0;
  sprintf(date," ");
  memset(JinfMap,-1,NJINF*sizeof(JinfMap[0]));
  memset(tdrRawMap,-1,NTDRS*sizeof(tdrRawMap[0]));
  memset(tdrCmpMap,-1,NTDRS*sizeof(tdrCmpMap[0]));

  for (int ii=0;ii<NTDRS;ii++)
    for (int jj=0;jj<16;jj++){
      CNMean[ii][jj]=0.;
      CNSigma[ii][jj]=0.;
    }
  
};
  //! default destructor
  virtual ~RHClass(){};
  //! conversion
  int FindPos(int tdrnum);
  //! Prints the Header infos
  void Print();
 
  ClassDef(RHClass,1)
};

//!  Tracker Event class. 
/*!  Tracker Event class contains all the information about a Event
 */


class Event:public TObject{

public:
  //! Progressive Event number
  int Evtnum;
  //! Jinj Status 
  int JINJStatus;
  //! Jinf Status 
  int JINFStatus[NJINF];
  //! Status word for the TDRs (  TDRStatus & 0x1f == TDR ID)
  int TDRStatus[NTDRS];
  double CNoise[NTDRS][16];
  //! Cluster number for (ladder 0-5, side 0(S) 1(K))
  int NClus[NTDRS][2];
  //! Total number of clusters
  int NClusTot;
  //! 0 if there are hits on all the ladders
  int notgood;
  //! (TClones) Array of the recontructed clusters
  TClonesArray *Cls;
  //! (TClones) Array of the recontructed hits

  /*! The frame of reference is X Horizontal (dir. from Saleve to Jura) 
                                Y Vertical    (dir from Top to Bottom) 
                                Z along the Beam (dir. upstream to downstream)
  */
  double x[4];
  double y[4];
  double z[4];
   //! Hard Z position
  static double hz[4];
 //!number of strips in the cluster 0-y 1-x
  int nstrip[2][4];

  static double algpar[11][4];
  /* alignement 0 scan 50296
     alignement 1 run 60067
     alignement 2 run 60625 
     alignement 3 run 60592 (a)*/
  static double algpar_fich[8][11][4];

  //!number of TDRS in raw mode
  int rawtdrnum;
  //! pointer to the data
  short int Signal[8][1024];   //

  short int ReadTDR[NTDRS];
  
  //! Default contructor

  Event(int tdrrawnum=0);
  //! Default destructor
  ~Event();

  //! Clear the event
  void Clear();
 
  //! Add an Cluster to the array
  Cluster* AddCluster(int lad,int side);
  //! Get the Cluster in the postion ii of the array
  Cluster* GetCluster(int ii);
  //! selection seule amas cote s =1, k =2, s et k =3 
  int bon_evenement(RHClass* rh);
 
  int NGoldenClus(int lad, int side);

  // mise-a-jour des centres du faisceau
  static void MaJXYpos(int mode,double x00,double y00,double x01,double y01,double x02,double y02,double x03,double y03) {
  // en cm
    if (mode==0) {
      algpar[0][0] = x00;
      algpar[1][0] = y00;
      algpar[0][1] = x01;
      algpar[1][1] = y01;
      algpar[0][2] = x02;
      algpar[1][2] = y02;
      algpar[0][3] = x03;
      algpar[1][3] = y03;
    }
    if (mode==1) {
      algpar[0][0]+=x00;
      algpar[1][0]+=y00;
      algpar[0][1]+=x01;
      algpar[1][1]+=y01;
      algpar[0][2]+=x02;
      algpar[1][2]+=y02;
      algpar[0][3]+=x03;
      algpar[1][3]+=y03;
    }
    return;
}
  // mise-a-jour de la correction pour l'angle entre les axes des detecteurs 0 et 1
  static void MaJRot01(double pente1,double inter1, double pente2, double inter2) {
    algpar[6][0] = pente1;
    algpar[7][0] = inter1;
    algpar[8][0] = pente2;
    algpar[9][0] = inter2;
    return;
  }
  // mise-a-jour de la correction pour l'angle entre les axes parelleles des detecteurs 2 et 3
  static void MaJRotxxyy(double pente1,double inter1, double pente2, double inter2,
                         double pente3,double inter3, double pente4, double inter4) {
    algpar[2][2] = pente1;
    algpar[3][2] = inter1;
    algpar[4][2] = pente2;
    algpar[5][2] = inter2;
    algpar[2][3] = pente3;
    algpar[3][3] = inter3;
    algpar[4][3] = pente4;
    algpar[5][3] = inter4;
    return;
  }
  // mise-a-jour de la correction pour l'angle entre les axes dans le plan xy pour 2 et 3
  static void MaJRotxyyx(double pente1,double inter1, double pente2, double inter2,
                         double pente3,double inter3, double pente4, double inter4) {
    algpar[6][2] = pente1;
    algpar[7][2] = inter1;
    algpar[8][2] = pente2;
    algpar[9][2] = inter2;
    algpar[6][3] = pente3;
    algpar[7][3] = inter3;
    algpar[8][3] = pente4;
    algpar[9][3] = inter4;
    return;
  }
  // retourne positons apres alignement i-detecteur(0-3), j-0(x),1(y),2(z)
  double apos(int i, int j) {
    // en cm
    if (j==0) {
       double val = x[i]-algpar[0][i];
      //       printf("even %d i %d  y % e y0 %e diff %e\n",nevent,i,x[i],algpar[0][i],val);
       //       if (i==3) val-=(algpar[2][i]*(x[i]-algpar[0][i])+algpar[3][i]);
       if (i==3) val-=(algpar[6][i]*(y[i]-algpar[1][i])+algpar[7][i]);
       //       if (i==3) val-=(algpar[2][i]*(x[i]-algpar[0][i])+algpar[3][i]);
       if (i==2) val-=(algpar[6][i]*(y[i]-algpar[1][i])+algpar[7][i]);
       if (i==0) val-=(algpar[6][i]*(y[i]-algpar[1][i])+algpar[7][i]);
       return (val);
    }
    if (j==1) {
      double val = y[i]-algpar[1][i];
      //      printf("i %d  y % e y0 %e diff %e\n",i,y[i],algpar[1][i],val);
      return (val);
    }
    if (j==2) return(hz[i]);
    return(-1.);
  }

  // retourne positons apres alignement i-detecteur(0-3), j-0(x),1(y),2(z)
  double apos(int h, int i, int j) {
    // en cm
    //    printf("j %d\n",j);
    if (j==0) {
       double val = x[i]-algpar_fich[h][0][i];
      //       printf("even %d i %d  y % e y0 %e diff %e\n",nevent,i,x[i],algpar_fich[h][0][i],val);
       if (i==3) val-=(algpar_fich[h][2][i]*(x[i]-algpar_fich[h][0][i])+algpar_fich[h][3][i]);
       if (i==3) val-=(algpar_fich[h][6][i]*(y[i]-algpar_fich[h][1][i])+algpar_fich[h][7][i]);
       if (h==2 && i==2) val-=(algpar_fich[h][6][i]*(y[i]-algpar_fich[h][1][i])+algpar_fich[h][7][i]);
       if (i==0) val-=(algpar_fich[h][6][i]*(y[i]-algpar_fich[h][1][i])+algpar_fich[h][7][i]);
       return (val);
    }
    if (j==1) {
      //      printf("gy %f\n",val);
      double val = y[i]-algpar_fich[h][1][i];
      //      printf("i %d  y % e y0 %e diff %e\n",i,y[i],algpar_fich[h][1][i],val);
      return (val);
    }
    if (j==2) return(hz[i]);
    return(-1.);
  }

  static void EcrireAlgPar(char *filename);

  static void LireAlgPar();

  static void LireFichAlgPar();

  // position x a 'k' du ligne entre 'i' and 'j'
  double ligne_projx(int i, int j, int k);
  // position y a 'k' du ligne entre 'i' and 'j'
  double ligne_projy(int i, int j, int k);

  // position x a z du ligne entre 'i' and 'j'
  double ligne_projxz(int i, int j, float z);
  // avec alignement h
  double ligne_projxz(int h, int i, int j, float z);
  // position y a z du ligne entre 'i' and 'j'
  double ligne_projyz(int i, int j, float z);
  // avec alignement h
  double ligne_projyz(int h, int i, int j, float z);

  // divergence dx/dz entre positions i,j
  double div_dxdz(int i, int j);
  // avec alignement h
  double div_dxdz(int h, int i, int j);
  // divergence dy/dz entre positions i,j
  double div_dydz(int i, int j);
  // avec alignement h
  double div_dydz(int h, int i, int j);

  // divergence dx/dz entre positions 1-3 avec fit des trois points
  double div_dxdz_lin();
  // avec alignement h
  double div_dxdz_lin(int h);
  // divergence dy/dz entre positions 1-3 avec fit des trois points
  double div_dydz_lin();
  // avec alignement h
  double div_dydz_lin(int h);

  ClassDef(Event,1)

};



#endif
