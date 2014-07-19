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


/*
 * Inverted slider
 *
 * T.C. Zhao and M. Overmars
 */

#include "forms.h"
#include <stdlib.h>

extern void invert_it(FL_OBJECT *, long);
typedef struct {
	FL_FORM *inv;
	void *vdata;
	char *cdata;
	long ldata;
	FL_OBJECT *sl[3];
	FL_OBJECT *done;
} FD_inv;

extern FD_inv * create_form_inv(void);

FD_inv *ui;
void invert_it(FL_OBJECT *ob, long data)
{
   if(fl_get_button(ob))
   {
      fl_set_slider_bounds(ui->sl[0], 1.0, 0.0);
      fl_set_slider_bounds(ui->sl[1], 1.0, 0.0);
      fl_set_slider_bounds(ui->sl[2], 1.0, 0.0);
   }
   else
   {
      fl_set_slider_bounds(ui->sl[0], 0.0, 1.0);
      fl_set_slider_bounds(ui->sl[1], 0.0, 1.0);
      fl_set_slider_bounds(ui->sl[2], 0.0, 1.0);
   }
}

int main(int argc, char *argv[])
{

   fl_initialize(&argc, argv, "FormDemo", 0, 0);
   ui = create_form_inv();

   /* fill-in form initialization code */

   fl_show_form(ui->inv,FL_PLACE_CENTER,FL_TRANSIENT,"inv");

   while (fl_do_forms() != ui->done)
      ;
   return 0;
}

/* Form definition file generated with fdesign. */

FD_inv *create_form_inv(void)
{
  FL_OBJECT *obj;
  FD_inv *fdui = (FD_inv *)fl_calloc(1, sizeof(FD_inv));

  fdui->inv = fl_bgn_form(FL_NO_BOX, 245, 280);
  obj = fl_add_box(FL_UP_BOX,0,0,245,280,"");
  fdui->sl[0] = obj = fl_add_valslider(FL_VERT_SLIDER,20,30,35,230,"");
  fdui->sl[1] = obj = fl_add_valslider(FL_VERT_FILL_SLIDER,65,30,35,230,"");
  fdui->sl[2] = obj = fl_add_valslider(FL_VERT_NICE_SLIDER,115,30,35,230,"");
    fl_set_object_boxtype(obj,FL_FLAT_BOX);
    fl_set_object_color(obj,FL_COL1,FL_BLUE);
  fdui->done = obj = fl_add_button(FL_RETURN_BUTTON,160,235,75,30,"Exit");
  obj = fl_add_checkbutton(FL_PUSH_BUTTON,165,30,75,35,"Invert");
    fl_set_object_callback(obj,invert_it,0);
  fl_end_form();

  return fdui;
}
/*---------------------------------------*/

