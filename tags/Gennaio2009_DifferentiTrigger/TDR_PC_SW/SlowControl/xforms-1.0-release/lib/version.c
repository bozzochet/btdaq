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
 * $Id: version.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao
 *  All rights reserved.
 *.
 *
 *  Version info
 *
 */

#if defined(F_ID) || defined(DEBUG)
char *fl_id_ver = "$Id: version.c,v 1.0 2002/05/08 05:16:30 zhao Release $";
#endif

#include <stdio.h>
#include "forms.h"

static const char *version[] =
{
    "FORMS Library",
    "$State: Release $  $Revision: 1.0 $ of $Date: 2002/05/08 05:16:30 $"
    "(Compiled "__DATE__")",
#ifdef FL_WIN32
    "Copyright (c) 1996-2002 by T.C. Zhao, Gang Li and Mark Overmars",
#else
    "Copyright (c) 1996-2002 by T.C. Zhao and Mark Overmars",
#endif
    "Parts Copyright(c) 1999-2002 by T.C. Zhao and Steve Lamont",
    0
};

/* it is important not to use the FL_INCLUDE_VERSION as it is
 * not automated with rcs checks in
 */
int
fl_library_version(int *ver, int *rev)
{
    int v, r = -1;
    int c = 0;

    sscanf(version[1], "%*s %*s %*s %*s %d.%d.%d", &v, &r, &c);
    if (c != FL_FIXLEVEL)
	M_err(0, "Inconsistent version: fixlevel:%d found %d", FL_FIXLEVEL, c);

    if (rev)
	*rev = r;
    if (ver)
	*ver = v;

    return v * 1000 + r;
}

void
fl_print_version(int g)
{
#ifndef M_XENIX
    const char *p[5], **q = version;
#else
    char *p[5], **q = (char **) version;
#endif
    int n;

    for (n = 0; *q; n++, q++)
    {
	/* SCO compiler needs the cast */
	p[n] = (char *) fl_rm_rcs_kw(*q);
	if (!g)
	    fprintf(stderr, (n == 0) ? "%s" : "%s\n", p[n]);
    }

    if (g)
    {
	if (n >= 3)
	    fl_show_message(p[0], p[1], p[2]);
	else if (n == 2)
	    fl_show_message(p[0], "", p[1]);
	else
	    fl_show_message("", p[0], "");
    }
}
