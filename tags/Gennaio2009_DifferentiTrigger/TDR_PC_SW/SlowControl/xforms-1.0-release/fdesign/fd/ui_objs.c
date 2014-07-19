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
#include "ui_objs.h"

FD_buttonform *create_form_buttonform(void)
{
  FL_OBJECT *obj;
  FD_buttonform *fdui = (FD_buttonform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->buttonform = fl_bgn_form(FL_NO_BOX, 581, 51);
  obj = fl_add_box(FL_FLAT_BOX,0,0,581,51,"");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,20,3,30,30,"button");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,60,3,30,30,"round");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,100,3,30,30,"round3d");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,140,3,30,30,"check");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,180,3,30,30,"light");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,220,3,30,30,"bitmap");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,260,3,30,30,"pixmap");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,300,3,30,30,"scroll");
  fl_end_form();

  fl_adjust_form_size(fdui->buttonform);
  fdui->buttonform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_valuatorform *create_form_valuatorform(void)
{
  FL_OBJECT *obj;
  FD_valuatorform *fdui = (FD_valuatorform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->valuatorform = fl_bgn_form(FL_NO_BOX, 581, 51);
  obj = fl_add_box(FL_FLAT_BOX,0,0,581,51,"");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,20,5,30,30,"slider");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,66,5,30,30,"valslider");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,112,5,30,30,"scrollbar");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,158,5,30,30,"dial");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,204,5,30,30,"counter");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,250,5,30,30,"positioner");
  fl_end_form();

  fl_adjust_form_size(fdui->valuatorform);
  fdui->valuatorform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

FD_staticform *create_form_staticform(void)
{
  FL_OBJECT *obj;
  FD_staticform *fdui = (FD_staticform *) fl_calloc(1, sizeof(*fdui));
  int old_bw = fl_get_border_width();

  fl_set_border_width(-2);
  fdui->staticform = fl_bgn_form(FL_NO_BOX, 581, 51);
  obj = fl_add_box(FL_FLAT_BOX,0,0,581,51,"");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,20,3,30,30,"box");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,70,3,30,30,"frame");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,120,3,30,30,"labelframe");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,170,3,30,30,"text");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,220,3,30,30,"bitmap");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,270,3,30,30,"pixmap");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,320,3,30,30,"chart");
  obj = fl_add_pixmapbutton(FL_NORMAL_BUTTON,370,3,30,30,"clock");
  fl_end_form();

  fl_adjust_form_size(fdui->staticform);
  fdui->staticform->fdui = fdui;
  fl_set_border_width(old_bw);

  return fdui;
}
/*---------------------------------------*/

