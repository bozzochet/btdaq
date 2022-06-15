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

//  time.h   General time handling

//  Revision History
//    Mar 2003 by Peter Dennett
//      - Initial version at San Jose Del Cabo - pool side.
//    Jul 2004 by Peter Dennett
//      - Split general time code and DAQD timers
//    Feb 2009 by Peter Dennett
//      - Suppressed duplicate

#ifndef _AALTIME_H
#define _AALTIME_H

#include <ssptime.h>

//-Code: typedef TIMEVAL
//-  Defines a the basic time data type 
//-
//-  Time within DAQD is expressed as microseconds
//-  <pre>
//-  1 second = 1,000             milliseconds (ms) 
//-           = 1,000,000         microseconds (us)
//-           = 1,000,000,000     nanoseconds  (ns)
//-           = 1,000,000,000,000 picoseconds  (ps)
//-  </pre>
#define aalTIMEVAL sspTIMEVAL
#define paalTIMEVAL psspTIMEVAL
//-Code_End:

#endif // _AALTIME_H
