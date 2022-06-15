/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2013 by PADSOFT, Inc.                        */
/*                                                                           */
/*    Developed for use on AMS-2, An International Space Station payload     */
/*                      searching for missing matter                         */
/*                                                                           */
/*                         No rights reserved                                */
/*                                                                           */
/* Developed by:        Peter Armistead Dennett                              */
/*                            PADSOFT, Inc                                   */
/*                           61 Harbor Lane                                  */
/*                         Kemah, Texas 77565                                */
/*                          +1 713 899 6100                                  */
/*                                                                           */
/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/

//  commreport.h    Create a communications report for specific day

//  Revision History
//    Nov 2013 by Peter Dennett
//      - Initial version at JSC and PADSOFT.  Thanks to A.Basili

#ifndef _COMMREPORT_H
#define _COMMREPORT_H

#include "sspdefs.h"
#include <time.h>
#include <sys/time.h>

//~============================================================================

typedef struct _COMMREPORT
{
  int year;
  int jday;

  int passes;
  struct _commpass 
  {
    char orbit[12];
    char tdrs[12];
    char band[12];
    enum _commservice
    {
      comm_ku=0,
      comm_s=1,
    } commservice;
    time_t start;
    time_t end;
  } commpass[1];
} COMMREPORT, *pCOMMREPORT;

//~============================================================================

pCOMMREPORT GetCommReport(int year, int jday, char *host, char *user, 
                          char *pass, int Debug);
void PrintCommReportGnuPlotKu(pCOMMREPORT pCR);
void PrintCommReportGnuPlotSband(pCOMMREPORT pCR);
void PrintCommReport(pCOMMREPORT pCR);

#endif // _COMMREPORT_H
