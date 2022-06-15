/*~*~*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*-*~*~*/
/*                                                                           */
/*                    Copyright 2009 by PADSOFT, Inc.                        */
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

//  argv.c   Building of argv arrays

//  Revision History
//    Sep 2009 by Peter Dennett
//      - Initial version CERN

#include "argv.h"
#include <malloc.h>
#include <string.h>

//~============================================================================

#ifndef isspace
# define isspace(ch) ((ch)==' ' || (ch)=='\t')
#endif

#ifndef NULL
# define NULL 0
#endif

#ifndef EOS
# define EOS '\0'
#endif

#define INITIAL_MAXARGC 8	// Number of args + NULL in initial argv 

//~============================================================================

void argvfree(char **vector)
{
register char **scan;

  if (vector != NULL)
  {
    for (scan = vector; *scan != NULL; scan++)
      free (*scan);
    free (vector);
  }
}

//~============================================================================

char **argvbuild(char *input, int *argcrtn)
{
char *arg=NULL;
char *copybuf;
int squote = 0;
int dquote = 0;
int bsquote = 0;
int curlyquote = 0;
int argc = 0;
int maxargc = 0;
char **argv = NULL;
char **nargv;

  if (input != NULL)
  {
    copybuf = malloc(strlen (input) + 1);
    do                                  // Always do at least once
    {
      while (isspace (*input))          // Skip leading white space 
	      input++;

      if ((maxargc == 0) || (argc >= (maxargc - 2)))
	    {
        if (argv == NULL)              // argv needs initialization, or expansion
        {
          maxargc = INITIAL_MAXARGC;
          nargv = (char **) malloc (maxargc * sizeof (char *));
        }
	      else
        {
          maxargc *= 2;
          nargv = (char **) realloc (argv, maxargc * sizeof (char *));
        }

	      if (nargv == NULL)
        {
          if (argv != NULL)
          {
            argvfree(argv);
            argv = NULL;
          }
          break;
        }

	      argv = nargv;
	      argv[argc] = NULL;
	    }

      arg = copybuf;                    // Begin scanning arg
      while (*input != EOS)
	    {
        //printf("inp \"%c\"\n", *input);
	      if (isspace (*input) && !(squote || dquote || curlyquote))
          break;

        if (bsquote)
        {
          bsquote = 0;
          *arg++ = *input;
        }
        else if (*input == '\\')
        {
          bsquote = 1;
        }
        else if (squote)
        {
          if (*input == '\'')
            squote = 0;
          *arg++ = *input;
        }
        else if (dquote)
        {
          if (*input == '"')
            dquote = 0;
          *arg++ = *input;
        }
        else if (curlyquote)
        {
          if (*input == '}')
            curlyquote = 0;
          *arg++ = *input;
        }
        else
        {
          if (*input == '\'')
            squote = 1;
          else if (*input == '"')
            dquote = 1;
          else if (*input == '{')
            curlyquote = 1;

          *arg++ = *input;
        }

        input++;
	    }

      if ( ! bsquote)
      {
        *arg = EOS;
        argv[argc] = strdup (copybuf);
        //printf("argv[%d] \"%s\" input [%s]\n", argc, argv[argc], input);
        if (argv[argc] == NULL)
        {
          argvfree(argv);
          argv = NULL;
          break;
        }
        argc++;
        argv[argc] = NULL;
      }
      else
      {
        bsquote = 0;
      }
    }
    while (*input != EOS);

    if (arg != copybuf)
    {
      argc++;
      argv[argc] = NULL;
    }

    free(copybuf);
  }

  *argcrtn = argc - 1;
  return argv;
}
