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
#include "butttypes_gui.h"

FD_form0 *create_form_form0(void)
{
  FL_OBJECT *obj;
  FD_form0 *fdui = (FD_form0 *) fl_calloc(1, sizeof(*fdui));

  fdui->form0 = fl_bgn_form(FL_NO_BOX, 360, 300);
  obj = fl_add_box(FL_UP_BOX,0,0,360,300,"");
  obj = fl_add_button(FL_HIDDEN_BUTTON,0,0,360,300,"");
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_INOUT_BUTTON,18,168,107,30,"InOutButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,18,15,107,30,"NormalButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_PUSH_BUTTON,18,53,107,30,"PushButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_TOUCH_BUTTON,18,130,107,30,"TouchButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_MENU_BUTTON,18,206,107,30,"MenuButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_RETURN_BUTTON,18,244,107,30,"ReturnButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  obj = fl_add_button(FL_RADIO_BUTTON,18,91,107,30,"RadioButton");
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_callback(obj,button_cb,0);
  fdui->br = obj = fl_add_browser(FL_NORMAL_BROWSER,135,15,210,260,"");
    fl_set_object_callback(obj,button_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->form0);
  fdui->form0->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

