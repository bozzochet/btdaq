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
#include "pallette.h"

FD_pmain *create_form_pmain(void)
{
  FL_OBJECT *obj;
  FD_pmain *fdui = (FD_pmain *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->pmain = fl_bgn_form(FL_NO_BOX, 380, 210);
  obj = fl_add_box(FL_FLAT_BOX,0,0,380,210,"");
  fdui->folder = obj = fl_add_tabfolder(FL_TOP_TABFOLDER,5,5,370,175,"");
    fl_set_object_resize(obj, FL_RESIZE_ALL);
  obj = fl_add_button(FL_NORMAL_BUTTON,311,183,62,23," Dismiss ");
    fl_set_object_callback(obj,dismiss_pallete,0);
  fl_end_form();

  fl_adjust_form_size(fdui->pmain);
  fdui->pmain->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "butt.xpm"
#include "lightb.xpm"
#include "roundb.xpm"
#include "r3dbut.xpm"
#include "check.xpm"
#include "picture.xbm"
#include "xconq.xpm"
FD_buttform *create_form_buttform(void)
{
  FL_OBJECT *obj;
  FD_buttform *fdui = (FD_buttform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->buttform = fl_bgn_form(FL_NO_BOX, 360, 150);
  obj = fl_add_box(FL_FLAT_BOX,0,0,360,150,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,10,345,130,"");
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,15,15,70,40,"Button");
    fl_set_button_shortcut(obj,"B#B",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_BUTTON);
    fl_set_pixmapbutton_data(obj,butt_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,101,15,70,40,"LightButton");
    fl_set_button_shortcut(obj,"L#L",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_LIGHTBUTTON);
    fl_set_pixmapbutton_data(obj,lightb_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,188,15,70,40,"RoundButton");
    fl_set_button_shortcut(obj,"R#R",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_ROUNDBUTTON);
    fl_set_pixmapbutton_data(obj,roundb_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,275,15,70,40,"Round3DButton");
    fl_set_button_shortcut(obj,"3#3",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_ROUND3DBUTTON);
    fl_set_pixmapbutton_align(obj, FL_ALIGN_TOP|FL_ALIGN_INSIDE, 3, 3);
    fl_set_pixmapbutton_data(obj,r3dbut_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,58,80,70,40,"CheckButton");
    fl_set_button_shortcut(obj,"C#C",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_CHECKBUTTON);
    fl_set_pixmapbutton_data(obj,check_pixels);
  obj = fl_add_bitmapbutton(FL_RADIO_BUTTON,145,80,70,40,"BitmapButton");
    fl_set_button_shortcut(obj,"B#B",1);
    fl_set_object_color(obj,FL_COL1,FL_YELLOW);
    fl_set_object_callback(obj,pallette_entry_callback,FL_BITMAPBUTTON);
    fl_set_bitmapbutton_data(obj,picture_width, picture_height, (unsigned char *)picture_bits);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,231,80,70,40,"PixmapButton");
    fl_set_button_shortcut(obj,"P#p",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_PIXMAPBUTTON);
    fl_set_pixmapbutton_data(obj,xconq);
  fl_end_form();

  fl_adjust_form_size(fdui->buttform);
  fdui->buttform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "box.xpm"
#include "text.xpm"
#include "frame.xpm"
#include "chart.xpm"
#include "clock.xpm"
#include "picture1.xbm"
#include "xconq1.xpm"
FD_staticform *create_form_staticform(void)
{
  FL_OBJECT *obj;
  FD_staticform *fdui = (FD_staticform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->staticform = fl_bgn_form(FL_NO_BOX, 360, 150);
  obj = fl_add_box(FL_FLAT_BOX,0,0,360,150,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,10,345,130,"");
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,20,65,40,"Box");
    fl_set_button_shortcut(obj,"B#B",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_BOX);
    fl_set_pixmapbutton_data(obj,box_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,110,20,65,40,"Text");
    fl_set_button_shortcut(obj,"T#T",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_TEXT);
    fl_set_pixmapbutton_data(obj,text_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,190,20,65,40,"Frame");
    fl_set_button_shortcut(obj,"F#F",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_LABELFRAME);
    fl_set_pixmapbutton_data(obj,frame_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,270,20,65,40,"Chart");
    fl_set_button_shortcut(obj,"C#C",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_CHART);
    fl_set_pixmapbutton_data(obj,chart_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,80,65,40,"Clock");
    fl_set_button_shortcut(obj,"o#O",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_CLOCK);
    fl_set_pixmapbutton_data(obj,clock_pixels);
  obj = fl_add_bitmapbutton(FL_RADIO_BUTTON,110,80,65,40,"Bitmap");
    fl_set_button_shortcut(obj,"B#B",1);
    fl_set_object_color(obj,FL_COL1,FL_YELLOW);
    fl_set_object_callback(obj,pallette_entry_callback,FL_BITMAP);
    fl_set_bitmapbutton_data(obj,picture1_width, picture1_height, (unsigned char *)picture1_bits);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,210,80,65,40,"Pixmap");
    fl_set_button_shortcut(obj,"P#P",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_PIXMAP);
    fl_set_pixmapbutton_data(obj,xconq1);
  fl_end_form();

  fl_adjust_form_size(fdui->staticform);
  fdui->staticform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "sld.xpm"
#include "scb.xpm"
#include "dial.xpm"
#include "cnt.xpm"
#include "pos.xpm"
#include "vals.xpm"
#include "twheel.xpm"
FD_valuatorform *create_form_valuatorform(void)
{
  FL_OBJECT *obj;
  FD_valuatorform *fdui = (FD_valuatorform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->valuatorform = fl_bgn_form(FL_NO_BOX, 360, 150);
  obj = fl_add_box(FL_FLAT_BOX,0,0,360,150,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,8,345,130,"");
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,16,70,40,"Slider");
    fl_set_button_shortcut(obj,"S#S",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_SLIDER);
    fl_set_pixmapbutton_data(obj,sld_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,183,16,70,40,"Scrollbar");
    fl_set_button_shortcut(obj,"b#b",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_SCROLLBAR);
    fl_set_pixmapbutton_data(obj,scb_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,260,16,70,40,"Dial");
    fl_set_button_shortcut(obj,"D#D",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_DIAL);
    fl_set_pixmapbutton_data(obj,dial_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,76,70,40,"Counter");
    fl_set_button_shortcut(obj,"C#C",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_COUNTER);
    fl_set_pixmapbutton_data(obj,cnt_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,105,76,70,40,"Positioner");
    fl_set_button_shortcut(obj,"P#P",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_POSITIONER);
    fl_set_pixmapbutton_data(obj,pos_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,106,16,70,40,"Valslider");
    fl_set_button_shortcut(obj,"#V",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_VALSLIDER);
    fl_set_pixmapbutton_data(obj,vals_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,185,76,70,40,"Thumbwheel");
    fl_set_button_shortcut(obj,"t#t",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_THUMBWHEEL);
    fl_set_pixmapbutton_data(obj,twheel);
  fl_end_form();

  fl_adjust_form_size(fdui->valuatorform);
  fdui->valuatorform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "menu.xpm"
#include "choice.xpm"
#include "br.xpm"
FD_choiceform *create_form_choiceform(void)
{
  FL_OBJECT *obj;
  FD_choiceform *fdui = (FD_choiceform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->choiceform = fl_bgn_form(FL_NO_BOX, 360, 150);
  obj = fl_add_box(FL_FLAT_BOX,0,0,360,150,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,10,345,130,"");
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,20,60,60,"Menu");
    fl_set_button_shortcut(obj,"M#M",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_MENU);
    fl_set_pixmapbutton_data(obj,menu_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,100,20,60,60,"Choice");
    fl_set_button_shortcut(obj,"C#C",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_CHOICE);
    fl_set_pixmapbutton_data(obj,choice_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,170,20,60,60,"Browser");
    fl_set_button_shortcut(obj,"B#B",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_BROWSER);
    fl_set_pixmapbutton_data(obj,br_pixels);
  fl_end_form();

  fl_adjust_form_size(fdui->choiceform);
  fdui->choiceform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

#include "timer.xpm"
#include "xyplot.xpm"
#include "glcan.xpm"
#include "tab.xpm"
FD_miscform *create_form_miscform(void)
{
  FL_OBJECT *obj;
  FD_miscform *fdui = (FD_miscform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->miscform = fl_bgn_form(FL_NO_BOX, 360, 150);
  obj = fl_add_box(FL_FLAT_BOX,0,0,360,150,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,10,345,130,"");
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,16,70,45,"Timer");
    fl_set_button_shortcut(obj,"T#T",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_TIMER);
    fl_set_pixmapbutton_data(obj,timer_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,103,16,70,45,"XYPlot");
    fl_set_button_shortcut(obj,"X#X",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_XYPLOT);
    fl_set_pixmapbutton_data(obj,xyplot_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,176,16,70,45,"Canvas");
    fl_set_button_shortcut(obj,"C#C",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_CANVAS);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,250,16,70,45,"GLCanvas");
    fl_set_button_shortcut(obj,"G#G",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_GLCANVAS);
    fl_set_pixmapbutton_data(obj,glcan_pixels);
  obj = fl_add_pixmapbutton(FL_RADIO_BUTTON,30,82,70,40,"Tabfolder");
    fl_set_button_shortcut(obj,"f#f",1);
    fl_set_object_callback(obj,pallette_entry_callback,FL_NTABFOLDER);
    fl_set_pixmapbutton_data(obj,tabfolder_pixels);
  fl_end_form();

  fl_adjust_form_size(fdui->miscform);
  fdui->miscform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

