/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2007 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  filesystemsize.c   Get the size of a filesystem

//  Revision History
//    Dec 21, 2007 by Peter Dennett
//      - Initial version at PADSOFT

#include "sspdefs.h"
#include "logfile.h"

#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>

#ifdef OS_WIN32
# include <windows.h>
# include <sys/types.h>
#endif

#ifdef OS_LINUX
# include <dirent.h>
#endif

#include <time.h>
#include <sys/stat.h>

#ifdef OS_LINUX
# include <sys/vfs.h>
#endif

#include <fcntl.h>
#include <string.h>			/* strcpy etc.. */

#ifdef OS_LINUX
# include <sys/ioctl.h>
# include <unistd.h>			/*  */
# include <pwd.h>
#endif

double diskfree(char *dir)
{
#ifdef OS_WIN32
	DWORD sectorspc, bytesps, freec, clusters;
	char tmp[4];
	double rtn;

	tmp[0]='\\'; 
  tmp[1]=0;      // Just a failsafe

	if (dir[0]=='/' || dir[0]=='\\') 
  {
		tmp[0]='\\';
		tmp[1]=0;
	} 
  else if (dir[1]==':')
  {
		tmp[0]=dir[0];
		tmp[1]=':';
		tmp[2]='\\';
		tmp[3]=0;
	}

	if (GetDiskFreeSpace(tmp, &sectorspc, &bytesps, &freec, &clusters))
  {
    rtn = ((double)freec * bytesps * sectorspc);
    //printf("diskfree (%s) %lf (%lf %lf %lf)\n", tmp, rtn, freec, bytesps, sectorspc);

    return rtn;
  }
  else
  {
    //printf("diskfree (%s) failed\n", tmp);
    LogError("diskfree (%s) failed\n", tmp);
    return 0.0l;
  }
#else
	struct statfs disk;
	char name[100],*slash;

	strcpy(name,dir);

	if(strlen(name))
  {
		slash = strrchr(name,'/');
		if (slash) slash[1] = 0;
	}
  else 
  {
    strcpy(name,"/");
  }

	if (statfs(name, &disk)) 
    return(-1);

	//LogNormal("filesystemsize: %d * %d = %d\n", disk.f_bavail, disk.f_bsize, disk.f_bavail * disk.f_bsize);
	//??return ( ((double) disk.f_bsize) * ((double) disk.f_bfree));
	return ( ((double) disk.f_bsize) * ((double) disk.f_bavail));
#endif
}
