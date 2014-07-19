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
#include "buttons_gui.h"

FD_buttform *create_form_buttform(void)
{
  FL_OBJECT *obj;
  FD_buttform *fdui = (FD_buttform *) fl_calloc(1, sizeof(*fdui));

  fdui->buttform = fl_bgn_form(FL_NO_BOX, 290, 260);
  fdui->backface = obj = fl_add_box(FL_UP_BOX,0,0,290,260,"");
  fdui->done = obj = fl_add_button(FL_NORMAL_BUTTON,185,215,90,30,"Done");
    fl_set_object_callback(obj,done_cb,0);

  fdui->objsgroup = fl_bgn_group();
  obj = fl_add_frame(FL_ENGRAVED_FRAME,175,170,100,30,"");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,210,170,30,30,"");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
  fdui->bbutt = obj = fl_add_bitmapbutton(FL_NORMAL_BUTTON,25,85,40,40,"bitmapbutton");
    fl_set_object_color(obj,FL_COL1,FL_BLACK);
  fdui->pbutt = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,25,25,40,40,"pixmapbutton");
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,100,31,70,32,"Red");
    fl_set_object_color(obj,FL_COL1,FL_RED);
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,100,60,70,32,"Green");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,100,90,70,32,"Blue");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,20,170,92,30,"LightButton");
    fl_set_button(obj, 1);
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,200,35,75,25,"Red");
    fl_set_object_color(obj,FL_COL1,FL_RED);
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,200,64,75,25,"Green");
    fl_set_object_color(obj,FL_COL1,FL_GREEN);
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,200,93,75,25,"Blue");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,180,170,30,30,"");
    fl_set_object_color(obj,FL_COL1,FL_RED);
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,240,170,30,30,"");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  obj = fl_add_button(FL_PUSH_BUTTON,130,210,30,30,"go");
    fl_set_object_boxtype(obj,FL_OVAL3D_UPBOX);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_button(FL_NORMAL_BUTTON,20,210,90,30,"Button");
    fl_set_object_boxtype(obj,FL_ROUNDED3D_UPBOX);
  fdui->bw_obj = obj = fl_add_choice(FL_NORMAL_CHOICE2,105,135,80,30,"BW");
    fl_set_object_callback(obj,bw_cb,0);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,190,25,85,100,"RoundButton");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,90,25,90,100,"CheckButton");
  fl_end_group();

  fl_end_form();

  fdui->buttform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

