/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
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

#ifndef _CHD_H
#define _CHD_H

//  chd.h   Display CHD data

//  Revision History
//    Dec 2003 by Alexei Lebedev
//      - Initial version
//    Jul 2009 by Peter Dennett
//      - Restructured

typedef struct _chds
{
  int8 CHD[10];
  int8 MuxPage[4*64];
} CHDS, *pCHDS;

typedef struct _chdbf
{
  unsigned HeartBeat:8;
  //-
  unsigned CmdCount:8;
  //-
  unsigned PowerStep:3;
  unsigned Alert:1;
  unsigned Vehicle:1;
  unsigned Software:1;
  unsigned JMDC:2;
  //-
  unsigned MCTowner:2;
  unsigned MOset:1;
  unsigned QActive:1;
  unsigned HRDLowner:2;
  unsigned HOset:1;
  unsigned HRDLlight:1;
  //-
  unsigned DAQowner:2;
  unsigned DOset:1;
  unsigned DAQactive:1;
  unsigned LRDLowner:2;
  unsigned LOset:1;
  unsigned LRDLactive:1;
  //-
  unsigned TimeMux:6;
  unsigned DataValid:1;
  unsigned DataInit:1;
  //-
  int8 MuxBytes[4];

  union _mux
  {
    int8 MuxBytes[4];
    struct _LRDLstatus   // ID 1
    {
      unsigned RxErrCnt:5;
      unsigned STS28:1;
      unsigned STS4:1;
      unsigned ISSRT:1;
      //-
      unsigned TxFrmCnt:8;
      unsigned RxFrmCnt:8;
      unsigned NASAcmdCnt:6;
      unsigned STSbus:1;
      unsigned ISSbus:1;
    } LRDLstatus;
    struct _HRDLstatus   // ID 2
    {
      unsigned TxFrmCnt:8;
      unsigned RxFrmCnt:8;
      unsigned JBUpercent:8;
      unsigned ErrorCount:6;
      unsigned Enabled:1;
      unsigned Interface:1;
    } HRDLstatus;
  } MuxPage[64];
} CHDbf, *pCHDbf;

void displayCHD(pCHDS pCHD);
void printfCHD(pCHDS pCHD);
void display_CHD_aal(int16 Status, int8 *Frame);

#endif // _CHD_H
