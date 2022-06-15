/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2017 by PADSOFT, Inc.                        */
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
/*                          +1 713 899 6100                                  */
/*                       pdennett@padsoft.com                                */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  abiDataSeries.h   Routines to extract data from block files by time

#ifndef _abiDataSeries_H
#define _abiDataSeries_H

//  Revision History
//    Mar 2017 by Peter Dennett
//      - At Business Integra

#include "sspdefs.h"
#include "abi.h"

#include <malloc.h>

#define ABS(a) (( (a) < 0 ) ? -(a) : (a))

#ifndef MIN
# define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
#endif

#if 0 //????????????????????????????????????????????????
typedef struct _DataSeries
{
  int32 time;
  float value;
} DataSeries;

typedef struct _DataWindow
{
  int32 StartTime;
  int32 EndTime;
} DataWindow;
#endif //????????????????????????????????????????????????

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct _abiFileIndexStruct
{
  int32 FileId;
  int32 NewestTime;
  int32 OldestTime;
  int32 AverageTime;
  int32 BlocksInFile;
} abiFileIndex, *pabiFileIndex;

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

typedef struct _abiFileSeriesStruct
{
  int FileIndexAllocated;
  int FileIndexDimension;
  pabiFileIndex pFileIndex;
  unsigned long long FileIndexTimeSum;
  int LastFile;
  int FirstFile;
  int FileNumber;
  char *Path;
  FILE *fIn;
  time_t TimeWindowStart;
  time_t TimeWindowEnd;
} abiFileSeries, *pabiFileSeries;


//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

pabiFileSeries abiDataSeriesOpen(const char *Path);
void abiDataSeriesClose(pabiFileSeries pabiSeries);

#endif // _abiDataSeries_H
