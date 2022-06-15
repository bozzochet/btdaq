/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
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

//  abiConstruct.c   AMS Block construction and destruction routines

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Lake Howard Hotel, Taiwan
//        based on MDCSoftware coding

#include "aallib.h"
#include "abi.h"

static char TaskName[] = "abi";

#define abiDebug 1

////////////////////////////////////////////////////////////////////////////
//-Extern: abiConstruct
pAMSBlock abiConstruct(int32 NodeAddress, int32 DataType, int32 DataCount, 
                       int32 BlockType)
//-Synopsis:    Allocate memory for and construct and AMSBlock
//-Returns:     Pointer to the created AMSBlock.  Can be NULL in the
//-             event memory is exhausted.
//-ARG_I:       NodeAddress     NodeAddress for the AMSBlock
//-ARG_I:       DataType        AMSBlock DataType
//-ARG_I:       DataCount       Number of bytes to allocate for the contents
//-ARG_I:       BlockType       BT_REQUEST, BT_REPLY, BT_WRITEREAD, BT_WRITE
//-Description: Allocate memory for and construct and AMSBlock.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pAMSBlock Block;
int32 len;

  len = sizeof(struct _AMSBlock) + DataCount;
  if (len & 1) len++;

  Block = (pAMSBlock) aalMalloc(len);
  if ( ! Block)
  {
#ifdef abiDebug
    if (abiDebug > 0)
      printf("%s: Out of memory: Block size of %ld\n", TaskName, len);
#endif

    return NULL;
  }

  memset((char *)Block, 0, sizeof(*Block));

  Block->NodeAddress = NodeAddress;
  Block->BufferSize = DataCount;
  Block->DataType = DataType;
  Block->DataCount = DataCount;
  Block->BlockType = BlockType & 3;
  Block->Contents = (void *)&Block->DataStart[0];

  return Block;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiDestruct
void abiDestruct(pAMSBlock Block)
//-Synopsis:    Release all resources for an AMSBlock
//-Returns:     N/A
//-ARG_I:       Block           Pointer to AMSBlock to release
//-Description: Release all resource for an AMSBlock
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  if (Block->Status & abiStatusFreeData)
    aalFree((char *)abiDataPtr8(Block));

  aalFree((char *)Block);

  return;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiClone
pAMSBlock abiClone(pAMSBlock Block, int32 DataCount)
//-Synopsis:    Construct a clone of an AMSBlock
//-Returns:     Pointer to the created AMSBlock.  Can be NULL in the
//-             event memory is exhausted.
//-ARG_I:       Block           AMSBlock to clone
//-ARG_I:       DataCount       Number of bytes to allocate for the contents
//-Description: Allocate memory for and clone an AMSBlock.  This propagates
//-             attachments.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pAMSBlock Clone;
int32 len;

  len = sizeof(struct _AMSBlock) + DataCount;
  if (DataCount & 1) len++;

  Clone = (pAMSBlock) aalMalloc(len);
  if ( ! Clone)
  {
#ifdef abiDebug
    if (abiDebug > 0)
      printf("%s: Out of memory: Block size of %ld\n", TaskName, len);
#endif

    return NULL;
  }

  *Clone = *Block;

  Clone->BufferSize = DataCount;
  Clone->DataCount = DataCount;
  Clone->Contents = (void *)&Clone->DataStart[0];

  return Clone;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: abiDuplicate
pAMSBlock abiDuplicate(pAMSBlock pBlock)
//-Synopsis:    Construct a duplicate of an AMSBlock
//-Returns:     Pointer to the created AMSBlock.  Can be NULL in the
//-             event memory is exhausted.
//-ARG_I:       Block           AMSBlock to duplicate
//-Description: Allocate memory for and duplicate an AMSBlock, including
//-             copying the memory.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pAMSBlock pClone;

  pClone = abiClone(pBlock, pBlock->DataCount);
  if ( ! pClone)
  {
#ifdef abiDebug
    if (abiDebug > 0)
      printf("%s: Out of memory: Block size of %ld\n", TaskName, pBlock->DataCount);
#endif

    return NULL;
  }

  memcpy(pClone->DataStart, pBlock->Contents, pClone->DataCount);

  return pClone;
}

