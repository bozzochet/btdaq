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
 * Showing the use of non-modal file selector
 *
 * T.C. Zhao and M. Overmars
 */

#include "forms.h"
#include <stdlib.h>

FL_FORM *ttt;
FL_OBJECT *bm, *pm;

static void create_form_ttt(void);
static int load_file(const char *, void *);

int
main(int argc, char *argv[])
{
    fl_initialize(&argc, argv, "FormDemo", 0, 0);
    create_form_ttt();

    fl_show_form(ttt, FL_PLACE_CENTER, FL_TRANSIENT, "PixmapBrowser");

    fl_set_fselector_placement(FL_PLACE_FREE);
    fl_set_fselector_callback(load_file, 0);
    fl_show_fselector("Load a Pixmap file", 0, "*.x?m",0);
    fl_do_forms();
    return 0;
}

static int
load_file(const char *fname, void *data)
{
     int ispix = 0;
     char *p;

     if (( p = strrchr(fname, '.')))
         ispix = strcmp(p+1, "xbm") != 0;

     if(ispix) 
     {
        fl_hide_object(bm);
        fl_show_object(pm);
        fl_free_pixmap_pixmap(pm);
        fl_set_pixmap_file(pm, fname);
     }
     else 
     {
        fl_hide_object(pm);
        fl_show_object(bm);
        fl_set_bitmap_file(bm, fname);
     }
     return 1;
}

static void
done(FL_OBJECT *ob, long q)
{
    fl_finish();
    exit(0);
}

static void
reload(FL_OBJECT *ob, long q)
{
    fl_set_fselector_placement(FL_PLACE_MOUSE);
    fl_set_fselector_callback(load_file, 0);
    fl_show_fselector("Load a Pix/bitMap file", 0, 0,0);
}


static void create_form_ttt(void)
{
  FL_OBJECT *obj;

  if (ttt)
     return;

  ttt = fl_bgn_form(FL_NO_BOX,330,320);
  obj = fl_add_box(FL_UP_BOX,0,0,330,320,"");
  bm = obj = fl_add_bitmap(FL_NORMAL_BITMAP,30,20,270,240,"");
  fl_set_object_boxtype(obj, FL_FLAT_BOX);
  pm = obj = fl_add_pixmap(FL_NORMAL_PIXMAP,30,20,270,240,"");
  fl_set_object_boxtype(obj, FL_FLAT_BOX);
  obj = fl_add_button(FL_NORMAL_BUTTON,220,280,90,30,"Done");
  fl_set_object_callback(obj, done, 0);
  obj = fl_add_button(FL_NORMAL_BUTTON,20,280,90,30,"Load");
  fl_set_object_callback(obj, reload, 0);
  fl_set_object_shortcut(obj,"L",1);
  fl_end_form();
}
