/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2011 by PADSOFT, Inc.                        */
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

//  rcb.h   Registered CallBacks

//  Revision History
//    Nov 2011 by Peter Dennett
//      - Moved from dldefs.h

#ifndef _RCB_H
#define _RCB_H

#include "aallib.h"
#include "abi.h"
#include "logfile.h"

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#define RCB_DEBUG     0x40000000        // Callback should print parameters
#define RCB_FUNC_MASK 0x000000FF        // Mask covering all values below

#define RCB_NOP       0                 // Do nothing
#define RCB_OPEN      1                 // Initialize callback
#define RCB_PROCESS   2                 // Normal callback processing
#define RCB_CLOSE     0xFF              // De-initialize callback

typedef bool rcbRoutine(int Function, void *reg_data, void *call_data);

typedef struct _rcbCallback
{
  rcbRoutine *callback_func;     // Function registered for call back 
  void *reg_data;                // User data from register
  int cbnest;                    // Control for recursive callbacks 
  struct _rcbCallback *next;     // Next function on list 
} rcbCallback, *prcbCallback;

bool rcbRegister(prcbCallback *list, rcbRoutine routine, void *reg_data);
bool rcbDo(prcbCallback *list, int Function, void *call_data);
bool rcbDelete(prcbCallback *list, prcbCallback callback);
bool rcbDeregister(prcbCallback *list, rcbRoutine routine, void *reg_data);

//~ - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

#endif // _RCB_H
