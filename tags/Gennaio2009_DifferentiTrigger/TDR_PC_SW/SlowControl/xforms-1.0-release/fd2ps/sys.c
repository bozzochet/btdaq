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
 * $Id: sys.c,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 */

#include "fd2ps.h"
#include <string.h>
#include <time.h>
#include <ctype.h>

const char *
ascii_date(void)
{
    time_t t = time(0);
    static char buf[64];

    strcpy(buf, asctime(localtime(&t)));
    buf[strlen(buf) - 1] = '\0';
    return buf;
}

#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

const char *
whoami(void)
{
#ifndef __VMS
    struct passwd *passwd = getpwuid(getuid());
    const char *name = passwd ? passwd->pw_name : "Unknown";
    endpwent();
#else
    const char *name = getenv("USER");
#endif
    return name ? name : "unknown";
}


#include "sys.h"

char *
fl_strdup(const char *s)
{
    char *p = malloc(1 + strlen(s));
    return p ? strcpy(p, s) : p;
}

#ifdef NO_STRCASECMP
int
strcasecmp(register const char *s1, register const char *s2)
{
    register int c1, c2;

    while (*s1 && *s2)
    {
	c1 = tolower(*s1);
	c2 = tolower(*s2);
	if (c1 != c2)
	    return (c1 - c2);
	s1++;
	s2++;
    }
    return (int) (*s1 - *s2);
}

#endif
