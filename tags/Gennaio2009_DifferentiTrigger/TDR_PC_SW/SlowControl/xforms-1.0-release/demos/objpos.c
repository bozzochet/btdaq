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
 * This is a crazy demo showing the "use" of changing
 * fields in objects.
 *
 *  T.C. Zhao and M. Overmars
 */

#include "forms.h"

void
move_cb(FL_OBJECT *ob, long data)
{
   static FL_Coord dx=11, dy=7;
   FL_OBJECT *but = (FL_OBJECT *)data;
   FL_Coord x,y,w,h; 

   fl_get_object_geometry(but,&x,&y, &w, &h);

   if( (x + dx) < 0 || (x+w+dx) >= but->form->w)
       dx = -dx;
   if( (y + dy) < 0 || (y+h+dy) >= but->form->h)
       dy = -dy;
    x += dx;
    y += dy;

    fl_set_object_position(but,x,y);
}

int
main(int argc, char *argv[])
{
   FL_FORM *form;
   FL_OBJECT *but, *obj;

   fl_initialize(&argc, argv, "FormDemo", 0, 0);

   form = fl_bgn_form(FL_DOWN_BOX,400,200);
     but = fl_add_button(FL_NORMAL_BUTTON,140,160,70,35,"Exit");
     obj = fl_add_button(FL_TOUCH_BUTTON,330,150,50,30,"Move");
      fl_set_object_callback(obj,move_cb,(long)but);
   fl_end_form();

   fl_show_form(form,FL_PLACE_MOUSE,FL_NOBORDER,"ObjPos");

   fl_do_forms();

   return 0;
}
