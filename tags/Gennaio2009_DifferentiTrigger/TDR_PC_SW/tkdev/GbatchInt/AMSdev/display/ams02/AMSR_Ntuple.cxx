//  $Id: AMSR_Ntuple.cxx,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_Ntuple                                                          //
// ===========                                                          //
//                                                                      //
// Class to handle ntuple and create link between it and ROOT Tree      //
// and interface to ROOT Tree                                           //
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

#include "AMSR_Ntuple.h"
#include "AMSR_Root.h"
/* #include "AMSR_Rootclasses.h" */
#include "hbook.h"
#include <TTree.h>
//#include <TClonesArray.h>
//#include <TObjString.h>
#include "Debugger.h"
#include <TFile.h>
#include <stdio.h>
#include <iostream.h>

PAWC_DEF PAWC;

EVENTH_DEF blkEventh;
BETA_DEF blkBeta;
CHARGE_DEF blkCharge;
PARTICLE_DEF blkParticle;
TOFCLUST_DEF blkTofclust;
TOFMCCLU_DEF blkTofmcclu;
TRCLUSTE_DEF blkTrcluste;
TRMCCLUS_DEF blkTrmcclus;
TRRECHIT_DEF blkTrrechit;
TRDCLMC_DEF blkTrdclmc;
TRDCL_DEF blkTrdcl;
TRTRACK_DEF blkTrtrack;
TRDTRK_DEF blkTrdtrk;
ECALSHOWER_DEF blkecsh;
MCEVENTG_DEF blkMceventg;
ANTICLUS_DEF blkAnticlus;
ANTIMCCL_DEF blkAntimccl;
LVL3_DEF blkLvl3;
LVL1_DEF blkLvl1;
TRRAWCL_DEF blkTrrawcl;
ANTIRAWC_DEF blkAntirawc;
TOFRAWCL_DEF blkTofrawcl;

static char dir1[] = "//PHY1", *top1 = "PHY1";
static char dir2[] = "//PHY2", *top2 = "PHY2";

AMSR_Ntuple *gAMSR_Ntuple = 0;

ClassImp(AMSR_Ntuple)

//_____________________________________________________________________________
AMSR_Ntuple::AMSR_Ntuple(const char *name, const char *title)
     : TNamed(name,title)
{
   m_DataFileType = kUnknown;
   m_NVar       = 0;
   m_Entries    = 0;
   m_Lun        = 82;
   m_NtupleID   = 1;
   m_NextID     = m_NtupleID;
   m_MemID      = m_NtupleID;
   m_NtupleOpen = kFALSE;
   m_SameRead   = kFALSE;
   m_Tree       = 0;
   
   int cols = sizeof(m_VarNames[0]);
   int rows = sizeof(m_VarNames)/cols;
   for (int i=0; i<rows; i++) {
      m_VarNames[i][cols-1] = 0;
   }
   
   m_BlkEventh   = &blkEventh;
   m_BlkBeta     = &blkBeta;
   m_BlkCharge   = &blkCharge;
   m_BlkParticle = &blkParticle;
   m_BlkTofclust = &blkTofclust;
   m_BlkTofmcclu = &blkTofmcclu;
   m_BlkTrcluste = &blkTrcluste;
   m_BlkTrmcclus = &blkTrmcclus;
   m_BlkTrrechit = &blkTrrechit;
   m_BlkTrdclmc  = &blkTrdclmc;
   m_BlkTrdcl    = &blkTrdcl;
   m_BlkTrdtrk   = &blkTrdtrk;
   m_BlkTrtrack  = &blkTrtrack;
   m_Blkecsh  =    &blkecsh;
   m_BlkMceventg = &blkMceventg;
   m_BlkAnticlus = &blkAnticlus;
   m_BlkAntimccl = &blkAntimccl;
   m_BlkLvl3     = &blkLvl3;
   m_BlkLvl1     = &blkLvl1;
   m_BlkTrrawcl  = &blkTrrawcl;
   m_BlkAntirawc = &blkAntirawc;
   m_BlkTofrawcl = &blkTofrawcl;
   
   if (gAMSR_Ntuple == 0) HLIMIT(NWPAW);

   gAMSR_Ntuple = this;
}

//_____________________________________________________________________________
AMSR_Ntuple::~AMSR_Ntuple()
{

}

//_____________________________________________________________________________
void AMSR_Ntuple::CloseNtuple()
{
   if (m_NtupleOpen) {
      if (m_Lun == 81) {
	 HCDIR(dir1," ");
         HDELET(m_MemID);
         HREND(top1);
      } else {
	 HCDIR(dir2," ");
         HDELET(m_MemID);
         HREND(top2);
      }
      CLOSE(m_Lun);
      m_NtupleOpen = kFALSE;
   }
}


//_____________________________________________________________________________
void AMSR_Ntuple::GetEvent(Int_t event)
{
   //
   //Get one event for Ntuple or Tree according to its data type
   //
   if (event<0 || event>=m_Entries) return;
   
   if (m_DataFileType == kRootFile) 
	{
	  debugger.Print("AMSR_Ntuple::GetEvent:getting event %d\n",event);

	  m_Tree->GetEvent(event);

//           debugger.Print("Run, event: %d %d %d %d \n",evroot02_Header._Run,evroot02_Header._Eventno, evroot02_Header._RawWords, evroot02_Header.Tracks);

// Now copy the stuff over... structures don't align though, sort of a mess

            memcpy(&m_BlkEventh->eventno,&evroot02_Header._Eventno,sizeof(int));
            memcpy(&m_BlkEventh->run,&evroot02_Header._Run,sizeof(int));


	}

   else if (m_DataFileType == kNtupleFile) {

      event++;
      int ierr;
      if (m_SameRead) 
	{ 	
	HGNT(m_MemID, event, ierr);
	}

      else {
            HGNT(m_MemID, event, ierr);

      }
      m_SameRead = kTRUE;

   }

}

//_____________________________________________________________________________
void AMSR_Ntuple::GetRunTime(time_t* time)
{
  time[0] = m_BlkEventh->time[0];
  time[1] = m_BlkEventh->time[1];
}

//_____________________________________________________________________________
Int_t AMSR_Ntuple::OpenNtuple(char *ntpfile)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //
  //  Return values:
  //    0       successful
  //    1       can't open file
  //    2       can't get the specified ntuple
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   
   int reclen = 0;
   int istat, ierr;
   int offset = 0;
   int lun, memID;
   char *top = 0;

   if (m_Lun == 81) {
      lun = 82;
      top = top2;
      offset = 2-m_NextID;
   } else {
      lun = 81;
      top = top1;
      offset = 1-m_NextID;
   }



   HROPEN(lun, top, ntpfile, "XP", reclen, istat);
   if (istat != 0) {
      Error("AMSR_Ntuple::OpenNtuple",
            "Failure in opening ntuple file %s\n",ntpfile);
      CLOSE(lun);
      return 1;
   }


   if (top==top1) HCDIR(dir1, " ");
   else if (top==top2) HCDIR(dir2, " ");

   HRIN(m_NextID, 9999, offset);
   memID = m_NextID+offset;
   istat = EXIST(memID);
   if (istat != 0) {
      Error("AMSR_Ntuple::OpenNtuple",
            "No such ntuple ID =%d\n",m_NextID);
      HREND(top);
      CLOSE(lun);
      //
      //Fails, so resume the previous directory
      //
      if (m_NtupleOpen) {
         if (top==top1) HCDIR(dir2, " ");
         else if (top==top2) HCDIR(dir1, " ");
      }
      return 2;
   }



   //
   //then close the old one after successful opening of new ntuple file
   //
   if (m_NtupleOpen) CloseNtuple();

   //
   //Save current I/O unit, NtupleID, and set m_NtupleOpen
   //Change directory, etc.
   //
   if (top==top1) HCDIR(dir1, " ");
   else if (top==top2) HCDIR(dir2, " ");
   m_NtupleOpen = kTRUE;
   m_Lun        = lun;
   m_MemID      = memID;
   m_NtupleID   = m_NextID;

   //
   //Get entries in the ntuple
   //
   HNOENT(m_MemID, m_Entries);

//   printf("ntuple of %d(MemID) has %d events\n", m_MemID, m_Entries);

   
   //
   //Set block addresses for ntuple
   //
   HBNAME(m_MemID, " ", 0, "$CLEAR");

   HBNAME(m_MemID, "EVENTH", &(m_BlkEventh->eventno), "$SET");
   HBNAME(m_MemID, "BETA", &(m_BlkBeta->nbeta), "$SET");
   HBNAME(m_MemID, "CHARGE", &(m_BlkCharge->ncharge), "$SET");
   HBNAME(m_MemID, "TRDMCCL", &(m_BlkTrdclmc->ntrdclmc), "$SET");
   HBNAME(m_MemID, "TRDClu", &(m_BlkTrdcl->ntrdcl), "$SET");
   HBNAME(m_MemID, "PARTICLE", &(m_BlkParticle->npart), "$SET");
   HBNAME(m_MemID, "TOFCLUST", &(m_BlkTofclust->ntof), "$SET");
   HBNAME(m_MemID, "TOFMCCLU", &(m_BlkTofmcclu->ntofmc), "$SET");
   HBNAME(m_MemID, "TRCLUSTE", &(m_BlkTrcluste->Ntrcl), "$SET");
   HBNAME(m_MemID, "TRMCCLUS", &(m_BlkTrmcclus->ntrclmc), "$SET");
   HBNAME(m_MemID, "TRRECHIT", &(m_BlkTrrechit->ntrrh), "$SET");
   HBNAME(m_MemID, "TRTRACK", &(m_BlkTrtrack->ntrtr), "$SET");
   HBNAME(m_MemID, "TRDTRK", &(m_BlkTrdtrk->ntrdtrk), "$SET");
   HBNAME(m_MemID, "ECALSHOW", &(m_Blkecsh->Necsh), "$SET");
   HBNAME(m_MemID, "MCEVENTG", &(m_BlkMceventg->nmcg), "$SET");
   HBNAME(m_MemID, "ANTICLUS", &(m_BlkAnticlus->nanti), "$SET");
   HBNAME(m_MemID, "ANTIMCCL", &(m_BlkAntimccl->nantimc), "$SET");
   HBNAME(m_MemID, "LVL3", &(m_BlkLvl3->nlvl3), "$SET");
   HBNAME(m_MemID, "LVL1", &(m_BlkLvl1->nlvl1), "$SET");
   HBNAME(m_MemID, "TRRAWCL", &(m_BlkTrrawcl->ntrraw), "$SET");
   HBNAME(m_MemID, "ANTIRAWC", &(m_BlkAntirawc->nantiraw), "$SET");
   HBNAME(m_MemID, "TOFRAWCL", &(m_BlkTofrawcl->ntofraw), "$SET"); 


   //
   //Need to invoke HGNT to enable successful calling of HGNTV
   //
   HGNT(m_MemID, 1, ierr);
   m_SameRead   = kFALSE;
 
   SetTree(0);

   
   return 0;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetBranchStatus(char *varname, Bool_t status)
{
   if(m_Tree)m_Tree->SetBranchStatus(varname,status);
   if (m_DataFileType == kNtupleFile) m_SameRead = kFALSE;
}

//_____________________________________________________________________________
void AMSR_Ntuple::SetTree(TTree *t)
{
   //
   //Ntuple data file and Root Tree data file
   //

   if (t == 0) {      //Ntuple data file

      m_DataFileType = kNtupleFile;

   } else {           //Root Tree data file

      m_Tree    = t;
      m_Entries = m_Tree->GetEntries();
	   debugger.Print("AMSR_Ntuple::SetTree(): no of entries %d\n",
          m_Entries);


      SetTreeAddress();
      m_DataFileType = kRootFile;

      //
      //Read the first event for all branches to ensure late selected read
      //
      m_Tree->GetEvent(0);
   }

}

//_____________________________________________________________________________
void AMSR_Ntuple::SetTreeAddress()
{
   //
   //Create association between ntuple and tree
   //

   if (m_Tree == 0) return;

   //
   //Set branch addresses
   //
   m_Tree->SetMakeClass(1);

   m_Tree->SetBranchAddress("evroot02.",&evroot02);
   m_Tree->SetBranchAddress("evroot02.Header",&evroot02_Header);
   m_Tree->SetBranchAddress("evroot02.fTRtrack",&m_trtrk);
   m_Tree->SetBranchAddress("evroot02.fTRDcluster",&m_trdcl);
   m_Tree->SetBranchAddress("evroot02.fTRDtrack",&m_trdtrk);
   m_Tree->SetBranchAddress("evroot02.fMCeventg",&m_mceventg);

   // Set branch status here

   debugger.Print("Set Tree Address\n");
}

