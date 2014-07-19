/* There has to be at least one line preceding the next one*/
/********************** crop here for forms.h **********************/
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
 *  forms.h   $Revision: 1.0 $ $State: Release $
 *            $Date: 2002/05/08 05:17:18 $ $Author: zhao $
 *
 *. All XForms files as distributed in this package are
 *  Copyright(c) 1996-2002 by T.C. Zhao and Mark Overmars,
 *  with part of the code Copyright (c) 1999-2002 by T.C. Zhao
 *  and Steve Lamont.
 *  ALL RIGHTS RESERVED.
 *
 * Permission to use, copy, and distribute this software in its entirety
 * without fee, is hereby granted, provided that the above copyright 
 * notice and this permission notice appear in all copies and their 
 * documentation.
 *
 * As of April 2002, xforms is released under the GNU LGPL license.
 * You can use xforms for any purpose that's compatible with
 * LGPL with the restriction that you will need a special license
 * for distributed  binary commercial software that requires or is
 * based on xforms or its derivative.
 *
 * This software is provided "as is" without expressed or implied
 * warranty of any kind.
 *
 * Contact tc_zhao@yahoo.com or xforms@world.std.com or visit
 * http://world.std.com/~xforms 
 * if you have questions about xforms.
 *
 * ******** This file is generated automatically. DO NOT CHANGE *********
 */

#ifndef FL_FORMS_H
#define FL_FORMS_H

#define FL_VERSION             1
#define FL_REVISION            0
#define FL_FIXLEVEL            0
#define FL_INCLUDE_VERSION    (FL_VERSION * 1000 + FL_REVISION)

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#if defined(__cplusplus)
extern "C"
{
#endif

#if defined(_WIN32)
#define FL_WIN32
#include <windows.h>
#endif

#if !defined(FL_WIN32) || !defined(SHARED_LIB)
#define FL_EXPORT extern
#else
#ifdef MAKING_FORMS
#define FL_EXPORT __declspec( dllexport ) extern
#else
#define FL_EXPORT __declspec( dllimport ) extern
#endif				/* MAKING_FORMS */
#endif				/* FL_WIN32 */
