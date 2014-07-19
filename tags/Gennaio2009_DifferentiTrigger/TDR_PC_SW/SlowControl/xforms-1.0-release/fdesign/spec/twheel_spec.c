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
#include "twheel_spec.h"

FD_twheelattrib *create_form_twheelattrib(void)
{
  FL_OBJECT *obj;
  FD_twheelattrib *fdui = (FD_twheelattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->twheelattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,30,20,470,240,"Thumbwheel Attributes");
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  fdui->prec = obj = fl_add_counter(FL_NORMAL_COUNTER,330,145,114,23,"Precision");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,twheel_adjust_precision,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 0, 0);
    fl_set_counter_step(obj, 0, 0);
  fdui->minval = obj = fl_add_input(FL_FLOAT_INPUT,210,80,82,25,"Minimum Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,twheel_minmax_change,0);
  fdui->maxval = obj = fl_add_input(FL_FLOAT_INPUT,210,111,82,25,"Maximum Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,twheel_minmax_change,0);
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE2,320,90,143,24,"ReturnSetting");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,twheel_returnsetting_change,0);
  fdui->initial_val = obj = fl_add_input(FL_FLOAT_INPUT,210,143,82,25,"Initial Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,twheel_initialvalue_change,0);
  fdui->step = obj = fl_add_input(FL_FLOAT_INPUT,210,175,82,25,"StepSize");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,twheel_step_change,0);
  fl_end_form();

  fl_adjust_form_size(fdui->twheelattrib);
  fdui->twheelattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

