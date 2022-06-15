/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2010 by PADSOFT, Inc.                        */
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

//  egsestats.c   Routines for common handling of statistics data among eAss
//                servers

//  Revision History
//    Apr 2010 by Peter Dennett
//      - Initial version PADSOFT offices

#include "aallib.h"
#include "abi.h"
#include "logfile.h"

#define DDRS_STATS_DEFINE 1
#include "egsedatatypes.h"

int egseClear(void)
{
  memset(&DDRStats, 0, sizeof DDRStats);
  DDRStats.Version = 1;
  return 1;
}

int egseUpdate(char *path)
{
static int fscnt=99;

  if (++fscnt > 60)
  {
    if (path)
      DDRStats.diskfree = diskfree(path) / 1000000.0;
    fscnt = 0;
  }

  DDRStats.HeartBeat++;

  return 1;
}

pAMSBlock egseProcessRequest(pAMSBlock pRequest)
{
pAMSBlock pReply = NULL;

  // Ignore non-requests

  if (pRequest->BlockType & BT_REPLY)
    return NULL;

#if 0
  if (pRequest->NodeAddress != 0x1FF)
  {
    pReply = abiClone(pRequest, 0);
    pReply->BlockType |= 2;  //  Make it a reply
    pReply->Error = BLOCK_S_ERROR;
  }
  else
#endif
  {
    switch (pRequest->DataType)
    {
      case EGSE_DT_DDRS:
        pReply = abiClone(pRequest, sizeof (DDRStats));
        memcpy(pReply->DataStart, &DDRStats, pReply->DataCount);
        pReply->BlockType |= 2;  //  Make it a reply
        pReply->Error = BLOCK_S_END;
        break;

      default:
        abiPrint("BadLocalReq", pRequest, 24);
        pReply = abiClone(pRequest, 0);
        pReply->BlockType |= 2;  //  Make it a reply
        pReply->Error = BLOCK_S_ERROR;
        break;
    }
  }

  return pReply;
}
