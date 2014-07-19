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
#include "scrollbar_spec.h"

FD_scrollbarattrib *create_form_scrollbarattrib(void)
{
  FL_OBJECT *obj;
  FD_scrollbarattrib *fdui = (FD_scrollbarattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->scrollbarattrib = fl_bgn_form(FL_NO_BOX, 525, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,525,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,20,15,470,245,"Scrollbar Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->minval = obj = fl_add_input(FL_FLOAT_INPUT,190,35,82,25,"Value at bottom/left");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_minmax_change,0);
  fdui->maxval = obj = fl_add_input(FL_FLOAT_INPUT,190,64,82,25,"Value at top/right");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_minmax_change,0);
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE2,310,110,143,24,"ReturnSetting");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,scb_returnsetting_change,0);
  fdui->initial_val = obj = fl_add_input(FL_FLOAT_INPUT,190,94,82,25,"Initial Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_initialvalue_change,0);
  fdui->slsize = obj = fl_add_input(FL_FLOAT_INPUT,190,123,82,26,"Scrollbar Size");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_slsize_change,0);
  fdui->step = obj = fl_add_input(FL_FLOAT_INPUT,190,154,82,27,"Step Size");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_step_change,0);
  fdui->ldelta = obj = fl_add_input(FL_FLOAT_INPUT,191,185,82,25,"Leftmouse  Increment");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_delta_change,0);
  fdui->rdelta = obj = fl_add_input(FL_FLOAT_INPUT,191,215,82,25,"Rightmouse Increment");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,scb_delta_change,0);
  fl_end_form();

  fl_adjust_form_size(fdui->scrollbarattrib);
  fdui->scrollbarattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

