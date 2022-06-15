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

#ifndef _APID_H
#define _APID_H

//  apid.h   Information about our APIDS

//  Revision History
//    Nov 2003 by Peter Dennett
//      - Initial version
//    Jul 2004 by Peter Dennett
//      - Changed from table to callable

#define APID_UDP_BASE_PORT 62000
#define APID_TCP_BASE_PORT 62000

//  Define our APID values

typedef struct _APIDNameEnt
{
  char Name[20];  // Name of the APID
  int16 APID;      // The assigned value
} APIDNameEnt;

#define APID_AMSCMD_X     974
#define APID_AMSCMD       124
#define APID_AMSHK        876
#define APID_AMSSCI       976
#define APID_AMSHKH       977
#define APID_PBCKSCI      978
#define APID_PBCKHKH      979
#define APID_PTOPCMD      980
#define APID_PTOPREPLY    981
#define APID_ACOPCMD      982
#define APID_ACOPHK       876
//?? No longer AMS APID  #define APID_ACOPQC       975
#define APID_SPARE1       983


#define APID_APLS_C       001
#define APID_APLS_D       002
#define APID_ACOP_C       003
#define APID_ACOP_D       004
#define APID_DDRS2_C      005
#define APID_DDRS2_D      007

// Dummy and alias
#define APID_CAN          010
#define APID_AMSW         011
#define APID_RS422        005
#define APID_HRDL         982
#define APID_1553         24

#define APID_FILL         0x7FF

int32 apidLookupValue(char *APID);
char *apidLookupName(int32 APID);

#endif // _APID_H
