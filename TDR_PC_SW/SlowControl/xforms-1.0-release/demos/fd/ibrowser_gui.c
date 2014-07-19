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
#include "ibrowser_gui.h"

FD_ibcanvas *create_form_ibcanvas(void)
{
  FL_OBJECT *obj;
  FD_ibcanvas *fdui = (FD_ibcanvas *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->ibcanvas = fl_bgn_form(FL_NO_BOX, 945, 730);
  obj = fl_add_box(FL_UP_BOX,0,0,945,730,"");
  fdui->canvas = obj = fl_add_canvas(FL_NORMAL_CANVAS,15,50,770,630,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  obj = fl_add_button(FL_NORMAL_BUTTON,830,35,70,30,"File");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,browse_file,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,850,695,70,30,"Quit");
    fl_set_button_shortcut(obj,"Qq#q^[",1);
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,quit_callback,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,873,100,60,30,"B&\\W");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convert_type,FL_IMAGE_MONO);
  obj = fl_add_button(FL_NORMAL_BUTTON,813,130,60,30,"Gray");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convert_type,FL_IMAGE_GRAY);
  obj = fl_add_button(FL_NORMAL_BUTTON,873,70,60,30,"RGB");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convert_type,FL_IMAGE_RGB);
  obj = fl_add_button(FL_NORMAL_BUTTON,813,70,60,30,"Packed");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convert_type,FL_IMAGE_PACKED);
  fdui->text = obj = fl_add_text(FL_NORMAL_TEXT,20,10,760,33,"");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_NorthWest, FL_NoGravity);
    fl_set_object_resize(obj, FL_RESIZE_X);
  fdui->status = obj = fl_add_text(FL_NORMAL_TEXT,10,690,230,30,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthWest);
  fdui->error = obj = fl_add_text(FL_NORMAL_TEXT,245,690,475,30,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
    fl_set_object_resize(obj, FL_RESIZE_X);
  obj = fl_add_button(FL_NORMAL_BUTTON,830,440,80,25,"Write");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,write_image,0);
  fdui->format = obj = fl_add_choice(FL_NORMAL_CHOICE2,818,477,109,27,"");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
  fdui->xfloat = obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,800,575,135,20,"");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_slider_precision(obj, 1);
    fl_set_slider_bounds(obj, 0, 5);
    fl_set_slider_value(obj, 1);
    fl_set_slider_step(obj, 0.1);
     fl_set_slider_return(obj, FL_RETURN_END_CHANGED);
  fdui->yfloat = obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,800,595,135,20,"");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_slider_precision(obj, 1);
    fl_set_slider_bounds(obj, 0, 5);
    fl_set_slider_value(obj, 1);
    fl_set_slider_step(obj, 0.1);
     fl_set_slider_return(obj, FL_RETURN_END_CHANGED);
  obj = fl_add_button(FL_NORMAL_BUTTON,800,620,50,25,"Scale");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,scale_image,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,835,540,75,25,"Rotate");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,rotate_image,0);
  fdui->angle = obj = fl_add_counter(FL_NORMAL_COUNTER,825,515,100,20,"");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_counter_precision(obj, 0);
    fl_set_counter_bounds(obj, -180, 180);
    fl_set_counter_step(obj, 1, 5);
  obj = fl_add_button(FL_NORMAL_BUTTON,815,655,40,26,"FlipX");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,flip_image,'y');
  obj = fl_add_button(FL_NORMAL_BUTTON,865,655,40,27,"FlipY");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,flip_image,'x');
  obj = fl_add_button(FL_NORMAL_BUTTON,813,100,60,30,"CI");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convert_type,FL_IMAGE_CI);
  fdui->anti_aliasing = obj = fl_add_checkbutton(FL_PUSH_BUTTON,850,615,30,30,"AA");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
  fdui->comments = obj = fl_add_button(FL_NORMAL_BUTTON,720,690,70,30,"comments");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,show_comments,0);
  fdui->subimage = obj = fl_add_checkbutton(FL_PUSH_BUTTON,815,290,30,30,"SubImage");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
  fdui->level = obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,794,359,146,20,"level");
    fl_set_object_gravity(obj, FL_East, FL_East);
    fl_set_object_callback(obj,window_level,0);
    fl_set_slider_precision(obj, 0);
    fl_set_slider_bounds(obj, 0, 4095);
    fl_set_slider_value(obj, 0);
    fl_set_slider_size(obj, 0.10);
  fdui->width = obj = fl_add_valslider(FL_HOR_BROWSER_SLIDER,792,400,148,20,"width");
    fl_set_object_gravity(obj, FL_East, FL_East);
    fl_set_object_callback(obj,window_level,0);
    fl_set_slider_precision(obj, 0);
    fl_set_slider_bounds(obj, 0, 4095);
    fl_set_slider_value(obj, 0);
  obj = fl_add_button(FL_NORMAL_BUTTON,803,168,64,30,"Sharpen");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convolve_it,(long)FL_SHARPEN);
  obj = fl_add_button(FL_NORMAL_BUTTON,803,198,64,30,"Smooth");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convolve_it,(long)FL_SMOOTH);
  obj = fl_add_button(FL_NORMAL_BUTTON,803,229,64,30,"Tint");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,tint_it,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,867,228,69,30,"Smooth");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,convolve_it,(long)FL_SMOOTH);
  obj = fl_add_button(FL_NORMAL_BUTTON,867,198,69,29,"HistoGram");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,enhance_it,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,803,260,64,30,"Crop");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,crop_it,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,868,260,69,30,"Border");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,crop_it,1);
  obj = fl_add_button(FL_NORMAL_BUTTON,867,168,69,28,"Warp");
    fl_set_object_gravity(obj, FL_NorthEast, FL_NorthEast);
    fl_set_object_callback(obj,warp_it,0);
  fdui->annotate = obj = fl_add_button(FL_NORMAL_BUTTON,805,320,55,25,"Annotate");
    fl_set_object_callback(obj,annotate_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,865,320,55,25,"Render");
    fl_set_object_callback(obj,render_cb,0);
  fl_end_form();

  fl_adjust_form_size(fdui->ibcanvas);
  fdui->ibcanvas->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

