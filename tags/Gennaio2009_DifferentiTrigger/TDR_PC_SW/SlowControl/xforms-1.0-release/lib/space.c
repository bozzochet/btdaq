/*
 *
 *  This file is part of the XForms library package.
 *
 * XForms is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1, or
 * (at your option) any later version.
 *
 * XForms is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with XForms; see the file COPYING.  If not, write to
 * the Free Software Foundation, 59 Temple Place - Suite 330, Boston,
 * MA 02111-1307, USA.
 *
 */


/*
 * $Id: space.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *
 *
 *  Remove unescaped leadingi/trailing spaces from a string.
 *
 ***********************************************************************/
#if !defined(lint) && defined(F_OK)
static char *id_space = "$Id: space.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "forms.h"
#include "ulib.h"

static int es = '\\';     /* escape character */

/** Remove leading space */
char *
fl_de_space(char *s)
{
    register char *p;
    /* not all isspace considers '\t' a white space */
    for (p = s ; p && (isspace( ( int ) *p) || *p=='\t'); p++)
       ;
    return (p == s) ? s : strcpy(s, p);
}

/* remove trailing space */
char *
fl_space_de(register char *s)
{
    register char *p, *q;

    if (!s || !*s)
	return s;

    q = p = s + strlen(s) - 1;
    /* maybe replace \ with space ? */
    for ( q--; p >= s && isspace( ( int ) *p) && (q < s || *q != es);p--,q--)
        ;
    *++p = '\0';
    return s;
}

/***** remove space from both ends *******/
char *
fl_de_space_de(char *p)
{
    return (fl_space_de(fl_de_space(p)));
}

/* remove all spaces from string */
char *
fl_nuke_all_spaces(char *s)
{
    char *p = s, *q = s + strlen(s), *b;
    char buf[1024];

    for  (b = buf ; p < q; p++)
        if(!isspace( ( int ) *p))
           *b++ = *p;
    *b = '\0';
    return strcpy(s, buf);
}

/* remove non-alphanumericals from string */
char *
fl_nuke_all_non_alnum(char *s)
{
    char *p = s, *q = s + strlen(s), *b;
    char buf[1024];
    for  (b = buf ; p < q; p++)
        if(isalnum( ( int ) *p))
           *b++ = *p;
    *b = '\0';
    return strcpy(s, buf);
}


#ifdef TEST
main()
{
    char buf[100], *p;

    while (fgets(buf, 100, stdin))
      {
	  buf[99] = '\0';
	  if((p = strchr(buf,'\n')))
	    *p = '\0';
	  fprintf(stderr, "^%s$\n",buf);
	  fprintf(stderr, "^%s$\n",de_space_de(buf));
      }
}
#endif /* TEST */
