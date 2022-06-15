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

#ifndef _AALALLOC_H
#define _AALALLOC_H

//  aalAlloc.h   Wedge to support transportable memory allocation (malloc/free)

//  Revision History
//    Aug 2004 by Peter Dennett
//      - Initial version 

typedef struct _aalPDStats
{
  int32 Allocated;
  int32 Freed;
} aalPDStats;

extern aalPDStats aalPDData;

#ifndef OS_ECOS
//# include <malloc.h>
#include <stdlib.h>

#   ifdef MEMWATCH
#     define aalMalloc malloc
#     define aalFree free
#     define aalReAlloc realloc
#   else
static __inline char * aalMalloc(int size)
{
  char *p = (char *)malloc(size);
  if (p)
    aalPDData.Allocated++;

  //  memset(p, 0xaa, size);

  return p;
}

static __inline void aalFree(char *ptr)
{
  free(ptr);
  aalPDData.Freed++;
  return;
}

static __inline char * aalReAlloc(char *pin, int size)
{
  char *p = (char *)realloc(pin, size);

  return p;
}
#endif // MEMWATCH
#endif // ! OS_ECOS

#ifdef OS_ECOS

static __inline char * aalMalloc(int size)
{
  char *p = malloc(size);
  if (p)
    aalPDData.Allocated++;

  //  memset(p, 0xaa, size);

  return p;
}

static __inline void aalFree(char *ptr)
{
  free(ptr);
  aalPDData.Freed++;
  return;
}

static __inline char * aalReAlloc(char *pin, int size)
{
  char *p = realloc(pin, size);

  return p;
}
#endif

#endif // _AALALLOC_H
