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
#include "ginput.h"

FD_form *create_form_form(void)
{
  FL_OBJECT *obj;
  FD_form *fdui = (FD_form *) fl_calloc(1, sizeof(*fdui));

  fdui->form = fl_bgn_form(FL_NO_BOX, 410, 120);
  obj = fl_add_box(FL_UP_BOX,0,0,410,120,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,8,9,394,67,"");
  fdui->input = obj = fl_add_input(FL_NORMAL_INPUT,20,33,370,30,"");
    fl_set_object_lalign(obj,FL_ALIGN_TOP_LEFT);
  fdui->cancel = obj = fl_add_button(FL_NORMAL_BUTTON,30,85,80,26,"Cancel");
    fl_set_button_shortcut(obj,"^[",1);
  obj = fl_add_button(FL_NORMAL_BUTTON,300,85,80,26,"Clear");
    fl_set_object_callback(obj,clear_cb,0);
  fdui->ok = obj = fl_add_button(FL_RETURN_BUTTON,165,85,80,26,"OK");
  fl_end_form();

  fdui->form->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

