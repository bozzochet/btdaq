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
#include "demotest_fd.h"

FD_form0 *create_form_form0(void)
{
  FL_OBJECT *obj;
  FD_form0 *fdui = (FD_form0 *) fl_calloc(1, sizeof(*fdui));

  fdui->form0 = fl_bgn_form(FL_NO_BOX, 320, 250);
  obj = fl_add_box(FL_UP_BOX,0,0,320,250,"");
  obj = fl_add_menu(FL_PULLDOWN_MENU,110,40,90,30,"Menu");
    fl_set_object_callback(obj,menu_callback,0);
    fl_addto_menu(obj, "create item");
    fl_addto_menu(obj, "Delete Item %l");
    fl_addto_menu(obj, "Item 1");
    fl_set_menu_item_mode(obj, 3, FL_PUP_CHECK);
    fl_addto_menu(obj, "Item 2");
  fl_end_form();

  fdui->form0->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

