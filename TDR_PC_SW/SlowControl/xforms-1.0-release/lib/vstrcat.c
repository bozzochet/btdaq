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
 * $Id: vstrcat.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *
 *   Similar to strcat, but takes variable number of argument
 *
 */
#if !defined(lint) && defined(F_ID)
char *id_vstrcat = "$Id: vstrcat.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "forms.h"
#include "ulib.h"

/* VARARGS1 */
char *
vstrcat(const char *s1,...)
{
    register size_t total = 0;
    register char *ret, *p;
    va_list ap;

    if (!s1)
	return 0;

    total = strlen(s1);

    /* record total length */
    va_start(ap, s1);
    while ((p = va_arg(ap, char *)))
	  total += strlen(p);
    va_end(ap);


    if (!(ret = malloc(total + 1)))
	return 0;

    strcpy(ret, s1);
    va_start(ap, s1);
    while ((p = va_arg(ap, char *)))
	  strcat(ret, p);
    va_end(ap);
    return ret;
}

/****** so to protect from M_DBG *******/
void
free_vstrcat(void *p)
{
    free(p);
}

/*
 * Simple test
 */
#ifdef TEST
#include <stdio.h>
#include <string.h>

main()
{
    char *p = vstrcat("Hello", " World", "!", (char *) 0);
    char *q = vstrcat(p, " again", " and again", (char *) 0);
    char *l = vstrcat(q, " again", " and again", (char *) 0);
    fprintf(stderr, "%s\n", p);
    fprintf(stderr, "%s\n", q);
    fprintf(stderr, "%s\n", l);
    free(p);
    free(q);
    free(l);
}

#endif

