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

//  abiFile.c   AMS Block file access and management routines

//  Revision History
//    Mar 2009 by Peter Dennett
//      - Initial version based on AAL's gcslib code that operates on the
//        "one minute" files

#include "aallib.h"
#include "abi.h"
#include "logfile.h"
#include "gsclib.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifndef OS_WIN32
#include <grp.h>
#include <pwd.h>
#endif

//~============================================================================

pAMSBlock abiFileRead(FILE *file, char *FileName, bool SecondaryHeader) 
{
int r_cnt;
int16 h1, h2, sh[3];
int s;

int Length;
int BlockType;
int NodeAddress;
int DataType;
pAMSBlock pBlock;

  r_cnt = 1;
  s = fread(&h1, 2, r_cnt, file);
  if (s != r_cnt) 
  {
    if ( ! feof(file))
      LogError("abiFileRead: %s: *** 1: %s (r_cnt = %d, s = %d)\n", 
               FileName, strerror(errno), r_cnt, s);
    return NULL;
  }

  if (h1 & 0x8000)
  {
    r_cnt = 1;
    s = fread(&h2, 2, r_cnt, file);
    if (s != r_cnt) 
    {
      if ( ! feof(file))
        LogError("abiFileRead: %s: *** 2: %s (r_cnt = %d, s = %d)\n", 
                 FileName, strerror(errno), r_cnt, s);
      return NULL;
    }

    Length = ((h1 & 0x7FFF) << 16) + h2;
  }
  else
  {
    Length = h1;
  }

  r_cnt = 1;
  s = fread(&h1, 2, r_cnt, file);
  if (s != r_cnt) 
  {
    if ( ! feof(file))
      LogError("abiFileRead: %s: *** 3: %s (r_cnt = %d, s = %d)\n", 
               FileName, strerror(errno), r_cnt, s);
    return NULL;
  }

  BlockType = h1 >> 14;
  NodeAddress = (h1 >> 5) & 0x1FF;
  DataType = h1 & 0x1F;

  Length -= 2;

  if (DataType == 0x1F)
  {
    r_cnt = 1;
    s = fread(&h1, 2, r_cnt, file);
    if (s != r_cnt)
    {
      LogError("abiFileRead: %s: *** 4: %s (r_cnt = %d, s = %d)\n", 
               FileName, strerror(errno), r_cnt, s);
      return NULL;
    }

    DataType = 0x1F0000 | h1;
    Length -= 2;
  }

  if (SecondaryHeader)
  {
    if ((BlockType) & 2)
      Length -= 6;
    else
      Length -= 2;
  }

#if 0
  printf("Length %d BlockType %d NodeAddress 0x%X DataType 0x%X\n",
         Length, BlockType, NodeAddress, DataType);
#endif

  if (Length < 0)
  {
    LogError("abiFileRead: %s: *** Invalid block length %d\n", FileName, Length);
    return NULL;
  }

  pBlock = abiConstruct(NodeAddress, DataType, Length, BlockType);
  if ( ! pBlock)
  {
    LogError("abiFileRead: out of memory\n");
    return NULL;
  }

  if (SecondaryHeader)
  {
    if ((pBlock->BlockType) & 2)
    {
      r_cnt = 3;
      s = fread(sh, 2, r_cnt, file);
      if (s != r_cnt)
      {
	LogError("abiFileRead: %s: *** shdr: %s (r_cnt = %d, s = %d)\n", 
		 FileName, strerror(errno), r_cnt, s);
	abiDestruct(pBlock);
	return NULL;
      }
      pBlock->CommandTag = (sh[0] & 0x0FFF);
      pBlock->Error      = (sh[0] >> 12);
      pBlock->DataTime   = (sh[1] << 16) | sh[2];
    }
    else
    {
      r_cnt = 1;
      s = fread(sh, 2, r_cnt, file);
      if (s != r_cnt)
      {
	LogError("abiFileRead: %s: *** shdr: %s (r_cnt = %d, s = %d)\n", 
		 FileName, strerror(errno), r_cnt, s);
	abiDestruct(pBlock);
	return NULL;
      }
      pBlock->CommandTag = (sh[0] & 0x0FFF);
      pBlock->Error      = (sh[0] >> 12);
    }
  }

  if (Length)
  {
    r_cnt = (Length + 1) / 2;

    s = fread(abiDataPtr16(pBlock), 2, r_cnt, file);
    if (s != r_cnt) 
    {
      LogError("abiFileRead: %s: *** data: %s (r_cnt = %d, s = %d)\n", 
               FileName, strerror(errno), r_cnt, s);
      LogError("             Length 0x%X BlockType %d NodeAddress 0x%X DataType 0x%X\n",
	       Length, BlockType, NodeAddress, DataType);
      abiDestruct(pBlock);
      return NULL;
    }
  }

  return pBlock;
}

//~============================================================================

bool abiFileWrite(FILE *file, char *FileName, bool SecondaryHeader,
                  pAMSBlock pBlock) 
{
int16 hdr[6];
int s;
int dc;

int i = 0;

  dc = pBlock->DataCount + 2;
#if 0  // The header data count is number of bytes but int16 is always written.
  if (dc & 1)
    dc += 1;
#endif

  if (SecondaryHeader)
  {
    if((pBlock->BlockType) & 2) dc += 6; // time MSB+LSB only in reply
    else                        dc += 2; //              not  in request
  }

  if ((pBlock->DataType & 0x1F0000) == 0x1F0000)
    dc += 2;

  if (dc > 0x7FFF)
    hdr[i++] = (dc >> 16) | 0x8000;

  hdr[i++] = (int16) dc;

  hdr[i] = (pBlock->BlockType << 14) | ((pBlock->NodeAddress&0x1FF) << 5);
  if (pBlock->DataType >= 0x1F)
  {
    hdr[i++] |= 0x1f;
    hdr[i++] = (int16) pBlock->DataType;
  }
  else
  {
    hdr[i++] |= pBlock->DataType;
  }

  if (SecondaryHeader)
  {
    hdr[i++] = (pBlock->CommandTag & 0xFFF) | (pBlock->Error << 12);
    if ((pBlock->BlockType) & 2)
    {
      hdr[i++] = pBlock->DataTime >> 16;
      hdr[i++] = (int16)pBlock->DataTime;
    }
  }

  s = fwrite(hdr, 2, i, file);
  if (s != i)
  {
    LogError("abiFileWrite: %s: *** hdr len: %d s: %d\n", FileName, i, s);
    return FALSE;
  }

  i = (pBlock->DataCount + 1) / 2;
  s = fwrite(abiDataPtr16(pBlock), 2, i, file);
  if (s != i)
  {
    LogError("abiFileWrite: %s: *** data len: %d s: %d\n", FileName, i, s);
    return FALSE;
  }

  return TRUE;
}

//~----------------------------------------------------------------------------

FILE *abiFileOpenTemp(char *Path, int *FileNumber)
{
char FileName[256];
FILE *fOut=NULL;

  remove_trailing_slash(Path);
  dir_check_and_create(Path);
  *FileNumber = the_last_file_in(Path);
  if (*FileNumber < 0)
    *FileNumber = 0;
  else
    *FileNumber += 1;

  sprintf(FileName, "%s/temp-%07d", Path, *FileNumber);

  fOut = fopen(FileName, "wb");
  if (fOut == NULL)
    LogError("Open of %s failed\n", FileName);

  return fOut;
}

//~----------------------------------------------------------------------------

int abiFileCommitTemp(char *Path, int FileNumber, char *user, char *group)
{
DIR *dir;
char b[800], bb[800];
int n[6] = {1, 10, 100, 1000, 10000, 100000};

#ifndef OS_WIN32
struct passwd *pwd=NULL;
struct group *grp=NULL;

  pwd = getpwnam(user);
  if (!pwd) 
  {
    printf("getpwnam('%s') failed: %s\n", user, strerror(errno));
    //??return 0;
  }

  grp = getgrnam(group);
  if (!grp) 
  {
    printf("getgrnam('%s') failed: %s\n", group, strerror(errno));
    //??return 0;
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
      if (grp && pwd)
      {
        if (chown(bb, pwd->pw_uid, grp->gr_gid)) 
        {
          printf("chown(%s, '%s', '%s') failed: %s\n", bb, user, group, strerror(errno));
          return 0;
        }
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
  if (grp && pwd)
  {
    if (chown(bb, pwd->pw_uid, grp->gr_gid)) 
    {
      printf("chown(%s, '%s', '%s') failed: %s\n", bb, user, group, strerror(errno));
      return 0;
    }
  }
#endif

  if (chmod(bb, 0444))
  {
    printf("chmod(%s, 0444) failed: %s\n", bb, strerror(errno));
  }

  return 1;
}

//~----------------------------------------------------------------------------

int abiFileUnlinkTemp(char *Path, int FileNumber)
{
  //char FileName[256];

  if ( ! gcsUnlinkTempFile(Path, FileNumber))
    return 0;

  return 1;
}

//~============================================================================

#ifdef DEBUG_abiFile

#include "gsclib.h"

int main(int argc, char **argv)
{
char *prog;
char *fnOut;
FILE *fIn=NULL;
FILE *fOut=NULL;
pAMSBlock pBlock;
int cnt = 0;

int LastFile = -1;
int FirstFile = -1;
char *Path = "\\c\\Data\\BLOCKS\\CAN";

char FileName[256];
int FileNumber;

//  LogOpen2("/Data/log", "abiFileTest");

  prog =  *argv++;
  Path =  *argv++;
  fnOut = *argv++;

  if (argc != 3)
  {
    printf("Usage: %s Path  output_block_file\n", prog);
    return (-1);
  }

  remove_trailing_slash(Path);

  if (FirstFile < 0)
    FirstFile = 0;
  if (LastFile < 0)
    LastFile = the_last_file_in(Path);

  if (LastFile < 0)
  {
    printf("No files appear to be in path: %s\n", Path);
    exit(1);
  }

  FileNumber = FirstFile;

  fOut = fopen(fnOut, "wb");
  if (fOut == NULL)
  {
    LogError("Open of %s failed\n", fnOut);
    exit(-3);
  }

  while (FirstFile <= LastFile)
  {
    while (fIn == NULL)
    {
      sprintf(FileName, "%s/%04d/%03d", Path, FileNumber/1000, FileNumber%1000);
      fIn = fopen(FileName, "rb");
      if (fIn == NULL)
      {
        printf("File %s failed to open, skipping\n", FileName);
        FileNumber++;
        if (FileNumber > LastFile)
        {
          LastFile = -1;
          break;
        }
      }
      else
      {
        printf("File %s opened\n", FileName);
      }
    }

    while((pBlock = abiFileRead(fIn, FileName, TRUE)) != NULL)
    {
      cnt++;
      abiPrint("  Block", pBlock, 32);
      if ( ! abiFileWrite(fOut, fnOut, TRUE, pBlock))
        LogError("Write for frame %d failed\n", cnt);

      abiDestruct(pBlock);
    }

    fclose(fIn);
    fIn = NULL;
    FileNumber++;
    if (FileNumber > LastFile)
    {
      LastFile = -1;
      break;
    }
  }

  printf("Copied %d blocks\n", cnt);

  exit(0);
}
#endif

