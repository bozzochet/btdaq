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

//  ringbuffer.c   Ring buffer 

//  Revision History
//    Feb 2014 by Peter Dennett at JSC POCC
//      -  Baseline development

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ringbuffer.h"

#define _RINGBUFFERDEBUG 1
#define  ERROUT stdout

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
// 
pRingBuffer RingBufferCreate(int len)
{
  pRingBuffer pR = calloc(1, sizeof(RingBuffer));
  if ( ! pR)
    return NULL;

  pR->length  = len;
  pR->start = 0;
  pR->end = 0;
  pR->buffer = calloc(pR->length, 1);

  if (! pR->buffer)
  {
    free (pR);
    return NULL;
  }

#if _RINGBUFFERDEBUG > 0
  *(pR->buffer+pR->length) = 0xAA;
#endif
  return pR;
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
void RingBufferDestroy(pRingBuffer pR)
{
  if (pR)
  {
    free(pR->buffer);
    free(pR);
  }
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
int RingBufferWrite(pRingBuffer pR, char *data, int len)
{
  unsigned int len2, len3;

#if _RINGBUFFERDEBUG > 2
  printf("RingBufferWrite len %d start %d end %d\n", len, pR->start, pR->end);
#endif

  // Wrap if it helps

  if ((pR->start != 0) && (pR->end >= pR->length))
    pR->end = 0;

  if (pR->start == pR->end)
  {
    pR->start = 0;
    pR->end = 0;
    len2 = (pR->length < len) ? pR->length : len;
#if _RINGBUFFERDEBUG > 1
    printf("move Write 1 %d '%0.*s'\n", len2, len2, data);
#endif
    memcpy(pR->buffer + pR->end, data, len2);
#if _RINGBUFFERDEBUG > 0
    if (*(pR->buffer+pR->length) != 0xAA)
      fprintf(ERROUT, "RingBufferWrite: past buffer 1 start %d end %d len2 %d\n", 
              pR->start, pR->end, len2);
#endif    
    pR->end = len2;
    return len2;
  }
  else if (pR->start < pR->end)  // Not wrapped
  {
    len2 = pR->length - pR->end;
    if (len2 > len)
      len2 = len;
#if _RINGBUFFERDEBUG > 1
    printf("move write 2 %d '%0.*s'\n", len2, len2, data);
#endif
    memcpy(pR->buffer + pR->end, data, len2);
#if _RINGBUFFERDEBUG > 0
      if (*(pR->buffer+pR->length) != 0xAA)
        fprintf(ERROUT, "RingBufferWrite: past buffer 2 start %d end %d\n",
                pR->start, pR->end);
#endif
    pR->end = pR->end + len2;
    data += len2;
    len3 = 0;
    if ((len > len2) && (pR->start != 0))
    {
      pR->end = 0;
      len3 = pR->start - 1;
      if (len < (len2 + len3))
        len3 = len - len2;
#if _RINGBUFFERDEBUG > 1
      printf("move write 3 %d '%0.*s'\n", len3, len3, data);
#endif
      memcpy(pR->buffer + pR->end, data, len3);
#if _RINGBUFFERDEBUG > 0
      if (*(pR->buffer+pR->length) != 0xAA)
        fprintf(ERROUT, "RingBufferWrite: past buffer 3 start %d end %d\n",
                pR->start, pR->end);
#endif    
      pR->end = len3;
    }
    return len2 + len3;
  }
  else  // Wrapped
  {
    len2 = pR->start - pR->end - 1;
    if (len2 > len)
      len2 = len;
#if _RINGBUFFERDEBUG > 1
    printf("move write 4 %d '%0.*s'\n", len2, len2, data);
#endif
    memcpy(pR->buffer + pR->end, data, len2);
#if _RINGBUFFERDEBUG > 0
      if (*(pR->buffer+pR->length) != 0xAA)
        fprintf(ERROUT, "RingBufferWrite: past buffer 4 start %d end %d\n",
                pR->start, pR->end);
#endif    
    pR->end += len2;
    if (pR->end >= pR->length)
      pR->end = 0;
    return len2;
  }
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
int RingBufferWritePeek(pRingBuffer pR)
{
  // Wrap if it helps

  if ((pR->start != 0) && (pR->end >= pR->length))
    pR->end = 0;

  if (pR->start == pR->end)      // Empty
    return pR->length;
  else if (pR->start < pR->end)  // Not wrapped
    return (pR->length - pR->end) + (pR->start - 1);
  else                           // Wrapped
    return (pR->start - pR->end - 1);
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
int RingBufferEmpty(pRingBuffer pR)
{
  // Wrap if it helps

  if ((pR->start != 0) && (pR->end >= pR->length))
    pR->end = 0;

  return (pR->start == pR->end);      // Empty
}

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
int RingBufferRead(pRingBuffer pR, char *data, int len)
{
  int len2, len3;
  if (len < 0)
  {
#if _RINGBUFFERDEBUG > 0
    fprintf(ERROUT, "RingBufferRead: negative length %d\n", len);
#endif    
    return 0;
  }

#if _RINGBUFFERDEBUG > 0
      if (*(pR->buffer+pR->length) != 0xAA)
        fprintf(ERROUT, "RingBufferRead: Buffer was over written 0x%02x length %d\n",
                *(pR->buffer+pR->length), pR->length);
#endif

  if (pR->end == pR->start)
    return 0;

  if (pR->end > pR->start)  // Not wrapped
  {
#if _RINGBUFFERDEBUG > 2
    fprintf(ERROUT, "RingBufferRead: Not wrapped end %d start %d length %d\n",
            pR->end, pR->start, pR->end-pR->start+1);
#endif
    len2 = pR->end - pR->start;
    if (len2 > len)
      len2 = len;
#if _RINGBUFFERDEBUG > 1
      printf("move read  1 %d '%0.*s'\n", len2, len2, pR->buffer + pR->start);
#endif
    memcpy(data, pR->buffer + pR->start, len2);
    pR->start += len2;
    if (pR->start >= pR->length)
      pR->start = 0;

    return len2;
  }
  else
  {
#if _RINGBUFFERDEBUG > 2
    fprintf(ERROUT, "RingBufferRead: Wrapped end %d start %d length %d\n",
            pR->end, pR->start, pR->length - pR->start + pR->end);
#endif
    len2 = pR->length - pR->start; //???? + 1;
    if (len2 > len)
      len2 = len;
#if _RINGBUFFERDEBUG > 1
      printf("move read 2  %d '%0.*s'\n", len2, len2, pR->buffer + pR->start);
#endif
    memcpy(data, pR->buffer + pR->start, len2);
    pR->start += len2;
    if (pR->start >= pR->length)
      pR->start = 0;

    data += len2;
    len3 = len - len2;
    if (len3 <= 0)
      return len2;
    if (len3 >= pR->end)
      len3 = pR->end-1;
    memcpy(data, pR->buffer + pR->start, len3);
    pR->start = len3;
    if (pR->start >= pR->length)
      pR->start = 0;

    return len2 + len3;
  }
}

#ifdef RINGBUFFER_UNIT_TEST

#include <stdlib.h>

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
//
int main(int argc, char *argv[])
{
  pRingBuffer pR;

  char input[120], *inp;
  char output[23];
  int i, cnt, len, rc;
  int line_count = 100;

  for (i=0; i<argc; i++)
    printf("argv[%d] %s\n", i, argv[i]);

  if (argc > 1)
    line_count = atol(argv[1]);

  printf("line_count %d\n", line_count);

  pR = RingBufferCreate(2570);
  len = 0;

  i = 0;
  cnt = 0;

  while(1)
  {
  next:
    if ( (! len) && (i<line_count))
    {
      len = snprintf(input, sizeof input, "%4.4d~012345678901234567890123456789"
                     "012345678901234567890123456789012345678901234567890123456789"
                     "0123456789-%4.4d\n", i, i);
      inp = input;
      cnt = 0;
      i++;
    }

    while ((len-cnt) && (rc = RingBufferWrite(pR, input+cnt, len-cnt)) > 0)
    {
      cnt += rc;

      //printf("Peek %d\n", RingBufferWritePeek(pR));

      if (rc < 0)
      {
        printf("what? rc %d cnt %d len %d\n", rc, cnt, len);
        len = 0;
        continue;
      }

      if (cnt == len)
      {
        len = 0;
        cnt = 0;

        if (i >= line_count)
          goto done;
        else
          goto next;
      }
      else if (cnt > len)
      {
        printf("\nwhat? cnt %d len %d\n", cnt, len);
        len = 0;
        cnt = 0;
      }
    }

    rc = RingBufferRead(pR, output, sizeof output);
    if (rc < 0)
    {
      printf("\nwhat? rc %d\n", rc);
      continue;
    }
    else if (rc > 0)
    {
      printf("%.*s", rc, output);
      memset(output, '!', sizeof output);
    }
  }

 done:

  //printf("\nFlush\n");

  while ((rc = RingBufferRead(pR, output, sizeof output)) > 0)
    printf("%.*s", rc, output);

  printf("\nDone - rc %d\n", rc);

  return 0;
}
#endif // RINGBUFFER_UNIT_TEST
