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
#include "pixmap_spec.h"

FD_pixmapattrib *create_form_pixmapattrib(void)
{
  FL_OBJECT *obj;
  FD_pixmapattrib *fdui = (FD_pixmapattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->pixmapattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,20,20,490,240,"Pixmap Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_TIMESBOLD_STYLE);
  fdui->filename = obj = fl_add_input(FL_NORMAL_INPUT,160,113,210,25,"Bitmap/Pixmap File");
    fl_set_object_callback(obj,pixmap_filename_change,0);
  fdui->pixalign = obj = fl_add_choice(FL_NORMAL_CHOICE2,282,60,155,25,"");
    fl_set_object_callback(obj,pixmapalign_change,0);
  fdui->browse = obj = fl_add_button(FL_NORMAL_BUTTON,372,113,66,25,"Browse ....");
    fl_set_button_shortcut(obj,"#B",1);
    fl_set_object_callback(obj,pixmaplookfor_pixmapfile_cb,0);
  fdui->use_data = obj = fl_add_checkbutton(FL_PUSH_BUTTON,175,155,30,30,"Use Data");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,pixmapusedata_change,0);
  fdui->fullpath = obj = fl_add_checkbutton(FL_PUSH_BUTTON,275,155,30,30,"Full Path");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT);
    fl_set_object_callback(obj,pixmapfullpath_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->pixmapattrib);
  fdui->pixmapattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

