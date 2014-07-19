//  $Id: amscommonsi.h,v 1.4 2008/07/07 18:32:37 zuccon Exp $
//  Author V. Choutko 24-may-1996
//

#ifndef __AMSCOMMONSI__
#define __AMSCOMMONSI__
#include "typedefs.h"

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"

/*!\class AMSDATADIR_DEF
  \brief Class holding the path to the database, It is mapped to a FORTRAN common block
*/
class AMSDATADIR_DEF{
public:
  //! length of amsdatadir
  integer amsdlength;
  //! length of amsdatabase
  integer amsdblength;
  //! Path to the AMS data dir generate from enviromnent variable $AMSDataDir
  char amsdatadir[128];
  //! Path to the AMS database dir generate from enviromnent variable $AMSDataDir
  char amsdatabase[128];
  //! The name of the magnetic field map file
  char fname[200];
};
#define AMSDATADIR COMMON_BLOCK(AMSDATADIR,amsdatadir)
COMMON_BLOCK_DEF(AMSDATADIR_DEF,AMSDATADIR);





/*! \class AMSCommonsI 
  \brief Static Class automatically istanciated with the static object cmnI
  It handles the interaction with the OS and sets the path to the DataBase
*/
class AMSCommonsI{
private:
  //! a counter used be sure init is executed only once
  static integer _Count;
  //! Database version
  static char _version[6];
  //! Code build number
  static uinteger _build;
  //! guessed OS id
  static uinteger _os;
  //! guessed OS name
  static char _osname[255];
  //! guessed OS version
  static char _osversion[255];
  //! guesed system performance
  static float _mips;
  //! pid of the running job
  static pid_t _pid;
  //! Remote database access flag
  static bool _remote;
public:
  //! Std constructor (it calls init)
  AMSCommonsI();
  //! initialization  (it can be called only once)
  void init();
  //! Accessor
  static const char * getversion(){return _version;}
  //! Accessor
  static integer getbuildno()  {return _build;}
  //! Accessor
  static integer getmips()  {return integer(_mips);}
  //! Accessor
  static integer getpid()  {return integer(_pid);}
  //! Accessor
  static bool remote(){return _remote;}
  //! Accessor
  static integer getosno()  {return _os;}
  //! Accessor
  static char* getosname()  {return _osname;}
  //! Accessor
  static char* getosversion(){return _version;}

  //! Setter fot the remote mode
  static void setremote(bool remote){_remote=remote;}

};
static AMSCommonsI cmnI;

#endif
