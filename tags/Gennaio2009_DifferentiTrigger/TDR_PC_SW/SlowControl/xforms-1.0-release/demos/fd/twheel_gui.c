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
#include "twheel_gui.h"

static FL_PUP_ENTRY fdchoice_returnsetting_0[] =
{ 
    /*  itemtext   callback  shortcut   mode */
    { "End & Changed",	0,	"",	 FL_PUP_NONE},
    { "Whenever Changed",	0,	"",	 FL_PUP_NONE},
    { "Always At End",	0,	"",	 FL_PUP_NONE},
    { "Always",	0,	"",	 FL_PUP_NONE},
    {0}
};

FD_twheelform *create_form_twheelform(void)
{
  FL_OBJECT *obj;
  FD_twheelform *fdui = (FD_twheelform *) fl_calloc(1, sizeof(*fdui));

  fdui->twheelform = fl_bgn_form(FL_NO_BOX, 220, 260);
  obj = fl_add_box(FL_UP_BOX,0,0,220,260,"");
  obj = fl_add_frame(FL_ENGRAVED_FRAME,15,70,190,130,"");
  fdui->vert = obj = fl_add_thumbwheel(FL_VERT_THUMBWHEEL,30,90,20,100,"");
    fl_set_object_callback(obj,valchange_cb,0);
    fl_set_thumbwheel_step(obj, 1.0);
    fl_set_thumbwheel_bounds(obj, 1.0,100);
  fdui->hor = obj = fl_add_thumbwheel(FL_HOR_THUMBWHEEL,60,130,120,23,"");
    fl_set_object_callback(obj,valchange_cb,0);
    fl_set_thumbwheel_step(obj, 0.01);
  fdui->report = obj = fl_add_text(FL_NORMAL_TEXT,60,90,120,30,"");
    fl_set_object_lalign(obj,FL_ALIGN_CENTER|FL_ALIGN_INSIDE);
  fdui->returnsetting = obj = fl_add_choice(FL_NORMAL_CHOICE2,35,20,160,30,"");
    fl_set_object_boxtype(obj,FL_EMBOSSED_BOX);
    fl_set_object_callback(obj,returnchange_cb,0);
    fl_set_choice_entries(obj, fdchoice_returnsetting_0);
    fl_set_choice(obj,2);
  obj = fl_add_button(FL_NORMAL_BUTTON,120,215,80,30,"Enough");
  fl_end_form();

  fdui->twheelform->fdui = fdui;

  return fdui;
}
/*---------------------------------------*/

