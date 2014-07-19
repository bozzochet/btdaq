// Authors: P.Zuccon, D.Caraffini - INFN di Perugia
#ifndef DHF_hh
#define DHF_hh
#include "TF1.h"
#include "TH2.h"
#include "TFile.h"
#include "TProfile.h"
#include "TStyle.h"
#include "TObject.h"
#include "TNamed.h"
#include "TDirectory.h"
#include "TROOT.h"
#include "TGraph.h"
#include "Event.hh"
#include "Trace.hh"
#include "Align.hh"
#include "Calib.hh"
#include "RHClass.hh"

//! Class DHF  Histogram Manager
/*!
  The class DHF implements the histogram manager. In the method BookHistos()
  the user can define a set histograms he want to fill at each cut level.

  The method Define(  ) provides a way to book automatically a many of histograms 
  versions of the same histograms corresponding to the different event classes defined in the
  Class Categories  these histograms are putted in a subdirectory

  In the method FillAll(), the user is expected to define the rules to fill 
  the histograms. The method FillSet()  is provided to fill automagically
  the histos booked with the method Define(). 
*/

class DHF : public TNamed
{

 protected:
  int NTdr;
  int TDRNum[24];
  // directoire des histogram
  TDirectory*	fDir;  
  
  // liste des histograms 
  TObjArray *	fHlist;
  TObjArray *   fDHlist0; 
  TObjArray *   fDHlist1; 
  TObjArray *   fDHlist2; 
  TObjArray *   fDHlist3; 
  TObjArray *   fDHlist4; 
  TObjArray *   fDHlist5;
  TObjArray *   fDHlistTop; 

 public:
   
  TFile *fhist;
  Event* Ev;
  int runno;

  // constructeurs  
  DHF(TDirectory * Dir,char * name,char * title,int ntdr,int * tdrnum, int run);
  DHF(char * name,char * title,int ntdr,int * tdrnum,int run);
  //! Copy constructor 
  DHF(const DHF &);
  /* Copy operator */
  //       DHF& operator=(const DHF & rhs);
  //!Default destructor 
  ~DHF();
  //! Initalize the class (call BookHistos plus some other init) 
  virtual void Init(Align* alg);

  //!Sets the pointers to the event and to the current particles
  //  void SetCurrentPart(Event* ev){ Ev=ev;}  

  //! ajoute histogram dans le directoire top
  void AddHisto(TObject* kk);

  //!Returns the pointer to the histogram with the required name
  TObject *GetHist(char* name);
  //!Returns the pointer to the directory holding the histograms
  inline TDirectory*	GetDir()	const {return fDir;}

  //!Fill un histogram TH1D
  void Fill1D(char *histo, Axis_t X,  Stat_t w);
  //!Fill un histogram TH2D
  void Fill2D(char *histo, Axis_t X1,  Axis_t X2, Stat_t w);

  //!Fill all histograms
  virtual void FillSelect(Event* ev, RHClass* rh, Align* alg);
  virtual void FillAll(Event* ev, RHClass* rh, Align* alg);

  //!Books 2d histo sets
  void Define(char*name,char*title,int binx,Axis_t lowx,Axis_t upx,int biny,Axis_t lowy,Axis_t upy,int sk=0,char *dirname=0);
  //!Books 1d histo sets
  void Define(char*name,char*title,int binx,Axis_t lowx,Axis_t upx,int sk ,char*dirname);
  //!Books Profile histo sets
  TDirectory * Define(char*prof,char *name, char *title, int binx, Axis_t lowx, Axis_t upx, int sk=0,char *dirname=0);
  
  //!Fills a 1D or 2D or Profile histogram 
  void Fill(int sk,Cluster *lad,char* histo, Axis_t X1,Axis_t X2=1,Stat_t w=1.);
  //!histos de calibration
  void FillCalib(int ntdr,char* histo, Axis_t X1,Axis_t X2=1,Stat_t w=1.);
 
  virtual void BookHistos(Align* alg);

  void align_xy_positions(Align* alg);
  void align_ref_dyx_dxy(Align* alg);
  void align_dxx_dyy(Align* alg);
  void align_dxy_dyx(Align* alg);
  void residus(Event* ev, Align* alg);
  void residus_petits(Event* ev, Align* alg);
  void residus_echelles(Event* ev, Align* alg);
  void residus_eff(Event* ev, Align* alg, int fin);
  void residus_scan_va(Event* ev, Align* alg, Calib* cal);
  int divergences(Event* ev, Align* alg);

  void hcalib(Calib* cal, RHClass* rh);

  void EcrireHistos(char *ffnom);
  
  
 protected:
  //!Returns the pointer to the container (TObjArray) of the histograms
  inline TObjArray *	GetHlist()	const {return fHlist;}
//!Request the error calculation for all the histograms
  void Sumw2();
  //!User routine to book the histos

  ClassDef(DHF,0)
    };



#endif
