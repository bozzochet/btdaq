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

//  cmdopt.h   getopt_long replacement

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Lake Howard Hotel, Taiwan

#ifndef CMDOPT_H
#define CMDOPT_H

#ifdef __cplusplus
extern "C" {
#endif


extern char *optarg;
extern int   optind;
extern int   opterr;
extern int   optopt;

#ifdef no_argument
# undef no_argument
#endif
#define no_argument 0

#ifdef required_argument
# undef required_argument
#endif
#define required_argument 1

#ifdef optional_argument
# undef optional_argument
#endif
#define optional_argument 2

struct _CmdOpt
{
  const char *name;
  int has_arg;
  int *flag;
  int val;
  const char *Description;
  const char *ArgName;
};

int CmdOptGet (int argc, char *const argv[], const char *optstring,
               const struct _CmdOpt *longopts, int *longindex);

void CmdOptUsage (const char *optstring, const struct _CmdOpt *longopts);

#ifdef __cplusplus
}
#endif

#endif // CMDOPT_H
