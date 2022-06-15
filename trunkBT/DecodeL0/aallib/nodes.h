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

#ifndef _NODES_H
#define _NODES_H

//  nodes.h   Information about our nodes

//  Revision History
//    Nov 2003 by Peter Dennett
//      - Initial version
//    Jul 2004 by Peter Dennett
//      - Changed from table to callable

#define nodesGroupNONE    0x00
#define nodesGroupCAN     0x01
#define nodesGroupAMSW    0x02
#define nodesGroupHRDL    0x04
#define nodesGroupFUNC    0x08
#define nodesGroupUNASG   0x10
#define nodesGroupDEFAULT 0x20
#define nodesGroupEXTERN  0x40

typedef struct _NodeEntry
{
  char Name[12];          // For debug only
  int32 Group;            // Which group this node belongs with
  int32 Task;             // Task to handle this node.  This is placed
                          // here by daqd during its initialization.
} NodeEntry, *pNodeEntry;

extern NodeEntry NodeNames[];
extern int32 NodeNamesDim;

int32 nodesLookupValue(char *Name);
char *nodesLookupName(int32 Node);

#endif // _NODES_H
