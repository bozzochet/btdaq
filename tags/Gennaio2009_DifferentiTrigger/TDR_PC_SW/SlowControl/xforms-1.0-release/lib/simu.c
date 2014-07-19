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
 * $Id: simu.c,v 1.0 2002/05/08 05:16:30 zhao Release $
 *.
 *  This file is part of the XForms library package.
 *  Copyright (c) 1996-2002  T.C. Zhao and Mark Overmars
 *  All rights reserved.
 *.
 *
 * temporary routine. Eventually it will be implemented as a
 * real routine that manipulates memebers of obj
 */

#include "forms.h"


void
fl_set_object_return(FL_OBJECT * ob, int when)
{
    if (!ob)
	return;

    if (when < 0 || when > 5)
	when = 0;

    if (ob->objclass == FL_SLIDER || ob->objclass == FL_VALSLIDER ||
	ob->objclass == FL_THUMBWHEEL)
	fl_set_valuator_return(ob, when);
    else if (ob->objclass == FL_INPUT)
	fl_set_input_return(ob, when);
    else if (ob->objclass == FL_COUNTER)
	fl_set_counter_return(ob, when);
    else if (ob->objclass == FL_DIAL)
	fl_set_dial_return(ob, when);
    else if (ob->objclass == FL_POSITIONER)
	fl_set_positioner_return(ob, when);
    else if (ob->objclass == FL_XYPLOT)
	fl_xyplot_return(ob, when);
}

