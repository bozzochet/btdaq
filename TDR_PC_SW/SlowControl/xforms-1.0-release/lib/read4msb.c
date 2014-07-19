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


/***********************************************************************
 * $Id: read4msb.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *   Read 4bytes MSB first
 ***********************************************************************/
#if !defined(lint) && defined(F_ID)
char *id_4msb = "$Id: read4msb.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include "forms.h"
#include "ulib.h"

int
fl_fget4MSBF(FILE * fp)
{
    register int ret;
    ret = getc(fp);
    ret = (ret << 8) | getc(fp);
    ret = (ret << 8) | getc(fp);
    ret = (ret << 8) | getc(fp);
    return ret;
}

int
fl_fput4MSBF(int n, FILE *fp)
{
     putc((n>>24)&0xff,fp); 
     putc((n>>16)&0xff,fp); 
     putc((n>>8)&0xff,fp); 
     putc(n&0xff,fp); 
     return n;
}

