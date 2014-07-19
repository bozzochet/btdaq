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
#include "dial_spec.h"

FD_dialattrib *create_form_dialattrib(void)
{
  FL_OBJECT *obj;
  FD_dialattrib *fdui = (FD_dialattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->dialattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE,305,90,138,24,"ReturnSetting");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,dial_returnsetting_change,0);
  fdui->minval = obj = fl_add_input(FL_FLOAT_INPUT,195,45,92,24,"Dial Min");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_minmax_change,0);
  fdui->maxval = obj = fl_add_input(FL_FLOAT_INPUT,195,77,92,24,"DialMax");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_minmax_change,0);
  fdui->initialval = obj = fl_add_input(FL_FLOAT_INPUT,195,109,92,24,"Initial Value");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_initialvalue_change,0);
  fdui->step = obj = fl_add_input(FL_FLOAT_INPUT,195,141,92,24,"Dial Step");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_stepchange_cb,0);
  fdui->thetai = obj = fl_add_input(FL_FLOAT_INPUT,195,173,92,24,"Min. Angle");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_thetachange_cb,0);
  fdui->thetaf = obj = fl_add_input(FL_FLOAT_INPUT,195,205,92,24,"Max. Angle");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_callback(obj,dial_thetachange_cb,1);
  fdui->dir = obj = fl_add_choice(FL_NORMAL_CHOICE2,306,155,134,25,"Direction");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,dir_cb,0);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,30,15,480,240,"Dial Attributes");
    fl_set_object_lcolor(obj,FL_DARKVIOLET);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE+FL_EMBOSSED_STYLE);
  fl_end_form();

  fdui->dialattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

