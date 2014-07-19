//  $Id: ntpSelectFor.cxx,v 1.1.1.1 2007/11/13 09:56:12 zuccon Exp $
#include "NtupleSelectorFor.h"
#include <signal.h>
#include <unistd.h>
#include <iostream.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <dirent.h> 

extern int scandir();
#ifdef __AIX__
extern int alphasort(struct dirent **, struct dirent **);
#else
extern int alphasort();
#endif
#if defined __HPUX__
int mySelect(const struct dirent *);
#else
int mySelect(struct dirent *);
#endif

enum EHowGetFile { kInput=0, kScan=1};

static char *selectExt = "hbk";

void printUsage()
{
      char *sp = "                ";
      cout << "\nUsage: $(OS)/bin/cutName [-h] [-b[Size]] [-iNtupleID] [-nNEvt]\n"
        << sp << "[-oOutFile] [-sDir [-eExt]] [-tInsert] [-uHistFile]\n"
        << sp << "[-FFrom] [-TTo] oldFile(s)\n"
        << " $(OS)=  your system name, can be got via shell \"uname\"\n"
        << " cutName = basename of your cut filename(default=\"usrcut.f\")\n"
        << "  -h  =  print this message\n"
        << "  -b  =  save survived event# into a buffer first, write it out later.\n"
        << "         buffer size(d=1000) can be optionally changed\n"
        << "  -i  =  set ntupleID, follows NtupleId(default=1)\n"
        << "  -n  =  set maximum events for each output ntuple, <=0 value\n"
        << "         and no \"-o\" will output a file for each input file\n"
        << "  -o  =  set output filename for all input ntuples, otherwise,\n"
        << "         one output file for each input ntuple, with filename\n"
        << "         string-inserted before extension if old filename\n"
        << "         or one output ntuple per NEvt events selected, with\n"
        << "         convention on filename=run.1st_evt.insert.ext .\n"
        << "         The file will be written into $AMSNewNtpDir if defined\n"
        << "         otherwise into the current directory\n"
        << "  -s  =  use scandir, follows the directory to scan files\n"
        << "  -e  =  file extension for scandir. Use together with \"-s\"\n"
        << "         default=\"hbk\"\n"
        << "  -t  =  insert string in output filename, default=\".select\"\n"
        << "  -u  =  specify user histogram file name\n"
        << "  -F  =  ordinal no. of first file among input files to process\n"
        << "  -T  =  ordinal no. of last file among input files to process\n"
        << " oldFile = filename of OLD ntuple to read, ignored if \"-s\"\n"
        << " How to change user cut ? See usrcut.f\n"
        << endl;
}

class FileContainer
{
 protected:
   EHowGetFile     m_HowGet;
   struct dirent **m_Namelist;
   int             m_Entries;
   int             m_Current;
   char          **m_Argv;
   char           *m_ScanDir;
   char           *m_Filename;

 public:
//                   FileContainer(EHowGetFile howGet) { m_HowGet = howGet;}
                   FileContainer(EHowGetFile howGet);
                  ~FileContainer();
   EHowGetFile     GetHowGet() { return m_HowGet;}
   inline void     SetArgv(char **argv, int argc);
   inline void     SetScan(char* dir, char *ext);
   inline char    *GetNextFile();
};

enum Bool { kFALSE=0, kTRUE=1 };
void (handler)(int);           
int main(int argc, char *argv[])
{
     *signal(SIGFPE, handler);
     *signal(SIGCONT, handler);
  //   *signal(SIGTERM, handler);
  //   *signal(SIGINT, handler);
     *signal(SIGQUIT, handler);
     *signal(SIGUSR1, handler);
  int   idNull    = -1;
  int   ntupleID  = idNull; //initial value
  Bool  useBuffer = kFALSE;
  int   bufSize   = idNull;
  Bool  useScan   = kFALSE;
  char *outFile   = 0;
  char *insertStr = 0;
  char *scanDir   = 0;
  char *scanExt   = 0;
  char *usrFile   = 0;
  int   mEvent    = 10000;
  const int  one2one   = 100000;
  int   ifirst    = 1;
  int   ilast     = 100000;
  Bool  isOpen    = kFALSE;

  for (char *pchar=0; argc>1 &&(pchar=argv[1],*pchar=='-'); (argv++,argc--))
  {
    pchar++;
    if (*pchar=='h') {   // print the help on usage
      printUsage();
      return 1;
    } else if (*pchar=='b' ) { // use buffer before writing out
      useBuffer = kTRUE;
      cout << "Buffer will be used" << endl;
      if (strlen(++pchar) > 0) { // change the buffer size
        bufSize = atoi(pchar);
        cout << "change buffer size to " << bufSize << endl;
      }
    } else if (*pchar=='i') {  // follows NtupleID
      ntupleID = atoi(++pchar);
      cout << "NtupleID set to " << ntupleID << endl;
    } else if (*pchar=='n') {  // follows maximum events for output ntuple
      mEvent = atoi(++pchar);
      cout << "Set maximum events to " << mEvent << endl;
    } else if (*pchar=='o') {  // follows output filename for all ntuples
      outFile = ++pchar;
      if (strlen(outFile) == 0) outFile=0;
      else {
        cout << "All selected events will be written into "
                << outFile << endl;
        mEvent = 0;
//        mEvent = 1000000;
//        cout << "Set maximum events to " << mEvent << endl;
      }
    } else if (*pchar=='s') {  // use "scandir"
      scanDir = ++pchar;
      if (strlen(scanDir) != 0) {
        useScan = kTRUE;
        cout << "invoke scandir to scan directory " << scanDir << endl;
      } else scanDir = 0;
    } else if (*pchar=='e') {  // set extension for scandir
      if (useScan) {
        scanExt = ++pchar;
        cout << "In \"scandir\", set extension " << scanExt << endl;
      }
    } else if (*pchar=='t') {  // set insert string for output filename
      insertStr = ++pchar;
      cout << "insert string set to " << insertStr << endl;
    } else if (*pchar=='u') {  // set user histgram filename
      usrFile = ++pchar;
      if (strlen(usrFile) != 0) {
        cout << "user histogram file set to " << usrFile << endl;
      } else usrFile = 0;
    } else if (*pchar=='F') {  // set no. of first file to process
      ifirst = atoi(++pchar);
      cout << "File processing starts from no = " << ifirst << endl;
    } else if (*pchar=='T') {  // set no. of last file to process
      ilast = atoi(++pchar);
      cout << "File processing ends at no = " << ilast << endl;
    } else cout << "Unknown option =" << *--pchar << endl;
  }

//=============> Processing actions then ======================================

  if ( !useScan && argc < 2 ) { // no filename given
    cerr << "::Error:: no filename given, Exit(1)" << endl;
    printUsage();
    exit(1);
  }

  //
  //NtupleSelectorFor
  //--------------
  NtupleSelectorFor *select = new NtupleSelectorFor();
//  select->SetCut( (int(*)(int)) usrcut_ );
//  select->SetCut( (int(*)(int)) __cf__USRCUT );
  select->SetCut( __cf__USRCUT );
  select->SetRecLen(1024);
  select->OpenUserFile(usrFile);

  //
  // Set ntupleID if necessary
  //
  if (ntupleID != idNull && ntupleID > 0)
     select->SetNtupleID(ntupleID);

  //
  // Change buffer size if specified
  //
  if (bufSize != idNull && bufSize > 0)
     select->SetBufferSize(bufSize);

  //
  // Set insert string
  //
  if (insertStr != 0) select->SetInsert(insertStr);
  else insertStr = select->GetInsert();

  //
  // Set maximum events for each new ntuple
  //
  if (mEvent <= 0) select->SetMaxEvents(one2one);
  else select->SetMaxEvents(mEvent);

  //
  //Get directory of new ntuple (invalid for option=-o)
  //
  char new_dir[100];
  char *newDir = getenv("AMSNewNtpDir");
  if ( !newDir || strlen(newDir) == 0 ) {
     cout << "AMSNewNtpDir Not Set, use CWN to save new ntuples !!" << endl;
     strcpy(new_dir,"./");
  } else {
     strcpy(new_dir,newDir);
     int leng = strlen(newDir);
     if ( newDir[leng-1] != '/') strcat(new_dir,"/");
  }
  select->SetNewDir(new_dir);

  int ifile=0;
  FileContainer *contain = 0;
  if (useScan) {
    contain = new FileContainer(kScan);
    contain->SetScan(scanDir, scanExt);
  } else {
    argv++,argc--;
    contain = new FileContainer(kInput);
    contain->SetArgv(argv,argc);
  }
  
  char *oldFile = 0;
  int iret;
  while ( oldFile=contain->GetNextFile() ) { //begin of LOOP over ntupel files
    if (++ifile < ifirst) continue;
    if (ifile > ilast) break;

    char *newFile=0, *slash, *dot;
    int headLen;

    slash = strrchr(oldFile, '/');
    if ( slash==0 ) slash = oldFile;
    else slash++;
    if ( strlen(slash)==0 ) {                 // nothing after '/'
       cerr << "The input file " << oldFile 
         << " looks like a directory, not a filename ?" << endl;
       return 1;
    }
    iret = select->OpenOldNtuple(oldFile);
    if (iret != 0) continue;
    cout << "\nNumber =" << ifile << " old ntuple file =" << oldFile << endl;

//    ifile++;
    
//    if (outFile == 0 && mEvent > one2one) {
    if (outFile == 0 && mEvent <= 0) {
      dot = strrchr(slash,'.');
      if (dot == 0) headLen = strlen(slash);
      else headLen = strlen(slash) - strlen(dot);
      int len = strlen(new_dir) + strlen(slash) + strlen(insertStr) + 1;
      newFile = new char[len];
      strcpy(newFile,new_dir);
      strncat(newFile,slash,headLen);
      strcat(newFile,insertStr);
      if (dot != 0) strcat(newFile,dot);
      select->OpenNewNtuple(newFile);
    } else if (outFile != 0) {
      if ( !isOpen ) select->OpenNewNtuple(outFile);
      isOpen = kTRUE;
    }

    if (useBuffer) while (select->FindNextAndSave() >= 0) ;
    else while (select->FindNextAndWrite() >= 0) ;

    select->CloseOldNtuple();
    if (outFile == 0 && newFile != 0) select->CloseNewNtuple();
    if (newFile != 0) delete newFile;

  } //end of LOOP over ntuple files

  select->CloseNewNtuple();
  select->CloseUserFile();

  delete select;
  delete contain;

  return 0;
}


FileContainer::FileContainer(EHowGetFile howGet)
{
   m_HowGet   = howGet;
   m_ScanDir  = 0;
   m_Filename = 0;
}

FileContainer::~FileContainer()
{
   if (m_HowGet == kScan) {
      for (int i=0; i<m_Entries; i++) {
         struct dirent **list = m_Namelist;
         delete *list;
         list++;
      }
      delete m_Namelist;
      if (m_ScanDir != 0 && m_Filename != 0) delete m_Filename;
   }
}

inline void FileContainer::SetArgv(char **argv, int argc)
{
   m_Current = 0;
   if (m_HowGet == kInput) {
      m_Namelist = 0;
      m_Argv     = argv;
      m_Entries  = argc;
   }
}

inline void FileContainer::SetScan(char* dir, char *ext)
{
   m_ScanDir = dir;
   m_Current = 0;
   if (m_HowGet == kScan) {
      m_Argv     = 0;
      if (ext != 0) selectExt = ext;
#ifdef __AIX__
      m_Entries  = scandir(dir, &m_Namelist,
                           mySelect, alphasort);
#else
      m_Entries  = scandir(dir, &m_Namelist, mySelect, alphasort);
#endif
   }
}

inline char *FileContainer::GetNextFile()
{
   static struct dirent **list =0;
   if (m_Current >=m_Entries) return 0;
   m_Current++;
   if (m_HowGet == kInput) {
      m_Filename = *m_Argv;
      m_Argv++;
   } else {
      if (list ==0) list=m_Namelist;
      if (m_Filename != 0) delete m_Filename;
      m_Filename = (*list)->d_name;
      list++;
      int leng = strlen(m_ScanDir)+strlen(m_Filename)+1+1;
      char *tmp = new char[leng];
      strcpy(tmp,m_ScanDir);
      strcat(tmp,"/");
      strcat(tmp,m_Filename);
      m_Filename = tmp;
   }
   return m_Filename;
}

#if defined __HPUX__
int mySelect(const struct dirent * entry)
#else 
int mySelect(struct dirent * entry)
#endif
{
   const char *name = entry->d_name;
   int leng = strlen(name) - strlen(selectExt);
   const char *last;
   if (leng > 0) {
      last = name+leng;
      if (strcmp(last,selectExt) == 0) return 1;
      else return 0;
   } else return 0;
      
//   const char *ext = strstr(name, selectExt);
//   if (ext != 0 && strlen(ext) == strlen(selectExt)) return 1;
//   else return 0;

//   const char *dot = strrchr(name, '.');
//   if (dot==0) return 0;
//   if (strcmp(++dot, selectExt) == 0) return 1;
//   return 0;
}

void (handler)(int sig){
  switch(sig){
  case SIGFPE:
//   cerr <<" FPE intercepted"<<endl;
   break;
  case SIGTERM: case SIGINT:
    cerr <<" SIGTERM intercepted"<<endl;
    break;
  case SIGQUIT:
    cerr <<" Process suspended"<<endl;
    pause();
    break;
  case SIGCONT:
      cerr <<" Process resumed"<<endl;
      break;
  case SIGUSR1:
      cerr<< "New Run Forced"<<endl;
      break;
  }
}
