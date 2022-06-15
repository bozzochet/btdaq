/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2014 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-02, An International Space Station payload    */
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

//  ptyopen.h   Wrapper to open a psuedo TTY

//  Revision History
//    Feb 2014 by Peter Dennett at GVA->IAD, whole row
//      -  Baseline development

#ifndef _PTYOPEN_H
#define _PTYOPEN_H

int ptyopen(char *command, char *user, int debug);
int ptyclose(int fdm, int debug);

#endif // _PTYOPEN_H
