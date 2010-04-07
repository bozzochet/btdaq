#ifndef __TrDHF_h__
#define __TrDHF_h__

//////////////////////////////////////////////////////////////////////////
///
///\file  TrDHF.h
///\brief Header file of TrDHF class
///
///\class TrDHF
///\brief The Histogram Manager 
///
///\date  2008/02/16 AO  First version
///
//////////////////////////////////////////////////////////////////////////

#include "TkPlane.h"
#include "TkLadder.h"
#include "TkDBc.h"

#include "TrRawCluster.h"
#include "TrCluster.h"
#include "Event.h"

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TObject.h"
#include "TDirectory.h"

//! Class TrDHF Histogram Manager
/*!
  The class TrDHF implements the histogram manager.

  The method Define() provides a way to book automatically many histogram 
  versions of the same variable corresponding to the Tracker TkObjects 
  (planes, ladders, sides, ...) putted in a ordered subdirectories.
  ATTENTION: The pn=0 option creates a unique cumulative plot for the p and 
  n side, while pn=1 creates 2 dipperent plots for the p and n sides.  

  In the method FillAll(), the user is expected to define the rules to fill 
  the histograms. The method FillSet() is provided to fill automagically
  the histos booked with the method Define(). 
*/

using namespace trconst;

class TrDHF : public TNamed {

 protected:
  //! Main directory where to store the histos
  TDirectory* fDir;  
  //! Directories for the layers; 
  TDirectory* fDirLayers[maxlay];  
  //! TObject hash table containing the histos
  map<int,TObject*> fHashTable;

 public:
  TrDHF(TDirectory* Dir, char* name, char* title);
  //! Copy constructor 
  TrDHF(const TrDHF &);
  //! Default destructor 
  ~TrDHF();

  void BookHistos();
  void FillAll(Event* ev);
  //! Add an histogram to the manager
  void Add(TObject* obj);
  //! Returns the pointer to the histogram with the required name
  TObject* Get(char* name);

  //! Returns the pointer to the directory holding the histograms
  inline TDirectory* GetDir() const { return fDir; }

  //! Books 1d / 2d / 3d histo sets (tracker/layers/ladders)
  TDirectory* Define(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn = 0);
  TDirectory* Define(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, 
		     Int_t biny, Double_t lowy, Double_t upy, Int_t sk = 0);
  TDirectory* Define(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
		     Int_t biny, Double_t lowy, Double_t upy, 
		     Int_t binz, Double_t lowz, Double_t upz, Int_t sk = 0);
    //! Books 1d / 2d / 3d tracker histo sets (1/2)
  TDirectory* DefineTracker(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn);
  TDirectory* DefineTracker(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
			    Int_t biny, Double_t lowy, Double_t upy, Int_t pn);
  TDirectory* DefineTracker(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, 
			    Int_t biny, Double_t lowy, Double_t upy, 
			    Int_t binz, Double_t lowz, Double_t upz, Int_t sk = 0);
  //! Books 1d / 2d / 3d layers histo sets (8/16)
  TDirectory* DefineLayers(char* name, char*title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn);
  TDirectory* DefineLayers(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
			   Int_t biny, Double_t lowy, Double_t upy, Int_t pn);
  TDirectory* DefineLayers(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, 
			   Int_t biny, Double_t lowy, Double_t upy, 
			   Int_t binz, Double_t lowz, Double_t upz, Int_t sk = 0);
  //! Books 1d / 2d / 3d ladders histo sets (192/384) 
  TDirectory* DefineLadders(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, Int_t pn);
  TDirectory* DefineLadders(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx,
			    Int_t biny, Double_t lowy, Double_t upy, Int_t pn);
  TDirectory* DefineLadders(char* name, char* title, Int_t binx, Double_t lowx, Double_t upx, 
			    Int_t biny, Double_t lowy, Double_t upy, 
			    Int_t binz, Double_t lowz, Double_t upz, Int_t sk = 0);
  //! Books an Entries histogram
  TDirectory* DefineEntries(char* name, Int_t pn);

  //! Fills a 1d / 2d / 3d histogram (entries/tracker/layers/ladders) 
  void Fill(int pn, TrRawClusterR* cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void Fill(int pn, TrClusterR*    cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d tracker histograms (by cluster)
  void FillTracker(Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillTracker(Int_t pn, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d tracker histograms (by side)
  void FillTracker(Int_t side, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d tracker histograms 
  void FillTracker(char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d layer histograms (by cluster) 
  void FillLayer  (Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLayer  (Int_t pn, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d layer histograms (by side/layer)
  void FillLayer  (Int_t side, Int_t layer, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d layer histograms (by layer)
  void FillLayer  (Int_t layer, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d ladder histograms (by cluster)
  void FillLadder (Int_t pn, TrRawClusterR* cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  void FillLadder (Int_t pn, TrClusterR*    cluster, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d ladder histograms (by side/TkId)
  void FillLadder (Int_t side, Int_t TkId, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills 1d / 2d / 3d ladder histograms (by TkId)
  void FillLadder (Int_t TkId, char *name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);
  //! Fills an Entries histogram (by cluster)
  void FillEntry  (int pn, TrRawClusterR* cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);  
  void FillEntry  (int pn, TrClusterR*    cluster, char* name, Double_t X1, Double_t X2 = 1., Double_t X3 = 1., Double_t w = 1.);  
  
 protected:
  //!Request the error calculation for all the histograms
  void Sumw2();

  ClassDef(TrDHF,1)
};

#endif
