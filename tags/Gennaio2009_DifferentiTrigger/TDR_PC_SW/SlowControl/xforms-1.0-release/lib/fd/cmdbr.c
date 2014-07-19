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
#include "cmdbr.h"

FD_cmd *create_form_cmd(void)
{
  FL_OBJECT *obj;
  FD_cmd *fdui = (FD_cmd *) fl_calloc(1, sizeof(*fdui));

  fdui->cmd = fl_bgn_form(FL_NO_BOX, 430, 190);
  fdui->backface = obj = fl_add_box(FL_UP_BOX,0,0,430,190,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,10,152,410,31,"");
    fl_set_object_gravity(obj, FL_SouthWest, FL_SouthEast);
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,10,8,410,140,"");
    fl_set_object_gravity(obj, FL_NorthWest, FL_SouthEast);
  fdui->close_br = obj = fl_add_button(FL_NORMAL_BUTTON,299,155,60,25,"Close");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,fl_cmdbr_close_cb,0);
  fdui->clear_br = obj = fl_add_button(FL_NORMAL_BUTTON,238,155,61,25,"Clear");
    fl_set_object_gravity(obj, FL_SouthEast, FL_SouthEast);
    fl_set_object_callback(obj,fl_cmdbr_clear_cb,0);
  fl_end_form();

  fdui->cmd->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

