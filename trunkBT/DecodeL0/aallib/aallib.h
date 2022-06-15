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

//  aallib.h   Defines for the general aallib routines

#ifndef _AALLIB_H
#define _AALLIB_H

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Lake Howard Hotel, Taiwan
//        based on MDCSoftware coding
//    Apr 2007 by Peter Dennett - CERN
//      - Moved to aal-tcl env
//    Dec 2007 by Peter Dennett
//        Updated for MinGW for WIN32 operation

#include "sspdefs.h"
#include "aalAlloc.h"

bool Panic(char *Format, ...);
void HexDump(void *Buffer, int32 Bytes, bool ByteSwap);
char *ExpandTilde(char *filename);

void Daemonize(char *LOCK_FILE, char *RUNNING_DIR, 
               char *LogDirectory, char *LogName, char *Instance);
void DaemonKill(char *LOCK_FILE, char *RUNNING_DIR);

double diskfree(char *dir);

#endif // _AALLIB_H
