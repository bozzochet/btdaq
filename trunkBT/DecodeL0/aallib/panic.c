/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2004 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                       searching for missing matter                        */
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

//  panic.c   Common diagnostic routine

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Extraction from MDCSoftware

#include "aallib.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: Panic
bool Panic(char *Format, ...)
//-Synopsis:    Log or display an error message a restart the system
//-Returns:     FALSE (if it returns at all)
//-ARG_I:       Format         Printf style format string
//-Description: Log or display an error message a restart the system.
//-             <p>
//-             This is for debugging only. Currently the message is
//-             just formatted and printf'd.
//-             <p> A value of FALSE is returned.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:       This routine should terminate or force WDT to expire.
//-Bugs:
//-Revisions:
//-End:
{
va_list arg_marker;
char msg[128];
#ifdef OS_LINUX
extern FILE *stdout;
#endif

  if ( ! Format)
    return FALSE;

  va_start(arg_marker, Format);
  vsnprintf(msg, sizeof msg - 2, Format, arg_marker);
  printf("%s\n", msg);

#ifdef OS_LINUX
  fflush(stdout);
#endif
  
  return FALSE;
}
