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

//  chd.c   Critical Health Data display routine

//  Revision History
//    Jul 2009 by Peter Dennett
//      - Initial version CERN bld 39

#include <stdio.h>
#include "aallib.h"
#include "chd.h"

//~============================================================================

static int8 unpack8(int8 code, int8 mask) {
  int16 LSB = mask & (mask ^ mask << 1);
  if (LSB) return (code & mask) / LSB;
  else     return 0;
}

//~----------------------------------------------------------------------------

void displayCHD(pCHDS pCHD) 
{
int8 *Frame = (int8*) pCHD;
int i;
struct 
{
  int   mod;
  int   ind;
  int8  msk;
  char *fmt;
  char *nam;
  int   jnd;
  int8  chk;
  char *err;
} CHD[] = 
{
  { 1, 1, 0xFF, " %3d", "HB"},
  { 1, 2, 0xFF, " %3d", "CC"},

  { 1, 3, 0xC0, " %3d", "ID"},
  { 1, 3, 0x20, " %3d", "SF"},
  { 1, 3, 0x10, " %3d", "VE"},
  { 1, 3, 0x08, " %3d", "AL"},
  { 1, 3, 0x07, " %3d", "PS"},

  { 1, 4, 0x80, " %3d", "HA"},
  { 1, 4, 0x40, " %3d", "HV"},
  { 2, 4, 0x30, " %3d", "HO", 4, 0x40, "   ?"},
  { 1, 4, 0x08, " %3d", "MA"},
  { 1, 4, 0x04, " %3d", "MV"},
  { 2, 4, 0x03, " %3d", "MO", 4, 0x04, "   ?"},

  { 1, 5, 0x80, " %3d", "LA"},
  { 1, 5, 0x40, " %3d", "LV"},
  { 2, 5, 0x30, " %3d", "LO", 5, 0x40, "   ?"},
  { 1, 5, 0x08, " %3d", "DA"},
  { 1, 5, 0x04, " %3d", "DV"},
  { 2, 5, 0x03, " %3d", "DO", 5, 0x04, "   ?"},

  { 1, 6, 0x80, " %3d", "TV"},
  { 1, 6, 0x40, " %3d", "TI"},
  { 1, 6, 0x3F, " %3d", "ID"},
  
  { 1, 7, 0xFF, "  %02X", "B0"},
  { 1, 8, 0xFF, "  %02X", "B1"},
  { 1, 9, 0xFF, "  %02X", "B2"},
  { 1,10, 0xFF, "  %02X", "B3"}
};
  
  printf("\rCHD: ");

  for (i=0; i<((sizeof CHD)/(sizeof CHD[0])); i++) 
  {
    switch (CHD[i].mod) 
    {
      case 1:
        printf(CHD[i].fmt, unpack8(Frame[CHD[i].ind - 1], CHD[i].msk));
        break;

      case 2:
        if ((Frame[CHD[i].jnd - 1] & CHD[i].chk) == 0) 
          printf(CHD[i].err);
        else 
          printf(CHD[i].fmt, unpack8(Frame[CHD[i].ind - 1], CHD[i].msk));
        break;
    }
  }

  printf("\nCHD  ");

  for (i=0; i<((sizeof CHD)/(sizeof CHD[0])); i++) 
    printf(" %3s", CHD[i].nam);

  fflush(stdout);
//??printf("\r\x1b[K");
}

//~----------------------------------------------------------------------------

void printfCHD(pCHDS pCHD) 
{
int8 *Frame = (int8*) pCHD;
int i;
struct 
{
  int   mod;
  int   ind;
  int8  msk;
  char *fmt;
  char *nam;
  int   jnd;
  int8  chk;
  char *err;
} CHD[] = 
{
  { 1, 1, 0xFF, " %3d", "HB"},
  { 1, 2, 0xFF, " %3d", "CC"},

  { 1, 3, 0xC0, " %3d", "ID"},
  { 1, 3, 0x20, " %3d", "SF"},
  { 1, 3, 0x10, " %3d", "VE"},
  { 1, 3, 0x08, " %3d", "AL"},
  { 1, 3, 0x07, " %3d", "PS"},

  { 1, 4, 0x80, " %3d", "HA"},
  { 1, 4, 0x40, " %3d", "HV"},
  { 2, 4, 0x30, " %3d", "HO", 4, 0x40, "   ?"},
  { 1, 4, 0x08, " %3d", "MA"},
  { 1, 4, 0x04, " %3d", "MV"},
  { 2, 4, 0x03, " %3d", "MO", 4, 0x04, "   ?"},

  { 1, 5, 0x80, " %3d", "LA"},
  { 1, 5, 0x40, " %3d", "LV"},
  { 2, 5, 0x30, " %3d", "LO", 5, 0x40, "   ?"},
  { 1, 5, 0x08, " %3d", "DA"},
  { 1, 5, 0x04, " %3d", "DV"},
  { 2, 5, 0x03, " %3d", "DO", 5, 0x04, "   ?"},

  { 1, 6, 0x80, " %3d", "TV"},
  { 1, 6, 0x40, " %3d", "TI"},
  { 1, 6, 0x3F, " %3d", "ID"},
  
  { 1, 7, 0xFF, "  %02X", "B0"},
  { 1, 8, 0xFF, "  %02X", "B1"},
  { 1, 9, 0xFF, "  %02X", "B2"},
  { 1,10, 0xFF, "  %02X", "B3"}
};
  
  printf("CHD: ");

  for (i=0; i<((sizeof CHD)/(sizeof CHD[0])); i++) 
  {
    switch (CHD[i].mod) 
    {
      case 1:
        printf(CHD[i].fmt, unpack8(Frame[CHD[i].ind - 1], CHD[i].msk));
        break;

      case 2:
        if ((Frame[CHD[i].jnd - 1] & CHD[i].chk) == 0) 
          printf(CHD[i].err);
        else 
          printf(CHD[i].fmt, unpack8(Frame[CHD[i].ind - 1], CHD[i].msk));
        break;
    }
  }

  printf("\nCHD  ");

  for (i=0; i<((sizeof CHD)/(sizeof CHD[0])); i++) 
    printf(" %3s", CHD[i].nam);

  printf("\n");
}

//~----------------------------------------------------------------------------

void displayCHDbf(pCHDbf pCHD) 
{
char title[256];

//HexDump((int8*) pCHD, sizeof *pCHD, TRUE);

  strcpy(title, "CHD  ");
  printf("\rCHD: ");
  strcat(title, "  HB");
  printf(" %3d", pCHD->HeartBeat);
  strcat(title, "  CC");
  printf(" %3d", pCHD->CmdCount);
  strcat(title, "  ID");
  printf(" %3d", pCHD->JMDC);
  strcat(title, "  SF");
  printf(" %3d", pCHD->Software);
  strcat(title, "  VE");
  printf(" %3d", pCHD->Vehicle);
  strcat(title, "  AL");
  printf(" %3d", pCHD->Alert);
  strcat(title, "  PS");
  printf(" %3d", pCHD->PowerStep);

  strcat(title, "  HA");
  printf(" %3d", pCHD->HRDLlight);
  strcat(title, "  HV");
  printf(" %3d", pCHD->HOset);
  strcat(title, "  HO");
  if (pCHD->HOset)
    printf(" %3d", pCHD->HRDLowner);
  else
    printf("   ?");
  strcat(title, "  MA");
  printf(" %3d", pCHD->QActive);
  strcat(title, "  MV");
  printf(" %3d", pCHD->MOset);
  strcat(title, "  MO");
  if (pCHD->MOset)
    printf(" %3d", pCHD->MCTowner);
  else
    printf("   ?");
    
  strcat(title, "  LA");
  printf(" %3d", pCHD->LRDLactive);
  strcat(title, "  LV");
  printf(" %3d", pCHD->LOset);
  strcat(title, "  LO");
  if (pCHD->LOset)
    printf(" %3d", pCHD->LRDLowner);
  else
    printf("   ?");
  
  strcat(title, "  DA");
  printf(" %3d", pCHD->DAQactive);
  strcat(title, "  DV");
  printf(" %3d", pCHD->DOset);
  strcat(title, "  DO");
  if (pCHD->DOset)
    printf(" %3d", pCHD->DAQowner);
  else
    printf("   ?");
  
  strcat(title, "  TV");
  printf(" %3d", pCHD->DataValid);
  strcat(title, "  TI");
  printf(" %3d", pCHD->DataInit);
  strcat(title, "  ID");
  printf(" %3d", pCHD->TimeMux);
  
  strcat(title, "  B0");
  printf("  %02X", pCHD->MuxBytes[0]);
  strcat(title, "  B1");
  printf("  %02X", pCHD->MuxBytes[1]);
  strcat(title, "  B2");
  printf("  %02X", pCHD->MuxBytes[2]);
  strcat(title, "  B3");
  printf("  %02X", pCHD->MuxBytes[3]);

  printf("\n%s", title);

  fflush(stdout);
  printf("\r\x1b[K");
}

//~============================================================================

// file chd.c
//
// CHD data display
//
// developed by Alexei Lebedev
//

//~============================================================================

static int16 unpack16(int16 code, int16 mask) {

  int16 LSB = mask & (mask ^ mask << 1);

  if (LSB) return (code & mask) / LSB;
  else     return 0;
}

//~----------------------------------------------------------------------------

void display_CHD_aal(int16 Status, int8 *Frame) {

  int i;
  struct {
    int   mod;
    int   ind;
    int8  msk;
    char *fmt;
    char *nam;
    int   jnd;
    int8  chk;
    char *err;
  } CHD[100] = {
    { 1, 1, 0xFF, " %3d", "HB"},
    { 1, 2, 0xFF, " %3d", "CC"},

    { 1, 3, 0xC0, " %3d", "ID"},
    { 1, 3, 0x20, " %3d", "SF"},
    { 1, 3, 0x10, " %3d", "VE"},
    { 1, 3, 0x08, " %3d", "AL"},
    { 1, 3, 0x07, " %3d", "PS"},

    { 1, 4, 0x80, " %3d", "HA"},
    { 1, 4, 0x40, " %3d", "HV"},
    { 2, 4, 0x30, " %3d", "HO", 4, 0x40, "   ?"},
    { 1, 4, 0x08, " %3d", "MA"},
    { 1, 4, 0x04, " %3d", "MV"},
    { 2, 4, 0x03, " %3d", "MO", 4, 0x04, "   ?"},

    { 1, 5, 0x80, " %3d", "LA"},
    { 1, 5, 0x40, " %3d", "LV"},
    { 2, 5, 0x30, " %3d", "LO", 5, 0x40, "   ?"},
    { 1, 5, 0x08, " %3d", "DA"},
    { 1, 5, 0x04, " %3d", "DV"},
    { 2, 5, 0x03, " %3d", "DO", 5, 0x04, "   ?"},

    { 1, 6, 0x80, " %3d", "TV"},
    { 1, 6, 0x40, " %3d", "TI"},
    { 1, 6, 0x3F, " %3d", "ID"},
  
    { 1, 7, 0xFF, "  %02X", "B0"},
    { 1, 8, 0xFF, "  %02X", "B1"},
    { 1, 9, 0xFF, "  %02X", "B2"},
    { 1,10, 0xFF, "  %02X", "B3"},
  
    { 0}};
  
  printf("\rCHD:   %04X", Status);

  for (i=0; i<100; i++) {

    int ind, jnd;

    if (!CHD[i].mod) break;
    ind = (CHD[i].ind - 1) ^ 1;

    switch (CHD[i].mod) {
    
    case 1:
      printf(CHD[i].fmt, unpack16((int16)Frame[ind], (int16)CHD[i].msk));
      break;

    case 2:
      jnd = (CHD[i].jnd - 1) ^ 1;
      if ((Frame[jnd] & CHD[i].chk) == 0) {
        printf(CHD[i].err);
      }
      else {
        printf(CHD[i].fmt, unpack16((int16)Frame[ind], (int16)CHD[i].msk));
      }
      break;
    
    }
  }

  printf("\nCHD: Status");

  for (i=0; i<100; i++) {
    if (!CHD[i].mod) break;
    printf(" %3s", CHD[i].nam);
  }
  fflush(stdout);
  printf("\r\x1b[K");
}

//~============================================================================
