/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2014 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
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

//  ringbuffer.h   Ring buffer 

//  Revision History
//    Feb 2014 by Peter Dennett at JSC POCC
//      -  Baseline development

#ifndef _RINGBUFFER_H
#define _RINGBUFFER_H

#include <string.h>

typedef struct 
{
  unsigned char *buffer;
  unsigned int length;
  unsigned int start;
  unsigned int end;
} RingBuffer, *pRingBuffer;

pRingBuffer RingBufferCreate(int length);
void RingBufferDestroy(pRingBuffer pR);
int RingBufferRead(pRingBuffer pR, char *target, int amount);
int RingBufferWrite(pRingBuffer pR, char *data, int length);
int RingBufferWritePeek(pRingBuffer pR);
int RingBufferEmpty(pRingBuffer pR);

#endif // _RINGBUFFER_H
