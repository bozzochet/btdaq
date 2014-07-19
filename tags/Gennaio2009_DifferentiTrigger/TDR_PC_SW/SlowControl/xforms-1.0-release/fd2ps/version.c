/*
 *
 * This file is part of XForms.
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
 * $Id: version.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *.
 *  This file is part of XForms package
 *  Copyright (c) 1997-2000  by T.C. Zhao
 *  All rights reserved.
 *.
 *
 * version information
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fd2ps.h"

static char *version[] =
{
    "fd2ps",
    "$State: beta $ $Revision: 0.36 $ of $Date: 1999/07/17 03:32:50 $",
    "Copyright (c) 1997-2000 by T.C. Zhao",
    0
};


static const char *
rm_rcs_kw(register const char *s, int strip_time)
{
    static char buf[6][255];
    static int nbuf;
    register char *q = buf[(nbuf = (nbuf + 1) % 5)];
    int left = 0, lastc = -1;

    while (*s && (q - buf[nbuf]) < sizeof(buf[nbuf]) - 2)
    {
	switch (*s)
	{
	case '$':
	    if ((left = !left))
		while (*s && *s != ':')
		    s++;
	    break;
	default:
	    /* copy the char and remove extra space */
	    if (!(lastc == ' ' && *s == ' '))
		*q++ = lastc = *s;
	    break;
	}
	s++;
    }

    *q = '\0';

    /* all xforms' RCS time has the format xx:xx:xx */
    if (strip_time)
    {
	if ((q = strchr(buf[nbuf], ':')))
	    *(q - 2) = '\0';
    }

    return buf[nbuf];
}

void
print_version(int die)
{
    char **q = version;

    for (; *q; q++)
	fprintf(stderr, (q == version) ? "%s" : "%s\n", rm_rcs_kw(*q, 0));

    if (die)
	exit(0);
}

const char *
get_version(void)
{
    static char buf[1024];
    char **q = version;

    for (buf[0] = '\0'; *q; q++)
	strcat(buf, rm_rcs_kw(*q, 1));

    return buf;
}
