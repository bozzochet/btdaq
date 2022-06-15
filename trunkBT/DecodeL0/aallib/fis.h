/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
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

//  fis.h   File Identifier Series

#ifndef _FIS_H
#define _FIS_H

//  Revision History
//    Jun 2009 by Peter Dennett
//      - Initial version CERN clean room based in part on code from
//        Cai, SEU, VK, and AAL (gcslib.c)

void fisRemoveTrailingSlash(char *s);
int fisFirstFileInDir(char *DirPath, int n);
int fisFirstFile(char *directory);
int fisLastFileInDir(char *DirPath, int n);
int fisLastFile(char *directory);
FILE *fisOpenBaseFile(char *Path, int FileNumber, char *extension);
int fisUnlinkBaseFile(char *Path, int FileNumber, char *extension);
int fisMoveBaseFile(char *Path, int FileNumber, char *extension, char *user,
                    char *group);
int fisDirCheckAndCreate(char *name);

#endif // _FIS_H
