//  $Id: AMSR_Ntuple.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
#ifndef AMSR_Ntuple_H
#define AMSR_Ntuple_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Ntuple                                                          //
// ===========                                                          //
//                                                                      //
// Class to handle ntuple and make association between ntuple and Root  //
//  tree and acts as an interface to ROOT tree when data is ROOT tree   //
//                                                                      //
// SetTree(TTree*) used to create link between ntuple and ROOT Tree     //
//                                                                      //
// SetBranchStatus(char *varname, Bool_t status) simulates TTree to     //
// set ActiveBranches in order to fasten ntuple data accessing          //
//                                                                      //
// Usage:                                                               //
//                                                                      //
//   1. Create a class, e.g., AMSR_Ntuple *ntp = new AMSR_Ntuple();     //
//   Only one object of AMSR_Ntuple should be created !                 //
//                                                                      //
//   2. Initialize data file :                                          //
//      For ROOT file, ntp->SetTree(TTree *t) to set up address.        //
//      For ntuple file, ntp->OpenNtuple(ntpfile) and SetTree(0) to     //
//   open file and set link between ntuple and ROOT tree. The ntuple ID //
//   is 1 by default which can be changed via "ntp->SetNtupleID(id)".   //
//                                                                      //
//   3. Read event: ntp->GetEvent(event).                               //
//                                                                      //
//   4. Select variables/branches for reading                           //
//      ntp->SetBranchStatus(varname, status) to (de)activate a variable//
//   or branch for reading. When varname == "*", all variables/branches //
//   will be invoked. e.g. ntp->SetBranchStatus("*",1) to activate all  //
//   variables/branches.                                                //
//                                                                      //
//   5. To impose cuts on ntuple/tree, create object of TTreeFormula    //
//  to tree which can be get "GetTree()".                               //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNamed.h>
#include <TClonesArray.h>
#ifndef __CINT__
 #include "AMSR_NtupleCommons.h"
// #include "AMSR_Rootclasses.h"
#endif
#ifndef AMSR_Types_H
#include "AMSR_Types.h"
#endif
#include <time.h>
#include "EventRoot02.h"
#include "AMSEventHeaderRoot.h"
#include "TRDClusterRoot.h"
#include "TRDTrackRoot.h"
#include "TrTrackRoot02.h"

class TTree;
class TFile;

class AMSR_Ntuple : public TNamed {

 private:
   Text_t        m_VarNames[500][20];    //Name list of m_NVar variables
   Int_t         m_NVar;        //Number of vairables in HGNTV
   Int_t         m_MemID;       //Actual ntuple ID in memory

 protected:
   EDataFileType m_DataFileType;//Data file type to access
   Int_t         m_Entries;     //Entries in ntuple or Root-tree
   Int_t         m_Lun;         //I/O unit for ntuple file
   Int_t         m_NtupleID;    //Ntuple ID for current ntuple
   Int_t         m_NextID;      //Ntuple ID for new ntuple file
   Bool_t        m_NtupleOpen;  //Indicator whether already ntuple open
   Bool_t        m_SameRead;    //Same variables reading for HGNTF
   TTree        *m_Tree;        //ROOT TTree associated with ntuple

   EventRoot02 evroot02;
   AMSEventHeaderRoot evroot02_Header;
   TClonesArray* m_trdcl;
   TClonesArray* m_trdtrk;
   TClonesArray* m_trtrk;
   TClonesArray* m_mceventg;
   

#ifndef __CINT__
   EVENTH_DEF   *m_BlkEventh;   //Pointers to common blocks used by CWN ntuple
   BETA_DEF     *m_BlkBeta;     // according to CWN block name
   CHARGE_DEF   *m_BlkCharge;
   PARTICLE_DEF *m_BlkParticle;
   TOFCLUST_DEF *m_BlkTofclust;
   TOFMCCLU_DEF *m_BlkTofmcclu;
   TRCLUSTE_DEF *m_BlkTrcluste;
   TRMCCLUS_DEF *m_BlkTrmcclus;
   TRRECHIT_DEF *m_BlkTrrechit;
   TRDCLMC_DEF  *m_BlkTrdclmc;
   TRDCL_DEF    *m_BlkTrdcl;
   TRTRACK_DEF  *m_BlkTrtrack;
   TRDTRK_DEF   *m_BlkTrdtrk;
   MCEVENTG_DEF *m_BlkMceventg;
   ANTICLUS_DEF *m_BlkAnticlus;
   ANTIMCCL_DEF *m_BlkAntimccl;
   LVL3_DEF     *m_BlkLvl3;
   LVL1_DEF     *m_BlkLvl1;
   TRRAWCL_DEF  *m_BlkTrrawcl;
   ANTIRAWC_DEF *m_BlkAntirawc;
   TOFRAWCL_DEF *m_BlkTofrawcl;
   ECALSHOWER_DEF  *m_Blkecsh;

#endif
   
 protected:
   void          SetTreeAddress();
   void          SetVarNames();
   
 public:
//                 AMSR_Ntuple();
                 AMSR_Ntuple(const char *name="AMSR_Ntuple",
			     const char *title="The AMS Ntuple");
                ~AMSR_Ntuple();
   void          CloseNtuple();
   Int_t         GetEntries()    { return m_Entries;}
   void          GetEvent(Int_t event);
   Int_t         GetEvtNum()     { return m_BlkEventh->eventno;}
   Int_t         GetNextID()     { return m_NextID;}
   Int_t         GetLun()        { return m_Lun;}
   Int_t         GetRunNum()     { return m_BlkEventh->run;}
   void          GetRunTime(time_t* time);
   Int_t         GetRunType()    { return m_BlkEventh->runtype;}
   Int_t         GetNtupleID()   { return m_NtupleID;}
   TTree        *GetTree()       { return m_Tree;}
   Bool_t        IsNtupleOpen()  { return m_NtupleOpen;}
   Int_t         OpenNtuple(char *ntpfile);
   void          SetBranchStatus(char *varname, Bool_t status);
   void          SetNtupleID(Int_t id) { m_NextID = id;}
   void          SetTree(TTree *t=0);

   friend   class    AMSR_AntiClusterReader;
   friend   class    AMSR_MCParticleReader;
   friend   class    AMSR_ParticleReader;
   friend   class    AMSR_SiHitReader;
   friend   class    AMSR_ToFClusterReader;
   friend   class    AMSR_TrdClusterReader;
   friend   class    AMSR_TrdTrackReader;
   friend   class    AMSR_TrMCClusterReader;
   friend   class    AMSR_TrackReader;
   friend   class    AMSR_EcalShowerReader;

   ClassDef(AMSR_Ntuple, 0)   //AMSR_Ntuple ntuple handling class
};

#endif
