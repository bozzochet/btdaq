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


#include "forms.h"
#include <stdlib.h>

extern void exit_cb(FL_OBJECT *, long);
extern void input_cb(FL_OBJECT *, long);

/**** Forms and Objects ****/

typedef struct {
	FL_FORM *form0;
	void *vdata;
	long ldata;
	FL_OBJECT *text;
} FD_form0;

extern FD_form0 * create_form_form0(void);

FD_form0 *fd_form0;

/* callbacks for form form0 */
void exit_cb(FL_OBJECT *ob, long data)
{
   exit(0);
}

void input_cb(FL_OBJECT *ob, long data)
{
    const char *s = fl_get_input(ob);
    int w = fl_get_string_width(ob->lstyle, ob->lsize, s, strlen(s));
    int h = fl_get_string_height(ob->lstyle, ob->lsize, s, strlen(s),0,0);
    char buf[18];
    sprintf(buf,"w=%d h=%d",w,h);
    fl_set_object_label(fd_form0->text,buf);
}


int main(int argc, char *argv[])
{
   fl_initialize(&argc, argv, 0, 0, 0);
   fd_form0 = create_form_form0();

   /* fill-in form initialization code */

   /* show the first form */
   fl_show_form(fd_form0->form0,FL_PLACE_CENTER,FL_FULLBORDER,"form0");

   fl_do_forms();
   return 0;
}

/* Form definition file generated with fdesign. */

FD_form0 *create_form_form0(void)
{
  FL_OBJECT *obj;
  FD_form0 *fdui = (FD_form0 *) fl_calloc(1, sizeof(*fdui));

  fdui->form0 = fl_bgn_form(FL_NO_BOX, 311, 181);
  obj = fl_add_box(FL_UP_BOX,0,0,311,181,"");
  obj = fl_add_button(FL_NORMAL_BUTTON,220,130,80,30,"Done");
    fl_set_object_callback(obj,exit_cb,0);
  obj = fl_add_input(FL_NORMAL_INPUT,20,30,280,30,"");
    fl_set_object_callback(obj,input_cb,0);
  fdui->text = obj = fl_add_text(FL_NORMAL_TEXT,60,90,130,30,"Text");
    fl_set_object_lalign(obj,FL_ALIGN_LEFT|FL_ALIGN_INSIDE);
  fl_end_form();

  return fdui;
}
/*---------------------------------------*/

