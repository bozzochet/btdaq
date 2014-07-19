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


/* Form definition file generated with fdesign. */

#include "forms.h"
#include <stdlib.h>
#include "inputall_gui.h"

FD_input *create_form_input(void)
{
  FL_OBJECT *obj;
  FD_input *fdui = (FD_input *) fl_calloc(1, sizeof(*fdui));

  fdui->input = fl_bgn_form(FL_NO_BOX, 441, 441);
  obj = fl_add_box(FL_UP_BOX,0,0,441,441,"");
  fdui->norminput = obj = fl_add_input(FL_NORMAL_INPUT,40,40,340,30,"NormalInput");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_callback(obj,input_cb,0);
  obj = fl_add_input(FL_INT_INPUT,40,100,160,30,"IntInput");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  obj = fl_add_input(FL_FLOAT_INPUT,230,100,160,30,"FloatInput");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  obj = fl_add_input(FL_DATE_INPUT,40,150,160,30,"DateInput");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  obj = fl_add_input(FL_SECRET_INPUT,230,150,160,30,"Secretnput");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->multiinput = obj = fl_add_input(FL_MULTILINE_INPUT,40,210,360,180,"MMM");
    fl_set_object_callback(obj,input_cb,0);
  fdui->report = obj = fl_add_text(FL_NORMAL_TEXT,30,400,210,30,"");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_button(FL_NORMAL_BUTTON,330,400,70,30,"Done");
    fl_set_object_callback(obj,done_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,250,400,70,30,"Hide/Show");
    fl_set_object_callback(obj,hide_show_cb,0);
  fl_end_form();
  fdui->input->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

