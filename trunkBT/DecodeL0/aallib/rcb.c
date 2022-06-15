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

//  rcb.c   Regisgtered call back routines

// This set of routines provides services to maintain and use registered
// call back lists.

//  Revision History
//    May 1994 by Peter Dennett
//      - Initial version developed for the EPS project
//    Jul 2004 by Peter Dennett
//      - Munged for AMS-02 in Taiwan
//    Oct 2010 by Peter Dennett
//      - Moved into eAss environment from aal/
//    Nov 2011 by Peter Dennett
//      - Moved from dl directory

#include "rcb.h"
#include "logfile.h"

////////////////////////////////////////////////////////////////////////////
//-Extern: rcbRegister
bool rcbRegister(prcbCallback *list, rcbRoutine routine, void *reg_data)
//-Synopsis:    Register a call back on a list of call backs.
//-Returns:     TRUE if no errors, else FALSE
//-ARG_U:       list          Pointer to the list head of call backs.
//-ARG_I:       routine       Pointer to the routine which will be called.
//-ARG_I:       reg_data      Pointer to a data area to be passed to the
//-                           the callback routine.
//-Description: Register a call back on a list of call backs.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
prcbCallback callback;

  callback = (prcbCallback) malloc(sizeof(*callback));
  if ( ! callback)
    return FALSE;

  callback->callback_func = routine;
  callback->reg_data = reg_data;
  callback->cbnest = 0;

  callback->next = *list;
  *list = callback;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: rcbDo
bool rcbDo(prcbCallback *list, int Func, void *call_data)
//-Synopsis:    Perform a list of registered callbacks.
//-ARG_U:       list          Pointer to the list head of call backs.
//-ARG_I:       Func          Function for the callback to perform
//-ARG_I:       call_data     Pointer to a data area to be passed to the
//-                           the callback routine.
//-Description: Perform a list of registered callbacks.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
static int cbnest = 0;
prcbCallback wlist;
int cbkey;

 if (list == NULL)
   return TRUE;

//  First establish our bit mask value that indicates that this callback
//  is to be done.

  cbkey = 1 << cbnest++;
  if (cbkey <= 0)
  {
    cbnest--;
    LogError("rcbDo: Too deep %d\n", cbnest);
    return FALSE;
  }

//  Now set this bit in all current callbacks

  for (wlist=(*list); wlist!=NULL; wlist=wlist->next)
    wlist->cbnest |= cbkey;

//  Now start at front of list and call each routine not yet called
//  (bit mask value is still one), then clear is value indicating it has
//  been called.  Start at front of list again each time a routine
//  is called in case the callback created or deleted a call back
//  resulting in a change in the list we are working.  If the routine
//  has been called press on down the list.

  for (wlist=*(list); wlist!=NULL;)
  {
    if ((wlist->cbnest & cbkey) != 0)
    {
      wlist->cbnest &= ~cbkey;
      (*(wlist->callback_func))(RCB_FUNC_MASK & Func, wlist->reg_data, call_data);
      wlist = (*list);
    }
    else
    {
      wlist = wlist->next;
    }
  }

  cbnest--;

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: rcbDelete
bool rcbDelete(prcbCallback *list, prcbCallback callback)
//-Synopsis:    Delete a registered call back from a list.
//-ARG_U:       list          Pointer to the list head of call backs.
//-ARG_I:       callback      Pointer to the structure defining the callback.
//-Description: Delete a registered call back from a list.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
prcbCallback pcb;

  if (callback == *list)
  {
    *list = callback->next;
  }
  else
  {
    for (pcb= *list; pcb!=NULL && pcb->next!=callback;)
      pcb=pcb->next;

    if (pcb == NULL)
    {
      Panic("rcbDelete: Not found");
      return FALSE;
    }

    pcb->next = callback->next;
  }

  free( (char *) callback);

  return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//-Extern: rcbDeregister
bool rcbDeregister(prcbCallback *list, rcbRoutine routine, void *reg_data)
//-Synopsis:    De-register a call back on a list of call backs.
//-ARG_U:       list          Pointer to the list head of call backs.
//-ARG_I:       routine       Pointer to the routine which will be called.
//-ARG_I:       reg_data      Pointer to a data area to be passed to the
//-                           the callback routine.
//-Description: De-register a call back on a list of call backs.
//-SeeAlso:
//-Includes:
//-Method:
//-Restrictions:
//-Notes:
//-Bugs:
//-Revisions:
//-End:
{
prcbCallback callback, tcb;

  for (callback = *list; callback!=NULL; )
  {
    if (callback->callback_func==routine&&callback->reg_data==reg_data)
    {
      tcb = callback->next;
      rcbDelete(list, callback);
      callback = tcb;
    }
    else
    {
      callback = callback->next;
    }
  }

  return TRUE;
}
