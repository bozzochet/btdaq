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

#include <stdio.h>
#include "forms.h"
#include "ulib.h"

int
fl_fget4LSBF(FILE * fp)
{
    register int ret;
    ret = getc(fp);
    ret |= getc(fp) << 8;
    ret |= getc(fp) << 16;
    ret |= getc(fp) << 24;
    return ret;
}


int
fl_fput4LSBF(register int code, FILE * fp)
{
    putc(code & 0xff, fp);
    putc((code >> 8) & 0xff, fp);
    putc((code >> 16) & 0xff, fp);
    return putc((code >> 24) & 0xff, fp);
}
