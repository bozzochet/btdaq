//  $Id: NtupleSelectorFor.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include "NtupleSelectorFor.h"
#include "hbook.h"
//#include "usrcom.h"
#include <iostream.h>
#include <stdio.h>
#include <string.h>

PAWC_DEF PAWC;

// USRCOM_DEF usrCom;  //initialized already in FORTRAN BLOCK DATA

EVENTH_DEF blkEventh;
BETA_DEF blkBeta;
CHARGE_DEF blkCharge;
PARTICLE_DEF blkParticle;
TOFCLUST_DEF blkTofclust;
TOFMCCLU_DEF blkTofmcclu;
TRCLUSTE_DEF blkTrcluste;
TRMCCLUS_DEF blkTrmcclus;
TRRECHIT_DEF blkTrrechit;
TRTRACK_DEF blkTrtrack;
MCEVENTG_DEF blkMceventg;
CTCCLUST_DEF blkCtcclust;
CTCMCCLU_DEF blkCtcmcclu;
ANTICLUS_DEF blkAnticlus;
ANTIMCCL_DEF blkAntimccl;
LVL3_DEF blkLvl3;
LVL1_DEF blkLvl1;
CTCHIT_DEF blkCtchit;
TRRAWCL_DEF blkTrrawcl;
ANTIRAWC_DEF blkAntirawc;
TOFRAWCL_DEF blkTofrawcl;

static char dir_old[] = "//OLD", *top_old = "OLD";
static char dir_new[] = "//NEW", *top_new = "NEW";
static char dir_usr[] = "//USR", *top_usr = "USR";      // for user's histogram
static char *defInsert = ".s";
static char *defNewDir = "./";
static int newFileLen = 100;

static NtupleSelectorFor *gNtupleSelectorFor = 0;

//_____________________________________________________________________________
NtupleSelectorFor::NtupleSelectorFor()
{
   m_RecLen      = 8000;

   m_OldEntries  = 0;
   m_NewEntries  = 0;
   
   m_OldFile     = 0;
   m_NewFile     = new char[newFileLen];
   m_UsrFile     = 0;
   m_NewFile[0]  = 0;
   m_Insert      = defInsert;
   m_NewDir      = defNewDir;
   
   m_OldLun      = 81;
   m_NewLun      = 82;
   m_UsrLun      = 83;
   
   m_Event       = -1;
   m_NtupleID    = 1;
   m_MemID       = m_NtupleID + 1;
   m_LastRead    = kNeverRead;
   m_Cut         = 0;

   m_NVar        = usrCom.nvar;
   int cols = sizeof(m_VarNames[0]);
   int rows = sizeof(m_VarNames)/cols;
   if (m_NVar > rows) {
//      fprintf(stderr, "Too much variables %d > maximum %d allowed !\n",
//              m_NVar, rows);
      cerr << "Read " << m_NVar << "variabless, >" << rows
           << " allowed maximum" << endl;
      m_NVar = rows;
   }

   int leng;
   int cols_1 = cols -1;
   char *varname, *space;
   for (int i=0; i<m_NVar; i++) {
      varname = usrCom.varlis[i];
      space = strchr(varname,' ');
      if (space == 0) leng = cols;
      else leng = strlen(varname) - strlen(space);
      if (leng > cols_1) {
//         fprintf(stderr, "Too long variable name %d > maximum %d allowed !\n",
//                 leng, cols_1);
         cerr << "Variable name length " << leng << " >" << cols_1
              << " allowed maximum" << endl;
         continue;
      }
      strncpy(m_VarNames[i], varname, leng);
   }

   cout << "m_NVar = " << m_NVar << endl;

   m_NBuffer     = 0;
   m_BufferSize  = 1000;
   m_PBuffer     = new int[m_BufferSize];
   m_MaxEvents   = 10000;
   
   m_BlkEventh   = &blkEventh;
   m_BlkBeta     = &blkBeta;
   m_BlkCharge   = &blkCharge;
   m_BlkParticle = &blkParticle;
   m_BlkTofclust = &blkTofclust;
   m_BlkTofmcclu = &blkTofmcclu;
   m_BlkTrcluste = &blkTrcluste;
   m_BlkTrmcclus = &blkTrmcclus;
   m_BlkTrrechit = &blkTrrechit;
   m_BlkTrtrack  = &blkTrtrack;
   m_BlkMceventg = &blkMceventg;
   m_BlkCtcclust = &blkCtcclust;
   m_BlkCtcmcclu = &blkCtcmcclu;
   m_BlkAnticlus = &blkAnticlus;
   m_BlkAntimccl = &blkAntimccl;
   m_BlkLvl3     = &blkLvl3;
   m_BlkLvl1     = &blkLvl1;
   m_BlkCtchit   = &blkCtchit;
   m_BlkTrrawcl  = &blkTrrawcl;
   m_BlkAntirawc = &blkAntirawc;
   m_BlkTofrawcl = &blkTofrawcl;
   
   if (gNtupleSelectorFor == 0) HLIMIT(NWPAW);

   gNtupleSelectorFor = this;
}

//_____________________________________________________________________________
NtupleSelectorFor::~NtupleSelectorFor()
{
   if (m_PBuffer != 0) delete[] m_PBuffer;
   if (m_NewFile != 0) delete[] m_NewFile;
}

//_____________________________________________________________________________
void NtupleSelectorFor::CloseNewNtuple()
{
   if (m_NewFile[0] == 0) return;

   if (m_NBuffer != 0 && m_NewEntries < m_MaxEvents) WriteBuffer();

   m_LastRead = kNeverRead;      //necessary to avoid strange trouble

   HCDIR(dir_new," ");
   int icycle;
   HROUT(m_NtupleID, icycle, "T");
//   HROUT(m_NtupleID, icycle, " ");
   HREND(top_new);
   CLOSE(m_NewLun);
   m_NewFile[0] = 0;
   m_NewEntries = 0;

   if (m_OldFile !=0 ) HCDIR(dir_old," ");
}

//_____________________________________________________________________________
void NtupleSelectorFor::CloseOldNtuple()
{
   if (m_OldFile == 0) return;

   if (m_NBuffer != 0) WriteBuffer();

//   HCDIR(dir_old," ");
   HREND(top_old);
   CLOSE(m_OldLun);
   m_OldFile    = 0;
   m_OldEntries = 0;

   if (m_NewFile[0] != 0) HCDIR(dir_new," ");
}

//_____________________________________________________________________________
void NtupleSelectorFor::CloseUserFile()
{
   //
   // Now call user's termination routine, such as printing
   //
   if (m_Cut != 0) m_Cut(1);

   if (m_UsrFile == 0) return;

   HCDIR(dir_usr, " ");

   int icycle;
   HROUT(0, icycle, "T");
   HREND(top_usr);   
   CLOSE(m_UsrLun);
   m_UsrFile    = 0;

   if (m_NewFile[0] != 0) HCDIR(dir_new," ");
}

//_____________________________________________________________________________
int NtupleSelectorFor::FindNext()
{
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //
   // Return values:
   //   >=0  successful
   //   -1   cut not yet set
   //   -2   fail to find event survived in m_Cut after current event
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   if (m_Cut == 0) {
//      fprintf(stderr,"NtupleSelectorFor::FindNext, cut not set !!\n");
      cerr << "NtupleSelectorFor::FindNext, cut not set !!" << endl;
      return -1;
   }

   for (int i=m_Event+1; i<m_OldEntries; i++) {
      GetEventVars(i);
      if (m_Cut(0) != 0) {
	 return i;
      }
   }
   
   return -2;
}

//_____________________________________________________________________________
int NtupleSelectorFor::FindNextAndSave()
{
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //
   // Return values:
   //   >=0  successful
   //   -1   cut not yet set
   //   -2   fail to find event survived in m_Cut after current event
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   int found = FindNext();

   if (found >= 0) {
      if (m_NBuffer >= m_BufferSize) WriteBuffer();
      m_PBuffer[m_NBuffer++] = found;
   }
   
   return found;
}


//_____________________________________________________________________________
int NtupleSelectorFor::FindNextAndWrite()
{
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
   //
   // Return values:
   //   >=0  successful
   //   -1   cut not yet set
   //   -2   fail to find event survived in m_Cut after current event
   //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   int found = FindNext();

   if (found >= 0) {
      GetEventAll(found);
      WriteEvent();
   }
   
   return found;
}

//_____________________________________________________________________________
void NtupleSelectorFor::GetEventAll(int event)
{
   if (event<0 || event>=m_OldEntries) return;
   
   int ierr;
   if (m_LastRead == kReadAll) HGNTF(m_MemID, event+1, ierr);
   else HGNT(m_MemID, event+1, ierr);
   m_LastRead = kReadAll;

   m_Event = event;
   return;
}

//_____________________________________________________________________________
void NtupleSelectorFor::GetEventVars(int event)
{
   if (event<0 || event>=m_OldEntries) return;
   
   if (m_NVar == 0) {
      GetEventAll(event);
//      fprintf(stderr,"ERROR 0 variable in GetEventVars, nothing read !\n");
//      cerr << "ERROR 0 variable in GetEventVars, nothing read !" << endl;
      return;
   }

   int ierr;
   if (m_LastRead == kReadVars) {
      HGNTF(m_MemID, event+1, ierr);
   } else {
      HGNTV(m_MemID, m_VarNames, m_NVar, event+1, ierr);
      m_LastRead = kReadVars;
   }

   m_Event = event;
   return;
}

//_____________________________________________________________________________
int NtupleSelectorFor::OpenNewNtuple(char *ntpfile)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //
  //  Return values:
  //    0       successful
  //    1       can't open file
  //    2       can't duplicate the specified ntuple
  //    3       old ntuple file not opened first
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

// static const  int reclen = 8000;
   int istat;

   if (m_OldFile == 0) {
//      printf("Open old ntuple first\n");
      cerr << "OpenNewNtuple must be after OpenOldNtuple" << endl;
      return 3;
   }

   if (m_NewFile[0] != 0 && ntpfile !=0 ) {
      CloseNewNtuple();
   }

   if (ntpfile == 0) ntpfile = m_NewFile;

   if (ntpfile == 0 || strlen(ntpfile) == 0) {
//      printf("NULL filename in NtupleSelectorFor::OpenNewNtuple\n");
      cerr << "NULL filename in NtupleSelectorFor::OpenNewNtuple" << endl;
      return 1;
   }

//   printf("Opening new ntuple file =%s\n",ntpfile);
   cout << "Opening new ntuple file =" << ntpfile << endl;

   HROPEN(m_NewLun, top_new, ntpfile, "NP", m_RecLen, istat);
   if (istat != 0) {
//      fprintf(stderr,"OpenNewNtuple, Failure in opening ntuple file %s\n",
//              ntpfile);
      cerr << "Failure in opening new ntuple file =" << ntpfile << endl;
      CLOSE(m_NewLun);
      return 1;
   }

//   HNTDUP(m_MemID, m_NtupleID, 0, "AMS Selected Ntuple", " ");
//   HNTDUP(m_MemID, m_NtupleID, -1, "AMS Selected Ntuple", " ");
   HNTDUP(m_MemID, m_NtupleID, m_RecLen, "AMS Selected Ntuple", " ");

   SetAddresses(m_NtupleID);
   m_LastRead = kNeverRead;      //necessary to avoid strange trouble

   if (ntpfile != m_NewFile) {
     int leng = strlen(ntpfile)+1;
     if (leng > newFileLen) {
       delete[] m_NewFile;
       m_NewFile = new char[leng];
       newFileLen = leng;
     }
     strcpy(m_NewFile, ntpfile);
   }

   return 0;
}

//_____________________________________________________________________________
int NtupleSelectorFor::OpenOldNtuple(char *ntpfile)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //
  //  Return values:
  //    0       successful
  //    1       can't open file
  //    2       can't duplicate the specified ntuple
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   static int reclen;
   int istat;

   if (m_OldFile != 0) {
      CloseOldNtuple();
   }

   if (ntpfile == 0 || strlen(ntpfile) == 0) {
//      printf("NULL filename in NtupleSelectorFor::OpenOldNtuple\n");
      cerr << "NULL filename in NtupleSelectorFor::OpenOldNtuple" << endl;
      return 1;
   }

   reclen = 0;
   HROPEN(m_OldLun, top_old, ntpfile, "XP", reclen, istat);
   if (istat != 0) {
//      fprintf(stderr,"OpenOldNtuple, Failure in opening ntuple file %s\n",
//              ntpfile);
      cerr << "Failure in opening old ntuple file =" << ntpfile << endl;
      CLOSE(m_OldLun);
      return 1;
   }

   int offset = m_MemID - m_NtupleID;
   HRIN(m_NtupleID, 9999, offset);
   istat = EXIST(m_MemID);
   if (istat != 0) {
//      fprintf(stderr,"OpenOldNtuple, No such ntuple ID =%d\n",m_NtupleID);
      cerr << "OpenOldNtuple, No such ntuple ID =" << m_NtupleID << endl;
      HREND(top_old);
      CLOSE(m_OldLun);
      return 2;
   }

   //
   //Get entries in the ntuple
   //
   HNOENT(m_MemID, m_OldEntries);

   SetAddresses(m_MemID);

   //
   //Need to invoke HGNT first to enable successful calling of HGNTV
   //
   int ierr;
   HGNT(m_MemID, 1, ierr);
   m_LastRead   = kNeverRead;
   m_Event      = -1;
   m_OldFile    = ntpfile;

   return 0;
}

//_____________________________________________________________________________
int NtupleSelectorFor::OpenUserFile(char *usrfile)
{
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
  //
  //  Return values:
  //    0       successful
  //    1       can't open file
  //-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

   int istat;

   //
   // Now call user's init routine
   //
   if (m_Cut != 0)  m_Cut(-1);

   if (m_UsrFile != 0) {
      CloseUserFile();
   }

   if (usrfile == 0 || strlen(usrfile) == 0) {
//      printf("NULL filename in NtupleSelectorFor::OpenUserFile\n");
      cerr << "NULL filename in NtupleSelectorFor::OpenUserFile" << endl;
      return 1;
   }

   HROPEN(m_UsrLun, top_usr, usrfile, "NP", m_RecLen, istat);
   if (istat != 0) {
//      fprintf(stderr,"OpenUserFile, Failure in opening user hbook file %s\n",
//              usrfile);
      cerr << "Failure in opening user hbook file =" << usrfile << endl;
      CLOSE(m_UsrLun);
      return 1;
   }

   m_UsrFile    = usrfile;

   return 0;
}

//_____________________________________________________________________________
void NtupleSelectorFor::SetAddresses(int id)
{
   //
   //Set block addresses for ntuple
   //
   HBNAME(id, " ", 0, "$CLEAR");

   HBNAME(id, "EVENTH", &(m_BlkEventh->eventno), "$SET");
   HBNAME(id, "BETA", &(m_BlkBeta->nbeta), "$SET");
   HBNAME(id, "CHARGE", &(m_BlkCharge->ncharge), "$SET");
   HBNAME(id, "PARTICLE", &(m_BlkParticle->npart), "$SET");
   HBNAME(id, "TOFCLUST", &(m_BlkTofclust->ntof), "$SET");
   HBNAME(id, "TOFMCCLU", &(m_BlkTofmcclu->ntofmc), "$SET");
   HBNAME(id, "TRCLUSTE", &(m_BlkTrcluste->Ntrcl), "$SET");
   HBNAME(id, "TRMCCLUS", &(m_BlkTrmcclus->ntrclmc), "$SET");
   HBNAME(id, "TRRECHIT", &(m_BlkTrrechit->ntrrh), "$SET");
   HBNAME(id, "TRTRACK", &(m_BlkTrtrack->ntrtr), "$SET");
   HBNAME(id, "MCEVENTG", &(m_BlkMceventg->nmcg), "$SET");
   HBNAME(id, "CTCCLUST", &(m_BlkCtcclust->nctccl), "$SET");
   HBNAME(id, "CTCMCCLU", &(m_BlkCtcmcclu->nctcclmc), "$SET");
   HBNAME(id, "ANTICLUS", &(m_BlkAnticlus->nanti), "$SET");
   HBNAME(id, "ANTIMCCL", &(m_BlkAntimccl->nantimc), "$SET");
   HBNAME(id, "LVL3", &(m_BlkLvl3->nlvl3), "$SET");
   HBNAME(id, "LVL1", &(m_BlkLvl1->nlvl1), "$SET");
   HBNAME(id, "CTCHIT", &(m_BlkCtchit->nctcht), "$SET");
   HBNAME(id, "TRRAWCL", &(m_BlkTrrawcl->ntrraw), "$SET");
   HBNAME(id, "ANTIRAWC", &(m_BlkAntirawc->nantiraw), "$SET");
   HBNAME(id, "TOFRAWCL", &(m_BlkTofrawcl->ntofraw), "$SET");
}

//_____________________________________________________________________________
void NtupleSelectorFor::SetBsize(int size)
{
   //
   //Set buffer size of each ntuple variable(i.e. column) a buffer of size
   // words in memory, default is 1024 as stated in HBOOK
   //
   int ierr;
   if (size <= 0) size = 1024;
   HBSET("BSIZE", size, ierr);
}

//_____________________________________________________________________________
void NtupleSelectorFor::SetBufferSize(int size)
{
   if (m_NBuffer != 0) WriteBuffer();

   if (m_PBuffer != 0) delete[] m_PBuffer;
   m_PBuffer = new int[size];
   m_BufferSize = size;
   m_NBuffer = 0;
}

//_____________________________________________________________________________
void NtupleSelectorFor::WriteBuffer()
{
//   printf("NtupleSelectorFor::WriteBuffer, m_NBuffer=%d\n",m_NBuffer);
   cout << "NtupleSelectorFor::WriteBuffer, m_NBuffer=" << m_NBuffer << endl;

   int current = m_Event;

   for (int i=0; i<m_NBuffer; i++) {
      int event = m_PBuffer[i];
      GetEventAll(event);
      WriteEvent();
   }
   m_NBuffer = 0;

   m_Event = current;
}

//_____________________________________________________________________________
int NtupleSelectorFor::WriteEvent()
{
   if (m_NewFile[0] == 0) {
     char text[100];

     // check if ending character is '/', otherwise, add it
     //
     char *slash = strrchr(m_NewDir, '/');
     if (slash == 0) slash=m_NewDir;
     int last = strlen(slash);
     if (last <= 1) last=0;
     else last = 1;

     sprintf(text,"%d.%d",m_BlkEventh->run,m_BlkEventh->eventno);

     char *ext = strrchr(m_OldFile,'.');
     int leng = strlen(m_NewDir) + last + strlen(text) + 1;
     if (m_Insert != 0) leng = leng + strlen(m_Insert);
     if (ext != 0) leng = leng + strlen(ext);

     if (leng > newFileLen) {
       delete[] m_NewFile;
       m_NewFile = new char[leng];
       newFileLen = leng;
     }

     strcpy(m_NewFile, m_NewDir);
     if (last == 1) strcat(m_NewFile, "/");
     strcat(m_NewFile, text);
     if (m_Insert != 0) strcat(m_NewFile, m_Insert);
     if (ext != 0) strcat(m_NewFile, ext);

//     printf("m_NewDir=%s, text=%s\n, m_Insert=%s, ext=%s\n",
//            m_NewDir, text, m_Insert, ext);

     OpenNewNtuple(0);
   }
   
   HFNT(m_NtupleID);
   m_NewEntries++;

   if (m_NewEntries >= m_MaxEvents) CloseNewNtuple();
   
   return 0;
}
