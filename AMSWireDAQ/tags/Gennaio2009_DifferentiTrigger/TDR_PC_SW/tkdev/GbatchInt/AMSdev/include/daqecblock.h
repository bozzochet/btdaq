// 1.0 version 16.11.2006 by E.Choumilov

#ifndef __AMSDAQECBLOCK__
#define __AMSDAQECBLOCK__
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include "ecaldbc.h"
// 
//
// EC-block class:
//
class DAQECBlock {
private:
  static int16u nodeids[2*ecalconst::ECRT];// valid JINF-node_id's (incl. 2 sides)
  static int16u format;// currently processing DATA-format: 0-raw, 1-compressed, 2-mixed
  static integer totbll;//total length of all(MAX 4) blocks
public:
  static void clrtbll(){totbll=0;};
  static integer gettbll(){return totbll;};
//
  static void setrawf(){format=0;}
  static void setcomf(){format=1;}
  static void setmixf(){format=2;}
  static int16u getformat(){return format;}
  static void node2crs(int16u nodeid, int16u &cr, int16u &sd);
  static integer checkblockid(int16u id);
  static void buildraw(integer len, int16u *p);
  static integer getmaxblocks();
  static integer calcblocklength(integer ibl);
  static void buildblock(integer ibl, integer len, int16u *p);
  static void frbdump(int16u *p, int16u len);//fragment bits-dump 
};

#endif
