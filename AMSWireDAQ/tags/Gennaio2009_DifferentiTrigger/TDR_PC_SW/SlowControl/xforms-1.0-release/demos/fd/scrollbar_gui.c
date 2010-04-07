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
#include "scrollbar_gui.h"

FD_scb *create_form_scb(void)
{
  FL_OBJECT *obj;
  FD_scb *fdui = (FD_scb *) fl_calloc(1, sizeof(*fdui));

  fdui->scb = fl_bgn_form(FL_NO_BOX, 470, 230);
  obj = fl_add_box(FL_UP_BOX,0,0,470,230,"");
  fdui->hor = obj = fl_add_scrollbar(FL_HOR_SCROLLBAR,30,15,230,17,"HOR_SCROLLBAR");
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,noop_cb,0);
  fdui->hor_thin = obj = fl_add_scrollbar(FL_HOR_THIN_SCROLLBAR,30,60,230,18,"HOR_THIN_SCROLLBAR");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,noop_cb,0);
    fl_set_scrollbar_value(obj, 0.11);
  fdui->hor_nice = obj = fl_add_scrollbar(FL_HOR_NICE_SCROLLBAR,30,110,230,18,"HOR_NICE_SCROLLBAR");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,noop_cb,0);
  fdui->vert = obj = fl_add_scrollbar(FL_VERT_SCROLLBAR,300,10,17,185,"");
    fl_set_object_callback(obj,noop_cb,0);
  fdui->vert_thin = obj = fl_add_scrollbar(FL_VERT_THIN_SCROLLBAR,338,10,17,185,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_callback(obj,noop_cb,0);
  fdui->hide = obj = fl_add_button(FL_NORMAL_BUTTON,20,195,80,25,"Hide");
    fl_set_object_callback(obj,hide_cb,0);
  fdui->deactivate = obj = fl_add_button(FL_NORMAL_BUTTON,100,195,80,25,"Deactivate");
    fl_set_object_callback(obj,deactivate_cb,0);
  obj = fl_add_button(FL_NORMAL_BUTTON,200,195,80,25,"Done");
    fl_set_object_callback(obj,done_cb,0);
  fdui->vert_nice = obj = fl_add_scrollbar(FL_VERT_NICE_SCROLLBAR,370,10,17,185,"");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_callback(obj,noop_cb,0);
    fl_set_scrollbar_value(obj, 1);
  obj = fl_add_scrollbar(FL_HOR_BASIC_SCROLLBAR,30,155,230,18,"HOR_BASIC_SCROLLBAR");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_lsize(obj,FL_TINY_SIZE);
    fl_set_object_callback(obj,noop_cb,0);
    fl_set_scrollbar_value(obj, 0.77);
    fl_set_scrollbar_size(obj, 0.20);
  obj = fl_add_scrollbar(FL_VERT_BASIC_SCROLLBAR,410,10,17,185,"");
    fl_set_object_boxtype(obj,FL_DOWN_BOX);
    fl_set_object_callback(obj,noop_cb,0);
    fl_set_scrollbar_value(obj, 0.97);
    fl_set_scrollbar_size(obj, 0.20);
  fl_end_form();

  fdui->scb->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

