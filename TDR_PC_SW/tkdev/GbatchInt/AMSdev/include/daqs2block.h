//  $Id: daqs2block.h,v 1.1.1.1 2007/11/13 09:56:11 zuccon Exp $
// 1.0 version 2.07.97 E.Choumilov

#ifndef __AMSDAQS2BLOCK__
#define __AMSDAQS2BLOCK__
#include "typedefs.h"
#include <iostream.h>
#include "link.h"
#include <fstream.h>
#include "tofdbc02.h"
//
// 
//
// S-block class:
//
class DAQS2Block {
private:
  static int16u nodeids[2*TOF2GC::SCCRAT];// valid node_id's (incl. sides)
  static int16u format;// currently processing DATA-format: 0-raw, 1-compressed, 2-mixed, 3-pedcal
  static integer totbll;//total length of all(MAX 4) scint. blocks
public:
  static void clrtbll(){totbll=0;};
  static integer gettbll(){return totbll;};
//
  static void setrawf(){format=0;}
  static void setcomf(){format=1;}
  static void setmixf(){format=2;}
  static void setpedf(){format=3;}
  static int16u getformat(){return format;}
  static void node2crs(int16u nodeid, int16u &cr, int16u &sd);
  static integer checkblockid(int16u id);
  static void buildraw(integer len, int16u *p);
  static integer getmaxblocks();
  static integer calcblocklength(integer ibl);
  static void buildblock(integer ibl, integer len, int16u *p);
};

#endif
