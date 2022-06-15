/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                     Copyright 2007 by PADSOFT, Inc.                       */
/*                                                                           */
/*      Developed for use on ELC, ExPRESS Logistics Carrier for the          */
/*                     International Space Station                           */
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

//  sspFifo.h   FIFO related includes

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Moved into aal software tree
//    Apr 2007 by Peter Dennett
//      - Re-cycled into ELC

#ifndef _SSPFIFO_H
#define _SSPFIFO_H

typedef struct _FIFO
{
  int32 Insert;
  int32 Remove;
  int32 Max;
} FIFO, *pFIFO;

static __inline bool fifoInitialize(pFIFO Fifo, int32 Entries)
{
  memset((char *) Fifo, 0, sizeof *Fifo);
  Fifo->Max = Entries;

  return TRUE;
}

static __inline bool fifoPut(pFIFO Fifo, int32 *Entry)
{
int ins = Fifo->Insert + 1;

  *Entry = Fifo->Insert;

  if (ins >= Fifo->Max)
    ins = 0;
  if (ins == Fifo->Remove)
    return FALSE;

  Fifo->Insert = ins;

  return TRUE;
}

static __inline bool fifoGet(pFIFO Fifo, int32 *Entry)
{
  if (Fifo->Insert != Fifo->Remove)
  {
    *Entry = Fifo->Remove;
    if (++(Fifo->Remove) >= Fifo->Max) 
      Fifo->Remove = 0;

    return TRUE;
  }

  return FALSE;
}

static __inline bool fifoPeek(pFIFO Fifo, int32 *Entry)
{
  if (Fifo->Insert != Fifo->Remove)
  {
    *Entry = Fifo->Remove;
    return TRUE;
  }

  return FALSE;
}

#endif // _SSPFIFO_H
