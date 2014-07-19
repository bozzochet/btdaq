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
#include "viewforms.h"

static FL_PUP_ENTRY fdmenu_file_0[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "Open",	0,	"^O",	 FL_PUP_NONE},
    { "Save",	0,	"^S",	 FL_PUP_NONE},
    { "Print %l",	0,	"^P",	 FL_PUP_NONE},
    { "Quit",	0,	"^Q",	 FL_PUP_NONE},
    {0}
};

static FL_PUP_ENTRY fdmenu_options_1[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "Interpolations",	0,	"",	 FL_PUP_NONE},
    { "Layout",	0,	"",	 FL_PUP_NONE},
    { "Colors",	0,	"",	 FL_PUP_NONE},
    { "Font",	0,	"",	 FL_PUP_NONE},
    {0}
};

#include "quit.xpm"
static FL_PUP_ENTRY fdmenu_help_2[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "About",	0,	"",	 FL_PUP_NONE},
    { "Index",	0,	"",	 FL_PUP_NONE},
    { "Log Browser",	0,	"",	 FL_PUP_NONE},
    {0}
};

#include "reload.xpm"
#include "button1.xpm"
#include "pause.xpm"
#include "play.xpm"
#include "forward.xpm"
#include "backward.xpm"
#include "rewind.xpm"
#include "squiggle.xpm"
FD_main *create_form_main(void)
{
  FL_OBJECT *obj;
  FD_main *fdui = (FD_main *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->main = fl_bgn_form(FL_NO_BOX, 620, 672);
  obj = fl_add_box(FL_UP_BOX,0,0,620,672,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,85,640,70,25,"Frame");
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,10,640,70,25,"Structure");
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,565,37,46,53,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,347,37,46,53,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,160,640,451,25,"Comment");
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  fdui->timeout = obj = fl_add_timer(FL_HIDDEN_TIMER,560,10,30,20,"Timer");
    fl_set_object_callback(obj,text_timeout_cb,0);
  obj = fl_add_frame(FL_ENGRAVED_FRAME,6,6,610,25,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
  fdui->file = obj = fl_add_menu(FL_PULLDOWN_MENU,10,8,40,20,"File");
    fl_set_object_shortcut(obj,"#f",1);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,file_mnu_cb,0);
    fl_set_menu_entries(obj, fdmenu_file_0);
  obj = fl_add_text(FL_NORMAL_TEXT,478,10,88,18,"RNA Movies");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->options = obj = fl_add_menu(FL_PULLDOWN_MENU,61,8,51,20,"Options");
    fl_set_object_shortcut(obj,"#o",1);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,options_mnu_cb,0);
    fl_set_menu_entries(obj, fdmenu_options_1);
  fdui->button5 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,570,40,36,36,"Quit");
    fl_set_button_shortcut(obj,"^Q",1);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,5);
    fl_set_pixmapbutton_data(obj,quit_xpm);
  fdui->glcanvas = obj = fl_add_glcanvas(FL_NORMAL_CANVAS,10,132,600,500,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->help = obj = fl_add_menu(FL_PULLDOWN_MENU,566,8,40,20,"Help");
    fl_set_object_shortcut(obj,"#h",1);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,help_mnu_cb,0);
    fl_set_menu_entries(obj, fdmenu_help_2);
  fdui->movie_slider = obj = fl_add_slider(FL_HOR_BROWSER_SLIDER,9,96,603,27,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthEast);
    fl_set_object_callback(obj,movie_slider_cb,0);
    fl_set_slider_precision(obj, 0);
    fl_set_slider_bounds(obj, 0, 0);
    fl_set_slider_size(obj, 0.10);

  fdui->file_grp = fl_bgn_group();
  obj = fl_add_frame(FL_ENGRAVED_FRAME,11,37,90,53,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->button2 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,60,40,36,36,"Reload");
    fl_set_button_shortcut(obj,"^L",1);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,2);
    fl_set_pixmapbutton_data(obj,reload_xpm);
  fdui->button1 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,16,40,36,36,"Open");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,1);
    fl_set_pixmapbutton_data(obj,button1_xpm);
  fl_end_group();


  fdui->player_grp = fl_bgn_group();
  obj = fl_add_frame(FL_ENGRAVED_FRAME,111,37,226,53,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->button60 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,115,40,36,36,"Pause");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,60);
    fl_set_pixmapbutton_data(obj,pause_xpm);
  fdui->button61 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,160,40,36,36,"Play");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,61);
    fl_set_pixmapbutton_data(obj,play_xpm);
  fdui->button62 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,205,40,36,36,"Forward");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,62);
    fl_set_pixmapbutton_data(obj,forward_xpm);
  fdui->button63 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,251,40,36,36,"Backward");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,63);
    fl_set_pixmapbutton_data(obj,backward_xpm);
  fdui->button64 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,296,40,36,36,"Rewind");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,64);
    fl_set_pixmapbutton_data(obj,rewind_xpm);
  fl_end_group();

  fdui->button9 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,352,40,36,36,"Snapshot");
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,9);
    fl_set_pixmapbutton_data(obj,squiggle_xpm);
  fdui->comment = obj = fl_add_text(FL_NORMAL_TEXT,164,645,444,17,"");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  fdui->struccount = obj = fl_add_button(FL_NORMAL_BUTTON,14,644,63,19,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,but_cb,101);
  fdui->framecount = obj = fl_add_button(FL_NORMAL_BUTTON,88,643,65,20,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,but_cb,102);
  fl_end_form();

  fdui->main->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "close.xpm"
FD_help *create_form_help(void)
{
  FL_OBJECT *obj;
  FD_help *fdui = (FD_help *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->help = fl_bgn_form(FL_NO_BOX, 625, 350);
  obj = fl_add_box(FL_UP_BOX,0,0,625,350,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,10,60,170,285,"Index");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
  obj = fl_add_text(FL_NORMAL_TEXT,445,10,100,35,"Help Browser");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->help_index = obj = fl_add_browser(FL_HOLD_BROWSER,15,70,160,270,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthWest);
    fl_set_object_resize(obj, FL_RESIZE_Y);
    fl_set_object_callback(obj,help_nav_cb,0);
    fl_set_browser_hscrollbar(obj, FL_OFF);
    fl_add_browser_line(obj, "Introduction");
    fl_add_browser_line(obj, "RNA Secondary Structures");
    fl_add_browser_line(obj, "Movie Navigation");
    fl_add_browser_line(obj, "Zoom, Pan, Rotate");
    fl_add_browser_line(obj, "Saving Frames");
    fl_add_browser_line(obj, "Layout Options");
    fl_add_browser_line(obj, "Font Choice");
    fl_add_browser_line(obj, "References");
    fl_add_browser_line(obj, "Acknowledgements");
    fl_add_browser_line(obj, "Copyright Issues");
  fdui->help_disp = obj = fl_add_browser(FL_NORMAL_BROWSER,190,60,430,285,"");
    fl_set_object_color(obj,FL_WHITE,FL_YELLOW);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
    fl_set_browser_vscrollbar(obj, FL_ON);
    fl_add_browser_line(obj, "   ");
    fl_add_browser_line(obj, "@C1@l@c Help Instructions");
    fl_add_browser_line(obj, "  ");
    fl_add_browser_line(obj, "@C4@c Select one of the topics on the left.");
  fdui->timeout = obj = fl_add_timer(FL_HIDDEN_TIMER,565,5,30,25,"");
    fl_set_object_callback(obj,text_timeout_cb,0);
  fdui->button10 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,7,5,36,36,"Close");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,10);
    fl_set_pixmapbutton_data(obj,close_xpm);
  fl_end_form();

  fdui->help->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "cancel.xpm"
static FL_PUP_ENTRY fdchoice_font_3[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "helvetica-medium-r",	0,	"",	 FL_PUP_NONE},
    { "helvetica-bold-r",	0,	"",	 FL_PUP_NONE},
    { "helvetica-medium-o",	0,	"",	 FL_PUP_NONE},
    { "helvetica-bold-o",	0,	"",	 FL_PUP_NONE},
    { "courier-medium-r",	0,	"",	 FL_PUP_NONE},
    { "courier-bold-r",	0,	"",	 FL_PUP_NONE},
    { "courier-medium-o",	0,	"",	 FL_PUP_NONE},
    { "courier-bold-o",	0,	"",	 FL_PUP_NONE},
    { "times-medium-r",	0,	"",	 FL_PUP_NONE},
    { "times-bold-r",	0,	"",	 FL_PUP_NONE},
    { "times-medium-o",	0,	"",	 FL_PUP_NONE},
    { "times-bold-o",	0,	"",	 FL_PUP_NONE},
    { "charter-medium-r",	0,	"",	 FL_PUP_NONE},
    { "charter-bold-r",	0,	"",	 FL_PUP_NONE},
    { "charter-medium-i",	0,	"",	 FL_PUP_NONE},
    { "symbol",	0,	"",	 FL_PUP_NONE},
    {0}
};

#include "accept.xpm"
static FL_PUP_ENTRY fdchoice_size_4[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "8 Tiny",	0,	"",	 FL_PUP_NONE},
    { "10 Small",	0,	"",	 FL_PUP_NONE},
    { "12 Normal",	0,	"",	 FL_PUP_NONE},
    { "14 Medium",	0,	"",	 FL_PUP_NONE},
    { "18 Large",	0,	"",	 FL_PUP_NONE},
    { "24 Huge",	0,	"",	 FL_PUP_NONE},
    {0}
};

FD_fontselect *create_form_fontselect(void)
{
  FL_OBJECT *obj;
  FD_fontselect *fdui = (FD_fontselect *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->fontselect = fl_bgn_form(FL_NO_BOX, 365, 145);
  obj = fl_add_box(FL_UP_BOX,0,0,365,145,"");
  obj = fl_add_text(FL_NORMAL_TEXT,243,10,110,35,"Font Selection");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button20 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,200,5,36,36,"Cancel");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,20);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fdui->font = obj = fl_add_choice(FL_NORMAL_CHOICE,32,62,135,26,"Font");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,font_choice_cb,0);
    fl_set_choice_entries(obj, fdchoice_font_3);
  fdui->button21 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,5,5,36,36,"Accept");
    fl_set_button_shortcut(obj,"^M",1);
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,21);
    fl_set_pixmapbutton_data(obj,accept_xpm);
  fdui->size = obj = fl_add_choice(FL_NORMAL_CHOICE,228,62,135,26,"Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,size_choice_cb,0);
    fl_set_choice_entries(obj, fdchoice_size_4);
  fdui->text = obj = fl_add_input(FL_NORMAL_INPUT,5,95,358,44,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_lsize(obj,FL_HUGE_SIZE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fl_end_form();

  fdui->fontselect->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_about *create_form_about(void)
{
  FL_OBJECT *obj;
  FD_about *fdui = (FD_about *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->about = fl_bgn_form(FL_NO_BOX, 290, 210);
  obj = fl_add_box(FL_UP_BOX,0,0,290,210,"");
  fdui->revision = obj = fl_add_text(FL_NORMAL_TEXT,80,145,155,20,"");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->date = obj = fl_add_text(FL_NORMAL_TEXT,80,125,155,20,"");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,80,65,155,20,"Written by Dirk Evers");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fdui->button30 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,5,5,36,36,"Close");
    fl_set_button_shortcut(obj,"^C",1);
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,but_cb,30);
    fl_set_pixmapbutton_data(obj,close_xpm);
  obj = fl_add_text(FL_NORMAL_TEXT,80,28,204,35,"The RNA 2D Structure Animator");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_NORMAL_STYLE+FL_SHADOW_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,80,5,194,35,"RNA Movies");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_LARGE_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE+FL_SHADOW_STYLE);
  obj = fl_add_text(FL_NORMAL_TEXT,80,85,155,20,"dirk@TechFak.Uni-Bielefeld.DE");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  obj = fl_add_text(FL_NORMAL_TEXT,20,185,255,20,"http://BiBiServ.TechFak.Uni-Bielefeld.DE/rnamovies/");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();

  fdui->about->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_brucc_opt *create_form_brucc_opt(void)
{
  FL_OBJECT *obj;
  FD_brucc_opt *fdui = (FD_brucc_opt *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->brucc_opt = fl_bgn_form(FL_NO_BOX, 360, 356);
  obj = fl_add_box(FL_UP_BOX,0,0,360,356,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,8,265,200,39,"Label");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,8,166,200,40,"Bonds");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,8,115,200,40,"Nucleotides");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->labelfreq = obj = fl_add_input(FL_INT_INPUT,13,273,53,22,"Frequency");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,0);
  fdui->button43 = obj = fl_add_checkbutton(FL_PUSH_BUTTON,116,118,84,36,"Colored");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,43);
    fl_set_button(obj, 1);
  fdui->button42 = obj = fl_add_checkbutton(FL_PUSH_BUTTON,37,118,72,34,"Labeled");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,42);
    fl_set_button(obj, 1);
  obj = fl_add_text(FL_NORMAL_TEXT,216,5,126,40,"Structure Layout");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button40 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,7,5,36,36,"Dismiss");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,40);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,8,64,200,40,"Backbone");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);

  fdui->spl_brucc = fl_bgn_group();
  fdui->button47 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,136,66,65,37,"Linear");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,47);
    fl_set_button(obj, 1);
  fdui->button44 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,12,66,66,37,"Spline");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,44);
  fdui->button48 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,75,67,66,37,"Strips");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,48);
  fl_end_group();

  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,8,215,200,39,"Direction");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
  fdui->but401 = obj = fl_add_checkbutton(FL_PUSH_BUTTON,113,219,65,36,"Display 3'");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,401);
    fl_set_button(obj, 1);
  fdui->but400 = obj = fl_add_checkbutton(FL_PUSH_BUTTON,35,219,78,36,"Display 5'");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,400);
    fl_set_button(obj, 1);

  fdui->dln_brucc = fl_bgn_group();
  fdui->button46 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,140,168,65,36,"None");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,46);
  fdui->button45 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,83,167,66,36,"Lines");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,45);
  fdui->button49 = obj = fl_add_checkbutton(FL_RADIO_BUTTON,10,168,66,36,"WC/GU");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,update_cb,49);
    fl_set_button(obj, 1);
  fl_end_group();

  fdui->label_dist = obj = fl_add_valslider(FL_HOR_SLIDER,218,123,129,18,"Nucleotide Distance");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,label_dist_cb,0);
    fl_set_slider_bounds(obj, 0, 2);
    fl_set_slider_size(obj, 0.15);
  fdui->freq_dist = obj = fl_add_valslider(FL_HOR_SLIDER,219,273,129,18,"Label Distance");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,freq_dist_cb,0);
    fl_set_slider_bounds(obj, 0, 3);
    fl_set_slider_value(obj, 0.7);
    fl_set_slider_size(obj, 0.15);
  fdui->anim_time = obj = fl_add_valslider(FL_HOR_SLIDER,218,318,129,18,"Anim Time");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_callback(obj,anim_time_cb,0);
    fl_set_slider_precision(obj, 0);
    fl_set_slider_bounds(obj, 0, 300);
    fl_set_slider_value(obj, 50);
    fl_set_slider_size(obj, 0.17);
     fl_set_slider_return(obj, FL_RETURN_END_CHANGED);
  fdui->strip_width = obj = fl_add_counter(FL_NORMAL_COUNTER,225,74,124,21,"Strip Width");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,update_cb,41);
    fl_set_counter_bounds(obj, 0.0, 1000000.0);
    fl_set_counter_value(obj, 0.5);
  fl_end_form();

  fdui->brucc_opt->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_snapshot *create_form_snapshot(void)
{
  FL_OBJECT *obj;
  FD_snapshot *fdui = (FD_snapshot *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->snapshot = fl_bgn_form(FL_NO_BOX, 325, 365);
  obj = fl_add_box(FL_UP_BOX,0,0,325,365,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,289,331,34,34,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
  obj = fl_add_glcanvas(FL_NORMAL_CANVAS,4,27,300,300,"");
    fl_set_object_color(obj,FL_COL1,FL_COL1);
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->x_displacmnt = obj = fl_add_slider(FL_HOR_SLIDER,100,331,186,15,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
    fl_set_object_callback(obj,x_snap_cb,0);
  fdui->y_displacmnt = obj = fl_add_slider(FL_VERT_SLIDER,308,26,15,302,"");
    fl_set_object_gravity(obj, FL_NorthEast, FL_SouthEast);
    fl_set_object_callback(obj,y_snap_cb,0);
  fdui->rotation = obj = fl_add_dial(FL_LINE_DIAL,292,334,29,29,"");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,r_snap_cb,0);
  fdui->zoom = obj = fl_add_counter(FL_NORMAL_COUNTER,3,331,94,15,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,z_snap_cb,0);
  fdui->comment = obj = fl_add_text(FL_NORMAL_TEXT,3,349,284,17,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  fdui->button70 = obj = fl_add_button(FL_NORMAL_BUTTON,3,3,32,20,"Close");
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,70);
  obj = fl_add_text(FL_NORMAL_TEXT,239,3,85,21,"Snapshot");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_NORMAL_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fl_end_form();

  fdui->snapshot->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_log *create_form_log(void)
{
  FL_OBJECT *obj;
  FD_log *fdui = (FD_log *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->log = fl_bgn_form(FL_NO_BOX, 530, 360);
  obj = fl_add_box(FL_UP_BOX,0,0,530,360,"");
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,10,65,510,285,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->button80 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,10,8,36,36,"Close");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,80);
    fl_set_pixmapbutton_data(obj,close_xpm);
  obj = fl_add_text(FL_NORMAL_TEXT,410,10,110,35,"Log Browser");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button81 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,58,8,36,36,"Clear Log");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,81);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fl_end_form();

  fdui->log->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_save *create_form_save(void)
{
  FL_OBJECT *obj;
  FD_save *fdui = (FD_save *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->save = fl_bgn_form(FL_NO_BOX, 300, 150);
  obj = fl_add_box(FL_UP_BOX,0,0,300,150,"");
    fl_set_object_resize(obj, FL_RESIZE_NONE);
  obj = fl_add_text(FL_NORMAL_TEXT,223,10,50,35,"Save");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button90 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,165,5,36,36,"Cancel");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,90);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fdui->button91 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,5,5,36,36,"Accept");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,91);
    fl_set_pixmapbutton_data(obj,accept_xpm);
  fdui->filename = obj = fl_add_input(FL_NORMAL_INPUT,50,110,245,30,"Filename");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
  fdui->xsize = obj = fl_add_counter(FL_NORMAL_COUNTER,5,70,130,30,"X-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,x_size_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  fdui->ysize = obj = fl_add_counter(FL_NORMAL_COUNTER,165,70,130,30,"Y-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,y_size_cb,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  fl_end_form();

  fdui->save->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_save_as_file *create_form_save_as_file(void)
{
  FL_OBJECT *obj;
  FD_save_as_file *fdui = (FD_save_as_file *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->save_as_file = fl_bgn_form(FL_NO_BOX, 300, 250);
  obj = fl_add_box(FL_UP_BOX,0,0,300,250,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,60,110,180,85,"Frames");
  obj = fl_add_text(FL_NORMAL_TEXT,223,10,50,35,"Save");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button90 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,165,5,36,36,"Cancel");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,90);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fdui->button91 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,5,5,36,36,"Accept");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,91);
    fl_set_pixmapbutton_data(obj,accept_xpm);
  fdui->filename = obj = fl_add_input(FL_NORMAL_INPUT,5,215,220,25,"Filename:");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_callback(obj,Void,0);
  fdui->xsize = obj = fl_add_counter(FL_NORMAL_COUNTER,5,70,130,30,"X-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,Void,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  fdui->ysize = obj = fl_add_counter(FL_NORMAL_COUNTER,165,70,130,30,"Y-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,Void,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  fdui->button92 = obj = fl_add_button(FL_NORMAL_BUTTON,230,215,60,25,"Browse");
    fl_set_object_callback(obj,but_cb,92);

  fdui->frames = fl_bgn_group();
  fdui->frames_fr_to = obj = fl_add_checkbutton(FL_RADIO_BUTTON,65,165,25,25,"");
    fl_set_object_callback(obj,Void,0);
  fdui->frames_from = obj = fl_add_input(FL_NORMAL_INPUT,119,167,40,20,"from:");
    fl_set_object_callback(obj,Void,0);
  fdui->frames_to = obj = fl_add_input(FL_NORMAL_INPUT,182,167,40,20,"to:");
    fl_set_object_callback(obj,Void,0);
  fdui->frames_current = obj = fl_add_checkbutton(FL_RADIO_BUTTON,65,115,25,25,"current");
    fl_set_object_callback(obj,Void,0);
    fl_set_button(obj, 1);
  fdui->frames_all = obj = fl_add_checkbutton(FL_RADIO_BUTTON,65,140,25,25,"all");
    fl_set_object_callback(obj,Void,0);
  fl_end_group();

  fl_end_form();

  fdui->save_as_file->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_save_as *create_form_save_as(void)
{
  FL_OBJECT *obj;
  FD_save_as *fdui = (FD_save_as *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->save_as = fl_bgn_form(FL_NO_BOX, 300, 315);
  obj = fl_add_box(FL_UP_BOX,0,0,300,315,"");
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,195,110,99,110,"File Format");
  obj = fl_add_text(FL_NORMAL_TEXT,223,10,50,35,"Save");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->button90 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,165,5,36,36,"Cancel");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,90);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fdui->button91 = obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,5,5,36,36,"Accept");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,91);
    fl_set_pixmapbutton_data(obj,accept_xpm);
  fdui->xsize = obj = fl_add_counter(FL_NORMAL_COUNTER,5,70,130,30,"X-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,Void,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  fdui->ysize = obj = fl_add_counter(FL_NORMAL_COUNTER,165,70,130,30,"Y-Size");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP);
    fl_set_object_callback(obj,Void,0);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, 10, 1024);
    fl_set_counter_value(obj, 512);
    fl_set_counter_step(obj, 1, 10);
  obj = fl_add_labelframe(FL_ENGRAVED_FRAME,6,110,180,110,"Frames");
  fdui->frames_from = obj = fl_add_input(FL_NORMAL_INPUT,65,167,40,20,"from:");
    fl_set_object_callback(obj,Void,0);
  fdui->frames_to = obj = fl_add_input(FL_NORMAL_INPUT,128,167,40,20,"to:");
    fl_set_object_callback(obj,Void,0);

  fdui->frames = fl_bgn_group();
  fdui->frames_fr_to = obj = fl_add_checkbutton(FL_RADIO_BUTTON,11,165,25,25,"");
    fl_set_object_callback(obj,Void,0);
  fdui->frames_current = obj = fl_add_checkbutton(FL_RADIO_BUTTON,11,115,25,25,"current");
    fl_set_object_callback(obj,Void,0);
    fl_set_button(obj, 1);
  fdui->frames_all = obj = fl_add_checkbutton(FL_RADIO_BUTTON,11,140,25,25,"all");
    fl_set_object_callback(obj,Void,0);
  fl_end_group();


  fdui->format = fl_bgn_group();
  fdui->format_gif = obj = fl_add_checkbutton(FL_RADIO_BUTTON,205,115,25,25,"GIF");
    fl_set_object_callback(obj,Void,0);
    fl_set_button(obj, 1);
  fdui->format_ps = obj = fl_add_checkbutton(FL_RADIO_BUTTON,205,140,25,25,"PS");
    fl_set_object_callback(obj,Void,0);
  fdui->format_eps = obj = fl_add_checkbutton(FL_RADIO_BUTTON,205,165,25,25,"EPS");
    fl_set_object_callback(obj,Void,0);
  fdui->format_2dcoords = obj = fl_add_checkbutton(FL_RADIO_BUTTON,205,190,25,25,"2D Coords");
    fl_set_object_callback(obj,Void,0);
  fl_end_group();

  fdui->pathname = obj = fl_add_input(FL_NORMAL_INPUT,6,238,288,27,"Path:");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,Void,0);
  fdui->prefix = obj = fl_add_input(FL_NORMAL_INPUT,6,282,288,27,"Prefix:");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
    fl_set_object_callback(obj,Void,0);
  fl_end_form();

  fdui->save_as->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_color_opt *create_form_color_opt(void)
{
  FL_OBJECT *obj;
  FD_color_opt *fdui = (FD_color_opt *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->color_opt = fl_bgn_form(FL_NO_BOX, 138, 525);
  obj = fl_add_box(FL_UP_BOX,0,0,138,525,"");
  fdui->backbone_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,94,29,28,"Backbone");
    fl_set_object_color(obj,FL_GREEN,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,1);
  fdui->background_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,59,29,28,"Background");
    fl_set_object_color(obj,FL_BLACK,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,0);
  fdui->nucno_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,129,29,28,"Nucleotide Number");
    fl_set_object_color(obj,FL_WHITE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,2);
  fdui->wc_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,165,29,28,"WC Bond");
    fl_set_object_color(obj,FL_RED,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,3);
  fdui->gu_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,200,29,28,"GU Bond");
    fl_set_object_color(obj,FL_TOMATO,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,4);
  fdui->other_bond_col = obj = fl_add_button(FL_NORMAL_BUTTON,6,237,29,28,"Other Bonds");
    fl_set_object_color(obj,FL_DARKORANGE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,5);
  fdui->a_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,274,29,28,"A");
    fl_set_object_color(obj,FL_RED,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,6);
  fdui->c_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,311,29,28,"C");
    fl_set_object_color(obj,FL_BLUE,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,7);
  fdui->g_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,349,29,28,"G");
    fl_set_object_color(obj,FL_LEFT_BCOL,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,8);
  fdui->u_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,384,29,28,"U");
    fl_set_object_color(obj,FL_GREEN,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,9);
  fdui->other_nuc_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,421,29,28,"Other Nucleotides");
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,10);
  obj = fl_add_text(FL_NORMAL_TEXT,76,7,56,34,"Colors");
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,6,8,36,36,"Dismiss");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,110);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  fdui->nuc_un_col = obj = fl_add_button(FL_NORMAL_BUTTON,5,457,29,28,"Single Color");
    fl_set_object_color(obj,FL_YELLOW,FL_MCOL);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_callback(obj,col_cb,11);
  fl_end_form();

  fdui->color_opt->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_interp *create_form_interp(void)
{
  FL_OBJECT *obj;
  FD_interp *fdui = (FD_interp *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(1);
  fdui->interp = fl_bgn_form(FL_NO_BOX, 284, 117);
  obj = fl_add_box(FL_UP_BOX,0,0,284,117,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,7,73,270,37,"");
    fl_set_object_callback(obj,but_cb,41);
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,7,8,36,36,"Accept");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,but_cb,121);
    fl_set_pixmapbutton_data(obj,accept_xpm);
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,124,8,36,36,"Cancel");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,but_cb,120);
    fl_set_pixmapbutton_data(obj,cancel_xpm);
  obj = fl_add_text(FL_NORMAL_TEXT,169,6,106,40,"Interpolations");
    fl_set_object_color(obj,FL_COL1,FL_LEFT_BCOL);
    fl_set_object_lcolor(obj,FL_BLUE);
    fl_set_object_lsize(obj,FL_MEDIUM_SIZE);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_lstyle(obj,FL_BOLD_STYLE);
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
  fdui->interp_freq = obj = fl_add_input(FL_INT_INPUT,17,81,53,21,"Interpolations");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_RIGHT);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NorthWest);
    fl_set_object_callback(obj,interp_freq_cb,0);
  fl_end_form();

  fdui->interp->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

