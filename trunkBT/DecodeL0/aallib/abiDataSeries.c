/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2017 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                  searching for dark and missing funding                   */
/*                                                                           */
/*                        All rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                          61 Harbor Lane                                   */
/*                        Kemah, Texas 77565                                 */
/*                         +1 713 899 6100                                   */
/*                       pdennett@padsoft.com                                */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  abiDataSeries.c     Select a series of AMSBlocks from a BLOCK stream

//  Revision History
//    Mar 2017 by Peter Dennett
//      - Initial version - BI

#include "abiDataSeries.h"

#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <fnmatch.h>

static int Debug = 0;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define FileIndexBump 1000

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

// -1 if not a directory at all
// -2 if empty directory
// -3 if no entries qualifies

static int FirstFileInDir(char *path, int lowest, int n)
{
DIR  *dir;
struct dirent *d;
int FirstFile = 0x7FFFFFFF;
int i;
int ents = 0;

  dir = opendir(path);
  if ( ! dir)
    return -1;

  while ((d = readdir(dir))) 
  {
    for (i=0; i<n; i++)
    {
      if ( ! isdigit(d->d_name[i]))
        goto SkipFile;
    }

    if (d->d_name[n] != 0) 
      goto SkipFile;

    i = atol(d->d_name);
    if (i >= lowest)
      FirstFile = MIN(FirstFile, i);   

    ents++;

    if (FirstFile == lowest)
      break;

  SkipFile: 
    ;
  }

  closedir(dir);

  if (FirstFile != 0x7FFFFFFF)
    return FirstFile;

  return (ents!=0)?(-3):(-2);
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static void RemoveTrailingSlash(char *s) 
{
  if (s[strlen(s)-1] == '/') s[strlen(s)-1] = '\0';
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static int FirstFileInPath(char *path, int lowest, int Debug)
{
int FirstDir  = lowest / 1000;
int FirstFile = lowest % 1000;
int FileId = 19999999;
int j;
char pathplus[256];
int status;
struct stat Stat;             // For FirstFile  


  while (FileId == 19999999)
  {
    j = FirstFileInDir(path, FirstDir, 4);
    if (j < 0)
      return j;

    FirstDir = j;
    sprintf(pathplus, "%s/%04d", path, FirstDir);

#if 0
    if (Debug > 2)
      LogNormal("  FirstFileInPath checking in %s\n", pathplus);
#endif

    while (FileId == 19999999)
    {
      j = FirstFileInDir(pathplus, FirstFile, 3);
      if (j < 0)
      {
        FirstFile = 0;
        if ((j == (-2)) && (Debug > 0))
          printf("Path %s is an empty directory (rmdir?)\n", pathplus);

        break;
      }

      FirstFile = j;

      sprintf(pathplus, "%s/%04d/%03d", path, FirstDir, FirstFile);
      status = stat(pathplus, &Stat);
      if (status < 0)
      {
        if (Debug > 0)
          printf("File %s stat failed (%d) %s\n", pathplus, errno, strerror(errno)); 

        FirstFile++;
        if (FirstFile > 999)
        {
          FirstDir++;
          FirstFile = 0;
          break;
        }
        continue;
      }

      FileId = FirstDir * 1000 + FirstFile;
    }

    FirstDir++;
    FirstFile = 0;

    if (FirstDir > 9999)
      return (-3);
  }

  return FileId;
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static int LastFileInSubdir(char *DirPath, int n) 
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
  
  return last_file;
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

static int LastFileInPath(char *directory) 
{
int last_file[2];
char b[800];
int n[6] = {1, 10, 100, 1000, 10000, 100000};

  sprintf(b, "%s", directory);
  last_file[0] = LastFileInSubdir(b, 4);
  if (last_file[0] < 0) return -1;

  sprintf(b, "%s/%04d", directory, last_file[0]);
  last_file[1] = LastFileInSubdir(b, 3);
  if (last_file[1] < 0) 
    return (last_file[0] * n[3]) - 1;  //  Return file number ending in 999

  return (last_file[0] * n[3] + last_file[1]);
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

pabiFileSeries abiDataSeriesOpen(const char *Path)
{
  pabiFileSeries pabiSeries;
  char FileName[256];
  pAMSBlock pBlock;
  int cnt = 0;

  pabiSeries = (pabiFileSeries)malloc(sizeof (struct _abiFileSeriesStruct));
  if (pabiSeries == NULL) 
    return NULL;

  memset(pabiSeries, 0, sizeof (struct _abiFileSeriesStruct));
  pabiSeries->Path = strdup(Path);
  if ( ! pabiSeries->Path)
  {
    free(pabiSeries);
    return NULL;
  }

  pabiSeries->LastFile  = -1;
  pabiSeries->FirstFile = -1;

  RemoveTrailingSlash(pabiSeries->Path);

  pabiSeries->FirstFile = FirstFileInPath(pabiSeries->Path, 0, FALSE);

  if (pabiSeries->FirstFile < 0)
    pabiSeries->FirstFile = 0;
  if (pabiSeries->LastFile < 0)
    pabiSeries->LastFile = LastFileInPath(pabiSeries->Path);

#if 0
  if (pabiSeries->LastFile < 0)
  {
    printf("No files appear to be in path: %s\n", Path);
    exit(1);
  }
#endif

  pabiSeries->FileNumber = pabiSeries->FirstFile;

  while (pabiSeries->FirstFile <= pabiSeries->LastFile)
  {
    while (pabiSeries->fIn == NULL)
    {
      if ((pabiSeries->FileIndexDimension+1) >= pabiSeries->FileIndexAllocated)
      {
        pabiSeries->FileIndexAllocated += FileIndexBump;
        pabiSeries->pFileIndex = realloc(pabiSeries->pFileIndex, 
                                         pabiSeries->FileIndexAllocated * sizeof(abiFileIndex));
        if ( ! pabiSeries->pFileIndex)
        {
          printf("abiDataSeries: Out of memory for pFileIndex\n");
          free(pabiSeries);
          return NULL;
        }
      }

      pabiSeries->FileIndexDimension += 1;
      memset((char*)&pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1], 0,
             sizeof pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1]);
      pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].FileId = pabiSeries->FileNumber;
      pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].NewestTime = 0xFFFFFFFF;
      pabiSeries->FileIndexTimeSum = 0;

      sprintf(FileName, "%s/%04d/%03d", pabiSeries->Path, pabiSeries->FileNumber/1000,
              pabiSeries->FileNumber%1000);
      pabiSeries->fIn = fopen(FileName, "rb");
      if (pabiSeries->fIn == NULL)
      {
        printf("File %s failed to open, skipping\n", FileName);
        pabiSeries->FileNumber++;
        if (pabiSeries->FileNumber > pabiSeries->LastFile)
        {
          pabiSeries->LastFile = -1;
          break;
        }
      }
      else
      {
        if (Debug > 2)
          printf("Reading file %s - ??\n", FileName);
      }
    }

    while((pBlock = abiFileRead(pabiSeries->fIn, FileName, TRUE)) != NULL)
    {
      cnt++;

      pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].BlocksInFile += 1;
      if (pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].NewestTime > pBlock->DataTime)
        pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].NewestTime = pBlock->DataTime;
      if (pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].OldestTime < pBlock->DataTime)
        pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].OldestTime = pBlock->DataTime;
      pabiSeries->FileIndexTimeSum += pBlock->DataTime;

      abiDestruct(pBlock);
    }

    if (pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].BlocksInFile)
      pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].AverageTime = 
        pabiSeries->FileIndexTimeSum / pabiSeries->pFileIndex[pabiSeries->FileIndexDimension - 1].BlocksInFile;

    fclose(pabiSeries->fIn);
    pabiSeries->fIn = NULL;
    pabiSeries->FileNumber++;
    if (pabiSeries->FileNumber > pabiSeries->LastFile)
    {
      pabiSeries->LastFile = -1;
      break;
    }
  }

  return pabiSeries;
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void abiDataSeriesClose(pabiFileSeries pabiSeries)
{
  if (pabiSeries->fIn != NULL)
    fclose(pabiSeries->fIn);
  if (pabiSeries->pFileIndex)
    free((char*)pabiSeries->pFileIndex);
  if (pabiSeries->Path)
    free(pabiSeries->Path);
  free((char*)pabiSeries);

  return;
}
