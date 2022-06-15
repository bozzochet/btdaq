/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2008 by PADSOFT, Inc.                        */
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

//  frmDump.c   Standardized frame display

//  Revision History
//    May 2008 by Peter Dennett
//      - Initial version in Kemah office
//    Feb 2009 by Peter Dennett
//     - LogNormal usage

#include "aallib.h"
#include "frm.h"
#include "logfile.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: frmDump
bool frmDump(pFRAME pFRM, int8 *Frame, char *Tag)
//-Synopsis:  Standardized frame display
//-Returns: TRUE
//-ARG_I:       pFRM       Object for this instance
//-ARG_I:       Frame      The frame to dumpcurrently being de-blocked.  The contents
//-                        maybe modified by this process. 
//-ARG_I:       Tag        Text tag to mark display
//-Description: Display an AMS Frame to stdout.
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
int16 APID;
char line[512], *cp;

  cp = line;

  APID = ((*Frame & 7) << 8) | (*(Frame+1));

  cp += sprintf(cp, "%s: APID:%X ", Tag, APID);

  for (i=0; i<24; i++) 
  {
    cp += sprintf(cp, "%02X", Frame[i]); 
    if (i%2  ==  1) 
      cp += sprintf(cp, " ");
  }

  cp += sprintf(cp, "  ...  ");

  for (i=pFRM->FrameSize-10; i<pFRM->FrameSize; i++)
  {
    cp += sprintf(cp, "%02X", Frame[i]); 
    if (i%2  ==  1)
      cp += sprintf(cp, " ");
  }

  frmLogError(pFRM, "%s\n", line);

  return TRUE;
}
