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

//  apid.c     Lookup APID names

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version at CSIST

#include "aallib.h"
#include "apid.h"

APIDNameEnt APIDNames[] =
{
  { Name: "AMSHK",         APID: APID_AMSHK},
  { Name: "AMSSCI",        APID: APID_AMSSCI},
  { Name: "AMSCMD",        APID: APID_AMSCMD},
  { Name: "AMSHKH",        APID: APID_AMSHKH},
  { Name: "PBCKSCI",       APID: APID_PBCKSCI},
  { Name: "PBCKHKH",       APID: APID_PBCKHKH},
  { Name: "PTOPCMD",       APID: APID_PTOPCMD},
  { Name: "PTOPREPLY",     APID: APID_PTOPREPLY},
  { Name: "ACOPCMD",       APID: APID_ACOPCMD},
  { Name: "ACOPHK",        APID: APID_ACOPHK},
  //??  { Name: "ACOPQC",        APID: APID_ACOPQC},

  { Name: "APLS_C",        APID: APID_APLS_C},
  { Name: "APLS_D",        APID: APID_APLS_D},
  { Name: "ACOP_C",        APID: APID_ACOP_C},
  { Name: "ACOP_D",        APID: APID_ACOP_D},
  { Name: "DDRS2_C",       APID: APID_DDRS2_C},
  { Name: "DDRS2_D",       APID: APID_DDRS2_D},
  { Name: "FILL",          APID: APID_FILL},
};

#define APIDDim  ((sizeof APIDNames) / sizeof APIDNames[0])

////////////////////////////////////////////////////////////////////////////
//-Extern: apidLookupValue
int32 apidLookupValue(char *APID)
//-Synopsis:    Look up a APIDs value from a symbolic name
//-Returns:     Returns the APID's value or zero on error
//-Description: Look up a APIDs value from a symbolic name
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int i;

 for (i=0; i<APIDDim; i++)
   if (strcasecmp(APIDNames[i].Name, APID) == 0)
     return APIDNames[i].APID;

  return 0;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: apidLookupName
char *apidLookupName(int32 APID)
//-Synopsis:    Look up a APIDs value from a symbolic name
//-Returns:     Returns the APID's value or zero on error
//-Description: Look up a APIDs value from a symbolic name
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
int i;
static char NotFound[10];

 for (i=0; i<APIDDim; i++)
   if (APIDNames[i].APID == APID)
     return APIDNames[i].Name;

  sprintf(NotFound, "0x%lX", APID);
  return NotFound;
}
