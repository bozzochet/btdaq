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
#include "positioner_spec.h"

FD_posattrib *create_form_posattrib(void)
{
  FL_OBJECT *obj;
  FD_posattrib *fdui = (FD_posattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->posattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,30,20,475,235,"Positioner Attributes");
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE2,195,55,138,24,"ReturnSetting");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,pos_returnsetting_change,0);
  fdui->xminval = obj = fl_add_input(FL_FLOAT_INPUT,170,95,92,24,"Xmin(left)");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,pos_xminmax_change,0);
  fdui->xmaxval = obj = fl_add_input(FL_FLOAT_INPUT,170,131,92,24,"Xmax(right)");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,pos_xminmax_change,0);
  fdui->initialxval = obj = fl_add_input(FL_FLOAT_INPUT,170,168,92,24,"Initial XValue");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,pos_initialxvalue_change,0);
  fdui->xstep = obj = fl_add_input(FL_FLOAT_INPUT,170,205,92,24,"X StepSize");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,pos_xstepchange_cb,0);
  fdui->yminval = obj = fl_add_input(FL_FLOAT_INPUT,275,95,92,24,"Ymin(bottom)");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,pos_yminmax_change,0);
  fdui->ymaxval = obj = fl_add_input(FL_FLOAT_INPUT,275,131,92,24,"Ymax(top)");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,pos_yminmax_change,0);
  fdui->initialyval = obj = fl_add_input(FL_FLOAT_INPUT,275,168,92,24,"Initial YValue");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,pos_initialyvalue_change,0);
  fdui->ystep = obj = fl_add_input(FL_FLOAT_INPUT,275,205,92,24,"YStepSize");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,pos_ystepchange_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->posattrib);
  fdui->posattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

