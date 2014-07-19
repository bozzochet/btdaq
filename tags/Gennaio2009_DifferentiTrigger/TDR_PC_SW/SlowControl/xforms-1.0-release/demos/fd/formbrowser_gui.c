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
#include "formbrowser_gui.h"

FD_buttonform *create_form_buttonform(void)
{
  FL_OBJECT *obj;
  FD_buttonform *fdui = (FD_buttonform *) fl_calloc(1, sizeof(*fdui));

  fdui->buttonform = fl_bgn_form(FL_NO_BOX, 430, 210);
  obj = fl_add_box(FL_FLAT_BOX,0,0,430,210,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,30,151,80,30,"Button");
  obj = fl_add_roundbutton(FL_PUSH_BUTTON,40,91,100,30,"RoundButton");
  obj = fl_add_round3dbutton(FL_PUSH_BUTTON,160,160,110,30,"Round3DButton");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  obj = fl_add_checkbutton(FL_PUSH_BUTTON,170,120,110,30,"CheckButton");
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,20,50,100,30,"LightButton");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,320,36,80,80,"PixmapButton");
    fl_set_pixmapbutton_file(obj, "porsche.xpm");
  obj = fl_add_button(FL_NORMAL_BUTTON,140,50,100,30,"Button");
    fl_set_object_boxtype(obj,FL_ROUNDED3D_UPBOX);
  obj = fl_add_lightbutton(FL_PUSH_BUTTON,290,146,100,30,"Button");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
  obj = fl_add_button(FL_NORMAL_BUTTON,210,90,60,25,"Button");
    fl_set_object_boxtype(obj,FL_SHADOW_BOX);
    fl_set_object_color(obj,FL_COL1,FL_SLATEBLUE);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,130,10,150,30,"Form1");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
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

  fdui->mainform = fl_bgn_form(FL_NO_BOX, 460, 290);
  obj = fl_add_box(FL_UP_BOX,0,0,460,290,"");
  fdui->done = obj = fl_add_button(FL_NORMAL_BUTTON,381,250,64,25,"Done");
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,done_cb,0);
  fdui->hide = obj = fl_add_button(FL_NORMAL_BUTTON,15,249,64,27,"Hide");
    fl_set_button_shortcut(obj,"#H",1);
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,hide_show_cb,0);
  fdui->show = obj = fl_add_button(FL_NORMAL_BUTTON,79,249,64,27,"Show");
    fl_set_button_shortcut(obj,"#S",1);
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,hide_show_cb,1);
  fdui->reshow = obj = fl_add_button(FL_NORMAL_BUTTON,155,249,64,27,"ReShow");
    fl_set_button_shortcut(obj,"#R",1);
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,reshow_cb,0);
  fdui->set = obj = fl_add_button(FL_NORMAL_BUTTON,217,249,79,27,"Jump/Smooth");
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,scroll_cb,0);
  fdui->deactivate = obj = fl_add_button(FL_NORMAL_BUTTON,296,249,69,27,"Deactivate");
    fl_set_object_resize(obj, FL_RESIZE_NONE);
    fl_set_object_callback(obj,deactivate_cb,0);
  fdui->formbrowser = obj = fl_add_formbrowser(FL_NORMAL_FORMBROWSER,20,20,420,220,"");
    fl_set_object_color(obj,FL_COL1,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_CENTER);
  fl_end_form();

  fdui->mainform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_valuatorform *create_form_valuatorform(void)
{
  FL_OBJECT *obj;
  FD_valuatorform *fdui = (FD_valuatorform *) fl_calloc(1, sizeof(*fdui));

  fdui->valuatorform = fl_bgn_form(FL_NO_BOX, 430, 390);
  obj = fl_add_box(FL_FLAT_BOX,0,0,430,390,"");
  obj = fl_add_positioner(FL_NORMAL_POSITIONER,260,180,110,110,"");
    fl_set_positioner_xvalue(obj, 0.679012);
    fl_set_positioner_yvalue(obj, 0.71831);
  obj = fl_add_valslider(FL_HOR_NICE_SLIDER,130,130,250,20,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_slider_value(obj, 0.87);
  obj = fl_add_counter(FL_NORMAL_COUNTER,30,270,110,20,"");
    fl_set_counter_value(obj, -1.0);
  obj = fl_add_slider(FL_VERT_NICE_SLIDER,10,40,20,160,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_RED);
    fl_set_slider_value(obj, 0.49);
  obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,60,350,150,21,"");
    fl_set_slider_size(obj, 0.10);
  obj = fl_add_slider(FL_HOR_FILL_SLIDER,60,170,159,22,"");
    fl_set_object_color(obj,FL_COL1,FL_SLATEBLUE);
    fl_set_slider_value(obj, 0.25);
  obj = fl_add_dial(FL_NORMAL_DIAL,160,220,60,58,"");
    fl_set_object_boxtype(obj,FL_UP_BOX);
  obj = fl_add_scrollbar(FL_VERT_THIN_SCROLLBAR,394,14,18,180,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_scrollbar_size(obj, 0.20);
  obj = fl_add_scrollbar(FL_HOR_SCROLLBAR,170,70,140,16,"");
    fl_set_scrollbar_size(obj, 0.25);
  obj = fl_add_thumbwheel(FL_VERT_THUMBWHEEL,80,70,17,84,"");
  obj = fl_add_thumbwheel(FL_HOR_THUMBWHEEL,280,330,90,19,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,100,10,180,30,"Valuator Form");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  fl_end_form();

  fdui->valuatorform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

static FL_PUP_ENTRY fdmenu_pulldown_0[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "Menu item 1",	0,	"1#1",	 FL_PUP_NONE},
    { "Menu item 2",	0,	"2#2",	 FL_PUP_NONE},
    { "Menu Item None",	0,	"",	 FL_PUP_GRAY},
    { "Menu item 3",	0,	"3#3",	 FL_PUP_NONE},
    {0}
};

static FL_PUP_ENTRY fdchoice_choice_1[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "Choice item 1",	0,	"1#1",	 FL_PUP_NONE},
    { "Choice item 2",	0,	"",	 FL_PUP_NONE},
    { "Choice item 3",	0,	"",	 FL_PUP_NONE},
    {0}
};

static FL_PUP_ENTRY fdmenu_pushmenu_2[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "1",	0,	"",	 FL_PUP_NONE},
    { "2",	0,	"",	 FL_PUP_NONE},
    { "3",	0,	"",	 FL_PUP_NONE},
    { "4",	0,	"",	 FL_PUP_NONE},
    {0}
};

FD_choiceform *create_form_choiceform(void)
{
  FL_OBJECT *obj;
  FD_choiceform *fdui = (FD_choiceform *) fl_calloc(1, sizeof(*fdui));

  fdui->choiceform = fl_bgn_form(FL_NO_BOX, 430, 210);
  obj = fl_add_box(FL_FLAT_BOX,0,0,430,210,"");
  fdui->pulldown = obj = fl_add_menu(FL_PULLDOWN_MENU,40,70,52,22,"Menu");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_menu_entries(obj, fdmenu_pulldown_0);
  fdui->choice = obj = fl_add_choice(FL_NORMAL_CHOICE2,10,120,111,27,"");
    fl_set_choice_entries(obj, fdchoice_choice_1);
  fdui->browser = obj = fl_add_browser(FL_HOLD_BROWSER,250,10,164,184,"");
    fl_add_browser_line(obj, "@c@bBrowser");
    fl_add_browser_line(obj, "This is line #1 of the browser");
    fl_add_browser_line(obj, "This is line #2 of the browser");
    fl_add_browser_line(obj, "This is line #3 of the browser");
    fl_add_browser_line(obj, "This is line #4 of the browser");
    fl_add_browser_line(obj, "This is line #5 of the browser");
    fl_add_browser_line(obj, "This is line #6 of the browser");
    fl_add_browser_line(obj, "This is line #7 of the browser");
    fl_add_browser_line(obj, "@C1This is line #8 of the browser");
    fl_add_browser_line(obj, "This is line #9 of the browser");
    fl_add_browser_line(obj, "@C2This is line #10 of the browser");
    fl_add_browser_line(obj, "This is line #11 of the browser");
    fl_add_browser_line(obj, "This is line #12 of the browser");
    fl_add_browser_line(obj, "This is line #13 of the browser");
    fl_add_browser_line(obj, "@C3This is line #14 of the browser");
  fdui->pushmenu = obj = fl_add_menu(FL_PUSH_MENU,150,130,75,26,"Menu");
    fl_set_object_boxtype(obj,FL_UP_BOX);
    fl_set_menu_entries(obj, fdmenu_pushmenu_2);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,40,10,170,30,"Choice Form");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
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
  obj = fl_add_input(FL_MULTILINE_INPUT,70,40,280,90,"MultiLine\nInput");
  obj = fl_add_input(FL_NORMAL_INPUT,80,152,250,34,"Input");
  fl_end_form();

  fdui->inputform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

FD_big *create_form_big(void)
{
  FL_OBJECT *obj;
  FD_big *fdui = (FD_big *) fl_calloc(1, sizeof(*fdui));

  fdui->big = fl_bgn_form(FL_NO_BOX, 520, 460);
  obj = fl_add_box(FL_BORDER_BOX,0,0,520,460,"");
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
  obj = fl_add_thumbwheel(FL_VERT_THUMBWHEEL,100,50,17,80,"");
  obj = fl_add_positioner(FL_NORMAL_POSITIONER,180,30,180,150,"");
  fdui->xyplot = obj = fl_add_xyplot(FL_ACTIVE_XYPLOT,30,210,290,210,"ActiveXYPlot");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_color(obj,FL_BLACK,FL_GREEN);
    fl_set_object_lsize(obj,FL_DEFAULT_SIZE);
  obj = fl_add_box(FL_UP_BOX,345,425,170,30,"End ofBif FORM");
  fl_end_form();

  fdui->big->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

