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
#include "free_spec.h"

FD_buttonattrib *create_form_buttonattrib(void)
{
  FL_OBJECT *obj;
  FD_buttonattrib *fdui = (FD_buttonattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->buttonattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,20,20,490,240,"Button Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  fdui->initialval = obj = fl_add_checkbutton(FL_PUSH_BUTTON,170,55,30,30,"Initially Set");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,initialval_change,0);
  fdui->filename = obj = fl_add_input(FL_NORMAL_INPUT,160,113,210,25,"Bitmap/Pixmap File");
    fl_set_object_callback(obj,iconbutton_filename_change,0);
  fdui->pixalign = obj = fl_add_choice(FL_NORMAL_CHOICE2,282,60,155,25,"");
    fl_set_object_callback(obj,pixalign_change,0);
  fdui->showfocus = obj = fl_add_checkbutton(FL_PUSH_BUTTON,170,80,30,30,"Show Focus");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,showfocus_change,0);
    fl_set_button(obj, 1);
  fdui->browse = obj = fl_add_button(FL_NORMAL_BUTTON,372,113,66,25,"Browse ....");
    fl_set_button_shortcut(obj,"#B",1);
    fl_set_object_callback(obj,lookfor_pixmapfile_cb,0);
  fdui->use_data = obj = fl_add_checkbutton(FL_PUSH_BUTTON,175,155,30,30,"Use Data");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,usedata_change,0);
  fdui->fullpath = obj = fl_add_checkbutton(FL_PUSH_BUTTON,275,155,30,30,"Full Path");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,fullpath_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->buttonattrib);
  fdui->buttonattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

