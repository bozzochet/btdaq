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
 * $Id: n2a.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *
 * Convert numbers to strings. Not the fastest but faster than sprintf.
 * Useful in monitoring situations.
 *
 * Converted strings are in a static buffer.
 *
 * On IRIX 3.3, itoa is about 3 times faster than sprintf
 *              ftoa is about 4 times (6digits)faster than sprintf
 */
#if !defined(lint) && defined(F_ID)
char *id_n2a = "$Id: n2a.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include <string.h>
#include "forms.h"
#include "ulib.h"

static const char *digits = "0123456789";

/************* convert integer to string **********/

const char *
fl_itoa(register int n)
{
    static char buf[20];
    register char *p = buf + sizeof(buf);
    register int l, sign = 0;

    if (n < 0)
    {
	sign = '-';
	n = -n;			/* might overflow if n most negative */
    }

    *--p = '\0';		/* terminating 0 */
    while (n >= 10)
    {
	*--p = *(digits + (n - (l = n / 10) * 10));
	n = l;
    }

    *--p = *(digits + n);
    if (sign)
	*--p = sign;

    return p;
}

/*******  convert float to a string. Max keep 6 digits *****/

static int multab[] =
{
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000
};

const char *
fl_ftoa(float f, int n)
{
    static char buf[33];
    register char *fbuf;
    static int nmax = sizeof(multab) / sizeof(multab[0]);
    int i, sign = 0;
    register float ff;
    register int fi, ffi, l;

#if 1
    if (n >= nmax || n < 0)
	n = nmax - 1;

    if (f < 0)
    {
	f = -f;
	sign = '-';
    }

    f += 0.05 / multab[n];	/* rounding */
    fi = f;
    ff = f - fi;

    *(fbuf = buf + n + 2) = '.';

    if (ff < 1.0 / multab[n])
    {
	*++fbuf = '0';
    }
    else
    {
	for (i = 1; i <= n; i++)
	{
	    *++fbuf = *(digits + (ffi = (ff *= 10.0)));
	    ff = ff - ffi;
	}
    }
    *++fbuf = '\0';
    /* covert the interal part */

    fbuf = buf + n + 2;
    while (fi >= 10)
    {
	*--fbuf = *(digits + (fi - (l = fi / 10) * 10));
	fi = l;
    }
    *--fbuf = *(digits + fi);
    if (sign)
	*--fbuf = sign;
    return fbuf;

#else
    sprintf(buf, "%.*f", n, f);
    return buf;
#endif
}


#ifdef TEST
#include <stdio.h>
main()
{
    static int nn[] =
    {0, 1, 5, -1, -10, 1234567, -121};
    static float ff[] =
    {1.2, -2.1, 0.01, 0.001, 1.00, -123456.252, 1.23552};
    int j;

    for (j = 0; j < sizeof(nn) / sizeof(nn[0]); j++)
	fprintf(stderr, "itoa(%d)=%s\n", nn[j], itoa(nn[j]));

    for (j = 0; j < sizeof(ff) / sizeof(ff[0]); j++)
	fprintf(stderr, "ftoa(%f, %d)=%s\n", ff[j], j, ftoa(ff[j], j));


}

#endif
