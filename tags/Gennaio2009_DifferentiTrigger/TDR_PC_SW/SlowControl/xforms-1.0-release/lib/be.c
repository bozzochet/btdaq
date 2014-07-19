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
 * $Id: be.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2000  T.C. Zhao 
 *  Copyright (c) 2002 T.C. Zhao and Steve Lamont
 *  All rights reserved.
 *.
 *
 *
 * Things need to be done between events: free stuff that can't
 * be free'ed right away, signals etc.
 */

#include "forms.h"
#include <stdlib.h>

void
fl_addto_freelist(void *data)
{
    FL_FREE_REC *rec = fl_context->free_rec;
    int n;

    if (!data)
	return;

    if (!rec)
    {
	rec = fl_context->free_rec = fl_calloc(1, sizeof(*rec));
	rec->avail = 10;
	rec->data = fl_calloc(rec->avail, sizeof(void *));
        rec->age = fl_malloc(rec->avail * sizeof *rec->age);
    }

    /* look for an open slot */

    for ( n = 0; n < rec->avail && rec->data[n]; n++)
            ;

    if (n  == rec->avail )
    {
        int osize = rec->avail;
	rec->avail *= 2;
	rec->data = fl_realloc(rec->data, rec->avail * sizeof(void *));
        rec->age = fl_realloc(rec->age, rec->avail * sizeof *rec->age);
        memset(rec->data + osize, 0, sizeof *rec->data * osize);
    }

    rec->data[n] = data;
    rec->age[n] = 0;
    ++rec->nfree;
}

void
fl_free_freelist(void)
{
    FL_FREE_REC *rec;
    int n;

    if (!(rec = fl_context->free_rec) || !rec->nfree)
	return;

    /* scan the list for objects eligible for deletion and delete those
       found. 
     */
    for ( n = 0; n < rec->avail; n++)
    {
        if (rec->data[n] && (++rec->age[n] > 25))
        {
             fl_free(rec->data[n]);
             rec->data[n] = 0;
             --rec->nfree;
        }
    }         
}
