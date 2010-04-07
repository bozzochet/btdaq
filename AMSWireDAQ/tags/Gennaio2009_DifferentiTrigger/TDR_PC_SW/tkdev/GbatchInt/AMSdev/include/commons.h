//  $Id: commons.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
//  Author V. Choutko 24-may-1996


#ifndef __AMSCOMMONS__
#define __AMSCOMMONS__
#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

class AMSDATADIR_DEF{
public:
  integer amsdlength;
  integer amsdblength;
  char amsdatadir[128];
  char amsdatabase[128];
  char fname[200];
};
#define AMSDATADIR COMMON_BLOCK(AMSDATADIR,amsdatadir)
COMMON_BLOCK_DEF(AMSDATADIR_DEF,AMSDATADIR);


class AMSCommonsI{
private:
 static integer _Count;
 static char _version[6];
 static uinteger _build;
 static uinteger _os;
 static char _osname[255];
 static char _osversion[255];
 static float _mips;
 static pid_t _pid;
 static bool _remote;
public:
 AMSCommonsI();
 void init();
 static const char * getversion(){return _version;}
 static integer getbuildno()  {return _build;}
 static integer getmips()  {return integer(_mips);}
 static integer getpid()  {return integer(_pid);}
 static void setremote(bool remote){_remote=remote;}
 static bool remote(){return _remote;}
 static integer getosno()  {return _os;}
 static char* getosname()  {return _osname;}
 static char* getosversion(){return _version;}
};
static AMSCommonsI cmnI;







#endif
