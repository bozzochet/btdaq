//  $Id: NtupleSelectorFor.h,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#ifndef NtupleSelectorFor_H
#define NtupleSelectorFor_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// NtupleSelectorFor                                                    //
// ===========                                                          //
//                                                                      //
// Class to select events in a ntuple and write the selected events     //
//  into a new file. The maximum events in the new ntuple file can be   //
//  set.                                                                //
//                                                                      //
//                                                                      //
// Usage:                                                               //
//                                                                      //
//   1. Create a class, e.g.                                            //
//          NtupleSelectorFor *select = new NtupleSelectorFor()         //
//                                                                      //
//   2. Change default settings (nothing needs to do if use default)    //
//      select->SetMaxEvents(...);  set Maximum events in a new ntuple  //
//      select->SetInsert(...); set insert string in filename for new   //
//           ntuple, the insert string will be inserted before extnsion //
//      select->SetNtupleID(...);  set Ntuple ID, default=1             //
//      select->SetBufferSize(...); set buffer size to save survived    //
//                                                  event#              //
//                                                                      //
//   3. Opening old ntuple file to get events                           //
//      select->OpenOldNtuple(char *oldFile);                           //
//                                                                      //
//   4. Opening new ntuple file to write out after old ntuple opened    //
//      select->OpenNewNtuple(char *oldFile);                           //
//                                                                      //
//   5. Set cut                                                         //
//      Change user fortran usrcut.f in which the cut function USRCUT   //
//      occurs. See in that file, on how to specify user cut.           //
//      select->SetCut(int (*usrcut_)())                                //
//      to tell the poiner of function USRCUT                           //
//                                                                      //
//   6. Open file for user's histgrams                                  //
//      select->OpenUserFile(...);  set filename for user's histograms  //
//                                                                      //
//   7. Searching cut-survived events                                   //
//      select->FindNext();  return the next event# passed cut          //
//      Select->FindNextAndSave(); same, but also save the event#       //
//      select->FindNextAndWrite(); same, but write out the whole event //
//                                                                      //
//   8. Close files                                                     //
//      select->CloseOldNtuple();  close old ntuple file for read       //
//      select->CloseNewNtuple();  close new ntuple file for writing    //
//      select->CloseUserFile();   close user's histogram file          //
//                                                                      //
//   9. Initialization and termination for histgram filling             //
//      Initialization would be done in select->OpenUserFile by calling //
//      USRCUT(-1), so put initialization in USRCUT with IOPT=-1        //
//      Termination done in select->CloseUserFile by calling USRCUT(1)  // 
//      where user's ending-up can put                                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "AMSR_NtupleCommons.h"
#include "usrcom.h"

enum EReadStatus { kNeverRead=0, kReadAll=1, kReadVars=2 };

class NtupleSelectorFor {

 private:
   char          m_VarNames[MXVAR][MXCOL];    //Name list of m_NVar variables
   int           m_NVar;         //Number of vairables in HGNTV
   int           m_MemID;        //Actual ntuple(to read) ID in memory
   int           m_BufferSize;   //Size of buffer to store event# found
   int           m_NBuffer;      //total events stored in buffer
   int          *m_PBuffer;      //pointer to the array of buffer
   int           m_MaxEvents;    //Maximum allowed events in new ntuple
   char         *m_Insert;       //Insert string in output ntuple filename
   char         *m_NewDir;       //directory to write new ntuple files
   int           m_RecLen;       //Record length for new ntuple

 protected:
   int           m_OldEntries;   //Entries in ntuple
   char         *m_OldFile;      //Filename of source ntuple to read
   int           m_OldLun;       //I/O unit for ntuple file
   int           m_NewEntries;   //Entries in ntuple
   char         *m_NewFile;      //Filename of ntuple to write into
   int           m_NewLun;       //I/O unit for ntuple file
   int           m_Event;        //last read event#
   int           m_NtupleID;     //Ntuple ID to read and write
   EReadStatus   m_LastRead;     //Same variables reading for HGNTF
//   int         (*m_Cut)(float);    //cut used in FindNext
   int         (*m_Cut)(int);          //cut used in FindNext
   char         *m_UsrFile;      //Filename of user's histogram file
   int           m_UsrLun;       //I/O unit for user's histogram file

   EVENTH_DEF   *m_BlkEventh;   //Pointers to common blocks used by CWN ntuple
   BETA_DEF     *m_BlkBeta;     // according to CWN block name
   CHARGE_DEF   *m_BlkCharge;
   PARTICLE_DEF *m_BlkParticle;
   TOFCLUST_DEF *m_BlkTofclust;
   TOFMCCLU_DEF *m_BlkTofmcclu;
   TRCLUSTE_DEF *m_BlkTrcluste;
   TRMCCLUS_DEF *m_BlkTrmcclus;
   TRRECHIT_DEF *m_BlkTrrechit;
   TRTRACK_DEF  *m_BlkTrtrack;
   MCEVENTG_DEF *m_BlkMceventg;
   CTCCLUST_DEF *m_BlkCtcclust;
   CTCMCCLU_DEF *m_BlkCtcmcclu;
   ANTICLUS_DEF *m_BlkAnticlus;
   ANTIMCCL_DEF *m_BlkAntimccl;
   LVL3_DEF     *m_BlkLvl3;
   LVL1_DEF     *m_BlkLvl1;
   CTCHIT_DEF   *m_BlkCtchit;
   TRRAWCL_DEF  *m_BlkTrrawcl;
   ANTIRAWC_DEF *m_BlkAntirawc;
   TOFRAWCL_DEF *m_BlkTofrawcl;
   
 protected:
   void          SetAddresses(int id);
   
 public:
                 NtupleSelectorFor();
                ~NtupleSelectorFor();
   void          ClearBuffer() { m_NBuffer=0;}
   void          CloseOldNtuple();
   void          CloseNewNtuple();
   void          CloseUserFile();
   int           FindNext();
   int           FindNextAndSave();
   int           FindNextAndWrite();
//   int         (*GetCut())(float) { return m_Cut;}
   int         (*GetCut())(int) { return m_Cut;}
   int           GetNewEntries()    { return m_NewEntries;}
   int           GetNBuffer()       { return m_NBuffer;}
   int           GetOldEntries()    { return m_OldEntries;}
   int           GetEventNum()      { return m_Event;}
   int           GetEvtNum()     { return m_BlkEventh->eventno;}
   int           GetNewLun()        { return m_NewLun;}
   int           GetNtupleID()     { return m_NtupleID;}
   int           GetOldLun()        { return m_OldLun;}
   int          *GetBufferPointer() { return m_PBuffer;}
   int           GetBufferSize()    { return m_BufferSize;}
   int           GetRunNum()     { return m_BlkEventh->run;}
   void          GetEventAll(int event);
   void          GetEventVars(int event);
   char         *GetInsert()      { return m_Insert;}
   char         *GetNewFilename() { return m_NewFile;}
   int           OpenNewNtuple(char *ntpfile);
   int           OpenOldNtuple(char *ntpfile);
   int           OpenUserFile (char *usrfile);
//   void          SetCut(int (*cut)(float)) { m_Cut = cut;}
   void          SetCut(int (*cut)(int)) { m_Cut = cut;}
   void          SetBsize(int size);
   void          SetBufferSize(int size);
   void          SetEvent(int event) { m_Event=event;}
   void          SetInsert(char *string) { m_Insert=string;}
   void          SetNewDir(char *dir) { m_NewDir=dir;}
   void          SetMaxEvents(int nevts) { m_MaxEvents = nevts;}
   void          SetNtupleID(int id) { m_NtupleID = id;}
   void          SetRecLen(int reclen) { m_RecLen = reclen;}
   void          WriteBuffer();
   int           WriteEvent();

};

#endif
