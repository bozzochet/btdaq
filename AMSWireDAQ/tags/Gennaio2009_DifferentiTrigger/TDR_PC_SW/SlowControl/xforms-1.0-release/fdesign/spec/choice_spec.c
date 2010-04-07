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
#include "choice_spec.h"

FD_choiceattrib *create_form_choiceattrib(void)
{
  FL_OBJECT *obj;
  FD_choiceattrib *fdui = (FD_choiceattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->choiceattrib = fl_bgn_form(FL_NO_BOX, 520, 270);
  obj = fl_add_box(FL_FLAT_BOX,0,0,520,270,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,15,20,480,230,"Choice Attributes");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,25,80,190,120,"Item Attributes");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,225,82,111,140,"Action");
  fdui->content_br = obj = fl_add_browser(FL_HOLD_BROWSER,353,85,120,140,"CurrentItems:");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->input = obj = fl_add_input(FL_NORMAL_INPUT,35,107,170,25,"EditItem");
    fl_set_input_shortcut(obj,"#E",1);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,97,84,26,"Add");
    fl_set_button_shortcut(obj,"#A",1);
    fl_set_object_callback(obj,add_choice_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,158,84,24,"Delete");
    fl_set_button_shortcut(obj,"#D",1);
    fl_set_object_callback(obj,delete_choice_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,187,84,24,"Change");
    fl_set_button_shortcut(obj,"#C",1);
    fl_set_object_callback(obj,change_choice_item_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,115,207,82,24,"ClearField");
    fl_set_button_shortcut(obj,"#F",1);
    fl_set_object_callback(obj,clear_choice_field_cb,0);
  fdui->auto_clear = obj = fl_add_checkbutton(FL_PUSH_BUTTON,30,205,24,27,"AutoClear");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_button(obj, 1);
  obj = fl_add_button(FL_NORMAL_BUTTON,235,127,84,26,"Replace");
    fl_set_button_shortcut(obj,"#R",1);
    fl_set_object_callback(obj,replace_choice_item_cb,0);
  fdui->mode = obj = fl_add_choice(FL_NORMAL_CHOICE2,106,155,98,25,"Mode");
    fl_set_object_shortcut(obj,"#M",1);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->shortcut = obj = fl_add_input(FL_NORMAL_INPUT,35,155,70,25,"Shortcut");
    fl_set_input_shortcut(obj,"#S",1);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->align = obj = fl_add_choice(FL_NORMAL_CHOICE2,45,39,150,24,"");
    fl_set_object_callback(obj,choice_align_cb,0);
  fdui->val = obj = fl_add_counter(FL_NORMAL_COUNTER,365,42,100,18,"Initial Choice");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,choice_val_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_value(obj, 1);
    fl_set_counter_step(obj, 1, 2);
  fdui->new_menuapi = obj = fl_add_checkbutton(FL_PUSH_BUTTON,210,39,70,25,"Use Struct");
    fl_set_button_shortcut(obj,"#U",1);
    fl_set_object_callback(obj,use_struct_cb,0);
  fdui->scope = obj = fl_add_choice(FL_NORMAL_CHOICE2,290,40,65,22,"");
    fl_set_object_callback(obj,choiceentry_scope_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->choiceattrib);
  fdui->choiceattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

