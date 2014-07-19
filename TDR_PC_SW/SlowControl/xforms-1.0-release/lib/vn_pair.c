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
 * $Id$
 *
 * Name value pair stuff
 */


#include "forms.h"
#include "ulib.h"
#include <stdlib.h>

int
fl_get_vn_value(register FL_VN_PAIR * vn_pair, const char *name)
{
    for (; vn_pair->val >= 0; vn_pair++)
        if (strcmp(vn_pair->name, name) == 0)
            return vn_pair->val;
    /* not found. Take a guess */
    return atoi(name);
}

const char *
fl_get_vn_name(register FL_VN_PAIR * vn_pair, int val)
{
    static char buf[5][16];
    static int k;

    k = ( k+1)%5;
    for (; vn_pair->val >= 0; vn_pair++)
        if (vn_pair->val == val)
            return vn_pair->name;
    sprintf(buf[k], "%d", val);
    return buf[k];
}
