/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2003 by PADSOFT, Inc.                        */
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

//  fifo.c   FIFO support routines

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Moved into aal software tree
//    May 2009 by Peter Dennett at CERN
//      - Added mutex support

#include "fifo.h"
#include "logfile.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoInitialize
bool fifoInitialize(pFIFO Fifo, int32 Entries)
//-Synopsis:    Initialize a FIFO structure
//-Returns:     TRUE, there are no errors possible
//-ARG_I:       Fifo            The FIFO structure to be initialized
//-Description: Initialize a FIFO structure
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int rc;

  memset((char *) Fifo, 0, sizeof *Fifo);
  Fifo->Max = Entries;
  rc = pthread_mutex_init(&(Fifo->Mutex), NULL);

  if (rc != 0)
    LogError("fifo: Failed to initialize MUTEX %d\n", rc);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoPut
bool fifoPut(pFIFO Fifo, int32 *Entry)
//-Synopsis:    Get the index into which to put the next FIFO entry.
//-Returns:     TRUE, there is room in the FIFO, else its full
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-ARG_U:       Entry           The entry index to put the new entry into
//-Description: Get the index into which to put the next FIFO entry.
//-
//-             The FIFO structure is updated to reflect the newly added
//-             entry.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int ins = Fifo->Insert + 1;

  *Entry = Fifo->Insert;

  if (ins >= Fifo->Max)
    ins = 0;
  if (ins == Fifo->Remove)
    return FALSE;

  Fifo->Insert = ins;
  Fifo->Count++;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoPutPeek
bool fifoPutPeek(pFIFO Fifo)
//-Synopsis:    Get the index into which to put the next FIFO entry.
//-Returns:     TRUE, there is room in the FIFO, else its full
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-ARG_U:       Entry           The entry index to put the new entry into
//-Description: Get the index into which to put the next FIFO entry.
//-
//-             The FIFO structure is updated to reflect the newly added
//-             entry.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int ins = Fifo->Insert + 1;

  if (ins >= Fifo->Max)
    ins = 0;
  if (ins == Fifo->Remove)
    return FALSE;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoGet
bool fifoGet(pFIFO Fifo, int32 *Entry)
//-Synopsis:    Get the index from which to get (remove) the next FIFO entry.
//-Returns:     TRUE, there is an entry in the FIFO, else its empty
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-ARG_U:       Entry           The entry index from which to get the entry from
//-Description: Get the index from which to get (remove) the next FIFO entry.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  if (Fifo->Insert != Fifo->Remove)
  {
    *Entry = Fifo->Remove;
    if (++(Fifo->Remove) >= Fifo->Max) 
      Fifo->Remove = 0;

    Fifo->Count--;
    return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoPeek
bool fifoPeek(pFIFO Fifo, int32 *Entry)
//-Synopsis:    Check to see if a FIFO has entries to get.
//-Returns:     TRUE, there is an entry in the FIFO, else its empty
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-ARG_U:       Entry           The entry index from which to get the entry from
//-Description: Check to see if a FIFO has entries to get.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  if (Fifo->Insert != Fifo->Remove)
  {
    if (Entry) 
      *Entry = Fifo->Remove;
    return TRUE;
  }

  return FALSE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoLock
bool fifoLock(pFIFO Fifo)
//-Synopsis:    Lock MUTEX for FIFO
//-Returns:     TRUE
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-Description: Lock MUTEX for FIFO.  Caller is reponsible for all 
//-             synchronization, including that of gets and puts.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  pthread_mutex_lock(&(Fifo->Mutex));
  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: fifoUnlock
bool fifoUnlock(pFIFO Fifo)
//-Synopsis:    Unlock MUTEX for FIFO
//-Returns:     TRUE
//-ARG_I:       Fifo            The FIFO structure to be manipulated
//-Description: Unlock MUTEX for FIFO.  Caller is reponsible for all 
//-             synchronization, including that of gets and puts.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
  pthread_mutex_unlock(&(Fifo->Mutex));
  return TRUE;
}
