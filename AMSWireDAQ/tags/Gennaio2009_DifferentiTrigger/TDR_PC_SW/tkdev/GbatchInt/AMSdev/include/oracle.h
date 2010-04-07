//  $Id: oracle.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//
// ORACLE related subroutines 
//
// argv[1] - run number
// argv[2] - TDV name
//
// A.Klimentov  Dec   6, 2000. 
//              Dec  22, 2000. Only 3 TDV tables to store all TDVs
//                             Tag and prodruns tables
//              Jan    , 2001. Hosts, Clients, Runs
//              Feb    , 2001. Active CLients, hosts.
//              Jun   7, 2001. gettdvbody add amsdatadir
//              Jul   3, 2001. Warm start subroutines
//
// Last Edit : July 18, 2001
//


#ifdef __AMSORACLE__

const int debug = 1;

const int  NON_EXISTENT = -942;
const int  NOT_FOUND    = 1403;
const int  MAXSIZE      = 610000/4; // 610KByte/4
const int  atimel       = 21;
const int  M_TDVDEF     = 0;        // select m_tdvdef table
const int  MAXTDVNAMELENGTH  = 256;
const int  MAXFILENAMELENGTH = 256;
const int  MAXDSTINFO        = 100;

const char runqueryfile[256] = {"RunQueryForm"};
const char runtablefile[256] = {"RunTableOracle"};

const int NROWSMAX = 2000;         // insert by min(NROWSMAX,events)
const int MAXRUNS  = 2000;         //
const int MAXRUNTABLESIZE = 2000;  // return not more than MAXRUNTABLESIZE
                                   // matching to the query 
const int TBdeleted       = -2;    // TDV is marked to be deleted
const int TBbad           = -1;    // TDV is marked as bad
const int TBprod          =  1;    //               as production

const int ProductionDone        = 0;
const int ProductionInProgress  = 1;
const int ProductionInSuspended = 2;

namespace AMSoracle {

  int  suppressBlanks(char *string);

  class TDVrec {
   public : 
      char    name[256];            // name
      int     datamc;               // datamc 
      int     idx;                  // tdv idx
      char    timei[32];            // insertion time
      char    timeb[32];            // begin of validity time
      char    timee[32];            // end of validity time
      char    timef[32];            // file creation date
      unsigned int    utimei;
      unsigned int    utimeb;
      unsigned int    utimee;
      long    nbytes;                //  size in bytes
      char    dirpath[1024];         // path to tdv dir
      char    filepath[512];         // path to tdvfile

     TDVrec() {idx = -1;}
    ~TDVrec() { };
     void setname(char *tname, int dmc) {
       if (tname && strlen(tname) < MAXTDVNAMELENGTH) 
         strcpy(name,tname);
         datamc = dmc;
     }

     void getutime(unsigned int &insert, unsigned int &begin, unsigned int &end) {
       insert = utimei;
       begin  = utimeb;
       end    = utimee;
     }
     
     const char *getfilepath() {return filepath;}
     const char *getdirpath()  {return dirpath;}
     const int  getid() {return idx;}
     const char *getname() {return name;}
     long getsize() { return nbytes;}
     void print() {
      cout<<"i/b/e "<<utimei<<" / "<<utimeb<<" / "<<utimee<<endl;
     }

     void setid(int id) {idx = id;}
     long setsize(long size) { nbytes = size;}
     void setfilepath(const char *fpath) {
       if (fpath && strlen(fpath) < MAXFILENAMELENGTH) {
         strcpy(filepath,fpath);
       }
     }
     void setdirpath(const char *dpath) {
       if (dpath && strlen(dpath) < MAXFILENAMELENGTH) {
         strcpy(dirpath,dpath);
       }
     }
     void utime(long insert, long begin, long end) {
      utimei = insert;
      utimeb = begin;
      utimee = end;
     }

    };


  class TDVutime {
    public :
    long insert;
    long begin;
    long end;

    TDVutime() {insert = 0; begin = 0; end = 0;}
    ~TDVutime() { };
    void fill(long ti, long tb, long te) {
      insert = ti;
      begin  = tb;
      end    = te;
    }
    void print() {
      cout<<"i/b/e "<<insert<<" / "<<begin<<" / "<<end<<endl;
    }
    void printa() {
      cout<<ctime(&insert)<<ctime(&begin)<<ctime(&end)<<endl;
    }
    void gettime(long &uinsert, long &ubegin, long &uend) {
      uinsert = insert;
      ubegin  = begin;
      uend    = end;
    }
    int getatime(char *tinsert, char *tbegin, char *tend) {
      int rstat = -1;
      if (strlen(tinsert) >= atimel ) tinsert = ctime(&insert);
      if (tbegin)  tbegin  = ctime(&begin);
      if (tend)    tend    = ctime(&end);
      if (tinsert && tbegin && tend) rstat = 1;
      return rstat;
    }
  };

  class ProdRun {
    public :
      unsigned int run;         // run number
      unsigned int idx;
      unsigned int events;        //  number of events
      unsigned int fevent;        // first event of run
      unsigned int levent;        // last  event of run
      long         tfirst;        // time of first event
      long         tlast;         // time of last event
      unsigned int timestamp;


      unsigned int eventsp;
      unsigned int lpevent;         // last processed event
               int errors;          // erorors found
               int cerrors;
      float        cputime;         // cputime spent
      float        eltime;          // elapsed time
      unsigned int status;          //
      unsigned int hostid;          //
      char         hostname[256];

    ProdRun() {run = 0; events = 0; fevent = 0; levent = 0; 
               eventsp =0; lpevent = 0; errors = -1; cerrors = -1;
               cputime = -1; eltime = -1; status = 0; hostid = 0;}
    ~ProdRun() { };
    void get(unsigned int &trun, unsigned int &tidx, unsigned int &tevent, 
                unsigned int &tfirst, unsigned int &tlast, 
                unsigned int tstatus)
      { trun   = run;
        tidx   = idx;
        tevent = events;
        tfirst = fevent;
        tlast  = levent;
        tstatus = status;
      }

    char *gethostname() { return hostname;}

    void set(unsigned int trun, unsigned int tevent, 
             unsigned int tfirst, unsigned int tlast,
             unsigned int tstamp, unsigned int tidx)
      { run    = trun;
        events = tevent;
        fevent = tfirst;
        levent = tlast;
        timestamp = tstamp;
        idx       = tidx;
      }



    void print() {
      cout<<"run, id, events fevent/levent "<<run<<", "
          <<idx<<", "<<events
          <<" "<<fevent<<"/"<<levent<<endl;
      cout<<"inserted "<<timestamp<<endl;
    }

    void getInfo(unsigned int &trun, 
                 unsigned int &tid,
                 unsigned int &teventsp,
                 unsigned int &tleventp,
                 int &tcerrors, int &terrors, float &tcputime,
                 float &teltime, unsigned int &tstatus, char *thostname)
      {
        trun = run;
        tid  = idx;
        teventsp  = eventsp;
        tleventp  = lpevent;
        terrors   = errors;
        tcerrors   = cerrors;
        tcputime   = cputime;
        teltime    = eltime;
        tstatus    = status;
        if (thostname) strcpy(thostname,hostname);
      }
    void getInfo(unsigned int &trun, 
                 unsigned int &tid,
                 int &teventsp,
                 int &tleventp,
                 int &tcerrors, int &terrors, float &tcputime,
                 float &teltime, unsigned int &tstatus, char *thostname)
      {
        trun = run;
        tid  = idx;
        teventsp  = eventsp;
        tleventp  = lpevent;
        terrors   = errors;
        tcerrors   = cerrors;
        tcputime   = cputime;
        teltime    = eltime;
        tstatus    = status;
        if (thostname) strcpy(thostname,hostname);
      }

    void updateInfo(unsigned int trun, unsigned int tid, 
                    unsigned int teventsp, 
                    unsigned int leventp, int tcerrors, 
                    int terrors,
                    float tcputime, float teltime, 
                    int  tstatus, const char *thostname)
      { 
        run = trun;
        idx = tid;
        eventsp    = teventsp;
        lpevent    = leventp;
        errors     = terrors;
        cerrors    = tcerrors;
        cputime    = tcputime; 
        eltime     = teltime;
        status     = tstatus;
        if (thostname) {
          strcpy(hostname,thostname);
          suppressBlanks(hostname);
        }
      }
    void clearInfo()
      { 
        eventsp    = -1;
        lpevent    = -1;
        errors     = -1;
        cerrors    = -1;
        cputime    = -1; 
        eltime     = -1;
        status     = -1;
   }
  };

  class EventTag {
    public :
      unsigned int run;
      unsigned int idx;
      unsigned int event;
      unsigned int position;
      unsigned int tag1;
      unsigned int tag2;
  
    EventTag() {idx = -1;}
    ~EventTag() { };
    void get(unsigned int &trun, unsigned int &tidx,
             unsigned int &tevent, unsigned int &tpos,
             unsigned int &ttag1, unsigned int &ttag2)
      { 
        trun   = run;
        tidx   = idx;
        tevent = event;
        tpos   = position;
        ttag1  = tag1;
        ttag2  = tag2;
      }

    unsigned int getevent() {return event;};

    unsigned int getid() {return idx;};

    void set(unsigned int trun,  unsigned int tidx,
             unsigned int tevent, unsigned int tpos,
             unsigned int ttag1, unsigned int ttag2)
      {  
         run   = trun;
         idx   = tidx;
         event = tevent;
         position = tpos;
         tag1  = ttag1;
         tag2  = ttag2;
      }

    void set(unsigned int ttag1, unsigned int ttag2)
      {  tag1  = ttag1;
         tag2  = ttag2;
      }

    void print() {
      cout<<"event, tags "<<event<<" "<<tag1<<" "<<tag2<<endl;
    }
  };

  class RunTable {
   
    public :

      unsigned int  run;
      unsigned int  timef;
      unsigned int  timel;
      unsigned int  eventf;
      unsigned int  eventl;
      char filepath[1024];


      unsigned int status;
      unsigned int history;

      unsigned int submittime;      // production job submit time

      unsigned int timestamp;       // record insert/update time

      unsigned int idx;

      int cuid;
      int priority;

      RunTable() { run = -1;}
      ~RunTable() { };

      void get(unsigned &trun, 
               unsigned int &ef, unsigned int &el, 
               unsigned int &tf, unsigned int &tl,
               char *path)
        {
          trun = run;
          tf = timef;
          tl = timel;
          ef = eventf;
          el = eventl;
          if (path && filepath) strcpy(path,filepath);
        }
      void get(unsigned int &tid, unsigned int &trun, 
               unsigned int &ef, unsigned int &el, 
               unsigned int &tf, unsigned int &tl,
               int &tpriority, char *path,
               unsigned int &tstatus, unsigned int &thistory, 
               unsigned int &tsubmit, unsigned int &tcuid)
        {
          tid  = idx;
          trun = run;
          tf = timef;
          tl = timel;
          ef = eventf;
          el = eventl;
          tpriority = priority;
          if (path && filepath) strcpy(path,filepath);
          tstatus  = status;
          thistory = history;
          tsubmit  = submittime;
          tcuid    = cuid;
        }

      void get(unsigned int &tid, unsigned int &trun, 
               int &ef, int &el, 
               unsigned int &tf, unsigned int &tl,
               int &tpriority, char *path,
               unsigned int &tstatus, unsigned int &thistory, 
               unsigned int &tsubmit, unsigned int &tcuid)
        {
          tid  = idx;
          trun = run;
          tf = timef;
          tl = timel;
          ef = eventf;
          el = eventl;
          tpriority = priority;
          if (path && filepath) strcpy(path,filepath);
          tstatus  = status;
          thistory = history;
          tsubmit  = submittime;
          tcuid    = cuid;
        }


      void set(unsigned int trun, 
               unsigned int ef, unsigned int el,
               unsigned int tf, unsigned int tl, 
               char *path)
        {
          run = trun;
          timef = tf;
          timel = tl;
          eventf = ef;
          eventl = el;
          if (path && filepath) {
            strcpy(filepath,path);
            suppressBlanks(filepath);
          }
        }
      void set(unsigned int trun, 
               unsigned int tidx, unsigned int ef, unsigned int el,
               unsigned int tf, unsigned int tl, int tpriority,
               char *path,
               unsigned int tstatus, unsigned int thistory, 
               unsigned int tsubmit, int tcuid)
        {
          run = trun;
          idx = tidx;
          timef = tf;
          timel = tl;
          eventf = ef;
          eventl = el;
          if (path && filepath) {
           strcpy(filepath,path);
           suppressBlanks(filepath);
          }
          status  = tstatus;
          history = thistory;
          submittime  = tsubmit;
          cuid        = tcuid;
          priority    = tpriority;
        }

  };

  class ProcDesc {
   
    public :

    unsigned long id;
    unsigned int  type;
    short int     maxclients;
    float         cpu;
    short int     memory;
    char          scriptpath[1024];
    char          logpath[1024];
    char          submitcmd[1024];
    char          hostname[40];
    short int     logend;
  
    ProcDesc() {id = -1;}
    ~ProcDesc() {};
    int  get(unsigned int &uid, 
             unsigned int &ttype,
             short int &tmaxclients,
             float &tcpu, 
             short int &tmem, 
             char *path, 
             char *lpath,
             char *cmd, 
             char *host,
             int  &logintheend)
      {
        uid = id;
        ttype = type;
        tmaxclients = maxclients;
        tcpu        = cpu;
        tmem        = memory;
        strcpy(path,scriptpath);
        strcpy(lpath,logpath);
        strcpy(cmd,submitcmd);
        strcpy(host,hostname);
        logintheend = logend;
      };
    int  set(unsigned long uid, 
             short int tmaxclients,
             float tcpu, 
             short int tmem, 
             const char *path, 
             const char *lpath,
             const char *cmd, 
             const char *host, 
             short int tlogend)
      {
        id = uid;
        maxclients = tmaxclients;
        cpu        = tcpu;
        memory     = tmem;
        if (path)  {
          strcpy(scriptpath,path);
          suppressBlanks(scriptpath);
        }
        if (lpath) {
         strcpy(logpath,lpath);
         suppressBlanks(logpath);
        }
        if (cmd && strlen(cmd) < 1024)   {
          strcpy(submitcmd,cmd);
          suppressBlanks(submitcmd);
        }
        if (host)  {
         strcpy(hostname, host);
         suppressBlanks(hostname);
        }
        logend = tlogend;        
      }

  };


  class ActiveClientDesc {
   
    public :

    unsigned int id;
    unsigned int lastupd;
    unsigned int starttime;
    unsigned int status;
 
    ActiveClientDesc() {id = -1;}
   ~ActiveClientDesc() {};
    int  get(unsigned int &uid,
             unsigned int &tlastupd, 
             unsigned int &tstarttime,
             unsigned int &tstatus)
      {
        uid        = id;
        tlastupd   = lastupd;
        tstarttime = starttime;
        tstatus    = status;
      };

    int  set(unsigned int uid,
             unsigned int tlastupd, 
             unsigned int tstarttime,
             unsigned int tstatus)
      {
        id        = uid;
        lastupd   = tlastupd;
        starttime = tstarttime;
        status    = tstatus;
      };
  };

  class ActiveClientRef {
   
    public :

    unsigned int id;
    char          ior[1024];
    char          interface[40];
    unsigned int  type;
    unsigned int  reftype;
    unsigned int  uid;

    ActiveClientRef() {id = -1;}
   ~ActiveClientRef() {};
    int  get(
             char         *tior,
             char         *tinterface,
             unsigned int &ttype,
             unsigned int &treftype,
             unsigned int &tuid)
      {
        if (tinterface) {
         strcpy(tinterface, interface);
        }
        if (tior) strcpy(tior,ior);

        ttype  = type;
        treftype  = reftype;
        tuid   = uid;
      };
    int  set(
             char         *tior,
             char         *tinterface,
             unsigned int ttype,
             unsigned int treftype,
             unsigned int tuid)
      {
        type = ttype;
        reftype = treftype;
        if (tinterface) {
           strcpy(interface, tinterface);
           suppressBlanks(interface);
        }
        if (ior)        {
          strcpy(ior,tior);
          suppressBlanks(ior);
        }
        uid  = tuid;
      };
  };

  class ActiveClientId {
   
    public :

    unsigned int id;
    unsigned int pid;
    unsigned int ppid;
    unsigned int  type;
    unsigned int  status;
    char          interface[40];
    char          hostname[40];
  
    ActiveClientId() {pid = -1;}
    ~ActiveClientId() {};
    int  get(unsigned int &uid,
             unsigned int &tpid,
             unsigned int &tppid, 
             unsigned int &ttype,
             unsigned int &tstatus,
             char         *tinterface,
             char         *thostname)
      {
        uid   = id;
        tpid  = pid;
        tppid = ppid;
        ttype = type;
        tstatus = status;
        if (tinterface) strcpy(tinterface, interface);
        if (thostname)  strcpy(thostname, hostname);
      };
    int  set(unsigned int uid,
             unsigned int tpid,
             unsigned int tppid, 
             unsigned int ttype,
             unsigned int tstatus,
             const char   *tinterface,
             const char   *thostname)


      {
        id   = uid;
        pid  = tpid;
        ppid = tppid;
        type = ttype;
        status = tstatus;
        if (tinterface) {
           strcpy(interface, tinterface);
           suppressBlanks(interface);
        }
        if (thostname)  {
          strcpy(hostname, thostname);
          suppressBlanks(hostname);
        }
      };
  };

  class NominalHost {
  
    public :
      char         name[40];
      char         interface[40];
      char         platform[80];
      short int    ncpu;
      short int    memory;
      int          clock;

      NominalHost() {};
      ~NominalHost() {};

      int set(char *tname, char *tinterface, char *tplatform,
              short int tncpu, short int tmemory, int tclock)
        {
          if (tname) {
           strcpy(name,tname);
           suppressBlanks(tname);
          }
          if (tinterface) {
           strcpy(interface,tinterface);
           suppressBlanks(interface);
          }
          if (tplatform)  strcpy(platform,tplatform);
          ncpu   = tncpu;
          memory = tmemory;
          clock  = tclock;
        };

      int get(char *tname, char *tinterface, char *tplatform,
              short int &tncpu, short int &tmemory, int &tclock)
        {
          if (tname) {
                       strcpy(tname,name);
                       suppressBlanks(tname);
          }
          if (tinterface) {
                        strcpy(tinterface,interface);
                        suppressBlanks(tinterface);
          }
          if (tplatform) {
                         strcpy(tplatform,platform);
                         suppressBlanks(tplatform);
          }
          tncpu   = ncpu;
          tmemory = memory;
          tclock  = clock;
        };
   };

  class ActiveHost {
  
    public :
      char         name[40];
      char         interface[40];
      unsigned int status;
      short int    clactive;
      short int    clallowed;
      int clprocessed;
      int clfailed;
      int clkilled;
      unsigned int lastupd;
      int clock;

      ActiveHost() {};
      ~ActiveHost() {};

      void print() {
        cout<<"Host      : "<<name<<endl;
        cout<<"Interface : "<<interface<<endl;
        cout<<"Status    : "<<status<<endl;
        cout<<"Clients   "<<endl;
        cout<<"Active/Allowed "<<clactive<<"/"<<clallowed<<endl;
        cout<<"Processes/Killed/Failed "<<clprocessed<<"/"<<clkilled
            <<"/"<<clfailed<<endl;
      }
      int set(
              const char *thost,
              const char *tinterface,
              unsigned int tstatus,
              short int    tclactive,
              short int    tclallowed,
              int tclprocessed,
              int tclfailed,
              int tclkilled,
              unsigned int tlastupd,
              int tclock)
        {
          if (thost) {
           strcpy(name,thost);
           suppressBlanks(name);
          }
          if (tinterface) {
           strcpy(interface,tinterface);
           suppressBlanks(interface);
          }
          status      = tstatus;
          clactive    = tclactive;
          clallowed   = tclallowed;
          clprocessed = tclprocessed;
          clfailed    = tclfailed;
          clkilled    = tclkilled;
          lastupd     = tlastupd;
          clock       = tclock;
        };

      int get(
              char *thost,
              char *tinterface,
              unsigned int &tstatus,
              short int    &tclactive,
              short int    &tclallowed,
              int &tclprocessed,
              int &tclfailed,
              int &tclkilled,
              unsigned int &tlastupd,
              int &tclock)
        {
          if (thost) strcpy(thost,name);
          if (tinterface) strcpy(tinterface,interface);
          tstatus      = status;
          tclactive    = clactive;
          tclallowed   = clallowed;
          tclprocessed = clprocessed;
          tclfailed    = clfailed;
          tclkilled    = clkilled;
          tlastupd     = lastupd;
          tclock       = clock;


        };
  };

  class DSTInfo {
    public :
      unsigned int dsttype;
      unsigned int uid;
      char         hostname[40];
      char         dirpath[1024];
      unsigned int runmode;
      long int updfreq;
      long int diehard;
      unsigned long int freespace;
      unsigned long int totalspace;

      DSTInfo() {};
      ~DSTInfo() {};
     
      int set(      const unsigned int dtype,
                    const unsigned int tuid,
                    const char *thostname,
                    const char *tdirpath,
                    unsigned int trunmode,
                    long int tupdfreq,
                    long int tdiehard,
                    long int tfreespace,
                    long int ttotalspace)
        {
          int rstat = 0;
          if (thostname && tdirpath) {
           dsttype = dtype;
           uid = tuid;
           strcpy(hostname,thostname);
           suppressBlanks(hostname);
           strcpy(dirpath,tdirpath);
           suppressBlanks(dirpath);
           runmode = trunmode;
           diehard = tdiehard;
           freespace = tfreespace;
           totalspace = ttotalspace;
           updfreq    = tupdfreq;
           rstat = 1;
          }
          return rstat;
        }
      int get(      unsigned int &ttype,
                    unsigned int &tuid,
                    char *thostname,
                    char *tdirpath,
                    unsigned int &trunmode,
                    int &tupdfreq,
                    int &tdiehard,
                    int &tfreespace,
                    int &ttotalspace)
        {
          int rstat = 0;

          if (thostname && tdirpath) {
            ttype = dsttype;
           tuid = uid;
           strcpy(thostname,hostname);
           strcpy(tdirpath,dirpath);
           trunmode = runmode;
           tdiehard = diehard;
           tfreespace =  freespace;
           tupdfreq   =  updfreq;
           ttotalspace = totalspace;
           rstat = 1;
          }
          return rstat;
        }
  };
  class DST {
    public :
                    unsigned int Insert;
                    unsigned int Begin;
                    unsigned int End;
                    long int Run;
                    long int FirstEvent;
                    long int LastEvent;
                    long int Events;
                    unsigned int Status;
                    unsigned int Type;
                    long int Size;
                    char Name[1024];

      DST() {};
      ~DST() {};
     
      int set(      
                  const unsigned int insert,
                  const unsigned int begin,
                  const unsigned int end,
                  const long int run,
                  const long int firstevent,
                  const long int lastevent,
                  const long int eventnumber,
                  unsigned int status,
                  unsigned int type,
                  long int size,
                  const char *name)
        {
          int rstat = 0;
          if (name) {
            Insert = insert;
            Begin  = begin;
            End    = end;
            Run    = run;
            FirstEvent = firstevent;
            LastEvent = lastevent;
            Events    = eventnumber;
            Status    = status;
            Type      = type;
            Size      = size;
            strcpy(Name,name);
           rstat = 1;
          }
          return rstat;
        }
      int get(
                  unsigned int &insert,
                  unsigned int &begin,
                  unsigned int &end,
                   int &run,
                   int &firstevent,
                   int &lastevent,
                   int &eventnumber,
                  unsigned int &status,
                  unsigned int &type,
                   int &size,
                  char    *name)
        {
          int rstat = 0;
          if (name) {
            insert = Insert;
            begin  = Begin;
            end    = End;
            run    = Run;
            firstevent = FirstEvent;
            lastevent = LastEvent;
            eventnumber    = Events;
            status    = Status;
            type      = Type;
            size      = Size;
            strcpy(name,Name);
          }
          return rstat;
        }
  };


  void Fmessage(const char *subr, const char *text, const char *errcode);
  int  addtdvname(const char *tdvnameS, int datamc, int &id);
  int  checktdvname(char *name, char *tablename);
  int  cleanRunTable(const unsigned int runstatus);
  int  resetProdDSTTable();
  int  resetRunTable();
  void commit();
  int  counttdv(const char *name, int datamc, long timef, long timel); 
  int  counttdvlob(const char *name, int datamc, long timef, long timel); 
  int  createtdvtable(char *tablename);

  int  deleteActiveClient(const unsigned int type);
  int  deleteActiveClient(const unsigned int id, const unsigned type);
  int  deleteActiveClientId(const unsigned int id, const unsigned type);
  int  deleteActiveClientRef(const unsigned int id, const unsigned type);

  int  deleteActiveHost(const unsigned int cltype);
  int  deleteActiveHost(const char *host);
  int  deleteActiveHost(unsigned int type, const char *host);

  int  deleteDSTInfo();
  int  deleteDSTInfo(const unsigned int id);
  int  deleteDST(unsigned int type);
  int  deleteDST(const char *name, unsigned int type);

  int  deleteRun(unsigned int run, unsigned int id);
  int  deleteProdRun(unsigned int run, unsigned int id);
  int  deleteProdInfo(unsigned int run, unsigned int id);

  int  deleteHostActiveClients(unsigned int type, const char *host); 
  int  deleteHostActiveClients(const char *host); 

  int  deleteNominalClient(const unsigned int type);
  int  deleteNominalClient(const unsigned int id, const unsigned int type);

  int  deleteNominalHosts();
  int  deleteNominalHost(const char *host);

  int  deleteProdRun(unsigned int  crun, unsigned int fevent, unsigned int  levent);
  int  deleteProdRun(unsigned int Run);
  int  deleteProdInfo(unsigned int Run);

  int  deleteRunEvInfo(const unsigned int status);

  int  deletetag(EventTag *tag);
  int  deletetags(int idx);

  int  deletetdv(TDVrec *tdv, int flag);

  int  droptable(char* tablename);

  int  dumpRunTable(RunTable *table, int nruns);

  int  findActiveClient(const unsigned int cid, const unsigned int cltype); 
  int  findDST(const char *name, const unsigned int type);
  int  findDSTInfo(const unsigned int uid);
  int  findNominalClient(const unsigned int id, unsigned int type); 

  int  finddeftdv(const char *tdvname, int datamc, TDVutime *utime);
  int  finddeftdvlob(const char *tdvname, int datamc, TDVrec *tdvrec);
  unsigned int  findHost(const char *host);

  int  findActiveHost(const char *host);
  int  findActiveHost(const char *host, unsigned int clienttype);

  int  findRun(unsigned int run, unsigned int uid);

  int  findInRunTable(unsigned int run, unsigned int fevent,
                      unsigned int levent, RunTable *rtable);
  unsigned int findInterface(const char *name);
  unsigned int findPlatform(const char *name);
  unsigned int findmaxprodidx();
  int findProdRun(const unsigned int  crun, unsigned int &idx, 
                        unsigned int &tstamp, 
                        unsigned int &fevent, unsigned int &levent, 
                        bool &flag);
  int  findrun(unsigned int  crun, long &utimef, long &utimel);
  unsigned int findrun(unsigned int  runid);
  int  findtdv(const char *name, int datamc, long utimef, long utimel, 
               int &ntdvs, TDVutime *utime); 
  int  findtdvlob(const char *name, int datamc, long utimef, long utimel, 
                  int &ntdvs, TDVutime *utime); 
  int  findtag(EventTag *tag); 
  int  findtdv(char *name, int datamc, long timeb, long timee, int deftable, long &timei);
  unsigned int  findtdvname(const char *tdvnameS, int datamc);
  unsigned int getActiveClientNextId(const unsigned int ctype);
  unsigned int getNominalClientNextId(const unsigned int ctype);
  int getNominalHostId(const char *hostname);
  unsigned int getNominalHostNextId();
  unsigned int getHostId(const char *name);
  int  getActiveClientN(unsigned int type);
  int  getActiveClientRefN(unsigned int clientid);
  int  getActiveClientDesc(const unsigned int type, ActiveClientDesc *procdesc);
  int  getActiveClientId(const unsigned int type, ActiveClientId *procdesc);
  int  getActiveClientRef(const unsigned int uid, ActiveClientRef *procdesc);
  int  getActiveHostList(unsigned int cltype, unsigned int hostid, ActiveHost *lhost);
  int  getActiveHostN();
  int  getDST(DST *dst);
  int  getDSTInfo(const char *hostname, DSTInfo *dstinfo);
  int  getDSTInfoALL(DSTInfo *dstinfo);
  int  getDSTInfoN();
  int  getDSTN();
  int  getActiveHostN(unsigned int cltype);
  int  getNominalClientN(unsigned int type);
  int  getNominalClientDesc(unsigned int type, ProcDesc *procdesc);
  int  getNominalHostN();
  int  getNominalHostList(NominalHost *lhost);
  int  getProdRunStat(unsigned int crun, ProdRun *runs);
  int  getProductionFlag();
  int  getprodruns(unsigned int run, ProdRun *runs);
  int  getprodrunN(unsigned int run);
  int  getRun(const char *host, RunTable *rtable);
  int  getRunsN();
  int  getRunTable(int &nruns, unsigned int &maxid, RunTable *rtable);
  int  getHost(unsigned int type, ActiveHost *host);
  unsigned int  getHostId(unsigned int cid);
  void gettablename(char *tdvn);
  int  getTableSize(const char *table);
  int  gettdv(TDVrec *tdv, int deftable);
  int  gettdvLOB(TDVrec *tdv, int deftable, unsigned int *pdata);
  int  gettdvbody(TDVrec *tdv, unsigned int *pdata);
  int  decHostClientsActive(unsigned int clientId, unsigned int ctype);
  int  incHostClientsActive(unsigned int clientId, unsigned int ctype);
  int  incHostClientsFailed(unsigned int clientId, unsigned int ctype);
  int  incHostClientsKilled(unsigned int clientId, unsigned int ctype);
  int  incHostClientsProcessed(unsigned int clientId, unsigned int ctype);

  int  initActiveHostTable(const unsigned int status);
  int  initActiveHostTables();
  int  resetActiveHostTables();
  int  initNominalClientTables();
  int  initActiveClientTables();
  int  initProdInfoTable();
  int  resetProdInfoTable();
  int  initProductionJob();

  int  insertActiveClient(const unsigned int id,
                          const unsigned int type,
                                    const unsigned int lastupdate,
                                    const unsigned int starttime,
                                    const unsigned int status);
  int  updateActiveClient(const unsigned int id,
                          const unsigned int type,
                                    const unsigned int lastupdate,
                                    const unsigned int starttime,
                                    const unsigned int status);
  int   insertActiveClientRef(const unsigned int clid,
                              const unsigned int cltype,
                                       const char *ior,
                                       const char *interface,
                                       const unsigned int type,
                                       const unsigned int uid);
  int    insertActiveClientId(const unsigned int id,
                                      const unsigned int pid,
                                      const unsigned int ppid,
                                      const char   *host,
                                      const unsigned int type,
                                      const unsigned int status,
                                      const char   *interface);
  int    updateActiveClientId(const unsigned int id,
                                      const unsigned int type,
                                      const unsigned int status);


  int  insertActiveHost(unsigned int type,
                        const char *host,
                        unsigned int status,
                        short int    clactive,
                        short int    clallowed,
                        unsigned int clprocessed,
                        unsigned int clfailed,
                        unsigned int clkilled,
                        unsigned int lastupd);

  int  updateActiveHost(unsigned int type,
                        const char *host,
                        unsigned int status,
                        short int    clactive,
                        short int    clallowed,
                        unsigned int clprocessed,
                        unsigned int clfailed,
                        unsigned int clkilled,
                        unsigned int lastfailed,
                        unsigned int lastupd);

  int   insertDataCards(const char *scriptpath);

  int   insertDST(
                  const unsigned int Insert,
                  const unsigned int Begin,
                  const unsigned int End,
                  const long int Run,
                  const long int FirstEvent,
                  const long int LastEvent,
                  const long int EventNumber,
                  unsigned int Status,
                  unsigned int Type,
                  long int Size,
                  const char *Name);


  int insertDSTInfo(
                    const unsigned int type,
                    const unsigned int uid,
                    const char *hostname,
                    const char *dirpath,
                    unsigned int runmode,
                    unsigned int updfreq,
                    unsigned int diehard,
                    long int freespace,
                    long int totalspace);

  int  insertHostId(unsigned int id, const char *name);
  int  insertNominalHost(
                                   const char *name,
                                   const char *interface,
                                   const char *platform,
                                   const short int ncpu,
                                   const short int memory,
                                   const int clock);
  int  insertNominalClient(
                           unsigned int uid,
                           unsigned int type,
                           short    int maxclients,
                           float        cpuneeded,
                           short    int memoryneeded,
                           const char           *scriptpath,
                           const char           *logpath,
                           const char           *submitcommand,
                           const char           *hostname,
                           short    int logintheend);

  int  insertProdRun(ProdRun *prun);
  int  insertRunTable(RunTable *table, int nruns);
  int  inserttag(EventTag *tag, int events);
  int  inserttags(EventTag *tag, int events);
  int  inserttdv(TDVrec *tdv, int flag);
  int  inserttdvlob(TDVrec *tdv, int flag, int buflng, unsigned int *buff);
  int  MakeQuery();
  int  oracle_connect();
  int  readtdv(char *filepath, long tdvsize, unsigned int *pdata);
  int  RunsToBeRerun();
  int  RunsProcessing();
  int  settdvstat(int id, long insert, long begin, long end, int prodstat);
  void sql_error(char *msg);
  void sql_nothing(char *msg);
  void sql_notfound(char *msg);


  int  updateHostLastUpdate(unsigned int hostid);

  int  AMSoracle::setActiveHostStatus(const unsigned int status,
                                      const unsigned int type,
                                      const char *hostname);
  int  setClientExitingStatus(unsigned int clientId, unsigned int status);
  int  setClientStatus(unsigned int clientId, unsigned int status);
  
  int  setRunStatus(unsigned int uid,const unsigned int status);
  int  setRunSubmitTime(unsigned int uid,const unsigned int submittime);

  int  suppressCR(char *string);
  void time_to_oracle(const time_t mtime, char* date);
  int  trimtdv(int &tablesize, TDVutime *utime);
  int  UnixFilestat(const char *path, long *id, long *size, long *flags, 
                    time_t *modtime);
  int  updateDST(
                  const unsigned int Insert,
                  const unsigned int Begin,
                  const unsigned int End,
                  const long int Run,
                  const long int FirstEvent,
                  const long int LastEvent,
                  const long int EventNumber,
                  unsigned int Status,
                  unsigned int Type,
                  long int Size,
                  const char *Name);

  int updateDSTInfo(
                    const unsigned int type,
                    const unsigned int uid,
                    const char *hostname,
                    const char *dirpath,
                    unsigned int runmode,
                    unsigned int updfreq,
                    unsigned int diehard,
                    unsigned int freespace,
                    unsigned int totalspace);

  int updateRunTable(RunTable *rtable);
  int updateProdTable(ProdRun *rtable);
  int updateProdRun(ProdRun *prun);
  int updateProductionJob();
};

#endif
