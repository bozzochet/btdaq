/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2011 by PADSOFT, Inc.                        */
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

//  abiEnvelope2.c   AMS Block envelope construction and processing
//                   Uses AMS-02 compatible envelope packing

//  Revision History
//    Feb 2011 by Peter Dennett
//      - Initial version at Cocoa Beach, Holiday Inn Express

#include "aallib.h"
#include "abi.h"

#ifdef NOT_LUA
#include "logfile.h"
#endif

////////////////////////////////////////////////////////////////////////////
//-Extern: abiEnvelope2
pAMSBlock abiEnvelope2(bool shf, int BlockType, int NodeAddress, int DataType,
                       int n, pAMSBlock pBlocks[])
//-Synopsis:    Create an AMSBlock envelope containing the AMSBlocks passed in
//-Returns:     Pointer to the created AMSBlock.  Can be NULL in the
//-             case memory is exhausted.
//-ARG_I:       shf             Secondary header flag for the created envelope
//-                             block.
//-ARG_I:       BlockType       BlockType (R/W Request/Reply) for the created
//-                             envelope block.
//-ARG_I:       NodeAddress     NodeAddress for the created
//-                             envelope block.
//-ARG_I:       DataType        DataType for the created
//-                             envelope block.
//-ARG_I:       n               Number of AMSBlocks in arrary
//-ARG_I:       pBlocks         Array of AMSBlocks
//-Description: Allocate memory for and construct an AMSBlock envelope.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       Not sure of the format AAL uses.  For now all envelop code
//-             should be contained in this file.
//-Bugs:
//-Revisions:
//-End:
{
pAMSBlock pBlock;
int i;
int32 len, blen;
int8 *ptr, *pend;

//printf("abiEnvelope2: shf %d BlockType %d NodeAddress %x DataType %x n %d\n",
//         shf, BlockType, NodeAddress, DataType, n);

//  Determine the size of the envelope

  for (i=0, blen = 0; i<n; i++)
  {
    if (pBlocks[i])
    {
      len = abiBlockSize(pBlocks[i], shf);
      if ( len > 0x7FFF ) len += 4;
      else                len += 2;
      blen += (len + (len & 1));
    }
  }

  //printf("abiEnvelope2: Size %d\n", blen);

//  Allocate the envelope

  pBlock = abiConstruct(NodeAddress, DataType, blen, BlockType);
  if ( ! pBlock)
    return NULL;

//  Build the envelope contents

  ptr  = abiDataPtr8(pBlock);
  pend = ptr + blen;

  for (i=0; i<n; i++)
  {
    if (pBlocks[i])
    {
      len = abiEncode(pBlocks[i], ptr, blen, shf, FALSE);      // no Swap
      if ( len < 0 ) break;     // Error
      len  += (len & 1);
      ptr  += len;
      blen -= len;
    }
  }

  if (ptr > pend)
#ifdef NOT_LUA
    LogError("abiEnvelope2: Error in length of envelope\n");
#else
    printf("abiEnvelope2: Error in length of envelope\n");
#endif

  return pBlock;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiProcessEnvelope2
int abiProcessEnvelope2(abicbRoutine *callback, void *caller_data,
                        pAMSBlock pBlock, bool shf)
//-Synopsis:    Extract AMSBlocks from envelope and callback routine to process them.
//-Returns:     Count of AMSBlocks processed successfully or -1 on error
//-ARG_I:       callback       Routine to call for each AMSBlock extracted.
//-ARG_I:       caller_data    Data passed to the callback routine
//-ARG_I:       pBlock         AMSBlock with the data
//-ARG_I:       shf            Secondary header flag
//-Description: Extract AMSBlocks from envelope and callback routine to process them.
//-
//-             The callback routine is responsible for destructs AMSBlocks it is called with
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int8 *ptr, *pend;
int32 len;
pAMSBlock pBlk;
int n=0;
int normal;
bool IgnoreError;

  ptr  = abiDataPtr8(pBlock);
  pend = ptr + pBlock->DataCount;

  while (1)
  {
    if ( ptr == pend ) break;
    if ( ptr > pend)
    {
#ifdef NOT_LUA
      LogError("abiProcessEnvelope: Envelope boundry error\n");
#else
      printf("abiProcessEnvelope: Envelope boundry error\n");
#endif
      return -1;
    }

    // Get lenght word
    len = abiSizeWord(&ptr);
    if ( len == 0 || len > (pend - ptr) ) return FALSE;

    // Decode block data
    pBlk = abiDecode(ptr, len, shf, FALSE);     // no Swap
    if ( pBlk == NULL ) return FALSE;
    if ( pBlk->CommandTag == 0 ) pBlk->CommandTag = pBlock->CommandTag;
    // if ( shf ) pBlk->CommandTag = pBlock->CommandTag;
    ptr += (len + (len & 1));

    if ( pBlk->BlockType & BT_REPLY ) IgnoreError = TRUE;     // Always ignore error for process replies
    else                              IgnoreError = ((pBlk->Error & 0x2) != 0);
    normal = callback(caller_data, pBlk);
    if ( !normal && !IgnoreError )
      break;
    n++;
  }

  return n;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiCountEnvelope2
int abiCountEnvelope2(pAMSBlock pBlock, bool shf)
//-Synopsis:    Count the number of AMSBlocks enveloped in an AMSBlock.  Good way
//-             to tell if AMSBlock is an envelope.
//-Returns:     Count of AMSBlocks processed.  Any error results in zero returned.
//-ARG_I:       pBlock         AMSBlock with the envelope data
//-ARG_I:       shf             Secondary header flag
//-Description:
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int8 *ptr;
int8 *pend;
int n=0;
int len;

  ptr  = abiDataPtr8(pBlock);
  pend = ptr + pBlock->DataCount;

  while (ptr < (pend-4))
  {
    len  = abiSizeWord(&ptr);
    ptr += (len + (len & 1));

    if (ptr > pend)
      return 0;

    n++;
  }

  if ((pend - ptr) != 0)  //  MUST end cleanly
    n = 0;

  return n;
}

#ifdef NOT_LUA

////////////////////////////////////////////////////////////////////////////
//-Static: PrintEnvelope2CB
static bool PrintEnvelope2CB(void *callerdata, pAMSBlock pBlock)
{
  abiPrint2("    ", (char *) callerdata, pBlock, 32);
  abiDestruct(pBlock);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiPrintEnvelope2
int abiPrintEnvelope2(char *tag, pAMSBlock pBlock, bool shf)
//-Synopsis:    Print a summary of the envelope contents
//-Returns:     Count of AMSBlocks processed.  Any error results in zero returned.
//-ARG_I:       tag            Text tag to print at start of the line
//-ARG_I:       pBlock         AMSBlock with the envelope data
//-ARG_I:       shf            Secondary header flag
//-Description:
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  abiPrint(tag, pBlock, 32);
  return abiProcessEnvelope2(PrintEnvelope2CB, tag, pBlock, shf);
}

#endif
