/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
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

//  whoami.c   Pretty up the whoami version

//  Revision History
//    Feb 2009 by Peter Dennett
//      - Initial version CERN 
//    Jul 2011 by Peter Dennett
//      - Added host name to whoami
//    Aug 2011 by Peter Dennett
//      - Added instance to whoami and provided interface to set

#include "aallib.h"
#include "abi.h"
#include "egsedatatypes.h"

#ifdef OS_WIN32
# include <windows.h>
#endif
#include <unistd.h> 

////////////////////////////////////////////////////////////////////////////////

void UpdateWhoAmI(pEGSE_WhoAmI pWhoAmI, char VERSION[], char BuildId[])
{
int i,j;
int dash=0;

  i = 0;

  if (strncmp(VERSION, "$Rev", 4) == 0)
  {
    for (j=0; VERSION[j]; )
    {
      if (VERSION[j] == ' ')
      {
        j++;
        goto Space1;
      }
      j++;
    }

   Space1:

    while (VERSION[j] )
    {
      if (VERSION[j] == ' ')
        goto Space2;
      pWhoAmI->Version[i++] = VERSION[j];
      j++;
      if (i > 5)
        goto Space2;
    }

   Space2:

    strcpy(&(pWhoAmI->Version[i]), " Bld ");
    j += 5;

    strcat(pWhoAmI->Version, BuildId);
  }
  else
  {
    for (j=0; VERSION[j]; j++)
    {
      switch (VERSION[j])
      {
        case '-':
          if (dash++ > 0)
          {
#if 0
            // Will always so dirty because we touch source files, like the scripts
            if (strcasestr(VERSION, "dirty"))
              strcat(pWhoAmI->Version, "+");
#endif
            goto SkipHash;
          }
          pWhoAmI->Version[i++] = '.';
          break;
        default:
          pWhoAmI->Version[i++] = VERSION[j];
          break;
      }
      if (i >= ((sizeof pWhoAmI->Version)-5))
        break;
    }
  SkipHash:
    if (strcmp(BuildId, "1") != 0)
    {
      if (dash < 1)
        strcat(pWhoAmI->Version, ".0");

      strcat(pWhoAmI->Version, ".");
      strcat(pWhoAmI->Version, BuildId);
    }
  }

  gethostname(pWhoAmI->HostName, sizeof pWhoAmI->HostName);

  for (i=0; i<sizeof pWhoAmI->HostName; i++)
    if (pWhoAmI->HostName[i] == '.')
      pWhoAmI->HostName[i] = 0;

  pWhoAmI->Instance[0] = 0;

  return;
}

////////////////////////////////////////////////////////////////////////////////

void UpdateWhoAmI2(pEGSE_WhoAmI pWhoAmI, char VERSION[], char BuildId[], 
                   char Instance[])
{
int i;

  UpdateWhoAmI(pWhoAmI, VERSION, BuildId);
  if ( ! Instance)
  {
    pWhoAmI->Instance[0] = 0;
  }
  else
  {
    strncpy(pWhoAmI->Instance, Instance, sizeof pWhoAmI->Instance);
    for (i=1; i<sizeof pWhoAmI->Instance; i++)
    {
      if (pWhoAmI->Instance[(sizeof pWhoAmI->Instance) - i] != ' ')
        return;
      pWhoAmI->Instance[(sizeof pWhoAmI->Instance) - i] = 0;
    }
  }
}
