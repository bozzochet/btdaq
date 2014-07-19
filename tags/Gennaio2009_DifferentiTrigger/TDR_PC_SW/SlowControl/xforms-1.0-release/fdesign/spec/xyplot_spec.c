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
#include "xyplot_spec.h"

FD_xyplotattrib *create_form_xyplotattrib(void)
{
  FL_OBJECT *obj;
  FD_xyplotattrib *fdui = (FD_xyplotattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->xyplotattrib = fl_bgn_form(FL_NO_BOX, 520, 270);
  obj = fl_add_box(FL_FLAT_BOX,0,0,520,270,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,10,20,500,240,"XYPlot Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->xgrid = obj = fl_add_choice(FL_NORMAL_CHOICE2,70,60,85,23,"XGrid");
    fl_set_object_callback(obj,grid_change_cb,0);
  fdui->ygrid = obj = fl_add_choice(FL_NORMAL_CHOICE2,70,90,85,23,"YGrid");
    fl_set_object_callback(obj,grid_change_cb,0);
  fdui->xscale = obj = fl_add_choice(FL_NORMAL_CHOICE2,70,170,85,23,"XScale");
    fl_set_object_callback(obj,xscale_change_cb,0);
  fdui->yscale = obj = fl_add_choice(FL_NORMAL_CHOICE2,70,210,85,23,"YScale");
    fl_set_object_callback(obj,yscale_change_cb,0);
  fdui->ymajor = obj = fl_add_counter(FL_NORMAL_COUNTER,370,160,115,20,"YMajor");
    fl_set_object_callback(obj,ymajorminor_change_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, -1, 20);
    fl_set_counter_value(obj, 1);
    fl_set_counter_step(obj, 1, 2);
  fdui->yminor = obj = fl_add_counter(FL_NORMAL_COUNTER,370,200,115,20,"YMinor");
    fl_set_object_callback(obj,ymajorminor_change_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 0, 10);
    fl_set_counter_value(obj, 1);
    fl_set_counter_step(obj, 1, 2);
  fdui->xmajor = obj = fl_add_counter(FL_NORMAL_COUNTER,370,70,115,20,"XMajor");
    fl_set_object_callback(obj,xmajorminor_change_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, -1, 20);
    fl_set_counter_value(obj, 1);
    fl_set_counter_step(obj, 1, 2);
  fdui->xminor = obj = fl_add_counter(FL_NORMAL_COUNTER,370,110,115,20,"XMinor");
    fl_set_object_callback(obj,xmajorminor_change_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 0, 10);
    fl_set_counter_value(obj, 1);
    fl_set_counter_step(obj, 1, 2);
  fdui->xbase = obj = fl_add_input(FL_FLOAT_INPUT,160,170,39,23,"xbase");
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,xscale_change_cb,0);
  fdui->ybase = obj = fl_add_input(FL_FLOAT_INPUT,158,210,39,23,"ybase");
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,yscale_change_cb,0);
  fdui->how_return = obj = fl_add_choice(FL_NORMAL_CHOICE2,193,29,150,22,"");
    fl_set_object_callback(obj,xyplot_returnsetting_change,0);
  fdui->gridstyle = obj = fl_add_choice(FL_NORMAL_CHOICE2,70,120,85,23,"Style");
    fl_set_object_callback(obj,gridstyle_change_cb,0);
  fdui->mark_active = obj = fl_add_checkbutton(FL_PUSH_BUTTON,230,80,100,25,"MarkActive");
    fl_set_object_callback(obj,markactive_change_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->xyplotattrib);
  fdui->xyplotattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

