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
#include "ui_attrib.h"

FD_attrib *create_form_attrib(void)
{
  FL_OBJECT *obj;
  FD_attrib *fdui = (FD_attrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->attrib = fl_bgn_form(FL_NO_BOX, 630, 310);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,630,310,"");
  obj = fl_add_box(FL_UP_BOX,541,0,89,310,"");
  fdui->readyobj = obj = fl_add_button(FL_RETURN_BUTTON,550,85,70,24,"Accept");
  fdui->cancelobj = obj = fl_add_button(FL_NORMAL_BUTTON,550,109,70,24,"Cancel");
    fl_set_button_shortcut(obj,"#C^[",1);
  fdui->applyobj = obj = fl_add_button(FL_NORMAL_BUTTON,550,133,70,24,"Apply");
    fl_set_button_shortcut(obj,"#A",1);
    fl_set_object_callback(obj,apply_cb,0);
  fdui->restoreobj = obj = fl_add_button(FL_NORMAL_BUTTON,550,158,70,24,"   Restore   ");
    fl_set_button_shortcut(obj,"#R",1);
    fl_set_object_callback(obj,restore_cb,0);
  fdui->autoapplyobj = obj = fl_add_checkbutton(FL_PUSH_BUTTON,545,25,25,30,"AutoApply");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_callback(obj,autoapply_setting_cb,0);
  fdui->attrib_folder = obj = fl_add_tabfolder(FL_TOP_TABFOLDER,6,5,531,299,"");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_resize(obj, FL_RESIZE_ALL);
    fl_set_object_callback(obj,folder_switch_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->attrib);
  fdui->attrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_generic_attrib *create_form_generic_attrib(void)
{
  FL_OBJECT *obj;
  FD_generic_attrib *fdui = (FD_generic_attrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->generic_attrib = fl_bgn_form(FL_NO_BOX, 534, 278);
  fdui->background = obj = fl_add_box(FL_FLAT_BOX,0,0,534,278,"");

  fdui->generic_group = fl_bgn_group();
  fdui->text[2] = obj = fl_add_labelframe(FL_ENGRAVED_FRAME,235,120,185,137,"Misc.");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->text[1] = obj = fl_add_labelframe(FL_ENGRAVED_FRAME,235,15,185,95,"Font");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->text[0] = obj = fl_add_labelframe(FL_ENGRAVED_FRAME,10,15,212,242,"Basic Attributes");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->typeobj = obj = fl_add_choice(FL_NORMAL_CHOICE,69,31,143,25,"Type");
    fl_set_object_shortcut(obj,"#T",1);
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->boxobj = obj = fl_add_choice(FL_NORMAL_CHOICE,69,61,143,25,"BoxType");
    fl_set_object_shortcut(obj,"#B",1);
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->nameobj = obj = fl_add_input(FL_NORMAL_INPUT,69,121,143,25,"Name");
    fl_set_input_shortcut(obj,"#N",1);
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
    fl_set_object_callback(obj,validate_cvar_name_cb,0);
  fdui->cbnameobj = obj = fl_add_input(FL_NORMAL_INPUT,69,151,143,25,"Callback");
    fl_set_input_shortcut(obj,"#k",1);
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
    fl_set_object_callback(obj,validate_cvar_name_cb,0);
  fdui->argobj = obj = fl_add_input(FL_NORMAL_INPUT,69,181,143,25,"Argument");
    fl_set_input_shortcut(obj,"#u",1);
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
  fdui->scobj = obj = fl_add_input(FL_NORMAL_INPUT,69,211,143,25,"Shortcut");
    fl_set_input_shortcut(obj,"#S",1);
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->labelobj = obj = fl_add_input(FL_NORMAL_INPUT,69,91,143,25,"Label");
    fl_set_input_shortcut(obj,"#L",1);
    fl_set_object_color(obj,FL_COL1,FL_TOP_BCOL);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->fontobj = obj = fl_add_choice(FL_NORMAL_CHOICE,278,24,131,25,"Font");
    fl_set_object_shortcut(obj,"#F",1);
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->styleobj = obj = fl_add_choice(FL_NORMAL_CHOICE,278,51,131,25,"Style");
    fl_set_object_shortcut(obj,"#S",1);
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->sizeobj = obj = fl_add_choice(FL_NORMAL_CHOICE,278,78,131,25,"Size");
    fl_set_object_shortcut(obj,"#z",1);
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->lcolobj = obj = fl_add_button(FL_NORMAL_BUTTON,440,191,70,20,"LabelColor");
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,setcolor_cb,0);
  fdui->col1obj = obj = fl_add_button(FL_NORMAL_BUTTON,440,111,70,20,"Color1");
    fl_set_button_shortcut(obj,"#1",1);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,setcolor_cb,1);
  fdui->col2obj = obj = fl_add_button(FL_NORMAL_BUTTON,440,151,70,20,"Color 2");
    fl_set_button_shortcut(obj,"#2",1);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,setcolor_cb,2);
  fdui->align = obj = fl_add_choice(FL_NORMAL_CHOICE,305,130,108,23,"LabelAlign");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->nwgravity = obj = fl_add_choice(FL_NORMAL_CHOICE,305,182,108,23,"NW Gravity");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
  fdui->segravity = obj = fl_add_choice(FL_NORMAL_CHOICE,305,206,108,23,"SE Gravity");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
  fdui->inside = obj = fl_add_choice(FL_NORMAL_CHOICE,305,154,108,23,"In/Out");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_callback(obj,auto_apply_cb,0);
  fdui->resize = obj = fl_add_choice(FL_NORMAL_CHOICE,305,230,108,23,"Resize");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
  fdui->text[3] = obj = fl_add_labelframe(FL_ENGRAVED_FRAME,431,80,89,150,"Color");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fl_end_group();

  fl_end_form();

  fl_adjust_form_size(fdui->generic_attrib);
  fdui->generic_attrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_nullattrib *create_form_nullattrib(void)
{
  FL_OBJECT *obj;
  FD_nullattrib *fdui = (FD_nullattrib *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->nullattrib = fl_bgn_form(FL_NO_BOX, 540, 280);
  obj = fl_add_box(FL_FLAT_BOX,0,0,540,280,"");
  obj = fl_add_box(FL_UP_BOX,90,100,350,50,"Not Implemented (or not interesting)");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  fl_end_form();

  fl_adjust_form_size(fdui->nullattrib);
  fdui->nullattrib->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

