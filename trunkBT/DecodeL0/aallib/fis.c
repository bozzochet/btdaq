/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                          (281) 334 3800                                   */
/*                      http://www.padsoft.com                               */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  fis.c   File Identifier Series

//  Revision History
//    Jun 2009 by Peter Dennett
//      - Initial version CERN clean room based in part on code from
//        Cai, SEU, VK, and AAL (gcslib.c)

#include "aallib.h"
#include "al_lib.h"
#include "sspdefs.h"
#include "logfile.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef OS_WIN32
#include <grp.h>
#include <pwd.h>
#endif

#include "fis.h"

//~----------------------------------------------------------------------------

void fisRemoveTrailingSlash(char *s)
{
  if (s[strlen(s)-1] == '/') s[strlen(s)-1] = '\0';
}

//~----------------------------------------------------------------------------
//  Added by X.Cai on 2020.11.03

int fisFirstFileInDir(char *DirPath, int n)
{
    int first_file;
    DIR  *dir;
    struct dirent *d;
    int i, ip;
    char name[n+3];

    dir = opendir(DirPath);
    if ( ! dir) {
        LogError("%s: opendir(%s) failed: %s\n", __func__, DirPath, strerror(errno));
        return -2;
    }

    first_file = 10000000;
    while ((d = readdir(dir))) {
        name[n] = 0;
        ip = 0;
        for (i=0; i<n; i++) {
            if ( ! isdigit(d->d_name[i])) goto SkipFile;
            name[ip++] = d->d_name[i];
        }

        name[ip] = 0;
        first_file = MIN(first_file, atol(name));

SkipFile:
        ;
    }

    if (closedir(dir)) {
        LogError("%s: closedir(%s) failed: %s\n", __func__, DirPath, strerror(errno));
    }

#if 0
  if (first_file < 0)
    printf("-- No files found in %s\n", DirPath);
  else
    printf("-- first file found in %s is %0*d\n", DirPath, n, first_file);
#endif

  return first_file;
}

//~----------------------------------------------------------------------------
// Added by X.Cai on 2020.11.03

int fisFirstFile(char *directory)
{
    char b[800];
    int firstdir;
    int firstfile;

    firstdir = fisFirstFileInDir(directory, 4);
    if (firstdir < 0) return -1;

    sprintf(b, "%s/%04d", directory, firstdir);
    firstfile = fisFirstFileInDir(b, 3);

    if (firstfile < 0)
        return firstdir * 1000 - 1;  //  Return file number ending in 999

    return firstdir * 1000 + firstfile;
}

//~----------------------------------------------------------------------------

int fisLastFileInDir(char *DirPath, int n)
{
int last_file;
DIR  *dir;
struct dirent *d;
int i, ip;
char name[n+3];

  dir = opendir(DirPath);
  if ( ! dir)
  {
    LogError("fisLastFileInDir: opendir(%s) failed: %s\n", DirPath,
             strerror(errno));
    return -2;
  }

  last_file = -1;
  while ((d = readdir(dir)))
  {
    name[n] = 0;
    ip = 0;
    for (i=0; i<n; i++)
    {
      if ( ! isdigit(d->d_name[i]))
        goto SkipFile;

      name[ip++] = d->d_name[i];
    }

    name[ip] = 0;
    last_file = MAX(last_file, atol(name));

  SkipFile:
    ;
  }

  if (closedir(dir))
  {
    LogError("fisLastFileInDir: closedir(%s) failed: %s\n", DirPath,
             strerror(errno));
  }

#if 0
  if (last_file < 0)
    printf("-- No files found in %s\n", DirPath);
  else
    printf("-- Last file found in %s is %0*d\n", DirPath, n, last_file);
#endif

  return last_file;
}

//~----------------------------------------------------------------------------

int fisLastFile(char *directory)
{
char b[800];
int lastdir;
int lastfile;

  lastdir = fisLastFileInDir(directory, 4);
  if (lastdir < 0)
    return -1;

  sprintf(b, "%s/%04d", directory, lastdir);
  lastfile = fisLastFileInDir(b, 3);

  if (lastfile < 0)
    return lastdir * 1000 - 1;  //  Return file number ending in 999

  return lastdir * 1000 + lastfile;
}

//~----------------------------------------------------------------------------

FILE *fisOpenBaseFile(char *Path, int FileNumber, char *extension)
{
char FileName[800];
FILE *frames;

  sprintf(FileName,  "%s/%07d%s", Path, FileNumber, extension);
  frames = fopen(FileName, "wb");

  if ( ! frames)
    LogError("fisOpenBaseFile: fopen failed. %s %s\n", FileName,
             strerror(errno));
#if 0
  else
    LogNormal("fisOpenBaseFile: opened %s\n", FileName);
#endif

  return frames;
}

//~----------------------------------------------------------------------------

int fisUnlinkBaseFile(char *Path, int FileNumber, char *extension)
{
char FileName[800];
int rtn;

  sprintf(FileName,  "%s/temp-%07d%s", Path, FileNumber, extension);

  rtn = (unlink(FileName) == 0);

  if ( ! rtn)
    LogError("fisUnlinkBaseFile: unlink(%s) (%s)\n", FileName, strerror(errno));

  return rtn;
}

//~----------------------------------------------------------------------------

int fisMoveBaseFile(char *Path, int FileNumber, char *extension, char *user,
                    char *group)
{
DIR *dir;
char b[800], bb[800];

#ifndef OS_WIN32
struct passwd *pwd;
struct group *grp;

  pwd = getpwnam(user);
  if (!pwd)
  {
    LogError("fisMoveBaseFile: getpwnam('%s') failed: %s\n", user, strerror(errno));
    return 0;
  }

  grp = getgrnam(group);
  if (!grp)
  {
    LogError("fisMoveBaseFile: getgrnam('%s') failed: %s\n", group, strerror(errno));
    return 0;
  }
#endif

  sprintf(bb, "%s/%04d", Path, FileNumber / 1000);

  dir = opendir(bb);
  if (!dir)
  {
#ifdef OS_WIN32
    if (mkdir(bb))
#else
    if (mkdir(bb, 0777))
#endif
    {
      LogError("fisMoveBaseFile: mkdir(%s,...) failed: %s\n", bb, strerror(errno));
      return 0;
    }
    else
    {
#ifdef OS_WIN32
      ;
#else
      if (chown(bb, pwd->pw_uid, grp->gr_gid))
      {
        LogError("fisMoveBaseFile: chown(%s, '%s', '%s') failed: %s\n", bb, user,
                 group, strerror(errno));
        return 0;
      }
#endif
    }
  }
  else
  {
    closedir(dir);
  }

  sprintf(b,  "%s/%07d%s", Path, FileNumber, extension);
  sprintf(bb, "%s/%03d", bb, FileNumber % 1000);
  if (rename(b, bb))
  {
    LogError("fisMoveBaseFile: rename(%s,%s) failed: %s\n", b, bb, strerror(errno));
    return 0;
  }

#ifndef OS_WIN32
  if (chown(bb, pwd->pw_uid, grp->gr_gid))
  {
    LogError("fisMoveBaseFile: chown(%s, '%s', '%s') failed: %s\n", bb, user,
             group, strerror(errno));
    return 0;
  }
#endif

  if (chmod(bb, 0440))
  {
    LogError("fisMoveBaseFile: chmod(%s, 0440) failed: %s\n", bb, strerror(errno));
  }

  return 1;
}

//~----------------------------------------------------------------------------

int fisDirCheckAndCreate(char *name)
{
char n[800];
DIR *dir;
char *p, *p1;
int i;

  strcpy(n, name);

  p = strrchr(n, '/');
  if ( p != NULL && ((int) (p - n) + 1) == strlen(n) )
    p[0] = '\0';

  dir = opendir(n);
  if ( ! dir )
  {
    if ( errno != ENOENT )
      return (-errno);

    for ( p = NULL, i = 1; i > 0; )
    {
      if (
#ifdef  OS_WIN32
          mkdir(n)
#else   // OS_WIN32
          mkdir(n, 0777)
#endif  // OS_WIN32
          )
      {
        if ( errno != ENOENT )
          return (-errno);

        p1 = p;
        p = strrchr(n, '/');
        if ( p == NULL )
          return (-ENOENT);

        p[0] = '\0';
        if ( p1 ) p1[0] = '/';
        i = 2;
      }
      else
      {
        LogNormal("fisDirCheckAndCreate: Directory %s is created\n", n);

        if ( p ) p[0] = '/';
        p = NULL;
        i--;
      }
    }
    dir = opendir(n);
    if ( ! dir )
      return (-errno);
  }

  closedir(dir);
  return 0;
}
