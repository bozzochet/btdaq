#ifndef __MAIN_HPP__
#define __MAIN_HPP__


#include <iostream>
#include <string>

#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>

//#include <asm/bitops.h>

#include <sys/io.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>

#include <sys/ioctl.h>

#include <linux/limits.h>
#include <dirent.h>

#include <ctype.h>
#include <time.h>

#include <stdarg.h>

#include "amsw_lib.h"
#include "pzlib.h"


#define    SUCCESS        0
#define    ERROR          1

#ifndef    true
# define    true          1
#endif
#ifndef    false
# define    false         0
#endif


#ifdef WITH_COL
# define COLCHAR	   1
# define COL		   "\001"
# define COL_NORMAL	   COL "00"
# define COL_EVIDENT   COL "01"
# define COL_STRONG	   COL "02"
#else
# define COLCHAR         1
# define COL             ""
# define COL_NORMAL      ""
# define COL_EVIDENT     ""
# define COL_STRONG	     ""
#endif


#ifndef    MIN
# define    MIN(x,y)      ( (x)<(y)?(x):(y))
#endif

#ifndef    MAX
# define    MAX(x,y)      ( (x)>(y)?(x):(y))
#endif

#define    WIN_W          800
#define    WIN_H          850

#include "amswcom.h"

/*
class EppLogger : public EppOutput
{
  //	private:
  //	TGTextEdit * te;

	public:
  //	EppLogger ( TGTextEdit * t )      { te = t; };
	void EppPrint(const std::string& s ) { 
		char * p = strdup( s.c_str() );
		if( p[strlen(p)-1] == '\n' )
			p[strlen(p)-1] = '\0';
		cout << p << endl;
		free(p);
	};
};
*/

#endif

