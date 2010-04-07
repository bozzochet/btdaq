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
 * $Id: sys.h,v 0.36 1999/07/17 03:32:50 zhao beta $
 *
 *  system dependent stuff
 *
 */

#ifndef SYS_H
#define SYS_H

#define FL_SIGRET void

#if defined(__ultrix)||defined(ultrix)||defined(linux) || defined(__linux__)
#define NO_STRDUP
#endif

/* unixware and Lynx do not have strcasecmp */
#if defined(__USLC__) || defined(Lynx)
#define NO_STRCASECMP
#endif

#ifdef NO_STRCASECMP
extern int strcasecmp(const char *, const char *);
#endif

#endif /* sys.h */
