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
 * $Id: readint.c,v 0.80 1994/02/24 09:48:11 zhao Exp $
 *
 *.  Copyright(c) 1993,1994 by T.C. Zhao
 *   All rights reserved.
 *.
 *
 *   Read an integer (decimal or hex) from a file.
 *   Valid sepertors are  SPACE, \t \n  and ,
 *   Comment is introduced by #
 *
 * For hex integers, a-f, A-F is assumed continous
 ***********************************************************************/
#if !defined(lint) && defined(F_ID)
char *id_rdint = "$Id: readint.c,v 0.80 1994/02/24 09:48:11 zhao Exp $";
#endif

#include <stdio.h>
#include <ctype.h>		/* for isdigit */
#include "forms.h"
#include "ulib.h"

#define IS_FS(c)      ((c)==' ' || (c)=='\t' || (c)=='\n' || (c)==',' )
#define IS_COMMENT(c) ((c) == '#')

static int yell = 0;

static void
bad_character(int c)
{
    if (yell && c != EOF)
	(void) fprintf(stderr, "Bad character %c Code=%d\n", c, c);
}

static int
skip_comment(FILE * fp)
{
    register int c;
    while ((c = getc(fp)) != EOF && c != '\n')
	;
    return (c != EOF) ? getc(fp) : EOF;
}

/* read an integer [+-]nnn. No way to return error status */
int
fl_readint(FILE * fp)
{
    register int c, num = 0, sign = 1;

    do
    {
	c = getc(fp);
	while (IS_COMMENT(c))
	    c = skip_comment(fp);
    }
    while (IS_FS(c));

    if (c == '-' || c == '+')
    {
	sign = (c == '-') ? -1 : 1;
	c = getc(fp);
    }

    while (isdigit(c))
    {
	num = 10 * num + c - '0';
	c = getc(fp);
    }

    if (!IS_FS(c))
    {
	bad_character(c);
	num = 123456;
    }

    return sign * num;
}

/* Read a positive integer. return EOF if error */
int
fl_readpint(FILE * fp)
{
    register int c, num = 0;

    do
    {
	c = getc(fp);
	while (IS_COMMENT(c))
	    c = skip_comment(fp);
    }
    while (IS_FS(c));

    if (!(c == '+' || isdigit(c)))
    {
	bad_character(c);
	return EOF;
    }

    do
    {
	num = 10 * num + c - '0';
	c = getc(fp);
    }
    while (isdigit(c));

    return num;
}

/*  Read a hex integer */
int
fl_readhexint(FILE * fp)
{
    register int num = 0, i, c;
    static short hextab[256];

    /* initialize the hex table */
    if (!hextab['1'])
    {
	for (i = '1'; i <= '9'; i++)
	    hextab[i] = i - '0';
	for (i = 'A'; i <= 'F'; i++)
	    hextab[i] = 10 + i - 'A';
	for (i = 'a'; i <= 'f'; i++)
	    hextab[i] = 10 + i - 'a';
    }

    do
    {
	c = getc(fp);
	while (IS_COMMENT(c))
	    c = skip_comment(fp);
    }
    while (IS_FS(c));

    /* demand  0[xX] */
    if (c != '0' || ((c = getc(fp)) != 'x' && c != 'X'))
    {
	bad_character(c);
	return EOF;
    }

    /* now do the coversion */
    while ((c = getc(fp)), isxdigit(c))
	num = (num << 4) + hextab[c];
    return num;
}
