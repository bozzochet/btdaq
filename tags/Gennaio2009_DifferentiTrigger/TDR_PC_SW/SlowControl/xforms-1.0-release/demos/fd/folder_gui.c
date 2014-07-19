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
#include "folder_gui.h"

FD_buttonform *create_form_buttonform(void)
{
  FL_OBJECT *obj;
  FD_buttonform *fdui = (FD_buttonform *) fl_calloc(1, sizeof(*fdui));

  fdui->buttonform = fl_bgn_form(FL_NO_BOX, 430, 210);
  obj = fl_add_box(FL_FLAT_BOX,0,0,430,210,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,30,151,80,30,"Button");
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,40,91,100,30,"RoundButton");
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,135,151,110,30,"Round3DButton");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  obj = fl_add_checkbutton(FL_PUSH_BUTTON,170,111,110,30,"CheckButton");
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,30,31,100,30,"LightButton");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,320,36,80,80,"PixmapButton");
    fl_set_pixmapbutton_file(obj, "porsche.xpm");
  obj = fl_add_button(FL_NORMAL_BUTTON,185,26,100,30,"Button");
    fl_set_object_boxtype(obj,FL_ROUNDED3D_UPBOX);
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,290,146,100,30,"Button");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
  obj = fl_add_button(FL_NORMAL_BUTTON,175,71,60,25,"Button");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_color(obj,FL_COL1,FL_SLATEBLUE);
  fl_end_form();

  fdui->buttonform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_staticform *create_form_staticform(void)
{
  FL_OBJECT *obj;
  FD_staticform *fdui = (FD_staticform *) fl_calloc(1, sizeof(*fdui));

  fdui->staticform = fl_bgn_form(FL_NO_BOX, 431, 211);
  obj = fl_add_box(FL_FLAT_BOX,0,0,431,211,"");
    fl_set_object_color(obj,FL_INDIANRED,FL_INDIANRED);
    fl_set_object_lcolor(obj,FL_INDIANRED);
  obj = fl_add_box(FL_UP_BOX,40,40,60,45,"A Box");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,130,30,120,55,"LabelFrame");
    fl_set_object_color(obj,FL_BLACK,FL_INDIANRED);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fdui->chart = obj = fl_add_chart(FL_PIE_CHART,270,20,130,105,"");
    fl_set_object_color(obj,FL_INDIANRED,FL_COL1);
  obj = fl_add_clock(FL_ANALOG_CLOCK,30,100,85,85,"");
    fl_set_object_color(obj,FL_COL1,FL_RIGHT_BCOL);
  obj = fl_add_bitmap(FL_NORMAL_BITMAP,150,140,30,25,"");
  obj = fl_add_pixmap(FL_NORMAL_PIXMAP,210,120,60,60,"");
  obj = fl_add_text(FL_NORMAL_TEXT,310,150,70,25,"Text");
    fl_set_object_boxtype(obj,FL_BORDER_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();

  fdui->staticform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_mainform *create_form_mainform(void)
{
  FL_OBJECT *obj;
  FD_mainform *fdui = (FD_mainform *) fl_calloc(1, sizeof(*fdui));

  fdui->mainform = fl_bgn_form(FL_NO_BOX, 461, 291);
  obj = fl_add_box(FL_UP_BOX,0,0,461,291,"");
  fdui->done = obj = fl_add_button(FL_NORMAL_BUTTON,381,250,64,25,"Done");
    fl_set_object_callback(obj,done_cb,0);
  fdui->hide = obj = fl_add_button(FL_NORMAL_BUTTON,15,249,64,27,"Hide");
    fl_set_button_shortcut(obj,"#H",1);
    fl_set_object_callback(obj,hide_show_cb,0);
  fdui->show = obj = fl_add_button(FL_NORMAL_BUTTON,79,249,64,27,"Show");
    fl_set_button_shortcut(obj,"#S",1);
    fl_set_object_callback(obj,hide_show_cb,1);
  fdui->reshow = obj = fl_add_button(FL_NORMAL_BUTTON,155,249,64,27,"ReShow");
    fl_set_button_shortcut(obj,"#R",1);
    fl_set_object_callback(obj,reshow_cb,0);
  fdui->folder = obj = fl_add_tabfolder(FL_TOP_TABFOLDER,15,11,435,230,"");
  fdui->set = obj = fl_add_button(FL_NORMAL_BUTTON,232,249,64,27,"Set");
    fl_set_object_callback(obj,set_cb,0);
  fdui->deactivate = obj = fl_add_button(FL_NORMAL_BUTTON,296,249,69,27,"Deactivate");
    fl_set_object_callback(obj,deactivate_cb,0);
  fl_end_form();

  fdui->mainform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_valuatorform *create_form_valuatorform(void)
{
  FL_OBJECT *obj;
  FD_valuatorform *fdui = (FD_valuatorform *) fl_calloc(1, sizeof(*fdui));

  fdui->valuatorform = fl_bgn_form(FL_NO_BOX, 431, 211);
  obj = fl_add_box(FL_FLAT_BOX,0,0,431,211,"");
  obj = fl_add_positioner(FL_NORMAL_POSITIONER,280,50,82,72,"");
    fl_set_positioner_xvalue(obj, 0.679012);
    fl_set_positioner_yvalue(obj, 0.71831);
  obj = fl_add_valslider(FL_HOR_NICE_SLIDER,55,10,240,20,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_RIGHT_BCOL);
    fl_set_slider_value(obj, 0.87);
  obj = fl_add_counter(FL_NORMAL_COUNTER,130,110,110,20,"");
    fl_set_counter_value(obj, -1.0);
  obj = fl_add_slider(FL_VERT_NICE_SLIDER,10,30,20,160,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_slider_value(obj, 0.49);
  obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,70,170,150,21,"");
    fl_set_slider_size(obj, 0.10);
  obj = fl_add_slider(FL_HOR_FILL_SLIDER,69,57,159,22,"");
    fl_set_object_color(obj,FL_COL1,FL_SLATEBLUE);
    fl_set_slider_value(obj, 0.25);
  obj = fl_add_dial(FL_NORMAL_DIAL,60,90,60,58,"");
    fl_set_object_boxtype(obj,FL_UP_BOX);
  obj = fl_add_scrollbar(FL_VERT_THIN_SCROLLBAR,394,14,18,180,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_scrollbar_size(obj, 0.20);
  obj = fl_add_scrollbar(FL_HOR_SCROLLBAR,238,158,140,16,"");
    fl_set_scrollbar_size(obj, 0.25);
  fl_end_form();

  fdui->valuatorform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_choiceform *create_form_choiceform(void)
{
  FL_OBJECT *obj;
  FD_choiceform *fdui = (FD_choiceform *) fl_calloc(1, sizeof(*fdui));

  fdui->choiceform = fl_bgn_form(FL_NO_BOX, 431, 211);
  obj = fl_add_box(FL_FLAT_BOX,0,0,431,211,"");
  fdui->pulldown = obj = fl_add_menu(FL_PULLDOWN_MENU,45,36,45,21,"Menu");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
  fdui->choice = obj = fl_add_choice(FL_NORMAL_CHOICE2,24,93,111,27,"");
  fdui->browser = obj = fl_add_browser(FL_HOLD_BROWSER,257,14,154,179,"");
  fdui->pushmenu = obj = fl_add_menu(FL_PUSH_MENU,152,51,75,26,"Menu");
    fl_set_object_boxtype(obj,FL_UP_BOX);
  fl_end_form();

  fdui->choiceform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_inputform *create_form_inputform(void)
{
  FL_OBJECT *obj;
  FD_inputform *fdui = (FD_inputform *) fl_calloc(1, sizeof(*fdui));

  fdui->inputform = fl_bgn_form(FL_NO_BOX, 430, 210);
  obj = fl_add_box(FL_FLAT_BOX,0,0,430,210,"");
  obj = fl_add_input(FL_MULTILINE_INPUT,70,20,280,90,"MultiLine\nInput");
  obj = fl_add_input(FL_NORMAL_INPUT,80,132,250,34,"Input");
  fl_end_form();

  fdui->inputform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

