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

//  abiLog.c   Log an AMSBlock

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version at CSIST

#include "aallib.h"

#include "abi.h"

#include <sys/time.h>

////////////////////////////////////////////////////////////////////////////
//-Extern: abiLog
bool abiLog(FILE *fh, pAMSBlock pBlock)
//-Synopsis:    Log the AMSBlock in XML format to a file
//-Returns:     TRUE, there are no errors possible
//-ARG_I:       fh              File handle to fprintf to
//-ARG_I:       pBlock          Pointer to the AMSBlock to log
//-Description: Log the AMSBlock in XML format to a file
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
struct timeval tv;

int i;
int8 *cp;
#define BPL 32

   gettimeofday(&tv, NULL);

  fprintf(fh, "<AMSBlock NodeAddress=\"0x%X\" DataType=\"0x%lX\" BlockType=\"%s-%s\" "
              "Sec=\"%ld\" USec=\"%ld\">", pBlock->NodeAddress, pBlock->DataType,
              pBlock->BlockType&BT_REPLY?"RP":"RQ",
              pBlock->BlockType&BT_WRITE?"W":"WR", tv.tv_sec, tv.tv_usec);

  cp = abiDataPtr8(pBlock);

  for (i=0; i<pBlock->DataCount; i++)
  {
    if ((i % BPL) == 0) 
      fprintf(fh, "\n  ");

    fprintf(fh, "%02X",  *cp++); 
    if (i & 1)
      fprintf(fh, " ");
  }

  if ((i % BPL) != 0) 
      fprintf(fh, "\n");

  fprintf(fh, "</AMSBlock>\n");

  return TRUE;
}
