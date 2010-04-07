/*
 *
 *  This file is part of the XForms library package.
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
#include "micntl.h"

FD_micntl *create_form_micntl(void)
{
  FL_OBJECT *obj;
  FD_micntl *fdui = (FD_micntl *) fl_calloc(1, sizeof(*fdui));

  fdui->micntl = fl_bgn_form(FL_NO_BOX, 180, 190);
  obj = fl_add_box(FL_UP_BOX,0,0,180,190,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,20,160,20,20,"@<<");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  obj = fl_add_button(FL_NORMAL_BUTTON,40,160,20,20,"@<");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  obj = fl_add_button(FL_NORMAL_BUTTON,60,160,20,20,"@square");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_YELLOW);
    fl_set_object_lcolor(obj,FL_BLUE);
  obj = fl_add_button(FL_NORMAL_BUTTON,80,160,20,20,"@>");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  obj = fl_add_button(FL_NORMAL_BUTTON,100,160,20,20,"@>>");
    fl_set_object_boxtype(obj,FL_FRAME_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
    fl_set_object_lcolor(obj,FL_BLUE);
  fdui->delay = obj = fl_add_choice(FL_NORMAL_CHOICE2,20,60,80,20,"");
  fl_end_form();

  fdui->micntl->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

