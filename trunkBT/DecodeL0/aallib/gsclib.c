// file gsclib.c
//
// Stolen from Cai, SEU, VK and reworked
//
// A.Lebedev  Nov-2007...
//

#include "sspdefs.h"
#include "al_lib.h"
#include "logfile.h"

#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#ifndef OS_WIN32
#include <fnmatch.h>
#include <grp.h>
#include <pwd.h>
#endif

#include "gsclib.h"

static int P = 0;

//~----------------------------------------------------------------------------

void remove_trailing_slash(char *s) 
{
  if (s[strlen(s)-1] == '/') s[strlen(s)-1] = '\0';
}

//~----------------------------------------------------------------------------

int last_file_in(char *DirPath, int n) 
{
int last_file;
DIR  *dir;
struct dirent *d;
#ifdef OS_WIN32
 int i;
#else
char *str[8] = {
  "[0-9]",
  "[0-9][0-9]",
  "[0-9][0-9][0-9]",
  "[0-9][0-9][0-9][0-9]",
  "[0-9][0-9][0-9][0-9][0-9]",
  "[0-9][0-9][0-9][0-9][0-9][0-9]",
  "[0-9][0-9][0-9][0-9][0-9][0-9][0-9]",
  "[0-9][0-9][0-9][0-9][0-9][0-9][0-9][0-9]"};
#endif

  if (P) printf("-- Start scan of directory:  %s\n", DirPath);
  dir = opendir(DirPath);
  if (!dir) 
  {
    printf("opendir(%s) failed: %s\n", DirPath, strerror(errno));
    return -2;
  }

  last_file = -1;
  while ((d = readdir(dir))) 
  {
#ifdef OS_WIN32
    for (i=0; i<n; i++)
      if ( ! isdigit(d->d_name[i]))
        goto SkipFile;
      last_file = MAX(last_file, atol(d->d_name));   
  SkipFile:
    ;
#else
    if (!fnmatch(str[n-1], d->d_name, 0))
      last_file = MAX(last_file, atol(d->d_name));
#endif
  }

  if (closedir(dir)) 
  {
    printf("closedir(%s) failed: %s\n", DirPath, strerror(errno));
    //    return -1; 
  }

  if (P) 
  {
    if (last_file < 0) 
      printf("-- No files found in %s\n", DirPath);
    else
      printf("-- Last file found in %s is %0*d\n", DirPath, n, last_file);
  }
  
  return last_file;
}

//~----------------------------------------------------------------------------

int the_last_file_in(char *directory) 
{
int last_file[2];
char b[800];
int n[6] = {1, 10, 100, 1000, 10000, 100000};
  
  sprintf(b, "%s", directory);
  last_file[0] = last_file_in(b, 4);
  if (last_file[0] < 0) return -1;

  sprintf(b, "%s/%04d", directory, last_file[0]);
  last_file[1] = last_file_in(b, 3);
  if (last_file[1] < 0) 
    return (last_file[0] * n[3]) - 1;  //  Return file number ending in 999

  return (last_file[0] * n[3] + last_file[1]);
}

//~----------------------------------------------------------------------------

FILE *gcsOpenTempFile(char *Path, int FileNumber)
{
char FileName[256];
FILE *frames;

  sprintf(FileName,  "%s/temp-%07d", Path, FileNumber);
  frames = fopen(FileName, "wb");

  if ( ! frames)
    LogError("fopen failed. %s %s\n", FileName, strerror(errno));

  return frames;
}

//~----------------------------------------------------------------------------

int gcsRenameTempFile(char *Path, int FileNumber, int SyncFileNumber)
{
char b[800], bb[800];

  sprintf(b,  "%s/temp-%07d", Path, FileNumber);
  sprintf(bb, "%s/temp-%07d", Path, SyncFileNumber);

  if (rename(b, bb)) 
  {
    printf("rename(%s,%s) failed: %s\n", b, bb, strerror(errno));
    return 0;
  }

  return 1;
}

//~----------------------------------------------------------------------------

int gcsUnlinkTempFile(char *Path, int FileNumber)
{
char FileName[256];

  sprintf(FileName,  "%s/temp-%07d", Path, FileNumber);
  LogNormal("Removing file \"%s\"\n", FileName);
  if (unlink(FileName) == 0)
    return 1;
  else
    return 0;
}

//~----------------------------------------------------------------------------

int gcsMoveTempFile(char *Path, int FileNumber, char *user, char *group)
{
DIR *dir;
char b[800], bb[800];
int n[6] = {1, 10, 100, 1000, 10000, 100000};

#ifndef OS_WIN32
struct passwd *pwd;
struct group *grp;

  pwd = getpwnam(user);
  if (!pwd) 
  {
    printf("getpwnam('%s') failed: %s\n", user, strerror(errno));
    return 0;
  }

  grp = getgrnam(group);
  if (!grp) 
  {
    printf("getgrnam('%s') failed: %s\n", group, strerror(errno));
    return 0;
  }
#endif

  sprintf(bb, "%s/%04d", Path, FileNumber / n[3]);
  dir = opendir(bb);
  if (!dir) 
  {
#ifdef OS_WIN32
    if (mkdir(bb))
#else
    if (mkdir(bb, 0777))
#endif
    {
      printf("mkdir(%s,...) failed: %s\n", bb, strerror(errno));
      return 0;
    }
    else 
    {
#ifdef OS_WIN32
      ;
#else
      if (chown(bb, pwd->pw_uid, grp->gr_gid)) 
      {
        printf("chown(%s, '%s', '%s') failed: %s\n", bb, user, group, strerror(errno));
        return 0;
      }
#endif
    }
  }
  else 
  {
    closedir(dir);
  }
  
  sprintf(b,  "%s/temp-%07d", Path, FileNumber);
  sprintf(bb, "%s/%03d", bb, FileNumber % n[3]);
  if (rename(b, bb)) 
  {
    printf("rename(%s,%s) failed: %s\n", b, bb, strerror(errno));
    return 0;
  }

#ifndef OS_WIN32
  if (chown(bb, pwd->pw_uid, grp->gr_gid)) 
  {
    printf("chown(%s, '%s', '%s') failed: %s\n", bb, user, group, strerror(errno));
    return 0;
  }
#endif

  if (chmod(bb, 0440))
  {
    printf("chmod(%s, 0440) failed: %s\n", bb, strerror(errno));
  }

  return 1;
}

//----------------------------------------------------------
//  Function: dir_check_and_create
//      Check directory and create if it is not existing
//
//  Inputs:
//      name ---- Directory name
//
//  Return:
//      = 0 : Directory is OK
//      < 0 : Failed, error code is returned
//----------------------------------------------------------
int dir_check_and_create(char *name) {
    char n[100];
    DIR *dir;
    char *p, *p1;
    int i;

    /* Make a copy of name string */
    strcpy(n, name);

    /* Remove '/' at the end of name string */
    p = strrchr(n, '/');
    if ( p != NULL && ((int) (p - n) + 1) == strlen(n) ) p[0] = '\0';

    /* Check directory */
    dir = opendir(n);
    if ( ! dir ) {
        /* Failed */
        if ( errno != ENOENT ) return (-errno);

        /* Create directory */
        for ( p = NULL, i = 1; i > 0; ) {
#ifdef  OS_WIN32
            if ( mkdir(n) ) {
#else   // OS_WIN32
            if ( mkdir(n, 0777) ) {
#endif  // OS_WIN32
                /* Failed on create, except directory not exist */
                if ( errno != ENOENT ) return (-errno);

                /* Go to one level up */
                p1 = p;
                p = strrchr(n, '/');
                if ( p == NULL ) return (-ENOENT);

                p[0] = '\0';
                if ( p1 ) p1[0] = '/';
                i = 2;
            }
            else {
                printf("Directory %s is created\n", n);

                /* Restore original directory */
                if ( p ) p[0] = '/';
                p = NULL;
                i--;
            }
        }

        /* Check again */
        dir = opendir(n);
        if ( ! dir ) return (-errno);
    }
    closedir(dir);
    return 0;
}

//----------------------------------------------------------
//  Function: dir_check_and_create_2
//      Check directory and create if it is not existing
//      --Sets owner and group
//
//  Inputs:
//      name ---- Directory name
//
//  Return:
//      = 0 : Directory is OK
//      < 0 : Failed, error code is returned
//----------------------------------------------------------
int dir_check_and_create_2(char *name, char *user, char *group) {
    char n[100];
    DIR *dir;
    char *p, *p1;
    int i;

#ifndef OS_WIN32
struct passwd *pwd;
struct group *grp;

  pwd = getpwnam(user);
  if (!pwd) 
  {
    printf("getpwnam('%s') failed: %s\n", user, strerror(errno));
    return 0;
  }

  grp = getgrnam(group);
  if (!grp) 
  {
    printf("getgrnam('%s') failed: %s\n", group, strerror(errno));
    return 0;
  }
#endif

    /* Make a copy of name string */
    strcpy(n, name);

    /* Remove '/' at the end of name string */
    p = strrchr(n, '/');
    if ( p != NULL && ((int) (p - n) + 1) == strlen(n) ) p[0] = '\0';

    /* Check directory */
    dir = opendir(n);
    if ( ! dir ) {
        /* Failed */
        if ( errno != ENOENT ) return (-errno);

        /* Create directory */
        for ( p = NULL, i = 1; i > 0; ) {
#ifdef  OS_WIN32
            if ( mkdir(n) ) {
#else   // OS_WIN32
            if ( mkdir(n, 0777) ) {
#endif  // OS_WIN32
                /* Failed on create, except directory not exist */
                if ( errno != ENOENT ) return (-errno);

                /* Go to one level up */
                p1 = p;
                p = strrchr(n, '/');
                if ( p == NULL ) return (-ENOENT);

                p[0] = '\0';
                if ( p1 ) p1[0] = '/';
                i = 2;
            }
            else {
#ifndef OS_WIN32
              if (chown(n, pwd->pw_uid, grp->gr_gid)) 
              {
                printf("chown(%s, '%s', '%s') failed: %s\n", n, user, group, strerror(errno));
                return 0;
              }
#endif                
                printf("Directory %s is created\n", n);

                /* Restore original directory */
                if ( p ) p[0] = '/';
                p = NULL;
                i--;
            }
        }

        /* Check again */
        dir = opendir(n);
        if ( ! dir ) return (-errno);
    }
    closedir(dir);
    return 0;
}
