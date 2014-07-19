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
#include "counter_spec.h"

FD_counterattrib *create_form_counterattrib(void)
{
  FL_OBJECT *obj;
  FD_counterattrib *fdui = (FD_counterattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->counterattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,25,20,485,235,"Counter Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->prec = obj = fl_add_counter(FL_NORMAL_COUNTER,60,150,145,23,"Precision");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,cnt_precision_cb,0);
  fdui->minval = obj = fl_add_input(FL_FLOAT_INPUT,340,52,92,24,"Counter Min");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,cnt_minmax_change,0);
  fdui->maxval = obj = fl_add_input(FL_FLOAT_INPUT,340,87,92,24,"Counter Max");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,cnt_minmax_change,0);
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE2,61,70,138,24,"ReturnSetting");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,cnt_returnsetting_change,0);
  fdui->initialval = obj = fl_add_input(FL_FLOAT_INPUT,340,122,92,24,"Initial Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,cnt_initialvalue_change,0);
  fdui->step1 = obj = fl_add_input(FL_FLOAT_INPUT,340,157,92,24,"Sml.Step Size");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,cnt_stepchange_cb,0);
  fdui->step2 = obj = fl_add_input(FL_FLOAT_INPUT,340,192,92,24,"Lg. Step Size");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,cnt_stepchange_cb,1);
  fl_end_form();

  fl_adjust_form_size(fdui->counterattrib);
  fdui->counterattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

