/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                     Copyright 2007 by PADSOFT, Inc.                       */
/*                                                                           */
/*        Developed for use on International Space Station Payloads          */
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

//  ssptcl.h   Pick up some slack in the TCL include files

//  Revision History
//    Jul 2007 by Peter Dennett
//      - Initial version - CERN

#ifndef _SSPTCL_H
#define _SSPTCL_H

int Tcl_Invoke TCL_VARARGS_DEF(Tcl_Interp *, arg1);
int Tcl_InvokeObj TCL_VARARGS_DEF(Tcl_Interp *, arg1);

#if 0
#ifndef OS_LINUX
Tcl_Namespace * Tcl_CreateNamespace(Tcl_Interp *interp, CONST char *name,
	                                  ClientData clientData, 
                                    Tcl_NamespaceDeleteProc *deleteProc);
void Tcl_DeleteNamespace(Tcl_Namespace *nsPtr);
#endif // OS_LINUX
#endif

#endif  // _SSPTCL_H
