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

//  fifo.h   FIFO related includes

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Moved into aal software tree
//    May 2009 by Peter Dennett at CERN
//      - Added mutex support
//      - Added count of entries

#ifndef _FIFO_H
#define _FIFO_H

#include "sspdefs.h"
#include <pthread.h>

typedef struct _FIFO
{
  int32 Insert;
  int32 Remove;
  int32 Max;
  int32 Count;
  pthread_mutex_t Mutex;
} FIFO, *pFIFO;

bool fifoInitialize(pFIFO Fifo, int32 Entries);
bool fifoPut(pFIFO Fifo, int32 *Entry);
bool fifoPutPeek(pFIFO Fifo);
bool fifoGet(pFIFO Fifo, int32 *Entry);
bool fifoPeek(pFIFO Fifo, int32 *Entry);
bool fifoLock(pFIFO Fifo);
bool fifoUnlock(pFIFO Fifo);

#endif // _FIFO_H
