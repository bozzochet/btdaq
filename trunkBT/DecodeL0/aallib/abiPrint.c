/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
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

//  abiPrint.c   Display an AMSBlock

//  Revision History
//    Jun 2004 by Peter Dennett
//      - Initial version at CERN
//    Jul 2004 by Peter Dennett
//      - Direct conversion from MDCSoftware
//    Oct 2008 by Peter Dennett at CERN
//      - Usage of LogPrintf

#include "aallib.h"
#include "abi.h"
#include "logfile.h"

static char *ErrorCode[] = 
{
  "OK",
  "Abort",
  "Error",
  "No Space",
  "IO error",
  "Bad Node Addr",
  "Busy",
  "7",
  "Not exist",
  "DSP error",
  "AMSW CRC error",
  "11",
  "12",
  "Timeout",
  "14",
  "15"
};

#define SHORTN 8

////////////////////////////////////////////////////////////////////////////
//-Extern: abiShow
bool abiShow(pAMSBlock pBlock, int NumberBytesToDump)
//-Synopsis:    Core of print an AMSBlock to stdout
//-Returns:     TRUE, there are no errors possible
//-ARG_I:       pBlock          Pointer to the AMSBlock to display
//-ARG_I:       NumberBytesToDump Number of bytes to dump
//-Description: Core of print an AMSBlock to stdout
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int w, i;
unsigned char *cp;

  w = pBlock->DataCount;

  if (w > (NumberBytesToDump-SHORTN))
    w = (NumberBytesToDump-SHORTN);

  if ((pBlock->Source != 0) || (pBlock->CommandTag != 0) || (pBlock->Error != BLOCK_S_END))
  {
    if ((pBlock->BlockType & BT_REPLY) == 0)
    {
      printf("[OPTS: 0x%X", pBlock->Error);
      if (log_fp)
        fprintf(log_fp, "[OPTS: 0x%X", pBlock->Error);
    }
    else if (pBlock->Error < (sizeof (ErrorCode)) / (sizeof (ErrorCode[0])))
    {
      printf("[STS: %s", ErrorCode[pBlock->Error]);
      if (log_fp)
        fprintf(log_fp, "[STS: %s", ErrorCode[pBlock->Error]);
    }
    else
    {
      printf("[STS: 0x%X", pBlock->Error);
      if (log_fp)
        fprintf(log_fp, "[STS: 0x%X", pBlock->Error);
    }

    if (pBlock->Source != 0)
    {
      printf(" Src:%d", pBlock->Source);
      if (log_fp)
        fprintf(log_fp, " Src:%d", pBlock->Source);
    }

    if (pBlock->CommandTag != 0)
    {
      printf(" Tag:%03X", pBlock->CommandTag);
      if (log_fp)
        fprintf(log_fp, " Tag:%03X", pBlock->CommandTag);
    }

    printf("] ");
    if (log_fp)
      fprintf(log_fp, "] ");
  }    

  printf("[%s %s NA=%03X DT=%06lX DC=%ld] ",
         pBlock->BlockType & 0x0002 ? "RP" : "RQ",
         pBlock->BlockType & 0x0001 ? "W"  : "R",
         pBlock->NodeAddress,
         pBlock->DataType,
         pBlock->DataCount);

  if (log_fp)
    fprintf(log_fp, "[%s %s NA=%03X DT=%06lX DC=%ld] ",
            pBlock->BlockType & 0x0002 ? "RP" : "RQ",
            pBlock->BlockType & 0x0001 ? "W"  : "R",
            pBlock->NodeAddress,
            pBlock->DataType,
            pBlock->DataCount);

  cp = abiDataPtr8(pBlock);
  for (i=0; i<w; i+=2)
  {
    if (((i+1) == w) && (pBlock->DataCount <= (NumberBytesToDump-SHORTN)))
    {
      printf("%02X ", *cp);
      if (log_fp)
        fprintf(log_fp, "%02X ", *cp);
    }
    else
    {
      printf("%02X%02X ", *cp, *(cp+1));
      if (log_fp)
        fprintf(log_fp, "%02X%02X ", *cp, *(cp+1));
    }
    cp += 2;
  }

  if (pBlock->DataCount > (NumberBytesToDump-SHORTN))
  {
    printf(".. ");
    if (log_fp)
      fprintf(log_fp, ".. ");

    cp = abiDataPtr8(pBlock);
    cp += pBlock->DataCount - 6;
    for (i=0; i<3; i++)
    {
      if ((i==2) && (pBlock->DataCount&1))
      {
        printf("%02X ", *cp);
        if (log_fp)
          fprintf(log_fp, "%02X ", *cp);
      }
      else
      {
        printf("%02X%02X ", *cp, *(cp+1));
        if (log_fp)
          fprintf(log_fp, "%02X%02X ", *cp, *(cp+1));
      }
      cp+=2;
    }
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiPrint
bool abiPrint(char *Tag, pAMSBlock pBlock, int NumberBytesToDump)
//-Synopsis:    Print an AMSBlock to stdout
//-Returns:     TRUE, there are no errors possible
//-ARG_I:       Tag             Text to print before the AMSBlock
//-ARG_I:       pBlock          Pointer to the AMSBlock to display
//-ARG_I:       NumberBytesToDump Number of bytes to dump
//-Description: Print an AMSBlock to stdout
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  LogEntryStart();
  printf("%s: ", Tag);
  if (log_fp)
    fprintf(log_fp, "%s: [ ] %s: ", LogTimeStamp, Tag);
  abiShow(pBlock, NumberBytesToDump); 

  printf("\n");
  if (log_fp)
    fprintf(log_fp, "\n");

  LogEntryStop();
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiPrint2
bool abiPrint2(char *Tag1, char *Tag2, pAMSBlock pBlock, int NumberBytesToDump)
//-Synopsis:    Print an AMSBlock to stdout
//-Returns:     TRUE, there are no errors possible
//-ARG_I:       Tag1            Text to print before the AMSBlock
//-ARG_I:       Tag2            Text to print before the AMSBlock
//-ARG_I:       pBlock          Pointer to the AMSBlock to display
//-ARG_I:       NumberBytesToDump Number of bytes to dump
//-Description: Print an AMSBlock to stdout
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  LogEntryStart();
  printf("%s-%s: ", Tag1, Tag2);
  if (log_fp)
    fprintf(log_fp, "%s: %s-%s: ", LogTimeStamp, Tag1, Tag2);

  abiShow(pBlock, NumberBytesToDump);

  printf("\n");
  if (log_fp)
    fprintf(log_fp, "\n");

  LogEntryStop();
  return TRUE;
}
