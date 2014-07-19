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
#include "browser_spec.h"

FD_browserattrib *create_form_browserattrib(void)
{
  FL_OBJECT *obj;
  FD_browserattrib *fdui = (FD_browserattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->browserattrib = fl_bgn_form(FL_NO_BOX, 520, 270);
  obj = fl_add_box(FL_FLAT_BOX,0,0,520,270,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,15,20,480,230,"Browser Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,225,57,111,140,"Action");
  fdui->content_br = obj = fl_add_browser(FL_HOLD_BROWSER,353,55,120,150,"CurrentItems:");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->input = obj = fl_add_input(FL_NORMAL_INPUT,35,67,170,25,"EditItem");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_callback(obj,add_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,72,84,26,"Add");
    fl_set_button_shortcut(obj,"#A",1);
    fl_set_object_callback(obj,add_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,133,84,24,"Delete");
    fl_set_button_shortcut(obj,"#D",1);
    fl_set_object_callback(obj,delete_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,162,84,24,"Insert");
    fl_set_button_shortcut(obj,"#I",1);
    fl_set_object_callback(obj,insert_cb,0);
  fdui->hscb_pref = obj = fl_add_choice(FL_NORMAL_CHOICE2,95,175,90,24,"");
    fl_set_object_shortcut(obj,"#H",1);
    fl_set_object_callback(obj,hscb_pref_cb,0);
  fdui->vscb_pref = obj = fl_add_choice(FL_NORMAL_CHOICE2,95,147,90,24,"");
    fl_set_object_shortcut(obj,"#v",1);
    fl_set_object_callback(obj,vscb_pref_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,120,101,82,24,"ClearField");
    fl_set_button_shortcut(obj,"#l",1);
    fl_set_object_callback(obj,clear_field_cb,0);
  fdui->auto_clear = obj = fl_add_checkbutton(FL_PUSH_BUTTON,35,99,24,27,"AutoClear");
    fl_set_button_shortcut(obj,"#u",1);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_button(obj, 1);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,102,84,26,"Replace");
    fl_set_button_shortcut(obj,"#R",1);
    fl_set_object_callback(obj,replace_item_cb,0);
  obj = fl_add_box(FL_NO_BOX,33,176,62,22,"H\010Scrollbar");
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  obj = fl_add_box(FL_NO_BOX,32,148,62,22,"V\010Scrollbar");
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT|FL_ALIGN_INSIDE);
  fl_end_form();

  fl_adjust_form_size(fdui->browserattrib);
  fdui->browserattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

