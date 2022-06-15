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

//  frmList.c   AMS Frame list handling

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Kemah office 

#include "aallib.h"
#include "frm.h"
 
////////////////////////////////////////////////////////////////////////////
//-Extern: frmListHeadConstruct
bool frmListHeadConstruct(pfrmListHead pFLH, char *TaskName)
//-Synopsis:    Initialize the list head for a list of frames.
//-Returns:     TRUE if success, else FALSE
//-ARG_I:       pFLH       Memory area to build list head in.
//-ARG_I:       TaskName   Pointer to a static text string for error message
//-                        identification.
//-Description: Initialize the list head for a list of frames.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  memset((char *) pFLH, 0, sizeof *pFLH);

  pFLH->Next = NULL;
  pFLH->Block = NULL;
  pFLH->Deblock = NULL;
  pFLH->TaskName = TaskName;
  pFLH->Debug = 0;
  aalMutexInit(&pFLH->BufferMutex, NULL);

#ifdef  ASSERT_FRMLIST
  pFLH->Marker = frmListHeadMarker;
  ASSERT_frmListHeader(pFLH);
#endif

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListHeadDestruct
bool frmListHeadDestruct(pfrmListHead pFLH)
//-Synopsis:    Destruct the list head for a list of frames.
//-Returns:     TRUE if success, else FALSE
//-ARG_I:       pFLH       The list head to operate on
//-Description: Destruct the list head for a list of frames.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  ASSERT_frmListHeader(pFLH);

  frmListEmpty(pFLH);

  if (pFLH->Block)
    frmListEntDestruct(pFLH, pFLH->Block);

  if (pFLH->Deblock)
    frmListEntDestruct(pFLH, pFLH->Deblock);

  if (pFLH->PDEntConstructed != pFLH->PDEntDestructed)
    printf("%s: PDEntConstructed (%ld) != PDEntDestructed (%ld)\n", pFLH->TaskName,
           pFLH->PDEntConstructed, pFLH->PDEntDestructed);

#ifdef  ASSERT_FRMLIST
  pFLH->Marker = 0xBBBB;
#endif

  aalMutexDestroy(&pFLH->BufferMutex);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListEntConstruct
pfrmListEnt frmListEntConstruct(pfrmListHead pFLH, int FrameSize)
//-Synopsis:    Initialize a list entry.
//-Returns:     Pointer to aalAlloc'd list entry, or NULL if out of memory.
//-ARG_I:       pFLH       The list head to operate on.
//-ARG_I:       FrameSize  Number of bytes for frame.
//-Description: Initialize a list entry.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pfrmListEnt pFLE;

  ASSERT_frmListHeader(pFLH);

  if (FrameSize > sizeof(pFLE->Buffer))
    Panic("%s: FrameSize too large %d", "frmListEntConstruct", FrameSize);

  pFLE = (pfrmListEnt) aalMalloc(sizeof *pFLE);
  if ( ! pFLE)
    Panic("%s: Out of memory for frame list", pFLH->TaskName);
  else
  {
    pFLE->FrameSize = FrameSize;
    pFLE->Next = NULL;

#ifdef  ASSERT_FRMLIST
    pFLE->Marker = frmListEntMarker;
    ASSERT_frmListEnt(pFLE);
#endif

    pFLH->PDEntConstructed++;
  }

  return pFLE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListEntDestruct
bool frmListEntDestruct(pfrmListHead pFLH, pfrmListEnt pFLE)
//-Synopsis:    Destruct a list entry.
//-Returns:     TRUE if success, else FALSE
//-ARG_I:       pFLH       The list head to operate on.
//-ARG_I:       pFLE       The list entry to operate on
//-Description: Destruct a list entry.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  ASSERT_frmListHeader(pFLH);
  ASSERT_frmListEnt(pFLE);

#ifdef  ASSERT_FRMLIST
  pFLE->Marker = 0xAAAA;
#endif

  pFLH->PDEntDestructed++;

  aalFree((char *) pFLE);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListAdd
bool frmListAdd(pfrmListHead pFLH, pfrmListEnt pFLE)
//-Synopsis:    Add an entry to a list
//-Returns:     TRUE if success, else FALSE
//-ARG_I:       pFLH       The list head to operate on.
//-ARG_I:       pFLE       The list entry to operate on
//-Description: Add an entry to a list
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pfrmListEnt pEnt;

  ASSERT_frmListHeader(pFLH);
  ASSERT_frmListEnt(pFLE);

  pFLE->Next = 0;

  aalMutexLock(&pFLH->BufferMutex);

  if (pFLH->Next == NULL)
  {
    pFLH->Next = pFLE;
  }
  else
  {
    for (pEnt=pFLH->Next; pEnt->Next; ) 
      pEnt = pEnt->Next;
    pEnt->Next = pFLE;
  }

  aalMutexUnlock(&pFLH->BufferMutex);

  pFLH->PDFramesAdded++;
  pFLH->PDFramesInList++;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListRemove
pfrmListEnt frmListRemove(pfrmListHead pFLH)
//-Synopsis:    Remove an entry from a list
//-Returns:     Pointer to the entry removed, or NULL if none available.
//-ARG_I:       pFLH       The list head to operate on.
//-Description: Remove an entry from a list
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
pfrmListEnt pEnt;

  ASSERT_frmListHeader(pFLH);

  aalMutexLock(&pFLH->BufferMutex);

  pEnt = pFLH->Next;
  if (pEnt)
    pFLH->Next = pEnt->Next;
  else
    pFLH->Next = NULL;

  aalMutexUnlock(&pFLH->BufferMutex);

  if (pEnt)
  {
    ASSERT_frmListEnt(pEnt);
    pFLH->PDFramesRemoved++;
    pFLH->PDFramesInList--;
  }

  return pEnt;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListEmpty
bool frmListEmpty(pfrmListHead pFLH)
//-Synopsis:    Remove all entries from a list
//-Returns:     TRUE
//-ARG_I:       pFLH       The list head to operate on.
//-Description: Remove all entries from a list
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  ASSERT_frmListHeader(pFLH);

  while(pFLH->Next)
    frmListEntDestruct(pFLH, frmListRemove(pFLH));

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListBlock
bool frmListBlock(pfrmListHead pFLH, pFRAME pFrm, pAMSBlock pRequest)
//-Synopsis:    Frame an AMSBlock to a list
//-Returns:     TRUE if successfull, else FALSE
//-ARG_I:       pFLH       The list head to operate on.
//-Description: Frame an AMSBlock to a list
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  ASSERT_frmListHeader(pFLH);

  if ( ! pFLH->Block)
    pFLH->Block = frmListEntConstruct(pFLH, pFrm->FrameSize);

  if ( ! pFLH->Block)
    return Panic("%s: out of memory for frames", pFLH->TaskName);

  if ((pFLH->Debug > 2) && (pRequest))
    abiPrint2(pFLH->TaskName, "TxFrm", pRequest, 24);

  while ( ! frmBlock(pFrm, &(pFLH->Block->Buffer[0]), pRequest))
  {
    if (pFLH->Debug > 1)
      printf("%s: frmBlock emitting frame\n", pFLH->TaskName);

    frmListAdd(pFLH, pFLH->Block);
    pFLH->Block = frmListEntConstruct(pFLH, pFrm->FrameSize);

    if ( ! pFLH->Block)
      return Panic("%s: out of memory for frames", pFLH->TaskName);
  }

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: frmListDeblock
bool frmListDeblock(pfrmListHead pFLH, pFRAME pFrm, pAMSBlock *pRequest)
//-Synopsis:    Deframe AMSBlocks from a list
//-Returns:     TRUE if successfull, else FALSE
//-ARG_I:       pFLH       The list head to operate on.
//-Description: Deframe AMSBlocks from a list
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  ASSERT_frmListHeader(pFLH);

  if ( ! pFLH->Deblock)
    pFLH->Deblock = frmListRemove(pFLH);

  while(pFLH->Deblock)
  {
    if (pFrm->FrameSize != pFLH->Deblock->FrameSize)
    {
      printf("%s: frmListDeblock - Warning frame size problems\n", pFLH->TaskName);
      frmListEntDestruct(pFLH, pFLH->Deblock);
      pFLH->Deblock = NULL;
      return FALSE;
    }

    while (frmDeblock(pFrm, &(pFLH->Deblock->Buffer[0]), pRequest))
    {
      if (pFLH->Debug > 2)
        abiPrint2(pFLH->TaskName, "RxFrm", *pRequest, 24);
      return TRUE;
    }

    frmListEntDestruct(pFLH, pFLH->Deblock);
    pFLH->Deblock = frmListRemove(pFLH);
  }

  return FALSE;
}

