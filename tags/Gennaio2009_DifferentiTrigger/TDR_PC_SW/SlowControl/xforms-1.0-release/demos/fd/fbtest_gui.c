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
#include "fbtest_gui.h"

FD_fbform *create_form_fbform(void)
{
  FL_OBJECT *obj;
  FD_fbform *fdui = (FD_fbform *) fl_calloc(1, sizeof(*fdui));

  fdui->fbform = fl_bgn_form(FL_NO_BOX, 371, 361);
  obj = fl_add_box(FL_UP_BOX,0,0,371,361,"");
  fdui->browser = obj = fl_add_browser(FL_NORMAL_BROWSER,20,50,240,290,"");
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,270,50,70,25,"AlphaSort");
    fl_set_object_callback(obj,sort_method_cb,FL_ALPHASORT);
    fl_set_button(obj, 1);
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,270,85,70,25,"SizeSort");
    fl_set_object_callback(obj,sort_method_cb,FL_SIZESORT);
  obj = fl_add_checkbutton(FL_RADIO_BUTTON,270,120,70,25,"TimeSort");
    fl_set_object_callback(obj,sort_method_cb,FL_MTIMESORT);
  obj = fl_add_button(FL_NORMAL_BUTTON,275,310,75,25,"Done");
    fl_set_object_callback(obj,done_cb,0);
  fdui->total = obj = fl_add_text(FL_NORMAL_TEXT,10,15,220,30,"");
    fl_set_object_boxtype(obj,FL_NO_BOX);
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();

  fdui->fbform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

