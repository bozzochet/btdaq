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

//  cmdopt.c   getopt_long replacement

//  Revision History
//    Jul 2004 by Peter Dennett
//      - Initial version in Lake Howard Hotel, Taiwan

#include "aallib.h"

#include "cmdopt.h"

#ifdef __cplusplus
extern "C" {
#endif

char *optarg = 0;
int   optind = -1;
int   opterr = 1;
int   optopt = 0;
int   optnewinv = 1;
int   optnch;

int CmdOptGet (int argc, char *const argv[], const char *optstring,
               const struct _CmdOpt *longopts, int *longindex)
{

// New invocation.

  if (optnewinv == 1)
  {
    optind = 0;
    optnch = 0;
    optnewinv = 0;
  }

// No arguments left.

  if (optind + 1 == argc)
  {
    ++optind;
    optnewinv = 1;
    return -1;
  }

// New argument.

  if (optnch == 0)
  {
    ++optind;

// Long option or end-of-options marker.

    if (argv[optind][0] == '-' && argv[optind][1] == '-')
    {
      if (argv[optind][2] == '\0')
      {
        ++optind;
        return -1;
      }
      else
      {
      const char *begin = argv[optind] + 2;
      const char *end;
      const struct _CmdOpt *op;
                
        end = strchr (begin, '=');
        if (end == 0)
        {
          end = begin + strlen (begin);
        }

        for (op = longopts; op->name || op->val; ++op)
        {
          if ( ! op->name)
            continue;

          if (end - begin == strlen (op->name)
              && strncmp (begin, op->name, end - begin) == 0)
          {
          int ret = 0;

            if (! op->has_arg && *end == '=')
            {
              if (opterr)
              {
                fprintf (stderr, "%s: option `--%s' doesn't"
                         " allow an argument\n", argv[0], op->name);
              }
              ret = '?';
            }
            else
            {
              if (op->has_arg)
              {
                optarg = 0;
                if (*end == '=')
                {
                  optarg = (char*) end + 1;
                }
                else if (optind + 1 >= argc
                         && op->has_arg == required_argument)
                {
                  if (opterr)
                  {
                    fprintf (stderr, "%s: option `--%s' requires"
                             " an argument\n", argv[0], op->name);
                  }
                  return '?';
                }
                else if (optind + 1 >= argc)
                {
                  ; // do nothing.
                }
                else if (argv[optind + 1][0] != '-'
                         || op->has_arg == required_argument)
                {
                  optarg = argv[optind + 1];
                  ++optind;
                }
              }
              if (longindex)
              {
                *longindex = op - longopts;
              }
              if (op->flag)
              {
                *(((struct _CmdOpt*)op)->flag) = op->val;
              }
              else
              {
                ret = op->val;
              }
            }
            return ret;
          }
        }
        if (opterr)
        {
          fprintf (stderr, "%s: unrecognized option `--%s'\n",
                   argv[0], begin);
        }
        return '?';
      }
    }

// Short option.

    else if (argv[optind][0] == '-' && argv[optind][1] != '\0')
    {
      optnch = 1;
    }

// Non-option.

    else
    {
      // Dont stop now      optnewinv = 1;
      return -1;
    }
  } 

// Second+ character of a short option.  Note that this cannot be an
// else block because optnch is updated in the block above.

  if (optnch)
  {
  const char *op = strchr (optstring, argv[optind][optnch]);

    ++optnch;
    if (op)
    {
      if (op[1] == ':')
      {
        if (argv[optind][optnch] != '\0')
        {
          optarg = argv[optind] + optnch;
          optnch = 0;
        }
        else if (optind + 1 >= argc)
        {
          if (opterr)
          {
            fprintf (stderr, "%s: option requires an argument -- %c\n", 
                     argv[0], *op);
          }
          return '?';
        }
        else
        {
          optarg = argv[optind + 1];
          ++optind;
          optnch = 0;
        }
      }

			if (argv[optind][optnch] == '\0')
      {
        optnch = 0;
			}
      return *op;
    }
    else
    {
      if (opterr)
      {
        fprintf (stderr, "%s: invalid option -- %c\n",
                 argv[0], argv[optind][optnch - 1]);
      }
      return '?';
    }
  }

  return -1;
}

void CmdOptUsage (const char *optstring, const struct _CmdOpt *longopts)
{
int i;
int ts1, ts2; 
const char *cp;
int arg;
int bracket;
int col;
int cnt;

//  Determine the column widths

  ts1 = 0;
  ts2 = 0;

  for (i=0; longopts[i].name || longopts[i].val; i++)
  {
    bracket = 0;
    col = 0;
    cnt = 0;
    arg = 0;
    for (cp=optstring; *cp; cp++)
    {
      if (*cp == ':')
      {
        arg = 1;
      }
      else
      {
        if (*cp == longopts[i].val)
        {
          if (longopts[i].name && ! bracket)
          {
            bracket = 1;
            col += 1;
          }
          if (cnt)
            col += 1;
          col += 2;
          cnt++;
        }
        arg = 0;
      }
    }

    if (longopts[i].name)
    {
      if (cnt)
        col += 1;
      col += 2 + strlen(longopts[i].name);
      cnt++;
    }

    if (bracket)
      col += 1;

    if (col > ts1)
      ts1 = col;

    if (longopts[i].ArgName)
    {
      if (ts2 < strlen(longopts[i].ArgName))
        ts2 = strlen(longopts[i].ArgName);
    }
  }

  ts1 += 3;
  ts2 += ts1 + 3;

  for (i=0; longopts[i].name || longopts[i].val; i++)
  {
    bracket = 0;
    col = 0;
    cnt = 0;
    arg = 0;

    printf("    ");

    for (cp=optstring; *cp; cp++)
    {
      if (*cp == ':')
      {
        ;
      }
      else
      {
        if (*cp == longopts[i].val)
        {
          if (*(cp+1) == ':')
            arg = 1;
          else
            arg = 0;
          if (((arg == 0) && (longopts[i].has_arg != 0)) ||
              ((arg != 0) && (longopts[i].has_arg == 0)))
          {
            if (opterr)
            {
              fprintf (stderr, "Option flag inconsistant.  Index %d Option '%c'\n",
                       i, *cp);
            }
          }
          if (longopts[i].name && ! bracket)
          {
            bracket = 1;
            col += printf("{");
          }
          if (cnt)
            col += printf(" ");
          col += printf("-%c", *cp);
          cnt++;
        }
      }
    }

    if (longopts[i].name)
    {
      if (cnt)
        col += printf(" ");
      col += printf("--%s", longopts[i].name);
      cnt++;
    }

    if (bracket)
      col += printf("}");

    while (col < ts1)
      col += printf(" ");

    if (longopts[i].ArgName)
      col += printf("%s", longopts[i].ArgName);

    while (col < ts2)
      col += printf(" ");

    if (longopts[i].Description)
    {
      //      col += printf("%s", longopts[i].Description);
      for (cp = longopts[i].Description; *cp; cp++)
      {
        if ((*cp == '\n') || ((col > 70) && isspace(*cp)))
        {
          printf("\n");
          col = 0;
          while (col < ts2)
            col += printf(" ");
        }
        else
          col += printf("%c", *cp);
      }
    }

    printf("\n");
  }
}

#ifdef __cplusplus
}
#endif

