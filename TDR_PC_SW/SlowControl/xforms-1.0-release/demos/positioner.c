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


/* This demo shows the use of a positioner.  */

#include <stdio.h>
#include "forms.h"

FL_OBJECT *xval, *yval;

/* callback routine */
void
positioner_cb(FL_OBJECT *ob, long q)
{
    char str[30];
    sprintf(str,"%f",fl_get_positioner_xvalue(ob));
    fl_set_object_label(xval,str);
    sprintf(str,"%f",fl_get_positioner_yvalue(ob));
    fl_set_object_label(yval,str);
}

int
main(int argc, char *argv[])
{
  FL_FORM *form;
  FL_OBJECT *pos;

  fl_initialize(&argc, argv, "FormDemo", 0, 0);

  form = fl_bgn_form(FL_UP_BOX,400,280);
     pos = fl_add_positioner(FL_NORMAL_POSITIONER,40,40,200,200,"");
     fl_set_positioner_xbounds(pos,0,1);
     fl_set_positioner_ybounds(pos,0,1);
     fl_set_object_callback(pos,positioner_cb,0);
     xval = fl_add_box(FL_DOWN_BOX,270,40,100,30,"");
     yval = fl_add_box(FL_DOWN_BOX,270,90,100,30,"");
     fl_set_object_color(xval,FL_COL1,FL_COL1);
     fl_set_object_color(yval,FL_COL1,FL_COL1);
     fl_add_button(FL_NORMAL_BUTTON,270,210,100,30,"Exit");
  fl_end_form();

  fl_show_form(form,FL_PLACE_CENTER,FL_NOBORDER,"positioner");

  positioner_cb(pos,0);

  fl_do_forms();
  fl_hide_form(form);
  return 0;
}
