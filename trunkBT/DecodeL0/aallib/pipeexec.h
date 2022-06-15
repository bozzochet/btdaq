/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2013 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
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

//  pipeexec.h   popen with PID and STDERR handling

//  Revision History
//    Aug 2013 by Peter Dennett - Kemah
//      - Coded using several Internet examples

#ifndef _PIPEEXEC_H
#define _PIPEEXEC_H

#include "sspdefs.h"
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

FILE *pipeexec(char *cmd, char *type, pid_t *pPID);
void pipeexecclose(FILE *fp, pid_t PID);

#endif // _PIPEEXEC_H
