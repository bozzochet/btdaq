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
 * Different label alignments 
 *
 *  This file is part of xforms package
 *  T.C. Zhao and M. Overmars
 */

#include "forms.h"
#include <stdlib.h>

/**** Forms and Objects ****/

typedef struct {
	FL_FORM *form0;
	void *vdata;
	long ldata;
	FL_OBJECT *box;
	FL_OBJECT *inside;
	FL_OBJECT *center;
} FD_form0;

extern FD_form0 * create_form_form0(void);

FD_form0 *fd_form0;
int main(int argc, char *argv[])
{
   int align;

   fl_initialize(&argc, argv, "FormDemo", 0, 0);
   fd_form0 = create_form_form0();

   /* fill-in form initialization code */
   fl_set_form_dblbuffer(fd_form0->form0, 1);
   align = fd_form0->box->align | FL_ALIGN_INSIDE;
   if(align != fd_form0->box->align)
     fl_set_button(fd_form0->inside, 1);

   /* show the first form */
   fl_show_form(fd_form0->form0,FL_PLACE_FREE,FL_FULLBORDER,"form0");

   fl_do_forms();
   return 0;
}

/* #define TEST_PIXMAP_ALIGN */

void
align_cb(FL_OBJECT *ob, long n)
{
    if(fl_get_button(fd_form0->inside))
       n |= FL_ALIGN_INSIDE;


#ifndef TEST_PIXMAP_ALIGN
    fl_set_object_lalign(fd_form0->box, n);
#else
    fl_set_pixmap_align(fd_form0->box, n, 3, 3);
#endif
}

void
inside_cb(FL_OBJECT *ob, long data)
{
   if(fl_get_button(ob))
      fd_form0->box->align |= FL_ALIGN_INSIDE;
   else
      fd_form0->box->align &= ~FL_ALIGN_INSIDE;
#ifdef TEST_PIXMAP_ALIGN
   fl_set_pixmap_align(fd_form0->box, fd_form0->box->align, 3, 3);
#else
   fl_redraw_form(fd_form0->form0);
#endif
}

FD_form0 *create_form_form0(void)
{
  FL_OBJECT *obj;
  FD_form0 *fdui = (FD_form0 *) fl_calloc(1, sizeof(FD_form0));

  fdui->form0 = fl_bgn_form(FL_NO_BOX, 351, 170);
  obj = fl_add_box(FL_UP_BOX,0,0,351,170,"");
#ifndef TEST_PIXMAP_ALIGN
  fdui->box = obj = fl_add_box(FL_UP_BOX,190,45,90,45,"abcdefg\nhijklmno");
#else
  fdui->box = obj = fl_add_pixmap(FL_NORMAL_PIXMAP,190,35,90,60,"");
  fl_set_pixmap_file(obj,"crab.xpm");
  fl_set_object_boxtype(obj,FL_UP_BOX);
#endif
  /*  fl_set_object_clip(obj, 1); */
  fdui->inside = obj = fl_add_lightbutton(FL_PUSH_BUTTON,20,120,90,30,"Inside");
    fl_set_object_callback(obj,inside_cb,0);
 fl_bgn_group();
  obj = fl_add_button(FL_RADIO_BUTTON,20,20,30,30,"@#7->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_LEFT_TOP);
  obj = fl_add_button(FL_RADIO_BUTTON,50,20,30,30,"@#8->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_TOP);
  obj = fl_add_button(FL_RADIO_BUTTON,80,20,30,30,"@#9->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_RIGHT_TOP);
  obj = fl_add_button(FL_RADIO_BUTTON,80,50,30,30,"@#->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_RIGHT);
  fdui->center = obj = fl_add_button(FL_RADIO_BUTTON,50,50,30,30,"@circle");
    fl_set_object_lcol(obj,FL_RED);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_CENTER);
  obj = fl_add_button(FL_RADIO_BUTTON,20,50,30,30,"@#<-");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_LEFT);
  obj = fl_add_button(FL_RADIO_BUTTON,20,80,30,30,"@#1->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_LEFT_BOTTOM);
  obj = fl_add_button(FL_RADIO_BUTTON,50,80,30,30,"@#2->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_BOTTOM);
  obj = fl_add_button(FL_RADIO_BUTTON,80,80,30,30,"@#3->");
    fl_set_object_lcol(obj,FL_BLUE);
    fl_set_object_callback(obj,align_cb,FL_ALIGN_RIGHT_BOTTOM);
  fl_end_group();

  obj = fl_add_button(FL_NORMAL_BUTTON,140,120,70,30,"Done");
  fl_end_form();

  return fdui;
}
