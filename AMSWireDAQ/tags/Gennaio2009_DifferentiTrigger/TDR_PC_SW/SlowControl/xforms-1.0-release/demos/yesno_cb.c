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
 * same functionality as yesno.c, but with callbacks.
 */

#include <stdlib.h>
#include "forms.h"

void yes_push(FL_OBJECT *ob, long data)
{
  fprintf(stderr,"Yes is pushed\n");
  exit(1);
}

void no_push(FL_OBJECT *ob, long data)
{
  fprintf(stderr,"No is pushed\n");
}

int
main(int argc, char *argv[])
{
   FL_FORM *form;
   FL_OBJECT *obj;
  
  fl_initialize(&argc, argv, "FormDemo", 0, 0);
  form = fl_bgn_form(FL_UP_BOX,320,120);
    fl_add_box(FL_NO_BOX,160,40,0,0,"Do you want to Quit?");
    obj = fl_add_button(FL_NORMAL_BUTTON,40,70,80,30,"Yes");
     fl_set_object_callback(obj, yes_push, 0);
    obj  = fl_add_button(FL_NORMAL_BUTTON,200,70,80,30,"No");
     fl_set_object_callback(obj, no_push, 0);
  fl_end_form();

  fl_show_form(form,FL_PLACE_MOUSE,FL_TRANSIENT,"Question");

  fl_do_forms();

  fl_hide_form(form);
  return 0;
}
