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
 * Routines implementing the "crossbutton" class
 *
 *  ob->col1 is the color of the box. ob->col2 is the color of cross
 *
 *  This file is part of xforms package
 *  T.C. Zhao and M. Overmars
 */

#include "forms.h"
#include "crossbut.h"

/** How to draw it */
typedef FL_BUTTON_STRUCT SPEC;

static void
draw_crossbutton(FL_OBJECT *ob)
{
    FL_Coord xx, yy, ww, hh;

   /* if redraw is demanded by FL_ENTER, ignore it */

   if(((SPEC *)ob->spec)->event == FL_ENTER)
      return;

   /* draw the bounding box first */
   fl_drw_box(ob->boxtype, ob->x, ob->y, ob->w, ob->h, ob->col1, ob->bw);

   /* draw the box that contains the cross */
    ww = hh = (int)(0.5 * FL_min(ob->w, ob->h))  -1;
    xx = ob->x + FL_BOUND_WIDTH;
    yy = ob->y + (ob->h - hh)/2;

   /* if pushed, draw a down box with the cross */

   if(((SPEC *)ob->spec)->val)
   {
      fl_drw_box(FL_DOWN_BOX, xx, yy, ww, hh, ob->col1, ob->bw);
      fl_drw_text(FL_ALIGN_CENTER, xx-2, yy-2, ww+4, hh+4, ob->col2, 0, 0, "@9plus");
   }
   else
   {
      fl_drw_box(FL_UP_BOX, xx, yy, ww, hh, ob->col1, ob->bw);
   }

    /* label */
    if (ob->align == FL_ALIGN_CENTER)
	fl_drw_text(FL_ALIGN_LEFT, xx + ww + 3, ob->y, 0, ob->h,
		    ob->lcol, ob->lstyle, ob->lsize, ob->label);
    else
        fl_draw_object_label_outside(ob);

    if (ob->type == FL_RETURN_BUTTON)
	fl_drw_text(FL_ALIGN_CENTER,
		    (FL_Coord) (ob->x + ob->w - 0.8 * ob->h),
		    (FL_Coord) (ob->y + 0.2 * ob->h),
		    (FL_Coord) (0.6 * ob->h),
                    (FL_Coord) (0.6 * ob->h), ob->lcol, 0, 0, "@returnarrow");
}


/* creation */
FL_OBJECT *
fl_create_crossbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w,
                      FL_Coord h, const char *label)
{
     FL_OBJECT *ob;
     fl_add_button_class(FL_CROSSBUTTON, draw_crossbutton, 0); 
     ob = fl_create_generic_button(FL_CROSSBUTTON, type, x, y,w, h, label);
     ob->boxtype = FL_NO_BOX;
     ob->col2 = FL_BLACK;   /* cross color */
     return ob;
}

FL_OBJECT *
fl_add_crossbutton(int type, FL_Coord x, FL_Coord y, FL_Coord w,
                   FL_Coord h, const char *label)
{
    FL_OBJECT *ob = fl_create_crossbutton(type, x, y, w, h, label);
    fl_add_object(fl_current_form, ob);
    return ob;
}
